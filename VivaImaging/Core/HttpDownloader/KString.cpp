#include "stdafx.h"
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
KString : 8-bit ascii string Ç¥Çö
*/

/********
int strwcpy(KString& szNote, LPWSTR pBuff, int len)
{
#ifdef QT_BASE
	QString str = QString::fromUtf16((const ushort *)pBuff, len);
	szNote.assign(str.toLocal8Bit().data());
#else
	LPSTR buff = szNote.GetBuffer(len * 2 + 2);
	bool bUseDefault=false;
	len = WideCharToMultiByte(CP_ACP, 0, pBuff, len, buff, len * 2 + 2, NULL, &bUseDefault);
	buff[len] = 0;
	szNote.ReleaseBuffer();
#endif
	return(len);
}

int strwcpy(KString& szNote, LPWSTR pBuff)
{
	int len = wK2P_StringLength(pBuff);
	strwcpy(szNote, pBuff, len);
	return(len);
}

int strwncpy(KString& szNote, LPWSTR pBuff, int nMax)
{
	if(nMax > wK2P_StringLength(pBuff))
		nMax = wK2P_StringLength(pBuff);
	strwcpy(szNote, pBuff, nMax);
	return(nMax);
}

int wstrcpy(LPWSTR pBuff, KString& szNote)
{
#ifdef QT_BASE
	QString str = QString::fromLocal8Bit(szNote.data(), szNote.size());
	int len = str.length();
	HCMemcpy(pBuff, str.data(), len * sizeof(WCHAR));
	pBuff[len] = 0;
#else
	int len = MultiByteToWideChar(CP_ACP, 0, szNote, szNote.GetLength(), pBuff, szNote.GetLength());
	pBuff[len] = 0;
#endif
	return(len);
}

int wstrncpy(LPWSTR pBuff, KString& szNote, int nMax)
{
#ifdef QT_BASE
	QString str = QString::fromLocal8Bit(szNote.data(), szNote.size());
	int len = str.length();
	HCMemcpy(pBuff, str.data(), len * sizeof(WCHAR));
	pBuff[len] = 0;
#else
	int len = MultiByteToWideChar(CP_ACP, 0, szNote, szNote.size(), pBuff, nMax);
	pBuff[len] = 0;
#endif
	return(nMax);
}

****/

#if defined(WIN32)
int strwlen(LPCSTR szStr, int len)
{
	// return MultiByteToWideChar(CP_ACP, 0, szStr, len, NULL, 0);
	return K2P_LocalCodeToUnicode(szStr, len, NULL, 0);
}

int strwlen(KString& szStr)
{
	// return MultiByteToWideChar(CP_ACP, 0, szStr.data(), (int)szStr.size(), NULL, 0);
	return K2P_LocalCodeToUnicode(szStr.data(), (int)szStr.size(), NULL, 0);
}
#endif

//################################
// KString
//################################

KString::KString()
{
	m_string = NULL;
	m_length = 0;
}

KString::KString(const char* psz)
{
	m_string = NULL;
	m_length = 0;
	assign(psz);
}

KString::KString(const char* psz, int len)
{
	m_string = NULL;
	m_length = 0;
	assign(psz, len);
}

KString::KString(const unsigned char* psz)
{
	m_string = NULL;
	m_length = 0;
	assign((const char *)psz);
}

KString::KString(const unsigned char* psz, int len)
{
	m_string = NULL;
	m_length = 0;
	assign((const char *)psz, len);
}

KString::KString(int len, char ch)
{
	m_string = NULL;
	m_length = 0;
	assign(len, ch);
}

KString::KString(const KString& str)
{
	m_string = NULL;
	m_length = 0;
	assign(str.data(), str.size());
}

KString::~KString()
{
	clear();
}

#ifndef USE_STL_STRING

void KString::swap(KString& m)
{
	char* s;
	int l;

	s = m_string;
	m_string = m.m_string;
	m.m_string = s;

	l = m_length;
	m_length = m.m_length;
	m.m_length = l;
}

KString& KString::assign(const char* psz)
{
	if(psz == NULL)
	{
		clear();
		return *this;
	}
	else
		return assign(psz, (int)K2P_StringLength(psz));
}

KString& KString::assign(const char* psz, int len)
{
	clear();
	if((psz != NULL) && (len > 0))
	{
		m_string = (char *)HCMemAlloc(len + 1);
		if(m_string != NULL)
		{
			HCMemcpy(m_string, psz, len);
			m_string[len] = '\0';
			m_length = len;
		}
	}
	return *this;
}

KString& KString::assign(int len, char ch)
{
	clear();
	if(len > 0)
	{
		m_string = (char *)HCMemAlloc(len + 1);
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
KString& KString::append(const char* psz)
{
	if( psz == NULL )
		return *this;

	return append(psz, (int)K2P_StringLength(psz));
}

KString& KString::append(const char* psz, int len)
{
	if((psz != NULL) && (len > 0))
	{
		/*
		char* buff;
		buff = (char *)HCMemAlloc(m_length + len + 1);
		if(buff != NULL)
		{
			if(m_string != NULL)
				HCMemcpy(buff, m_string, m_length);
			HCMemcpy(&(buff[m_length]), psz, len);
			if(m_string != NULL)
				HCMemFree(m_string);

			m_string = buff;
			m_length += len;
			m_string[m_length] = '\0';
		}
		*/
		m_string = (char *)HCMemRealloc(m_string, m_length + len + 1);
		if(m_string != NULL)
		{
			HCMemcpy(&(m_string[m_length]), psz, len);
			m_length += len;
			m_string[m_length] = '\0';
		}
	}
	return *this;
}

KString& KString::append(int len, char ch)
{
	if(len > 0)
	{
		int i;

		/*
		char* buff;
		buff = (char *)HCMemAlloc(m_length + len + 1);
		if(buff != NULL)
		{
			if(m_string != NULL)
				HCMemcpy(buff, m_string, m_length);
			for(i = 0; i < len; i++)
				buff[i+m_length] = ch;
			if(m_string != NULL)
				HCMemFree(m_string);

			m_string = buff;
			m_length += len;
			m_string[m_length] = '\0';
		}
		*/
		m_string = (char *)HCMemRealloc(m_string, m_length + len + 1);
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

void KString::clear()
{
	if(m_string != NULL)
	{
		HCMemFree(m_string);
		m_string = NULL;
	}
	m_length = 0;
}

void KString::resize(int s)
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
			char* buff;
			buff = (char *)HCMemAlloc(len + 1);
			if(buff != NULL)
			{
				HCMemset(buff, 0, len);
				if(len > m_length)
					len = m_length;
				if(m_string != NULL)
				{
					HCMemcpy(buff, m_string, len);
					HCMemFree(m_string);
				}
				buff[s] = '\0';
				m_string = buff;
				m_length = s;
			}
			*/
			if(len > m_length)
				m_string = (char *)HCMemRealloc(m_string, len + 1);
			if(m_string != NULL)
			{
				if(len > m_length)
					HCMemset(&(m_string[m_length]), 0, len - m_length);
				m_string[s] = '\0';
				m_length = s;
			}
		}
	}
}

int KString::appendNullTerminater()
{
	append(1, 0);
	return length();
}

KString& KString::truncate(int len)
{
	resize(len);
	return *this;
}

KString& KString::remove(int index, int len)
{
	 if(len > (int)(m_length - index))
		 len = (int)(m_length - index);
	if(len > 0)
	{
		char* buff;

		buff = (char *)HCMemAlloc(m_length - len + 1);	// fixed memory exception by wooarmy.
		if(buff != NULL)
		{
			HCMemcpy(buff, m_string, index);
			HCMemcpy(&(buff[index]), &(m_string[index+len]), m_length-index-len);
			HCMemFree(m_string);

			m_length -= len;
			m_string = buff;
			m_string[m_length] = '\0';
		}

	}
	return *this;
}

#endif // not USE_STL_STRING

int strCompare(LPCSTR src, int srcLength, LPCSTR dst, int destLength, bool caseSense)
{
	int length, r;
	length = srcLength;
	if(length > destLength)
		length = destLength;

	if(caseSense)
	{
		r = HCMemcmp(src, dst, sizeof(char) * length);
		if(r != 0)
			return r;
	}
	else
	{
		char schar, dchar;
		while(length-- > 0)
		{
			schar = isAlpha(*src) ? toUpperChar(*src) : *src;
			dchar = isAlpha(*dst) ? toUpperChar(*dst) : *dst;
			if(schar != dchar)
				return((schar > dchar) ? 1 : -1);
			src++;
			dst++;
		};
	}
	if( srcLength != destLength )
		return (srcLength > destLength) ? 1 : -1;

	return 0;
}

char KString::at(int i) const
{
	if((i >= 0) && (i < size()))
#ifdef USE_STL_STRING
		return m_string.at(i);
#else
		return m_string[i];
#endif
	return 0;
}

void KString::setAt(int i, char c)
{
	if( i == size() )	// by wooarmy
	{
		append(1,c);
	}
	else if((i >= 0) && (i < size()))
	{
#ifdef USE_STL_STRING
		char *p = (char *)data();
		p[i] = c;
#else
		m_string[i] = c;
#endif
	}
}

int KString::find(char ch, int offset, bool caseSense) const
{
	int p;

	if(caseSense)
	{
#ifdef USE_STL_STRING
		p = m_string.find(ch, offset);
		if(p == std::string::npos)
			return -1;
		return((int) p);
#else
		LPSTR pst = (LPSTR)data();
		int length = size();

		p = 0;
		ch = ch;
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
		LPCSTR pst = data();
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

int KString::find(LPCSTR pstr, int length, int offset, bool cs) const
{
	const char* pSrc;

	if(length < 0)
		length = K2P_StringLength((LPCSTR)pstr);
	int slen = (int)size() - length - offset;
	if(slen >= 0)
	{
		pSrc = (const char*) data();
		for(int i=0 ; i<=slen ; i++)
		{
			if(strCompare(&(pSrc[offset+i]), length, pstr, length, cs) == 0)
				return(offset+(int)i);
		}
	}
	return -1;
}

int KString::find(const KString& expr, int offset, bool cs) const
{
	return find((LPCSTR) expr.data(), expr.size(), offset, cs);
}

int findOneOfChar(LPCSTR sstr, int slen, LPCSTR pstr, int plen, int offset)
{
	int i, c;

	if(plen < 0)
		plen = K2P_StringLength(pstr);
	if(slen < 0)
		slen = K2P_StringLength(sstr);

	slen -= offset;
	if(slen > 0)
	{
		sstr += offset;

		for(i = 0 ; i < slen ; i++)
		{
			for(c = 0; c < plen; c++)
			{
				if(pstr[c] == *sstr)
					return(i + offset);
			}
			sstr++;
		}
	}
	return -1;
}


int KString::findOneOf(LPCSTR pstr, int plen, int offset) const
{
	return findOneOfChar((LPCSTR)m_string, m_length, pstr, plen, offset);
}

int KString::rfind(char ch, int offset, bool caseSense) const
{ 
	int p;

	if(caseSense)
	{
#ifdef USE_STL_STRING
		p = m_string.rfind(ch, offset);
		if(p != std::string::npos)
			return((int) p);
		return -1;
#else
		int length = size();
		LPCSTR pstr = data();
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
		LPCSTR pstr = data();
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

int KString::rfind(const char* pstr, int length, int offset, bool caseSense) const
{ 
	const char* pSrc;

	if(length < 0)
		length = K2P_StringLength(pstr);
	pSrc = data();
	if((offset <= 0) || (offset > (size()-length)))
		offset = size()-length;

	for(int i=offset ; i>=0 ; i--)
	{
		if(strCompare(&(pSrc[i]), K2P_StringLength(&(pSrc[i])), pstr, length, caseSense) == 0)
			return(i);
	}
	return -1;
}

int KString::rfind(const KString& str, int offset, bool caseSense) const
{ 
	return rfind(str.data(), str.size(), offset, caseSense);
}

int KString::compare(const KString& expr, bool caseSense) const
{
	if(expr.length() == 0)
		return((m_length == 0) ? 0 : -1);

	if(m_length == 0)	// fix abort by wooarmy
		return((expr.length() == 0) ? 0 : 1);

	return strCompare(data(), size(), expr.data(), expr.size(), caseSense);
}

int KString::compare(LPCSTR str, bool caseSense) const
{
	int length;

	length = K2P_StringLength(str);

	if(length == 0)
		return((m_length == 0) ? 0 : -1);

	if(m_length == 0)	// fix abort by wooarmy
		return((length == 0) ? 0 : 1);

	return strCompare(data(), size(), str, length, caseSense);
}

int KString::compare(LPCSTR str, int length, bool caseSense) const
{
	if(length == 0)
		return((m_length == 0) ? 0 : -1);

	if(m_length == 0)	// fix abort by wooarmy
		return((length == 0) ? 0 : 1);

	return strCompare(data(), size(), str, length, caseSense);
}

int KString::compareLength(LPCSTR str, int length, bool caseSense) const
{
	if(length < 0)
		length = K2P_StringLength(str);
	if(length == 0)
		return((m_length == 0) ? 0 : -1);

	if(length > m_length)
		return -1;
	return strCompare(data(), length, str, length, caseSense);
}

void KString::replace(int pos, int len, KString& repstr)
{
#ifdef USE_STL_STRING
	*this = m_string.replace(pos, len, repstr.data());
#else
	int nlen, npos;
	char* buff;

	nlen = m_length - len + repstr.length();
	buff = (char *)HCMemAlloc(nlen + 1);
	if(buff != NULL)
	{
		HCMemcpy(buff, m_string, pos);
		HCMemcpy(&(buff[pos]), repstr.data(), repstr.length());
		npos = pos + repstr.length();
		if(m_length > (pos+len))
		{
			HCMemcpy(&(buff[npos]), &(m_string[pos+len]), (m_length - pos - len));
			npos += (m_length - pos - len);
		}
		buff[npos] = '\0';
		HCMemFree(m_string);
		m_string = buff;
		m_length = npos;
	}
#endif
}

int KString::replace(char ch1, char ch2)
{
	int iReplace=0;
	char* buff = (char*)data();

	for(int i=0 ; i<size() ; i++)
	{
		if(*buff == ch1)
		{
			*buff = ch2;
			iReplace++;
		}
		buff++;
	}
	return (iReplace);
}

int KString::replace(const KString& findstr, const KString& repstr)
{
	int search_pos;
	int i;
	KString out;
	int count = 0;

	search_pos = 0;
	while((i = find(findstr, search_pos)) >= 0)
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

KString KString::substr(int start, int len) const
{
	KString sub;

	if(start + len > size())
		len = size() - start;
	if(len > 0)
	{
		char* pstr = (char *) data();
		if(start > 0)
			pstr += start;
		sub.assign(pstr, len);
	}
	return sub;
}

void KString::clearSideWhiteSpace()
{
	int i, len;
	LPSTR d, s, e;
	
	if(m_length == 0)
		return;
	d = s = (LPSTR) data();
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

	/*
	int start = -1;
	int i;

	for(i=0 ; i<size() ; i++)
	{
		if( !isWhiteSpace(at(i)) ) // space
		{
			start = (int) i;
			break;
		}
	}
	if(start < 0)
	{
		clear();
		return;
	}
	if(start > 0)
		*this = substr(start, size() - start);

	for(i=size()-1 ; i>=0 ; i--)
	{
		if(!isWhiteSpace(at(i))) // space
		{
			if(i < size()-1)
				*this = substr(0, i + 1); // len = i + 1;
			break;
		}
	}
	if(i < 0)
		clear();
	*/
}

void KString::clearCrLf()
{
	LPSTR d, s;
	bool bOccured;
	int i;
	
	d = s = (LPSTR) data();
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

void KString::clearSideQuot()
{
	if(m_length > 2)
	{
		if( ((at(0) == '"') && (at(m_length-1) == '"')) ||
			((at(0) == '\'') && (at(m_length-1) == '\'')) )
			*this = substr(1, m_length - 2);
	}
}

KString KString::stripWhiteSpace() const
{
	KString str(*this);
	str.clearSideWhiteSpace();
	return str;
}

KString KString::simplifyWhiteSpace() const
{
	int i;
	KString str(*this);
#ifdef USE_STL_STRING
	KStringIterator ptr = str.begin();

	for(i=0 ; i<size() ; i++)
	{
		if( isWhiteSpace( *ptr ) )
			*ptr = (char) ' ';
		ptr++;
	}
#else
	char* ptr = (char *)str.data();

	for(i=0 ; i<size() ; i++)
	{
		if( isWhiteSpace( *ptr ) )
			*ptr = (char) ' ';
		ptr++;
	}
#endif
	return str;
}

// return true if it only has number or alpha
bool KString::isSimpleAscii() const
{
	int i;
	bool r = false;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( !isNumeric(*ptr) && !isAlpha(*ptr) )
			return false;
		ptr++;
		r = true;
	}
	return r;
}

bool KString::isSimpleDigit() const
{
	int i;
	bool r = false;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( !isNumeric(*ptr) )
			return false;
		ptr++;
		r = true;
	}
	return r;
}

bool KString::isSimpleAlpha() const
{
	int i;
	bool r = false;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( !isAlpha(*ptr) )
			return false;
		ptr++;
		r = true;
	}
	return r;
}

bool KString::isSimpleHexa() const
{
	int i;
	bool r = false;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( !isNumericHexa(*ptr) )
			return false;
		ptr++;
		r = true;
	}
	return r;
}

int KString::stripNumber(int digit)
{
	int i;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( isNumeric(*ptr) )
			ptr++;
		else if((digit == 16) && isNumericHexa(*ptr))
			ptr++;
		else
			break;
	}
	resize(i);
	return i;
}

int KString::toInt(bool* pok, int base) const
{
	if(pok != NULL)
		*pok = true;
	if(base != 10)
		{
		int s, value = 0;
		for(int i=0 ; i<size() ; i++)
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
	if(size() <= 0)
		return 0;
	return K2P_atoi((const char *)this->data());
}

__int64 KString::toInt64(bool* pok, int base) const
{
	if(pok != NULL)
		*pok = true;
	if(base != 10)
	{
		int s;
    __int64 value = 0;
		for(int i=0 ; i<size() ; i++)
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
	if(size() <= 0)
		return 0;
	return K2P_atoi64((const char *)this->data());
}

float KString::toFloat(bool* pok) const
{
	float d;

#ifdef WINCE
	if( sscanf(this->data(), "%f", &d) == 1)
#else
	// if( sscanf_s(this->data(), "%f", &d) == 1)
	d = (float) K2P_atof(this->data());
#endif
		{
		if(pok != NULL)
			*pok = true;
		}
	return d;
}

double KString::toDouble(bool* pok) const
{
	double d;

#ifdef WINCE
	if( sscanf(this->data(), "%lf", &d) == 1)
#else
	// if( sscanf_s(this->data(), "%lf", &d) == 1)
	d = K2P_atof(this->data());
#endif
	{
		if(pok != NULL)
			*pok = true;
	}
	return d;
}

void KString::setNum(int n, int base, int minLength)
{
#ifdef _DEBUG
    if (base < 2 || base > 16 )
		{
		// KTRACE( "KString::setNum: Invalid base %d", base );
		base = 10;
		}
#endif
    char   charbuf[65];
    LPSTR p = &charbuf[64];

    int  len = 0;
    bool neg = false;
	int	digit;
	char chr = 0;

    if ( n < 0 )
	{
	    n = -n;
	    neg = true;
	}
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

	if((minLength > 0) && (minLength > len))
	{
		for(digit = len; digit < minLength; digit++)
		{
			*--p = '0';
			++len;
		}
	}
	if(neg)
	{
		*--p = '-';
		++len;
	}
	//*this = KString(p, len);
	assign(p, len);
}

void KString::setNum(double n, char f, int prec)
{
#ifndef WINCE
#ifdef _DEBUG_DUMP
    if ( !(f=='f' || f=='F' || f=='e' || f=='E' || f=='g' || f=='G') )
		KTRACE( "KString::setNum: Invalid format char '%c'", f );
#endif
#endif
    char format[20];
    char buff[120] = "";
    char *fs = format;

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
    K2P_snprintf(buff, 120, format, n );
	assign(buff);
}

void KString::makeLowerCase()
{
#ifdef USE_STL_STRING
	KStringIterator it = m_string.begin();
	while(it != m_string.end())
	{
		if( isAlphaUpper(*it ) )
			*it = toLowerChar(*it);
		it++;
	};
#else
	int i;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( isAlphaUpper( *ptr ) )
			*ptr = toLowerChar(*ptr);
		ptr++;
	}
#endif
}

void KString::makeUpperCase()
{
#ifdef USE_STL_STRING
	KStringIterator it = m_string.begin();
	while(it != m_string.end())
	{
		if( isAlphaLower(*it ) )
			*it = toUpperChar(*it);
		it++;
	};
#else
	int i;
	char* ptr = (char *)data();

	for(i=0 ; i<size() ; i++)
	{
		if( isAlphaLower( *ptr ) )
			*ptr = toUpperChar(*ptr);
		ptr++;
	}
#endif
}

KString KString::toLower() const
{
	KString str(*this);
	str.makeLowerCase();
	return str;
}

KString KString::toUpper() const
{
	KString str(*this);
	str.makeUpperCase();
	return str;
}

/*******
KString KString::Left(int nCount) const
{
	_ASSERTE(nCount >= 0);
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	KString dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

KString KString::Right(int nCount) const
{
	_ASSERTE(nCount >= 0);
	if (nCount < 0)
		nCount = 0;
	if (nCount >= GetData()->nDataLength)
		return *this;

	KString dest;
	AllocCopy(dest, nCount, GetData()->nDataLength-nCount, 0);
	return dest;
}

KString KString::Mid(int nFirst, int nCount) const
{
	_ASSERTE(nFirst >= 0);
	_ASSERTE(nCount > 0);

	LPCSTR str = GetLeftPtr(nFirst);
	KString dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}
***/

#ifdef USE_STL_STRING

const KString& KString::operator=(const std::string& string)
{
	assign(string.data(), string.size());
	return *this;
}

#endif

const KString& KString::operator=(const KString& string)
{
	if(string.size() > 0)
		assign(string.data(), string.size());
	else
		clear();
	return *this;
}

const KString& KString::operator=(const char * string)
{
	if(string == NULL)
		clear();
	else
		assign(string, K2P_StringLength(string));
	return *this;
}

const KString& KString::operator+=(const KString& string)
{	
	// ConcatInPlace(string.GetData()->nDataLength, string.m_pchData);
	if(string.size() > 0)
		append(string.data(), string.size());
	return *this;
}

const KString& KString::operator+=(char ch)
{
	// this->Concat(ch);
	append(1, ch);
	return *this;
}

bool KString::operator==(const KString& str) const
{
	if(str.length() == 0)
		return(m_length == 0);
	if(m_length == 0)
		return false;
	return(compare(str.m_string) == 0);
}

bool KString::operator==(const char* pstr) const
{
	if((pstr == NULL) || (K2P_StringLength(pstr) == 0))
		return(m_length == 0);
	if(m_length == 0)
		return false;
	return(compare(pstr) == 0);
}

bool KString::operator!=(const KString& str) const
{
	if(str.length() == 0)
		return(m_length != 0);
	if(m_length == 0)
		return true;
	return(compare(str.m_string) != 0);
}

bool KString::operator!=(const char* pstr) const
{
	if((pstr == NULL) || (K2P_StringLength(pstr) == 0))
		return(m_length != 0);
	if(m_length == 0)
		return false;
	return(compare(pstr) != 0);
}

KString KString::operator+(const KString &s2) const
{
    KString str(*this);
	str += s2;
	return str;
}
KString KString::operator+(const char* s2) const
{
    KString str(*this);
	str += KString(s2);
	return str;
}
KString KString::operator+(char c2) const
{
    KString str(*this);
	str += c2;
	return str;
}

char KString::operator[](int pos) const
{
	if( m_string == NULL )
		return (char) ' ';	// by wooarmy
	return m_string[pos];
}

#ifdef USE_STRING_SPRINTF
int KString::sprintf(const char* format,...)
{
    char buf[1024];

	K2P_sprintf(buf, format);
	/*
    va_list ap;
    va_start( ap, format );			// use variable arg list
#ifdef WINCE
	vsprintf( buf, format, ap );
#else
	vsprintf_s( buf, format, ap );
#endif
	va_end( ap );
	*/
	assign(buf);
	return (int) size();
}
#endif

int KString::findWithBrace(char find_chr, int offset)
{
	int p;
	int depth = 0;
	LPCSTR pst = data();
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

static char invalid_char_in_filename[] = "\\/:*?\"<>| ";

void KString::CheckValidFilename()
{
	int len;
	KString fileExt;
	int nExt;

	nExt = rfind('.');
	if(nExt > 0)
	{
		fileExt = substr(nExt+1, length() - nExt - 1);
		resize(nExt);
	}

	len = findOneOf((LPCSTR)invalid_char_in_filename, 10);
	if(len > 24) // max filename
		len = 24;
	if(len > 0)
		resize(len);
	else if(len == 0)
		assign("tmp");

	if(fileExt.size() > 0)
	{
		len = fileExt.findOneOf((LPCSTR)invalid_char_in_filename, 10);
		if(len >= 0)
			fileExt.resize(len);

		append(1, '.');
		append(fileExt);
	}
}

void KString::makeSequentialName()
{
	KString name;
	KString fileExt;
	int nExt;
	int i;
	KString num_part;

	nExt = rfind('.');
	if(nExt > 0)
	{
		fileExt = substr(nExt, length() - nExt);
		name = substr(0, nExt);
	}
	else
		name = *this;

	// find number part
	i = name.length() - 1;
	while(i >= 0)
	{
		if( !isNumeric(name.at(i)) )
			break;
		i--;
	};

	if(i < (name.size() - 1))
	{
		i++;
		num_part = name.substr(i, name.length() - i);
		name = name.substr(0, i);
		i = num_part.toInt();
		num_part.setNum(i+1);
	}
	else
		num_part.setNum(0, 10);

	assign(name);
	append(num_part);
	append(fileExt);
}

bool KString::isBaseDirectory(const KStringList& pathnames)
{
	int i, count;

	count = (int)pathnames.size();
	for(i = 0; i < count ; i++)
	{
		const KString& path = pathnames.at(i);
		if( isBaseDirectory(path) )
			return true;
	}
	return false;
}

bool KString::isBaseDirectory(const KString& pathname)
{
	int length;

	if((pathname.length() > 0) && (m_length > pathname.length()))
	{
		length = pathname.length();
		if(strCompare(data(), length, pathname.data(), length, true) == 0)
			return true;
	}
	return false;
}

bool KString::getTokenString(KString& szDest, char delimeter, bool checkBrace)
{
	int		len, idx;

	szDest.clear();
	len = size();
	if(len > 0)
		{
		if(checkBrace)
			idx = findWithBrace(delimeter);
		else
			idx = find(delimeter);
		if(idx != -1)
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

bool KString::getTokenStringReverse(KString& szDest, char delimeter)
{
	int		len, idx;

	szDest.clear();
	len = (int) size();

	if(len > 0)
		{
		idx = (int)rfind(delimeter, 0);
		if(idx != -1)
			{
			szDest = substr(idx+1, len - idx - 1);
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

KString KString::number_string(int val, int base)
{
	KString str;

	str.setNum(val, base);
	return str;
}

KString KString::number_string(double val)
{
	KString str;

	str.setNum(val);
	return str;
}

int KString::appendAsRefString(const KString& contents)
{
	int i, start;
	char ch;
	KString sub;

	start = 0;
	for(i = 0; i < contents.length(); i++)
	{
		ch = contents.at(i);

		if(	(ch == '<') ||(ch == '>') || (ch == '&') )
		{
			if(i > start)
			{
				sub = contents.substr(start, i - start);
				append(sub);
			}

			if(ch == '<')
				append("&lt;", 4);
			else if(ch == '>')
				append("&gt;", 4);
			else if(ch == '&')
				append("&amp;", 5);
			start = i + 1;
		}
	}
	if(i > start)
	{
		sub = contents.substr(start, i - start);
		append(sub);
	}
	return length();
}

void KString::AddSubPathName(const KString& name)
{
	char delimiter_chr;

	delimiter_chr = K2P_GetDirectoryDelimiter();
	AddSubPathNameEx(name, delimiter_chr);
}

void KString::AddPathToName(const KString& name)
{
	if( name.IsAbsoluteFilePathName() )
	{
		*this = name;
		return;
	}

	AddSubPathName(name);
}

void KString::AddSubPathNameEx(const KString& name, char delimiter_chr)
{
	int		len;

	len = size();
	if( (len > 0) && (name.size() > 0) )
		{
		char c1, c2;

		c1 = at(len - 1);
		c2 = name.at(0);

		/*
		if( (c1 != delimiter_chr) && (c2 != delimiter_chr) )
			path += delimiter_chr;
		else if( (c1 == delimiter_chr) && (c2 == delimiter_chr) )
			path = path.substr(0, len - 1);
		*/

		if( !K2P_IsDirectoryDelimeter(c1) && !K2P_IsDirectoryDelimeter(c2) )
			*this += delimiter_chr;
		else if( K2P_IsDirectoryDelimeter(c1) && K2P_IsDirectoryDelimeter(c2) )
			*this = substr(0, len - 1);

		*this += name;
		}
	else
		{
		if(len <= 0)
			*this = name;
		}
}

bool KString::IsAbsoluteFilePathName() const
{
	if(size() > 0)
	{
#ifdef PLATFORM_WIN32
		if (':' == at(1))
#else
		if( ('/' == at(0)) || ('\\' == at(0)) )
#endif
			return(true);
	}
	return(false);
}

void LoadProgString(UINT id, KString& szBuff)
{
#ifdef QT_BASE
#ifdef NO_TRANSLATE
	szBuff = QString::null;
#else
	KString strname;
	strname.sprintf("STRING_%d", (int)id);
	szBuff = Res::tr(strname);
	if(szBuff == strname)
		KTRACE("Cannot load string %d\n", (int)id);
#endif
#else

#endif
}

/**
KString KString::fromLocal8Bit(char* pChar, int nLength)
{
	if(nLength < 0)
		nLength = lK2P_StringLength(pChar);
	KString s;
	s.Assign(pChar, nLength);
	return s;
}

KString KString::fromUtf8(char* buff, int len)
{
	char*	mbuff;
	WORD*	wbuff;
	bool	bUseDefault;
	int		mlen, wlen;
	KString	out;

	if(len < 0)
		len = lK2P_StringLength(buff);
	mlen = len * 2 + 4;
	mbuff = new char[mlen];
	wbuff = new WORD[len+1];
	wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) buff, len, wbuff, len);
	mlen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) wbuff, wlen, mbuff, mlen, NULL, &bUseDefault);
	if(mlen)
		out.Assign(mbuff, mlen);
	else
		out.Assign("");

	delete[] mbuff;
	delete[] wbuff;
	return out;
}

KString KString::fromUtf8(KString& txt)
{
	char*	mbuff;
	WORD*	wbuff;
	bool	bUseDefault;
	int		mlen, wlen;
	KString out;
	int		len = txt.GetLength();
	LPBYTE buff = (LPBYTE) txt.data();

	if(buff)
		{
		mlen = len * 2 + 4;
		mbuff = new char[mlen];
		HCMemset(mbuff, 0, mlen);
		wbuff = new WORD[len+1];
		HCMemset(wbuff, 0, (len+1) * sizeof(WORD));
		wlen = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR) buff, len, wbuff, len);
		mlen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) wbuff, wlen, mbuff, mlen, NULL, &bUseDefault);
		if(mlen)
			out.Assign(mbuff, mlen);
		else
			out.Assign("");

		delete []mbuff;
		delete []wbuff;
		}

	return out;
}

KString KString::toUtf8(KString& txt)
{
	char*	mbuff;
	WORD*	wbuff;
	int		mlen, wlen;
	KString out;
	int		len = txt.GetLength();
	LPBYTE buff = (LPBYTE) txt.data();

	if(buff)
		{
		mlen = len * 2 + 4;
		mbuff = new char[mlen];
		HCMemset(mbuff, 0, mlen);
		wbuff = new WORD[len+1];
		HCMemset(wbuff, 0, (len+1) * sizeof(WORD));
		wlen = MultiByteToWideChar(CP_ACP, 0, (LPCSTR) buff, len, wbuff, len);
		mlen = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR) wbuff, wlen, mbuff, mlen, NULL, NULL);
		if(mlen > 0)
			out.Assign(mbuff, mlen);
		else
			out.Assign("");

		delete []mbuff;
		delete []wbuff;
		}
	return out;
}
***/

//################################
// KStringList
//################################

KStringList::KStringList()
{
}

KStringList::~KStringList()
{
	clear();
}

KStringList::KStringList(const KStringList& sl)
{
	*this = sl;
}

const KStringList& KStringList::operator=(const KStringList& src)
{
	if(this != &src)
	{
		KString str;
		KStringListConstIterator iter;

		clear();
		for(iter=src.begin() ; iter != src.end() ; iter++)
		{
			str = *iter;
			push_back(str);
		}
	}
	return *this;
}

const KStringList& KStringList::operator+=(const KString& str)
{
	push_back(str);
	return *this;
}

const KString& KStringList::operator[](int pos) const
{
	return at(pos);
}

KString KStringList::join(KString& separator)
{
	KString str;
	KString jstr;
	KStringListConstIterator iter;

	for(iter=begin() ; iter!= end() ; iter++)
	{
		str = *iter;
		if(jstr.size() > 0)
			jstr += separator;
		jstr += str;
	}
	return jstr;
}

int KStringList::find(const KString& szStr, bool cs) const
{
	int index = 0;
	KStringListConstIterator iter;

	for(iter=begin() ; iter!=end() ; iter++)
	{
		if(iter->compare(szStr, cs) == 0)
			return index;
		index++;
	}
	return -1;
}

bool KStringList::remove(int index)
{
	if((index >= 0) && (index <= (int)m_list.size()))
	{
		KStringListIterator iter;

		iter = begin();
		iter += index;

		m_list.erase(iter);
		return true;
	}
	return false;
}

const char *KStringList::getString(int index)
{
	if((index >= 0) && (index < size()))
	{
		const KString& str = at(index);
		return str.data();
	}
	return NULL;
}

/*
int KStringList::add(KString& szStr)
{
	push_back(szStr);
	return((int) m_list.size() - 1);
}

int KStringList::addHead(KString& szStr)
{
	push_front(szStr);
	return((int) m_list.size() - 1);
}
**/

void KStringList::insert(int index, const KString& str)
{
	if((index >= 0) && (index <= (int)m_list.size()))
	{
		m_list.insert(m_list.begin() + index, str);
	}
}

KStringList KStringList::split(WCHAR c, const KString& szStr)
{
	KStringList a;
	KString src = szStr;
	KString seg;

	while( src.getTokenString(seg, (char) c) )
		a += seg;
	if(src.size() > 0)
		a += src;
	return a;
}

#ifdef _USE_K_ARCHIVE

KArchive& operator>>( KArchive& a, KString& s)
{
	int len;

	a >> len;
	s.assign(len, ' ');
	a.Read((void *)s.data(), len);
	return a;
}

KArchive& operator<<( KArchive& a, KString& s)
{
	a << (int) s.size();
	a.Write((void *)s.data(), s.size());
	return a;
}

#endif
