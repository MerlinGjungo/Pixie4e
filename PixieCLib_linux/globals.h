
#ifndef __GLOBALS_H
#define __GLOBALS_H

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
 * File Name:
 *
 *     Globals.h
 *
 * Description:
 *
 *     Definitions of global variables and data structures
 *
 * Revision:
 *
 *     12-1-2004
 *
 ******************************************************************************/


/* If this is compiled by a C++ compiler, make it */
/* clear that these are C routines.               */
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __DEFS_H
#include "defs.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "PlxTypes.h"   
#include "PciTypes.h"
#include "Plx.h"
#include "PexApi.h"

#include "boot.h"
#include "sharedfiles.h"
#include "utilities.h"

#ifdef WINDRIVER_API
/*
#include "wdc_defs.h"
#include "wdc_lib.h"
#include "utils.h"
#include "status_strings.h"
*/
#include "pixie500e_lib.h"
#endif

/***********************************************************
*		Global variables
***********************************************************/

extern U32 VAddr[PRESET_MAX_MODULES];						// Virtual addresses
extern U16 Phy_Slot_Wave[PRESET_MAX_MODULES];				// The slot number of the PXI crate
extern U8 Sys_Slot_Wave[PRESET_MAX_MODULES];				// The PCI device number
extern PLX_DEVICE_OBJECT Sys_hDevice[PRESET_MAX_MODULES];	// PCI device handle
extern U8 Number_Modules;									// Total number of modules in the crate
extern U8 Offline;											// Indicator of offline analysis
extern U8 Max_Number_of_Modules;							// The maximum possible number of modules in the crate (acts as crate type ID)
extern U8 Chosen_Module;									// Current module
extern U8 Chosen_Chan;										// Current channel
extern U8 Bus_Number[PRESET_MAX_MODULES];					// PCI bus number
extern U16 Random_Set[IO_BUFFER_LENGTH];					// Random indices used by TauFinder and BLCut
extern double SGA_Computed_Gain[N_SGA_GAIN];				// SGA computed gain
extern double Filter_Int[PRESET_MAX_MODULES];				// Energy filter interval (depends on Clock speed and filter range)
extern U16 Writing_IOBuffer_Address;						// The start address of I/O buffer for writing
extern U16 Writing_IOBuffer_Length;							// The number of words to write into the I/O buffer
extern double One_Cycle_Time;								// Number of ns for each wait cycle
extern S8  ErrMSG[256];										// A string for error messages
extern S8  next_base_name[256];								// A string for future file name (without suffixes) for a multi-file run
extern U16 MakeNewFile;									// if 1, Write_DMA_List_Mode_File switches to new files when done
extern U32 MODULE_EVENTS[2*PRESET_MAX_MODULES];				// Internal copy of ModuleEvents array modified by task 0x7001 and used by other 0x7000 tasks
extern U32 P500E_DSP_CODE_BYTES;							// Size of P500e DSP code to be determined dynamically
extern U32 PCIBusType;										// Type of PCI bus: express or regular

extern U32 Polling;											// if 1, use (old style) polling of module in LM runs, if 0 use intertupts from module  
extern U32 PrintDebugMsg_Boot;								// if 1, print debug messages during booting
extern U32 PrintDebugMsg_QCerror;							// if 1, print error debug messages during LM buffer quality check
extern U32 PrintDebugMsg_QCdetail;							// if 1, print detail debug messages during LM buffer quality check
extern U32 PrintDebugMsg_other;								// if 1, print other debug messages 
extern U32 BufferQC;										// if 1, execute data quality control check before writing LM data to file
extern U32 PollForNewData;									// if 1, return new data in DMA buffer during polling
extern U32 MultiThreadDAQ;									// if 1, run 0x400 or 0x10x as a separate thread
extern U8 AutoProcessLMData;								// To control if the LM parse routine processes compressed LM data
extern U8 KeepCW;											// To control update and enforced minimum of coincidence wait

#ifdef WINDRIVER_API
extern WDC_DEVICE_HANDLE hDev[PRESET_MAX_MODULES];			// WinDriver device handle
extern WD_DMA *pDmaTrace;
extern WD_DMA *pDmaList[PRESET_MAX_MODULES];
extern U32 *LMBuffer[PRESET_MAX_MODULES];
extern  FILE *listFile[PRESET_MAX_MODULES];
extern U32 *LMBufferCopy[PRESET_MAX_MODULES];
extern U32 LMBufferCounter[PRESET_MAX_MODULES];
//extern U32 EndRunFound[PRESET_MAX_MODULES];  // EOR block found in data strea,
extern U32 dt3EventCounter[PRESET_MAX_MODULES];
extern U32 numDWordsLeftover[PRESET_MAX_MODULES];								// carryover from LM buffer to next
extern U16 EventLengthTotal[PRESET_MAX_MODULES];
extern U16 EventLength[PRESET_MAX_MODULES][NUMBER_OF_CHANNELS];
extern U16 traceBlocksPrev_QC[PRESET_MAX_MODULES];
extern U32 RunStartTicks;
extern U32 RunStopTicks;
extern S8 msgBuffer[65536];									//  message buffer for info from the polling thread
extern U32 DMADataPos;									// position from which to read new data in DMA buffer
extern S32 EndRunFound[PRESET_MAX_MODULES];	

#ifdef COMPILE_IGOR_XOP
extern DWORD pollingThreadId;
#endif
#endif
DWORD gMainThreadId;
U32 MT_KeepPolling;
//***********************************************************
//		Frequently used indices
//***********************************************************

extern U16 Run_Task_Index;							// RUNTASK
extern U16 Control_Task_Index;						// CONTROLTASK
extern U16 Resume_Index;							// RESUME
extern U16 SYNCHDONE_Index;							// SYNCHDONE
extern U16 FASTPEAKS_Index[NUMBER_OF_CHANNELS];		// FASTPEAKS
extern U16 COUNTTIME_Index[NUMBER_OF_CHANNELS];		// COUNTTIME
extern U16 RUNTIME_Index;							// RUNTIME
extern U16 NUMEVENTS_Index;							// NUMEVENTS
extern U16 BoardVersion_Index;					// BoardVersion




//***********************************************************
//		Common configuration files and parameter names
//		used by all Pixie modules
//***********************************************************

/* Configuration file names */
extern S8 Boot_File_Name_List[N_BOOT_FILES][MAX_FILE_NAME_LENGTH];

/* Communication FPGA configuration (P4 Rev. B) */
extern U8 ComFPGA_Configuration_P4_RevB[N_COMFPGA_BYTES];

/* Communication FPGA configuration (P4 Rev. C-E) */
extern U8 ComFPGA_Configuration_P4_RevC[N_COMFPGA_BYTES];

/* Communication FPGA configuration (P500 Rev. B) */
extern U8 ComFPGA_Configuration_P500_RevB[N_COMFPGA_BYTES];

/* DSP executable code P4*/
extern U16 DSP_Code_P4[N_DSP_CODE_BYTES];

/* DSP executable code P500*/
extern U16 DSP_Code_P500[N_DSP_CODE_BYTES];

/* P500e DSP executable code */
extern U8 P500e_DSP_Code[N_DSP_CODE_BYTES];

/* FIPPI configuration */
extern U8 FIPPI_Configuration[N_FIPPI_BYTES];

/* P4e FIPPI configuration */
extern U8 P4e_FPGA_Configuration[N_P4E_BYTES];
extern U8 P4e14500_FPGA_Configuration[N_P4E_BYTES];

/* P500e FIPPI configuration */
extern U8 P500e_FPGA_Configuration[N_P500E_BYTES];

/* DSP I/O parameter names */					
extern S8 DSP_Parameter_Names[N_DSP_PAR][MAX_PAR_NAME_LENGTH];
// defined by .var file

/* DSP internal parameter names; not being used in this driver now */
//extern U8 DSP_Memory_Names[N_MEM_PAR][MAX_PAR_NAME_LENGTH];

/* Parameter names applicable to a Pixie channel */
extern S8 Channel_Parameter_Names[N_CHANNEL_PAR][MAX_PAR_NAME_LENGTH];
// defined by Igor or globals.c

/* Parameter names applicable to a Pixie module */
extern S8 Module_Parameter_Names[N_MODULE_PAR][MAX_PAR_NAME_LENGTH];
// defined by Igor or globals.c

/* Parameter names applicable to a whole Pixie system */
extern S8 System_Parameter_Names[N_SYSTEM_PAR][MAX_PAR_NAME_LENGTH];
// defined by Igor or globals.c

/* Parameter values applicable to a whole Pixie system */
extern double System_Parameter_Values[N_SYSTEM_PAR];

//***********************************************************
//		Pixie global data structure
//***********************************************************

struct Pixie_Configuration
{
	/* Parameter values applicable to a Pixie channel */
	double Channel_Parameter_Values[NUMBER_OF_CHANNELS][N_CHANNEL_PAR];
	
	/* DSP I/O parameter values */
	U16 DSP_Parameter_Values[N_DSP_PAR];
	
	/* DSP internal parameter values; not being used in this driver now */
	//U16 DSP_Memory_Values[N_MEM_PAR];

	/* Parameter values applicable to a whole Pixie system */
	double Module_Parameter_Values[N_MODULE_PAR];
};

/* Define PRESET_MAX_MODULES Pixie devices */
extern struct Pixie_Configuration Pixie_Devices[PRESET_MAX_MODULES];
extern U32 gThr_lo, gThr_hi;
#ifdef __cplusplus
}
#endif	/* End of notice for C++ compilers */

#endif	/* End of globals.h */
