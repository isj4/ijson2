config=debug
ARCH=$(shell uname -m)

CXXFLAGS:=-g -std=c++11
CXXFLAGS += -MMD -MP

#compiler warning setup
ifeq ($(findstring clang++, $(CXX)),clang++)
CXXFLAGS += -Wall -Wvla
CXXFLAGS += -Wshift-negative-value
CXXFLAGS += -Wodr
CXXFLAGS += -Wswitch-bool
CXXFLAGS += -Wlogical-not-parentheses
CXXFLAGS += -Wsizeof-array-argument
CXXFLAGS += -Wstrict-aliasing=1
CXXFLAGS += -Weverything
CXXFLAGS += -Wno-c++98-compat-pedantic
CXXFLAGS += -Wno-global-constructors
CXXFLAGS += -Wno-exit-time-destructors
CXXFLAGS += -Wno-padded
CXXFLAGS += -Wno-weak-vtables
CXXFLAGS += -Wno-disabled-macro-expansion
CXXFLAGS += -Wno-undefined-reinterpret-cast #we know what we're doing in Endianess.cc
CXXFLAGS += -Wno-sign-conversion
CXXFLAGS += -Wno-unknown-pragmas
CXXFLAGS += -Wno-old-style-cast
CXXFLAGS += -Wno-float-equal
else ifeq ($(findstring g++, $(CXX)),g++)
CXXFLAGS += -Wall -Wvla
CXXFLAGS += -Wshift-negative-value
CXXFLAGS += -Wodr
CXXFLAGS += -Wswitch-bool
CXXFLAGS += -Wlogical-not-parentheses
CXXFLAGS += -Wsizeof-array-argument
CXXFLAGS += -Wstrict-aliasing=1
CXXFLAGS += -Wextra
CXXFLAGS += -Wno-unknown-pragmas
CXXFLAGS += -Wduplicated-branches
CXXFLAGS += -Wshift-overflow=2
CXXFLAGS += -Wduplicated-cond
CXXFLAGS += -Wbool-compare
CXXFLAGS += -Wunused
else ifeq ($(findstring icc, $(CXX)),icc)
CXXFLAGS += -Wall -Wvla
CXXFLAGS += -Wstrict-aliasing=1
CXXFLAGS += -Wextra
CXXFLAGS += -Wno-unknown-pragmas
CXXFLAGS += -Wunused
CXXFLAGS += -wd2102  #violation of ansi-alias rules  (happens in std::vector<>)
CXXFLAGS += -wd11074 -wd11076 #Inlining inhibited by...
endif

#compiler optimization setup
ifeq ($(config),release)
CXXFLAGS += -march=native -mtune=native -O3
ifeq ($(ARCH),x86_64)
#nothing special
else ifeq ($(ARCH),armv7l)
#nothing special
endif
CXXFLAGS += -fstrict-aliasing
endif


.PHONY: default
default: libijson2.a


.PHONY: clean
clean:
	rm -f libijson2.a
	rm -f *.o
	rm -f *.d
	rm -f test_ijson2
	rm -f ijson2_unittest ijson2_parser_unittest ijson2_formatter_unittest ijson2_convert_unittest
	rm -f ijson2_direct_formatter_unittest
	rm -f parser_performance_test
	rm -f test_pretty_formatting
	rm -f direct_formatter_performance_test
	rm -f formatter_performance_test
	rm -f test_pretty_direct_formatting

OBJS = \
	ijson2_string_view.o \
	ijson2_memory_arena.o \
	ijson2_parser.o \
	ijson2_formatter.o \
	ijson2_direct_formatter.o \


libijson2.a: $(OBJS) double-conversion/libdouble-conversion.a
	ar rcs $@ $(OBJS)
	@rm -rf dtmp
	mkdir dtmp && (cd dtmp; ar x ../double-conversion/libdouble-conversion.a; ar r ../$@ *.o)
	@rm -rf dtmp

double-conversion/libdouble-conversion.a:
	(cd double-conversion && cmake . && make)


#Test driver program for JSONTestSuite
test_ijson2: test_ijson2.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test_ijson2.o libijson2.a

#test program for measuring parser performance
parser_performance_test: parser_performance_test.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ parser_performance_test.o libijson2.a

#test program for measuring formatter performance
formatter_performance_test: formatter_performance_test.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ formatter_performance_test.o libijson2.a

direct_formatter_performance_test: direct_formatter_performance_test.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ direct_formatter_performance_test.o libijson2.a


test_pretty_formatting: test_pretty_formatting.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test_pretty_formatting.o libijson2.a

test_pretty_direct_formatting: test_pretty_direct_formatting.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ test_pretty_direct_formatting.o libijson2.a

UNITTESTS += ijson2_unittest
ijson2_unittest: ijson2_unittest.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ ijson2_unittest.o libijson2.a
.PHONY: ijson2_unittest_run
ijson2_unittest_run: ijson2_unittest
	valgrind --error-exitcode=1 ./ijson2_unittest


UNITTESTS += ijson2_parser_unittest
ijson2_parser_unittest: ijson2_parser_unittest.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ ijson2_parser_unittest.o libijson2.a
.PHONY: ijson2_parser_unittest_run
ijson2_parser_unittest_run: ijson2_parser_unittest
	valgrind --error-exitcode=1 ./ijson2_parser_unittest


UNITTESTS += ijson2_formatter_unittest
ijson2_formatter_unittest: ijson2_formatter_unittest.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ ijson2_formatter_unittest.o libijson2.a
.PHONY: ijson2_formatter_unittest_run
ijson2_formatter_unittest_run: ijson2_formatter_unittest
	valgrind --error-exitcode=1 ./ijson2_formatter_unittest


UNITTESTS += ijson2_formatter_unittest
ijson2_direct_formatter_unittest: ijson2_direct_formatter_unittest.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ ijson2_direct_formatter_unittest.o libijson2.a
.PHONY: ijson2_direct_formatter_unittest_run
ijson2_direct_formatter_unittest_run: ijson2_direct_formatter_unittest
	valgrind --error-exitcode=1 ./ijson2_direct_formatter_unittest


UNITTESTS += ijson2_convert_unittest
ijson2_convert_unittest:ijson2_convert_unittest.o libijson2.a
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ ijson2_convert_unittest.o libijson2.a
.PHONY: ijson2_convert_unittest_run
ijson2_convert_unittest_run: ijson2_convert_unittest
	valgrind --error-exitcode=1 ./ijson2_convert_unittest


.PHONY: unittests
unittests: $(UNITTESTS)


UNITTESTS_RUN:= $(UNITTESTS:=_run)
.PHONY: unittests_run
unittests_run: $(UNITTESTS_RUN)


DEPS := $(OBJS:.o=.d)
DEPS += ijson2_unittest.d
DEPS += ijson2_parser_unittest.d
DEPS += ijson2_formatter_unittest.d
DEPS += ijson2_convert_unittest.d
DEPS += parser_performance_test.d
DEPS += test_pretty_formatting.d

-include $(DEPS)
