/*----------------------------------------------------------------------
 * Copyright (c) 2004, 2009, 2015 XIA LLC
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
 *
 ******************************************************************************/

#include "globals.h"

/***********************************************************
*		Global variables
***********************************************************/

//PLX_UINT_PTR	VAddr[PRESET_MAX_MODULES];			// Virtual addresses	
U32	VAddr[PRESET_MAX_MODULES];			// Virtual addresses	
U16 Phy_Slot_Wave[PRESET_MAX_MODULES];				// The slot number of the PXI crate
U8 Sys_Slot_Wave[PRESET_MAX_MODULES];				// The PCI device number
PLX_DEVICE_OBJECT Sys_hDevice[PRESET_MAX_MODULES];	// PCI device handle
U8 Number_Modules;									// Total number of modules in the crate
U8 Offline;											// Indicator of offline analysis
U8 Max_Number_of_Modules;							// The maximum possible number of modules in the crate (acts as crate type ID)
U8 Chosen_Module;									// Current module
U8 Chosen_Chan;										// Current channel
U8 Bus_Number[PRESET_MAX_MODULES];					// PCI bus number
U16 Random_Set[IO_BUFFER_LENGTH];					// Random indices used by TauFinder and BLCut
double SGA_Computed_Gain[N_SGA_GAIN];				// SGA computed gain
double Filter_Int[PRESET_MAX_MODULES];				// Energy filter interval (depends on Clock speed and filter range)
U16 Writing_IOBuffer_Address;						// The start address of I/O buffer for writing
U16 Writing_IOBuffer_Length;						// The number of words to write into the I/O buffer
double One_Cycle_Time;								// Number of ns for each wait cycle
S8  ErrMSG[256];									// A string for error messages
S8  next_base_name[256];							// A string for future file name (without suffixes) for a multi-file run
U16 MakeNewFile;									// if 1, Write_DMA_List_Mode_File switches to new files when done
U32 MODULE_EVENTS[2*PRESET_MAX_MODULES];			// Internal copy of ModuleEvents array modified by task 0x7001 and used by other 0x7000 tasks
U32 P500E_DSP_CODE_BYTES;							// Size of P500e DSP code in bytes to be determined dynamically
U32 PCIBusType;										// PCI bus either regular or express

// set via C_CONTROL variable in DSP settings
U32 Polling;										// if 1, use (old style) polling of module in LM runs, if 0 use intertupts from module  
U32 PrintDebugMsg_Boot;								// if 1, print debug messages during booting
U32 PrintDebugMsg_QCerror;							// if 1, print error debug messages during LM buffer quality check
U32 PrintDebugMsg_QCdetail;							// if 1, print detail debug messages during LM buffer quality check
U32 PrintDebugMsg_other;							// if 1, print other debug messages
extern U32 PrintDebugMsg_daq;								// if 1, print debug messages for run start/stop    //by Hongyi Wu
extern U32 PrintDebugMsg_file;								// if 1, Igor also prints to a file    //by Hongyi Wu
U32 BufferQC;										// if 1, execute data quality control check before writing LM data to file
U32 PollForNewData;									// if 1, return new data in DMA buffer during polling
U32 MultiThreadDAQ;									// if 1, run 0x400 or 0x10x as a separate thread
U8 AutoProcessLMData;								// To control if the LM parse routine processes compressed LM data
U8 KeepCW;											// To control update and enforced minimum of coincidence wait
extern U8 KeepBL;											// if 1, do not automatically adjust BLcut after gain or filter settings changes  //by Hongyi Wu


#ifdef WINDRIVER_API
WDC_DEVICE_HANDLE hDev[PRESET_MAX_MODULES]; // WinDriver device handle
WD_DMA *pDmaTrace;
WD_DMA *pDmaList[PRESET_MAX_MODULES];
#endif
U32 *LMBuffer[PRESET_MAX_MODULES]; // DMA framebuffer
FILE *listFile[PRESET_MAX_MODULES]; // list-mode run files
U32 *LMBufferCopy[PRESET_MAX_MODULES]; // Copying DMA data for disk I/O
U32 LMBufferCounter[PRESET_MAX_MODULES]; // framebuffer counter for all modules
//U32 EndRunFound[PRESET_MAX_MODULES];  // EOR block found in data strea,
U32 dt3EventCounter[PRESET_MAX_MODULES];
U32 numDWordsLeftover[PRESET_MAX_MODULES];			// carryover from LM buffer to next
U16 EventLengthTotal[PRESET_MAX_MODULES]; // TODO: this has to be global? Need one for each module?
U16 EventLength[PRESET_MAX_MODULES][NUMBER_OF_CHANNELS];// TODO: this has to be global? Need one for each module?
U16 traceBlocksPrev_QC[PRESET_MAX_MODULES];
U32 RunStartTicks;
U32 RunStopTicks;
S8 msgBuffer[65536]; // message buffer for info from the polling thread
U32 DMADataPos;		 // position from which to read new data in DMA buffer
S32 EndRunFound[PRESET_MAX_MODULES];	//

#ifdef COMPILE_IGOR_XOP  // Printing from polling thread
DWORD pollingThreadId;
#endif
DWORD gMainThreadId;
U32 MT_KeepPolling;
//***********************************************************
//		Frequently used indices
//***********************************************************

U16 Run_Task_Index;							// RUNTASK
U16 Control_Task_Index;						// CONTROLTASK
U16 Resume_Index;							// RESUME
U16 SYNCHDONE_Index;						// SYNCHDONE
U16 FASTPEAKS_Index[NUMBER_OF_CHANNELS];	// FASTPEAKS
U16 COUNTTIME_Index[NUMBER_OF_CHANNELS];		// COUNTTIME
U16 RUNTIME_Index;							// RUNTIME
U16 NUMEVENTS_Index;						// NUMEVENTS
U16 BoardVersion_Index;					// BoardVersion


//***********************************************************
//		Common configuration files and parameter names
//		used by all Pixie modules
//***********************************************************

/* Configuration file names */
S8 Boot_File_Name_List[N_BOOT_FILES][MAX_FILE_NAME_LENGTH];

/* Communication FPGA configuration (P4 Rev. B) */
U8 ComFPGA_Configuration_P4_RevB[N_COMFPGA_BYTES];

/* Communication FPGA configuration (P4 Rev. C) */
U8 ComFPGA_Configuration_P4_RevC[N_COMFPGA_BYTES];

/* Communication FPGA configuration (P500 Rev. B) */
U8 ComFPGA_Configuration_P500_RevB[N_COMFPGA_BYTES];

/* P500e DSP executable code */
U8 P500e_DSP_Code[N_DSP_CODE_BYTES];

/* DSP executable code P4 */
U16 DSP_Code_P4[N_DSP_CODE_BYTES];

/* DSP executable code P500 */
U16 DSP_Code_P500[N_DSP_CODE_BYTES];

/* FIPPI configuration */
U8 FIPPI_Configuration[N_FIPPI_BYTES];

/* P4e FIPPI configuration */
U8 P4e_FPGA_Configuration[N_P4E_BYTES];
U8 P4e14500_FPGA_Configuration[N_P4E_BYTES];

/* P500e FIPPI configuration */
U8 P500e_FPGA_Configuration[N_P500E_BYTES];

/* DSP I/O parameter names */					
S8 DSP_Parameter_Names[N_DSP_PAR][MAX_PAR_NAME_LENGTH];
// defined by .var file

/* DSP internal parameter names; not being used in this driver now */
//U8 DSP_Memory_Names[N_MEM_PAR][MAX_PAR_NAME_LENGTH];

/* Parameter names applicable to a Pixie channel */
S8 Channel_Parameter_Names[N_CHANNEL_PAR][MAX_PAR_NAME_LENGTH] = {
	"CHANNEL_CSRA", 
	"CHANNEL_CSRB", 
	"ENERGY_RISETIME", 
	"ENERGY_FLATTOP", 
	"TRIGGER_RISETIME", 
	"TRIGGER_FLATTOP", 
	"TRIGGER_THRESHOLD", 
	"VGAIN", 
	"VOFFSET", 
	"TRACE_LENGTH", 
	"TRACE_DELAY", 
	"PSA_START", 
	"PSA_END", 
	"", 
	"BINFACTOR", 
	"TAU", 
	"BLCUT", 
	"XDT", 
	"BASELINE_PERCENT", 
	"CFD_THRESHOLD", 
	"INTEGRATOR", 
	"CHANNEL_CSRC", 
	"GATE_WINDOW", 
	"GATE_DELAY", 
	"COINC_DELAY", 
	"BLAVG", 
	"COUNT_TIME", 
	"INPUT_COUNT_RATE", 
	"FAST_PEAKS", 
	"OUTPUT_COUNT_RATE", 
	"NOUT", 
	"GATE_RATE", 
	"GATE_COUNTS", 
	"FTDT", 
	"SFDT", 
	"GDT",
	"CURRENT_ICR",
	"CURRENT_OORF",
	"PSM_GAIN_AVG",
	"PSM_GAIN_AVG_LEN",
	"PSM_TEMP_AVG",
	"PSM_TEMP_AVG_LEN",
	"PSM_GAIN_CORR",
	"QDC0_LENGTH",
	"QDC1_LENGTH",
	"QDC0_DELAY",
	"QDC1_DELAY",
	"NPPI",
	"PASS_PILEUP_RATE",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};
// Igor uses local definition!

/* Parameter names applicable to a Pixie module */
S8 Module_Parameter_Names[N_MODULE_PAR][MAX_PAR_NAME_LENGTH] = {

	"MODULE_NUMBER", 
	"MODULE_CSRA", 
	"MODULE_CSRB", 
	"C_CONTROL", 
	"MAX_EVENTS", 
	"COINCIDENCE_PATTERN", 
	"ACTUAL_COINCIDENCE_WAIT", 
	"MIN_COINCIDENCE_WAIT", 
	"SYNCH_WAIT", 
	"IN_SYNCH", 
	"RUN_TYPE", 
	"FILTER_RANGE", 
	"MODULEPATTERN", 
	"NNSHAREPATTERN", 
	"DBLBUFCSR", 
	"MODULE_CSRC", 
	"BUFFER_HEAD_LENGTH", 
	"EVENT_HEAD_LENGTH", 
	"CHANNEL_HEAD_LENGTH", 
	"", 
	"NUMBER_EVENTS", 
	"RUN_TIME", 
	"EVENT_RATE", 
	"TOTAL_TIME", 
	"BOARD_VERSION", 
	"SERIAL_NUMBER", 
	"DSP_RELEASE", 
	"DSP_BUILD", 
	"FIPPI_ID", 
	"SYSTEM_ID",
	"XET_DELAY",
	"PDM_MASKA",
	"PDM_MASKB",
	"PDM_MASKC",
	"USER_IN","","","","","","","",		// reserve 16 values for USER_IN
	"","","","","","","","",			//
	"USER_OUT","","","","","","","",	// reserve 16 values for USER_OUT
	"","","","","","","","",			//
	"ADC_BITS","ADC_RATE","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","",""
};
// Igor uses local definition!

/* Parameter names applicable to a whole Pixie system */
S8 System_Parameter_Names[N_SYSTEM_PAR][MAX_PAR_NAME_LENGTH] = {
	"NUMBER_MODULES",
	"OFFLINE_ANALYSIS",
	"AUTO_PROCESSLMDATA",
	"MAX_NUMBER_MODULES",
	"C_LIBRARY_RELEASE",
	"C_LIBRARY_BUILD",
	"KEEP_CW",
	"SLOT_WAVE",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","","",
	"","","","","","","",""
};
// Igor uses local definition!

/* Parameter values applicable to a whole Pixie system */
double System_Parameter_Values[N_SYSTEM_PAR];
struct Pixie_Configuration Pixie_Devices[PRESET_MAX_MODULES];
U32 gThr_lo, gThr_hi;


