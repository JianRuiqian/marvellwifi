#ifndef _MPORT_KERNEL_MISC_H_
#define _MPORT_KERNEL_MISC_H_

#define KERN_DEBUG
#define KERN_ERR
#define KERN_ALERT

#define LINUX_VERSION_CODE              (((3) << 16) + ((2) << 8) + (88))
#define KERNEL_VERSION(a,b,c)           (((a) << 16) + ((b) << 8) + (c))

#define likely(x)                       (__builtin_expect(!!(x), 1))
#define unlikely(x)                     (__builtin_expect(!!(x), 0))

#define max(x, y)                       (((x) > (y)) ? (x) : (y))
#define min(x, y)                       (((x) < (y)) ? (x) : (y))

#define __constant_htons(x) ((__u16)(               \
    (((__u16)(x) & (__u16)0x00ffU) << 8) |          \
    (((__u16)(x) & (__u16)0xff00U) >> 8)))

#define num_possible_cpus()             1U

#define le16_to_cpu(val)                (val)
#define le32_to_cpu(val)                (val)
#define le64_to_cpu(val)                (val)

#define cpu_to_le16(val)                (val)
#define cpu_to_le32(val)                (val)
#define cpu_to_le64(val)                (val)

#define E2BIG                           7
#define ENOTSUPP                        524
#define ERR_PTR(err)                    (void *)err

#define THIS_MODULE                     1
#define BIT(nr)                         (1UL << (nr))
#define ARRAY_SIZE(a)                   (sizeof(a) / sizeof(a[0]))

#define SCHED_NORMAL                    0
#define SCHED_FIFO                      1
#define SCHED_RR                        2
#define SCHED_BATCH                     3
/* SCHED_ISO: reserved but not implemented yet */
#define SCHED_IDLE                      5

#define ETH_ALEN                        6
#define ETH_FRAME_LEN                   1514
#define ETH_P_IP                        0x0800
#define ETH_P_ARP                       0x0806

#define ARPHRD_ETHER                    1

#define NET_SKB_PAD                     32
#define NUMA_NO_NODE                    (-1)

#define SIOCDEVPRIVATE                  0x89F0
#define SIOCPROTOPRIVATE                0x89E0

#define PAGE_SHIFT                      12
#define PAGE_SIZE                       (1UL << PAGE_SHIFT)
#define PAGE_MASK                       (~(PAGE_SIZE-1))

#define __user
#define __kernel
#define __safe
#define __force
#define __nocast
#define __iomem
#define __chk_user_ptr(x)               (void)0
#define __chk_io_ptr(x)                 (void)0
#define __builtin_warning(x, y...)      (1)
#define __must_hold(x)
#define __acquires(x)
#define __releases(x)
#define __acquire(x)                    (void)0
#define __release(x)                    (void)0
#define __cond_lock(x,c)                (c)
#define __percpu
#define __rcu
#define __private

#endif  /* _MPORT_KERNEL_MISC_H_ */
