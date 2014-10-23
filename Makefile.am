lib_LTLIBRARIES = libcows.la

libcows_la_SOURCES = libcows.c libcows_frame.c base64.c buffers.c

check_buffers_SOURCES = buffers.c
check_buffers_CFLAGS = -DUNIT_TEST_BUFFERS $(AM_CFLAGS)

check_PROGRAMS = check_buffers
TESTS = $(check_PROGRAMS)

