//
// Created by 孙诚雄 on 2021/7/6.
//

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <cassert>
#include <arpa/inet.h>
#include "ConsoleStream.h"

#define OPT(v) ((v) == 0 ? "off" : "on")

#define CHECK_INT_OPT(sock, level, optname, flag) do {\
    int opt;\
    socklen_t len = sizeof(opt);\
    int rc = getsockopt(sock, level, optname, &opt, &len);\
    if (rc == 0) {\
        if (flag) {\
            RC_INFO << #level << ':' << #optname << ": "<< OPT(opt);\
        } else {\
            RC_INFO << #level << ':' << #optname << ": "<< opt;\
        }\
    } else {\
        RC_ERROR << #level << ':'<< #optname << " not found.";\
    }\
} while (0)

#define CHECK_LINGER_OPT(sock, level, optname) do {\
    linger lin;\
    socklen_t lin_len = sizeof(lin);\
    int rc = getsockopt(sock, level, optname, &lin, &lin_len);\
    if (rc == 0) {\
        RC_INFO << #level << ':' << #optname << ": l_linger = " << lin.l_linger << ", l_onoff = " << lin.l_onoff;\
    } else {\
        RC_ERROR << #level << ':'<< #optname << " not found.";\
    }\
} while (0)

#define CHECK_TIMEVAL_OPT(sock, level, optname) do {\
    timeval tv;\
    socklen_t tv_len = sizeof(tv);\
    int rc = getsockopt(sock, level, optname, &tv, &tv_len);\
    if (rc == 0) {\
        RC_INFO << #level << ':' << #optname << ": tv_sec = " << tv.tv_sec << ", tv_usec = " << tv.tv_usec;\
    } else {\
        RC_ERROR << #level << ':'<< #optname << " not found.";\
    }\
} while (0)

#define CHECK_IN_ADDR_OPT(sock, level, optname) do {\
    in_addr addr;\
    socklen_t addr_len = sizeof(addr);\
    int rc = getsockopt(sock, level, optname, &addr, &addr_len);\
    if (rc == 0) {\
        RC_INFO << #level << ':' << #optname << ": " << ::inet_ntoa(addr);\
    } else {\
        RC_ERROR << #level << ':'<< #optname << " not found.";\
    }\
} while (0)

#define CHECK_U_CHAR_OPT(sock, level, optname) do {\
    u_char ch;\
    socklen_t ch_len = sizeof(ch);\
    int rc = getsockopt(sock, level, optname, &ch, &ch_len);\
    if (rc == 0) {\
        RC_INFO << #level << ':' << #optname << ": " << ch;\
    } else {\
        RC_ERROR << #level << ':'<< #optname << " not found.";\
    }\
} while (0)

int main(int argc, const char *argv[]) {
    int sock = ::socket(PF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    assert(sock > 0);

    CHECK_INT_OPT(sock, SOL_SOCKET, SO_BROADCAST, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_DEBUG, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_DONTROUTE, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_ERROR, false);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_KEEPALIVE, true);

    CHECK_LINGER_OPT(sock, SOL_SOCKET, SO_LINGER);

    CHECK_INT_OPT(sock, SOL_SOCKET, SO_OOBINLINE, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_RCVBUF, false);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_SNDBUF, false);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_RCVLOWAT, false);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_SNDLOWAT, false);

    CHECK_TIMEVAL_OPT(sock, SOL_SOCKET, SO_RCVTIMEO);
    CHECK_TIMEVAL_OPT(sock, SOL_SOCKET, SO_SNDTIMEO);

    CHECK_INT_OPT(sock, SOL_SOCKET, SO_REUSEADDR, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_REUSEPORT, true);
    CHECK_INT_OPT(sock, SOL_SOCKET, SO_TYPE, false);

//    CHECK_INT_OPT(sock, SOL_SOCKET, SO_USELOOPBACK, true);

    CHECK_INT_OPT(sock, IPPROTO_IP, IP_HDRINCL, true);

    //TODO: IPPROTO_IP:IP_OPTIONS

//    CHECK_INT_OPT(sock, IPPROTO_IP, IP_RECVDSTADDR, true);

//    CHECK_INT_OPT(sock, IPPROTO_IP, IP_RECVIF, true);

    CHECK_INT_OPT(sock, IPPROTO_IP, IP_TOS, false);
    CHECK_INT_OPT(sock, IPPROTO_IP, IP_TTL, false);

    CHECK_IN_ADDR_OPT(sock, IPPROTO_IP, IP_MULTICAST_IF);
    CHECK_U_CHAR_OPT(sock, IPPROTO_IP, IP_MULTICAST_TTL);
    CHECK_U_CHAR_OPT(sock, IPPROTO_IP, IP_MULTICAST_LOOP);

    //FIXME: rc == -1, why?
    ip_mreq im;
    socklen_t im_len = sizeof(im);
//    rc = getsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &im, &im_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_ADD_MEMBERSHIP: imr_interface = " << ::inet_ntoa(im.imr_interface) << ", imr_multiaddr = " << ::inet_ntoa(im.imr_multiaddr);

    //FIXME: rc == -1, why?
//    rc = getsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &im, &im_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_DROP_MEMBERSHIP: imr_interface = " << ::inet_ntoa(im.imr_interface) << ", imr_multiaddr = " << ::inet_ntoa(im.imr_multiaddr);

    //FIXME: rc == -1, why?
    ip_mreq_source ims;
    socklen_t ims_len = sizeof(ims);
//    rc = getsockopt(sock, IPPROTO_IP, IP_BLOCK_SOURCE, &ims, &ims_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_BLOCK_SOURCE: imr_sourceaddr = " << ::inet_ntoa(ims.imr_sourceaddr)
//            << ", imr_interface = " << ::inet_ntoa(ims.imr_interface)
//            << ", imr_multiaddr = " << ::inet_ntoa(ims.imr_multiaddr);

    //FIXME: rc == -1, why?
//    rc = getsockopt(sock, IPPROTO_IP, IP_UNBLOCK_SOURCE, &ims, &ims_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_UNBLOCK_SOURCE: imr_sourceaddr = " << ::inet_ntoa(ims.imr_sourceaddr)
//            << ", imr_interface = " << ::inet_ntoa(ims.imr_interface)
//            << ", imr_multiaddr = " << ::inet_ntoa(ims.imr_multiaddr);

    //FIXME: rc == -1, why?
//    rc = getsockopt(sock, IPPROTO_IP, IP_ADD_SOURCE_MEMBERSHIP, &ims, &ims_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_ADD_SOURCE_MEMBERSHIP: imr_sourceaddr = " << ::inet_ntoa(ims.imr_sourceaddr)
//            << ", imr_interface = " << ::inet_ntoa(ims.imr_interface)
//            << ", imr_multiaddr = " << ::inet_ntoa(ims.imr_multiaddr);

    //FIXME: rc == -1, why?
//    rc = getsockopt(sock, IPPROTO_IP, IP_DROP_SOURCE_MEMBERSHIP, &ims, &ims_len);
//    assert(rc == 0);
//    RC_INFO << "IPPROTO_IP:IP_DROP_SOURCE_MEMBERSHIP: imr_sourceaddr = " << ::inet_ntoa(ims.imr_sourceaddr)
//            << ", imr_interface = " << ::inet_ntoa(ims.imr_interface)
//            << ", imr_multiaddr = " << ::inet_ntoa(ims.imr_multiaddr);

    //FIXME: icmp6_filter not found.
//    icmp6_filter filter;

    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_CHECKSUM, false);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_DONTFRAG, true);

    //TODO: IPV6_NEXTHOP, IPV6_PATHMTU

    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVDSTOPTS, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVHOPOPTS, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVPATHMTU, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVPKTINFO, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVRTHDR, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_RECVTCLASS, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_UNICAST_HOPS, false);
//    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_USE_MIN_MTU, true);
    CHECK_INT_OPT(sock, IPPROTO_IPV6, IPV6_V6ONLY, true);


    CHECK_INT_OPT(sock, IPPROTO_TCP, TCP_MAXSEG, false);
    CHECK_INT_OPT(sock, IPPROTO_TCP, TCP_NODELAY, true);

    return 0;
}

