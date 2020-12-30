#ifndef _DESTINY_APPLICATION_SETTING_MODULE_H_
#define _DESTINY_APPLICATION_SETTING_MODULE_H_

#define WM_LOG_MESSAGE (WM_USER + 280)

#if defined(USE_LOCALSYNC)

char *GetCurrentSyncFolders();

// caller should free returned memory by delete[]
LPTSTR GetCurrentSyncFoldersT();

BOOL RemoveSyncFolder(char *path);
BOOL ReplaceSyncFolder(char *oldPath, char *newPath, LPCTSTR ownerOID);

BOOL RemoveSyncFolderT(LPCTSTR path);
BOOL ReplaceSyncFolderT(LPCTSTR oldFolderName, LPCTSTR newFolderName, LPCTSTR ownerOID);

BOOL RemoveSyncFolderList();

BOOL IsCurrentSyncFoldersT(TCHAR *path);

BOOL AppendSyncFolder(char *path);
BOOL AppendSyncFolderT(LPCTSTR path);

#endif //USE_LOCALSYNC

int FindLFSeparateStringT(LPTSTR paths, LPCTSTR path);

LPSTR AddSettingKeyValue(LPSTR buff, int &buff_length, const char *setting_key, const char *value);
BOOL StoreSettingData(LPTSTR setting_file, const char *buff);

void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, int value);
void StoreSettingKeyValue(LPTSTR filename, const char *setting_key, const char *value);

BOOL ReadSettingKeyValue(TCHAR *filename, const char *setting_key, char *buff, int buff_size);

// append user's setting file
void AppendSettingKeyValue(const char *setting_key, const char *value);

// read from user's setting file
BOOL GetSettingKeyValue(const char *setting_key, char *buff, int buff_size);

#ifndef USE_BACKUP // 백업에서는 사용 안함.
// append common(root) setting file
void AppendCommonSettingKeyValue(const char *setting_key, const char *value);

// read common(root) setting file
BOOL GetCommonSettingKeyValue(const char *setting_key, char *buff, int buff_size);
#endif

BOOL MakeNextSequenceName(int max_length, LPTSTR candidate);

#if 0
BOOL GetMetaFolderNameMapping(const TCHAR *obj_path, const char *subfolder, char *mapping_name, BOOL createIfNone);
BOOL GetMetaFolderNameOrigin(const TCHAR *obj_path, char *subfolder, int buff_len, const char *mapping_name);

BOOL UpdateMetaFolderNameMapping(LPCTSTR obj_path, LPCSTR oldPath, LPCSTR newPath, BOOL includeSub);
BOOL UpdateMetaFolderNameMappingT(LPCTSTR obj_path, LPCTSTR oldPath, LPCTSTR newPath, BOOL includeSub);
#endif

#define SYNC_MSG_LOG 1
#define SYNC_MSG_HISTORY 2
#define SYNC_MSG_DONE 4
#define SYNC_MSG_NO_GUI 8
#define SYNC_MSG_SERVICE_LOG 0x10
#define SYNC_MSG_DEBUG 0x20

#define SYNC_EVENT_ERROR 0x0100
#define SYNC_EVENT_WARN 0x0200

// history status
// history for set sync folder
#define SYNC_H_SET_SYNCFOLDER 1
#define SYNC_H_CLEAR_SYNCFOLDER 2
#define SYNC_H_SYNCFOLDER_PROPERTY 3
#define SYNC_H_SYNC_EXCLUDE_FOLDER 4
#define SYNC_H_SYNC_INCLUDE_FOLDER 5
#define SYNC_H_CREATE_BACKUP_VERSION 6
#define SYNC_H_CONNECT_BACKUP_VERSION 7

// history for start/stop sync
// #define SYNC_H_START_SYNC 10
#define SYNC_H_START_AUTO_SYNC 10
#define SYNC_H_START_MANUAL_SYNC 10
#define SYNC_H_END_SYNC 11
#define SYNC_H_START_FULL_BACKUP 12
#define SYNC_H_START_INSTANT_BACKUP 13
#define SYNC_H_END_BACKUP 14
#define SYNC_H_START_RESTORE 15
#define SYNC_H_END_RESTORE 16
#define SYNC_H_FAIL_BACKUP 17
#define SYNC_H_FAIL_RESTORE 18

// history for each items
#define SYNC_H_UPLOAD 21
#define SYNC_H_DOWNLOAD 22
#define SYNC_H_CREATE_LOCAL_FOLDER 23
#define SYNC_H_CREATE_SERVER_FOLDER 24
#define SYNC_H_RENAME_LOCAL_FILE 25
#define SYNC_H_RENAME_SERVER_FILE 26
#define SYNC_H_RENAME_LOCAL_FOLDER 27
#define SYNC_H_RENAME_SERVER_FOLDER 28
#define SYNC_H_DELETE_LOCAL_FILE 29
#define SYNC_H_DELETE_SERVER_FILE 30
#define SYNC_H_DELETE_LOCAL_FOLDER 31
#define SYNC_H_DELETE_SERVER_FOLDER 32
#define SYNC_H_REGIST_AS_NEW 33 // separate
#define SYNC_H_RENAME_LOCAL_ON_CONFLICT 34
#define SYNC_H_RENAME_SERVER_ON_CONFLICT 35

#define SYNC_H_CONFLICT 40
#define SYNC_H_SYNCERR 50
#define SYNC_H_ETC 200

void CloseLogFile();
void AppendLogMessage(TCHAR *buff);
#ifndef HISTORY_BY_SQLITE
void AppendHistoryMessage(TCHAR *buff, SYSTEMTIME &systime);
#endif
void StoreLogHistory(LPCTSTR module_name, HRESULT hr, DWORD type);

void StoreLogHistory(LPCTSTR module_name, LPCTSTR msg, DWORD type);

#ifdef USE_SYNC_ENGINE
void StoreHistory(int state, LPCTSTR path, LPCTSTR name, int type, LPCTSTR lastUser, LPCTSTR extra);
void StoreHistory(int state, LPCTSTR pathname, int type, LPCTSTR lastUser, LPCTSTR extra);

int DeleteHistory(long *ids, int count);
int GetHistoryStateFromSyncResult(int r, int default_state);

typedef LRESULT (__stdcall *HistoryQueryCallback)(long id, int state, LPTSTR pathname,
  int type, LPTSTR ext, SYSTEMTIME& time, LPTSTR user, void *pData);

void GetHistoryList(HistoryQueryCallback pCallback, void *pData, int stateMatch, LPCTSTR msgMatch,
  __int64 time_start, __int64 time_end, LPCTSTR userMatch, int limit, int offset);

void GetHistoryStatusName(LPTSTR buff, int length, int state);
#else
#define StoreHistory
#define GetHistoryStateFromSyncResult
#endif

// BOOL GetLastSyncTime(SYSTEMTIME& time);

BOOL GetMetaSettingValue(LPCTSTR metafilename, char *tag, char *out);

void GetDefaultSettingsFilename(TCHAR *filename);

int RemoveFromHistoryFile(SYSTEMTIME &systime, char *location, char *path, char *name, int result);
int RemoveFromHistoryDB(long id);

#endif

