#include "ijson2.hh"
#include <assert.h>
#include <stdio.h>

using namespace ijson2;

int main(void) {
	printf("Normal constructors and type-checking value query functions\n");
	{
		Value v;
		assert(v.value_type==value_type_t::null);
		assert(v.is_null());
	}
	{
		Value v(nullptr);
		assert(v.value_type==value_type_t::null);
		assert(v.is_null());
	}
	{
		Value v("abc");
		assert(v.value_type==value_type_t::string);
		assert(v.string() == "abc");
	}
	{
		Value v(false);
		assert(v.value_type==value_type_t::boolean);
		assert(v.boolean() == false);
	}
	{
		Value v(true);
		assert(v.value_type==value_type_t::boolean);
		assert(v.boolean() == true);
	}
	{
		Value v(17);
		assert(v.value_type==value_type_t::number_int64);
		assert(v.int64value() == 17);
	}
	{
		Value v(42.5);
		assert(v.value_type==value_type_t::number_double);
		assert(v.doublevalue() == 42.5);
	}
	{
		Value v(Value::map_type{});
		assert(v.value_type==value_type_t::object);
		assert(v.object().empty());
	}
	{
		Value v(Value::map_type{{"abc",17},{"xyz",42}});;
		assert(v.value_type==value_type_t::object);
		assert(v.object().size()==2);
		assert(v.object().at("abc").int64value()==17);
		assert(v.object().at("xyz").int64value()==42);
	}
	{
		Value v(Value::array_type{});
		assert(v.value_type==value_type_t::array);
		assert(v.array().empty());
	}
	{
		Value v(Value::array_type{17,42});
		assert(v.value_type==value_type_t::array);
		assert(v.array().size()==2);
		assert(v.array()[0].int64value()==17);
		assert(v.array()[1].int64value()==42);
	}
	
	{
		Value v(42.5);
		try {
			v.boolean();
			assert(false);
		} catch(const unexpected_value_type&) {
		}
	}
	
	printf("Copy-constructors\n");
	{
		Value v0(true);
		Value v1(v0);
		assert(v1.value_type==value_type_t::boolean);
		assert(v1.boolean() == true);
	}
	{
		Value v0("abc");
		Value v1(v0);
		assert(v1.value_type==value_type_t::string);
		assert(v1.string() == "abc");
	}
	{
		Value v0(42);
		Value v1(v0);
		assert(v1.value_type==value_type_t::number_int64);
		assert(v1.int64value() == 42);
	}
	{
		Value v0(Value::array_type{12,34});
		Value v1(v0);
		assert(v1.array().size()==2);
		assert(v1.array()[0].int64value()==12);
		assert(v1.array()[1].int64value()==34);
	}
	{
		Value v0(Value::map_type{{"abc",17},{"xyz",42}});;
		Value v1(v0);
		assert(v1.object().size()==2);
		assert(v1.object().at("abc").int64value()==17);
		assert(v1.object().at("xyz").int64value()==42);
	}
	
	//not done: booring assignment-operators
	
	return 0;
}
