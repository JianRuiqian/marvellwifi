#ifndef _MPORT_TYPE_H_
#define _MPORT_TYPE_H_

#include <stdint.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t  __u8;
typedef uint16_t __u16;
typedef uint32_t __u32;
typedef uint64_t __u64;

typedef int8_t  __s8;
typedef int16_t __s16;
typedef int32_t __s32;

typedef uint8_t  t_u8;
typedef uint16_t t_u16;
typedef uint32_t t_u32;

typedef u16 __le16;
typedef u16 __be16;
typedef u32 __le32;
typedef u32 __be32;
typedef u64 __le64;
typedef u64 __be64;

typedef u16 __sum16;
typedef u32 __wsum;

typedef u32 spinlock_t;

typedef struct {
	int counter;
} atomic_t;

struct firmware {
	u32 size;
	const u8 *data;
    int fd;
};

struct page {
	unsigned long flags;
	atomic_t _count;
	char  *addr;
};

union ktime {
	struct {
        long nsec, sec;
	} tv;
};
typedef union ktime ktime_t;

struct ethhdr {
	unsigned char	h_dest[6];	    /* destination eth addr	*/
	unsigned char	h_source[6];	/* source ether addr	*/
	unsigned short	h_proto;		/* packet type ID field	*/
};

struct iphdr {
	__u8	ihl:4,
		version:4;
	__u8	tos;
	__be16	tot_len;
	__be16	id;
	__be16	frag_off;
	__u8	ttl;
	__u8	protocol;
	__sum16	check;
	__be32	saddr;
	__be32	daddr;
	/*The options start here. */
};

struct tcphdr {
	__be16	source;
	__be16	dest;
	__be32	seq;
	__be32	ack_seq;
	__u16	res1:4,
		doff:4,
		fin:1,
		syn:1,
		rst:1,
		psh:1,
		ack:1,
		urg:1,
		ece:1,
		cwr:1;
	__be16	window;
	__sum16	check;
	__be16	urg_ptr;
};

typedef enum {
	GFP_KERNEL,
	GFP_ATOMIC,
	__GFP_HIGHMEM,
	__GFP_HIGH
} gfp_t;

#endif /* !_MWIFIEX_DECL_H_ */
