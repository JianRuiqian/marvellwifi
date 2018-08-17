#ifndef _MPORT_KERNEL_H_
#define _MPORT_KERNEL_H_

#include <stdio.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "kernel_misc.h"
#include "type.h"

/* debug */
#define panic(fmt, arg...)              rt_kprintf(fmt, ##arg); RT_ASSERT(0)
#define printk(fmt, arg...)             rt_kprintf(fmt, ##arg)

/* memory */
#define kmalloc(x,y)                    rt_malloc(x)
#define kzalloc(x,y)                    rt_calloc(1, x)
#define kfree(x)                        rt_free(x)
#define vmalloc(x)                      rt_malloc(x)
#define vfree(x)                        rt_free(x)

/* tick */
#if RTTHREAD_VERSION < 30000
extern void rt_hw_us_delay(rt_uint32_t us);
#endif

#define mdelay(ms)                      mmcsd_delay_ms(ms)
#define udelay(us)                      rt_hw_us_delay(us)
#define schedule_timeout                rt_thread_delay
#define jiffies                         rt_tick_get()
#define HZ                              RT_TICK_PER_SECOND

/* semaphore */
#define semaphore                       rt_semaphore
#define sema_init(sem, v)               rt_sem_init(sem, "mw_sem", v, RT_IPC_FLAG_FIFO)
#define up(x)                           rt_sem_release(x)
#define down_interruptible(x)           rt_sem_take(x, RT_WAITING_FOREVER)
#define down_trylock(x)                 rt_sem_trytake(x)

/* waitqueue */
#if RTTHREAD_VERSION < 30000
#define wait_queue_head_t               struct rt_semaphore
#define init_waitqueue_head(wq)         rt_sem_init(wq, "mw_waitq", 0, RT_IPC_FLAG_FIFO)
#define wait_event_interruptible_exclusive(wq, condition)   \
        rt_sem_take(&wq, RT_WAITING_FOREVER)
#define wait_event_interruptible(wq, condition) \
        rt_sem_take(&wq, RT_WAITING_FOREVER)
#define wait_event_timeout(wq, condition, timeout)  \
        rt_sem_take(&wq, timeout)
#define wait_event_interruptible_timeout(wq, condition, timeout)    \
        rt_sem_take(&wq, timeout)
#define wake_up(wq)                     rt_sem_release(wq)
#define wake_up_interruptible(wq)       rt_sem_release(wq)
#else
#define wait_queue_head_t               rt_wqueue_t
#if RTTHREAD_VERSION < 30100
#define init_waitqueue_head(wq)         rt_list_init(wq)
#else
#define init_waitqueue_head(wq)         rt_wqueue_init(wq)
#endif
#define wait_event_interruptible_exclusive(wq, condition)   \
        rt_wqueue_wait(&wq, condition, RT_WAITING_FOREVER)
#define wait_event_interruptible(wq, condition) \
        rt_wqueue_wait(&wq, condition, RT_WAITING_FOREVER)
#define wait_event_timeout(wq, condition, timeout)  \
        rt_wqueue_wait(&wq, condition, 1000 * timeout / RT_TICK_PER_SECOND)
#define wait_event_interruptible_timeout(wq, condition, timeout)    \
        rt_wqueue_wait(&wq, condition, 1000 * timeout / RT_TICK_PER_SECOND)
#define wake_up(wq)                     rt_wqueue_wakeup(wq, RT_NULL)
#define wake_up_interruptible(wq)       rt_wqueue_wakeup(wq, RT_NULL)
#endif

/* workqueue */
#define work_struct                     rt_work
#define workqueue_struct                rt_workqueue
#define INIT_WORK(_work, _func)         rt_work_init(_work, ((void (*)(struct rt_work*, void*))_func), NULL)
#define INIT_DELAYED_WORK               INIT_WORK
#define create_workqueue(name)          rt_workqueue_create(name, 1024, 14)
#define destroy_workqueue               rt_workqueue_destroy
#define queue_work                      rt_workqueue_dowork
#define flush_workqueue(workq)

/* network */
#define netif_carrier_ok(x)             netif_is_link_up((x)->dev.netif)
#define netif_carrier_on(x)             eth_device_linkchange(&(x)->dev, RT_TRUE)
#define netif_carrier_off(x)            eth_device_linkchange(&(x)->dev, RT_FALSE)
#define netif_device_attach(x)
#define netif_device_detach(x)

/* irq */
#define in_interrupt                    rt_interrupt_get_nest
#define in_irq                          rt_interrupt_get_nest
#define in_atomic                       rt_critical_level
#define irqs_disabled                   rt_critical_level

/* spinlock */
#define spin_lock_init(lock)
rt_inline void spin_lock_irqsave(spinlock_t *lock, unsigned long flags)
{
    rt_enter_critical();
}
rt_inline void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags)
{
    rt_exit_critical();
}

/* sdio */
#define SDIO_DEVICE(vend, dev) \
    .manufacturer = (vend), \
    .product = (dev)

#define sdio_device_id                  rt_sdio_device_id
#define mmc_host                        rt_mmcsd_host
#define sdio_func                       rt_sdio_function
#define sdio_claim_host(func)           mmcsd_host_lock(func->card->host)
#define sdio_release_host(func)         mmcsd_host_unlock(func->card->host)
#define sdio_claim_irq                  sdio_attach_irq
#define sdio_release_irq                sdio_detach_irq

rt_inline unsigned char
sdio_readb(struct sdio_func *func, unsigned int addr, int *err_ret)
{
    return sdio_io_readb(func, addr, (rt_int32_t *)err_ret);
}

rt_inline void
sdio_writeb(struct sdio_func *func, unsigned char b, unsigned int addr, int *err_ret)
{
    *err_ret = sdio_io_writeb(func, addr, b);
}

rt_inline int
sdio_readsb(struct sdio_func *func, void *dst, unsigned int addr, int count)
{
    return sdio_io_read_multi_fifo_b(func, addr, dst, count);
}

rt_inline int
sdio_writesb(struct sdio_func *func, unsigned int addr, void *src, int count)
{
    return sdio_io_write_multi_fifo_b(func, addr, src, count);
}

rt_inline unsigned char
sdio_f0_readb(struct sdio_func *func, unsigned int addr, int *err_ret)
{
    struct sdio_func *func0;

    func0 = func->card->sdio_function[0];

    return sdio_io_readb(func0, addr, (rt_int32_t *)err_ret);
}

rt_inline void
sdio_f0_writeb(struct sdio_func *func, unsigned char b, unsigned int addr, int *err_ret)
{
    struct sdio_func *func0;

    func0 = func->card->sdio_function[0];
    *err_ret = sdio_io_writeb(func0, addr, b);
}

/* copy */
rt_inline int copy_from_user(void *to, void *from, int n)
{
    rt_memcpy(to, from, n);
    return 0;
}

rt_inline int copy_to_user(void *to, void *from, int n)
{
    rt_memcpy(to, from, n);
    return 0;
}

/* page */
rt_inline void get_page(struct page *page)
{
    page->addr = (char *)rt_malloc(PAGE_SIZE);
}

rt_inline void put_page(struct page *page)
{
    rt_free(page->addr);
    page->addr = NULL;
}

/* module */
rt_inline int try_module_get(int module)
{
    return 1;
}

rt_inline void module_put(int module)
{

}

/* time */
#ifndef RT_USING_LIBC
#error Please define RT_USING_LIBC in rtconfig.h
#else
#include <sys/time.h>
#endif

rt_inline void do_gettimeofday(struct timeval *tv)
{
    rt_tick_t tick;

    tick = rt_tick_get();
    tv->tv_sec = tick / RT_TICK_PER_SECOND;
    tv->tv_usec = (tick % RT_TICK_PER_SECOND) *
                  (1000000 / RT_TICK_PER_SECOND);
}

rt_inline ktime_t ktime_set(const long secs, const unsigned long nsecs)
{
    return (ktime_t)
    {
        .tv = { .sec = secs, .nsec = nsecs }
    };
}

rt_inline ktime_t timeval_to_ktime(struct timeval tv)
{
    return ktime_set(tv.tv_sec, tv.tv_usec * 1000);
}

#endif  /* _MPORT_KERNEL_H_ */
