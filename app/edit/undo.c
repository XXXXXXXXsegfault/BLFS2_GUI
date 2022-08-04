#define MAX_UNDOS 1048576
short int op_c_fifo[MAX_UNDOS];
long int op_off_fifo[MAX_UNDOS];
int op_fifo_size,op_fifo_start,op_fifo_x;

void op_push(short int c,long int off)
{
	int x;
	op_fifo_size=op_fifo_x;
	x=op_fifo_start+op_fifo_size;
	if(x>=MAX_UNDOS)
	{
		x-=MAX_UNDOS;
	}
	op_c_fifo[x]=c;
	op_off_fifo[x]=off;
	if(op_fifo_size<MAX_UNDOS)
	{
		++op_fifo_size;
		++op_fifo_x;
	}
	else
	{
		++op_fifo_start;
	}
}
void gotooff(long int off)
{
	long int diff;
	diff=off-current_pos.off;
	if(current_pos_end)
	{
		--diff;
	}
	if(diff<0)
	{
		while(diff)
		{
			cursor_left();
			current_x_refine();
			++diff;
		}
	}
	else
	{
		while(diff)
		{
			cursor_right();
			current_x_refine();
			--diff;
		}
	}
}
void addc_off(long int off,char c)
{
	gotooff(off);
	if(current_pos_end)
	{
		addc_end(c);
		current_pos_end=0;
		cursor_right();
		current_pos_end=1;
	}
	else
	{
		addc(c);
	}
	current_x_refine();
}
void delc_off(long int off)
{
	gotooff(off+1);
	if(current_pos_end)
	{
		current_pos_end=0;
		if(!cursor_left())
		{
			current_pos.block=NULL;
			current_pos.pos=0;
			current_pos.off=0;
		}
		delc_end();
		current_pos_end=1;
	}
	else
	{
		delc();
	}
	current_x_refine();
}
void undo(void)
{
	long int off;
	short int c;
	int x;
	if(op_fifo_x==0)
	{
		return;
	}
	x=op_fifo_start+op_fifo_x-1;
	if(x>=MAX_UNDOS)
	{
		x-=MAX_UNDOS;
	}
	c=op_c_fifo[x];
	off=op_off_fifo[x];
	if(c>>8)
	{
		addc_off(off,c);
	}
	else
	{
		delc_off(off);
	}
	--op_fifo_x;
}
void redo(void)
{
	long int off;
	short int c;
	int x;
	if(op_fifo_x==op_fifo_size)
	{
		return;
	}
	x=op_fifo_start+op_fifo_x;
	if(x>=MAX_UNDOS)
	{
		x-=MAX_UNDOS;
	}
	c=op_c_fifo[x];
	off=op_off_fifo[x];
	if(c>>8)
	{
		delc_off(off);
	}
	else
	{
		addc_off(off,c);
	}
	++op_fifo_x;
}
void c_insert(char c)
{
	if(current_pos_end)
	{
		op_push(c,current_pos.off+1);
		addc_end(c);
		current_pos_end=0;
		cursor_right();
		current_pos_end=1;
	}
	else
	{
		op_push(c,current_pos.off);
		addc(c);
	}
	current_x_refine();
}
void c_delete(void)
{
	int c1;
	struct file_pos pos;
	if(current_pos_end)
	{
		current_pos_end=0;
		c1=file_getc(&current_pos);
		if(c1!=-1)
		{
			op_push(c1|0x100,current_pos.off);
		}
		if(!cursor_left())
		{
			current_pos.block=NULL;
			current_pos.pos=0;
			current_pos.off=0;
		}
		delc_end();
		current_pos_end=1;
	}
	else
	{
		memcpy(&pos,&current_pos,sizeof(pos));
		if(file_pos_move_left(&pos))
		{
			c1=file_getc(&pos);
			op_push(c1|0x100,pos.off);
		}
		delc();
	}
	current_x_refine();
}
