#ifndef IJSON2_DIRECT_FORMATTER_HH_
#define IJSON2_DIRECT_FORMATTER_HH_
#include "ijson2_formatter.hh"

//A JSON formatter to appends directly to an output buffer. If used incorrectly can produce incorrect output.

namespace ijson2 {


class DirectFormatter {
	append_fn_t append_pfn;
	void *append_context;
	bool pretty;
	char intermediate_buffer[16384];
	size_t intermediate_buffer_used = 0;
	int level = 0;
	bool nl_indent_pending = false;
	bool suppress_indent = false;
	void append(const char *s, size_t l);
	void append_indent(int indents);
	void append_string(const string_view &sv, bool raw);
public:
	DirectFormatter(append_fn_t append_pfn_, void *append_context_, bool pretty_=false)
	  : append_pfn(append_pfn_),
	    append_context(append_context_),
	    pretty(pretty_)
	{}
	
	void open_object();
	void open_array();
	void close_array();
	void close_object();
	void append_string(const string_view &sv) { append_string(sv,false); }
	void append_string(const std::string &s) { append_string(string_view(s.data(),s.size())); }
	void append_string(const char *s) { append_string(string_view(s)); }
	void append_number(int64_t i);
	void append_number(int32_t i) { append_number(static_cast<int64_t>(i)); }
	void append_number(uint32_t i) { append_number(static_cast<int64_t>(i)); }
	void append_number(double d);
	void append_boolean(bool b);
	void append_null();
	void begin_object_member(const string_view &sv);
	void begin_object_member(const std::string &s) { begin_object_member(string_view(s.data(),s.size())); }
	void begin_object_member(const char *s) { begin_object_member(string_view(s)); }
	void append_object_member_separator();
	void append_array_member_separator();
	
	void flush();
};

}

#endif
