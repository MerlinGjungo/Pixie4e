/*	All structures passed between Igor and an XOP must be aligned on
	two-byte boundaries. We instruct the compiler to do this as follows:
	
		#include "XOPStructureAlignmentTwoByte.h"
		. . . define structures here . . .
		#include "XOPStructureAlignmentReset.h"
*/

#if defined(TARGET_OS_MAC)
	#pragma options align=mac68k
#endif

#ifdef WIN32
	#pragma pack(push,2)
#endif
