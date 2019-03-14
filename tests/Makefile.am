
AM_CXXFLAGS = -D_UNIT_TEST -I$(top_srcdir)/src

SOURCES = tests.cpp \
		  crc32.cpp \
		  random.cpp \
		  datetime.cpp \
		  ring_queue.cpp \
		  preprocessor.cpp \
		  delegate.cpp \
		  mmap_vector.cpp \
		  entry_index.cpp \
		  entry_manager.cpp \
		  skiplist.cpp

noinst_PROGRAMS = tests
tests_SOURCES = $(SOURCES)
tests_LDADD = $(top_builddir)/src/libetl.la -lgtest -lpthread
