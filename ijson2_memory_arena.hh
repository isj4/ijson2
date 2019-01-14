#ifndef IJSON2_MEMORY_ARENA_HH_
#define IJSON2_MEMORY_ARENA_HH_
#include <stddef.h>
#include <vector>


namespace ijson2 {

//A non-freeing memory arena.
//You construct it, allocate several items from it and then free all in one go.
class MemoryArena {
	MemoryArena(const MemoryArena&) = delete;
	MemoryArena& operator=(const MemoryArena&) = delete;
public:
	MemoryArena(size_t chunk_size_ = 4096)
	  : chunks(),
	    lastest_chunk_size(0),
	    lastest_chunk_used(0),
	    chunk_size(chunk_size_)
	  {}
	~MemoryArena() { clear(); }
	
	void *alloc(size_t bytes, size_t alignment);
	void clear();

private:
	std::vector<char *> chunks;
	size_t lastest_chunk_size;
	size_t lastest_chunk_used;
	const size_t chunk_size;
};


template<class T>
struct arena_allocator {
	using value_type = T;
	
	arena_allocator(MemoryArena &ma_) noexcept
	  : ma(&ma_)
	  {}
	arena_allocator(const arena_allocator&) noexcept = default;
	template<class U>
	arena_allocator(const arena_allocator<U> &aa) noexcept
	  : ma(aa.ma) {}
	arena_allocator& operator=(const arena_allocator&) noexcept = default;
	
	T* allocate(std::size_t n) {
		return reinterpret_cast<T*>(ma->alloc(n*sizeof(T),alignof(T)));
	}
	void deallocate(T * /*ptr*/, std::size_t /*n*/) {
		//noop
	}
	
	MemoryArena *ma;
};

} //namespace

template<class T, class U>
inline bool operator==(const ijson2::arena_allocator<T> &lhs, const ijson2::arena_allocator<U> &rhs) {
	return &lhs.ma == rhs.ma;
}

template<class T, class U>
inline bool operator!=(const ijson2::arena_allocator<T> &lhs, const ijson2::arena_allocator<U> &rhs) {
	return &lhs.ma != rhs.ma;
}


#endif
