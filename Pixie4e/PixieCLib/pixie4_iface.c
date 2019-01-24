/*----------------------------------------------------------------------
 * Copyright (c) 2004, 2009 XIA LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, 
 * with or without modification, are permitted provided 
 * that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above 
 *     copyright notice, this list of conditions and the 
 *     following disclaimer.
 *   * Redistributions in binary form must reproduce the 
 *     above copyright notice, this list of conditions and the 
 *     following disclaimer in the documentation and/or other 
 *     materials provided with the distribution.
 *   * Neither the name of XIA LLC 
 *     nor the names of its contributors may be used to endorse 
 *     or promote products derived from this software without 
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON 
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE.
 *----------------------------------------------------------------------*/


/******************************************************************************
 *
 * File name:
 *
 *      pixie4_iface.c
 *
 * Description:
 *
 *      Igor external operation for Pixie-4.
 *
 * Revision:
 *
 *		3-11-2004
 *
 ******************************************************************************/

#include <XOPStandardHeaders.h>		// Include ANSI headers, Mac headers, IgorXOP.h, XOP.h and XOPSupport.h
#include "pixie4_iface.h"

/* All structures are 2-byte-aligned. */
#if GENERATINGPOWERPC
	#pragma options align=mac68k
#endif
#ifdef _WINDOWS_
	#pragma pack(2)
#endif

#include "XOPStructureAlignmentTwoByte.h"

/* Global Variables (none) */


void
Go(IORecHandle ioRecHandle)
{
	HOST_IMPORT void main(IORecHandle);

	main(ioRecHandle);
}

static S32
RegisterFunction()
{
	int funcIndex;

	/*	NOTE:
		Some XOPs should return a result of NIL in response to the FUNCADDRS message.
		See XOP manual "Restrictions on Direct XFUNCs" section.
	*/

	funcIndex = GetXOPItem(0);		/* which function invoked ? */
	switch (funcIndex)
	{
		case 0:						
			return((S32)Pixie4_Hand_Down_Names);
			break;
		case 1:
			return((S32)Pixie4_Boot_System);
			break;
		case 2:
			return((S32)Pixie4_User_Par_IO);
			break;
		case 3:
			return((S32)Pixie4_Acquire_Data);
			break;
		case 4:
			return((S32)Pixie4_Set_Current_ModChan);
			break;
		case 5:
			return((S32)Pixie4_Buffer_IO);
			break;
	}
	return(NIL);
}


/*	XOPEntry()

	This is the entry point from the host application to the XOP for all messages after the
	INIT message.
*/

static void
XOPEntry(void)
{	
	S32 result = 0;

	switch (GetXOPMessage())
	{
		case FUNCADDRS:
			result = RegisterFunction();
			break;
	}
	SetXOPResult(result);
	// results other than zero are interpreted as errors
	// -1 does not pop up an alert, others do. 
	// -2 .. -32768 are MAC OS errors, some are translated into meaningful messages in the alert,
	//    others show and generic alert. 
	// 10000 .. 10999 are custom alerts defined in PixieWinCustom.rc under 1100 STR#
	//    and indexed in pixie4_iface.h as N + FIRST_XOP_ERR
	// result is the _return_ value of the xop functions. 
	// in the function, p->result passes the return value of the xop function call to Igor
	// For example, 
	//		p->result = 12;
	//      return(0)
	//  returns a value of 12 with no error
	//		p->result = 234;
	//      return(-7)
	//  returns a value of 234 with error code -7
	// In the Pixie xop, both are generally identical (causing extra pop up alerts)
	// WM recommends returning an error only for serious problems. 
}

/*	main(ioRecHandle)

	This is the initial entry point at which the host application calls XOP.
	The message sent by the host must be INIT.
	main() does any necessary initialization and then sets the XOPEntry field of the
	ioRecHandle to the address to be called for future messages.
*/

HOST_IMPORT void
main(ioRecHandle)
	IORecHandle ioRecHandle;
{

#ifdef applec					/* for MPW C for 68K only */
	void _DATAINIT(void);
	_DATAINIT();				/* for MPW C only */
	UnloadSeg(_DATAINIT);
#endif

#ifdef XOP_GLOBALS_ARE_A4_BASED
	#ifdef __MWERKS__
		SetCurrentA4();									/* Set up correct A4. This allows globals to work. */
		SendXOPA4ToIgor(ioRecHandle, GetA4());			/* And communicate it to Igor. */
	#endif
#endif
	
	//LoadXOPSegs(); obsolete in toolkit 5
	XOPInit(ioRecHandle);							/* do standard XOP initialization */
	SetXOPEntry(XOPEntry);							/* set entry point for future calls */
	
	if (igorVersion < 200)
		SetXOPResult(REQUIRES_IGOR_200);
	else
		SetXOPResult(0L);
}

#if GENERATINGPOWERPC
	#pragma options align=reset
#endif
#ifdef _WINDOWS_
	#pragma pack()
#endif
#include "XOPStructureAlignmentReset.h"
/* All structures are 2-byte-aligned */