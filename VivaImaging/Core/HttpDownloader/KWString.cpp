#include "stdafx.h"
#include "KWString.h"
#include "KString.h"
#ifdef _USE_K_ARCHIVE
  #include "KArchive.h"
#endif
#include "KMemory.h"
#include "PlatformLib.h"


#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char	THIS_FILE[]=__FILE__;
#define	new	DEBUG_NEW
#endif 

/*
KString : 16-bit UNICODEi string 표현
*/

KWString::KWString()
{
	m_string = NULL;
	m_length = 0;
}

KWString::~KWString()
{
	clear();
}

KWString::KWString(int nRepeat, WCHAR ch)
{
	m_string = NULL;
	m_length = 0;
	assign(nRepeat, ch);
}

KWString::KWString(const WCHAR* lpsz)
{
	m_string = NULL;
	m_length = 0;
	assign((WCHAR*) lpsz);
}

KWString::KWString(const WCHAR* lpsz, int length)
{
	m_string = NULL;
	m_length = 0;
	assign((WCHAR*) lpsz, length);
}

#ifdef USE_STL_STRING
KWString::KWString(const std::wstring& str)
{
	m_string = NULL;
	m_length = 0;
	assign(str.data(), str.size());
}
#endif

KWString::KWString(const KWString& str)
{
	m_string = NULL;
	m_length = 0;
	assign(str.data(), str.size());
}

KWString::KWString(const char* lpsz)
{
	m_string = NULL;
	m_length = 0;
	if(lpsz)
		assignLocal8Bit(lpsz);
}

KWString::KWString(const KString& str)
{
	m_string = NULL;
	m_length = 0;
	if(str.size() > 0)
		assignLocal8Bit(str.data());
}

KWString::KWString(const char* psz, int length)
{
	m_string = NULL;
	m_length = 0;
	assignLocal8Bit(psz, length);
}

#ifdef QT_BASE
KWString::KWString(const QString& str)
{
	assign((WCHAR *)str.utf16(), str.size());
}
#endif


#ifndef USE_STL_STRING

void KWString::swap(KWString& m)
{
	WCHAR* s;
	int l;

	s = m_string;
	m_string = m.m_string;
	m.m_string = s;

	l = m_length;
	m_length = m.m_length;
	m.m_length = l;
}

KWString& KWString::assign(const WCHAR* psz)
{
	clear();
	if(psz != NULL)
		return assign(psz, K2P_WStringLength(psz));
	return *this;
}

KWString& KWString::assign(const WCHAR* psz, int len)
{
	clear();
	if((psz != NULL) && (len > 0))
	{
		m_string = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (len + 1));
		if(m_string != NULL)
		{
			HCMemcpy(m_string, psz, sizeof(WCHAR) * len);
			m_string[len] = '\0';
			m_length = len;
		}
	}
	return *this;
}

KWString& KWString::assign(int len, WCHAR ch)
{
	clear();
	if(len > 0)
	{
		m_string = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (len + 1));
		if(m_string != NULL)
		{
			int i;
			for(i = 0; i < len; i++)
				m_string[i] = ch;
			m_string[len] = '\0';
			m_length = len;
		}
	}
	return *this;
}

KWString& KWString::append(const WCHAR* psz)
{
	return append(psz, K2P_WStringLength(psz));
}

KWString& KWString::append(const WCHAR* psz, int len)
{
	if((psz != NULL) && (len > 0))
	{
		/*
		WCHAR* buff;
		buff = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (m_length + len + 1));
		if(buff != NULL)
		{
			if(m_string != NULL)
				HCMemcpy(buff, m_string, sizeof(WCHAR) * m_length);
			HCMemcpy(&(buff[m_length]), psz, sizeof(WCHAR) * len);
			if(m_string != NULL)
				HCMemFree(m_string);

			m_string = buff;
			m_length += len;
			m_string[m_length] = '\0';
		}
		*/
		m_string = (WCHAR *)HCMemRealloc(m_string, sizeof(WCHAR) * (m_length + len + 1));
		if(m_string != NULL)
		{
			HCMemcpy(&(m_string[m_length]), psz, sizeof(WCHAR) * len);
			m_length += len;
			m_string[m_length] = '\0';
		}
	}
	return *this;
}

KWString& KWString::append(int len, WCHAR ch)
{
	if(len > 0)
	{
		int i;

		/*
		WCHAR* buff;
		buff = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (m_length + len + 1));
		if(buff != NULL)
		{
			if(m_string != NULL)
				HCMemcpy(buff, m_string, sizeof(WCHAR) * m_length);
			for(i = 0; i < len; i++)
				buff[i+m_length] = ch;
			if(m_string != NULL)
				HCMemFree(m_string);

			m_string = buff;
			m_length += len;
			m_string[m_length] = '\0';
		}
		*/
		m_string = (WCHAR *)HCMemRealloc(m_string, sizeof(WCHAR) * (m_length + len + 1));
		if(m_string != NULL)
		{
			for(i = 0; i < len; i++)
				m_string[i+m_length] = ch;
			m_length += len;
			m_string[m_length] = '\0';
		}
	}
	return *this;
}

int KWString::appendNullTerminater()
{
	append(1, 0);
	return length();
}

void KWString::clear()
{
	if(m_string != NULL)
	{
		HCMemFree(m_string);
		m_string = NULL;
	}
	m_length = 0;
}

void KWString::resize(int s)
{
	if(m_length != s)
	{
		if(s == 0)
		{
			clear();
		}
		else
		{
			int len;

			len = s;
			/*
			WCHAR* buff;
			int len;

			len = s;
			buff = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (len + 1));
			if(buff != NULL)
			{
				HCMemset(buff, 0, sizeof(WCHAR) * len);
				if(len > m_length)
					len = m_length;
				if(m_string != NULL)
				{
					HCMemcpy(buff, m_string, sizeof(WCHAR) * len);
					HCMemFree(m_string);
				}
				buff[s] = '\0';
				m_string = buff;
				m_length = s;
			}
			*/
			if(len > m_length)
				m_string = (WCHAR *)HCMemRealloc(m_string, sizeof(WCHAR) * (len + 1));
			if(m_string != NULL)
			{
				if(len > m_length)
					HCMemset(&(m_string[m_length]), 0, sizeof(WCHAR) * (len - m_length));
				m_string[s] = '\0';
				m_length = s;
			}
		}
	}
}

int KWString::compare(const KWString& str)
{
	if(str.length() == 0)
		return((m_length == 0) ? 0 : -1);
	/*
	if((data() == NULL) && (str.data() == NULL))
		return 0;
	if((data() == NULL)
		return -1;
	if(str.data() == NULL)
		return 1;
		*/

	return wstrCompare(data(), length(), str.data(), str.length(), true);
}

#endif // not USE_STL_STRING

void KWString::assignLocal8Bit(const char* psz, int length)
{
	if(length < 0)
	{
		if(psz != NULL)
			length = (int)K2P_StringLength(psz);
		else
			length = 0;
	}
	if(length > 0)
	{
		int wlen = K2P_LocalCodeToUnicode(psz, length, NULL, 0);
		assign(wlen, ' ');
		K2P_LocalCodeToUnicode(psz, length, (LPWSTR) data(), wlen);
	}
	else
		clear();
}

void KWString::assignUtf8(const char* psz, int length)
{
	if(length < 0)
	{
		if(psz != NULL)
			length = (int)K2P_StringLength(psz);
		else
			length = 0;
	}
	if(length > 0)
	{
		int wlen = K2P_Utf8CodeToUnicode(psz, length, NULL, 0);
		assign(wlen, ' ');
		K2P_Utf8CodeToUnicode(psz, length, (LPWSTR) data(), wlen);
	}
	else
		clear();
}

void KWString::assignMultiByte(K2Charset charset, const char* psz, int length)
{
	if(length < 0)
	{
		if(psz != NULL)
			length = (int)K2P_StringLength(psz);
		else
			length = 0;
	}
	if(length > 0)
	{
		int wlen = K2P_MultiByteCodeToUnicode(charset, psz, length, NULL, 0);
		assign(wlen, ' ');
		K2P_MultiByteCodeToUnicode(charset, psz, length, (LPWSTR) data(), wlen);
	}
	else
		clear();
}

void KWString::operator=(WCHAR ch)
{
	clear();
	assign(1, ch);
}

void KWString::operator=(LPCSTR lpsz)
{
	assignLocal8Bit(lpsz);
}

void KWString::operator=(LPWSTR psz)
{
	assign((WCHAR *)psz);
}

#ifdef QT_BASE
void KWString::operator=(const QString& str)
{
	assign((WCHAR *)str.utf16(), str.size());
	// *this = str.toStdWString();
}
#endif

KWString& KWString::operator=(const KString& str)
{
	assignLocal8Bit(str.data(), str.size());
	return *this;
}

KWString& KWString::operator=(const KWString& str)
{
	assign(str.data(), str.size());
	return *this;
}

void KWString::operator+=(WCHAR ch)
{
	append(1, ch);
}

void KWString::operator+=(LPCSTR psz)
{
	KWString str(psz);
	append(str.data(), str.size());
}

void KWString::operator+=(LPWSTR psz)
{
	append((WCHAR *)psz);
}

void KWString::operator+=(const KWString& str)
{
	append(str.data(), str.size());
}

bool KWString::operator==(const KWString& str)
{
	if(str.length() == 0)
		return(m_length == 0);
	if(m_length == 0)
		return false;
	return(compare(str) == 0);
}

bool KWString::operator!=(const KWString& str)
{
	if(str.length() == 0)
		return(m_length != 0);
	if(m_length == 0)
		return true;
	return(compare(str) != 0);
}

KWString KWString::operator+(const KWString &str) const
{
	KWString tmp(*this);
	tmp += str;
	return tmp;
}
KWString KWString::operator+(const WCHAR* pstr) const
{
	KWString tmp(*this);
	tmp += pstr;
	return tmp;
}
KWString KWString::operator+(WCHAR ch) const
{
	KWString tmp(*this);
	tmp += ch;
	return tmp;
}

int wstrCompare(LPCWSTR src, LPCWSTR dst, int length, bool caseSense)
{
	return wstrCompare(src, -1, dst, length, caseSense);
}

int wstrCompare(LPCWSTR src, int srclen, LPCWSTR dst, int dstlen, bool caseSense)
{
	int compare_length;

	if(srclen < 0)
		srclen = K2P_WStringLength((LPWSTR)src);
	if(dstlen < 0)
		dstlen = K2P_WStringLength((LPWSTR)dst);
	compare_length = srclen;
	if(compare_length > dstlen)
		compare_length = dstlen;

	if(!caseSense)
	{
		WCHAR schar, dchar;
		while(compare_length-- > 0)
		{
			schar = isAlpha(*src) ? toUpperChar(*src) : *src;
			dchar = isAlpha(*dst) ? toUpperChar(*dst) : *dst;
			if(schar != dchar)
				return((schar > dchar) ? 1 : -1);
			src++;
			dst++;
		};

		if(srclen > dstlen)
			return 1;
		else if(srclen < dstlen)
			return -1;
	}
	else
	{
		int cmp = HCMemcmp(src, dst, sizeof(WCHAR) * compare_length);
		if(cmp == 0)
		{
			if(srclen > dstlen)
				cmp = 1;
			else if(srclen < dstlen)
				cmp = -1;
		}
		return cmp;
	}
	return 0;
}

WCHAR KWString::at(int i) const
{
	if((i >= 0) && (i < size()))
#ifdef USE_STL_STRING
		return m_string.at(i);
#else
		return m_string[i];
#endif
	return 0;
}

void KWString::setAt(int i, WCHAR c)
{
	if((i >= 0) && (i < size()))
	{
#ifdef USE_STL_STRING
		WCHAR *p = (WCHAR *)data();
		p[i] = c;
#else
		m_string[i] = c;
#endif
	}
}

int KWString::find(WCHAR ch, int offset, bool caseSense) const
{
	int p;

	if(caseSense)
	{
#ifdef USE_STL_STRING
		p = m_string.find(ch, offset);
		if(p == std::wstring::npos)
			return -1;
		return(p);
#else
		LPCWSTR pst = data();
		int length = size();

		p = 0;
		if(offset > 0)
		{
			p = offset;
			pst += offset;
		}
		while(p < length)
		{
			if(*pst == ch)
				return p;
			pst++;
			p++;
		};
		return -1;
#endif
	}
	else
	{
		LPCWSTR pst = data();
		int length = size();

		p = 0;
		ch = toUpperChar(ch);
		if(offset > 0)
		{
			p = offset;
			pst += offset;
		}
		while(p < length)
		{
			if(toUpperChar(*pst) == ch)
				return p;
			pst++;
			p++;
		};
		return -1;
	}
}

int KWString::find(LPCWSTR pstr, int length, int offset, bool caseSense) const
{
	WCHAR* pSrc;

	if(length < 0)
		length = K2P_WStringLength((LPWSTR)pstr);
	int slen = (int) size() - length - offset;
	if(slen >= 0)
	{
		pSrc = (WCHAR*) data();
		for(int i=0 ; i<=slen ; i++)
		{
			if(wstrCompare(&(pSrc[offset+i]), length, (LPWSTR)pstr, length, caseSense) == 0)
				return(offset+(int)i);
		}
	}
	return -1;
}

int KWString::find(const KWString& expr, int offset, bool caseSense) const
{
	return find(expr.data(), expr.size(), offset, caseSense);
}

int KWString::findWhiteSpace(int offset)
{
	int p;
	LPCWSTR pst = data();
	int length = size();

	p = 0;
	if(offset > 0)
	{
		p = offset;
		pst += offset;
	}
	while(p < length)
	{
		if(isWhiteSpace(*pst))
			return p;
		pst++;
		p++;
	};
	return -1;
}

int KWString::findOneOf(WCHAR s, WCHAR e, bool caseSense)
{
	LPCWSTR pst = data();
	int length = size();
	WCHAR ch;
	int p;

	p = 0;
	if(!caseSense)
	{
		s = toUpperChar(s);
		e = toUpperChar(e);
	}

	while(p < length)
	{
		ch = *pst;
		if(!caseSense)
			ch = toUpperChar(ch);
		if((s <= ch) && (e >= ch))
			return p;
		pst++;
		p++;
	};
	return -1;
}

int KWString::rfind(WCHAR ch, int offset, bool caseSense) const
{
	int p;

	if(caseSense)
	{
#ifdef USE_STL_STRING
		p = m_string.rfind(ch, offset);
		if(p != std::wstring::npos)
			return((int) p);
		return -1;
#else
		int length = size();
		LPCWSTR pstr = data();
		if(offset > 0)
		{
			pstr += offset;
			length = offset;
			p = offset;
		}
		else
		{
			pstr += length - 1;
			p = length - 1;
		}

		while(length-- > 0)
		{
			if(*pstr == ch)
				return p;
			p--;
			pstr--;
		};
		return -1;
#endif
	}
	else
	{
		int length = size();
		LPCWSTR pstr = data();
		if(offset > 0)
		{
			pstr += offset;
			length = offset;
			p = offset;
		}
		else
		{
			pstr += length - 1;
			p = length - 1;
		}

		ch = toUpperChar(ch);
		while(length-- > 0)
		{
			if(toUpperChar(*pstr) == ch)
				return p;
			p--;
			pstr--;
		};
		return -1;
	}
}

int KWString::rfind(LPCWSTR pstr, int length, int offset, bool caseSense) const
{
	WCHAR* pSrc;

	if(length < 0)
		length = K2P_WStringLength((LPWSTR)pstr);
	pSrc = (WCHAR*) data();
	if((offset <= 0) || (offset > (int)(size()-length)))
		offset = size()-length;

	for(int i=offset ; i>=0 ; i--)
	{
		if(wstrCompare(&(pSrc[i]), length, pstr, length, caseSense) == 0)
			return(i);
	}
	return -1;
}

int KWString::rfind(const KWString& str, int offset, bool caseSense) const
{
	return rfind(str.data(), str.size(), offset, caseSense);
}

int KWString::compare(const KWString& expr, bool caseSense) const
{
	if(expr.length() == 0)
		return((m_length == 0) ? 0 : -1);
	return wstrCompare(data(), size(), expr.data(), expr.size(), caseSense);
}

void KWString::replace(int pos, int len, KWString& repstr)
{
#ifdef USE_STL_STRING
	*this = m_string.replace(pos, len, repstr.data());
#else
	int nlen, npos;
	WCHAR* buff;

	nlen = m_length - len + repstr.length();
	buff = (WCHAR *)HCMemAlloc((nlen + 1) * sizeof(WCHAR));
	if(buff != NULL)
	{
		HCMemcpy(buff, m_string, pos * sizeof(WCHAR));
		HCMemcpy(&(buff[pos]), repstr.data(), repstr.length() * sizeof(WCHAR));
		npos = pos + repstr.length();
		if(m_length > (pos+len))
		{
			HCMemcpy(&(buff[npos]), &(m_string[pos+len]), (m_length - pos - len) * sizeof(WCHAR));
			npos += (m_length - pos - len);
		}
		buff[npos] = '\0';
		HCMemFree(m_string);
		m_string = buff;
		m_length = npos;
	}
#endif
}

int KWString::replace(WCHAR ch1, WCHAR ch2)
{
	int iReplace=0;
	WCHAR* buff = (WCHAR*)data();

	for(int i=0 ; i<size() ; i++)
	{
		if(buff[i] == ch1)
		{
			buff[i] = ch2;
			iReplace++;
		}
	}
	return (iReplace);
}

 int KWString::replace(const KWString& findstr, const KWString& repstr)
{
	int search_pos;
	int i;
	KWString out;
	int count = 0;

	search_pos = 0;
	while((i = find(findstr, search_pos, true)) >= 0)
	{
		if(i > (int)search_pos)
			out += substr(search_pos, i - search_pos);
		out += repstr;
		count++;
		search_pos = i + findstr.size(); 
	};
	if(search_pos < (int)size())
		out += substr(search_pos, size() - search_pos);
	*this = out;
	return count;
}

#ifdef USE_STRING_SPRINTF
int KWString::sprintf(const char* format,...)
{
	KString str;

	str.sprintf(format);
	assignLocal8Bit(str.data(), str.length());
	return (int) size();
}
#endif

int KWString::insert(int pos, const KWString& str)
{
#ifdef USE_STL_STRING
	m_string.insert(pos, (WCHAR *) str.data(), str.size());
	return (int)m_string.size();
#else
	if(str.length() > 0)
	{
		WCHAR* buff;
		int len;

		len = m_length + str.length();
		buff = (WCHAR *)HCMemAlloc(sizeof(WCHAR) * (len + 1));
		if(buff != NULL)
		{
			if(pos > (int)m_length)
				pos = m_length;
			if(pos < 0)
				pos = 0;

			HCMemset(buff, 0, sizeof(WCHAR) * len);
			if(m_string != NULL)
			{
				HCMemcpy(buff, m_string, sizeof(WCHAR) * pos);
				if(pos < (int)m_length)
					HCMemcpy(&(buff[pos+str.length()]), &(m_string[pos]), sizeof(WCHAR) * (m_length - pos));
				HCMemFree(m_string);
			}
			m_string = buff;
			HCMemcpy(&(m_string[pos]), str.data(), sizeof(WCHAR) * str.length());
			m_length += str.length();
			m_string[m_length] = '\0';
		}
	}
	
	return m_length;
#endif
}

int KWString::MergeCrLf()
{
	// '\r'+'\n' 를 하나의 '\n'로 치환.
	KWString t = *this;

	for(int i=0 ; i<= (int)t.size() ; i++) // 마지막 NULL 문자도 복사해야 하므로 <= 를 사용
	{
		if(t[i] == '\r')
		{
			if( (i < ((int)t.size()-1)) && (t[i+1] == '\n') )
				i++;
			*this += '\n';
		}
		else
			*this += t[i];
	}
	return (int)size();
}

int KWString::RemoveCrLf()
{
	int len = size();
	for(int i=size()-1 ; i>=0 ; i--)
	{
		if( (at(i) == '\r') || (at(i) == '\n') )
			continue;
		len = i + 1;
		*this = substr(0, len);
		break;
	}
	return (int)len;
}

/***
int KWString::ReverseFind(WCHAR ch) const
{
	for(int i=size()-1 ; i>=0 ; i--)
	{
		if(at(i) == ch)
			return i;
	}
	return -1;
}

int KWString::Find(LPCSTR lpsz, int nstart) const
{
	KWString str(lpsz);
	return Find(str, nstart);
}

int KWString::Find(LPWSTR lpsz, int nstart) const
{
	KWString str(lpsz);
	return Find(str, nstart);
}

int KWString::Find(const KWString& str, int nstart) const
{
	if(str.GetLength() > 0)
	{
		int pos = nstart;
		KWString substr;

		KWString	szWTest = *this;
		KWString	szWTest2 = str;
#ifdef _DEBUG
		KString	szFileTest, szAllTest, szTest;
		szAllTest = szWTest.toLocal8Bit();
		szFileTest = szWTest2.toLocal8Bit();
#endif
		while( (pos = Find(str.at(0), pos)) >= 0 )
		{
			if(pos + str.GetLength() > len)
				break;

			substr = KWString(&(buff[pos]), str.GetLength());
#ifdef _DEBUG
			substr.toLocal8Bit(szTest);
#endif
			if( str.Compare(substr) == 0 )
				return pos;
			pos++;
		}
	}
	return -1;
}

KWString KWString::Mid(int start, int nLen)
{
	KWString str;
	if(start >= len)
		return str;

	if(nLen > len)
		nLen = len;
	if((start+nLen) > len)
		nLen = len - start;

	if(nLen > 0)
	{
		LPWSTR d = str.GetBufferSetLength(nLen);
		HCMemcpy(d, buff+start, sizeof(WCHAR) * nLen);
	}
	return str;
}

KWString KWString::Left(int slen)
{
	KWString str;

	LPWSTR d = str.GetBufferSetLength(slen);
	if(slen > len)
		slen = len;
	HCMemcpy(d, buff, sizeof(WCHAR) * slen);
	return str;
}

KWString KWString::Right(int slen)
{
	KWString str;
	LPWSTR d = str.GetBufferSetLength(slen);
	int start = 0;

	if(slen > len)
		slen = len;
	if(slen < len)
		start = len - slen;
	if(slen > 0)
		HCMemcpy(d, &(buff[start]), sizeof(WCHAR) * slen);
	return str;
}

int KWString::Compare(LPCSTR lpsz) const
{
	KWString str(lpsz);
	return Compare(str);
}

int KWString::Compare(LPCWSTR lpsz) const
{
	KWString str(lpsz);
	return Compare(str);
}

bool KWString::operator==(KWString& str)
{
	return ( Compare(str) == 0);
}

bool KWString::operator!=(KWString& str)
{
	return ( Compare(str) != 0);
}

int KWString::Compare(KWString& sz) const
{
	WCHAR	c1, c2;

	for(int i=0 ; i<len ; i++)
	{
		c1 = at(i);
		c2 = sz.at(i);
		if(c1 < c2)
			return -1;
		else if(c1 > c2)
			return 1;
	}

	if(len == sz.GetLength())
		return 0;
	return ((len > sz.GetLength()) ? 1 : -1);
}

// NULL-Character인 곳에서 스트링 길이 재 조정.
int KWString::CheckValidString()
{
	for(int i=0 ; i<len ; i++)
	{
		if(buff[i] == 0)
		{
			len = i;
			break;
		}
	}
	return len;
}
********/

int KWString::fromUtf8(const char *psz, int length)
{
	assignUtf8(psz, length);
	return size();
}

int	KWString::fromUtf8(KString &str)
{
	return fromUtf8(str.data(), str.size());
}

int KWString::fromLocal8Bit(const char *psz, int length)
{
	assignLocal8Bit(psz, length);
	return (int) size();
}

int KWString::fromLocal8Bit(KString &str)
{
	return fromLocal8Bit(str.data(), str.size());
}

void KWString::clearAllNoneVisible()
{
	int i;
	LPWSTR s;

	if(m_length == 0)
		return;
	s = (LPWSTR) data();
	for(i=0 ; i<m_length ; i++)
	{
		if(*s < 0x20)
			*s = ' ';
		s++;
	}
}

void KWString::clearSideWhiteSpace()
{
	clearFirstWhiteSpace();
	clearLastWhiteSpace();
	/*
	int i, len;
	LPWSTR d, s, e;

	if(m_length == 0)
		return;
	d = s = (LPWSTR) data();
	for(i=0 ; i<m_length ; i++)
	{
		if( isWhiteSpace(*s) ) // space
			s++;
		else
			break;
	}

	if(i >= m_length)
		clear();
	else
	{
		e = d + m_length - 1;
		for(i=0 ; i<m_length ; i++)
		{
			if( isWhiteSpace(*e) )
				e--;
			else
				break;
		}
		len = (int)(e - s + 1);
		if(d < s)
		{
			while(s <= e)
			{
				*d++ = *s++;
			}
			*d = 0;
		}
		resize(len);
	}
	**/
}

bool KWString::clearFirstWhiteSpace()
{
	int i, len;
	LPWSTR d, s;

	if(m_length == 0)
		return false;
	d = s = (LPWSTR) data();
	for(i=0 ; i<m_length ; i++)
	{
		if( isWhiteSpace(*s) ) // space
			s++;
		else
			break;
	}
	if(d != s)
	{
		len = m_length - (int)(s - d);
		if(len > 0)
			*this = substr((int)(s - d), len);
		else
			clear();
		return true;
	}
	return false;
}

bool KWString::clearLastWhiteSpace()
{
	int i, len;
	LPWSTR e;

	if(m_length == 0)
		return false;
	e = (LPWSTR) data() + m_length - 1;
	for(i=0 ; i<m_length ; i++)
	{
		if( isWhiteSpace(*e) )
			e--;
		else
			break;
	}
	len = (int)(e - data() + 1);
	if(len == m_length)
		return false;
	resize(len);
	return true;
}

void KWString::clearCrLf()
{
	LPWSTR d, s;
	bool bOccured;
	int i;
	
	d = s = (LPWSTR) data();
	bOccured = false;
	for(i=0 ; i<size() ; i++)
	{
		if( !isCrLf(*s) ) // space
			*d++ = *s++;
		else
		{
			s++;
			bOccured = true;
		}
	}

	if(bOccured)
	{
		*d = 0;
		resize((int)(d - data()));
	}
}

void KWString::clearSideQuot()
{
	if(m_length > 2)
	{
		if( ((at(0) == '"') && (at(m_length-1) == '"')) ||
			((at(0) == '\'') && (at(m_length-1) == '\'')) )
			*this = substr(1, m_length - 2);
	}
}

void KWString::clearCenter(int pos, int length)
{
	if(pos+length >= m_length)
		resize(pos - 1);
	else
	{
		LPWSTR d, s;
		int l;
		
		d = (LPWSTR) data() + pos;
		s = (LPWSTR) data() + pos + length;
		l = m_length - (pos + length);
		HCMemcpy(d, s, sizeof(WCHAR) * l);
		resize(pos + l);
	}
}

KWString KWString::stripWhiteSpace() const
{
	KWString wstr(*this);
	wstr.clearSideWhiteSpace();
	return wstr;
}

KWString KWString::simplifyWhiteSpace() const
{
	int i;
	KWString str(*this);
#ifdef USE_STL_STRING
	KWStringIterator ptr = str.begin();

	for(i=0 ; i<size() ; i++)
	{
		if( isWhiteSpace( *ptr ) )
			*ptr = (WCHAR) ' ';
		ptr++;
	}
#else
	char* ptr = (char *)str.data();

	for(i=0 ; i<size() ; i++)
	{
		if( isWhiteSpace( *ptr ) )
			*ptr = (WCHAR) ' ';
		ptr++;
	}
#endif
	return str;
}

#ifdef QT_BASE
QString KWString::toQString() const
{
	QString str;

	str = QString::fromUtf16((const ushort *)data(), size());
	return str;
}
#endif

int KWString::toUtf8(char* mbuff, int mlen) const
{
	return K2P_UnicodeCodeToUtf8((LPCWSTR) data(), size(), mbuff, mlen);
}

KString KWString::toUtf8() const
{
	KString str;
	int mlen;

	if(size() > 0)
	{
		mlen = K2P_UnicodeCodeToUtf8(data(), size(), NULL, 0);
		str.assign(mlen, ' ');
		K2P_UnicodeCodeToUtf8(data(), size(), (LPSTR) str.data(), mlen);
	}
	return str;
}

int KWString::toLocal8Bit(char* buff, int len) const
{
	return K2P_UnicodeCodeToLocal(data(), size(), buff, len);
}

KString KWString::toLocal8Bit() const
{
	KString str;
	int mlen;

	if(size() > 0)
	{
		mlen = K2P_UnicodeCodeToLocal(data(), size(), NULL, 0);
		str = KString(mlen, ' ');
		K2P_UnicodeCodeToLocal(data(), size(), (LPSTR) str.data(), mlen);
	}
	return str;
}

/*
KString KWString::toString()
{
	KString str;
	int i, mlen;

	mlen = 0;
	for(i=0 ; i<len ; i++)
		mlen += (buff[i] > 0xFF) ? 2 : 1;

	LPSTR p = str.GetBufferSetLength(mlen);
	for(i=0 ; i<len ; i++)
	{
		if(buff[i] > 0xFF)
			*(p++) = (BYTE) ((buff[i] >> 8) & 0xFF);
		*(p++) = (BYTE) (buff[i] & 0xFF);
	}
	return str;
}

KWString::KWString(const unsigned char* lpsz)
{
int slen = lstrlen(lpsz);
len = strwlen(lpsz, slen);
ReadyBuffer(len);

len = MultiByteToWideChar(CP_ACP, 0, lpsz, slen, buff, len+1);
buff[len] = 0;
}
*/

void KWString::makeUpperCase()
{
	WCHAR *wchar = (WCHAR *)data();

	for(int i=0 ; i<size() ; i++)
	{
		if(*wchar >= 'a'  && *wchar <= 'z')
			*wchar -= 32;
		wchar++;
	}
}

void KWString::makeLowerCase()
{
	WCHAR *wchar = (WCHAR *)data();

	for(int i=0 ; i<size() ; i++)
	{
		if(*wchar >= 'A'  && *wchar <= 'Z')
			*wchar += 32;
		wchar++;
	}
}

KWString KWString::toUpper() const
{
	KWString szUpper(*this);

	szUpper.makeUpperCase();
	return szUpper;
}

KWString KWString::toLower() const
{
	KWString szLower;

	szLower.makeLowerCase();
	return szLower;
}

void KWString::SetPureFilename(int maxlen)
{
	int		nLen, ofs, ofsx;

	nLen = (int)size();
	ofs = rfind(DIRECTORY_DELIMETER_WIN32);
	ofsx = rfind(DIRECTORY_DELIMETER_LINUX);
	if(ofs < ofsx)
		ofs = ofsx;
	if(ofs >= 0)
		*this = substr(ofs+1, (nLen - ofs - 1));

	if( (maxlen > 0) && ((int) size() > maxlen) )
		*this = substr(0, maxlen);
}

int KWString::ReverseFinds(KWString& str)
{
	return ReverseFinds(str.data(), str.size());
}

int KWString::ReverseFinds(LPCWSTR str, int slen)
{
	int len = (int)size();
	LPCWSTR pbuff = data();

	if(slen < 0)
		slen = K2P_WStringLength((LPWSTR) str);
	if(slen > 0)
	{
		for(int i=len-slen ; i>=0 ; i--)
		{
			if(HCMemcmp(&(pbuff[i]), str, sizeof(WCHAR) * slen) == 0)
				return i;
		}
	}
	return -1;
}

#if defined(_DEBUG) && defined(WINCE)

void KWString::TraceData()
{
	int len = (int)size();
	int pos = 0;
	KWString sstr;
	KString str;

	while(pos < len)
	{
		sstr = substr(pos, 100);
		str = sstr.toLocal8Bit();
		// KTRACE(str.data());
		pos += (int)sstr.size();
	};
}

#endif // _DEBUG

int KWString::toInt(bool* pok, int base) const
{
	if(pok != NULL)
		*pok = true;

	int s, value = 0;
	for(int i=0 ; i<(int)size() ; i++)
	{
		s = (int) at(i);
		if((s >= '0') && (s <= '9'))
			s -= '0';
		else if((s >= 'A') && (s <= 'F'))
			s -= 'A' - 10;
		else if((s >= 'a') && (s <= 'f'))
			s -= 'a' - 10;
		else
			break;
		if(s >= base)
			break;
		value = value * base + s;
	}
	return value;
}

float KWString::toFloat(bool* pok) const
{
	int s;
	float value = 0;
	bool is_float = false;
	int div = 10;

	for(int i=0 ; i<(int)size() ; i++)
	{
		s = (int) at(i);
		if((s >= '0') && (s <= '9'))
			s -= '0';
		else if(s == '.')
		{
			is_float = true;
			continue;
		}
		else
			break;
		if(is_float)
		{
			value += (float) s / div;
			div = div * 10;
		}
		else
			value = value * 10 + s;
	}
	return value;
}

void KWString::setNum(int n, int base)
{
#ifdef _DEBUG
	if (base < 2 || base > 16 )
	{
		// KTRACE( "KWString::setNum: Invalid base %d", base );
		base = 10;
	}
#endif
	WCHAR   charbuf[65];
	LPWSTR p = &charbuf[64];

	int  len = 0;
	bool neg = false;
	int	digit;
	char chr = 0;

	if ( n < 0 )
	{
		n = -n;
		neg = true;
	}
	else
	{
		do
		{
			digit = n % base;
			if(digit <= 9)
				chr = '0' + digit;
			else if(digit <= 16)
				chr = 'a' + (digit - 10);
			*--p = chr;
			n /= base;
			++len;
		} while ( n > 0 );

	}
	if(neg)
	{
		*--p = '-';
		++len;
	}
	*this = KWString(p, len);
}

void KWString::setNumEx(int v, WCHAR start, int base, int repeat)
{
	int x;
	int c = 1;
	KWString chs;

	clear();
	
	if(repeat == NUMBERING_REPEAT_NONE)
		v = v % base;
	else if(repeat == NUMBERING_REPEAT_COUNT)
	{
		c = (v / base) + 1;
		v = v % base;
	}

	do
	{
		x = v % base;
		v = v / base;
		chs.assign(c, start+x);
		*this = chs + *this;
	} while(v > 0);
}

void KWString::setNumEx(int v, WCHAR* start, int base, int repeat)
{
	int x;
	int c = 1;
	KWString chs;

	clear();
	if(repeat == NUMBERING_REPEAT_NONE)
		v = v % base;
	else if(repeat == NUMBERING_REPEAT_COUNT)
	{
		c = (v / base) + 1;
		v = v % base;
	}
	do
	{
		x = v % base;
		v = v / base;
		chs.assign(c, start[x]);
		*this = chs + *this;
	}while(v > 0);
}

const char* romanTenth[9] = 
{
	"X",
	"XX",
	"XXX",
	"XL",
	"L",
	"LX",
	"LXX",
	"LXXXX",
	"XC",
};

void KWString::setNumRoman(int v, bool bCapital)
{
	int x;
	KWString chs;

	clear();
	if(v >= 100)
	{
		*this += bCapital ? 'C' : 'c';
		v = v % 100;
	}
	x = v / 10;
	v = v % 10;
	if(x > 0)
	{
		KWString s;
		s.assignLocal8Bit(romanTenth[x-1]);
		if(!bCapital)
			s.makeLowerCase();
		*this += s;
	}

	if(v > 0)
	{
		v += (bCapital ? 0x2160 : 0x2170) - 1;
		append(1, v);
	}
}

void KWString::setNum(double n, WCHAR f, int prec)
{
#ifdef _DEBUG__
	if ( !(f==(WCHAR)'f' || f==(WCHAR)'F' || f==(WCHAR)'e' || f==(WCHAR)'E' || f==(WCHAR)'g' || f==(WCHAR)'G') )
		KTRACE( "KWString::setNum: Invalid format char '%c'", f );
#endif
	WCHAR format[20];
	WCHAR buff[120];
	WCHAR *fs = format;

	*fs++ = '%'; //   "%.<prec>l<f>"
	if ( prec > 99 )
		prec = 99;
	*fs++ = '.';
	if ( prec >= 10 )
	{
		*fs++ = prec / 10 + '0';
		*fs++ = prec % 10 + '0';
	} else {
		*fs++ = prec + '0';
	}
	*fs++ = 'l';
	*fs++ = f;
	*fs = '\0';
	// sprintf(buff, format, n );
	K2P_snwprintf(buff, 120, format, n );
	assign(buff);
}

KWString KWString::substr(int offset, int size) const
{
	KWString str;
	int length = this->size();

	if(length < (offset+size))
		size = length - offset;
	if(size > 0)
	{
		str.assign(size, ' ');
		HCMemcpy((void *)str.data(), (void *) ((WCHAR *)this->data() + offset), sizeof(WCHAR) * size);
	}
	return str;
}

void KWString::dump()
{
#ifdef WIN32_DESKTOP
	size_t i;

	for(i=0 ; i<=m_length; i++)
		KTRACE("0x%.4X,", m_string[i]);
	KTRACE("\n");
#endif
}

/*
void KWString::clear()
{
	std::wstring::clear();
}
*/
/**
void KWString::replace_internal(int pos, int len, const KWString& repstr)
{
	KWString out;

	if(pos > 0)
		out = substr(0, pos);
	out += repstr;
	if(size() > pos+len)
		out += substr(pos+len, size() - (pos+len));
	*this = out;
}
**/

int KWString::findWithBrace(WCHAR find_chr, int offset)
{
	int p;
	int depth = 0;
	LPCWSTR pst = data();
	int length = size();

	p = 0;
	if(offset > 0)
	{
		p = offset;
		pst += offset;
	}
	while(p < length)
	{
		if((*pst == find_chr) && (depth == 0))
			return p;
		else if( isOpenBraceChar(*pst) )
			depth++;
		else if( isCloseBraceChar(*pst) )
			depth--;
		pst++;
		p++;
	};
	return -1;
}

// length & bad character check
void KWString::CheckValidFilename()
{
	LPWSTR p = (LPWSTR) data();
	int length = size();
	LPCWSTR e;

	if(length > 24)
		length = 24;
	e = p + length;
	length = 0;
	while(p < e)
	{
		if( (*p == '\\') || (*p == '/') ||
			(*p == ':') || (*p == '*') ||
			(*p == '?') || (*p == '"') ||
			(*p == '<') || (*p == '>') ||
			(*p == '|') || (*p < ' ') )
		{
			*p = '\0';
			break;
		}
		p++;
		length++;
	};
	if(length > 0)
		resize(length);
	else
		assignLocal8Bit("tmp");
}

bool KWString::getTokenString(KWString& szDest, WCHAR delimeter, bool checkBrace)
{
	int		len, idx;

	szDest.clear();
	len = (int)size();
	if(len > 0)
	{
		if(checkBrace)
			idx = findWithBrace(delimeter);
		else
			idx = find(delimeter, 0, true);

		if(idx >= 0)
		{
			szDest = substr(0, idx);
			*this = substr(idx+1, len - idx - 1);
		}
		else
		{
			szDest = *this;
			clear();
		}
		return(true);
	}
	return(false);
}

bool KWString::getTokenString(KWString& szDest, KWString& delimeterStr)
{
	int		len, idx, sublen;

	szDest.clear();
	len = (int)size();
	if(len > 0)
	{
		idx = find(delimeterStr);
		if(idx >= 0)
		{
			szDest = substr(0, idx);
			sublen = delimeterStr.length();
			*this = substr(idx+sublen, len - idx - sublen);
		}
		else
		{
			szDest = *this;
			clear();
		}
		return(true);
	}
	return(false);
}

bool KWString::getTokenStringWhiteSpace(KWString& szDest)
{
	int		len, idx;

	szDest.clear();
	len = (int)size();
	if(len > 0)
	{
		idx = findWhiteSpace();
		if(idx >= 0)
		{
			szDest = substr(0, idx);
			*this = substr(idx+1, len - idx - 1);
		}
		else
		{
			szDest = *this;
			clear();
		}
		return(true);
	}
	return(false);
}

bool KWString::getTokenStringReverse(KWString& szDest, WCHAR delimeter)
{
	int		len, idx;

	szDest.clear();
	len = (int)size();

	if(len > 0)
	{
		idx = rfind(delimeter);
		if(idx >= 0)
		{
			szDest = substr(idx+1, (len - idx - 1));
			*this = substr(0, idx);
		}
		else
		{
			szDest = *this;
			clear();
		}
		return(true);
	}
	return(false);
}

KWString KWString::number_string(int val, int base)
{
	KWString str;
	str.setNum(val, base);
	return str;
}

KWString KWString::number_string(double val)
{
	KWString str;
	str.setNum(val);
	return str;
}

KWStringList::KWStringList()
{
}


KWStringList::KWStringList(const KWStringList& sl)
{
	*this = sl;
}


KWStringList::~KWStringList()
{
	clear();
}

const KWStringList& KWStringList::operator=(const KWStringList& src)
{
	if(this != &src)
	{
		KWString str;
		KWStringListConstIterator iter;

		clear();
		for(iter=src.begin() ; iter != src.end() ; iter++)
		{
			str = *iter;
			push_back(str);
		}
	}
	return *this;
}

const KWStringList& KWStringList::operator+=(const KWString& str)
{
	push_back(str);
	return *this;
}

const KWString& KWStringList::operator[](int pos) const
{
	return at(pos);
}

KWString KWStringList::join(KWString& separator)
{
	KWString str;
	KWString jstr;

	KWStringListConstIterator iter;
	for(iter=begin() ; iter!= end() ; iter++)
	{
		str = *iter;
		if(jstr.size() > 0)
			jstr += separator;
		jstr += str;
	}
	return jstr;
}

KWStringList KWStringList::split(WCHAR c, const KWString& szStr)
{
	KWStringList a;
	KWString src(szStr);
	KWString seg;

	while( src.getTokenString(seg, c) )
		a += seg;
	if(src.size() > 0)
		a += src;
	return a;
}

int KWStringList::find(const KWString& szStr, bool cs) const
{
	int index = 0;
	KWStringListConstIterator iter;

	for(iter=begin() ; iter!=end() ; iter++)
	{
		if(iter->compare(szStr, cs) == 0)
			return index;
		index++;
	}
	return -1;
}

bool KWStringList::remove(int index)
{
	if((index >= 0) && (index <= (int)m_list.size()))
	{
		KWStringListIterator iter;

		iter = begin();
		iter += index;

		m_list.erase(iter);
		return true;
	}
	return false;
}

void KWStringList::insert(int index, const KWString& str)
{
	if((index >= 0) && (index <= (int)m_list.size()))
	{
		m_list.insert(m_list.begin() + index, str);
	}
}

/**
int KWStringList::add(KWString& szStr)
{
	push_back(szStr);
	return(size() - 1);
}

int KWStringList::addHead(KWString& szStr)
{
	// push_front(szStr);
	m_list.insert(begin(), szStr);
	return(size() - 1);
}
**/

////////////////////////////////////////////
////////////////////////////////////////////

bool GetFileExtension(KWString& szFileName, KWString& fileExt)
{
	if(szFileName.size() > 0)
		{
		int nExt = szFileName.rfind('.');
		int nName = szFileName.rfind('/');
		if( (nExt > 0) && (nExt > nName) )
			{
			fileExt = szFileName.substr(nExt+1, szFileName.size() - nExt - 1);
			return(true);
			}
		}
	return(false);
}

int wstrwcpy(LPWSTR dBuff, LPCWSTR pBuff)
{
	int		len = K2P_WStringLength(pBuff);
	return wstrwncpy(dBuff, pBuff, len);
}

int wstrwncpy(LPWSTR dBuff, LPCWSTR pBuff, int len)
{
	if(dBuff != NULL)
	{
		int slen = K2P_WStringLength(pBuff);

		if(len > slen)
			len = slen;
		if((pBuff != NULL) && (len > 0))
		{
			HCMemcpy(dBuff, pBuff, sizeof(WCHAR) * len);
			dBuff += len;
		}
		*dBuff = 0;
	}
	return(len);
}

int strwcpy(KString& szNote, LPCWSTR pBuff)
{
	KWString wstr;

	wstr.assign(pBuff);
	szNote = wstr.toLocal8Bit();
	return szNote.size();
}

int wstrcpy(LPWSTR pBuff, KString& szNote)
{
	KWString wstr;
	wstr.fromLocal8Bit(szNote.data(), szNote.size());
	wstrwcpy(pBuff, (LPWSTR)wstr.data());
	return wstr.size();
}

int wstrncpy(LPWSTR pBuff, KString& szNote, int nMax)
{
	KWString wstr;
	wstr.fromLocal8Bit(szNote.data(), szNote.size());
	if(wstr.size() > (int)nMax)
		wstr.resize(nMax);
	wstrwcpy(pBuff, (LPWSTR)wstr.data());
	return wstr.size();
}

int strwncpy(KString& szNote, LPCWSTR pBuff, int len)
{
	KWString wstr;

	wstr.assign(pBuff, len);
	szNote = wstr.toLocal8Bit();
	return szNote.size();
}

#ifdef _USE_K_ARCHIVE
KArchive& operator>>( KArchive& a, KWString& s)
{
	int len;

	a >> len;
	s.assign(len, ' ');
	a.Read((void *)s.data(), len * sizeof(WCHAR));
	return a;
}

KArchive& operator<<( KArchive& a, KWString& s)
{
	a << (int) s.size();
	a.Write((void *)s.data(), s.size() * sizeof(WCHAR));
	return a;
}
#endif

bool isSpaceKChar(WCHAR ch)
{
	return( (ch == '\t') || (ch == ' ') ||
		(ch == '\x0C') || (ch == '\n') || (ch == '\r') );
}

bool isBreakableKChar(WCHAR ch)
{
	if (ch == 0xad ) // soft hyphen
		return true;

	if (ch >= 0xFF ) // none-ASCII character will be breakable
		return true;

	if ( ch == ' ' || ch == '\n' || ch == '\t' ||
		ch == '.' || ch == ',' || ch == ':' || ch == ';')
		return true;
	return false;
}

bool isAsciiKChar(WCHAR ch)
{
	if( ((ch >= 'A') && (ch <= 'Z')) ||
		((ch >= 'a') && (ch <= 'z')) )
		return true;
	return false;
}
