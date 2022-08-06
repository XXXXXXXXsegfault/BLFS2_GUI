#include "../../include/malloc.c"
#include "../../include/mem.c"
#include "../../include/dirent.c"
#include "../../include/stat.c"
char out_buf[16384];
int msgfd;
int _insmod(char *path)
{
	int fd;
	int failed;
	fd=open(path,0,0);
	if(fd<0)
	{
		return 1;
	}
	failed=finit_module(fd,"",0);
	close(fd);
	return failed;
}
int insmod(char *path)
{
	char buf[1280];
	int l;
	int ret;
	l=strlen(path);
	if(l>1023)
	{
		l=1023;
	}
	if(ret=_insmod(path))
	{
		if(ret==-17)
		{
			l=0;
			ret=0;
		}
		else
		{
			memcpy(buf,"insmod FAILED: ",15);
			memcpy(buf+15,path,l);
			l+=15;
			buf[l]='\n';
		}
	}
	else
	{
		memcpy(buf,"insmod ",7);
		memcpy(buf+7,path,l);
		l+=7;
		buf[l]='\n';
	}
	if(l)
	{
		write(msgfd,buf,l+1);
		write(1,buf,l+1);
	}
	return ret;
}
struct mod
{
	struct mod *list_next;
	struct mod *tab_next;
	int used;
	short dev;
	short load;
	char *name;
	char *path;
} *modtab[65537],*modlist;
char *str_dup(char *str)
{
	long int l;
	char *new_str;
	l=strlen(str);
	new_str=malloc(l+1);
	if(!new_str)
	{
		return NULL;
	}
	memcpy(new_str,str,l+1);
	return new_str;
}
unsigned int name_hash(char *name)
{
	unsigned int hash;
	char c;
	hash=0;
	while(c=*name)
	{
		if(c=='.'||c==',')
		{
			break;
		}
		if(c=='-')
		{
			c='_';
		}
		hash=(hash>>5|hash<<19)+c;
		++name;
	}
	return hash%65537;
}
int namecmp(char *name1,char *name2)
{
	char c;
	while(c=*name1)
	{
		if(c=='.'||c==',')
		{
			break;
		}
		if(c=='-'||c=='_')
		{
			if(*name2!='-'&&*name2!='_')
			{
				return 1;
			}
		}
		else
		{
			if(c!=*name2)
			{
				return 1;
			}
		}
		++name1;
		++name2;
	}
	if(*name2&&*name2!='.'&&*name2!=',')
	{
		return 1;
	}
	return 0;
}
int rmmod(char *name)
{
	char buf[512];
	char buf2[1024];
	int x;
	char c;
	x=0;
	while(x<511&&(c=name[x]))
	{
		if(c=='.'||c==',')
		{
			break;
		}
		if(c=='-')
		{
			buf[x]='_';
		}
		else
		{
			buf[x]=c;
		}
		++x;
	}
	buf[x]=0;
	if(!delete_module(buf,0))
	{
		strcpy(buf2,"rmmod ");
		strcat(buf2,buf);
		strcat(buf2,"\n");
		write(msgfd,buf2,strlen(buf2));
		write(1,buf2,strlen(buf2));
		return 0;
	}
	return 1;
}
int modlist_fd;
int modlist_getc(void)
{
	static unsigned char buf[4096];
	static int x,size;
	int ret;
	if(x==size)
	{
		x=0;
		size=read(modlist_fd,buf,4096);
		if(size<=0)
		{
			size=0;
			return -1;
		}
	}
	ret=buf[x];
	++x;
	return ret;
}
void scan_mods(void)
{
	char buf[4097];
	int x,c,x1;
	struct mod *mod;
	int hash;
	int load;
	modlist_fd=open("/etc/modules",0,0);
	if(modlist_fd<0)
	{
		return;
	}
	do
	{
		x=0;
		c=-1;
		while(x<4096&&(c=modlist_getc())!=-1)
		{
			if(c=='\n')
			{
				break;
			}
			buf[x]=c;
			++x;
		}
		load=1;
		buf[x]=0;
		x1=0;
		if(buf[0]=='#')
		{
			load=0;
			x1=1;
		}
		if(buf[0]=='!')
		{
			load=2;
			x1=1;
		}
		if(mod=malloc(sizeof(*mod)))
		{
			memset(mod,0,sizeof(*mod));
			mod->path=str_dup(buf+x1);
			if(mod->path==NULL)
			{
				free(mod);
			}
			else
			{
				x=strlen(buf);
				while(x>x1)
				{
					--x;
					if(buf[x]=='/')
					{
						++x;
						break;
					}
				}
				mod->name=str_dup(buf+x);
				if(mod->name==NULL)
				{
					free(mod->path);
					free(mod);
				}
				else
				{
					hash=name_hash(mod->name);
					mod->tab_next=modtab[hash];
					modtab[hash]=mod;
					mod->list_next=modlist;
					modlist=mod;
					mod->load=load;
				}
			}
		}
	}
	while(c!=-1);
	close(modlist_fd);
}
struct mod *find_mod(char *name)
{
	int hash;
	struct mod *mod;
	hash=name_hash(name);
	mod=modtab[hash];
	while(mod)
	{
		if(!namecmp(mod->name,name))
		{
			return mod;
		}
		mod=mod->tab_next;
	}
	return NULL;
}
struct ELF_header
{
	unsigned char ident[16];
	unsigned short int type;
	unsigned short int machine;
	unsigned int version;
	unsigned long long int entry;
	unsigned long long int phoff;
	unsigned long long int shoff;
	unsigned int flags;
	unsigned short int ehsize;
	unsigned short int phentsize;
	unsigned short int phnum;
	unsigned short int shentsize;
	unsigned short int shnum;
	unsigned short int shstrndx;
};
struct ELF_section_header
{
	unsigned int name;
	unsigned int type;
	unsigned long long int flags;
	unsigned long long int addr;
	unsigned long long int offset;
	unsigned long long int size;
	unsigned int link;
	unsigned int info;
	unsigned long long int addralign;
	unsigned long long int entsize;
};
char *moddep(struct mod *mod)
{
	int fd,n;
	long int size;
	char *modmap;
	char *str,*ret;
	struct ELF_header *eh;
	struct ELF_section_header *sh;
	mod->dev=0;
	fd=open(mod->path,0,0);
	if(fd<0)
	{
		return NULL;
	}
	size=lseek(fd,0,2);
	if(size<0)
	{
		close(fd);
		return NULL;
	}
	modmap=mmap(0,size,1,1,fd,0);
	if(!valid(modmap))
	{
		close(fd);
		return NULL;
	}
	if(memcmp(modmap,"\x7f\x45\x4c\x46\x02\x01\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x3e\x00\x01\x00\x00\x00",24))
	{
		munmap(modmap,size);
		close(fd);
		return NULL;
	}
	eh=(void *)modmap;
	n=eh->shnum;
	sh=(void *)(modmap+eh->shoff);
	str=NULL;
	ret=NULL;
	while(n)
	{
		--n;
		if(!strcmp(modmap+sh[eh->shstrndx].offset+sh[n].name,".modinfo"))
		{
			str=modmap+sh[n].offset;
			break;
		}
	}
	if(str)
	{
		while(*str)
		{
			n=strlen(str);
			if(n>=6&&!memcmp(str,"alias=",6))
			{
				mod->dev=1;
			}
			else if(n>=8&&!memcmp(str,"depends=",8))
			{
				if(str[8]==0)
				{
					ret=NULL;
				}
				else
				{
					ret=str_dup(str+8);
				}
			}
			str+=n+1;
		}
	}
	munmap(modmap,size);
	close(fd);
	return ret;
}
void modprobe(char *name,int force)
{
	struct mod *mod;
	char *deps,*dep_mod,c;
	if(mod=find_mod(name))
	{
		if(!mod->used)
		{
			deps=moddep(mod);
			if(deps)
			{
				dep_mod=deps;
				while(*dep_mod)
				{
					modprobe(dep_mod,1);
					while(c=*dep_mod)
					{
						++dep_mod;
						if(c==',')
						{
							break;
						}
					}
				}
				free(deps);
			}
			if(!insmod(mod->path))
			{
				mod->used=1;
			}
		}
	}
}
int scan_dev(int dirfd,char *name)
{
	int fd;
	struct DIR db;
	struct dirent *dir;
	fd=openat(dirfd,name,0,0);
	if(fd<0)
	{
		return 0;
	}
	dir_init(fd,&db);
	while(dir=readdir(&db))
	{
		if(strcmp(dir->name,".")&&strcmp(dir->name,"..")&&strcmp(dir->name,"bind")&&
		strcmp(dir->name,"module")&&strcmp(dir->name,"new_id")&&strcmp(dir->name,"remove_id")&&
		strcmp(dir->name,"uevent")&&strcmp(dir->name,"unbind"))
		{
			close(fd);
			return 1;
		}
	}
	close(fd);
	return 0;
}
int detect_dev(struct mod *mod)
{
	char name[384],c;
	int x;
	char path[1024];
	int fd;
	struct DIR db;
	struct dirent *dir;
	x=0;
	while(x<256&&(c=mod->name[x]))
	{
		if(c=='.')
		{
			break;
		}
		if(c=='-')
		{
			c='_';
		}
		name[x]=c;
		++x;
	}
	name[x]=0;
	strcpy(path,"/sys/module/");
	strcat(path,name);
	strcat(path,"/drivers");
	fd=open(path,0,0);
	if(fd<0)
	{
		return 0;
	}
	dir_init(fd,&db);
	while(dir=readdir(&db))
	{
		if(strcmp(dir->name,".")&&strcmp(dir->name,".."))
		{
			if(scan_dev(fd,dir->name))
			{
				close(fd);
				return 1;
			}
		}
	}
	close(fd);
	return 0;
}
int main(void)
{
	struct mod *mod;
	char *str;
	int s;
	if(getuid()!=0)
	{
		return 1;
	}
	msgfd=open("/tmp/mod_probe.log",578,0644);
	if(msgfd<0)
	{
		msgfd=1;
	}
	scan_mods();
	mod=modlist;
	while(mod)
	{
		if(mod->load==2)
		{
			modprobe(mod->name,0);
		}
		mod=mod->list_next;
	}
	mod=modlist;
	while(mod)
	{
		if(mod->load==1)
		{
			modprobe(mod->name,0);
		}
		mod=mod->list_next;
	}
	do
	{
		s=0;
		mod=modlist;
		while(mod)
		{
			if(mod->load)
			{
				if(!detect_dev(mod))
				{
					if(!rmmod(mod->name))
					{
						s=1;
						mod->load=0;
					}
				}
			}
			mod=mod->list_next;
		}
	}
	while(s);
	return 0;
}
