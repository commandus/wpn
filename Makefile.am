SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4

commoncppflags = -std=c++11 -O2 -I/usr/local/include -Imcs/gen -Ithird_party

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

#
#	Binaries
#
bin_PROGRAMS = wpn

QRHEADERS = third_party/nayuki/QR-Code-generator/BitBuffer.hpp third_party/nayuki/QR-Code-generator/QrCode.hpp third_party/nayuki/QR-Code-generator/QrSegment.hpp
QRSOURCES = third_party/nayuki/QR-Code-generator/BitBuffer.cpp third_party/nayuki/QR-Code-generator/QrCode.cpp third_party/nayuki/QR-Code-generator/QrSegment.cpp

nobase_dist_include_HEADERS = \
	platform.h \
	wpn-config.h wpn.h wpn-notify.h \
	wp-storage-file.h wp-subscribe.h wp-connection.h wp-push.h \
	sslfactory.h \
	utilstring.h commandoutput.h \
	mcs/mcsclient.h \
	onullstream.hpp \
	third_party/nlohmann/json.hpp \
	third_party/sole/sole.hpp \
	utilqr.h $(QRHEADERS) \
	utilvapid.h \
	$(gengrpcs_h)
common_src = 

commonlibs = -L/usr/local/lib/ -lssl -lcrypto -largtable2 -lcurl -lece -lnghttp2 -lglog -lunwind -lgrpc++ -lgrpc -lgpr -lprotobuf

#
#	wpn
#
wpn_SOURCES= \
	wpn-config.cpp wpn.cpp wp-storage-file.cpp \
	wp-push.cpp wp-subscribe.cpp wp-connection.cpp \
	sslfactory.cpp \
	mcs/mcsclient.cpp \
	utilstring.cpp params.cpp commandoutput.cpp \
	utilqr.cpp $(QRSOURCES) \
	utilvapid.cpp \
	$(common_src)

wpn_LDADD = $(commonlibs) -ldl
wpn_CPPFLAGS = $(commoncppflags)

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	$(proto) \
	COPYING  CODE_OF_CONDUCT.md  CONTRIBUTING.md  HISTORY  README.md autogen.sh .kdev_include_paths
