#include "ijson2_parser.hh"
#include <string.h>
#include <memory>
#include <errno.h>
#include <math.h>
#include <limits.h>


//By default do strict validation of numbers and strings. Eg. no leading zeroes, no unescaped control characters.
#define STRICT_PARSING 1


using namespace ijson2;


static bool is_ws(char c) {
	return c==' ' || c=='\t' || c=='\n' || c=='\r';
}


static const char *skip_ws(const char *s, const char *end) {
	while(s<end && is_ws(*s))
		s++;
	return s;
}

static const char *skip_ws_reverse(const char *begin, const char *s) {
	while(s>begin && is_ws(s[-1]))
		s--;
	return s;
}

static bool is_value_end(char c) {
	return is_ws(c) || c==',' || c=='}' || c==']';
}


static int hexdigit_value(char c) {
	if(c>='0' && c<='9')
		return c-'0';
	if(c>='A' && c<='F')
		return c-'A';
	if(c>='a' && c<='f')
		return c-'a';
	return -1;
}


//Convert a unicode codepoint i an UTF-8 byte sequence.
static size_t uc_to_utf8(uint32_t uc, char *dst) {
	if((uc&0xffffff80)==0) {
		dst[0] = static_cast<char>(uc);
		return 1;
	}
	if((uc&0xfffff800)==0) {
		dst[0] = static_cast<char>((0xc0 | (uc >>  6 & 0x1f)));
		dst[1] = static_cast<char>((0x80 | (uc       & 0x3f)));
		return 2;
	}
	if((uc&0xffff0000)==0) {
		dst[0] = static_cast<char>((0xe0 | (uc >> 12 & 0x0f)));
		dst[1] = static_cast<char>((0x80 | (uc >>  6 & 0x3f)));
		dst[2] = static_cast<char>((0x80 | (uc       & 0x3f)));
		return 3;
	}
	if((uc&0xe0000000)==0) {
		dst[0] = static_cast<char>((0xf0 | (uc >> 18 & 0x07)));
		dst[1] = static_cast<char>((0x80 | (uc >> 12 & 0x3f)));
		dst[2] = static_cast<char>((0x80 | (uc >>  6 & 0x3f)));
		dst[3] = static_cast<char>((0x80 | (uc       & 0x3f)));
		return 4;
	}
	return 0; 
}


const char *ijson2::Parser::parse_string(const char *s, const char *end, string_view *sv) {
	if(end-s<2)
		throw unterminated_string(s);
	const char *p = s+1;
	bool any_backslashes = false;
	while(p<end) {
		char c = *p;
		if(c=='"')
			break;
#if STRICT_PARSING
		if((uint8_t)c<32 && !any_backslashes)
			throw missing_escape(p);
#endif
		p++;
		if(c=='\\') {
			if(p==end)
				throw unterminated_string(s);
			any_backslashes = true;
			p++;
		}
	}
	if(p>=end)
		throw unterminated_string(s);
	if(!any_backslashes) {
		//no backslashes - use string_view directly into source
		*sv = string_view{s+1, size_t(p-s-1)};
	} else {
		char *dst_start = reinterpret_cast<char*>(memory_arena.alloc(end-s-2,1));
		char *dst = dst_start;
		const char *src = s+1;
		const char *src_end = p;
		while(src<src_end) {
			char c = *src;
			if(c!='\\') {
#if STRICT_PARSING
				if((uint8_t)c<32)
					throw missing_escape(src);
#endif
				*dst++ = c;
				src++;
			} else {
				if(src+1==src_end)
					throw invalid_escape(src);
				switch(src[1]) {
					case '"':
					case '\\':
					case '/':
						*dst++ = src[1];
						src += 2;
						break;
					case 'b':
						*dst++ = '\b';
						src += 2;
						break;
					case 'f':
						*dst++ = '\f';
						src += 2;
						break;
					case 'n':
						*dst++ = '\n';
						src += 2;
						break;
					case 'r':
						*dst++ = '\r';
						src += 2;
						break;
					case 't':
						*dst++ = '\t';
						src += 2;
						break;
					case 'u': {
						if(src+1+1+4 >= end)
							throw invalid_escape(src);
						int v0 = hexdigit_value(src[2]);
						int v1 = hexdigit_value(src[3]);
						int v2 = hexdigit_value(src[4]);
						int v3 = hexdigit_value(src[5]);
						if(v0<0 || v1<0 || v2<0 || v3<0)
							throw invalid_escape(src);
						uint32_t uc = (static_cast<uint32_t>(v0))<<24 |
						              (static_cast<uint32_t>(v1))<<16 |
						              (static_cast<uint32_t>(v2))<< 8 |
						              (static_cast<uint32_t>(v3))     ;
						//todo: handle surrogate pairs
						size_t utf8_len = uc_to_utf8(uc,dst);
						if(utf8_len==0)
							throw invalid_escape(src);
						dst += utf8_len;
						src += 6;
						break;
					}
					default:
						throw invalid_escape(src);
				}
			}
		}
		*sv = string_view{dst_start, size_t(dst-dst_start)};
	}
	return p+1;
}


const char *ijson2::Parser::parse_null_value(const char *s, const char *end, Value *value) {
	if(end-s<4 || memcmp(s,"null",4)!=0)
		throw junk(s);
	if(end-s>4 && !is_ws(s[4]) && s[4]!=',' && s[4]!='}' && s[4]!=']')
		throw junk(s+4);
	
	value->value_type = value_type_t::null;
	return s+4;
}

const char *ijson2::Parser::parse_false_value(const char *s, const char *end, Value *value) {
	if(end-s<5 || memcmp(s,"false",5)!=0)
		throw junk(s);
	if(end-s>5 && !is_ws(s[5]) && s[5]!=',' && s[5]!='}' && s[5]!=']')
		throw junk(s+5);
	
	value->value_type = value_type_t::boolean;
	value->u.bool_value = false;
	return s+5;
}

const char *ijson2::Parser::parse_true_value(const char *s, const char *end, Value *value) {
	if(end-s<4 || memcmp(s,"true",4)!=0)
		throw junk(s);
	if(end-s>4 && !is_value_end(s[4]))
		throw junk(s+4);
	
	value->value_type = value_type_t::boolean;
	value->u.bool_value = true;
	return s+4;
}

const char *ijson2::Parser::parse_string_value(const char *s, const char *end, Value *value) {
	new (&value->u.string_value) string_view;
	value->value_type = value_type_t::string;
	const char *p = parse_string(s,end,&value->u.string_value);
	return p;
}

const char *ijson2::Parser::parse_number_value(const char *s, const char *end, Value *value) {
	const char *p = s;
	unsigned float_chars = 0;
	const char *decimal_point = nullptr;
	while(p<end) {
		char c = *p;
		if(c=='.' || c=='e' || c=='E')
			float_chars++;
		if(c=='.')
			decimal_point = p;
		if(c=='0' ||
		   c=='1' ||
		   c=='2' ||
		   c=='3' ||
		   c=='4' ||
		   c=='5' ||
		   c=='6' ||
		   c=='7' ||
		   c=='8' ||
		   c=='9' ||
		   c=='.' ||
		   c=='+' ||
		   c=='-' ||
		   c=='e' ||
		   c=='E')
			p++;
		else
			break;
	}
	if(p<end && !is_value_end(*p))
		throw junk(s);
	if(p==s)
		throw junk(s); //empty string
	//optimization of small integers 0..9
	if(p==s+1 && s[0]>='0' && s[0]<='9') {
		value->value_type = value_type_t::number_int64;
		value->u.number_int64value = s[0] - '0';
		return p;
	}
#if STRICT_PARSING
	//more strict checking
	if(*s=='+')
		throw unparseable_number(p); //numbers must not start with a plus
	//check for leading zeros
	if(p-s>=2) {
		const char *q = s;
		if(*q=='-')
			q++;
		if(p-q>=2) {
			if(q[0]=='0' && q[1]!='.' && q[1]!='e')
				throw unparseable_number(p);
		}
		if(q[0]=='.')
			throw unparseable_number(p);
	}
	if(decimal_point) {
		if(decimal_point+1==p)
			throw unparseable_number(p);
		if(decimal_point[1]<'0' || decimal_point[1]>'9')
			throw unparseable_number(p);
	}
#endif
	
	std::string copy(s,p-s);
	if(float_chars) {
		if(float_chars>2)
			throw unparseable_number(s);
		char *endptr=nullptr;
		errno = 0;
		double d = strtod(copy.c_str(),&endptr);
		if(endptr && *endptr)
			throw unparseable_number(s);
		if((d==HUGE_VAL || d==-HUGE_VAL) && errno==ERANGE)
			throw unparseable_number(s);
		value->value_type = value_type_t::number_double;
		value->u.number_doublevalue = d;
	} else {
		char *endptr=nullptr;
		errno = 0;
		long l = strtol(copy.c_str(),&endptr,10);
		if(endptr && *endptr)
			throw unparseable_number(s);
		if((l==LONG_MAX || l==LONG_MIN) && errno==ERANGE)
			throw unparseable_number(s);
		value->value_type = value_type_t::number_int64;
		value->u.number_int64value = l;
	}
	return p;
}


const char *ijson2::Parser::parse_array_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels) {
	new (&value->u.array_elements) Value::array_type;
	value->value_type = value_type_t::array;
	bool first = true;
	const char *p = s+1;
	while(p<end) {
		p = skip_ws(p,end);
		if(p==end)
			throw unterminated_array(p);
		if(*p==']')
			return p+1;
		if(!first) {
			if(*p!=',')
				throw junk(p);
			p++;
			p = skip_ws(p,end);
		}
		value->u.array_elements.push_back(Value());
		p = parse_value(p,end,&value->u.array_elements.back(),max_nesting_levels);
		first = false;
	}
	throw unterminated_array(s);
}


const char *ijson2::Parser::parse_object_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels) {
	new (&value->u.object_members) Value::map_type;
	value->value_type = value_type_t::object;
	bool first = true;
	const char *p = s+1;
	while(p<end) {
		p = skip_ws(p,end);
		if(p==end)
			throw unterminated_object(p);
		if(*p=='}')
			return p+1;
		if(!first) {
			if(*p!=',')
				throw junk(p);
			p++;
			p = skip_ws(p,end);
		}
		if(*p!='"')
			throw expected_string(p);
		
		string_view sv;
		p = parse_string(p,end,&sv);
		p = skip_ws(p,end);
		if(p==end || *p!=':')
			throw expected_colon(p);
		p++;
		p = skip_ws(p,end);
		value->u.object_members[sv];
		p = parse_value(p,end,&value->u.object_members[sv],max_nesting_levels);
		first = false;
	}
	throw unterminated_object(p);
}


const char *ijson2::Parser::parse_value(const char *s, const char *end, Value *value, unsigned max_nesting_levels) {
	s = skip_ws(s,end);
	if(s==end)
		throw expected_value(s);
	switch(s[0])  {
		case '{':
			if(max_nesting_levels==0)
				throw too_many_levels(s);
			return parse_object_value(s,end,value,max_nesting_levels-1);
		case '[':
			if(max_nesting_levels==0)
				throw too_many_levels(s);
			return parse_array_value(s,end,value,max_nesting_levels-1);
		case '"':
			return parse_string_value(s,end,value);
		case 'f':
			return parse_false_value(s,end,value);
		case 'n':
			return parse_null_value(s,end,value);
		case 't':
			return parse_true_value(s,end,value);
		default:
			return parse_number_value(s,end,value);
	}
}



bool ijson2::Parser::may_be_complete(const char *s, size_t sz) {
	const char *end = s+sz;
	s = skip_ws(s,end);
	end = skip_ws_reverse(s,end);
	if(end-s<2)
		return false;
	switch(s[0])  {
		case '{':
			return end[-1]=='}';
		case '[':
			return end[-1]==']';
		case '"':
			return end[-1]=='"';
		case 'f':
			return end[-1]=='e';
		case 'n':
			return end[-1]=='l';
		case 't':
			return end[-1]=='e';
		default:
			return true;
	}
}


void ijson2::Parser::parse(const char *s, size_t sz, unsigned max_nesting_levels) {
	//skip BOM if present
	if(sz>=3 && s[0]==(char)0xEF && s[1]==(char)0xBB && s[2]==(char)0xBF) {
		s += 3;
		sz -= 3;
	}
	
	const char *e = parse_value(s,s+sz, &top_value, max_nesting_levels);
	e = skip_ws(e,s+sz);
	if(e!=s+sz)
		throw junk(e);
}
