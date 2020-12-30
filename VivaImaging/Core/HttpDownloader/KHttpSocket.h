#ifndef _K_HTTP_SOCKET_H
#define _K_HTTP_SOCKET_H

#include "KString.h"
#include "KWString.h"
#include "KFile.h"
// #include <K2ComposerLibDef.h>

#ifdef WIN32
#include <afxsock.h>
#else

#endif

#ifdef WIN32
class KHttpSocket : public CAsyncSocket
#else
class KHttpSocket
#endif
{
public:
	KHttpSocket();
	virtual ~KHttpSocket();


	void Init();

	virtual void OnReceive(int nErrorCode);
	virtual void OnSend(int nErrorCode);
	virtual void OnAccept(int nErrorCode);
	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode);

	inline int hasConnect() { return m_bConnected; }
	inline int hasClose() { return m_bClose; }
	inline int hasSend() { return m_bSend; }
	inline int hasReceive() { return m_bReceive; }
	inline void clearReceive() { m_bReceive = 0; }

	unsigned int m_bReceive : 2;
	unsigned int m_bSend : 2;
	unsigned int m_bAccept : 2;
	unsigned int m_bConnected : 2;
	unsigned int m_bClose : 2;
	unsigned int m_bOutOfBandData : 2;
};

#endif // _K_HTTP_SOCKET_H

