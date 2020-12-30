#pragma once
#include "TwainApp.h"

#include "..\Utils.h"

#define DEVICE_NAME_BUFF_LEN 64
#define DEVICE_SID_BUFF_LEN 64
#define MANUFACTURE_BUFF_LEN 64

#define WM_KTWAIN_MSG (WM_USER + 400)

#define SCAN_SUCCESS 0
#define SCAN_BAD_DEVICE_TYPE 1
#define SCAN_IN_PROCESS 2
#define SCAN_CONNECT_DSM_FAIL 3
#define SCAN_ENABLE_DS_FAIL 4
#define SCAN_DS_NOT_READY 5

#define SCAN_WIA_NOT_READY 10
#define SCAN_DEVICE_NOT_READY 11
#define SCAN_WIA_NO_INTERFACE 12
#define SCAN_WIA_FAIL_DOWNLOAD 13

typedef enum tagKIMAGING_DEVICE_TYPE {
  K_TWAIN_DEVICE,
  K_WIA_DEVICE
}KIMAGING_TWAIN_DEVICE;

typedef struct tagKImageCaptureDevice
{
  KIMAGING_TWAIN_DEVICE type;
  UINT32 TwainId; // twain device ID
  TCHAR DeviceIDS[64]; // wia device ID
  TCHAR DeviceName[64];
  TCHAR Manufacture[64];
  BSTR bstrDeviceID;
}KImageCaptureDevice;

struct CapSetting
{
  TW_UINT16   CapID;      // The ID of the Capability
  TW_UINT8    byChanged;  // Each bit is used to indicate if that colume has changed.
  TW_UINT8    bReadOnly;  // The Cap is only read only
};

typedef std::vector<KImageCaptureDevice*> KImageCaptureDeviceVector;
typedef std::vector<KImageCaptureDevice*>::iterator KImageCaptureDeviceIterator;


class KTwainControl
{
public:
  KTwainControl();
  ~KTwainControl();

public:
  BOOL Init(HWND hWnd);
  BOOL ConnectDSM();
  BOOL DisconnectDSM();

  int GetDataSourceList(KImageCaptureDeviceVector& vector);
  BOOL LoadDataSource(UINT32 deviceID);
  BOOL UnloadDataSource();
  void GetDataSourceInfo(char *buff, int length);

  int GetState();
  void SetState(int state);

  inline TW_IDENTITY* GetAppID() { return mAppID; }
  inline void SetDSMessage(TW_UINT16 m) { if (m_TWAINApp != NULL) m_TWAINApp->m_DSMessage = m; }
  inline BOOL IsUseCallback() { if (m_TWAINApp != NULL) return m_TWAINApp->gUSE_CALLBACKS;  return FALSE; }

  pTW_IDENTITY GetDataSource();
  TW_INT16 GetCAP(TW_CAPABILITY& cap);
  TW_INT16 GetLabel(TW_UINT16 _cap, string &sLable);
  TW_INT16 QuerySupport_CAP(TW_UINT16 cap, TW_UINT32& qs);

  TW_UINT16 SetCapabilityOneValue(TW_UINT16 Cap, const int value, TW_UINT16 type);
  TW_UINT16 SetCapabilityOneValue(TW_UINT16 Cap, const pTW_FIX32 value);
  TW_UINT16 SetCapabilityOneValue(TW_UINT16 Cap, const pTW_FRAME value);

  BOOL EnableADF(HWND hWnd, BOOL enableADF);

  BOOL EnableDS(TW_HANDLE hWnd, BOOL bShowUI);
  BOOL DisableDS();

  BOOL GetNativeBitmaps(KVoidPtrArray& imageArray);
  BOOL initiateTransfer_File(TW_UINT16 fileformat, KVoidPtrArray* pArray, LPCTSTR workingPath);
  BOOL initiateTransfer_Memory();

  void OnDSMessage(TW_UINT16 msg);

  static KTwainControl* CreateController(HWND hWnd);
  static void CloseController();
  static void ClearDataSourceList(KImageCaptureDeviceVector& vector);

private:
  HWND mParentWnd;
  TwainApp* m_TWAINApp;
  TW_IDENTITY* mAppID;
  //int KTwainControl PopulateDataSourceList();

};

