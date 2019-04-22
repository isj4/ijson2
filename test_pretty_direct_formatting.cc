#include "ijson2_direct_formatter.hh"
#include <stdio.h>
#include <string.h>


int main(void) {
	//parser.parse("{\"foo\":17,\"boo\":[null,42,117,{},1234],\"goo\":{\"zoo\":9876,\"xoo\":[]}}");
	
	std::string s;
	auto append = [](const char *src, size_t srcsize, void *append_context) {
		*reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
	};

	ijson2::DirectFormatter df(append,&s,true);
	df.open_object();
	
	df.begin_object_member("foo");
	df.append_number(17);
	
	df.append_object_member_separator();
	
	df.begin_object_member("boo");
	
	df.open_array();
	df.append_null();
	df.append_array_member_separator();
	df.append_number(42);
	df.append_array_member_separator();
	df.append_number(117);
	df.append_array_member_separator();
	df.open_object();
	df.close_object();
	df.append_array_member_separator();
	df.append_number(1234);
	df.close_array();
	
	df.append_object_member_separator();
	
	df.begin_object_member("goo");
	df.open_object();
	df.begin_object_member("zoo");
	df.append_number(9876);
	df.append_object_member_separator();
	df.begin_object_member("xoo");
	df.open_array();
	df.close_array();
	df.close_object();
	
	df.append_object_member_separator();
	df.begin_object_member("doo");
	df.append_boolean(false);
	df.close_object();
	
	df.flush();
	printf("%s",s.c_str());
	return 0;
}
