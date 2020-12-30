#include "stdafx.h"
#include "KShapeContainer.h"
#include "../Core/KSettings.h"

KShapeContainer::KShapeContainer()
{
  Width = 0;
  Height = 0;
}

KShapeContainer::~KShapeContainer()
{
  Clear();
}

void KShapeContainer::Clear()
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    delete s;
    it++;
  }
  mShapeList.clear();
}

void KShapeContainer::ClearList()
{
  mShapeList.clear();
}

void KShapeContainer::SetSize(int w, int h)
{
  Width = w;
  Height = h;
}

void KShapeContainer::AddShape(KShapeBase* s)
{
  mShapeList.push_back(s);
}

/**
* @brief 지정한 index에 해당하는 개체를 찾아 리턴한다.
* @param index : 찾을 대상 개체의 index
*/
KShapeBase* KShapeContainer::GetObjectByIndex(int index)
{
  KShapeIterator it = mShapeList.begin();

  if (index > 0)
    it += index;
  KShapeBase* s = *it;
  return s;
}

/**
* @brief 지정한 object_id에 해당하는 개체를 찾아 리턴한다.
* @param object_id : 찾을 대상 개체의 id
*/
KShapeBase* KShapeContainer::GetObject(int object_id)
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->ObjectId == object_id)
      return s;
    it++;
  }
  return NULL;
}

int KShapeContainer::RemoveObjectsById(KIntVector& objectId)
{
  int count = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (DoesListContainValue(objectId, s->ObjectId))
    {
      it = mShapeList.erase(it);
      delete s;
      ++count;
    }
    else
    {
      it++;
    }
  }
  return count;
}


/**
* @brief 지정한 id의 개체를 스프레드 Graphic 목록에서 제거하고 개체를 리턴한다.
* @param child_id : 개체의 ID
* @return Graphic : 스프레드에서 제거된 개체
*/
KShapeBase* KShapeContainer::DetachChildObject(int child_id)
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (child_id == s->ObjectId)
    {
      mShapeList.erase(it);
      return s;
    }
    it++;
  }
  return NULL;
}

KShapeBase* KShapeContainer::DetachChildObject(KShapeBase* obj)
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (obj == s)
    {
      mShapeList.erase(it);
      return s;
    }
    it++;
  }
  return NULL;
}

void KShapeContainer::Draw(Graphics& g, KImageDrawer& info, int flag)
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible() && s->HitTest(info))
      s->Draw(g, info, flag);
  }
}

int KShapeContainer::GetObjectsBound(KShapeVector& objs, RECT& area)
{
  int count = 0;
  KShapeIterator it = objs.begin();
  while (it != objs.end())
  {
    KShapeBase* s = *it++;
    if (s->IsVisible())
    {
      RECT rect = s->GetBounds(NULL);
      if (count == 0)
        memcpy(&area, &rect, sizeof(RECT));
      else
        AddRect(area, rect);
      count++;
    }
  }
  return count;
}

int KShapeContainer::GetSelectedItems(KShapeContainer& container, BOOL bRemove)
{
  int index = 0;
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it;
    if (s->IsVisible() && !s->IsLocked() && s->IsSelected())
    {
      s->SetChildIndex(index);
      container.AddShape(s);

      if (bRemove)
      {
        it = mShapeList.erase(it);
      }
      else
      {
        ++it;
      }
    }
    else
    {
      ++it;
    }
    ++index;
  }
  return index;
}

#ifdef USE_XML_STORAGE
HRESULT KShapeContainer::Store(CComPtr<IXmlWriter> pWriter)
{
  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it++;
    HRESULT hr = s->Store(pWriter);
    if (FAILED(hr))
      return hr;
  }
  return S_OK;
}
#else
DWORD KShapeContainer::Load(KMemoryStream& ms)
{
  return LoadFromBufferBody(ms);
}
DWORD KShapeContainer::Store(KMemoryStream& ms)
{
  return StoreToBuffer(ms);
}
#endif // USE_XML_STORAGE

DWORD KShapeContainer::StoreToBuffer(KMemoryStream& mf)
{
  mf.WriteDword(KMS_ANNOTATE_GROUP); // TIFF_ANNOTATE_BLOCK);
  mf.WriteDword(0); // block size
  DWORD pos = mf.Size();

  // mf.WriteDword(mShapeList.size());

  KShapeIterator it = mShapeList.begin();
  while (it != mShapeList.end())
  {
    KShapeBase* s = *it++;
    if (!s->StoreToBuffer(mf))
      break;
  }

  DWORD size = mf.UpdateBlockSize(pos);

  return size + sizeof(DWORD) * 2;
}

DWORD KShapeContainer::LoadFromBuffer(KMemoryStream& mf)
{
  DWORD dw = mf.GetDWord();
  if (dw != KMS_ANNOTATE_GROUP)
  {
    StoreLogHistory(_T(__FUNCTION__), _T("Invalid ShapeContainer"), SYNC_MSG_LOG | SYNC_MSG_DEBUG);
    return FALSE;
  }
  int readed = sizeof(DWORD);

  readed += LoadFromBufferBody(mf);
  return readed;
}

DWORD KShapeContainer::LoadFromBufferBody(KMemoryStream& mf, KVoidPtrArray* pShapeList)
{
  DWORD blockSize = mf.GetDWord();
  DWORD readed = 0;

  while (blockSize > readed)
  {
    int readSize = 0;
    KShapeBase* s = KShapeBase::LoadFromBuffer(mf, &readSize);
    readed += readSize;

    if (s != NULL)
    {
      mShapeList.push_back(s);
      if (pShapeList != NULL)
        pShapeList->push_back(s);
    }
    else
      break;
  }

  if (blockSize != readed)
  {
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("size mismatch(headCount=%d, readed=%d)"), blockSize, readed);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
  }
  return readed + sizeof(DWORD);
}

