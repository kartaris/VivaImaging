#pragma once

#include "ClientDataType.h"

#ifdef LOCALSYNC_EXPORTS
  #define ECMLS __declspec(dllexport) __stdcall
#else
  #define ECMLS __declspec(dllimport) __stdcall
#endif

#define OBJECT_ID_LENGTH       22 
#define MAX_USERID_LENGTH      60
#define MAX_SESSION_KEY_LENGTH 60

#ifndef _WINDEF_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

typedef unsigned long DWORD;
typedef int           BOOL;

typedef void          *LPVOID, *PVOID;
typedef const void    *LPCVOID, *PCVOID;

typedef wchar_t       *LPWSTR, *PWSTR;
typedef const wchar_t *LPCWSTR, *PCWSTR;

typedef __int64 LONGLONG;
typedef unsigned __int64 ULONGLONG;

typedef unsigned int        UINT;

#define MAX_PATH          260

#ifndef __int3264
#ifdef _WIN64
    typedef __int64 INT_PTR, *PINT_PTR;
    typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

    typedef __int64 LONG_PTR, *PLONG_PTR;
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int64
#else
    typedef __w64 int INT_PTR, *PINT_PTR;
    typedef __w64 unsigned int UINT_PTR, *PUINT_PTR;

    typedef __w64 long LONG_PTR, *PLONG_PTR;
    typedef __w64 unsigned long ULONG_PTR, *PULONG_PTR;

    #define __int3264   __int32
#endif
#endif

typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

#ifndef _FILETIME_
typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#define _FILETIME_
#endif

#endif

#ifndef _TIME64_T_DEFINED
typedef __int64 __time64_t;     /* 64-bit time value */
#define _TIME64_T_DEFINED
#endif

typedef struct _st_LSDD_Policy_Information
{
    char cNormalDD;                     // R/W/D/N
    char cNormalDDExplorer;             // R/W/D/N
    char cSecurityDD;                   // R/W/D/N
    char cEcmDD;                        // R/W/D/N
    int nCapacityOfSecurityDD;          // 보안 DD 허가 용량으로 단위는 MB
    char cSecurityDDVolume;             //A~Z 보안 DD Volume Letter
    char cEcmDDVolume;                  //A~Z ECM DD Volume Letter
    wchar_t wszSecurityDDVolumeName[32];
    wchar_t wszEcmDDVolumeName[32];
    int nLookupListCountPerPage;        // 전용 탐색기에서 페이지 네비게이션 시 조회 목록 수.
    int nLookupListCountOfApplication;  // Application에서 FindFile이 옵션이 '*' 일때 서버에서 한번에 받아오는 조회 목록 수 ==> 0일때는 전부 받아온다.
    DWORD dwDocumentIncludeType;
    int  nFileViewTypeInGeneralExplorer;// 0 : ReadOnly, 1: Writable
    int  nFileViewTypeInOnlyExplorer;   // 0 : ReadOnly, 1: Writable
    char cClipBoardPermission;          // A/D ==>Allow/Deny
    char cCapturePermission;            // A/D ==>Allow/Deny
    char cPrintPermission;              // A/D ==>Allow/Deny
    char cAccessOutlook;                // A/D/S ==>Allow/Deny/Security
    char cVirtualDDVolume;              //A~Z Virtual DD Volume Letter (임시 영역 드라이브)
    char cCentralDisk;                  //Y/N
} LSDD_POLICY_INFORMATION,*PLSDD_POLICY_INFORMATION;

typedef struct _st_LSDD_Policy_InformationEx
{
	LSDD_POLICY_INFORMATION PolicyInfo;
	wchar_t wszDownLoadPath[MAX_PATH];
}LSDD_POLICY_INFORMATIONEX,*PLSDD_POLICY_INFORMATIONEX;

typedef enum _LS_NOTIFY_MESSAGE_
{
    LSNM_Invalid = -1,
    LSNM_FileSyncProgress = 0,
    LSNM_MenuClicked,
    LSNM_LocaleChanged,
    LSNM_AfterLogin,
    LSNM_AfterLogout,
    LSNM_Shutdown,
    LSNM_Refresh,
    LSNM_DDPolicyInfo,
    LSNM_LocalSyncStatus,
    LSNM_DriveReadyToService,
    LSNM_SessionKeyChanged, // 로그인 정보가 변경되지 않았고, 재로그인한 경우도 아니면서
                            // SessionKey만 변동된 경우
    LSNM_DlpMedialPermission,

} LS_NOTIFY_MESSAGE;

enum LS_LOGOUT_STATUS
{
    LSLS_SESSION_TIMEOUT = 0,
    LSLS_LOGOUT,
    LSLS_LOCAL_LOGIN,
};

struct SessionValue
{
    wchar_t userId[MAX_USERID_LENGTH];
    wchar_t userOid[OBJECT_ID_LENGTH]; 
    wchar_t sessionKey[MAX_SESSION_KEY_LENGTH];
};

struct LocalSyncUploadFileDetails
{
	LONGLONG localFileSize;
	FILETIME localLastWriteTime;
	FILETIME localLastAccessTime;
};

struct LocalSyncUploadFileDetails2 : LocalSyncUploadFileDetails
{
	FILETIME localCreationTime;
};

// usb Permission
static const DWORD DLP_VOLUME_DENY           = 0x0001;    // 사용 금지
static const DWORD DLP_VOLUME_READONLY       = 0x0002;    // 읽기 전용
static const DWORD DLP_VOLUME_ALL_ACCESS     = 0x0004;    // 모든권한

struct DlpMediaPermission
{ 
    DWORD usbPermission;
    BOOL mtpWriteLock;
    BOOL cdrwriteLock;
};

struct DlpMediaInfo
{ 
    DlpMediaPermission mediaPermission;
    DWORD removableDrives;
    DWORD cdrwDrives;
};

typedef enum class _IF_TYPE_
{
	Unknown,
	CommonIF,
	LSIF = 10,
	Imaging,
	PantaIF = 50,
	PantaService,
}IF_TYPE;

typedef LRESULT (__stdcall *FN_NotifyCallback)(LS_NOTIFY_MESSAGE message, WPARAM wParam, LPARAM lParam);

#ifdef __cplusplus
extern "C" {
#endif

BOOL ECMLS LocalSync_DLLInit(FN_NotifyCallback pfnNotifyCallback);
BOOL ECMLS LocalSync_DLLInitEx(FN_NotifyCallback pfnNotifyCallback, IF_TYPE ifType);
BOOL ECMLS LocalSync_GetLogInID(LPWSTR lpcwszUserId, LPWSTR lpcwszUserOId, bool needAutoLoginWhenSessionTimeout=false);
BOOL ECMLS LocalSync_GetLogInID2(SessionValue* sessionValue, bool needAutoLoginWhenSessionTimeout=false);

BOOL ECMLS LocalSync_CreateDocument(ERROR_ST* err, LPCWSTR szFolderOID, LPCWSTR szFileName, ULONGLONG fileSize, LPCWSTR szStorageFileID, __time64_t localLastModifiedAt, LPCWSTR param5, OUT XSYNCDOCUMENT_ST** ppXDoc);
BOOL ECMLS LocalSync_CreateDocument2(ERROR_ST* err_EnvP, LPCWSTR szFolderOID, LPCWSTR szFileName, ULONGLONG fileSize, LPCWSTR szStorageFileID, LONGLONG localLastModifiedAt, LONGLONG localCreatedAt,  LPCWSTR param5, OUT XSYNCDOCUMENT_ST** ppXDoc);
BOOL ECMLS LocalSync_UpdateDocument(ERROR_ST* err, LPCWSTR documentOID, LPCWSTR fileName, ULONGLONG fileSize, LPCWSTR storageFileID, __time64_t localLastModifiedAt, LPCWSTR folderOID, LPCWSTR folderFullPathIndex, OUT XSYNCDOCUMENT_ST** ppXDoc);
BOOL ECMLS LocalSync_DeleteDocument(ERROR_ST* err, LPCWSTR szDocOID, LPCWSTR param1, LPCWSTR param2);
BOOL ECMLS LocalSync_GetDocument(ERROR_ST* err, LPCWSTR szDocOID, OUT XSYNCDOCUMENT_ST** ppXDoc);
BOOL ECMLS LocalSync_RenameDocument(ERROR_ST* err, LPCWSTR szDocOID, LPCWSTR szDocName, LPCWSTR param2, LPCWSTR param3);
BOOL ECMLS LocalSync_CreateFolder(ERROR_ST* err, LPCWSTR szParentFolderOID, LPCWSTR szFolderName, LPCWSTR param2, OUT XSYNCFOLDER_ST** ppXFolder);
BOOL ECMLS LocalSync_RenameFolder(ERROR_ST* err, LPCWSTR szFolderOID, LPCWSTR szFolderName, LPCWSTR param2);
BOOL ECMLS LocalSync_DeleteFolder(ERROR_ST* err, LPCWSTR szFolderOID, LPCWSTR param1);
BOOL ECMLS LocalSync_GetFolder(ERROR_ST* err, LPCWSTR szFolderOID, OUT XSYNCFOLDER_ST** ppXFolder);
BOOL ECMLS LocalSync_GetFolders(ERROR_ST* err, LPCWSTR szTargetOID, OUT XSYNCFOLDERLIST_ST** ppXFolderList);
BOOL ECMLS LocalSync_GetDocuments(ERROR_ST* err, LPCWSTR szFolderOID, int nRowCount, int nStartPage, OUT XSYNCDOCUMENTLIST_ST** ppXDocList);
BOOL ECMLS LocalSync_GetConfig(ERROR_ST* err, LPCWSTR szKey, OUT LPCWSTR* ppszValue);
BOOL ECMLS LocalSync_GetCurrentUser(ERROR_ST* err, OUT LPCWSTR* ppszUserOID);
BOOL ECMLS LocalSync_GetUserDisplayName(ERROR_ST* err, LPCWSTR szUserOID, OUT LPCWSTR* ppszUserName);

void ECMLS LocalSync_InitEnv(ERROR_ST* env);
void ECMLS LocalSync_CleanEnv(ERROR_ST* const envP);
void ECMLS LocalSync_DestroyStringW(LPCWSTR psz);
void ECMLS LocalSync_DestroyXSYNCDOCUMENT_ST(XSYNCDOCUMENT_ST* st_data, bool bIsArray=false);
void ECMLS LocalSync_DestroyXSYNCFOLDER_ST(XSYNCFOLDER_ST* st_data, bool bIsArray=false);
void ECMLS LocalSync_DestroyXSYNCFOLDERLIST_ST(XSYNCFOLDERLIST_ST* st_data, bool bIsArray=false);
void ECMLS LocalSync_DestroyXSYNCDOCUMENTLIST_ST(XSYNCDOCUMENTLIST_ST* st_data, bool bIsArray=false);
void ECMLS LocalSync_DestroyXSYNCUSERINFO_ST(XSYNCUSERINFO_ST* st_data, bool bIsArray=false);
void ECMLS LocalSync_DestroyXDOCTYPE_ST(XDOCTYPE_ST* old);
void ECMLS LocalSync_DestroyXDOCTYPEARRAY_ST(XDOCTYPE_ST* old, int size);
void ECMLS LocalSync_DestroyXSYNCDOCUMENTWITHEXTATTR_ST(XSYNCDOCUMENTWITHEXTATTR_ST* st_data, bool bIsArray = false);

BOOL ECMLS LocalSync_FileDownload(ERROR_ST* err, LPCWSTR szFileOID, LPCWSTR szStorageFileID, LPCWSTR szDownloadPath);
BOOL ECMLS LocalSync_FileView(ERROR_ST* err_EnvP, LPCWSTR szFileOID, LPCWSTR szFileName, bool fOpen, LPCWSTR* ppszFilePath);
BOOL ECMLS LocalSync_FileUpload(ERROR_ST* err, IN OUT LPWSTR szFileID, LPCWSTR szFilePath);
BOOL ECMLS LocalSync_FileUpload2(ERROR_ST* err, IN OUT LPWSTR szFileID, LPCWSTR szFilePath, OUT LocalSyncUploadFileDetails* uploadFileDetails);
BOOL ECMLS LocalSync_FileUpload3(ERROR_ST* err, IN OUT LPWSTR szFileID, LPCWSTR szFilePath, OUT LocalSyncUploadFileDetails2* uploadFileDetails);
BOOL ECMLS LocalSync_SelectServerFolder(ERROR_ST* err, IN OUT LPWSTR lpszCurrentFolderOID, LONGLONG hWnd);
BOOL ECMLS LocalSync_ShowDocumentProperty(ERROR_ST* err, LPCWSTR szDocumentOID, LONGLONG hWnd);
BOOL ECMLS LocalSync_ShowProperty(ERROR_ST* err, LPCWSTR oid, bool isDocument, LONGLONG hWnd);
BOOL ECMLS LocalSync_IsCheckOut(ERROR_ST* err, LPCWSTR fileOID, LPCWSTR folderOID, LPCWSTR folderFullPathIndex, bool* pIsCheckout);

BOOL ECMLS LocalSync_GetDDPolicyInfo(ERROR_ST* err, PLSDD_POLICY_INFORMATIONEX pDDPolicyInfo);
LRESULT ECMLS LocalSync_NotifyMessage(LS_NOTIFY_MESSAGE message, WPARAM wParam, LPARAM lParam);

BOOL ECMLS LocalSync_MoveFolders(ERROR_ST* err, LPCWSTR szFolderOIDs, LPCWSTR szTargetFolderOID, LPCWSTR param2);
BOOL ECMLS LocalSync_MoveDocuments(ERROR_ST* err, LPCWSTR szDocumentOIDs, LPCWSTR szTargetFolderOID, LPCWSTR param2);

BOOL ECMLS LocalSync_DuplicateDocumentName(ERROR_ST* err, LPCWSTR param0, LPCWSTR param1, LPCWSTR param2, bool* pbResult);
BOOL ECMLS LocalSync_DuplicateFolderName(ERROR_ST* err, LPCWSTR param0, LPCWSTR param1, LPCWSTR param2, bool* pbResult);
BOOL ECMLS LocalSync_IsExistedFolder(ERROR_ST* err, LPCWSTR folderOID, LPCWSTR folderFullPathIndex, bool* pbResult);

DWORD ECMLS LocalSync_ExpandSecurityDisk(ULONGLONG requiredSize);
BOOL ECMLS LocalSync_GetDlpMediaPermission(DlpMediaInfo* dlpMediaInfo);

BOOL ECMLS LocalSync_GetDocTypeInfosByFolderOID(ERROR_ST* err, LPCWSTR folderOID, LPWSTR* outRes);
BOOL ECMLS LocalSync_GetDocTypeData(ERROR_ST* err, LPCWSTR docTypeOID, OUT XDOCTYPE_ST** outRes);

// 확장속성 관련
BOOL ECMLS LocalSync_GetDocumentWithExtAttr(ERROR_ST* err, LPCWSTR szDocOID, OUT XSYNCDOCUMENTWITHEXTATTR_ST** ppXDoc);
BOOL ECMLS LocalSync_CreateDocWithExtAttr(ERROR_ST* err, XSYNCDOCUMENTWITHEXTATTR_ST* document, OUT XSYNCDOCUMENTWITHEXTATTR_ST** ppXDoc);
BOOL ECMLS LocalSync_UpdateDocWithExtAttr(ERROR_ST* err, XSYNCDOCUMENTWITHEXTATTR_ST* document, OUT XSYNCDOCUMENTWITHEXTATTR_ST** ppXDoc);
BOOL ECMLS LocalSync_GetObjectOIDByLocalFullPathName(ERROR_ST* err, LPCWSTR localPath, OUT LPWSTR oid);

BOOL ECMLS LocalSync_GetAllDocType(ERROR_ST* err, OUT int *xDocTypeSize, OUT XDOCTYPE_ST **ppXDoc);
#ifdef __cplusplus
} // extern "C"
#endif