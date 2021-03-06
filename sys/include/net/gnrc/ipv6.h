/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_gnrc_ipv6 IPv6
 * @ingroup     net_gnrc
 * @brief       GNRC's IPv6 implementation
 *
 * The IPv6 control thread understands messages of type
 *
 *  * @ref GNRC_NETAPI_MSG_TYPE_RCV, and
 *  * @ref GNRC_NETAPI_MSG_TYPE_SND,
 *
 * @{
 *
 * @file
 * @brief       Definitions for GNRC's IPv6 implementation
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */


#ifndef GNRC_IPV6_H
#define GNRC_IPV6_H

#include "kernel_types.h"
#include "net/gnrc.h"
#include "thread.h"

#include "net/ipv6.h"
#include "net/gnrc/ipv6/ext.h"
#include "net/gnrc/ipv6/hdr.h"
#include "net/gnrc/ipv6/nc.h"
#include "net/gnrc/ipv6/netif.h"

#ifdef MODULE_FIB
#include "net/fib.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default stack size to use for the IPv6 thread
 */
#ifndef GNRC_IPV6_STACK_SIZE
#define GNRC_IPV6_STACK_SIZE        (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * @brief   Default priority for the IPv6 thread
 */
#ifndef GNRC_IPV6_PRIO
#define GNRC_IPV6_PRIO              (THREAD_PRIORITY_MAIN - 3)
#endif

/**
 * @brief   Default message queue size to use for the IPv6 thread.
 */
#ifndef GNRC_IPV6_MSG_QUEUE_SIZE
#define GNRC_IPV6_MSG_QUEUE_SIZE    (8U)
#endif

#ifdef DOXYGEN
/**
 * @brief   Add a static IPv6 link local address to any network interface
 *
 * This macro allows to specify a certain link local IPv6 address to be assigned
 * to a network interface on startup, which might be handy for testing.
 * Note: a) a interface will keep its auto-generated link local address, too
 *       b) the address is incremented by 1, if multiple interfaces are present
 *
 * To use the macro just add it to `CFLAGS` in the application's Makefile, like:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * IPV6_STATIC_LLADDR ?= '"fe80::cafe:cafe:cafe:1"'
 * CFLAGS += -DGNRC_IPV6_STATIC_LLADDR=$(STATIC_IPV6_LLADDR)
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define GNRC_IPV6_STATIC_LLADDR
#endif /* DOXYGEN */

/**
 * @brief   The PID to the IPv6 thread.
 *
 * @note    Use @ref gnrc_ipv6_init() to initialize. **Do not set by hand**.
 *
 * @details This variable is preferred for IPv6 internal communication *only*.
 *          Please use @ref net_gnrc_netreg for external communication.
 */
extern kernel_pid_t gnrc_ipv6_pid;

#ifdef MODULE_FIB

/**
 * @brief   Maximum number of entries in the IPv6 FIB table.
 */
#ifndef GNRC_IPV6_FIB_TABLE_SIZE
#   ifdef MODULE_GNRC_RPL
#       define GNRC_IPV6_FIB_TABLE_SIZE     (20)
#   else
#       define GNRC_IPV6_FIB_TABLE_SIZE     (5)
#   endif
#endif

/**
 * @brief   The forwarding information base (FIB) for the IPv6 stack.
 *
 * @see @ref net_fib
 */
extern fib_table_t gnrc_ipv6_fib_table;
#endif

/**
 * @brief   Initialization of the IPv6 thread.
 *
 * @return  The PID to the IPv6 thread, on success.
 * @return  a negative errno on error.
 * @return  -EOVERFLOW, if there are too many threads running already
 * @return  -EEXIST, if IPv6 was already initialized.
 */
kernel_pid_t gnrc_ipv6_init(void);

/**
 * @brief   Demultiplexes a packet according to @p nh.
 *
 * @internal
 *
 * **Do not use outside this module or its submodules!!!**
 * Public access needed for Extension Headers.
 *
 * About `current` and `pkt`:
 *
 *                     current     pkt
 *                     |           |
 *                     v           v
 * IPv6 <- IPv6_EXT <- IPv6_EXT <- UNDEF
 *
 * This situation may happen when the packet has a source routing extension
 * header (RFC 6554), and the packet is forwarded from an interface to another.
 *
 * @param[in] iface     The receiving interface.
 * @param[in] current   A snip to process.
 * @param[in] pkt       A packet.
 * @param[in] nh        A protocol number (see @ref net_protnum) of the current snip.
 */
void gnrc_ipv6_demux(kernel_pid_t iface, gnrc_pktsnip_t *current, gnrc_pktsnip_t *pkt, uint8_t nh);

/**
 * @brief   Get the IPv6 header from a given list of @ref gnrc_pktsnip_t
 *
 *          This function may be used with e.g. a pointer to a (full) UDP datagram.
 *
 * @param[in] pkt    The pointer to the first @ref gnrc_pktsnip_t of the
 *                   packet.
 *
 * @return A pointer to the @ref ipv6_hdr_t of the packet.
 * @return NULL if the packet does not contain an IPv6 header.
 */
ipv6_hdr_t *gnrc_ipv6_get_header(gnrc_pktsnip_t *pkt);

#ifdef __cplusplus
}
#endif

#endif /* GNRC_IPV6_H */
/**
 * @}
 */
