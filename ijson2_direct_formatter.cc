#include "ijson2_direct_formatter.hh"
#include "double-conversion/double-conversion/double-conversion.h"
#include <string.h>
#include <math.h>


void ijson2::DirectFormatter::append(const char *s, size_t l) {
	if(intermediate_buffer_used+l<sizeof(intermediate_buffer)) {
		memcpy(intermediate_buffer+intermediate_buffer_used, s, l);
		intermediate_buffer_used += l;
	} else {
		if(intermediate_buffer_used>0)
			append_pfn(intermediate_buffer,intermediate_buffer_used,append_context);
		if(l<sizeof(intermediate_buffer)) {
			memcpy(intermediate_buffer,s,l);
			intermediate_buffer_used = l;
		} else {
			append_pfn(s,l,append_context);
		}
	}
}


void ijson2::DirectFormatter::append_indent(int indents) {
	//yes, we use tabs. 
	while(indents>0) {
		static const char eight_tabs[8]={'\t','\t','\t','\t','\t','\t','\t','\t'};
		append(eight_tabs, indents%8);
		indents -= 8;
	}
}



void ijson2::DirectFormatter::open_object() {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		if(!suppress_indent) append_indent(level);
		level++;
		append("{",1);
		nl_indent_pending = true;
		suppress_indent = false;
	} else
		append("{",1);
}


void ijson2::DirectFormatter::open_array() {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		if(!suppress_indent) append_indent(level);
		level++;
		append("[",1);
		nl_indent_pending = true;
		suppress_indent = false;
	} else
		append("[",1);
}


void ijson2::DirectFormatter::close_array() {
	if(pretty) {
		if(!nl_indent_pending) append("\n",1);
		level--;
		if(!nl_indent_pending) append_indent(level);
		append("]",1);
		suppress_indent = false;
		nl_indent_pending = false;
	} else
		append("]",1);
}


void ijson2::DirectFormatter::close_object() {
	if(pretty) {
		if(!nl_indent_pending) append("\n",1);
		level--;
		if(!nl_indent_pending) append_indent(level);
		append("}",1);
		suppress_indent = false;
		nl_indent_pending = false;
	} else
		append("}",1);
}


void ijson2::DirectFormatter::append_string(const string_view &sv, bool raw) {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		if(!raw && !suppress_indent) append_indent(level);
	}
	append("\"",1);
	for(char c : sv) {
		switch(c) {
			case '"': //quotation mark
				append("\\\"",2);
				break;
			case '\\': //reverse solidus
				append("\\\\",2);
				break;
			//case '/': //why would you want to escape that?
			case '\b': //backspace
				append("\\b",2);
				break;
			case '\f': //form feed
				append("\\f",2);
				break;
			case '\n': //newline
				append("\\n",2);
				break;
			case '\r': //carriage return
				append("\\r",2);
				break;
			case '\t': //tab
				append("\\t",2);
				break;
			default:
				if(static_cast<uint8_t>(c) >= 32) {
					append(&c,1);
				} else {
					char buf[6+1];
					buf[0] = '\\';
					buf[1] = 'u';
					sprintf(buf+2,"%04X", static_cast<uint8_t>(c));
					append(buf,6);
				}
		}
	}
	append("\"",1);
}


void ijson2::DirectFormatter::append_number(int64_t i) {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		if(!suppress_indent) append_indent(level);
	}
#if 0
	//the obvious way
	char buf[64];
	size_t l = sprintf(buf,"%" PRId64, i);
	append(buf,l);
#else
	//the optimized way
	if(i==0)
		append("0",1);
	else {
		char buf[24];
		char *p=buf;
		char *p2 = buf+sizeof(buf);
		uint64_t u;
		if(i<0) {
			*p++ = '-';
			u = -i;
		} else
			u = i;
		while(u!=0) {
			auto q = u/10;
			auto r = u%10;
			*--p2 = '0'+r;
			u = q;
		}
		auto digits = buf+sizeof(buf)-p2;
		memmove(p,p2,digits);
		append(buf,digits+(p-buf));
	}
#endif
}


void ijson2::DirectFormatter::append_number(double d) {
	//in many scenarios doubles are actually whole numbers
	int64_t i = static_cast<int64_t>(d);
	if(d == i) {
		append_number(i);
		return;
	}
	
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		if(!suppress_indent) append_indent(level);
	}
	switch(fpclassify(d)) {
		case FP_NAN:
		case FP_INFINITE: {
			char buf[64];
			size_t l = sprintf(buf,"%.18f", d);
			if(memchr(buf,'e',l)==nullptr) {
				while(l>=2 && buf[l-1]=='0' && buf[l-2]!='.')
					l--;
			}
			append(buf,l);
			break;
		}
		case FP_ZERO:
		case FP_SUBNORMAL:
			append("0",1);
			break;
		default: {
			using namespace double_conversion;
			DoubleToStringConverter dtsc(DoubleToStringConverter::NO_FLAGS,
			                             "INF",
						     "NAN",
						     'e',
						     -16, 16,
						     16,16);
			char buf[64];
			StringBuilder sb(buf,sizeof(buf));
			dtsc.ToShortest(d,&sb);
			append(buf,sb.position());
			break;
		}
	}
}


void ijson2::DirectFormatter::append_boolean(bool b) {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		if(!suppress_indent) append_indent(level);
	}
	if(b)
		append("true",4);
	else
		append("false",5);
}


void ijson2::DirectFormatter::append_null() {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		if(!suppress_indent) append_indent(level);
	}
	append("null",4);
}



void ijson2::DirectFormatter::begin_object_member(const string_view &sv) {
	if(pretty) {
		if(nl_indent_pending) append("\n",1);
		nl_indent_pending=false;
		append_indent(level);
	}
	append_string(sv,true);
	append(":",1);
	suppress_indent = true;
}


void ijson2::DirectFormatter::append_object_member_separator() {
	if(pretty) {
		append(",\n",2);
		suppress_indent = false;
	} else
		append(",",1);
}


void ijson2::DirectFormatter::append_array_member_separator() {
	if(pretty) {
		append(",\n",2);
		suppress_indent = false;
	} else
		append(",",1);
}


void ijson2::DirectFormatter::flush() {
	if(pretty) append("\n",1);
	if(intermediate_buffer_used>0)
		append_pfn(intermediate_buffer,intermediate_buffer_used,append_context);
	intermediate_buffer_used = 0;
}
