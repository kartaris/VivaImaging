#ifndef _K_HTTP_DOWNLOADER_HEADER_H
#define _K_HTTP_DOWNLOADER_HEADER_H

#include "KString.h"
#include "KWString.h"
#include "KFile.h"

typedef enum 
{
	K_UNKNOWN,
	K_IMAGE_GIF,
	K_IMAGE_JPG,
	K_IMAGE_PNG,
	K_TEXT_TEXT,
	K_TEXT_HTML
}KContentType;

// ServerFolderScan Error
typedef enum 
{
  SFS_NO_ERROR,
  SFS_ERR_UNAUTHORIZED,
  SFS_ERR_SESSION_EXPIRED,
  SFS_ERR_NO_EXIST_FOLDER,
  SFS_ERR_NO_EXIST_URL,
  SFS_ERR_INVALID_DATETIME,
  SFS_ERR_BAD_URL,
  SFS_ERR_UNKNOWN
} KDownloaderError;

class KHttpHeader
{
public:
	KHttpHeader();
	~KHttpHeader();

	bool setHeader(LPBYTE buff, int length);
	void SetContentType(const KString& value);
  void SetTransferEncoding(const KString& value);
	void SetRedirection(const KString& value);
  void SetErrorCode(const KString& value);
  void SetErrorMessage(const KString& value);

  inline void SetRequestResult(int r) { m_Result = r; }

	static LPBYTE findHeaderEnd(LPBYTE buff, int length);
	static LPBYTE findRecordEnd(LPBYTE buff, int length);

	inline KContentType getContentType() { return m_ContentType; }
	inline int getContentLength() { return m_ContentLength; }
	KString getContentTypeExtension();

	bool isRedirection() { return(m_RedirectionLocation.size() > 0); }
  bool isChunked() { return m_Chunked; }

	KContentType m_ContentType;
	int m_ContentLength;
  int m_Result;
  bool m_Chunked;
	KString m_RedirectionLocation;
  KString m_ErrorCode;
  KString m_ErrorMessage;
  
  KDownloaderError GetErrorCodeValue();
  LPCSTR GetErrorMessage() { if (m_ErrorMessage.length() > 0) return m_ErrorMessage.data(); return NULL; }
};

#endif // _K_HTTP_DOWNLOADER_HEADER_H