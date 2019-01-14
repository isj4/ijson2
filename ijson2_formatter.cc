#include "ijson2_formatter.hh"
#include <string.h>
#include <float.h>


using namespace ijson2;


static void format_string(const ijson2::string_view sv, ijson2::append_fn_t append_pfn, void *append_context) {
	append_pfn("\"",1,append_context);
	for(char c : sv) {
		switch(c) {
			case '"': //quotation mark
				append_pfn("\\\"",2,append_context);
				break;
			case '\\': //reverse solidus
				append_pfn("\\\\",2,append_context);
				break;
			//case '/': //why would you want to escape that?
			case '\b': //backspace
				append_pfn("\\b",2,append_context);
				break;
			case '\f': //form feed
				append_pfn("\\f",2,append_context);
				break;
			case '\n': //newline
				append_pfn("\\n",2,append_context);
				break;
			case '\r': //carriage return
				append_pfn("\\r",2,append_context);
				break;
			case '\t': //tab
				append_pfn("\\t",2,append_context);
				break;
			default:
				if(static_cast<uint8_t>(c) >= 32) {
					append_pfn(&c,1,append_context);
				} else {
					char buf[6+1];
					buf[0] = '\\';
					buf[1] = 'u';
					sprintf(buf+2,"%04X", static_cast<uint8_t>(c));
					append_pfn(buf,6,append_context);
				}
		}
	}
	append_pfn("\"",1,append_context);
}


static void format_double(double d, ijson2::append_fn_t append_pfn, void *append_context) {
	char buf[64];
	size_t l = sprintf(buf,"%.18f", d);
	if(memchr(buf,'e',l)==nullptr) {
		while(l>=2 && buf[l-1]=='0' && buf[l-2]!='.')
			l--;
	}
	append_pfn(buf,l,append_context);
}


static void format_int64(int64_t i, ijson2::append_fn_t append_pfn, void *append_context) {
	char buf[64];
	size_t l = sprintf(buf,"%" PRId64, i);
	append_pfn(buf,l,append_context);
}

static void format_array(const Value::array_type &a, ijson2::append_fn_t append_pfn, void *append_context, bool pretty) {
	append_pfn("[",1,append_context);
	bool first=true;
	for(const auto &e : a) {
		if(!first)
			append_pfn(",",1,append_context);
		format(e,append_pfn,append_context,pretty);
		first = false;
	}
	append_pfn("]",1,append_context);
}

static void format_object(const Value::map_type &o, ijson2::append_fn_t append_pfn, void *append_context, bool pretty) {
	append_pfn("{",1,append_context);
	bool first=true;
	for(const auto &e : o) {
		if(!first)
			append_pfn(",",1,append_context);
		format_string(e.first,append_pfn,append_context);
		append_pfn(":",1,append_context);
		format(e.second,append_pfn,append_context,pretty);
			first = false;
	}
	append_pfn("}",1,append_context);
}


void ijson2::format(const Value &v, ijson2::append_fn_t append_pfn, void *append_context, bool pretty) {
	switch(v.value_type) {
		case value_type_t::object:
			format_object(v.u.object_members,append_pfn,append_context,pretty);
			break;
		case value_type_t::array:
			format_array(v.u.array_elements,append_pfn,append_context,pretty);
			break;
		case value_type_t::string:
			format_string(v.u.string_value,append_pfn,append_context);
			break;
		case value_type_t::boolean:
			if(v.u.bool_value)
				append_pfn("true",4,append_context);
			else
				append_pfn("false",5,append_context);
			break;
		case value_type_t::number_double:
			format_double(v.u.number_doublevalue,append_pfn,append_context);
			break;
		case value_type_t::number_int64:
			format_int64(v.u.number_int64value,append_pfn,append_context);
			break;
		case value_type_t::null:
			append_pfn("null",4,append_context);
			break;
	}
}


size_t ijson2::format(const Value &v, char *dst, size_t dstsize, bool pretty) {
	struct context_t {
		char *dst;
		size_t dstsize;
		size_t bytes;
	} context{dst,dstsize,0};
	append_fn_t append = [](const char *src, size_t srcsize, void *append_context_) {
		context_t *append_context = reinterpret_cast<context_t*>(append_context_);
		if(append_context->bytes+srcsize > append_context->dstsize)
			throw insufficient_room();
		memcpy(append_context->dst+append_context->bytes, src, srcsize);
		append_context->bytes += srcsize;
	};
	format(v,append,&context,pretty);
	return context.bytes;
}