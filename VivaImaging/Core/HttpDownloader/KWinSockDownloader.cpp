#include "stdafx.h"

#include "Platform/Platform.h"

#include "KString.h"
#include "KWString.h"
#include "PlatformLib.h"
#include "KWinSockDownloader.h"

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

// #define FAKE_DOWNLOAD_DEBUG
#define _DEBUG_DOWNLOADER


#if defined(USE_SOCKET_DOWNLOADER)

static void SocketThreadInit()
{
#ifndef _AFXDLL
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE
#define _afxSockThreadState AfxGetModuleThreadState()

  _AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
  if (pState->m_pmapSocketHandle == NULL)
      pState->m_pmapSocketHandle = new CMapPtrToPtr;
  if (pState->m_pmapDeadSockets == NULL)
      pState->m_pmapDeadSockets = new CMapPtrToPtr;
  if (pState->m_plistSocketNotifications == NULL)
      pState->m_plistSocketNotifications = new CPtrList;

#endif
}

/**********************************
 class KWinSockDownloader
**********************************/

KWinSockDownloader::KWinSockDownloader()
{
	m_buffer = NULL;
	m_socket = NULL;
	m_bodyBegin = 0;
	m_bodyLength = 0;
	m_bodyWritten = 0;

  m_processedPt = 0;
	m_buffer = NULL;
  m_Result = 0;

  mFolderCount = 0;
  mFileCount = 0;

  m_SocketBuffer = NULL;

#ifdef USE_SCAN_BY_WORKER_THREAD
  SocketThreadInit();
#endif

	m_socket = K2P_SocketCreate(m_socket);
}

KWinSockDownloader::~KWinSockDownloader()
{
	Clear();
}

bool KWinSockDownloader::StartDownloadFile(const KString& in_url)
{
#ifdef USE_DOWNLOAD_CACHE
	// check if already cached.
	if(m_DownloadItemMap.findCacheFile(m_currentCacheFile, in_url))
	{
		OnDownloadFileDone(in_url);
		return false; // to process next file
	}
#endif
	m_state = DownloaderInit;

	m_currentDownloadUrl = in_url;
	m_currentRedirection = false;

	if( ConnectDownloadSocket(in_url) )
	{
		m_ReceiveBuffer.Seek(0, begin);
		m_ReceiveBuffer.Close();

  #ifdef STORE_TO_FILE
		m_DownloadItemMap.readyCacheFilename(m_currentCacheFile, in_url);
		KTRACE("Download cache file[%s]\n", m_currentCacheFile.data());
#endif
		return true;
	}
	else
		return false;
}

void KWinSockDownloader::Clear()
{
	KDownloader::Clear();
	if(m_socket != NULL)
	{
		CloseDownloadSocket(true);

		K2P_SocketDelete(m_socket);
		m_socket = NULL;
	}
	if(m_buffer != NULL)
	{
		delete m_buffer;
		m_buffer = NULL;
	}

  if (m_SocketBuffer != NULL)
  {
    delete[] m_SocketBuffer;
    m_SocketBuffer = NULL;
  }

	m_ReceiveBuffer.Initialize();
#ifdef USE_DOWNLOAD_CACHE
	m_DownloadItemMap.clear();
#endif
}

bool KWinSockDownloader::ConnectDownloadSocket(const KString& in_url)
{
	K2PSocketError err;

#ifdef _DEBUG_URL
	int i, count;
	LPTSTR str;

	static char* test_urls[] = 
	{
		"http://www.k2.co.kr/mylog#content1",
		"http://www.k2.co.kr:82/mylog#content1",
		"http://quest:12345@www.k2.co.kr:82/mylog#content1",
		"ftp://quest:12345@www.k2.co.kr:82/mylog#upload?12",
		"/mydata/mylog#content1",
		"mailto:ya@k2.com",
		"https://www.k2.co.kr/mylog#content1",
		"news://www.k2.co.kr/news#today",
	};

	count = sizeof(test_urls) / sizeof(char*);
	for(i = 0; i < count; i++)
	{
		str.assign(test_urls[i]);
		url.SetUrl(str);
		KTRACE("Source:%s\n", str.data());
		url.CreateUrl(str);
		KTRACE("Result:%s\n", str.data());
	}
#endif

	m_url.SetUrl(in_url);

	if( (err = K2P_SocketConnect(m_socket, m_url.m_HostName, m_url.m_PortNumber)) == K2P_Socket_NoError)
	{
		m_state = DownloaderConnecting;
		m_waitReceiveLast = K2P_GetSystemTimeTick();
		return true;
	}

	if( (err == K2P_SocketError_NotReady) || 
		(err == K2P_SocketError_NetDown) || 
		(err == K2P_SocketError_NoSocket) ||
		(err == K2P_SocketError_NoBuffer) )
	{
		KTRACE("Connect error stop download\n");
		m_bContinue = false;
	}
	return false;
}

#define SOCKET_BUFFER_SIZE 8192

void KWinSockDownloader::ContinueDownload()
{
	int rtn;

	if(m_socket == NULL)
	{
		m_bContinue = false;
		return;
	}

	if(m_state == DownloaderConnecting)
	{
    DWORD time;

    if (K2P_SocketHasConnect(m_socket) == 2) // connect error
    {
      m_state = DownloaderFail;
    }
		else if( K2P_SocketHasConnect(m_socket) )
    {
			m_state = DownloaderConnected;
    }
		else if( (time = K2P_GetSystemTimeAfter(m_waitReceiveLast)) > HTTP_CONNECT_WAIT_TIME)
		{
			KTRACE("Closed by No OnConnected : %s, timeout=%d\n", m_currentDownloadUrl, time);
		}
	}
	else if(m_state == DownloaderSendRequest)
	{
		if( K2P_SocketHasSend(m_socket) )
		{
			m_waitReceiveLast = K2P_GetSystemTimeTick();
			m_state = DownloaderWaitReceive;
		}
	}

	if(m_state == DownloaderConnected)
	{
		if(K2P_SocketSendRequestEnable())
		{
			KString str;
      m_url.CreateUrl(str);
#ifdef _DEBUG_DOWNLOADER
			KTRACE("Request URL:%s\n", str);
#endif
			if((rtn = K2P_SocketSendRequest(m_socket, str.data())) == K2P_Socket_NoError)
			{
  			m_waitReceiveLast = K2P_GetSystemTimeTick();
				m_state = DownloaderSendRequest;
				return;
			}
		}
		else
		{
			KString str;
      m_url.CreateRequest(str, m_sessionKey);

#ifdef _DEBUG_DOWNLOADER
			KTRACE("Request:%s\n", str);
#endif

      if( (rtn = K2P_SocketSend(m_socket, (LPBYTE)str.data(), str.length())) == K2P_Socket_NoError)
			{
  			m_waitReceiveLast = K2P_GetSystemTimeTick();
				m_state = DownloaderSendRequest;
				return;
			}
		}
	}
	else if(m_state == DownloaderWaitReceive)
	{
		if( true ) // K2P_SocketHasReceive(m_socket) )
		{
      if (m_SocketBuffer == NULL)
        m_SocketBuffer = new BYTE[SOCKET_BUFFER_SIZE];

			while(m_bContinue && (m_state == DownloaderWaitReceive))
			{
				rtn = K2P_SocketReceive(m_socket, m_SocketBuffer, SOCKET_BUFFER_SIZE);
				if(rtn > 0)
				{
					m_waitReceiveLast = K2P_GetSystemTimeTick();
					m_bContinue = OnReceived(m_SocketBuffer, rtn);

#ifdef _DEBUG
					if(!m_bContinue)
						KTRACE("OnReceived stop download\n");
#endif
				}
				else if(rtn == 0)
				{
					if( K2P_SocketReceiveDone(m_socket) )
						m_state = DownloaderDone;

					if( K2P_GetSystemTimeAfter(m_waitReceiveLast) > HTTP_STREAM_WAIT_TIME )
					{
						KTRACE("Closed by No OnReceived : %s\n", m_currentDownloadUrl.data());
						m_state = (m_buffer != NULL) ? DownloaderDone : DownloaderFail;
            if(m_httpHeader.m_Result > 0)
              SetResult(m_httpHeader.m_Result); // HTTP error
						m_bContinue = false;
					}
					return;
				}
				else
				{
					if(K2P_SockethasClose(m_socket))
					{
            /*
						if( (rtn == K2P_SocketError_NotReady) || 
							(rtn == K2P_SocketError_NetDown) || 
							(rtn == K2P_SocketError_NoSocket) ||
							(rtn == K2P_SocketError_NoBuffer) )
              */
            if (rtn != K2P_Socket_NoError)
						{
							m_bContinue = false;
							KTRACE("OnReceived error stop download\n");
						}

						return;
					}

					// no receive until 6000 milli second, cancel download
					if( K2P_GetSystemTimeAfter(m_waitReceiveLast) > HTTP_STREAM_WAIT_TIME )
					{
						KTRACE("Closed by No OnReceived : %s\n", m_currentDownloadUrl.data());
						m_state = (m_buffer != NULL) ? DownloaderDone : DownloaderFail;
            if(m_httpHeader.m_Result > 0)
              SetResult(m_httpHeader.m_Result); // HTTP error
					}

					return; // return to timer. it need on SHP.
				}
			}
		}
	}
}

bool KWinSockDownloader::OnReceived(LPBYTE buff, int length)
{
	// save to file m_currentCacheFile
#ifdef _DEBUG_DOWNLOADER
	KTRACE("OnReceived(%d bytes)\n", length);
  /*
  char dump[17 * 5] = { 0 };
  for(int i =0 ; i <length; i++)
  {
    char val[5];
    sprintf_s(val, 5, "%.2X,", buff[i]);
    strcat_s(dump, 17 * 4, val);
    if ((i & 0x00F) == 0x00F)
    {
      KString dstr;

      dstr.assign((const char *)&(buff[i-15]), 16);
      strcat_s(dump, 17 * 5, " : ");
      strcat_s(dump, 17 * 5, dstr);
      strcat_s(dump, 17 * 5, "\n");
      KTRACE(dump);

      dump[0] = 0;
    }
  }
  if (strlen(dump) > 0)
    KTRACE(dump);
  */
#endif

	bool with_http_header = K2P_SocketReceiveWithHeader();

	if(with_http_header)
	{
		m_ReceiveBuffer.Write(buff, length);

		// find end-of header
		if(m_bodyBegin == 0)
		{
			LPBYTE p;

			p = KHttpHeader::findHeaderEnd(m_ReceiveBuffer.Data(), length);
			if(p > 0)
			{
				int headerLen;
				KString ext;

				headerLen = (int) (p - m_ReceiveBuffer.Data());
#ifdef _DEBUG_DOWNLOADER
				KTRACE("Http Header Length = %d\n", headerLen);
#endif

				m_bodyBegin = headerLen + 4;
				m_httpHeader.setHeader(m_ReceiveBuffer.Data(), headerLen);
				m_bodyLength = m_httpHeader.getContentLength();

				if(m_httpHeader.isRedirection())
				{
					m_currentRedirection = true;
#ifdef USE_DOWNLOAD_CACHE
					m_DownloadItemMap.insertRedirection(m_currentDownloadUrl, m_httpHeader.m_RedirectionLocation);
#endif
					DownloadRequest(m_httpHeader.m_RedirectionLocation);

#ifdef _DEBUG_DOWNLOADER
					KTRACE("Redirection (%s -> %s)\n", m_currentDownloadUrl.data(), m_httpHeader.m_RedirectionLocation.data());
#endif
				}
				// skip HTTP header
				buff += m_bodyBegin;

#ifdef USE_DOWNLOAD_CACHE
				if(m_currentRedirection)
					m_currentCacheFile.clear();
				else
				{
					ext = m_httpHeader.getContentTypeExtension();
					if(ext.length() > 0)
						ChangeFileExtension(m_currentCacheFile, ext);
					CheckUniqueFilename(m_currentCacheFile);
				}
#endif

#ifdef _DEBUG_DOWNLOADER
				KTRACE("Header Length=%d, BodyData(%d)\n", headerLen, m_ReceiveBuffer.Size() - m_bodyBegin);
#endif
			}
		}

		if((m_bodyBegin <= 0) || (m_ReceiveBuffer.Size() <= m_bodyBegin))
			return true;
	}
	else
	{
		m_ReceiveBuffer.Write(buff, length);
	}

	if(!m_currentRedirection)
	{
		if(m_ReceiveBuffer.Size() > m_bodyBegin)
    {
		  if(m_buffer == NULL)
		  {
  #ifdef STORE_TO_FILE
			  m_buffer = new KFile(m_currentCacheFile);
			  if( !m_buffer->Open(WriteOnly, 1) )
			  {
  #ifdef _DEBUG_DOWNLOADER
				  KString local_name;
				  KWString wfilename;

				  wfilename.assignUtf8(m_currentCacheFile);
				  local_name = wfilename.toLocal8Bit();
				  KTRACE("Cannot create cache file(%s)\n", local_name.data());
  #endif
				  m_bReadyNextFile = true;
				  m_state = DownloaderDone;
				  return false; // finished
			  }

			  if(m_ReceiveBuffer.Size() > m_bodyBegin)
			  {
				  m_bodyWritten = m_ReceiveBuffer.Size() - m_bodyBegin;
				  m_buffer->Write(buff, m_bodyWritten);
			  }
  #else
				m_bodyWritten = m_ReceiveBuffer.Size() - m_bodyBegin;
        AttachData(m_ReceiveBuffer.Data() + m_bodyBegin, m_bodyWritten);
        /*
			  m_buffer = new KMemoryFile();
				m_bodyWritten = m_ReceiveBuffer.Size() - m_bodyBegin;
        if (m_bodyWritten > 0)
          m_buffer->Write(m_ReceiveBuffer.Data() + m_bodyBegin, m_bodyWritten);
        */
  #endif
		  }
		  else
		  {
			  // m_buffer->Write(buff, length);
        AttachData(buff, length);
			  m_bodyWritten += length;
		  }
    }
	}
	else
  {
		m_bodyWritten += length;
  }

	if(with_http_header)
	{
		if((m_bodyLength >= 0) && (m_bodyWritten >= m_bodyLength))
    {
			m_state = DownloaderDone;

#ifdef _DEBUG_DOWNLOADER
      KTRACE("Body Length=%d Completed\n", m_bodyLength);
#endif

      int result = GetResult();
      if (result != 200)
      {
#ifdef _DEBUG_DOWNLOADER
        KTRACE("HTTP result %d\n", result);
#endif

        LPTSTR recv_data = MakeUnicodeString((LPCSTR)m_ReceiveBuffer.Data(), m_ReceiveBuffer.Size());
        if (recv_data != NULL)
        {
          OutputDebugString(recv_data);
          delete[] recv_data;
        }
      }
    }
	}
	return true;
}

LPBYTE findChunkEndOfLine(LPBYTE s, LPBYTE e)
{
  while((s+2) < e)
  {
    if ((s[0] == 0x0D) && (s[1] == 0x0A))
      return s;
    s++;
  }
  return NULL;
}

void KWinSockDownloader::AttachData(LPBYTE stream, int length)
{
  if (m_buffer == NULL)
    m_buffer = new KMemoryFile();

  if (m_httpHeader.isChunked())
  {
    if (m_processedPt == 0)
      m_processedPt = m_bodyBegin;

    int len = m_ReceiveBuffer.Size() - m_processedPt;
    LPBYTE p = m_ReceiveBuffer.Data() + m_processedPt;
    KString str;

    while(len > 4)
    {
      // check chunk block
      LPBYTE e = findChunkEndOfLine(p, m_ReceiveBuffer.Data() + m_ReceiveBuffer.Size());
      if (e == NULL)
      {
        OutputDebugString(_T("cannot find end of chunk size\n"));
        break;
      }

      str.assign((const char *)p, (int)(e - p));
      int block_size = str.toInt(NULL, 16);
      int h_size = (e - p) + 2;
      
#ifdef _DEBUG
      LPBYTE ppp = m_ReceiveBuffer.Data() + m_processedPt;
#endif

      if ((len - h_size)  >= (int)block_size)
      {
#ifdef _DEBUG
        KTRACE("Chunked block(%d)\n", block_size);
#endif
        p += h_size;

        len -= h_size + (int)block_size + 2;
        m_processedPt += h_size + (int) block_size + 2;

        if (block_size > 0)
        {
          m_buffer->Write(p, block_size);
          OnAttachData((const char *)p, block_size);
#ifdef _DEBUG
          // check end of block
          if ((p[block_size] != 0x0D) || (p[block_size+1] != 0x0A))
            KTRACE("Chunked block eof CR-LF not found\n");
#endif

          p += block_size;
          p += 2; // end of data tab
        }
        else // end of chunk
        {
          if (TRUE) // (len - h_size) == (int)block_size)
          {
            m_bodyLength = m_buffer->Size();
            break;
          }
          p += 2; // end of data tab
        }
      }
      else
      {
        break;
      }
    }
  }
  else
  {
    m_buffer->Write(stream, length);
    OnAttachData((const char *)stream, length);
  }
}

void KWinSockDownloader::OnAttachData(const char* data, int length)
{
  while(length > 0)
  {
    LPCSTR e = strchr(data, SCAN_ITEM_DELEMETER);
    if ((data < e) && (e < data + length))
    {
      int block_len = e - data + 1;
      if (*data == 'F')
        mFolderCount++;
      else if(*data == 'D')
        mFileCount++;

      if (block_len > 0)
      {
        length -= block_len;
        data += block_len;
      }
      else
      {
        break;
      }
    }
    else
      break;
  }
}

BOOL KWinSockDownloader::GetProcessedFolderFileCount(int& folders, int& files)
{
  folders = mFolderCount;
  files = mFileCount;
  return TRUE;
}

BOOL KWinSockDownloader::IsCloseDone()
{
  if (K2P_SockethasClose(m_socket))
  {
    m_state = DownloaderInit;
    return TRUE;
  }

  K2P_SocketClose(m_socket);
  return FALSE;
}

void KWinSockDownloader::CloseDownloadSocket(bool failed)
{
  if (DownloaderInit == m_state)
    return;

  if((m_Result == 0) && (m_httpHeader.m_Result != 0))
  {
    m_Result = m_httpHeader.m_Result;
  }


	K2P_SocketClose(m_socket);
  m_state = DownloaderInit;

	m_ReceiveBuffer.Seek(0, begin);
	m_ReceiveBuffer.Close();

	m_bodyBegin = 0;
	m_bodyLength = 0;
	m_bodyWritten = 0;

	#ifdef _DEBUG_DOWNLOADER
  #ifdef STORE_TO_FILE
	if(m_buffer != NULL)
	{
		m_buffer->Close();
		delete m_buffer;
		m_buffer = NULL;
	}

  KString localName;
	KWString wname;

	if(m_currentCacheFile.size() > 0)
	{
		wname.assignUtf8(m_currentCacheFile);
		localName = wname.toLocal8Bit();
		if(failed)
			KTRACE("Download fail(%s)\n", localName.data());
		else
			KTRACE("Download success(%s)\n", localName.data());
	}
#else
	if(failed)
  	KTRACE("Download fail(%s)\n", m_currentDownloadUrl.data());
  else
  	KTRACE("Download success(%s)\n", m_currentDownloadUrl.data());
#endif // STORE_TO_FILE
#endif // _DEBUG_DOWNLOADER

	KDownloader::CloseDownloadSocket(failed);
}

LPBYTE KWinSockDownloader::GetDownloadData(long& length)
{
  LPBYTE r = NULL;
#ifndef STORE_TO_FILE
  if (m_buffer != NULL)
    length = ((KMemoryFile*) m_buffer)->Size();
  else
    length = 0;
	if((m_buffer != NULL) && (length > 0))
  {
    r = new BYTE[length];
    memcpy(r, ((KMemoryFile*) m_buffer)->Data(), length);
  }
#endif //
  return r;
}

#endif //
