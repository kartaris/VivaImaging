#include "StdAfx.h"
#include <tchar.h>
#include "KSyncPolicy.h"
#include "KSettings.h"
#include "KConstant.h"
#include "KSyncItem.h"

#include "Platform/Platform.h"

#include <Strsafe.h>
#include <Shlwapi.h>

#ifdef _DEBUG_MEM
#define new DEBUG_NEW
#endif

KSyncPolicy::KSyncPolicy(void)
{
  mSyncMethod = SYNC_METHOD_MANUAL; // manual, auto-repeat, auto-realtime
  mRepeatType = SYNC_REPEAT_HOURLY; // weekly, daily, hourly
  mRepeatDatesInWeek = 0; // sync date weekly(sun=0x01,mon=0x02,tues=0x04
  mRepeatEveryNDays = 1; // sync every number of dates(1 ~ 30)
  mRepeatEveryNHours = 1; // sync at every 1 hour, 2 hour, 3 hour,,, 23 hour
  mRepeatEveryNMinutes = 30; // sync at every 10 minute, 30 minute
  mSyncTimeAt = 9 * 60;
#ifdef USE_SYNC_DURATION
  mSyncTimeEnd = 18 * 60;
#endif
  mExcludeMasks = NULL;
  mSyncFlag = SYNC_ENABLE_UPLOAD | SYNC_ENABLE_DOWNLOAD; // rights for upload, download
  mPaused = FALSE;

  // mSystemExcludeMasks = NULL;

  /*
  TCHAR masks[] = _T("*.jpg,a*.m*,?ab,*t*.abc");
  LPTSTR names[] = { _T("star.JPG"), _T("abc.map"), _T("ABC.1"), _T("suta.abc"), _T("BUT.abc") };

  for (int i = 0; i < 5; i++)
  {
    BOOL r = IsMatchFileMask(masks, names[i]);
    TCHAR msg[256];
    StringCchPrintf(msg, 256, _T("Mask=%s : filename=%s : result=%d\n"), masks, names[i], r);
    OutputDebugString(msg);
  }
  */
#ifdef _DEBUG_NAME_MATCH
  LPTSTR mask_test[5] = {
    _T("~#*.*"),
    _T("~?abc.*"),
    _T("~?.*"),
    _T("~*.*"),
    _T("~*.do?"),
  };

  LPTSTR name_test[5] = {
    _T("~$abc.doc"),
    _T("~#abc.docx"),
    _T("~~abc.doc"),
    _T("~#.dox"),
    _T("#abc.doc"),
  };

  TCHAR msg[256];
  for(int j = 0; j < 5; j++)
  for(int i = 0; i < 5; i++)
  {
    StringCchPrintf(msg, 256, _T("Mask=%s, Filename=%s, Result=%d\n"), mask_test[j], name_test[i],
      IsNameMatching(mask_test[j], name_test[i]));
    OutputDebugString(msg);
  }
  BOOL r = IsNameMatching(mask_test[1], name_test[1]);
  OutputDebugString(_T("Done\n"));
#endif
}

KSyncPolicy::~KSyncPolicy(void)
{
  Clear();
}

void KSyncPolicy::Clear()
{
  mSyncMethod = SYNC_METHOD_MANUAL; // manual, auto-repeat, auto-realtime
  mRepeatType = SYNC_REPEAT_HOURLY; // weekly, daily, hourly
  mRepeatDatesInWeek = 0; // sync date weekly(sun=0x01,mon=0x02,tues=0x04
  mRepeatEveryNDays = 1; // sync every number of dates(1 ~ 30)
  mRepeatEveryNHours = 1; // sync at every 1 hour, 2 hour, 3 hour,,, 23 hour
  mRepeatEveryNMinutes = 30; // sync at every 10 minute, 30 minute
  mSyncTimeAt = 12 * 60;
  mSyncFlag = SYNC_ENABLE_UPLOAD | SYNC_ENABLE_DOWNLOAD; // rights for upload, download
  mPaused = FALSE;
  if (mExcludeMasks != NULL)
  {
    delete[] mExcludeMasks;
    mExcludeMasks = NULL;
  }
}

KSyncPolicy& KSyncPolicy::operator=(KSyncPolicy &policy)
{
  if (&policy != this)
  {
    mSyncMethod = policy.mSyncMethod;
    mRepeatType = policy.mRepeatType;
    mRepeatDatesInWeek = policy.mRepeatDatesInWeek;
    mRepeatEveryNDays = policy.mRepeatEveryNDays;
    mRepeatEveryNHours = policy.mRepeatEveryNHours;
    mRepeatEveryNMinutes = policy.mRepeatEveryNMinutes;
    mSyncTimeAt = policy.mSyncTimeAt;
    SetExcludeMasks(policy.mExcludeMasks);
    mSyncFlag = policy.mSyncFlag;
    mPaused = policy.mPaused;
  }
  return *this;
}

BOOL KSyncPolicy::MaskRepeatDatesInWeek(int date, BOOL set)
{
  int old_value = mRepeatDatesInWeek;
  if (set)
    mRepeatDatesInWeek |= date;
  else
    mRepeatDatesInWeek &= ~date;
  return(old_value != mRepeatDatesInWeek);
}

#if 0
TCHAR KSyncPolicy::defaultServerExcludeMasks[512] = _T("");

LPTSTR KSyncPolicy::GetDefaultMasks()
{
  char str[512];

  GetSettingKeyValue(KEY_DEFAULT_SYSTEM_EXCLUDE_MASKS, str, 512);
  ToUnicodeString(defaultServerExcludeMasks, 512, str, (int)strlen(str));
  return defaultServerExcludeMasks;
}
#endif

/**
BOOL IsMatching(LPCTSTR pattern, LPCTSTR str)
{
  LPTSTR p = StrChr(pattern, '*');
  if (p == NULL)
    return (StrCmp(pattern, str) == 0);

  int len = p - pattern;
  if (len > 0)
  {
    if (StrNCmp(pattern, str, len) != 0)
      return FALSE;
  }
  ++p;
  len = lstrlen(pattern) - (p - pattern);
  if (len > 0)
  {
    if (lstrlen(str) >= len)
    {
      if (lstrlen(str) > len)
        str += (lstrlen(str) - len);

      if (StrNCmp(p, str, len) != 0)
        return FALSE;
    }
    else
      return FALSE;
  }
  return TRUE;
}
**/

BOOL wildCharMatch(LPCTSTR wild, LPCTSTR string)
{
  LPCTSTR cp = NULL;
  LPCTSTR mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != '?') && (toupper(*wild) != toupper(*string))) {
      return FALSE;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return TRUE;
      }
      mp = wild;
      cp = string+1;
    } else if ((toupper(*wild) == toupper(*string)) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

BOOL KSyncPolicy::IsNameMatching(LPCTSTR pattern, LPCTSTR name)
{
  TCHAR mask[KMAX_PATH];
  StringCchCopy(mask, KMAX_PATH, pattern);
  TCHAR src[KMAX_PATH];
  StringCchCopy(src, KMAX_PATH, name);

  LPTSTR m = StrRChr(mask, NULL, '.');
  if (m != NULL)
    *m = '\0';
  LPTSTR s = StrRChr(src, NULL, '.');
  if (s != NULL)
    *s = '\0';
  if (wildCharMatch(mask, src))
  {
    if (m == NULL)
      return TRUE;

    m++;
    if (lstrlen(m) > 0)
    {
      if (s != NULL)
      {
        s++;
        if (wildCharMatch(m, s))
          return TRUE;
      }
      return FALSE;
    }
    if ((s == NULL) || (lstrlen(s) == 0))
      return TRUE;
  }
  return FALSE;
}

// convert "*.exe/*.jpg/*.mp3" as "*.exe|*.jpg|*.mp3"
LPTSTR KSyncPolicy::UpdateSyncExcludeMasks(LPTSTR buff)
{
  LPTSTR newMask = NULL;
  if (buff != NULL)
  {
    size_t cchBuff = lstrlen(buff) + 1;
    if (cchBuff > 1)
    {
      newMask = new TCHAR[cchBuff];
      StringCchCopy(newMask, cchBuff, buff);
      TrimStr(newMask);
      ReplaceChar(newMask, '/', '|');
    }
  }

  return newMask;
}

BOOL KSyncPolicy::IsMatchFileMask(LPCTSTR masks, LPCTSTR name)
{
  const TCHAR *p = masks;
  TCHAR str[KMAX_PATH];

  while(*p != '\0')
  {
    TCHAR *n = StrChr(p, DA_VALUE_SEPARATOR);
    if (n != NULL)
    {
      _tcsncpy_s(str, p, (n - p));
      p = n+1;
    }
    else
    {
      _tcscpy_s(str, p);
      p += lstrlen(p);
    }

    if (IsNameMatching(str, name))
      return TRUE;
  }
  return FALSE;
}

BOOL KSyncPolicy::IsExistFileMask(LPCTSTR masks, LPCTSTR name)
{
  const TCHAR *p = masks;
  int len;
  int name_len = lstrlen(name);

  while(*p != '\0')
  {
    const TCHAR *n = StrChr(p, DA_VALUE_SEPARATOR);
    if (n != NULL)
    {
      len = (int)(n - p);
      ++n;
    }
    else
    {
      len = lstrlen(p);
      n = p + len;
    }

    if ((len == name_len) && (StrCmpNI(p, name, len) == 0))
      return TRUE;
    p = n;
  }
  return FALSE;
}

BOOL KSyncPolicy::IsExcludeFileMask(LPCTSTR name)
{
  if ((mExcludeMasks != NULL) && (lstrlen(mExcludeMasks) > 0))
  {
    if (IsMatchFileMask(mExcludeMasks, name))
      return TRUE;
  }
  return FALSE;
}

BOOL KSyncPolicy::IsExcludeFileMask(KSyncItem* item)
{
  if ((mExcludeMasks != NULL) && (lstrlen(mExcludeMasks) > 0))
  {
    if (IsMatchFileMask(mExcludeMasks, item->GetFilename()))
      return TRUE;

    BOOL excluded = FALSE;
    LPTSTR pathName = item->AllocRelativePathName();
    if (lstrlen(pathName) > lstrlen(item->GetFilename()))
      excluded = IsExcludeFileMask(pathName);
    delete[] pathName;
    return excluded;
  }
  return FALSE;
}

BOOL KSyncPolicy::IsExistExcludeFileMask(LPCTSTR mask)
{
  if ((mExcludeMasks != NULL) && (lstrlen(mExcludeMasks) > 0))
  {
    if (IsExistFileMask(mExcludeMasks, mask))
      return TRUE;
  }
  return FALSE;
}

void KSyncPolicy::SetExcludeMasks(LPCTSTR masks)
{
  if (mExcludeMasks != NULL)
  {
    delete[] mExcludeMasks;
    mExcludeMasks = NULL;
  }

  if ((masks != NULL) && (lstrlen(masks) > 0))
  {
    int length = lstrlen(masks) + 1;
    mExcludeMasks = new TCHAR[length];
    StringCchCopy(mExcludeMasks, sizeof(TCHAR) * (length), masks);
  }
}

BOOL KSyncPolicy::AddExcludeMask(LPCTSTR masks)
{
  int len = lstrlen(masks) + lstrlen(mExcludeMasks) + 2;
  LPTSTR nMasks = new TCHAR[len];
  LPTSTR p = nMasks;
  if ((mExcludeMasks != NULL) && (lstrlen(mExcludeMasks) > 0))
  {
    StringCchCopy(p, len, mExcludeMasks);
    p += lstrlen(mExcludeMasks);
    *(p++) = DA_VALUE_SEPARATOR;
  }
  StringCchCopy(p, len, masks);
  if (mExcludeMasks != NULL)
    delete[] mExcludeMasks;
  mExcludeMasks = nMasks;
  return TRUE;
}

void KSyncPolicy::StoreDefaultSettingFile(LPTSTR filename)
{
  char str[64];

  sprintf_s(str, 64, "%d", SYNC_METHOD_MANUAL);
  StoreSettingKeyValue(filename, SYNC_POLICY_METHOD, str);

#ifdef USE_BACKUP
  char ex_mask[] = "";
#else
  char ex_mask[] = "*.lnk|~*.*";
#endif
  StoreSettingKeyValue(filename, SYNC_POLICY_EXCLUDES, ex_mask);
}

void KSyncPolicy::SetDefaultPolicy()
{
  Clear();
#ifndef USE_BACKUP
  SetExcludeMasks(_T("*.lnk|~*.*"));
#endif
}

#if 0
#include "../SqliteLib/sqlite3.h"
#include "KSqliteUtil.h"

BOOL KSyncPolicy::Store(sqlite3 *db)
{
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_METHOD, mSyncMethod);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_REPEAT_TYPE, mRepeatType);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_DATES_IN_WEEK, mRepeatDatesInWeek);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_EVERY_N_DAYS, mRepeatEveryNDays);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_EVERY_N_HOURS, mRepeatEveryNHours);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_EVERY_N_MINUTES, mRepeatEveryNMinutes);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_TIMEAT, mSyncTimeAt);
#ifdef USE_SYNC_DURATION
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_TIMEEND, mSyncTimeEnd);
#endif

  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_FLAG, mSyncFlag);
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_EXCLUDES, mExcludeMasks);

  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_PAUSED, mPaused);

#ifndef USE_BACKUP
  // store setting-time when policy changed
  TCHAR buff[32] = { 0 };
  SYSTEMTIME time;
  GetLocalTime(&time);
  SystemTimeToTString(buff, 32, time);
  sql3_StoreSettingKeyValue(db, KEYB_LAST_SETTING, buff);
#endif
}

BOOL KSyncPolicy::Load(sqlite3 *db)
{
  char sel_cmd[100] = "SELECT * FROM LS_SETTING WHERE KEY > ";
  const char *pTail;
  sqlite3_stmt *pStatement;

  Clear();

  sql3_attach_arg(sel_cmd, 100, KEYB_SYNC_POLICY_METHOD, 0);
  if ((sqlite3_prepare_v2(db, sel_cmd, -1, &pStatement, &pTail) == SQLITE_OK) &&
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
        int key = sqlite3_column_int(pStatement, 0);
        LPSTR context = (LPSTR)sqlite3_column_text(pStatement, 1);

        if (key == KEYB_SYNC_POLICY_METHOD)
          sscanf_s(context, "%d", &mSyncMethod);
        else if (key == KEYB_SYNC_POLICY_REPEAT_TYPE)
          sscanf_s(context, "%d", &mRepeatType);
        else if (key == KEYB_SYNC_POLICY_DATES_IN_WEEK)
          sscanf_s(context, "%d", &mRepeatDatesInWeek);
        else if (key == KEYB_SYNC_POLICY_EVERY_N_DAYS)
          sscanf_s(context, "%d", &mRepeatEveryNDays);
        else if (key == KEYB_SYNC_POLICY_EVERY_N_HOURS)
          sscanf_s(context, "%d", &mRepeatEveryNHours);
        else if (key == KEYB_SYNC_POLICY_EVERY_N_MINUTES)
          sscanf_s(context, "%d", &mRepeatEveryNMinutes);
        else if (key == KEYB_SYNC_POLICY_TIMEAT)
          sscanf_s(context, "%d", &mSyncTimeAt);
        else if (key == KEYB_SYNC_POLICY_FLAG)
          sscanf_s(context, "%d", &mSyncFlag);
        else if (key == KEYB_SYNC_POLICY_EXCLUDES)
          mExcludeMasks = MakeUnicodeString(context);
        else if (key == KEYB_SYNC_POLICY_PAUSED)
          sscanf_s(context, "%d", &mPaused);
      }
    } while((next == SQLITE_BUSY) || (next == SQLITE_ROW));

    sqlite3_finalize(pStatement);
  }
  return TRUE;
}

BOOL KSyncPolicy::StorePaused(sqlite3 *db)
{
  sql3_StoreSettingKeyValue(db, KEYB_SYNC_POLICY_PAUSED, mPaused);
  return TRUE;
}
#endif

BOOL KSyncPolicy::Store(LPTSTR filename)
{
  char str[64];

  sprintf_s(str, 64, "%d", mSyncMethod);
  StoreSettingKeyValue(filename, SYNC_POLICY_METHOD, str);
  sprintf_s(str, 64, "%d", mRepeatType);
  StoreSettingKeyValue(filename, SYNC_POLICY_REPEAT_TYPE, str);
  sprintf_s(str, 64, "%d", mRepeatDatesInWeek);
  StoreSettingKeyValue(filename, SYNC_POLICY_DATES_IN_WEEK, str);
  sprintf_s(str, 64, "%d", mRepeatEveryNDays);
  StoreSettingKeyValue(filename, SYNC_POLICY_EVERY_N_DAYS, str);
  sprintf_s(str, 64, "%d", mRepeatEveryNHours);
  StoreSettingKeyValue(filename, SYNC_POLICY_EVERY_N_HOURS, str);
  sprintf_s(str, 64, "%d", mRepeatEveryNMinutes);
  StoreSettingKeyValue(filename, SYNC_POLICY_EVERY_N_MINUTES, str);
  sprintf_s(str, 64, "%d", mSyncFlag);
  StoreSettingKeyValue(filename, SYNC_POLICY_FLAG, str);
  sprintf_s(str, 64, "%d", mSyncTimeAt);
  StoreSettingKeyValue(filename, SYNC_POLICY_TIMEAT, str);

  if ((mExcludeMasks != NULL) && (lstrlen(mExcludeMasks) > 0))
  {
    char *buff = MakeUtf8String(mExcludeMasks);
    /*
    int len = ToUtf8String(NULL, 0, mExcludeMasks, lstrlen(mExcludeMasks));
    char *buff = new char[len + 2];
    ToUtf8String(buff, len+1, mExcludeMasks, lstrlen(mExcludeMasks));
    */
    StoreSettingKeyValue(filename, SYNC_POLICY_EXCLUDES, buff);
    if (buff != NULL)
      delete[] buff;
  }
  else
  {
    StoreSettingKeyValue(filename, SYNC_POLICY_EXCLUDES, "");
  }

  sprintf_s(str, 64, "%d", (int)mPaused);
  StoreSettingKeyValue(filename, SYNC_POLICY_PAUSED, str);

  // store setting-time when policy changed
  char buff[32] = { 0 };
  SYSTEMTIME time;
  GetLocalTime(&time);
  SystemTimeToString(buff, 32, time);
  StoreSettingKeyValue(filename, KEY_LAST_SETTING, buff);

  return TRUE;
}

BOOL KSyncPolicy::Load(LPTSTR filename)
{
  int value;
  char str[1024];
  BOOL r = FALSE;

  if (ReadSettingKeyValue(filename, SYNC_POLICY_METHOD, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mSyncMethod = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_REPEAT_TYPE, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mRepeatType = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_DATES_IN_WEEK, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mRepeatDatesInWeek = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_EVERY_N_DAYS, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mRepeatEveryNDays = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_EVERY_N_HOURS, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mRepeatEveryNHours = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_EVERY_N_MINUTES, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mRepeatEveryNMinutes = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_FLAG, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mSyncFlag = value;
      r = TRUE;
    }
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_TIMEAT, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mSyncTimeAt = value;
      r = TRUE;
    }
  }
  if (mExcludeMasks != NULL)
  {
    delete[] mExcludeMasks;
    mExcludeMasks = NULL;
  }

  if (ReadSettingKeyValue(filename, SYNC_POLICY_EXCLUDES, str, 1024))
  {
    int len = ToUnicodeString(NULL, 0, str, (int)strlen(str));
    mExcludeMasks = new TCHAR[len + 1];
    ToUnicodeString(mExcludeMasks, len+1, str, (int)strlen(str));
    r = TRUE;
  }
  if (ReadSettingKeyValue(filename, SYNC_POLICY_PAUSED, str, 256))
  {
    if (sscanf_s(str, "%d", &value) == 1)
    {
      mPaused = (BOOL)value;
      r = TRUE;
    }
  }
  return r;
}

BOOL KSyncPolicy::StorePaused(LPTSTR filename)
{
  char str[8];
  sprintf_s(str, 8, "%d", (int)mPaused);
  StoreSettingKeyValue(filename, SYNC_POLICY_PAUSED, str);
  return TRUE;
}

static BOOL isSameDate(SYSTEMTIME &d, SYSTEMTIME &s)
{
  return ((d.wYear == s.wYear) &&
    (d.wMonth == s.wMonth) &&
    (d.wDay == s.wDay));
}

static BOOL isSameDateHM(SYSTEMTIME &d, SYSTEMTIME &s)
{
  return ((d.wYear == s.wYear) &&
    (d.wMonth == s.wMonth) &&
    (d.wDay == s.wDay) &&
    (d.wHour == s.wHour) &&
    (d.wMinute == s.wMinute));
}

void KSyncPolicy::calcSyncStartMinute(SYSTEMTIME OUT &at, SYSTEMTIME IN &current, int timeAt)
{
#ifdef USE_SYNC_DURATION
  int duratio = 0;
  if (isSameDate(at, current))
  {
    int curr_min = current.wHour * 60 + current.wMinute;
    if (curr_min < timeAt)
      curr_min = timeAt;
    duratio = (curr_min < mSyncTimeEnd) ? (mSyncTimeEnd - curr_min) : 0;
    timeAt = curr_min;
  }
  else
  {
    duratio = mSyncTimeEnd - timeAt;
  }
  if (duratio > 0)
  {
    // 전체 구간을 랜덤하게 설정.
    // duratio = (int) ((double)duratio * 0.5); // 절반 나눔 사용 안함.

    duratio--; // 마지막 분에 걸리면 패스될 수 있으니, -1 한다.
    if (duratio > 1)
      timeAt += (rand() % duratio);
    if (timeAt >= mSyncTimeEnd)
      timeAt = mSyncTimeEnd - 1;
  }
#endif

  at.wHour = timeAt / 60;
  at.wMinute = timeAt % 60;
}

#ifdef USE_SYNC_DURATION
BOOL KSyncPolicy::IsMiddleOfTime(int minutes_of_day)
{
  // 지정한 시-분 값이 시간구간 안쪽이면 TRUE
  return ((mSyncTimeAt <= minutes_of_day) && (minutes_of_day <= mSyncTimeEnd));
}

BOOL KSyncPolicy::IsTimeOver(int minutes_of_day)
{
  // 지정한 시-분 값이 시간구간 안쪽이면 TRUE
  return (minutes_of_day > mSyncTimeEnd);
}
#endif

/*
last : last sync time.
base : MIN(login time, setting changed time)
*/
BOOL KSyncPolicy::CalcNextSyncTime(SYSTEMTIME OUT &at, SYSTEMTIME IN &last, SYSTEMTIME IN &base)
{
  if ((mSyncMethod == SYNC_METHOD_AUTO) && !mPaused)
  {
    SYSTEMTIME current;
    GetLocalTime(&current);

#ifdef USE_BACKUP
    memcpy(&at, &base, sizeof(base));
    // 백업은 last 또는 base 기준으로 시작한다
#else
    memcpy(&at, &current, sizeof(current));
#endif

    int cur_min = current.wHour * 60 + current.wMinute;
    int last_min = last.wHour * 60 + last.wMinute;
    int base_min = base.wHour * 60 + base.wMinute;

    if (mRepeatType == SYNC_REPEAT_WEEKLY)
    {
      // current.wDayOfWeek : 0:sun, 1:mon,,,
      int today_mask = 1 << current.wDayOfWeek;
      int date_offset = -1;
      for (int i = 0; i < 8; i++)
      {
        if (i == 0)
        {
          if (isSameDate(last, current) &&  // 이전 동기화 시간이 오늘이고,
            // ((cur_min == mSyncTimeAt) && (last_min == cur_min)) || // 방금 처리했거나
              ((last_min >= mSyncTimeAt) || // 이미 처리했거나
#ifdef USE_SYNC_DURATION
              (base_min > mSyncTimeEnd)) ) // 시간이 지나서 로그인했으면 다음날로(펜딩되어 시간 지난것도 포함되도록)
#else
              (base_min > mSyncTimeAt)) ) // 시간이 지나서 로그인했으면 다음날로(펜딩되어 시간 지난것도 포함되도록)
#endif
          goto next_day_of_week;
        }

        if (mRepeatDatesInWeek & today_mask)
        {
          date_offset = i;
          break;
        }
      next_day_of_week:
        today_mask = today_mask << 1;
        if (today_mask == 0x80)
          today_mask = 0x01;
      }
      if (date_offset < 0)
        return FALSE;
      if (date_offset > 0)
        AddSystemTime(at, at, date_offset, 0, 0);

      //at.wHour = mSyncTimeAt / 60;
      //at.wMinute = mSyncTimeAt % 60;
      calcSyncStartMinute(at, current, mSyncTimeAt);

      at.wSecond = 0;
      return TRUE;
    }
    else if (mRepeatType == SYNC_REPEAT_DAILY)
    {
#ifndef USE_BACKUP //백업에서는 바로 시작.
      if (mRepeatEveryNDays > 1)
      {
        SYSTEMTIME time = { 0, 0 };

        if (IsValidSystemTime(last))
          AddSystemTime(at, last, mRepeatEveryNDays, 0, 0);
        else if (IsValidSystemTime(base))
          AddSystemTime(at, base, mRepeatEveryNDays - 1, 0, 0);
        else
          AddSystemTime(at, current, mRepeatEveryNDays - 1, 0, 0);
      }

      if (!isSameDate(last, current)) // 마지막 처리일이 오늘이 아니면
        last_min = -1;
      if (!isSameDate(base, current)) // 기준일이 오늘이 아니면
        base_min = -1;

      while (CompareSystemTime(current, at) > 0) // if past, 
        AddSystemTime(at, at, mRepeatEveryNDays, 0, 0);

      if ((CompareSystemTime(current, at) == 0) && // 오늘이고
        ((last_min >= mSyncTimeAt) || // 이미 처리거나,
          (base_min > mSyncTimeAt)) ) // 시간이 지나서 로그인했으면 다음날로(펜딩되어 시간 지난것도 포함되도록)
        AddSystemTime(at, at, mRepeatEveryNDays, 0, 0);

      at.wHour = mSyncTimeAt / 60;
      at.wMinute = mSyncTimeAt % 60;
      at.wSecond = 0;
      return TRUE;
#endif

      int days_after_start = GetEllapsedDays(current, at);
      if (days_after_start >= mRepeatEveryNDays)
      {
        int ncount = days_after_start / mRepeatEveryNDays;
        AddSystemTime(at, at, ncount * mRepeatEveryNDays, 0, 0);
      }

      while (CompareSystemTime(current, at) > 0) // if past, add recurr date
        AddSystemTime(at, at, mRepeatEveryNDays, 0, 0);

      if ((CompareSystemTimeDate(current, at) == 0) && // 오늘이고 이미 오늘 처리했으면 다음으로
        (CompareSystemTimeDate(last, at) == 0))
        AddSystemTime(at, at, mRepeatEveryNDays, 0, 0);

      // at.wHour = mSyncTimeAt / 60;
      // at.wMinute = mSyncTimeAt % 60;
      calcSyncStartMinute(at, current, mSyncTimeAt);

      at.wSecond = 0;
      return TRUE;
    }
    else // SYNC_REPEAT_HOURLY, SYNC_REPEAT_MINUTELY
    {
      int base_min = -1;
      int duration;

      if (mRepeatType == SYNC_REPEAT_MINUTELY)
        duration = mRepeatEveryNMinutes;
      else
        duration = mRepeatEveryNHours * 60;

      if (IsValidSystemTime(base) && IsValidSystemTime(last))
      {
        if (CompareSystemTime(last, base) > 0)
          base = last;
      }
      else if (IsValidSystemTime(last))
      {
          base = last;
      }
      TCHAR msg[256];
      TCHAR timestr[32];
      SystemTimeToTString(timestr, 32, base);
      StringCchPrintf(msg, 256, _T("autosync base = %s"), timestr);
      OutputDebugString(msg);

      AddSystemTime(at, base, 0, duration / 60, duration % 60);
      at.wSecond = 0;
      return TRUE;
    }
  }
  return FALSE;
}
