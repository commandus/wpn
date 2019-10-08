SUBDIRS = .
# In case of not protoc is available, copy precompiled protobuf
EXT_PRE = /home/andrei/src/wpn/mcs/gen
EXTGENMCS = $(EXT_PRE)/mcs.pb.h $(EXT_PRE)/android_checkin.pb.h $(EXT_PRE)/checkin.pb.h \
	$(EXT_PRE)/mcs.pb.cc $(EXT_PRE)/android_checkin.pb.cc $(EXT_PRE)/checkin.pb.cc

ACLOCAL_AMFLAGS = -I m4

#	Libraries
lib_LIBRARIES = libece.a libwpnpp.a
lib_LTLIBRARIES = libwpnapi.la libwpn.la

#	Binaries
bin_PROGRAMS = \
	wpnr wpnw wpn-print wpn-grant wpn wpnlink ec wpnlinkj

# commoncppflags = -O2 -I/usr/local/include -Imcs/gen -Ithird_party
# commoncppflags = -O2 -I/usr/include -Imcs/gen -Ithird_party
commoncppflags = -O2 -I/usr/include -Imcs/gen -Ithird_party -Ithird_party/ecec
commoncppflags += -I/usr/local/opt/openssl@1.1/include

# curllibs = -L/usr/local/lib/ -L. -lssl -lcrypto -lcurl
curllibs = -L. -lssl -lcrypto -lcurl
protolibs = -lprotobuf-lite
commonlibs = $(curllibs) $(protolibs) -L. -lece
commonlibs += -L/usr/local/opt/openssl@1.1/lib

#
#	Generate protobuf first, then grpc
#
proto = mcs/mcs.proto mcs/android_checkin.proto mcs/checkin.proto
gengrpcs_h = mcs/gen/mcs.pb.h mcs/gen/android_checkin.pb.h mcs/gen/checkin.pb.h
gengrpcs_c = mcs/gen/mcs.pb.cc mcs/gen/android_checkin.pb.cc mcs/gen/checkin.pb.cc
gengrpcs = $(gengrpcs_h) $(gengrpcs_c)
$(gengrpcs): Makefile
	protoc -I ./mcs --cpp_out=mcs/gen $(proto) || cp $(EXTGENMCS) mcs/gen

#
#	Exclude generated sources from distribution
#
nodist_wpn_SOURCES = $(gengrpcs)
BUILT_SOURCES = $(gengrpcs)
CLEANFILES = $(gengrpcs)

QRHEADERS = \
	third_party/nayuki/QR-Code-generator/BitBuffer.hpp \
	third_party/nayuki/QR-Code-generator/QrCode.hpp \
	third_party/nayuki/QR-Code-generator/QrSegment.hpp
QRSOURCES = \
	third_party/nayuki/QR-Code-generator/BitBuffer.cpp \
	third_party/nayuki/QR-Code-generator/QrCode.cpp \
	third_party/nayuki/QR-Code-generator/QrSegment.cpp

ARGT3HEADERS = third_party/argtable3/argtable3.h
ARGT3SOURCES = third_party/argtable3/argtable3.c

ECECSOURCES = \
	third_party/ecec/base64url.c  third_party/ecec/decrypt.c  third_party/ecec/encrypt.c  third_party/ecec/keys.c \
	third_party/ecec/params.c  third_party/ecec/trailer.c
ECECHEADERS = \
	third_party/ecec/ece.h \
	third_party/ecec/ece/keys.h	third_party/ecec/ece/trailer.h
wpn_lib_source = \
	wp-storage-file.cpp notify2string.cpp \
	wp-push.cpp wp-subscribe.cpp wp-connection.cpp \
	sslfactory.cpp \
	mcs/mcsclient.cpp mcs/heartbeat.cpp \
	$(gengrpcs_c) \
	utilfile.cpp utilstring.cpp params.cpp commandoutput.cpp endpoint.cpp \
	utilqr.cpp $(QRSOURCES) \
	vapid.cpp utilvapid.cpp utilrecv.cpp utilinstance.cpp wp-registry.cpp utiljson.cpp \
	config-filename.cpp

wpnw_source = \
	tools/wpnw/wpnw.cpp \
	wp-storage-file.cpp notify2string.cpp \
	wp-push.cpp wp-subscribe.cpp wp-connection.cpp \
	sslfactory.cpp \
	$(gengrpcs_c) \
	utilfile.cpp utilstring.cpp params.cpp commandoutput.cpp endpoint.cpp \
	vapid.cpp utilvapid.cpp utilrecv.cpp utilinstance.cpp wp-registry.cpp utiljson.cpp \
	config-filename.cpp

#
# Comment this line for old gcc versions 
# Usedful to build wpnw under CentOS 6
#
AM_CXXFLAGS = -std=c++11 

#
#	libece.a
#
libece_a_SOURCES = $(ECECSOURCES)
libece_a_CPPFLAGS = -std=c99 $(commoncppflags) -Ithird_party/ecec

#
#	libwpnpp.a C++ static library
#
libwpnpp_a_SOURCES = $(wpn_lib_source)
libwpnpp_a_CPPFLAGS = $(commoncppflags)

#
#	libwpn.la C++ shared library
#   $(wpn_lib_source)
#
libwpn_la_SOURCES = 
libwpn_la_LIBADD = -L. -lwpnpp -lece
libwpn_la_CPPFLAGS = $(commoncppflags)
libwpn_la_LDFLAGS = -version-info 0:2:0

#
#	libwpnapi.la C shared library (Delphi compatible)
#	$(wpn_lib_source)
#
libwpnapi_la_SOURCES = wpnapi.cpp
libwpnapi_la_LIBADD = -L. -lwpnpp -lece
libwpnapi_la_CPPFLAGS = $(commoncppflags) 
libwpnapi_la_LDFLAGS = -version-info 0:2:0

nobase_dist_include_HEADERS = \
	platform.h vapid.h notify2string.h \
	wpn-config.h wpn.h wpn-notify.h utilvapid.h utilrecv.h utiljson.h endpoint.h \
	wp-storage-file.h wp-subscribe.h wp-connection.h wp-push.h wp-registry.h \
	wpnapi.h sslfactory.h utilstring.h utilinstance.h utilfile.h commandoutput.h params.h \
	mcs/mcsclient.h mcs/heartbeat.h \
	onullstream.hpp \
	third_party/nlohmann/json.hpp \
	utilqr.h $(QRHEADERS) $(ECECHEADERS) \
	$(ARGT3HEADERS) \
	$(gengrpcs_h) \
	config-filename.h

common_src = $(ARGT3SOURCES)

#
#	wpn
#
wpn_SOURCES = \
	wpn-config.cpp wpn.cpp \
	$(common_src)
wpn_LDADD = -L. -lwpnpp -lece $(curllibs) $(protolibs) -ldl
wpn_LDFLAGS = -static
wpn_CPPFLAGS = $(commoncppflags)

#
# ----- Static -----
#

#
#	wpnr
#
wpnr_SOURCES = \
	tools/wpnr/wpnr.cpp \
	$(common_src)
wpnr_LDADD = -L. -lwpnpp -lece $(curllibs) $(protolibs)
wpnr_LDFLAGS = -static
wpnr_CPPFLAGS = $(commoncppflags)

#
#	wpnw
#
wpnw_SOURCES= \
	$(wpnw_source) \
	$(common_src)
wpnw_LDADD = -L. -lece $(curllibs) $(protolibs)
wpnw_LDFLAGS = -static
wpnw_CPPFLAGS = $(commoncppflags)

#
#	wpn-print
#
wpn_print_SOURCES= \
	tools/wpn-print/wpn-print.cpp \
	$(common_src)
wpn_print_LDADD = $(commonlibs) -L. -lwpnpp -lece $(curllibs) $(protolibs)
wpn_print_LDFLAGS = -static
wpn_print_CPPFLAGS = $(commoncppflags)

#
#	wpnlink
#
wpnlink_SOURCES= \
	tools/wpnlink/wpnlink.cpp \
	$(common_src)

wpnlink_LDADD = $(commonlibs) -L. -lwpnpp -lece $(curllibs)
wpnlink_LDFLAGS = -static
wpnlink_CPPFLAGS = $(commoncppflags)

#
#	wpnlinkj
#
wpnlinkj_SOURCES= \
	tools/wpnlinkj/wpnlinkj.cpp \
	$(common_src)
wpnlinkj_LDADD = $(commonlibs) -L. -lwpnpp -lece $(curllibs) $(protolibs)
wpnlinkj_LDFLAGS = -static
wpnlinkj_CPPFLAGS = $(commoncppflags)

#
#	ec
#
ec_SOURCES= \
	tests/ec.cpp vapid.cpp \
	$(common_src)

ec_LDADD = -lssl -lcrypto -lece
ec_CPPFLAGS = $(commoncppflags)

#
#	wpn-grant
#
wpn_grant_SOURCES = \
	wpn-grant.cpp \
	$(common_src)

wpn_grant_LDADD = $(commonlibs) -L. -lwpnpp -lece $(curllibs) $(protolibs)
wpn_grant_LDFLAGS = -static
wpn_grant_CPPFLAGS = $(commoncppflags)

#
# ----- Build library first -----
#
$(commonlibs) : libece.a

libwpnpp.la : libwpnpp.a
libwpnapi.la : libwpnpp.a

#
# ----- With static libraries, library first -----
#
wpn.cpp : libwpnpp.a
wpn-grant.cpp : libwpnpp.a
tools/wpnr/wpnr.cpp : libwpnpp.a
# tools/wpnw/wpnw.cpp : libwpnpp.a
tools/wpnw/wpnw.cpp : libece.a
tools/wpn-print/wpn-print.cpp : libwpnpp.a
tools/wpnlink/wpnlink.cpp : libwpnpp.a
tools/wpnlink/wpnlinkj.cpp : libwpnpp.a

#
# ----- With shared libraries, shared library first -----
#
tools/wpnr/wpnr.cpp : libwpnapi.la
# tools/wpnw/wpnw.cpp : libwpnapi.la
tools/wpn-print/wpn-print.cpp : libwpnapi.la
tools/wpnlink/wpnlink.cpp : libwpnapi.la
tools/wpnlink/wpnlinkj.cpp : libwpnapi.la

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	$(proto) \
	vs/dllmain.cpp vs/libwpn-c.vcxproj vs/resource-webpush-curl.h vs/webpush-curl.vcxproj vs/wpn.sln \
	vs/icon1.ico vs/libwpn-c.vcxproj.user vs/resource-wpn.h vs/webpush-curl.vcxproj.user vs/wpn.vcxproj \
	vs/libwpn-c.rc vs/resource-libwpn-c.h vs/webpush-curl.rc vs/wpn.rc vs/wpn.vcxproj.user \
	CMakeLists.txt \
	LICENSE COPYING CODE_OF_CONDUCT.md CONTRIBUTING.md HISTORY README.md TODO autogen.sh \
	incoming-call email-template.html \
	tools/mkcert tools/install-protobuf-2.6.1.sh \
	tools/android/build-curl tools/android/build-openssl tools/android/build-protobuf tools/android/build-wpn tools/android/build-z \
	tools/wpn-print/README.wpn-print.md tools/wpnr/README.wpnr.md tools/wpnw/README.wpnw.md
