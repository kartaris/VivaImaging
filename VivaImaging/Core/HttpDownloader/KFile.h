#ifndef _KFILE_H
#define _KFILE_H

#ifdef QT_BASE
#include <qiodevice.h>
#endif

#ifdef USE_STL_STRING
#include <string>
#endif
//#include "KWString.h"
//#include "PlatformLib.h"

#define HK_ERROR_NO_READFLAG		1		// OpenFlag에 Read flag 없음
#define HK_ERROR_CANNOT_READ		2		// 지정한 크기만큼 읽을 수 없음
#define HK_ERROR_NO_WRITEFLAG	3		// OpenFlag에 Write flag 없음
#define HK_ERROR_CANNOT_WRITE	4		// 지정한 크기만큼 쓸 수 없음
#define HK_ERROR_NOT_WRITEOPEN	5		// 파일이 오픈되지 않음.
#define HK_ERROR_NOT_READOPEN	6		// 파일이 오픈되지 않음.

#ifdef WINCE
#define CHANGE_ENDIAN_SYSTEM
#endif
#define UNICODE_FILENAME

enum ERROR_CODE
{
	ERR_OUT_OF_MEMORY = 10,
	K_BUFFER_READ_OVER,
};

enum OpenMode { ReadOnly = 0, WriteOnly = 1, ReadWrite = 2 };
enum FilePosition { begin = 0, current = 1, end = 2 };

class KBuffer
{
public:
	KBuffer() { }
	virtual ~KBuffer() { }

	virtual int Read(void *pbuf, int len) = 0;
	virtual int Write(const void *pbuf, int len) = 0;
	virtual int Seek(int offset, FilePosition position) = 0;
	virtual int Size(void) const = 0;

	virtual int SeekRel(int offset) { return Seek(offset, current); }
	virtual int GetPosition() { return Seek(0, current); }
	virtual void SetSize(int size) { ; }
	virtual void Clear() { ; }
	virtual void Close() { ; }
	virtual void Initialize() { ; }

	virtual BOOL IsRandomAccess() { return false; }

	// Big/Little endian converted read/write
	virtual BOOL PutByte(BYTE d);
	virtual BOOL PutWord(WORD d);
	virtual BOOL PutInt(int d);
	virtual BOOL PutDWord(DWORD d);
	virtual BYTE GetByte();
	virtual WORD GetWord();
	virtual short GetShort();
	virtual int GetInt();
	virtual DWORD GetDWord();

	// direct memory read/write
	virtual BYTE ReadByte();
	virtual short ReadShort();
	virtual WORD ReadWord();
	virtual int ReadInt();
	virtual long ReadLong();
	virtual DWORD ReadDword();
	virtual double ReadDouble();
	virtual BOOL WriteByte(BYTE d);
	virtual BOOL WriteBytes(BYTE d, int len);
	virtual BOOL WriteShort(short d);
	virtual BOOL WriteWord(WORD d);
	virtual BOOL WriteInt(int d);
	virtual BOOL WriteLong(long d);
	virtual BOOL WriteDword(DWORD d);
	virtual BOOL WriteDouble(double dnum);

	virtual BOOL IsEndOfFile() { return(Size() <= GetPosition()); }
	virtual void SetBufferError(int e) { ;}
	virtual BOOL hasBufferError() { return false; }

	// virtual BOOL SkipByte(int count);

};

#ifdef QT_BASE
class KFile : public QFile
#else
class KFile : public KBuffer
#endif
{
public:
	KFile();
	KFile(LPCTSTR fileName);
	//KFile(const KString& filename);
	//KFile(const KWString& filename);
	virtual ~KFile();

	int		m_OpenFlag;
	int		m_error;
  LPTSTR m_szFileName;

#ifndef QT_BASE
	HANDLE	fh;
#endif

	void swap(KFile& f);

	virtual int Read(void* lpBuff, int nSize);
	virtual int Write(const void* lpBuff, int nSize);
	virtual int Seek(int offset, FilePosition position);
	virtual int SeekRel(int offset);
	virtual int Size() const;
	virtual int GetPosition();

	virtual BOOL Open(int mode, int truncateOldFile);
	virtual void Close();

	virtual void SetBufferError(int e) { m_error = e;}
	virtual BOOL hasBufferError() { return !!m_error; }

#if 1
	static BOOL isWritable(LPCTSTR dirpath);
	static BOOL Remove(LPCTSTR filepathname);
	//static BOOL Remove(const KString& filepathname);
	//static BOOL Remove(const KWString& filepathname);
#ifdef SUPPORT_FILE_RENAME
	static BOOL Rename(const char* filepathname, const char* newpathname);
	static BOOL Rename(const KString& filepathname, const KString& newpathname);
	static BOOL Rename(const KWString& filepathname, const KWString& newpathname);
#endif
	static int WriteToFile(LPCTSTR filename, LPBYTE buff, int length);
	static BOOL isExist(LPCTSTR filepathname);
	static BOOL isLocalFile(LPCTSTR filepathname);
	static BOOL IsHttpURL(LPCTSTR filename);
	static int getFileSize(LPCTSTR filepathname);

	static BOOL IsSameBinary(LPCTSTR filename, LPBYTE buff, int length);
	static BOOL IsSameBinary(LPCTSTR source_file, LPCTSTR dest_file);
#endif

	static BOOL FileCopy(LPCTSTR o_filenameUtf8, LPCTSTR n_filenameUtf8);

	static int m_nLastError;
	static void SetError(int error);
	static int GetError();
	static void ThrowFileException();
};

class KMemoryFile : public KFile
{
public:
	KMemoryFile();
	KMemoryFile(LPBYTE buff, int bufflen, OpenMode mode = ReadWrite);
	virtual ~KMemoryFile();

	int		m_nSize;
	int		m_nAlloc;
	int		m_nPos;
	int		m_incrementalSize;
	LPBYTE	m_Buff;

	void setMode(int m) { m_OpenFlag = m; }
	void setIncremental(int s);
	void AllocateBuffer(int nSize);
	virtual void SetSize(int size) { m_nSize = size; }
	virtual void Initialize();
	virtual BOOL IsRandomAccess() { return true; }

	BOOL loadFromData(LPBYTE buff, int bufflen, OpenMode mode=ReadWrite);
	BOOL loadFromFile(KFile& file, OpenMode mode);
	BOOL loadFromFile(LPCTSTR filename, OpenMode mode);
	//BOOL loadFromFile(LPCTSTR filename, OpenMode mode);

	void swap(KMemoryFile& m);
	BOOL copyFrom(const KMemoryFile* m);
	int Compare(KMemoryFile& m);

	void operator+=(KMemoryFile& mf);
	KMemoryFile& operator=(const KMemoryFile& mf);

	void Attach(LPBYTE buff, int bufflen, OpenMode mode = ReadWrite);
	LPBYTE Detach();
	void TrimAppend(LPBYTE buff, int bufflen);

	LPBYTE Data() { return m_Buff; }
	void* GetAddress() { return(m_Buff + m_nPos); }
	int GetAvailable() { return(m_nSize - m_nPos); }

	virtual int Read(void* lpBuff, int nSize);
	virtual int Write(const void* lpBuff, int nSize);

	virtual int Seek(int offset, FilePosition position);
	virtual int SeekRel(int offset);
	virtual int Size() const;
	virtual int GetPosition();

	virtual BOOL WriteBytes(BYTE d, int len);

	virtual void Clear();
	virtual void Close();

	int m_buff_error;
	virtual void SetBufferError(int e);
	virtual BOOL hasBufferError();
};

#endif // _KFILE_H
