#ifndef IJSON2_FORMATTER_HH_
#define IJSON2_FORMATTER_HH_
#include "ijson2.hh"

namespace ijson2 {


typedef void (*append_fn_t)(const char *src, size_t srcsize, void *context);

void format(const Value &v, append_fn_t pfn, void *append_context, bool pretty=false);

size_t format(const Value &v, char *dst, size_t dstsize, bool pretty=false);


class formatter_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class insufficient_room : public formatter_error {
public:
	insufficient_room() : formatter_error("Insufficient room in output buffer") {}
};

class invalid_utf8 : public formatter_error {
public:
	invalid_utf8() : formatter_error("Invalid or truncated UTF-8 sequence found") {}
};


} //namespace

#endif
