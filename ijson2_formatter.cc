#include "ijson2_formatter.hh"
#include <string.h>
#include <float.h>


namespace {

struct Context {
	ijson2::append_fn_t append_pfn;
	void *append_context;
	char intermediate_buffer[16384];
	size_t ibuf_used;
	
	Context(ijson2::append_fn_t append_pfn_, void *append_context_)
	  : append_pfn(append_pfn_),
	    append_context(append_context_),
	    ibuf_used(0)
	 {}
	 
	void append(const char *s, size_t l);
	void flush();
};

void Context::append(const char *s, size_t l) {
	if(ibuf_used+l<sizeof(intermediate_buffer)) {
		memcpy(intermediate_buffer+ibuf_used, s, l);
		ibuf_used += l;
	} else {
		if(ibuf_used>0)
			append_pfn(intermediate_buffer,ibuf_used,append_context);
		if(l<sizeof(intermediate_buffer)) {
			memcpy(intermediate_buffer,s,l);
			ibuf_used = l;
		} else {
			append_pfn(s,l,append_context);
		}
	}
}

void Context::flush() {
	if(ibuf_used>0)
		append_pfn(intermediate_buffer,ibuf_used,append_context);
	ibuf_used = 0;
}

} //anonymous namespace


static void format(const ijson2::Value &v, Context &context, int level);


static void append_indent(Context &context, int level) {
	//yes, we use tabs. 
	while(level>0) {
		static const char eight_tabs[8]={'\t','\t','\t','\t','\t','\t','\t','\t'};
		context.append(eight_tabs, level%8);
		level -= 8;
	}
}




static void format_string(const ijson2::string_view sv, Context &context) {
	context.append("\"",1);
	for(char c : sv) {
		switch(c) {
			case '"': //quotation mark
				context.append("\\\"",2);
				break;
			case '\\': //reverse solidus
				context.append("\\\\",2);
				break;
			//case '/': //why would you want to escape that?
			case '\b': //backspace
				context.append("\\b",2);
				break;
			case '\f': //form feed
				context.append("\\f",2);
				break;
			case '\n': //newline
				context.append("\\n",2);
				break;
			case '\r': //carriage return
				context.append("\\r",2);
				break;
			case '\t': //tab
				context.append("\\t",2);
				break;
			default:
				if(static_cast<uint8_t>(c) >= 32) {
					context.append(&c,1);
				} else {
					char buf[6+1];
					buf[0] = '\\';
					buf[1] = 'u';
					sprintf(buf+2,"%04X", static_cast<uint8_t>(c));
					context.append(buf,6);
				}
		}
	}
	context.append("\"",1);
}


static void format_double(double d, Context &context) {
	//todo: use an optimized algorithm, eg Grisu
	char buf[64];
	size_t l = sprintf(buf,"%.18f", d);
	if(memchr(buf,'e',l)==nullptr) {
		while(l>=2 && buf[l-1]=='0' && buf[l-2]!='.')
			l--;
	}
	context.append(buf,l);
}


static void format_int64(int64_t i, Context &context) {
#if 0
	//the obvious way
	char buf[64];
	size_t l = sprintf(buf,"%" PRId64, i);
	context.append(buf,l);
#else
	//the optimized way
	if(i==0)
		context.append("0",1);
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
		context.append(buf,digits+(p-buf));
	}
#endif
}

static void format_array(const ijson2::Value::array_type &a, Context &context, int level) {
	if(a.empty()) {
		context.append("[]",2);
	} else {
		context.append("[",1);
		bool first=true;
		for(const auto &e : a) {
			if(!first)
				context.append(",",1);
			if(level>=0) context.append("\n",1);
			append_indent(context,level>=0?level+1:level);
			format(e,context,level>=0?level+1:level);
			first = false;
		}
		if(level>=0) context.append("\n",1);
		append_indent(context,level);
		context.append("]",1);
	}
}

static void format_object(const ijson2::Value::map_type &o, Context &context, int level) {
	if(o.empty()) {
		context.append("{}",2);
	} else {
		context.append("{",1);
		bool first=true;
		for(const auto &e : o) {
			if(!first)
				context.append(",",1);
			if(level>=0) context.append("\n",1);
			append_indent(context,level>=0?level+1:level);
			format_string(e.first,context);
			context.append(":",1);
			format(e.second,context,level>=0?level+1:level);
			first = false;
		}
		if(level>=0) context.append("\n",1);
		append_indent(context,level);
		context.append("}",1);
	}
}


static void format(const ijson2::Value &v, Context &context, int level) {
	switch(v.value_type) {
		case ijson2::value_type_t::object:
			format_object(v.u.object_members,context,level);
			break;
		case ijson2::value_type_t::array:
			format_array(v.u.array_elements,context,level);
			break;
		case ijson2::value_type_t::string:
			format_string(v.u.string_value,context);
			break;
		case ijson2::value_type_t::boolean:
			if(v.u.bool_value)
				context.append("true",4);
			else
				context.append("false",5);
			break;
		case ijson2::value_type_t::number_double:
			format_double(v.u.number_doublevalue,context);
			break;
		case ijson2::value_type_t::number_int64:
			format_int64(v.u.number_int64value,context);
			break;
		case ijson2::value_type_t::null:
			context.append("null",4);
			break;
	}
}


void ijson2::format(const Value &v, ijson2::append_fn_t append_pfn, void *append_context, bool pretty) {
	Context context(append_pfn,append_context);
	::format(v,context, pretty?0:-1);
	if(pretty) context.append("\n",1);
	context.flush();
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
