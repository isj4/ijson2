#include "ijson2_direct_formatter.hh"
#include <stdio.h>
#include <assert.h>
#include <string>

using ijson2::DirectFormatter;

int main() {
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};
	
	printf("formatting null\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_null();
		df.flush();
		assert(s=="null");
	}
	
	printf("formatting true/false\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_boolean(true);
		df.flush();
		assert(s=="true");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_boolean(false);
		df.flush();
		assert(s=="false");
	}
	
	printf("formatting int64\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(0);
		df.flush();
		assert(s=="0");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(42);
		df.flush();
		assert(s=="42");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(-42);
		df.flush();
		assert(s=="-42");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(123456);
		df.flush();
		assert(s=="123456");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(-123456);
		df.flush();
		assert(s=="-123456");
	}
	
	printf("formatting double\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(0.0);
		df.flush();
		assert(s=="0.0" || s=="0");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(42.5);
		df.flush();
		assert(s=="42.5");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(-42.5);
		df.flush();
		assert(s=="-42.5");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_number(0.0042500000001);
		df.flush();
		assert(s=="0.0042500000001");
	}
	
	printf("formatting strings\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_string("foo");
		df.flush();
		assert(s=="\"foo\"");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.append_string("fo\no");
		df.flush();
		assert(s=="\"fo\\no\"");
	}
	
	printf("formatting arrays\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.open_array();
		df.close_array();
		df.flush();
		assert(s=="[]");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.open_array();
		df.append_string("abc");
		df.append_array_member_separator();
		df.append_number(17);
		df.close_array();
		df.flush();
		assert(s=="[\"abc\",17]");
	}
	
	printf("formatting objects\n");
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.open_object();
		df.close_object();
		df.flush();
		assert(s=="{}");
	}
	{
		DirectFormatter df(append,&s);
		s.clear();
		df.open_object();
		df.begin_object_member("foo");
		df.append_string("abc");
		df.append_object_member_separator();
		df.begin_object_member("boo");
		df.append_number(17);
		df.close_object();
		df.flush();
		assert(s=="{\"foo\":\"abc\",\"boo\":17}" || s=="{\"boo\":17,\"foo\":\"abc\"}");
	}
	
	return 0;
}
