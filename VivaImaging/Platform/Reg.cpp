/****************************** Module Header ******************************\
Module Name:  Reg.cpp
Project:      CppShellExtContextMenuHandler
Copyright (c) Microsoft Corporation.

The file implements the reusable helper functions to register and unregister 
in-process COM components and shell context menu handlers in the registry.

RegisterInprocServer - register the in-process component in the registry.
UnregisterInprocServer - unregister the in-process component in the registry.
RegisterShellExtContextMenuHandler - register the context menu handler.
UnregisterShellExtContextMenuHandler - unregister the context menu handler.

This source is subject to the Microsoft Public License.
See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
All other rights reserved.

THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***************************************************************************/

#include "stdafx.h"
#include "Reg.h"
#include <strsafe.h>
#include <Shlwapi.h>
#include <objbase.h>
#include <psapi.h>

#include "Platform.h"

#include "../Core/KSettings.h"
#include "../Core/KConstant.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma region Registry Helper Functions

static PCTSTR DESTINY_ECM_AGENT_KEY = L"Software\\AppDataLow\\Software\\DestinyECM\\DestinyECMAgent";

static TCHAR DEA_LANGUAGE_NAME[] = L"Language";
static TCHAR DEA_USE_AUTO_SYNC[] = L"UseLSAutoSync";
static TCHAR DEA_EXPLORER_TYPE[] = L"ExplorerType";
static TCHAR DEA_RPC_IP[] = L"RPCIP";
static TCHAR DEA_RPC_PORT[] = L"RPCPort";
static TCHAR DEA_RPC_PATH[] = L"StreamingContextPath";
static TCHAR DEA_USE_SSL[] = L"UseSSL";
static TCHAR DEA_SESSION_TIMEOUT[] = L"SessionTimeout";
static TCHAR DEA_IS_APPLIANCE[] = L"IsAppliance";

WORD RegGetDestinyAgentLanguage()
{
	HKEY hKey = NULL;
	WORD rtn = 0x409; // default is EN
	TCHAR data[32];
	DWORD cbData;

	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, DESTINY_ECM_AGENT_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		cbData = 32 * sizeof(TCHAR);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_LANGUAGE_NAME, NULL,
			NULL, reinterpret_cast<LPBYTE>(data), &cbData));

		if (SUCCEEDED(hr))
    {
      if ((toupper(data[0]) == 'K') && (toupper(data[1]) == 'O'))
        rtn = 0x412;
      else if ((toupper(data[0]) == 'Z') && (toupper(data[1]) == 'H'))
        rtn = 0x804;
      /*
      else if ((toupper(data[0]) == 'J') && (toupper(data[1]) == 'P'))
        rtn = 0x411;
      */
    }
	  RegCloseKey(hKey);
	}
  return rtn;
}

WORD RegGetDestinyAgentUseAutoSync()
{
	HKEY hKey = NULL;
	WORD rtn = 1; // default is Enable
	DWORD data;
	DWORD cbData;

	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, DESTINY_ECM_AGENT_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		cbData = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_USE_AUTO_SYNC, NULL,
			NULL, reinterpret_cast<LPBYTE>(&data), &cbData));

		if (SUCCEEDED(hr))
    {
      if (data == 0)
        rtn = 0;
    }
	  RegCloseKey(hKey);
	}
  return rtn;
}

// windowMode : 1 : 전용 탐색기, 2 : 윈도우 탐색기
// timeout_ms : 세션 타임아웃 MS
// ecmMode : 1 : ECM, 0 : Claudium
BOOL RegGetDestinyAgentValues(OUT WORD& windowMode, OUT int& timeout_ms, OUT WORD& ecmMode)
{
	HKEY hKey = NULL;
	BOOL rtn = TRUE; // default is Enable
	DWORD data;
	DWORD cbData;

	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, DESTINY_ECM_AGENT_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		cbData = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_EXPLORER_TYPE, NULL,
			NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
		if (SUCCEEDED(hr))
      windowMode = (WORD) data;

		RegCloseKey(hKey);
	}
	else
		rtn = FALSE;

	hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, DESTINY_ECM_AGENT_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
    timeout_ms = 30000;
		cbData = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_SESSION_TIMEOUT, NULL,
			NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
		if (SUCCEEDED(hr))
      timeout_ms = (int)data;

    ecmMode = 0; // default is Claudium
		cbData = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_IS_APPLIANCE, NULL,
			NULL, reinterpret_cast<LPBYTE>(&data), &cbData));
		if (SUCCEEDED(hr))
      ecmMode = (data == 0) ? 1 : 0;

		RegCloseKey(hKey);
	}
	else
		rtn = FALSE;
  return rtn;
}

BOOL RegGetServerFolderScanUrl(LPTSTR buff, int len)
{
	HKEY hKey = NULL;

  buff[0] = '\0';
	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, DESTINY_ECM_AGENT_KEY, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
	  DWORD cbData;
    TCHAR address[256];
    TCHAR path[256];
    TCHAR use_ssl[8];
    DWORD port = 0;
    BOOL useSSL = FALSE;

    /*
    기존에 HKCU에서 읽고 값이 없으면 HKLM에서 다시 읽어오게 하였으나 이제는 HKLM에서만 읽음(2018.11.15)
    */
#ifdef _UNUSED_DOUBLE_CHECK
      auto regQueryValueLocalMachine = [&address, &path, &port, &useSSL](bool getAddress, bool getPath, bool getPort, bool getUseSSL) -> HRESULT
      {
        HKEY hKey = NULL;
        HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_LOCAL_MACHINE, DESTINY_ECM_AGENT_KEY, 0,
	    NULL, REG_OPTION_NON_VOLATILE, KEY_READ, NULL, &hKey, NULL));

	    if (SUCCEEDED(hr))
	    {
          DWORD cbData;
          if (getAddress)
          {
            cbData = sizeof(TCHAR) * 256;
            hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_IP, NULL,
              NULL, reinterpret_cast<LPBYTE>(address), &cbData));

            if (!SUCCEEDED(hr))
              address[0] = L'\0';
          }

          if (getPath)
          {
            cbData = sizeof(TCHAR) * 256;
            hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_PATH, NULL,
              NULL, reinterpret_cast<LPBYTE>(path), &cbData));

            if (!SUCCEEDED(hr))
              path[0] = L'\0';
          }

          if (getPort)
          {
            cbData = sizeof(DWORD);
            hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_PORT, NULL,
              NULL, reinterpret_cast<LPBYTE>(&port), &cbData));

            if (!SUCCEEDED(hr))
              port = 0;
          }
          if (getUseSSL)
          {
            TCHAR use_ssl[8];
            cbData = sizeof(use_ssl);
            hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_USE_SSL, NULL,
              NULL, reinterpret_cast<LPBYTE>(use_ssl), &cbData));
            if (SUCCEEDED(hr))
              useSSL = CompareStringICNC(use_ssl, _T("true")) == 0;
          }
        }

        RegCloseKey(hKey);

        return hr;
      };
#endif

		cbData = sizeof(TCHAR) * 256;
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_IP, NULL,
			NULL, reinterpret_cast<LPBYTE>(address), &cbData));

		if (!SUCCEEDED(hr))
      address[0] = L'\0';

		cbData = sizeof(TCHAR) * 256;
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_PATH, NULL,
			NULL, reinterpret_cast<LPBYTE>(path), &cbData));

		if (!SUCCEEDED(hr))
      path[0] = L'\0';

    cbData = sizeof(DWORD);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_RPC_PORT, NULL,
			NULL, reinterpret_cast<LPBYTE>(&port), &cbData));

		if (!SUCCEEDED(hr))
      port = 0;

    cbData = sizeof(use_ssl);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, DEA_USE_SSL, NULL,
			NULL, reinterpret_cast<LPBYTE>(use_ssl), &cbData));
    if (SUCCEEDED(hr))
      useSSL = CompareStringICNC(use_ssl, _T("true")) == 0;
    else
      useSSL = -1;

#ifdef _UNUSED_DOUBLE_CHECK
    regQueryValueLocalMachine(L"\0" == address, L'\0' == path[0], 0 == port, -1 == useSSL);
#endif

    if (lstrlen(address) > 0)
    {
      StringCchCopy(buff, len, L"http");
      if (useSSL)
        StringCchCat(buff, len, L"s");
      StringCchCat(buff, len, L"://");
      StringCchCat(buff, len, address);
    }
    if (port > 0)
    {
      TCHAR port_num[16];
      StringCchPrintf(port_num, 16, L":%d", port);
      StringCchCat(buff, len, port_num);
    }

    if (lstrlen(path) > 0)
    {
      StringCchCat(buff, len, L"/");
      StringCchCat(buff, len, path);
    }

	  RegCloseKey(hKey);
	}
  return (lstrlen(buff) > 0);
}

static PCTSTR VIVA_IMAGING_SESSIONS_KEY = L"Software\\Destiny Imaging\\VivaImaging\\Sessions";

DWORD RegCreateSessionID()
{
  HKEY hKey = NULL;

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER,
    VIVA_IMAGING_SESSIONS_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, NULL));

  if (SUCCEEDED(hr))
  {
    DWORD cbData = sizeof(DWORD);

    for (int i = 0; i < MAX_SESSION_ID; i++)
    {
      WCHAR sessionid[32];
      DWORD processid = 0;

      StringCchPrintf(sessionid, 32, _T("SessionID%d"), i);
      hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, sessionid, NULL,
        NULL, reinterpret_cast<LPBYTE>(&processid), &cbData));

      if (SUCCEEDED(hr))
      {
        HANDLE process_handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processid);
        if (process_handle != NULL)
        {
          HMODULE hMod;
          DWORD cbNeeded;

          if (EnumProcessModules(process_handle, &hMod, sizeof(hMod), &cbNeeded))
          {
            CloseHandle(process_handle);
            continue;
          }
          CloseHandle(process_handle);
        }
      }

      // save current process onto sessionid
      processid = GetCurrentProcessId();
      hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, sessionid, NULL,
        REG_DWORD, reinterpret_cast<LPBYTE>(&processid), cbData));

      RegCloseKey(hKey);
      return (DWORD)i;
    }

    RegCloseKey(hKey);
  }
  return INVALID_SESSION_ID;
}

void RegClearSessionID(DWORD sessionId)
{
  HKEY hKey = NULL;

  HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER,
    VIVA_IMAGING_SESSIONS_KEY, 0,
    NULL, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL, &hKey, NULL));

  if (SUCCEEDED(hr))
  {
    WCHAR sessionids[32];

    StringCchPrintf(sessionids, 32, _T("SessionID%d"), (int)sessionId);

    hr = HRESULT_FROM_WIN32(RegDeleteValue(hKey, sessionids));

    if (!SUCCEEDED(hr))
    {
      TCHAR msg[128] = _T("DeleteSessionID fail : ");
      StringCchCat(msg, 128, sessionids);
      StoreLogHistory(_T(__FUNCTION__), msg, SYNC_MSG_LOG);
    }

    RegCloseKey(hKey);
  }
}

// #ifdef USE_SYNC_GUI
#include "Graphics.h"

const TCHAR REGISTRY_APP_SECTION[] = _T("Settings");
const TCHAR REGISTRY_MODULE_START[] = _T("binday");

BOOL LoadRegistryValue(LPCTSTR name, CString& value)
{
  CWinApp* pApp = AfxGetApp();
  value = pApp->GetProfileString(REGISTRY_APP_SECTION, name, NULL);
  return (value.GetLength() > 0);
}

void StoreRegistryValue(LPCTSTR name, CString& value)
{
  CWinApp* pApp = AfxGetApp();
  pApp->WriteProfileString(REGISTRY_APP_SECTION, name, value);
}

void StoreRegistryValue(LPCTSTR name, LPCTSTR value)
{
  CWinApp* pApp = AfxGetApp();
  pApp->WriteProfileString(REGISTRY_APP_SECTION, name, value);
}

BOOL LoadDialogLocation(LPCTSTR name, RECT& rect)
{
  CWinApp* pApp = AfxGetApp();
  CString str = pApp->GetProfileString(REGISTRY_APP_SECTION, name, NULL);
  if (str.GetLength() > 0)
  {
    int r = _stscanf_s(str, _T("%d,%d,%d,%d"), &rect.left, &rect.top, &rect.right, &rect.bottom);
    if ((r == 4) && IsValidWindowPosition(rect))
      return TRUE;
  }
  return FALSE;
}

void StoreDialogLocation(LPCTSTR name, RECT& rect)
{
  TCHAR buff[128];
  StringCchPrintf(buff, 128, _T("%d,%d,%d,%d"), rect.left, rect.top, rect.right, rect.bottom);
  CWinApp* pApp = AfxGetApp();
  pApp->WriteProfileString(REGISTRY_APP_SECTION, name, buff);
}

BOOL RegistryAppTickCount()
{
  LPCTSTR name = _T("WakeCount");
  CWinApp* pApp = AfxGetApp();
  CString value = pApp->GetProfileString(REGISTRY_APP_SECTION, name, NULL);

  int tick_count = 0;
  if (value.GetLength() > 0)
    _stscanf_s(value, _T("%d"), &tick_count);

  tick_count++;
  if (tick_count > 100)
    tick_count = 0;

  TCHAR buff[32];
  StringCchPrintf(buff, 32, _T("%d"), tick_count);
  pApp->WriteProfileString(REGISTRY_APP_SECTION, name, buff);

  return (tick_count == 1);
}

BOOL LoadModuleStateDate(SYSTEMTIME& OUT date, LPCTSTR userOid)
{
#define USE_ECM_AGENT_ACCOUT_LOCATION
  // load-store from DESTINY_ECM_AGENT_KEY = L"Software\\AppDataLow\\Software\\DestinyECM\\DestinyECMAgent";
#ifdef USE_ECM_AGENT_ACCOUT_LOCATION
  TCHAR keyName[256];
  StringCchPrintf(keyName, 256, DESTINY_ECM_AGENT_KEY);
  StringCchCat(keyName, 256, _T("\\Accounts\\Account_"));
  StringCchCat(keyName, 256, userOid);

	HKEY hKey = NULL;
	DWORD cbData;
  TCHAR data[32] = _T("");

	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, keyName, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_READ|KEY_WRITE, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		cbData = 32 * sizeof(TCHAR);
		hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, REGISTRY_MODULE_START, NULL,
			NULL, reinterpret_cast<LPBYTE>(data), &cbData));

		if (!StringToSystemDateT(data, date))
    {
      GetLocalTime(&date);
      StringFromSystemDateT(data, 32, date);
      hr = RegSetValueEx(hKey, REGISTRY_MODULE_START, 0, REG_SZ,
        reinterpret_cast<LPBYTE>(data), sizeof(TCHAR) * (lstrlen(data) + 1));
    }
	  RegCloseKey(hKey);
    return TRUE;
	}
  else
  {
    StringCchCat(keyName, 256, _T(" : not found"));
    StoreLogHistory(_T(__FUNCTION__), keyName, SYNC_MSG_LOG);
  }

#else
  CString value;
  if (!LoadRegistryValue(REGISTRY_MODULE_START, value) ||
    !StringToSystemDateT((LPCTSTR)value, date))
  {
    GetLocalTime(&date);

    TCHAR datestr[32];
    StringFromSystemDateT(datestr, 32, date);
    value.SetString(datestr);
    StoreRegistryValue(REGISTRY_MODULE_START, value);
  }
#endif
  return FALSE;
}

// #endif // USE_SYNC_GUI

#if 0

//
//   FUNCTION: SetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function creates a HKCR registry key and sets the specified 
//   registry value.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function will create the registry key.
//   * pszValueName - specifies the registry value to be set. If pszValueName 
//     is NULL, the function will set the default value.
//   * pszData - specifies the string data of the registry value.
//
//   RETURN VALUE: 
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code.
// 


HRESULT CreateRegistryKeyAndValue(HKEY hkeyRoot, PCWSTR pszSubKey)
{
	HRESULT hr;
	HKEY hKey = NULL;

	// Creates the specified registry key. If the key already exists, the 
	// function opens it. 
	hr = HRESULT_FROM_WIN32(RegCreateKeyEx(hkeyRoot/*HKEY_CURRENT_USER*/, pszSubKey, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		RegCloseKey(hKey);
	}

	return hr;
}


HRESULT SetRegistryKeyAndValue(HKEY hkeyRoot, PCWSTR pszSubKey, PCWSTR pszValueName, 
    PCWSTR pszData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Creates the specified registry key. If the key already exists, the 
    // function opens it. 
    hr = HRESULT_FROM_WIN32(RegCreateKeyEx(hkeyRoot, pszSubKey, 0, 
        NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

    if (SUCCEEDED(hr))
    {
        if (pszData != NULL)
        {
            // Set the specified value of the key.
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0, 
                REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));
        }

        RegCloseKey(hKey);
    }

    return hr;
}


//
//   FUNCTION: GetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function opens a HKCR registry key and gets the data for the 
//   specified registry value name.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function returns an error.
//   * pszValueName - specifies the registry value to be retrieved. If 
//     pszValueName is NULL, the function will get the default value.
//   * pszData - a pointer to a buffer that receives the value's string data.
//   * cbData - specifies the size of the buffer in bytes.
//
//   RETURN VALUE:
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code. For example, if the specified registry key does not 
//   exist or the data for the specified value name was not set, the function 
//   returns COR_E_FILENOTFOUND (0x80070002).
// 
HRESULT GetRegistryKeyAndValue(HKEY hkeyRoot, PCWSTR pszSubKey, PCWSTR pszValueName, 
    PWSTR pszData, DWORD cbData)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Try to open the specified registry key. 
    hr = HRESULT_FROM_WIN32(RegOpenKeyEx(hkeyRoot/*HKEY_CURRENT_USER*/, pszSubKey, 0, 
        KEY_READ, &hKey));

    if (SUCCEEDED(hr))
    {
        // Get the data for the specified value name.
        hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL, 
            NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

        RegCloseKey(hKey);
    }

    return hr;
}

#pragma endregion

static PCWSTR app_key = L"Software\\CyberdigmEx\\SyncFileManager";
static PCTSTR folder_key = L"Software\\CyberdigmEx\\SyncFileManager\\SyncFolders";


HRESULT RegReadyHKCURegistry()
{
	WCHAR sub_key[256] = L"Software\\CyberdigmEx";

	HRESULT hr = CreateRegistryKeyAndValue(HKEY_CURRENT_USER, sub_key);
	if (SUCCEEDED(hr))
	{
		StringCbCat(sub_key, 255, L"\\SyncFileManager");
		hr = CreateRegistryKeyAndValue(HKEY_CURRENT_USER, sub_key);
		if (SUCCEEDED(hr))
		{
			StringCbCat(sub_key, 255, L"\\SyncFolders");
			hr = CreateRegistryKeyAndValue(HKEY_CURRENT_USER, sub_key);

		}
	}
	return hr;
}

LPTSTR RegGetRegisteredSyncFolders()
{
	HKEY hKey = NULL;
	DWORD index = 0;
	TCHAR valueName[64] = L"folder";
	DWORD valueNameSize;
	DWORD type;
	BYTE data[1024];
	DWORD dataSize;
	PTSTR dest = NULL;
	PTSTR d = NULL;
	int alloc_size = 0;

	// Creates the specified registry key. If the key already exists, the 
	// function opens it. 
	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, folder_key, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		do
		{
			dataSize = 1024;
			HRESULT hr = HRESULT_FROM_WIN32(RegEnumValue(hKey, index, valueName, &valueNameSize,
				NULL, &type, (LPBYTE)data, &dataSize));
			if (SUCCEEDED(hr))
			{
				if (dest == NULL)
				{
					alloc_size = 512;
					dest = new TCHAR[alloc_size]);
					d = dest;
				}

				// change byte-size to character size
				dataSize = dataSize / sizeof(TCHAR);
				LPTSTR s_folder = (LPTSTR)data;
				LPTSTR p = StrChr(s_folder, ':');
				if (p != NULL)
				{
					*p = '\0';
					dataSize = lstrlen(s_folder);
					int offset = (int)(d - dest) / sizeof(TCHAR);
					if ((alloc_size - offset) >= (int)(dataSize + 2))
					{
						alloc_size += 512;
						dest = (PWSTR)LocalReAlloc(dest, alloc_size * sizeof(TCHAR), 0);
						d = dest + offset * sizeof(TCHAR);
					}

					StringCchCopy(d, (alloc_size - offset), (LPTSTR)data);
					d += (dataSize + 1);
				}

				index++;
			}
			else {
				break;
			}
		} while (TRUE);

		if (d != NULL)
			*(++d) = '\0';
		RegCloseKey(hKey);
	}
	return dest;
}

BOOL RegAppendRegisteredSyncFolders(LPCTSTR localPathName, LPCTSTR serverPathName)
{
	HKEY hKey = NULL;
	int id = 1;
	TCHAR pszValueName[64];
	TCHAR pszData[512];
	DWORD cbData;

	HRESULT hr = HRESULT_FROM_WIN32(RegCreateKeyEx(HKEY_CURRENT_USER, folder_key, 0,
		NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL));

	if (SUCCEEDED(hr))
	{
		do
		{
			StringCchPrintf(pszValueName, 63, L"%d", id);

			cbData = 512 * sizeof(TCHAR);
			hr = HRESULT_FROM_WIN32(RegQueryValueEx(hKey, pszValueName, NULL,
				NULL, reinterpret_cast<LPBYTE>(pszData), &cbData));

			if ((hr == ERROR_FILE_NOT_FOUND) || (hr == E_ACCESSDENIED))
				break;

			if (!SUCCEEDED(hr))
				return FALSE;

			id++;
		} while (TRUE);
	}

	StringCchCopy(pszData, 511, localPathName);
	StringCchCat(pszData, 511, L":");
	StringCchCat(pszData, 511, serverPathName);

	cbData = lstrlen(pszData) * sizeof(TCHAR);
	hr = HRESULT_FROM_WIN32(RegSetValueEx(hKey, pszValueName, 0,
		REG_SZ, reinterpret_cast<const BYTE *>(pszData), cbData));

	RegCloseKey(hKey);

	return TRUE;
}


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
    PCWSTR pszFriendlyName, PCWSTR pszThreadModel)
{
    if (pszModule == NULL || pszThreadModel == NULL)
    {
        return E_INVALIDARG;
    }

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Create the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = SetRegistryKeyAndValue(HKEY_CLASSES_ROOT, szSubkey, NULL, pszFriendlyName);

        // Create the HKCR\CLSID\{<CLSID>}\InprocServer32 key.
        if (SUCCEEDED(hr))
        {
            hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), 
                L"CLSID\\%s\\InprocServer32", szCLSID);
            if (SUCCEEDED(hr))
            {
                // Set the default value of the InprocServer32 key to the 
                // path of the COM module.
                hr = SetRegistryKeyAndValue(HKEY_CLASSES_ROOT, szSubkey, NULL, pszModule);
                if (SUCCEEDED(hr))
                {
                    // Set the threading model of the component.
                    hr = SetRegistryKeyAndValue(HKEY_CLASSES_ROOT, szSubkey, 
                        L"ThreadingModel", pszThreadModel);
                }
            }
        }
    }

  if (!SUCCEEDED(hr))
    OutputDebugString(L"fail to update RegisterInprocServer");

    return hr;
}


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
HRESULT UnregisterInprocServer(const CLSID& clsid)
{
    HRESULT hr = S_OK;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Delete the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr))
    {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
    }

    return hr;
}

#endif
