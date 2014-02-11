/** @file alloc.c */#include <stdlib.h>#include <stdio.h>#include <unistd.h>#include <string.h>#include "debug.h"

typedef struct _meta {	struct _meta * next;	size_t len;} __attribute__((__may_alias__)) mem;
//there is a bug in gcc

#define MINCHUNKSIZE (2 * sizeof(mem) + 16)#define MINALLOCSIZE (1<<13)
static mem * free_list_ptr = NULL;
/** * Allocate space for array in memory *  * Allocates a block of memory for an array of num elements, each of them size * bytes long, and initializes all its bits to zero. The effective result is * the allocation of an zero-initialized memory block of (num * size) bytes. *  * @param num *    Number of elements to be allocated. * @param size *    Size of elements. * * @return *    A pointer to the memory block allocated by the function. * *    The type of this pointer is always void*, which can be cast to the *    desired type of data pointer in order to be dereferenceable. * *    If the function failed to allocate the requested block of memory, a *    NULL pointer is returned. * * @see http://www.cplusplus.com/reference/clibrary/cstdlib/calloc/ */void *calloc(size_t num, size_t size){	/* Note: This function is complete. You do not need to modify it. */	void *ptr = malloc(num * size);		if (ptr)		memset(ptr, 0x00, num * size);	return ptr;}/** * Allocate memory block * * Allocates a block of size bytes of memory, returning a pointer to the * beginning of the block.  The content of the newly allocated block of * memory is not initialized, remaining with indeterminate values. * * @param size *    Size of the memory block, in bytes. * * @return *    On success, a pointer to the memory block allocated by the function. * *    The type of this pointer is always void*, which can be cast to the *    desired type of data pointer in order to be dereferenceable. * *    If the function failed to allocate the requested block of memory, *    a null pointer is returned. * * @see http://www.cplusplus.com/reference/clibrary/cstdlib/malloc/ */void *malloc(size_t size){	mem * finder = free_list_ptr;
	mem * prev = (void *) &free_list_ptr;
	mem * curr_good_chunk = NULL;
	mem * prev_good_chunk = NULL;
	unsigned int chunk_size = 0;
	unsigned int min_chunk_size = 0;
	void * heap_top = NULL;
	void * heap_top_new = NULL;	size = (size+7) & ~0x7; //alignment to 8 bits	
	while(finder!=NULL)
	{		if ((chunk_size = finder->len) >= size
			 && (!curr_good_chunk || min_chunk_size > chunk_size))
		{			min_chunk_size = chunk_size;			curr_good_chunk = finder;			prev_good_chunk = prev;		}
		prev = finder;
		finder = finder->next;	}
	if (curr_good_chunk)
	{		if (curr_good_chunk->len - size >= MINCHUNKSIZE)
		{ /* split block */			finder = (mem *) (((void *) (curr_good_chunk+1)) + size);			finder->next = curr_good_chunk->next;			finder->len = curr_good_chunk->len - size - sizeof(mem);			prev_good_chunk->next = finder;			curr_good_chunk->len = size;		}
		else
		{
			prev_good_chunk->next = curr_good_chunk->next;
		}	}	else
	{		heap_top = (void *)(((long)sbrk(0) + 7) & ~7);		if (free_list_ptr != NULL
			&& (void *)(free_list_ptr+1) + free_list_ptr->len == heap_top) 
		{			curr_good_chunk = free_list_ptr;			free_list_ptr = free_list_ptr->next;		}		else
		{			curr_good_chunk = (mem *) heap_top;
		}		heap_top_new = (void *)(curr_good_chunk+1) + size;		if (sbrk((int)(heap_top_new-heap_top+MINALLOCSIZE)) == (void *) -1) return NULL;		prev = (mem *)heap_top_new;		prev->len = MINALLOCSIZE - sizeof(mem);		prev->next = free_list_ptr;		free_list_ptr = prev;		heap_top_new += MINALLOCSIZE;		curr_good_chunk->len = size;	}	return (void *) (curr_good_chunk+1);}/** * Deallocate space in memory *  * A block of memory previously allocated using a call to malloc(), * calloc() or realloc() is deallocated, making it available again for * further allocations. * * Notice that this function leaves the value of ptr unchanged, hence * it still points to the same (now invalid) location, and not to the * null pointer. * * @param ptr *    Pointer to a memory block previously allocated with malloc(), *    calloc() or realloc() to be deallocated.  If a null pointer is *    passed as argument, no action occurs. */void free(void *ptr){	if (!ptr) return;
	mem * finder = free_list_ptr;
	mem * head = ptr - sizeof(mem);
	mem * prev = (void*)&free_list_ptr;
	void * finder_next = NULL;
	void * head_next = ptr + head->len;

	while(1){		if (head_next == (void *) finder)
		{			head->len += finder->len + sizeof(mem);			finder = finder->next;		}
		finder_next = finder ? ((void *) (finder+1)) + finder->len : 0;
		if (finder_next == (void *)head)
		{			finder->len += sizeof(mem) + head->len;			head->len = 0;			head->next = finder;			prev->next = finder;			break;		}		if (finder_next < (void *) head)
		{			prev->next = head;			head->next = finder;			break;		}
		prev = finder;
		finder = finder->next;	}}/** * Reallocate memory block * * The size of the memory block pointed to by the ptr parameter is changed * to the size bytes, expanding or reducing the amount of memory available * in the block. * * The function may move the memory block to a new location, in which case * the new location is returned. The content of the memory block is preserved * up to the lesser of the new and old sizes, even if the block is moved. If * the new size is larger, the value of the newly allocated portion is * indeterminate. * * In case that ptr is NULL, the function behaves exactly as malloc, assigning * a new block of size bytes and returning a pointer to the beginning of it. * * In case that the size is 0, the memory previously allocated in ptr is * deallocated as if a call to free was made, and a NULL pointer is returned. * * @param ptr *    Pointer to a memory block previously allocated with malloc(), calloc() *    or realloc() to be reallocated. * *    If this is NULL, a new block is allocated and a pointer to it is *    returned by the function. * * @param size *    New size for the memory block, in bytes. * *    If it is 0 and ptr points to an existing block of memory, the memory *    block pointed by ptr is deallocated and a NULL pointer is returned. * * @return *    A pointer to the reallocated memory block, which may be either the *    same as the ptr argument or a new location. * *    The type of this pointer is void*, which can be cast to the desired *    type of data pointer in order to be dereferenceable. *     *    If the function failed to allocate the requested block of memory, *    a NULL pointer is returned, and the memory block pointed to by *    argument ptr is left unchanged. * * @see http://www.cplusplus.com/reference/clibrary/cstdlib/realloc/ */void *realloc(void *ptr, size_t size){	if (!ptr) return malloc(size);
	if (!size)
	{
		free(ptr);
		return NULL;
	}

	mem * prev = (mem *) (ptr - sizeof(mem));	size_t prev_chunk_size = prev->len;

	void * newptr;
	void * finder;
	free(ptr);	if (prev_chunk_size > size)
	{		prev_chunk_size = (size + 7) & ~7;
	}
	if (!prev->len)
	{		finder = (void *)(prev->next + 1);		memcpy(finder, ptr, prev_chunk_size);		ptr = finder;	}	newptr = malloc(size);	if (newptr && newptr != ptr)		memcpy(newptr, ptr, prev_chunk_size);	return newptr;}
