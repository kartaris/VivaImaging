#include "stdafx.h"

#include "KConstant.h"
#include "KSettings.h"
#include "KMetadataInfo.h"
#include "KEcmDocType.h"
#include "HttpDownloader/KString.h"
#include "../Document/Shape/KShapeDef.h"
#include "../Document/KImageBase.h"
#include "../Platform/Platform.h"
#include "../Platform/Utils.h"

#include <strsafe.h>


KMetadataInfo::KMetadataInfo()
{
  mDocTypeOid[0] = '\0';
}

KMetadataInfo::~KMetadataInfo()
{
  Clear(FALSE);
}

BOOL KMetadataInfo::IsDocTypeMetadata(MetaKeyNameValue* p)
{
  return ((EcmExtDocTypeColumnType::None < p->type) && (p->type < EcmExtDocTypeColumnType::MaxColumnType));
  // return (*key != '/');
}

void RationalsToString(LPTSTR msg, int len, ULARGE_INTEGER* values, int count)
{
  for (int i = 0; i < count; i++)
  {
    TCHAR str[64];

    double fv;
    if (values[i].HighPart > 0)
      fv = (double)values[i].LowPart / values[i].HighPart;
    else
      fv = (double)values[i].LowPart;
    StringCchPrintf(str, 64, _T("%.2f"), fv);
    if (i > 0)
      StringCchCat(msg, len, _T(","));
    StringCchCat(msg, len, str);
  }
}

void KMetadataInfo::dumpKeyNameValue(MetaKeyNameValue* p)
{
  if (p->type == EcmExtDocTypeColumnType::Binary)
  {
    TCHAR msg[256];

    StringCchCopy(msg, 256, p->key);
    StringCchCat(msg, 256, _T(" : "));

    int len = lstrlen(msg);
    RationalsToString(&msg[len], 256-len, (ULARGE_INTEGER*)p->value, p->len / 8);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_DEBUG);
  }
}

void KMetadataInfo::Clear(BOOL clearEditOnly)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    MetaKeyNameValue* d = *it;
    if (!clearEditOnly || IsDocTypeMetadata(d))
    {
      it = mVector.erase(it);
      ClearMetaKeyNameValue(d);
    }
    else
    {
      it++;
    }
  }
}

void KMetadataInfo::SetDocTypeInfo(LPCTSTR docTypeOid, LPCTSTR docTypeName)
{
  StringCchCopy(mDocTypeOid, MAX_OID, docTypeOid);
}

void KMetadataInfo::Add(MetaKeyNameValue* d)
{
  if (Find(d->key) == NULL)
    mVector.push_back(d);
}

void KMetadataInfo::addItem(MetaKeyNameValue* d)
{
  mVector.push_back(d);
}

BOOL KMetadataInfo::Set(LPCTSTR key, LPCTSTR value, LPCTSTR name, EcmExtDocTypeColumnType type,
  MetaDataCategory category, BOOL addNewIfNotExist)
{
  MetaKeyNameValue* p = Find(key);
  if (p != NULL)
  {
    if (p->value != NULL)
      delete[] p->value;
    p->value = AllocString(value);

    if (name != NULL)
    {
      if (p->name != NULL)
        delete[] p->name;
      p->name = AllocString(name);
    }
    return TRUE;
  }

  if (addNewIfNotExist)
  {
    MetaKeyNameValue* n = new MetaKeyNameValue;
    memset(n, 0, sizeof(MetaKeyNameValue));
    n->key = AllocString(key);
    n->name = AllocString(name);
    n->value = AllocString(value);
    n->type = type; // KEcmDocTypeInfo::TypeFromString(type);
    n->category = category;
    addItem(n);
    return TRUE;
  }
  return FALSE;
}

BOOL KMetadataInfo::Set(LPCTSTR key, LPBYTE buff, int len, WORD valueType, LPCTSTR name,
  EcmExtDocTypeColumnType type, MetaDataCategory category, BOOL addNewIfNotExist)
{
  MetaKeyNameValue* p = Find(key);
  if (p != NULL)
  {
    if (p->value != NULL)
      delete[] p->value;

    p->vtype = valueType;
    if (len > 0)
    {
      p->value = (LPTSTR)new BYTE[len];
      p->len = len;
      memcpy(p->value, buff, len);
    }
    else
    {
      p->value = NULL;
      p->len = 0;
    }

    if (name != NULL)
    {
      if (p->name != NULL)
        delete[] p->name;
      p->name = AllocString(name);
    }
    return TRUE;
  }

  if (addNewIfNotExist)
  {
    MetaKeyNameValue* n = new MetaKeyNameValue;
    memset(n, 0, sizeof(MetaKeyNameValue));
    n->key = AllocString(key);
    n->name = AllocString(name);
    if (len > 0)
    {
      n->value = (LPTSTR)new BYTE[len];
      n->len = len;
      memcpy(n->value, buff, len);
    }
    else
    {
      n->value = NULL;
      n->len = 0;
    }
    n->vtype = valueType;
    n->type = type;
    n->category = category;
    dumpKeyNameValue(n);
    addItem(n);
    return TRUE;
  }
  return FALSE;
}

MetaKeyNameValue* KMetadataInfo::Find(LPCTSTR key)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*it;

    if (CompareStringNC(p->key, key) == 0)
    {
      return p;
    }
    ++it;
  }
  return NULL;
}

BOOL KMetadataInfo::Remove(LPCTSTR key)
{
  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*it;

    if (CompareStringNC(p->key, key) == 0)
    {
      ClearMetaKeyNameValue(p);
      it = mVector.erase(it);
      return TRUE;
    }
    ++it;
  }
  return FALSE;
}

void KMetadataInfo::InitMetaKeyNameValue(MetaKeyNameValue* p)
{
  memset(p, 0, sizeof(MetaKeyNameValue));
  p->type = EcmExtDocTypeColumnType::None;
}

void KMetadataInfo::ClearMetaKeyNameValue(MetaKeyNameValue* p)
{
  if (p->key != NULL)
    delete[] p->key;
  if (p->name != NULL)
    delete[] p->name;
  if (p->value != NULL)
    delete[] p->value;
  delete p;
}

PMetaKeyNameValue KMetadataInfo::CopyMetaKeyNameValue(MetaKeyNameValue* p)
{
  MetaKeyNameValue* n = new MetaKeyNameValue;
  if (n != NULL)
  {
    n->key = AllocString(p->key);
    n->name = AllocString(p->name);
    n->type = p->type;
    n->category = p->category;
    n->vtype = p->vtype;
    n->len = p->len;
    if (p->type == EcmExtDocTypeColumnType::Binary)
    {
      n->value = (LPTSTR)new BYTE[p->len];
      memcpy(n->value, p->value, p->len);
    }
    else
    {
      n->value = AllocString(p->value);
    }
  }
  return n;
}

#ifdef USE_XML_STORAGE
LPSTR KMetadataInfo::StoreToString()
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

HRESULT KMetadataInfo::StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly)
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

  if (FAILED(hr))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    MetaKeyNameValue* p = *(it++);
    StoreMetaKeyNameValue(pWriter, p);
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

HRESULT KMetadataInfo::StoreMetaKeyNameValue(CComPtr<IXmlWriter> pWriter, MetaKeyNameValue* p)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagExtDocTypeColumn, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  TCHAR str[32];
  KEcmDocTypeInfo::TypeToString(p->type, str, 32);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnType, NULL, str);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnKey, NULL, p->key);
  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnName, NULL, p->name);

  if (SUCCEEDED(hr))
    hr = pWriter->WriteAttributeString(NULL, tagExtDocTypeColumnValue, NULL, p->value);

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

void ClearKeyNameValueArray(KVoidPtrArray& list)
{
  KVoidPtrIterator it = list.begin();
  while (it != list.end())
  {
    PMetaKeyNameValue p = (PMetaKeyNameValue)*it;
    KMetadataInfo::ClearMetaKeyNameValue(p);
    ++it;
  }
  list.clear();
}

DWORD KMetadataInfo::loadMetaKeyNameValueFromBuffer(KMemoryStream& mf, int blockSize, MetaKeyNameValue* p)
{
  // category는 저장되지 않음.
  p->category = MetaDataCategory::None;
  p->type = (EcmExtDocTypeColumnType) mf.ReadInt();
  int readed = sizeof(int);

  LPTSTR buff = NULL;

  readed += mf.ReadStringBlock(&buff);
  if (buff != NULL)
  {
    p->key = buff;
    buff = NULL;
  }
  blockSize -= readed;

  readed += mf.ReadStringBlock(&buff);
  if (buff != NULL)
  {
    p->name = buff;
    buff = NULL;
  }
  blockSize -= readed;

  readed += mf.ReadStringBlock(&buff);
  if (buff != NULL)
  {
    p->value = buff;
    buff = NULL;
  }
  return readed;
}

DWORD KMetadataInfo::LoadFromBuffer(KMemoryStream& mf)
{
  LPTSTR buff = NULL;
  DWORD readed = 0;
  DWORD blockSize = mf.GetDWord();

  if (blockSize >= 4)
  {
    readed = mf.ReadStringBlock(&buff);
    if (buff != NULL)
    {
      StringCchCopy(mDocTypeOid, MAX_OID, buff);
      delete[] buff;
    }
  }

  while (blockSize > readed)
  {
    DWORD tag = mf.ReadDword();
    int subSize = mf.ReadInt();
    readed += sizeof(DWORD) + sizeof(int);

#ifdef _DEBUG_STORAGE_BLOCK
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("ReadPos : %d, BlockSize:%d\n"), mf.GetPosition(), subSize);
    OutputDebugString(msg);
#endif

    if (tag == KMS_NAMEVALUE_ITEM) //TIFF_METADATA_VALUE)
    {
      MetaKeyNameValue* p = new MetaKeyNameValue;
      memset(p, 0, sizeof(MetaKeyNameValue));
      int r = loadMetaKeyNameValueFromBuffer(mf, subSize, p);

      if (r != subSize)
      {
        TCHAR msg[128];
        StringCchPrintf(msg, 128, _T("MetaKeyNameValue reading miss, readed:%d, block:%d\n"), r, subSize);
        OutputDebugString(msg);
      }

      readed += r;
      mVector.push_back(p);
    }
    else
    {
      TCHAR msg[256];
      StringCchPrintf(msg, 256, _T("Unknown tag 0X%x"), tag);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      return 0;
    }
  }

  return readed + sizeof(DWORD);
}

DWORD KMetadataInfo::storeMetaKeyNameValue(KMemoryStream& mf, MetaKeyNameValue* p)
{
  mf.WriteDword(KMS_NAMEVALUE_ITEM); // TIFF_METADATA_VALUE);
  mf.WriteDword(0); // block size
  DWORD startPos = mf.Size();

  mf.WriteInt((int)p->type);

  mf.WriteStringBlock(p->key);
  mf.WriteStringBlock(p->name);
  mf.WriteStringBlock(p->value);

  // reset block size
  DWORD blockSize = mf.UpdateBlockSize(startPos);
  return (blockSize + sizeof(DWORD) + sizeof(DWORD));
}

DWORD KMetadataInfo::StoreToBuffer(KMemoryStream& mf)
{
  mf.WriteDword(KMS_METADATA_GROUP); // TIFF_DOCTYPE_INFO);
  mf.WriteDword(0); // block size
  DWORD startPos = mf.Size();

  mf.WriteStringBlock(mDocTypeOid);

  KMetadataVectorIterator it = mVector.begin();
  while (it != mVector.end())
  {
    MetaKeyNameValue* p = *(it++);
    storeMetaKeyNameValue(mf, p);
  }

  // reset block size
  DWORD blockSize = mf.UpdateBlockSize(startPos);
  return (blockSize + sizeof(DWORD) + sizeof(DWORD));
}
