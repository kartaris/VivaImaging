#include "stdafx.h"
//#include "KUtility.h"
#include "PlatformLib.h"
#include "KMemory.h"

#include "KUrl.h"

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif 

//Empty constructor
KUrl::KUrl()
{
	m_PortNumber = -1;
	m_Scheme = KProtocol_NONE;
}

KUrl::~KUrl()
{
	Clear();
}

void KUrl::Clear()
{
	m_SchemeName.clear();
	m_HostName.clear();
	m_UserName.clear();
	m_Password.clear();
	m_UrlPath.clear();
	m_ExtraInfo.clear();

	m_PortNumber = -1;
	m_Scheme = KProtocol_NONE;
}

KProtocolScheme GetScheme(KString& schemeName)
{
	if(schemeName.compare("http", false) == 0)
		return KProtocol_HTTP;
	else if(schemeName.compare("ftp", false) == 0)
		return KProtocol_FTP;
	else if(schemeName.compare("https", false) == 0)
		return KProtocol_HTTPS;
	else if(schemeName.compare("gopher", false) == 0)
		return KProtocol_GOPHER;
	else if(schemeName.compare("news", false) == 0)
		return KProtocol_NEWS;
	else if(schemeName.compare("file", false) == 0)
		return KProtocol_FILE;
	else if(schemeName.compare("mailto", false) == 0)
		return KProtocol_MAILTO;
	else if(schemeName.compare("socks", false) == 0)
		return KProtocol_SOCKS;
	return KProtocol_NONE;
}

int GetDefaultUrlPort(KProtocolScheme scheme)
{
	switch (scheme)
	{
		case KProtocol_FTP:
			return 21;
		case KProtocol_GOPHER:
			return 70;
		case KProtocol_HTTP:
			return 80;
		case KProtocol_HTTPS:
			return 443;
		case KProtocol_SOCKS:
			return 1080;
		default:
			return -1;
	}
}

bool KUrl::isDownloadableScheme()
{
	return( (m_Scheme == KProtocol_FTP) ||
		(m_Scheme == KProtocol_HTTP) ||
		(m_Scheme == KProtocol_HTTPS) ||
		(m_Scheme == KProtocol_GOPHER) );
}

bool KUrl::isValid()
{
	return((m_Scheme != KProtocol_NONE) && (m_HostName.size() > 0));
}

bool KUrl::SetUrl(const KWString& str, BOOL unesc)
{
	KString ustr;

	ustr = str.toUtf8();
  if (unesc)
	MakeUnescapeString(ustr);
	return _SetUrl(ustr);
}

bool KUrl::SetUrl(const KString& str, BOOL unesc)
{
	KString ustr(str);

  if (unesc)
	MakeUnescapeString(ustr);
	return _SetUrl(ustr);
}

bool KUrl::IsUnsafeEscapeChar(char ch)
{
	if( isNumeric(ch) || isAlpha(ch) || 
		(ch == '-') || (ch == '_') ||
		(ch == '(') || (ch == ')') ||
		(ch == '.') || (ch == ',') )
		return false;
	return true;
	/*
	switch(ch)
	{
		case ';': case '\\': case '?': case '@': case '&':
		case '=': case '+': case '$': case ',': case ' ':
		case '<': case '>': case '#': case '%': case '\"':
		case '{': case '}': case '|':
		case '^': case '[': case ']': case '`':
			return true;
		default:
		{
			if (ch < 32 || ch > 126)
				return true;
		}
	}
	return false;
	*/
}

LPSTR KUrl::AllocEscapeString(LPCSTR str, int length)
{
  if (length < 0)
    length = strlen(str);
  int max = 0;
	for(int i = 0; i < length; i++)
	{
		if( IsUnsafeEscapeChar(str[i]) )
      max += 3;
    else
      max++;
  }

  LPSTR buff = new char[max+2];
  int count = 0;
	for(int i = 0; i < length; i++)
	{
		if( IsUnsafeEscapeChar(str[i]) )
    {
      buff[count++] = '%';
      sprintf_s(&buff[count], max - count, "%.2X", (BYTE)str[i]);
      count += 2;
    }
    else
    {
      buff[count++] = str[i];
    }
  }
  buff[count] = '\0';
  return buff;
}

// convert none number-alpha char as hexa code char
void KUrl::MakeEscapeString(KString& str)
{
	int i, count;
	KString src(str);
	KString chstr;
	char ch;

	str.clear();
	count = src.length();
	for(i = 0; i < count; i++)
	{
		ch = src.at(i);
		if( IsUnsafeEscapeChar(ch) )
		{
			str.append(1, '%');
			chstr.setNum(ch, 16, 2);
			str.append(chstr);
		}
		else
			str.append(1, ch);
	}
}

// convert EscapeString to normal string ('%30%40' as '1a'...)
void KUrl::MakeUnescapeString(KString& str)
{
	int i, count;
	KString src(str);
	KString chstr;
	char ch;

	str.clear();
	count = src.length();
	for(i = 0; i < count; i++)
	{
		ch = src.at(i);
		if(ch == '%')
		{
			chstr = src.substr(i + 1, 2);
			if(chstr.length() == 2)
			{
				ch = (char) chstr.toInt(NULL, 16);
				i += 2;
			}
		}
		str.append(1, ch);
	}
}

bool KUrl::isDownloadUrl(const KString& url)
{
	int p;
	KString scheme_name;

	
	p = url.find(':');
	if( (p >= 3) && (url.length() > (p+2)) &&
		(url.at(p+1) == '/') && (url.at(p+2) == '/') )
	{
		scheme_name = url.substr(0, p);
		if(scheme_name.compare("file", 4, false) != 0)
			return true;
	}
	return false;
}

//Set the url
/*
<Scheme>://<UserName>:<Password>@<HostName>:<PortNumber>/<UrlPath><ExtraInfo>
(ex, consider this URL: http://someone:secret@www.microsoft.com:80/visualc/stuff.htm#contents)
Scheme: "http" or ATL_URL_SCHEME_HTTP
UserName: "someone"
Password: "secret"
HostName: "www.microsoft.com"
PortNumber: 80
UrlPath: "visualc/stuff.htm"
ExtraInfo: "#contents"
*/

bool KUrl::_SetUrl(const KString& str)
{
	Clear();

	bool bGotScheme = false;
	bool bGotUserName = false;
	bool bGotHostName = false;
	bool bGotPortNumber = false;
	KString strCurrentUrl;
	char ch;
	LPCSTR lpszUrl = str.data();

	//<protocol>://user:pass@server:port
	while ((ch = *lpszUrl) != '\0')
	{
		if (ch == ':')
		{
			//3 cases:
			//(1) Just encountered a scheme
			//(2) Port number follows
			//(3) Form of username:password@

			// Check to see if we've just encountered a scheme
			if (!bGotScheme)
			{
				m_SchemeName = strCurrentUrl;
				m_Scheme = GetScheme(m_SchemeName);
				if(m_Scheme == KProtocol_NONE)
				{
					m_SchemeName.clear();
					m_UrlPath = str;
					return false;
				}
        if (m_PortNumber < 0)
				  m_PortNumber = GetDefaultUrlPort(m_Scheme);
				bGotScheme = true;

				if (*(lpszUrl+1) == '/')
				{
					if (*(lpszUrl+2) == '/')
					{
						//the mailto scheme cannot have a '/' following the "mailto:" portion
						if (bGotScheme && m_Scheme == KProtocol_MAILTO)
							goto error;

						//Skip these characters and continue
						lpszUrl+= 2;
					}
					else 
					{
						//it is an absolute path
						//no domain name, port, username, or password is allowed in this case
						//break to loop that gets path
						lpszUrl++;
						strCurrentUrl.clear();
						break;
					}
				}

				//reset pszCurrentUrl
				lpszUrl++;
				strCurrentUrl.clear();

				//if the scheme is file, skip to getting the path information
				if (m_Scheme == KProtocol_FILE)
					break;
				continue;
			}
			else if (!bGotUserName || !bGotPortNumber)
			{
				KString str;

				//get the user or portnumber (break on either '/', '@', or '\0'
				while (((ch = *(++lpszUrl)) != '/') && (ch != '@') && (ch != '\0'))
				{
					str += ch;
				}

				//if we broke on a '/' or a '\0', it must be a port number
				if (!bGotPortNumber && (ch == '/' || ch == '\0'))
				{
					//the host name must immediately preced the port number
					m_HostName = strCurrentUrl;

					//get the port number
					m_PortNumber = str.toInt();
					bGotPortNumber = bGotHostName = true;
				}
				else if (!bGotUserName && ch=='@')
				{
					//otherwise it must be a username:password
					m_UserName = strCurrentUrl;
					m_Password = str;
					bGotUserName = true;
					lpszUrl++;
				}
				else
				{
					goto error;
				}
				strCurrentUrl.clear();
			}
		}
		else if (ch == '@')
		{
			m_UserName = strCurrentUrl;
			bGotUserName = true;
			lpszUrl++;
			strCurrentUrl.clear();
		}
		else if (ch == '/' || ch == '?' || (!*(lpszUrl+1)))
		{
			//we're at the end of this loop
			//set the domain name and break
			if (!*(lpszUrl+1) && ch != '/' && ch != '?')
			{
				strCurrentUrl += ch;
				lpszUrl++;
			}

			if (!bGotHostName)
				m_HostName = strCurrentUrl;

			strCurrentUrl.clear();
			break;
		}
		else
		{
			strCurrentUrl += ch;
			lpszUrl++;
		}
	}

	if (!bGotScheme)
	{
		m_Scheme = KProtocol_NONE;
		m_SchemeName.clear();
		m_UrlPath = str;
		return false;
	}

	if(m_Scheme == KProtocol_FILE)
	{
		if(K2P_StringLength(lpszUrl) >= 9)
		{
			KString hostname;

			hostname.assign(lpszUrl, 9);
			if(hostname.compare("localhost", 9, false) == 0)
			{
				m_HostName = hostname;
				lpszUrl += 9;
			}
		}

		// check absolute path
#ifdef WIN32
		if(*lpszUrl == '/')
			lpszUrl++;
#else
		if(*lpszUrl != '/')
			lpszUrl--;
#endif
	}

	//Now build the path
	while ((ch = *lpszUrl) != '\0')
	{
		//break on a '#' or a '?', which delimit "extra information"
		if (m_Scheme != KProtocol_FILE && (ch == '#' || ch == '?'))
		{
			break;
		}
		strCurrentUrl += ch;
		lpszUrl++;
	}

	m_UrlPath = strCurrentUrl;
	strCurrentUrl.clear();

	while ((ch = *lpszUrl++) != '\0')
	{
		strCurrentUrl += ch;
	}

	m_ExtraInfo = strCurrentUrl;
	return true;

error:
	Clear();
	return false;
}

bool KUrl::CreateUrl(KString& out_url)
{
	//build URL: <scheme>://<user>:<pass>@<domain>:<port><path><extra>
	out_url.clear();
	if(m_SchemeName.length() > 0)
	{
		out_url += m_SchemeName;
		out_url += ':';
		if (m_Scheme != KProtocol_MAILTO)
			out_url += "//";

		if(m_UserName.length() > 0)
		{
			out_url += m_UserName;
			if(m_Password.length() > 0)
			{
				out_url += ':';
				out_url += m_Password;
			}

			out_url += '@';
		}
	}

	if(m_HostName.length() > 0)
	{
		out_url += m_HostName;

		if((m_PortNumber > 0) && (m_PortNumber != GetDefaultUrlPort(m_Scheme)))
		{
			KString szPort;

			szPort.setNum(m_PortNumber);
			out_url += ':';
			out_url += szPort;
		}
	}

	if(m_UrlPath.length() > 0)
		out_url.AddSubPathNameEx(m_UrlPath, '/');

	if(m_ExtraInfo.length() > 0)
		out_url += m_ExtraInfo;

	// escape character conversion
	/*
	if (dwFlags & ATL_URL_ESCAPE)
		UnescapeUrl(our_url);
	*/
	return true;
}

bool KUrl::CreateRequest(KString& out_request, const KString& sessionKey, bool bProxy)
{
	KString str;

	out_request.assign("GET ");

	// proxy
	if(bProxy)
	{
		CreateUrl(str);
		out_request += str;

		out_request.append(" HTTP/1.1\r\n");
		
		out_request.append("Host: ");
		out_request += m_HostName;
		if((m_PortNumber > 0) && (m_PortNumber != GetDefaultUrlPort(m_Scheme)))
		{
			str.setNum(m_PortNumber);
			out_request.append(":");
			out_request += str;
		}
		out_request.append("\r\n");
	}
	else
	{
		out_request += m_UrlPath;
		out_request += m_ExtraInfo;

		out_request.append(" HTTP/1.1\r\n");

		out_request.append("Host: ");
		out_request += m_HostName;
		if((m_PortNumber > 0) && (m_PortNumber != GetDefaultUrlPort(m_Scheme)))
		{
			str.setNum(m_PortNumber);
			out_request.append(":");
			out_request += str;
		}

		out_request.append("\r\n");

	}

  if (sessionKey.size() > 0)
  {
	  out_request.append("Authorization: Basic ");
    out_request.append(sessionKey);
	  out_request.append("\r\n");
  }

	out_request.append("User-Agent: LocalSyncClient 1.1\r\n");
	out_request.append("\r\n");
	return true;
}

void KUrl::GetAddressString(KString& out_request)
{
	out_request = m_UrlPath;
  if (m_ExtraInfo.length() > 0)
	out_request += m_ExtraInfo;
}

LPCSTR HTTP_POST_CONTENT_TYPE_HEADER = "Content-type: application/x-www-form-urlencoded";

void KUrl::GetHeaderString(KString& out_request, BOOL asPost, const KString& sessionKey)
{
  out_request.clear();

	out_request.append("Host: ");
	out_request += m_HostName;
	if((m_PortNumber > 0) && (m_PortNumber != GetDefaultUrlPort(m_Scheme)))
	{
    KString str;
		str.setNum(m_PortNumber);
		out_request.append(":");
		out_request += str;
	}

	out_request.append("\r\n");

  if (sessionKey.size() > 0)
  {
	  out_request.append("Authorization: Basic ");
    out_request.append(sessionKey);
	  out_request.append("\r\n");
  }

  if (asPost)
  {
    out_request.append(HTTP_POST_CONTENT_TYPE_HEADER);
	  out_request.append("\r\n");
  }

#ifdef HTTP_MSG_SEND_TEST
	out_request.append("User-Agent: FileBackupMsgSendTest 1.0\r\n");
#else
	out_request.append("User-Agent: LocalSyncClient 1.1\r\n");
#endif

	out_request.append("\r\n");
}

void KUrl::GetOptionalString(KString& out_request)
{
  if (m_ExtraInfo.length() > 1)
  {
    out_request = m_ExtraInfo.substr(1, m_ExtraInfo.length()-1);
    m_ExtraInfo.clear();
  }
}

bool KUrl::GetPathName(KString& pathname)
{
	pathname = m_UrlPath;
	return(pathname.length() > 0);
}

