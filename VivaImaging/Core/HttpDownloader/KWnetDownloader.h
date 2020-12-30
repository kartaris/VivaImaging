/**
* @file KWnetDownloader.h
* @date 2016.10
* @brief KWnetDownloader class header file
*/

#ifndef K_WNET_DOWNLOADER_H
#define K_WNET_DOWNLOADER_H

#include <WinInet.h>
#include <vector>

#include "KDownloader.h"

#define USE_WNET_DOWNLOADER

#define HTTPS_STREAM_WAIT_TIME		6000	// if there is no stream receive during this time. stop download

#define SCAN_ITEM_DELEMETER '\b'
#define SCAN_VALUE_DELEMETER '|'

#if defined(USE_WNET_DOWNLOADER)

/**
* @class KWnetDownloader
* @brief getModifiedFolderNDocument() API 사용을 위한 HTTP/HTTPS 연결 및 다운로드 기능을 수행하는 클래스.
* WinINET 라이브러리를 사용하여 구현됨.
*/
class KWnetDownloader : public KDownloader
{
public:
	KWnetDownloader();
	virtual ~KWnetDownloader();

	virtual void Clear();
	virtual bool StartDownloadFile(const KString& in_url);
	virtual void ContinueDownload();
	virtual void CloseDownloadSocket(bool failed);
  virtual BOOL IsCloseDone();

  virtual LPBYTE GetDownloadData(long& length);
  virtual int GetResult() { if (m_Result != 0) return m_Result; return m_httpHeader.m_Result; }
  virtual void SetResult(int r) { m_Result = r; }

  virtual KDownloaderError GetErrorCode() { return m_httpHeader.GetErrorCodeValue(); }
  virtual LPCSTR GetErrorMessage() { return m_httpHeader.GetErrorMessage(); }

	bool ConnectDownloadSocket(const KString& in_url);
	bool OnReceived(LPBYTE buff, int length);
  void AttachData(LPBYTE stream, int length);

  int mFolderCount;
  int mFileCount;
  void OnAttachData(const char* data, int length);
  virtual BOOL GetProcessedFolderFileCount(int& folders, int& files);

	HINTERNET m_internetHandle;
  HINTERNET m_internetConnect;
  HINTERNET m_internetRequest;

  KFile* m_buffer;
	KUrl m_url;

	KHttpHeader m_httpHeader;
  
	KMemoryFile m_ReceiveBuffer;
	int m_headerLength;
	int m_bodyBegin;
	int m_bodyLength;
	int m_bodyWritten;

  int m_processedPt;
  int m_Result;
	DWORD m_waitReceiveLast;

  BYTE* m_SocketBuffer;

};
#endif //USE_WNET_DOWNLOADER

#endif // K_WNET_DOWNLOADER_H

