/*
 * This file implement the Wireless Extensions core API.
 *
 * Authors :	Jean Tourrilhes - HPL - <jt@hpl.hp.com>
 * Copyright (c) 1997-2007 Jean Tourrilhes, All Rights Reserved.
 * Copyright	2009 Johannes Berg <johannes@sipsolutions.net>
 *
 * (As all part of the Linux kernel, this file is GPL)
 */
 
/*
 * Main event dispatcher. Called from other parts and drivers.
 * Send the event on the appropriate channels.
 * May be called from interrupt context.
 */

#include "netdevice.h"
#include "wireless.h"

void wireless_send_event(struct net_device *	dev,
			 unsigned int		cmd,
			 union iwreq_data *	wrqu,
			 const char *		extra)
{
	if (cmd == IWEVCUSTOM)
		printk("%s\n", extra);
}
