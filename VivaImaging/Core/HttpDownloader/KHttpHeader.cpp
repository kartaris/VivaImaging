/**************************************
 class KHttpHeader
**************************************/
#include "stdafx.h"
//#include "KUtility.h"
#include "PlatformLib.h"
#include "KMemory.h"
#include "KHttpHeader.h"

KHttpHeader::KHttpHeader()
{
	m_ContentType = K_UNKNOWN;
	m_ContentLength = 0;
  m_Chunked = false;
  m_Result = 0;
}

KHttpHeader::~KHttpHeader()
{
}

static BYTE http_packet_header_end[] = "\r\n";
#define FIELDNAME_DELIMITER ':'

LPBYTE KHttpHeader::findHeaderEnd(LPBYTE buff, int length)
{
	LPBYTE e;

	e = buff + length - 3;
	while(buff < e)
	{
		if(HCMemcmp(buff, http_packet_header_end, 2) == 0)
		{
			if((buff[2] == 0x0d) || (buff[2] == 0x0a))
				return buff;
		}
		buff++;
	}
	return NULL;
}

LPBYTE KHttpHeader::findRecordEnd(LPBYTE buff, int length)
{
	LPBYTE e;

	e = buff + length - 1;
	while(buff < e)
	{
		if(HCMemcmp(buff, http_packet_header_end, 2) == 0)
			return buff;
		buff++;
	}
	return NULL;
}

void KHttpHeader::SetContentType(const KString& value)
{
	KString ext(value);
	KString type;

	ext.getTokenString(type, '/');
	if(type.compare("image", false) == 0)
	{
		if(ext.compare("gif", false) == 0)
			m_ContentType = K_IMAGE_GIF;
		else if(ext.compare("jpeg", false) == 0)
			m_ContentType = K_IMAGE_JPG;
		else if(ext.compare("jpg", false) == 0)
			m_ContentType = K_IMAGE_JPG;
		else if(ext.compare("png", false) == 0)
			m_ContentType = K_IMAGE_PNG;
	}
	else if(type.compare("text", false) == 0)
	{
		if(ext.compare("txt", false) == 0)
			m_ContentType = K_TEXT_TEXT;
		if(ext.compare("html", false) == 0)
			m_ContentType = K_TEXT_HTML;
	}
}

void KHttpHeader::SetTransferEncoding(const KString& value)
{
	if(value.compare("chunked", false) == 0)
    m_Chunked = true;
}

void KHttpHeader::SetRedirection(const KString& value)
{
	m_RedirectionLocation = value;
}

void KHttpHeader::SetErrorCode(const KString& value)
{
  m_ErrorCode = value;
}

void KHttpHeader::SetErrorMessage(const KString& value)
{
  m_ErrorMessage = value;
}

bool KHttpHeader::setHeader(LPBYTE p, int length)
{
	LPBYTE start_record;
	LPBYTE e = p + length;
	LPBYTE end_record;
	KString name;
	KString value;

#ifdef _DEBUG_DOWNLOADER
	KTRACE_0("HTTP-HEADER\n");
#endif
	m_ContentLength = -1;
	while(p < (e - 2))
	{
		end_record = findRecordEnd(p, (int)(e - p));
		if(end_record == NULL)
			end_record = e;

		start_record = p;
		while((*p != FIELDNAME_DELIMITER) && (p < end_record))
			p++;

		if (*p == FIELDNAME_DELIMITER)
		{
			name.assign((const char *)start_record, (int)(p - start_record));
			p++;
			value.assign((const char *)p, (int)(end_record - p));

			name.clearSideWhiteSpace();
			value.clearSideWhiteSpace();

#ifdef _DEBUG_DOWNLOADER
			KTRACE("%s:%s\n", name.data(), value.data());
#endif
		
			if(name.compare("Content-Length", false) == 0)
			{
				m_ContentLength = value.toInt();
			}
			else if(name.compare("Transfer-Encoding", false) == 0)
      {
				SetTransferEncoding(value);
      }
			else if(name.compare("Content-Type", false) == 0)
      {
				SetContentType(value);
      }
			else if(name.compare("Location", false) == 0)
      {
				SetRedirection(value);
      }
			else if(name.compare("X-ErrorCode", false) == 0)
      {
        SetErrorCode(value);
      }
			else if(name.compare("X-ErrorMessage", false) == 0)
      {
        SetErrorMessage(value);
      }
      else
      {
			  name.assign((const char *)start_record, (int)(end_record - start_record));
        if ((name.length() > 10) && (name.compareLength("HTTP/1.1", 8, false) == 0))
        {
          KTRACE("%s\n", name.data());

          KString value = name.right(name.length() - 9);
          SetRequestResult(value.toInt());
        }
        else
        {
          KTRACE("%s\n", name.data());
        }
      }
		}
		else
		{
			name.assign((const char *)start_record, (int)(end_record - start_record));
      if ((name.length() > 10) && (name.compareLength("HTTP/1.1", 8, false) == 0))
      {
        KTRACE("%s\n", name.data());
        KString value = name.right(name.length() - 9);
        SetRequestResult(value.toInt());
        m_ErrorMessage = value;
      }
#ifdef _DEBUG_DOWNLOADER
			KTRACE("%s\n", name.data());
#endif
		}
		p = end_record + 2;
	}
	return true;
}

KString KHttpHeader::getContentTypeExtension()
{
	KString extension;

	switch(m_ContentType)
	{
	case K_IMAGE_GIF:
		extension.assign("gif");
		break;
	case K_IMAGE_JPG:
		extension.assign("jpg");
		break;
	case K_IMAGE_PNG:
		extension.assign("png");
		break;
	case K_TEXT_TEXT:
		extension.assign("txt");
		break;
	case K_TEXT_HTML:
		extension.assign("html");
		break;
	}
	return extension;
}

KDownloaderError KHttpHeader::GetErrorCodeValue()
{
  if (m_ErrorCode.length() > 0)
  {
    if (m_ErrorCode.compare("CM1001") == 0)
      return SFS_ERR_UNAUTHORIZED;
    else if (m_ErrorCode.compare("CM1002") == 0)
      return SFS_ERR_SESSION_EXPIRED;
    else if (m_ErrorCode.compare("DM1002") == 0)
      return SFS_ERR_NO_EXIST_FOLDER;
    else if (m_ErrorCode.compare("DM4001") == 0)
      return SFS_ERR_NO_EXIST_URL;
    else if (m_ErrorCode.compare("DM4004") == 0)
      return SFS_ERR_INVALID_DATETIME;
    else
      return SFS_ERR_UNKNOWN;
  }
  else
  {
    if ((m_Result == 400) || (m_Result == 404))
      return SFS_ERR_BAD_URL;
  }
  return SFS_NO_ERROR;
}


