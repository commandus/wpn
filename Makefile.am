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

nobase_dist_include_HEADERS = \
	wpn-config.h  wpn.h \
	wp-storage-file.h wp-subscribe.h wp-connection.h wp-push.h \
	sslfactory.h \
	utilstring.h \
	mcs/mcsclient.h \
	oauth/oauth.h oauth/tokenpair.h oauth/oauth20.h oauth/oauthrequest.h \
	third_party/nlohmann/json.hpp \
	third_party/sole/sole.hpp \
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
	oauth/tokenpair.cpp oauth/oauth20.cpp oauth/oauthrequest.cpp \
	utilstring.cpp params.cpp \
	$(common_src)

wpn_LDADD = $(commonlibs)
wpn_CPPFLAGS = $(commoncppflags)

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	$(proto) \
	COPYING  CODE_OF_CONDUCT.md  CONTRIBUTING.md  HISTORY  README.md autogen.sh .kdev_include_paths
