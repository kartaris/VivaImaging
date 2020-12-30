#include "stdafx.h"

#include "Core/KSettings.h"

#include "PlatformLib.h"
#include "KHttpSocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

#define _DEBUG_HTTP_SOCKET

KHttpSocket::KHttpSocket()
{
	Init();
}

KHttpSocket::~KHttpSocket()
{

}

void KHttpSocket::Init()
{
	m_bReceive = 0;
	m_bSend = 0;
	m_bAccept = 0;
	m_bConnected = 0;
	m_bClose = 0;
	m_bOutOfBandData = 0;
}

void TraceSocketErrorMessage(int nErrorCode)
{
  static LPTSTR socketErrorMsg[] = {
    _T("Unknown socket error."),
    _T("The specified address is already used."),
    _T("The specified address is not available from the local machine."),
    _T("Addresses in the specified family cannot be used with this socket."),
    _T("The attempt to connect was forcefully rejected."),
    _T("A destination address is required."),
    _T("The lpSockAddrLen argument is incorrect."),
    _T("The socket is already bound to an address."),
    _T("The socket is already connected."),
    _T("No more file descriptors are available."),
    _T("The network cannot be reached from this host at this time."),
    _T("No buffer space is available. The socket cannot be connected."),
    _T("The socket is not connected."),
    _T("The descriptor is a file, not a socket."),
    _T("The attempt to connect timed out without establishing a connection.")
  };

  int i = 0;
  if (nErrorCode == WSAEADDRINUSE)
    i = 1;
  else if (nErrorCode == WSAEADDRNOTAVAIL)
    i = 2;
  else if (nErrorCode == WSAEAFNOSUPPORT)
    i = 3;
  else if (nErrorCode == WSAECONNREFUSED)
    i = 4;
  else if (nErrorCode == WSAEDESTADDRREQ)
    i = 5;
  else if (nErrorCode == WSAEFAULT)
    i = 6;
  else if (nErrorCode == WSAEINVAL)
    i = 7;
  else if (nErrorCode == WSAEISCONN)
    i = 8;
  else if (nErrorCode == WSAEMFILE)
    i = 9;
  else if (nErrorCode == WSAENETUNREACH)
    i = 10;
  else if (nErrorCode == WSAENOBUFS)
    i = 11;
  else if (nErrorCode == WSAENOTCONN)
    i = 12;
  else if (nErrorCode == WSAENOTSOCK)
    i = 13;
  else if (nErrorCode == WSAETIMEDOUT)
    i = 14;

  StoreLogHistory(__FUNCTIONW__, socketErrorMsg[i], SYNC_MSG_LOG);

  OutputDebugString(socketErrorMsg[i]);
  OutputDebugString(_T("\n"));
}

void KHttpSocket::OnReceive(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnReceive(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
    TraceSocketErrorMessage(nErrorCode);
#endif
	m_bReceive = 1;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnReceive(nErrorCode);
#endif
}

void KHttpSocket::OnSend(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnSend(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
    TraceSocketErrorMessage(nErrorCode);
#endif
	m_bSend = 1;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnSend(nErrorCode);
#endif
}

void KHttpSocket::OnAccept(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnAccept(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
    TraceSocketErrorMessage(nErrorCode);
#endif
	m_bAccept = 1;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnAccept(nErrorCode);
#endif
}

void KHttpSocket::OnConnect(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnConnect(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
  {
    TraceSocketErrorMessage(nErrorCode);
    K2P_SocketClose(this);
  }
#endif
	m_bConnected = (nErrorCode == 0) ? 1 : 2;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnConnect(nErrorCode);
#endif
}

void KHttpSocket::OnClose(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnClose(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
    TraceSocketErrorMessage(nErrorCode);
#endif
	m_bClose = 1;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnClose(nErrorCode);
#endif
}

void KHttpSocket::OnOutOfBandData(int nErrorCode)
{
#ifdef _DEBUG_HTTP_SOCKET
	KTRACE("OnOutOfBandData(err=%d)\n", nErrorCode);
  if (nErrorCode != 0)
    TraceSocketErrorMessage(nErrorCode);
#endif
	m_bOutOfBandData = 1;
#ifdef PLATFORM_WIN32
	CAsyncSocket::OnOutOfBandData(nErrorCode);
#endif
}
