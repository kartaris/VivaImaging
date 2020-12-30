#include "stdafx.h"

#include "KConstant.h"
#include "KSettings.h"
#include "KEcmDocType.h"
#include "../Document/KImageBase.h"
#include "../Document/Shape/KShapeDef.h"
#include "../Platform/Platform.h"
#include "../Core/HttpDownloader/KFile.h"

#include <strsafe.h>

KEcmDocTypeInfo::KEcmDocTypeInfo()
{
  mTargetFolderOid[0] = '\0';
  mDocTypeOid[0] = '\0';
  mpDocTypeName = NULL;
}

KEcmDocTypeInfo::~KEcmDocTypeInfo()
{
  Clear();
}

EcmExtDocTypeColumnType KEcmDocTypeInfo::TypeFromString(LPCTSTR typeName)
{
  if (lstrcmp(typeName, _T("LINE")) == 0)
    return EcmExtDocTypeColumnType::SingleLine;
  else if (lstrcmp(typeName, _T("LINES")) == 0)
    return EcmExtDocTypeColumnType::MultiLine;
  else if (lstrcmp(typeName, _T("DATE")) == 0)
    return EcmExtDocTypeColumnType::Date;
  else if (lstrcmp(typeName, _T("NUMBER")) == 0)
    return EcmExtDocTypeColumnType::Number;
  else
  {
    CString msg = _T("Unknown DocType-ColumnType : ");
    msg.Append(typeName);
    OutputDebugString(msg);
    return EcmExtDocTypeColumnType::None;
  }
}

BOOL KEcmDocTypeInfo::TypeToString(EcmExtDocTypeColumnType type, LPTSTR str, int buffLength)
{
  switch (type)
  {
  case EcmExtDocTypeColumnType::SingleLine:
    StringCchCopy(str, buffLength, _T("LINE"));
    return TRUE;
  case EcmExtDocTypeColumnType::MultiLine:
    StringCchCopy(str, buffLength, _T("LINES"));
    return TRUE;
  case EcmExtDocTypeColumnType::Date:
    StringCchCopy(str, buffLength, _T("DATE"));
    return TRUE;
  case EcmExtDocTypeColumnType::Number:
    StringCchCopy(str, buffLength, _T("NUMBER"));
    return TRUE;
  default:
    StringCchCopy(str, buffLength, _T("UNKNOWN"));
    return FALSE;
  }
}

static LPCTSTR MetaValueTypeName[] = {
  _T(""),
  _T("LINE"),
  _T("LINES"),
  _T("DATE"),
  _T("NUMBER"),
};

LPCTSTR KEcmDocTypeInfo::GetValueTypeName(EcmExtDocTypeColumnType type)
{
  if ((EcmExtDocTypeColumnType::None <= type) && (type <= EcmExtDocTypeColumnType::MaxColumnType))
    return MetaValueTypeName[(int)type];
  return MetaValueTypeName[0];
}


KEcmDocTypeInfo& KEcmDocTypeInfo::operator=(const KEcmDocTypeInfo& ob)
{
  if (this != &ob)
  {
    Clear();

    this->SetDocTypeInfo(ob.mTargetFolderOid, ob.mDocTypeOid, ob.mpDocTypeName);

    KEcmExtDocTypeColumnConstIterator it = ob.mArray.begin();
    while (it != ob.mArray.end())
    {
      EcmExtDocTypeColumn* s = *(it++);
      EcmExtDocTypeColumn* p = CreateColumn(s->type, s->name, s->key, s->maxValueLength, s->valueString);
      mArray.push_back(p);
    }
  }
  return *this;
}

void KEcmDocTypeInfo::Clear()
{
  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    ClearDocTypeColumn(p);
  }
  mArray.clear();

  mTargetFolderOid[0] = '\0';
  mDocTypeOid[0] = '\0';
  if (mpDocTypeName != NULL)
  {
    delete[] mpDocTypeName;
    mpDocTypeName = NULL;
  }
}

void KEcmDocTypeInfo::ClearDocTypeColumn(EcmExtDocTypeColumn* p)
{
  if (p->valueString != NULL)
    delete[] p->valueString;
  delete p;
}

void KEcmDocTypeInfo::SetDocTypeInfo(LPCTSTR targetFolderOid, LPCTSTR docTypeOid, LPCTSTR docTypeName)
{
  if (targetFolderOid != NULL)
    StringCchCopy(mTargetFolderOid, MAX_OID, targetFolderOid);
  else
    mTargetFolderOid[0] = '\0';

  if (docTypeOid != NULL)
    StringCchCopy(mDocTypeOid, MAX_OID, docTypeOid);
  else
    mDocTypeOid[0] = '\0';

  if (mpDocTypeName != NULL)
    delete[] mpDocTypeName;
  mpDocTypeName = AllocString(docTypeName);
}

EcmExtDocTypeColumn* KEcmDocTypeInfo::CreateColumn(EcmExtDocTypeColumnType type, LPCTSTR name, LPCTSTR key, UINT maxLength, LPCTSTR value)
{
  EcmExtDocTypeColumn* p = new EcmExtDocTypeColumn;
  p->type = type;
  StringCchCopy(p->name, COLUMNNAME_LENGTH, name);
  StringCchCopy(p->key, KEYNAME_LENGTH, key);
  p->maxValueLength = maxLength;
  p->valueString = AllocString(value);
  return p;
}

void KEcmDocTypeInfo::Add(EcmExtDocTypeColumn* n)
{
  EcmExtDocTypeColumn* p = Get(n->key);
  if (p == NULL)
  {
    mArray.push_back(n);
  }
  else
  {
    p->type = n->type;
    StringCchCopy(p->name, COLUMNNAME_LENGTH, n->name);
    // StringCchCopy(p->key, KEYNAME_LENGTH, key);
    p->maxValueLength = n->maxValueLength;
    if (p->valueString != NULL)
      delete[] p->valueString;
    p->valueString = AllocString(n->valueString);
    ClearDocTypeColumn(n);
  }
}

void KEcmDocTypeInfo::Add(EcmExtDocTypeColumnType type, LPCTSTR name, LPCTSTR key, UINT maxLength, LPCTSTR value)
{
  EcmExtDocTypeColumn* p = CreateColumn(type, name, key, maxLength, value);
  Add(p);
}

BOOL KEcmDocTypeInfo::SetValue(LPCTSTR key, LPCTSTR value)
{
  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    if (lstrcmp(p->key, key) == 0)
    {
      if (p->valueString != NULL)
        delete[] p->valueString;

      p->valueString = AllocString(value);
      return TRUE;
    }
  }
  return FALSE;
}

EcmExtDocTypeColumn* KEcmDocTypeInfo::Get(LPCTSTR key)
{
  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    if (lstrcmp(p->key, key) == 0)
    {
      return p;
    }
  }
  return NULL;
}

EcmExtDocTypeColumnType KEcmDocTypeInfo::GetColumnType(LPCTSTR key)
{
  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    if (lstrcmp(p->key, key) == 0)
    {
      return p->type;
    }
  }
  return EcmExtDocTypeColumnType::None;
}

BOOL KEcmDocTypeInfo::IsValidKey(LPCTSTR key)
{
  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    if (lstrcmp(p->key, key) == 0)
    {
      return TRUE;
    }
  }
  return FALSE;
}

#ifdef USE_XML_STORAGE
HRESULT KEcmDocTypeInfo::ReadAttributes(IXmlReader* pReader)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (CompareStringNC(pwszLocalName, tagExtDocTypeOid) == 0)
      {
        StringCchCopy(mDocTypeOid, MAX_OID, pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocTypeName) == 0)
      {
        if (mpDocTypeName != NULL)
          delete[] mpDocTypeName;
        mpDocTypeName = AllocString(pwszValue);
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return S_OK;
}

HRESULT KEcmDocTypeInfo::ReadDocTypeInfo(CComPtr<IXmlReader> pReader, EcmExtDocTypeColumn* col)
{
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  const WCHAR* pwszValue;

  HRESULT hr = pReader->MoveToFirstAttribute();
  while (hr != S_FALSE)
  {
    if (!pReader->IsDefault())
    {
      UINT cwchPrefix;
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnKey) == 0)
      {
        StringCchCopy(col->key, MAX_OID, pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnType) == 0)
      {
        col->type = TypeFromString(pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnName) == 0)
      {
        StringCchCopy(col->name, MAX_OID, pwszValue);
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnMaxLength) == 0)
      {
        swscanf_s(pwszValue, _T("%d"), &col->maxValueLength);
      }
      else if (CompareStringNC(pwszLocalName, tagExtDocTypeColumnValue) == 0)
      {
        col->valueString = AllocString(pwszValue);
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return S_OK;
}

HRESULT KEcmDocTypeInfo::ReadDocument(CComPtr<IXmlReader> pReader, int flag)
{
  HRESULT hr;

  if (FAILED(hr = ReadAttributes(pReader)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  if (FAILED(hr = pReader->MoveToElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  if (pReader->IsEmptyElement())
    return S_OK;

  XmlNodeType nodeType;
  const WCHAR* pwszPrefix;
  const WCHAR* pwszLocalName;
  UINT cwchPrefix;

  while (S_OK == (hr = pReader->Read(&nodeType)))
  {
    switch (nodeType)
    {
    case XmlNodeType_Element:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on Element, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }

      if (CompareStringNC(pwszLocalName, tagExtDocTypeColumn) == 0)
      {
        EcmExtDocTypeColumn* col = new EcmExtDocTypeColumn;
        hr = ReadDocTypeInfo(pReader, col);
        if (FAILED(hr))
          return hr;
        Add(col);
      }

      if (FAILED(hr = pReader->MoveToElement()))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      break;

    case XmlNodeType_EndElement:
      if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting prefix on EndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting local name on EndElement, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      return S_OK;
      break;

    case XmlNodeType_Comment:
    default:
      break;
    } // end of switch
  } // end of while
  return S_OK;
}

LPSTR KEcmDocTypeInfo::StoreToString()
{
  //LPSTREAM pStream;
  CComPtr<IStream> spStream;
  CComPtr<IXmlWriter> pWriter;
  HRESULT hr;
  CString errmsg;

  HGLOBAL hg = GlobalAlloc(GMEM_FIXED, 4096);
  if (FAILED(hr = CreateStreamOnHGlobal(hg, FALSE, &spStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating file writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**)&pWriter, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error creating xml writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->SetOutput(spStream)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error setting output for writer, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  if (FAILED(hr = pWriter->WriteStartDocument(XmlStandalone_Omit)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartDocument, error is %08.8lx"), hr);
    errmsg.SetString(msg);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  hr = StoreDocument(pWriter, FALSE);
  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, on StoreDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->WriteEndDocument()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndDocument, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }
  if (FAILED(hr = pWriter->Flush()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: Flush, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return FALSE;
  }

  BYTE* p = NULL;
  ULARGE_INTEGER streamSize;
  LARGE_INTEGER zero;
  zero.QuadPart = 0;
  spStream->Seek(zero, STREAM_SEEK_CUR, &streamSize);
  UINT nSize = (UINT)(streamSize.LowPart) + 2;
  if (nSize > 0)
  {
    p = new BYTE[nSize];
    memset(p, 0, nSize);
    spStream->Seek(zero, STREAM_SEEK_SET, nullptr);
    spStream->Read(p, nSize, nullptr);
  }

  return (LPSTR)p;
}


HRESULT KEcmDocTypeInfo::StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagExtDocType, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  
  hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeOid, NULL, mDocTypeOid);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeName, NULL, mpDocTypeName);

  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
    EcmExtDocTypeColumn* p = *(it++);
    StoreDocTypeInfo(pWriter, p);
  }

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  return S_OK;
}

HRESULT KEcmDocTypeInfo::StoreDocTypeInfo(CComPtr<IXmlWriter> pWriter, EcmExtDocTypeColumn* p)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagExtDocTypeColumn, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnKey, NULL, p->key);

  TCHAR str[32];
  TypeToString(p->type, str, 32);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnType, NULL, str);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnName, NULL, p->name);

  StringCchPrintf(str, 32, _T("%d"), p->maxValueLength);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnMaxLength, NULL, str);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnValue, NULL, p->valueString);

  if (FAILED(hr = pWriter->WriteEndElement()))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteEndElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  return S_OK;
}
#endif // USE_XML_STORAGE

HANDLE KEcmDocTypeInfo::ToHandle()
{
  KMemoryStream ms;

  StoreToBuffer(ms);

  HANDLE h = GlobalAlloc(0, ms.Size());
  if (h != NULL)
  {
    void* p = GlobalLock(h);
    if (p != NULL)
      memcpy(p, ms.Data(), ms.Size());
    GlobalUnlock(h);
  }
  return h;
}

BOOL KEcmDocTypeInfo::FromHandle(HANDLE h)
{
  void* p = GlobalLock(h);
  if (p != NULL)
  {
    KMemoryStream ms((LPBYTE)p, GlobalSize(h), ReadOnly);

    DWORD tag = ms.GetDWord();
    if (tag == KMS_DOCTYPE_GROUP)
      LoadFromBuffer(ms);

    GlobalUnlock(h);
    return (tag == KMS_DOCTYPE_GROUP);
  }
  return FALSE;
}

DWORD KEcmDocTypeInfo::StoreToBuffer(KMemoryStream& mf)
{
  mf.WriteDword(KMS_DOCTYPE_GROUP);
  mf.WriteDword(0); // block size
  DWORD startPos = mf.Size(); // 62

  mf.WriteStringBlock(mDocTypeOid);
  mf.WriteStringBlock(mpDocTypeName);

  KEcmExtDocTypeColumnIterator it = mArray.begin();
  while (it != mArray.end())
  {
      EcmExtDocTypeColumn* p = *(it++);
      StoreDocTypeColumn(mf, p);
  }

  // reset block size
  DWORD blockSize = mf.UpdateBlockSize(startPos);
  return (blockSize + sizeof(DWORD) + sizeof(DWORD));
}

DWORD KEcmDocTypeInfo::StoreDocTypeColumn(KMemoryStream& mf, EcmExtDocTypeColumn* p)
{
  mf.WriteDword(KMS_DOCTYPE_COLUMN);// TIFF_DOCTYPE_INFO);
  mf.WriteDword(0); // block size
  DWORD startPos = mf.Size();

  mf.Write(&p->maxValueLength, sizeof(DWORD));
  mf.WriteStringBlock(p->key);

  TCHAR str[32];
  TypeToString(p->type, str, 32);
  mf.WriteStringBlock(str);

  mf.WriteStringBlock(p->name);

  mf.WriteStringBlock(p->valueString);

  // reset block size
  DWORD blockSize = mf.UpdateBlockSize(startPos);
  return (blockSize + sizeof(DWORD) + sizeof(DWORD));
}

DWORD KEcmDocTypeInfo::LoadFromBuffer(KMemoryStream& mf)
{
  DWORD r = 0;
  LPTSTR buff = NULL;
  DWORD readed;
  DWORD blockSize = mf.GetDWord();

  if (blockSize >= 4)
  {
    readed = mf.ReadStringBlock(&buff);
    if (buff != NULL)
    {
      StringCchCopy(mDocTypeOid, MAX_OID, buff);
      delete[] buff;
    }
    r += readed;
  }

  if (blockSize >= 4)
  {
    if (mpDocTypeName != NULL)
    {
      delete[] mpDocTypeName;
      mpDocTypeName = NULL;
    }
    readed = mf.ReadStringBlock(&mpDocTypeName);
    r += readed;
  }

  while (blockSize > r)
  {
    DWORD tag = mf.ReadDword();
    DWORD subSize = mf.ReadDword();
    r += subSize + sizeof(DWORD) + sizeof(int);

#ifdef _DEBUG_STORAGE_BLOCK
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("ReadPos : %d, BlockSize:%d\n"), mf.GetPosition(), subSize);
    OutputDebugString(msg);
#endif

    if (tag == KMS_DOCTYPE_COLUMN) //TIFF_DOCTYPE_INFO)
    {
      EcmExtDocTypeColumn* col = new EcmExtDocTypeColumn;
      memset(col, 0, sizeof(EcmExtDocTypeColumn));
      int readed = LoadDocTypeColumn(mf, col, subSize);
      if (readed > 0)
      {
#ifdef _DEBUG
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Read column:%s, %s, %s\n"), col->name, col->key, col->valueString);
        OutputDebugString(msg);
#endif

        mArray.push_back(col);
      }
      else
        delete col;

      if (readed != subSize)
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("Read miss readed:%d, block:%d\n"), readed, subSize);
        OutputDebugString(msg);
      }
    }
  }
  return r + sizeof(DWORD);
}

DWORD KEcmDocTypeInfo::LoadDocTypeColumn(KMemoryStream& mf, EcmExtDocTypeColumn* p, int blockSize)
{
  DWORD readed = 0;

  mf.Read(&p->maxValueLength, sizeof(UINT));
  readed += sizeof(UINT);

  LPTSTR str = NULL;
  readed += mf.ReadStringBlock(&str);
  if (str != NULL)
  {
    StringCchCopy(p->key, KEYNAME_LENGTH, str);
    delete[] str;
    str = NULL;
  }

  readed += mf.ReadStringBlock(&str);
  if (str != NULL)
  {
    p->type = TypeFromString(str);
    delete[] str;
    str = NULL;
  }
  else
  {
    p->type = EcmExtDocTypeColumnType::None;
  }

  readed += mf.ReadStringBlock(&str);
  if (str != NULL)
  {
    StringCchCopy(p->name, KEYNAME_LENGTH, str);
    delete[] str;
    str = NULL;
  }

  readed += mf.ReadStringBlock(&str);
  p->valueString  = str;

  return readed;
}
