#ifndef _PLATFORM_LIB_H_
#define _PLATFORM_LIB_H_

/***********************************
 Platform Porting Layer for Win32
***********************************/

#ifdef __cplusplus
extern "C" {
#endif

#define isWhiteSpace(c)		(((c >= 9) && (c <= 13)) || (c == 0x20))
#define isCrLf(c)			((c == 10) || (c == 13))

#define isNumeric(c)		((c >= '0') && (c <= '9'))
#define isAlpha(c)			(((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z')))
#define isNumericHexa(c)	(((c >= '0') && (c <= '9')) || ((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')))

#define isAlphaUpper(c)		((c >= 'A') && (c <= 'Z'))
#define isAlphaLower(c)		((c >= 'a') && (c <= 'z'))
#define toLowerChar(c)		(((c >= 'A') && (c <= 'Z')) ? (c - 'A' + 'a') : c)
#define toUpperChar(c)		(((c >= 'a') && (c <= 'z')) ? (c - 'a' + 'A') : c)

#define isOpenBraceChar(c)	((c == '(') || (c == '{'))
#define isCloseBraceChar(c)	((c == ')') || (c == '}'))

#define is_directory_delimeter(c)	((c =='/') || (c == '\\'))

#define PLATFORMLIB_API 
//#define USE_SOCKET_DOWNLOADER
#define PLATFORM_WIN32

#define USE_K2P_CODEC

#ifdef USE_K2P_CODEC
#include "KCharset.h"
#endif

/***********************************
 STDIO
***********************************/
/**
@brief Write formatted data to a string.

@param buf Storage location for output.
@param format Format-control string.
@param argument Optional arguments.
@return the number of bytes stored in buffer, not counting the terminating null character.
*/
PLATFORMLIB_API int K2P_sprintf(char* buf, const char* format,...);
/**
@brief Write formatted data to a string.

@param buf Storage location for output.
@param bufflen Maximum number of characters to store.
@param format Format-control string.
@param argument Optional arguments.
@return the number of bytes stored in buffer, not counting the terminating null character.
*/
PLATFORMLIB_API int K2P_snprintf(char* buf, int bufflen, const char* format,...);
/**
@brief Copy a string.

@param d Destination string.
@param s Null-terminated source string.
@return the destination string.
*/
PLATFORMLIB_API LPSTR K2P_StringCopy(LPSTR d, LPCSTR s);

/**
@brief Cocat a string.

@param d Destination string.
@param s Null-terminated source string.
@return the destination string.
*/
PLATFORMLIB_API LPSTR K2P_StringCat(LPSTR d, LPCSTR s);

/**
@brief Copy characters of one string to another.

@param d Destination string.
@param s Null-terminated source string.
@param n Number of characters to be copied.
@return the destination string.
*/
PLATFORMLIB_API LPSTR K2P_StringCopyn(LPSTR d, LPCSTR s, int n);
/**
@brief Get the length of a string.

@param str Null-terminated string.
@return the number of characters in string, excluding the terminal NULL.
*/
PLATFORMLIB_API int K2P_StringLength(LPCSTR str);
/**
@brief Compare strings.

@param s Null-terminated strings to compare. 
@param d Null-terminated strings to compare. 
@return the lexicographic relation of string1 to string2.
*/
PLATFORMLIB_API int K2P_StringCompare(LPCSTR s, LPCSTR d);

/**
@brief Compare characters of two strings.

@param s Null-terminated strings to compare. 
@param d Null-terminated strings to compare. 
@param n Number of characters to compare.
@return the lexicographic relation of string1 to string2.
*/
PLATFORMLIB_API int K2P_StringCompareN(LPCSTR s, LPCSTR d, int n);

/**
@brief Write formatted data to a wide-character string.

@param buf Storage location for output.
@param bufflen Maximum number of characters to store.
@param format Format-control string.
@param argument Optional arguments.
@return the number of bytes stored in buffer, not counting the terminating null character.
*/
PLATFORMLIB_API int K2P_snwprintf(WCHAR* buf, int bufflen, const WCHAR* format,...);
/**
@brief Copy a wide-character string.

@param d Destination string.
@param s Null-terminated source string.
@return the destination string.
*/
PLATFORMLIB_API WCHAR* K2P_WStringCopy(WCHAR* d, int len, const WCHAR* s);
/**
@brief Copy wide-character of one string to another.

@param d Destination string.
@param s Null-terminated source string.
@param n Number of characters to be copied.
@return the destination string.
*/
PLATFORMLIB_API WCHAR* K2P_WStringCopyn(WCHAR* d, int len, const WCHAR* s, int n);
/**
@brief Get the length of a wide-character string.

@param d Null-terminated string.
@return the number of characters in string, excluding the terminal NULL.
*/
PLATFORMLIB_API int K2P_WStringLength(const WCHAR* d);
/**
@brief Compare wide-character strings.

@param s Null-terminated strings to compare. 
@param d Null-terminated strings to compare. 
@return the lexicographic relation of string1 to string2.
*/
PLATFORMLIB_API int K2P_WStringCompare(const WCHAR* s, const WCHAR* d);

/**
@brief Convert strings to integer.

@param p String to be converted.
@return the int value produced by interpreting the input characters as a number. 
*/
PLATFORMLIB_API int K2P_atoi(const char *p);

PLATFORMLIB_API __int64 K2P_atoi64(const char *p);

/**
@brief Convert strings to double.

@param p String to be converted.
@return the double value produced by interpreting the input characters as a number. 
*/
PLATFORMLIB_API double K2P_atof(const char *p);

#ifdef USE_K2P_CODEC
// PLATFORMLIB_API int K2P_sscanf(const char* buff, const char* format,...);

//PLATFORMLIB_API WCHAR* K2P_WStringCopy_s(d,l,s)
//PLATFORMLIB_API WCHAR* K2P_WStringCat_s(d,l,s)

/**
@brief This function maps a character (ANSI code) string to a wide-character (Unicode) string.

@param in_str [in] Pointer to the character string to be converted.
@param in_len [in] The size in bytes of the string pointed to by the in_str parameter.
@param out_wstr [out] Pointer to a buffer that receives the translated string.
@param out_wlen [in] The size, in wide characters, of the buffer pointed to by the out_wstr parameter.
@return the number of wide characters written to the buffer pointed to by out_wstr.
*/
PLATFORMLIB_API int K2P_LocalCodeToUnicode(LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen);
/**
@brief This function maps a character (UTF-8 code) string to a wide-character (Unicode) string.

@param in_str [in] Pointer to the character string to be converted.
@param in_len [in] The size in bytes of the string pointed to by the in_str parameter.
@param out_wstr [out] Pointer to a buffer that receives the translated string.
@param out_wlen [in] The size, in wide characters, of the buffer pointed to by the out_wstr parameter.
@return the number of wide characters written to the buffer pointed to by out_wstr.
*/
PLATFORMLIB_API int K2P_Utf8CodeToUnicode(LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen);

/**
@brief This function maps a character (Multibyte code) string to a wide-character (Unicode) string.

@param charset [in] Character set for input string.
@param in_str [in] Pointer to the character string to be converted.
@param in_len [in] The size in bytes of the string pointed to by the in_str parameter.
@param out_wstr [out] Pointer to a buffer that receives the translated string.
@param out_wlen [in] The size, in wide characters, of the buffer pointed to by the out_wstr parameter.
@return the number of wide characters written to the buffer pointed to by out_wstr.
*/
PLATFORMLIB_API int K2P_MultiByteCodeToUnicode(K2Charset charset, LPCSTR in_str, int in_len, WCHAR* out_wstr, int out_wlen);

/**
@brief This function maps a wide-character string to a new character (ANSI code) string.

@param in_wstr [in] Pointer to the wide-character string to be converted.
@param in_wlen [in] The number of Unicode (16-bit) characters in the string pointed to by the in_str parameter.
@param out_str [out] Pointer to a buffer that receives the translated string.
@param out_len [in] The size in bytes of the buffer pointed to by the out_str parameter.
@return Zero indicates failure.
*/
PLATFORMLIB_API int K2P_UnicodeCodeToLocal(const WCHAR* in_wstr, int in_wlen, LPSTR out_str, int out_len);
/**
@brief This function maps a wide-character string to a new character (UTF-8 code) string.

@param in_wstr [in] Pointer to the wide-character string to be converted.
@param in_wlen [in] The number of Unicode (16-bit) characters in the string pointed to by the in_str parameter.
@param out_str [out] Pointer to a buffer that receives the translated string.
@param out_len [in] The size in bytes of the buffer pointed to by the out_str parameter.
@return Zero indicates failure.
*/
PLATFORMLIB_API int K2P_UnicodeCodeToUtf8(const WCHAR* in_wstr, int in_wlen, LPSTR out_str, int out_len);

#endif // USE_K2P_CODEC


#ifdef USE_SOCKET_DOWNLOADER

typedef enum{
	K2P_Socket_NoError = 0,
	K2P_SocketError_NotReady = -100,
	K2P_SocketError_NetDown,
	K2P_SocketError_AddressInUse,
	K2P_SocketError_InProgress,
	K2P_SocketError_AddressNotAvail,
	K2P_SocketError_FamilyNotSupport,
	K2P_SocketError_ConnectRefused,
	K2P_SocketError_DestAddressRequired,
	K2P_SocketError_BadHostAddressLength,
	K2P_SocketError_InvalidHostAddress,
	K2P_SocketError_AlreadyConnected,
	K2P_SocketError_NoMoreFile,
	K2P_SocketError_NetworkCannotReach,
	K2P_SocketError_NoBuffer,
	K2P_SocketError_NoSocket,
	K2P_SocketError_TimeOut,
	K2P_SocketError_WouldBlock,
	K2P_SocketError_NotConnected,
	K2P_SocketError_ConnectionAborted,
	K2P_SocketError_ConnectionReset,
  K2P_Socket_AlreadyBound,
	K2P_Socket_UnknownError
}K2PSocketError;

enum OpenMode { ReadOnly = 0, WriteOnly = 1, ReadWrite = 2 };
enum FilePosition { begin = 0, current = 1, end = 2 };

PLATFORMLIB_API HANDLE K2P_SocketCreate(HANDLE h);
PLATFORMLIB_API K2PSocketError K2P_SocketConnect(HANDLE socket, const char* server_address, int port);

PLATFORMLIB_API K2PSocketError K2P_SocketBind(HANDLE h, SOCKADDR* sockAddr, int sockAddrLen);
PLATFORMLIB_API K2PSocketError K2P_SocketGetAddress(HANDLE h, SOCKADDR* sockAddr, int* sockAddrLen);

PLATFORMLIB_API BOOL K2P_SocketSendRequestEnable();
PLATFORMLIB_API BOOL K2P_SocketReceiveWithHeader();

PLATFORMLIB_API K2PSocketError K2P_SocketSend(HANDLE h, LPBYTE buff, int size);
PLATFORMLIB_API K2PSocketError K2P_SocketSendRequest(HANDLE h, const char* in_url);

PLATFORMLIB_API int K2P_SocketHasConnect(HANDLE h);
PLATFORMLIB_API int K2P_SocketHasReceive(HANDLE h);
PLATFORMLIB_API int K2P_SocketHasSend(HANDLE h);
PLATFORMLIB_API int K2P_SockethasClose(HANDLE h);
PLATFORMLIB_API BOOL K2P_SocketReceiveDone(HANDLE h);

PLATFORMLIB_API int K2P_SocketReceive(HANDLE h, LPBYTE buff, int size);
PLATFORMLIB_API void K2P_SocketShutDown(HANDLE h);
PLATFORMLIB_API void K2P_SocketClose(HANDLE h);
PLATFORMLIB_API void K2P_SocketDelete(HANDLE socket);
#endif // USE_SOCKET_DOWNLOADER

void KTRACE(const char *msg, ... );
void KTRACE_W(LPCTSTR msg, ... );

PLATFORMLIB_API DWORD K2P_GetSystemTimeTick();
PLATFORMLIB_API DWORD K2P_GetSystemTimeAfter(DWORD start);

/***********************************
 FILE
***********************************/
// openMode : ReadOnly = 0, WriteOnly = 1, ReadWrite = 2
/**
@brief This function opens a file.

@param openMode Type of access to the object.( ReadOnly = 0, WriteOnly = 1, ReadWrite = 2 )
@param truncate_old truncate or not truncate old file(no truncate = 0, truncate old file = 1)
@param filename Pointer to a null-terminated string that specifies the name of the object to create or open.
@return An open handle to the specified file indicates success.
*/
PLATFORMLIB_API HANDLE K2P_OpenFile(int openMode, LPCTSTR filename, int truncate_old);

/**
@brief This function reads data from a file.

@param h Handle to the file to be read.
@param lpBuff Pointer to the buffer that receives the data read from the file.
@param nsize The Number of bytes to be read from the file. 
@return the number of bytes read.
*/
PLATFORMLIB_API int K2P_ReadFile(HANDLE h, void* lpBuff, int nsize);
/**
@brief This function writes data to a file.

@param fh Handle to the file to be written to. 
@param lpBuff Pointer to the buffer containing the data to write to the file. 
@param nsize Number of bytes to write to the file.
@return the number of bytes written by this function call.
*/
PLATFORMLIB_API int K2P_WriteFile(HANDLE fh, const void* lpBuff, int nsize);

// position : begin = 0, current = 1, end = 2
/**
@brief This function moves the file pointer of an open file.

@param fh Handle to the file whose file pointer is to be moved.
@param offset the number of bytes to move the file pointer.
@param position Starting point for the file pointer move.( begin = 0, current = 1, end = 2 )
@return the new file pointer indicates success.
*/
PLATFORMLIB_API int K2P_SeekFile(HANDLE fh, int offset, int position);
/**
@brief This function retrieves the size, in bytes, of the specified file.

@param fh Open handle of the file whose size is being returned.
@return the file size indicates success.
*/
PLATFORMLIB_API int K2P_FileSize(HANDLE fh);
/**
@brief This function closes an open object handle.

@param fh Handle to an open object.
@return None.
*/
PLATFORMLIB_API void K2P_FileClose(HANDLE fh);


/**
@brief This function copies an existing file to new file.

@param oldfilename Pointer to a null-terminated string that original file.
@param newfilename Pointer to a null-terminated string that destination file.
@param bFailIfExists Flag to do not copy when destination file exists already.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_FileCopy(LPCTSTR oldfilename, LPCTSTR newfilename, BOOL bFailIfExists);

/**
@brief This function renames an existing file to new file.

@param filename Pointer to a null-terminated string that original file.
@param destination Pointer to a null-terminated string that destination file.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_FileMove(LPCTSTR oldfilename, LPCTSTR newfilename);

/**
@brief This function deletes an existing file from a file system.

@param filename Pointer to a null-terminated string that specifies the file to be deleted.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_FileRemove(LPCTSTR filename);

/**
@brief This function deletes an existing file from a file system.

@param filename Pointer to a null-terminated string that specifies the file to be deleted.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_FileRemoveW(const WCHAR* filename);

/**
@brief This function gets whether the file exists at local.

@param filename Pointer to a null-terminated string that specifies the name of a file.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_isLocalFile(LPCTSTR filename);
/**
@brief This function gets whether the file is exists.

@param filename Pointer to a null-terminated string that specifies the name of a file.
@return true indicates success.
*/
PLATFORMLIB_API BOOL K2P_FileExist(LPCTSTR filename);

PLATFORMLIB_API char K2P_GetDirectoryDelimiter();
PLATFORMLIB_API BOOL K2P_IsDirectoryDelimeter(WCHAR ch);

#ifdef __cplusplus
}
#endif

#endif // _PLATFORM_LIB_H_

