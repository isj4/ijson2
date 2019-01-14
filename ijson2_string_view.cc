#include "ijson2_string_view.hh"
#include <string.h>

ijson2::string_view::string_view(const char *s_)
  : s{s_},
    count{strlen(s_)}
{}


int ijson2::string_view::compare(ijson2::string_view v) const noexcept {
	size_type l = count<v.count ? count : v.count;
	int r = memcmp(s,v.s,l);
	if(r!=0)
		return r;
	else if(count<v.count)
		return -1;
	else if(count>v.count)
		return  1;
	else
		return 0;
}