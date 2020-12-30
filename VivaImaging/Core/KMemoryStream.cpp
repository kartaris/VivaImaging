#include "stdafx.h"
#include "KMemoryStream.h"

KMemoryStream::KMemoryStream()
{

}

KMemoryStream::KMemoryStream(LPBYTE buff, int bufflen, OpenMode mode)
	: KMemoryFile(buff, bufflen, mode)
{

}
KMemoryStream::~KMemoryStream()
{

}

int KMemoryStream::WriteString(LPCTSTR str, int nLen)
{
	if (nLen < 0)
		nLen = lstrlen(str);

	return Write((const void*)str, sizeof(TCHAR) * nLen);
}

int KMemoryStream::WriteStringBlock(LPCTSTR str, int nLen)
{
	int writtenBytes;
	if (nLen < 0)
		nLen = lstrlen(str);

	Write(&nLen, sizeof(UINT));
	writtenBytes = sizeof(UINT);

	if (nLen > 0)
		writtenBytes += Write((const void*)str, sizeof(TCHAR) * nLen);

	return writtenBytes;
}

int KMemoryStream::ReadStringBlock(LPTSTR* pBuff)
{
	int nLen;
	int readedBytes;

	Read(&nLen, sizeof(int));
	readedBytes = sizeof(int);
	if (nLen > 0)
	{
		*pBuff = new TCHAR[nLen + 1];
		Read((void*)*pBuff, sizeof(TCHAR) * nLen);
		(*pBuff)[nLen] = '\0';
		readedBytes += sizeof(TCHAR) * nLen;
	}
	return readedBytes;
}

DWORD KMemoryStream::UpdateBlockSize(DWORD startPos)
{
	DWORD lastPos = Size();
	Seek(startPos - sizeof(DWORD), FilePosition::begin);
	DWORD blockSize = lastPos - startPos;
	Write(&blockSize, sizeof(DWORD)); // block size

#ifdef _DEBUG_STORAGE_BLOCK
	TCHAR msg[128];
	StringCchPrintf(msg, 128, _T("Pos:%d, Block Size:%d\n"), startPos, blockSize);
	OutputDebugString(msg);
#endif

	Seek(lastPos, FilePosition::begin);
	return blockSize;
}

