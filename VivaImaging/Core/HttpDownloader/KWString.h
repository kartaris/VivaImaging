#ifndef _KWSTRING_HEADER_
#define _KWSTRING_HEADER_

// #define USE_STL_STRING

#ifdef USE_STL_STRING
#include <string>
#include <vector>
typedef std::wstring::iterator  KWStringIterator;
typedef std::wstring::const_iterator  KWStringConstIterator;
typedef std::wstring::reverse_iterator KWStringReverseIterator;
#endif

#include "KString.h"
#include "KCharset.h"

#define NUMBERING_REPEAT_NONE		0
#define NUMBERING_REPEAT_DIGIT		1
#define NUMBERING_REPEAT_COUNT		2

class KWString
{
public:
	KWString();
	KWString(const WCHAR* lpsz);
	KWString(const WCHAR* lpch, int nLength);
#ifdef USE_STL_STRING
	KWString(const std::wstring& str);
#endif
	KWString(const KWString& str);
	KWString(int nRepeat, WCHAR ch);

	KWString(const char* lpsz);
	KWString(const KString& str);
	KWString(const char* lpch, int nLength);
#ifdef QT_BASE
	KWString(const QString& str);
#endif
	~KWString();

#ifdef USE_STL_STRING
	std::wstring	m_string;

    operator const WCHAR *() const { return m_string.data(); }

	int size() const { return (int) m_string.size(); }
	int length() const { return (int) m_string.length(); }
    const WCHAR* data() const { return m_string.data(); }
	KWString& assign(const WCHAR* psz) { m_string.assign(psz); return *this; }
	KWString& assign(const WCHAR* psz, int len) { m_string.assign(psz, len); return *this; }
	KWString& assign(int len, WCHAR ch) { m_string.assign(len, ch); return *this; }
	KWString& append(const WCHAR* psz) { m_string.append(psz); return *this; }
	KWString& append(const WCHAR* psz, int len) { m_string.append(psz, len); return *this; }
	KWString& append(int len, WCHAR ch) { m_string.append(len, ch); return *this; }

	KWStringIterator begin() { return m_string.begin(); }
	KWStringIterator end() { return m_string.end(); }
	KWStringReverseIterator rbegin() { return m_string.rbegin(); }
	KWStringReverseIterator rend() { return m_string.rend(); }

	void clear() { m_string.clear(); }
	void resize(int s) { m_string.resize(s); }
	KWString & truncate(int len) { m_string.erase(len,length()-len); return (*this); }

#else
	WCHAR* m_string;
	int m_length;

  operator const WCHAR *() const { return m_string; }

	void swap(KWString& f);
	int size() const { return (int) m_length; }
	int length() const { return (int) m_length; }
  const WCHAR* data() const { return m_string; }

	KWString& assign(const WCHAR* psz);
	KWString& assign(const WCHAR* psz, int len);
	KWString& assign(int len, WCHAR ch);
	KWString& append(const WCHAR* psz);
	KWString& append(const WCHAR* psz, int len);
	KWString& append(int len, WCHAR ch);

	int appendNullTerminater();

	void clear();
	void resize(int s);
	KWString& truncate(int len) { resize(len); return (*this); }

	int compare(const KWString& str);
#endif

	void assignLocal8Bit(const char* str, int length=-1);
	void assignUtf8(const char* psz, int length=-1);
	void assignMultiByte(K2Charset charset, const char* psz, int length=-1);

	WCHAR at(int i) const;
	void setAt(int i, WCHAR c);

	int find(WCHAR ch, int offset=0, bool caseSense=false) const;
	int find(LPCWSTR pstr, int length=-1, int offset=0, bool caseSense=true) const;
	int find(const KWString& expr, int offset=0, bool caseSense=true) const;
	int findWhiteSpace(int offset=0);
	int findOneOf(WCHAR s, WCHAR e, bool caseSense=false);

	int rfind(WCHAR ch, int offset=-1, bool caseSense=true) const;
	int rfind(LPCWSTR pstr, int length=-1, int offset=-1, bool caseSense=true) const;
	int rfind(const KWString& str, int offset=-1, bool caseSense=true) const;

	void replace(int pos, int len, KWString& repstr);
	int replace(WCHAR ch1, WCHAR ch2);
	int replace(const KWString& sfindstr, const KWString& repstr);

	// int sprintf(const char* format,...);

	int insert(int pos, const KWString& str);
	int compare(const KWString& str, bool caseSense=true) const;

	int MergeCrLf();
	int RemoveCrLf();

	void makeUpperCase();
	void makeLowerCase();
	KWString toLower() const;
	KWString toUpper() const;

	void operator=(WCHAR ch);
	void operator=(LPCSTR lpsz);
	void operator=(LPWSTR lpsz);
#ifdef QT_BASE
	void operator=(const QString& str);
#endif
	KWString& operator=(const KString& str);
	KWString& operator=(const KWString& str);

	void operator+=(WCHAR ch);
	void operator+=(LPCSTR lpsz);
	void operator+=(LPWSTR lpsz);
	void operator+=(const KWString& str);

	bool operator==(const KWString& str);
	bool operator!=(const KWString& str);

	KWString operator+(const KWString& str) const;
	KWString operator+(const WCHAR* pstr) const;
	KWString operator+(WCHAR ch) const;

	int	fromUtf8(const char *psz, int len=-1);
	int	fromUtf8(KString &str);
	int	fromLocal8Bit(const char *psz, int nlen=-1);
	int	fromLocal8Bit(KString &str);

	void clearAllNoneVisible();
	void clearSideWhiteSpace();
	bool clearFirstWhiteSpace();
	bool clearLastWhiteSpace();
	void clearCrLf();
	void clearSideQuot();
	void clearCenter(int s, int length);

	KWString stripWhiteSpace() const;
	KWString simplifyWhiteSpace() const;
	// KWString stripCrLf() const;

	int toUtf8(char* mbuff, int mlen) const;
	KString toUtf8() const;

	int toLocal8Bit(char* mbuff, int mlen) const;
	KString toLocal8Bit() const;

#ifdef QT_BASE
	QString toQString() const;
#endif

	void	SetPureFilename(int maxlen=-1);
	int		ReverseFinds(KWString& str);
	int		ReverseFinds(LPCWSTR str, int slen);

#ifdef _DEBUG
	void TraceData();
#endif

	int findWithBrace(WCHAR find_chr, int start=0);

	// length & bad character check
	void CheckValidFilename();

	bool getTokenString(KWString& szDest, WCHAR delimeter, bool checkBrace = false);
	bool getTokenString(KWString& szDest, KWString& delimeterStr);
	bool getTokenStringWhiteSpace(KWString& szDest);
	bool getTokenStringReverse(KWString& szDest, WCHAR delimeter);

	int toInt(bool* pok=NULL, int base=10) const;
	float toFloat(bool* pok=NULL) const;
	void setNum(int n, int base=10);
	void setNum(double n, WCHAR f = (WCHAR)'g', int prec = 6);	// by wooarmy

	void setNumEx(int v, WCHAR start, int base, int bRepeat);
	void setNumEx(int v, WCHAR* start, int base, int bRepeat);
	void setNumRoman(int v, bool bCapital);

	KWString substr(int offset, int size) const;
	// void clear();
	void dump();

	static KWString number_string(int val, int base=10);
	static KWString number_string(double val);	// by wooarmy

	bool IsEmpty() { if(size() <= 0 || at(0) == (WCHAR)'\0') return true; return false; }
};

typedef std::vector<KWString>::iterator  KWStringListIterator;
typedef std::vector<KWString>::const_iterator  KWStringListConstIterator;

class KWStringList
{
public:

	KWStringList();
	KWStringList(const KWStringList& sl);
	virtual ~KWStringList();

	std::vector<KWString> m_list;

	const KWStringList& operator=(const KWStringList& str);
	const KWStringList& operator+=(const KWString& str);
	const KWString& operator[](int pos) const;

	void clear() { m_list.clear(); }
	int size() const { return (int) m_list.size(); }
	const KWString& at(int i) const { return m_list.at(i); }

	const KWString& back() const { return m_list.back(); }

	KWStringListIterator begin() { return m_list.begin(); }
	KWStringListIterator end() { return m_list.end(); }
	KWStringListConstIterator begin() const { return m_list.begin(); }
	KWStringListConstIterator end() const { return m_list.end(); }

	KWString join(KWString& separator);
	int find(const KWString& szStr, bool cs = false) const;
	bool remove(int index);

	static KWStringList split(WCHAR c, const KWString& szStr);

	/*
	int add(KWString& szStr);
	int addHead(KWString& szStr);
	*/
	void pop_back() { m_list.pop_back(); }
	// void pop_front() { m_list.pop_front(); }
	void push_back(const KWString& str) { m_list.push_back(str); }
	// void push_front(const KWString& str) { m_list.push_front(str); }
	void push_front(const KWString& str) { KWStringListIterator it; it = m_list.begin(); m_list.insert(it, str); }	// by wooarmy
	void insert(int index, const KWString& str);

	// CObList ¸É¹ö
	int GetCount() {return (int) size();}
};

extern int wstrCompare(LPCWSTR src, LPCWSTR dst, int length=-1, bool caseSense=true);
extern int wstrCompare(LPCWSTR src, int slen, LPCWSTR dst, int dstlen, bool caseSense=true);

extern int wstrwcpy(LPWSTR dBuff, LPCWSTR pBuff);
extern int wstrwncpy(LPWSTR dBuff, LPCWSTR pBuff, int len);
extern int strwcpy(KString& szNote, LPCWSTR pBuff);
extern int wstrcpy(LPWSTR pBuff, KString& szNote);
extern int wstrncpy(LPWSTR pBuff, KString& szNote, int nMax);
extern int strwncpy(KString& szNote, LPCWSTR pBuff, int len);

extern bool isSpaceKChar(WCHAR ch);
extern bool isAsciiKChar(WCHAR ch);
extern bool isBreakableKChar(WCHAR ch);

#ifdef _USE_K_ARCHIVE
class KArchive;
KArchive& operator>>( KArchive& a, KWString& s);
KArchive& operator<<( KArchive& a, KWString& s);
#endif

#endif // _KWSTRING_HEADER_
