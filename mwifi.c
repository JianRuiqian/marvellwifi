#include <rtthread.h>

#if defined(MARVELLWIFI_USING_UAP) || defined(MARVELLWIFI_USING_STA)
#include "moal_main.h"
#define CMD_BUFSIZE 256

#ifdef MARVELLWIFI_USING_UAP
#include "moal_uap.h"
#include "moal_uap_priv.h"

rt_err_t mwifi_config(struct net_device *ndev, const char *ssid,
                      const char *key, int ch, int max_sta)
{
    struct ifreq ifr;
    struct iwreq *wrq;
    char *cmd_buf = RT_NULL;
    int cmd_len = 0;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    if ((cmd_buf = rt_calloc(1, CMD_BUFSIZE)) == RT_NULL)
        return -RT_ENOMEM;
    cmd_len += sprintf(cmd_buf + cmd_len, "ASCII_CMD=%s", "AP_CFG");
    cmd_len += sprintf(cmd_buf + cmd_len, ",SSID=%s", ssid);
    cmd_len += sprintf(cmd_buf + cmd_len, ",SEC=%s", key ? "wpa2-psk" : "open");
    cmd_len += key ? sprintf(cmd_buf + cmd_len, ",KEY=%s", key) : 0;
    cmd_len += ch ? sprintf(cmd_buf + cmd_len, ",CHANNEL=%d", ch) : 0;
    cmd_len += max_sta ? sprintf(cmd_buf + cmd_len, ",MAX_SCB=%d", max_sta) : 0;
    cmd_len += sprintf(cmd_buf + cmd_len, ",END");
    wrq = (struct iwreq *)&ifr;
    wrq->u.data.pointer = cmd_buf;
    wrq->u.data.length = cmd_len;
    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, &ifr, WOAL_UAP_FROYO_AP_SET_CFG);
    rt_free(cmd_buf);

    return ret;
}

rt_err_t mwifi_start(struct net_device *ndev)
{
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, NULL, WOAL_UAP_FROYO_AP_BSS_START);
#ifdef LWIP_USING_DHCPD
    extern void dhcpd_start(char *netif_name);
    dhcpd_start(ndev->name);
#endif

    return ret;
}

rt_err_t mwifi_stop(struct net_device *ndev)
{
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, NULL, WOAL_UAP_FROYO_AP_BSS_STOP);
#ifdef LWIP_USING_DHCPD
    extern void dhcpd_stop(void);
    dhcpd_stop();
#endif

    return ret;
}

rt_err_t mwifi_get_sta_list(struct net_device *ndev)
{
    struct ifreq ifr;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    if ((ifr.ifr_data = rt_calloc(1, CMD_BUFSIZE)) == RT_NULL)
        return -RT_ENOMEM;
    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, &ifr, UAP_GET_STA_LIST);
    if (!ret)
    {
        int i;
        mlan_ds_sta_list *list;

        list = (mlan_ds_sta_list *)ifr.ifr_data;
        for (i = 0; i < list->sta_count; i ++)
        {
            rt_kprintf("STA(%02d): " MACSTR ", ", i,
                       MAC2STR(list->info[i].mac_address));
            rt_kprintf("RSSI[%03d dBm], ", list->info[i].rssi);
            rt_kprintf("Power MFG%s\n",
                       (list->info[i].power_mfg_status ==
                        0) ? "[active]" : "[power save]");
        }
        if (i == 0)
            rt_kprintf("station list is empty!\n");
    }
    rt_free(ifr.ifr_data);

    return ret;
}

rt_err_t mwifi_deauth_sta(struct net_device *ndev, int index)
{
    struct ifreq ifr;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    if ((ifr.ifr_data = rt_calloc(1, CMD_BUFSIZE)) == RT_NULL)
        return -RT_ENOMEM;
    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, &ifr, UAP_GET_STA_LIST);
    if (!ret)
    {
        mlan_ds_sta_list *list = (mlan_ds_sta_list *)ifr.ifr_data;

        if (index <= list->sta_count)
        {
            mlan_deauth_param deauth_param;

            ifr.ifr_data = &deauth_param;
            deauth_param.reason_code = 0;
            memcpy(deauth_param.mac_addr, list->info[index].mac_address, 6);
            ret = ndev->netdev_ops->ndo_do_ioctl
                  (ndev, &ifr, UAP_STA_DEAUTH);
            if (!ret)
                rt_kprintf("removed STA(%02d): " MACSTR "\n", index,
                           MAC2STR(deauth_param.mac_addr));
            ifr.ifr_data = list;
        }
    }
    rt_free(ifr.ifr_data);

    return ret;
}
#endif  /* MARVELLWIFI_USING_UAP */
#ifdef MARVELLWIFI_USING_STA
rt_err_t mwifi_connect(struct net_device *ndev, const char *ssid,
                       const char *key)
{
    union iwreq_data uwrq;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    if (key)
    {
        struct ifreq ifr;
        struct iwreq *wrq;
        char *cmd_buf = RT_NULL;
        int cmd_len = 0;

        if ((cmd_buf = rt_calloc(1, CMD_BUFSIZE)) == RT_NULL)
            return -RT_ENOMEM;
        cmd_len += sprintf(cmd_buf + cmd_len, "1");
        cmd_len += sprintf(cmd_buf + cmd_len, ";ssid=%s", ssid);
        cmd_len += sprintf(cmd_buf + cmd_len, ";passphrase=%s", key);
        wrq = (struct iwreq *)&ifr;
        wrq->u.data.pointer = cmd_buf;
        wrq->u.data.length = cmd_len;
        wrq->u.data.flags = WOAL_PASSPHRASE;
        ret = ndev->netdev_ops->ndo_do_ioctl
              (ndev, &ifr, WOAL_SET_GET_256_CHAR);
        rt_free(cmd_buf);
        if (ret)
            goto done;
    }
    uwrq.mode = IW_MODE_INFRA;
    ret = ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCSIWMODE)](ndev, NULL, &uwrq, NULL);
    if (ret)
        goto done;
    uwrq.essid.flags = 1;
    uwrq.essid.length = strlen(ssid);
    ret = ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCSIWESSID)](ndev, NULL, &uwrq, (char *)ssid);

done:
    return ret;
}

rt_err_t mwifi_disconnect(struct net_device *ndev)
{
    struct ifreq ifr;
    struct iwreq *wrq;
    char cmd_buf[sizeof(struct iwreq)];
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    memset(cmd_buf, 0, sizeof(cmd_buf));
    ifr.ifr_data = &cmd_buf;
    wrq = (struct iwreq *)&ifr;
    wrq->u.data.length = 0;
    wrq->u.data.flags = WOAL_DEAUTH;
    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, &ifr, WOAL_SETADDR_GETNONE);

    return ret;
}

rt_err_t mwifi_scan(struct net_device *ndev)
{
    union iwreq_data uwrq;
    char cmd_buf[sizeof(WEXT_CSCAN_HEADER)] = {0};
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    memcpy(cmd_buf, WEXT_CSCAN_HEADER, sizeof(cmd_buf));
    uwrq.data.pointer = cmd_buf;
    uwrq.data.length = WEXT_CSCAN_HEADER_SIZE;
    ret = ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCSIWPRIV)](ndev, NULL, &uwrq, NULL);

    return ret;
}

rt_err_t mwifi_powersave(struct net_device *ndev, int enabled)
{
    union iwreq_data uwrq;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    uwrq.param.flags = 0;
    uwrq.param.disabled = enabled ? 0 : 1;
    ret = (enabled < 0) ? ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCGIWPOWER)](ndev, NULL, &uwrq, NULL) :
          ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCSIWPOWER)](ndev, NULL, &uwrq, NULL);
    if (!ret)
        rt_kprintf("powersave: %s\n",
                   uwrq.param.disabled ? "disabled" : "enabled");

    return ret;
}

#if defined(REASSOCIATION)
rt_err_t mwifi_reassociation(struct net_device *ndev, int enabled)
{
    struct ifreq ifr;
    struct iwreq *wrq;
    char cmd_buf[sizeof(struct iwreq)];
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    memset(cmd_buf, 0, sizeof(cmd_buf));
    ifr.ifr_data = &cmd_buf;
    wrq = (struct iwreq *)&ifr;
    wrq->u.data.flags = WOAL_SET_GET_REASSOC;
    wrq->u.data.pointer = &enabled;
    wrq->u.data.length = (enabled < 0) ? 0 : 1;
    ret = ndev->netdev_ops->ndo_do_ioctl
          (ndev, &ifr, WOAL_SETONEINT_GETONEINT);
    if (!ret)
        rt_kprintf("reassociation: %s\n",
                   enabled ? "enabled" : "disabled");

    return ret;
}
#endif

rt_err_t mwifi_set_region(struct net_device *ndev, const char *country)
{
    union iwreq_data uwrq;
    char cmd_buf[sizeof("COUNTRY XX ")];
    int cmd_len = 0;
    rt_err_t ret = -RT_ERROR;

    RT_ASSERT(ndev);

    /* ISO 3166-1 alpha-2 country code:
    US, CA, SG, EU, AU, KR, FR, CN, JP */
    cmd_len += sprintf(cmd_buf + cmd_len, "COUNTRY ");
    cmd_len += snprintf(cmd_buf + cmd_len, strlen("XX "), "%s", country);
    uwrq.data.pointer = cmd_buf;
    uwrq.data.length = cmd_len;
    ret = ndev->wireless_handlers->standard
          [IW_IOCTL_IDX(SIOCSIWPRIV)](ndev, NULL, &uwrq, NULL);
    if (!ret)
        rt_kprintf("Region changed\n");

    return ret;
}
#endif  /* MARVELLWIFI_USING_STA */
#ifdef RT_USING_FINSH
#include <finsh.h>

int cmd_mwifi(int argc, char **argv)
{
    struct rt_device *dev;
    struct net_device *ndev;
    int i = 0, ret = 0;

    if (argc == 1)
    {
#ifdef MARVELLWIFI_USING_UAP
        rt_kprintf("\nUsage(UAP): ");
        rt_kprintf("\nmwifi uapX config <ssid> [-k key] [-c channel] [-m max]");
        rt_kprintf("\n           start");
        rt_kprintf("\n           stop");
        rt_kprintf("\n           getsta");
        rt_kprintf("\n           delsta <index>");
        rt_kprintf("\n");
#endif
#ifdef MARVELLWIFI_USING_STA
        rt_kprintf("\nUsage(STA): ");
        rt_kprintf("\nmwifi mlanX connect <ssid> [-k key]");
        rt_kprintf("\n            disconnect");
        rt_kprintf("\n            scan");
        rt_kprintf("\n            pwrsave [-e] [-d]");
        rt_kprintf("\n            reassoc [-e] [-d]");
        rt_kprintf("\n            region <country>");
        rt_kprintf("\n");
#endif
        goto done;
    }

#ifdef MARVELLWIFI_USING_UAP
    if (!strncmp("uap", argv[1], strlen("uap")) &&
            ((dev = rt_device_find(argv[1])) != RT_NULL) &&
            ((ndev = dev->user_data) != RT_NULL))
    {
        if (!strcmp("config", argv[2]) && argc > 3)
        {
            const char *key = NULL;
            int channel = 0;
            int max_sta = 0;

            for (i = 4; i < argc; i++)
            {
                switch (argv[i++][1])
                {
                case 'k':
                    key = argv[i];
                    break;
                case 'c':
                    channel = atoi(argv[i]);
                    break;
                case 'm':
                    max_sta = atoi(argv[i]);
                    break;
                default:
                    rt_kprintf("ignore: %s\n", argv[i - 1]);
                    break;
                }
            }
            mwifi_config(ndev, argv[3], key, channel, max_sta);
        }
        else if (!strcmp("start", argv[2]))
            mwifi_start(ndev);
        else if (!strcmp("stop", argv[2]))
            mwifi_stop(ndev);
        else if (!strcmp("getsta", argv[2]))
            mwifi_get_sta_list(ndev);
        else if (!strcmp("delsta", argv[2]) && argc == 4)
            mwifi_deauth_sta(ndev, atoi(argv[3]));
        else
            rt_kprintf("unsupported cmd: %s or too few arguments\n", argv[2]);
    }
    else
#endif
#ifdef MARVELLWIFI_USING_STA
        if (!strncmp("mlan", argv[1], strlen("mlan")) &&
                ((dev = rt_device_find(argv[1])) != RT_NULL) &&
                ((ndev = dev->user_data) != RT_NULL))
        {
            if (!strcmp("connect", argv[2]) && argc > 3)
            {
                const char *key = NULL;

                for (i = 4; i < argc; i++)
                {
                    switch (argv[i++][1])
                    {
                    case 'k':
                        key = argv[i];
                        break;
                    default:
                        rt_kprintf("ignore: %s\n", argv[i - 1]);
                        break;
                    }
                }
                mwifi_connect(ndev, argv[3], key);
            }
            else if (!strcmp("disconnect", argv[2]))
                mwifi_disconnect(ndev);
            else if (!strcmp("scan", argv[2]))
                mwifi_scan(ndev);
            else if (!strcmp("pwrsave", argv[2]))
            {
                int enabled = -1;

                for (i = 3; i < argc; i++)
                {
                    switch (argv[i++][1])
                    {
                    case 'e':
                        enabled = 1;
                        break;
                    case 'd':
                        enabled = 0;
                        break;
                    default:
                        rt_kprintf("ignore: %s\n", argv[i - 1]);
                        break;
                    }
                }
                mwifi_powersave(ndev, enabled);
            }
#if defined(REASSOCIATION)
            else if (!strcmp("reassoc", argv[2]))
            {
                int enabled = -1;

                for (i = 3; i < argc; i++)
                {
                    switch (argv[i++][1])
                    {
                    case 'e':
                        enabled = 1;
                        break;
                    case 'd':
                        enabled = 0;
                        break;
                    default:
                        rt_kprintf("ignore: %s\n", argv[i - 1]);
                        break;
                    }
                }
                mwifi_reassociation(ndev, enabled);
            }
#endif
            else if (!strcmp("region", argv[2]) && argc == 4)
                mwifi_set_region(ndev, argv[3]);
            else
                rt_kprintf("unsupported cmd: %s or too few arguments\n", argv[2]);
        }
        else
#endif
        {
            rt_kprintf("invalid network interface: %s\n", argv[1]);
        }

done:
    return ret;
}
MSH_CMD_EXPORT_ALIAS(cmd_mwifi, mwifi, Marvell WiFi commands);
#endif /* RT_USING_FINSH */
#endif  /* MARVELLWIFI_USING_UAP || MARVELLWIFI_USING_STA */
