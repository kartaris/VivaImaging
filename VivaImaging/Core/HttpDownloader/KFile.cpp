#include "stdafx.h"
#include "KMemory.h"
#include "KFile.h"
//#include "KUtility.h"
//#include "KMacros.h"

#include "PlatformLib.h"

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int KFile::m_nLastError = 0;

/* 
KBuffer : System Endian 형식에 따른 WORD, DWORD, int 등의 포맷을 변경하는 base class.
 KFile::ReadDword() 는 Endian 형식을 무시한 읽기이고, GetDWord()는 Endian형식 변환한 읽기이다.
*/

BOOL KBuffer::PutByte(BYTE d)
{
	Write(&d, 1);
	return true;
}

BOOL KBuffer::PutWord(WORD d)
{
#ifdef CHANGE_ENDIAN_SYSTEM
	d = (d >> 8) | (d << 8);
#endif
	Write(&d, 2);
	return true;
}

BOOL KBuffer::PutInt(int d)
{
#ifdef CHANGE_ENDIAN_SYSTEM
	DWORD a;
	a = d >> 24;
	a |= (d >> 8) & 0x0000FF00;
	a |= (d << 8) & 0x00FF0000;
	a |= (d << 24);
	Write(&a, 4);
#else
	Write(&d, 4);
#endif
	return true;
}

BOOL KBuffer::PutDWord(DWORD d)
{
#ifdef CHANGE_ENDIAN_SYSTEM
	DWORD a;
	a = d >> 24;
	a |= (d >> 8) & 0x0000FF00;
	a |= (d << 8) & 0x00FF0000;
	a |= (d << 24);
	Write(&a, sizeof(DWORD));
#else
	Write(&d, 4);
#endif
	return true;
}

BYTE KBuffer::GetByte()
{
	BYTE d;
	Read(&d, 1);
	return d;
}

WORD KBuffer::GetWord()
{
	WORD d;
	Read(&d, 2);
#ifdef CHANGE_ENDIAN_SYSTEM
	d = (d >> 8) | (d << 8);
#endif
	return d;
}

short KBuffer::GetShort()
{
	WORD d;
	Read(&d, 2);
#ifdef CHANGE_ENDIAN_SYSTEM
	d = (d >> 8) | (d << 8);
#endif
	return d;
}

int KBuffer::GetInt()
{
	int d;
	Read(&d, sizeof(int));
#ifdef CHANGE_ENDIAN_SYSTEM
	DWORD a;
	a = d >> 24;
	a |= (d >> 8) & 0x0000FF00;
	a |= (d << 8) & 0x00FF0000;
	a |= (d << 24);
	return (int) a;
#endif
	return d;
}

DWORD KBuffer::GetDWord()
{
	DWORD d;
	Read(&d, 4);
#ifdef CHANGE_ENDIAN_SYSTEM
	DWORD a;
	a = d >> 24;
	a |= (d >> 8) & 0x0000FF00;
	a |= (d << 8) & 0x00FF0000;
	a |= (d << 24);
	return a;
#else
	return d;
#endif
}

BYTE KBuffer::ReadByte()
{
	BYTE v;

	if( Read(&v, 1) == 1)
		return v;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

short KBuffer::ReadShort()
{
	short d;
	if( Read(&d, sizeof(short)) == sizeof(short))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

WORD KBuffer::ReadWord()
{
	WORD d;
	if( Read(&d, sizeof(WORD)) == sizeof(WORD))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

int KBuffer::ReadInt()
{
	int d;
	if( Read(&d, sizeof(int)) == sizeof(int))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

long KBuffer::ReadLong()
{
	long d;
	if( Read(&d, sizeof(long)) == sizeof(long))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

DWORD KBuffer::ReadDword()
{
	DWORD d;
	if( Read(&d, sizeof(DWORD)) == sizeof(DWORD))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0;
}

double KBuffer::ReadDouble()
{
	double d;
	if( Read(&d, sizeof(double)) == sizeof(double))
		return d;
	SetBufferError(K_BUFFER_READ_OVER);
	return 0.0;
}

BOOL KBuffer::WriteByte(BYTE d)
{
	return(Write(&d, sizeof(BYTE)) == sizeof(BYTE));
}

BOOL KBuffer::WriteBytes(BYTE d, int len)
{
	int i;

	for(i = 0; i < len; i++)
	{
		if( !WriteByte(d) )
			return false;
	}
	return true;
}

BOOL KBuffer::WriteShort(short d)
{
	return(Write(&d, sizeof(short)) == sizeof(short));
}

BOOL KBuffer::WriteWord(WORD d)
{
	return(Write(&d, sizeof(WORD)) == sizeof(WORD));
}

BOOL KBuffer::WriteInt(int d)
{
	return(Write(&d, sizeof(int)) == sizeof(int));
}

BOOL KBuffer::WriteLong(long d)
{
	return(Write(&d, sizeof(long)) == sizeof(long));
}

BOOL KBuffer::WriteDword(DWORD d)
{
	return(Write(&d, sizeof(DWORD)) == sizeof(DWORD));
}

BOOL KBuffer::WriteDouble(double d)
{
	return(Write(&d, sizeof(double)) == sizeof(double));
}

/**
BOOL KBuffer::SkipByte(int count)
{
	SeekRel(count);
}
**/

void KFile::SetError(int error)
{
	m_nLastError = error;
}
int KFile::GetError()
{
	return m_nLastError;
}
void KFile::ThrowFileException()
{
}

#ifdef QT_BASE
KFile::KFile() : QFile()
#else
KFile::KFile()
#endif
{
	m_error = 0;
	m_OpenFlag = 0;
  m_szFileName = NULL;
#ifndef QT_BASE
	fh = NULL;
#endif
} 

KFile::KFile(LPCTSTR filename)
#ifdef QT_BASE
	: QFile(fileName)
#endif
{
	m_error = 0;
	m_OpenFlag = 0;
  int len = lstrlen(filename);
  if (len > 0)
  {
    m_szFileName = new TCHAR[len + 1];
    K2P_WStringCopy(m_szFileName, len, filename);
  }
  else
  {
    m_szFileName = NULL;
  }
}

#if 0
KFile::KFile(const KString& filename)
#ifdef QT_BASE
	: QFile(fileName)
#endif
{
	m_error = 0;
	m_OpenFlag = 0;
	m_szFileName = filename;
}

KFile::KFile(const KWString& filename)
#ifdef QT_BASE
	: QFile( QString::fromUtf16((const ushort *)filename.data(), filename.size()) )
#endif
{
	m_error = 0;
	m_OpenFlag = 0;
	m_szFileName = filename.toUtf8();
}
#endif

KFile::~KFile()
{
  if (m_szFileName != NULL)
    delete[] m_szFileName;
}

void KFile::swap(KFile& m)
{
	int		f;
	int		e;

	f = m_OpenFlag;
	m_OpenFlag = m.m_OpenFlag;
	m.m_OpenFlag = f;

	e = m_error;
	m_error = m.m_error;
	m.m_error = e;

  LPTSTR t = m.m_szFileName;
  m.m_szFileName = m_szFileName;
	m_szFileName = t;

#ifndef QT_BASE
	HANDLE	h;
	h = fh;
	fh = m.fh;
	m.fh = h;
#endif
}

BOOL KFile::Open(int mode, int truncateOldFile)
{
	m_OpenFlag = mode;
	fh = K2P_OpenFile(mode, m_szFileName, truncateOldFile);
	return(fh != 0);
}

int KFile::Read(void* lpBuff, int nsize)
{
	int nReaded;

	if(m_OpenFlag == WriteOnly)
	{
		m_error = HK_ERROR_NO_READFLAG;
		return(0);
	}
	nReaded = K2P_ReadFile(fh, lpBuff, nsize);
	if(nReaded != nsize)
		m_error = HK_ERROR_CANNOT_READ;
	return(nReaded);
}

int KFile::Write(const void* lpBuff, int nsize)
{
	int nWrited;

	if(m_OpenFlag == ReadOnly)
	{
		m_error = HK_ERROR_NO_WRITEFLAG;
		return(0);
	}
	nWrited = K2P_WriteFile(fh, lpBuff, nsize);

	if(nWrited != nsize)
		m_error = HK_ERROR_CANNOT_WRITE;
	return(nWrited);
}

int KFile::Seek(int offset, FilePosition position)
{
	return K2P_SeekFile(fh, offset, position);
}

int KFile::SeekRel(int offset)
{
	return Seek(offset, current);
}

int KFile::Size() const
{
	return K2P_FileSize(fh);
}

int KFile::GetPosition()
{
	return Seek(0, current);
}

void KFile::Close()
{
	K2P_FileClose(fh);
	m_error = 0;
}

BOOL KFile::isWritable(LPCTSTR filepathname)
{
#ifdef QT_BASE
	QFileInfo fi(filepathname);

	// qDebug("isWritable [%s]", filepathname.toLocal8Bit().data());
	return  fi.isWritable();
#else
	KFile f(filepathname);
	if( f.Open(WriteOnly, 0) )
	{
		f.Close();
		return true;
	}
	return false;
#endif
}

BOOL KFile::Remove(LPCTSTR filepathname)
{
	return K2P_FileRemove(filepathname);
}

/*
BOOL KFile::Remove(const KString& filepathname)
{
	return K2P_FileRemove(filepathname.data());
}

BOOL KFile::Remove(const KWString& filepathname)
{
	KString uname;

	uname = filepathname.toUtf8();
	return K2P_FileRemove(uname.data());
}
*/

#ifdef SUPPORT_FILE_RENAME
BOOL KFile::Rename(const char* filepathname, const char* newpathname)
{
	KWString filename(filepathname);
	KWString newname(filepathname);
	return Rename(filename, newname);
}

BOOL KFile::Rename(const KString& filepathname, const KString& newpathname)
{
	KWString filename(filepathname);
	KWString newname(filepathname);
	return Rename(filename, newname);
}

BOOL KFile::Rename(const KWString& filepathname, const KWString& newpathname)
{
#ifdef QT_BASE
	QString qname = filepathname.toQString();
	QString nname = newpathname.toQString();
	QFile::rename(qname, nname);
#else
	return !!K2P_MoveFile(filepathname.data(), newpathname.data());
#endif
}
#endif

int KFile::WriteToFile(LPCTSTR filename, LPBYTE buff, int length)
{
	KFile f(filename);

	if( f.Open(WriteOnly, 1) )
	{
		f.Write(buff, length);
		f.Close();
		return length;
	}
	return 0;
}

BOOL KFile::isExist(LPCTSTR filepathname)
{
	if((filepathname != NULL) && (lstrlen(filepathname) > 0))
	{
#ifdef QT_BASE
		return QFile::exists(filepathname);
#else
		return K2P_FileExist(filepathname);
#endif
	}
	return false;
}

BOOL KFile::isLocalFile(LPCTSTR filename)
{
	if(filename != 0)
	{
		return K2P_isLocalFile(filename);
	}
	return false;
}

BOOL KFile::IsHttpURL(LPCTSTR filename)
{
	if((filename != NULL) && (lstrlen(filename) > 7))
	{
#ifdef WIN32_SBCS
		KString head;
		KString local;

		head = filename.substr(0, 7);
		return(head.compare("http://", false) == 0);
#else
    return (StrCmpN(filename, _T("http://"), 7) == 0);
#endif
	}
	return false;
}

int KFile::getFileSize(LPCTSTR filepathname)
{
	int file_size = 0;
	HANDLE h;
	
	h = K2P_OpenFile(ReadOnly, filepathname, 0);
	if(h != NULL)
	{
		file_size = K2P_FileSize(h);
		K2P_FileClose(h);
	}
	return file_size;
}

BOOL KFile::IsSameBinary(LPCTSTR filename, LPBYTE buff, int length)
{
	if(KFile::getFileSize(filename) == length)
	{
		KMemoryFile mf;

		if( mf.loadFromFile(filename, ReadOnly) )
		{
			if(HCMemcmp(buff, mf.Data(), length) == 0)
				return true;
		}
	}
	return false;
}

BOOL KFile::IsSameBinary(LPCTSTR source_file, LPCTSTR dest_file)
{
	if(KFile::getFileSize(source_file) == KFile::getFileSize(dest_file))
	{
		KMemoryFile mf;

		mf.loadFromFile(source_file, ReadOnly);
		return KFile::IsSameBinary(dest_file, mf.Data(), mf.Size());
	}
	return false;
}

BOOL KFile::FileCopy(LPCTSTR o_filename, LPCTSTR n_filename)
{
	HANDLE o_h;
	HANDLE n_h;

	o_h = K2P_OpenFile(ReadOnly, o_filename, 0);
	n_h = K2P_OpenFile(WriteOnly, n_filename, 1);
	if( (o_h != NULL) && (n_h != NULL) )
	{
		int size;
		char buff[512];

		while((size = K2P_ReadFile(o_h, buff, 512)) > 0)
		{
			K2P_WriteFile(n_h, buff, size);
		}
		K2P_FileClose(o_h);
		K2P_FileClose(n_h);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
#define KBUFFER_ALLOC_UNIT 256

KMemoryFile::KMemoryFile()
	: KFile(NULL)
{
	m_error = 0;
	m_nSize = 0;
	m_nAlloc = 0;
	m_nPos = 0;
	m_Buff = NULL;
	m_OpenFlag = ReadWrite;
	m_buff_error = 0;
	m_incrementalSize = KBUFFER_ALLOC_UNIT;
}

KMemoryFile::KMemoryFile(LPBYTE buff, int bufflen, OpenMode mode)
	: KFile(NULL)
{
	m_error = 0;
	m_OpenFlag = mode;
	m_buff_error = 0;
	m_incrementalSize = KBUFFER_ALLOC_UNIT;

	if( (bufflen > 0) && (buff != NULL) )
	{
		m_Buff = (LPBYTE)HCMemAlloc(bufflen);
		HCMemcpy(m_Buff, buff, bufflen);
		m_nPos = 0;
		m_nSize = bufflen;
		m_nAlloc = bufflen;
	}
	else
	{
		m_nSize = m_nAlloc = 0;
		m_Buff = NULL;
		m_nPos = 0;
		m_nSize = 0;
		m_nAlloc = 0;
	}
}

KMemoryFile::~KMemoryFile()
{
	Clear();
}

void KMemoryFile::Clear()
{
	if(m_Buff != NULL)
		HCMemFree(m_Buff);
	m_Buff = NULL;
	m_nSize = m_nAlloc = 0;
	m_nPos = 0;
	m_OpenFlag = 0;
	m_buff_error = 0;
	m_incrementalSize = KBUFFER_ALLOC_UNIT;
}

void KMemoryFile::setIncremental(int s)
{
	m_incrementalSize = s;
}

void KMemoryFile::AllocateBuffer(int nSize)
{
	BYTE* p;
	int alloc;

	alloc = (m_nPos + nSize + m_incrementalSize - 1) / m_incrementalSize;
	if(m_Buff == NULL)
		p = (LPBYTE)HCMemAlloc(alloc * m_incrementalSize);
	else
		p = (LPBYTE)HCMemRealloc(m_Buff, alloc * m_incrementalSize);
	if(p != NULL)
	{
		m_nAlloc = alloc * m_incrementalSize;
		m_Buff = p;
	}
}

void KMemoryFile::Initialize()
{
	Clear();
	m_OpenFlag = ReadWrite;
}

void KMemoryFile::Close()
{
	m_nSize = m_nPos;
	m_nPos = 0;
}

BOOL KMemoryFile::loadFromData(LPBYTE buff, int bufflen, OpenMode mode)
{
	Clear();
	m_OpenFlag = mode;

	if( (bufflen > 0) && (buff != NULL) )
	{
		m_Buff = (LPBYTE)HCMemAlloc(bufflen);
		HCMemcpy(m_Buff, buff, bufflen);
		m_nSize = m_nAlloc = bufflen;
	}
	return true;
}

BOOL KMemoryFile::loadFromFile(KFile& file, OpenMode mode)
{
	if( file.Open(mode, 0) )
	{
		Clear();
		m_nSize = file.Size();
		if(m_nSize > 0)
		{
			m_Buff = (LPBYTE)HCMemAlloc(m_nSize);
			if(m_Buff != NULL)
			{
				m_nAlloc = m_nSize;
				file.Read(m_Buff, m_nSize);
			}
			else
			{
				m_nAlloc = m_nSize = 0;
			}
		}
		m_OpenFlag = mode;
		file.Close();
		return true;
	}
	return false;
}
BOOL KMemoryFile::loadFromFile(LPCTSTR filename, OpenMode mode)
{
	KFile file(filename);
	return loadFromFile(file, mode);
}
/*
BOOL KMemoryFile::loadFromFile(KWString& filename, OpenMode mode)
{
	KFile file(filename);
	return loadFromFile(file, mode);
}
*/
void KMemoryFile::swap(KMemoryFile& m)
{
	int		s;
	int		a;
	int		p;
	int		i;
	LPBYTE	b;
	int		e;

	s = m_nSize;
	m_nSize = m.m_nSize;
	m.m_nSize = s;

	a = m_nAlloc;
	m_nAlloc = m.m_nAlloc;
	m.m_nAlloc = a;

	p = m_nPos;
	m_nPos = m.m_nPos;
	m.m_nPos = p;

	i = m_incrementalSize;
	m_incrementalSize = m.m_incrementalSize;
	m.m_incrementalSize = i;

	b = m_Buff;
	m_Buff = m.m_Buff;
	m.m_Buff = b;

	e = m_buff_error;
	m_buff_error = m.m_buff_error;
	m.m_buff_error = e;

	KFile::swap(m);
}

BOOL KMemoryFile::copyFrom(const KMemoryFile* m)
{
	Clear();
	m_nSize = m->Size();
	if(m_nSize > 0)
	{
		m_Buff = (LPBYTE)HCMemAlloc(m_nSize);
		if(m_Buff != NULL)
		{
			HCMemcpy(m_Buff, m->m_Buff, m_nSize);
			m_nAlloc = m_nSize;
		}
		else
			m_nAlloc = m_nSize = 0;
	}
	m_OpenFlag = m->m_OpenFlag;
	return true;
}

int KMemoryFile::Compare(KMemoryFile& m)
{
	if(m_nSize == m.m_nSize)
		return HCMemcmp(m_Buff, m.m_Buff, m.m_nSize);
	return((m_nSize > m.m_nSize) ? 1 : -1);
}

void KMemoryFile::operator+=(KMemoryFile& mf)
{
	if(mf.Size() > 0)
		Write(mf.Data(), mf.Size());
}

KMemoryFile& KMemoryFile::operator=(const KMemoryFile& mf)
{
	if(this != &mf)
		copyFrom(&mf);
	return *this;
}

void KMemoryFile::Attach(LPBYTE buff, int bufflen, OpenMode mode)
{
	Clear();
	m_nPos = 0;
	m_nSize = m_nAlloc = bufflen;
	m_OpenFlag = mode;
	m_Buff = buff;
}

LPBYTE KMemoryFile::Detach()
{
  LPBYTE buff = m_Buff;
	m_nPos = 0;
	m_nSize = m_nAlloc = 0;
	m_OpenFlag = 0;
	m_Buff = NULL;
  return buff;
}

void KMemoryFile::TrimAppend(LPBYTE buff, int bufflen)
{
	int newsize;
	LPBYTE newBuff, p;
	int alloc = 0;

	newsize = (m_nSize - m_nPos) + bufflen;
	newBuff = NULL;
	if(newsize > m_nAlloc)
	{
		alloc = (newsize + m_incrementalSize - 1) / m_incrementalSize;
		p = newBuff = (LPBYTE)HCMemAlloc(alloc * m_incrementalSize);
		m_nAlloc = alloc * m_incrementalSize;
	}
	else
		p = m_Buff;

	if(p != NULL)
	{
		if((m_nSize - m_nPos) > 0)
		{
			HCMemcpy(p, m_Buff + m_nPos, (m_nSize - m_nPos));
			m_nSize = (m_nSize - m_nPos);
			p += m_nSize;
		}
		else
		{
			m_nSize = 0;
		}

		m_nPos = 0;
		HCMemcpy(p, buff, bufflen);
		m_nSize += bufflen;

		if(newBuff != NULL) // newly allocated
		{
			if(m_Buff != NULL)
				HCMemFree(m_Buff);
			m_Buff = newBuff;
		}
	}
	else if(m_Buff != NULL)
	{
		if((m_nPos > 0) && ((m_nSize - m_nPos) > 0))
		{
			HCMemcpy(m_Buff, m_Buff+m_nPos, (m_nSize - m_nPos));
			m_nSize -= m_nPos;
			m_nPos = 0;
		}
	}
}

int KMemoryFile::Read(void* lpBuff, int nSize)
{
	if(m_OpenFlag == WriteOnly)
	{
#ifndef NO_DEBUG
		KTRACE("Read Permission error");
#endif
		return(0);
	}
	if(m_nPos + nSize > m_nSize)
		nSize = m_nSize - m_nPos;
	if(nSize > 0)
	{
		HCMemcpy(lpBuff, &m_Buff[m_nPos], nSize);
		m_nPos += nSize;
		return(nSize);
	}
	else
		return(0);
}

int KMemoryFile::Write(const void* lpBuff, int nSize)
{
	if(m_OpenFlag == ReadOnly)
	{
#ifndef NO_DEBUG
		KTRACE("Write Permission error");
#endif
		return(0);
	}
	if(m_nPos + nSize >= m_nAlloc)
	{
		BYTE* p;
		int alloc;

		alloc = (m_nPos + nSize + m_incrementalSize - 1) / m_incrementalSize;

		if(m_Buff == NULL)
		{
			p = (LPBYTE)HCMemAlloc(alloc * m_incrementalSize);
		}
		else
		{
			p = (LPBYTE)HCMemRealloc(m_Buff, alloc * m_incrementalSize);
			/*
			if(m_nPos > 0)
			{
				HCMemcpy(p, m_Buff, m_nPos);
				HCMemFree(m_Buff);
			}
			*/
		}

		if(p == NULL)
			return 0;

		m_nAlloc = alloc * m_incrementalSize;
		m_Buff = p;
	}
	if(m_nPos + nSize > m_nAlloc)
		nSize = m_nAlloc - m_nPos;

	if(nSize > 0)
	{
		if(lpBuff != NULL)
			HCMemcpy(&(m_Buff[m_nPos]), lpBuff, nSize);
		else
			HCMemset(&(m_Buff[m_nPos]), 0, nSize);
		m_nPos += nSize;
		/*
    if (m_nPos < m_nAlloc)
      m_Buff[m_nPos] = '\0';
		*/
		if(m_nPos > m_nSize)
			m_nSize = m_nPos;
	}
	return nSize;
}

int KMemoryFile::Seek(int offset, FilePosition position)
{
	if(position == begin)
		m_nPos = offset;
	else if(position == current)
		m_nPos += offset;
	else if(position == end)
		m_nPos = m_nSize + offset;
	else
		KTRACE("unknown SeekPositon(%d)\n", (int)position);

	if(m_nPos > m_nSize)
	{
		if(m_OpenFlag == ReadOnly)
		{
			KTRACE("Write Permission error on seek\n");
			return(m_nPos);
		}
		int alloc = m_nPos;
		BYTE* p = (LPBYTE)HCMemAlloc(alloc);
		if(p != NULL)
		{
			if(m_nPos > 0)
			{
				HCMemcpy(p, m_Buff, m_nSize);
				HCMemFree(m_Buff);
			}
			m_Buff = p;
			m_nSize = alloc;
		}
	}
	return(m_nPos);
}

int KMemoryFile::SeekRel(int offset)
{
	return Seek(offset, current);
}

int KMemoryFile::Size() const
{
	return( m_nSize );
}

int KMemoryFile::GetPosition()
{
	return m_nPos;
}

BOOL KMemoryFile::WriteBytes(BYTE d, int len)
{
	return(Write(NULL, len) == len);
}

void KMemoryFile::SetBufferError(int e)
{
	KTRACE("KMemoryFile::BufferError(position=%d, size=%d)\n", m_nPos, m_nSize);
	m_buff_error = e;
}

BOOL KMemoryFile::hasBufferError()
{
	return !!m_buff_error;
}

int SkipArchive(KFile& ar, int nByte)
{
	int		rtn = 0;
	if(nByte > 0)
		{
		rtn = ar.SeekRel(nByte);
		/***
		char*	buff;
		buff = new char[nByte];
		
#ifdef QT_V4
		rtn = ar.read((char *) buff, nByte);
#else
		rtn = ar.readBlock((char *) buff, nByte);
#endif
		delete buff;
		***/
		}
	return(rtn);
}

