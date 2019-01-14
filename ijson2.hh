#ifndef IJSON2_HH_
#define IJSON2_HH_
#include "ijson2_string_view.hh"
#include <inttypes.h>
#include <map>
#include <vector>


namespace ijson2 {

enum class value_type_t {
	object,
	array,
	string,
	boolean,
	number_double,
	number_int64,
	null,
};

class Value {
	void clear() {
		switch(value_type) {
			case value_type_t::object:
				u.object_members.~map_type();
				break;
			case value_type_t::array:
				u.array_elements.~array_type();
				break;
			case value_type_t::string:
				u.string_value.~string_view();
				break;
			case value_type_t::boolean:
			case value_type_t::number_double:
			case value_type_t::number_int64:
			case value_type_t::null:
				break;
		}
		value_type = value_type_t::null;
	}
public:
	using map_type = std::map<string_view,Value>;
	using array_type = std::vector<Value>;
	Value() noexcept
	  : value_type(value_type_t::null)
	{}
	Value(const Value &v)
	  : value_type(value_type_t::null)
	{
		*this = v;
	}
	~Value() {
		clear();
	}
	
	Value(std::nullptr_t)
	  : value_type(value_type_t::null)
	{}
	Value(string_view sv)
	  : value_type(value_type_t::string)
	{
		u.string_value = sv;
	}
	explicit Value(bool b)
	  : value_type(value_type_t::boolean)
	{
		u.bool_value = b;
	}
	Value(int64_t i)
	  : value_type(value_type_t::number_int64)
	{
		u.number_int64value = i;
	}
	Value(double d)
	  : value_type(value_type_t::number_double)
	{
		u.number_doublevalue = d;
	}
	Value(const array_type &a) {
		new(&u.array_elements) array_type(a);
		value_type = value_type_t::array;
	}
	Value(const map_type &o) {
		new(&u.object_members) map_type(o);
		value_type = value_type_t::object;
	}
	
	Value& operator=(const Value &v) {
		if(this!=&v) {
			switch(v.value_type) {
				case value_type_t::object:
					return (*this) = v.u.object_members;
				case value_type_t::array:
					return (*this) = v.u.array_elements;
				case value_type_t::string:
					return (*this) = v.u.string_value;
				case value_type_t::boolean:
					return (*this) = v.u.bool_value;
				case value_type_t::number_double:
					return (*this) = v.u.number_doublevalue;
				case value_type_t::number_int64:
					return (*this) = v.u.number_int64value;
				case value_type_t::null:
					value_type = value_type_t::null;
					break;
			}
		}
		return *this;
	}
	
	Value& operator=(std::nullptr_t) {
		clear();
		value_type = value_type_t::null;
		return *this;
	}
	Value& operator=(string_view sv)
	{
		clear();
		u.string_value = sv;
		value_type = value_type_t::string;
		return *this;
	}
	Value& operator=(bool b) {
		clear();
		u.bool_value = b;
		value_type = value_type_t::boolean;
		return *this;
	}
	Value& operator=(int64_t i) {
		clear();
		u.number_int64value = i;
		value_type = value_type_t::number_int64;
		return *this;
	}
	Value& operator=(double d) {
		clear();
		u.number_doublevalue = d;
		value_type = value_type_t::number_double;
		return *this;
	}
	Value& operator=(const array_type &a) {
		clear();
		new(&u.array_elements) array_type(a);
		value_type = value_type_t::array;
		return *this;
	}
	Value& operator=(const map_type &o) {
		clear();
		new(&u.object_members) map_type(o);
		value_type = value_type_t::object;
		return *this;
	}
	
	value_type_t value_type;
	union U {
		U() {}
		~U() {}
		std::map<string_view,Value> object_members;
		std::vector<Value> array_elements;
		string_view string_value;
		bool bool_value;
		double number_doublevalue;
		int64_t number_int64value;
	} u;
};

} //namespace


#endif
