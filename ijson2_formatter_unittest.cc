#include "ijson2_formatter.hh"
#include <stdio.h>
#include <assert.h>
#include <string>


int main() {
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	
	printf("formatting null\n");
	{
		ijson2::Value v{nullptr};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="null");
	}
	
	printf("formatting true/false\n");
	{
		ijson2::Value v{true};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="true");
	}
	{
		ijson2::Value v{false};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="false");
	}
	
	printf("formatting int64\n");
	{
		ijson2::Value v{0};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="0");
	}
	{
		ijson2::Value v{42};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="42");
	}
	{
		ijson2::Value v{-42};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="-42");
	}
	{
		ijson2::Value v{123456};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="123456");
	}
	{
		ijson2::Value v{-123456};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="-123456");
	}
	
	printf("formatting double\n");
	{
		ijson2::Value v{0.0};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="0.0" || s=="0");
	}
	{
		ijson2::Value v{42.5};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="42.5");
	}
	{
		ijson2::Value v{-42.5};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="-42.5");
	}
	
	printf("formatting strings\n");
	{
		ijson2::Value v{"foo"};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="\"foo\"");
	}
	{
		ijson2::Value v{"fo\no"};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="\"fo\\no\"");
	}
	
	printf("formatting arrays\n");
	{
		ijson2::Value v{ijson2::Value::array_type{}};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="[]");
	}
	{
		ijson2::Value v{ijson2::Value::array_type{}};
		v.u.array_elements.push_back("abc");
		v.u.array_elements.push_back(17);
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="[\"abc\",17]");
	}
	
	printf("formatting objects\n");
	{
		ijson2::Value v{ijson2::Value::map_type{}};
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="{}");
	}
	{
		ijson2::Value v{ijson2::Value::map_type{}};
		v.u.object_members["foo"] = "abc";
		v.u.object_members["boo"] = 17;
		s.clear();
		ijson2::format(v, append,&s);
		assert(s=="{\"foo\":\"abc\",\"boo\":17}" || s=="{\"boo\":17,\"foo\":\"abc\"}");
	}
	return 0;
}
