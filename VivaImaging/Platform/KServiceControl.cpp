#include "stdafx.h"
#include <tchar.h>
#include <Winsvc.h>

#include "KServiceControl.h"

#include "Core/KServiceDefines.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void SendKSyncServiceControl(int code)
{
    SC_HANDLE schSCManager = NULL;
    SC_HANDLE schService = NULL;
    SERVICE_STATUS ssSvcStatus = {};

    // Open the local default service control manager database
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (schSCManager == NULL)
    {
        // wprintf(L"OpenSCManager failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Open the service with delete, stop, and query status permissions
    schService = OpenService(schSCManager, KSYNC_SERVICE_NAME, SERVICE_STOP | 
        SERVICE_QUERY_STATUS | DELETE);
    if (schService == NULL)
    {
        // wprintf(L"OpenService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

    // Try to stop the service
    if (ControlService(schService, code, &ssSvcStatus))
    {
        // wprintf(L"Stopping %s.", pszServiceName);
        Sleep(1000);
    }

    // Now remove the service by calling DeleteService.
    if (!DeleteService(schService))
    {
        // wprintf(L"DeleteService failed w/err 0x%08lx\n", GetLastError());
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (schSCManager)
    {
        CloseServiceHandle(schSCManager);
        schSCManager = NULL;
    }
    if (schService)
    {
        CloseServiceHandle(schService);
        schService = NULL;
    }
}
