#include "ijson2_memory_arena.hh"


void *ijson2::MemoryArena::alloc(size_t bytes, size_t alignment) {
	if(alignment>2)
		lastest_chunk_used = (lastest_chunk_used+alignment-1)&~(alignment-1);
	if(chunks.empty() || lastest_chunk_used+bytes > lastest_chunk_size) {
		chunks.reserve(chunks.size()+10);
		size_t cs = bytes<=chunk_size ? chunk_size : bytes;
		char *new_chunk = new char[cs];
		chunks.push_back(new_chunk);
		lastest_chunk_size = cs;
		lastest_chunk_used = 0;
	}
	
	char *ptr = chunks.back() + lastest_chunk_used;
	lastest_chunk_used += bytes;
	return ptr;
}


void ijson2::MemoryArena::clear() {
	for(auto e : chunks)
		delete[] e;
	chunks.clear();
	lastest_chunk_size = 0;
	lastest_chunk_used = 0;
}

#ifdef UNITTEST
#include <map>
#include <string>
int main() {
	ijson2::MemoryArena ma;
	std::map<int,std::string,std::less<int>,ijson2::arena_allocator<char>> m(ma);
	m[17] = "1717";
	m[32] = "3232";
	m[42] = "4242";
	m[18] = "1818";
}
#endif
