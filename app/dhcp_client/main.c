#include "../../include/syscall.c"
#include "../../include/mem.c"
#include "../../include/socket.c"
#include "../../include/signal.c"
#include "../../include/dirent.c"
#include "../../include/iformat.c"

int sockfd,sockfd2;
unsigned char hwaddr[6];
int ifindex;
struct sockaddr_ll sock_addr,sock_addr2;
unsigned int ip_addr,server_ip_addr,mask,gateway,dns,lease_time;
int sock_init(char *name)
{
	static struct ifreq req;
	int fd,val;
	strcpy(req.name,name);
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if(fd<0)
	{
		return 1;
	}
	if(ioctl(fd,SIOCGIFFLAGS,&req))
	{
		close(fd);
		return 1;
	}
	req.data.flags|=1;
	if(ioctl(fd,SIOCSIFFLAGS,&req))
	{
		close(fd);
		return 1;
	}
	memset(&req.data,0,16);
	if(ioctl(fd,SIOCGIFHWADDR,&req))
	{
		close(fd);
		return 1;
	}
	memcpy(hwaddr,req.data.bytes+2,6);
	memset(&req.data,0,16);
	if(ioctl(fd,SIOCGIFINDEX,&req))
	{
		close(fd);
		return 1;
	}
	ifindex=req.data.ifindex;
	memset(&req.data,0,16);
	req.data.addr.sin_family=AF_INET;
	if(ioctl(fd,SIOCSIFADDR,&req))
	{
		close(fd);
		return 1;
	}
	close(fd);
	sockfd=socket(AF_PACKET,SOCK_DGRAM,ETH_P_IP);
	if(sockfd<0)
	{
		return 1;
	}
	sock_addr.sll_family=AF_PACKET;
	sock_addr.sll_protocol=ETH_P_IP;
	sock_addr.sll_ifindex=ifindex;
	sock_addr.sll_halen=6;
	memset(sock_addr.sll_addr,0xff,6);
	if(bind(sockfd,&sock_addr,sizeof(sock_addr)))
	{
		close(sockfd);
		return 1;
	}
	sockfd2=socket(AF_PACKET,SOCK_DGRAM,ETH_P_IP);
	if(sockfd2<0)
	{
		close(sockfd);
		return 1;
	}
	sock_addr2.sll_family=AF_PACKET;
	sock_addr2.sll_protocol=ETH_P_IP;
	sock_addr2.sll_ifindex=ifindex;
	if(bind(sockfd2,&sock_addr2,sizeof(sock_addr2)))
	{
		close(sockfd);
		close(sockfd2);
		return 1;
	}
	val=1;
	if(setsockopt(sockfd2,263,8,&val,4))
	{
		close(sockfd);
		close(sockfd2);
		return 1;
	}
	val=fcntl(sockfd2,F_GETFL,0);
	fcntl(sockfd2,F_SETFL,val|04000);
	return 0;
}
int init_addr(char *name)
{
	static struct ifreq req;
	static struct rtentry rtentry;
	int fd,val;
	char buf[32];
	strcpy(req.name,name);
	fd=socket(AF_INET,SOCK_DGRAM,0);
	if(fd<0)
	{
		return 1;
	}
	req.data.addr.sin_family=AF_INET;
	req.data.addr.sin_addr=ip_addr;
	if(ioctl(fd,SIOCSIFADDR,&req))
	{
		close(fd);
		return 1;
	}
	req.data.addr.sin_addr=mask;
	if(ioctl(fd,SIOCSIFNETMASK,&req))
	{
		close(fd);
		return 1;
	}
	rtentry.gateway.sin_addr=gateway;
	rtentry.gateway.sin_family=AF_INET;
	rtentry.dst.sin_family=AF_INET;
	rtentry.genmask.sin_family=AF_INET;
	rtentry.flags=3; // UP|GATEWAY
	rtentry.dev=name;

	if(ioctl(fd,SIOCADDRT,&rtentry))
	{
		close(fd);
		return 1;
	}
	close(fd);
	buf[0]=0;
	sprinti(buf,dns&0xff,1);
	strcat(buf,".");
	sprinti(buf,dns>>8&0xff,1);
	strcat(buf,".");
	sprinti(buf,dns>>16&0xff,1);
	strcat(buf,".");
	sprinti(buf,dns>>24&0xff,1);
	strcat(buf,"\n");
	fd=open("/run/dns_addr",578,0644);
	if(fd<0)
	{
		return 1;
	}
	write(fd,buf,strlen(buf));
	close(fd);
	return 0;
}
struct udp_packet
{
	unsigned char version_len; // 0x45
	unsigned char type;
	unsigned short packlen;
	unsigned short id;
	unsigned short flag_off;
	unsigned char ttl; // 64
	unsigned char prot; // 17
	unsigned short checksum;
	unsigned int src_addr;
	unsigned int dst_addr;

	unsigned short src_port;
	unsigned short dst_port;
	unsigned short len;
	unsigned short udp_checksum;
	unsigned char data[65508];
} udp;
unsigned int ip_checksum(void *buf,int size)
{
	unsigned short *ptr;
	unsigned int checksum;
	ptr=buf;
	checksum=0;
	while(size>0)
	{
		checksum+=htonw(*ptr);
		size-=2;
		++ptr;
	}
	while(checksum>0xffff)
	{
		checksum=(checksum>>16)+(checksum&0xffff);
	}
	return htonw(~checksum);
}
int dhcp_udp_send(unsigned int src,unsigned int dst,void *buf,int size)
{
	memset(&udp,0,sizeof(udp));
	memcpy(udp.data,buf,size);
	udp.src_addr=src;
	udp.dst_addr=dst;
	udp.prot=17;
	udp.src_port=htonw(68);
	udp.dst_port=htonw(67);
	udp.len=htonw(size+8);
	udp.packlen=htonw(size+8);
	udp.udp_checksum=ip_checksum(&udp,size+28);
	udp.packlen=htonw(size+28);
	udp.version_len=0x45;
	udp.ttl=64;
	udp.checksum=ip_checksum(&udp,20);

	sendto(sockfd,&udp,size+28,0,&sock_addr,sizeof(sock_addr));
}
int dhcp_udp_recv(void *buf)
{
	int ret;
	memset(&udp,0,sizeof(udp));
	sleep(0,100000);
	ret=recvfrom(sockfd2,&udp,sizeof(udp),0,NULL,NULL);
	if(ret>28)
	{
		memcpy(buf,udp.data,ret-28);
		ret-=28;
	}
	else
	{
		ret=-1;
	}
	return ret;
}
struct dhcp_msg
{
	unsigned char op;
	unsigned char htype;
	unsigned char hlen;
	unsigned char hops;
	unsigned int xid;
	unsigned short secs;
	unsigned short flags;
	unsigned int ciaddr;
	unsigned int yiaddr;
	unsigned int siaddr;
	unsigned int giaddr;
	unsigned char chaddr[16];
	unsigned char sname[64];
	unsigned char file[128];
	unsigned char options[65536];
} dhcp_msg,dhcp_msg2;
int dhcp_send_discover(void)
{
	unsigned int xid;
	memset(&dhcp_msg,0,sizeof(dhcp_msg));
	dhcp_msg.op=1;
	dhcp_msg.htype=1;
	dhcp_msg.hlen=6;
	getrandom(&xid,4,1);
	dhcp_msg.xid=xid;
	memcpy(dhcp_msg.chaddr,hwaddr,6);
	dhcp_msg.options[0]=99;
	dhcp_msg.options[1]=130;
	dhcp_msg.options[2]=83;
	dhcp_msg.options[3]=99;
	dhcp_msg.options[4]=53;
	dhcp_msg.options[5]=1;
	dhcp_msg.options[6]=1;
	dhcp_msg.options[7]=255;
	do
	{
		if(dhcp_udp_send(0x0,0xffffffff,&dhcp_msg,312)<0)
		{
			return -1;
		}
		if(dhcp_udp_recv(&dhcp_msg2)<=0)
		{
			return -1;
		}
	}
	while(dhcp_msg2.xid!=xid);
	return 0;
}
int dhcp_send_request(int s)
{
	unsigned int xid,val;
	unsigned int addr;
	memset(&dhcp_msg,0,sizeof(dhcp_msg));
	dhcp_msg.op=1;
	dhcp_msg.htype=1;
	dhcp_msg.hlen=6;
	getrandom(&xid,4,1);
	dhcp_msg.xid=xid;
	addr=0;
	if(s)
	{
		dhcp_msg.yiaddr=ip_addr;
		dhcp_msg.ciaddr=ip_addr;
		addr=ip_addr;
	}
	memcpy(dhcp_msg.chaddr,hwaddr,6);
	dhcp_msg.options[0]=99;
	dhcp_msg.options[1]=130;
	dhcp_msg.options[2]=83;
	dhcp_msg.options[3]=99;
	dhcp_msg.options[4]=53;
	dhcp_msg.options[5]=1;
	dhcp_msg.options[6]=3;
	dhcp_msg.options[7]=51;
	dhcp_msg.options[8]=4;
	val=htonl(lease_time);
	memcpy(dhcp_msg.options+9,&val,4);
	dhcp_msg.options[13]=50;
	dhcp_msg.options[14]=4;
	val=ip_addr;
	memcpy(dhcp_msg.options+15,&val,4);
	dhcp_msg.options[19]=54;
	dhcp_msg.options[20]=4;
	val=server_ip_addr;
	memcpy(dhcp_msg.options+21,&val,4);
	dhcp_msg.options[25]=255;
	do
	{
		if(dhcp_udp_send(addr,0xffffffff,&dhcp_msg,312)<0)
		{
			return -1;
		}
		if(dhcp_udp_recv(&dhcp_msg2)<=0)
		{
			return -1;
		}
	}
	while(dhcp_msg2.xid!=xid);
	return 0;
}
int main(void)
{
	int fd,sfd;
	struct DIR db;
	struct dirent *dir;
	int x;
	int valid,s;
	if(getuid()!=0)
	{
		return 1;
	}
	fd=open("/sys/class/net",0,0);
	if(fd<0)
	{
		return 1;
	}
	dir_init(fd,&db);
	signal(SIGCHLD,SIG_IGN);
	signal(SIGPIPE,SIG_IGN);
	while(dir=readdir(&db))
	{
		if(dir->name[0]=='e')
		{
			if(fork()==0)
			{
				while(1)
				{
					do
					{
						while(1)
						{
							if(!sock_init(dir->name))
							{
								if(!dhcp_send_discover())
								{
									break;
								}
								close(sockfd);
								close(sockfd2);
							}
							sleep(2,0);
						}
						ip_addr=dhcp_msg2.yiaddr;
						valid=0;
						x=4;
						while(dhcp_msg2.options[x]!=255&&dhcp_msg2.options[x+1]!=0)
						{
							if(dhcp_msg2.options[x]==53&&dhcp_msg2.options[x+1]==1)
							{
								if(dhcp_msg2.options[x+2]==2)
								{
									valid=1;
								}
							}
							else if(dhcp_msg2.options[x]==1&&dhcp_msg2.options[x+1]==4)
							{
								memcpy(&mask,dhcp_msg2.options+x+2,4);
							}
							else if(dhcp_msg2.options[x]==3&&dhcp_msg2.options[x+1]==4)
							{
								memcpy(&gateway,dhcp_msg2.options+x+2,4);
							}
							else if(dhcp_msg2.options[x]==6&&dhcp_msg2.options[x+1]==4)
							{
								memcpy(&dns,dhcp_msg2.options+x+2,4);
							}
							else if(dhcp_msg2.options[x]==51&&dhcp_msg2.options[x+1]==4)
							{
								memcpy(&lease_time,dhcp_msg2.options+x+2,4);
								lease_time=htonl(lease_time);
							}
							x+=2+dhcp_msg2.options[x+1];
						}
						sleep(0,250000);
						if(valid)
						{
							break;
						}
						close(sockfd);
						close(sockfd2);
					}
					while(1);
					server_ip_addr=dhcp_msg2.siaddr;
					s=0;
					while(1)
					{
						if(dhcp_send_request(s))
						{
							close(sockfd);
							close(sockfd2);
							break;
						}
						valid=0;
						x=4;
						while(dhcp_msg2.options[x]!=255&&dhcp_msg2.options[x+1]!=0)
						{
							if(dhcp_msg2.options[x]==53&&dhcp_msg2.options[x+1]==1)
							{
								if(dhcp_msg2.options[x+2]==5)
								{
									valid=1;
								}
							}
							x+=2+dhcp_msg2.options[x+1];
						}
						if(!valid)
						{
							close(sockfd);
							close(sockfd2);
							sleep(2,0);
							break;
						}
						else
						{
							if(!s)
							{
								s=1;
								while(init_addr(dir->name))
								{
									sleep(0,333333);
								}
							}
							sleep(lease_time/2,0);
						}
					}
				}
			}
		}
	}
	return 0;
}
