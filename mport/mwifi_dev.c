#include <rtthread.h>
#include "netdevice.h"

/* Net Device */
#if RTTHREAD_VERSION < 30000
static rt_err_t rt_mwifi_control(rt_device_t dev, rt_uint8_t cmd, void *args)
#else
static rt_err_t rt_mwifi_control(rt_device_t dev, int cmd, void *args)
#endif
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args)
        {
            struct net_device *ndev;

            ndev = (struct net_device *)dev->user_data;
            rt_memcpy(args, ndev->dev_addr, 6);
        }
        else return -RT_ERROR;
        break;

    default :
        break;
    }

    return RT_EOK;
}

/* ethernet device interface */
/* transmit packet. */
static rt_err_t mwifi_eth_tx(rt_device_t dev, struct pbuf *p)
{
    struct pbuf *q;
    rt_uint32_t offset = 0;
    struct sk_buff *skb = NULL;
    struct net_device *ndev;

    ndev = (struct net_device *)dev->user_data;

#if RT_LWIP_ETH_PAD_SIZE    /* ETH_PAD_SIZE */
    pbuf_header(p, -RT_LWIP_ETH_PAD_SIZE); /* drop the padding word */
#endif

    skb = alloc_skb(p->tot_len + ndev->hard_header_len, GFP_KERNEL);
    if (!skb) return RT_ENOMEM;

    skb_reserve(skb, ndev->hard_header_len);

    for (q = p; q != NULL; q = q->next)
    {
        uint8_t *to;

        to = (uint8_t *)((skb->data) + offset);
        rt_memcpy(to, q->payload, q->len);
        offset += q->len;
    }
    skb->len = offset;

    ndev->netdev_ops->ndo_start_xmit(skb, ndev);

#if RT_LWIP_ETH_PAD_SIZE    /* ETH_PAD_SIZE */
    pbuf_header(p, RT_LWIP_ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    return RT_EOK;
}

/* reception packet. */
int netif_rx(struct sk_buff *skb)
{
    struct pbuf *p = NULL;
    struct eth_device *device;
    rt_uint32_t offset;
    rt_uint16_t len;

    len = skb->len;

#if RT_LWIP_ETH_PAD_SIZE    /* ETH_PAD_SIZE */
    len += RT_LWIP_ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* allocate buffer */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p != RT_NULL)
    {
        struct pbuf *q;

#if RT_LWIP_ETH_PAD_SIZE    /* ETH_PAD_SIZE */
        pbuf_header(p, -RT_LWIP_ETH_PAD_SIZE); /* drop the padding word */
#endif

        offset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            rt_memcpy(q->payload, ((uint8_t *)skb->data + offset), q->len);
            offset += q->len;
        }

#if RT_LWIP_ETH_PAD_SIZE    /* ETH_PAD_SIZE */
        pbuf_header(p, RT_LWIP_ETH_PAD_SIZE); /* reclaim the padding word */
#endif

        /* notify to upper layer */
        device = &(skb->dev->dev);
        if (device->netif->input(p, device->netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: Input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
    kfree_skb(skb);
    skb = NULL;

    return 0;
}

int register_netdev(struct net_device *dev)
{
    rt_err_t err = -RT_ERROR;

    /* When net_device's are persistent, this will be fatal. */
    RT_ASSERT(dev->reg_state == NETREG_UNINITIALIZED);

    dev->dev.parent.init = RT_NULL;
    dev->dev.parent.open = RT_NULL;
    dev->dev.parent.close = RT_NULL;
    dev->dev.parent.read = RT_NULL;
    dev->dev.parent.write = RT_NULL;
    dev->dev.parent.control = rt_mwifi_control;
    dev->dev.parent.user_data = dev;

    dev->dev.eth_rx = RT_NULL/*mwifi_eth_rx*/;
    dev->dev.eth_tx = mwifi_eth_tx;


    if (!strncmp(dev->name, "mlan", strlen("mlan")))
    {
        /* register eth device */
        err = eth_device_init(&dev->dev, dev->name);
    }
    else if (!strncmp(dev->name, "uap", strlen("uap")))
    {
        rt_uint16_t flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

#if LWIP_IGMP
        /* IGMP support */
        flags |= NETIF_FLAG_IGMP;
#endif
        /* register eth device */
        err = eth_device_init_with_flag(&dev->dev, dev->name, flags);
    }
    if (err == RT_EOK)
        dev->reg_state = NETREG_REGISTERED;

    return err;
}

void unregister_netdev(struct net_device *dev)
{
    /* unregister eth device */
    netif_remove(dev->dev.netif);
    dev->reg_state = NETREG_UNREGISTERED;
}

struct net_device *alloc_etherdev(int sizeof_priv)
{
    return rt_calloc(1, sizeof(struct net_device) + sizeof_priv);
}

void free_netdev(struct net_device *dev)
{
    rt_free(dev);
}

int dev_alloc_name(struct net_device *dev, const char *name)
{
    char buf[IFNAMSIZ];
    rt_device_t rtdev;
    int i = 0;
    int ret = -1;

    do
    {
        snprintf(buf, IFNAMSIZ, name, i);
        rtdev = rt_device_find(buf);
        if (!rtdev)
        {
            strcpy(dev->name, buf);
            ret = 0;
            break;
        }
    }
    while (++i);

    return ret;
}
