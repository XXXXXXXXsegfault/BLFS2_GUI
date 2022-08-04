/*
 * NOTE: This implementation has no multithreading support.
 *
 * */

#define MALLOC_MAGIC 0xacf31e53
#define MALLOC_TABLEN 65537
unsigned long int __malloc_count_del;
struct __malloc_zone
{
	unsigned int magic;
	unsigned char start_color;
	unsigned char end_color;
	unsigned char color;
	unsigned char used;
	unsigned long int *block_links;
	unsigned long int size;
	struct __malloc_zone *start_left;
	struct __malloc_zone *start_right;
	struct __malloc_zone *start_parent;
	struct __malloc_zone *end_left;
	struct __malloc_zone *end_right;
	struct __malloc_zone *end_parent;
	struct __malloc_zone *left;
	struct __malloc_zone *right;
	struct __malloc_zone *parent;
};
#define RBTREE_TYPE struct __malloc_zone
#define RBTREE_CMP(n1,n2) ((n1)->size>(n2)->size||(n1)->size==(n2)->size&&(unsigned long int)(n1)>(unsigned long int)(n2))
#define RBTREE_LEFT left
#define RBTREE_RIGHT right
#define RBTREE_PARENT parent
#define RBTREE_COLOR color
#define RBTREE_INSERT __malloc_zone_size_add
#define RBTREE_DELETE __malloc_zone_size_del
#include "templates/rbtree.c"
#define RBTREE_TYPE struct __malloc_zone
#define RBTREE_CMP(n1,n2) ((unsigned long int)(n1)>(unsigned long int)(n2))
#define RBTREE_LEFT start_left
#define RBTREE_RIGHT start_right
#define RBTREE_PARENT start_parent
#define RBTREE_COLOR start_color
#define RBTREE_INSERT __malloc_zone_start_add
#define RBTREE_DELETE __malloc_zone_start_del
#include "templates/rbtree.c"
#define RBTREE_TYPE struct __malloc_zone
#define RBTREE_CMP(n1,n2) ((unsigned long int)(n1)+(n1)->size>(unsigned long int)(n2)+(n2)->size)
#define RBTREE_LEFT end_left
#define RBTREE_RIGHT end_right
#define RBTREE_PARENT end_parent
#define RBTREE_COLOR end_color
#define RBTREE_INSERT __malloc_zone_end_add
#define RBTREE_DELETE __malloc_zone_end_del
#include "templates/rbtree.c"
char *__current_brk;
unsigned long int __heap_size;
void *__set_heap_size(unsigned long int size)
{
	char *new_brk,*old_brk;
	if(__current_brk==0)
	{
		__current_brk=brk(0);
	}
	old_brk=__current_brk+__heap_size;
	new_brk=brk(__current_brk+size);
	if(new_brk==old_brk&&size!=__heap_size)
	{
		return 0;
	}
	__heap_size=size;
	return old_brk;
}

struct __malloc_zone *__malloc_start_tab[MALLOC_TABLEN],*__malloc_end_tab[MALLOC_TABLEN],*__malloc_zone_root;
void __malloc_zone_start_tab_add(struct __malloc_zone *node)
{
	unsigned long long int addr;
	int hash;
	addr=(unsigned long long int)node;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	__malloc_zone_start_add(__malloc_start_tab+hash,node);
}
void __malloc_zone_end_tab_add(struct __malloc_zone *node)
{
	unsigned long long int addr;
	int hash;
	addr=(unsigned long long int)node+node->size;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	__malloc_zone_end_add(__malloc_end_tab+hash,node);
}
void __malloc_zone_start_tab_del(struct __malloc_zone *ptr)
{
	unsigned long long int addr;
	int hash;
	addr=(unsigned long long int)ptr;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	__malloc_zone_start_del(__malloc_start_tab+hash,ptr);
}
void __malloc_zone_end_tab_del(struct __malloc_zone *ptr)
{
	unsigned long long int addr;
	int hash;
	addr=(unsigned long long int)ptr+ptr->size;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	__malloc_zone_end_del(__malloc_end_tab+hash,ptr);
}
struct __malloc_zone *__malloc_zone_start_tab_find(void *ptr)
{
	unsigned long long int addr;
	int hash;
	struct __malloc_zone *node;
	addr=(unsigned long long int)ptr;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	node=__malloc_start_tab[hash];
	while(node&&(unsigned long long int)node!=addr)
	{
		if((unsigned long long int)node>addr)
		{
			node=node->start_left;
		}
		else
		{
			node=node->start_right;
		}
	}
	return node;
}
struct __malloc_zone *__malloc_zone_end_tab_find(void *ptr)
{
	unsigned long long int addr;
	int hash;
	struct __malloc_zone *node;
	addr=(unsigned long long int)ptr;
	hash=(addr>>16|addr<<48)%MALLOC_TABLEN;
	node=__malloc_end_tab[hash];
	while(node&&(unsigned long long int)node+node->size!=addr)
	{
		if((unsigned long long int)node+node->size>addr)
		{
			node=node->end_left;
		}
		else
		{
			node=node->end_right;
		}
	}
	return node;
}
struct __malloc_zone *__malloc_zone_size_find(unsigned long long int size)
{
	struct __malloc_zone *node,*p;
	node=__malloc_zone_root;
	p=0;
	while(node)
	{
		if(node->size>=size)
		{
			p=node;
			node=node->left;
		}
		else
		{
			node=node->right;
		}
	}
	return p;
}
void __malloc_error(void)
{
	write(2,"invalid pointer or corruption detected.\n",40);
	while(1)
	{
		asm "int3"
	}
}
void __malloc_zone_add(struct __malloc_zone *node)
{
	if(node->magic!=MALLOC_MAGIC)
	{
		__malloc_error();
	}
	node->used=0;
	__malloc_zone_size_add(&__malloc_zone_root,node);
	__malloc_zone_start_tab_add(node);
	__malloc_zone_end_tab_add(node);
}
void __malloc_zone_del(struct __malloc_zone *node)
{
	if(node->magic!=MALLOC_MAGIC)
	{
		__malloc_error();
	}
	node->used=0;
	__malloc_zone_size_del(&__malloc_zone_root,node);
	__malloc_zone_start_tab_del(node);
	__malloc_zone_end_tab_del(node);
}
void *malloc(unsigned long long int size)
{
	unsigned long long int size1,size2;
	struct __malloc_zone *zone,*new_zone;
	int hash;
	void *ret;
	if(size==0)
	{
		return 0;
	}
	size1=((size-1>>4)+1<<4)+128;

	zone=__malloc_zone_size_find(size1);
	if(zone==0)
	{
		if(size1<0x8000)
		{
			size2=0x200000;
		}
		else if(size1<0x40000)
		{
			size2=0x1000000;
		}
		else
		{
			size2=size1*8;
		}
		size2=(size2-1>>12)+1<<12;
		if(!(zone=__set_heap_size(__heap_size+size2)))
		{
			size2=size1;
			size2=(size2-1>>12)+1<<12;
			if(!(zone=__set_heap_size(__heap_size+size2)))
			{
				return 0;
			}
		}
		zone->size=size2;
		zone->magic=MALLOC_MAGIC;
	}
	else
	{
		__malloc_zone_del(zone);
	}
	ret=(char *)zone+32;
	if(size1>zone->size)
	{
		__malloc_error();
	}
	if(size1+384<zone->size)
	{
		new_zone=(void *)((char *)zone+size1);
		new_zone->magic=MALLOC_MAGIC;
		new_zone->size=zone->size-size1;
		__malloc_zone_add(new_zone);
		zone->size=size1;
	}
	zone->used=1;
	return ret;
}
void _free(struct __malloc_zone *zone)
{
	struct __malloc_zone *start,*end;
	__malloc_zone_add(zone);
	if(start=__malloc_zone_end_tab_find(zone))
	{
		__malloc_zone_del(zone);
		__malloc_zone_del(start);
		start->size+=zone->size;
		__malloc_zone_add(start);
		zone=start;
	}
	if(end=__malloc_zone_start_tab_find((char *)zone+zone->size))
	{
		__malloc_zone_del(zone);
		__malloc_zone_del(end);
		zone->size+=end->size;
		__malloc_zone_add(zone);
	}
	if((char *)zone+zone->size==__current_brk+__heap_size&&zone->size>=16384)
	{
		__malloc_zone_del(zone);
		__set_heap_size(__heap_size-zone->size);
	}
}
void free(void *ptr)
{
	struct __malloc_zone *zone;
	if(!ptr)
	{
		return;
	}
	zone=(void *)((char *)ptr-32);
	if(zone->used!=1)
	{
		__malloc_error();
	}
	_free(zone);
}
#undef MALLOC_TABLEN
#undef MALLOC_MAGIC
