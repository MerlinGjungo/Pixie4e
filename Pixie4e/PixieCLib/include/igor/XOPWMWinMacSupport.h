/*	This file provides support for Macintosh routines that are needed by Windows XOPs
	that use full Macintosh emulation.
*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS_			// Compiling for Windows [
	// These are for Macintosh emulation on Windows so that we can write portable XOPs.

	#pragma pack(2)							/* ALL EXTERNAL XOP STRUCTURES ARE 2-byte-aligned. */
	
	// Cross-platform long rect required for "MacQuickDraw.h". Copied from WMStandardIncludes.h.
	typedef struct LongRect {
		long left;
		long top;
		long right;
		long bottom;
	} LongRect;
	typedef LongRect *LongRectPtr;

	// Needed for  "MacQuickDraw.h". Copied from StandardIncludes.h.
	enum ExportType{
		kExpandedPICT,		// expansion factor =1..8; normal MacDraw and LW comments
		kGenericPICT,		// no comments -- just pure quickdraw; may be expanded
		kHiresPICT,			// new high resolution CPICT
		kLaserPICT,			// PS enhanced PICT
		kPostScriptPICT,	// pure PostScript for printable portion of PICT
		kEPSF,				// writing EPS to file
		kHiResBitmap,		// writing to high res bitmap
		kPNGImage,			// writing to PNG image format
		kTiffImage			// writing to TIFF image format
	};

	#include "WinMacTypes.h"
	#include "MacQuickDraw.h"
	#include "MacMenus.h"
	#include "MacFonts.h"
	#include "MacWindows.h"
	#include "MacControls.h"
	#include "MacEvents.h"
	#include "MacFiles.h"
	#include "MacLists.h"
	#include "MacTextEdit.h"
	#include "MacPrinting.h"
	#include "WinGeneralProto.h"
	#include "WinMacProto.h"
	#include "WinMemoryProto.h"
	#include "MacMenusProto.h"
	#include "WinUtilitiesProto.h"
	
	// Miscellaneous routines exported by IGOR.
	HOST_EXPORT HDC GetWindowHDC(WindowPtr theWindow);
	
	// From IGOR's WMFileUtils.c file.
	HOST_EXPORT int GetFullMacPathToDirectory(long vRefNum, long dirIDOrZero, char* pathOut, int maxPathLen);
	HOST_EXPORT void GetStandardFileVRefNumAndDirID(long* vRefNumPtr, long* dirIDPtr);
	HOST_EXPORT void SetStandardFileVRefNumAndDirID(long vRefNum, long dirID);

	#pragma pack()							/* ALL EXTERNAL XOP STRUCTURES ARE 2-byte-aligned. */

#endif						// Compiling for Windows ]

#ifdef __cplusplus
}
#endif
