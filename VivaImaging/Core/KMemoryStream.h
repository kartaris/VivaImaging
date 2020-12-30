#pragma once
#include "HttpDownloader/KFile.h"

#define KMS_FOLDER_ITEM 0x3E7DFA01
#define KMS_IMAGE_ITEM 0x3E7DFA02
#define KMS_METADATA_GROUP 0x3E7DFA03
#define KMS_NAMEVALUE_ITEM 0x3E7DFA04
#define KMS_ANNOTATE_GROUP 0x3E7DFA05
#define KMS_SHAPE_ITEM 0x3E7DFA06
#define KMS_DOCTYPE_GROUP 0x3E7DFA07
#define KMS_DOCTYPE_COLUMN 0x3E7DFA08


class KMemoryStream : public KMemoryFile
{
public:
	KMemoryStream();
	KMemoryStream(LPBYTE buff, int bufflen, OpenMode mode = ReadWrite);
	virtual ~KMemoryStream();

	int WriteString(LPCTSTR str, int nLen = -1);

	int WriteStringBlock(LPCTSTR str, int nLen = -1);

	int ReadStringBlock(LPTSTR* pBuff);

	DWORD UpdateBlockSize(DWORD startPos);

};

