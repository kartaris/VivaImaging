/**
* @file Reg.h
* @date 2015.02
* @brief Registry specific functions header file
*/

#pragma once

#ifndef _DESTINY_LOCALSYNC_REG_MODULE_H_
#define _DESTINY_LOCALSYNC_REG_MODULE_H_


/**
* @brief ECM Agent의 언어 설정 내용을 읽어온다.
* @n 한국어는 0x412, 중국어는 0x804등의 로케일값을 리턴한다.
*/
WORD RegGetDestinyAgentLanguage();

/**
* @brief ECM Agent의 자동동기화 사용 여부를 읽어온다.
*/
WORD RegGetDestinyAgentUseAutoSync();

/**
* @brief ECM Agent의 탐색기 윈도우 모드, 타임아웃 값, ECM/Claudium 모드를 읽어온다.
* @n windowMode = 1 : 전용 탐색기, 2 : 윈도우 탐색기
* @n timeout_ms = 타임아웃 값
* @n ecmMode = ECM모드이면 1, Claudium이면 0
*/
BOOL RegGetDestinyAgentValues(OUT WORD& windowMode, OUT int& timeout_ms, OUT WORD& ecmMode);

/**
* @brief ECM 서버에 getModifiedFolderNDocument를 요청하는 URL 주소를 읽어온다.
* @param buff : URL 주소를 저장할 버퍼
* @param len : 버퍼 크기
*/
BOOL RegGetServerFolderScanUrl(LPTSTR buff, int len);

/**
* @brief Unique한 session id를 생성(process_id와 매치).
*/
DWORD RegCreateSessionID();

void RegClearSessionID(DWORD mSessionIdx);

// #ifdef USE_SYNC_GUI

/**
* @brief 설정값에서 해당 키에 해당하는 값을 리턴한다.
*/
BOOL LoadRegistryValue(LPCTSTR name, CString& value);

/**
* @brief 설정값에 해당 키에 해당하는 값을 저장한다.
*/
void StoreRegistryValue(LPCTSTR name, CString& value);

void StoreRegistryValue(LPCTSTR name, LPCTSTR value);

/**
* @brief 설정값에서 다이얼로그의 이전 좌표를 읽어온다. 
* @param name : 다이얼로그 이름.
* @param rect : 화면 좌표.
* @return BOOL : 정상적으로 읽어왔으면 TRUE를 리턴한다.
*/
BOOL LoadDialogLocation(LPCTSTR name, RECT& rect);

/**
* @brief 설정값에 다이얼로그의 좌표를 저장한다. 
* @param name : 다이얼로그 이름.
* @param rect : 화면 좌표.
*/
void StoreDialogLocation(LPCTSTR name, RECT& rect);

/**
* @brief 호출 회수가 100번이면 한 번 TRUE를 리턴한다.
*/
BOOL RegistryAppTickCount();

/**
* @brief 설정값에서 모듈 처음 실행일을 리턴한다. 없으면 오늘로 설정하고 저장해 둔다.
*/
BOOL LoadModuleStateDate(SYSTEMTIME& OUT date, LPCTSTR userOid);

// #endif // USE_SYNC_GUI

#if 0

//
//   FUNCTION: ReadyHKCURegistry
//
//   PURPOSE: Initialize Registry folders
//
HRESULT RegReadyHKCURegistry();


//
//   FUNCTION: RegGetRegisteredSyncFolders
//
//   PURPOSE: Get Registry folders which are registered as sync-folder
//
//   RETURN: NULL terminated string array(last item is NULL string).
//
//   NOTE: Caller should free returned memory via delete[].

LPTSTR RegGetRegisteredSyncFolders();



//
//   FUNCTION: RegAppendRegisteredSyncFolders
//
//   PURPOSE: Append new sync folder
//
BOOL RegAppendRegisteredSyncFolders(LPCTSTR localPathName, LPCTSTR serverPathName);

//
//   FUNCTION: RegisterInprocServer
//
//   PURPOSE: Register the in-process component in the registry.
//
//   PARAMETERS:
//   * pszModule - Path of the module that contains the component
//   * clsid - Class ID of the component
//   * pszFriendlyName - Friendly name
//   * pszThreadModel - Threading model
//
//   NOTE: The function creates the HKCR\CLSID\{<CLSID>} key in the registry.
// 
//   HKCR
//   {
//      NoRemove CLSID
//      {
//          ForceRemove {<CLSID>} = s '<Friendly Name>'
//          {
//              InprocServer32 = s '%MODULE%'
//              {
//                  val ThreadingModel = s '<Thread Model>'
//              }
//          }
//      }
//   }
//
HRESULT RegisterInprocServer(PCWSTR pszModule, const CLSID& clsid, 
    PCWSTR pszFriendlyName, PCWSTR pszThreadModel);


//
//   FUNCTION: UnregisterInprocServer
//
//   PURPOSE: Unegister the in-process component in the registry.
//
//   PARAMETERS:
//   * clsid - Class ID of the component
//
//   NOTE: The function deletes the HKCR\CLSID\{<CLSID>} key in the registry.
//
HRESULT UnregisterInprocServer(const CLSID& clsid);

#endif

#endif // _DESTINY_LOCALSYNC_REG_MODULE_H_
