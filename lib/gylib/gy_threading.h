/*
File:   gy_threading.h
Author: Taylor Robbins
Date:   03\17\2024
*/

#ifndef _GY_THREADING_H
#define _GY_THREADING_H

#ifdef GYLIB_THREADING_ENABLED

//You must now typedef GyMutex_t before including gy_threading.h and implement the following functions:
void InitGyMutex(GyMutex_t* mutex);
void FreeGyMutex(GyMutex_t* mutex);
bool IsValidGyMutex(GyMutex_t* mutex);
bool TryLockGyMutex(GyMutex_t* mutex, u32 timeout);
void LockGyMutex(GyMutex_t* mutex);
void UnlockGyMutex(GyMutex_t* mutex);

#else // !GYLIB_THREADING_ENABLED

typedef u8 GyMutex_t; //dummy typedef
#ifdef GYLIB_HEADER_ONLY
void InitGyMutex(GyMutex_t* mutex);
void FreeGyMutex(GyMutex_t* mutex);
bool IsValidGyMutex(GyMutex_t* mutex);
bool TryLockGyMutex(GyMutex_t* mutex, u32 timeout);
void LockGyMutex(GyMutex_t* mutex);
void UnlockGyMutex(GyMutex_t* mutex);
#else // !GYLIB_HEADER_ONLY
void InitGyMutex(GyMutex_t* mutex) { } // Stub Function
void FreeGyMutex(GyMutex_t* mutex) { } // Stub Function
bool IsValidGyMutex(GyMutex_t* mutex) { return false; }
bool TryLockGyMutex(GyMutex_t* mutex, u32 timeout) { return false; } // Stub Function
void LockGyMutex(GyMutex_t* mutex) { } // Stub Function
void UnlockGyMutex(GyMutex_t* mutex) { } // Stub Function
#endif // GYLIB_HEADER_ONLY

#endif // GYLIB_THREADING_ENABLED

#endif //  _GY_THREADING_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
GYLIB_THREADING_ENABLED
@Types
GyMutex_t
@Functions
void InitGyMutex(GyMutex_t* mutex)
void FreeGyMutex(GyMutex_t* mutex)
bool IsValidGyMutex(GyMutex_t* mutex)
bool TryLockGyMutex(GyMutex_t* mutex, u32 timeout)
void LockGyMutex(GyMutex_t* mutex)
void UnlockGyMutex(GyMutex_t* mutex)
*/
