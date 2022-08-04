char input_buf[16384];
struct file
{
	struct stat st;
	struct stat lst;
	short st_valid;
	short lst_valid;
	int executable;
	char name[256];
	struct file *next;
} *files,*file_selected;
unsigned int files_lock;
int current_y,total_y,max_y;
char *current_wd;
char file_removing[256];
char file_chmod[256];
void files_scan(void)
{
	struct file *node,*p,*pp;
	int fd;
	struct DIR db;
	struct dirent *dir;
	unsigned long ino,dev;
	mutex_lock(&files_lock);
	ino=0;
	dev=0xffffffffffffffff;
	if(file_selected)
	{
		if(!file_selected->lst_valid)
		{
			ino=file_selected->lst.ino;
			dev=file_selected->lst.dev;
		}
	}
	file_selected=NULL;
	while(node=files)
	{
		files=node->next;
		free(node);
	}
	total_y=0;
	fd=open(".",0,0);
	if(fd>=0)
	{
		dir_init(fd,&db);
		while(dir=readdir(&db))
		{
			if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
			{
				node=malloc(sizeof(*node));
				if(node)
				{
					node->executable=1;
					if(access(dir->name,1))
					{
						node->executable=0;
					}
					node->lst_valid=lstat(dir->name,&node->lst);
					node->st_valid=stat(dir->name,&node->st);
					strcpy(node->name,dir->name);
					pp=NULL;
					p=files;
					while(p&&strcmp(p->name,node->name)<0)
					{
						pp=p;
						p=p->next;
					}
					node->next=p;
					if(pp)
					{
						pp->next=node;
					}
					else
					{
						files=node;
					}
					if(!node->lst_valid&&node->lst.ino==ino&&node->lst.dev==dev)
					{
						file_selected=node;
					}
					total_y+=24;
				}
			}
		}
		close(fd);
	}
	if(total_y<FILE_H)
	{
		max_y=0;
	}
	else
	{
		max_y=total_y-FILE_H;
	}

	mutex_unlock(&files_lock);
}
void p_file_icon(unsigned int type,int executable,int x,int y)
{
	if(type==STAT_DIR)
	{
		display_icon(icon_dir,x,y);
	}
	else if(type==STAT_FIFO)
	{
		display_icon(icon_pipe,x,y);
	}
	else if(type==STAT_SOCK)
	{
		display_icon(icon_sock,x,y);
	}
	else if(type==STAT_CHR)
	{
		display_icon(icon_chr,x,y);
	}
	else if(type==STAT_BLK)
	{
		display_icon(icon_blk,x,y);
	}
	else if(type==STAT_REG)
	{
		if(executable)
		{
			display_icon(icon_exec,x,y);
		}
		else
		{
			display_icon(icon_file,x,y);
		}
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
void p_size(unsigned long size,int y)
{
	char buf[24];
	size_to_str(size,buf);
	p_str(buf,strlen(buf),224,y,0x0,pbuf,WINW,WINH);
}
void time_to_str(unsigned long t,char *buf)
{
	int year,month,day,hour,minute,second;
	int days_in_month[12];
	int pm;
	year=1970;
	days_in_month[0]=31;
	days_in_month[2]=31;
	days_in_month[3]=30;
	days_in_month[4]=31;
	days_in_month[5]=30;
	days_in_month[6]=31;
	days_in_month[7]=31;
	days_in_month[8]=30;
	days_in_month[9]=31;
	days_in_month[10]=30;
	days_in_month[11]=31;
	while(1)
	{
		if(year%4==0&&year%100||year%400==0)
		{
			days_in_month[1]=29;
			if(t<366*86400)
			{
				break;
			}
			t-=366*86400;
		}
		else
		{
			days_in_month[1]=28;
			if(t<365*86400)
			{
				break;
			}
			t-=365*86400;
		}
		++year;
	}
	month=0;
	while(month<12)
	{
		if(t<days_in_month[month]*86400)
		{
			break;
		}
		t-=days_in_month[month]*86400;
		++month;
	}
	day=t/86400;
	hour=t%86400;
	minute=hour%3600;
	hour/=3600;
	second=minute%60;
	minute/=60;

	buf[0]=0;
	sprinti(buf,month+1,1);
	strcat(buf,"/");
	sprinti(buf,day+1,1);
	strcat(buf,"/");
	sprinti(buf,year,1);
	strcat(buf," ");
	pm=0;
	if(hour>=12)
	{
		hour-=12;
		pm=1;
	}
	if(hour==0)
	{
		hour=12;
	}
	sprinti(buf,hour,2);
	strcat(buf,":");
	sprinti(buf,minute,2);
	strcat(buf,":");
	sprinti(buf,second,2);
	if(pm)
	{
		strcat(buf," PM");
	}
	else
	{
		strcat(buf," AM");
	}
}
void p_time(unsigned long t,int y)
{
	char buf[48];
	time_to_str(t,buf);
	p_str(buf,strlen(buf),408,y,0x0,pbuf,WINW,WINH);
}
void p_file(int y,struct file *node)
{
	unsigned int type;
	int l;
	y+=PATH_H;
	if(y<=-24||y>WINH)
	{
		return;
	}
	if(node==file_selected)
	{
		rect(pbuf,WINW,WINH,0,y,WINW,24,0xff80ff);
	}
	if(node->lst_valid)
	{
		display_icon(icon_broken_link,4,y+4);
	}
	else 
	{
		type=node->lst.mode&0170000;
		if(type==STAT_LNK)
		{
			if(node->st_valid)
			{
				display_icon(icon_broken_link,4,y+4);
			}
			else
			{
				type=node->st.mode&0170000;
				p_file_icon(type,node->executable,4,y+4);
				display_icon(icon_link,4,y+4);
			}
		}
		else
		{
			p_file_icon(type,node->executable,4,y+4);
		}
	}
	l=strlen(node->name);
	if(l>24)
	{
		l=24;
	}
	p_str(node->name,l,24,y+4,0x0,pbuf,WINW,WINH);
	if(!node->st_valid)
	{
		p_size(node->st.size,y+4);
		p_time(node->st.atime,y+4);
	}
}
void p_files(void)
{
	struct file *node;
	int y;
	mutex_lock(&files_lock);
	y=-current_y;
	node=files;
	while(node)
	{
		p_file(y,node);
		node=node->next;
		y+=24;
	}
	mutex_unlock(&files_lock);
}
char *getwd(void)
{
	int fd,pid,status;
	long int l,x;
	char name[32];
	strcpy(name,"cwd");
	sprinti(name,getpid(),1);
	char *buf;
	pid=fork();
	if(pid<0)
	{
		return NULL;
	}
	if(pid==0)
	{
		fd=openat(tmpfd,name,66,0644);
		if(fd>=0)
		{
			char *argv[2];
			argv[0]="pwd";
			argv[1]=NULL;
			flock(fd,2);
			while(dup2(fd,1)<0);
			execv("/bin/pwd",argv);
		}
		exit(1);
	}
	status=0;
	waitpid(pid,&status,0);
	if(status)
	{
		return NULL;
	}
	fd=openat(tmpfd,name,02000002,0);
	if(fd<0)
	{
		return NULL;
	}
	buf=NULL;
	flock(fd,2);
	l=lseek(fd,0,2);
	if(l>0)
	{
		buf=malloc(l);
		if(buf)
		{
			lseek(fd,0,0);
			l=read(fd,buf,l);
			x=0;
			while(x<l-1)
			{
				if(buf[x]=='\n')
				{
					break;
				}
				++x;
			}
			buf[x]=0;
		}
	}
	flock(fd,8);
	close(fd);
	return buf;
}
void reload_cwd(void)
{
	mutex_lock(&files_lock);
	free(current_wd);
	current_wd=getwd();
	mutex_unlock(&files_lock);
}
void set_copy_path(int m)
{
	int fd;
	mutex_lock(&files_lock);
	if(current_wd==NULL||file_selected==NULL)
	{
		mutex_unlock(&files_lock);
		return;
	}
	fd=openat(tmpfd,"copy_src",66|02000000,0644);
	if(fd<0)
	{
		mutex_unlock(&files_lock);
		return;
	}
	flock(fd,2);
	if(m)
	{
		write(fd,"Move:",5);
	}
	else
	{
		write(fd,"Copy:",5);
	}
	write(fd,current_wd,strlen(current_wd));
	write(fd,"/",1);
	write(fd,file_selected->name,strlen(file_selected->name));
	write(fd,"\n",1);
	flock(fd,8);
	close(fd);
	mutex_unlock(&files_lock);
}
void do_paste_file(char *new_name)
{
	int fd,pid;
	unsigned long int l,x;
	int move;
	char *buf;
	pid=fork();
	if(pid<0)
	{
		return;
	}
	if(pid==0)
	{
		fd=openat(tmpfd,"copy_src",0,0);
		if(fd>=0)
		{
			flock(fd,2);
			l=lseek(fd,0,2);
			lseek(fd,0,0);
			buf=mmap(0,l,3,0x22,-1,0);
			if(valid(buf))
			{
				read(fd,buf,l);
				move=0;
				if(!memcmp(buf,"Move:",5))
				{
					move=1;
				}
				x=5;
				while(x<l-1)
				{
					if(buf[x]=='\n')
					{
						break;
					}
					++x;
				}
				buf[x]=0;
				if(move)
				{
					exit(renameatl(AT_FDCWD,buf+5,AT_FDCWD,new_name));
				}
				else
				{
					char *argv[4];
					argv[3]=NULL;
					argv[0]="copy";
					argv[1]=buf+5;
					argv[2]=new_name;
					execv("/bin/copy",argv);
				}
			}
		}
		exit(1);
	}
	waitpid(pid,NULL,0);
}
int T_paste_file(void *arg)
{
	char *new_name;
	new_name=arg;
	do_paste_file(new_name);
	return 0;
}
int T_exec_file(void *arg)
{
	int pid;
	char *name;
	char *argv[2];
	name=arg;
	pid=fork();
	if(pid<0)
	{
		free(name);
		return 1;
	}
	if(pid==0)
	{
		argv[1]=NULL;
		argv[0]=name;
		execv(name,argv);
		exit(1);
	}
	waitpid(pid,NULL,0);
	free(name);
	return 0;
}
void exec_file(void)
{
	char *name;
	mutex_lock(&files_lock);
	if(file_selected)
	{
		name=malloc(strlen(file_selected->name)+1);
		if(name)
		{
			strcpy(name,file_selected->name);
			if(create_lwp(4096,T_exec_file,name)<0)
			{
				free(name);
			}
		}
	}
	mutex_unlock(&files_lock);
}
void remove_file_msg(void)
{
	mutex_lock(&files_lock);
	if(file_selected)
	{
		memcpy(file_removing,file_selected->name,256);
		strcpy(input_buf,"Remove ");
		if(file_selected->lst_valid||(file_selected->lst.mode&0170000)==STAT_REG)
		{
			strcat(input_buf,"file");
		}
		else if((file_selected->lst.mode&0170000)==STAT_DIR)
		{
			strcat(input_buf,"directory (AND ITS CONTENTS)");
		}
		else if((file_selected->lst.mode&0170000)==STAT_LNK)
		{
			strcat(input_buf,"link");
		}
		else if((file_selected->lst.mode&0170000)==STAT_FIFO)
		{
			strcat(input_buf,"pipe");
		}
		else if((file_selected->lst.mode&0170000)==STAT_SOCK)
		{
			strcat(input_buf,"socket");
		}
		else if((file_selected->lst.mode&0170000)==STAT_CHR)
		{
			strcat(input_buf,"character device");
		}
		else if((file_selected->lst.mode&0170000)==STAT_BLK)
		{
			strcat(input_buf,"block device");
		}
		else
		{
			strcat(input_buf,"file");
		}
		strcat(input_buf," \'");
		strcat(input_buf,file_removing);
		strcat(input_buf,"\'\?");
	}
	else
	{
		file_removing[0]=0;
	}
	mutex_unlock(&files_lock);
}
int T_remove_file(void *arg)
{
	int pid;
	pid=fork();
	if(pid<0)
	{
		return 1;
	}
	if(pid==0)
	{
		char *argv[3];
		argv[0]="remove";
		argv[1]=file_removing;
		argv[2]=NULL;
		execv("/bin/remove",argv);
		exit(1);
	}
	waitpid(pid,NULL,0);
}
void do_remove_file(void)
{
	create_lwp(4096,T_remove_file,NULL);
}
void file_chmod_msg(void)
{
	mutex_lock(&files_lock);
	if(file_selected)
	{
		memcpy(file_chmod,file_selected->name,256);
	}
	else
	{
		file_chmod[0]=0;
	}
	mutex_unlock(&files_lock);
}
