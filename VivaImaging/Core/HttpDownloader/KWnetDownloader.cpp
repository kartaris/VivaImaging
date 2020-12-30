#include "stdafx.h"

#include "../../Platform/Platform.h"
#include "../../Core/KSettings.h"

#include "KString.h"
#include "KWString.h"
#include "PlatformLib.h"
#include "KWnetDownloader.h"

#include <Strsafe.h>

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

// #define FAKE_DOWNLOAD_DEBUG
#ifdef _DEBUG
#define _DEBUG_DOWNLOADER
#endif

#if defined(USE_WNET_DOWNLOADER)

/**********************************
 class KWnetDownloader
**********************************/

KWnetDownloader::KWnetDownloader()
{
	m_internetHandle = NULL;
  m_internetConnect = NULL;
  m_internetRequest = NULL;

  m_buffer = NULL;
  m_headerLength = 0;
	m_bodyBegin = 0;
	m_bodyLength = 0;
	m_bodyWritten = 0;

  m_processedPt = 0;
	m_buffer = NULL;
  m_Result = 0;

  mFolderCount = 0;
  mFileCount = 0;

  m_url.SetPortNumber(INTERNET_DEFAULT_HTTPS_PORT);

  m_SocketBuffer = NULL;
}

KWnetDownloader::~KWnetDownloader()
{
	Clear();
}

bool KWnetDownloader::StartDownloadFile(const KString& in_url)
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

void KWnetDownloader::Clear()
{
	KDownloader::Clear();

  if (m_internetRequest != NULL)
  {
  ::InternetCloseHandle(m_internetRequest);
  m_internetRequest = NULL;
  }
  if (m_internetConnect != NULL)
  {
    ::InternetCloseHandle(m_internetConnect);
    m_internetConnect = NULL;
  }
  if (m_internetHandle != NULL)
  {
    ::InternetCloseHandle(m_internetHandle);
    m_internetHandle = NULL;
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

#ifdef _DEBUG_DOWNLOADER

void __stdcall kWnetStatusCallMaster(
    HINTERNET hInternet,
    DWORD_PTR dwContext,
    DWORD dwInternetStatus,
    LPVOID lpvStatusInformation,
    DWORD dwStatusInformationLength
)
{
  TCHAR szStatusText[80] = { 0 };

    switch (dwInternetStatus)
    {
        case INTERNET_STATUS_CLOSING_CONNECTION:
            StringCchCopy( szStatusText, 80, _T("CLOSING_CONNECTION"));
            break;
        case INTERNET_STATUS_CONNECTED_TO_SERVER:
            StringCchCopy( szStatusText, 80, _T("CONNECTED_TO_SERVER"));
            break;
        case INTERNET_STATUS_CONNECTING_TO_SERVER:
            StringCchCopy( szStatusText, 80, _T("CONNECTING_TO_SERVER"));
            break;
        case INTERNET_STATUS_CONNECTION_CLOSED:
            StringCchCopy( szStatusText, 80, _T("CONNECTION_CLOSED"));
            break;
        case INTERNET_STATUS_HANDLE_CLOSING:
            StringCchCopy( szStatusText, 80, _T("HANDLE_CLOSING"));
            break;
        case INTERNET_STATUS_HANDLE_CREATED:
            StringCchCopy( szStatusText, 80, _T("HANDLE_CREATED"));
            break;
        case INTERNET_STATUS_INTERMEDIATE_RESPONSE:
            StringCchCopy( szStatusText, 80, _T("INTERMEDIATE_RESPONSE"));
            break;
        case INTERNET_STATUS_NAME_RESOLVED:
            StringCchCopy( szStatusText, 80, _T("NAME_RESOLVED"));
            break;
        case INTERNET_STATUS_RECEIVING_RESPONSE:
            StringCchCopy( szStatusText, 80, _T("RECEIVING_RESPONSE"));
            break;
        case INTERNET_STATUS_RESPONSE_RECEIVED:
            StringCchCopy( szStatusText, 80, _T("RESPONSE_RECEIVED"));
            break;
        case INTERNET_STATUS_REDIRECT:
            StringCchCopy( szStatusText, 80, _T("REDIRECT"));
            break;
        case INTERNET_STATUS_REQUEST_COMPLETE:
            StringCchCopy( szStatusText, 80, _T("REQUEST_COMPLETE"));
            break;
        case INTERNET_STATUS_REQUEST_SENT:
            StringCchCopy( szStatusText, 80, _T("REQUEST_SENT"));
            break;
        case INTERNET_STATUS_RESOLVING_NAME:
            StringCchCopy( szStatusText, 80, _T("RESOLVING_NAME"));
            break;
        case INTERNET_STATUS_SENDING_REQUEST:
            StringCchCopy( szStatusText, 80, _T("SENDING_REQUEST"));
            break;
        case INTERNET_STATUS_STATE_CHANGE:
            StringCchCopy( szStatusText, 80, _T("STATE_CHANGE"));
            break;
        default:
            StringCchPrintf( szStatusText, 80, _T("%d : Unknown Status"), dwInternetStatus);
            break;
    }

    if (lstrlen(szStatusText) > 0)
    {
      OutputDebugString(szStatusText);
      OutputDebugString(_T("\n"));
    }
}
#endif


#ifdef HTTP_MSG_SEND_TEST
  TCHAR HttpClientModuleName[] = _T("LocalBackupMsgSendTest");
#else
  TCHAR HttpClientModuleName[] = _T("LocalSyncClient");
#endif

static LPWSTR HTTP_REQ_GET = L"GET";
static LPWSTR HTTP_REQ_POST = L"POST";
const WCHAR* post_acceptTypes[] = { L"application/x-www-form-urlencoded", NULL};

bool KWnetDownloader::ConnectDownloadSocket(const KString& in_url)
{
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

	m_url.SetUrl(in_url, FALSE);

  BOOL connected = FALSE;
  m_internetHandle = ::InternetOpen(HttpClientModuleName, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
  if (m_internetHandle != NULL) 
  {
#ifdef _DEBUG_DOWNLOADER
    ::InternetSetStatusCallback(m_internetHandle, (INTERNET_STATUS_CALLBACK)kWnetStatusCallMaster);
#endif

    // open HTTP session
    LPTSTR hostName = MakeUnicodeString(m_url.m_HostName);
    int port = (m_url.m_Scheme == KProtocol_HTTPS) ? INTERNET_DEFAULT_HTTPS_PORT : INTERNET_DEFAULT_HTTP_PORT;
    if (m_url.m_PortNumber > 0)
      port = m_url.m_PortNumber;

    m_internetConnect = ::InternetConnect(m_internetHandle,
      hostName, 
      port,
      NULL, NULL,
      INTERNET_SERVICE_HTTP, 0, NULL);

    if (m_internetConnect != NULL) 
    {
      LPTSTR request_method = HTTP_REQ_GET;

      // use POST method
      KString optional_str;
      const WCHAR** acceptTypes = NULL;

      if (m_url.m_ExtraInfo.length() > 1024)
      {
        m_url.GetOptionalString(optional_str);
        request_method = HTTP_REQ_POST;
        acceptTypes = post_acceptTypes;
      }

      // open request
      KString path;
      m_url.GetAddressString(path);
      LPTSTR pathStr = MakeUnicodeString(path);

      DWORD inet_flag = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
      if (m_url.m_Scheme == KProtocol_HTTPS)
        inet_flag |= INTERNET_FLAG_SECURE | INTERNET_FLAG_IGNORE_CERT_CN_INVALID;

      m_internetRequest = ::HttpOpenRequest(m_internetConnect, request_method, 
        pathStr,
        NULL, /* http version */
        NULL, /* referer */
        NULL, /* accept type */
        inet_flag, // INTERNET_FLAG_KEEP_CONNECTION,
        1);

      if (m_internetRequest != NULL)
      {
        KString header_str;
        m_url.GetHeaderString(header_str, (optional_str.length() > 0), m_sessionKey);

        LPWSTR headerStr = MakeUnicodeString(header_str);
        // send request
        if (request_method == HTTP_REQ_GET)
        {
          connected = ::HttpSendRequest(m_internetRequest, headerStr, lstrlen(headerStr), NULL, 0);
        }
        else
        {
          connected = ::HttpSendRequest(m_internetRequest, headerStr, lstrlen(headerStr),
            (LPVOID)optional_str.data(), optional_str.length());
        }

		    TCHAR msg[256] = _T("HttpRequest path=");
        StringCchCat(msg, 256, pathStr);
        /*
        if (headerStr != NULL)
        {
          StringCchCat(msg, 256, _T(", header="));
          StringCchCat(msg, 256, headerStr);
        }
        */
        if (optional_str.length() > 0)
        {
          LPWSTR pOptional = MakeUnicodeString(optional_str.data());
          StringCchCat(msg, 256, _T(", optional="));
          StringCchCat(msg, 256, pOptional);
          delete[] pOptional;
        }
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

        if (headerStr != NULL)
          delete[] headerStr;
      }
      if (pathStr != NULL)
        delete[] pathStr;
    }

    if (hostName != NULL)
      delete[] hostName;
  }
	if(!connected)
	{
		TCHAR msg[256];
    if (m_internetRequest == NULL)
    StringCchPrintf(msg, 256, _T("Connect error stop download(error=%d)"), GetLastError());
    else
      StringCchPrintf(msg, 256, _T("Request error stop download(error=%d)"), GetLastError());
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
		m_bContinue = false;

    OutputDebugString(msg);
    OutputDebugString(_T("\n"));
	}
  else
  {
		m_waitReceiveLast = K2P_GetSystemTimeTick();
    m_state = DownloaderWaitReceive;
  }
	return m_bContinue;
}

#define SOCKET_BUFFER_SIZE 8192

void KWnetDownloader::ContinueDownload()
{
	int rtn;

  if(m_state == DownloaderWaitReceive)
	{
		if( true ) // K2P_SocketHasReceive(m_socket) )
		{
      if (m_headerLength == 0)
      {
        DWORD dwSize = 0;
        LPBYTE headerBuffer = NULL;
        HttpQueryInfo(m_internetRequest, HTTP_QUERY_RAW_HEADERS_CRLF, headerBuffer, &dwSize, 0);
        if (dwSize > 0)
        {
          headerBuffer = new BYTE[dwSize+1];
          if (HttpQueryInfo(m_internetRequest, HTTP_QUERY_RAW_HEADERS_CRLF, headerBuffer, &dwSize, 0))
          {
            // convert to utf-8
            LPSTR pheader = MakeUtf8String((LPCTSTR)headerBuffer);
            if (pheader != NULL)
            {
              m_headerLength = strlen(pheader);
              m_httpHeader.setHeader((LPBYTE)pheader, m_headerLength);
				      m_bodyLength = m_httpHeader.getContentLength();
              delete[] pheader;
            }
          }
          delete[] headerBuffer;
        }
        KTRACE("InternetReadHeader : %d\n", dwSize);
      }

      if (m_SocketBuffer == NULL)
      {
        m_SocketBuffer = new BYTE[SOCKET_BUFFER_SIZE+2];
        memset(m_SocketBuffer, 0, SOCKET_BUFFER_SIZE+2);
      }

			while(m_bContinue && (m_state == DownloaderWaitReceive))
			{
				// rtn = K2P_SocketReceive(m_socket, m_SocketBuffer, SOCKET_BUFFER_SIZE);
        DWORD dwByteRead = 0;
        rtn = ::InternetReadFile(m_internetRequest, m_SocketBuffer, SOCKET_BUFFER_SIZE, &dwByteRead);
#ifdef _DEBUG_DOWNLOADER
				KTRACE("InternetReadFile : %d\n", dwByteRead);
#endif
				if(rtn != 0)
				{
          if (dwByteRead > 0)
          {
					  m_waitReceiveLast = K2P_GetSystemTimeTick();
					  m_bContinue = OnReceived(m_SocketBuffer, (int)dwByteRead);
#ifdef _DEBUG_DOWNLOADER
					  if(!m_bContinue)
						  KTRACE("OnReceived stop download\n");
#endif
          }
          else
          {
            // end of data
						m_state = ((m_buffer != NULL) || (m_headerLength > 0)) ? DownloaderDone : DownloaderFail;
            if(m_httpHeader.m_Result > 0)
              SetResult(m_httpHeader.m_Result); // HTTP error
            else
              SetResult(200); // temporary
            m_bContinue = false;
            return;
          }
				}
				else
				{
          // error or pending

          if(m_httpHeader.m_Result > 0)
          {
            SetResult(m_httpHeader.m_Result); // HTTP error
					  m_bContinue = false;
					  m_state = (m_buffer != NULL) ? DownloaderDone : DownloaderFail;
          }
          else
          {
					  if( K2P_GetSystemTimeAfter(m_waitReceiveLast) > HTTPS_STREAM_WAIT_TIME )
					  {
						  // KTRACE("Closed by No OnReceived : %s\n", m_currentDownloadUrl.data());

		          TCHAR msg[256] = _T("Closed by No OnReceived : ");
              LPTSTR urlStr = MakeUnicodeString(m_currentDownloadUrl);
              StringCchCat(msg, 256, urlStr);
              delete[] urlStr;
              StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);

						  m_state = (m_buffer != NULL) ? DownloaderDone : DownloaderFail;
            }
          }
					return;
				}
			}
		}
	}
}

bool KWnetDownloader::OnReceived(LPBYTE buff, int length)
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

	bool with_http_header = FALSE;

	m_ReceiveBuffer.Write(buff, length);

	if(!m_currentRedirection)
	{
    AttachData(buff, length);
		m_bodyWritten += length;
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

extern LPBYTE findChunkEndOfLine(LPBYTE s, LPBYTE e);

void KWnetDownloader::AttachData(LPBYTE stream, int length)
{
  if (m_buffer == NULL)
    m_buffer = new KMemoryFile();

#if 0 // chunked data will automatically translated by WinINET
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
#endif
  {
    m_buffer->Write(stream, length);
    OnAttachData((const char *)stream, length);
  }
}

void KWnetDownloader::OnAttachData(const char* data, int length)
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

BOOL KWnetDownloader::GetProcessedFolderFileCount(int& folders, int& files)
{
  folders = mFolderCount;
  files = mFileCount;
  return TRUE;
}

BOOL KWnetDownloader::IsCloseDone()
{
  if (m_state == DownloaderInit)
    return TRUE;
  /*
  if (K2P_SockethasClose(m_socket))
  {
    m_state = DownloaderInit;
    return TRUE;
  }

  K2P_SocketClose(m_socket);
  */
  return FALSE;
}

void KWnetDownloader::CloseDownloadSocket(bool failed)
{
  if (DownloaderInit == m_state)
    return;

  if((m_Result == 0) && (m_httpHeader.m_Result != 0))
  {
    m_Result = m_httpHeader.m_Result;
  }

  if (m_internetRequest != NULL)
  {
  ::InternetCloseHandle(m_internetRequest);
  m_internetRequest = NULL;
  }
  if (m_internetConnect != NULL)
  {
    ::InternetCloseHandle(m_internetConnect);
    m_internetConnect = NULL;
  }
  if (m_internetHandle != NULL)
  {
    ::InternetCloseHandle(m_internetHandle);
    m_internetHandle = NULL;
  }

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

  KString dump = m_currentDownloadUrl.left(150);

	if(failed)
  	KTRACE("Download fail(%s)\n", dump.data());
  else
  	KTRACE("Download success(%s)\n", dump.data());
#endif // STORE_TO_FILE
#endif // _DEBUG_DOWNLOADER

	if(failed)
  {
	  TCHAR msg[256] = _T("Download fail : ");
    LPTSTR urlStr = MakeUnicodeString(m_currentDownloadUrl);
    StringCchCat(msg, 256, urlStr);
    delete[] urlStr;
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
  }

	KDownloader::CloseDownloadSocket(failed);
}

LPBYTE KWnetDownloader::GetDownloadData(long& length)
{
  LPBYTE r = NULL;
#ifndef STORE_TO_FILE
  if (m_buffer != NULL)
    length = ((KMemoryFile*) m_buffer)->Size();
  else
    length = 0;
	if((m_buffer != NULL) && (length > 0))
  {
    // test for bad 
#ifdef _DEBUG_DOUBLE_ENC
    length += 5;
    r = (LPBYTE)GlobalAlloc(GMEM_FIXED, length+1);
    memcpy(r, "a3c\r\n", 5);
    memcpy(r+5, ((KMemoryFile*) m_buffer)->Data(), length-5);
#else
    r = (LPBYTE)GlobalAlloc(GMEM_FIXED, length+1);
    memcpy(r, ((KMemoryFile*) m_buffer)->Data(), length);
#endif
    r[length] = '\0';
  }
#endif //
  return r;
}

#endif //
