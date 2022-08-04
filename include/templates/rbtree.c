/* This file is a template.
 * To use this template, #define some macros and #include it.
 *
 * Macros:
 * RBTREE_TYPE -- RBTREE structure type
 * RBTREE_CMP(node1, node2) -- function to compare two nodes, returns 1 if node1 > node2, 0 if node1 <= node2
 * RBTREE_COLOR -- COLOR field of RBTREE structure, at least 1 bit
 * RBTREE_LEFT -- LEFT_POINTER field of RBTREE structure
 * RBTREE_RIGHT -- RIGHT_POINTER field of RBTREE structure
 * RBTREE_PARENT -- PARENT_POINTER field of RBTREE structure
 * RBTREE_INSERT -- name of function to insert a node into RBTREE
 * RBTREE_DELETE -- name of function to delete a node from RBTREE
 * RBTREE_FIND -- name of function to find a node in RBTREE
 * RBTREE_NEXT -- name of function to locate "next" node
 * RBTREE_PREV -- name of function to locate "previous" node
 * */
#define RBTREE_RED 0
#define RBTREE_BLACK 1
#ifdef RBTREE_INSERT
void RBTREE_INSERT(RBTREE_TYPE **root,RBTREE_TYPE *node)
{
	RBTREE_TYPE *p,*pp,*pa,*pr,*insert_pos;
	int if_left;
	if(*root==0)
	{
		*root=node;
		node->RBTREE_COLOR=RBTREE_BLACK;
		node->RBTREE_LEFT=0;
		node->RBTREE_RIGHT=0;
		node->RBTREE_PARENT=0;
		return;
	}
	p=*root;
	do
	{
		insert_pos=p;
		if(RBTREE_CMP(node,insert_pos))
		{
			p=insert_pos->RBTREE_RIGHT;
			if_left=0;
		}
		else
		{
			p=insert_pos->RBTREE_LEFT;
			if_left=1;
		}
	}
	while(p);
	if(if_left)
	{
		insert_pos->RBTREE_LEFT=node;
	}
	else
	{
		insert_pos->RBTREE_RIGHT=node;
	}
	node->RBTREE_COLOR=RBTREE_RED;
	node->RBTREE_LEFT=0;
	node->RBTREE_RIGHT=0;
	node->RBTREE_PARENT=insert_pos;
	p=insert_pos;
	if(p==0||p->RBTREE_COLOR==RBTREE_BLACK)
	{
		return;
	}
	pp=insert_pos->RBTREE_PARENT;
	while(1)
	{
		if(pp->RBTREE_LEFT==p)
		{
			pa=pp->RBTREE_RIGHT;
			if(pa&&pa->RBTREE_COLOR==RBTREE_RED)
			{
				pa->RBTREE_COLOR=RBTREE_BLACK;
				p->RBTREE_COLOR=RBTREE_BLACK;
				pp->RBTREE_COLOR=RBTREE_RED;
				node=pp;
				p=node->RBTREE_PARENT;
				if(p==0||p->RBTREE_COLOR==RBTREE_BLACK)
				{
					break;
				}
				pp=p->RBTREE_PARENT;
			}
			else
			{
				if(p->RBTREE_RIGHT==node)
				{
#define RBTREE_ROTATE_LEFT
#define RBTREE_NODE_ROTATE p
#define RBTREE_PARENT_ROTATE pp
#include "rbtree_rotate.c"
					p=node;
					node=p->RBTREE_LEFT;
				}
				pp->RBTREE_COLOR=RBTREE_RED;
				p->RBTREE_COLOR=RBTREE_BLACK;
				pr=pp->RBTREE_PARENT;
#define RBTREE_NODE_ROTATE pp
#define RBTREE_PARENT_ROTATE pr
#include "rbtree_rotate.c"
				break;
			}
		}
		else
		{
			pa=pp->RBTREE_LEFT;
			if(pa&&pa->RBTREE_COLOR==RBTREE_RED)
			{
				pa->RBTREE_COLOR=RBTREE_BLACK;
				p->RBTREE_COLOR=RBTREE_BLACK;
				pp->RBTREE_COLOR=RBTREE_RED;
				node=pp;
				p=node->RBTREE_PARENT;
				if(p==0||p->RBTREE_COLOR==RBTREE_BLACK)
				{
					break;
				}
				pp=p->RBTREE_PARENT;
			}
			else
			{
				if(p->RBTREE_LEFT==node)
				{
#define RBTREE_NODE_ROTATE p
#define RBTREE_PARENT_ROTATE pp
#include "rbtree_rotate.c"
					p=node;
					node=p->RBTREE_RIGHT;
				}
				pp->RBTREE_COLOR=RBTREE_RED;
				p->RBTREE_COLOR=RBTREE_BLACK;
				pr=pp->RBTREE_PARENT;
#define RBTREE_ROTATE_LEFT
#define RBTREE_NODE_ROTATE pp
#define RBTREE_PARENT_ROTATE pr
#include "rbtree_rotate.c"
				break;
			}
		}
	}
	(*root)->RBTREE_COLOR=RBTREE_BLACK;
}
#undef RBTREE_INSERT
#endif

#ifdef RBTREE_DELETE
RBTREE_TYPE *RBTREE_DELETE(RBTREE_TYPE **root,RBTREE_TYPE *node)
{
	RBTREE_TYPE *n,*p,*p1,*p2,*n1;
	int if_left,color;
	n=*root;
	while(1)
	{
		if(n==0)
		{
			return 0;
		}
		if(RBTREE_CMP(node,n))
		{
			n=n->RBTREE_RIGHT;
		}
		else if(RBTREE_CMP(n,node))
		{
			n=n->RBTREE_LEFT;
		}
		else
		{
			break;
		}
	}
	n1=n;
	if(n->RBTREE_LEFT==0)
	{
		if(n->RBTREE_RIGHT==0)
		{
			p=n->RBTREE_PARENT;
			if(p)
			{
				if(p->RBTREE_LEFT==n)
				{
					p->RBTREE_LEFT=0;
					if_left=1;
				}
				else
				{
					p->RBTREE_RIGHT=0;
					if_left=0;
				}
			}
			else
			{
				*root=0;
				return n1;
			}
		}
		else
		{
			p=n->RBTREE_PARENT;
			if(p)
			{
				if(p->RBTREE_LEFT==n)
				{
					p->RBTREE_LEFT=n->RBTREE_RIGHT;
					if_left=1;
				}
				else
				{
					p->RBTREE_RIGHT=n->RBTREE_RIGHT;
					if_left=0;
				}
				n->RBTREE_RIGHT->RBTREE_PARENT=p;
			}
			else
			{
				*root=n->RBTREE_RIGHT;
				n->RBTREE_RIGHT->RBTREE_PARENT=0;
				(*root)->RBTREE_COLOR=RBTREE_BLACK;
				return n1;
			}
		}
		color=n->RBTREE_COLOR;
	}
	else if(n->RBTREE_RIGHT==0)
	{
		p=n->RBTREE_PARENT;
		if(p)
		{
			if(p->RBTREE_LEFT==n)
			{
				p->RBTREE_LEFT=n->RBTREE_LEFT;
				if_left=1;
			}
			else
			{
				p->RBTREE_RIGHT=n->RBTREE_LEFT;
				if_left=0;
			}
			n->RBTREE_LEFT->RBTREE_PARENT=p;
		}
		else
		{
			*root=n->RBTREE_LEFT;
			n->RBTREE_LEFT->RBTREE_PARENT=0;
			(*root)->RBTREE_COLOR=RBTREE_BLACK;
			return n1;
		}
		color=n->RBTREE_COLOR;
	}
	else
	{
		p1=n->RBTREE_RIGHT;
		while(p1->RBTREE_LEFT)
		{
			p1=p1->RBTREE_LEFT;
		}
		if(p1==n->RBTREE_RIGHT)
		{
			p=n->RBTREE_PARENT;
			if(p)
			{
				if(p->RBTREE_LEFT==n)
				{
					p->RBTREE_LEFT=p1;
				}
				else
				{
					p->RBTREE_RIGHT=p1;
				}
			}
			else
			{
				*root=p1;
			}
			p1->RBTREE_PARENT=p;
			p1->RBTREE_LEFT=n->RBTREE_LEFT;
			n->RBTREE_LEFT->RBTREE_PARENT=p1;
			color=p1->RBTREE_COLOR;
			p1->RBTREE_COLOR=n->RBTREE_COLOR;
			p=p1;
			if_left=0;
		}
		else
		{
			p=p1->RBTREE_PARENT;
			if(p->RBTREE_LEFT==p1)
			{
				p->RBTREE_LEFT=p1->RBTREE_RIGHT;
			}
			else
			{
				p->RBTREE_RIGHT=p1->RBTREE_RIGHT;
			}
			if(p1->RBTREE_RIGHT)
			{
				p1->RBTREE_RIGHT->RBTREE_PARENT=p;
			}
			p1->RBTREE_LEFT=n->RBTREE_LEFT;
			p1->RBTREE_RIGHT=n->RBTREE_RIGHT;
			p1->RBTREE_PARENT=n->RBTREE_PARENT;
			color=p1->RBTREE_COLOR;
			p1->RBTREE_COLOR=n->RBTREE_COLOR;
			p2=n->RBTREE_PARENT;
			if(p2)
			{
				if(p2->RBTREE_LEFT==n)
				{
					p2->RBTREE_LEFT=p1;
				}
				else
				{
					p2->RBTREE_RIGHT=p1;
				}
			}
			else
			{
				*root=p1;
			}
			if(n->RBTREE_LEFT)
			{
				n->RBTREE_LEFT->RBTREE_PARENT=p1;
			}
			if(n->RBTREE_RIGHT)
			{
				n->RBTREE_RIGHT->RBTREE_PARENT=p1;
			}
			if_left=1;
		}
	}
	if(color==RBTREE_RED)
	{
		return n1;
	}
	n=p;
	while(1)
	{
		if(if_left)
		{
			if(n->RBTREE_LEFT&&n->RBTREE_LEFT->RBTREE_COLOR==RBTREE_RED)
			{
				n->RBTREE_LEFT->RBTREE_COLOR=RBTREE_BLACK;
				break;
			}
			p=n->RBTREE_RIGHT;
		}
		else
		{
			if(n->RBTREE_RIGHT&&n->RBTREE_RIGHT->RBTREE_COLOR==RBTREE_RED)
			{
				n->RBTREE_RIGHT->RBTREE_COLOR=RBTREE_BLACK;
				break;
			}
			p=n->RBTREE_LEFT;
		}
		if(if_left)
		{
			if(p&&p->RBTREE_COLOR==RBTREE_RED)
			{
				p->RBTREE_COLOR=RBTREE_BLACK;
				n->RBTREE_COLOR=RBTREE_RED;
				p1=n->RBTREE_PARENT;
#define RBTREE_ROTATE_LEFT
#define RBTREE_NODE_ROTATE n
#define RBTREE_PARENT_ROTATE p1
#include "rbtree_rotate.c"
			}
			else
			{
				if(p&&p->RBTREE_RIGHT&&p->RBTREE_RIGHT->RBTREE_COLOR==RBTREE_RED)
				{
					p->RBTREE_COLOR=n->RBTREE_COLOR;
					n->RBTREE_COLOR=RBTREE_BLACK;
					p->RBTREE_RIGHT->RBTREE_COLOR=RBTREE_BLACK;
					p1=n->RBTREE_PARENT;
#define RBTREE_ROTATE_LEFT
#define RBTREE_NODE_ROTATE n
#define RBTREE_PARENT_ROTATE p1
#include "rbtree_rotate.c"
					break;
				}
				else if(p&&p->RBTREE_LEFT&&p->RBTREE_LEFT->RBTREE_COLOR==RBTREE_RED)
				{
					p->RBTREE_LEFT->RBTREE_COLOR=RBTREE_BLACK;
					p->RBTREE_COLOR=RBTREE_RED;
#define RBTREE_NODE_ROTATE p
#define RBTREE_PARENT_ROTATE n
#include "rbtree_rotate.c"
				}
				else
				{
					p->RBTREE_COLOR=RBTREE_RED;
					p=n;
					n=n->RBTREE_PARENT;
					if(n==0)
					{
						break;
					}
					if(n->RBTREE_RIGHT==p)
					{
						if_left=0;
					}
				}
			}
		}
		else
		{
			if(p&&p->RBTREE_COLOR==RBTREE_RED)
			{
				p->RBTREE_COLOR=RBTREE_BLACK;
				n->RBTREE_COLOR=RBTREE_RED;
				p1=n->RBTREE_PARENT;
#define RBTREE_NODE_ROTATE n
#define RBTREE_PARENT_ROTATE p1
#include "rbtree_rotate.c"
			}
			else
			{
				if(p&&p->RBTREE_LEFT&&p->RBTREE_LEFT->RBTREE_COLOR==RBTREE_RED)
				{
					p->RBTREE_COLOR=n->RBTREE_COLOR;
					n->RBTREE_COLOR=RBTREE_BLACK;
					p->RBTREE_LEFT->RBTREE_COLOR=RBTREE_BLACK;
					p1=n->RBTREE_PARENT;
#define RBTREE_NODE_ROTATE n
#define RBTREE_PARENT_ROTATE p1
#include "rbtree_rotate.c"
					break;
				}
				else if(p&&p->RBTREE_RIGHT&&p->RBTREE_RIGHT->RBTREE_COLOR==RBTREE_RED)
				{
					p->RBTREE_RIGHT->RBTREE_COLOR=RBTREE_BLACK;
					p->RBTREE_COLOR=RBTREE_RED;
#define RBTREE_ROTATE_LEFT
#define RBTREE_NODE_ROTATE p
#define RBTREE_PARENT_ROTATE n
#include "rbtree_rotate.c"
				}
				else
				{
					p->RBTREE_COLOR=RBTREE_RED;
					p=n;
					n=n->RBTREE_PARENT;
					if(n==0)
					{
						break;
					}
					if(n->RBTREE_LEFT==p)
					{
						if_left=1;
					}
				}
			}
		}
	}
	(*root)->RBTREE_COLOR=RBTREE_BLACK;
	return n1;
}
#undef RBTREE_DELETE
#endif

#ifdef RBTREE_FIND
RBTREE_TYPE *RBTREE_FIND(RBTREE_TYPE *root,RBTREE_TYPE *node)
{
	RBTREE_TYPE *n;
	n=root;
	while(1)
	{
		if(n==0)
		{
			return 0;
		}
		if(RBTREE_CMP(node,n))
		{
			n=n->RBTREE_RIGHT;
		}
		else if(RBTREE_CMP(n,node))
		{
			n=n->RBTREE_LEFT;
		}
		else
		{
			return n;
		}
	}
}
#undef RBTREE_FIND
#endif

#ifdef RBTREE_NEXT
RBTREE_TYPE *RBTREE_NEXT(RBTREE_TYPE *node)
{
	RBTREE_TYPE *n;
	if(node->RBTREE_RIGHT)
	{
		n=node->RBTREE_RIGHT;
		while(n->RBTREE_LEFT)
		{
			n=n->RBTREE_LEFT;
		}
		return n;
	}
	else
	{
		n=node->RBTREE_PARENT;
		while(n&&n->RBTREE_RIGHT==node)
		{
			node=n;
			n=n->RBTREE_PARENT;
		}
		if(!n)
		{
			return (void *)0;
		}
		return n;
	}
}
#undef RBTREE_NEXT
#endif

#ifdef RBTREE_PREV
RBTREE_TYPE *RBTREE_PREV(RBTREE_TYPE *node)
{
	RBTREE_TYPE *n;
	if(node->RBTREE_LEFT)
	{
		n=node->RBTREE_LEFT;
		while(n->RBTREE_RIGHT)
		{
			n=n->RBTREE_RIGHT;
		}
		return n;
	}
	else
	{
		n=node->RBTREE_PARENT;
		while(n&&n->RBTREE_LEFT==node)
		{
			node=n;
			n=n->RBTREE_PARENT;
		}
		if(!n)
		{
			return (void *)0;
		}
		return n;
	}
}
#undef RBTREE_PREV
#endif

#undef RBTREE_TYPE
#undef RBTREE_CMP
#undef RBTREE_COLOR
#undef RBTREE_LEFT
#undef RBTREE_RIGHT
#undef RBTREE_PARENT
#undef RBTREE_RED
#undef RBTREE_BLACK
