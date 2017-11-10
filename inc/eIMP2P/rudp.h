/*
 * rudp.h
 *
 *  Created on: 2013-2-28
 *      Author: Administrator
 */

#ifndef RUDP_H_
#define RUDP_H_

#ifdef WIN32
#include <winsock2.h>
#endif

#if !defined(P2P_API_)
# if defined(WIN32)
#  if defined(P2P_EXPORTS)
#   define P2P_API_(TYPE) extern "C" _declspec(dllexport) TYPE __stdcall
#  else
#   define P2P_API_(TYPE) extern "C" _declspec(dllimport) TYPE __stdcall
#  endif
# else
#  define P2P_API_(TYPE)
# endif
#endif

#define RUDP_MSG_MAX_LEN						1464	/** peer与peer之间往返消息长度限制. */
#define RUDP_TYPE_CLIENT						0x01	/** rudp作为客户端(仅用于子网直通时). */
#define RUDP_TYPE_SERVER						0x02	/** rudp作为服务端(仅用于子网直通时). */

/**
 * rudp环境初始化. 在进程上下文中, 函数应该调且只调用一次.
 * return, 0表示成功, 其它为失败, 失败时, 下面的函数都不可使用.
 *
 * */
P2P_API_(int) rudp_setup();

/**
 *
 * 尝试建立子网环境下的rudp连接.
 * 参数host, 本地IP, 如果不关心, 置为NULL.
 * 参数port, 本地UDP端口.
 * 参数token, peer与peer之间的通信凭证, 32字节字符串.
 * 参数type, 可能取值是RUDP_TYPE_CLIENT | RUDP_TYPE_SERVER.
 * return, 成功时, 返回一个大于0的rudp通道描述字(fd), 否则返回-1, 表示失败.
 *
 * */
P2P_API_(int) rudp_scinit(char* host, unsigned port, char* token, unsigned char type);

/**
 *
 * 尝试建立公网环境下的rudp连接.
 * 参数host, 本地IP, 如果不关心, 置为NULL.
 * 参数port, 本地UDP端口.
 * 参数token, peer与peer之间的通信凭证, 32字节字符串.
 * 参数p2ps, p2p-server的IP地址.
 * 参数p2psport, p2p-server的监听端口.
 * return, 成功时, 返回一个大于0的rudp通道描述字(fd), 否则返回-1, 表示失败.
 *
 * */
P2P_API_(int) rudp_p2pinit(char* host, unsigned port, char* token, char* p2ps, unsigned short p2psport);

/**
 *
 *  reliable recv. 可靠与有序的消息接收. 在连接建立的情况下, 函数总是阻塞, 直到收到一个消息.
 *  参数fd, rudp通道描述字.
 *  参数buff, 接收消息缓冲区, 为避免缓冲区溢出, 它的长度应该 >= RUDP_MSG_MAX_LEN.
 *  参数size, 接收消息缓冲区的长度.
 *  return, 成功时, 返回实际收到的消息长度, 否则如果buff和size参数合法, 返回-1表示连接已经断开.
 *
 * */
P2P_API_(int) rudp_rrecv(int fd, unsigned char* buff, int size);

/**
 *  reliable send. 与rudp_rrecv成对使用, 在连接建立, 且消息还未到达缓冲区之前, 函数总是阻塞.
 *  因此, 当函数返回时, 并不表示消息已经到达对端. 而是仅仅保证会尽力投递.
 *  参数fd, rudp通道描述字.
 *  参数buff, 要发送的消息.
 *  参数size, 要发送的消息长度.
 *  return, 成功时, 返回size, 否则返回-1, 表示连接已经断开.
 *
 * */
P2P_API_(int) rudp_rsend(int fd, unsigned char* buff, int size);


/**
*
*  获得已到达接收缓冲区, 但未被take的消息数.
*  参数fd, rudp通道描述字.
*  return, 成功时, 返回未被take的消息数, 否则返回-1, 表示连接已经断开.
* */
P2P_API_(int) rudp_rqsize(int fd);


/**
 *
 * 获得通道类型.
 * 参数fd, rudp通道描述字.
 * return, 如果通道还在的话, 可能返回值是: RUDP_TYPE_CLIENT | RUDP_TYPE_SERVER, 否则返回-1.
 *
 * */
P2P_API_(int) rudp_type(int fd);

/**
 *
 *  关闭rudp连接, 函数会尽力发送残留在缓冲区中的消息, 直到发送缓冲区为空, 或连接断开时, 或在30秒后缓冲区仍有残留时返回.
 *  参数fd, rudp通道描述字.
 *	return, 0表示成功, 否则表示无此描述字.
 * */
P2P_API_(int) rudp_close(int fd);


extern "C" _declspec(dllexport) int rudp_sock(int fd);
//
#endif /* RUDP_H_ */
