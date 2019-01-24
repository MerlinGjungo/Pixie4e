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
 *      pixie4_igor.c
 *
 * Description:
 *
 *      Pixie-4 functions seen by the Igor interface.
 *
 * Revision:
 *
 *		12-1-2004
 *
 * Member functions:
 *
 *		Pixie4_Hand_Down_Names
 *		Pixie4_Boot_System
 *		Pixie4_User_Par_IO
 *		Pixie4_Acquire_Data
 *		Pixie4_Set_Current_ModChan
 *		Pixie4_Buffer_IO
 *
 ******************************************************************************/


#include <windows.h>
#include <tchar.h>
#include <strsafe.h>

#include <XOPStandardHeaders.h>
#include <stdlib.h>
#include <stdio.h>

#include "PlxTypes.h"
#include "Plx.h"

#include "globals.h"
#include "sharedfiles.h"
#include "utilities.h"

/****************************************************************
 *	Pixie4_Hand_Down_Names function:
 *		Download file names or parameter names from the host
 *		to the Pixie-4 C driver.
 *
 *		Names is a Igor text wave containing either the
 *		All_Files or parameter names.
 *
 *		All_Files is a string array with 16 entries which include
 *		the communication FPGA file, FiPPI file, DSP code bianry
 *		file(DSPcode.bin), DSP code I/O parameter names file (DSPcode.var),
 *		DSP code all parameter names file (.lst), and DSP I/O parameter
 *		values file (.itx).
 *
 *		Return Value:
 *			 0 - download successful
 *			-1 - invalid name
 *			-2 - cannot allocate memory for file names
 *			-3 - cannot allocate memory for parameter names
 *
 *    Revised 2/15/16 WH: Occasional crashes of this function, except in Igor debug mode: traced down to "free(New_Names)"
 *                        Workaround: For file names: use global array Boot_File_Name_List, so no new waves need to be created (and no more need to call API function)
 *                                    For parameter names, Igor and C lib globals are identical, so no need to hand them down in the first place. 
 *
 ****************************************************************/

S32 Pixie4_Hand_Down_Names (
		struct {
			Handle Name;		// a string indicating which type of names to be downloaded
			waveHndl Names;		// a two dimensional string array
			double result;		// return value
		}* p )
{

	Handle hTemp = NewHandle(0);
	Handle hName = NewHandle(0);
	S32 indices[4], len, hState, k, Num_Par;
	S8  New_Name[MAX_PAR_NAME_LENGTH];
	S8  *New_Names[MAX(MAX(N_SYSTEM_PAR, N_MODULE_PAR), N_CHANNEL_PAR)];

//sprintf(ErrMSG, "*INFO* (Pixie4_Hand_Down_Names): starting");
//Pixie_Print_MSG(ErrMSG,1);                          //   Pixie_Print_MSGonly seems to work after call to Pixie_Hand_Down_Names, probably due to new threading
//XOPNotice(ErrMSG);                                  // so use plain XOPnotice instead


	/* Copy content of Name to New_Name */
	hName = p->Name;
	hState=MoveLockHandle(hName);
	len=MIN(GetHandleSize(hName), MAX_PAR_NAME_LENGTH);
	strncpy(New_Name, *hName, len);
	New_Name[len]=0;
	HSetState(hName,hState);
	DisposeHandle(hName);

	if(strcmp(New_Name, "ALL_FILES")==0)
	{
		/* Download file names */ 
      indices[1]=0;         // column number
      indices[2]=0;         // layer number
		for( k = 0; k < N_BOOT_FILES; k ++ )
		{
			indices[0] = k;            // row number
			MDGetTextWavePointValue(p->Names, indices, hTemp);
	//		hState = MoveLockHandle(hTemp);                        // supposedly obsolete and should be removed
			len = MIN(GetHandleSize(hTemp), MAX_FILE_NAME_LENGTH);
         // sprintf(ErrMSG, "*INFO* (Pixie4_Hand_Down_Names): file name %d length %d", k, len);
         // Pixie_Print_MSG(ErrMSG,1);
         // XOPNotice(ErrMSG);

   /*      New_Names[k] = (S8 *)malloc(len+1);
			if(New_Names[k] == NULL)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie4_Hand_Down_Names): cannot allocate memory for file names");
				Pixie_Print_MSG(ErrMSG,1);
            XOPNotice(ErrMSG);
				p->result = -2;
				return(p->result);
			}

			strncpy(New_Names[k], *hTemp, len);
			New_Names[k][len]=0;                  */

         strncpy(Boot_File_Name_List[k], *hTemp, len);        // instead of creating array and crashing on freeing it, use global filename array directly
			Boot_File_Name_List[k][len]=0;  

			HSetState(hTemp,hState);
		}
		DisposeHandle(hTemp);


      p->result = 0;
      return(p->result);         //  return without call to API, since globals are already updated

	}
	else if((strcmp(New_Name, "SYSTEM")==0) || (strcmp(New_Name, "MODULE")==0)
					|| (strcmp(New_Name, "CHANNEL")==0))
	{
	
	   p->result = 0;
      return(p->result);         //  return without doing anything, Igor and C names are identical


      /* Download names of global variables applicable to all modules or individual modules */ 
		
		if(strcmp(New_Name, "SYSTEM")==0)
		{
			Num_Par = N_SYSTEM_PAR;
		}
		else if(strcmp(New_Name, "MODULE")==0)
		{
			Num_Par = N_MODULE_PAR;
		}
		else if (strcmp(New_Name, "CHANNEL")==0)
		{
			Num_Par = N_CHANNEL_PAR;
		}

		for(k = 0; k < Num_Par; k ++ )
		{
			indices[0] = k;
			MDGetTextWavePointValue(p->Names, indices, hTemp);
			hState=MoveLockHandle(hTemp);
			len = MIN(GetHandleSize(hTemp), MAX_PAR_NAME_LENGTH);

			if((New_Names[k] = (S8 *)malloc(len+1)) == NULL)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie4_Hand_Down_Names): cannot allocate memory for parameter names");
				Pixie_Print_MSG(ErrMSG,1);
				p->result = -3;
				return(p->result);
			}

			strncpy(New_Names[k], *hTemp, len);
			New_Names[k][len]=0;

			HSetState(hTemp,hState);
		}
		DisposeHandle(hTemp);
	}
	else
	{
		/* Invalid names */
		sprintf(ErrMSG, "*ERROR* (Pixie4_Hand_Down_Names): invalid name %s", New_Name);
		Pixie_Print_MSG(ErrMSG,1);
		p->result = -1;
		return(p->result);
	}

	p->result = Pixie_Hand_Down_Names(New_Names, New_Name);

	/* Free allocated memory */
	free(New_Names);

	return(p->result);
}


/****************************************************************
 *	Pixie4_Boot_System function:
 *		Boot all the Pixie modules in the system.
 *
 *		Boot_Pattern is a bit mask:
 *			bit 0:	Boot communication FPGA
 *			bit 1:	Boot FIPPI
 *			bit 2:	Boot DSP
 *			bit 3:	Load DSP parameters
 *			bit 4:	Apply DSP parameters (calls Set_DACs and
 *					Program_FIPPI)
 *
 *		Return Value:
 *			result of Pixie_Boot_System
 *
 ****************************************************************/

S32 Pixie4_Boot_System (
		struct {
			double Boot_Pattern;	// Pixie-4 boot patten
			double result;			// return value
		}* p )
{

	p->result = Pixie_Boot_System((U16)p->Boot_Pattern);
	return(p->result);
}


/****************************************************************
 *	Pixie4_User_Par_IO function:
 *		Download or upload User Values from or to the host.
 *
 *		User_Par_Values is a double precision wave containing
 *		the User Values.
 *
 *		User_Par_Name is a string variable.
 *
 *		direction:
 *			0:	download from the host to this library.
 *			1:	upload from this library to the host.
 *
 *		Return Value:
 *			result of Pixie_User_Par_IO
 *
 ****************************************************************/


S32 Pixie4_User_Par_IO (
		struct {
			double ChanNum;					// channel number of the Pixie module
			double ModNum;					// number of the module to work on
			double direction;				// transfer direction (read or write)
			Handle User_Par_Type;			// user parameter type 
			Handle User_Par_Name;			// user parameter name
			waveHndl User_Par_Values;		// user parameters to be transferred
			double result;					// return value
		}* p )
{

	Handle hName = NewHandle(0);
	S32 len, hState;
	S8  user_par_name[MAX_PAR_NAME_LENGTH];
	S8  user_par_type[MAX_PAR_NAME_LENGTH];

	/* make this point to the data in User_Par_Values */
	double *user_par_values = WaveData(p->User_Par_Values);

	/* Copy content of User_Par_Name to user_par_name */
	hName = p->User_Par_Name;
	hState=MoveLockHandle(hName);
	len=MIN(GetHandleSize(hName), MAX_PAR_NAME_LENGTH);
	strncpy(user_par_name, *hName, len);
	user_par_name[len]=0;
	HSetState(hName,hState);

	/* Copy content of User_Par_Type to user_par_type */
	hName = p->User_Par_Type;
	hState=MoveLockHandle(hName);
	len=MIN(GetHandleSize(hName), MAX_PAR_NAME_LENGTH);
	strncpy(user_par_type, *hName, len);
	user_par_type[len]=0;
	HSetState(hName,hState);
	DisposeHandle(hName);
    
	p->result = Pixie_User_Par_IO(user_par_values, user_par_name, user_par_type,
								(U16)p->direction, (U8)p->ModNum, (U8)p->ChanNum);

	return(p->result);

}


/****************************************************************
 *	Pixie4_Acquire_Data function:
 *		Acquire data for MCA run or list mode run.
 *
 *		Run_Type is a IGOR variable.
 *		User_Data is a double precision wave.
 *		file_name is a string.
 *
 *		Return Value:
 *			result of Pixie_Acquire_Data
 *
 ****************************************************************/
 
 
#define MAX_THREADS 1
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void ErrorHandler(LPTSTR lpszFunction);

// Sample custom data structure for threads to use.
// This is passed by void pointer so it can be any data type
// that can be passed using a single void pointer (LPVOID).
typedef struct MyData {
    U16 result; 
	U16 Run_Type;
	U32 *UserData;
	U8  *filnam;
	U8  ModNum;
} MYDATA, *PMYDATA;
 
PMYDATA pDataArray[MAX_THREADS];
DWORD   dwThreadIdArray[MAX_THREADS];
HANDLE  hThreadArray[MAX_THREADS]; 

S32 Pixie4_Acquire_Data (
		struct {
			double   ModNum;		// Pixie module number
			Handle   file_name;		// file name
			waveHndl User_data;		// array holding transferred data
			double   Run_Type;		// data acquisition type
			double   result;		// return value
		}* p )
{

	Handle hName = NewHandle(0);
	S32 len, hState;
	S8 filnam[MAX_FILE_NAME_LENGTH];
	int i = 0;

	// make this point to the data array in User_data 
	U32 *Host_Data = WaveData(p->User_data);

	// Copy content of file_name to filnam 
	hName = p->file_name;
	hState=MoveLockHandle(hName);
	len=MIN(GetHandleSize(hName), MAX_FILE_NAME_LENGTH);
	strncpy(filnam, *hName, len);
	filnam[len]=0;
	HSetState(hName,hState);
	DisposeHandle(hName);	

	if(!MultiThreadDAQ) {
		// Single thread operation 
		p->result=Pixie_Acquire_Data((U16)p->Run_Type, Host_Data, filnam, (U8)p->ModNum);
	} else {
		// Multi thread operation for some run types
		switch ((U16)p->Run_Type) {
			case 0x1403:
			case 0x1400:
			case 0x1103:
			case 0x1102:
			case 0x1101:
			case 0x1100:
				sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread starting RunStart");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				MT_KeepPolling = 1;
				//XOPNotice(ErrMSG);
				/* Use threads for 0x1400 */
				/* Create MAX_THREADS worker threads */
				for(i=0; i<MAX_THREADS; i++)
				{
					// Allocate memory for thread data.
					pDataArray[i] = (PMYDATA) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MYDATA));
					if( pDataArray[i] == NULL )
					{
						// If the array allocation fails, the system is out of memory
						// so there is no point in trying to print an error message.
						// Just terminate execution.
						ExitProcess(2);
					}
					// Generate unique data for each thread to work with. 
					pDataArray[i]->Run_Type = (U16)p->Run_Type;
					pDataArray[i]->UserData = Host_Data;
					pDataArray[i]->filnam = filnam;
					pDataArray[i]->ModNum = (U8)p->ModNum;
					// Create the thread to begin execution on its own.
					hThreadArray[i] = CreateThread( 
							NULL,                   // default security attributes
							0,                      // use default stack size  
							MyThreadFunction,       // thread function name
							pDataArray[i],          // argument to thread function 
							0,                      // use default creation flags 
							&dwThreadIdArray[i]);   // returns the thread identifier 
					// Check the return value for success.
					// If CreateThread fails, terminate execution. 
					// This will automatically clean up threads and memory.
					if (hThreadArray[i] == NULL) 
					{
						ErrorHandler(TEXT("CreateThread"));
						ExitProcess(3);
					}
					// NB: assuming that there is only one thread (for polling), so dwThreadIdArray[0] is used to 
					// semafore the messages into msgBuffer!
					pollingThreadId = dwThreadIdArray[0];
				} // End of main thread creation loop.
				sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread done RunStart");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				//XOPNotice(ErrMSG);
				break;
			case 0x3403:
			case 0x3400:
			case 0x3103:
			case 0x3102:
			case 0x3101:
			case 0x3100:
				// THIS CODE SHOULD BE called at the run end 

				sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread starting RunStop");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				//XOPNotice(ErrMSG);

			 
				MT_KeepPolling = 0; //stop the infinite polling loop

				// P4e: stop run, then wait for thread to finish up (to stop data creation and continue readout)
		//		if( ((U16)p->Run_Type == 0x3403) || ((U16)p->Run_Type == 0x3400) ) {
		//			p->result=Pixie_Acquire_Data((U16)p->Run_Type, Host_Data, filnam, (U8)p->ModNum);
		//			Pixie_Sleep(20);
		//		}

				// Wait until all threads have terminated.
				if (WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, 5000) != 0) {
					sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread termination timeout");
					Pixie_Print_MSG(ErrMSG,1);
					//XOPNotice(ErrMSG);
				};

				sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread done waiting for end, cleaning up");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				//XOPNotice(ErrMSG);
				// Close all thread handles and free memory allocations.
				for(i=0; i<MAX_THREADS; i++)
				{
					TerminateThread(hThreadArray[i], hState);
					CloseHandle(hThreadArray[i]);
					if(pDataArray[i] != NULL)
					{
						HeapFree(GetProcessHeap(), 0, pDataArray[i]);
						pDataArray[i] = NULL;    // Ensure address is not reused.
					}
					pollingThreadId = 0;
				}

				// P4: stop thread first (above), then stop run in module (data creation stops without readout )
		//		if( ((U16)p->Run_Type != 0x3403) && ((U16)p->Run_Type != 0x3400) ) {
					p->result=Pixie_Acquire_Data((U16)p->Run_Type, Host_Data, filnam, (U8)p->ModNum);
					Pixie_Sleep(20);
		//		}

				//p->result=Pixie_Acquire_Data((U16)p->Run_Type, Host_Data, filnam, (U8)p->ModNum);
				sprintf(ErrMSG, "*DEBUG* (Pixie4_Acquire_Data [MT ctrl]): thread done RunStop");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				//XOPNotice(ErrMSG);
				break;
			default:
				// Single thread operation for other codes
				p->result=Pixie_Acquire_Data((U16)p->Run_Type, Host_Data, filnam, (U8)p->ModNum);
				break;
		}	// end switch
	}	// end if multithread
	return (0);
}


DWORD WINAPI MyThreadFunction( LPVOID lpParam ) 
{ 
    HANDLE hStdout;
    PMYDATA pDataArray;

    TCHAR msgBuf[BUF_SIZE];
    size_t cchStringSize;
    DWORD dwChars;

    // Make sure there is a console to receive output results. 

    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if( hStdout == INVALID_HANDLE_VALUE )
        return 1;

    // Cast the parameter to the correct data type.
    // The pointer is known to be valid because 
    // it was checked for NULL before the thread was created.
 
    pDataArray = (PMYDATA)lpParam;
	
	/* Execute Pixie_Acquire_Data with the arguments */
	
	pDataArray->result=Pixie_Acquire_Data(pDataArray->Run_Type, 
										  pDataArray->UserData, 
										  pDataArray->filnam, 
										  pDataArray->ModNum);

    // Print the parameter values using thread-safe functions.

    StringCchPrintf(msgBuf, BUF_SIZE, TEXT("Parameters = %d, %d\n"), 
        pDataArray->Run_Type, pDataArray->ModNum); 
    StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
    WriteConsole(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);

    return 0; 
} 


void ErrorHandler(LPTSTR lpszFunction) 
{ 
    // Retrieve the system error message for the last-error code.

    LPVOID lpMsgBuf;
    LPVOID lpDisplayBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message.

    lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
        (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) * sizeof(TCHAR)); 
    StringCchPrintf((LPTSTR)lpDisplayBuf, 
        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
        TEXT("%s failed with error %d: %s"), 
        lpszFunction, dw, lpMsgBuf); 
    MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK); 

    // Free error-handling buffer allocations.

    LocalFree(lpMsgBuf);
    LocalFree(lpDisplayBuf);
}









/****************************************************************
 *	Pixie4_Set_Current_ModChan function:
 *		Set the current Module number and Channel number.
 *
 *		Return Value:
 *			result of Pixie_Set_Current_ModChan
 *
 ****************************************************************/

S32 Pixie4_Set_Current_ModChan (
		struct {
			double Module;
			double Channel;
			double result;
	}* p )
{
	p->result=Pixie_Set_Current_ModChan((U8)p->Module, (U8)p->Channel);
	return (p->result);
}


/****************************************************************
 *	Pixie4_Buffer_IO function:
 *		Download or upload I/O parameter values from or to the
 *		host.
 *
 *		Direction:IGOR variable;	0 (write), 1 (read)
 *		Type:     IGOR variable;	0 (DSP I/O parameters)
 *									1 (All DSP variables)
 *									2 (Settings file I/O)
 *									3 (Copy or extract Settings)
 *		Values:   IGOR unsigned short wave
 *
 *		Return Value:
 *			result of Pixie_Buffer_IO
 *
 ****************************************************************/

S32 Pixie4_Buffer_IO (
		struct {
			double ModNum;			// number of the module to work on 
			Handle file_name;		// settings file name
			double direction;		// I/O direction
			double type;			// I/O type
			waveHndl Values;		// parameter values
			double result;			// return value
		}* p )
{

	Handle hName = NewHandle(0);
	S32 len, hState;
	S8  filnam[MAX_FILE_NAME_LENGTH];

	/* Make this point to the data array in Values */
	U16 *pData = WaveData(p->Values);

	/* Copy content of file_name to filnam */
	hName = p->file_name;
	hState=MoveLockHandle(hName);
	len=MIN(GetHandleSize(hName), MAX_FILE_NAME_LENGTH);
	strncpy(filnam, *hName, len);
	filnam[len]=0;
	HSetState(hName,hState);
	DisposeHandle(hName);

	p->result=Pixie_Buffer_IO(pData, (U8)p->type, (U8)p->direction, filnam, (U8)p->ModNum);
	return (p->result);
}
