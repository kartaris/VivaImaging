#pragma once

#include <vector>
typedef std::vector<void *> KVoidPtrArray;
typedef std::vector<void *>::iterator KVoidPtrIterator;

typedef std::vector<int> KIntVector;
typedef std::vector<int>::iterator KintIterator;

extern BOOL DoesListContainValue(KIntVector& objectId, int value);

extern void AddVoidPtrArray(KVoidPtrArray& dst, KVoidPtrArray& src);

extern void ClearVoidPtrArray(KVoidPtrArray& ar);

extern void ClearTStrArray(KVoidPtrArray& ar);
