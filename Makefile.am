SUBDIRS = .
ACLOCAL_AMFLAGS = -I m4

commoncppflags = -std=c++11 -O2 -I/usr/local/include

#
#	Exclude generated sources from distribution
#
nodist_wpn_SOURCES =
BUILT_SOURCES =
CLEANFILES =

#
#	Binaries
#
bin_PROGRAMS = wpn

nobase_dist_include_HEADERS = \
	wpn-config.h  wpn.h wp-storage-file.h
common_src = 

commonlibs = -L/usr/local/lib/ -lcrypto -largtable2 -lcurl -lece -lnghttp2

#
#	wpn
#
wpn_SOURCES= \
	wpn-config.cpp wpn.cpp wp-storage-file.cpp \
	$(common_src)

wpn_LDADD = $(commonlibs)
wpn_CPPFLAGS = $(commoncppflags)

#
#	Configs, readme, CMake etc.
#
configdir = $(datadir)
dist_config_DATA = \
	COPYING  CODE_OF_CONDUCT.md  CONTRIBUTING.md  HISTORY  README.md autogen.sh .kdev_include_paths
