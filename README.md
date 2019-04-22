# C++11 JSON library
This is a C++ library for handling JSON. It uses C++11 constructs. It has a liberal license.
It is reasonably efficient. The library provides a "Basic exception safety" meaning no memory leaks.

It is RFC 8259 compliant as far as I know. It passes all the mandatory tests on JSONTestSuite (https://github.com/nst/JSONTestSuite).

# Value representation
The library uses the most straight-forward representation of values as long as they are efficient.

All values are in a `Value` container which contains a union `u`. Which part of the union is valid is determined by Value::value_type.

## Numbers
JSON numbers are stored either as an `int64_t` in Value::u::number_int64value or as a `double` in Value::u::number_doublevalue
## Strings
Strings are stored as `isjon2::string_view` in Value::u::string_value which some day (years..) will be replaced with C++17 `std::string_view`.
Note such values retain a direct reference to the value so if the string you want to use is a temporary or 'rvalue' then you have to allocate it somewhere. You can use ijson2::memory_arena for that if you like.
Don't do this:
```
    std::string s;
    ...
    ijson2::Value v;
    v = (s+"foo").c_str();
    return v;
```
The advantage of retaining a direct reference outweighs the disadvantage above. It means that the library doesn't have to use precious heap memory for constant strings.
## Booleans (true/false)
Stored as `bool` in Value::u::bool_value.
## Null
Stored as ... well a value signifying null.
## Arrays
Stored as `std::vector<Value>` in Value::u::array_elements.
## Objects
Stored as `std::map<string_view,Value>` in Value::u::object_members.



# Parser
It parses JSON input in ASCII or UTF-8. It ignores any UTF-8 BOM. It decodes any escaped characters including \n, \uXXX etc. it passes other characters straight through, so eg. half a unicode surrogate pair will slip through. It accepts non-objects at the top level (eg. a string or array).
It parses all or nothing.

Example use:
```
    ijson2::Parser parser;
    try {
        parser.parse(some_data, some_data_length_in_bytes);
        //top-level element is in parser.value()
	    if(parser.value().value_type!=ijson2::value_type_t::object)
	        throw ...
        if(parser.value().u.object_memebrs.at("foo") ...
	//use result
    } catch(const ijson2::parser_error &ex) {
        cerr << "Could not parse JSON:" << ex.what() << " at " << ex.where() << "\n";
    } catch(const std::bad_alloc &) {
        cerr << "Buy more memory\n";
    }
```
Note: The parser retains references into the given data in the string_view items.

If the input could not be parsed the parser will throw an exception derived from `ijson2::parser_error`.
`std::bad_alloc` from `std::map` or `std::vector` are passed straigh up to the caller.

# Formatter / output
It produces either compact JSON, or mostly-readable JSON with indentation and newlines.
It only escapes the characters it must escape (< u+0020). It does not check for invalid UTF-8 in strings. It does not check for NaN or infinity in doubles.


Example use:
```
    //We need a callback for storing the output.
    std::string s;
    auto append = [](const char *src, size_t srcsize, void *append_context) {
        *reinterpret_cast<std::string*>(append_context) += std::string(src,srcsize);
    };
    
    ijson2::Value v;
    //fill in value
    ...
    
    ijson2::format(v, append,&s);
```

# Direct formatter
If you need to produce a large JSON output and heap memory is a concern then there is an alternative formatter 'DirectFormatter' which instead of nice ijson2::Value items take raw values and strings and leaves much of the reposibility of producing a correct JSON output to the programmer.

# Compiling and linking
Just use `make` or `make config=release` and you will get libijson2.a
The library has been tested with gcc-7, gcc-8 and clang-5.


# Reason for making this library
None of the C++ libraries I could find at the time had a liberal license or any visible exception guarantee.
