#include "ijson2_parser.hh"
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

	
int main() {
	printf("Checking may_be_complete()\n");
	{
		TestParser p;
		assert(p.may_be_complete("{}",2));
		assert(p.may_be_complete("[]",2));
		assert(p.may_be_complete("false",5));
		assert(p.may_be_complete("true",4));
		assert(p.may_be_complete("null",4));
		assert(!p.may_be_complete("{{",2));
		assert(!p.may_be_complete("[[",2));
		assert(!p.may_be_complete("[}",2));
		assert(!p.may_be_complete("{]",2));
		assert(p.may_be_complete("123",3));
	}
	
	printf("Parsing numbers(int64)\n");
	{
		TestParser p;
		p.parse("1");
		assert(p.value().value_type==value_type_t::number_int64);
		assert(p.value().u.number_int64value==1);
	}
	{
		TestParser p;
		p.parse("17");
		assert(p.value().value_type==value_type_t::number_int64);
		assert(p.value().u.number_int64value==17);
	}
	{
		TestParser p;
		p.parse("-123456789");
		assert(p.value().value_type==value_type_t::number_int64);
		assert(p.value().u.number_int64value==-123456789);
	}
	{
		TestParser p;
		try {
			p.parse("-123456-789");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("-123456789Q");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("-123123123123123123123123123123");
			assert(false);
		} catch(const unparseable_number&) {
		}
	}
	
	printf("Parsing number(double)\n");
	{
		TestParser p;
		p.parse("1.5");
		assert(p.value().value_type==value_type_t::number_double);
		assert(p.value().u.number_doublevalue==1.5);
	}
	{
		TestParser p;
		p.parse("-1.5");
		assert(p.value().value_type==value_type_t::number_double);
		assert(p.value().u.number_doublevalue==-1.5);
	}
	{
		TestParser p;
		p.parse("17.45e1");
		assert(p.value().value_type==value_type_t::number_double);
		assert(p.value().u.number_doublevalue==174.5);
	}
	{
		TestParser p;
		try {
			p.parse("-123.456.789");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("-1234e56789eQ");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("1234e56789e1000000000");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("-1234e56789e1000000000");
			assert(false);
		} catch(const unparseable_number&) {
		} catch(const junk&) {
		}
	}
	
	printf("Parsing strings\n");
	{
		TestParser p;
		p.parse("\"\"");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==0);
	}
	{
		TestParser p;
		p.parse("\"abc\"");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==3);
		assert(memcmp(p.value().u.string_value.data(),"abc",3)==0);
	}
	{
		TestParser p;
		p.parse("\"ab\\\"c\"");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==4);
		assert(memcmp(p.value().u.string_value.data(),"ab\"c",4)==0);
	}
	{
		TestParser p;
		p.parse("\"abc\"  ");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==3);
		assert(memcmp(p.value().u.string_value.data(),"abc",3)==0);
	}
	{
		TestParser p;
		p.parse("  \"abc\"");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==3);
		assert(memcmp(p.value().u.string_value.data(),"abc",3)==0);
	}
	{
		TestParser p;
		p.parse("\"abc\"\r\n");
		assert(p.value().value_type==value_type_t::string);
		assert(p.value().u.string_value.size()==3);
		assert(memcmp(p.value().u.string_value.data(),"abc",3)==0);
	}
	{
		TestParser p;
		try {
			p.parse("\"abc");
			assert(false);
		} catch(const unterminated_string&) {
		}
	}
	{
		TestParser p;
		static const char weird[] = { (char)0x22, (char)0xf4, (char)0x8f, (char)0xbf, (char)0xbf, (char)0x22, '\0' };
		p.parse(weird);
	}
	
	printf("Parsing booleans\n");
	{
		TestParser p;
		p.parse("true");
		assert(p.value().value_type==value_type_t::boolean);
		assert(p.value().u.bool_value==true);
	}
	{
		TestParser p;
		p.parse("false");
		assert(p.value().value_type==value_type_t::boolean);
		assert(p.value().u.bool_value==false);
	}

	printf("Parsing null\n");
	{
		TestParser p;
		p.parse("null");
		assert(p.value().value_type==value_type_t::null);
	}

	printf("Parsing arrays (simple)\n");
	{
		TestParser p;
		p.parse("[]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.empty());
	}
	{
		TestParser p;
		p.parse("[17]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.size()==1);
		assert(p.value().u.array_elements[0].value_type==value_type_t::number_int64);
		assert(p.value().u.array_elements[0].u.number_int64value==17);
	}
	{
		TestParser p;
		p.parse("[\"abc\"]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.size()==1);
		assert(p.value().u.array_elements[0].value_type==value_type_t::string);
		assert(p.value().u.array_elements[0].u.string_value=="abc");
	}
	{
		TestParser p;
		p.parse("[\"a\"]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.size()==1);
		assert(p.value().u.array_elements[0].value_type==value_type_t::string);
		assert(p.value().u.array_elements[0].u.string_value=="a");
	}
	{
		TestParser p;
		p.parse("[17, 18,19]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.size()==3);
		assert(p.value().u.array_elements[0].value_type==value_type_t::number_int64);
		assert(p.value().u.array_elements[0].u.number_int64value==17);
		assert(p.value().u.array_elements[1].value_type==value_type_t::number_int64);
		assert(p.value().u.array_elements[1].u.number_int64value==18);
		assert(p.value().u.array_elements[2].value_type==value_type_t::number_int64);
		assert(p.value().u.array_elements[2].u.number_int64value==19);
	}
	{
		TestParser p;
		try {
			p.parse("[1,,]");
			assert(false);
		} catch(const parser_error&) {
		}
	}
	{
		TestParser p;
		try {
			p.parse("[1,]");
			assert(false);
		} catch(const parser_error&) {
		}
	}
	
	printf("Parsing objects (simple)\n");
	{
		TestParser p;
		p.parse("{}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.empty());
	}
	{
		TestParser p;
		p.parse("{\"foo\":17}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==1);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("foo").u.number_int64value==17);
	}
	{
		TestParser p;
		p.parse("  {  \"foo\"  :  17  }  ");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==1);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("foo").u.number_int64value==17);
	}
	{
		TestParser p;
		p.parse("{\"foo\":17,\"boo\":42}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==2);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("foo").u.number_int64value==17);
		assert(p.value().u.object_members.at("boo").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("boo").u.number_int64value==42);
	}
	{
		TestParser p;
		p.parse("{\"foo\":\"boo\"}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==1);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::string);
		assert(p.value().u.object_members.at("foo").u.string_value=="boo");
	}
	{
		TestParser p;
		p.parse("{\"foo\":17,\"boo\":\"goo\"}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==2);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("foo").u.number_int64value==17);
		assert(p.value().u.object_members.at("boo").value_type==value_type_t::string);
		assert(p.value().u.object_members.at("boo").u.string_value=="goo");
	}
	{
		TestParser p;
		try {
			p.parse("{\"foo\":17,}");
			assert(false);
		} catch(const parser_error&) {
		}
	}
	
	printf("Parsing large integers\n");
	{
		TestParser p;
		p.parse("{\"start\": 1565115093136, \"end\": 1565115113136, \"queries\": [{\"aggregator\": \"avg\", \"metric\": \"temperature\"}]}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==3);
		assert(p.value().u.object_members.at("start").value_type==value_type_t::number_int64);
		assert(p.value().u.object_members.at("end").value_type==value_type_t::number_int64);
	}
	
	printf("Parsing arrays (complex)\n");
	{
		TestParser p;
		p.parse("[{\"foo\":[17]},{\"boo\":42},117,false]");
		assert(p.value().value_type==value_type_t::array);
		assert(p.value().u.array_elements.size()==4);
	}
	
	printf("Parsing objects (complex)\n");
	{
		TestParser p;
		p.parse("{\"foo\":[17,42],\"boo\":{\"goo\":117}}");
		assert(p.value().value_type==value_type_t::object);
		assert(p.value().u.object_members.size()==2);
		assert(p.value().u.object_members.at("foo").value_type==value_type_t::array);
		assert(p.value().u.object_members.at("boo").value_type==value_type_t::object);
	}
	
	printf("Parsing too-deep object\n");
	{
		TestParser p0;
		p0.parse("{\"\":[{\"\":[{\"\":[{\"\":[{}]}]}]}]}");
		TestParser p1;
		p1.parse("{\"\":[{\"\":[{\"\":[{\"\":[{}]}]}]}]}",10);
		try {
			TestParser p2;
			p2.parse("{\"\":[{\"\":[{\"\":[{\"\":[{}]}]}]}]}",7);
			assert(false);
		} catch(const too_many_levels&) {
		}
	}
	
	printf("Parsing BOM\n");
	{
		TestParser p;
		static const char bom_and_one[] = { (char)0xEF, (char)0xBB, (char)0xBF, '1', '\0'};
		p.parse(bom_and_one);
		assert(p.value().value_type==value_type_t::number_int64);
		assert(p.value().u.number_int64value==1);
	}
	
	return 0;
}
