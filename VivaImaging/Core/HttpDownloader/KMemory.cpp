#include "stdafx.h"
#include "KMemory.h"
#include "PlatformLib.h"

#if defined(WIN32) && defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifdef _DEBUG_MEMORY_DIRECT

#ifdef TEST_ALLOC_FREE_MEM
#define MAX_ALLOC_LIST	4000
DWORD	gpAllocMemList[MAX_ALLOC_LIST];
int		gnAllocMemCount = 0;

void SaveAllocMemList(LPSTR buff)
{
	for(int i=0 ; i<gnAllocMemCount ; i++)
		{
		if(gpAllocMemList[i] == 0)
			{
			gpAllocMemList[i] = (DWORD) buff;
			return;
			}
		}

	if(gnAllocMemCount < MAX_ALLOC_LIST)
		{
		gpAllocMemList[gnAllocMemCount] = (DWORD) buff;
		gnAllocMemCount++;
		}
	else
		KTRACE("memory recording stop");
}

void ClearAllocMemList(LPSTR buff)
{
	for(int i=0 ; i<gnAllocMemCount ; i++)
		{
		if(gpAllocMemList[i] == (DWORD) buff)
			{
			gpAllocMemList[i] = 0;
			return;
			}
		}
	KTRACE("Unrecorded memory freed=%x", (DWORD) buff);
}

#endif // TEST_ALLOC_FREE_MEM

LPSTR HCMemAlloc(int size)
{
#ifdef HLIB_NO_DEBUG
	LPSTR buff = new char[size];
	HCMemset(buff, 0x00, size);
	return(buff);
#else
	LPSTR buff = new char[size + 8];

#ifdef TEST_ALLOC_FREE_MEM
	if(gnAllocMemCount == 0)
		HCMemset(gpAllocMemList, 0, sizeof(DWORD) * MAX_ALLOC_LIST);
	SaveAllocMemList(buff);
#endif

	HCMemset(buff, 0x00, size+8);
	*((int *) buff) = size;
	*((int *) (buff+size+4)) = 0x33445566;
	return(buff+4);
#endif
}

void HCMemFree(LPVOID lp)
{
#ifdef HLIB_NO_DEBUG
	delete (LPSTR) lp;
#else
	LPSTR buff = (LPSTR) lp;
	int		size;

	buff -= 4;
	size = *((int *) buff);
	if( *((int *) (buff+4+size)) != 0x33445566)
		KTRACE("bad memory");
	else
		{
#ifdef TEST_ALLOC_FREE_MEM
		ClearAllocMemList(buff);
#endif
		delete buff;
		}
#endif
}

void CheckAllocMemList()
{
#ifdef TEST_ALLOC_FREE_MEM
	for(int i=0 ; i<gnAllocMemCount ; i++)
		{
		if(gpAllocMemList[i] != 0)
			KTRACE("Unfreed memory=%d:%x", i, (DWORD) gpAllocMemList[i]);
		}
#endif
}

// end _DEBUG_MEMORY_DIRECT

#elif defined(DEBUG_MEMORY_LOCALHEAP)

int m_fixedSize;
int m_usedSize = 0;
int	m_maxBlockSize = 0;
static void* m_fixedLocalMemory = NULL;
KWCHAR dbgmsg[256];

void HCMemInitialize(int fixed_size)
{
	m_fixedSize = fixed_size;
	m_maxBlockSize = fixed_size - 4;
	m_usedSize = 0;
	m_fixedLocalMemory = K2P_MemAlloc(m_fixedSize);
	if(m_fixedLocalMemory == NULL)
		KTRACE_0("fail to create FixedMemory");
	else
		HCMemset(m_fixedLocalMemory, 0, m_fixedSize);
}

void* HCMemAlloc(int size)
{
	BYTE* p;
	DWORD t, block_size;
	DWORD candidate_size;
	BYTE* candidate_p;
	BYTE retry_count = 0;

	// size must be multiple of 4
	block_size = (size + 3) / 4;
	size = (int)(block_size << 2);
	if(size <= 0)
	{
		KTRACE_0("fail to zero-size MemoryAlloc");
		size = 4;
	}

alloc_retry:
	p = (BYTE *)m_fixedLocalMemory;
	candidate_size = 0x0FFFFFFF;
	candidate_p = NULL;

	// KTRACE("HCMemAlloc : %d\n", size);
	while(p < (BYTE*)m_fixedLocalMemory + m_fixedSize)
	{
		t = *((DWORD *) p);
#ifdef _DEBUG
		if(t & 3)
			KTRACE("----KMemory::invalid block size----:%d(0x%x)(address = 0x%x)\n", (t & 0x7FFFFFFF), (t & 0x7FFFFFFF), p);
#endif
		if(t == 0)
		{
			// check candidate is available
			if(candidate_size < 0x0FFFFFFF) // (DWORD)(size * 2))
			{
				if(candidate_size - size >= 8) // need to split block
				{
					*((DWORD *) candidate_p) = ((DWORD) size | 0x80000000); // mark as used area
					p = candidate_p + size + sizeof(DWORD);
					*((DWORD *) p) = (DWORD) (candidate_size - size - sizeof(DWORD)); // remaining block
					m_usedSize += size;
				}
				else
				{
					*((DWORD *) candidate_p) |= 0x80000000; // mark as used area
					m_usedSize += candidate_size;
				}
				candidate_p += sizeof(DWORD);
				return candidate_p;
			}

			// this is free area
			// check needed space
			block_size = m_fixedSize - (int)(p - (BYTE*)m_fixedLocalMemory);
			if(block_size > (DWORD)size)
			{
				*((DWORD *) p) = ((DWORD) size | 0x80000000); // set this is allocated storage;
				p += sizeof(DWORD);
				m_usedSize += size;

				// set end of record flag
				*((DWORD *)(p + size)) = 0;
				// calculate k_max free block
				m_maxBlockSize = m_fixedSize - (int)(p - (BYTE*)m_fixedLocalMemory) - size;
#ifdef KMEM_DEBUG_ALLOC_FREE
				_snwprintf(dbgmsg, 256, _T("alloc=%d, used=%d\n"), size, m_usedSize);
				OutputDebugString(dbgmsg);
#endif
				return p;
			}
			else
				break;
		}
		else if( !(t & 0x80000000) ) // this is freed area
		{
			block_size = (t & 0x7FFFFFFF);
			if((block_size >= (DWORD)size) && (block_size <= (DWORD)(size+8)))
			{
				*((DWORD *) p) |= 0x80000000; // mark as used area
				p += sizeof(DWORD);
				m_usedSize += block_size;
#ifdef KMEM_DEBUG_ALLOC_FREE
				_snwprintf(dbgmsg, 256, _T("alloc=%d, used=%d\n"), size, m_usedSize);
				OutputDebugString(dbgmsg);
#endif
				return p;
			}
			else if(block_size > (DWORD)size) // && (block_size < (DWORD)size * 2))
			{
				// this will be candidate
				if(candidate_size > block_size)
				{
					candidate_size = block_size;
					candidate_p = p;
				}
			}
		}
		else
		{
			block_size = (t & 0x7FFFFFFF);
		}
		p += sizeof(DWORD) + block_size;
	};

	retry_count++;
	if(retry_count < 2)
	{
		HCMemGarbageCollect();
		goto alloc_retry;
	}

alloc_fault:
	KTRACE("----KMemory::Alloc failure----:needed=%d(0x%x), k_max block=%d\n", size, size, m_maxBlockSize);
	HCMemBlockDump();
	return NULL;
}

void* HCMemRealloc(void* p, int size)
{
	DWORD block_size;

	// size must be multiple of 4
	block_size = (size + 3) / 4;
	size = (int)(block_size << 2);

	if(p == NULL)
		return HCMemAlloc(size);

	// printf("HCMemRealloc : %d\n", size);
	if( (p > (BYTE*)m_fixedLocalMemory) && (p < (BYTE*)m_fixedLocalMemory + m_fixedSize) )
	{
		DWORD* buff = (DWORD *) p;

		buff--;
		if(*buff & 0x80000000)
		{
			DWORD ext_size, new_size, remain_size;
			DWORD curr_block_size;

			block_size = (*buff & 0x7FFFFFFF);
			if(block_size >= (DWORD)size)
				return p;

			// next block
			ext_size = 0;
			buff += (block_size + sizeof(DWORD)) / sizeof(void*);
			while(true)
			{
				if( !(*buff & 0x80000000) ) // this is free block
				{
					if(*buff == 0) // this is end of records
					{
						if(size <= (m_fixedSize - ((BYTE*)p - (BYTE*)m_fixedLocalMemory)))
						{
							buff = (DWORD *) p;
							buff--;
							*buff = ((DWORD) size | 0x80000000);

							// set end of record flag
							*((DWORD *)((BYTE*)p + size)) = 0;

							m_usedSize += (size - block_size);

							// calculate k_max free block
							m_maxBlockSize = m_fixedSize - (int)((BYTE*)p - (BYTE*)m_fixedLocalMemory) - size;
							// KTRACE("realloc on free 0x%x, size=%d\n", p, size);
							return p;
						}
						else
							return NULL;
					}

					curr_block_size = (*buff & 0x7FFFFFFF) + sizeof(DWORD);
					ext_size += curr_block_size;
					if((block_size + ext_size) >= (DWORD)size)
					{
						remain_size = (block_size + ext_size - size);
						if(remain_size >= 8)
						{
							// split it
							new_size = size;
							buff = (DWORD *) p;
							buff += new_size / sizeof(void*);
							*buff = remain_size - sizeof(DWORD);
							// KTRACE("split on 0x%x -> 0x%x, ext_size=%d, size=%d\n", p, buff, ext_size, remain_size - sizeof(DWORD));
						}
						else
						{
							new_size = block_size + ext_size;
						}

						buff = (DWORD *) p;
						buff--;
						*buff = ((DWORD) new_size | 0x80000000);
						// KTRACE("realloc on 0x%x, size=%d\n", buff, new_size);

						m_usedSize += (new_size - block_size);
						return p; // stretch buffer success.
					}
					buff += curr_block_size / sizeof(void*);
				}
				else
					break;
			}

			// cannot realloc, allocate new buffer
			LPBYTE b = (LPBYTE)HCMemAlloc(size);
			if(b != NULL)
			{
				// KTRACE("first alloc on 0x%x, size=%d\n", b, size);
				HCMemcpy(b, p, block_size);
				HCMemset(b+block_size, 0, size - block_size);
				HCMemFree(p);
			}
			return b;
		}
	}
	// bad realloc memory
	KTRACE("----KMemory::Realloc failure----:bad block(0x%x)\n", p);
	return NULL;
}

void HCMemFree(void* p)
{
	if( (p > (BYTE*)m_fixedLocalMemory) && (p < (BYTE*)m_fixedLocalMemory + m_fixedSize) )
	{
		DWORD* buff = (DWORD *) p;
		buff--;
		if(*buff & 0x80000000)
		{
			*buff &= 0x7FFFFFFF; // mark as unused area
			m_usedSize -= (int) *buff;
#ifdef KMEM_DEBUG_ALLOC_FREE
			_snwprintf(dbgmsg, 256, _T("free=%d, used=%d\n"), (int) *buff, m_usedSize);
			OutputDebugString(dbgmsg);
#endif
		}
#ifdef _DEBUG_DUMP
		else
		{
			_snwprintf(dbgmsg, 256, _T("already freed=%d\n"), (int) *buff);
			OutputDebugString(dbgmsg);
		}
#endif
	}
}

void HCMemClose()
{
	if(m_fixedLocalMemory != NULL)
		K2P_MemFree(m_fixedLocalMemory);
	m_fixedLocalMemory = NULL;
	m_fixedSize = 0;
	return;
}

void HCMemGarbageCollect()
{
	BYTE* p = (BYTE *)m_fixedLocalMemory;
	DWORD t, block_size, total;
	BYTE* free_start;
	DWORD free_size, free_block;

	total = 0;
	free_start = NULL;
	free_block = 0;
	m_usedSize = 0;
	block_size = free_size = 0;
	m_maxBlockSize = 0;
	KTRACE("GarbageCollect start(maxBlockSize=0x%x)\n", m_maxBlockSize);

	while(p < (BYTE*)m_fixedLocalMemory + m_fixedSize)
	{
		t = *((DWORD *) p);
		if(t == 0)
		{
			if(free_block > 1)
			{
				*((DWORD *)free_start) = 0;
#ifdef _GARBAGE_COLLECT_TRACE
				KTRACE("last %d block is merged as %d(0x%x)\n", free_block, free_size, free_size);
#endif
				p = free_start;
			}

			total += block_size;

			block_size = (DWORD)(((BYTE*)m_fixedLocalMemory + m_fixedSize) - p);
			if(m_maxBlockSize < (int)block_size)
				m_maxBlockSize = (int)block_size;
			break;
		}

		block_size = (t & 0x7FFFFFFF);
		if( !(t & 0x80000000) )
		{
			if(free_start == NULL)
			{
				free_start = p;
				free_size = block_size;
				free_block = 1;
			}
			else
			{
				free_size += block_size + sizeof(DWORD);
				free_block++;
			}
			if(m_maxBlockSize < (int)block_size)
				m_maxBlockSize = (int)block_size;
#ifdef _GARBAGE_COLLECT_TRACE
			KTRACE("free=%d(0x%x)\n", block_size, block_size);
#endif
		}
		else
		{
			if(free_block > 1)
			{
				*((DWORD *)free_start) = free_size;
				if(m_maxBlockSize < (int)free_size)
					m_maxBlockSize = (int)free_size;
#ifdef _GARBAGE_COLLECT_TRACE
				KTRACE("%d block is merged as %d\n", free_block, free_size);
#endif
			}
			free_start = NULL;
			free_block = 0;
			m_usedSize += block_size;
#ifdef _GARBAGE_COLLECT_TRACE
			KTRACE("used=%d(0x%x)\n", block_size, block_size);
#endif
		}
		total += sizeof(DWORD) + block_size;
		p += sizeof(DWORD) + block_size;
	};

	KTRACE("GarbageCollect end(maxBlockSize=0x%x), total=%d, used=%d\n", m_maxBlockSize, total, m_usedSize);
	// HCMemBlockDump();
}

void HCMemDump()
{
	KTRACE("KMemory\ntotal memory=%d,used memory=%d(%d%%), maxBlock=%d\n",
		m_fixedSize, m_usedSize, m_usedSize * 100 / m_fixedSize, m_maxBlockSize);
}

void HCMemBlockDump()
{
	BYTE* p = (BYTE *)m_fixedLocalMemory;
	DWORD t, block_size, total;
	DWORD used, freed;

	total = used = freed = 0;
	while(p < (BYTE*)m_fixedLocalMemory + m_fixedSize)
	{
		t = *((DWORD *) p);
		if(t == 0)
		{
			block_size = (DWORD)(((BYTE*)m_fixedLocalMemory + m_fixedSize) - p);
			KTRACE("empty=%d(0x%x)\n", block_size, block_size);
			total += block_size;
			freed += block_size;
			break;
		}

		block_size = (t & 0x7FFFFFFF);
		if(t & 0x80000000)
		{
			KTRACE("used=");
			used += block_size;
		}
		else
		{
			KTRACE("free=");
			freed += block_size;
		}
		KTRACE("%d(0x%x) p=0x%x\n, offset=%d(0x%x)", block_size, block_size, p, p - m_fixedLocalMemory, p - m_fixedLocalMemory);
		total += sizeof(DWORD) + block_size;

		p += sizeof(DWORD) + block_size;
	};

	KTRACE("FixedMemory=%d(0x%x), sum=%d(0x%x), used=%d, freed=%d\n", m_fixedSize, m_fixedSize, total, total, used, freed);
}

#endif // end DEBUG_MEMORY_LOCALHEAP

#ifdef USE_MEMORY_HANDLE

#ifdef USE_MEMORY_TRACE
#define MAX_ALLOC_LIST	4000
HANDLE	gpAllocMemList[MAX_ALLOC_LIST];
int		gnAllocMemCount = 0;
#endif

void KMemoryInit()
{
#ifdef USE_MEMORY_TRACE
  gnAllocMemCount = 0;
  memset(gpAllocMemList, 0, sizeof(HANDLE) * MAX_ALLOC_LIST);
#endif
}
void KMemoryDump()
{
#ifdef USE_MEMORY_TRACE
  TCHAR msg[64];
  for (int i = 0; i < gnAllocMemCount; i++)
  {
    if (gpAllocMemList[i] != NULL)
    {
      StringCchPrintf(msg, 64, _T("Memory Leak=0x%x\n"), gpAllocMemList[i]);
      OutputDebugString(msg);
    }
  }
#endif
}

HANDLE KMemoryAlloc(SIZE_T size)
{
  HANDLE h = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, size);
#ifdef USE_MEMORY_TRACE
  gpAllocMemList[gnAllocMemCount++] = h;
#endif
  return h;
}

void KMemoryFree(HANDLE h)
{
#ifdef USE_MEMORY_TRACE
  TCHAR msg[64];
  for (int i = 0; i < gnAllocMemCount; i++)
  {
    if (gpAllocMemList[i] == h)
    {
      gpAllocMemList[i] = NULL;
      return;
    }
  }
  StringCchPrintf(msg, 64, _T("No Memory Free=0x%x\n"), h);
  OutputDebugString(msg);
#endif
  GlobalFree(h);
}

SIZE_T KMemorySize(HANDLE h)
{
  return GlobalSize(h);
}

LPVOID KMemoryLock(HANDLE h)
{
  return GlobalLock(h);
}

BOOL KMemoryUnlock(HANDLE h)
{
  return GlobalUnlock(h);
}

#endif

void HCMemGetInformation(int& total, int& used, int& k_max)
{
#ifdef DEBUG_MEMORY_LOCALHEAP
	total = m_fixedSize;
	used = m_usedSize;
	k_max = m_maxBlockSize;
#else
	total = 10000000;
	used = 1;
	k_max = 10000000;
#endif
}

bool IsSystemMemoryLow(bool bOnlyBusy)
{
#ifdef DEBUG_MEMORY_LOCALHEAP
	bool rtn;

	rtn = (m_fixedSize / 10 >= m_maxBlockSize);
	if(rtn)
	{
		HCMemGarbageCollect();
		rtn = (m_fixedSize / 10 >= m_maxBlockSize);
	}
	return(rtn);
#else
	return false;
#endif
}
