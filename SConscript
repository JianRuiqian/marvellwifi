Import('RTT_ROOT')
Import('rtconfig')
from building import *

# get current directory
cwd = GetCurrentDir()
define = []

src = Split('''
mwifi.c
mport/mwifi_dev.c
mport/firmware.c
mport/skbuff.c
mport/wext-core.c
mlinux/moal_main.c
mlinux/moal_ioctl.c
mlinux/moal_shim.c
mlinux/moal_eth_ioctl.c
mlinux/moal_sdio_mmc.c
mlan/mlan_shim.c
mlan/mlan_init.c
mlan/mlan_txrx.c
mlan/mlan_cmdevt.c
mlan/mlan_misc.c
mlan/mlan_cfp.c
mlan/mlan_wmm.c
mlan/mlan_sdio.c
mlan/mlan_11n_aggr.c
mlan/mlan_11n_rxreorder.c
mlan/mlan_11n.c
mlan/mlan_11d.c
mlan/mlan_11h.c
''')

sta_src = Split('''
mlan/mlan_meas.c
mlan/mlan_scan.c
mlan/mlan_sta_ioctl.c
mlan/mlan_sta_rx.c
mlan/mlan_sta_tx.c
mlan/mlan_sta_event.c
mlan/mlan_sta_cmd.c
mlan/mlan_sta_cmdresp.c
mlan/mlan_join.c
''')

sta_wext = Split('''
mlinux/moal_priv.c
mlinux/moal_wext.c
''')

if GetDepend(['MARVELLWIFI_USING_STA']):
    define += ['STA_SUPPORT']
    sta_src += sta_wext
    src += sta_src

uap_src = Split('''
mlan/mlan_uap_ioctl.c
mlan/mlan_uap_cmdevent.c
mlan/mlan_uap_txrx.c
mlinux/moal_uap.c
''')

uap_wext = Split('''
mlinux/moal_uap_priv.c
mlinux/moal_uap_wext.c
''')

if GetDepend(['MARVELLWIFI_USING_UAP']):
    define += ['UAP_SUPPORT']
    uap_src += uap_wext
    src += uap_src

path = [cwd + '/mlinux', 
        cwd + '/mlan']

group = DefineGroup('MarvellWiFi', src, depend = ['RT_USING_LWIP','RT_USING_SDIO','PKG_USING_WLANMARVELL'], CPPPATH = path, CPPDEFINES = define)

Return('group')
