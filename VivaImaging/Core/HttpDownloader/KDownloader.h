/**
* @file KDownloader.h
* @date 2016.10
* @brief KDownloader class header file
*/
#ifndef K_DOWNLOADER_H
#define K_DOWNLOADER_H

#include "KUrl.h"
#include "KHttpHeader.h"
#include "KFile.h"

#ifdef USE_DOWNLOAD_CACHE
#include "KCacheFileManager.h"
#endif

#define USE_SOCKET_DOWNLOADER
#define SUPPORT_SUBFILE_DOWNLOAD

#include <vector>

typedef std::vector<KString>		KDownloadQueue;

typedef enum 
{
	DownloaderInit,
	DownloaderConnecting,
	DownloaderConnected,
	DownloaderSendRequest,
	DownloaderWaitReceive,
	DownloaderReceive,
	DownloaderFail,
	DownloaderDone,
}DownloaderState;

/**
* @class KDownloader
* @brief getModifiedFolderNDocument() API 사용을 위한 연결 및 다운로드 기능을 수행하는 베이스 클래스.
* WinINET 라이브러리를 사용하는 KWnetDownloader와 winsock을 사용하는 KWinSockDownloader가 있다.
*/
class KDownloader
{
public:
	KDownloader();
	virtual ~KDownloader();

	virtual void Clear();

#ifdef SUPPORT_SUBFILE_DOWNLOAD
	KDownloadQueue m_RequestQueue;

#ifdef USE_DOWNLOAD_CACHE
	KCacheFileManager m_DownloadItemMap;
	KStringList m_DownloadCacheFiles;
#endif

	KStringList m_DownloadDoneFiles;

	KString m_currentCacheFile;

	bool m_bReadyNextFile;

	int m_RequestCount;
	int m_ReceiveCount;
	KStringList m_insertedTempImages;
#endif

	bool m_bContinue;
	DownloaderState m_state;
	KString m_currentDownloadUrl;
	bool m_currentRedirection;
	KString m_localDirectory;
	KString m_baseUrl;
  KString m_sessionKey;

	void SetLocalDirectory(const KString& dir) { m_localDirectory = dir; }
	void SetBaseUrl(const KString& dir) { m_baseUrl = dir; }

	bool ReadyFullPathUrl(KString& fullName, const KString& in_url);
	bool GetShortenName(const KString& in_url, KString& shortName);

  void EncodeBase64(char* buff, int buff_size, BYTE* src, int length);

	bool DownloadRequest(const KStringList& in_urlList);
	bool DownloadRequest(const KString& in_url);
  void SetSessionKey(const KString& key) { m_sessionKey = key; }
  inline DownloaderState GetState() { return m_state; }

  virtual int GetResult() { return 0; }
  virtual void SetResult(int r) { ; }

  virtual KDownloaderError GetErrorCode() { return SFS_NO_ERROR; }
  virtual LPCSTR GetErrorMessage() { return NULL; }

#ifdef USE_DOWNLOAD_CACHE
	KString& getCacheFolder() { return m_DownloadItemMap.getCacheFolder(); }
#else
  virtual LPBYTE GetDownloadData(long& length) { length = 0; return NULL; }
#endif

#ifdef SUPPORT_SUBFILE_DOWNLOAD
	virtual bool ProcessDownload(KStringList& out_completedItems);
	virtual bool StartDownloadFile(const KString& in_url);
	virtual void ContinueDownload();
	virtual void OnDownloadFileDone(const KString& in_url, bool bSuccess = true);
	virtual bool isDownloadDone();
#endif
	virtual void CloseDownloadSocket(bool failed);
  virtual BOOL IsCloseDone() { return TRUE; }

  virtual BOOL GetProcessedFolderFileCount(int& folders, int& files) { return FALSE; }

	// bool findCacheFile(KString& cacheFile, const KString& in_url);
	// void ReadyCacheFilename(KString& cacheFile, const KString& in_url);

	bool GetLocalCache(KString& filename, const KString& in_url);
	bool GetImageOriginalSize(const KString& in_url, SIZE& originSize);
	// save stream data as new file on cache folder.
	bool InsertImageStreamFile(KString& cacheFileName, const KString& name, LPBYTE buff, int length);
	// find image file from cache folder which has equal binary data.
	bool FindImageStreamFile(KString& cacheFileName, LPBYTE buff, int length);
};


#endif // K_DOWNLOADER_H

