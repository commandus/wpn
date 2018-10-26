SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4

#	Libraries
lib_LIBRARIES = \
	libwpn.a
lib_LTLIBRARIES = \
	libwpn.la libwpnapi.la

#	Binaries
bin_PROGRAMS = \
	wpn \
	webpush-curl \
	wpnr wpnw wpnlink wpnadd

commoncppflags = -O2 -I/usr/local/include -Imcs/gen -Ithird_party
commonlibs = -L/usr/local/lib/ -L. -lssl -lcrypto -lcurl -lece -lnghttp2 -lglog -lunwind -lprotobuf

#
#	Generate protobuf first, then grpc
#
proto = mcs/mcs.proto mcs/android_checkin.proto mcs/checkin.proto
gengrpcs_h = mcs/gen/mcs.pb.h mcs/gen/android_checkin.pb.h mcs/gen/checkin.pb.h
gengrpcs_c = mcs/gen/mcs.pb.cc mcs/gen/android_checkin.pb.cc mcs/gen/checkin.pb.cc
gengrpcs = $(gengrpcs_h) $(gengrpcs_c)
$(gengrpcs): Makefile
	protoc -I ./mcs --cpp_out=mcs/gen $(proto)

#
#	Exclude generated sources from distribution
#
nodist_wpn_SOURCES = $(gengrpcs)
BUILT_SOURCES = $(gengrpcs)
CLEANFILES = $(gengrpcs)

QRHEADERS = third_party/nayuki/QR-Code-generator/BitBuffer.hpp third_party/nayuki/QR-Code-generator/QrCode.hpp third_party/nayuki/QR-Code-generator/QrSegment.hpp
QRSOURCES = third_party/nayuki/QR-Code-generator/BitBuffer.cpp third_party/nayuki/QR-Code-generator/QrCode.cpp third_party/nayuki/QR-Code-generator/QrSegment.cpp

ARGT3HEADERS = third_party/argtable3/argtable3.h
ARGT3SOURCES = third_party/argtable3/argtable3.c

wpn_lib_source = \
	wp-storage-file.cpp \
	wp-push.cpp wp-subscribe.cpp wp-connection.cpp \
	sslfactory.cpp \
	mcs/mcsclient.cpp \
	$(gengrpcs_c) \
	utilstring.cpp params.cpp commandoutput.cpp \
	utilqr.cpp $(QRSOURCES) \
	utilvapid.cpp utilrecv.cpp utilinstance.cpp
AM_CXXFLAGS = -std=c++11 

#
#	libwpn.a
#
libwpn_a_SOURCES = $(wpn_lib_source)
libwpn_a_CPPFLAGS = $(commoncppflags)

#
#	libwpn.la C++ shared library
#
libwpn_la_SOURCES = $(wpn_lib_source)
libwpn_la_CPPFLAGS = $(commoncppflags)
libwpn_la_LDFLAGS = -version-info 0:2:0

#
#	libwpnapi.la C shared library (Delphi compatible)
#
libwpnapi_la_SOURCES = $(wpn_lib_source) wpnapi.cpp
libwpnapi_la_CPPFLAGS = $(commoncppflags) 
libwpnapi_la_LDFLAGS = -version-info 0:2:0

nobase_dist_include_HEADERS = \
	platform.h \
	wpn-config.h wpn.h wpn-notify.h utilvapid.h utilrecv.h \
	wp-storage-file.h wp-subscribe.h wp-connection.h wp-push.h \
	wpnapi.h sslfactory.h utilstring.h utilinstance.h utilfile.h commandoutput.h params.h \
	mcs/mcsclient.h \
	onullstream.hpp \
	third_party/nlohmann/json.hpp \
	utilqr.h $(QRHEADERS) \
	$(ARGT3HEADERS) \
	$(gengrpcs_h) \
	config-filename.h

common_src = $(ARGT3SOURCES)

#
#	wpn
#
wpn_SOURCES= \
	wpn-config.cpp wpn.cpp wp-storage-file.cpp \
	wp-push.cpp wp-subscribe.cpp wp-connection.cpp sslfactory.cpp \
	mcs/mcsclient.cpp \
	utilstring.cpp params.cpp commandoutput.cpp \
	utilqr.cpp $(QRSOURCES) \
	utilvapid.cpp utilrecv.cpp utilinstance.cpp config-filename.cpp \
	$(common_src)

wpn_LDADD = $(commonlibs) -ldl
wpn_CPPFLAGS = $(commoncppflags)

#
#	webpush-curl
#
webpush_curl_SOURCES= \
	tools/curl/webpush-curl.cpp \
	utilvapid.cpp \
	$(common_src)

webpush_curl_LDADD = $(commonlibs)
webpush_curl_CPPFLAGS = $(commoncppflags)

#
#	wpnr (with shared library)
#
wpnr_SOURCES= \
	tools/wpnr/wpnr.cpp config-filename.cpp utilfile.cpp \
	$(common_src)

wpnr_LDADD = -L.libs -lwpnapi $(commonlibs)
wpnr_CPPFLAGS = $(commoncppflags)

#
#	wpnw (with shared library)
#
wpnw_SOURCES= \
	tools/wpnw/wpnw.cpp config-filename.cpp \
	$(common_src)

wpnw_LDADD = -L.libs -lwpnapi $(commonlibs)
wpnw_CPPFLAGS = $(commoncppflags)

#
#	wpnadd (with shared library)
#
wpnadd_SOURCES= \
	tools/wpnadd/wpnadd.cpp config-filename.cpp utilfile.cpp \
	$(common_src)

wpnadd_LDADD = -L.libs -lwpnapi $(commonlibs)
wpnadd_CPPFLAGS = $(commoncppflags)

#
#	wpnlink (with shared library)
#
wpnlink_SOURCES= \
	tools/wpnlink/wpnlink.cpp config-filename.cpp \
	$(common_src)

wpnlink_LDADD = -L.libs -lwpnapi $(commonlibs)
wpnlink_CPPFLAGS = $(commoncppflags)

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	$(proto) \
	vs/wpn.sln vs/libwpn-dynamic.vcxproj vs/libwpn-static.vcxproj vs/webpush-curl.vcxproj vs/wpn.vcxproj vs/dllmain.cpp \
	CMakeLists.txt \
	tools/mkcert tools/curl/README \
	tools/wpnr/README.md tools/wpnw/README.md tools/wpnlink/README.md \
	LICENSE COPYING CODE_OF_CONDUCT.md CONTRIBUTING.md HISTORY README.md \
	autogen.sh email-template.html
