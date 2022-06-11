#include "common.h"
#include "ctype.h"
#include "io.h"
#include "stdio.h"
#include "stdlib.h"
#include "system_call.h"

typedef struct _ComHeader
{
	void *text_base;
	void *rodata_base;
	void *data_base;
	void *bss_base;
	void *common_base;
	void *common_deletion_prevention_base;
	void *heap_and_stack_base;
	unsigned int heap_and_stack_size;
} ComHeader;

typedef struct _MemorySection
{
	struct _MemorySection *previous;
	struct _MemorySection *next;
	size_t size; // This doesn't include sizeof(MemorySection)
	unsigned char flags;
	#define MEMORY_SECTION_ALLOCATED 0x01
} MemorySection;

typedef struct _Ring
{
	size_t element_size;
	size_t number_of_elements;
	void *data;
} Ring;

ComHeader *com_header = NULL;
MemorySection *root_memory_section = NULL;
Ring *mersenne_twister_ring = NULL;
unsigned int mersenne_twister_N = 624;

Ring *create_ring(size_t number_of_elements, size_t element_size);
void delete_ring(Ring *ring);
void *ring_element(Ring *ring, int index);
unsigned int *mersenne_twister_ring_element(int index);

int atoi(char const *digits)
{
	int value = 0;
	unsigned char flags = 0x00;
	#define ATOI_FLAG_MINUS 0x01
	if(*digits == '-')
	{
		flags |= ATOI_FLAG_MINUS;
		digits++;
	}
	for(; *digits; digits++)
		if(isdigit(*digits))
		{
			value *= 10;
			if(flags & ATOI_FLAG_MINUS)value -= *digits - '0';
			else value += *digits - '0';
		}
		else return 0;
	return value;
}

Ring *create_ring(size_t number_of_elements, size_t element_size)
{
	Ring *ring;
	ring = malloc(sizeof(*ring));
	ring->element_size = element_size;
	ring->number_of_elements = number_of_elements;
	ring->data = malloc(number_of_elements * element_size);
	return ring;
}

void delete_ring(Ring *ring)
{
	free(ring->data);
	free(ring);
}

void exit(int status)
{
	system_call_exit(status);
}

void free(void *address)
{
	MemorySection *memory_section;
	memory_section = (MemorySection *)(address - sizeof(*memory_section));
	if(memory_section->flags & MEMORY_SECTION_ALLOCATED)
	{
		memory_section->flags &= ~MEMORY_SECTION_ALLOCATED;
		if((void *)memory_section->previous + sizeof(*memory_section->previous) + memory_section->previous->size == (void *)memory_section && memory_section->previous->flags == memory_section->flags) // merge memory_section and memory_section->previous
		{
			if(root_memory_section == memory_section)root_memory_section = memory_section->previous;
			memory_section->previous->next = memory_section->next;
			memory_section->next->previous = memory_section->previous;
			memory_section->previous->size += sizeof(*memory_section) + memory_section->size;
			memory_section = memory_section->previous;
		}
		if((void *)memory_section + sizeof(*memory_section) + memory_section->size == (void *)memory_section->next && memory_section->next->flags == memory_section->flags) // merge memory_section and memory_section->next
		{
			if(root_memory_section == memory_section->next)root_memory_section = memory_section;
			memory_section->size += sizeof(*memory_section->next) + memory_section->next->size;
			memory_section->next->next->previous = memory_section;
			memory_section->next = memory_section->next->next;
		}
	}
	else
	{
		ERROR();// double free error!
		exit(1);
	}
	return;
}

// It's equal to init_by_array in http://math.sci.hiroshima-u.ac.jp/m-mat/MT/MT2002/CODES/mt19937ar.c
void init_mersenne_twister(unsigned int *key, unsigned int length)
{
	unsigned int i, j;
	srand(19650218);
	i = 1;
	j = 0;
	for(unsigned int k = length < mersenne_twister_N ? mersenne_twister_N : length; k; k--)
	{
		*mersenne_twister_ring_element(i) ^= 1664525 * (*mersenne_twister_ring_element(i - 1) ^ *mersenne_twister_ring_element(i - 1) >> 30);
		*mersenne_twister_ring_element(i) += key[j] + j;
		i++;
		if(mersenne_twister_N <= i)
		{
			*mersenne_twister_ring_element(0) = *mersenne_twister_ring_element(-1);
			i = 1;
		}
		j++;
		j %= length;
	}
	for(unsigned int k = mersenne_twister_N - 1; k; k--)
	{
		*mersenne_twister_ring_element(i) ^= 1566083941 * (*mersenne_twister_ring_element(i - 1) ^ *mersenne_twister_ring_element(i - 1) >> 30);
		*mersenne_twister_ring_element(i) -= i;
		i++;
		if(mersenne_twister_N <= i)
		{
			*mersenne_twister_ring_element(0) = *mersenne_twister_ring_element(-1);
			i = 1;
		}
	}
	*mersenne_twister_ring_element(0) = 0x80000000;
	for(unsigned int i = 0; i < mersenne_twister_N; i++)printf("%u\n", *mersenne_twister_ring_element(i));
}

void *malloc(size_t size)
{
	MemorySection *memory_section;
	if(!size)return NULL;
	if(!root_memory_section)root_memory_section = com_header->heap_and_stack_base;
	memory_section = root_memory_section;
	do
	{
		if(!(memory_section->flags & MEMORY_SECTION_ALLOCATED) && size <= memory_section->size)
		{
			void *allocated = (void *)memory_section + sizeof(*memory_section);
			if(memory_section->size <= size + sizeof(MemorySection))memory_section->flags |= MEMORY_SECTION_ALLOCATED; // Can't devide the memory section
			else // devide the memory section
			{
				MemorySection *remaining_memory_section = (MemorySection *)((void *)memory_section + sizeof(*memory_section) + size);
				if(get_esp() <= (void *)(remaining_memory_section + 1))
				{
					ERROR(); // Collision between stack and heap
					exit(1);
				}
				remaining_memory_section->previous = memory_section;
				remaining_memory_section->next = memory_section->next;
				remaining_memory_section->size = memory_section->size - size - sizeof(*remaining_memory_section);
				remaining_memory_section->flags = memory_section->flags;
				memory_section->next->previous = remaining_memory_section;
				memory_section->next = remaining_memory_section;
				memory_section->size = size;
				memory_section->flags |= MEMORY_SECTION_ALLOCATED;
			}
			root_memory_section = memory_section->next;
			return allocated;
		}
		memory_section = memory_section->next;
	} while(memory_section != root_memory_section);
	ERROR(); // There is no enough memory.
	exit(1);
	return NULL;
}

unsigned int *mersenne_twister_ring_element(int index)
{
	return (unsigned int *)ring_element(mersenne_twister_ring, index);
}

void *ring_element(Ring *ring, int index)
{
	index %= ring->number_of_elements;
	if(index < 0)index += ring->number_of_elements;
	return ring->data + index * ring->element_size;
}

void srand(unsigned int seed)
{
	if(mersenne_twister_ring)delete_ring(mersenne_twister_ring);
	mersenne_twister_ring = create_ring(mersenne_twister_N, sizeof(unsigned int));
	for(unsigned int i = 0; i < mersenne_twister_N; i++)*mersenne_twister_ring_element(i) = i ? 1812433253 * (*mersenne_twister_ring_element(i - 1) ^ (*mersenne_twister_ring_element(i - 1) >> 30)) + i : seed;
}

