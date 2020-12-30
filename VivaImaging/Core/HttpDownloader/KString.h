#ifndef _KSTRING_UTIL_H_
#define _KSTRING_UTIL_H_

#include <vector>
//#include "KBaseClasses.h"

#ifdef USE_STL_STRING
#include <string>
typedef std::string::iterator  KStringIterator;
typedef std::string::const_iterator  KStringConstIterator;
typedef std::string::reverse_iterator KStringReverseIterator;
#endif

#if defined(UNICODE)
#define StringCopy(d,s)		K2P_WStringCopy(d,s)
#define StringCopyn(d,s,c)	K2P_WStringCopyn(d,s,c)
#define StringLength(s)		K2P_WStringLength(s)
#define StringCompare(s,d)	K2P_WStringCompare(s,d) wcscmp(s,d)

// #define StringCopy_s(d,l,s)	K2P_WStringCopy_s(d,l,s)
// #define StringCat_s(d,l,s)	K2P_WStringCat_s(d,l,s)
#else
#define StringCopy_s(d,l,s)	K2P_StringCopy(d,s)
#define StringCat_s(d,l,s)	K2P_StringCat(d,s)
#define StringCompare(s,d)	K2P_StringCompare(s,d)
#define StringLength(s)		K2P_StringLength(s)
#endif

class KString;
class KStringList;

// 0x09(tab), 0x0A(Linefeed), 0x0B(VT), 0x0C(formfeed), 0x0D(CR), 0x20(space)
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

#define DIRECTORY_DELIMETER_WIN32 '\\'
#define DIRECTORY_DELIMETER_LINUX '/'

#ifdef PLATFORM_SHP
#define DIRECTORY_DELIMETER	DIRECTORY_DELIMETER_LINUX
#elif defined(WIN32)
#define DIRECTORY_DELIMETER	DIRECTORY_DELIMETER_WIN32
#else
#define DIRECTORY_DELIMETER	DIRECTORY_DELIMETER_LINUX
#endif

class KString
{
public:
	KString();
	KString(const char* psz);
	KString(const char* psz, int len);
	KString(const unsigned char* psz);
	KString(const unsigned char* psz, int len);
	KString(int len, char ch);
	KString(const KString& str);
	~KString();

#ifdef USE_STL_STRING
	std::string	m_string;

    operator const char *() const { return m_string.data(); }

	int size() const { return (int) m_string.size(); }
	int length() const { return (int) m_string.length(); }
    const char* data() const { return m_string.data(); }
	KString& assign(const char* psz) { m_string.assign(psz); return *this; }
	KString& assign(const char* psz, int len) { m_string.assign(psz, len); return *this; }
	KString& assign(int len, char ch) { m_string.assign(len, ch); return *this; }
	KString& append(const char* psz) { m_string.append(psz); return *this; }
	KString& append(const char* psz, int len) { m_string.append(psz, len); return *this; }
	KString& append(int len, char ch) { m_string.append(len, ch); return *this; }

	void clear() { m_string.clear(); }
	void resize(int s) { m_string.resize(s); }

	KStringIterator begin() { return m_string.begin(); }
	KStringIterator end() { return m_string.end(); }
	KStringReverseIterator rbegin() { return m_string.rbegin(); }
	KStringReverseIterator rend() { return m_string.rend(); }
#else
	char* m_string;
	int m_length;

    operator const char *() const { return m_string; }

	void swap(KString& m);
	int size() const { return (int) m_length; }
	int length() const { return (int) m_length; }
    const char* data() const { return m_string; }
	KString& assign(const char* psz);
	KString& assign(const char* psz, int len);
	KString& assign(int len, char ch);
	KString& append(const char* psz);
	KString& append(const char* psz, int len);
	KString& append(int len, char ch);

	void clear();
	void resize(int s);
#endif

	char at(int i) const;
	void setAt(int i, char c);

	int find(char ch, int offset=0, bool caseSense=true) const;
	int find(LPCSTR pstr, int length=-1, int offset=0, bool cs=true) const;
	int find(const KString& expr, int offset=0, bool caseSense=true) const;

	int findOneOf(LPCSTR pstr, int length, int offset=0) const;

	int rfind(char ch, int offset=-1, bool caseSense=true) const;
	int rfind(const char* str, int length=-1, int offset=-1, bool caseSense=true) const;
	int rfind(const KString& str, int offset=-1, bool caseSense=true) const;

	int compare(const KString& str, bool caseSense=true) const;
	int compare(LPCSTR str, bool caseSense) const;
	int compare(LPCSTR str, int length, bool caseSense) const;
	int compareLength(LPCSTR str, int length, bool caseSense) const;

	KString substr(int start, int len) const;
	void clearSideWhiteSpace();
	void clearCrLf();
	void clearSideQuot();

	KString stripWhiteSpace() const;
	KString simplifyWhiteSpace() const;

	bool isSimpleAscii() const;
	bool isSimpleDigit() const;
	bool isSimpleHexa() const;
	bool isSimpleAlpha() const;

	int stripNumber(int digit);

	void makeLowerCase();
	void makeUpperCase();

	KString toLower() const;
	KString toUpper() const;

	int		toInt(bool* pok=NULL, int base=10 ) const;
  __int64 toInt64(bool* pok=NULL, int base=10 ) const;
	double	toDouble(bool* pok=NULL) const;
	float	toFloat(bool* pok=NULL) const;

	void	setNum(int n, int base=10, int minLength=-1);	// ambiguous error; modify by wooarmy
	void	setNum(double n, char f = 'g', int prec = 6);

	void replace(int pos, int len, KString& repstr);
	int replace(char ch1, char ch2);
	int replace(const KString& expr, const KString& repstr);

	// int sprintf(const char* format,...);

	const KString& operator=(const KString& stringSrc);
#ifdef USE_STL_STRING
	const KString& operator=(const std::string& string);
#endif
	const KString& operator=(const char * string);
	const KString& operator+=(const KString& string);
	const KString& operator+=(char ch);
	bool operator==(const KString& str) const;
	bool operator==(const char* pstr) const;
	bool operator!=(const KString& str) const;
	bool operator!=(const char* pstr) const;
	KString operator+(const KString& str) const;
	KString operator+(const char* pstr) const;
	KString operator+(char ch) const;
	char operator[](int pos) const;

	int findWithBrace(char find_chr, int start=0);

	void CheckValidFilename();
	void makeSequentialName();

	bool isBaseDirectory(const KStringList& pathnames);
	bool isBaseDirectory(const KString& pathname);

	bool getTokenString(KString& szDest, char delimeter, bool checkBrace = false);
	bool getTokenStringReverse(KString& szDest, char delimeter);

	static KString number_string(int val, int base=10);
	static KString number_string(double val);	// by wooarmy

	int appendAsRefString(const KString& contents);

  void AddSubPathName(const KString& name);
  void AddPathToName(const KString& name);
  void AddSubPathNameEx(const KString& name, char delimiter_chr);

  bool IsAbsoluteFilePathName() const;

	/**
	static KString fromLocal8Bit(char* pChar, int nLength=-1);
	static KString fromUtf8(char* pChar, int nLength=-1);
	static KString toUtf8(KString& txt);
	static KString fromUtf8(KString& txt);
	**/

	int appendNullTerminater();

	// added by wooarmy
#ifdef USE_STL_STRING
	KString & truncate(int len) { m_string.erase(len,length()-len); return (*this); }
	KString & remove(int index, int len) { m_string.erase(index,len); return (*this); }
#else
	KString& truncate(int len);
	KString& remove(int index, int len);
#endif

	KString left(unsigned int len) { return substr(0,len); }
	KString right(unsigned int len) { return substr(length()-len,len); }
	KString mid(unsigned int index, int len = -1) { if( len == -1 ) len = length()-index; return substr(index,len); }
	bool IsEmpty() { if(size() <= 0 || at(0) == (WCHAR)'\0') return true; return false; }
	// end of added by wooarmy
};

typedef std::vector<KString>::iterator  KStringListIterator;
typedef std::vector<KString>::const_iterator  KStringListConstIterator;

class KStringList
{
public:
	KStringList();
	KStringList(const KStringList& sl);
	~KStringList();

	std::vector<KString> m_list;

	const KStringList& operator+=(const KString& str);
	const KStringList& operator=(const KStringList& str);
	const KString& operator[](int pos) const;

	void reserve(int s) { m_list.reserve(s); }
	void clear() { m_list.clear(); }
	int size() const { return (int) m_list.size(); }
	const KString& at(int i) const { return m_list.at(i); }
	const char *getString(int index);

	KStringListIterator begin() { return m_list.begin(); }
	KStringListIterator end() { return m_list.end(); }
	KStringListConstIterator begin() const { return m_list.begin(); }
	KStringListConstIterator end() const { return m_list.end(); }

	KString join(KString& separator);
	int find(const KString& szStr, bool cs = false) const;
	bool remove(int index);
	/*
	int add(KString& szStr);
	int addHead(KString& szStr);
	*/
	void pop_back() { m_list.pop_back(); }
	// void pop_front() { m_list.pop_front(); }
	void push_back(const KString& str) { m_list.push_back(str); }
	// void push_front(const KString& str) { m_list.push_front(str); }
	void push_front(const KString& str) { KStringListIterator it; it = m_list.begin(); m_list.insert(it, str); }
	void insert(int index, const KString& str);

#ifdef USE_WIN
	KString join(KString& separator);
	KString& at(int index) { return (KString &)GetAt(FindIndex(index)); }
	void remove(int index) { RemoveAt(FindIndex(index)); }
	void set(int index, KString& str) { SetAt(FindIndex(index), str); }
#endif
	static KStringList split(WCHAR c, const KString& szStr);
};

extern int strCompare(LPCSTR src, int srcLength, LPCSTR dst, int destLength, bool caseSense);
extern int findOneOfChar(LPCSTR sstr, int slen, LPCSTR pstr, int plen, int offset);

extern void LoadProgString(UINT id, KString& szBuff);
/*
extern WCHAR* WStringCopy(WCHAR* d, const WCHAR* s);
extern WCHAR* WStringCopyn(WCHAR* d, const WCHAR* s, int n);
*/

#if defined(WIN32)
int strwlen(KString& szStr);
int strwlen(LPCSTR szStr, int len);
#endif

class KArchive;
KArchive& operator>>( KArchive& a, KString& s);
KArchive& operator<<( KArchive& a, KString& s);

#endif // _KSTRING_UTIL_H_
