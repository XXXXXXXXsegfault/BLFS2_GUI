#define MAX_BUFLEN 1024
struct file
{
	struct file *next;
	struct file *prev;
	char *buf;
	unsigned long long int buflen;
} *file_head,*file_end;
struct file_pos
{
	struct file *block;
	unsigned long long int pos;
	unsigned long long int off;
} current_pos,view_pos;
int current_pos_end;
void file_block_insert(struct file *prev,struct file *node)
{
	node->prev=prev;
	if(prev)
	{
		node->next=prev->next;
		if(prev->next)
		{
			prev->next->prev=node;
		}
		prev->next=node;
	}
	else
	{
		node->next=file_head;
		if(file_head)
		{
			file_head->prev=node;
		}
		file_head=node;
	}
	if(prev==file_end)
	{
		file_end=node;
	}
}
void file_block_delete(struct file *node)
{
	if(node->prev)
	{
		node->prev->next=node->next;
	}
	else
	{
		file_head=node->next;
	}
	if(node->next)
	{
		node->next->prev=node->prev;
	}
	else
	{
		file_end=node->prev;
	}
	free(node->buf);
	free(node);
}
int file_load(void)
{
	int fd;
	char buf[MAX_BUFLEN];
	int n;
	int total_read;
	struct file *node;
	fd=open(file_name,0,0);
	if(fd<0)
	{
		return 1;
	}
	total_read=0;
	while((n=read(fd,buf,MAX_BUFLEN))>0)
	{
		node=malloc(sizeof(*node));
		if(node==NULL)
		{
			return 1;
		}
		node->buf=malloc(n);
		if(node->buf==NULL)
		{
			return 1;
		}
		memcpy(node->buf,buf,n);
		node->buflen=n;
		file_block_insert(file_end,node);
		total_read+=n;
		if(total_read>0x10000000)
		{
			return 1;
		}
	}
	close(fd);
	current_pos.block=file_head;
	view_pos.block=file_head;
	if(file_head==NULL)
	{
		current_pos_end=1;
	}
	return 0;
}
int file_pos_move_left(struct file_pos *pos)
{
	if(!pos->block)
	{
		return 0;
	}
	if(pos->pos)
	{
		--pos->pos;
		--pos->off;
		return 1;
	}
	else if(pos->block->prev)
	{
		pos->block=pos->block->prev;
		pos->pos=pos->block->buflen-1;
		--pos->off;
		return 1;
	}
	return 0;
}
int file_pos_move_right(struct file_pos *pos)
{
	if(!pos->block)
	{
		return 0;
	}
	++pos->pos;
	if(pos->pos==pos->block->buflen)
	{
		if(pos->block->next)
		{
			pos->block=pos->block->next;
			pos->pos=0;
			++pos->off;
			return 1;
		}
		else
		{
			--pos->pos;
			return 0;
		}
	}
	++pos->off;
	return 1;
}
int file_getc(struct file_pos *pos)
{
	unsigned char c;
	if(!pos->block)
	{
		return -1;
	}
	c=pos->block->buf[pos->pos];
	return c;
}
int move_next_line(struct file_pos *pos)
{
	int c;
	struct file_pos pos1;
	if(!pos->block)
	{
		return 0;
	}
	memcpy(&pos1,pos,sizeof(pos1));
	while((c=file_getc(pos))!='\n')
	{
		if(!file_pos_move_right(pos))
		{
			c=-1;
			break;
		}
	}
	if(c==-1)
	{
		memcpy(pos,&pos1,sizeof(pos1));
		return 0;
	}
	if(!file_pos_move_right(pos))
	{
		memcpy(pos,&pos1,sizeof(pos1));
		return 0;
	}
	return 1;
}
int move_prev_line(struct file_pos *pos)
{
	int c;
	struct file_pos pos1;
	if(!pos->block)
	{
		return 0;
	}
	memcpy(&pos1,pos,sizeof(pos1));
	if(!file_pos_move_left(pos))
	{
		memcpy(pos,&pos1,sizeof(pos1));
		return 0;
	}
	do
	{
		if(!file_pos_move_left(pos))
		{
			c=-1;
			break;
		}
	}
	while((c=file_getc(pos))!='\n');
	if(c==-1)
	{
		return 1;
	}
	if(!file_pos_move_right(pos))
	{
		memcpy(pos,&pos1,sizeof(pos1));
		return 0;
	}
	return 1;
}
long long int lines_rel(void)
{
	long long int lines;
	struct file_pos pos;
	lines=0;
	memcpy(&pos,&current_pos,sizeof(pos));
	while(pos.off>=view_pos.off)
	{
		if(file_getc(&pos)=='\n')
		{
			++lines;
		}
		if(!file_pos_move_left(&pos))
		{
			break;
		}
	}
	return lines;
}
int lines_remain(int max)
{
	struct file_pos pos;
	int lines;
	int c;
	lines=0;
	memcpy(&pos,&view_pos,sizeof(pos));
	if(!pos.block)
	{
		return 0;
	}
	while(lines<max)
	{
		c=file_getc(&pos);
		if(c=='\n')
		{
			++lines;
		}
		if(!file_pos_move_right(&pos))
		{
			break;
		}
	}
	return lines;
}
int lines_off(int max)
{
	struct file_pos pos;
	int lines;
	int c;
	lines=0;
	memcpy(&pos,&view_pos,sizeof(pos));
	if(!pos.block)
	{
		return 0;
	}
	while(lines<max)
	{
		c=file_getc(&pos);
		if(c=='\n')
		{
			++lines;
		}
		if(!file_pos_move_left(&pos))
		{
			break;
		}
	}
	return lines;
}
int cursor_right(void)
{
	int c;
	c=file_getc(&current_pos);
	if(current_pos_end==1)
	{
		return 0;
	}
	if(c==-1)
	{
		return 0;
	}
	if(!file_pos_move_right(&current_pos))
	{
		current_pos_end=1;
		return 1;
	}
	if(c=='\n')
	{
		if(lines_rel()>CH/2&&lines_remain(CH)==CH)
		{
			move_next_line(&view_pos);
		}
	}
	return 1;
}
int cursor_left(void)
{
	int c;
	c=file_getc(&current_pos);
	if(current_pos_end==1)
	{
		if(c==-1)
		{
			return 0;
		}
		current_pos_end=0;
		return 1;
	}
	if(!file_pos_move_left(&current_pos))
	{
		return 0;
	}
	if(file_getc(&current_pos)=='\n')
	{
		if(lines_rel()<CH/2&&lines_off(1)==1)
		{
			move_prev_line(&view_pos);
		}
	}
	return 1;
}
long long int line_off(struct file_pos *pos)
{
	long long int off;
	struct file_pos pos1;
	int c;
	off=1;
	memcpy(&pos1,pos,sizeof(pos1));
	if(!file_pos_move_left(&pos1))
	{
		return 0;
	}
	while((c=file_getc(&pos1))!='\n')
	{
		++off;
		if(!file_pos_move_left(&pos1))
		{
			break;
		}
	}
	--off;
	return off;
}
long long int line_len(struct file_pos *pos)
{
	struct file_pos pos1;
	int c;
	memcpy(&pos1,pos,sizeof(pos1));
	while((c=file_getc(&pos1))!='\n')
	{
		if(!file_pos_move_right(&pos1))
		{
			break;
		}
	}
	return line_off(&pos1)+1;
}
void current_x_refine(void)
{
	long long int off;
	off=line_off(&current_pos);
	if(off<current_x)
	{
		current_x=off;
	}
	if(off-current_x>CW-1)
	{
		current_x=off-CW+1;
	}
}
int cursor_down(void)
{
	long long int len;
	int c,n;
	struct file_pos pos;
	int old_end;
	len=line_len(&current_pos);
	old_end=current_pos_end;
	n=0;
	memcpy(&pos,&current_pos,sizeof(pos));
	while(len&&(c=file_getc(&current_pos))!=-1)
	{
		if(c=='\n')
		{
			++n;
			if(n==2)
			{
				break;
			}
		}
		if(!cursor_right())
		{
			current_pos_end=old_end;
			memcpy(&current_pos,&pos,sizeof(pos));
			return 0;
		}
		--len;
	}
	return 1;
}
int cursor_up(void)
{
	long long int off;
	long long int off1;
	long long int len;
	struct file_pos pos;
	int old_end;
	off=line_off(&current_pos)+1;
	off1=off;
	old_end=current_pos_end;
	memcpy(&pos,&current_pos,sizeof(pos));
	if(current_pos.block==0)
	{
		return 0;
	}
	while(off)
	{
		if(!cursor_left())
		{
			current_pos_end=old_end;
			memcpy(&current_pos,&pos,sizeof(pos));
			return 0;
		}
		--off;
	}
	len=line_off(&current_pos);
	len-=off1-1;
	if(len<=0)
	{
		return 1;
	}
	while(len)
	{
		cursor_left();
		--len;
	}
	return 1;
}
void addc_end(int c)
{
	struct file *node;
	char *ptr;
	if(!file_end||file_end->buflen==MAX_BUFLEN)
	{
		if(node=malloc(sizeof(*node)))
		{
			if(node->buf=malloc(1))
			{
				node->buf[0]=c;
				node->buflen=1;
				file_block_insert(file_end,node);
			}
			else
			{
				free(node);
			}
		}
	}
	else
	{
		ptr=malloc(file_end->buflen+1);
		if(ptr)
		{
			memcpy(ptr,file_end->buf,file_end->buflen);
			free(file_end->buf);
			file_end->buf=ptr;
			ptr[file_end->buflen]=c;
			++file_end->buflen;
		}
	}
	if(view_pos.block==0)
	{
		view_pos.block=file_head;
		view_pos.off=0;
		view_pos.pos=0;
		current_pos.block=file_head;
		current_pos.off=0;
		current_pos.pos=0;
	}
}
void delc_end(void)
{
	if(!file_end)
	{
		return;
	}
	if(file_end->buflen==1)
	{
		file_block_delete(file_end);
	}
	else
	{
		--file_end->buflen;
	}
	if(file_head==0)
	{
		view_pos.block=0;
		view_pos.pos=0;
		view_pos.off=0;
	}
}
struct file *split_block(struct file *node,long long int off)
{
	struct file *new_node;
	char *ptr;
	if((new_node=malloc(sizeof(*node)))==0)
	{
		return (void *)0;
	}
	if((ptr=malloc(node->buflen-off))==0)
	{
		free(new_node);
		return (void *)0;
	}
	memcpy(ptr,node->buf+off,node->buflen-off);
	new_node->buf=ptr;
	new_node->buflen=node->buflen-off;
	node->buflen=off;
	file_block_insert(node,new_node);
	return node;
}
void addc(int c)
{
	struct file *node,*prev;
	char *ptr;
	if(!current_pos.block)
	{
		return;
	}
	if(current_pos.block->buflen!=MAX_BUFLEN)
	{
		prev=current_pos.block;
		ptr=malloc(prev->buflen+1);
		if(ptr)
		{
			memcpy(ptr,prev->buf,prev->buflen);
			free(prev->buf);
			prev->buf=ptr;
			memmove(prev->buf+current_pos.pos+1,prev->buf+current_pos.pos,prev->buflen-current_pos.pos);
			ptr[current_pos.pos]=c;
			++prev->buflen;
			++current_pos.pos;
			++current_pos.off;
		}
		else
		{
			return;
		}
	}
	else if(current_pos.pos==0)
	{
		if((prev=current_pos.block->prev)&&prev->buflen!=MAX_BUFLEN)
		{
			ptr=malloc(prev->buflen+1);
			if(ptr)
			{
				memcpy(ptr,prev->buf,prev->buflen);
				free(prev->buf);
				prev->buf=ptr;
				ptr[prev->buflen]=c;
				++prev->buflen;
				++current_pos.off;
			}
			else
			{
				return;
			}
		}
		else if(node=malloc(sizeof(*node)))
		{
			if(ptr=malloc(1))
			{
				ptr[0]=c;
				node->buf=ptr;
				node->buflen=1;
				file_block_insert(prev,node);
				if(view_pos.block==current_pos.block)
				{
					view_pos.block=node;
					view_pos.pos=0;
				}
				++current_pos.off;
			}
			else
			{
				free(node);
				return;
			}
		}
		else
		{
			return;
		}

	}
	else
	{
		prev=split_block(current_pos.block,current_pos.pos);
		prev=prev->next;
		ptr=malloc(prev->buflen+1);
		if(ptr)
		{
			memcpy(ptr,prev->buf,prev->buflen);
			free(prev->buf);
			prev->buf=ptr;
			memmove(prev->buf+1,prev->buf,prev->buflen);
			ptr[0]=c;
			++prev->buflen;
			current_pos.block=prev;
			current_pos.pos=1;
			++current_pos.off;
		}
		else
		{
			return;
		}
	}
	if(c=='\n')
	{
		if(lines_rel()>CH/2&&lines_remain(CH)==CH)
		{
			move_next_line(&view_pos);
		}
	}
}
void delc(void)
{
	int c;
	struct file *node,*node1;
	c=file_getc(&current_pos);
	if(c==-1)
	{
		return;
	}
	node=current_pos.block;
	if(node->prev==0&&current_pos.pos==0)
	{
		return;
	}
	if(current_pos.pos!=0)
	{
		node1=split_block(node,current_pos.pos);
		node=node1->next;
	}
	else
	{
		node1=node->prev;
	}
	--node1->buflen;
	current_pos.block=node;
	current_pos.pos=0;
	--current_pos.off;
	if(node1->buflen==0)
	{
		if(view_pos.block==node1)
		{
			view_pos.block=node;
			view_pos.pos=0;
		}
		file_block_delete(node1);
	}
	if(c=='\n')
	{
		if(lines_rel()<CH/2&&lines_off(1)==1)
		{
			move_prev_line(&view_pos);
		}
	}
}
void save_file(void)
{
	struct file *node;
	int fd;
	node=file_head;
	if((fd=open(file_name,01002,0))<0)
	{
		return;
	}
	while(node)
	{
		write(fd,node->buf,node->buflen);
		node=node->next;
	}
	close(fd);
}
