#ifndef K_MEMORY_H
#define K_MEMORY_H

#include "PlatformLib.h"

// Alloc, Free, Lock, Unlock
#define USE_MEMORY_HANDLE
#define USE_MEMORY_TRACE // check for leak


// #define DEBUG_MEMORY_LOCALHEAP
#define _DEBUG_MEMORY_MFC
#define USE_STD_MEMSET

#ifdef _DEBUG_MEMORY_MFC

#include <crtdbg.h>
#define HCMemAlloc(size) _malloc_dbg(size, _NORMAL_BLOCK, __FILE__, __LINE__)
#define HCMemFree(p) _free_dbg(p, _NORMAL_BLOCK)
#define HCMemRealloc(p, s) _realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)

#define HCMemInitialize(x)
#define HCMemDump()
#define HCMemClose()
#define HCMemGarbageCollect()

#elif defined(_DEBUG_MEMORY_DIRECT)

LPSTR HCMemAlloc(int size);
void HCMemFree(LPVOID lp);
void CheckAllocMemList();

#define HCMemInitialize(x)
#define HCMemDump()
#define HCMemClose()
#define HCMemGarbageCollect()

#elif defined(DEBUG_MEMORY_LOCALHEAP)

void HCMemInitialize(int fixed_size);
void* HCMemAlloc(int size);
void* HCMemRealloc(void* lp, int size);
void HCMemFree(void* lp);
void HCMemGarbageCollect();
void HCMemClose();

void HCMemDump();
void HCMemBlockDump();
#else

#define HCMemAlloc(size) malloc(size)
#define HCMemFree(p) free(p)

#define HCMemInitialize(x)
#define HCMemDump()
#define HCMemClose()

#endif

#ifdef USE_MEMORY_HANDLE
void KMemoryInit();
void KMemoryDump();
HANDLE KMemoryAlloc(SIZE_T size);
void KMemoryFree(HANDLE h);
SIZE_T KMemorySize(HANDLE h);
LPVOID KMemoryLock(HANDLE h);
BOOL KMemoryUnlock(HANDLE h);
#endif

#ifdef USE_STD_MEMSET
#define HCMemcpy(d, s, l) memcpy(d, s, l)
#define HCMemcmp(d, c, l) memcmp(d, c, l)
#define HCMemset(d, c, l) memset(d, c, l)
#else
#define HCMemcpy(d, s, l) K2P_MemoryCopy(d, s, l)
#define HCMemcmp(d, s, l) K2P_MemoryCompare(d, s, l)
#define HCMemset(d, c, l) K2P_MemorySet(d, c, l)
#endif

void HCMemGetInformation(int& total, int& used, int& k_max);
bool IsSystemMemoryLow(bool bOnlyBusy=false);

#endif // K_MEMORY_H
