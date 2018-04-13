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
	wpn-config.h  wpn.h wp-storage-file.h wp-subscribe.h wp-connection.h \
	mcs-client.h \
	$(gengrpcs_h)
#	mcs/mcs-client.h \	
#	mcs/mcs-message.h mcs/mcs-util.h
common_src = 

commonlibs = -L/usr/local/lib/ -lcrypto -largtable2 -lcurl -lece -lnghttp2 -lglog -lunwind -lgrpc++ -lgrpc -lgpr -lprotobuf

#
#	wpn
#
wpn_SOURCES= \
	wpn-config.cpp wpn.cpp wp-storage-file.cpp wp-subscribe.cpp wp-connection.cpp \
	mcs-client.cpp \
	$(common_src)
#	mcs/mcs-client.cpp	
#	mcs/mcs-message.cpp mcs/mcs-util.cpp

wpn_LDADD = $(commonlibs)
wpn_CPPFLAGS = $(commoncppflags)

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	$(proto) \
	COPYING  CODE_OF_CONDUCT.md  CONTRIBUTING.md  HISTORY  README.md autogen.sh .kdev_include_paths
