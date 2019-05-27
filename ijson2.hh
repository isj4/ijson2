#ifndef IJSON2_HH_
#define IJSON2_HH_
#include "ijson2_string_view.hh"
#include <inttypes.h>
#include <map>
#include <vector>
#include <stdexcept>
#include <string>


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
static const char *value_type_name[] = {
	"object",
	"array",
	"string",
	"boolean",
	"number_double",
	"number_int64",
	"null"
};

class unexpected_value_type : public std::runtime_error {
public:
	unexpected_value_type(value_type_t expected, value_type_t actual)
	  : std::runtime_error(std::string("Unexpected value type '")+value_type_name[static_cast<int>(actual)]+"', expected '"+value_type_name[static_cast<int>(expected)]+"'")
	{}
};

class Value {
	void clear() noexcept {
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
	Value(Value &&v)
	  : value_type(value_type_t::null)
	{
		*this = std::move(v);
	}
	~Value() noexcept {
		clear();
	}
	
	Value(std::nullptr_t) noexcept
	  : value_type(value_type_t::null)
	{}
	Value(string_view sv) noexcept
	  : value_type(value_type_t::string)
	{
		u.string_value = sv;
	}
	explicit Value(bool b) noexcept
	  : value_type(value_type_t::boolean)
	{
		u.bool_value = b;
	}
	Value(int64_t i) noexcept
	  : value_type(value_type_t::number_int64)
	{
		u.number_int64value = i;
	}
	Value(double d) noexcept
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
	
	//delegating constructors ad nauseam
	Value(int8_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(uint8_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(int16_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(uint16_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(int32_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(uint32_t i) noexcept : Value(static_cast<int64_t>(i)) {}
	Value(float d) noexcept : Value(static_cast<double>(d)) {}
	Value(const char *s) noexcept : Value(string_view(s)) {}
	
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
	Value& operator=(Value &&v) {
		if(this!=&v) {
			switch(v.value_type) {
				case value_type_t::object:
					clear();
					new (&u.object_members) map_type(std::move(v.u.object_members));
					value_type = value_type_t::object;
					return *this;
				case value_type_t::array:
					clear();
					new (&u.array_elements) array_type(std::move(v.u.array_elements));
					value_type = value_type_t::array;
					return *this;
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
	
	Value& operator=(std::nullptr_t) noexcept {
		clear();
		value_type = value_type_t::null;
		return *this;
	}
	Value& operator=(string_view sv) noexcept {
		clear();
		u.string_value = sv;
		value_type = value_type_t::string;
		return *this;
	}
	Value& operator=(bool b) noexcept {
		clear();
		u.bool_value = b;
		value_type = value_type_t::boolean;
		return *this;
	}
	Value& operator=(int64_t i) noexcept {
		clear();
		u.number_int64value = i;
		value_type = value_type_t::number_int64;
		return *this;
	}
	Value& operator=(int8_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(uint8_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(int16_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(uint16_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(int32_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(uint32_t i) noexcept { return *this = static_cast<int64_t>(i); }
	Value& operator=(double d) noexcept {
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
	Value& operator=(const char *s) noexcept {
		return *this = string_view(s);
	}
	
	value_type_t value_type;
	union U {
		U() noexcept {}
		~U() noexcept {}
		std::map<string_view,Value> object_members;
		std::vector<Value> array_elements;
		string_view string_value;
		bool bool_value;
		double number_doublevalue;
		int64_t number_int64value;
	} u;
	
	const map_type &object() const {
		if(value_type!=value_type_t::object)
			throw unexpected_value_type(value_type_t::object, value_type);
		return u.object_members;
	}
	map_type &object() {
		if(value_type!=value_type_t::object)
			throw unexpected_value_type(value_type_t::object, value_type);
		return u.object_members;
	}
	const array_type &array() const {
		if(value_type!=value_type_t::array)
			throw unexpected_value_type(value_type_t::array, value_type);
		return u.array_elements;
	}
	array_type &array() {
		if(value_type!=value_type_t::array)
			throw unexpected_value_type(value_type_t::array, value_type);
		return u.array_elements;
	}
	string_view string() const {
		if(value_type!=value_type_t::string)
			throw unexpected_value_type(value_type_t::string, value_type);
		return u.string_value;
	}
	bool boolean() const {
		if(value_type!=value_type_t::boolean)
			throw unexpected_value_type(value_type_t::boolean, value_type);
		return u.bool_value;
	}
	double doublevalue() const {
		if(value_type!=value_type_t::number_double)
			throw unexpected_value_type(value_type_t::number_double, value_type);
		return u.number_doublevalue;
	}
	int64_t int64value() const {
		if(value_type!=value_type_t::number_int64)
			throw unexpected_value_type(value_type_t::number_int64, value_type);
		return u.number_int64value;
	}
	bool is_null() const { return value_type==value_type_t::null; }
};

} //namespace


#endif
