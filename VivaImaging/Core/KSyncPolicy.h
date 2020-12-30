/**
* @file KSyncPolicy.h
* @date 2015.02
* @brief KSyncPolicy class header file
*/
#pragma once

#ifndef _LOCALSYNC_SYNC_POLICY_CLASS_H_
#define _LOCALSYNC_SYNC_POLICY_CLASS_H_

#define SYNC_METHOD_MANUAL 1
#define SYNC_METHOD_AUTO 2
#define SYNC_METHOD_INSTANT 4
#define SYNC_METHOD_BACKUP 8
#define SYNC_METHOD_RESTORE 0x10
#define SYNC_METHOD_BACKUP_RESTORE (SYNC_METHOD_BACKUP|SYNC_METHOD_RESTORE)
#define SYNC_METHOD_FULL_BACKUP (SYNC_METHOD_AUTO|SYNC_METHOD_BACKUP)

#define SYNC_METHOD_SYNCING 0x00FF
#define SYNC_METHOD_RESOLVE_CONFLICT 0x0100 // by 'Resolve conflict' menu
#define SYNC_METHOD_SILENCE 0x0200
#define SYNC_METHOD_PARTIAL 0x0400
#define SYNC_METHOD_CHECK 0x0800
#define SYNC_METHOD_SETTING 0x01000
#define SYNC_METHOD_IMMIGRATION 0x02000

#define SYNC_METHOD_TYPE 0x0FFFF

// sync extra flag
#define SYNC_MERGABLE 0x010000
#define SYNC_HIGH_PRIORITY 0x020000

#define SYNC_REPEAT_MINUTELY 1
#define SYNC_REPEAT_HOURLY 2
#define SYNC_REPEAT_DAILY 3
#define SYNC_REPEAT_WEEKLY 4

#define SYNC_ENABLE_UPLOAD 1
#define SYNC_ENABLE_DOWNLOAD 2
#define SYNC_CONFLICT_UPLOAD        0x010
#define SYNC_CONFLICT_DOWNLOAD  0x020
#define SYNC_SERVER_LOCATION_CHANGED 0x0100

#define SYNC_ON_SUNDAY 0x01
#define SYNC_ON_MONDAY 0x02
#define SYNC_ON_TUESDAY 0x04
#define SYNC_ON_WEDNESDAY 0x08
#define SYNC_ON_THURSDAY 0x010
#define SYNC_ON_FRIDAY 0x020
#define SYNC_ON_SATERDAY 0x040

#define MAX_EXCLUDE_STRING_LENGTH 250

#if 0
#include "../SqliteLib/sqlite3.h"
#endif

class KSyncItem;

/**
* @class KSyncPolicy
* @brief ����ȭ ������ ����ȭ ��å ���� Ŭ����.
*/
class KSyncPolicy
{
public:
  KSyncPolicy(void);
  ~KSyncPolicy(void);

  /**
  * @brief ����ȭ ��å �������� �ʱ�ȭ�ϰ� �����Ѵ�.
  */
  void Clear();

  /**
  * @brief ����ȭ ���(�ڵ� �� ����, �浹 ó��, �κ� ����ȭ ��)�� �����Ѵ�.
  */
  inline int GetSyncMethod() { return mSyncMethod; }

  /**
  * @brief ����ȭ ���(�ڵ� �� ����, �浹 ó��, �κ� ����ȭ ��)�� �����Ѵ�.
  */
  inline void SetSyncMethod(int method) { mSyncMethod = method; }

  /**
  * @brief ����ȭ �ݺ� ����(�ְ�, �ϰ�, �ð���)�� �����Ѵ�.
  */
  inline int GetRepeatType() { return mRepeatType; }
  /**
  * @brief ����ȭ �ݺ� ����(�ְ�, �ϰ�, �ð���)�� �����Ѵ�.
  */
  inline void SetRepeatType(int type) { mRepeatType = type; }

  /**
  * @brief �ְ� �ݺ����� ���õ� ���� ������ �����Ѵ�.
  * sun=0x01,mon=0x02,tues=0x04,,,
  */
  inline int GetRepeatDatesInWeek() { return mRepeatDatesInWeek; } // weekly
  /**
  * @brief �ְ� �ݺ����� ���õ� ���� ������ �����Ѵ�.
  */
  inline void SetRepeatDatesInWeek(int dates) { mRepeatDatesInWeek = dates; }
  /**
  * @brief �ְ� �ݺ����� ���õ� ���� ������ Ư�� ������ �����ϰų� �����Ѵ�.
  */
  BOOL MaskRepeatDatesInWeek(int date, BOOL set);

  /**
  * @brief �ϰ� �ݺ����� �ݺ����� �����Ѵ�.
  */
  inline int GetRepeatEveryNDays() { return mRepeatEveryNDays; } // daily
  /**
  * @brief �ϰ� �ݺ����� �ݺ����� �����Ѵ�.
  */
  inline void SetRepeatEveryNDays(int dates) { mRepeatEveryNDays = dates; }

  /**
  * @brief �ð� �ݺ����� �ݺ��ð��� �����Ѵ�.
  */
  inline int GetRepeatEveryNHours() { return mRepeatEveryNHours; } // hourly
  /**
  * @brief �ð� �ݺ����� �ݺ��ð��� �����Ѵ�.
  */
  inline void SetRepeatEveryNHours(int hours) { mRepeatEveryNHours = hours; }

  /**
  * @brief �а� �ݺ����� �ݺ����� �����Ѵ�.
  */
  inline int GetRepeatEveryNMinutes() { return mRepeatEveryNMinutes; } // hourly
  /**
  * @brief �а� �ݺ����� �ݺ����� �����Ѵ�.
  */
  inline void SetRepeatEveryNMinutes(int minutes) { mRepeatEveryNMinutes = minutes; }

  /**
  * @brief ����ȭ �ð��� �����Ѵ�.
  */
  inline int GetSyncTimeAt() { return mSyncTimeAt; }
  /**
  * @brief ����ȭ �ð��� �����Ѵ�.
  */
  inline void SetSyncTimeAt(int at) { mSyncTimeAt = at; }

#ifdef USE_SYNC_DURATION
  /**
  * @brief ����ȭ ���� ���� �ð��� �����Ѵ�.
  */
  inline int GetSyncTimeEnd() { return mSyncTimeEnd; }
  /**
  * @brief ����ȭ ���� ���� �ð��� �����Ѵ�.
  */
  inline void SetSyncTimeEnd(int at) { mSyncTimeEnd = at; }

  /**
  * @brief ����ȭ �ð� ���� �̳��̸� TRUE�� �����Ѵ�.
  */
  BOOL IsMiddleOfTime(int minutes_of_day);

  /**
  * @brief ����ȭ �ð� ������ �������� TRUE�� �����Ѵ�.
  */
  BOOL IsTimeOver(int minutes_of_day);

#endif

  /**
  * @brief ����ȭ �÷��׸� �����Ѵ�.
  */
  inline int GetSyncFlag() { return mSyncFlag; }
  /**
  * @brief ����ȭ �÷��׸� �����Ѵ�.
  */
  inline void SetSyncFlag(int flag) { mSyncFlag = flag; }
  /**
  * @brief ����ȭ �÷��׸� �����ϰų� �����Ѵ�.
  */
  inline void ChangeSyncFlag(int flag, BOOL set) { if (set) mSyncFlag |= flag; else mSyncFlag &= ~flag;}

  /**
  * @brief ����ȭ ���� ���¸� �����Ѵ�.
  */
  inline BOOL GetPaused() { return mPaused; }
  /**
  * @brief ����ȭ ���� ���¸� �����ϰų� �����Ѵ�.
  */
  inline void SetPaused(BOOL p) { mPaused = p; }

  /**
  * @brief ����ȭ ��å ��ü�� ���� ������.
  */
  KSyncPolicy& operator=(KSyncPolicy &policy);

  /**
  * @brief ������ ���ϰ� �̸��� ��ġ�ϴ��� Ȯ���Ѵ�.
  */
  static BOOL IsNameMatching(LPCTSTR pattern, LPCTSTR name);

  // static TCHAR defaultServerExcludeMasks[512];
  // static LPTSTR GetDefaultMasks();

  /** ���� �������� ���ڿ��� ��ȯ�Ѵ�.
  * convert (*.exe / *.jpg / *.mp3) as (*.exe | *.jpg | *.mp3)
  */
  static LPTSTR UpdateSyncExcludeMasks(LPTSTR buff);

  /**
  * @brief ������ ���� ��ϰ� �̸��� ��ġ�ϴ��� Ȯ���Ѵ�. ���� ����� ������ '|'�� ���еǴ� ��Ʈ���̴�.
  */
  static BOOL IsMatchFileMask(LPCTSTR masks, LPCTSTR name);
  /**
  * @brief ������ ���� ��Ͽ� ������ �̸��� �����ϴ��� Ȯ���Ѵ�.
  */
  static BOOL IsExistFileMask(LPCTSTR masks, LPCTSTR name);

  /**
  * @brief ������ �̸��� �� ����ȭ ��å�� ���� ���� ���Ŀ� �ش��ϴ��� Ȯ���Ѵ�.
  * @param name : ���ϸ�
  * @return BOOL : �ش�Ǹ� TRUE�� �����Ѵ�.
  */
  BOOL IsExcludeFileMask(LPCTSTR name);

  /**
  * @brief ������ �׸��� �� ����ȭ ��å�� ���� ���� ���Ŀ� �ش��ϴ��� Ȯ���Ѵ�.
  * @param item : ��� �׸�
  * @return BOOL : �ش�Ǹ� TRUE�� �����Ѵ�.
  */
  BOOL IsExcludeFileMask(KSyncItem* item);

  /**
  * @brief ������ ����ũ ������ �� ����ȭ ��å�� ���� ���� ���Ŀ� �ִ��� Ȯ���Ѵ�.
  * @param mask : ����ũ ����
  * @return BOOL : ������ TRUE�� �����Ѵ�.
  */
  BOOL IsExistExcludeFileMask(LPCTSTR mask);

  /**
  * @brief ����ȭ ���� ���� ������ �����Ѵ�.
  */
  inline TCHAR *GetExcludeMasks() { return mExcludeMasks; }
  /**
  * @brief ����ȭ ���� ���� ������ �����Ѵ�.
  */
  void SetExcludeMasks(LPCTSTR masks);
  /**
  * @brief ����ȭ ���� ���� ������ �߰��Ѵ�.
  */
  BOOL AddExcludeMask(LPCTSTR masks);

  /**
  * @brief �⺻ ��å���� setting ���Ͽ� �����Ѵ�.(DB storage������ ��� ����)
  */
  static void StoreDefaultSettingFile(LPTSTR filename);

  /**
  * @brief �⺻ ��å���� �����Ѵ�.
  * @details A. ���� ����ȭ, ���� ������ "*.lnk|~*.*" ���� �����Ѵ�.
  */
  void SetDefaultPolicy();

  /**
  * @brief ����ȭ ��å���� ���� ���Ͽ� �����Ѵ�.(1.x ����)
  */
  BOOL Store(LPTSTR filename);
  /**
  * @brief ����ȭ ��å���� ���� ���Ͽ��� �о�´�.(1.x ����)
  */
  BOOL Load(LPTSTR filename);
  /**
  * @brief ����ȭ ��å���� �Ͻ� �������� ���� ���Ͽ� �����Ѵ�.(1.x ����)
  */
  BOOL StorePaused(LPTSTR filename);

#if 0
  /**
  * @brief ����ȭ ��å���� DB Storage�� �����Ѵ�.(2.x����)
  */
  BOOL Store(sqlite3 *db);
  /**
  * @brief ����ȭ ��å���� DB Storage���� �о�´�.(2.x����)
  */
  BOOL Load(sqlite3 *db);
  /**
  * @brief ����ȭ ��å���� �Ͻ� �������� DB Storage�� �����Ѵ�.(2.x ����)
  */
  BOOL StorePaused(sqlite3 *db);
#endif

  /**
  * @brief ����ȭ ������ �ð� �������� ����� ��, �������� ���� ���� �ð��� �����Ѵ�.
  */
  void calcSyncStartMinute(SYSTEMTIME OUT &at, SYSTEMTIME IN &current, int timeAt);

  /**
  * @brief �ڵ� ����ȭ ������ ���� �ð��� ����Ѵ�.
  * @param time : ���� �ð��� ������ ����
  * @param last_sync_time : ������ ����ȭ �ð�
  * @param base : ���� �ð�(�α��� �Ǵ� ����ȭ ��å ���� �ð�)
  * @return BOOL : ���������� ���Ǿ����� TRUE�� �����Ѵ�.
  * @details A. ����ȭ ����� �ڵ� ����ȭ�̰� �Ͻ� ���� ���°� �ƴϸ�,
  *     - ��� �ð��� ���� �ð����� �����ϰ�,
  *     - �ְ� �ݺ��̸�,
  *       + �ݺ� ����(mRepeatDatesInWeek)�� ���� ���� ���� ��¥�� ����Ͽ� ���� ��¥�� ���ϰ�,
  *       + ����ȭ ���� �ð�(mSyncTimeAt)�� ��� �ð��� �����ϰ� TRUE�� �����Ѵ�.
  *     - �ϰ� �ݺ��̸�,
  *       + ������ ����ȭ �ð��� ��ȿ�ϸ� ���⿡ mRepeatEveryNDays�� ���� ��¥�� ����� �ϰ�,
  *       + �ƴϰ� base �ð��� ��ȿ�ϸ� ���⿡ (mRepeatEveryNDays-1)�� ���� ��¥�� ����� �ϰ�,
  *       + �ƴϸ� ���� ��¥�� (mRepeatEveryNDays-1)�� ���� ��¥�� ����� �ϰ�,
  *       + ��� ��¥�� ���ú��� �����̸� mRepeatEveryNDays�� ���Ѵ�.
  *       + ��� ��¥�� �����̰�, �̹� ����Ǿ��ų� �ð��� ������ �α����� ������ mRepeatEveryNDays�� ���Ѵ�.
  *       + ����ȭ ���� �ð�(mSyncTimeAt)�� ��� �ð��� �����ϰ� TRUE�� �����Ѵ�.
  *     - �ƴϸ�,
  *       + �ð�, �а� ������ ����ϰ�,
  *       + ���� �ð��� �ð�, �а� ������ ���ϰ� TRUE�� �����Ѵ�.
  *     .
  * @n B. FALSE�� �����Ѵ�.
  */
  BOOL CalcNextSyncTime(SYSTEMTIME OUT &time, SYSTEMTIME IN &last_sync_time, SYSTEMTIME IN &base);

private:
  /** manual, auto-repeat, auto-realtime */
  int mSyncMethod;
  /** minutely, hourly, weekly, daily */
  int mRepeatType;
  /** sync every 10 minute, 30 minute */
  int mRepeatEveryNMinutes;
  /** sync every 1 hour, 2 hour, 3 hour,,, 23 hour */
  int mRepeatEveryNHours;
  /** sync every number of dates(1 ~ 30) */
  int mRepeatEveryNDays;
  /** sync date weekly(sun=0x01,mon=0x02,tues=0x04 */
  int mRepeatDatesInWeek;
  /** �ְ� �� �ϰ� �ݺ����� ����ȭ ���� �ð� */
  int mSyncTimeAt;

  /** �ְ� �� �ϰ� �ݺ����� ����ȭ ���� ���� �ð� */
  int mSyncTimeEnd;

  /** ����ȭ ���� ���� ����ũ ��� */
  TCHAR *mExcludeMasks;
  /** ����ȭ �÷���, ���ѿ� ���� ���ε� �� �ٿ�ε� */
  int mSyncFlag;
  /** �ڵ� ����ȭ �Ͻ� ���� */
  BOOL mPaused;

};

#endif
