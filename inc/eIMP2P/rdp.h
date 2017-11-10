#ifndef __RDP_H__
#define __RDP_H__

typedef struct tagNetInfo
{	// For RDP Query net information
	BYTE  u8Type;		// Type: RUDP_TYPE_CLIENT(1), RUDP_TYPE_SERVER(2)
	ULONG u32SelfIP;	// Self address
	WORD  u16SelfPort;	// Self port
	ULONG u32PeerIP;	// Peer address
	WORD  u16PeerPort;	// Peer port
}S_NetInfo, *PS_NetInfo;

// For RDP query net information
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

P2P_API_(bool) RdpQueryNetInfo(const char* host, unsigned port, const char* token, const char* p2ps, unsigned short p2psport, PS_NetInfo psNetInfo);
typedef bool (__stdcall* PFN_RdpQueryNetInfo)(const char*, unsigned, const char*, const char*, unsigned short, PS_NetInfo);

#endif /* __RDP_H__ */
