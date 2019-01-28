#ifndef IJSON2_STRING_VIEW_HH_
#define IJSON2_STRING_VIEW_HH_
#include <stddef.h>

namespace ijson2 {

//C++17 introduces std::string_view, but we cannot rely on that yet. So we roll our own.
//But we only implement enough of it to support our use. In 5-10 years time we can
//switch to std::string_view
class string_view {
	const char *s;
	size_t count;
public:
	typedef char value_type;
	typedef char *pointer;
	typedef const char *const_pointer;
	typedef char& reference;
	typedef const char& const_reference;
	typedef char *iterator;
	typedef const char *const_iterator;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	
	static constexpr size_type npos = size_type(-1);
	
	
	constexpr string_view() noexcept
	  : s{nullptr},
	    count{0}
	{}
	constexpr string_view(const string_view&) noexcept = default;
	constexpr string_view(const char *s_, size_t count_) noexcept
	  : s{s_},
	    count{count_}
	{}
	string_view(const char *s_) noexcept;
	
	string_view& operator=(const string_view&) noexcept = default;
	
	constexpr const_iterator begin() const noexcept {
		return s;
	}
	constexpr const_iterator end() const noexcept {
		return s+count;
	}
	
	constexpr const_reference operator[](size_type pos) const noexcept {
		return s[pos];
	}
	constexpr const_reference front() const noexcept {
		return s[0];
	}
	constexpr const_reference back() const noexcept {
		return s[count-1];
	}
	constexpr const_pointer data() const noexcept {
		return s;
	}
	
	constexpr size_type size() const noexcept {
		return count;
	}
	constexpr size_type length() const noexcept {
		return count;
	}
	
	constexpr size_type max_size() const noexcept {
		return npos-sizeof(size_type)-sizeof(char*);
	}
	
	constexpr bool empty() const noexcept {
		return count==0;
	}
	
	void remove_prefix(size_type n) {
		s += n;
	}
	void remove_suffix(size_type n) {
		count -= n;
	}
	
	
	void swap(string_view &v) noexcept {
		const char *s_tmp = s;
		s = v.s;
		v.s = s_tmp;
		size_type count_tmp = count;
		count = v.count;
		v.count = count_tmp;
	}
	
	constexpr string_view substr(size_type pos=0, size_type count_=npos) const {
		return string_view(s+pos, pos+count_ <= count ? count_ : count-pos);
	}
	
	int compare(string_view v) const noexcept;

	bool operator==(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)==0;
	}
	bool operator!=(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)!=0;
	}
	bool operator<(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)<0;
	}
	bool operator<=(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)<=0;
	}
	bool operator>(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)>0;
	}
	bool operator>=(const ijson2::string_view rhs) const noexcept {
		return compare(rhs)>=0;
	}
	
	bool operator==(const char *str) const noexcept;
};

} //namespace


#endif
