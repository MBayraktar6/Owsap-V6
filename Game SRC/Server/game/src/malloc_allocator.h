#ifndef __INC_MALLOC_ALLOCATOR_H__
#define __INC_MALLOC_ALLOCATOR_H__

// Allocator implementation detail based on default CRT malloc/free.
class MallocAllocator
{
public:
	MallocAllocator() {}
	~MallocAllocator() {}

	void SetUp() {}
	void TearDown() {}

	void* Alloc(size_t size)
	{
		return ::malloc(size);
	}

	void Free(void* p)
	{
		::free(p);
	}
};

#endif // __INC_MALLOC_ALLOCATOR_H__
