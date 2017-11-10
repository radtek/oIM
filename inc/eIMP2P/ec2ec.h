/*
 * ec2ec.h
 *
 *  Created on: 2013-4-19
 *      Author: Administrator
 */

#ifndef EC2EC_H_
#define EC2EC_H_

typedef struct
{
	int fd; /** 文件描述符. */
	unsigned long long int size; /** 文件实际大小. */
	unsigned long long int ssize; /** 实际发送的字节数. */
	unsigned long long int rsize; /** 实际接收的字节数. */
#define PPP_FILE_PATH_MAX										0x200	/** 最长文件路径. */
	char name[PPP_FILE_PATH_MAX]; /** 文件名. */
} ppp_file; /** 文件传输任务. */

typedef struct
{
	unsigned short stid; /** 源事务id, 仅用于安全通道建立过程. */
	unsigned short dtid; /** 目的事务id, 仅用于安全通道建立过程. */
#define PPPS_RANDOM_LEN											0x10	/** 随机码固定长度. */
#define PPPS_PASSWD_LEN											0x10	/** 密码固定长度. */
#define PPPS_SHA512_LEN											0x40	/** SHA-512加密后的长度. */
	unsigned char pwd[PPPS_PASSWD_LEN]; /** 密码. */
	/*-------------------------*/
	/*                         */
	/*       安全通道状态                  */
	/*                         */
	/*-------------------------*/
#define PPP_SEC_STAT_INIT										0x00	/** 初始状态. */
#define PPP_SEC_STAT_WAUR										0x01	/** 等待AUTH_REQ. */
#define PPP_SEC_STAT_WAUP										0x02	/** 等待AUTH_RSP. */
#define PPP_SEC_STAT_AUHC										0x03	/** 鉴权规程完成, 可进行消息收发. */
	unsigned char stat;
	unsigned char rpwd[PPPS_RANDOM_LEN]; /** 随机 码. */
	unsigned char ssalt[PPPS_RANDOM_LEN]; /** 本地盐值.*/
	unsigned char psalt[PPPS_RANDOM_LEN]; /** 对端盐值. */
	unsigned char cryp[PPPS_SHA512_LEN + 4 + PPPS_RANDOM_LEN]; /** 对端的鉴权加密块(由AUTH_REQ携带). */
	unsigned int sseq; /** 自增序列, 起始于零, 终止于0xFFFFFFFF, 循环使用, 用于出栈消息.  */
	unsigned int pseq; /** 对端序列. */
} ppp_sec; /** 安全通道. */

typedef struct
{
	int fd; /** rudp通道. */
	unsigned short stid; /** 与事件有关的源事务id. */
	unsigned short dtid; /** 与事件有关的目的事务id. */
	/*-------------------------*/
	/*                         */
	/*      任务与管道事件               */
	/*                         */
	/*-------------------------*/
#define PPP_EVENT_PIPE											0x01	/** rudp通道断开. */
#define PPP_EVENT_TASK_END										0x02	/** 任务正常结束. */
#define PPP_EVENT_TASK_ABT										0x03	/** 任务异常结束. */
	/*-------------------------*/
	/*                         */
	/*       安全通道类                       */
	/*                         */
	/*-------------------------*/
#define PPP_EVENT_SEC_AURQ										0x40	/** 鉴权请求事件. */
#define PPP_EVENT_SEC_AUHF										0x41	/** 鉴权失败. */
#define PPP_EVENT_SEC_AUHC										0x42	/** 安全传输通道已确立. */
	/*-------------------------*/
	/*                         */
	/*         文件类                            */
	/*                         */
	/*-------------------------*/
#define PPP_EVENT_FILE_SREQ										0xF0	/** 发送文件请求. */
#define PPP_EVENT_FILE_ACFM										0xF1	/** 确认接收文件. */
#define PPP_EVENT_FILE_PREJ										0xF2	/** 对方拒绝. */
#define PPP_EVENT_FILE_PFAI										0xF3	/** 对方同意, 但异常. */
#define PPP_EVENT_FILE_PCAN										0xF4	/** 对端取消. */
#define PPP_EVENT_FILE_ADDS										0xF5	/** 添加发送文件任务成功. */
	/** 文件类事件值. */
#define PPP_EVAL_FILE_RSEC										0xF0	/** 仅对PPP_EVENT_FILE_PREJ事件有效, 表示因安全原因对端客户进程自动拒绝. */
#define PPP_EVAL_FILE_RITV										0xF1	/** 仅对PPP_EVENT_FILE_PREJ事件有效, 表示对端主动拒绝. */
#define PPP_EVAL_FILE_CNOM										0xF0	/** 仅对PPP_EVENT_FILE_PCAN事件有效, 表示对方主动取消了文件发送. */
#define PPP_EVAL_FILE_CTOF										0xF1	/** 仅对PPP_EVENT_FILE_PCAN事件有效, 表示对方主动取消了文件发送, 并改用离线发送. */
#define PPP_EVAL_FILE_COTH										0xF2	/** 仅对PPP_EVENT_FILE_PCAN事件有效, 表示因异常原因(如, 无法创建文件)导致对方无法接收文件, 由进程内部取消. */
	/*-------------------------*/
	/*                         */
	/*          音频类                         */
	/*                         */
	/*-------------------------*/

	/*-------------------------*/
	/*                         */
	/*          视频类                         */
	/*                         */
	/*-------------------------*/
	//
	unsigned char evn; /** 事件. */
	unsigned char val; /** 可能的事件描述. */
	unsigned char spare;
#define PPP_TYPE_SEC											0x10	/** 安全通道. */
#define PPP_TYPE_MSG											0x20	/** 消息. */
#define PPP_TYPE_FILE											0xF0	/** 文件. */
#define PPP_TYPE_AUDIO											0x30	/** 音频. */
#define PPP_TYPE_VIDEO											0x40	/** 视频. */
	unsigned char type; /** 事件类型. */
	union
	{
		ppp_file file;
		ppp_sec sec;
	};
} ppp_event;
/*-----------------------------------------------------*/
/*                                                     */
/*                                                     */
/*                                                     */
/*-----------------------------------------------------*/
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


/** 点对点通信功能初始化, return 0表示成功. */
P2P_API_(int) ec2ec_init();

/** 获得事件, return 0表示成功, event被设置.  */
P2P_API_(int) ec2ec_event(ppp_event* event);

/** 关注fd上的事件. return 0表示成功. */
P2P_API_(int) ec2ec_reg_fd(int fd);

/*-----------------------------------------------------*/
/*                                                     */
/*                  安全通道相关API                     */
/*                                                     */
/*-----------------------------------------------------*/

/** 使能安全通道. return 0仅表示任务提交成功, 需等待后续事件. */
P2P_API_(int) ec2ec_sec_enable(int fd, char* key);

/** 如果在调用ec2ec_sec_enable初始化之前, 收到了对端使能安全传输请求(PPP_EVENT_SSL_AURQ), 则调用此函数. */
P2P_API_(int) ec2ec_sec_auth(ppp_event* event, char* key);

/** 在安全通道上发送一个消息. */
P2P_API_(int) ec2ec_sec_send(int fd, unsigned char* dat, int size);

/*-----------------------------------------------------*/
/*                                                     */
/*                  文件传输相关API                     */
/*                                                     */
/*-----------------------------------------------------*/
/** 在通道上发送一个文件. return 0表示任务添加成功, event被设置为PPP_EVENT_FILE_ADDS.  */
P2P_API_(int) ec2ec_file_send(int fd, char* file, ppp_event* event);

/** 同意接收文件. c == 0 时表示重新接收, 否则使用断点续传方式接收. */
P2P_API_(int) ec2ec_file_accept(ppp_event* event, char* file, unsigned char c);

/** 返回文件传输进度. return 0时size被设置. */
P2P_API_(int) ec2ec_file_progress(ppp_event* event, unsigned long long* size);

/** 拒绝接收文件, val可能的取值是PPP_EVAL_FILE_RSEC | PPP_EVAL_FILE_RITV */
P2P_API_(void) ec2ec_file_reject(ppp_event* event, unsigned char val);

/** 取消文件传输任务, val可能取值是PPP_EVAL_FILE_CNOM | PPP_EVAL_FILE_CTOF | PPP_EVAL_FILE_COTH. */
P2P_API_(void) ec2ec_file_cancel(ppp_event* event, unsigned char val);

#endif /* EC2EC_H_ */
