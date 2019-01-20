#ifndef IJSON2_PARSER_HH_
#define IJSON2_PARSER_HH_
#include "ijson2.hh"
#include "ijson2_memory_arena.hh"
#include <stdexcept>

namespace ijson2 {

class Parser {
	MemoryArena memory_arena;
	Value top_value;
public:
	Parser()
	{}
	~Parser() {
	}
	Parser(const Parser&) = delete;
	Parser operator=(const Parser&) = delete;
	
	//Is the data block possibly a complete value/object?
	static bool may_be_complete(const char *s, size_t sz);
	
	void parse(const char *s, size_t sz, unsigned max_nesting_levels=64);
	
	const Value &value() const { return top_value; }
private:
	const char *parse_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels);
	const char *parse_object_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels);
	const char *parse_array_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels);
	const char *parse_number_value(const char *s, const char *end, Value *value);
	const char *parse_string_value(const char *s, const char *end, Value *value);
	const char *parse_true_value(const char *s, const char *end, Value *value);
	const char *parse_false_value(const char *s, const char *end, Value *value);
	const char *parse_null_value(const char *s, const char *end, Value *value);
	const char *parse_string(const char *s, const char *end, string_view *sv);
};


class parser_error : public std::runtime_error {
	const char *where_; //points into the given data
public:
	parser_error(const char *what_arg, const char *where_arg)
	  : std::runtime_error(what_arg),
	    where_(where_arg)
	  {}
	const char *where() const { return where_; }
};

class unterminated_string : public parser_error {
public:
	unterminated_string(const char *where_arg)
	  : parser_error("unterminated string",where_arg)
	  {}
};

class unterminated_object : public parser_error {
public:
	unterminated_object(const char *where_arg)
	  : parser_error("unterminated object",where_arg)
	  {}
};

class unterminated_array : public parser_error {
public:
	unterminated_array(const char *where_arg)
	  : parser_error("unterminated array",where_arg)
	  {}
};

class junk : public parser_error {
public:
	junk(const char *where_arg)
	  : parser_error("junk",where_arg)
	  {}
};

class unparseable_number : public parser_error {
public:
	unparseable_number(const char *where_arg)
	  : parser_error("unparseable number",where_arg)
	  {}
};

class expected_string : public parser_error {
public:
	expected_string(const char *where_arg)
	  : parser_error("expected string",where_arg)
	  {}
};

class expected_colon : public parser_error {
public:
	expected_colon(const char *where_arg)
	  : parser_error("expected colon",where_arg)
	  {}
};

class expected_comma : public parser_error {
public:
	expected_comma(const char *where_arg)
	  : parser_error("expected comma",where_arg)
	  {}
};

class expected_value : public parser_error {
public:
	expected_value(const char *where_arg)
	  : parser_error("expected value",where_arg)
	  {}
};

class too_many_levels : public parser_error {
public:
	too_many_levels(const char *where_arg)
	  : parser_error("too many levels",where_arg)
	  {}
};

class invalid_escape : public parser_error {
public:
	invalid_escape(const char *where_arg)
	  : parser_error("invalid escape",where_arg)
	  {}
};

class missing_escape : public parser_error {
public:
	missing_escape(const char *where_arg)
	  : parser_error("missing escape",where_arg)
	  {}
};


} //namespace

#endif
