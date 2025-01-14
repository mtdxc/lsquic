////////////////////////////////////////////////////////////////////////
/// @file       socket_util.cpp
/// @brief      socket设置公共函数定义
/// @details    socket设置公共函数定义
/// @author     王超
/// @version    1.0
/// @date       2021/11/19
/// @copyright  (c) 2021-2031 。保留所有权利
////////////////////////////////////////////////////////////////////////
#include "socket_util.h"
#ifdef _WIN32
#include <winsock2.h>
#include <ws2ipdef.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip.h>
#include <error.h>
#endif

int set_fd_nonblocking(int fd)
{
#ifdef _WIN32
    unsigned long ul = 1;
    int ret = ioctlsocket(fd, FIONBIO, (unsigned long *)&ul);
    if (ret == SOCKET_ERROR)
        return -1;
#else
    int flags = fcntl(fd, F_GETFL);
    if (-1 == flags)
        return -1;
    flags |= O_NONBLOCK;
    if (0 != fcntl(fd, F_SETFL, flags))
        return -1;
#endif
    return 0;
}


void tut_proc_ancillary(struct msghdr *msg, 
                            struct sockaddr_storage *storage, int *ecn)
{
#ifndef _WIN32
    const struct in6_pktinfo *in6_pkt;
    struct cmsghdr *cmsg;

    for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg))
    {
        if (cmsg->cmsg_level == IPPROTO_IP &&
            cmsg->cmsg_type  ==
#if defined(IP_RECVORIGDSTADDR)
                                IP_ORIGDSTADDR
#else
                                IP_PKTINFO
#endif
                                              )
        {
#if defined(IP_RECVORIGDSTADDR)
            memcpy(storage, CMSG_DATA(cmsg), sizeof(struct sockaddr_in));
#else
            const struct in_pktinfo *in_pkt;
            in_pkt = (void *) CMSG_DATA(cmsg);
            ((struct sockaddr_in *) storage)->sin_addr = in_pkt->ipi_addr;
#endif
        }
        else if (cmsg->cmsg_level == IPPROTO_IPV6 &&
                 cmsg->cmsg_type  == IPV6_PKTINFO)
        {
            in6_pkt = (void *) CMSG_DATA(cmsg);
            ((struct sockaddr_in6 *) storage)->sin6_addr = in6_pkt->ipi6_addr;
        }
        else if ((cmsg->cmsg_level == IPPROTO_IP && cmsg->cmsg_type == IP_TOS)
                 || (cmsg->cmsg_level == IPPROTO_IPV6
                                            && cmsg->cmsg_type == IPV6_TCLASS))
        {
            memcpy(ecn, CMSG_DATA(cmsg), sizeof(*ecn));
            *ecn &= IPTOS_ECN_MASK;
        }
    }
#endif
}


/* Set up the socket to return original destination address in ancillary data */
int tut_set_origdst(int fd, const struct sockaddr *sa)
{
    int s;

#ifdef _WIN32
    char on = 1;
#else
    int on = 1;
#endif
    if (AF_INET == sa->sa_family)
        s = setsockopt(fd, IPPROTO_IP,
#if defined(IP_RECVORIGDSTADDR)
                                       IP_RECVORIGDSTADDR,
#else
                                       IP_PKTINFO,
#endif
                                                           &on, sizeof(on));
#ifndef _WIN32
    else
        s = setsockopt(fd, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on, sizeof(on));
#endif

    if (s != 0)
        perror("setsockopt");

    return s;
}

/* ToS is used to get ECN value */
int tut_set_ecn (int fd, const struct sockaddr *sa)
{
    int s;
#ifdef _WIN32
    char on = 1;
#else
    int on = 1;
#endif
    if (AF_INET == sa->sa_family)
        s = setsockopt(fd, IPPROTO_IP, IP_RECVTOS, &on, sizeof(on));
    else
        s = setsockopt(fd, IPPROTO_IPV6, IPV6_RECVTCLASS, &on, sizeof(on));
    if (s != 0)
        perror("setsockopt(ecn)");

    return s;
}
