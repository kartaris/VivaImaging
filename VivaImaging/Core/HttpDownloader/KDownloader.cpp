#include "stdafx.h"

#include "../../Platform/Platform.h"

#include "KString.h"
#include "KWString.h"
#include "PlatformLib.h"
#include "KDownloader.h"

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

// #define FAKE_DOWNLOAD_DEBUG
#define _DEBUG_DOWNLOADER

/*************************************
 KDownloader
*************************************/
KDownloader::KDownloader()
{
#ifdef SUPPORT_SUBFILE_DOWNLOAD
	m_bReadyNextFile = true;
	m_state = DownloaderInit;
	m_RequestCount = 0;
	m_ReceiveCount = 0;
#endif
	m_bContinue = true;
	m_currentRedirection = FALSE;
}

KDownloader::~KDownloader()
{
#ifdef USE_DOWNLOAD_CACHE
	m_DownloadItemMap.saveCacheList();
	m_DownloadItemMap.clear();
#endif
	Clear();
}

void KDownloader::Clear()
{
#ifdef SUPPORT_SUBFILE_DOWNLOAD
	m_bReadyNextFile = true;
	m_state = DownloaderInit;
	m_RequestCount = 0;
	m_ReceiveCount = 0;
	m_RequestQueue.clear();
#endif

	m_bContinue = true;
}

void KDownloader::CloseDownloadSocket(bool failed)
{
	// m_currentCacheFile.clear();
}


static char base64Char(char c)
{
  if (c < 26)
    return(c + 'A');
  else if (c < 52)
    return(c - 26 + 'a');
  else if (c < 62)
    return(c - 52 + '0');
  else if (c == 62)
    return('+');
  else // if (c == 63)
    return('/');
}

void KDownloader::EncodeBase64(char* buff, int buff_size, BYTE* src, int length)
{
  while (length > 0)
  {
    char d[3];
    if (length >= 3)
    {
      memcpy(d, src, 3);
      length -= 3;
      src += 3;
    }
    else
    {
      memset(d, 0, 3);
      memcpy(d, src, length);
      src += length;
      length = 0;
    }

    if (buff_size > 4)
    {
      // first 6 bit
      *buff++ = base64Char((d[0] >> 2) & 0x3F);
      *buff++ = base64Char(((d[0] << 4) & 0x30) | ((d[1] >> 4) & 0x0F));
      *buff++ = base64Char(((d[1] << 2) & 0x3C) | ((d[2] >> 6) & 0x03));
      *buff++ = base64Char(d[2] & 0x03F);
      buff_size -= 4;
    }
    else
    {
      break;
    }
  }
  *buff = '\0';
}

bool KDownloader::DownloadRequest(const KStringList& in_urlList)
{
#ifdef SUPPORT_SUBFILE_DOWNLOAD
	int i;
	bool rtn = false;

	// check if already exist on queue
	for(i = 0; i < in_urlList.size(); i++)
	{
		if( DownloadRequest(in_urlList[i]) )
			rtn = true;
	}
	return rtn;
#else
	return false;
#endif
}

// return false if download failure.
bool KDownloader::DownloadRequest(const KString& in_url)
{
#ifdef SUPPORT_SUBFILE_DOWNLOAD
	int j;
	bool bExist;
	KString cacheFile;
	KString urlName;
	KUrl url;

	// check if local files
	ReadyFullPathUrl(urlName, in_url);
	url.SetUrl(urlName, FALSE);

	if( !url.isValid() || !url.isDownloadableScheme() )
	{
#ifdef _DEBUG_DOWNLOADER
		KString local_url;
		KWString wurl;

		wurl.assignUtf8(in_url);
		local_url = wurl.toLocal8Bit();
		KTRACE("Bad Url ignored:%s\n", local_url.data());
#endif
		bExist = true;
	}
#ifdef USE_DOWNLOAD_CACHE
	// check if already downloaded
	else if( m_DownloadItemMap.findCacheFile(cacheFile, urlName) )
		bExist = true;
#endif
	else
	{
		bExist = false;
		for(j = 0; j < (int)m_RequestQueue.size(); j++)
		{
			if(m_RequestQueue[j] == urlName)
			{
				bExist = true;
				break;
			}
		}
	}
	if( !bExist )
		m_RequestQueue.push_back(urlName);
	return true;
#else
	return false;
#endif
}

bool KDownloader::ReadyFullPathUrl(KString& fullName, const KString& in_url)
{
	KUrl url;

	fullName = in_url;
	url.SetUrl(in_url);

	if( !url.isValid() || !url.isDownloadableScheme() )
	{
    KWString in_urlw(in_url);
		if( !KFile::isLocalFile(in_urlw) && (m_baseUrl.length() > 0))
		{
			KString fullPath = m_baseUrl;
			fullPath.AddPathToName(in_url);
      url.SetUrl(fullPath, FALSE);
      url.CreateUrl(fullName);
			return true;
		}
	}
	return false;
}

bool KDownloader::GetShortenName(const KString& in_url, KString& shortName)
{
	// check base-url
	if( (m_baseUrl.length() > 0) && (m_baseUrl.length() < in_url.length()) &&
		(m_baseUrl.compare(in_url.data(), m_baseUrl.length(), true) == 0) )
	{
		shortName = in_url.substr(m_baseUrl.length(), in_url.length() - m_baseUrl.length());
		if( K2P_IsDirectoryDelimeter(shortName.at(0)) )
			shortName = shortName.substr(1, shortName.length() - 1);

		return true;
	}
	return false;
}

#ifdef SUPPORT_SUBFILE_DOWNLOAD

// return false if downloading is completed.
bool KDownloader::ProcessDownload(KStringList& out_completedItems)
{
	KString urlPath;

	// KTRACE("on ProcessDownload : m_bReadyNextFile=%d, m_state=%d, m_RequestQueue.size()=%d\n", m_bReadyNextFile, m_state, m_RequestQueue.size());
	if(m_bReadyNextFile)
	{
		if(m_RequestQueue.size() > 0)
		{
			urlPath = m_RequestQueue.front();

			m_RequestCount++;
			// start downloading...
#ifdef FAKE_DOWNLOAD_DEBUG
			// fake downloading
			KWString filename;
			WCHAR localCacheName[256];

			filename = urlPath;
			SetPureFilename(filename, 128);

			if( K2P_FindCacheLocalFile(filename.data(), localCacheName) )
			{
				KWString cacheFile(localCacheName);

				m_DownloadItemMap.insert(WStringWString_Pair(urlPath, cacheFile));
				// 다운로드 완료된 항목을 윗단에 올려주어 화면을 업데이트 하도록 함.
				out_completedItems += urlPath;
			}
			m_RequestQueue.erase(m_RequestQueue.begin());
			return true;
#else
#ifdef _DEBUG_DOWNLOADER
      KString dumpstr = urlPath.left(100);
			KTRACE("start download [%s]\n", dumpstr.data());
#endif
			m_bReadyNextFile = !StartDownloadFile(urlPath);
			// if(m_bReadyNextFile)
				m_RequestQueue.erase(m_RequestQueue.begin());
#endif
		}
		else
		{
			KTRACE("Downloader ended(request=%d, received=%d)\n", m_RequestCount, m_ReceiveCount);
			return false; // downloading complete.
		}
	}
	else
	{
		// continue download
		ContinueDownload();

		if((m_state == DownloaderDone) || (m_state == DownloaderFail))
		{
#ifdef STORE_TO_FILE
#ifdef USE_DOWNLOAD_CACHE
			if(m_currentCacheFile.size() > 0)
			{
				KString shortenName;

				m_DownloadItemMap.insert(m_currentDownloadUrl, m_currentCacheFile);

				out_completedItems += m_currentDownloadUrl;
				if( GetShortenName(m_currentDownloadUrl, shortenName) )
					out_completedItems += shortenName;

				// add redirected url on updating-list
				m_DownloadItemMap.AddRedirectedUrl(out_completedItems, m_currentDownloadUrl, 1);
			}
#endif
#endif
      BOOL failed = (m_state == DownloaderFail);
      CloseDownloadSocket(m_state == DownloaderFail);

			m_bReadyNextFile = true;
			m_ReceiveCount++;
      if (failed && (GetResult() == 0))
      {
        SetResult(1200); // socket downloader error
      }
			return false;
		}
    /*
		else if(m_state == DownloaderSendRequest)
    {
			if( K2P_GetSystemTimeAfter(m_waitReceiveLast) > HTTP_STREAM_WAIT_TIME )
      {
    		KTRACE("no SendRequest ack\n");
        return false;
      }
    }
    */
		else if(m_state == DownloaderInit)
		{
			m_bReadyNextFile = true;
			return true;
		}
	}

	if(!m_bContinue)
	{
		KTRACE("Downloader ended(request=%d, received=%d)\n", m_RequestCount, m_ReceiveCount);
	}
	return m_bContinue;
}

bool KDownloader::StartDownloadFile(const KString& in_url)
{
	m_currentDownloadUrl = in_url;
	m_currentCacheFile = in_url;
	m_currentRedirection = false;
	m_bReadyNextFile = true;
	return true;
}

void KDownloader::ContinueDownload()
{

}

void KDownloader::OnDownloadFileDone(const KString& in_url, bool bSuccess)
{
	if(bSuccess)
	{
		KString shortenName;

		m_DownloadDoneFiles += in_url;

		if( GetShortenName(in_url, shortenName) )
			m_DownloadDoneFiles += shortenName;
	}

	m_bReadyNextFile = true;
}

bool KDownloader::isDownloadDone()
{
	return m_bReadyNextFile;
}

bool KDownloader::GetLocalCache(KString& filename, const KString& in_url)
{
#ifdef USE_DOWNLOAD_CACHE
	if(in_url.size() > 0)
	{
		// find it from current base directory
		KString pathname;
		KString name;
		KString urlName(in_url);
		KUrl url;

		url.SetUrl(in_url);

		if( !url.isDownloadableScheme() )
		{
			filename = m_localDirectory;
			url.GetPathName(pathname);
			AddPathToName(filename, pathname);

			if( K2P_FileExist(filename.data()) )
				return true;

			// check if baseHref
			if( !ReadyFullPathUrl(urlName, in_url) )
				return false;
		}

#ifdef SUPPORT_SUBFILE_DOWNLOAD
		filename.clear();

		if( !m_DownloadItemMap.findCacheFile(filename, urlName) )
		{
			// when download done.
			// to do not search again.
			if( !m_bContinue )
				filename.assign("unreadable-x-file");
			return false;
		}
		return true;
#else
		filename.clear();
#endif
	}
#endif
	return false;
}

bool KDownloader::GetImageOriginalSize(const KString& in_url, SIZE& originSize)
{
#ifdef USE_DOWNLOAD_CACHE
	KString realName;
	int width, height;

	if( GetLocalCache(realName, in_url) &&
		K2P_LoadImageSize(realName.data(), width, height) )
	{
		originSize.cx = width;
		originSize.cy = height;
		return true;
	}
#endif
	return false;
}

bool KDownloader::InsertImageStreamFile(KString& cacheFileName, const KString& name, LPBYTE buff, int length)
{
#ifdef USE_DOWNLOAD_CACHE
	m_DownloadItemMap.readyCacheFilename(cacheFileName, name);
	if( KFile::WriteToFile(cacheFileName, buff, length) )
	{
		m_insertedTempImages += cacheFileName;
		return true;
	}
#endif
	return false;
}

bool KDownloader::FindImageStreamFile(KString& cacheFileName, LPBYTE buff, int length)
{
#ifdef USE_DOWNLOAD_CACHE
	int i, count;

	count = (int)m_insertedTempImages.size();
	for(i = 0; i < count; i++)
	{
		const KString& filename = m_insertedTempImages.at(i);
		if( KFile::IsSameBinary(filename, buff, length) )
		{
			cacheFileName = filename;
			return true;
		}
	}
	
	return m_DownloadItemMap.findImageStreamFile(cacheFileName, buff, length);
#else
  return false;
#endif
}
#endif
