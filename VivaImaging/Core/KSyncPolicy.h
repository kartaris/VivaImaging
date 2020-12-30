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
* @brief 동기화 폴더의 동기화 정책 정보 클래스.
*/
class KSyncPolicy
{
public:
  KSyncPolicy(void);
  ~KSyncPolicy(void);

  /**
  * @brief 동기화 정책 정보값을 초기화하고 해제한다.
  */
  void Clear();

  /**
  * @brief 동기화 방식(자동 및 수동, 충돌 처리, 부분 동기화 등)을 리턴한다.
  */
  inline int GetSyncMethod() { return mSyncMethod; }

  /**
  * @brief 동기화 방식(자동 및 수동, 충돌 처리, 부분 동기화 등)을 설정한다.
  */
  inline void SetSyncMethod(int method) { mSyncMethod = method; }

  /**
  * @brief 동기화 반복 형식(주간, 일간, 시간별)을 리턴한다.
  */
  inline int GetRepeatType() { return mRepeatType; }
  /**
  * @brief 동기화 반복 형식(주간, 일간, 시간별)을 설정한다.
  */
  inline void SetRepeatType(int type) { mRepeatType = type; }

  /**
  * @brief 주간 반복에서 선택된 요일 정보를 리턴한다.
  * sun=0x01,mon=0x02,tues=0x04,,,
  */
  inline int GetRepeatDatesInWeek() { return mRepeatDatesInWeek; } // weekly
  /**
  * @brief 주간 반복에서 선택된 요일 정보를 설정한다.
  */
  inline void SetRepeatDatesInWeek(int dates) { mRepeatDatesInWeek = dates; }
  /**
  * @brief 주간 반복에서 선택된 요일 정보에 특정 요일을 설정하거나 해제한다.
  */
  BOOL MaskRepeatDatesInWeek(int date, BOOL set);

  /**
  * @brief 일간 반복에서 반복일을 리턴한다.
  */
  inline int GetRepeatEveryNDays() { return mRepeatEveryNDays; } // daily
  /**
  * @brief 일간 반복에서 반복일을 설정한다.
  */
  inline void SetRepeatEveryNDays(int dates) { mRepeatEveryNDays = dates; }

  /**
  * @brief 시간 반복에서 반복시간을 리턴한다.
  */
  inline int GetRepeatEveryNHours() { return mRepeatEveryNHours; } // hourly
  /**
  * @brief 시간 반복에서 반복시간을 설정한다.
  */
  inline void SetRepeatEveryNHours(int hours) { mRepeatEveryNHours = hours; }

  /**
  * @brief 분간 반복에서 반복분을 리턴한다.
  */
  inline int GetRepeatEveryNMinutes() { return mRepeatEveryNMinutes; } // hourly
  /**
  * @brief 분간 반복에서 반복분을 설정한다.
  */
  inline void SetRepeatEveryNMinutes(int minutes) { mRepeatEveryNMinutes = minutes; }

  /**
  * @brief 동기화 시간을 리턴한다.
  */
  inline int GetSyncTimeAt() { return mSyncTimeAt; }
  /**
  * @brief 동기화 시간을 설정한다.
  */
  inline void SetSyncTimeAt(int at) { mSyncTimeAt = at; }

#ifdef USE_SYNC_DURATION
  /**
  * @brief 동기화 시작 종료 시간을 리턴한다.
  */
  inline int GetSyncTimeEnd() { return mSyncTimeEnd; }
  /**
  * @brief 동기화 시작 종료 시간을 설정한다.
  */
  inline void SetSyncTimeEnd(int at) { mSyncTimeEnd = at; }

  /**
  * @brief 동기화 시간 구간 이내이면 TRUE를 리턴한다.
  */
  BOOL IsMiddleOfTime(int minutes_of_day);

  /**
  * @brief 동기화 시간 구간이 지났으면 TRUE를 리턴한다.
  */
  BOOL IsTimeOver(int minutes_of_day);

#endif

  /**
  * @brief 동기화 플래그를 리턴한다.
  */
  inline int GetSyncFlag() { return mSyncFlag; }
  /**
  * @brief 동기화 플래그를 설정한다.
  */
  inline void SetSyncFlag(int flag) { mSyncFlag = flag; }
  /**
  * @brief 동기화 플래그를 설정하거나 해제한다.
  */
  inline void ChangeSyncFlag(int flag, BOOL set) { if (set) mSyncFlag |= flag; else mSyncFlag &= ~flag;}

  /**
  * @brief 동기화 멈춤 상태를 리턴한다.
  */
  inline BOOL GetPaused() { return mPaused; }
  /**
  * @brief 동기화 멈춤 상태를 설정하거나 해제한다.
  */
  inline void SetPaused(BOOL p) { mPaused = p; }

  /**
  * @brief 동기화 정책 개체의 복사 연산자.
  */
  KSyncPolicy& operator=(KSyncPolicy &policy);

  /**
  * @brief 지정한 패턴과 이름이 일치하는지 확인한다.
  */
  static BOOL IsNameMatching(LPCTSTR pattern, LPCTSTR name);

  // static TCHAR defaultServerExcludeMasks[512];
  // static LPTSTR GetDefaultMasks();

  /** 제외 파일형식 문자열을 변환한다.
  * convert (*.exe / *.jpg / *.mp3) as (*.exe | *.jpg | *.mp3)
  */
  static LPTSTR UpdateSyncExcludeMasks(LPTSTR buff);

  /**
  * @brief 지정한 패턴 목록과 이름이 일치하는지 확인한다. 패턴 목록은 패턴이 '|'로 구분되는 스트링이다.
  */
  static BOOL IsMatchFileMask(LPCTSTR masks, LPCTSTR name);
  /**
  * @brief 지정한 패턴 목록에 지정한 이름이 존재하는지 확인한다.
  */
  static BOOL IsExistFileMask(LPCTSTR masks, LPCTSTR name);

  /**
  * @brief 지정한 이름이 이 동기화 정책의 제외 파일 형식에 해당하는지 확인한다.
  * @param name : 파일명
  * @return BOOL : 해당되면 TRUE를 리턴한다.
  */
  BOOL IsExcludeFileMask(LPCTSTR name);

  /**
  * @brief 지정한 항목이 이 동기화 정책의 제외 파일 형식에 해당하는지 확인한다.
  * @param item : 대상 항목
  * @return BOOL : 해당되면 TRUE를 리턴한다.
  */
  BOOL IsExcludeFileMask(KSyncItem* item);

  /**
  * @brief 지정한 마스크 형식이 이 동기화 정책의 제외 파일 형식에 있는지 확인한다.
  * @param mask : 마스크 형식
  * @return BOOL : 있으면 TRUE를 리턴한다.
  */
  BOOL IsExistExcludeFileMask(LPCTSTR mask);

  /**
  * @brief 동기화 제외 파일 형식을 리턴한다.
  */
  inline TCHAR *GetExcludeMasks() { return mExcludeMasks; }
  /**
  * @brief 동기화 제외 파일 형식을 설정한다.
  */
  void SetExcludeMasks(LPCTSTR masks);
  /**
  * @brief 동기화 제외 파일 형식을 추가한다.
  */
  BOOL AddExcludeMask(LPCTSTR masks);

  /**
  * @brief 기본 정책값을 setting 파일에 저장한다.(DB storage에서는 사용 안함)
  */
  static void StoreDefaultSettingFile(LPTSTR filename);

  /**
  * @brief 기본 정책값을 설정한다.
  * @details A. 수동 동기화, 제외 형식은 "*.lnk|~*.*" 으로 설정한다.
  */
  void SetDefaultPolicy();

  /**
  * @brief 동기화 정책값을 설정 파일에 저장한다.(1.x 버전)
  */
  BOOL Store(LPTSTR filename);
  /**
  * @brief 동기화 정책값을 설정 파일에서 읽어온다.(1.x 버전)
  */
  BOOL Load(LPTSTR filename);
  /**
  * @brief 동기화 정책값의 일시 정지값을 설정 파일에 저장한다.(1.x 버전)
  */
  BOOL StorePaused(LPTSTR filename);

#if 0
  /**
  * @brief 동기화 정책값을 DB Storage에 저장한다.(2.x버전)
  */
  BOOL Store(sqlite3 *db);
  /**
  * @brief 동기화 정책값을 DB Storage에서 읽어온다.(2.x버전)
  */
  BOOL Load(sqlite3 *db);
  /**
  * @brief 동기화 정책값의 일시 정지값을 DB Storage에 저장한다.(2.x 버전)
  */
  BOOL StorePaused(sqlite3 *db);
#endif

  /**
  * @brief 동기화 시작을 시간 구간으로 사용할 때, 랜덤값을 더해 시작 시간을 설정한다.
  */
  void calcSyncStartMinute(SYSTEMTIME OUT &at, SYSTEMTIME IN &current, int timeAt);

  /**
  * @brief 자동 동기화 다음번 실행 시간을 계산한다.
  * @param time : 계산된 시간을 저장할 버퍼
  * @param last_sync_time : 마지막 동기화 시간
  * @param base : 기준 시간(로그인 또는 동기화 정책 설정 시간)
  * @return BOOL : 정상적으로 계산되었으면 TRUE를 리턴한다.
  * @details A. 동기화 방법이 자동 동기화이고 일시 중지 상태가 아니면,
  *     - 결과 시간을 지금 시간으로 복사하고,
  *     - 주간 반복이면,
  *       + 반복 요일(mRepeatDatesInWeek)에 따라 다음 행할 날짜를 계산하여 오늘 날짜에 더하고,
  *       + 동기화 실행 시간(mSyncTimeAt)을 결과 시간에 저장하고 TRUE를 리턴한다.
  *     - 일간 반복이면,
  *       + 마지막 동기화 시간이 유효하면 여기에 mRepeatEveryNDays를 더한 날짜를 결과로 하고,
  *       + 아니고 base 시간이 유효하면 여기에 (mRepeatEveryNDays-1)를 더한 날짜를 결과로 하고,
  *       + 아니면 오늘 날짜에 (mRepeatEveryNDays-1)를 더한 날짜를 결과로 하고,
  *       + 결과 날짜가 오늘보다 이전이면 mRepeatEveryNDays를 더한다.
  *       + 결과 날짜가 오늘이고, 이미 실행되었거나 시간이 지나서 로그인을 했으면 mRepeatEveryNDays를 더한다.
  *       + 동기화 실행 시간(mSyncTimeAt)을 결과 시간에 저장하고 TRUE를 리턴한다.
  *     - 아니면,
  *       + 시간, 분간 간격을 계산하고,
  *       + 기준 시간에 시간, 분간 간격을 더하고 TRUE를 리턴한다.
  *     .
  * @n B. FALSE를 리턴한다.
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
  /** 주간 및 일간 반복에서 동기화 실행 시간 */
  int mSyncTimeAt;

  /** 주간 및 일간 반복에서 동기화 실행 마감 시간 */
  int mSyncTimeEnd;

  /** 동기화 제외 형식 마스크 목록 */
  TCHAR *mExcludeMasks;
  /** 동기화 플래그, 권한에 따라 업로드 및 다운로드 */
  int mSyncFlag;
  /** 자동 동기화 일시 중지 */
  BOOL mPaused;

};

#endif
