#include "ece.h"

// This file implements a parser for the `Crypto-Key` and `Encryption` HTTP
// headers, used by the older "aesgcm" encoding. The newer "aes128gcm" encoding
// includes the relevant information in a binary header, directly in the
// payload.

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ece.h"
#include "ece/keys.h"
#include "ece/trailer.h"
#include <openssl/evp.h>
#include <openssl/rand.h>

#define ECE_HEADER_STATE_BEGIN_PARAM 1
#define ECE_HEADER_STATE_BEGIN_NAME 2
#define ECE_HEADER_STATE_NAME 3
#define ECE_HEADER_STATE_END_NAME 4
#define ECE_HEADER_STATE_BEGIN_VALUE 5
#define ECE_HEADER_STATE_VALUE 6
#define ECE_HEADER_STATE_BEGIN_QUOTED_VALUE 7
#define ECE_HEADER_STATE_QUOTED_VALUE 8
#define ECE_HEADER_STATE_END_VALUE 9
#define ECE_HEADER_STATE_INVALID_HEADER 10

#define ECE_HEADER_DH_PREFIX "dh="
#define ECE_HEADER_DH_PREFIX_LENGTH 3

// Extracts an unsigned 32-bit integer in network byte order.
static inline uint32_t
ece_read_uint32_be(const uint8_t* bytes) {
  uint32_t value = bytes[3];
  value |= (uint32_t) bytes[2] << 8;
  value |= (uint32_t) bytes[1] << 16;
  value |= (uint32_t) bytes[0] << 24;
  return value;
}

// A linked list that holds name-value pairs for a parameter in a header
// value. For example, if the parameter is `a=b; c=d; e=f`, the parser will
// allocate three `ece_header_pairs_t` structures, one for each ;-delimited
// pair. "=" separates the name and value.
typedef struct ece_header_pairs_s {
  struct ece_header_pairs_s* next;
  // The name and value are pointers into the backing header value; the parser
  // doesn't allocate new strings. Freeing the backing string will invalidate
  // all `name` and `value` references. Also, because these are not true C
  // strings, it's important to use them with functions that take a length, like
  // `strncmp`. Functions that assume a NUL-terminated string will read until
  // the end of the backing string.
  const char* name;
  const char* value;
  size_t nameLen;
  size_t valueLen;
} ece_header_pairs_t;

// Initializes a name-value pair node at the head of the pair list. `head` may
// be `NULL`.
static ece_header_pairs_t*
ece_header_pairs_alloc(ece_header_pairs_t* head) {
  ece_header_pairs_t* pairs = (ece_header_pairs_t*)malloc(sizeof(ece_header_pairs_t));
  if (!pairs) {
    return NULL;
  }
  pairs->next = head;
  pairs->name = NULL;
  pairs->value = NULL;
  pairs->nameLen = 0;
  pairs->valueLen = 0;
  return pairs;
}

// Indicates whether a name-value pair node matches the `name`.
static inline bool
ece_header_pairs_has_name(ece_header_pairs_t* pair, const char* name) {
  return !strncmp(pair->name, name, pair->nameLen);
}

// Indicates whether a name-value pair node matches the `value`.
static inline bool
ece_header_pairs_has_value(ece_header_pairs_t* pair, const char* value) {
  return !strncmp(pair->value, value, pair->valueLen);
}

// Copies a pair node's value into a C string.
static char*
ece_header_pairs_value_to_str(ece_header_pairs_t* pair) {
  char* value = (char *)malloc(pair->valueLen + 1);
  if (!value) {
    return NULL;
  }
  strncpy(value, pair->value, pair->valueLen);
  value[pair->valueLen] = '\0';
  return value;
}

// Frees a name-value pair list and all its nodes.
static void
ece_header_pairs_free(ece_header_pairs_t* pairs) {
  ece_header_pairs_t* pair = pairs;
  while (pair) {
    ece_header_pairs_t* next = pair->next;
    free(pair);
    pair = next;
  }
}

// A linked list that holds parameters extracted from a header value. For
// example, if the header value is `a=b; c=d, e=f; g=h`, the parser will
// allocate two `ece_header_params_t` structures: one to hold the parameter
// `a=b; c=d`, and the other to hold `e=f; g=h`.
typedef struct ece_header_params_s {
  struct ece_header_params_s* next;
  ece_header_pairs_t* pairs;
} ece_header_params_t;

// Initializes a parameter node at the head of the parameter list. `head` may be
// `NULL`.
static ece_header_params_t*
ece_header_params_alloc(ece_header_params_t* head) {
  ece_header_params_t* params = (ece_header_params_t*) malloc(sizeof(ece_header_params_t));
  if (!params) {
    return NULL;
  }
  params->next = head;
  params->pairs = NULL;
  return params;
}

// Reverses a parameter list in-place and returns a pointer to the new head.
static ece_header_params_t*
ece_header_params_reverse(ece_header_params_t* params) {
  ece_header_params_t* sibling = NULL;
  while (params) {
    ece_header_params_t* next = params->next;
    params->next = sibling;
    sibling = params;
    params = next;
  }
  return sibling;
}

// Frees a parameter list and all its nodes.
static void
ece_header_params_free(ece_header_params_t* params) {
  ece_header_params_t* param = params;
  while (param) {
    ece_header_pairs_free(param->pairs);
    ece_header_params_t* next = param->next;
    free(param);
    param = next;
  }
}

// Indicates whether `c` is whitespace, per `WSP` in RFC 5234, Appendix B.1.
static inline bool
ece_header_is_space(char c) {
  return c == ' ' || c == '\t';
}

// Indicates whether `c` can appear in a pair name. Only lowercase letters and
// numbers are allowed.
static inline bool
ece_header_is_valid_pair_name(char c) {
  return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9');
}

// Indicates whether `c` can appear in a pair value. This includes all
// characters in the Base64url alphabet.
static inline bool
ece_header_is_valid_pair_value(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
         (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '=';
}

// A header parameter parser.
typedef struct ece_header_parser_s {
  int state;
  ece_header_params_t* params;
} ece_header_parser_t;

// Parses the next token in `input` and updates the parser state. Returns true
// if the caller should advance to the next character; false otherwise.
static bool
ece_header_parse(ece_header_parser_t* parser, const char* input) {
  switch (parser->state) {
  case ECE_HEADER_STATE_BEGIN_PARAM: {
    ece_header_params_t* param = ece_header_params_alloc(parser->params);
    if (!param) {
      break;
    }
    parser->params = param;
    parser->state = ECE_HEADER_STATE_BEGIN_NAME;
    return false;
  }

  case ECE_HEADER_STATE_BEGIN_NAME:
    if (ece_header_is_space(*input)) {
      return true;
    }
    if (ece_header_is_valid_pair_name(*input)) {
      ece_header_pairs_t* pair = ece_header_pairs_alloc(parser->params->pairs);
      if (!pair) {
        break;
      }
      parser->params->pairs = pair;
      pair->name = input;
      parser->state = ECE_HEADER_STATE_NAME;
      return false;
    }
    break;

  case ECE_HEADER_STATE_NAME:
    if (ece_header_is_valid_pair_name(*input)) {
      parser->params->pairs->nameLen++;
      return true;
    }
    if (ece_header_is_space(*input) || *input == '=') {
      parser->state = ECE_HEADER_STATE_END_NAME;
      return false;
    }
    break;

  case ECE_HEADER_STATE_END_NAME:
    if (ece_header_is_space(*input)) {
      return true;
    }
    if (*input == '=') {
      parser->state = ECE_HEADER_STATE_BEGIN_VALUE;
      return true;
    }
    break;

  case ECE_HEADER_STATE_BEGIN_VALUE:
    if (ece_header_is_space(*input)) {
      return true;
    }
    if (ece_header_is_valid_pair_value(*input)) {
      parser->params->pairs->value = input;
      parser->state = ECE_HEADER_STATE_VALUE;
      return false;
    }
    if (*input == '"') {
      parser->state = ECE_HEADER_STATE_BEGIN_QUOTED_VALUE;
      return true;
    }
    break;

  case ECE_HEADER_STATE_VALUE:
    if (ece_header_is_space(*input) || *input == ';' || *input == ',') {
      parser->state = ECE_HEADER_STATE_END_VALUE;
      return false;
    }
    if (ece_header_is_valid_pair_value(*input)) {
      parser->params->pairs->valueLen++;
      return true;
    }
    break;

  case ECE_HEADER_STATE_BEGIN_QUOTED_VALUE:
    if (ece_header_is_valid_pair_value(*input)) {
      // Quoted strings allow spaces and escapes, but neither `Crypto-Key` nor
      // `Encryption` accept them. We keep the parser simple by rejecting
      // non-Base64url characters here. We also disallow empty quoted strings.
      parser->params->pairs->value = input;
      parser->params->pairs->valueLen++;
      parser->state = ECE_HEADER_STATE_QUOTED_VALUE;
      return true;
    }
    break;

  case ECE_HEADER_STATE_QUOTED_VALUE:
    if (ece_header_is_valid_pair_value(*input)) {
      parser->params->pairs->valueLen++;
      return true;
    }
    if (*input == '"') {
      parser->state = ECE_HEADER_STATE_END_VALUE;
      return true;
    }
    break;

  case ECE_HEADER_STATE_END_VALUE:
    if (ece_header_is_space(*input)) {
      return true;
    }
    if (*input == ';') {
      // New name-value pair for the same parameter. Advance the parser;
      // `ECE_HEADER_STATE_BEGIN_NAME` will prepend a new node to the pairs
      // list.
      parser->state = ECE_HEADER_STATE_BEGIN_NAME;
      return true;
    }
    if (*input == ',') {
      // New parameter. Advance the parser; `ECE_HEADER_STATE_BEGIN_PARAM` will
      // prepend a new node to the parameters list and begin parsing its pairs.
      parser->state = ECE_HEADER_STATE_BEGIN_PARAM;
      return true;
    }
    break;

  default:
    // Unexpected parser state.
    assert(false);
  }
  parser->state = ECE_HEADER_STATE_INVALID_HEADER;
  return false;
}

// Parses a `header` value of the form `a=b; c=d; e=f, g=h, i=j` into a
// parameter list.
static ece_header_params_t*
ece_header_extract_params(const char* header) {
  ece_header_parser_t parser;
  parser.state = ECE_HEADER_STATE_BEGIN_PARAM;
  parser.params = NULL;

  const char* input = header;
  while (*input) {
    if (ece_header_parse(&parser, input)) {
      input++;
    }
    if (parser.state == ECE_HEADER_STATE_INVALID_HEADER) {
      goto error;
    }
  }
  if (parser.state != ECE_HEADER_STATE_END_VALUE) {
    // If the header ends with an unquoted value, the parser might still be in a
    // non-terminal state. Try to parse an extra space to reach the terminal
    // state.
    ece_header_parse(&parser, " ");
    if (parser.state != ECE_HEADER_STATE_END_VALUE) {
      // If we're still in a non-terminal state, the header is incomplete.
      goto error;
    }
  }
  return ece_header_params_reverse(parser.params);

error:
  ece_header_params_free(parser.params);
  return NULL;
}

int
ece_aes128gcm_payload_extract_params(const uint8_t* payload, size_t payloadLen,
                                     const uint8_t** salt, size_t* saltLen,
                                     const uint8_t** keyId, size_t* keyIdLen,
                                     uint32_t* rs, const uint8_t** ciphertext,
                                     size_t* ciphertextLen) {
  if (payloadLen < ECE_AES128GCM_HEADER_LENGTH) {
    return ECE_ERROR_SHORT_HEADER;
  }

  *saltLen = ECE_SALT_LENGTH;

  *keyIdLen = payload[ECE_SALT_LENGTH + 4];
  if (payloadLen < ECE_AES128GCM_HEADER_LENGTH + *keyIdLen) {
    return ECE_ERROR_SHORT_HEADER;
  }

  *rs = ece_read_uint32_be(&payload[ECE_SALT_LENGTH]);
  if (*rs < ECE_AES128GCM_MIN_RS) {
    return ECE_ERROR_INVALID_RS;
  }

  size_t payloadStart = ECE_AES128GCM_HEADER_LENGTH + *keyIdLen;
  *ciphertextLen = payloadLen - payloadStart;
  if (!(*ciphertextLen)) {
    return ECE_ERROR_ZERO_CIPHERTEXT;
  }

  *salt = payload;
  if (keyIdLen) {
    *keyId = &payload[ECE_AES128GCM_HEADER_LENGTH];
  } else {
    *keyId = NULL;
  }
  *ciphertext = &payload[payloadStart];

  return ECE_OK;
}

int
ece_webpush_aesgcm_headers_extract_params1(const char* cryptoKeyHeader,
                                          const char* encryptionHeader,
                                          uint8_t* salt, size_t saltLen,
                                          uint8_t* rawSenderPubKey,
                                          size_t rawSenderPubKeyLen,
                                          uint32_t* rs) {
  int err = ECE_OK;

  ece_header_params_t* encryptionParams = NULL;
  ece_header_params_t* cryptoKeyParams = NULL;
  char* keyId = NULL;
  uint32_t rsValue = 0;
  size_t decodedSaltLen = 0;
  size_t decodedKeyLen = 0;
	ece_header_params_t* cryptoKeyParam;

  // First, extract the key ID, salt, and record size from the first key in the
  // `Encryption` header.
  encryptionParams = ece_header_extract_params(encryptionHeader);
  if (!encryptionParams) {
    err = ECE_ERROR_INVALID_ENCRYPTION_HEADER;
    goto end;
  }
  for (ece_header_pairs_t* pair = encryptionParams->pairs; pair;
       pair = pair->next) {
    if (ece_header_pairs_has_name(pair, "keyid")) {
      // The key ID is optional, and is used to identify the public key in the
      // `Crypto-Key` header if multiple encryption keys are specified.
      if (keyId) {
        err = ECE_ERROR_INVALID_ENCRYPTION_HEADER;
        goto end;
      }
      keyId = ece_header_pairs_value_to_str(pair);
      if (!keyId) {
        err = ECE_ERROR_OUT_OF_MEMORY;
        goto end;
      }
      continue;
    }
    if (ece_header_pairs_has_name(pair, "rs")) {
      // The record size is optional.
      if (rsValue) {
        err = ECE_ERROR_INVALID_ENCRYPTION_HEADER;
        goto end;
      }
      char* value = ece_header_pairs_value_to_str(pair);
      if (!value) {
        err = ECE_ERROR_OUT_OF_MEMORY;
        goto end;
      }
      int result = sscanf(value, "%" SCNu32, &rsValue);
      free(value);
      if (result <= 0 || rsValue < ECE_AESGCM_MIN_RS) {
        err = ECE_ERROR_INVALID_RS;
        goto end;
      }
      continue;
    }
    if (ece_header_pairs_has_name(pair, "salt")) {
      // The salt is required, and must be Base64url-encoded without padding.
      if (decodedSaltLen) {
        err = ECE_ERROR_INVALID_ENCRYPTION_HEADER;
        goto end;
      }
      decodedSaltLen =
        ece_base64url_decode(pair->value, pair->valueLen,
                             ECE_BASE64URL_IGNORE_PADDING, salt, saltLen);
      if (!decodedSaltLen) {
        break;
      }
      continue;
    }
  }
  if (decodedSaltLen != saltLen) {
    err = ECE_ERROR_INVALID_SALT;
    goto end;
  }
  if (!rsValue) {
    // The record size defaults to 4096 if unspecified.
    rsValue = 4096;
  }
  *rs = rsValue;

  // Next, find the ephemeral public key in the `Crypto-Key` header.
  cryptoKeyParams = ece_header_extract_params(cryptoKeyHeader);
  if (!cryptoKeyParams) {
    err = ECE_ERROR_INVALID_CRYPTO_KEY_HEADER;
    goto end;
  }
  cryptoKeyParam = cryptoKeyParams;
  if (keyId) {
    // If the sender specified a key ID in the `Encryption` header, find the
    // matching parameter in the `Crypto-Key` header. Otherwise, we assume
    // there's only one key, and use the first one we see.
    while (cryptoKeyParam) {
      bool keyIdMatches = false;
      for (ece_header_pairs_t* pair = cryptoKeyParam->pairs; pair;
           pair = pair->next) {
        if (!ece_header_pairs_has_name(pair, "keyid")) {
          continue;
        }
        keyIdMatches = ece_header_pairs_has_value(pair, keyId);
        if (keyIdMatches) {
          break;
        }
      }
      if (keyIdMatches) {
        break;
      }
      cryptoKeyParam = cryptoKeyParam->next;
    }
    if (!cryptoKeyParam) {
      // We don't have a matching key ID with a `dh` name-value pair.
      err = ECE_ERROR_INVALID_DH;
      goto end;
    }
  }
  for (ece_header_pairs_t* pair = cryptoKeyParam->pairs; pair;
       pair = pair->next) {
    if (!ece_header_pairs_has_name(pair, "dh")) {
      continue;
    }
    // The sender's public key must be Base64url-encoded without padding.
    decodedKeyLen = ece_base64url_decode(pair->value, pair->valueLen,
                                         ECE_BASE64URL_IGNORE_PADDING,
                                         rawSenderPubKey, rawSenderPubKeyLen);
    break;
  }
  if (decodedKeyLen != rawSenderPubKeyLen) {
    err = ECE_ERROR_INVALID_DH;
    goto end;
  }

end:
  ece_header_params_free(encryptionParams);
  ece_header_params_free(cryptoKeyParams);
  free(keyId);
  return err;
}

int
ece_webpush_aesgcm_headers_from_params(const void* salt, size_t saltLen,
                                       const void* rawSenderPubKey,
                                       size_t rawSenderPubKeyLen, uint32_t rs,
                                       char* cryptoKeyHeader,
                                       size_t* cryptoKeyHeaderLen,
                                       char* encryptionHeader,
                                       size_t* encryptionHeaderLen) {
  size_t b64SenderPubKeyLen = ece_base64url_encode(
    rawSenderPubKey, rawSenderPubKeyLen, ECE_BASE64URL_OMIT_PADDING, NULL, 0);
  if (!b64SenderPubKeyLen ||
      b64SenderPubKeyLen > SIZE_MAX - ECE_HEADER_DH_PREFIX_LENGTH) {
    return ECE_ERROR_INVALID_DH;
  }
  size_t requiredCryptoKeyHeaderLen =
    b64SenderPubKeyLen + ECE_HEADER_DH_PREFIX_LENGTH;
  if (*cryptoKeyHeaderLen) {
    if (*cryptoKeyHeaderLen < requiredCryptoKeyHeaderLen) {
      return ECE_ERROR_OUT_OF_MEMORY;
    }
    memcpy(cryptoKeyHeader, ECE_HEADER_DH_PREFIX, ECE_HEADER_DH_PREFIX_LENGTH);
    ece_base64url_encode(
      rawSenderPubKey, rawSenderPubKeyLen, ECE_BASE64URL_OMIT_PADDING,
      &cryptoKeyHeader[ECE_HEADER_DH_PREFIX_LENGTH], b64SenderPubKeyLen);
  }
  *cryptoKeyHeaderLen = requiredCryptoKeyHeaderLen;

  size_t b64SaltLen =
    ece_base64url_encode(salt, saltLen, ECE_BASE64URL_OMIT_PADDING, NULL, 0);
  if (!b64SaltLen) {
    return ECE_ERROR_INVALID_SALT;
  }
  int maybeEncryptionPrefixLen = snprintf(NULL, 0, "rs=%" PRIu32 ";salt=", rs);
  if (maybeEncryptionPrefixLen <= 0) {
    return ECE_ERROR_INVALID_SALT;
  }
  size_t encryptionPrefixLen = (size_t) maybeEncryptionPrefixLen;
  if (b64SaltLen > SIZE_MAX - encryptionPrefixLen) {
    return ECE_ERROR_INVALID_SALT;
  }
  size_t requiredEncryptionHeaderLen = b64SaltLen + encryptionPrefixLen;
  if (*encryptionHeaderLen) {
    if (*encryptionHeaderLen < requiredEncryptionHeaderLen) {
      return ECE_ERROR_OUT_OF_MEMORY;
    }
    sprintf(encryptionHeader, "rs=%" PRIu32 ";salt=", rs);
    ece_base64url_encode(salt, saltLen, ECE_BASE64URL_OMIT_PADDING,
                         &encryptionHeader[encryptionPrefixLen], b64SaltLen);
  }
  *encryptionHeaderLen = requiredEncryptionHeaderLen;

  return ECE_OK;
}













// Extracts an unsigned 16-bit integer in network byte order.
static inline uint16_t
ece_read_uint16_be(const uint8_t* bytes) {
  uint16_t value = (uint16_t) bytes[1];
  value |= bytes[0] << 8;
  return value;
}
// Removes padding from a decrypted "aesgcm" block.
static int
ece_aesgcm_unpad(uint8_t* block, bool lastRecord, size_t* blockLen) {
  ECE_UNUSED(lastRecord);

  assert(*blockLen >= ECE_AESGCM_PAD_SIZE);

  uint16_t padLen = ece_read_uint16_be(block);
  if (padLen > *blockLen - ECE_AESGCM_PAD_SIZE) {
    return ECE_ERROR_DECRYPT_PADDING;
  }
  size_t plaintextStart = ECE_AESGCM_PAD_SIZE + padLen;

  for (size_t i = ECE_AESGCM_PAD_SIZE; i < plaintextStart; i++) {
    if (block[i]) {
      // All padding bytes must be zero.
      return ECE_ERROR_DECRYPT_PADDING;
    }
  }

  // Move the unpadded plaintext to the start of the block.
  *blockLen -= plaintextStart;
  memmove(block, &block[plaintextStart], *blockLen);
  return ECE_OK;
}

typedef int (*unpad_t)(uint8_t* block, bool lastRecord, size_t* blockLen);
// Calculates the maximum plaintext length, including room for the padding
// delimiter and padding.
static inline size_t
ece_plaintext_max_length(uint32_t rs, size_t padSize, size_t ciphertextLen) {
  assert(padSize <= 2);
  size_t overhead = padSize + ECE_TAG_LENGTH;
  if (rs <= overhead) {
    return 0;
  }
  size_t numRecords = ciphertextLen / rs;
  if (ciphertextLen % rs) {
    // If the ciphertext length doesn't fall on a record boundary, we have
    // a smaller final record.
    numRecords++;
  }
  if (numRecords > ciphertextLen / ECE_TAG_LENGTH) {
    // Each record includes a trailing auth tag. If the number of records
    // exceeds the number of tags, the ciphertext is truncated.
    return 0;
  }
  return ciphertextLen - (ECE_TAG_LENGTH * numRecords);
}
// Converts an encrypted record to a decrypted block.
static int
ece_decrypt_record(EVP_CIPHER_CTX* ctx, const uint8_t* key, const uint8_t* iv,
                   const uint8_t* record, size_t recordLen, uint8_t* block) {
  int chunkLen = -1;

  if (EVP_DecryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, key, iv) != 1) {
    return ECE_ERROR_DECRYPT;
  }

  assert(recordLen > ECE_TAG_LENGTH);
  size_t blockLen = recordLen - ECE_TAG_LENGTH;

  // The authentication tag is included at the end of the encrypted record.
  uint8_t tag[ECE_TAG_LENGTH];
  memcpy(tag, &record[blockLen], ECE_TAG_LENGTH);
  if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, ECE_TAG_LENGTH, tag) !=
      1) {
    return ECE_ERROR_DECRYPT;
  }

  if (blockLen > INT_MAX ||
      EVP_DecryptUpdate(ctx, block, &chunkLen, record, (int) blockLen) != 1) {
    return ECE_ERROR_DECRYPT;
  }

  // Since we're using a stream cipher, finalization shouldn't write out any
  // bytes.
  assert(EVP_CIPHER_CTX_block_size(ctx) == 1);
  if (EVP_DecryptFinal_ex(ctx, NULL, &chunkLen) != 1) {
    return ECE_ERROR_DECRYPT;
  }

  if (EVP_CIPHER_CTX_reset(ctx) != 1) {
    return ECE_ERROR_DECRYPT;
  }

  return ECE_OK;
}

static int
ece_decrypt_records(const uint8_t* key, const uint8_t* nonce, uint32_t rs,
                    size_t padSize, const uint8_t* ciphertext,
                    size_t ciphertextLen, unpad_t unpad, uint8_t* plaintext,
                    size_t* plaintextLen) {
  int err = ECE_OK;
	size_t plaintextStart;
	size_t ciphertextStart;
  EVP_CIPHER_CTX* ctx = NULL;

  // Make sure the plaintext array is large enough to hold the full plaintext.
  size_t maxPlaintextLen = ece_plaintext_max_length(rs, padSize, ciphertextLen);
  if (!maxPlaintextLen) {
    err = ECE_ERROR_DECRYPT;
    goto end;
  }
  if (*plaintextLen < maxPlaintextLen) {
    err = ECE_ERROR_OUT_OF_MEMORY;
    goto end;
  }

  ctx = EVP_CIPHER_CTX_new();
  if (!ctx) {
    err = ECE_ERROR_OUT_OF_MEMORY;
    goto end;
  }

  // The offset at which to start reading the ciphertext.
  ciphertextStart = 0;

  // The offset at which to start writing the plaintext.
  plaintextStart = 0;

  for (size_t counter = 0; ciphertextStart < ciphertextLen; counter++) {
    size_t ciphertextEnd;
    if (rs > ciphertextLen - ciphertextStart) {
      // This check is equivalent to `ciphertextStart + rs > ciphertextLen`;
      // it's written this way to avoid an integer overflow.
      ciphertextEnd = ciphertextLen;
    } else {
      ciphertextEnd = ciphertextStart + rs;
    }

    assert(ciphertextEnd > ciphertextStart);

    // The full length of the encrypted record.
    size_t recordLen = ciphertextEnd - ciphertextStart;
    if (recordLen <= ECE_TAG_LENGTH) {
      err = ECE_ERROR_SHORT_BLOCK;
      goto end;
    }

    // Generate the IV for this record using the nonce.
    uint8_t iv[ECE_NONCE_LENGTH];
    ece_generate_iv(nonce, counter, iv);

    // Decrypt the record.
    err = ece_decrypt_record(ctx, key, iv, &ciphertext[ciphertextStart],
                             recordLen, &plaintext[plaintextStart]);
    if (err) {
      goto end;
    }

    // `unpad` sets `blockLen` to the actual plaintext block length, without
    // the padding delimiter and padding.
    bool lastRecord = ciphertextEnd >= ciphertextLen;
    size_t blockLen = recordLen - ECE_TAG_LENGTH;
    if (blockLen < padSize) {
      err = ECE_ERROR_DECRYPT_PADDING;
      goto end;
    }
    err = unpad(&plaintext[plaintextStart], lastRecord, &blockLen);
    if (err) {
      goto end;
    }

    ciphertextStart = ciphertextEnd;
    plaintextStart += blockLen;
  }

  // Finally, set the actual plaintext length.
  *plaintextLen = plaintextStart;

end:
  EVP_CIPHER_CTX_free(ctx);
  return err;
}
// A generic decryption function shared by "aesgcm" and "aes128gcm".
// `deriveKeyAndNonce` and `unpad` are function pointers that change based on
// the scheme.
static int
ece_webpush_decrypt(const uint8_t* rawRecvPrivKey, size_t rawRecvPrivKeyLen,
                    const uint8_t* authSecret, size_t authSecretLen,
                    const uint8_t* salt, size_t saltLen,
                    const uint8_t* rawSenderPubKey, size_t rawSenderPubKeyLen,
                    uint32_t rs, size_t padSize, const uint8_t* ciphertext,
                    size_t ciphertextLen, needs_trailer_t needsTrailer,
                    derive_key_and_nonce_t deriveKeyAndNonce, unpad_t unpad,
                    uint8_t* plaintext, size_t* plaintextLen) {
  int err = ECE_OK;

  EC_KEY* recvPrivKey = NULL;
  EC_KEY* senderPubKey = NULL;

  if (authSecretLen != ECE_WEBPUSH_AUTH_SECRET_LENGTH) {
    err = ECE_ERROR_INVALID_AUTH_SECRET;
    goto end;
  }
  if (saltLen != ECE_SALT_LENGTH) {
    err = ECE_ERROR_INVALID_SALT;
    goto end;
  }
  if (!ciphertextLen) {
    err = ECE_ERROR_ZERO_CIPHERTEXT;
    goto end;
  }
  if (needsTrailer(rs, ciphertextLen)) {
    // If we're missing a trailing block, the ciphertext is truncated. This only
    // applies to "aesgcm".
    err = ECE_ERROR_DECRYPT_TRUNCATED;
    goto end;
  }

  recvPrivKey = ece_import_private_key(rawRecvPrivKey, rawRecvPrivKeyLen);
  if (!recvPrivKey) {
    err = ECE_ERROR_INVALID_PRIVATE_KEY;
    goto end;
  }
  senderPubKey = ece_import_public_key(rawSenderPubKey, rawSenderPubKeyLen);
  if (!senderPubKey) {
    err = ECE_ERROR_INVALID_PUBLIC_KEY;
    goto end;
  }

  uint8_t key[ECE_AES_KEY_LENGTH];
  uint8_t nonce[ECE_NONCE_LENGTH];
  err = deriveKeyAndNonce(ECE_MODE_DECRYPT, recvPrivKey, senderPubKey,
                          authSecret, authSecretLen, salt, saltLen, key, nonce);
  if (err) {
    goto end;
  }

  err = ece_decrypt_records(key, nonce, rs, padSize, ciphertext, ciphertextLen,
                            unpad, plaintext, plaintextLen);

end:
  EC_KEY_free(recvPrivKey);
  EC_KEY_free(senderPubKey);
  return err;
}

int
ece_webpush_aesgcm_decrypt1(const uint8_t* rawRecvPrivKey,
                           size_t rawRecvPrivKeyLen, const uint8_t* authSecret,
                           size_t authSecretLen, const uint8_t* salt,
                           size_t saltLen, const uint8_t* rawSenderPubKey,
                           size_t rawSenderPubKeyLen, uint32_t rs,
                           const uint8_t* ciphertext, size_t ciphertextLen,
                           uint8_t* plaintext, size_t* plaintextLen) {
  rs = ece_aesgcm_rs(rs);
  if (!rs) {
    return 0;
  }
  return ece_webpush_decrypt(
    rawRecvPrivKey, rawRecvPrivKeyLen, authSecret, authSecretLen, salt, saltLen,
    rawSenderPubKey, rawSenderPubKeyLen, rs, ECE_AESGCM_PAD_SIZE, ciphertext,
    ciphertextLen, &ece_aesgcm_needs_trailer,
    &ece_webpush_aesgcm_derive_key_and_nonce, &ece_aesgcm_unpad, plaintext,
    plaintextLen);
}
