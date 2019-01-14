#include "ijson2_parser.hh"
#include "ijson2_formatter.hh"
#include <assert.h>
#include <string.h>
#include <stdio.h>

using namespace ijson2;

class TestParser : public Parser {
public:
	void parse(const char *s, unsigned max_nesting_levels=64) {
		return Parser::parse(s,strlen(s),max_nesting_levels);
	}
};


int main(void) {
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	
	printf("Simpel stuff\n");
	{
		TestParser p;
		p.parse("null");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="null");
	}
	{
		TestParser p;
		p.parse("true");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="true");
	}
	{
		TestParser p;
		p.parse("false");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="false");
	}
	{
		TestParser p;
		p.parse("17");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="17");
	}
	{
		TestParser p;
		p.parse("42.5");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="42.5");
	}

	printf("Simple structures\n");
	{
		TestParser p;
		p.parse("[]");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="[]");
	}
	{
		TestParser p;
		p.parse("{}");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="{}");
	}
	
	printf("less simple structures\n");
	{
		TestParser p;
		p.parse("[17]");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="[17]");
	}
	{
		TestParser p;
		p.parse("[17,42,117]");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="[17,42,117]");
	}
	{
		TestParser p;
		p.parse("[17,true,\"abc\"]");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="[17,true,\"abc\"]");
	}
	
	printf("complex simple structures\n");
	{
		TestParser p;
		p.parse("[{}]");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="[{}]");
	}
	{
		TestParser p;
		p.parse("{\"foo\":[17]}");
		s.clear();
		ijson2::format(p.value(),append,&s);
		assert(s=="{\"foo\":[17]}");
	}
	
	return 0;
}
