#include "stdafx.h"
#include "KImageFolder.h"
#include "KImagePage.h"

int KImageFolder::g_DefaultFolderNameIndex = 1;

KImageFolder::KImageFolder()
  : KImageBase(NULL)
{
  mPermission |= SFP_INSERT_CONTAINER | SFP_INSERT_ELEMENT | SFP_LOOKUP_ELEMENT | SFP_READ_ELEMENT | SFP_WRITE_ELEMENT | SFP_DELETE_ELEMENT;
}

KImageFolder::KImageFolder(KImageBase* p)
  : KImageBase(p)
{
  mPermission |= SFP_INSERT_CONTAINER | SFP_INSERT_ELEMENT | SFP_LOOKUP_ELEMENT | SFP_READ_ELEMENT | SFP_WRITE_ELEMENT | SFP_DELETE_ELEMENT;
}

KImageFolder::~KImageFolder()
{
  Clear();
}


void KImageFolder::Clear()
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it;
    delete page;
    it++;
  }
  m_Childs.clear();
  KImageBase::Clear();
}

int KImageFolder::GetNumberOfPages(int flag)
{
  int pages = ((IMAGE_ONLY & flag) || (mParent == NULL)) ? 0 : 1;

  if ((EXCLUDE_FOLDED & flag) && IsFolded())
    return pages;

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it++;
    if (EXCLUDE_CHILD_FOLDER & flag)
    {
      if (IMAGE_ONLY & flag)
      {
        if (page->GetType() == IMAGE_PAGE)
          pages++;
      }
      else
      {
        pages++;
      }
    }
    else
    {
      pages += page->GetNumberOfPages(flag);
    }
  }
  return pages;
}

LPTSTR KImageFolder::GetImageFolderPath()
{
  KImageBase* p = GetParent();
  if (p != NULL)
  {
    LPTSTR path = NULL;
    if (p->mParent != NULL) // parent is root item
    {
      path = p->GetImageFolderPath();
    }
    path = AllocPathName(path, mName);
    return path;
  }
  return NULL;

}

BOOL KImageFolder::IsLastChild(KImageBase* p)
{
  KImageDocRevIterator it = m_Childs.rbegin();
  return (*it == p);
}

int KImageFolder::GetPageList(KImageDocVector& imagePages, int flag) // IMAGE_ONLY | EXCLUDE_CHILD_FOLDER, SELECTED_ONLY;
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it++;

    if (!(SELECTED_ONLY & flag) || page->IsSelected())
    {
      if ((IMAGE_ONLY & flag) && (page->GetType() == IMAGE_PAGE)) 
      {
        imagePages.push_back(page);
      }
      if ((FOLDER_ONLY & flag) && (page->GetType() == FOLDER_PAGE))
      {
        imagePages.push_back(page);
      }
    }

    if (!(EXCLUDE_CHILD_FOLDER & flag) &&
      (page->GetType() == FOLDER_PAGE))
    {
      ((KImageFolder*)page)->GetPageList(imagePages, flag);
    }
  }
  return (int)imagePages.size();
}

int KImageFolder::MoveChildTo(KImageFolder& d)
{
  int count = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it++;
    d.InsertPage(page, (KImageBase*)NULL);
    count++;
    if (page->GetType() == FOLDER_PAGE)
      count += ((KImageFolder*)page)->MoveChildTo(d);
  }
  m_Childs.clear();
  return count;
}

KImageBase* KImageFolder::FindPageByName(LPCTSTR name, int type)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it++;
    if ((CompareStringNC(page->mName, name) == 0) &&
      ((type == 0) || (type == page->GetType())))
      return page;
  }
  return NULL;
}

KImageBase* KImageFolder::FindPageByID(UINT id)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it++;
    if (page->mID == id)
      return page;
    if (page->GetType() == FOLDER_PAGE)
    {
      page = ((KImageFolder*)page)->FindPageByID(id);
      if (page != NULL)
        return page;
    }
  }
  return NULL;
}

void KImageFolder::SetModified(BOOL bRecurr, PageModifiedFlag modified)
{
  KImageBase::SetModified(bRecurr, modified);
  if (bRecurr)
  {
    KImageDocIterator it = m_Childs.begin();
    while (it != m_Childs.end())
    {
      KImageBase* page = *it++;
      page->SetModified(bRecurr, modified);
    }
  }
}

// folderOID + '|' + fullPathIndex
BOOL KImageFolder::GetEcmInfo(LPTSTR folderOID, LPTSTR* pFullPathIndex)
{
  if (mpEcmInfo != NULL)
  {
    LPCTSTR s = StrChr(mpEcmInfo, '|');
    if (s != NULL)
    {
      size_t len = s - mpEcmInfo;
      StringCchCopyN(folderOID, MAX_OID, mpEcmInfo, len);
      s++;

      if (pFullPathIndex != NULL)
      {
        *pFullPathIndex = AllocString(s);
      }
      return TRUE;
    }
  }
  return FALSE;
}

KImageBase* KImageFolder::GetIndexedPage(int index, int flag)
{
  // IMAGE_ONLY이거나 루트 항목은 건너뛴다.
  /*
  if (!(IMAGE_ONLY & flag) && (mParent != NULL))
  {
    if (index == 0)
      return this;
    index--;
  }
  */

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;
    if ((index == 0) &&
      (!(IMAGE_ONLY & flag) || (p->GetType() == IMAGE_PAGE)) )
    {
      return p;
    }

    if(!(IMAGE_ONLY & flag) || (p->GetType() == IMAGE_PAGE))
      index--;

    if (p->GetType() == FOLDER_PAGE)
    {
      if (!(EXCLUDE_FOLDED & flag) || !p->IsFolded())
      {
        KImageBase* n = ((KImageFolder *)p)->GetIndexedPage(index, flag);
        if (n != NULL)
          return n;
        else
          index -= ((KImageFolder *)p)->GetNumberOfPages(flag) - 1;
      }
    }
    it++;
  }
  return NULL;
}

int KImageFolder::GetIndexOfPage(KImageBase* page, int flag)
{
  int index = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;
    if (page == p)
    {
      return index;
    }

    if ((p->GetType() != FOLDER_PAGE) || !(IMAGE_ONLY & flag))
      index++;

    if ((p->GetType() == FOLDER_PAGE) &&
      (!(EXCLUDE_FOLDED & flag) || !p->IsFolded()))
    {
      int n = ((KImageFolder *)p)->GetIndexOfPage(page, flag);
      if (n >= 0)
        return (index + n);
      else
        index += ((KImageFolder *)p)->GetNumberOfPages(flag) - 1;
    }

    it++;
  }
  return -1;
}

int KImageFolder::IsChildOf(KImageBase* page)
{
  int index = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    if (page == p)
    {
      return index;
    }
    index++;
  }
  return -1;
}

BOOL KImageFolder::IsLastOf(KImageBase* page)
{
  KImageDocRevIterator it = m_Childs.rbegin();

  if (it != m_Childs.rend())
  {
    KImageBase* p = *it;

    if (p == page)
      return TRUE;

    if ((p->GetType() & FOLDER_PAGE) && ((KImageFolder*)p)->IsLastOf(page))
      return TRUE;

  }
  return FALSE;
}

void KImageFolder::ClearThumbImage(BOOL bRecurr)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    p->ClearThumbImage(bRecurr);
  }
}

void KImageFolder::InsertPage(KImageBase* page, KImageBase* afterPage)
{
  if (afterPage != NULL)
  {
    KImageDocIterator it = m_Childs.begin();
    while (it != m_Childs.end())
    {
      KImageBase* p = *it;
      if (p == afterPage)
      {
        it++;
        m_Childs.insert(it, page);
        return;
      }
      it++;
    }
  }

  // else end of list
  m_Childs.push_back(page);
  // set parent
  page->SetParent(this);
}

void KImageFolder::InsertPage(KImageBase* page, int index)
{
  if ((index >= 0) && (index < (int)m_Childs.size()))
  {
    KImageDocIterator it = m_Childs.begin();
    if (index > 0)
      it += index;
    m_Childs.insert(it, page);
  }
  else
  {
    m_Childs.push_back(page);
  }
  // set parent
  page->SetParent(this);
}

BOOL KImageFolder::RemovePage(KImageBase* p)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it;
    if (page == p)
    {
      m_Childs.erase(it);
      return TRUE;
    }

    if (page->GetType() == FOLDER_PAGE)
    {
      BOOL r = ((KImageFolder*)page)->RemovePage(p);
      if (r)
        return TRUE;
    }

    it++;
  }
  return FALSE;
}

/*
* p[0] : previous page
* p[1] : finding page
* p[2] : previous page
* s : 0=not found, 1= found
*/
BOOL KImageFolder::NextOrPreviousPage(KImageBase* p[], int s, int flag)
{
  if (s == 0)
    p[0] = this;

  if (s == 1)
  {
    p[2] = this;
    return TRUE;
  }

  if ((EXCLUDE_FOLDED & flag) && IsFolded())
    return FALSE;

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* page = *it;
    if (s == 0)
    {
      if (page == p[1])
        s = 1;
      else
        p[0] = page; // update last
    }
    else if ((s == 1) && (p[2] == NULL))
    {
      p[2] = page;
      return TRUE;
    }

    if ((page->GetType() == FOLDER_PAGE) && !page->IsFolded())
    {
      BOOL r = ((KImageFolder*)page)->NextOrPreviousPage(p, s, flag);
      if (r)
        return TRUE;
    }

    it++;
  }
  return (s > 0);
}

void KImageFolder::CheckReadyFolderPage(KImageDocVector* pPageList)
{
  // move to temp list
  KImageDocVector pages;
  KImageDocIterator it = pPageList->begin();
  while (it != pPageList->end())
  {
    KImageBase* page = *it++;
    pages.push_back(page);
    RemovePage(page);
  }
  pPageList->clear();

  // ordering
  it = pages.begin();
  while (it != pages.end())
  {
    KImageBase* page = *it++;

    if (page->mpParentKeyName != NULL)
    {
      KImageFolder* f = GetPageFromParentKeyName(page->mpParentKeyName, FALSE);
      if ((f != NULL) && (f != page->GetParent()))
      {
        if (f->GetChildCount() == 0)
          pPageList->push_back(f);

        f->InsertPage(page, -1); // insert at tail
        page = NULL;
      }
    }

    if (page != NULL)
    {
      m_Childs.push_back(page);
      pPageList->push_back(page);
    }
  }
  pages.clear();
}

KImageFolder* KImageFolder::GetPageFromParentKeyName(LPCTSTR parentKeyName, bool CreateIfNotExist)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;
    if (p->GetType() == FOLDER_PAGE)
    {
      if ((p->mpParentKeyName != NULL) &&
        (CompareStringNC(p->mpParentKeyName, parentKeyName) == 0))
        return (KImageFolder*)p;
    }
    it++;
  }

  // if not found, create new folder if this is root
  if (CreateIfNotExist) // mParent == NULL)
  {
    KImageFolder* folder = new KImageFolder(this);

    folder->SetParentKeyName(parentKeyName);
    m_Childs.push_back(folder);

    LPTSTR e = StrRChr(parentKeyName, NULL, DA_SLASH_SEPARATOR);
    if (e != NULL)
    {
      *e = NULL;
      folder->SetName(parentKeyName);
      *e = DA_SLASH_SEPARATOR;
    }
    else
    {
      folder->SetName(parentKeyName);
    }
    return folder;
  }
  return NULL;
}


BOOL KImageFolder::SetSelectedRecursive(BOOL set)
{
  BOOL changed = (IsSelected() != set);

  SetSelected(set);
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;
    if (p->SetSelectedRecursive(set))
      changed = TRUE;
    it++;
  }
  return changed;
}

int KImageFolder::SelectPageRange(int& index, int p1, int p2, int flag)
{
  int count = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;
    int asFolder = (p->GetType() == FOLDER_PAGE);

    if (asFolder)
    {
      // 닫힌 폴더도 하나로
      if ((p->GetType() == FOLDER_PAGE) && (EXCLUDE_FOLDED & flag) && p->IsFolded())
        asFolder = 3;
    }

    if ((p1 <= index) && (index <= p2))
    {
      if (asFolder & 2) // folded folder
        p->SetSelectedRecursive(TRUE);
      else
        p->SetSelected(TRUE);
      count++;
    }
    else if (flag & SELECT_EXCLUSIVE)
    {
      p->SetSelectedRecursive(FALSE);
    }
    index++;

    if (asFolder == 1) // unfolded folder
    {
      count += ((KImageFolder *)p)->SelectPageRange(index, p1, p2, flag);
    }
    it++;
  }
  return count;
}

/*
return true if selection changed
*/
BOOL KImageFolder::SelectAllPages(BOOL bSelect)
{
  BOOL changed = FALSE;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    if (p->SetSelectedRecursive(bSelect))
      changed = TRUE;
  }
  return changed;
}

int KImageFolder::DeleteSelectedPages()
{
  int count = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;

    if (p->IsSelected())
    {
      it = m_Childs.erase(it);
      count += p->GetNumberOfPages(0);
      delete p;
      // pages.push_back(p);
    }
    else
    {
      if (p->GetType() == FOLDER_PAGE)
        count += ((KImageFolder*)p)->DeleteSelectedPages();
      it++;
    }
  }

  return count;
}

int KImageFolder::DetachSelectedPages(KVoidPtrArray& pages)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;

    if (p->IsSelected())
    {
      it = m_Childs.erase(it);
      pages.push_back(p);
    }
    else
    {
      if (p->GetType() == FOLDER_PAGE)
        ((KImageFolder*)p)->DetachSelectedPages(pages);
      ++it;
    }
  }

  return (int)pages.size();
}

KImageBase* KImageFolder::DetachPageById(int id)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;

    if (p->GetID() == id)
    {
      it = m_Childs.erase(it);
      return p;
    }
    ++it;
  }
  return NULL;
}


int KImageFolder::DeletePagesById(KIntVector& pageIdList)
{
  int count = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it;

    if (DoesListContainValue(pageIdList, p->mID))
    {
      it = m_Childs.erase(it);
      count += p->GetNumberOfPages(0);
      delete p;
    }
    else
    {
      if (p->GetType() == FOLDER_PAGE)
        count += ((KImageFolder*)p)->DeletePagesById(pageIdList);
      it++;
    }
  }
  return count;
}

BOOL KImageFolder::InsertPages(int targetIndex, KImageBase* at, InsertTargetType inside, KVoidPtrArray& pages)
{
  BOOL rtn = FALSE;
  BOOL target_found = FALSE;
  int current_depth = GetDepth();
  /*
  KImageDocIterator it = m_Childs.begin();
  if ((at == NULL) && (targetIndex < 0))
  {
    KVoidPtrIterator sit = pages.begin();
    while (sit != pages.end())
    {
      KImageBase* sp = (KImageBase*)*sit;
      if ((current_depth == 0) || (sp->GetType() == IMAGE_PAGE))
      {
        sit = pages.erase(sit);

        sp->SetParent(this);
        it = m_Childs.insert(it, sp);
        rtn = TRUE;
      }
      else
      {
        sit++;
      }
    }
  }

  if (pages.size() == 0)
    return rtn;
  */

  KImageDocIterator it = m_Childs.begin();
  if (at != NULL)
  {
    while (it != m_Childs.end())
    {
      KImageBase* p = *it;
      if (p == at)
      {
        if (inside == InsertTargetType::TARGET_NEXT)
          it++;
        target_found = TRUE;
        break;
      }
      if (p->GetType() == FOLDER_PAGE)
      {
        if (((KImageFolder*)p)->InsertPages(targetIndex, at, inside, pages))
        {
          rtn = TRUE;
          break;
        }
      }
      it++;
    }
  }
  else if (inside == InsertTargetType::TARGET_INSIDE)
  {
    if (targetIndex == 0) // insert at front
      it = m_Childs.begin();
    else
      it = m_Childs.end();
    target_found = TRUE;
  }
  else
  {
    it = m_Childs.end();
  }

  if (target_found || (current_depth == 0))
  {
    // 남은것은 여기서 다 넣는다.
    KVoidPtrIterator sit = pages.begin();
    while (sit != pages.end())
    {
      KImageBase* sp = (KImageBase*)*sit;

      // 폴더 안에 폴더가 안 들어가게.
      if ((current_depth == 0) || (sp->GetType() == IMAGE_PAGE))
      {
        sit = pages.erase(sit);

        sp->SetParent(this);

        if (it == m_Childs.end())
        {
          m_Childs.push_back(sp);
          it = m_Childs.end();
        }
        else
        {
          it = m_Childs.insert(it, sp);
          it++;
        }
        rtn = TRUE;
      }
      else
      {
        sit++;
      }
    }
  }
  return rtn;
}

int KImageFolder::StorePageIds(int* buffer, int count, int flag)
{
  int stored = 0;
  if ((count > 0) && !(IMAGE_ONLY & flag))
  {
    if (buffer != NULL)
      *buffer++ = GetID();
    stored++;
  }

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    if ((p->GetType() == FOLDER_PAGE) &&
      (!(EXCLUDE_FOLDED & flag) || !p->IsFolded()))
      {
      int s = ((KImageFolder*)p)->StorePageIds(buffer, count, flag);
      if (s > 0)
      {
        if (buffer != NULL)
        {
          buffer += s;
          *buffer++ = 0; // end of folder flag
        }
        stored += s + 1; // add end of folder flag
      }
    }
    else
    {
      if ((count > 0) &&
        (!(IMAGE_ONLY & flag) || (p->GetType() != FOLDER_PAGE)))
      {
        if (buffer != NULL)
          *buffer++ = p->GetID();
        stored++;
      }
    }
  }

  return stored;
}

int KImageFolder::RestorePageOrdering(int* pageIds, int count)
{
  KImageFolder tempPage;
  MoveChildTo(tempPage);

  // root id
  ASSERT(*pageIds == GetID());
  pageIds++;
  count--;

  KImageFolder* parent = this;

  while (count > 0)
  {
    if (*pageIds == 0) // end of child
    {
      pageIds++;
      count--;
    
      if ((parent != NULL) && (parent != this))
      {
        parent = (KImageFolder*)parent->GetParent();
      }
      else
      {
        StoreLogHistory(_T(__FUNCTION__), _T("Overflow Parent"), SYNC_EVENT_ERROR | SYNC_MSG_LOG);
      }
    }

    KImageBase* p = tempPage.DetachPageById(*pageIds);

    if (p == NULL)
    {
      TCHAR msg[128];
      StringCchPrintf(msg, 128, _T("Unknown page id=%d"), *pageIds);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    }
    pageIds++;
    count--;

    if (p != NULL)
    {
      parent->InsertPage(p, (KImageBase*)NULL);
      if (p->GetType() == FOLDER_PAGE)
        parent = (KImageFolder*)p;
    }
  }
  return count;
}

int KImageFolder::MergeAnnotateObjects()
{
  int r = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    int count = p->MergeAnnotateObjects();
    if (p->GetType() == FOLDER_PAGE)
      r += count;
    else if (count > 0)
      r++;
  }
  return r;
}

int KImageFolder::ApplyEffect(ImageEffectJob* effect, BOOL bAll)
{
  int r = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    if (bAll || p->IsSelected() || (p->GetType() == FOLDER_PAGE))
      r |= p->ApplyEffect(effect, bAll);
  }
  return r;
}

#ifdef USE_XML_STORAGE
HRESULT KImageFolder::ReadAttributes(IXmlReader* pReader)
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
      /*
      if (cwchPrefix > 0)
        wprintf(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue);
      else
        wprintf(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
      */
      if (CompareStringNC(pwszLocalName, tagName) == 0)
      {
        if (mName != NULL)
          delete[] mName;
        mName = AllocString(pwszValue);
      }
    }

    hr = pReader->MoveToNextAttribute();
  }
  return S_OK;
}

HRESULT KImageFolder::ReadDocument(CComPtr<IXmlReader> pReader, KImageDocVector* newPages, int flag)
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
  const WCHAR* pwszValue;
  UINT cwchPrefix;
  MetaKeyNameValue* m = NULL;

  while (S_OK == (hr = pReader->Read(&nodeType)))
  {
    switch (nodeType)
    {

    case XmlNodeType_XmlDeclaration:
      if (FAILED(hr = ReadAttributes(pReader)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      break;

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
      /*
      if (cwchPrefix > 0)
        wprintf(L"Element: %s:%s\n", pwszPrefix, pwszLocalName);
      else
        wprintf(L"Element: %s\n", pwszLocalName);
      */
      if (CompareStringNC(pwszLocalName, tagFolder) == 0)
      {
        KImageFolder* child = new KImageFolder(this);
        if (newPages != NULL)
          newPages->push_back(child);
        hr = child->ReadDocument(pReader, newPages, flag);
        if (hr != S_OK)
          return hr;
        InsertPage(child, -1);
      }
      else if (CompareStringNC(pwszLocalName, tagImage) == 0)
      {
        KImagePage* child = new KImagePage(this);
        if (newPages != NULL)
          newPages->push_back(child);
        hr = child->ReadDocument(pReader, NULL, flag);
        if (hr != S_OK)
          return hr;
        InsertPage(child, -1);
      }
      else if (CompareStringNC(pwszLocalName, tagMetaData) == 0)
      {
        m = new MetaKeyNameValue;
        KMetadataInfo::InitMetaKeyNameValue(m);

        hr = ReadMetaData(pReader, m);
        if (FAILED(hr))
        {
          delete m;
          return hr;
        }
        else
        {
          mMetadataContainer.Add(m);
          m = NULL;
        }
      }
      else
      {
        if (FAILED(hr = ReadAttributes(pReader)))
        {
          TCHAR msg[256];
          StringCchPrintf(msg, 256, _T("Error reading attributes, error is %08.8lx"), hr);
          StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
          return hr;
        }
      }

      if (FAILED(hr = pReader->MoveToElement()))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error moving to the element that owns the current attribute node, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      /*
      if (pReader->IsEmptyElement())
        wprintf(L" (empty element)\n");
      */
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
      /*
      if (cwchPrefix > 0)
        wprintf(L"End Element: %s:%s\n", pwszPrefix, pwszLocalName);
      else
        wprintf(L"End Element: %s\n", pwszLocalName);
      */
      return S_OK;
      break;

    case XmlNodeType_Text:

    case XmlNodeType_Whitespace:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Text, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Whitespace text: >%s<\n", pwszValue);
      break;

    case XmlNodeType_CDATA:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on CDATA, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"CDATA: %s\n", pwszValue);
      break;

    case XmlNodeType_ProcessingInstruction:
      if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting name on ProcssingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on ProcessingInstruction, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Processing Instruction name:%S value:%S\n", pwszLocalName, pwszValue);
      break;

    case XmlNodeType_Comment:
      if (FAILED(hr = pReader->GetValue(&pwszValue, NULL)))
      {
        TCHAR msg[256];
        StringCchPrintf(msg, 256, _T("Error getting value on Comment, error is %08.8lx"), hr);
        StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
        return hr;
      }
      //wprintf(L"Comment: %s\n", pwszValue);
      break;
    } // end of switch
  } // end of while
  return S_OK;
}

int KImageFolder::ValidateMetadatas(KEcmDocTypeInfo& info)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    p->ValidateMetadatas(info);
  }
  return (int)m_Childs.size();
}


HRESULT KImageFolder::StoreDocument(CComPtr<IXmlWriter> pWriter, BOOL selectedOnly)
{
  HRESULT hr;

  if (FAILED(hr = pWriter->WriteStartElement(NULL, tagFolder, NULL)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteStartElement, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }
  if (FAILED(hr = pWriter->WriteAttributeString(NULL, tagName, NULL,
    mName)))
  {
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Error, Method: WriteAttributeString, error is %08.8lx"), hr);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);
    return hr;
  }

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    if (!selectedOnly || p->IsSelected())
    {
      // update last-modified if changed
      if (p->IsModified())
      {
        p->InitMetaDatas(FALSE);
      }

      hr = p->StoreDocument(pWriter, selectedOnly);
      if (hr != S_OK)
        return hr;
    }
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
#else
DWORD KImageFolder::LoadMetaData(KMemoryStream& ms)
{
  DWORD blockSize = ms.ReadDword();
  DWORD readed = 0;

  while (readed < blockSize)
  {
    DWORD id = ms.ReadDword();
    readed += sizeof(DWORD) * 2;

    KImageBase* p = FindPageByID(id);
    if (p != NULL)
    {
      p->ClearMetaData(TRUE);

      DWORD tag = ms.ReadDword();
      if (tag == KMS_METADATA_GROUP)
      {
        readed += p->LoadMetaData(ms);
      }
      else
      {
        DWORD size = ms.ReadDword();
        if (size > 0)
          ms.SeekRel(size);
        readed += size + sizeof(DWORD);
      }
    }
    else
    {
      TCHAR msg[128];
      StringCchPrintf(msg, 128, _T("Cannot found page ID=%d"), (int)id);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_EVENT_ERROR | SYNC_MSG_LOG);

      DWORD tag = ms.ReadDword();
      DWORD size = ms.ReadDword();
      if (size > 0)
        ms.SeekRel(size);
      readed += size + sizeof(DWORD);
    }

  }
  return readed;
}

DWORD KImageFolder::SaveMetaData(KMemoryStream& ms)
{
  ms.WriteDword(KMS_FOLDER_ITEM);
  ms.WriteDword(0);

  int startPos = ms.GetPosition();
  DWORD blockSize = sizeof(DWORD) * 2;

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    ms.WriteDword(p->GetID());
    DWORD size = p->SaveMetaData(ms);
    blockSize += size;
  }
  ms.UpdateBlockSize(startPos);
  return blockSize;
}

DWORD KImageFolder::ReadDocument(KMemoryStream& ms, KImageDocVector* newPages, int flag)
{
  DWORD blockSize = ms.ReadDword();
  DWORD readed = 0;

  LPTSTR str = NULL;
  ms.ReadStringBlock(&str);
  if (str != NULL)
    mName = str;

  while (blockSize > readed)
  {
    DWORD tag = ms.ReadDword();
    DWORD size = 0;

    if (tag == KMS_FOLDER_ITEM)
    {
      KImageFolder* child = new KImageFolder(this);
      if (newPages != NULL)
        newPages->push_back(child);
      size = child->ReadDocument(ms, newPages, flag);
      if (size == 0)
        return 0;
      InsertPage(child, -1);
    }
    else if (tag == KMS_IMAGE_ITEM)
    {
      KImagePage* child = new KImagePage(this);
      if (newPages != NULL)
        newPages->push_back(child);
      size = child->ReadDocument(ms, NULL, flag);
      if (size != S_OK)
        return 0;
      InsertPage(child, -1);
    }

    readed += size;
  }
  readed += sizeof(DWORD);
  return readed;
}

DWORD KImageFolder::StoreDocument(KMemoryStream& ms, BOOL selectedOnly)
{
  ms.WriteDword(KMS_FOLDER_ITEM);
  ms.WriteDword(0);

  int startPos = ms.GetPosition();
  DWORD blockSize = sizeof(DWORD) * 2;

  ms.WriteStringBlock(mName);

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    if (!selectedOnly || p->IsSelected())
    {
      // update last-modified if changed
      if (p->IsModified())
      {
        p->InitMetaDatas(FALSE);
      }

      DWORD size = p->StoreDocument(ms, selectedOnly);
      if (size == 0)
        return 0;
      blockSize += size;
    }
  }

  ms.UpdateBlockSize(startPos);
  return blockSize;
}

#endif // USE_XML_STORAGE

void KImageFolder::ReadyDefaultName(LPTSTR pageName)
{
  StringCchPrintf(pageName, 60, _T("Folder %d"), g_DefaultFolderNameIndex++);
}

int KImageFolder::MoveToWorkingFolder(LPCTSTR pathName)
{
  int r = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    r += p->MoveToWorkingFolder(pathName);
  }
  return r;
}

int KImageFolder::StoreImageFile(LPCTSTR pathName, KEcmDocTypeInfo* info, LPCTSTR firstFilename, BOOL asTemporaryIfExist)
{
  int r = 0;
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;

    if ((p->GetType() == FOLDER_PAGE) || p->IsModified() || (firstFilename != NULL))
      r += p->StoreImageFile(pathName, info, firstFilename, asTemporaryIfExist);

    firstFilename = NULL;
  }
  return r;
}

BOOL KImageFolder::RefreshMetaData()
{
  KImageBase::RefreshMetaData();
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    p->RefreshMetaData();
  }
  return TRUE;
}

int KImageFolder::ImageAutoRename(LPCTSTR pathName, int& index, int& base)
{
  int count = 0;
  KImageDocIterator it = m_Childs.begin();
  base = index; // set base same as index
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    count += p->ImageAutoRename(pathName, index, base);
  }
  return count;
}

HANDLE KImageFolder::BackupPageNames()
{
  KMemoryStream mf;

  WriteImageName(mf);

  if (mf.Size() > 0)
  {
    HANDLE h = KMemoryAlloc(mf.Size());
    LPBYTE buff = (LPBYTE)KMemoryLock(h);
    memcpy(buff, mf.Data(), mf.Size());
    KMemoryUnlock(h);
    return h;
  }
  return NULL;
}

BOOL KImageFolder::RestorePageNames(HANDLE h)
{
  LPBYTE buff = (LPBYTE)KMemoryLock(h);
  UINT size = KMemorySize(h);

  KMemoryStream mf;
  mf.Attach(buff, size, ReadOnly);

  int count = ReadImageName(mf);
  mf.Detach();
  KMemoryUnlock(h);
  return (count > 0);
}

void KImageFolder::WriteImageName(KMemoryStream& mf)
{
  KImageBase::WriteImageName(mf);

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    p->WriteImageName(mf);
  }
}

int KImageFolder::ReadImageName(KMemoryStream& mf)
{
  int count = 1;
  KImageBase::ReadImageName(mf);

  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    count += p->ReadImageName(mf);
  }
  return count;
}

LPTSTR KImageFolder::GetFolderNameID()
{
  KMemoryStream mf;
  WriteFolderNameID(mf);
  mf.WriteWord(0);

  LPTSTR namestr = AllocString((LPCTSTR)mf.Data());
  return namestr;
}

int KImageFolder::WriteFolderNameID(KMemoryStream& mf)
{
  int len = 0;
  LPCTSTR name = GetName();
  if (name != NULL)
  {
    mf.WriteString(name);
    len += lstrlen(name);
  }

  mf.WriteWord(DA_SLASH_SEPARATOR);
  ++len;

  TCHAR idstr[16];
  StringCchPrintf(idstr, 16, _T("%d"), GetID());
  mf.WriteString(idstr);
  len += lstrlen(idstr);
  return len;
}

LPTSTR KImageFolder::GetSubFolderNames()
{
  KImageDocVector folderPages;

  GetPageList(folderPages, FOLDER_ONLY);

  if (folderPages.size() > 0)
  {
    KMemoryStream mf;
    int count = 0;

    KImageDocIterator it = folderPages.begin();
    while (it != folderPages.end())
    {
      KImageFolder* p = (KImageFolder*)*it++;
      if (count > 0)
        mf.WriteWord(LF_CHAR);
      p->WriteFolderNameID(mf);
      count++;
    }
    mf.WriteWord(0);

    LPTSTR names = AllocString((LPCTSTR)mf.Data());
    return names;
  }
  return NULL;
}

void KImageFolder::ReadyFolderPages(LPTSTR folderList)
{
  LPTSTR p = folderList;

  while ((p != NULL) && (*p != '\0'))
  {
    LPTSTR e = StrChr(p, LF_CHAR);
    if (e != NULL)
    {
      *e = '\0';
      ++e;
    }

    KImageFolder* f = GetPageFromParentKeyName(p, FALSE);

    if (f == NULL)
    {
      TCHAR msg[256] = _T("Cannot create folder : ");
      StringCchCat(msg, 256, p);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_DEBUG | SYNC_MSG_LOG);
    }

    p = e;
  }
}

void KImageFolder::ClearTempFiles(LPCTSTR workingPath)
{
  KImageDocIterator it = m_Childs.begin();
  while (it != m_Childs.end())
  {
    KImageBase* p = *it++;
    p->ClearTempFiles(workingPath);
  }
}

KMetadataInfo* KImageFolder::GetMetaDataInfo(int index)
{
  KImageBase* p = GetIndexedPage(index, IMAGE_ONLY);
  if (p == NULL)
  {
    TCHAR msg[128];
    StringCchPrintf(msg, 128, _T("No indexed page : %d"), index);
    StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_DEBUG | SYNC_MSG_LOG);
  }
  if ((p != NULL) && (p->GetType() == IMAGE_PAGE))
  {
    return &((KImagePage*)p)->GeDocExtAttributes();
  }
  return NULL;
}
