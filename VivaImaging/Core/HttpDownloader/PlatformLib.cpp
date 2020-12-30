// PlatformLib.cpp
#include "stdafx.h"
#include <Strsafe.h>

#include "PlatformLib.h"
//#include "KHttpSocket.h"

/***********************************
 STDIO
***********************************/

PLATFORMLIB_API int K2P_sprintf(char* buf, int len, const char* format,...)
{
    va_list ap;

	va_start( ap, format );			// use variable arg list

#ifdef PLATFORM_SHP
	AcVsprintf( buf, format, ap );
#else
	vsprintf_s( buf, len, format, ap );
#endif

	va_end( ap );
	return (int) K2P_StringLength(buf);
}

PLATFORMLIB_API int K2P_snprintf(char* buf, int bufflen, const char* format,...)
{
    va_list ap;

	va_start( ap, format );			// use variable arg list

#ifdef PLATFORM_SHP
	AcVsnprintf( buf, bufflen, format, ap );
#elif defined(WINCE)
	vsnprintf( buf, bufflen, format, ap );
#else
	vsnprintf_s( buf, bufflen, bufflen-1, format, ap );//vsprintf_s( buf, bufflen, format, ap );
#endif

	va_end( ap );
	return (int) K2P_StringLength(buf);
}

PLATFORMLIB_API LPSTR K2P_StringCopy(LPSTR d, int len, LPCSTR s)
{
#ifdef PLATFORM_SHP
	return AcStrcpy(d, s);
#else
	strcpy_s(d, len, s);
  return d;
#endif
}

PLATFORMLIB_API LPSTR K2P_StringCat(LPSTR d, int len, LPCSTR s)
{
#ifdef PLATFORM_SHP
	return AcStrcat(d, s);
#else
	strcat_s(d, len, s);
  return d;
#endif
}

PLATFORMLIB_API LPSTR K2P_StringCopyn(LPSTR d, int len, LPCSTR s, int n)
{
#ifdef PLATFORM_SHP
	return AcStrncpy(d, s, n);
#else
	strncpy_s(d, len, s, n);
  return d;
#endif
}

PLATFORMLIB_API int K2P_StringLength(LPCSTR str)
{
#ifdef PLATFORM_SHP
	return (int) AcStrlen(str);
#else
	return (int) strlen(str);
#endif
}

PLATFORMLIB_API int K2P_StringCompare(LPCSTR s, LPCSTR d)
{
#ifdef PLATFORM_SHP
	return AcStrcmp(s, d);
#else
	return strcmp(s, d);
#endif
}

PLATFORMLIB_API int K2P_StringCompareN(LPCSTR s, LPCSTR d, int n)
{
#ifdef PLATFORM_SHP
	return AcStrncmp(s, d, n);
#else
	return strncmp(s, d, n);
#endif
}

PLATFORMLIB_API int K2P_snwprintf(WCHAR* buf, int bufflen, const WCHAR* format,...)
{
#ifdef PLATFORM_SHP
	AcSnwprintf((wchar_t *)buf, bufflen, (const wchar_t *)format);
	return (int) AcWcslen((wchar_t *)buf);
#elif defined(PLATFORM_GTK)
	swprintf((wchar_t *)buf, bufflen, (const wchar_t *)format);
	return (int) K2P_WStringLength((WCHAR *)buf);
#else
	StringCchPrintf((wchar_t *)buf, bufflen, (const wchar_t *)format);
	return (int) wcslen((wchar_t *)buf);
#endif
}


PLATFORMLIB_API WCHAR* K2P_WStringCopy(WCHAR* d, int len, const WCHAR* s)
{
	if(d != NULL)
	{
		if((s != NULL) && (*s != '\0'))
		{
#ifdef PLATFORM_SHP
			AcWcscpy((wchar_t *)d, (const wchar_t *)s);
#elif defined(PLATFORM_GTK)
			int n = K2P_WStringLength(s);
			if(n > 0)
				K2P_MemoryCopy((void *)d, (const void *)s, sizeof(WCHAR) * n);
			d[n] = '\0';
#else
			StringCchCopy(d, len, s);
#endif
		}
		else
			*d = '\0';
	}
	return d;
}

PLATFORMLIB_API WCHAR* K2P_WStringCopyn(WCHAR* d, int len, const WCHAR* s, int n)
{
	if((d != NULL) && (n > 0))
	{
		if((s != NULL) && (*s != '\0'))
		{
#ifdef PLATFORM_SHP
			AcWcsncpy((wchar_t *)d, (const wchar_t *)s, n);
#elif defined(PLATFORM_GTK)
			if(n > 0)
				K2P_MemoryCopy((void *)d, (const void *)s, sizeof(WCHAR) * n);
			d[n] = '\0';
#else
      StringCchCopyN(d, len, s, n);
			//wcsncpy((wchar_t *)d, (const wchar_t *)s, n);
#endif
		}
		else
			*d = '\0';
	}
	return d;
}

PLATFORMLIB_API int K2P_WStringLength(const WCHAR* s)
{
#ifdef PLATFORM_SHP
	return (int)AcWcslen((const wchar_t *)s);
#elif defined(PLATFORM_GTK)
	int len = 0;

	if(s == NULL)
		return 0;

	while(*s != '\0')
	{
		s++;
		len++;
	}
	return len;
#else
	return (int)wcslen((const WCHAR *)s);
#endif
}

PLATFORMLIB_API int K2P_WStringCompare(const WCHAR* s, const WCHAR* d)
{
#ifdef PLATFORM_SHP
	return AcWcscmp((const wchar_t *)s, (const wchar_t *)d);
#elif defined(PLATFORM_GTK)
	if(s == NULL)
		return 1;
	if(d == NULL)
		return -1;
	while((*s != '\0') && (*d != '\0'))
	{
		if(*s < *d)
			return 1;
		else if(*s > *d)
			return -1;
		s++;
		d++;
	}
	return 0;
#else
	return wcscmp((const WCHAR *)s, (const WCHAR *)d);
#endif
}

PLATFORMLIB_API int K2P_atoi(const char *p)
{
#ifdef PLATFORM_SHP
	return AcAtoi(p);
#else
	return atoi(p);
#endif
}

PLATFORMLIB_API __int64 K2P_atoi64(const char *p)
{
#ifdef PLATFORM_SHP
	return AcAtoi(p);
#else
	return _atoi64(p);
#endif
}

PLATFORMLIB_API double K2P_atof(const char *p)
{
#ifdef PLATFORM_SHP
	return AcAtof(p);
#else
	return atof(p);
#endif
}


/*
PLATFORMLIB_API int K2P_sscanf(const char* buff, const char* format,...)
{
#ifdef PLATFORM_SHP
	return AcSscanfS(buff,format,__VA_ARGS__);
#else
	return sscanf_s(buff,format,__VA_ARGS__);
#endif
}
*/

#ifdef USE_K2P_CODEC

#ifdef PLATFORM_SHP
static char s_code_buff[512] = {0,};

static int _getPCodeSize(K2Charset charset, LPCSTR in_str, int in_len)
{
	int out_len = 0;
	char* buff;
	char* mbuff = NULL;

	if(in_str[in_len] == 0)
	{
		buff = (char *)in_str;
	}
	else
	{
		if(in_len > 512)
		{
			mbuff = (char *)K2P_MemAlloc(in_len + 1);
			AcMemset(mbuff,0x00,in_len + 1);
			AcStrncpy(mbuff, in_str, in_len);
			buff = mbuff;
		}
		else
		{
			AcMemset(s_code_buff,0x00,sizeof(s_code_buff));
			AcStrncpy(s_code_buff, in_str, in_len);
			buff = s_code_buff;
		}
	}

	if(charset == KCHARSET_UTF_8)
		out_len = WmGetPCodeSize(buff) / 2;
	else
		out_len = WmGetPCodeSize(buff) / 2;

	/* invalid output size, so do not use WmGetPCodeSizefromKSC5601()
	else
		out_len = WmGetPCodeSizefromKSC5601(buff) / 2 - 1;
	*/

	if(mbuff != NULL)
		K2P_MemFree(mbuff);

	return out_len;
}
#endif

PLATFORMLIB_API int K2P_LocalCodeToUnicode(LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen)
{
#ifdef PLATFORM_SHP
	int out_len = 0;

	if(in_len > 0)
	{
		// assume local code is UTF-8
		out_len = _getPCodeSize(KCHARSET_UTF_8, in_str, in_len);

		if((out_wstr != NULL) && (out_len <= out_wlen))
			WmConvertUTF2PCode(out_wstr, (out_wlen + 1) * sizeof(WCHAR), (UCHAR*)in_str, in_len);
			// WmConvertKSC56012Pcode(out_wstr, (char *)in_str);//WmConvertKSC56012PcodeN(out_wstr, (char *)in_str, in_len);
	}
	return out_len;
#elif defined(PLATFORM_GTK)
	//TODO: by wooarmy
	GError *error = NULL;
	gsize bytes_read = 0;
	gsize bytes_written = 0;

	if( in_str == NULL )
		return 0;

	gchar *utf8str = g_locale_to_utf8((const gchar*)in_str, in_len, &bytes_read, &bytes_written, &error);
	if(error != NULL)
	{
        KTRACE("Error g_locale_to_utf8 conversion: %s", error->message);
		g_error_free(error);
		return 0;
	}
	if( utf8str == NULL )
	{
		KTRACE("### K2P_ERROR [%s]: error occurs. [src=%s, len=%d]\n", __FUNCTION__, in_str, in_len);
		return 0;
	}

	int length = K2P_Utf8CodeToUnicode((LPCSTR)utf8str, (int)bytes_written, out_wstr, out_wlen);
	g_free(utf8str);
	return length;
#else
	return MultiByteToWideChar(CP_ACP, 0, in_str, in_len, (LPWSTR)out_wstr, out_wlen);
#endif
}

PLATFORMLIB_API int K2P_Utf8CodeToUnicode(LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen)
{
#ifdef PLATFORM_SHP
	int out_len = 0;
	if(in_len > 0)
	{
		out_len = _getPCodeSize(KCHARSET_UTF_8, in_str, in_len);
		if((out_wstr != NULL) && (out_len <= out_wlen))
			 WmConvertUTF2PCode(out_wstr, (out_wlen + 1) * sizeof(WCHAR), (UCHAR*)in_str, in_len);
	}
	return out_len;
#elif defined(PLATFORM_GTK)
	//TODO:.. by wooarmy
	GError *error = NULL;
	glong items_read = 0;
	glong items_written = 0;

	if( (in_str == NULL) || (in_str[0] == '\0') || (in_len == 0) )
		return 0;

	gunichar2 *unistr = g_utf8_to_utf16((const gchar*)in_str, in_len, &items_read, &items_written, &error);
	if(error != NULL)
	{
        KTRACE("Error g_utf8_to_utf16 conversion: %s", error->message);
		g_error_free(error);
		return 0;
	}
	if( unistr == NULL || items_written == 0 )
	{
		KTRACE("### K2P_ERROR [%s]: error occurs.(in=%d, out=%d)\n", __FUNCTION__, in_len, items_written);
		return 0;
	}
	if(out_wstr != NULL)
	{
		if( out_wlen < items_written )
			KTRACE("### K2P_WARNING [%s]: check length.\n", __FUNCTION__);
		K2P_WStringCopyn((WCHAR *)out_wstr, (const WCHAR *)unistr, out_wlen);
		out_wstr[out_wlen] = 0;
	}
	g_free(unistr);

	return items_written;
#else
	return MultiByteToWideChar(CP_UTF8, 0, in_str, in_len, (LPWSTR)out_wstr, out_wlen);
#endif
}

#if defined(PLATFORM_GTK)

static const gchar* k2_get_charset_name(K2Charset charset)
{
	switch(charset)
	{
	case KCHARSET_EN:
		return "en";
	case KCHARSET_US:
		return "us";
	case KCHARSET_FR:
		return "fr";
	case KCHARSET_DE:
		return "de";
	case KCHARSET_IT:
		return "it";
	case KCHARSET_NL:
		return "nl";
	case KCHARSET_EL:
		return "el";
	case KCHARSET_ES:
		return "es";
	case KCHARSET_PT:
		return "pt";
	case KCHARSET_AR:
		return "ar";
	case KCHARSET_HE:
		return "he";
	case KCHARSET_RU:
		return "ru";
	case KCHARSET_ZH:
		return "EUC-CN";
	case KCHARSET_JP:
		return "EUC-JP";
	case KCHARSET_KO:
		return "EUC-KR";
	/*
	case KCHARSET_HI = ,
	case KCHARSET_UR,
	case KCHARSET_SA,
	*/
	case KCHARSET_UTF_8:
		return "utf-8";
	case KCHARSET_8859_1:
		return "ISO-8859-1";
	case KCHARSET_8859_2:
		return "ISO-8859-2";
	case KCHARSET_8859_3:
		return "ISO-8859-3";
	case KCHARSET_8859_4:
		return "ISO-8859-4";
	case KCHARSET_8859_5:
		return "ISO-8859-5";
	case KCHARSET_8859_6:
		return "ISO-8859-6";
	case KCHARSET_8859_7:
		return "ISO-8859-7";
	case KCHARSET_8859_8:
		return "ISO-8859-8";
	case KCHARSET_8859_9:
		return "ISO-8859-9";
	case KCHARSET_8859_15:
		return "ISO-8859-15";
	case KCHARSET_LAO:
		return "cp1258";
	case KCHARSET_THAI:
		return "cp874";
	case KCHARSET_GB2312:
		return "GB2312";
	case KCHARSET_GB18030:
		return "GB18030";
	case KCHARSET_BIG5:
		return "BIG5";
	default:
		return "ISO-8859-1";
	}
}

#endif


PLATFORMLIB_API int K2P_MultiByteCodeToUnicode(K2Charset charset, LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen)
{
#ifdef PLATFORM_SHP
	int out_len = 0;

	if(in_len > 0)
	{
		// ASSERT("should fixed");
		out_len = _getPCodeSize(charset, in_str, in_len);
		if((out_wstr != NULL) && (out_len <= out_wlen))
		{
			char last;
			char* buff = (char *)in_str;

			last = buff[in_len];
			buff[in_len] = '\0';
			if(charset == KCHARSET_UTF_8)
				WmConvertUTF2PCode(out_wstr, (out_wlen + 1) * sizeof(WCHAR), (UCHAR*)in_str, in_len);
			else
				WmConvertUTF2PCode(out_wstr, (out_wlen + 1) * sizeof(WCHAR), (UCHAR*)in_str, in_len);
			
			// KSC_5601 code conversion is not supported
			/*
			else if(charset == KCHARSET_KO)
				WmConvertKSC56012PcodeN(out_wstr, (char *)in_str, in_len);
			*/
			//else
			//	WmConvertAscIICode2PCode(out_wstr, (out_wlen + 1) * sizeof(WCHAR), (char*)in_str);

			buff[in_len] = last;
		}
	}
	return out_len;
#elif defined(PLATFORM_GTK)
	GError *error = NULL;
	gsize items_read = 0;
	gsize utf8_written;
	glong utf8_read = 0;
	glong items_written = 0;
	gunichar2 *unistr;

	static int last_charset = KCHARSET_UNDEFINED;
	static const gchar* last_charset_name = NULL;

	if( (in_str == NULL) || (in_str[0] == '\0') || (in_len == 0) )
		return 0;

	if(last_charset != charset)
	{
		last_charset = charset;
		last_charset_name = k2_get_charset_name(charset);
		KTRACE("Change charset : %d(%s)\n", charset, last_charset_name);
	}

	if(last_charset != KCHARSET_UTF_8)
	{
		gchar *utf8str = g_convert((const gchar*)in_str, in_len, "UTF-8", last_charset_name, &items_read, &utf8_written, &error);
		if(error != NULL)
		{
            KTRACE("Error charset conversion: %s", error->message);
			g_error_free(error);
			return 0;
		}

		if( utf8str == NULL || utf8_written == 0 )
		{
			KTRACE("### K2P_ERROR [%s]: error1 occurs.(in=%d, out=%d)\n", __FUNCTION__, in_len, utf8_written);
			return 0;
		}
		unistr = g_utf8_to_utf16((const gchar*)utf8str, utf8_written, &utf8_read, &items_written, &error);
		if(error != NULL)
		{
            KTRACE("Error g_utf8_to_utf16 conversion: %s", error->message);
			g_error_free(error);
			return 0;
		}

		g_free(utf8str);
	}
	else
	{
		unistr = g_utf8_to_utf16((const gchar*)in_str, in_len, &utf8_read, &items_written, &error);
		if(error != NULL)
		{
            KTRACE("Error g_utf8_to_utf16 conversion: %s", error->message);
			g_error_free(error);
			return 0;
		}
	}

	if( unistr == NULL || items_written == 0 )
	{
		KTRACE("### K2P_ERROR [%s]: error2 occurs.(in=%d, out=%d)\n", __FUNCTION__, utf8_read, items_written);
		return 0;
	}

	if(out_wstr != NULL)
	{
		if(out_wlen > items_written)
			out_wlen = items_written;
		K2P_WStringCopyn((WCHAR *)out_wstr, (const WCHAR *)unistr, out_wlen);
	}
	g_free(unistr);
	// KTRACE("result(%s):in=%d, out=%d\n", in_str, in_len, items_written);

	return items_written;
#else
	if(charset == KCHARSET_UTF_8)
		return MultiByteToWideChar(CP_UTF8, 0, in_str, in_len, (LPWSTR)out_wstr, out_wlen);
	else
		return MultiByteToWideChar(CP_ACP, 0, in_str, in_len, (LPWSTR)out_wstr, out_wlen);
#endif
}

PLATFORMLIB_API int K2P_UnicodeCodeToLocal(const WCHAR* in_wstr, int in_wlen, LPSTR out_str, int out_len)
{
#ifdef PLATFORM_SHP
	int needLength;

	needLength =  WmGetLCodeSizeN((MCHAR *)in_wstr, in_wlen);
	if((out_str != NULL) && (out_len >= needLength))
		WmConvertPCode2UTF((UCHAR *) out_str, out_len+1, (MCHAR *)in_wstr, in_wlen);
	return needLength;
#elif defined(PLATFORM_GTK)
	//TODO:.. by wooarmy
	GError *error = NULL;
	glong items_read = 0;
	glong items_written = 0;
	
	if( in_wstr == NULL )
		return 0;

	gchar *utf8str = g_utf16_to_utf8((const gunichar2*)in_wstr, in_wlen, &items_read, &items_written, &error);
	if(error != NULL)
	{
        KTRACE("Error g_utf16_to_utf8 conversion: %s", error->message);
		g_error_free(error);
		return 0;
	}
	if( utf8str == NULL )
	{
		KTRACE("### K2P_ERROR [%s]: error occurs.\n", __FUNCTION__);
		return 0;
	}
	if(out_str != NULL)
	{
		if( out_len < items_written )
			KTRACE("### K2P_WARNING [%s]: check length.\n", __FUNCTION__);
		strncpy((gchar*)out_str, (const gchar*)utf8str, out_len);
	}
	g_free(utf8str);

	return items_written;
#else
	return WideCharToMultiByte(CP_ACP, 0, (LPWSTR)in_wstr, in_wlen, out_str, out_len, NULL, NULL);
#endif
}

PLATFORMLIB_API int K2P_UnicodeCodeToUtf8(const WCHAR* in_wstr, int in_wlen, LPSTR out_str, int out_len)
{
#ifdef PLATFORM_SHP
	int needLength;

	needLength =  WmGetLCodeSizeN((MCHAR *)in_wstr, in_wlen);
	if((out_str != NULL) && (out_len >= needLength))
		WmConvertPCode2UTF((UCHAR *) out_str, out_len+1, (MCHAR *)in_wstr, in_wlen);
	return needLength;
#elif defined(PLATFORM_GTK)
	//TODO:.. by wooarmy
	GError *error = NULL;
	glong items_read = 0;
	glong items_written = 0;
	
	if( in_wstr == NULL )
		return 0;

	gchar *utf8str = g_utf16_to_utf8((const gunichar2*)in_wstr, in_wlen, &items_read, &items_written, &error);
	if(error != NULL)
	{
        KTRACE("Error g_utf16_to_utf8 conversion: %s", error->message);
		g_error_free(error);
		return 0;
	}
	if( utf8str == NULL )
	{
		KTRACE("### K2P_ERROR [%s]: error occurs.\n", __FUNCTION__);
		return 0;
	}
	if(out_str != NULL)
	{
		if( out_len < items_written )
			KTRACE("### K2P_WARNING [%s]: check length.(%d,%d)\n", __FUNCTION__, out_len, items_written);
		strncpy((gchar*)out_str, (const gchar*)utf8str, out_len);
	}
	g_free(utf8str);

	return items_written;
#else
	return WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)in_wstr, in_wlen, out_str, out_len, NULL, NULL);
#endif
}
#endif  // USE_K2P_CODEC

/***********************************
 SOCKET
***********************************/
#ifdef USE_SOCKET_DOWNLOADER

K2PSocketError socketError(int err)
{
#ifdef PLATFORM_WIN32
	if(err == WSANOTINITIALISED)
		return K2P_SocketError_NotReady;
	else if(err == WSAENETDOWN)
		return K2P_SocketError_NetDown;
	else if(err == WSAEADDRINUSE)
		return K2P_SocketError_AddressInUse;
	else if(err == WSAEINPROGRESS)
		return K2P_SocketError_InProgress;
	else if(err == WSAEADDRNOTAVAIL)
		return K2P_SocketError_AddressNotAvail;
	else if(err == WSAEAFNOSUPPORT)
		return K2P_SocketError_FamilyNotSupport;
	else if(err == WSAECONNREFUSED)
		return K2P_SocketError_ConnectRefused;
	else if(err == WSAEDESTADDRREQ)
		return K2P_SocketError_DestAddressRequired;
	else if(err == WSAEFAULT)
		return K2P_SocketError_BadHostAddressLength;
	else if(err == WSAEINVAL)
		return K2P_SocketError_InvalidHostAddress;
	else if(err == WSAEISCONN)
		return K2P_SocketError_AlreadyConnected;
	else if(err == WSAEMFILE)
		return K2P_SocketError_NoMoreFile;
	else if(err == WSAENETUNREACH)
		return K2P_SocketError_NetworkCannotReach;
	else if(err == WSAENOBUFS)
		return K2P_SocketError_NoBuffer;
	else if(err == WSAENOTSOCK)
		return K2P_SocketError_NoSocket;
	else if(err == WSAETIMEDOUT)
		return K2P_SocketError_TimeOut;
	else if(err == WSAENOTCONN)
		return K2P_SocketError_NotConnected;
	else if(err == WSAEWOULDBLOCK)
		return K2P_SocketError_WouldBlock;
	else if(err == WSAECONNABORTED)
		return K2P_SocketError_ConnectionAborted;
	else if(err == WSAECONNRESET)
		return K2P_SocketError_ConnectionReset;
#endif
	return K2P_Socket_UnknownError;
}

#ifdef _DEBUG
LPCSTR socketErrorMessage(int err)
{
#ifdef PLATFORM_WIN32
	if(err == WSANOTINITIALISED)
		return "NotReady";
	else if(err == WSAENETDOWN)
		return "NetDown";
	else if(err == WSAEADDRINUSE)
		return "AddressInUse";
	else if(err == WSAEINPROGRESS)
		return "InProgress";
	else if(err == WSAEADDRNOTAVAIL)
		return "AddressNotAvail";
	else if(err == WSAEAFNOSUPPORT)
		return "FamilyNotSupport";
	else if(err == WSAECONNREFUSED)
		return "ConnectRefused";
	else if(err == WSAEDESTADDRREQ)
		return "DestAddressRequired";
	else if(err == WSAEFAULT)
		return "BadHostAddressLength";
	else if(err == WSAEINVAL)
		return "InvalidHostAddress";
	else if(err == WSAEISCONN)
		return "AlreadyConnected";
	else if(err == WSAEMFILE)
		return "NoMoreFile";
	else if(err == WSAENETUNREACH)
		return "NetworkCannotReach";
	else if(err == WSAENOBUFS)
		return "NoBuffer";
	else if(err == WSAENOTSOCK)
		return "NoSocket";
	else if(err == WSAETIMEDOUT)
		return "TimeOut";
	else if(err == WSAENOTCONN)
		return "NotConnected";
	else if(err == WSAEWOULDBLOCK)
		return "WouldBlock";
	else if(err == WSAECONNABORTED)
		return "ConnectionAborted";
	else if(err == WSAECONNRESET)
		return "ConnectionReset";
#endif // PLATFORM_WIN32
	return "UnknownError";
}
#endif

#ifdef PLATFORM_SHP
#define PLATFORM_SHP_DOWNLOAD
#endif

#if defined(PLATFORM_SHP_DOWNLOAD)

char* k2p_readed_body_data = NULL;
int k2p_readed_body_length = 0;
int k2p_readed_body_ptr = 0;
int k2p_read_done = 0;

HProtocol k2p_hProto = INVALID_HOBJ;
HHTTP k2p_h_Http = INVALID_HOBJ;
HConnection k2p_h_Connection = INVALID_HOBJ;
HMessage k2p_h_requestMsg = INVALID_HOBJ;
BOOL k2p_state_close = false;
BOOL k2p_connected = false;

#define M_TRACE		KTRACE
// #define M_TRACE

static void
k2c_ProtoCallback(ProtoEvent* pEvent)
{
    switch (pEvent->type)
    {
    case PROTO_BEARER_STARTED:
		M_TRACE("ProtoCallback::PROTO_BEARER_STARTED\n");
        // pHttp->StartHttp();
		k2p_connected = true;
        break;

    case PROTO_BEARER_SUSPEND:
		M_TRACE("ProtoCallback::PROTO_BEARER_SUSPEND\n");
        break;

    case PROTO_BEARER_RESUME:
		M_TRACE("ProtoCallback::PROTO_BEARER_RESUME\n");
        break;

    case PROTO_BEARER_STOPPED:
		M_TRACE("ProtoCallback::PROTO_BEARER_STOPPED\n");
		// ignore PROTO_BEARER_STOPPED for temporary
		// k2p_state_close = true;
        break;

	/*
    case PROTO_SOCKET_USER:
        break;

    case PROTO_SSL_CERT_EVT:
        break;
	*/

    default:
		M_TRACE("ProtoCallback::eventType=%d\n", pEvent->type);
        break;
    }
}

static void
k2c_HttpCallback(HttpEventType eventType, HttpErrorCode errorCode, void* detailInfo, HMessage hMessage)
{
    switch (eventType)
    {
    case HTTP_EVENT_SEND_MORE_DATA_REQ:
		M_TRACE("HttpCallback::HTTP_EVENT_SEND_MORE_DATA_REQ\n");
        break;

    case HTTP_EVENT_READ_HEADER:
	{
		int readed_body_length;
		UINT32 Status;

		readed_body_length = HttpQueryHeaderLength(hMessage);
		M_TRACE("HttpCallback::HTTP_EVENT_READ_HEADER(%d)\n", readed_body_length);

		if(readed_body_length > 0)
		{
			if(k2p_readed_body_data == NULL)
			{
				k2p_readed_body_data = (char*) K2P_MemAlloc(readed_body_length + 1);
				k2p_readed_body_length = 0;
			}
			else
			{
				k2p_readed_body_data = (char*) K2P_MemRealloc(k2p_readed_body_data, k2p_readed_body_length + readed_body_length + 1);
			}

			if(k2p_readed_body_data != NULL)
			{
				HttpGetResponseHeader(hMessage, &Status, k2p_readed_body_data + k2p_readed_body_length, readed_body_length);
				k2p_readed_body_length += readed_body_length;
			}
		}

        break;
	}

    case HTTP_EVENT_READ_BODY:
	{
		int readed_body_length;

		readed_body_length = HttpQueryBodyLength(hMessage);
		M_TRACE("HttpCallback::HTTP_EVENT_READ_BODY(%d)\n", readed_body_length);

		if(readed_body_length > 0)
		{
			if(k2p_readed_body_data == NULL)
			{
				k2p_readed_body_data = (char*) K2P_MemAlloc(readed_body_length + 1);
				k2p_readed_body_length = 0;
			}
			else
			{
				k2p_readed_body_data = (char*) K2P_MemRealloc(k2p_readed_body_data, k2p_readed_body_length + readed_body_length + 1);
			}

			if(k2p_readed_body_data != NULL)
			{
				HttpGetResponseBody(hMessage, k2p_readed_body_data + k2p_readed_body_length, readed_body_length);
				k2p_readed_body_length += readed_body_length;
			}
		}
		break;
	}

    case HTTP_EVENT_READ_COMPLETE:
		M_TRACE("HttpCallback::HTTP_EVENT_READ_COMPLETE\n");
		k2p_read_done = 1;
        break;

    case HTTP_EVENT_ABORT_REQUEST:
		M_TRACE("HttpCallback::HTTP_EVENT_ABORT_REQUEST\n");
        break;

    case HTTP_EVENT_CERT_VERIFY_NOTI:
	{
		HttpCertBuf certBuf;

		M_TRACE("HttpCallback::HTTP_EVENT_CERT_VERIFY_NOTI\n");
		if (HttpGetCertificate(hMessage, &certBuf))
		{
			SysDebug((MID_EXCEPTION, "%s: HTTP_EVENT_CERT_VERIFY_NOTI\n%s\n",  __FUNCTION__, certBuf.szCertBuf));
		}
		HttpResumeRequest(hMessage, HTTP_MSG_CMD_CERT_RESUME);
		break;
	}

	default:
		M_TRACE("HttpCallback::eventType=%d\n", eventType);
		break;
	}
}

#endif

PLATFORMLIB_API HANDLE K2P_SocketCreate(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;
	int err;

	if(socket == NULL)
		socket = new KHttpSocket();
	if(socket != NULL)
	{
    int nPort = 0; // local address port should be auto

		if(socket->Create(nPort, SOCK_STREAM) == 0)
		{
			err = GetLastError();
#ifdef _DEBUG
			KTRACE("Socket Create error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			delete socket;
			socket = NULL;
		}
    else
    {
      /*
      linger ling;
      int buff_size = sizeof(linger);
      int r = socket->GetSockOpt(SO_LINGER, &ling, &buff_size);
      ling.l_onoff = 1; // remove buffered data
      r = socket->SetSockOpt(SO_LINGER, &ling, buff_size);
      */
      BOOL reuseAddress = FALSE;
      int buff_size = sizeof(BOOL);
      int r = socket->GetSockOpt(SO_REUSEADDR, &reuseAddress, &buff_size);
      reuseAddress = TRUE;
      r = socket->SetSockOpt(SO_REUSEADDR, &reuseAddress, buff_size);
    }
	}
	return (HANDLE) socket;
#elif defined(PLATFORM_SHP_DOWNLOAD)
	M_TRACE("ProtoRegisterClient\n");

	//const char* hname = WmGetEventHandlerName(WmGetCurrentEventHandler());

	k2p_connected = false;
	k2p_state_close = false;
	k2p_hProto = ProtoRegisterClient(PROTO_EVENT_CLASS_BEARER, k2c_ProtoCallback, "k2c_event_handler");

	HProtoAccount hAccount = INVALID_HOBJ;
	SettingSvcAccessPointGetAccountHandle (SETTING_SVC_ACCESS_POINT_EMAIL, &hAccount);

	if (k2p_hProto == INVALID_HOBJ && hAccount == INVALID_HOBJ)
	{
		M_TRACE("K2P_SocketCreate: Invalid m_hProto(%d) hAccount(%d)\n", k2p_hProto, hAccount);
		return NULL;
	}

	if (ProtoSetNetworkInfo(k2p_hProto, hAccount) == FALSE)
	{
		M_TRACE("K2P_SocketCreate: ProtoSetNetworkInfo(err=%d)\n", SysGetLastError());
		return NULL;
	}

	if (ProtoStartNetwork(k2p_hProto) == FALSE)
	{
		M_TRACE( "K2P_SocketCreate: ProtoStartNetwork(err=%d)\n", SysGetLastError());
		return NULL;
	}

	M_TRACE( "ProtoStartNetwork Ok(handle=%d)\n", k2p_hProto);
	return (HANDLE) k2p_hProto;
#else
	return NULL;
#endif
}

PLATFORMLIB_API K2PSocketError K2P_SocketConnect(HANDLE h, const char* server_address, int port)
{
#ifdef PLATFORM_WIN32
	int err;
	WCHAR wserver[256];
	KHttpSocket* socket = (KHttpSocket *) h;

	int len = K2P_Utf8CodeToUnicode(server_address, strlen(server_address), (WCHAR *)wserver, 256);
	wserver[len] = 0;

	socket->Init();
	err = socket->Connect((LPCTSTR)wserver, port);
	if(err == 0)
	{
		err = GetLastError();

		if(err != WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			KTRACE("Socket Connect error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			K2PSocketError serr = socketError(err);
			return serr;
		}
	}

	return K2P_Socket_NoError;
#elif defined(PLATFORM_SHP_DOWNLOAD)
	HProtocol hProto = (HProtocol) h;

	if(k2p_readed_body_data != NULL)
	{
		K2P_MemFree(k2p_readed_body_data);
		k2p_readed_body_data = NULL;
	}
	k2p_readed_body_length = 0;
	k2p_readed_body_ptr = 0;
	k2p_read_done = 0;

	//const char* hname = WmGetEventHandlerName(WmGetCurrentEventHandler());

	k2p_h_Http = HttpOpenClientByCallback(hProto, k2c_HttpCallback, "k2c_event_handler");
	M_TRACE("HttpOpenClientByCallback, h_http=%d\n", k2p_h_Http);
	if( k2p_h_Http == INVALID_HOBJ )
	{
		KTRACE("[Error]%s: HttpOpenClientByCallback(%d)\n", __FUNCTION__, SysGetLastError());
		return K2P_SocketError_NotReady;
	}

	k2p_h_Connection = HttpOpenConnection(k2p_h_Http, HTTP_SCHEME_NORMAL, (char *)server_address);
	M_TRACE("HttpOpenConnection, h_Connection=%d\n", k2p_h_Connection);
	if( k2p_h_Connection == INVALID_HOBJ )
	{
		KTRACE("[Error]%s: HttpOpenConnection(%d)\n", __FUNCTION__, SysGetLastError());
		return K2P_SocketError_NotReady;
	}

	return K2P_Socket_NoError;
#else
	return K2P_SocketError_NotReady;
#endif
}

PLATFORMLIB_API K2PSocketError K2P_SocketBind(HANDLE h, SOCKADDR* sockAddr, int sockAddrLen)
{
	KHttpSocket* socket = (KHttpSocket *) h;
	int err = socket->Bind(sockAddr, sockAddrLen);
	if(err == 0)
	{
		err = GetLastError();
		if(err != WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			KTRACE("Socket Bind error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			K2PSocketError serr = socketError(err);
			return serr;
		}
	}
	return K2P_Socket_NoError;
}

PLATFORMLIB_API K2PSocketError K2P_SocketGetAddress(HANDLE h, SOCKADDR* sockAddr, int* sockAddrLen)
{
	KHttpSocket* socket = (KHttpSocket *) h;
  /*
  CString socketAddress;
  UINT sockPort;
	int err = socket->GetPeerName(socketAddress, sockPort);
  */
	int err = socket->GetSockName(sockAddr, sockAddrLen);
	if(err == 0)
	{
		err = GetLastError();
		if(err != WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			KTRACE("Socket GetAddress error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			K2PSocketError serr = socketError(err);
			return serr;
		}
	}
	return K2P_Socket_NoError;
}

PLATFORMLIB_API BOOL K2P_SocketSendRequestEnable()
{
#if defined(PLATFORM_SHP_DOWNLOAD)
	return true;
#else
	return false;
#endif
}

PLATFORMLIB_API BOOL K2P_SocketReceiveWithHeader()
{
#if defined(PLATFORM_SHP_DOWNLOAD)
	return true;
#else
	return true;
#endif
}

PLATFORMLIB_API K2PSocketError K2P_SocketSend(HANDLE h, LPBYTE buff, int size)
{
#ifdef PLATFORM_WIN32
	int err;
	KHttpSocket* socket = (KHttpSocket *) h;

	// Sleep(1);
	err = socket->Send(buff, size);
	if(err == 0)
	{
		err = GetLastError();

		if(err != WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			KTRACE("Socket Connect error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			K2PSocketError serr = socketError(err);
			return serr;
		}
		KTRACE("Socket send(%d)\n", size);
	}
	return K2P_Socket_NoError;
#else
	return K2P_SocketError_NotReady;
#endif
}

PLATFORMLIB_API K2PSocketError K2P_SocketSendRequest(HANDLE h, const char* in_url)
{
#if defined(PLATFORM_SHP_DOWNLOAD)
	k2p_h_requestMsg = HttpMakeRequest(k2p_h_Connection, HTTP_REQUEST_METHOD_GET, in_url, "Accept-Language: en-us");
	M_TRACE("HttpMakeRequest, h_requestMsg=%d\n", k2p_h_requestMsg);
	if( k2p_h_requestMsg == INVALID_HOBJ )
	{
		KTRACE("[Error]%s: HttpMakeRequest(%d)\n", __FUNCTION__, SysGetLastError());
		return K2P_SocketError_NotReady;
	}
	
	if( !HttpSendRequest(k2p_h_requestMsg) )
	{
		//KTRACE_0("HttpSendRequest failure\n");
		KTRACE("[Error]%s: HttpSendRequest(%d)\n", __FUNCTION__, SysGetLastError());
		return K2P_SocketError_NotReady;
	}
#endif
	return K2P_Socket_NoError;
}

PLATFORMLIB_API int K2P_SocketHasConnect(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;

	return socket->hasConnect();
#elif defined(PLATFORM_SHP_DOWNLOAD)
	return(k2p_h_Connection != INVALID_HOBJ); // k2p_connected;
#else
	return false;
#endif
}

PLATFORMLIB_API int K2P_SocketHasReceive(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;

	return socket->hasReceive();
#elif defined(PLATFORM_SHP_DOWNLOAD)
	return(k2p_readed_body_length > 0);
#else
	return false;
#endif
}

PLATFORMLIB_API int K2P_SocketHasSend(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;

	return socket->hasSend();
#elif defined(PLATFORM_SHP_DOWNLOAD)
	return true;
#else
	return false;
#endif
}

PLATFORMLIB_API int K2P_SockethasClose(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;
	return socket->hasClose();
#elif defined(PLATFORM_SHP_DOWNLOAD)
	return k2p_state_close;
#else
	return false;
#endif
}

PLATFORMLIB_API BOOL K2P_SocketReceiveDone(HANDLE h)
{
#if defined(PLATFORM_SHP_DOWNLOAD)
	return ((k2p_read_done==0) ? false : true);
#else
	return false;
#endif
}


PLATFORMLIB_API int K2P_SocketReceive(HANDLE h, LPBYTE buff, int size)
{
#ifdef PLATFORM_WIN32
	int err;
	KHttpSocket* socket = (KHttpSocket *) h;

	socket->clearReceive();
	err = socket->Receive(buff, size, 0);
	if(err < 0)
	{
		err = GetLastError();
		if(err != WSAEWOULDBLOCK)
		{
#ifdef _DEBUG
			KTRACE("Socket Receive error(%d, %s)\n", err, socketErrorMessage(err));
#endif
			K2PSocketError serr = socketError(err);
			return serr;
		}
		// Sleep(1);
		return K2P_SocketError_WouldBlock;
	}
	return err;
#elif defined(PLATFORM_SHP_DOWNLOAD)
	if((k2p_readed_body_length > 0) && (k2p_readed_body_data != NULL))
	{
		if(k2p_readed_body_ptr < k2p_readed_body_length)
		{
			int len;

			len = k2p_readed_body_length - k2p_readed_body_ptr;
			if(len > size)
				len = size;
			K2P_MemoryCopy(buff, k2p_readed_body_data + k2p_readed_body_ptr, len);
			k2p_readed_body_ptr += len;

			if(k2p_readed_body_ptr >= k2p_readed_body_length)
			{
				k2p_readed_body_ptr = 0;
				k2p_readed_body_length = 0;
				K2P_MemFree(k2p_readed_body_data);
				k2p_readed_body_data = NULL;
			}
			return len;
		}
	}
	return(k2p_read_done ? 0 : K2P_SocketError_WouldBlock);
#else
	return 0;
#endif
}


PLATFORMLIB_API void K2P_SocketShutDown(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;

  linger ling;
  int buff_size = sizeof(linger);
  int r = socket->GetSockOpt(SO_LINGER, &ling, &buff_size);
  ling.l_onoff = 1; // remove buffered data
  ling.l_linger = 0;
  r = socket->SetSockOpt(SO_LINGER, &ling, buff_size);

  r = socket->ShutDown(CAsyncSocket::both);
	KTRACE("Socket ShutDown(%d)\n", r);
#endif
}

PLATFORMLIB_API void K2P_SocketClose(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;

	socket->Close();
	KTRACE("Socket Close\n");

#elif defined(PLATFORM_SHP_DOWNLOAD)
	if(k2p_readed_body_data != NULL)
	{
		K2P_MemFree(k2p_readed_body_data);
		k2p_readed_body_data = NULL;
	}
	k2p_readed_body_length = 0;
	k2p_readed_body_ptr = 0;

	if(k2p_h_requestMsg != INVALID_HOBJ)
	{
		M_TRACE("HttpCloseRequest, h_requestMsg=%d\n", k2p_h_requestMsg);
		HttpCloseRequest(k2p_h_requestMsg);
		k2p_h_requestMsg = INVALID_HOBJ;
	}
	if(k2p_h_Connection != INVALID_HOBJ)
	{
		M_TRACE("HttpCloseConnection, h_Connection=%d\n", k2p_h_Connection);
		HttpCloseConnection(k2p_h_Connection);
		k2p_h_Connection = INVALID_HOBJ;
	}
	if(k2p_h_Http != INVALID_HOBJ)
	{
		M_TRACE("HttpCloseClient, h_Http=%d\n", k2p_h_Http);
		HttpCloseClient(k2p_h_Http);
		k2p_h_Http = INVALID_HOBJ;
	}

	M_TRACE("ProtoStopNetwork(%d)\n", h);
	ProtoStopNetwork((HProtocol)h);
	M_TRACE("ProtoUnregisterClient(%d)\n", h);
	ProtoUnregisterClient((HProtocol)h);

	k2p_connected = false;
	k2p_hProto = INVALID_HOBJ;

#endif
}

PLATFORMLIB_API void K2P_SocketDelete(HANDLE h)
{
#ifdef PLATFORM_WIN32
	KHttpSocket* socket = (KHttpSocket *) h;
	delete socket;
#endif
}
#endif // USE_SOCKET_DOWNLOADER

void KTRACE(const char *msg, ... )
{
  char buf[1024];

  va_list ap;
  va_start( ap, msg );			// use variable arg list
#ifdef WINCE
	vsprintf( buf, msg, ap );
#elif defined(PLATFORM_GTK)
	vsprintf( buf, msg, ap );
#elif defined(PLATFORM_SHP)
	AcVsprintf( buf, msg, ap );
#else
	vsprintf_s( buf, msg, ap );
#endif
	va_end( ap );

	WCHAR wstr[2048];
	int len = (int)strlen(buf);
	len = MultiByteToWideChar(CP_ACP, 0, buf, len, wstr, 2048);
	wstr[len] = '\0';
	OutputDebugString(wstr);
}

void KTRACE_W(LPCTSTR msg, ... )
{
  TCHAR buf[1024];

  va_list ap;
  va_start( ap, msg );			// use variable arg list
#ifdef WINCE
	vsprintf( buf, msg, ap );
#elif defined(PLATFORM_GTK)
	vsprintf( buf, msg, ap );
#elif defined(PLATFORM_SHP)
	AcVsprintf( buf, msg, ap );
#else
	StringCchPrintf(buf, 1024, msg, ap);
#endif
	va_end( ap );

	OutputDebugString(buf);
}

DWORD K2P_GetSystemTimeTick()
{
  return GetTickCount();
}

PLATFORMLIB_API DWORD K2P_GetSystemTimeAfter(DWORD start)
{
	DWORD current;
	
	current = GetTickCount();
	if(start <= current)
		return current - start;
	else
		return(0xFFFFFFFF - start + current);
}

/***********************************
 FILE
***********************************/

// openMode : ReadOnly = 0, WriteOnly = 1, ReadWrite = 2
// truncate_old_file : not truncate = 0, truncate old file = 1
PLATFORMLIB_API HANDLE K2P_OpenFile(int mode, LPCTSTR filename, int truncate_old)
{
	BOOL truncate_file = false;

	if((mode != 0) && truncate_old)
		truncate_file = K2P_FileExist(filename);

#ifdef PLATFORM_SHP
	ULONG fm_mode;
	HANDLE h;

	if(mode == 0)
		fm_mode = FM_READ;
	else if(mode == 1)
		fm_mode = FM_CREATE | FM_WRITE;
	else
		fm_mode = FM_CREATE | FM_READ | FM_WRITE;

	if(truncate_file)
		fm_mode |= FM_TRUNCATE;

	if(filename[0] != '/')
		return NULL;

	h = (HANDLE)FmOpenFile(filename, fm_mode);
	if(h == (HANDLE)INVALID_HOBJ)
		h = 0;
	return h;
#elif defined(PLATFORM_GTK)
	HANDLE h;
	int oflag = 0;

	if(mode == 0)
		oflag |= O_RDONLY;
	else if(mode == 1)
		oflag |= (O_WRONLY | O_CREAT);
	else
		oflag |= O_RDWR;

	if(truncate_file)
		oflag |= O_TRUNC;

    if(mode == 1)
		h = (HANDLE)open(filename, oflag, 0755);
	else
		h = (HANDLE)open(filename, oflag);

	if(h == (HANDLE)0xFFFFFFFF)
		h = 0;
	return h;
#elif defined(WIN32_SBCS)
  DWORD access, share, create;
  HANDLE h;
  /*
  WCHAR wfilename[256];
  int len;

  len = K2P_Utf8CodeToUnicode(filename, K2P_StringLength(filename), wfilename, 256);
  wfilename[len] = 0;
  */
  access = share = create = 0;
  if (mode == 0)
  {
    access |= GENERIC_READ;
    share |= FILE_SHARE_READ;
    create = OPEN_EXISTING;
  }
  else if (mode == 1)
  {
    access |= GENERIC_WRITE;
    share = 0;
    create = CREATE_ALWAYS;
  }
  else if (mode == 2)
  {
    access |= GENERIC_READ | GENERIC_WRITE;
    share |= FILE_SHARE_READ;
    create = OPEN_EXISTING;
    if (truncate_file)
      create |= TRUNCATE_EXISTING;
  }

  h = CreateFile(wfilename, access, share, NULL, create, 0, 0);
  if (h == INVALID_HANDLE_VALUE)
    h = 0;
  return h;
#else
	DWORD access, share, create;
	HANDLE h;

  access = share = create = 0;
	if(mode == 0)
	{
		access |= GENERIC_READ;
		share |= FILE_SHARE_READ;
		create = OPEN_EXISTING;
	}
	else if(mode == 1)
	{
		access |= GENERIC_WRITE;
		share = 0;
		create = CREATE_ALWAYS;
	}
	else if(mode == 2)
	{
		access |= GENERIC_READ | GENERIC_WRITE;
		share |= FILE_SHARE_READ;
		create = OPEN_EXISTING;
		if(truncate_file)
			create |= TRUNCATE_EXISTING;
	}

	h = CreateFile(filename, access, share, NULL, create, 0, 0);
	if(h == INVALID_HANDLE_VALUE)
		h = 0;
	return h;
#endif
}

PLATFORMLIB_API int K2P_ReadFile(HANDLE fh, void* lpBuff, int nsize)
{
#ifdef PLATFORM_SHP
	ULONG nReaded;
	FmReadFile((HFile)fh, lpBuff, (ULONG) nsize, &nReaded);
#elif defined(QT_BASE)
	#ifdef QT_V4
		DWORD nReaded = read((char *) lpBuff, (uint) nsize);
	#else
		DWORD nReaded = readBlock((char *) lpBuff, (DWORD) nsize);
	#endif
#elif defined(PLATFORM_GTK)
	DWORD nReaded = read((int)fh, lpBuff, nsize);
#else
	DWORD nReaded;
	ReadFile(fh, lpBuff, (DWORD) nsize, &nReaded, NULL);
#endif
	return (int) nReaded;
}

PLATFORMLIB_API int K2P_WriteFile(HANDLE fh, const void* lpBuff, int nsize)
{
#ifdef PLATFORM_SHP
	ULONG nWrited = 0;
	if( FmWriteFile((HFile)fh, (LPVOID) lpBuff, (ULONG) nsize) )
		nWrited = nsize;
#elif defined(QT_BASE)
	#ifdef QT_V4
		DWORD nWrited = write((const char *) lpBuff, (uint) nsize);
	#else
		DWORD nWrited = writeBlock((const char *) lpBuff, (uint) nsize);
	#endif
#elif defined(PLATFORM_GTK)
	DWORD nWrited = write((int)fh, lpBuff, nsize);
#else
	DWORD nWrited;
	WriteFile(fh, (LPVOID) lpBuff, (DWORD) nsize, &nWrited, NULL);
#endif
	return (int) nWrited;
}

// position : begin = 0, current = 1, end = 2
PLATFORMLIB_API int K2P_SeekFile(HANDLE fh, int offset, int position)
{
#ifdef PLATFORM_SHP
	if((offset != 0) || (position != 1))
		FmSeekFile((HFile)fh, position, offset);
	offset = FmTellFile((HFile)fh);
#elif defined(QT_BASE)
		if(position == KFile::current)
	#ifdef QT_V4
			offset += pos();
	#else
			offset += at();
	#endif
		else if(position == KFile::end)
			offset = size() - offset;
	#ifdef QT_V4
		seek(offset);
	#else
		at(offset);
	#endif
#elif defined(PLATFORM_GTK)
	int whence = SEEK_SET;
	switch(position)
	{
	case 0:
		whence = SEEK_SET;
		break;

	case 1:
		whence = SEEK_CUR;
		break;

	case 2:
		whence = SEEK_END;
		break;
	}
	offset = lseek((int)fh, offset, whence);
#else
	offset = SetFilePointer(fh, offset, 0, position);
#endif
	return offset;
}

PLATFORMLIB_API int K2P_FileSize(HANDLE fh)
{
#ifdef PLATFORM_SHP
	int size, offset;

	offset = FmTellFile((HFile)fh);
	size = K2P_SeekFile(fh, 0, 2);
	K2P_SeekFile(fh, offset, 0);
	return size;
#elif defined(QT_BASE)
	return size();
#elif defined(PLATFORM_GTK)
	struct stat fs;
	if( fstat((int)fh, &fs) == -1 )
		return 0;
	return fs.st_size;
#else
	DWORD sizeHigh;
	if(fh != 0)
		return GetFileSize(fh, &sizeHigh);
	return 0;
#endif
}

PLATFORMLIB_API void K2P_FileClose(HANDLE fh)
{
#ifdef PLATFORM_SHP
	FmCloseFile((HFile)fh);
#elif defined(QT_BASE)
	close();
#elif defined(PLATFORM_GTK)
	close((int)fh);
#else
	CloseHandle(fh);
#endif
}


PLATFORMLIB_API BOOL K2P_FileCopy(LPCTSTR oldfilename, LPCTSTR newfilename, BOOL bFailIfExists)
{
#ifdef PLATFORM_SHP
	if(oldfilename[0] != '/')
		return false;

	return !!FmCopyFile(oldfilename, newfilename, bFailIfExists);
#elif defined(PLATFORM_GTK)
	// not ready
	if( g_copy(oldfilename, newfilename) == -1 )
		return false;
	return true;
#elif defined(WIN32_SBCS)
	WCHAR woldname[256];
	WCHAR wnewname[256];

	int len = K2P_Utf8CodeToUnicode(oldfilename, K2P_StringLength(oldfilename), woldname, 256);
	woldname[len] = 0;
	len = K2P_Utf8CodeToUnicode(newfilename, K2P_StringLength(newfilename), wnewname, 256);
	wnewname[len] = 0;

	return !!CopyFile((LPCWSTR)woldname, (LPCWSTR)wnewname, bFailIfExists);
#else
  return !!CopyFile(oldfilename, newfilename, bFailIfExists);
#endif
}

PLATFORMLIB_API BOOL K2P_FileMove(LPCTSTR oldfilename, LPCTSTR newfilename)
{
#ifdef PLATFORM_SHP
	if(oldfilename[0] != '/')
		return false;

	return !!FmMoveFile(oldfilename, newfilename);
#elif defined(PLATFORM_GTK)
	if( g_rename(oldfilename, newfilename) == -1 )
		return false;
	return true;
#elif defined(WIN32_SBCS)
  WCHAR woldname[256];
  WCHAR wnewname[256];

  int len = K2P_Utf8CodeToUnicode(oldfilename, K2P_StringLength(oldfilename), woldname, 256);
  woldname[len] = 0;
  len = K2P_Utf8CodeToUnicode(newfilename, K2P_StringLength(newfilename), wnewname, 256);
  wnewname[len] = 0;

  return !!MoveFile((LPCWSTR)woldname, (LPCWSTR)wnewname);
#else
	return !!MoveFile(oldfilename, newfilename);
#endif
}

PLATFORMLIB_API BOOL K2P_FileRemove(LPCTSTR filename)
{
#ifdef PLATFORM_SHP
	if(filename[0] != '/')
		return false;

	return !!FmRemoveFile(filename);
#elif defined(PLATFORM_GTK)
	if( remove(filename) == -1 )
		return false;
	return true;
#elif defined(WIN32_SBCS)
  WCHAR wfilename[256];

  int len = K2P_Utf8CodeToUnicode(filename, K2P_StringLength(filename), wfilename, 256);
  wfilename[len] = 0;
  return !!DeleteFile((LPCWSTR)wfilename);
#else
	return !!DeleteFile(filename);
#endif
}

PLATFORMLIB_API BOOL K2P_FileRemoveW(const WCHAR* filename)
{
#ifdef PLATFORM_SHP
	char o_name[256];

	if(filename[0] != '/')
		return false;

	int len = K2P_UnicodeCodeToUtf8((const WORD*)filename, K2P_WStringLength((const WORD*)filename), o_name, 256);
	o_name[len] = 0;

	return !!FmRemoveFile(o_name);
#elif defined(PLATFORM_GTK)
	char o_name[256];

	int len = K2P_UnicodeCodeToUtf8((const WORD*)filename, K2P_WStringLength((const WORD*)filename), o_name, 256);
	o_name[len] = 0;

	if( remove(filename) == -1 )
		return false;
	return true;
#else
	return !!DeleteFile((LPCWSTR)filename);
#endif
}

PLATFORMLIB_API BOOL K2P_isLocalFile(LPCTSTR filename)
{
#if defined(PLATFORM_SHP) || defined(PLATFORM_GTK)
	if(filename[0] == '/')
		return true;
#elif defined(PLATFORM_WIN32)
	if(isAlpha(filename[0]) && (filename[1] == ':'))
		return true;
	if((filename[0] == '\\') && (filename[1] == '\\'))
		return true;
	return false;
#endif
	return false;
}

PLATFORMLIB_API BOOL K2P_FileExist(LPCTSTR filename)
{
#ifdef PLATFORM_SHP
	FmFileAttribute fileAttribute;

	if((filename == NULL) || (filename[0] != '/'))
		return false;

	AcMemset(&fileAttribute, 0, sizeof(FmFileAttribute));
	if( FmGetFileAttributes(filename, &fileAttribute) &&
		!(fileAttribute.attribute & FM_ATTRIBUTE_DIR) )
	{
		return true;
	}
	return false;

#elif defined(WIN32_MFC)
	OFSTRUCT        of;
	HFILE			hFile;
	WCHAR wfilename[256];

	int len = K2P_Utf8CodeToUnicode(filename, K2P_StringLength(filename), (WORD *)wfilename, 256);
	wfilename[len] = 0;
	hFile = OpenFile(wfilename, (OFSTRUCT *) &of, OF_EXIST);
	return( (hFile == HFILE_ERROR) ? FALSE : TRUE );
#elif defined(PLATFORM_GTK)
	int fd;

	fd = open(filename, O_RDONLY);
	if( fd == -1 )
		return false;
	close(fd);

	return true;
#elif defined(WIN32_SBCS)
  BOOL exist;
  HANDLE h;
  WCHAR wfilename[256];

  int len = K2P_Utf8CodeToUnicode(filename, K2P_StringLength(filename), wfilename, 256);
  wfilename[len] = 0;

  h = CreateFile((LPCWSTR)wfilename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
  if (h == INVALID_HANDLE_VALUE)
    h = CreateFile((LPCWSTR)wfilename, 0, 0, NULL, OPEN_EXISTING, 0, 0);

  exist = (h != INVALID_HANDLE_VALUE);
  if (h != INVALID_HANDLE_VALUE)
    CloseHandle(h);
  return exist;
#else
	BOOL exist;
	HANDLE h;

  h = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
	if(h == INVALID_HANDLE_VALUE)
		h = CreateFile(filename, 0, 0, NULL, OPEN_EXISTING, 0, 0);

	exist = (h != INVALID_HANDLE_VALUE);
	if(h != INVALID_HANDLE_VALUE)
		CloseHandle(h);
	return exist;
#endif
}

PLATFORMLIB_API char K2P_GetDirectoryDelimiter()
{
#ifdef PLATFORM_SHP
	return '/';
#elif defined(PLATFORM_GTK)
	return '/';
#else
	return '/'; // '\\'
#endif
}

PLATFORMLIB_API BOOL K2P_IsDirectoryDelimeter(WCHAR ch)
{
#ifdef PLATFORM_WIN32
	return((ch == '/') || (ch == '\\'));
#else
	return(ch == '/');
#endif
}

