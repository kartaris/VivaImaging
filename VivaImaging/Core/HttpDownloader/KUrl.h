#ifndef _K_URL_H
#define _K_URL_H

#include "KString.h"
#include "KWString.h"
#include "KFile.h"

typedef enum 
{
	KProtocol_NONE,
	KProtocol_HTTP,
	KProtocol_HTTPS,
	KProtocol_FTP,
	KProtocol_GOPHER,
	KProtocol_FILE,
	KProtocol_NEWS,
	KProtocol_MAILTO,
	KProtocol_SOCKS
}KProtocolScheme;

class KUrl
{
public:
	KUrl();
	~KUrl();

	void Clear();
	bool isDownloadableScheme();
	bool isValid();

	bool SetUrl(const KWString& str, BOOL unesc = TRUE);
	bool SetUrl(const KString& str, BOOL unesc = TRUE);

  inline void SetPortNumber(int port) { m_PortNumber = port; }

	bool CreateUrl(KString& our_url);
  /**
  * @brief make header string to use on WinSock SendRequest()
  */
	bool CreateRequest(KString& our_request, const KString& sessionKey, bool bProxy = false);

  void GetAddressString(KString& out_request);

  /**
  * @brief make additional header string to use on WinINET HttpSendRequest()
  */
  void GetHeaderString(KString& out_request, BOOL asPost, const KString& sessionKey);

  void GetOptionalString(KString& out_request);

	bool GetPathName(KString& pathname);

	bool _SetUrl(const KString& str);

	static bool IsUnsafeEscapeChar(char ch);
  static LPSTR AllocEscapeString(LPCSTR str, int length);
	static void MakeEscapeString(KString& str);
	static void MakeUnescapeString(KString& str);

	// check if 'http://, ftp://"
	static bool isDownloadUrl(const KString& url);

	KString m_SchemeName;
	KString m_HostName;
	KString m_UserName;
	KString m_Password;
	KString m_UrlPath;
	KString m_ExtraInfo;

	int m_PortNumber;
	KProtocolScheme m_Scheme;

};

#endif // _K_URL_H

