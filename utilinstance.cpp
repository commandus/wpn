#include "utilinstance.h"
#include <cstdlib>
#include <algorithm>
#include "utilvapid.h"

/**
 * 1) Generates the random number in 8 bytes which is required by the server.
 * 		The server might reject the ID if there is a conflict or problem.
 * 2) Transforms the first 4 bits to 0x7. Note that this is required by the server.
 * 3) Encode the value in Android-compatible base64 scheme
 *		URL safe: '/' replaced by '_' and '+' replaced by '-'.
 *		No padding: any trailing '=' will be removed.
 * @see https://cs.chromium.org/chromium/src/components/gcm_driver/instance_id/instance_id_impl.cc?g=0&l=228
 */
std::string mkInstanceId()
{
	uint8_t bytes[8];
	srand(time(NULL));
	int *p0 = (int*) &bytes[0];
	int *p1 = (int*) &bytes[4];
	*p0 = rand();
	*p1 = rand();
	bytes[0] &= 0x0f;
	bytes[0] |= 0x70;
	std::string id = base64UrlEncode(bytes, sizeof(bytes));
	std::replace(id.begin(), id.end(), '+', '-');
	std::replace(id.begin(), id.end(), '/', '_');
	return id;
}
