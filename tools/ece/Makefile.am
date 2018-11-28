SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4

#	Libraries
lib_LIBRARIES = \
	libece.a

P = ../../third_party/ecec

ECECSOURCES = \
	../../third_party/ecec/base64url.c  \
	../../third_party/ecec/decrypt.c \
	../../third_party/ecec/encrypt.c \
	../../third_party/ecec/keys.c \
	../../third_party/ecec/params.c \
	../../third_party/ecec/trailer.c
ECECHEADERS = \
	../../third_party/ecec/ece.h \
	../../third_party/ecec/ece/keys.h	$P/ece/trailer.h

libece_a_SOURCES = $(ECECSOURCES)
libece_a_CPPFLAGS = $(commoncppflags) -I../../third_party/ecec

nobase_dist_include_HEADERS = \
	$(ECECHEADERS) 
