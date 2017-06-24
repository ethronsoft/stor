# Copyright (c) 2011 The LevelDB Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file. See the AUTHORS file for names of contributors.

#-----------------------------------------------
# Uncomment exactly one of the lines labelled (A), (B), and (C) below
# to switch between compilation modes.

# (A) Production use (optimized mode)
 OPT ?= -O2 -DNDEBUG -m64
# (B) Debug mode, w/ full line-level debugging symbols
# OPT ?= -g2
# (C) Profiling mode: opt, but w/debugging symbols
# OPT ?= -O2 -g2 -DNDEBUG
#-----------------------------------------------

# cmd.exe won't be able to run build_detect_platform,
# so let's manually inject the following variables:
#   CC                          C Compiler path
#   CXX                         C++ Compiler path
#   PLATFORM_LDFLAGS            Linker flags
#   PLATFORM_LIBS               Libraries flags
#   PLATFORM_SHARED_EXT         Extension for shared libraries
#   PLATFORM_SHARED_LDFLAGS     Flags for building shared 

# The PLATFORM_CCFLAGS and PLATFORM_CXXFLAGS might include the following:
#
#       -DLEVELDB_ATOMIC_PRESENT     if <atomic> is present
#       -DLEVELDB_PLATFORM_POSIX     for Posix-based platforms
#       -DSNAPPY                     if the Snappy library is present


CC = gcc
CXX = g++

PLATFORM_CCFLAGS=
PLATFORM_CXXFLAGS=
PLATFORM_LDFLAGS=
PLATFORM_LIBS=
PLATFORM_SHARED_EXT="so"
PLATFORM_SHARED_LDFLAGS="-shared -Wl,-soname -Wl,"
PLATFORM_SHARED_CFLAGS="-fPIC"
PLATFORM_SHARED_VERSIONED=true

PLATFORM=OS_WINDOWS
COMMON_FLAGS=-fno-builtin-memcmp -D_REENTRANT -DOS_WINDOWS -DLEVELDB_PLATFORM_WINDOWS -DWINVER=0x0500 -D__USE_MINGW_ANSI_STDIO=1
PLATFORM_SOURCES=util/env_win.cc
PLATFORM_LIBS=-lshlwapi
PORT_FILE=port/port_win.cc
CROSS_COMPILE=true

PLATFORM_CCFLAGS:= $(PLATFORM_CCFLAGS) $(COMMON_FLAGS)
PLATFORM_CXXFLAGS:= $(PLATFORM_CXXFLAGS) $(COMMON_FLAGS)

SOURCES = \
	db/builder.cc \
	db/c.cc \
	db/db_impl.cc \
	db/db_iter.cc \
	db/dbformat.cc \
	db/dumpfile.cc \
	db/filename.cc \
	db/log_reader.cc \
	db/log_writer.cc \
	db/memtable.cc \
	db/repair.cc \
	db/table_cache.cc \
	db/version_edit.cc \
	db/version_set.cc \
	db/write_batch.cc \
	table/block.cc \
	table/block_builder.cc \
	table/filter_block.cc \
	table/format.cc \
	table/iterator.cc \
	table/merger.cc \
	table/table.cc \
	table/table_builder.cc \
	table/two_level_iterator.cc \
	util/arena.cc \
	util/bloom.cc \
	util/cache.cc \
	util/coding.cc \
	util/comparator.cc \
	util/crc32c.cc \
	util/env.cc \
	util/filter_policy.cc \
	util/hash.cc \
	util/histogram.cc \
	util/logging.cc \
	util/options.cc \
	util/status.cc \
	$(PLATFORM_SOURCES) \
	$(PORT_FILE)	

MEMENV_SOURCES=\
	helpers/memenv/memenv.cc

CFLAGS += -I. -I./include $(PLATFORM_CCFLAGS) $(OPT)
CXXFLAGS += -I. -I./include $(PLATFORM_CXXFLAGS) $(OPT)

LDFLAGS += $(PLATFORM_LDFLAGS)
LIBS += $(PLATFORM_LIBS)

LIBOBJECTS = $(SOURCES:.cc=.o)
MEMENVOBJECTS = $(MEMENV_SOURCES:.cc=.o)

TESTUTIL = ./util/testutil.o
TESTHARNESS = ./util/testharness.o $(TESTUTIL)

TESTS = \
	arena_test \
	autocompact_test \
	bloom_test \
	c_test \
	cache_test \
	coding_test \
	corruption_test \
	crc32c_test \
	db_test \
	dbformat_test \
	env_test \
	filename_test \
	filter_block_test \
	hash_test \
	issue178_test \
	issue200_test \
	log_test \
	memenv_test \
	skiplist_test \
	table_test \
	version_edit_test \
	version_set_test \
	write_batch_test

PROGRAMS = db_bench leveldbutil $(TESTS)
BENCHMARKS = db_bench_sqlite3 db_bench_tree_db

LIBRARY = libleveldb.a
MEMENVLIBRARY = libmemenv.a

default: all

all: $(LIBRARY)

check: all $(PROGRAMS) $(TESTS)
	for t in $(TESTS); do echo "***** Running $$t"; ./$$t || exit 1; done

clean:
	-rm -f $(PROGRAMS) $(BENCHMARKS) $(LIBRARY) $(MEMENVLIBRARY) */*.o */*/*.o

$(LIBRARY): $(LIBOBJECTS)
#	rm -f $@
	$(AR) -rs $@ $(LIBOBJECTS)

db_bench: db/db_bench.o $(LIBOBJECTS) $(TESTUTIL)
	$(CXX) $(LDFLAGS) db/db_bench.o $(LIBOBJECTS) $(TESTUTIL) -o $@ $(LIBS)

db_bench_sqlite3: doc/bench/db_bench_sqlite3.o $(LIBOBJECTS) $(TESTUTIL)
	$(CXX) $(LDFLAGS) doc/bench/db_bench_sqlite3.o $(LIBOBJECTS) $(TESTUTIL) -o $@ -lsqlite3 $(LIBS)

db_bench_tree_db: doc/bench/db_bench_tree_db.o $(LIBOBJECTS) $(TESTUTIL)
	$(CXX) $(LDFLAGS) doc/bench/db_bench_tree_db.o $(LIBOBJECTS) $(TESTUTIL) -o $@ -lkyotocabinet $(LIBS)

leveldbutil: db/leveldb_main.o $(LIBOBJECTS)
	$(CXX) $(LDFLAGS) db/leveldb_main.o $(LIBOBJECTS) -o $@ $(LIBS)

arena_test: util/arena_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/arena_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

autocompact_test: db/autocompact_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/autocompact_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

bloom_test: util/bloom_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/bloom_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

c_test: db/c_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/c_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

cache_test: util/cache_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/cache_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

coding_test: util/coding_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/coding_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

corruption_test: db/corruption_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/corruption_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

crc32c_test: util/crc32c_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/crc32c_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

db_test: db/db_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/db_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

dbformat_test: db/dbformat_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/dbformat_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

env_test: util/env_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/env_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

filename_test: db/filename_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/filename_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

filter_block_test: table/filter_block_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) table/filter_block_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

hash_test: util/hash_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) util/hash_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

issue178_test: issues/issue178_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) issues/issue178_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

issue200_test: issues/issue200_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) issues/issue200_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

log_test: db/log_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/log_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

table_test: table/table_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) table/table_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

skiplist_test: db/skiplist_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/skiplist_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

version_edit_test: db/version_edit_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/version_edit_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

version_set_test: db/version_set_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/version_set_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

write_batch_test: db/write_batch_test.o $(LIBOBJECTS) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) db/write_batch_test.o $(LIBOBJECTS) $(TESTHARNESS) -o $@ $(LIBS)

$(MEMENVLIBRARY) : $(MEMENVOBJECTS)
	rm -f $@
	$(AR) -rs $@ $(MEMENVOBJECTS)

memenv_test : helpers/memenv/memenv_test.o $(MEMENVLIBRARY) $(LIBRARY) $(TESTHARNESS)
	$(CXX) $(LDFLAGS) helpers/memenv/memenv_test.o $(MEMENVLIBRARY) $(LIBRARY) $(TESTHARNESS) -o $@ $(LIBS)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

