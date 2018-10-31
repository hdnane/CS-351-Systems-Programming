/*
* mm.c - The fastest, least memory-efficient malloc package.
*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "mm.h"
#include "memlib.h"

/* single word (4) or double word (8) alignment */

#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~ 0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))
#define HEADER_T_SIZE (ALIGN (sizeof(header_t) ))
#define FOOTER_T_SIZE (ALIGN (sizeof(footer_t) ))
#define FREE_BLK_T_SIZE (ALIGN (sizeof(header_t) + sizeof(footer_t)))

typedef int boolean;
#define true 1
#define false 0

void *mm_find_first_fit(size_t size);
void mm_split();
void mm_coalesce();

typedef struct footer footer_t;
typedef struct header header_t;

struct footer {
	size_t size;
};

struct header {
	size_t size;
	header_t *next;
	header_t *prior;
};

/*
* mm_init - initialize the malloc package.
*/

int mm_init()
{
	header_t *hptr = mem_sbrk(FREE_BLK_T_SIZE);
	hptr->size = FREE_BLK_T_SIZE;
	hptr->next = hptr;
	hptr->prior = hptr;

	footer_t *fptr = (footer_t *)((char *)hptr + HEADER_T_SIZE);
	fptr->size = FREE_BLK_T_SIZE;
	return 0;
}

/*
* mm_malloc - Allocate a block by incrementing the brk pointer.
*     Always allocate a block whose size is a multiple of the alignment.
*/
void *mm_malloc(size_t size)
{
	if (size >100 && size < 128) {
		size = 128;
	}
	if (size > 400 && size < 512) {
		size = 512;
	}

	int newsize = ALIGN(size + FREE_BLK_T_SIZE);

	if (size <= 0) {
		return NULL;
	}

	header_t *hptr = mm_find_first_fit(newsize);
	footer_t *fptr = NULL;

	if (hptr == NULL) {

		hptr = mem_sbrk(newsize);
		if ((long)hptr == -1) {
			return NULL;
		} else {
			hptr->size = newsize | 1;
			fptr = (footer_t *)((char*)hptr + newsize - FOOTER_T_SIZE);
			fptr->size = newsize | 1;
		}
	}
	else {
		if ( hptr->size >= 2 * newsize ) {
			mm_split(hptr, fptr, newsize);
		} else {
			fptr = (footer_t *)((char *)hptr + hptr->size - FOOTER_T_SIZE);
			fptr->size |= 1;
			hptr->size |= 1;

			hptr->prior->next = hptr->next;
			hptr->next->prior = hptr->prior;
		}
	}

	return (void*)((char *)hptr + HEADER_T_SIZE);
}

// newsize is required_payload + sizeof(headrer) + sizeof(footer)
void mm_split(header_t* hptr, footer_t* fptr, size_t small_size) {

	size_t	free_space = hptr->size - small_size;
	header_t * free_hptr = NULL;
	footer_t * free_fptr = NULL;

	free_hptr = (header_t*)((char*)hptr + small_size);
	free_hptr->size = free_space & ~1;
	free_fptr = (footer_t*)((char*)free_hptr + free_space - FOOTER_T_SIZE);
	free_fptr->size = free_space & ~1;

	fptr = (footer_t*)((char*)free_hptr - FOOTER_T_SIZE);
	fptr->size = small_size | 1;
	hptr->size = small_size | 1;
	hptr->next->prior = hptr->prior;
	hptr->prior->next = hptr->next;

	header_t * mm_ptr = NULL;
	mm_ptr = mem_heap_lo();
	free_hptr->next = mm_ptr->next;
	free_hptr->prior = mm_ptr;
	mm_ptr->next = free_hptr;
	free_hptr->next->prior = free_hptr;
}

void *mm_find_first_fit(size_t newsize) {

	header_t *ptr = NULL;

	for (ptr = ((header_t *)mem_heap_lo())->next;
		ptr != mem_heap_lo() && ptr->size < newsize;
		ptr = ptr->next);
	if (ptr != mem_heap_lo())
	{
		return ptr;
	}
	else {
		return NULL;
	}

}

/*
* mm_free - Freeing a block does nothing.
*/
void mm_free(void *ptr)
{
	header_t *hptr = ptr - HEADER_T_SIZE;
	header_t *head = mem_heap_lo();
	hptr->size &= ~1;
	hptr->next = head->next;
	hptr->prior = head;
	head->next = hptr;
	hptr->next->prior = hptr;

	footer_t *fptr = (footer_t *)((char *)hptr + hptr->size - FOOTER_T_SIZE);
	fptr->size &= ~1;
	mm_coalesce(hptr);
}

/*
*mm_coalesce
*/
void mm_coalesce(header_t* hptr) {
	
	footer_t *pre_fptr = (footer_t*)((char*)hptr - FOOTER_T_SIZE);
	boolean pre_set;
	
	header_t *nxt_hptr = (header_t*)((char*)hptr + hptr->size);
	boolean next_set;

	if ((char*) nxt_hptr + FREE_BLK_T_SIZE < (char*) mem_heap_hi()) {
		next_set = nxt_hptr->size & 1; /* Could be true or false */
	} else {
		next_set = true;
	}

	if ((char*) pre_fptr > (char*) mem_heap_lo() + FREE_BLK_T_SIZE) {
		pre_set = pre_fptr->size & 1;
	} else {
		pre_set = true;
	}

	if (pre_set && next_set)
	{
		return;

	} else if (next_set && !pre_set) {
		((header_t*)((char*)hptr - pre_fptr->size))->size += hptr->size;
		((header_t*)((char*)hptr + hptr->size - FOOTER_T_SIZE))->size = ((header_t*)((char*)hptr - pre_fptr->size))->size;

		hptr->next->prior = hptr->prior;
		hptr->prior->next = hptr->next;
	} else if (pre_set && !next_set) {
		hptr->size += nxt_hptr->size;
		((footer_t *)((char *)nxt_hptr + nxt_hptr->size - FOOTER_T_SIZE))->size = hptr->size;

		nxt_hptr->next->prior = nxt_hptr->prior;
		nxt_hptr->prior->next = nxt_hptr->next;
	} else {
		((header_t*)((char*)hptr - pre_fptr->size))->size += (hptr->size + nxt_hptr->size);
		((header_t*)((char*)hptr + hptr->size - FOOTER_T_SIZE))->size = ((header_t *)((char*)hptr - pre_fptr->size))->size;
		((footer_t*)((char*)nxt_hptr + nxt_hptr->size - FOOTER_T_SIZE))->size = ((header_t*)((char *)hptr - pre_fptr->size))->size;

		hptr->next->prior = hptr->prior;
		hptr->prior->next = hptr->next;
		nxt_hptr->next->prior = nxt_hptr->prior;
		nxt_hptr->prior->next = nxt_hptr->next;
	}
	return;
}

/*
* mm_realloc - Implemented simply in terms of mm_malloc and mm_free
*/
void *mm_realloc(void *ptr, size_t size)
{
	header_t * hptr = ptr - HEADER_T_SIZE;
	size_t new_size = ALIGN(size + FREE_BLK_T_SIZE);
	size_t old_size = hptr->size;
	void * newptr = NULL;

	if (new_size == 0) {
		mm_free(ptr);
		return 0;
	}

	if (old_size > new_size) {
		return ptr;
	}

	newptr = mm_malloc(new_size);
	if (!newptr) {
		return 0;
	}
	size_t copySize = hptr->size - HEADER_T_SIZE;

	if (size < copySize) {
		copySize = new_size;
	}

	memcpy(newptr, ptr, copySize);
	mm_free(ptr);
	return newptr;
}
