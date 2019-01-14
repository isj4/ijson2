#include "ijson2_parser.hh"
#include "ijson2_formatter.hh"
#include <stdio.h>
#include <string.h>


class TestParser : public ijson2::Parser {
public:
	void parse(const char *s, unsigned max_nesting_levels=64) {
		return Parser::parse(s,strlen(s),max_nesting_levels);
	}
};

int main(void) {
	TestParser parser;
	parser.parse("{\"foo\":17,\"boo\":[null,42,117,{},1234],\"goo\":{\"zoo\":9876,\"xoo\":[]}}");
	
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	ijson2::format(parser.value(),append,&s,true);
	printf("%s",s.c_str());
	return 0;
}
