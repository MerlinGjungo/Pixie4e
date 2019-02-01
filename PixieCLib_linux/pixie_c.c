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
 * File name:
 *
 *      pixie_c.c
 *
 * Description:
 *
 *      This file contains all the main driver funtions which
 *		can be used to control the Pixie modules.
 *
 * Revision:
 *
 *		3-18-2004
 *
 * Member functions:
 *
 *		Pixie_Hand_Down_Names
 *		Pixie_Boot_System
 *		Pixie_User_Par_IO
 *		Pixie_Acquire_Data
 *		Pixie_Set_Current_ModChan
 *		Pixie_Buffer_IO
 *
******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "PlxTypes.h"
#include "Plx.h"

#include "globals.h"
#include "sharedfiles.h"
#include "utilities.h"
#ifdef COMPILE_IGOR_XOP
#include <XOPStandardHeaders.h>
#endif

/****************************************************************
 *	Pixie_Hand_Down_Names function:
 *		Download file names or parameter names from the host
 *		to the driver.
 *
 *		Names is a two dimensional string array containing
 *		either the All_Files or parameter names.
 *
 *		All_Files is a string array with 6 entries which include
 *		the communication FPGA file, FiPPI file, DSP code bianry
 *		file(DSPcode.bin), DSP code I/O parameter names file (DSPcode.var),
 *		DSP code all parameter names file (.lst), and DSP I/O parameter
 *		values file (.itx).
 *
 *		Return Value:
 *			0 - download successful
 *		   -1 - invalid names
 *
 ****************************************************************/

S32 Pixie_Hand_Down_Names (S8 *Names[],	// a two dimensional string array
			   S8 *Name )		// a string indicating which type of names to be downloaded
{

	U32 len, k, n;
	S8  *pSource, *pDest;

	// Main thread ID (not to use msgBuffer for Pixie_Print_MSG()
// KS DEBUG
//	gMainThreadId = GetCurrentThreadId();

	sprintf(ErrMSG, "*DEBUG* Pixie_Hand_Down_Names: starting ...");
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

	if(strcmp(Name, "ALL_FILES")==0)
	{
		/* Download file names */
		for( k = 0; k < N_BOOT_FILES; k ++ )
		{
			if (Names[k] == NULL) {                 // in case there is a null element in the array
				sprintf(ErrMSG, "*ERROR* Pixie_Hand_Down_Names(ALL_FILES): null array element %d", k);
				Pixie_Print_MSG(ErrMSG,1);
				continue;
			}
			len = MIN(strlen(Names[k]), MAX_FILE_NAME_LENGTH);
			pSource = Names[k];
			pDest = Boot_File_Name_List[k];
			n = 0;
			while( n++ < len ) *pDest++ = *pSource++;			
			*pDest = 0;

		}
	}
	else if((strcmp(Name, "SYSTEM")==0) || (strcmp(Name, "MODULE")==0) || (strcmp(Name, "CHANNEL")==0))
	{
		// Download names of global variables applicable to all modules or individual modules 
		if(strcmp(Name, "SYSTEM")==0)
		{
			// Download names of global variables applicable to all modules 
			for( k = 0; k < N_SYSTEM_PAR; k ++ )
			{
				if (Names[k] == NULL) {                 // in case there is a null element in the array
					sprintf(ErrMSG, "*ERROR* Pixie_Hand_Down_Names(SYSTEM): null array element %d", k);
					Pixie_Print_MSG(ErrMSG,1);
					continue;
				}
				len = MIN(strlen(Names[k]), MAX_PAR_NAME_LENGTH);
				pSource = Names[k];
				pDest = System_Parameter_Names[k];
				n = 0;
				while( n++ < len ) *pDest++ = *pSource++;			
				*pDest = 0;
			}

		}
		if(strcmp(Name, "MODULE")==0)
		{
			// Download names of global variables applicable to individual modules 
			for( k = 0; k < N_MODULE_PAR; k ++ )
			{
				if (Names[k] == NULL) {                 // in case there is a null element in the array
					sprintf(ErrMSG, "*ERROR* Pixie_Hand_Down_Names(MODULE): null array element %d", k);
					Pixie_Print_MSG(ErrMSG,1);
					continue;
				}
				len = MIN(strlen(Names[k]), MAX_PAR_NAME_LENGTH);
				pSource = Names[k];
				pDest = Module_Parameter_Names[k];
				n = 0;
				while( n++ < len )
				{
					*pDest++ = *pSource++;
				}

				*pDest = 0;
			}
		}
		if(strcmp(Name, "CHANNEL")==0)
		{
			// Download user variable names applicable to individual channels 
			for( k = 0; k < N_CHANNEL_PAR; k ++ )
			{
				
				if (Names[k] == NULL) {                 // in case there is a null element in the array
					sprintf(ErrMSG, "*ERROR* Pixie_Hand_Down_Names(CHANNEL): null array element %d", k);
					Pixie_Print_MSG(ErrMSG,1);
					continue;
				}
				len = MIN(strlen(Names[k]), MAX_PAR_NAME_LENGTH);
				pSource = Names[k];
				pDest = Channel_Parameter_Names[k];
				n = 0;
				while( n++ < len )
				{
					*pDest++ = *pSource++;
				}
				*pDest = 0;
			}
		}
	}
	else
	{
		// Invalid names 
		sprintf(ErrMSG, "*ERROR* (Pixie_Hand_Down_Names): invalid name %s", Name);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	return (0);
}


/****************************************************************
 *	Pixie_Boot_System function:
 *		Boot all the Pixie modules in the system.
 *
 *		Boot_Pattern is a bit mask:
 *			bit 0:	Boot communication FPGA
 *			bit 1:	Boot FIPPI
 *			bit 2:	Boot DSP
 *			bit 3:	Load DSP parameters
 *			bit 4:	Apply DSP parameters (calls Set_DACs and Program_FIPPI)
 *			bit 5:	Close handle (release to other process). Exclusive with lower bits!
 *
 *		Return Value:
 *			 0 - boot successful
 *			-1 - unable to scan crate slots
 *			-2 - unable to read communication FPGA configuration (Rev. B)
 *			-3 - unable to read communication FPGA configuration (Rev. C)
 *			-4 - unable to read FiPPI configuration
 *			-5 - unable to read DSP executable code
 *			-6 - unable to read DSP parameter values
 *			-7 - unable to initialize DSP parameter names
 *			-8x - failed to boot all modules present in the system (see Pixie_Boot)
  *			-13 - failed to close handles for modules
 *
 ****************************************************************/

S32 Pixie_Boot_System ( U16 Boot_Pattern )	// bit mask
{
	S32 retval = 0;
	U32 m;

	
	// Main thread ID (not to use msgBuffer for Pixie_Print_MSG()
// KS DEBUG
//	gMainThreadId = GetCurrentThreadId();

	sprintf(ErrMSG, "*DEBUG* Pixie_Boot_System: starting ...");
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

#ifdef WINDRIVER_API
	if(Boot_Pattern == 0x20)
	{
		if(PCIBusType==EXPRESS_PCI) {
			for(m=0; m<Number_Modules; m++)
			{
				if (hDev[m]) {
					sprintf(ErrMSG, "*INFO* Pixie_Boot_System: Closing handle in module %d",m);
					Pixie_Print_MSG(ErrMSG,1);
					PIXIE500E_DeviceClose(hDev[m]); 
				} else 	{
					sprintf(ErrMSG, "*ERROR* Pixie_Boot_System: Found no handle to close");
					Pixie_Print_MSG(ErrMSG,1);
					return(CLOSE_HDLE_ERR);
				}		
			}	// end for
		} else {
			sprintf(ErrMSG, "*INFO* Pixie_Boot_System: Closing handle only applied to P4e and P500e");
			Pixie_Print_MSG(ErrMSG,1);
		}

		return(0);
	}
#endif


	/* Scan all crate slots and find the address for each slot where a PCI device is installed */
	retval=Pixie_Scan_Crate_Slots(Number_Modules, &Phy_Slot_Wave[0]);
	if(retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Scanning crate slots unsuccessful, error %d.", retval);
		Pixie_Print_MSG(ErrMSG,1);
		return(SCAN_CRATE_ERR);
	}
	switch (PCIBusType) {
		case REGULAR_PCI:
			// Read communication FPGA configuration (P4 Rev. C-E) 
			retval=Load_U16(Boot_File_Name_List[1], COM_FPGA_CONFIG_REV_C, (N_COMFPGA_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read communication FPGA configuration (P4 Rev. C).");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_SYS_C_ERR);
			}
	/*		// Read communication FPGA configuration (P500 Rev. B) 
			retval=Load_U16(Boot_File_Name_List[7], COM_FPGA_CONFIG_P500_REV_B, (N_COMFPGA_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read communication FPGA configuration (P500).");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_SYS_B_ERR);
			} */
			// Read P4 FiPPI configuration 
			retval=Load_U16(Boot_File_Name_List[2], FIPPI_CONFIG, (N_FIPPI_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read FiPPI configuration.");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_FIP_ERR);
			}
			// Read DSP executable code P4 
			retval=Load_U16(Boot_File_Name_List[3], DSP_CODE, (N_DSP_CODE_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read DSP executable code.");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_DSP_EXE_ERR);
			}
	/*		// Read DSP executable code P500
			retval=Load_U16(Boot_File_Name_List[10], P500_DSP_CODE, (N_DSP_CODE_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read DSP executable code.");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_DSP_EXE_ERR);
			} */
			break;
		case EXPRESS_PCI:
			/* Read P4e FPGA configuration (general)   */
			retval=Load_U16(Boot_File_Name_List[0], P4E_FPGA_CONFIG, (N_P4E_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read FiPPI configuration (P4e).");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_FIP_ERR);
			}
			/* Read P4e FPGA configuration (14/500)  */
			retval=Load_U16(Boot_File_Name_List[8], P4E14500_FPGA_CONFIG, (N_P4E_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read FiPPI configuration (P4e 14/500).");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_FIP_ERR);
			}
			/* Read P500e FPGA configuration */
			retval=Load_U16(Boot_File_Name_List[9], P500E_FPGA_CONFIG, (N_P500E_BYTES/4));
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read FiPPI configuration (P500e).");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_FIP_ERR);
			}
			/* Read P500e DSP executable code */
			retval=Load_U16(Boot_File_Name_List[11], P500E_DSP_CODE, 0); /* Code size is determined dynamically */
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read DSP executable code.");
				Pixie_Print_MSG(ErrMSG,1);
				return(RD_DSP_EXE_ERR);
			}
			break;
		default:
			break;
	}

	/* Read DSP parameter values */
	retval=Load_U16(Boot_File_Name_List[4], DSP_PARA_VAL, N_DSP_PAR);
	if(retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to read DSP parameter values.");
		Pixie_Print_MSG(ErrMSG,1);
		return(RD_DSP_PAR_ERR);
	}
// KS DEBUG
sprintf(ErrMSG,"*DEBUG* read DSP parameter values done from %s", Boot_File_Name_List[4]);
Pixie_Print_MSG(ErrMSG,1);

	/* Initialize DSP variable names */
	retval=Pixie_Init_VarNames();
	if(retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_System): Unable to initialize DSP parameter names.");
		Pixie_Print_MSG(ErrMSG,1);
		return(INIT_DSP_PAR_ERR);
	}
// KS DEBUG
sprintf(ErrMSG,"*DEBUG* init DSP variable names done");
Pixie_Print_MSG(ErrMSG,1);


	/* Initialize several global variables */
	Pixie_Init_Globals();
// KS DEBUG
sprintf(ErrMSG,"*DEBUG* Init_Globals  done");
Pixie_Print_MSG(ErrMSG,1);



	/* Boot all modules present in the system */
	if (Offline == 1) {
		sprintf(ErrMSG, "System started up successfully in offline mode");
		Pixie_Print_MSG(ErrMSG,1);
	}
	else {
		retval = Pixie_Boot(Boot_Pattern);
		if( retval < 0 )
		{
			return(retval);
		}
	}

	return(0);
}


/****************************************************************
 *	Pixie_User_Par_IO function
 *		Download or upload User Values from or to the host.
 *
 *		User_Par_Values is a double precision array containing
 *		the User Values.
 *
 *		User_Par_Name is a string variable which indicates which user
 *		parameter should be downloaded or uploaded.
 *
 *		User_Par_Type is a string variable specifying the type of
 *		user parameters (System, Module or Channel).
 *
 *		direction:
 *			0:	download from the host to this library.
 *			1:	upload from this library to the host.
 *
 *		Return Value:
 *			 0 - download successful
 *			-1 - null pointer for user parameter values
 *			-2 - invalid user parameter name
 *			-3 - invalid user parameter type
 *			-4 - invalid I/O direction
 *			-5 - invalid Pixie module number
 *			-6 - invalid Pixie channel number
 *
 ****************************************************************/

S32 Pixie_User_Par_IO (
			double *User_Par_Values,	// user parameters to be transferred
			S8 *User_Par_Name,			// user parameter name
			S8 *User_Par_Type,			// user parameter type
			U16 direction,				// transfer direction (read or write)
			U8  ModNum,					// number of the module to work on
			U8  ChanNum )				// channel number of the Pixie module
{
	S32 retval;

	/* Check validity of input parameters */
	if(User_Par_Values == NULL)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): NULL User_Par_Values pointer");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	if(direction > 1)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): invalid I/O direction, direction=%d", direction);
		Pixie_Print_MSG(ErrMSG,1);
		return(-4);
	}

	if((strcmp(User_Par_Type, "SYSTEM") != 0) && (ModNum >= Number_Modules))
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): invalid Pixie module number, ModNum=%d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-5);
	}
	if(ChanNum >= NUMBER_OF_CHANNELS)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): invalid Pixie channel number, ChanNum=%d", ChanNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-6);
	}

	/* Start I/O transfer */
	if((strcmp(User_Par_Type, "SYSTEM") == 0) || (strcmp(User_Par_Type, "MODULE") == 0) || (strcmp(User_Par_Type, "CHANNEL") == 0))
	{

		retval = UA_PAR_IO(User_Par_Values, User_Par_Name, User_Par_Type, direction, ModNum, ChanNum);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): invalid user parameter name, name=%s", User_Par_Name);
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);
		}
	}
	else
	{
		/* Invalid user parameter types */
		sprintf(ErrMSG, "*ERROR* (Pixie_User_Par_IO): invalid user parameter type, type=%s", User_Par_Type);
		Pixie_Print_MSG(ErrMSG,1);
		return(-3);
	}

	return(0);
}


/****************************************************************
 *	Pixie_Acquire_Data function:
 *		This is the main function used for data acquisition in MCA run
 *		or list mode run mode. Additionally, it can be used to parse list
 *		mode data files offline.
 *
 *		Run_Type is a 16-bit long word whose lower 12-bit
 *		specifies either data_run_type or control_task_run_type
 *		and upper 4-bit specifies actions(start\stop\poll)
 *
 *			Lower 12-bit:
 *				0x100,0x101,0x102,0x103 list mode runs
 *				0x200,0x201,0x202,0x203	fast list mode runs. discontinued in V2, but keep here for backwards compatibility (treat as 0x10?)
 *				0x301					MCA run
 *				0x1 -> 0x15				control task runs
 *
 *			Upper 4-bit:
 *				0x0000					start a control task run
 *				0x1000					start a new data run
 *				0x2000					resume a data run
 *				0x3000					stop a data run
 *				0x4000					poll run status
 *				0x5000					read histogram data and save it to a file
 *				0x6000					read list mode buffer data and save it to a file
 *				0x7000					offline list mode data parse routines
 *					0x7001					parse list mode data file
 *					0x7002					locate traces
 *					0x7003					read traces
 *					0x7004					read energies
 *					0x7005					read PSA values
 *					0x7006					read extended PSA values
 *					0x7007					locate events
 *					0x7008					read events
 *					0x7010					call custom process function
 *					0x7020					error check and save in new file (.b##)
 *					0x7021					error check and save in new file (.bin)
 *				0x8000					manually read spectrum from a previously saved MCA file
 *				0x9000					external memory (EM) I/O
 *					0x9001					read histogram memory section of EM 
 *					0x9002					write to histogram memory section of EM
 *					0x9003					read list mode memory section of EM
 *					0x9004					write to list mode memory section of EM
 *				0xA000 					special tasks
 *					0xA001					read data, then resume
 *
 *		User_data receives either the histogram, list mode data
 *		or the ADC trace.
 *
 *		filname needs to have complete path.
 *
 *		Return Value:
 *
 *			Run type 0x0000
 *				 0x0  - success
 *				-0x1  - invalid Pixie module number
 *				-0x2  - failure to adjust offsets
 *				-0x3  - failure to acquire ADC traces
 *				-0x4  - failure to start the control task run
 *
 *			Run type 0x1000
 *				 0x10 - success
 *				-0x11 - invalid Pixie module number
 *				-0x12 - failure to start the data run
 *
 *			Run type 0x2000
 *				 0x20 - success
 *				-0x21 - invalid Pixie module number
 *				-0x22 - failure to resume the data run
 *
 *			Run type 0x3000
 *				 0x30 - success
 *				-0x31 - invalid Pixie module number
 *				-0x32 - failure to end the run
 *
 *			Run type 0x4000
 *				-0x41						- invalid Pixie module number
 *				-0x42						- error during readout
 *					0						- no run in progress
 *					1						- run in progress
 *				CSR value					- when run tpye = 0x40FF
 *          total number of spills written  - when run tpye = 0x4400 or 0x4401
 *
 *			Run type 0x5000
 *				 0x50 - success
 *				-0x51 - failure to save histogram data to a file
 *
 *			Run type 0x6000
 *				 0x60 - success
 *				-0x61 - failure to save list mode data to a file
 *
 *			Run type 0x7000
 *				 0x70 - success
 *				-0x71 - failure to parse the list mode data file
 *				-0x72 - failure to locate list mode traces
 *				-0x73 - failure to read list mode traces
 *				-0x74 - failure to read event energies
 *				-0x75 - failure to read PSA values
 *				-0x76 - invalid list mode parse analysis request
 *
 *			Run type 0x8000
 *				 0x80 - success
 *				-0x81 - failure to read out MCA spectrum from the MCA file
 *
 *			Run type 0x9000
 *				 0x90 - success
 *				-0x91 - failure to read out MCA section of external memory
 *				-0x92 - failure to write to MCA section of external memory
 *				-0x93 - failure to read out LM section of external memory
 *				-0x94 - failure to write to LM section of external memory
 *				-0x95 - invalid external memory I/O request
 *
 *          Run type 0xA000
 *				 0xA0 - success
 *              -0xA1 - invalid pixie module number
 *				-0xA2 - failure to stop
 *				-0xA3 - failure to write
 *				-0xA4 - failure to resume
 *              -0xA5 - invalid external runtype (lower 12 bits)        
 *
 ****************************************************************/

S32 Pixie_Acquire_Data (
			U16 Run_Type,		// data acquisition run type
			U32 *User_data,		// an array holding data to be transferred back to the host
			S8  *file_name,		// file name for storing run data 
			U8  ModNum )		// Pixie module number
{
	U8  str[256];
	S8  base_name[256];
	U8	k, m, len;
	U16	upper, lower, i;
	U32 tl, th, tlh;
	U16 idx = 65535;
	U16 *Run_Header = NULL;
	U16  HeaderInfo[6] = {0};
	U16 ListFileVariant, BoardRevision;
	U32	CSR;
	U32 value;
	S32	allexpress, retval=0, active, error=0, status;
	double 	BLcut, tau;
	unsigned char eepromEntry[6];
	FILE *ListFilePointer = NULL;	
	U32 CurrentModNum, MNstart, MNend;	// for looping over modules if ModNum==Number_Modules
	U64 filelen, filepos;
	S64 readlen;

	// for temp list run
	U32 *listBuffer = NULL;
	U32 Wcount, dwStatus, datatimeout, timeout, timeouterror;
	U32 clrbuffer[MAX_HISTOGRAM_LENGTH*NUMBER_OF_CHANNELS]={0};
//#define MEASURERUNTIME	// to measure time between spills for data rate measurement

	len = strlen(file_name);
	strcpy(base_name, file_name);	// get base file name without last 2 characters
	// len>4 check to avoid stack corruption when Pixie_Acquire_Data() is called with empty file name
	if (len>4) base_name[len-4] = '\0';

	upper=(U16)(Run_Type & 0xF000);
	lower=(U16)(Run_Type & 0x0FFF);
	
	if (Offline == 1 && upper != 0x7000 && upper != 0x8000) {
		sprintf(ErrMSG, "(Pixie_Acquire_Data): Offline mode. No I/O operations possible");
		Pixie_Print_MSG(ErrMSG,1);
		return (0);
	}

	/* Module muber check */
	if(ModNum > Number_Modules)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid Pixie module number for control task run, ModNum=%d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-0x1 - (upper >> 8));	// e.g. -0x11 for error in task 0x1000
	}

	// ModNum is either equal to Number_Modules -> loop over all modules, or 
	// ModNum is less than       Number_Modules -> execute for that module only
	if(ModNum == Number_Modules)
	{
		MNstart = 0;
		MNend = Number_Modules;
	}
	else
	{
		MNstart = ModNum;
		MNend = ModNum+1;
	}
//sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): task %x, ModNum=%d", Run_Type, ModNum);
//Pixie_Print_MSG(ErrMSG,1);
	switch(upper)
	{
		case 0x0000:  /* Start a control task run in a Pixie module */

			if (lower == 0x0003) lower = ADJUST_OFFSETS;		// legacy run type (controltask) 3 and 4 are now 0x83 and 0x84
			if (lower == 0x0004) lower = ACQUIRE_ADC_TRACES;
			


			switch(lower)
			{  
				case ADJUST_OFFSETS:  
					if (PCIBusType==EXPRESS_PCI) 
						retval=Adjust_Offsets_DSP(ModNum);	// for P4e, redirect old adjust offsets with ramp to new DSP implementation 
					else
						retval=Adjust_Offsets(ModNum);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to adjust offsets in Module %d, retval=%d", ModNum, retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x2);
					}
					break;

				case ADJUST_OFFSETS_DSP:  
					retval=Adjust_Offsets_DSP(ModNum);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to adjust offsets in Module %d, retval=%d", ModNum, retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x2);
					}
					break;

				case ACQUIRE_ADC_TRACES:
					if(ModNum == Number_Modules)	// can not loop over modules
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid Pixie module number for 'get traces' controltask, ModNum=%d", ModNum);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x1);
					}
					retval=Get_Traces(User_data, ModNum, NUMBER_OF_CHANNELS);	/* Acquire traces for all channels */
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to acquire ADC traces in Module %d, retval=%d", ModNum, retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x3);
					}
					break;

				case 24:
					if(ModNum == Number_Modules)	// can not loop over modules
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid Pixie module number for controltask 24, ModNum=%d", ModNum);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x1);
					}
					retval=Get_Slow_Traces(User_data, ModNum, file_name);	/* Acquire traces for all channels */
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to acquire ADC traces in Module %d, retval=%d", ModNum, retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x3);
					}
					break;
					
				case ADJUST_BLCUT:
					/* Find baseline cut value */
					/* k is the current channel number */
					/* CurrentModNum is the current module number */
					for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
						for(k = 0; k < NUMBER_OF_CHANNELS; k++) {
							BLcut_Finder(CurrentModNum, k, &BLcut);
						}
						/* Program FiPPI */
						Control_Task_Run(CurrentModNum, PROGRAM_FIPPI, 1000);
						sprintf(ErrMSG, "Module %d finished adjusting BLcut", CurrentModNum);
						Pixie_Print_MSG(ErrMSG,1);
					}
					break;
					

				case ADJUST_TAU:
						
				    /* Find tau value */
				    /* k is the current channel number */
				    /* CurrentModNum is the current module number */ 
				    for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
						for(k = 0; k < NUMBER_OF_CHANNELS; k++) {
							/* The index offset for channel parameters */
							idx=Find_Xact_Match("TAU", Channel_Parameter_Names, N_CHANNEL_PAR);
							tau = Pixie_Devices[CurrentModNum].Channel_Parameter_Values[k][idx]*1.0e-6; 
							Tau_Finder(CurrentModNum, (U8)k, &tau);	
							tau /= 1.0e-6;
							Pixie_Devices[CurrentModNum].Channel_Parameter_Values[k][idx]=tau;
							/* Update DSP parameters */
							sprintf(str,"PREAMPTAUA%d",k);
							idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
							Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx]=(U16)floor(tau);
							/* Download to the data memory */
							value = (U32)Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];
							Pixie_IODM(CurrentModNum, (DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);
							sprintf(str,"PREAMPTAUB%d",k);
							idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
							Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx]=(U16)((tau-floor(tau))*65536);
							/* Download to the data memory */
							value = (U32)Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];
							Pixie_IODM(CurrentModNum, (DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);			
							BLcut_Finder(CurrentModNum, (U8)k, &BLcut);
						}
						/* Program FiPPI */
						Control_Task_Run(CurrentModNum, PROGRAM_FIPPI, 1000);
						sprintf(ErrMSG, "Module %d finished adjusting tau", CurrentModNum);
						Pixie_Print_MSG(ErrMSG,1);
				    }
				    break;

				

				case READ_EEPROM_MEMORY:
					if (PCIBusType==REGULAR_PCI) {
						for (i = 0; i < EEPROM_MEMORY_SIZE; i++) 
							  I2C24LC16B_Read_One_Byte (ModNum, i, (U8 *)&User_data[i]);
					}
#ifdef WINDRIVER_API
					else if (PCIBusType==EXPRESS_PCI) {
						for (i=0;i < EEPROM_SIZE/EEPROM_WD_SIZE; i++) {				// read all available data from EEPROM			
							PIXIE500E_ReadI2C_Word(hDev[ModNum], &value, i);		// read a 32 bit word
							User_data[4*i+0] = (value & 0x000000FF);				// unpack into bytes 
							User_data[4*i+1] = (value & 0x0000FF00) >> 8;
							User_data[4*i+2] = (value & 0x00FF0000) >> 16;
							User_data[4*i+3] = (value & 0xFF000000) >> 24;
						}
						for (i=EEPROM_SIZE/EEPROM_WD_SIZE*4;i < EEPROM_MEMORY_SIZE; i++) {
							User_data[i] = 0;										// fill the rest with zeros
						}
					}
#endif
				    break;
				    
				case WRITE_EEPROM_MEMORY:

					if (PCIBusType==REGULAR_PCI) {
						sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): writing to I2C EEPROM");
						Pixie_Print_MSG(ErrMSG,1);
						for (i = 0; i < EEPROM_MEMORY_SIZE; i++) {
							I2C24LC16B_Write_One_Byte (ModNum, i, (U8 *)&User_data[i]);
							Pixie_Sleep(6);
						}
					}
#ifdef WINDRIVER_API
					else if (PCIBusType==EXPRESS_PCI) {
						sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): writing to GN EEPROM");
						Pixie_Print_MSG(ErrMSG,1);
						// write only into space available for XIA, skip write for words with GN configuration
						for (i=EEPROM_XIA_OFFSET;i < EEPROM_SIZE/EEPROM_WD_SIZE; i++) {
							// pack byte values into array
							eepromEntry[0] = (unsigned char)(0xF108 & 0x00FF);			
							eepromEntry[1] = (unsigned char)((0xF108 & 0xFF00) >> 8);	//		 entry[0,1] are a (bogus) GN register address
							eepromEntry[2] = (unsigned char)(User_data[4*i+0] & 0xFF);
							eepromEntry[3] = (unsigned char)(User_data[4*i+1] & 0xFF);
							eepromEntry[4] = (unsigned char)(User_data[4*i+2] & 0xFF);
							eepromEntry[5] = (unsigned char)(User_data[4*i+3] & 0xFF);
							dwStatus=PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, i);
							if( (dwStatus==WD_WINDRIVER_STATUS_ERROR) || (dwStatus<0) ) {
								sprintf(ErrMSG, "*ERROR* (Pixie500e_program_EEPROM): Error in PIXIE500E_WriteI2C_Word");
								Pixie_Print_MSG(ErrMSG,1);
								return(-1);
							}
						}
					}
#endif 
				    break;
				
				case WRITE_EEPROM_MEMORY_SHORT: // Write only 64 bytes of most frequently changed data
					if (PCIBusType==REGULAR_PCI) {
						for (i = 0; i < 64; i++) {
							I2C24LC16B_Write_One_Byte (ModNum, i, (U8 *)&User_data[i]);
							Pixie_Sleep(6);
						}
					}

#ifdef WINDRIVER_API
					else if (PCIBusType==EXPRESS_PCI) {
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): writing to GN EEPROM (byte 28-95 only)");
					Pixie_Print_MSG(ErrMSG,1);
						// write only into space available for XIA, skip write for words with GN configuration
						for (i=EEPROM_XIA_OFFSET;i < 24; i++) {
							// pack byte values into array
							eepromEntry[0] = (unsigned char)(0xF108 & 0x00FF);			//		 entry[0,1] are a (bogus) GN register address
							eepromEntry[1] = (unsigned char)((0xF108 & 0xFF00) >> 8);
							eepromEntry[2] = (unsigned char)(User_data[4*i+0] & 0xFF);
							eepromEntry[3] = (unsigned char)(User_data[4*i+1] & 0xFF);
							eepromEntry[4] = (unsigned char)(User_data[4*i+2] & 0xFF);
							eepromEntry[5] = (unsigned char)(User_data[4*i+3] & 0xFF);
							dwStatus=PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, i);
							if( (dwStatus==WD_WINDRIVER_STATUS_ERROR) || (dwStatus<0) ) {
								sprintf(ErrMSG, "*ERROR* (Pixie500e_program_EEPROM): Error in PIXIE500E_WriteI2C_Word");
								Pixie_Print_MSG(ErrMSG,1);
								return(-1);
							}
						}
					}
#endif 
				    break;
				
				default:  /* note, host must check for run end of the control tasks */
				    retval=Start_Run(ModNum, NEW_RUN, 0, lower);
				    if(retval < 0) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to start control run in Module %d, retval=%d", ModNum, retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x4);
				    }
				    break;
			}

			retval=0x0;
			break;

/* ****************************************************************************** */
		case 0x1000:  /* Start a new data acquisition run */

			switch(lower)
			{  
			case 0x100:
			case 0x101:
			case 0x102:
			case 0x103:

				
				// check module type, 0x10# is only valid for a systems without Pixie-500e (for now)
				allexpress=Pixie_Check_Moduletypes(ModNum);
				if(allexpress!=2)
				{	
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): run type 0x100 only supported for systems without Pixie-500 Express");
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x13);
				}

				retval=Start_Run(ModNum, NEW_RUN, lower, 0);
				if(retval < 0)  // failure to start a new run 
				{
					if(ModNum == Number_Modules)
					{
						// need to stop the run in case some modules started the run OK 
						End_Run(Number_Modules);
					}
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to start a data run, retval=%d", retval);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x12);
				}

				// Polling loop for MultiThreadDAQ, executed on separate thread, poll while DSP is active.
				if (MultiThreadDAQ) 
				{ 
					// clear spill counters, msg buffer, open LM file
					memset(msgBuffer, 0, sizeof(msgBuffer)); // clear the Igor message buffer
					listFile[0] = fopen(file_name, "wb"); // create empty file, kept open for polling loop 
					for  (CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
						LMBufferCounter[CurrentModNum]=0;
						EndRunFound[CurrentModNum]=0;
					}
					
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): MultiThreadDAQ, Going into polling loop, file %s",file_name);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					//status=0;      // default: no error and run in progress
					
					do {
						Pixie_Sleep(2);
						retval = Read_Resume_Run(0, 0, ""); /* poll, write, resume -- module 0 always*/
						if(retval < 0)  /* failure to resume run in all modules */
						{
							/* need to stop the run in case some modules started the run OK */
							End_Run(Number_Modules);
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Read_Resume_Run failed, retval=%d", retval);
							Pixie_Print_MSG(ErrMSG,1);
							MT_KeepPolling = 0;
							return(-0xA0 + retval);
						} 
						LMBufferCounter[0] = LMBufferCounter[0] + retval;		// increment by # spills saved by Read_Resume_Run
	
					} while (MT_KeepPolling); // poll until end of run. (cleared by end run task)
					// ********************* end Polling loop ******************************************************************
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): MultiThreadDAQ, Done polling loop");
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					FlushIgorMSG();
				} // if MultiThreadDAQ

				break;

			case 0x400:
			case 0x401:
			case 0x402:
			case 0x403:
				memset(msgBuffer, 0, sizeof(msgBuffer)); // clear the Igor message buffer
				//sprintf(ErrMSG, "*info* (Pixie_Acquire_Data): Run type 0x400 started %s", base_name);
				//Pixie_Print_MSG(ErrMSG,1);
				// check module type, 0x40# is only valid for a pure P500e system
				allexpress=Pixie_Check_Moduletypes(ModNum);
				if(allexpress!=1)
				{	
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Run type 0x400 only supported for all-Express systems");
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x13);
				}

#ifdef WINDRIVER_API
				DMADataPos =0;
				for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
					
					// Create file and write file header
					MakeNewFile = 0;  // initialize global, also indicates to Create_List_Mode_File that this is the first call (no next)
					EndRunFound[CurrentModNum]=0;
					retval = Create_List_Mode_File(CurrentModNum, base_name, lower);
					if(retval<0) {						
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): StartRun 0x140# file not found error");
						Pixie_Print_MSG(ErrMSG,1);
						return(-1);
					}

					// This shall be moved to Boot.
					// DMA setup
					pDmaList[CurrentModNum] = NULL;
					LMBuffer[CurrentModNum] = malloc(DMA_LM_FRAMEBUFFER_LENGTH);
					if (!LMBuffer[CurrentModNum]) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Memory allocation for list mode buffer failure");
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x13);
					}
					memset(LMBuffer[CurrentModNum], 0x69, DMA_LM_FRAMEBUFFER_LENGTH); 
					// cache for list mode data for buffer quality control, only if not allocated yet
					LMBufferCopy[CurrentModNum] = malloc(DMA_LM_FRAMEBUFFER_LENGTH);
					if (!LMBufferCopy[CurrentModNum]) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Memory allocation for list mode buffer copy failure");
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x13);
					}
					// PIXIE500E_DMA_Trace_Setup() does
					// 1. WDC_DMASGBufLock()
					// 2. PIXIE500E_VDMACodeGen_TraceOut()
					// 3. PIXIE500E_DMA_ProgramSequencer()
					retval = PIXIE500E_DMA_Trace_Setup(hDev[CurrentModNum],  DMA_LM_FRAMEBUFFER_LENGTH, LMBuffer[CurrentModNum], &pDmaList[CurrentModNum]);
					if (retval != 0) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Run start, DMA setup failed, %d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x13);
					}
					retval = PIXIE500E_DMA_Init(hDev[CurrentModNum]);
					if (retval != 0) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Run start, DMA init failed, %d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x13);
					}

					// Up to this point, moved to Boot

					// Prepare interrupt processing
					// Set up INT3 for INT_CFG0, edge, GPIO_BYPASS mode for INT3, VDMA event 0x8 enable, 
					// ISR as PIXIE500E_IntHandler_INT3,
					// clearing IRQ, clear and re-enable VDMA event 0x8, calling Write_DMA_List_Mode_File()
					// TODO: no error check in PIXIE500E_InterruptSetup_INT3!				
					if ((Polling==0) && (lower != 0x403)) 
						PIXIE500E_InterruptSetup_INT3(hDev[CurrentModNum], (U8)CurrentModNum, lower);


				} // for modules

				//sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): about to start a data run, polling=%d", Polling);
				//Pixie_Print_MSG(ErrMSG,1);
				retval=Start_Run(ModNum, NEW_RUN, lower, 0);

				if(retval < 0) { /* failure to start a new run */
					if(ModNum == Number_Modules) {
						/* need to stop the run in case some modules started the run OK */
						End_Run(Number_Modules);
					}
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to start a data run, retval=%d", retval);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x12);
				} // if failed to start

#ifdef MEASURERUNTIME
				RunStartTicks = GetTickCount();
#endif

				for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {	
					retval = WDC_DMASyncCpu(pDmaList[CurrentModNum]); // SyncCpu needed before performing DMA transfers.
					VDMADriver_Go(hDev[CurrentModNum]);
				} // for modules, DMA go.

				// Polling loop for MultiThreadDAQ, executed on separate thread, poll while DSP is active.
				if (MultiThreadDAQ) { 
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): MultiThreadDAQ, Going into polling loop");
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					status=0;      // default: no module saved data

					do {
						Pixie_Sleep(2);
						for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
							// Not using VDMADriver_isIdle (with DMA_CSR) here, but just check the LMBuffer[last] for content?
							// if 0x69 (initialized on run start), or 0xA5 (initialized on buffer dump), then
							// the frame buffer is not filled yet, we should not be idle.
							if (LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0xA5A5A5A5 || LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0x69696969) {				
								//if (VDMADriver_isIdle(hDev[CurrentModNum])!= TRUE) {
								//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x1403): buffer NOT FILLED yet (not idle).");
								//Pixie_Print_MSG(ErrMSG,1);
								//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0xA010): POLL, DMA NOT IDLE");
								//Pixie_Print_MSG(ErrMSG,1);
							}
							else { // some values in the last frame buffer element: real data, we should be idle
								VDMADriver_Halt(hDev[CurrentModNum]);
								if (Write_DMA_List_Mode_File ((U8)CurrentModNum, "", lower)<0) {// read data, check, dump to file
									// if error
									status = -1;
								}
								if (!Check_Run_Status((U8)CurrentModNum)) status = 1;

								sprintf(ErrMSG, "Thread 0x1403, DMA idle, Got buffer %d", LMBufferCounter[CurrentModNum]);
								Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
							} // if checking last frame buffer element
						} // for modules
					} while (status==0); // poll until end of run.
					// ********************* end Polling loop ******************************************************************
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): MultiThreadDAQ, Done polling loop");
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					FlushIgorMSG();
				} // if MultiThreadDAQ


#endif
				break; // 0x400, 0x401, 0x402, or 0x403

			default: 
				if (lower != 0x301)
				{
					sprintf(ErrMSG, "*WARNING* (Pixie_Acquire_Data): Start run with unknown/obsolete run type %d.", lower);
					Pixie_Print_MSG(ErrMSG,1);
				}

				retval=Start_Run(ModNum, NEW_RUN, lower, 0);
				if(retval < 0)  // failure to start a new run 
				{
					if(ModNum == Number_Modules)
					{
						// need to stop the run in case some modules started the run OK 
						End_Run(Number_Modules);
					}
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to start a data run, retval=%d", retval);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x12);
				}
				break;

			} // end lower switch
			retval=0x10;
			break;

/* ****************************************************************************** */

		case 0x2000:  /* Resume data acquisition run in all modules */
			// Note: Runtypes 0x301, 0x40# do not resume, so this really only is for the case of 0x10# and new files for 0x40#

			switch(lower)
			{  
			case 0x100:
			case 0x101:
			case 0x102:
			case 0x103:
				retval=Start_Run(ModNum, RESUME_RUN, lower, 0);
				if(retval < 0)  /* failure to resume run in all modules */
				{
					if(ModNum == Number_Modules)
					{
						/* need to stop the run in case some modules started the run OK */
						End_Run(Number_Modules);
					}
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to resume a data run, retval=%d", retval);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x22);
				}

				break;

			case 0x400:	
			case 0x401: 
			case 0x402:
					// "resume" for P4e/P500e runs means to switch files to new names
					// NB Potentially dangerous if closing/opening file at the same time
					// as Write_DMA_List_Mode_File() dump data (may be called by interrupt).
					// so here we just remember the file name, Write_DMA_List_Mode_File() makes the new files 
					strcpy(next_base_name, base_name);		// copy top level's new file name (base) for later use
					MakeNewFile = 1;						// indicate to Write_DMA_List_Mode_File 
					break;

			default: 
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Unknown resume run task");
					Pixie_Print_MSG(ErrMSG,1);
				break;
			} // end lower switch

			retval=0x20;
			break;

/* ****************************************************************************** */

		case 0x3000:  /* Stop a data run */


			
	//		sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): End run: starting 0x%x",lower);
	//		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
			FlushIgorMSG();

			switch(lower)
			{  
				case 0x400:
				case 0x401:
				case 0x402:
				case 0x403:
	#ifdef WINDRIVER_API


					// 1. issue command to stop the run, but it won't be finished quite yet, keep DMA going
					for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
					{
						/* Set RESUME in DSP to 2 to signal a run-stop in all cards */
				//		value = 2;
				//		Pixie_IODM((U8)CurrentModNum, Resume_Index+DATA_MEMORY_ADDRESS, MOD_WRITE, 1, &value);
						
						/* Stop run in all modules */
						dwStatus = Run_Enable_Clear((U8)CurrentModNum);
					//	if (dwStatus != 0) {
					//		sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data) End Run: Unable to clear Run Enable.");
					//		Pixie_Print_MSG(ErrMSG,1);
					//		return(-0x13);
					//	}
					}

					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): End run, cleared RunEna");
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					FlushIgorMSG();


					// 2. read out any remaining data from SDRAM until the module indicates run is no longer active. 
					//	  (could be improved in speed by reading out modules in parallel)
					timeouterror = 0;
					for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
					{
						// DSP is stopped now, but there was some data out of DSP.
						// Read it out and dump to disk until DMA is idle.
						datatimeout=0;	// combined end condition
						timeout=0;		// timeout just for time
						Wcount = 0;		// timeout after each transfer
						if (Polling==1)	// if no interupts, we still need to read out the old way. But what about multi-thread 0x403?
						{
							while(!datatimeout)
							{
								Pixie_Sleep(1);

								active = Check_Run_Status((U8)CurrentModNum);
								if(active<0) {
									sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Failed to read Run Status, aborting");
									Pixie_Print_MSG(ErrMSG,1);
									return (active);
								}
								if(active) Wcount = 0;		// this ensures we wait at least N cycles after DSP is done
								sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): End run, checked RS =  %x",active);
								Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);


								retval = 0; 
								// if the frame buffer is not filled yet, we should not be idle.
								if (LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0xA5A5A5A5 || LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0x69696969) {				
									// do nothing
								}
								else { 
									// Finished transferring the buffer: dump it, rewind, restart DMA sequencer
									retval = Write_DMA_List_Mode_File ((U8)CurrentModNum, "", lower);	// =1 if end 
									sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): End run, written another buffer for ModNum=%d, count=%d",CurrentModNum, timeout);
									Pixie_Print_MSG(ErrMSG,1);
									Wcount = 0;			// this ensures we wait at least N cycles after each transfer
								} // if DMA is idle	
								if(EndRunFound[CurrentModNum]) {
									sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): End run, QC found EOR");
									Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
								}
								 		
								Wcount++; 
								timeout++;
								if( (Wcount==DMABUFREFILL_TIMEOUT && active==0) || EndRunFound[CurrentModNum]==1 || retval<0 || timeout==DMATRANSFER_TIMEOUT) {		// end with timeout after DSP finished DMA flush, or on error or timeout
									datatimeout=1;												// 50ms after DSP or transfer done, 5000ms max, or EOR found by readout routine
									//Also assumes that a max of 128 x 2MB buffer (SDRAM completely fullwith backlog) is read out in < 5s 
								}								
							}
						}	// polling
						else {
							while(!datatimeout)
							{
								Pixie_Sleep(1); 
								//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): Waiting for endrun timeout=%d, active=%d,Wcount=%d",timeout,active,Wcount );
								//Pixie_Print_MSG(ErrMSG,1);

								active = Check_Run_Status((U8)CurrentModNum);
								if(active<0) {
									sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Failed to read Run Status, aborting");
									Pixie_Print_MSG(ErrMSG,1);
									return (active);
								}
								if(active) Wcount = 0;										// this ensures we wait at least N cycles after DSP is done

								sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): End run, checked RS =  %x",active);
								Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

								Wcount++;
								timeout++;
								// TODO: remove Wcount from the condition below? As is will stop loop50ms after DSP finishes, with no guarantee that interrupts are all done
								if( (Wcount==DMABUFREFILL_TIMEOUT && active==0) || EndRunFound[CurrentModNum]==1 || timeout==DMATRANSFER_TIMEOUT) {				// end with timeout after DSP finished DMA flush, or on error or timeout
									datatimeout=1;											// 50ms after DSP or transfer done, 5000ms max, or EOR found by readout routine 	
								}
							}	// end while

							FlushIgorMSG();
						}	// end not polling
						
						if(timeout==DMATRANSFER_TIMEOUT) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): End run timed out for ModNum=%d",CurrentModNum);
							Pixie_Print_MSG(ErrMSG,1);
							timeouterror =1;
							//return(-0x32);
						}

					//	sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): data read wait done for module %d",CurrentModNum);
					//	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

					} // for modules

					if(timeouterror) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): End run timed out for at least one module");
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x32);
					}

#ifdef MEASURERUNTIME
				RunStopTicks = GetTickCount();
				sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): Windows DAQ time %d (ms)",(RunStopTicks-RunStartTicks));
				Pixie_Print_MSG(ErrMSG,1);
				sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): Average data transfer rate (module 0) = %d (MB/s),%d",2000*LMBufferCounter[0]/(RunStopTicks-RunStartTicks),LMBufferCounter[0]);
				Pixie_Print_MSG(ErrMSG,1);
#endif


				// 3. clean up the DMA
       			for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
					{
						// Halt DMA
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): End run, halt DMA");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

						VDMADriver_Halt(hDev[CurrentModNum]);

						Pixie_Sleep(10);
 						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): DMA halted.");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

						// To make the flush data (64 words at the end of the run from DSP) disappear.
						retval = PIXIE500E_ReadWriteReg(hDev[CurrentModNum], APP_SDRAM_STATUS, WDC_READ, &value, FALSE);						
						value |= 0x8;	// Re-init SDRAM, assert bit 3
						retval = PIXIE500E_ReadWriteReg(hDev[CurrentModNum], APP_SDRAM_STATUS, WDC_WRITE, &value, FALSE);
						// Reset FIFOs
						VDMADriver_EventSet(hDev[CurrentModNum], 0x400);
						Pixie_Sleep(10);

						// Clean up after the run
						if (pDmaList[CurrentModNum] != NULL) {
							retval = WDC_DMASyncIo(pDmaList[CurrentModNum]);
							retval = WDC_DMABufUnlock(pDmaList[CurrentModNum]);
							pDmaList[CurrentModNum] = NULL;
						}
						if (LMBuffer[CurrentModNum] != NULL) {
							free(LMBuffer[CurrentModNum]);
							LMBuffer[CurrentModNum] = NULL;
						}
						if (LMBufferCopy[CurrentModNum] != NULL) {
							free(LMBufferCopy[CurrentModNum]);
							LMBufferCopy[CurrentModNum] = NULL;
						}
						fclose(listFile[CurrentModNum]); // close if using global listFile array
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x34xx ListMode DMA resources released.");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

						//// Without ADC trace acquisition between the runs, after the first list-mode run,
						//// there is some junk data in the framebuffer.
						//// The only way to avoid this was found to do the ADC trace acquisition,
						//// with Start_Run, VDMADriver_Go, and Pixie_IODM.
						//// AND, it HAS to be at the end of the run, not the beginning.
						//// Or we're missing some data in the beginning of list-mode data, but only in even runs:
						//// some bit must be toggling.

						// Prepare for ADC trace (to emulate ADC trace between the list-mode runs)
						//////////////////////////////////////////////////////////
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x34xx Run end, start dummy trace DMA...");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
						pDmaTrace = NULL;
						listBuffer = malloc(IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
						if(!listBuffer) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Memory allocation failure");
							Pixie_Print_MSG(ErrMSG,1);
							return(-0x13);
						}
						//Get_Traces(listBuffer, CurrentModNum, NUMBER_OF_CHANNELS);
						// *
						retval = PIXIE500E_DMA_Trace_Setup(hDev[CurrentModNum],  IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32), listBuffer, &pDmaTrace);
						if (retval != 0) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Run end, dummy trace DMA setup failed, %d", retval);
							Pixie_Print_MSG(ErrMSG,1);
							return(-0x13);
						}
						retval = PIXIE500E_DMA_Init(hDev[CurrentModNum]);
						if (retval != 0) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): Run end, dummy trace DMA init failed, %d", retval);
							Pixie_Print_MSG(ErrMSG,1);
							return(-0x13);
						}
						retval = WDC_DMASyncCpu(pDmaTrace);
						retval = Start_Run((U8)CurrentModNum, NEW_RUN, 0, GET_TRACES);
						VDMADriver_Go(hDev[CurrentModNum]);
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x34xx started run in DSP & started DMA");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
						dwStatus = PIXIE500E_DMA_WaitForCompletion(hDev[CurrentModNum], TRUE);
						if (dwStatus != 0) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data) IO_BUFFER read: DMA transfer timed out.");
							Pixie_Print_MSG(ErrMSG,1);
							return(-0x13);
						}
						
		/*	// WH_TODO: this can be commented out, since we reset properly now in DMA start. Already removed in get_Traces
						// To make the flush data (64 words at the end of the run from DSP) disappear.
						retval = PIXIE500E_ReadWriteReg(hDev[CurrentModNum], APP_SDRAM_STATUS, WDC_READ, &value, FALSE);
						value |= 0x8;	// Re-init SDRAM, assert bit 3
						retval = PIXIE500E_ReadWriteReg(hDev[CurrentModNum], APP_SDRAM_STATUS, WDC_WRITE, &value, FALSE);
						retval = PIXIE500E_ReadWriteReg(hDev[CurrentModNum], APP_SDRAM_STATUS, WDC_READ, &value, FALSE);
						// Reset FIFOs
						VDMADriver_EventSet(hDev[CurrentModNum], 0x400);
			// comment out until here */

						retval = WDC_DMASyncIo(pDmaTrace);
						retval = WDC_DMABufUnlock(pDmaTrace); 
							// * /
						free(listBuffer);
						pDmaTrace = NULL;
						
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x34xx Run end, dummy trace DMA done!");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
						Pixie_Sleep(100);
						//////////////////////////////////////////////////////////////////
						// ADC trace DMA finished


						// Clean up interrupts
						if (Polling==0)
							PIXIE500E_IntDisable(hDev[CurrentModNum]);

						//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x34xx DONE.");
						//Pixie_Print_MSG(ErrMSG,1);
					} // for modules
#endif
					break;

				default:
					retval=End_Run(ModNum);  /* Stop the run */

					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to end the run, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x32);
					}
	//				sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): succeeded to end the run 0x%x, retval=%d", lower, retval);
	//				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					FlushIgorMSG();

					break;

			}	// end lower switch

			retval=0x30;
			break;
/* ****************************************************************************** */

		case 0x4000:  /* Poll run status */

			switch(lower)
			{	

			case 0x100:
			case 0x101:
			case 0x102:
			case 0x103:
				if(MultiThreadDAQ) {
					// in MultiThreadDAQ, return number of spills saved since run start
					retval = LMBufferCounter[0];
					FlushIgorMSG();
				} else {
					// otherwise Check run status; only return value of ACTIVE bit 
					retval=Check_Run_Status(ModNum);
				}

			case 0x400:
			case 0x401:
			case 0x402:
			case 0x403:
				// P4e/P500e only
				// poll all modules individually, if data ready, store to file
				// returns total number of spills saved to file
				retval=0;      // default: no module saved data
#ifdef WINDRIVER_API
				for (CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {

					if (Polling==1)
					{
						//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x44xx Polling mode");
						//Pixie_Print_MSG(ErrMSG,1);						
						// When polling, do nothing if DMA is not idle, write_DMA_List_Mode_File() if DMA is idle
						// options: not use VDMADriver_isIdle (with DMA_CSR) here, but just check the LMBuffer[last] for content
						// if 0x69 (initialized on run start), or 0xA5 (initialized on buffer dump), then the frame buffer is not filled yet, we should not be idle.
						if (LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0xA5A5A5A5 || LMBuffer[CurrentModNum][DMA_LM_FRAMEBUFFER_LENGTH/sizeof(UINT32)-1]==0x69696969) {				
							// do nothing
							if (PollForNewData)
							{
								status = FindNewDMAData();		// finds to position of just after the last new RS block
								//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440x): data up to %d words (16bit).",2*status);
								//Pixie_Print_MSG(ErrMSG,1);//PrintDebugMsg_other);
								if(status != DMADataPos)		// if unchanged, there is no new data
								{
									memcpy(User_data, &LMBuffer[0][DMADataPos], (status-DMADataPos)*sizeof(U32));
								
									sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440x): copying = %d words (16bit).",2*(status-DMADataPos));
									Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
									DMADataPos =status;		// advance by amount read
								}
								sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440x): DMADataPos = %d.",DMADataPos);
								Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
							}
						}
						else { // some values in the last frame buffer element: real data, we should be idle
							//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440#): buffer is FILLED (idle).");
							//Pixie_Print_MSG(ErrMSG,1);
							VDMADriver_Halt(hDev[CurrentModNum]);
							if (Write_DMA_List_Mode_File ((U8)CurrentModNum, "", lower) < 0) { // read data, check, dump to file
								// if error
								error = -1;
							}
							else {
								if (PollForNewData)
								{
									// return new data = from current to end of block	
									memcpy(User_data, &LMBuffer[0][DMADataPos], (DMA_LM_FRAMEBUFFER_LENGTH/4-DMADataPos)*sizeof(U32));
									DMADataPos =numDWordsLeftover[0];		// restart from beginning plus the leftovers from last buffer
									sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440x): reset DMADataPos = %d.",DMADataPos);
									Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
								}
										
							}
						} // end if checking last frame buffer element (if DMA idle)		

					}	// end if polling
					//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x440x): polling done.");
					//Pixie_Print_MSG(ErrMSG,1);

					// polling or not, increment the buffer counters
					retval += LMBufferCounter[CurrentModNum];
 
				} // for modules
#endif

				if (!Polling | MultiThreadDAQ)
				{
					// When using interrupts or MT polling from run start, 
					// Write_DMA_List_Mode_File() is excuted from ISR,
					// so on poll only report number of framebuffers [done above] and messages to Igor.
					FlushIgorMSG();
				}

				if(error<0)				// propagate error to retval
					retval = -0x42;

				break;

/*		can be consolidated with other 0x40#
			case 0x403:
				// actual polling is done on a separate thread in 0x1403. Returning current number of spills
				retval = 0;
				for  (CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++) {
					retval += LMBufferCounter[CurrentModNum];
				}
				//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data): 0x4403 poll, framebuffers=%d %%%%%%%%%%%%%%%%%%%%%%%%%", retval);
				//Pixie_Print_MSG(ErrMSG,1);
				FlushIgorMSG()
				// TODO: what if we are putting stuff there from Write_DMA_List_File()?

				break;
*/
				
				case 0x0F0:  /* read Config Status Register */
					Pixie_Register_IO(ModNum, PCI_CFSTATUS, MOD_READ, &CSR);
					retval=CSR & 0xFFFF;
					break;

				case 0x0F1:  /* read Config Version Register */
					Pixie_Register_IO(ModNum, PCI_VERSION, MOD_READ, &CSR);
					retval=CSR & 0xFFFF;
					break;

				case 0x0F2:  /* read Config I2C Register */
					Pixie_Register_IO(ModNum, PCI_I2C, MOD_READ, &CSR);
					retval=CSR & 0xFFFF;
					break;

#ifdef WINDRIVER_API
				case 0x0F3:  /* read debug */
					PIXIE500E_ReadWriteReg(hDev[ModNum], PL_WR_BRG_RD_THRES, WDC_READ, &CSR, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F3): register read %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					retval=CSR;
					break;
				case 0x0F4:  /* write debug */
					CSR = 23;
					PIXIE500E_ReadWriteReg(hDev[ModNum], PL_WR_BRG_RD_THRES, WDC_WRITE, &CSR, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F4): register write %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					retval=CSR;
					break;
				case 0x0F5:  /* write debug */
					CSR = 0xE4;
					PIXIE500E_ReadWriteReg(hDev[ModNum], PL_WR_BRG_RD_THRES, WDC_WRITE, &CSR, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F5): register write %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					retval=CSR;
					break;
				case 0x0F6:  /* SDRAM debug */
					PIXIE500E_ReadWriteReg(hDev[ModNum], APP_SDRAM_STATUS, WDC_READ, &CSR, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F6): SDRAM pointer diff %x.",(CSR >> 8));
					Pixie_Print_MSG(ErrMSG,1);
					retval=CSR;
					break;
				case 0x0F7:  /* CLK PLL reset debug, all modules */

					for (CurrentModNum = 0; CurrentModNum < Number_Modules ; CurrentModNum ++) {
						PIXIE500E_LBClkReset(hDev[CurrentModNum]);
						//PIXIE500E_ReadWriteReg(hDev[CurrentModNum], CLK_CSR, WDC_READ, &CSR, FALSE);		// read clk csr
						//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F7): read CLK CSR %x.",CSR);
						//Pixie_Print_MSG(ErrMSG,1);
						//retval = CSR;
						//CSR = CSR | (U32)(pow(2.0,27));												// set reset bit
						//sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F7): write CLK CSR %x.",CSR);
						//Pixie_Print_MSG(ErrMSG,1);
						//PIXIE500E_ReadWriteReg(hDev[CurrentModNum], CLK_CSR, WDC_WRITE, &CSR, FALSE);		// write
						//Pixie_Sleep(50);
						//CSR = retval;
						//PIXIE500E_ReadWriteReg(hDev[CurrentModNum], CLK_CSR, WDC_WRITE, &CSR, FALSE);	// write without reset
						//Pixie_Sleep(50);
						PIXIE500E_ReadWriteReg(hDev[CurrentModNum], CLK_CSR, WDC_READ, &CSR, FALSE);		// read clk csr
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F7): read CLK CSR %x.",CSR);
						Pixie_Print_MSG(ErrMSG,1);
						retval = CSR;
					}
					break;
				case 0x0F8:  /* APP CFG r/w debug */
					PIXIE500E_ReadWriteReg(hDev[ModNum], APP_CFG, WDC_READ, &CSR, FALSE);		// read APP_CFG
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F8): read APP_CFG %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					CSR = User_data[0];															// set new value
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F8): write APP_CFG %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					PIXIE500E_ReadWriteReg(hDev[ModNum], APP_CFG, WDC_WRITE, &CSR, FALSE);		// write
					
					PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS_MASK, WDC_READ, &CSR, FALSE);		// read APP_CFG
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F8): read APP_STATUS_MASK %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					CSR = User_data[1];															// set new value
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F8): write APP_STATUS_MASK %x.",CSR);
					Pixie_Print_MSG(ErrMSG,1);
					PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS_MASK, WDC_WRITE, &CSR, FALSE);		// write
					break;

			case 0x0F9:  /* bridge thresholds debug. P4e has thresholds hardcoded in FPGA, only one register remains for I/O test */
					th = User_data[0]; //cap_wr_brg_rd_hi = 460;
					tl = User_data[1]; //cap_wr_brg_rd_lo = 20;
					tlh = (tl << 10) + th;
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CAP_WR_BRG_RD_THRES, WDC_WRITE, &tlh, FALSE);
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CAP_WR_BRG_RD_THRES, WDC_READ, &tlh, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F9): CAP_WR_BRG_RD_THRES=0x%08X", tlh);
					Pixie_Print_MSG(ErrMSG,1);
					Pixie_Sleep(1);

					th = User_data[2]; //cap_rd_brg_wr_hi = 460;//gThr_hi;
					tl = User_data[3]; //cap_rd_brg_wr_lo = 20;//gThr_lo;		actually ignored in FPGA
					tlh = (tl << 10) + th;
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CAP_RD_BRG_WR_THRES, WDC_WRITE, &tlh, FALSE);
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CAP_RD_BRG_WR_THRES, WDC_READ, &tlh, FALSE);
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40F9): CAP_RD_BRG_WR_THRES=0x%08X", tlh);
					Pixie_Print_MSG(ErrMSG,1);
					Pixie_Sleep(1);
					break;

				case 0x0FA: 
					// Option 1) // send user the copy of the DMA buffer used to write (append to) the file
					//memcpy(User_data, LMBufferCopy, DMA_LM_FRAMEBUFFER_LENGTH);		

					// Option 2) // send user the actual DMA buffer
					//memcpy(User_data, &LMBuffer[0][0], DMA_LM_FRAMEBUFFER_LENGTH);		

					// Option 3) read new data from file
					// probably this should be made more resilient with CreateFile or fsopen, but for now it seems to work
					// first open file 
					if(ListFilePointer) 
						fclose(ListFilePointer);	// (close if already open)
					if(!(ListFilePointer = fopen(file_name, "rb"))) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data 0x40FA): can't open list mode data file %s", file_name);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x47);
					}
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40FA): list mode data file %s", file_name);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

					// get current length of file
					Pixie_fseek(ListFilePointer, 0, SEEK_END);
					filelen = Pixie_ftell(ListFilePointer); 
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40FA): file length %d words (16bit)", filelen/2);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					// get starting position from User_data[0]

					// compute how much to read (in bytes)
					readlen = filelen - 2*User_data[0];	// User_data[0] has last read position in 16bit words
					sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40FA): reading %d words (16bit)", readlen/2);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
					// move to last position and read
					if(readlen>0) {
						Pixie_fseek (ListFilePointer, 2*User_data[0], SEEK_SET); // move pointer in file N 16 bit words to position where previous read ended
						filepos = Pixie_ftell(ListFilePointer);
						sprintf(ErrMSG, "*DEBUG* (Pixie_Acquire_Data 0x40FA): reading from word %d (16bit)", filepos/2);
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
						status = fread (User_data, 2, readlen/2 , ListFilePointer);
						if(status != readlen/2) {
							sprintf(ErrMSG, "*error* (Pixie_Acquire_Data 0x40FA): Error reading file %s", file_name);
							Pixie_Print_MSG(ErrMSG,1);
							fclose(ListFilePointer);
							return(-0x48);			// Things could be bad, return <0 to alert GUI. 
						}
					} else {
						sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data 0x40FA): No new data in file %s", file_name);
						Pixie_Print_MSG(ErrMSG,1);
						fclose(ListFilePointer);
						return(User_data[0]);		// User_data has not changed, so return the same value
					}
					filepos = Pixie_ftell(ListFilePointer); 
					retval=filepos/2;
					fclose(ListFilePointer);
					break;

				case 0x0FB:  /* reboot FPGA */
					BoardRevision = (U16)Pixie_Devices[ModNum].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];
 					if ((BoardRevision & 0x0FF0) == MODULETYPE_P4e_16_125) 
						retval=PIXIE500E_ProgramFPGA(hDev[ModNum], 1, 12);		// type 12 is debug mode: force reboot
					if ((BoardRevision & 0x0FF0) == MODULETYPE_P4e_14_500) 
						retval=PIXIE500E_ProgramFPGA(hDev[ModNum], 1, 13);		// type 13 is debug mode: force reboot
					break; 	
#endif

				case 0x0FC:  /* write/clear SP1 */
					CSR=0;
					Pixie_Register_IO(ModNum, PCI_SP1, MOD_WRITE, &CSR);
					break; 		
				case 0x0FD:  /* read SP1 */
					Pixie_Register_IO(ModNum, PCI_SP1, MOD_READ, &CSR);
					retval=CSR;
					break;
				case 0x0FE:  /* Check run status; return value of WCR or HostControl */
					Pixie_RdWrdCnt(ModNum, &CSR);
					retval=CSR;
					break;
				case 0x0FF:  /* Check run status; return value of CSR or HostStatus */
					Pixie_ReadCSR(ModNum, &CSR);
					//retval=CSR;
					if(PCIBusType==EXPRESS_PCI) {
#ifdef WINDRIVER_API
						CSR = ClrBit(BIT_DATA_READY, (U16)CSR);
						CSR = CSR | (VDMADriver_isIdle(hDev[ModNum]) << BIT_DATA_READY);	// replace Data ready bit with DMA status
#endif
					}
					retval=CSR;
					break;

				default:  /* Check run status; only return value of ACTIVE bit */
					retval=Check_Run_Status(ModNum);

					if(PCIBusType==EXPRESS_PCI) 	// also check if DMA is idle
#ifdef WINDRIVER_API					
						retval = retval && VDMADriver_isIdle(hDev[ModNum]);
#endif
					break;
			}
			break;


		case 0x5000:  /* Read histogram data from Pixie's external memory and save it to a file */

			retval=Write_Spectrum_File(file_name);
			if(retval < 0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to save histogram data to a file, retval=%d", retval);
				Pixie_Print_MSG(ErrMSG,1);
				return(-0x51);
			}

			retval=0x50;
			break;


		case 0x6000:  /* Read list mode buffer data from Pixie's external memory and save it to a file */
			switch(lower)
			{  
				case 0x100:
				case 0x101:
				case 0x102:
				case 0x103:
					retval=Write_List_Mode_File(file_name);
					break;

				case 0x400:
				case 0x401:
				case 0x402:
				case 0x403:
					sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): List mode readout task 0x6000 not suported for runtype 0x40#.");
					Pixie_Print_MSG(ErrMSG,1);
					retval = -1;
					/*
#ifdef WINDRIVER_API	
					for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
					// Note: DMA buffers may fill up at different rates, so a simple loop here is not appropriate
					{
						sprintf(current_name,"%s.b%02d",base_name,CurrentModNum);// add module number as last 2 characters
						VDMADriver_Halt(hDev[ModNum]);
						retval = Write_DMA_List_Mode_File ((U8)CurrentModNum, current_name, lower);
					} // for modules
					break;
#endif
					*/

				default:
					retval = -1;
					break;
			}

			if(retval < 0) 	{
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to save list mode data to a file, retval=%d", retval);
				Pixie_Print_MSG(ErrMSG,1);
				return(-0x61);
			}
			
			retval=0x60;
			break;

		case 0x7000:  /* Offline list mode data parse routines */
			sprintf(ErrMSG, "*INFO* (Pixie_Acquire_Data): list mode data file %s", file_name);
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
			
			if (AutoProcessLMData > 4) lower = 0x100; /* Do nothing if auto processing mode is above 4 */
			/* Open the list mode file to check format */
			if(ListFilePointer) fclose(ListFilePointer);
			if(!(ListFilePointer = fopen(file_name, "rb"))) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): can't open list mode data file %s", file_name);
				Pixie_Print_MSG(ErrMSG,1);
				return(-0x77);
			}
			fread (HeaderInfo, sizeof(U16), 6,ListFilePointer);
			ListFileVariant = 0;
			if ((HeaderInfo[2] & 0x0F0F) >= 0x400 && (HeaderInfo[2] & 0x0F0F) <= 0x403) ListFileVariant = P500E_LIST_FILE;
			if ((HeaderInfo[2] & 0x0F0F) >= 0x100 && (HeaderInfo[2] & 0x0F0F) <= 0x103) ListFileVariant = P4_LIST_FILE;
			if (!ListFileVariant) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): bad data file (unknown runtype) %s %hu %hu", file_name, HeaderInfo[1], HeaderInfo[2]);
				Pixie_Print_MSG(ErrMSG,1);
				fclose(ListFilePointer);
				return(-0x77);
			}
			fclose(ListFilePointer);
			switch(lower)
			{
				case 0x100:  /* Do nothing mode */
					break;
				case 0x1:  /* Parse list mode data file */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Parse_List_Mode_Events(file_name, User_data);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7001);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to parse list mode data file, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x71);
					}
					break;

				case 0x2:  /* locate traces */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Locate_List_Mode_Traces(file_name, User_data);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7002);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to locate list mode traces, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x72);
					}

					break;

				case 0x3:  /* Vacant code */				
					break;

				case 0x4:  /* read energies */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Read_Energies(file_name, User_data, ModNum);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7004);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read event energies, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x74);
					}

					break;

				case 0x5:  /* read PSA values */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Read_Event_PSA(file_name, User_data, ModNum);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7005);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read PSA values, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x75);
					}

					break;

					// XENON modification
				case 0x6:  /* read extended PSA values */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Read_Long_Event_PSA(file_name, User_data, ModNum);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7006);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read extended PSA values, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x76);
					}

					break;

        		case 0x7:  /* locate events */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Locate_List_Mode_Events(file_name, User_data);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7007);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to locate list mode traces, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x77);
					}

					break;

        		case 0x8:  /* read events */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_Read_List_Mode_Events(file_name, User_data);
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_Event_Browser(file_name, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read list mode traces, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x78);
					}

					break;

				case 0x9:  // full header info in runtype 0x402
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7009);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read list mode data, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x79);
					}

					break;
					
				case 0x10:  /* user reader function for arbitrary processing (or file conversion) */
					if (ListFileVariant == P4_LIST_FILE) retval=Pixie_User_List_Mode_Reader(file_name, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read list mode data, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x79);
					}
					if (ListFileVariant == P500E_LIST_FILE) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid list mode parse analysis request for P4/500e, Run Type=%d", Run_Type);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x79);
					}

					break;

				case 0x20:  /* apply error checking and save corrected file */
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7020);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read list mode data, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x79);
					}

					break;

				case 0x21:  /* file conversion to P4 .bin format */
					if (ListFileVariant == P500E_LIST_FILE) retval=Pixie_List_Mode_Parser(file_name, User_data, 0x7021);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read list mode data, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x79);
					}

					break;



				default:
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid list mode parse analysis request, Run Type=%d", Run_Type);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x79);
			}

			retval=0x70;
			break;

		case 0x8000:  /* Manually read out MCA spectrum from a MCA file */

			/* Check validity of ModNum: can not loop over all */
			if(ModNum == Number_Modules)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid Pixie module number, ModNum=%d", lower, ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-0x82);
			}
			retval = Read_Spectrum_File(ModNum, User_data, file_name);
			if(retval < 0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read out MCA spectrum from the MCA file, retval=%d", retval);
				Pixie_Print_MSG(ErrMSG,1);
				return(-0x81);
			}

			retval=0x80;
			break;

		case 0x9000:  /* Pixie external memory I/O */

			switch(lower)
			{
				case 0x1:  
					/* Read out MCA section of EM */
					retval=Pixie_IOEM(ModNum, HISTOGRAM_MEMORY_ADDRESS, MOD_READ, HISTOGRAM_MEMORY_LENGTH, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read out MCA section of external memory, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x91);
					}
					
					break;

				case 0x2:
					/* debug: write to MCA section of EM */
					retval=Pixie_IOEM(ModNum, HISTOGRAM_MEMORY_ADDRESS, MOD_WRITE, HISTOGRAM_MEMORY_LENGTH, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to write to MCA section of external memory, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x92);
					}

					break;

				case 0x3:
					/* debug: read LM section of EM */
					retval=Pixie_IOEM(ModNum, LIST_MEMORY_ADDRESS, MOD_READ, LIST_MEMORY_LENGTH, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to read out LM section of external memory, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x93);
					}

					break;

				case 0x4:	
					/* debug: write to LM section of EM */
					retval=Pixie_IOEM(ModNum, LIST_MEMORY_ADDRESS, MOD_WRITE, LIST_MEMORY_LENGTH, User_data);
					if(retval < 0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to write to LM section of external memory, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0x94);
					}

					break;
					
				default:
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid external memory I/O request, Run Type=%d", Run_Type);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0x95);
			}
			break;
			
		case 0xA000:		// special functions
			switch(lower)
			{
				case 0x0:
				case 0x1:  /* poll, write, resume a data run */
				case 0x2:
				case 0x3:
									
					retval = Read_Resume_Run(ModNum, (U8)lower, file_name); /* poll, write, resume */
					if(retval < 0)  /* failure to resume run in all modules */
					{
						/* need to stop the run in case some modules started the run OK */
						End_Run(Number_Modules);
						sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): failure to stop modules, retval=%d", retval);
						Pixie_Print_MSG(ErrMSG,1);
						return(-0xA0 + retval);
					}
					
					retval=0xA0+retval;
					break;

           case 0x10:
			   // Moved to 0x4400 and 0x4401
			   sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): use 0x4400 or 0x4401 for Pixie-500e polling");
			   Pixie_Print_MSG(ErrMSG,1);

				break;
		

				default:
					sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid run request, Run Type=%x", Run_Type);
					Pixie_Print_MSG(ErrMSG,1);
					return(-0xAF);
			}
			break;
			
		default:
			sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_Data): invalid run type, Run Type=%x", Run_Type);
			Pixie_Print_MSG(ErrMSG,1);
			return(-0xA0);
	}

	return(retval);

}


/****************************************************************
 *	Pixie_Set_Current_ModChan function:
 *		Set the current Module number and Channel number.
 *
 *		Return Value:
 *			 0 if successful
 *			-1 invalid module number
 *			-2 invalid channel number
 *
 ****************************************************************/

S32 Pixie_Set_Current_ModChan (
			U8 Module,			// module name
			U8 Channel )		// channel name
{
	U16 idx;
	U32 value;

	

	if (Module >= Number_Modules)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Set_Current_ModChan): invalid module number, ModNum=%d", Module);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	if(Channel >= NUMBER_OF_CHANNELS)	// This is a NUMBER_OF_CHANNELS-channel device
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Set_Current_ModChan): invalid channel number, ChanNum=%d", Channel);
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}

	Chosen_Chan = Channel;
	Chosen_Module = Module;

	/* Update ChanNum in DSP */
	idx = Find_Xact_Match("CHANNUM", DSP_Parameter_Names, N_DSP_PAR);
	Pixie_Devices[Chosen_Module].DSP_Parameter_Values[idx] = Chosen_Chan;

	/* Download to the data memory */
	value = (U32)Pixie_Devices[Chosen_Module].DSP_Parameter_Values[idx] ;
	Pixie_IODM(Chosen_Module, (DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	return(0);
}


/****************************************************************
 *	Pixie_Buffer_IO function:
 *	  Download or upload I/O parameter values from or to the host.
 *    Valid for P4/500/500e/P4e using IODM for DSP memory
 *
 *		Direction:	0 (write), 1 (read)
 *		Type:     	0 (DSP I/O parameters)
 *					1 (All DSP variables)
 *					2 (Settings file I/O)
 *					3 (Copy or extract Settings)
 *					4 (Set the address and length of I/O buffer
 *					   for writing)
 *		Values:		unsigned short array
 *
 *		Return Value:
 *			 0 - Success
 *			-1 - failure to set DACs after writing DSP parameters
 *			-2 - failure to program Fippi after writing DSP parameters
 *			-3 - failure to set DACs after loading DSP parameters
 *			-4 - failure to program Fippi after loading DSP parameters
 *			-5 - can't open settings file for loading
 *			-6 - can't open settings file for reading
 *			-7 - can't open settings file to extract settings
 *			-8 - failure to set DACs after copying or extracting settings
 *			-9 - failure to program Fippi after copying or extracting settings
 *		   -10 - invalid module number
 *		   -11 - invalid I/O direction
 *		   -12 - invalid I/O type
 *
 ****************************************************************/

S32 Pixie_Buffer_IO (
			U16 *Values,		// an array hold the data for I/O
			U8 type,			// I/O type
			U8 direction,		// I/O direction
			S8 *file_name,		// file name
			U8 ModNum)			// number of the module to work on
{
	U32 buffer[DATA_MEMORY_LENGTH];
	U16 len, i, j, DSPIOValues[N_DSP_PAR], idx, value16;
	S8  filnam[MAX_FILE_NAME_LENGTH];
	FILE *settingsFile = NULL;
	S32 retval;

	U16 SYSTEM_CLOCK_MHZ = P4_SYSTEM_CLOCK_MHZ;	// initialize ot Pixie-4 default
	U16 FILTER_CLOCK_MHZ = P4_FILTER_CLOCK_MHZ;
	U16	ADC_CLOCK_MHZ = P4_ADC_CLOCK_MHZ;
	U16	DSP_CLOCK_MHZ = P4_DSP_CLOCK_MHZ;
	U16	CTscale =P4_CTSCALE;			// The scaling factor for count time counters

	/* Check the validity of Modnum */
	if (ModNum >= Number_Modules)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid Module number %d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-10);
	}

	/* Ensure file name length not greater than the maximum length */
	len = (U16)MIN(strlen(file_name), MAX_FILE_NAME_LENGTH);
	strncpy(filnam, file_name, len);
	filnam[len]=0;

	if(type==0)  /* DSP I/O parameter values */
	{
		if(direction==1)  /* Read */
		{
			if(Offline == 1)  /* Load previously saved parameter values for offline analysis */
			{
				for(i=0; i<N_DSP_PAR; i++)
				{
					*Values++ = Pixie_Devices[ModNum].DSP_Parameter_Values[i];
				}

				return(0);
			}

			/* Read out all the DSP I/O parameters */
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_READ, N_DSP_PAR, buffer);

			for(i=0; i<N_DSP_PAR; i++)
			{
				*Values++ = (U16) buffer[i];
				Pixie_Devices[ModNum].DSP_Parameter_Values[i] = (U16) buffer[i];
			}
		}
		else if(direction==0)  /* Write */
		{
			/* Load DSP values into DSP_Parameter_Values */
			for(i=0; i<DSP_IO_BORDER; i++)
			{
				Pixie_Devices[ModNum].DSP_Parameter_Values[i] = *Values;
				buffer[i] = (U32)*Values++;
			}
			if (Offline == 1) return (0); //No I/O in offline mode
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);
			retval = Control_Task_Run(ModNum, SET_DACS, 10000);
			if(retval<0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to set DACs in Module %d after writing DSP parameters.", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}

			retval = Control_Task_Run(ModNum, PROGRAM_FIPPI, 1000);
			if(retval<0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to program Fippi in Module %d after writing DSP parameters.", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-2);
			}
		}
		else if(direction==2)  /* Write, but do not apply */
		{
			sprintf(ErrMSG, "*INFO* (Pixie_Buffer_IO): write only.");
			Pixie_Print_MSG(ErrMSG,1);
			/* Load DSP values into DSP_Parameter_Values */
			for(i=0; i<DSP_IO_BORDER; i++)
			{
				Pixie_Devices[ModNum].DSP_Parameter_Values[i] = *Values;
				buffer[i] = (U32)*Values++;
			}
			if (Offline == 1) return (0);
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);
		}
		else
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid I/O direction %d", direction);
			Pixie_Print_MSG(ErrMSG,1);
			return(-11);
		}
	}
	else if(type==1)  /* All DSP variables, but read only */
	{
		if(direction==1)  /* Read */
		{
			if(Offline == 1)  /* Returns immediately for offline analysis */
			{
				return(0);
			}
			
			/* Read out all the DSP I/O parameters */
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_READ, DATA_MEMORY_LENGTH, buffer);
			for(i=0; i<N_MEM_PAR; i++)
			{
				*Values++ = (U16)buffer[i];
				//Pixie_Devices[ModNum].DSP_Memory_Values[i] = (U16) buffer[i];
			}

		}
		else if(direction==0)  /* Write */
		{
			/* Skip the first Writing_IOBuffer_Address words of the DSP data memory buffer */
			Values += Writing_IOBuffer_Address;

			/* Load DSP values into DSP_Parameter_Values */
			for(i=0; i<Writing_IOBuffer_Length; i++)
			{
				buffer[i] = (U32)*Values++;
			}
			if (Offline == 1) return (0);
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+Writing_IOBuffer_Address, MOD_WRITE, Writing_IOBuffer_Length, buffer);
		}
		else
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid I/O direction %d", direction);
			Pixie_Print_MSG(ErrMSG,1);
			return(-11);
		}
	}
	else if(type==2)  /* Settings file I/O */
	{
		if(direction==1)  /* Load */
		{

			settingsFile = fopen(filnam, "rb");
			if(settingsFile != NULL)
			{
				for(i=0;i<PRESET_MAX_MODULES;i++)
				{
					fread(Pixie_Devices[i].DSP_Parameter_Values, 2, N_DSP_PAR, settingsFile);
				}
				fclose(settingsFile);
				
				// update Ccontrol options stored in settings file (one of 3 places)
				idx = Find_Xact_Match("CCONTROL", DSP_Parameter_Names, N_DSP_PAR);
				value16 = Pixie_Devices[0].DSP_Parameter_Values[idx];
				PrintDebugMsg_Boot	  = TstBit(4,value16);		// if 1, print debug messages during booting
				PrintDebugMsg_QCerror = TstBit(5,value16);		// if 1, print error debug messages during LM buffer quality check
				PrintDebugMsg_QCdetail= TstBit(6,value16);		// if 1, print detail debug messages during LM buffer quality check
				PrintDebugMsg_other	  = TstBit(7,value16);		// if 1, print other debug messages 
				Polling				= TstBit(8,value16);		// if 1, use (old style) polling of module in LM runs, if 0 use intertupts from module  
				BufferQC			= TstBit(9,value16);		// if 1, execute data quality control check before writing LM data to file
				//KeepCW			= tstbit(10,value16);		// To control update and enforced minimum of coincidence wait
				PollForNewData		= TstBit(11,value16);		// if 1, return new data in DMA buffer during polling
				MultiThreadDAQ		= TstBit(12,value16);		// if 1, run 0x400, 0x301, and 0x10x as a separate thread

				if (Offline == 1) // no further update in offline mode
					return (0);
                
				/* Find the index of FILTERRANGE in the DSP; energy filter interval needs update */
        		idx=Find_Xact_Match("FILTERRANGE", DSP_Parameter_Names, N_DSP_PAR);

                for(i=0;i<Number_Modules;i++)
				{
					/* Convert data type */
					for(j=0; j<DSP_IO_BORDER; j++)
					{
						buffer[j] = (U32)Pixie_Devices[i].DSP_Parameter_Values[j];
					}

					/* Download new settings into the module */
					Pixie_IODM((U8)i, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);
					retval = Control_Task_Run((U8)i, SET_DACS, 10000);
					if(retval<0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to set DACs in Module %d after loading DSP parameters.", i);
						Pixie_Print_MSG(ErrMSG,1);
						return(-3);
					}

					retval = Control_Task_Run((U8)i, PROGRAM_FIPPI, 1000);
					if(retval<0)
					{
						sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to program Fippi in Module %d after loading DSP parameters.", i);
						Pixie_Print_MSG(ErrMSG,1);
						return(-4);
					}

					// Define clock constants according to BoardRevision 
					Pixie_Define_Clocks ((U8)i,(U8)0,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale,&DSP_CLOCK_MHZ );


                    /* Update energy filter interval */
                    Filter_Int[i]=pow(2.0, (double)Pixie_Devices[i].DSP_Parameter_Values[idx])/FILTER_CLOCK_MHZ;
				}
			}
			else
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): can't open settings file %s for loading", filnam);
				Pixie_Print_MSG(ErrMSG,1);
				return(-5);
			}
		}
		else if(direction==0)  /* Save */
		{
			settingsFile = fopen(filnam, "wb");
			if(settingsFile != NULL)
			{
				/* Read out the DSP I/O parameters for all the Pixie modules in the system */
				if (Offline == 0) {
					for(i=0; i<Number_Modules; i++)
					{
						Pixie_IODM((U8)i, DATA_MEMORY_ADDRESS, MOD_READ, N_DSP_PAR, buffer);
					
						for(j=0; j<N_DSP_PAR; j++)
						{
							Pixie_Devices[i].DSP_Parameter_Values[j] = (U16)buffer[j];
						}
					}
				}
				for(i=0; i<PRESET_MAX_MODULES; i++)
				{
					fwrite(Pixie_Devices[i].DSP_Parameter_Values, 2, N_DSP_PAR, settingsFile);
				}

				fclose(settingsFile);
			}
			else
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): can't open settings file %s for reading", filnam);
				Pixie_Print_MSG(ErrMSG,1);
				return(-6);
			}
		}
		else
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid I/O direction %d", direction);
			Pixie_Print_MSG(ErrMSG,1);
			return(-11);
		}
	}
	else if(type==3)  /* Copy or extract settings */
	{
		if(direction==1)  /* Copy */
		{
			/* Load settings in source module and channel to DSPIOValues */
			for(i=0;i<N_DSP_PAR;i++)
			{
				DSPIOValues[i] = Pixie_Devices[Values[0]].DSP_Parameter_Values[i];
			}
		}
		else if(direction==0)  /* Extract */
		{
			/* Open the settings file */
			settingsFile = fopen(filnam, "rb");
			if(settingsFile != NULL)
			{
				/* Position settingsFile to the source module location */
				Pixie_fseek(settingsFile, Values[0]*N_DSP_PAR*2, SEEK_SET);
				fread(DSPIOValues, 2, N_DSP_PAR, settingsFile);
				fclose(settingsFile);
			}
			else
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): can't open settings file %s to extract settings", filnam);
				Pixie_Print_MSG(ErrMSG,1);
				return(-7);
			}
		}
		else
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid I/O direction %d", direction);
			Pixie_Print_MSG(ErrMSG,1);
			return(-11);
		}

		/* Copy settings to the destination modules and channels */
		for(i=0;i<Number_Modules;i++)
		{
			for(j=0;j<NUMBER_OF_CHANNELS;j++)
			{
				if( TstBit(j, Values[i+3]) )
				{
					Pixie_CopyExtractSettings((U8)Values[1], Values[2], (U8)i, (U8)j, DSPIOValues);
				}
			}
		}
		if (Offline == 1) return (0);
		/* Download new settings into each module present in the system */
		for(i=0;i<Number_Modules;i++)
		{			
			for(j=0; j<DSP_IO_BORDER; j++)
			{
				buffer[j] = (U32)Pixie_Devices[i].DSP_Parameter_Values[j];
			}
			Pixie_IODM((U8)i, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);

			retval = Control_Task_Run((U8)i, SET_DACS, 10000);
			if(retval<0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to set DACs in Module %d after copying or extracting settings.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(-8);
			}

			retval = Control_Task_Run((U8)i, PROGRAM_FIPPI, 1000);
			if(retval<0)
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): failure to program Fippi in Module %d after copying or extracting settings.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(-9);
			}
		}
	}
	else if(type==4)	/* Set the address and length of I/O buffer for writing */
	{
		Writing_IOBuffer_Address = *Values++;
		Writing_IOBuffer_Length = *Values;
	}

	else
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Buffer_IO): invalid I/O type %d", type);
		Pixie_Print_MSG(ErrMSG,1);
		return(-12);
	}

	return(0);
}


S16 Pixie_Get_Par_Idx (S8 *ParName, S8 *ParType)
{
	S16 Idx = -1;
	if(!strcmp(ParType, "SYSTEM"))  Idx = Find_Xact_Match(ParName, System_Parameter_Names, N_SYSTEM_PAR);
	if(!strcmp(ParType, "MODULE"))  Idx = Find_Xact_Match(ParName, Module_Parameter_Names, N_MODULE_PAR);
	if(!strcmp(ParType, "CHANNEL")) Idx = Find_Xact_Match(ParName, Channel_Parameter_Names, N_CHANNEL_PAR);
	if (Idx == -1)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Idx): invalid parameter type %s", ParType);
		Pixie_Print_MSG(ErrMSG,1);
		return (-1);
	}
	if (Idx == (N_SYSTEM_PAR - 1))
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Idx): invalid system parameter name %s", ParName);
		Pixie_Print_MSG(ErrMSG,1);
		return (-2);
	}
	if (Idx == (N_MODULE_PAR - 1)) 
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Idx): invalid module parameter name %s", ParName);
		Pixie_Print_MSG(ErrMSG,1);
		return (-3);
	}
	if (Idx == (N_CHANNEL_PAR - 1)) 
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Idx): invalid channel parameter name %s", ParName);
		Pixie_Print_MSG(ErrMSG,1);
		return (-4);
	}
	return (Idx);
}


S16 Pixie_Get_Par_Name (U16 Idx, S8 *ParType, S8 *ParName)
{
	if(!ParName)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Name): NULL parameter name array");
		Pixie_Print_MSG(ErrMSG,1);
		return (-5);
	}
	if(!strcmp(ParType, "SYSTEM"))
	{
		if (Idx >= 0 && Idx < N_SYSTEM_PAR)
		{
			strcpy(ParName, System_Parameter_Names[Idx]);
			return (0);
		}
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Name): invalid system parameter index %s", Idx);
		Pixie_Print_MSG(ErrMSG,1);
		return (-1);
	}
	if(!strcmp(ParType, "MODULE"))
	{
		if (Idx >= 0 && Idx < N_MODULE_PAR)
		{
			strcpy(ParName, Module_Parameter_Names[Idx]);
			return (0);
		}
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Name): invalid module parameter index %s", Idx);
		Pixie_Print_MSG(ErrMSG,1);
		return (-2);
	}
	if(!strcmp(ParType, "CHANNEL"))
	{
		if (Idx >= 0 && Idx < N_CHANNEL_PAR)
		{
			strcpy(ParName, Channel_Parameter_Names[Idx]);
			return (0);
		}
		sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Name): invalid channel parameter index %s", Idx);
		Pixie_Print_MSG(ErrMSG,1);
		return (-3);
	}
	sprintf(ErrMSG, "*ERROR* (Pixie_Get_Par_Idx): invalid parameter type %s", ParType);
	Pixie_Print_MSG(ErrMSG,1);
	return (-4);
}


