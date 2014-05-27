#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

//#ifdef __cplusplus
//extern "C" 
//{
//
//#endif
//
//#include <ntddk.h>
//#include <ntddstor.h>
//#include <mountdev.h>
//#include <ntddvol.h>
//
//#include "common.h"
//#include "debug.h"
//// #include "dll_inject.h"
//#include "runtime.h"
//#include "undocnt.h"
//#include "shellcode2_struct.h"
//
//#ifdef __cplusplus
//}
//#endif
//

#pragma warning(disable: 4200)

extern "C"
{

#include <ntddk.h>
#include <ntimage.h>
#include <ntddstor.h>
#include "undocnt.h"
}
#include "common.h"
#include "debug.h"
#include "runtime.h"
// #include "dll_inject.h"
