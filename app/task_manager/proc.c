struct proc
{
	unsigned int cpu_usage; // 0.1%
	int pid;
	unsigned long memory;
	unsigned long cpu_time;
	char name[16];
	struct proc *next;
	struct proc *paint_next;
} *proc_list[127],*proc_old[127];
struct proc *proc_paint_list;
unsigned int proc_lock;
unsigned long total_cpu_time,cpu_time_diff;
unsigned long mem_total,mem_avail;
int current_y,sorting_mode;
int selected_pid;
int num_proc;
#define HASH(pid) ((pid)%127)

int skip_entry(char *buf,int index)
{
	while(buf[index]&&buf[index]!=32)
	{
		++index;
	}
	while(buf[index]==32)
	{
		++index;
	}
	return index;
}
unsigned long int get_old_cpu_time(int pid)
{
	struct proc *proc;
	proc=proc_old[HASH(pid)];
	while(proc)
	{
		if(proc->pid==pid)
		{
			return proc->cpu_time;
		}
		proc=proc->next;
	}
	return 0;
}
struct proc *get_proc_info(int pid)
{
	char name[48];
	int fd,n,i;
	struct proc *proc;
	static char buf[2048];
	unsigned long cpu_time,val;
	strcpy(name,"/proc/");
	sprinti(name,pid,1);
	strcat(name,"/stat");
	fd=open(name,0,0);
	if(fd<0)
	{
		return NULL;
	}
	proc=malloc(sizeof(*proc));
	if(proc==NULL)
	{
		close(fd);
		return NULL;
	}
	memset(proc,0,sizeof(proc));
	n=read(fd,buf,2047);
	close(fd);
	if(n<=0)
	{
		free(proc);
		return NULL;
	}
	buf[n]=0;
	while(n)
	{
		--n;
		if(buf[n]==')')
		{
			break;
		}
	}
	i=skip_entry(buf,0);
	if(n-i-1>15)
	{
		memcpy(proc->name,buf+i+1,15);
	}
	else if(n>i+1)
	{
		memcpy(proc->name,buf+i+1,n-i-1);
	}
	n+=2;
	i=11;
	while(i)
	{
		n=skip_entry(buf,n);
		--i;
	}
	i=4;
	cpu_time=0;
	while(i)
	{
		val=0;
		sinputi(buf+n,&val);
		cpu_time+=val;
		--i;
		n=skip_entry(buf,n);
	}
	proc->pid=pid;
	proc->cpu_time=cpu_time;
	i=5;
	while(i)
	{
		n=skip_entry(buf,n);
		--i;
	}
	val=0;
	sinputi(buf+n,&val);
	proc->memory=val;
	cpu_time-=get_old_cpu_time(pid);
	if(cpu_time_diff)
	{
		proc->cpu_usage=cpu_time*1000/cpu_time_diff;
	}
	return proc;
}
void read_system_info(void)
{
	int fd;
	int n,i;
	unsigned long val;
	static char buf[1024];
	fd=open("/proc/stat",0,0);
	if(fd>=0)
	{
		n=read(fd,buf,1023);
		if(n>0)
		{
			buf[n]=0;
			cpu_time_diff=total_cpu_time;
			total_cpu_time=0;
			i=5;
			n=10;
			while(n)
			{
				val=0;
				sinputi(buf+i,&val);
				total_cpu_time+=val;
				i=skip_entry(buf,i);
				--n;
			}
			cpu_time_diff=total_cpu_time-cpu_time_diff;
		}
		close(fd);
	}
	fd=open("/proc/meminfo",0,0);
	if(fd>=0)
	{
		n=read(fd,buf,1023);
		if(n>0)
		{
			buf[n]=0;
			i=skip_entry(buf,0);
			sinputi(buf+i,&mem_total);
			n=4;
			while(n)
			{
				i=skip_entry(buf,i);
				--n;
			}
			sinputi(buf+i,&mem_avail);
		}
		close(fd);
	}
}
void sort_proc(void);
void scan_proc(void)
{
	struct proc *node;
	int fd;
	struct DIR db;
	struct dirent *dir;
	long int pid;
	int i;
	long int spid;

	mutex_lock(&proc_lock);
	num_proc=0;
	spid=selected_pid;
	selected_pid=0;
	read_system_info();
	memcpy(proc_old,proc_list,sizeof(proc_list));
	memset(proc_list,0,sizeof(proc_list));
	fd=open("/proc",0,0);
	if(fd>=0)
	{
		dir_init(fd,&db);
		while(dir=readdir(&db))
		{
			if(dir->name[0]>='1'&&dir->name[0]<='9')
			{
				pid=0;
				sinputi(dir->name,&pid);
				node=get_proc_info(pid);
				if(node)
				{
					if(node->pid==spid)
					{
						selected_pid=node->pid;
					}
					if(!node->memory)
					{
						free(node);
					}
					else
					{
						i=HASH(pid);
						node->next=proc_list[i];
						proc_list[i]=node;
						++num_proc;
					}
				}
			}
		}
		close(fd);
	}
	i=0;
	while(i<127)
	{
		while(node=proc_old[i])
		{
			proc_old[i]=node->next;
			free(node);
		}
		++i;
	}
	sort_proc();
	mutex_unlock(&proc_lock);
}
int proc_cmp(struct proc *p1,struct proc *p2)
{
	int val;
	if(sorting_mode==0)
	{
		val=strcmp(p1->name,p2->name);
		if(val)
		{
			return val;
		}
	}
	if(sorting_mode==1)
	{
		val=strcmp(p2->name,p1->name);
		if(val)
		{
			return val;
		}
	}
	if(sorting_mode==2)
	{
		if(val=p1->cpu_usage-p2->cpu_usage)
		{
			return val;
		}
	}
	if(sorting_mode==3)
	{
		if(val=p2->cpu_usage-p1->cpu_usage)
		{
			return val;
		}
	}
	if(sorting_mode==4)
	{
		if(val=p1->memory-p2->memory)
		{
			return val;
		}
	}
	if(sorting_mode==5)
	{
		if(val=p2->memory-p1->memory)
		{
			return val;
		}
	}
	return p1->pid-p2->pid;
}
void proc_paint_insert(struct proc *proc)
{
	struct proc *p,*node;
	p=NULL;
	node=proc_paint_list;
	while(node)
	{
		if(proc_cmp(proc,node)<0)
		{
			break;
		}
		p=node;
		node=node->paint_next;
	}
	proc->paint_next=node;
	if(p)
	{
		p->paint_next=proc;
	}
	else
	{
		proc_paint_list=proc;
	}
}
void size_to_str(unsigned long size,char *buf)
{
	buf[0]=0;
	if(size<0x400)
	{
		sprinti(buf,size,1);
		strcat(buf," Bytes");
	}
	else if(size<0x100000)
	{
		sprinti(buf,size>>10,1);
		strcat(buf,".");
		sprinti(buf,(size&0x3ff)*100/0x400,2);
		strcat(buf," KB");
	}
	else if(size<0x40000000)
	{
		sprinti(buf,size>>20,1);
		strcat(buf,".");
		sprinti(buf,(size&0xfffff)*100/0x100000,2);
		strcat(buf," MB");
	}
	else
	{
		sprinti(buf,size>>30,1);
		strcat(buf,".");
		sprinti(buf,(size&0x3fffffff)*100/0x40000000,2);
		strcat(buf," GB");
	}
}
void p_proc(struct proc *proc,int y)
{
	char buf[20];
	int n;
	if(y<=-24||y>WINH)
	{
		return;
	}
	y+=56;
	if(proc->pid==selected_pid)
	{
		rect(pbuf,WINW,WINH,0,y,WINW,24,0x0080ff);
	}
	y+=4;
	buf[0]=0;
	sprinti(buf,proc->pid,1);
	p_str(buf,strlen(buf),1,y,0x0,pbuf,WINW,WINH);
	p_str(proc->name,strlen(proc->name),88+1,y,0x0,pbuf,WINW,WINH);
	buf[0]=0;
	sprinti(buf,proc->cpu_usage/10,1);
	strcat(buf,".");
	sprinti(buf,proc->cpu_usage%10,1);
	strcat(buf,"%");
	p_str(buf,strlen(buf),88+136+1,y,0x0,pbuf,WINW,WINH);
	size_to_str(proc->memory,buf);
	p_str(buf,strlen(buf),88+136+64+1,y,0x0,pbuf,WINW,WINH);
}
void sort_proc(void)
{
	int i;
	struct proc *node;
	proc_paint_list=NULL;
	i=0;
	while(i<127)
	{
		node=proc_list[i];
		while(node)
		{
			proc_paint_insert(node);
			node=node->next;
		}
		++i;
	}
}
void paint_proc(void)
{
	int i;
	struct proc *node;
	int y;
	static char buf[256];
	char buf2[32];
	mutex_lock(&proc_lock);
	if(current_y>num_proc*24-(WINH-25-32))
	{
		current_y=num_proc*24-(WINH-25-32);
	}
	if(current_y<0)
	{
		current_y=0;
	}
	sort_proc();
	node=proc_paint_list;
	y=-current_y;
	while(node)
	{
		p_proc(node,y);
		y+=24;
		node=node->paint_next;
	}
	rect(pbuf,WINW,WINH,0,24,WINW,32,0xc0c0c0);
	strcpy(buf,"Total Memory: ");
	size_to_str(mem_total*1024,buf2);
	strcat(buf,buf2);
	strcat(buf,"   Available Memory: ");
	size_to_str(mem_avail*1024,buf2);
	strcat(buf,buf2);
	strcat(buf,"   Memory Usage: ");
	i=(mem_total-mem_avail)*1000/mem_total;
	sprinti(buf,i/10,1);
	strcat(buf,".");
	sprinti(buf,i%10,1);
	strcat(buf,"%");
	p_str(buf,strlen(buf),1,24,0xff0000,pbuf,WINW,WINH);

	p_str("PID",3,1,40,0x0,pbuf,WINW,WINH);
	if(sorting_mode==0)
	{
		p_str("Name",4,1+88,40,0xff0000,pbuf,WINW,WINH);
	}
	else if(sorting_mode==1)
	{
		p_str("Name",4,1+88,40,0xff00,pbuf,WINW,WINH);
	}
	else
	{
		p_str("Name",4,1+88,40,0xff,pbuf,WINW,WINH);
	}

	if(sorting_mode==2)
	{
		p_str("CPU",3,1+88+136,40,0xff0000,pbuf,WINW,WINH);
	}
	else if(sorting_mode==3)
	{
		p_str("CPU",3,1+88+136,40,0xff00,pbuf,WINW,WINH);
	}
	else
	{
		p_str("CPU",3,1+88+136,40,0xff,pbuf,WINW,WINH);
	}
	
	if(sorting_mode==4)
	{
		p_str("Memory",6,1+88+136+64,40,0xff0000,pbuf,WINW,WINH);
	}
	else if(sorting_mode==5)
	{
		p_str("Memory",6,1+88+136+64,40,0xff00,pbuf,WINW,WINH);
	}
	else
	{
		p_str("Memory",6,1+88+136+64,40,0xff,pbuf,WINW,WINH);
	}

	mutex_unlock(&proc_lock);	
}
