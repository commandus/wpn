SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4

#	Libraries
lib_LIBRARIES = \
	libece.a

P = ../../third_party/ecec

ECECSOURCES = \
	$P/base64url.c  $P/decrypt.c  $P/encrypt.c  $P/keys.c \
	$P/params.c  $P/trailer.c
ECECHEADERS = \
	$P/ece.h \
	$P/ece/keys.h	$P/ece/trailer.h

libece_a_SOURCES = $(ECECSOURCES)
libece_a_CPPFLAGS = $(commoncppflags) -I$P

nobase_dist_include_HEADERS = \
	$(ECECHEADERS) 
