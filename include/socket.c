#ifndef _SOCKET_C_
#define _SOCKET_C_
#define AF_UNIX 1
#define AF_INET 2
#define AF_INET6 10
#define AF_NETLINK 16
#define AF_PACKET 17
#define AF_ALG 38

#define SOCK_STREAM 1
#define SOCK_DGRAM 2
#define SOCK_RAW 3
#define SOCK_SEQPACKET 5
#define SOCK_CLOEXEC 02000000
#define SOCK_NONBLOCK 04000

#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
#define ETH_P_ALL 0x0300
#define ETH_P_IP 0x0008

#define SOL_ALG 279

struct sockaddr_un
{
	unsigned short int family;
	char sun_path[108];
};
struct sockaddr_in
{
	unsigned short int sin_family;
	unsigned short int sin_port;
	unsigned int sin_addr;
	char pad[8];
};
struct sockaddr_in6
{
	unsigned short int sin6_family;
	unsigned short int sin6_port;
	unsigned int sin6_flowinfo;
	unsigned char sin6_addr[16];
	unsigned int sin6_scope_id;
};
struct sockaddr_nl
{
	unsigned short int nl_family;
	unsigned short int pad;
	unsigned int pid;
	unsigned int groups;
};
struct sockaddr_ll
{
	unsigned short sll_family;
	unsigned short sll_protocol;
	int sll_ifindex;
	unsigned short sll_hatype;
	unsigned char sll_pkttype;
	unsigned char sll_halen;
	unsigned char sll_addr[8];
};
struct sockaddr_alg
{
	unsigned short salg_family;
	unsigned char salg_type[14];
	unsigned int salg_feat;
	unsigned int salg_mask;
	unsigned char salg_name[64];
};

#define SIOCADDRT 0x890b
#define SIOCGIFCONF 0x8912
#define SIOCGIFFLAGS 0x8913
#define SIOCSIFFLAGS 0x8914
#define SIOCGIFADDR 0x8915
#define SIOCSIFADDR 0x8916
#define SIOCSIFNETMASK 0x891c
#define SIOCGIFHWADDR 0x8927
#define SIOCGIFINDEX 0x8933

struct in6_ifreq
{
	unsigned char ifr6_addr[16];
	unsigned int ifr6_prefixlen;
	int ifr6_ifindex;
};
union __ifreq_data
{
	unsigned char bytes[16];
	struct sockaddr_in addr;
	unsigned short flags;
	unsigned int ifindex;
};
struct ifreq
{
	char name[16];
	union __ifreq_data data;
	char pad[8];
};
struct ifconf
{
	int ifc_len;
	void *ifcu_req;
};
struct rtentry
{
	unsigned long pad1;
	struct sockaddr_in dst;
	struct sockaddr_in gateway;
	struct sockaddr_in genmask;
	unsigned short flags;
	short pad2;
	unsigned int _pad;
	unsigned long pad3;
	unsigned long pad4;
	short metric;
	unsigned short _pad2[3];
	char *dev;
	unsigned long mtu;
	unsigned long window;
	unsigned short irtt;
	unsigned short _pad3[3];
};

#define htonw(n) ((unsigned short)(n)<<8|(unsigned short)(n)>>8)
#define htonl(n) ((unsigned int)(n)<<24|(unsigned int)(n)>>24|(unsigned int)(n)<<8&0xff0000|(unsigned int)(n)>>8&0xff00)

#define MSG_DONTWAIT 0x40
struct msghdr
{
	void *name;
	unsigned long namelen;
	void *iov;
	unsigned long iovlen;
	void *control;
	unsigned long controllen;
	unsigned long unused;
};
struct cmsghdr
{
	unsigned long len;
	int level;
	int type;
	unsigned char data[8];
};

#endif
