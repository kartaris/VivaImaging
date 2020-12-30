#include "StdAfx.h"
//#include "../../LocalSyncExec/resource.h"

#include <tchar.h>
#include "KSettings.h"
#include "KConstant.h"

#include "../Platform/Platform.h"
#include "../Mainfrm.h"

#include <Strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

const TCHAR CURRENT_MODULE_NAME[] = _T("VivaImaging");
//#include "KSqliteUtil.h"
//#include "../SqliteLib/sqlite3.h"

const TCHAR HISTORY_FILENAME_FORMAT[] = _T("history%d-%.2d%.2d.txt");

#ifdef USE_SYNC_GUI
const TCHAR CURRENT_MODULE_NAME[] = _T("LocalSyncExec");
#elif defined(SYNC_OVERLAY_ICON)
const TCHAR CURRENT_MODULE_NAME[] = _T("SyncOverIcon");
#elif defined(SYNC_SHELLEXT)
const TCHAR CURRENT_MODULE_NAME[] = _T("SyncShellExt");
#elif defined(WIN32_SERVICES)
const TCHAR CURRENT_MODULE_NAME[] = _T("SyncServices");
#elif defined(HTTP_MSG_SEND_TEST)
const TCHAR CURRENT_MODULE_NAME[] = _T("HttpMsgSendTest");
#endif

static BYTE utf8_bom[] = { 0xEF, 0xBB, 0xBF };

#if defined(USE_LOCALSYNC)

char *GetCurrentSyncFolders()
{
  char *buff = NULL;
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, SYNC_FOLDER_LIST_FILENAME);

	FILE *f = OpenDelayedFile(setting_file, _T("r"));
  /*
  FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
	if (err == 0)
  */
  if (f != NULL)
  {
      fseek(f, 0, SEEK_END);
      size_t size = ftell(f);
      fseek(f, 0, SEEK_SET);
      if (size > 0)
      {
        buff = new char[size + 2];
        if (buff != NULL)
        {
          size = fread(buff, 1, size, f);
          buff[size] = '\0';
        }
      }
    fclose(f);
  }
  return buff;
}

// caller should free returned memory by delete[]
LPTSTR GetCurrentSyncFoldersT()
{
  LPTSTR rtn = NULL;
  char *buff = GetCurrentSyncFolders();
  if ((buff != NULL) && (strlen(buff) > 0))
  {
    int len = ToUnicodeString((TCHAR *)NULL, 0, buff, (int)strlen(buff));

    rtn = new TCHAR[len + 1];

    ToUnicodeString(rtn, len+1, buff, (int)strlen(buff));
    rtn[len] = '\0';
    delete[] buff;
  }
  return rtn;
}

BOOL RemoveSyncFolder(char *path)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, SYNC_FOLDER_LIST_FILENAME);

  BOOL item_found = FALSE;
  char *buff = NULL;
	FILE *f = OpenDelayedFile(setting_file, _T("r"));
  /*
	FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
	if (err == 0)
  */
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];
      char *dest = new char[size + 2];
      char *d = dest;

      if ((buff != NULL) && (dest != NULL))
      {
        size = fread(buff, 1, size, f);
        buff[size] = '\0';
        char *p = buff;
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          size_t len;

          if (n != NULL)
            len = (size_t)(n - p);
          else
            len = size;

          // check if it is path that will removed
          if ((len != strlen(path)) || (strncmp(p, path, len) != 0))
          {
            memcpy(d, p, len);
            d += len;
            memcpy(d, LF_STR, 1);
            d++;
          }
          else
          {
            item_found = TRUE;
          }

          p += len + 1;
          size -= (len + 1);
        }

        if (item_found) // need update
        {
          fclose(f);
          /*
          err = _tfopen_s(&f, setting_file, _T("w"));
	        if (err == 0)
          */
          MakeWritabeFile(setting_file);
          f = OpenDelayedFile(setting_file, _T("w"));
          if (f != NULL)
          {
            // fseek(f, 0, SEEK_SET);
            if (d > dest)
              fwrite(dest, 1, (size_t)(d - dest), f);
          }
        }
      }

      if (buff != NULL)
        delete[] buff;
      if (dest != NULL)
        delete[] dest;
    }
    fclose(f);
    return item_found;
  }
  return FALSE;
}

BOOL ReplaceSyncFolder(char *oldPath, char *newPath, LPCTSTR ownerOID)
{
  TCHAR setting_file[KMAX_PATH];

  if (ownerOID == NULL)
  {
    MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, SYNC_FOLDER_LIST_FILENAME);
  }
  else
  {
    MakeFullPathName(setting_file, KMAX_PATH, DESTINY_HOME_FOLDER, ownerOID);
    MakeFullPathName(setting_file, KMAX_PATH, setting_file, SYNC_FOLDER_LIST_FILENAME);
  }


  BOOL item_found = FALSE;
  char *buff = NULL;
	FILE *f = OpenDelayedFile(setting_file, _T("r"));
  /*
	FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
	if (err == 0)
  */
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];
      char *dest = new char[size + 262];
      char *d = dest;

      if ((buff != NULL) && (dest != NULL))
      {
        size = fread(buff, 1, size, f);
        buff[size] = '\0';
        char *p = buff;
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          size_t len;

          if (n != NULL)
            len = (size_t)(n - p);
          else
            len = size;

          // check if it is path that will removed
          if ((len != strlen(oldPath)) || (strncmp(p, oldPath, len) != 0))
          {
            memcpy(d, p, len);
            d += len;
            memcpy(d, LF_STR, 1);
            d++;
          }
          else
          {
            memcpy(d, newPath, strlen(newPath));
            d += strlen(newPath);
            memcpy(d, LF_STR, 1);
            d++;
            item_found = TRUE;
          }

          p += len + 1;
          size -= (len + 1);
        }

        if (item_found) // need update
        {
          fclose(f);

          MakeWritabeFile(setting_file);
          f = OpenDelayedFile(setting_file, _T("w"));
          if (f != NULL)
          {
            // fseek(f, 0, SEEK_SET);
            if (d > dest)
              fwrite(dest, 1, (size_t)(d - dest), f);
          }
        }
      }

      if (buff != NULL)
        delete[] buff;
      if (dest != NULL)
        delete[] dest;
    }
    fclose(f);
    return item_found;
  }
  return FALSE;
}

BOOL RemoveSyncFolderT(LPCTSTR path)
{
  if ((path != NULL) && (lstrlen(path) > 0))
  {
    char *path_a = MakeUtf8String(path, lstrlen(path));
    if (path_a != NULL)
    {
      BOOL r = RemoveSyncFolder(path_a);
      delete[] path_a;
      return r;
    }
  }
  return FALSE;
}

BOOL ReplaceSyncFolderT(LPCTSTR oldFolderName, LPCTSTR newFolderName, LPCTSTR ownerOID)
{
  BOOL r = FALSE;
  char *oldPath = MakeUtf8String(oldFolderName);
  char *newPath = MakeUtf8String(newFolderName);
  if ((oldPath != NULL) && (newPath != NULL))
  {
    r = ReplaceSyncFolder(oldPath, newPath, ownerOID);
  }
  if (oldPath != NULL)
    delete[] oldPath;
  if (newPath != NULL)
    delete[] newPath;
  /**
  char *oldPath[KMAX_PATH_A];
  char *newPath[KMAX_PATH_A];
  if ( (ToUtf8String(oldPath, KMAX_PATH_A, oldFolderName, lstrlen(oldFolderName)) > 0) &&
    (ToUtf8String(newPath, KMAX_PATH_A, newFolderName, lstrlen(newFolderName)) > 0) )
  {
    return ReplaceSyncFolder(oldPath, newPath);
  }
  **/
  return r;
}

BOOL RemoveSyncFolderList()
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, SYNC_FOLDER_LIST_FILENAME);
  return KDeleteFile(setting_file);
}


BOOL IsCurrentSyncFoldersT(TCHAR *path)
{
  LPTSTR paths = GetCurrentSyncFoldersT();
  BOOL found = (FindLFSeparateStringT(paths, path) >= 0);
  delete[] paths;
  return found;
}

BOOL AppendSyncFolder(char *path)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, SYNC_FOLDER_LIST_FILENAME);

  char *buff = NULL;
  FILE *f = OpenDelayedFile(setting_file, _T("r+"));
  /*
  FILE *f;
  errno_t err = _tfopen_s(&f, setting_file, _T("r+"));
  if (err == 0)
  */
  if (f != NULL)
  {
    // check if already exist
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      BOOL already_exist = FALSE;
      char *buff = new char[size + 2];
      if (buff != NULL)
      {
        fread(buff, 1, size, f);
        buff[size] = '\0';
        char *p = buff;
        char *e = buff + size;
        while (p < e)
        {
          char *n = strchr(p, LF_CHAR);
          if (n != NULL)
            *n = '\0';
          if (_stricmp(path, p) == 0)
          {
            already_exist = TRUE;
            break;
          }
          if (n != NULL)
            p = n + 1;
          else
            p = e;
        }

        delete[] buff;
      }

      if (already_exist)
      {
        fclose(f);
        return TRUE;
      }
    }
  }
  else
  {
    MakeWritabeFile(setting_file);
    f = OpenDelayedFile(setting_file, _T("w"));
  }

  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    fwrite(path, 1, strlen(path), f);
    fwrite(LF_STR, 1, 1, f);
    fclose(f);
    return TRUE;
  }

  return FALSE;
}

BOOL AppendSyncFolderT(LPCTSTR path)
{
  if ((path != NULL) && (lstrlen(path) > 0))
  {
    BOOL r = FALSE;
    char *apath = MakeUtf8String(path);
    if (apath != NULL)
    {
      r = AppendSyncFolder(apath);
      delete[] apath;
    }
    return r;
    /*
    char buff[KMAX_PATH];
    if (ToUtf8String(buff, KMAX_PATH, path, lstrlen(path)) > 0)
    {
    return AppendSyncFolder(buff);
    }
    */
  }
  return FALSE;
}

#endif // LOCALSYNC

int FindLFSeparateStringT(LPTSTR paths, LPCTSTR path)
{
  BOOL found = FALSE;
  if (paths != NULL)
  {
    LPTSTR p = paths;
    LPTSTR e = p + lstrlen(paths);

    while (p < e)
    {
      LPTSTR n = StrChr(p, LF_CHAR);
      if (n != NULL)
      {
        *n = '\0';
        if (CompareStringICNC(path, p) == 0)
        {
          return (int)(p - paths);
        }
        p = n + 1;
      }
      else
      {
        if (CompareStringICNC(path, p) == 0)
        {
          return (int)(p - paths);
        }
        p = e;
      }
    }
  }
  return -1;
}

LPSTR AddSettingKeyValue(LPSTR buff, int &buff_length, const char *setting_key, const char *value)
{
  int need_len = (int)(strlen(buff) + strlen(setting_key) + strlen(value) + 20);
  if (need_len > buff_length)
  {
    int incr = need_len - buff_length;
    if(incr < 1024)
      incr = 1024;
    LPSTR nbuff = new char[buff_length + incr];
    memcpy(nbuff, buff, buff_length);
    delete[] buff;
    buff = nbuff;
    buff_length += incr;
  }

  LPSTR d = buff + strlen(buff);
  memcpy(d, setting_key, strlen(setting_key));
  d += strlen(setting_key);
  *(d++) = '=';
  memcpy(d, value, strlen(value));
  d += strlen(value);
  memcpy(d, LF_STR, 1);
  d += 1;
  *d = '\0';
  return buff;
}

BOOL StoreSettingData(LPTSTR setting_file, const char *buff)
{
  MakeWritabeFile(setting_file);
	FILE *f = OpenDelayedFile(setting_file, _T("w"));
  if (f != NULL)
  {
    fwrite(buff, 1, strlen(buff), f);
    fclose(f);
    return TRUE;
  }
  return FALSE;
}

void StoreSettingKeyValue(LPTSTR setting_file, const char *setting_key, const char *value)
{
  /*
	FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
  int retry = 0;

	while (err != 0)
  {
    if(retry > 30)
    {
      TCHAR msg[512] = _T("Cannot read to setting-file:");
      StringCchCat(msg,512, setting_file);
      OutputDebugString(msg);
      return;
    }
    retry++;
    ::Sleep(2);
    err = _tfopen_s(&f, setting_file, _T("r+"));
  }
	if (err == 0)
  */
	FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
  {
    char key[64];
    char data[1000];
    BOOL update_need = TRUE;
    BOOL item_exist = FALSE;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buff = new char[size + 2];
    char *dest = new char[size + 1000];
    char *d = dest;
    if ((buff != NULL) && (dest != NULL))
    {
      if (size > 0)
      {
        size = fread(buff, 1, size, f);
        fclose(f);
        f = NULL;

        buff[size] = '\0';
        char *p = buff;

        // UTF8 encoding signature
        if((size > 3) && (memcmp(p, utf8_bom, 3) == 0))
        {
          p += 3;
          size -= 3;
        }
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          char *s = strchr(p, '=');
          size_t len = 0;

          if(s == NULL)
            break;

          if(n != NULL)
            len = (n - p + 1);
          else
            len = strlen(p);

          if (item_exist || (n < s))
          {
            memcpy(d, p, len);
            d += len;
            /*
            if ((*p != LF_CHAR) || (len > 1))
            {
              memcpy(d, LF_STR, 1);
              d += 1;
            }
            */
          }
          else
          {
            if ((p != NULL) && (s != NULL))
            {
              strncpy_s(key, 64, p, (size_t)(s - p));
              if (n != NULL)
                strncpy_s(data, 1000, s+1, (size_t)(n - s - 1));
              else
                strcpy_s(data, 1000, s+1);

              if (strcmp(key, setting_key) == 0)
              {
                item_exist = TRUE;
                if (strcmp(value, data) == 0) // no need to update
                {
                  update_need = FALSE;
                  break;
                }
                else
                  strcpy_s(data, 1000, value);
              }

              // copy to new buffer
              memcpy(d, key, strlen(key));
              d += strlen(key);
              *(d++) = '=';
              memcpy(d, data, strlen(data));
              d += strlen(data);
              memcpy(d, LF_STR, 1);
              d += 1;
            }
          }
          p += len;
          size -= len;
        }
      }

      if (update_need)
      {
        if (!item_exist)
        {
            memcpy(d, setting_key, strlen(setting_key));
            d += strlen(setting_key);
            *(d++) = '=';
            memcpy(d, value, strlen(value));
            d += strlen(value);
            memcpy(d, LF_STR, 1);
            d += 1;
        }

        // re-open
        /*
		    errno_t err = _tfopen_s(&f, setting_file, _T("w"));
		    if (err == 0)
        */
        MakeWritabeFile(setting_file);
        f = OpenDelayedFile(setting_file, _T("w"));
        if (f != NULL)
        {
          fwrite(dest, 1, (d - dest), f);
          fclose(f);
          f = NULL;
        }
        else
        {
          OutputDebugString(_T("Cannot open file to write : "));
          OutputDebugString(setting_file);
          OutputDebugString(_T("\n"));
        }
      }

      if (buff != NULL)
        delete[] buff;
      if (dest != NULL)
        delete[] dest;
    }
  }
  else
  {
    /*
  	errno_t err = _tfopen_s(&f, setting_file, _T("w"));
    if (err == 0)
    */
    MakeWritabeFile(setting_file);
    f = OpenDelayedFile(setting_file, _T("w"));
    if (f != NULL)
    {
      fwrite(setting_key, 1, strlen(setting_key), f);
      fwrite("=", 1, 1, f);
      fwrite(value, 1, strlen(value), f);
      fwrite(LF_STR, 1, 1, f);
    }
    else
    {
      OutputDebugString(_T("Cannot create file : "));
      OutputDebugString(setting_file);
      OutputDebugString(_T("\n"));
    }
  }
  if (f != NULL)
    fclose(f);
}

void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, int value)
{
  char str[30];
  sprintf_s(str, 30, "%d", value);
  StoreSettingKeyValue(filename, setting_key, str);
}

/*
Should free returned memory by delete[]
*/
// char *ReadSettingKeyValue(TCHAR *filename, const char *setting_key)

BOOL ReadSettingKeyValue(TCHAR *setting_file, const char *setting_key, char *outbuff, int outbuff_size)
{
  BOOL rtn = FALSE;
  /*
	FILE *f;
	errno_t err = _tfopen_s(&f, setting_file, _T("r"));
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
  {
    char key[64];
    BOOL update_need = TRUE;

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];

      if (buff != NULL)
      {
        size = fread(buff, 1, size, f);
        fclose(f);
        f = NULL;
        buff[size] = '\0';
        char *p = buff;

        // UTF8 encoding signature
        if((size > 3) && (memcmp(p, utf8_bom, 3) == 0))
        {
          p += 3;
          size -= 3;
        }
        while(size > 0)
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          char *s = strchr(p, '=');
          size_t len;
          if(n != NULL)
            len = (n - p + 1);
          else
            len = strlen(p);

          if (s != NULL)
          {
            strncpy_s(key, 64, p, (size_t)(s - p));

            if (strcmp(key, setting_key) == 0)
            {
              size_t value_len;

              if (n != NULL)
                value_len = n - s - 1;
              else
                value_len = strlen(s+1);

              if ((value_len + 1) > (size_t)outbuff_size)
              {
                TCHAR msg[256];
                StringCchPrintf(msg, 256, _T("ReadSettingKeyValue need buffer length %d\n"), value_len+1);
                OutputDebugString(msg);
                value_len = outbuff_size - 1;
              }

              if (value_len > 0)
              {
                memcpy(outbuff, (s + 1), value_len);
                outbuff[value_len] = '\0';
                rtn = TRUE;
              }
              break;
            }

            p += len;
            size -= len;
          }
          else
            break;
        }
      }

      if (buff != NULL)
        delete[] buff;
    }

    if (f != NULL)
      fclose(f);
  }
  else
  {
    DWORD err = GetLastError();
    if (err == 3)
      return rtn;
    if (err != 2)
    {
      TCHAR msg[300];
      StringCchPrintf(msg, 300, _T("fopen error : %d : "), err);
      StringCchCat(msg, 300, setting_file);
      StoreLogHistory(__FUNCTIONW__, msg, SYNC_MSG_LOG);
    }
  }
  return rtn;
}

void AppendSettingKeyValue(const char *setting_key, const char *value)
{
  if (lstrlen(mHomeFolder) > 0)
  {
    TCHAR setting_file[KMAX_PATH];
    MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, APP_SETTING_FILENAME);
    StoreSettingKeyValue(setting_file, setting_key, value);
  }
}

BOOL GetSettingKeyValue(const char *setting_key, char *buff, int buff_size)
{
  if (lstrlen(mHomeFolder) > 0)
  {
    TCHAR setting_file[KMAX_PATH];
    MakeFullPathName(setting_file, KMAX_PATH, mHomeFolder, APP_SETTING_FILENAME);
    return ReadSettingKeyValue(setting_file, setting_key, buff, buff_size);
  }
  return FALSE;
}

#ifndef USE_BACKUP
void AppendCommonSettingKeyValue(const char *setting_key, const char *value)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, DESTINY_HOME_FOLDER, APP_SETTING_FILENAME);
  StoreSettingKeyValue(setting_file, setting_key, value);
}

BOOL GetCommonSettingKeyValue(const char *setting_key, char *buff, int buff_size)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, DESTINY_HOME_FOLDER, APP_SETTING_FILENAME);
  return ReadSettingKeyValue(setting_file, setting_key, buff, buff_size);
}
#endif

BOOL MakeNextSequenceName(int max_length, LPTSTR candidate)
{
  int p = (int)lstrlen(candidate) - 1;

  while(p >= 0)
  {
    if (candidate[p] < 'Z')
    {
      ++candidate[p];
      return TRUE;
    }
    else
    {
      candidate[p] = 'A';
    }
    p--;
  }

  // extend length
  p = (int)lstrlen(candidate);
  if (p < max_length)
  {
    candidate[p] = 'A';
    candidate[++p] = '\0';
    return TRUE;
  }
  return FALSE;
}

#if 0
BOOL makeNextMapNameCo(int max_length, char *candidate)
{
  int p = (int)strlen(candidate) - 1;

  while(p >= 0)
  {
    if (candidate[p] < 'Z')
    {
      ++candidate[p];
      return TRUE;
    }
    else
    {
      candidate[p] = 'A';
    }
    p--;
  }

  // extend length
  p = (int)strlen(candidate);
  if (p < max_length)
  {
    candidate[p] = 'A';
    candidate[++p] = '\0';
    return TRUE;
  }
  return FALSE;
}

static char *msdos_preserved_devices[4] = {
  "AUX",
  "CON",
  "PRN",
  "NUL" };

BOOL makeNextMapName(int max_length, char *candidate)
{
  while (makeNextMapNameCo(max_length, candidate))
  {
    // check if preserved name
    if (strlen(candidate) == 3)
    {
      BOOL preserved = FALSE;
      for (int n = 0; n < 4; n++)
      {
        if (_stricmp(candidate, msdos_preserved_devices[n]) == 0)
        {
          preserved = TRUE;
          break;
        }
      }
      if (!preserved)
        return TRUE;
    }
    else
      return TRUE;
  }
  return FALSE;
}

BOOL GetMetaFolderNameMapping(const TCHAR *obj_path, const char *subfolder, char *mapping_name, BOOL createIfNone)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, obj_path, FOLDERMAP_LIST_FILENAME);

  BOOL found = FALSE;
  char candidate[16] = { '\0' };

  /*
  FILE *f = NULL;
	errno_t err = _tfopen_s(&f, setting_file, L"r");
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
	{
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];
      if (buff != NULL)
      {
        size = fread(buff, 1, size, f);
        char *p = buff;
        size_t line_size;
  
        while(p < (buff+size))
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          if (n != NULL)
            *n = '\0';
          line_size = strlen(p) + 1;
          char *s = strchr(p, '=');
          if (s != NULL)
          {
            *s = '\0';
            strcpy_s(candidate, 16, p);
            if (_stricmp(subfolder, s+1) == 0)
            {
              strcpy_s(mapping_name, 16, p);
              found = TRUE;
              break;
            }
          }
          p += line_size;
        }

        delete[] buff;
      }
    }
    fclose(f);
  }

  // add new meta-folder-name
  if ((found == FALSE) && createIfNone)
  {
    if (strlen(candidate) == 0)
    {
      strcpy_s(candidate, 16, DEF_FOLDER_MAP_NAME);
      found = TRUE;
    }
    else
    {
      found = makeNextMapName(5, candidate);
    }

    if (found)
    {
      strcpy_s(mapping_name, 16, candidate);
      // append to file
      /*
      errno_t err = _tfopen_s(&f, setting_file, L"a+");
	    if (err == 0)
      */
      f = OpenDelayedFile(setting_file, _T("a+"));
      if (f != NULL)
      {
        fseek(f, 0, SEEK_END);
        fwrite(mapping_name, 1, strlen(mapping_name), f);
        fwrite("=", 1, 1, f);
        fwrite(subfolder, 1, strlen(subfolder), f);
        fwrite(LF_STR, 1, 1, f);
        fclose(f);
      }
    }
  }
  return found;
}

BOOL GetMetaFolderNameOrigin(const TCHAR *obj_path, char *subfolder, int buff_len, const char *mapping_name)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, obj_path, FOLDERMAP_LIST_FILENAME);

  BOOL found = FALSE;
  char candidate[16] = { '\0' };

  /*
  FILE *f = NULL;
	errno_t err = _tfopen_s(&f, setting_file, L"r");
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
	{
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];
      if (buff != NULL)
      {
        size = fread(buff, 1, size, f);
        char *p = buff;
        size_t line_size;
  
        while(p < (buff+size))
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          if (n != NULL)
            *n = '\0';
          line_size = strlen(p) + 1;
          char *s = strchr(p, '=');
          if (s != NULL)
          {
            *s = '\0';
            if (_stricmp(p, mapping_name) == 0)
            {
              ++s;
              strcpy_s(subfolder, buff_len, s);
              found = TRUE;
              break;
            }
          }
          p += line_size;
        }

        delete[] buff;
      }
    }
    fclose(f);
  }
  return found;
}

static BOOL isMatchingRelativePath(LPCSTR IN oldPath, BOOL includeSub, LPSTR IN srcpath, LPSTR OUT subpath)
{
  BOOL match = FALSE;

  subpath[0] = '\0';
  if (includeSub && (strlen(oldPath) < strlen(srcpath)))
  {
    match = (strncmp(oldPath, srcpath, strlen(oldPath)) == 0);
    if (match)
      strcpy_s(subpath, KMAX_PATH, &srcpath[strlen(oldPath)]);
  }
  else
  {
    match = (strcmp(oldPath, srcpath) == 0);
  }
  return match;
}

BOOL UpdateMetaFolderNameMapping(LPCTSTR obj_path, LPCSTR oldPath, LPCSTR newPath, BOOL includeSub)
{
  TCHAR setting_file[KMAX_PATH];
  MakeFullPathName(setting_file, KMAX_PATH, obj_path, FOLDERMAP_LIST_FILENAME);
  char subpath[KMAX_PATH];

  BOOL found = FALSE;
  /*
  FILE *f = NULL;
	errno_t err = _tfopen_s(&f, setting_file, L"r");
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(setting_file, _T("r"));
  if (f != NULL)
	{
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      char *buff = new char[size + 2];
      char *dest = new char[size + 262];
      if ((buff != NULL) && (dest != NULL))
      {
        size = fread(buff, 1, size, f);
        buff[size] = '\0';
        fclose(f);
        f = NULL;

        char *p = buff;
        char *d = dest;
        size_t line_size;
  
        while(p < (buff+size))
        {
          // find end-of-line
          char *n = strchr(p, LF_CHAR);
          if (n != NULL)
            *n = '\0';
          line_size = strlen(p) + 1;

          if (found && !includeSub)
          {
            memcpy(d, p, line_size - 1);
            d += (line_size - 1);
          }
          else
          {
            char *s = strchr(p, '=');
            if (s != NULL)
            {
              *s = '\0';

              // if includeSub, when 'aa' is changed 'bb',
              // change 'AA=aa' to 'AA=bb', and also 'AB=aa/xx' to 'AB=bb/xx'

              if (isMatchingRelativePath(oldPath, includeSub, s+1, subpath))
              {
                memcpy(d, p, strlen(p));
                d += strlen(p);
                *d = '=';
                d++;

                memcpy(d, newPath, strlen(newPath));
                d += strlen(newPath);

                if (strlen(subpath) > 0)
                {
                  memcpy(d, subpath, strlen(subpath));
                  d += strlen(subpath);
                }

                found = TRUE;
              }
              /**
              if (strcmp(oldPath, s+1) == 0)
              {
                memcpy(d, p, strlen(p));
                d += strlen(p);
                *d = '=';
                d++;

                memcpy(d, newPath, strlen(newPath));
                d += strlen(newPath);
                found = TRUE;
              }
              **/
              else
              {
                *s = '=';
                memcpy(d, p, strlen(p));
                d += strlen(p);
              }
            }
          }
          p += line_size;
          *d = LF_CHAR;
          d++;
        }
        delete[] buff;

        if (found)
        {
          /*
	        errno_t err = _tfopen_s(&f, setting_file, L"w");
	        if (err == 0)
          */
          MakeWritabeFile(setting_file);
          f = OpenDelayedFile(setting_file, _T("w"));
          if (f != NULL)
	        {
            size_t len = (size_t)(d - dest);
            if (len > 0)
              fwrite(dest, 1, len, f);
            fclose(f);
            f = NULL;
          }
        }
        delete[] dest;
      }
    }
    if (f != NULL)
      fclose(f);
  }
  return found;
}

BOOL UpdateMetaFolderNameMappingT(LPCTSTR obj_path, LPCTSTR oldPath, LPCTSTR newPath, BOOL includeSub)
{
  char *oldfolder = MakeUtf8String(oldPath);
  char *newfolder = MakeUtf8String(newPath);
  BOOL r = FALSE;
  if ((oldfolder != NULL) && (newfolder != NULL))
    r = UpdateMetaFolderNameMapping(obj_path, oldfolder, newfolder, includeSub);
  if (oldfolder != NULL)
    delete[] oldfolder;
  if (newfolder != NULL)
    delete[] newfolder;
  return r;
  /*
  char oldfolder[KMAX_PATH];
  ToUtf8String(oldfolder, KMAX_PATH, oldPath, lstrlen(oldPath));
  char newfolder[KMAX_PATH];
  ToUtf8String(newfolder, KMAX_PATH,newPath, lstrlen(newPath));

  return UpdateMetaFolderNameMapping(obj_path, oldfolder, newfolder, includeSub);
  */
}
#endif

#define LOGFILE_WITH_NO_CLOSE

#if defined(LOGFILE_WITH_NO_CLOSE)
static HANDLE hLogFile = NULL;
static LPTSTR hLogName = NULL;
#endif

void CloseLogFile()
{
#if defined(LOGFILE_WITH_NO_CLOSE)
  if (hLogFile != NULL)
  {
    CloseLogFileShared(hLogFile);
    hLogFile = NULL;
  }
  if (hLogName != NULL)
  {
    delete[] hLogName;
    hLogName = NULL;
  }
#endif
}

static BOOL AppendMessageToFile(const TCHAR *filename, char *buff, BOOL bClose)
{
#if defined(LOGFILE_WITH_NO_CLOSE)
  HANDLE f = NULL;

  if (bClose)
  {
    f = OpenLogFileShared(filename);
  }
  else
  {
    // check if filename changed
    if ((hLogName != NULL) && (CompareStringICNC(filename, hLogName) != 0))
    {
      if (hLogFile != NULL)
      {
        CloseLogFileShared(hLogFile);
        hLogFile = NULL;
      }

      delete[] hLogName;
      hLogName = NULL;
    }

    if (hLogFile == NULL)
    {
      hLogFile = OpenLogFileShared(filename);
      if (hLogName != NULL)
        delete[] hLogName;
      hLogName = AllocString(filename);
    }
    f = hLogFile;
  }

  if (f != NULL)
  {
    DWORD written = 0;
    int bufflen = strlen(buff);
    LONG ptr = 0;

    DWORD p = SetFilePointer(f, 0, &ptr, FILE_END);
    if (!WriteFile(f, buff, bufflen, &written, NULL))
    {
      TCHAR msg[64];
      StringCchPrintf(msg, 64, _T("Write log fail : 0x%X\n"), GetLastError());
      OutputDebugString(msg);
    }

    if (bClose)
      CloseLogFileShared(f);
    return TRUE;
  }
  return FALSE;
#else
  FILE *f = OpenDelayedFile(filename, _T("a+"));
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    fwrite(buff, 1, strlen(buff), f);
    fclose(f);
  }
#endif
}

static BOOL AppendMessageToFile(const TCHAR *filename, TCHAR *buff, BOOL bClose)
{
  BOOL r = TRUE;
  char *p = MakeUtf8String(buff);
  if (p != NULL)
  {
    r = AppendMessageToFile(filename, p, bClose);
    delete[] p;
  }
  return r;
}

void AppendLogMessageEx(TCHAR *buff, SYSTEMTIME &systime, LPCTSTR logFolderName, BOOL bClose)
{
  TCHAR filename[KMAX_PATH];

  LPCTSTR baseFolder = (lstrlen(gModulePathName) > 0) ? gModulePathName : DESTINY_HOME_FOLDER;
  MakeFullPathName(filename, KMAX_PATH, baseFolder, LOG_FOLDER_NAME);

	if (IsDirectoryExist(filename) || CreateDirectory(filename))
  {
    MakeFullPathName(filename, KMAX_PATH, filename, logFolderName);
  	if (IsDirectoryExist(filename) || CreateDirectory(filename))
    {
      // make date filename
      TCHAR name[64];
      StringCchPrintf(name, 64, _T("%d%.2d%.2d-%.2d.log"), systime.wYear, systime.wMonth, systime.wDay, mSessionIdx);
      MakeFullPathName(filename, KMAX_PATH, filename, name);
      BOOL r = AppendMessageToFile(filename, buff, bClose);

    }
  }
}

void AppendLogMessage(TCHAR *buff, SYSTEMTIME &systime)
{
#ifdef MAIN_EXEC_MODULE
  AppendLogMessageEx(buff, systime, LOG_SUBFOLDER_NAME, FALSE);
#else
  AppendLogMessageEx(buff, systime, LOG_SUBFOLDER_NAME, TRUE);
#endif
}

#ifdef USE_SYNC_ENGINE

#ifdef HISTORY_BY_SQLITE

static BOOL sql3_checkReadyHistoryTables(sqlite3 *db)
{
  char *errmsg = NULL;

  char sqlcmd[] = "CREATE TABLE IF NOT EXISTS LS_HISTORY("  \
        "ID INTEGER PRIMARY KEY AUTOINCREMENT," \
        "STATUS INT," \
        "PATHNAME TEXT," \
        "EXTRA TEXT," \
        "DATE INT," \
        "TYPE INT," \
        "USER CHAR(32));";

  BOOL r = !!sqlite3_exec(db, sqlcmd, sql_callback, 0, &errmsg);
    
  if (errmsg != NULL)
  {
    debug_sql_msg(errmsg, sqlcmd);
    sqlite3_free(errmsg);
  }
  return r;
}

static BOOL writeHistoryItem(sqlite3 *db, int st, LPCTSTR pathName, int type, LPCTSTR extra, __int64 time_ms, LPCTSTR user)
{
  char ins_cmd[] = "INSERT INTO LS_HISTORY(STATUS, PATHNAME, EXTRA, DATE, TYPE, USER) VALUES(";
  char *strc = MakeUtf8String(pathName);
  char *strt = MakeUtf8String(extra);
  char *stru = MakeUtf8String(user);
  int cmd_len = 200;
  if (strc != NULL)
    cmd_len += strlen(strc);
  if (strt != NULL)
    cmd_len += strlen(strt);
  char *cmd = new char[cmd_len];

  strcpy_s(cmd, cmd_len, ins_cmd);
  sql3_attach_arg(cmd, cmd_len, st, WITH_COMMA);
  sql3_attach_arg(cmd, cmd_len, strc, WITH_COMMA|WITH_QUOT);
  sql3_attach_arg(cmd, cmd_len, strt, WITH_COMMA|WITH_QUOT);
  sql3_attach_arg(cmd, cmd_len, time_ms, WITH_COMMA);
  sql3_attach_arg(cmd, cmd_len, type, WITH_COMMA);
  sql3_attach_arg(cmd, cmd_len, stru, WITH_QUOT);
  strcat_s(cmd, cmd_len, ");");

  if (strc != NULL)
    delete[] strc;
  if (strt != NULL)
    delete[] strt;
  if (stru != NULL)
    delete[] stru;

  char *errmsg = NULL;
  BOOL rtn = !sqlite3_exec(db, cmd, sql_callback, 0, &errmsg);
  if (errmsg != NULL)
  {
    debug_sql_msg(errmsg, cmd);
     sqlite3_free(errmsg);
  }
  delete[] cmd;
  return rtn;
}

#endif // HISTORY_BY_SQLITE

static void appendHistoryMessageEx(int state, LPCTSTR pathName, int type, LPCTSTR extra, SYSTEMTIME &systime, LPCTSTR user_id)
{
  sqlite3 *history_db = NULL;

  if (history_db == NULL)
  {
    TCHAR db_filename[260];
    MakeFullPathName(db_filename, 260, mHomeFolder, HISTORY_DB_FILENAME);

    int r = sqlite3_open16(db_filename, &history_db);
    if (r) 
    {
      TCHAR msg[400] = _T("Cannot open history db : ");
      StringCchCat(msg, 400, db_filename);
      StringCchCat(msg, 400, _T("\n"));
      OutputDebugString(msg);
      return;
    }
  }

  if (sql3_checkReadyHistoryTables(history_db) == 0)
  {
    __int64 tick = SystemTimeToMilliSecond(systime);
    if (!writeHistoryItem(history_db, state, pathName, type, extra, tick, user_id))
      OutputDebugString( _T("write history fail\n") );

    sqlite3_close(history_db);
  }
  else
  {
    OutputDebugString(_T("create history table fail\n"));
  }
}

void StoreHistory(int state, LPCTSTR path, LPCTSTR name, int type, LPCTSTR lastUser, LPCTSTR extra)
{
  int len = lstrlen(path) + lstrlen(name) + 2;
  LPTSTR pathname = new TCHAR[len];
  MakeFullPathName(pathname, len, path, name);
  StoreHistory(state, pathname, type, lastUser, extra);
  delete[] pathname;
}

void StoreHistory(int state, LPCTSTR pathname, int type, LPCTSTR lastUser, LPCTSTR extra)
{
  if (lstrlen(mHomeFolder) > 0)
  {
    SYSTEMTIME time;
    GetLocalTime(&time);

    if (lastUser == NULL)
      lastUser = mUserOid;
    appendHistoryMessageEx(state, pathname, type, extra, time, lastUser);
  }
}

int DeleteHistory(long *ids, int count)
{
  sqlite3 *history_db = NULL;
  int r = 0;

  if (history_db == NULL)
  {
    TCHAR db_filename[260];
    MakeFullPathName(db_filename, 260, mHomeFolder, HISTORY_DB_FILENAME);

    int r = sqlite3_open16(db_filename, &history_db);
    if (r) 
    {
      TCHAR msg[400] = _T("Cannot open history db : ");
      StringCchCat(msg, 400, db_filename);
      StringCchCat(msg, 400, _T("\n"));
      OutputDebugString(msg);
      return 0;
    }
  }

  if (sql3_checkReadyHistoryTables(history_db) == 0)
  {
    char cmd[100] = "DELETE FROM LS_HISTORY WHERE ID=";
    int p = (int)strlen(cmd);

    for (int i = 0; i < count; i++)
    {
      char *errmsg = NULL;
      sprintf_s(&cmd[p], 80-p, "%d;", ids[i]);
      if (sqlite3_exec(history_db, cmd, sql_callback, 0, &errmsg) == 0)
        r++;
      sqlite3_free(errmsg);
    }

    sqlite3_close(history_db);
  }
  return r;
}

int GetHistoryStateFromSyncResult(int r, int default_state)
{
  if (r & FILE_SYNC_ERR) // 0x0FF00000
    return (((r & FILE_SYNC_ERR) >> 20) + SYNC_H_SYNCERR);
  else if(r & FILE_CONFLICT) // 0x0FF000
  {
    if (r & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_MODIFIED) // 0x01000// local modified, server modified
      return SYNC_H_CONFLICT;
    else if (r & FILE_CONFLICT_LOCAL_MODIFIED_SERVER_DELETED) // 0x02000 // local modified, server deleted
      return (SYNC_H_CONFLICT + 1);
    else if (r & FILE_CONFLICT_LOCAL_DELETED_SERVER_MODIFIED) // 0x04000 // local deleted, server modified
      return (SYNC_H_CONFLICT + 2);
    else if (r & FILE_CONFLICT_LOCAL_ADDED_SERVER_ADDED) // 0x08000 // local added, server added
      return (SYNC_H_CONFLICT + 3);
  }
  return default_state;
}

void GetHistoryList(HistoryQueryCallback pCallback, void *pData, int stateMatch, LPCTSTR msgMatch, __int64 time_start, __int64 time_end, LPCTSTR userMatch, int limit, int offset)
{
    TCHAR db_filename[260];
    MakeFullPathName(db_filename, 260, mHomeFolder, HISTORY_DB_FILENAME);

    sqlite3 *history_db = NULL;
    if (sqlite3_open16(db_filename, &history_db) == 0)
    {
      char sel_cmd[100] = "SELECT * FROM LS_HISTORY ORDER BY DATE DESC";

      if (limit > 0)
      {
        char buff[16];

        strcat_s(sel_cmd, 100, " LIMIT ");
        sprintf_s(buff, 16, "%d", limit);
        strcat_s(sel_cmd, 100, buff);

        if (offset > 0)
        {
          strcat_s(sel_cmd, 100, " OFFSET ");
          sprintf_s(buff, 16, "%d", offset);
          strcat_s(sel_cmd, 100, buff);
        }
      }

      const char *pTail;
      sqlite3_stmt *pStatement;
      if ((sqlite3_prepare_v2(history_db, sel_cmd, -1, &pStatement, &pTail) == SQLITE_OK) &&
        (pStatement != NULL))
      {
        int next = 0;

        do {
          next = sqlite3_step(pStatement);
          if (next == SQLITE_BUSY)
          {
            Sleep(10);
            continue;
          }
          else if (next == SQLITE_ROW)
          {
            long id = sqlite3_column_int(pStatement, 0);
            int state = sqlite3_column_int(pStatement, 1);
            LPTSTR pathname = (LPTSTR)sqlite3_column_text16(pStatement, 2);
            LPTSTR ext = (LPTSTR)sqlite3_column_text16(pStatement, 3);
            __int64 date = (__int64)sqlite3_column_int64(pStatement, 4);
            int type = sqlite3_column_int(pStatement, 5);
            LPTSTR user = (LPTSTR)sqlite3_column_text16(pStatement, 6);

            SYSTEMTIME systime;
            memset(&systime, 0, sizeof(systime));
            if (date != 0)
              MilliSecondToSystemTime(date, systime);

            LRESULT r = pCallback(id, state, pathname, type, ext, systime, user, pData);

            // no need to free, it automatically freed in sqlite3_finalize()
            /*
            if (context != NULL)
              sqlite3_free(context);
            if (target != NULL)
              sqlite3_free(target);
            if (user != NULL)
              sqlite3_free(user);
            */
            if (r == 0)
              break;
          }
        } while((next == SQLITE_BUSY) || (next == SQLITE_ROW));

        sqlite3_finalize(pStatement);
      }
    sqlite3_close(history_db);
    }
}

void GetHistoryStatusName(LPTSTR buff, int length, int state)
{
  if (state < SYNC_H_ETC)
  {
    int string_rid = IDS_HS_SET_SYNC_FOLDER + (state - SYNC_H_SET_SYNCFOLDER);
    CString name;
    name.LoadString(AfxGetInstanceHandle(), string_rid, mResourceLanguage);

    if (string_rid = IDS_HS_NO_PERMISSION_ECM_DISK) // "%s 접근 권한 없음"
      StringCchPrintf(buff, length, name, gECMDriveName);
    else if (string_rid = IDS_HS_NO_PERMISSION_SECURITY_DISK) // "%s 접근 권한 없음"
      StringCchPrintf(buff, length, name, gSecurityDriveName);
    else
      StringCchCopy(buff, length, name.GetBuffer());
  }
  else
  {
    StringCchCopy(buff, length, _T("Unknown"));
  }

  /*
  if (state >= SYNC_H_ETC)
    ;
  else if (state >= SYNC_H_SYNCERR)
  {
    string_rid = IDS_SYNCERR_UPLOAD_FAIL + (state - SYNC_H_SYNCERR);
  }
  else if(state >= SYNC_H_CONFLICT)
  {
    string_rid = IDS_SYNC_CONFLICT1 + (state - SYNC_H_CONFLICT);
  }
  else
  {
    switch(state)
    {
    case SYNC_H_SET_SYNCFOLDER:
      string_rid = IDS_SET_SYNC_FOLDER;
      break;
    case SYNC_H_CLEAR_SYNCFOLDER:
      string_rid = IDS_CLEAR_SYNC_FOLDER;
      break;
    case SYNC_H_SYNCFOLDER_PROPERTY:
      string_rid = IDS_SYNCFOLDER_PROPERTY;
      break;
    case SYNC_H_SYNC_EXCLUDE_FOLDER:
      string_rid = IDS_SYNCFOLDER_EXCLUDE;
      break;
    case SYNC_H_SYNC_INCLUDE_FOLDER:
      string_rid = IDS_SYNCFOLDER_INCLUDE;
      break;
    case SYNC_H_START_MANUAL_SYNC:
    case SYNC_H_START_AUTO_SYNC:
      string_rid = IDS_SYNCFOLDER_BEGINSYNC;
      break;
    case SYNC_H_END_SYNC:
      string_rid = IDS_SYNCFOLDER_ENDSYNC;
      break;

    case SYNC_H_UPLOAD:
      string_rid = IDS_SYNC_UPLOAD;
      break;
    case SYNC_H_DOWNLOAD:
      string_rid = IDS_SYNC_UPLOAD;
      break;
    case SYNC_H_CREATE_LOCAL_FOLDER:
      string_rid = IDS_SYNC_CREATE_LOCAL_FOLDER;
      break;
    case SYNC_H_CREATE_SERVER_FOLDER:
      string_rid = IDS_SYNC_CREATE_SERVER_FOLDER;
      break;
    case SYNC_H_RENAME_LOCAL_FILE:
      string_rid = IDS_SYNC_RENAME_LOCAL_FILE;
      break;
    case SYNC_H_RENAME_SERVER_FILE:
      string_rid = IDS_SYNC_RENAME_SERVER_FILE;
      break;
    case SYNC_H_RENAME_LOCAL_FOLDER:
      string_rid = IDS_SYNC_RENAME_LOCAL_FOLDER;
      break;
    case SYNC_H_RENAME_SERVER_FOLDER:
      string_rid = IDS_SYNC_RENAME_SERVER_FOLDER;
      break;
    case SYNC_H_DELETE_LOCAL_FILE:
      string_rid = IDS_SYNC_DELETE_LOCAL_FILE;
      break;
    case SYNC_H_DELETE_SERVER_FILE:
      string_rid = IDS_SYNC_DELETE_SERVER_FILE;
      break;
    case SYNC_H_DELETE_LOCAL_FOLDER:
      string_rid = IDS_SYNC_DELETE_LOCAL_FOLDER;
      break;
    case SYNC_H_DELETE_SERVER_FOLDER:
      string_rid = IDS_SYNC_DELETE_SERVER_FOLDER;
      break;

    default:
      StringCchCopy(buff, length, _T("Unknown"));
      break;
    }
  }

  if (string_rid > 0)
  {
    CString name;
    name.LoadString(AfxGetInstanceHandle(), string_rid, mResourceLanguage);
    StringCchCopy(buff, length, name.GetBuffer());
  }
  */
}

#ifndef HISTORY_BY_SQLITE

void AppendHistoryMessage(TCHAR *buff, SYSTEMTIME &systime)
{
  if (lstrlen(mHomeFolder) > 0)
  {
    TCHAR pathname[KMAX_PATH];

	  MakeFullPathName(pathname, KMAX_PATH, mHomeFolder, HISTORY_FOLDERNAME);
	  if (IsDirectoryExist(pathname) || CreateDirectory(pathname))
	  {
      TCHAR filename[KMAX_PATH];
      StringCchPrintf(filename, KMAX_PATH, HISTORY_FILENAME_FORMAT,
        systime.wYear, systime.wMonth, systime.wDay);
  	  MakeFullPathName(pathname, KMAX_PATH, pathname, filename);
      AppendMessageToFile(pathname, buff, FALSE);
    }
  }
}

#endif // HISTORY_BY_SQLITE

#endif // USE_SYNC_ENGINE

void StoreLogHistory(LPCTSTR module_name, HRESULT hr, DWORD type)
{
  TCHAR msg[256];

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, hr,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    (LPTSTR)msg, 256, NULL);
  StoreLogHistory(module_name, msg, SYNC_MSG_LOG | SYNC_MSG_DEBUG);
}

void StoreLogHistory(LPCTSTR module_name, LPCTSTR msg, DWORD type)
{
  TCHAR *buff;
  char time_str[64];
  int buff_len = lstrlen(msg) + 200;

  buff = new TCHAR[buff_len];
  SYSTEMTIME time;
  GetLocalTime(&time);
  SystemTimeToStringEx(time_str, 64, time); // HH:MM:SS:MSC
  ToUnicodeString(buff, buff_len, time_str, (int)strlen(time_str));

  StringCchCat(buff, buff_len, L",");
  StringCchCat(buff, buff_len, CURRENT_MODULE_NAME);
  StringCchCat(buff, buff_len, L"::");
  StringCchCat(buff, buff_len, module_name);
  
  // system error or warning
  if (type & SYNC_EVENT_ERROR)
    StringCchCat(buff, buff_len, L"[?ER]");
  if (type & SYNC_EVENT_WARN)
    StringCchCat(buff, buff_len, L"[?WN]");

  StringCchCat(buff, buff_len, L",");
  StringCchCat(buff, buff_len, msg);
  StringCchCat(buff, buff_len, L_CRLF_STR);

  if (type & SYNC_MSG_SERVICE_LOG)
  {
    // Log/Service폴더에 로그 저장
    AppendLogMessageEx(buff, time, LOG_EX_SUBFOLDER_NAME, TRUE);
  }
  else if (type & SYNC_MSG_LOG)
  {
    AppendLogMessage(buff, time);
  }

#ifndef HISTORY_BY_SQLITE
  if (type & SYNC_MSG_HISTORY)
  {
#ifdef USE_SYNC_ENGINE
    AppendHistoryMessage(buff, time);
#endif
  }
#endif

#ifdef _DEBUG
  if (TRUE)
#else
  if (!(type & SYNC_MSG_DEBUG))
#endif
  {
    CMainFrame *pMain = (CMainFrame*) AfxGetMainWnd();
    if ((pMain != NULL) && !(type & SYNC_MSG_NO_GUI))
    {
      pMain->AppendOutputString(buff);
      /*
      int line_len = lstrlen(buff);
      buff[line_len-1] = 0x0D;
      buff[line_len] = 0x0A;
      buff[line_len+1] = 0x0;
      pMain->PostMessage(WM_LOG_MESSAGE, type, (LPARAM)buff);
      buff = NULL;
      */
    }
  }
  if (buff != NULL)
    delete[] buff;
}

void GetDefaultSettingsFilename(TCHAR *filename)
{
#if defined(USE_LOCALSYNC)
  MakeFullPathName(filename, KMAX_PATH, DESTINY_HOME_FOLDER, SYNC_ROOT_FOLDER_SETTING_FILENAME);
#endif
}

static int removeHistoryItem(TCHAR *filename, char *linestr)
{
  size_t linelen = strlen(linestr);
  size_t result_size = 0;
  char *buff = NULL;
  int rtn = 0;

  /*
  FILE *f = NULL;
  errno_t err = _tfopen_s(&f, filename, L"r");
	if (err == 0)
  */
  FILE *f = OpenDelayedFile(filename, _T("r"));
  if (f != NULL)
  {
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (size > 0)
    {
      buff = new char[size + 2];
      if (buff != NULL)
      {
        size = fread(buff, 1, size, f);
        buff[size] = '\0';
        char *p = buff;
        char *e = buff + size;
        while(p < e)
        {
          char *n = strchr(p, LF_CHAR);

          if (strncmp(p, linestr, linelen) == 0)
          {
            if (n != NULL)
            {
              size -= (n + 1 - p);
              if (size > 0)
              {
                result_size += size;
                memcpy(p, n + 1, size);
              }
              n++;
            }
            rtn = 1;
            break;
          }
          if (n != NULL)
          {
            result_size += (n + 1 - p);
            size -= (n + 1 - p);
            p = n + 1;
          }
          else
            p = e;
        }
      }
    }
  fclose(f);
  }

  /*
  err = _tfopen_s(&f, filename, L"w");
	if (err == 0)
  */
  if (rtn)
  {
    MakeWritabeFile(filename);
    f = OpenDelayedFile(filename, _T("w"));
    if (f != NULL)
    {
      if (result_size > 0)
        fwrite(buff, 1, result_size, f);
    fclose(f);
    }
  }

  if (buff != NULL)
    delete[] buff;
  return rtn;
}

int RemoveFromHistoryFile(SYSTEMTIME &systime, char *location, char *path, char *name, int result)
{
#ifdef HISTORY_BY_SQLITE
  return 0;
#else
  TCHAR filename[KMAX_PATH];
  TCHAR date[64];
	MakeFullPathName(filename, KMAX_PATH, mHomeFolder, HISTORY_FOLDERNAME);
  StringCchPrintf(date, 64, HISTORY_FILENAME_FORMAT, systime.wYear, systime.wMonth, systime.wDay);
	MakeFullPathName(filename, KMAX_PATH, filename, date);

  char linestr[512];

  SystemTimeToStringEx(linestr, 256, systime); // HH:MM:SS:MSC

  strcat_s(linestr, 512, ",");
  strcat_s(linestr, 512, location);

  if (result & FILE_NEED_DOWNLOAD)
    strcat_s(linestr, 512, ",down");
  else
    strcat_s(linestr, 512, ",up");
  strcat_s(linestr, 512, "load_done");

  strcat_s(linestr, 512, ",path=");
  strcat_s(linestr, 512, path);

  strcat_s(linestr, 512, ",name=");
  strcat_s(linestr, 512, name);

  int n = removeHistoryItem(filename, linestr);
#ifdef _DEBUG
  if (n == 0)
  {
    OutputDebugString(_T("cannot found history item ["));
    TCHAR msg[600];
    ToUnicodeString(msg, 600, linestr, (int)strlen(linestr));
    OutputDebugString(msg);
    OutputDebugString(_T("]\n"));
  }
#endif
  return n;
#endif
}

