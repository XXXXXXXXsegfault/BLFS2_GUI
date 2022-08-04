/* DO NOT INCLUDE THIS FILE DIRECTLY
 * This file is a template.
 * To use this template, #define some macros and #include it.
 *
 * Macros:
 * RBTREE ROTATE_LEFT: rotate left if defined
 * RBTREE_NODE_ROTATE: node to rotate
 * RBTREE_PARENT_ROTATE: parent of node to rotate, or NULL if root node is to rotate.
 * */
{
#ifdef RBTREE_ROTATE_LEFT
	RBTREE_TYPE *rotate_r,*rotate_rl;
	rotate_r=RBTREE_NODE_ROTATE->RBTREE_RIGHT;
	rotate_rl=rotate_r->RBTREE_LEFT;
	if(RBTREE_PARENT_ROTATE)
	{
		if(RBTREE_PARENT_ROTATE->RBTREE_LEFT==RBTREE_NODE_ROTATE)
		{
			RBTREE_PARENT_ROTATE->RBTREE_LEFT=rotate_r;
		}
		else
		{
			RBTREE_PARENT_ROTATE->RBTREE_RIGHT=rotate_r;
		}
	}
	else
	{
		*root=rotate_r;
	}
	rotate_r->RBTREE_PARENT=RBTREE_PARENT_ROTATE;
	rotate_r->RBTREE_LEFT=RBTREE_NODE_ROTATE;
	RBTREE_NODE_ROTATE->RBTREE_PARENT=rotate_r;
	RBTREE_NODE_ROTATE->RBTREE_RIGHT=rotate_rl;
	if(rotate_rl)
	{
		rotate_rl->RBTREE_PARENT=RBTREE_NODE_ROTATE;
	}
#undef RBTREE_ROTATE_LEFT
#else
	RBTREE_TYPE *rotate_l,*rotate_lr;
	rotate_l=RBTREE_NODE_ROTATE->RBTREE_LEFT;
	rotate_lr=rotate_l->RBTREE_RIGHT;
	if(RBTREE_PARENT_ROTATE)
	{
		if(RBTREE_PARENT_ROTATE->RBTREE_LEFT==RBTREE_NODE_ROTATE)
		{
			RBTREE_PARENT_ROTATE->RBTREE_LEFT=rotate_l;
		}
		else
		{
			RBTREE_PARENT_ROTATE->RBTREE_RIGHT=rotate_l;
		}
	}
	else
	{
		*root=rotate_l;
	}
	rotate_l->RBTREE_PARENT=RBTREE_PARENT_ROTATE;
	rotate_l->RBTREE_RIGHT=RBTREE_NODE_ROTATE;
	RBTREE_NODE_ROTATE->RBTREE_PARENT=rotate_l;
	RBTREE_NODE_ROTATE->RBTREE_LEFT=rotate_lr;
	if(rotate_lr)
	{
		rotate_lr->RBTREE_PARENT=RBTREE_NODE_ROTATE;
	}
#endif
}
#undef RBTREE_NODE_ROTATE
#undef RBTREE_PARENT_ROTATE
