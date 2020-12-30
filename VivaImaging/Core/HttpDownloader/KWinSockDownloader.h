#ifndef K_HTTP_DOWNLOADER_H
#define K_HTTP_DOWNLOADER_H

#include "KDownloader.h"

#ifdef USE_DOWNLOAD_CACHE
#include "KCacheFileManager.h"
#endif

#define USE_SOCKET_DOWNLOADER
#define SUPPORT_SUBFILE_DOWNLOAD

#include <vector>

#define HTTP_CONNECT_WAIT_TIME		6000	// if there is no connect answer until this time. stop download
#define HTTP_STREAM_WAIT_TIME		6000	// if there is no stream receive during this time. stop download

#define SCAN_ITEM_DELEMETER '\b'
#define SCAN_VALUE_DELEMETER '|'

#if defined(USE_SOCKET_DOWNLOADER)

class KWinSockDownloader : public KDownloader
{
public:
	KWinSockDownloader();
	virtual ~KWinSockDownloader();

	virtual void Clear();
	virtual bool StartDownloadFile(const KString& in_url);
	virtual void ContinueDownload();
	virtual void CloseDownloadSocket(bool failed);
  virtual BOOL IsCloseDone();

  virtual LPBYTE GetDownloadData(long& length);
  virtual int GetResult() { if (m_Result != 0) return m_Result; return m_httpHeader.m_Result; }
  virtual void SetResult(int r) { m_Result = r; }

  KDownloaderError GetErrorCode() { return m_httpHeader.GetErrorCodeValue(); }
  LPCSTR GetErrorMessage() { return m_httpHeader.GetErrorMessage(); }

	bool ConnectDownloadSocket(const KString& in_url);
	bool OnReceived(LPBYTE buff, int length);
  void AttachData(LPBYTE stream, int length);

  int mFolderCount;
  int mFileCount;
  void OnAttachData(const char* data, int length);
  virtual BOOL GetProcessedFolderFileCount(int& folders, int& files);

	KFile* m_buffer;
	HANDLE m_socket;
	KUrl m_url;

	KHttpHeader m_httpHeader;
  
	KMemoryFile m_ReceiveBuffer;
	int m_bodyBegin;
	int m_bodyLength;
	int m_bodyWritten;

  int m_processedPt;
  int m_Result;
	DWORD m_waitReceiveLast;

  BYTE* m_SocketBuffer;

};

#elif defined(USE_DOWNLOAD_AGENT_DOWNLOADER)

class KDownloadAgentDownloader : public KDownloader
{
public:
	KDownloadAgentDownloader();
	virtual ~KDownloadAgentDownloader();

	virtual void Clear();
	virtual bool StartDownloadFile(const KString& in_url);
	virtual void ContinueDownload();
	virtual void CloseDownloadSocket(bool failed);

	int m_client_id;
	int m_request_id;
};

#endif

#endif // K_HTTP_DOWNLOADER_H

