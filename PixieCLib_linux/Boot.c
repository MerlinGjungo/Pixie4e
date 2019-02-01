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
*      Boot.c
*
* Description:
*
*      This file contains all the funtions used to boot Pixie modules.
*
* Revision:
*
*		11-30-2004
*
* Member functions:
*
*		Pixie_Init_VarNames
*		Load_Names
*		Load_U16
*		Pixie_Boot
*		Pixie_Boot_ComFPGA
*		Pixie_Boot_FIPPI

*		Pixie_Boot_DSP
*		Pixie_Scan_Crate_Slots
*		Pixie_Close_PCI_Devices
*		Pixie_Init_Globals
*
******************************************************************************/

#include "globals.h"
#include <time.h>

// PXI system initialization arrays replacing the pxisys.ini files

U8  PCIBusNum_18Slots_ini[PRESET_MAX_PXI_SLOTS+1]    = {0, 0, 1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3};
U8  PCIBusNum_14Slots_ini[PRESET_MAX_PXI_SLOTS+1]    = {0, 0, 1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0};
U8  PCIBusNum_8Slots_ini [PRESET_MAX_PXI_SLOTS+1]    = {0, 0, 1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
U8  PCIBusNum_1062_ini [PRESET_MAX_PXI_SLOTS+1]      = {0, 0, 1,  1,  1,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
U8  PCIDeviceNum_18Slots_ini[PRESET_MAX_PXI_SLOTS+1] = {0, 0, 15, 14, 13, 11, 10, 15, 14, 13, 11, 10, 9,  15, 14, 13, 12, 11, 10};		// slot 5 is not a PCI slot
U8  PCIDeviceNum_14Slots_ini[PRESET_MAX_PXI_SLOTS+1] = {0, 0, 15, 14, 13, 12, 11, 10, 8,  15, 14, 13, 12, 11, 10, 0,  0,  0,  0}; 
U8  PCIDeviceNum_8Slots_ini [PRESET_MAX_PXI_SLOTS+1] = {0, 0, 15, 14, 13, 12, 11, 10, 9,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
U8  PCIDeviceNum_1062_ini [PRESET_MAX_PXI_SLOTS+1]   = {0, 0, 15, 14, 13, 13, 12, 11, 10, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

/****************************************************************
*	Pixie_Init_VarNames:
*		Initialize DSP I/O parameter names and DSP internal memory
*		parameter names from files, namely, DSPcode.var and DSPcode.lst
*		file, by calling function Load_Names.
*
*		Return Value:
*			 0 - load successful
*			-1 - file not found
*			-2 - requested number of names to be read exceeds limits
*			-3 - premature EOF encountered
*
*****************************************************************/

S32 Pixie_Init_VarNames(void)
{
	S32   retval;
	/* DSP I/O parameter names */
	// i.e. reading *.var file
	retval = Load_Names(Boot_File_Name_List[5], DSP_PARA_NAM, N_DSP_PAR, MAX_PAR_NAME_LENGTH);
	if( retval == -1 )
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Init_VarNames): DSP I/O parameter file %s was not found", Boot_File_Name_List[5]); 
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	else if( retval == -2 )
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Init_VarNames): Requested number of DSP I/O parameter names to be read exceeds limits"); 
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}
	else if( retval == -3 )
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Init_VarNames): Premature EOF encountered when read DSP I/O parameter file %s", Boot_File_Name_List[5]); 
		Pixie_Print_MSG(ErrMSG,1);
		return(-3);
	}

	/* DSP internal memory parameter names; currently not being used. */
	/*
	retval = Load_Names(Boot_File_Name_List[6], DSP_MEM_NAM, N_MEM_PAR, MAX_MEM_NAME_LENGTH);
	if( retval == -1 ){
	sprintf(ErrMSG, "*ERROR* (Pixie_Init_VarNames): DSP all variable file %s was not found", Boot_File_Name_List[6]); 
	Pixie_Print_MSG(ErrMSG,1);
	return(-1);
	}
	else if( retval == -2 ){
	sprintf(ErrMSG, "*ERROR* (Pixie_Init_VarNames): Requested number of DSP internal memory parameter names to be read exceeds limits"); 
	Pixie_Print_MSG(ErrMSG,1);
	return(-2);
	}
	If ever used, need to add equivalent for Pixie-500e memory names,Boot_File_Name_List[12] 
	*/

	return 0;
}


/****************************************************************
*	Load_Names:
*		Receive DSP I/O parameter names and and DSP internal memory
*		parameter names from files, namely, DSPcode.var and DSPcode.lst
*		file.
*
*		Return Value:
*			 0 - load successful
*			-1 - file not found
*			-2 - requested number of names to be read exceeds limits
*			-3 - premature EOF encountered
*
****************************************************************/

S32 Load_Names (
				S8 *filnam,			// file name
				U8 ConfigName,		// determine which type of name to load
				U32 nNames,			// number of names to be loaded
				U16 Name_Length)	// the limit of the name length
{
	U16  k, len;
	S32  retval=0;
	U8   i;

	S8	 DSPParaName[MAX_PAR_NAME_LENGTH];

	FILE* namesFile = NULL;
	namesFile = fopen(filnam, "r");

	if(namesFile != NULL)
	{
		switch(ConfigName)
		{

		case DSP_PARA_NAM:  /* DSP parameter names */

			/* Check if nNames is within limits of N_DSP_PAR */
			if(nNames > N_DSP_PAR)
			{
				retval = -2;
				break;
			}

			for(k=0; k<nNames; k++)
			{
				strcpy(DSP_Parameter_Names[k],"");  /* Clear all names */
			}

			for(k=0; k<nNames; k++)  /* Read names */
			{
				if( fgets(DSPParaName, Name_Length, namesFile) != NULL )  /* Read one line */
				{
					len=(U16)strlen(DSPParaName);  /*  Get the line length */
					if(len>6)  /* Check if the line contains real names; the index holds 5 spaces */
					{
						for(i=0;i<(len-5);i++)
						{
							DSP_Parameter_Names[k][i]=DSPParaName[i+5];
						}

						DSP_Parameter_Names[k][len-6]=0;  /* Remove the new line character at the end of the name */
					}
				}
				else  /* premature EOF encountered */
				{
					retval=-3;
					break;
				}
			}
			retval=0;
			break;

			/* Read DSP internal memory parameter names; currently not being used. */
			/*
			case DSP_MEM_NAM:	// All DSP variable names

			// Check if nNames is within limits of N_MEM_PAR
			if(nNames > N_MEM_PAR)
			{
			retval = -2;
			return(retval);
			}

			for(k=0; k<nNames; k++)
			{
			strcpy(DSP_Memory_Names[k],"");  // Clear all names
			}


			do  // Read names
			{					
			if( fgets(DSPParaName, Name_Length, namesFile) != NULL )  // Read one line
			{
			for(i=0; i<5; i++)
			{
			NameConv[i]=DSPParaName[i];  // Get the name index
			}

			idx=atoi(NameConv);	 // Convert the index into numerical value
			len=strlen(DSPParaName); // Get the line length

			for(i=0;i<(len-7);i++)
			{
			DSP_Memory_Names[idx][i]=DSPParaName[i+7];
			}

			DSP_Memory_Names[idx][len-8]=0;  // Remove the new line character at the end of the name

			}
			else  // The end of file was reached
			break;
			}while(1);
			retval=0;
			break;
			*/
		}

		fclose(namesFile);  // Close file
		return(retval);		
	}
	else
	{
		return(-1);  // File not found
	}
}


/****************************************************************
*	Load_U16:
*		Load communication FPGA configuration, FIPPI configuration,
*		DSP code, and DSP I/O variable values from files in unsigned
*		16-bit format.
*	
*		Return Value:
*			 0 - load successful
*			-1 - file not found
*			-2 - requested number of values to be read exceeds limits
*			-3 - reading incomplete
*
****************************************************************/

S32 Load_U16 ( S8 *filnam,			// file name
			   U8 ConfigName,		// determine which type of parameter to load
			   U32 ndat)			// number of data words to load
{

	U32   dummy[N_DSP_CODE_BYTES/4], nWords;
	S32   retval=0;
	U16   k;
	FILE *dataFile = NULL;
	dataFile = fopen(filnam, "rb");
	if(dataFile != NULL) {	
		switch(ConfigName) {		
		case COM_FPGA_CONFIG_REV_B:	/* Communication FPGA configuration (P4 Rev. B) */
			nWords = fread(ComFPGA_Configuration_P4_RevB, 4, (N_COMFPGA_BYTES/4), dataFile);
			if(((N_COMFPGA_BYTES/4) - nWords) > 1) {
				/* ndat differing from nWords by 1 is OK if N_COMFPGA_BYTES is an odd number */
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading communication FPGA configuration (Rev. B) incomplete.");
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case COM_FPGA_CONFIG_REV_C:	/* Communication FPGA configuration (P4 Rev. C-E) */
			nWords = fread(ComFPGA_Configuration_P4_RevC, 4, (N_COMFPGA_BYTES/4), dataFile);
			if(((N_COMFPGA_BYTES/4) - nWords) > 1) {
				/* ndat differing from nWords by 1 is OK if N_COMFPGA_BYTES is an odd number */
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading communication FPGA configuration (Rev. C) incomplete.");
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case COM_FPGA_CONFIG_P500_REV_B:	/* Communication FPGA configuration (P4 Rev. C-E) */
			nWords = fread(ComFPGA_Configuration_P500_RevB, 4, (N_COMFPGA_BYTES/4), dataFile);
			if(((N_COMFPGA_BYTES/4) - nWords) > 1) {
				/* ndat differing from nWords by 1 is OK if N_COMFPGA_BYTES is an odd number */
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading communication FPGA configuration (Rev. C) incomplete.");
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case FIPPI_CONFIG:	/* FIPPI configuration */
			nWords = fread(FIPPI_Configuration, 4, (N_FIPPI_BYTES/4), dataFile);
			if(((N_FIPPI_BYTES/4) - nWords) > 1) {
				/* ndat differing from nWords by 1 is OK if N_FIPPI_BYTES is an odd number */
				sprintf(ErrMSG, "*ERROR* (Load_U16): reading FIPPI configuration in Module %d incomplete.", Chosen_Module);
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case P4E_FPGA_CONFIG:	/* P4e FPGA configuration (general) */
			nWords = fread(P4e_FPGA_Configuration, 2, (N_P4E_BYTES/2), dataFile);
			if (nWords < N_P4E_BYTES/2) {
				sprintf(ErrMSG, "*ERROR* (Load_U16): reading P4e FPGA code in Module %d incomplete.", Chosen_Module);
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case P4E14500_FPGA_CONFIG:	/* P4e FPGA configuration (14/500) */
			nWords = fread(P4e14500_FPGA_Configuration, 2, (N_P4E_BYTES/2), dataFile);
			if (nWords < N_P4E_BYTES/2) {
				sprintf(ErrMSG, "*ERROR* (Load_U16): reading P4e FPGA code in Module %d incomplete.", Chosen_Module);
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break; 		
		case P500E_FPGA_CONFIG:	/* P500E FPGA configuration */
			nWords = fread(P500e_FPGA_Configuration, 2, (N_P500E_BYTES/2), dataFile);
			if (nWords < N_P500E_BYTES/2) {
				sprintf(ErrMSG, "*ERROR* (Load_U16): reading P500e FPGA code in Module %d incomplete, %u %d", Chosen_Module, nWords, N_P500E_BYTES/2);
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}         
			break; 
		case P500E_DSP_CODE:	/* P500e DSP code */
			Pixie_fseek(dataFile, 0, SEEK_END);
			P500E_DSP_CODE_BYTES = Pixie_ftell(dataFile); /* This is a global variable due to variable code size of SHARC DSPs */
			rewind(dataFile);
			ndat = fread(P500e_DSP_Code, 1, P500E_DSP_CODE_BYTES, dataFile);
			if(ndat < P500E_DSP_CODE_BYTES) {
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading DSP code incomplete. %u  %u", ndat, P500E_DSP_CODE_BYTES);
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			break;
		case DSP_CODE:	/* DSP code P4*/
			nWords = fread(dummy, 4, (N_DSP_CODE_BYTES/4), dataFile);
			if( nWords < (N_DSP_CODE_BYTES/4) ) {
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading P4 DSP code incomplete.");
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			else {
				for(k=0;k<(N_DSP_CODE_BYTES/4);k++) {
					DSP_Code_P4[2*k+1]= (U16)(dummy[k] & 0xFF);
					DSP_Code_P4[2*k]= (U16)(dummy[k]>>8);
				}
				retval = 0;
			}
			break;
		case P500_DSP_CODE:	/* DSP code P500*/
			nWords = fread(dummy, 4, (N_DSP_CODE_BYTES/4), dataFile);
			if( nWords < (N_DSP_CODE_BYTES/4) ) {
				sprintf(ErrMSG,"*ERROR* (Load_U16): reading P500 DSP code incomplete.");
				Pixie_Print_MSG(ErrMSG,1);
				retval = -3;
			}
			else {
				for(k=0;k<(N_DSP_CODE_BYTES/4);k++) {
					DSP_Code_P500[2*k+1]= (U16)(dummy[k] & 0xFF);
					DSP_Code_P500[2*k]= (U16)(dummy[k]>>8);
				}
				retval = 0;
			}
			break;
		case DSP_PARA_VAL:	/* DSP parameter values */
			/* Read DSP parameter values module by module from the settings file */
			for(k=0; k<PRESET_MAX_MODULES; k++) {
				nWords = fread(Pixie_Devices[k].DSP_Parameter_Values, 2, (N_DSP_PAR), dataFile);
				if( nWords < (N_DSP_PAR) ) {
					sprintf(ErrMSG, "*ERROR* (Load_U16): reading DSP parameter values incomplete.");
					Pixie_Print_MSG(ErrMSG,1);
					retval = -3;
					break;
				}
				/* Force the correct Module Number in the DSP Parameter Values.*/
				/* This is necessary since the wrong module number in the      */
				/* settings file could screw up the operation of that module   */
				/* without being easily detected.                              */
				Pixie_Devices[k].DSP_Parameter_Values[0] = k;
			}
			retval = 0;
			break;
		}

		fclose(dataFile);	/* Close file */
		sprintf(ErrMSG, "*INFO* (Load_U16): finished loading file %s", filnam);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		return(retval);		
	}
	else {
		sprintf(ErrMSG, "*ERROR* (Load_U16): file %s was not found", filnam);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);		/* File not found */
	}
}


/****************************************************************
*	Pixie_Boot:
*		Boot the Pixie according the Boot_Pattern.
*		Boot_Pattern is a bit mask:
*			bit 0:	Boot ComFPGA
*			bit 1:	Boot FIPPI
*			bit 2:	Boot DSP
*			bit 3:	Load DSP parameters
*			bit 4:	Apply DSP parameters (calls Set_DACs
*					and Program_FIPPI)
*
*		Return Value:
*			 0 - boot successful
*			-81 - downloading communication FPGA failed
*			-82 - downloading FiPPI configuration failed
*			-83 - downloading DSP code failed
*			-84 - failed to set DACs
*			-85 - failed to program FiPPI
*			-86 - failed to enable detector input
*			-87 - incorrect boot pattern
*
****************************************************************/

S32 Pixie_Boot (U16 Boot_Pattern)		// boot pattern is a bit mask
{

	S32 retval = -2;
	U16 k;
	U32 buffer[DATA_MEMORY_LENGTH];
	U8  modulenumber, i;
	U16	BoardRevision;

	U32 dwData,dwStatus;
	U16 Cversion, Cbuild;

	time_t current_time;
	char* c_time_string;

	Cversion  = C_LIBRARY_RELEASE;				// optionally print time and version number (useful for pixie.msg from dll)
	Cbuild = C_LIBRARY_BUILD;			
	current_time = time(NULL);					// Obtain current time as seconds elapsed since the Epoch.	
	c_time_string = ctime(&current_time);		// Convert to local time format. 
	sprintf(ErrMSG, "*INFO* (Pixie_Boot): Pixie API version %0X build %0X, booting at %s",Cversion, Cbuild, c_time_string);
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
	
	if(Offline == 1) return(0);  /* Returns immediately for offline analysis */
	if(!Boot_Pattern){
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): 0x0 boot pattern, doing nothing");
		Pixie_Print_MSG(ErrMSG,1);
		return (0);
	}
	if(Boot_Pattern > 0x1F){
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot): incorrect boot pattern: %hu", Boot_Pattern);
		Pixie_Print_MSG(ErrMSG,1);
		return (BOOT_PATTERN_ERR);
	}

	if (PCIBusType == REGULAR_PCI) {
		// **************************************************************
		//	Set up pullup resisters (only one module should be set the
		//	pullup resisters to 1, but this module could be any module in
		//	in the system)
		// **************************************************************
		buffer[0] = 0x3F;	/* Connect pullup resisters on FT, ET, Veto, Time, Sync, and TRreturn */
		modulenumber = 0;	/* Here we set Module #0 */
		Pixie_Register_IO(modulenumber, PCI_PULLUP, MOD_WRITE, buffer);
		/* For all other modules, we disconnect pullup resisters */
		for(i=1; i<Number_Modules; i++) {
			buffer[0] = 0x00;	// Disconnect pullup resisters on FT, ET, Veto, Time, Sync, and TRreturn 
			Pixie_Register_IO(i, PCI_PULLUP, MOD_WRITE, buffer);
		}
	}

	// **************************************************************
	// Download communication FPGA
	// **************************************************************
	if((Boot_Pattern & 0x1) && (PCIBusType == REGULAR_PCI)) {  // no comm FPGA for express modules
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): Begin booting ComFPGA");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		// Boot communication FPGA 
		for(i=0; i<Number_Modules; i++) {
			if(Pixie_Boot_ComFPGA(i) < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): downloading communication FPGA configuration to module %d was not successful.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(BOOT_SYS_ERR);
			}
		}
	}

	// **************************************************************
	// Download FIPPI
	// **************************************************************
	if(Boot_Pattern & 0x2) {
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): Begin booting FIPPI");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		for(i=0; i<Number_Modules; i++) {
			// Get Board Revision 
			BoardRevision = (U16)Pixie_Devices[i].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];
			sprintf(ErrMSG, "*INFO* (Pixie_Boot): Revision 0x%X", BoardRevision);
			Pixie_Print_MSG(ErrMSG,1);
			if((BoardRevision & 0x0F00) == MODULETYPE_P4 )  retval=Pixie_Boot_FIPPI(i);  // For Pixie-4: Boot FIPPI 
#ifdef WINDRIVER_API
			retval = PIXIE500E_ProgramFPGA(hDev[i], 2, (BoardRevision & 0x0FF0));
#endif
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): downloading FiPPI configuration to module %d was not successful.", i);
				Pixie_Print_MSG(ErrMSG,1);
				if (retval==REGIO_ERR) {
					sprintf(ErrMSG, "Encountered potentially fatal error communicating to module, PC may lock up.");
					Pixie_Print_MSG(ErrMSG,1);
					sprintf(ErrMSG, "Save all open files now, then try a PLL reset for a chance to recover");
					Pixie_Print_MSG(ErrMSG,1);
				}
				return(BOOT_FIPPI_ERR);
			}
			else {
				sprintf(ErrMSG, "*INFO* (Pixie_Boot): FiPPI configuration in module %d was successful.", i);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
				Pixie_Sleep(1);
			}
		}
	}

	if (PCIBusType == REGULAR_PCI) {
		/* enable LED */
		for(i=0; i<Number_Modules; i++) {
			buffer[0] = 0x38;
			Pixie_Register_IO(i, PCI_CFCTRL, MOD_WRITE, buffer);
		}
	}

	// **************************************************************
	// Download DSP code
	// **************************************************************
	if(Boot_Pattern & 0x4) {
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): Begin booting DSP");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		/* Boot DSP */
		for(i=0; i<Number_Modules; i++) {
			BoardRevision = (U16)Pixie_Devices[i].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];
			if((BoardRevision & 0x0F00) == MODULETYPE_P4 ) retval=Pixie_Boot_DSP(i, MODULETYPE_P4);  		// Boot DSP for Pixie-4 
	#ifdef WINDRIVER_API		
			if((BoardRevision & 0x0F00) == MODULETYPE_P500e ) retval=PIXIE500E_ProgramDSP(hDev[i]); 	// Boot DSP for P500e
			if((BoardRevision & 0x0F00) == MODULETYPE_P4e)    retval=PIXIE500E_ProgramDSP(hDev[i]);     // Boot DSP for P4e (same as P500e) 
	#endif
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): downloading DSP code to module %d was not successful.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(BOOT_DSP_ERR);
			}
		}
	}

	// **************************************************************
	// Download DSP parameter values
	// Pass the 64 Module parameters and 48*4 channel parameters
	// to DSP (DSP_IO_BORDER)
	// **************************************************************
	if((Boot_Pattern & 0x8)) {
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): Begin DSP parameter download");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		/* Download DSP parameter values */
		for(i=0; i<Number_Modules; i++) {
				/* Convert data type */
			for(k=0; k < DSP_IO_BORDER; k++) buffer[k] = (U32)Pixie_Devices[i].DSP_Parameter_Values[k];
			/* Download new settings into the module */
			Pixie_IODM(i, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);
			// NOTE: MUST HAVE Sleep below for Sample.c (otherwise DSP parameters are set with errors)
			Pixie_Sleep(500);
		}
	}

	// **************************************************************
	// Now start control tasks 0 and 5 to set DACs and program FIPPI
	// **************************************************************
	if(Boot_Pattern & 0x10)  {
		sprintf(ErrMSG, "*INFO* (Pixie_Boot): Begin Program FIPPI");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
		for(i=0; i<Number_Modules; i++) {
			retval=Control_Task_Run(i, SET_DACS, 1000); /* Set DACs */
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): Set DACs in module %d failed.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(SET_DAC_ERR);
			}
			retval=Control_Task_Run(i, PROGRAM_FIPPI, 1000); /* Program FiPPI */
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): Program FiPPI in module %d failed.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(PROG_FIPPI_ERR);
			}
			retval=Control_Task_Run(i, ENABLE_INPUT, 1000); /* Connect detector input */
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Boot): Enable detector input in module %d failed.", i);
				Pixie_Print_MSG(ErrMSG,1);
				return(ENA_INTS_ERR);
			}

	/*		BoardRevision = (U16)Pixie_Devices[i].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];
			if((BoardRevision & 0x0F00) == MODULETYPE_P500e ) 
         //TODO: Add P4e, 500 MHZ versions
			{
				retval=Apply_default_I2E(i); // Enable/apply ADC I2E calibration 
				if(retval < 0) {
					sprintf(ErrMSG, "*Warning* (Pixie_Boot): Enabling ADC calibration in module %d failed.", i);
					Pixie_Print_MSG(ErrMSG,1);
				//	return(-7);
				}
			}
			*/
		}
	}

	// Success
	for(i=0; i<Number_Modules; i++) {
		sprintf(ErrMSG, "Module %d in slot %d started up successfully!", i, Phy_Slot_Wave[i]);
		Pixie_Print_MSG(ErrMSG,1);
	}

	return(0);
}


/****************************************************************
*	Pixie_Boot_ComFPGA:
*		Download communication FPGA configuration.
*
*		Return Value:
*			 0 - boot successful
*			-1 - failed to read board version
*			-2 - clearing communication FPGA timed out
*			-3 - downloading communication FPGA timed out
*
****************************************************************/

S32 Pixie_Boot_ComFPGA (
						U8 ModNum )		// Pixie module number
{

	U16	BoardRevision;
	U32	buffer[0x5];
	U32     k;
	U32     counter0;
	U32     counter1;

	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);

	/* Get Board Revision */
	BoardRevision = (U16)Pixie_Devices[ModNum].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];

	sprintf(ErrMSG, "Module %d, Revision = %X", ModNum, BoardRevision);
	Pixie_Print_MSG(ErrMSG,1);

	/* Initialize counter0 to 0 */
	buffer[0] = counter0 = 0;
	/* if buffer[0] = 0x21, download Conf. FPGA successful */
	while ((buffer[0] & 0x21) != 0x21 && counter0 < 10) {
		/* Set communication FPGA Program*=0 to clear it */
		buffer[0] = 0x41; 
		/* Write to an offset from the PCI-to-Local space window */
		Pixie_Register_IO(ModNum, PCI_CFCTRL, MOD_WRITE, buffer);
		/* Set communication FPGA Program*=1 to start configuration */
		buffer[0] = 0x49;
		Pixie_Register_IO(ModNum, PCI_CFCTRL, MOD_WRITE, buffer);
		/* Initialize counter1 to 0. If buffer[0] = 0x20, finished clearing communication FPGA */
		counter1 = 0;
		while ((buffer[0] & 0x21) != 0x20 && counter1 < 100) {
			Pixie_Register_IO(ModNum, PCI_CFSTATUS, MOD_READ, buffer);
			counter1++;
		}
		if (counter1 == 100)
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Boot_ComFPGA): Clearing communication FPGA in Module %d timed out.", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);
		}
		/* Set communication FPGA Program *= 1 to start downloading */
		// Rev. D uses same FPGA (for now)
		for (k = 0; k < N_COMFPGA_BYTES; k++)
		{
			 if (BoardRevision >= MODULETYPE_P4C)       Pixie_Register_IO(ModNum, PCI_CFDATA, MOD_WRITE, (U32 *)&ComFPGA_Configuration_P4_RevC[k]);  // Pixie-4, Rev C, D, E
			// a very short wait if it fails the first time, increasing as we fail more often
			counter1 = 0;
			while (counter1 < (100*counter0/One_Cycle_Time))	counter1++;	// 500 works for PXIe-8130 (mostly); longer delays seem (sometimes) necessary for Xe laptop
			Pixie_Register_IO(ModNum, PCI_CFSTATUS, MOD_READ, buffer); // Dummy read except for the status read on the last byte
		}
		counter0++;
	}

	if(counter0 == 10)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_ComFPGA): buffer = %x", buffer[0]);
		Pixie_Print_MSG(ErrMSG,1);
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_ComFPGA): Downloading communication FPGA to Module %d timed out", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-3);
	}
	else
	{
		sprintf(ErrMSG, "*INFO* Downloaded communication FPGA successfully to Module %d", ModNum);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);	
	}

	return(0);
}


/****************************************************************
*	Pixie_Boot_FIPPI:
*		Download RTPU FPGA (FIPPI).
*
*		Return Value:
*			0  - boot successful
*			-1 - clearing FiPPI timed out
*			-2 - downloading FiPPI timed out
*
****************************************************************/



S32 Pixie_Boot_FIPPI (
					  U8 ModNum )		// Pixie module number
{

	U32	buffer[0x5], k, counter0, counter1;


	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);


	/* Initialize counter0 to 0 */
	buffer[0] = counter0 = 0;
	/* if buffer[0] = 0x3F, download FIPPI0&1 FPGA successful */
	while (buffer[0] != 0x3F && counter0 < 10) {
		/* Set FIP0&FIP1 Chip Program*=0 to clear configuration */
		buffer[0] = 0x4E; 

		/* Write to an offset from the PCI-to-Local space window */
		Pixie_Register_IO(ModNum, PCI_CFCTRL, MOD_WRITE, buffer);

		/* Set FIP0&FIP1 Chip Program*=1 to start configuration */
		buffer[0] = 0x7E;
		Pixie_Register_IO(ModNum, PCI_CFCTRL, MOD_WRITE, buffer);

		/* Initialize counter to 0 */
		/* if buffer[0] = 0x39, finished clearing FIPPI configuration */
		counter1 = 0;

		while (buffer[0] != 0x39 && counter1 < 100) {
			Pixie_Register_IO(ModNum, PCI_CFSTATUS, MOD_READ, buffer);
			counter1++;
		}

		if (counter1 == 100) {
			sprintf(ErrMSG, "*ERROR* (Pixie_Boot_FIPPI): Clearing FIPPI configuration timed out for Module %d", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}

		for (k = 0; k < N_FIPPI_BYTES; k++)	// now download
		{
			Pixie_Register_IO(ModNum, PCI_CFDATA, MOD_WRITE, (U32 *)&FIPPI_Configuration[k]);
			// a very short wait if it fails the first time, increasing as we fail more often
			counter1 = 0;
			while (counter1 < (100*counter0/One_Cycle_Time))	counter1++;	// 500 works for PXIe-8130 (mostly); longer delays seem (sometimes) necessary for Xe laptop	
			Pixie_Register_IO(ModNum, PCI_CFSTATUS, MOD_READ, buffer); // Dummy read except the last byte to get status
		}
		counter0++;
	}


	if (counter0 == 10) 
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_FIPPI): Downloading FIP0&1 FPGA timed out for Module %d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}
	else
	{
		sprintf(ErrMSG, "*INFO* Downloaded FIP0&1 FPGAs Successfully for Module %d", ModNum);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
	}


	return(0);
}


/*******************************************************************
*	Pixie_Boot_DSP:
*		Download DSP code. 	Note: The conversion routine that
*		creates the binary DSP code will produce a fixed-length
*		data array with 32768 16-bit words.
*
*		Return Value:
*			 0 - boot successful
*			-1 - downloading DSP code failed
*
****************************************************************/

S32 Pixie_Boot_DSP (
					U8 ModNum, 		// Pixie module number
					U16 ModuleType )// P4 or P500
{

	U32	buffer[0x5], k, counter;

	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);

	/* Initialize counter to 0 */
	k = counter = 0;
	while (k < N_DSP_CODE_BYTES/2 && counter < 3) {
		/* reset DSP for 6 cycles */
		buffer[0] = 0x14;
		Pixie_Register_IO(ModNum, PCI_CSR, MOD_WRITE, buffer);
		Pixie_Sleep(70);

		/* Set RUNENABLE=0 */
		buffer[0] = 0x00;
		Pixie_Register_IO(ModNum, PCI_CSR, MOD_WRITE, buffer);

		/*  DSP codes download, with ready enabled, but no burst. IDMA long write and long read */
		/* write PM address starting from 0x01 */
		buffer[0] = 0x01;
		Pixie_Register_IO(ModNum, PCI_IDMAADDR, MOD_WRITE, buffer);
		Pixie_Sleep(3);
		for (k = 2; k < N_DSP_CODE_BYTES/2; k++) 
			if(ModuleType==MODULETYPE_P4)
				Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, (U32 *)&DSP_Code_P4[k]);
			else
				Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, (U32 *)&DSP_Code_P500[k]);

		/* write IDMA address 0x00 */
		buffer[0] = 0x00;
		Pixie_Register_IO(ModNum, PCI_IDMAADDR, MOD_WRITE, buffer);

		/* download DSP code at PM 00 address */
		if(ModuleType==MODULETYPE_P4)
		{
			buffer[0]=(U32)DSP_Code_P4[0];
			Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, buffer);
			buffer[0]=(U32)DSP_Code_P4[1];
			Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, buffer);
			Pixie_Sleep(20);
		}
		else
		{
			buffer[0]=(U32)DSP_Code_P500[0];
			Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, buffer);
			buffer[0]=(U32)DSP_Code_P500[1];
			Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_WRITE, buffer);
			Pixie_Sleep(20);
		}

		/* write IDMA address 0x00 */
		buffer[0] = 0x00;
		Pixie_Register_IO(ModNum, PCI_IDMAADDR, MOD_WRITE, buffer);
		Pixie_Sleep(3);

		/* Read back PM for verifying */
		k = 0;
		if(ModuleType==MODULETYPE_P4)
			while (Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_READ, buffer), buffer[0] == (U32)DSP_Code_P4[k] && k < N_DSP_CODE_BYTES/2) { k++; }
		else
			while (Pixie_Register_IO(ModNum, PCI_IDMADATA, MOD_READ, buffer), buffer[0] == (U32)DSP_Code_P500[k] && k < N_DSP_CODE_BYTES/2) { k++; }

		counter++;
	}

	if (k < N_DSP_CODE_BYTES/2) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Boot_DSP): Downloading DSP code failed for module %d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	sprintf(ErrMSG, "*INFO* Downloaded DSP codes sucessfully for module %d", ModNum);
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

	return(0);
}




/****************************************************************
*	Pixie_Scan_Crate_Slots:
*		Scan all PXI/CompactPCI crate slots and obtain virtual address
*		for each slot where a PCI device is installed.
*
*		Return Value:
*			 0 - Successful
*			-1 - Failed to measure host computer speed (ns per cycle)
*			-2 - Can't open PXI system initialization file
*			-3 - Unable to close the PLX device
*			-4 - Unable to find any modules in the system
*			-5 - Could not open PCI(e) Device
*			-6 - Unable to map a PCI BAR and obtain a virtual address 
*        -7 - Failed scanning the PCIe bus
*        -8 - unable to determine bus offset
*        -9 - unknown serial number in slot mapping
****************************************************************/


S32 Pixie_Scan_Crate_Slots ( U8 NumModules, // Number of modules present
							 U16 *PXISlotMap ) // Slot-module correspondence map
{
	PLX_DEVICE_KEY DevKey;
	PLX_STATUS     rc;
	U8             ByteValue;
	U8             *PCIDeviceNum = NULL;
	U8             *PCIBusNum    = NULL;
	U16            k;
	U16            m, value16;
	U16			   index_SN, index_BV, index_AR, index_AB;
	U32            BoardInfo;
	U32            BoardRevision;
	U32            ClassCodeReg;
	S8             BusNumOffset;		  // the logic ensures that this number is always >=0
	S32            retval;

#ifdef WINDRIVER_API
	WDC_PCI_SCAN_RESULT scanResult;
	WD_PCI_SLOT pSlot;
	DWORD dwStatus, dwNumDevices, dwData;
	WD_PCI_CARD_INFO deviceInfo;
	U16 i;
	PWDC_DEVICE pDev;
	WDC_ADDR_DESC AddrDesc[3];// bar0, bar2, bar4

	// temp variables for rearranging hDev by serial numbers
	WDC_DEVICE_HANDLE tmp_hDev[PRESET_MAX_MODULES]; // temp array, before arranged by serial numbers
	double tmp_SerialNumber[PRESET_MAX_MODULES], tmp_BoardVersion[PRESET_MAX_MODULES];
	U32 tmp_VAddr[PRESET_MAX_MODULES];	
#endif

	if(Offline == 1) return(0); /* Returns immediately for offline analysis */

	index_BV = Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR);
	index_SN = Find_Xact_Match("SERIAL_NUMBER", Module_Parameter_Names, N_MODULE_PAR);
	index_AR = Find_Xact_Match("ADC_RATE", Module_Parameter_Names, N_MODULE_PAR);
	index_AB = Find_Xact_Match("ADC_BITS", Module_Parameter_Names, N_MODULE_PAR);

	// Measure host computer speed (ns per cycle)
	retval = get_ns_per_cycle(&One_Cycle_Time); 
	if(retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Failed to measure host computer speed (ns per cycle), retval = %d", retval);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): Host computer speed (ns per cycle) = %.3f", One_Cycle_Time);
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);


	// Setting the slot - (device number) correspondence mimicing the pxisys.ini file
	sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): Maximum Number of Modules in a Crate = %i", Max_Number_of_Modules);
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

	if (Max_Number_of_Modules == 7) {
		PCIBusNum    = &PCIBusNum_8Slots_ini[0];
		PCIDeviceNum = &PCIDeviceNum_8Slots_ini[0];
	}
	if (Max_Number_of_Modules == 13) {
		PCIBusNum    = &PCIBusNum_14Slots_ini[0];
		PCIDeviceNum = &PCIDeviceNum_14Slots_ini[0];
	}
	if (Max_Number_of_Modules == 17) {
		PCIBusNum    = &PCIBusNum_18Slots_ini[0];
	 	PCIDeviceNum = &PCIDeviceNum_18Slots_ini[0];
	}
	if (Max_Number_of_Modules == 62) {
		PCIBusNum    = &PCIBusNum_1062_ini[0];
		PCIDeviceNum = &PCIDeviceNum_1062_ini[0];
	}
	if (PCIBusNum == NULL) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Cannot find PXI initialization constants for selected chassis, retval = %d", -2);
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}

#ifdef WINDRIVER_API
	/* Determine the PCI bus type */
	PCIBusType = 0;
	memset(&DevKey, PCI_FIELD_IGNORE, sizeof(PLX_DEVICE_KEY));
	BZERO(scanResult);
	DevKey.VendorId = 0x10b5;
	DevKey.DeviceId = 0x9054;
	if ((rc = PlxPci_DeviceFind(&DevKey, 0)) == ApiSuccess) {
		PCIBusType = REGULAR_PCI;
		sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): PCI Bus is REGULAR type");
		Pixie_Print_MSG(ErrMSG,1);
		if (PrintDebugMsg_Boot) {
			for (k = 0; k < NumModules; k++) {
				sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): will set device %d to slot %d", k, PXISlotMap[k]);
				Pixie_Print_MSG(ErrMSG,1);
			}
		}
	}
	if (PCIBusType != REGULAR_PCI) { // not regular PCI, try to initialize PCIe API
		// Initialize the WinDriver library
		// TODO: If library is already initialized, uninitialize it here.
		PIXIE500E_LibUninit();
		if ((PIXIE500E_LibInit() != WD_STATUS_SUCCESS) && (rc != ApiSuccess)) {
			sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Failed to initialize the PIXIE5000E library: %s", PIXIE500E_GetLastErr());
			Pixie_Print_MSG(ErrMSG,1);
			sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): System has no devices");
			Pixie_Print_MSG(ErrMSG,1);
			return -4;
		}
		if ((dwStatus = WDC_PciScanDevices(PIXIE500E_DEFAULT_VENDOR_ID, PIXIE500E_DEFAULT_DEVICE_ID, &scanResult)) == WD_STATUS_SUCCESS) {
			PCIBusType = EXPRESS_PCI;
			sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots):PCI bus is EXPRESS type");
			Pixie_Print_MSG(ErrMSG,1);
			if (PrintDebugMsg_Boot) {
				for (k = 0; k < NumModules; k++) {
					sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): will set device %d to serial number %d", k, PXISlotMap[k]);
					Pixie_Print_MSG(ErrMSG,1);
				}
			}
		}
		else {
			sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots_GN): Failed scanning the PCI bus.\n Error: 0x%lx - %s (%x)\n", dwStatus, Stat2Str(dwStatus), scanResult);
			Pixie_Print_MSG(ErrMSG,1);
			return (-7);
		}
	} // if not regular PCI
#endif

	switch (PCIBusType) {
		case REGULAR_PCI:
			/* Determine bus number offset */
			memset(&DevKey, PCI_FIELD_IGNORE, sizeof(PLX_DEVICE_KEY));
			DevKey.VendorId = 0x10b5;
			DevKey.DeviceId = 0x9054;
			// Looks for the first PLX device defined by the system
			if ((rc = PlxPci_DeviceFind(&DevKey, 0)) != ApiSuccess) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Can't find any PLX devices, PlxPci_DeviceFind rc=%d", rc);
				Pixie_Print_MSG(ErrMSG,1);
				return(-4);
			}
			ClassCodeReg = 0;
			k = 0;
			// KS TODO: check if we are really looking for 0x604, because PXI-1036 with Pixie-4 does this loop all MAX_PCI_DEV times.
			while (k < MAX_PCI_DEV && ClassCodeReg != 0x604) {  // Looking for a PCI-to-PCI bridge on the bus of the first PLX device
				ClassCodeReg = PlxPci_PciRegisterRead_BypassOS(DevKey.bus, (U8)k, 0, 0x8, &rc) >> 16;
				switch (rc) {		// more return code checking, 
					case ApiSuccess:
						break;
					case ApiNoActiveDriver:
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): PlxPci_PciRegisterRead_BypassOS() - no driver");
						Pixie_Print_MSG(ErrMSG,1);
						return(-8);
					case ApiUnsupportedFunction:
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): PlxPci_PciRegisterRead_BypassOS() - unsupported");
						Pixie_Print_MSG(ErrMSG,1);
						return(-8);
					default:
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): PlxPci_PciRegisterRead_BypassOS() - 0x%x  ?", rc);
						Pixie_Print_MSG(ErrMSG,1);
						return(-8);
				}
			    if (rc != ApiSuccess)  {
					sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to read register #1");
					Pixie_Print_MSG(ErrMSG,1);
					return(-8);
			    }
				k++;
			}
			sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): Scanning for busses done");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

			if (Max_Number_of_Modules == 7 && k == MAX_PCI_DEV) BusNumOffset = DevKey.bus - 1; // No more bridges exist on the bus of the 8-slot crate
			if (Max_Number_of_Modules == 7 && k != MAX_PCI_DEV) { // Otherwise we got a problem
				sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to find bus number offset");
				Pixie_Print_MSG(ErrMSG,1);
				return(-8);
			}
			if (Max_Number_of_Modules == 62 && k == MAX_PCI_DEV) BusNumOffset = DevKey.bus - 1; // No more bridges exist on the bus of the 8-slot crate
			if (Max_Number_of_Modules == 62 && k != MAX_PCI_DEV) { // Otherwise we got a problem
				sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to find bus number offset");
				Pixie_Print_MSG(ErrMSG,1);
				return(-8);
			}
			if (Max_Number_of_Modules == 13 && k == MAX_PCI_DEV) BusNumOffset = DevKey.bus - 2; // If no bridges exist on the bus of the first PLX device in a 14-slot crate then the device is in the second bus of the crate
			if (Max_Number_of_Modules == 13 && k != MAX_PCI_DEV) BusNumOffset = DevKey.bus - 1; // Otherwise the device is in the first bus of the crate
			if (Max_Number_of_Modules == 17 && k == MAX_PCI_DEV) BusNumOffset = DevKey.bus - 3; // If no bridges exist on the bus of the first PLX device in a 18-slot crate then the device is in the third bus of the crate
			if (Max_Number_of_Modules == 17 && k != MAX_PCI_DEV) { // Otherwise more checking is necessary
				ClassCodeReg = k = 0;
				while (k < MAX_PCI_DEV && ClassCodeReg != 0x604) {  // Looking for a PCI-to-PCI bridge on the bus next to the one containing the first PLX device
					ClassCodeReg = PlxPci_PciRegisterRead_BypassOS((U8)(DevKey.bus + 1), (U8)k, 0, 0x8, &rc) >> 16;
					if (rc != ApiSuccess) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to read register #2");
						Pixie_Print_MSG(ErrMSG,1);
						return(-8);
					}
					k++;
				}
				if (k == MAX_PCI_DEV) BusNumOffset = DevKey.bus - 2; // If none is found the device is in the second bus
				else                  BusNumOffset = DevKey.bus - 1; // Otherwise the device is in the first bus
			}
			// Open PCI devices for all modules
			for(k = 0; k < (U16)NumModules; k++) {
				// Clear key structure
				memset(&DevKey, PCI_FIELD_IGNORE, sizeof(PLX_DEVICE_KEY));
				DevKey.VendorId = 0x10b5; // PLX Vendor ID is 0x10b5
				DevKey.DeviceId = 0x9054; // PLX Device ID is 0x9054
				DevKey.bus  = PCIBusNum   [PXISlotMap[k]] + (U8)BusNumOffset;		// BusNumOffset is >=0 by logic above
				DevKey.slot = PCIDeviceNum[PXISlotMap[k]];
				// Use the previously mapped PlxModIndex as the Device Number in the search
				rc = PlxPci_DeviceFind(&DevKey, 0);
				if (rc == ApiSuccess) { // if succesfully found

					sprintf(ErrMSG, "System Bus = %i  Local bus  = %i", DevKey.bus, PCIBusNum[PXISlotMap[k]]);                         // Physical device location
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
					sprintf(ErrMSG, "Device Number = %i  Crate Slot Number = %i", DevKey.slot, PXISlotMap[k]);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

					rc = PlxPci_DeviceClose(&Sys_hDevice[k]);
					if (rc != ApiSuccess && rc != 0x206) {
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to close the PLX device for module %d; rc=%d", k, rc);
						Pixie_Print_MSG(ErrMSG,1);
						return(-2);
					}
					rc = PlxPci_DeviceOpen(&DevKey, &Sys_hDevice[k]);
					if(rc != ApiSuccess) { // Print error if failure
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Could not open PCI Device Number (%d) at Bus Number %d; rc=%d", PCIDeviceNum[PXISlotMap[k]], PCIBusNum[PXISlotMap[k]] + BusNumOffset, rc);
						Pixie_Print_MSG(ErrMSG,1);
						// Before return, we need to close those PCI devices that are already opened
						for(m=0; m<k; m++) {
							rc = PlxPci_DeviceClose(&Sys_hDevice[m]);
							if (rc != ApiSuccess) {
								sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to close the PLX device for module %d; rc=%d", k, rc);
								Pixie_Print_MSG(ErrMSG,1);
								return(-3);
							}
						}
						return(-6);
					} // if unable to open
					else {
						// Map a PCI BAR into user virtual space and return the virtual address
						// for the opened PCI device. For PLX 9054, Space 0 is at PCI BAR 2.
						rc = PlxPci_PciBarMap(&Sys_hDevice[k], 2, (VOID**)&VAddr[k]);
						if(rc != ApiSuccess) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to map a PCI BAR and obtain a virtual address for Module=%d; rc=%d", k, rc);
							Pixie_Print_MSG(ErrMSG,1);
							// Before return, we need to close those PCI devices that are already opened
							for(m=0; m<k; m++) {
								rc = PlxPci_DeviceClose(&Sys_hDevice[m]);
								if (rc != ApiSuccess) {
									sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to close the PLX device for module %d; rc=%d", k, rc);
									Pixie_Print_MSG(ErrMSG,1);
									return(-3);
								}
							}
							return(-7);
						} // if cannot map virtual address
						else {
							sprintf(ErrMSG, "VAddr[%d][%d]=0x%lx", PCIBusNum[PXISlotMap[k]] + BusNumOffset, k, VAddr[k]);
							Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
						} // if successfully mapped virtual address
						
						// Get Revision and Serial Number
						Pixie_ReadVersion((U8)k, &BoardInfo); // read from Xilinx PROM
						Pixie_Devices[k].Module_Parameter_Values[index_BV]=(double)(BoardInfo & 0xFFFF);

						if( (BoardInfo & 0x0F00) == MODULETYPE_P4) { // Pixie-4
							Pixie_Devices[k].Module_Parameter_Values[index_AB] = 14.0;
							Pixie_Devices[k].Module_Parameter_Values[index_AR] = 75.0;
						} else {
							Pixie_Devices[k].Module_Parameter_Values[index_AB] = 12.0;		// Pixie-500 (should never happen)
							Pixie_Devices[k].Module_Parameter_Values[index_AR] = 500.0;
						}
						/* Read from Microchip EEPROM
						EEPROM contents		Pixie-4		Pixie-500	Notes
						word 0: revision		0,2,3,4		0,1,2		0,1,2, ... = Rev A, B, C ..., except P4 Rev B is "0" 
						word 1: S/N          0-250       0-250		add 250 for P4 Rev D to get true S/N
																            add 450 for P4 Rev E to get true S/N
						*/
						BoardRevision = 255; // initialize to bad value
						retval = I2C24LC16B_Read_One_Byte((U8)k, 0x0, &ByteValue);
						if(retval < 0) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Failed to read I2C board revision in Module %d", k);
							Pixie_Print_MSG(ErrMSG,1);
						}
						else {
							if( (BoardInfo & 0x0F00) == MODULETYPE_P4) { // Pixie-4
								if(ByteValue == 0)                   BoardRevision = 1;			// Rev/ B
								else if( (ByteValue >= 2) && (ByteValue <= 4) ) BoardRevision = ByteValue;  // Rev. C & D & E
							}		
							sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots): Xilinx: %X I2C: %d", BoardInfo, ByteValue );
							Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);

							if( (BoardInfo & 0xF) != (BoardRevision)) {
								sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Board revision mismatch in Module %d", k);
								Pixie_Print_MSG(ErrMSG,1);
								// But since we use the Xilinx PROM value, reading a mismatch value from the EEPROM is not fatal
							}
						}
						retval = I2C24LC16B_Read_One_Byte((U8)k, 0x1, &ByteValue);
						if(retval < 0) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Failed to read serial number in Module %d", k);
							Pixie_Print_MSG(ErrMSG,1);
						}
						else {
							// Pixie-4 Rev. D serial numbers start at 250, but are stored as S/N-250
							if((BoardInfo & 0xFF0F) == MODULETYPE_P4D) Pixie_Devices[k].Module_Parameter_Values[index_SN]=(double)(ByteValue) +250.0;	
							else if ( (BoardInfo & 0xFF0F ) == MODULETYPE_P4E) Pixie_Devices[k].Module_Parameter_Values[index_SN]=(double)(ByteValue) +450.0;	// Pixie-4 Rev. E serial numbers start at 450, but are stored as S/N-450
							else                                       Pixie_Devices[k].Module_Parameter_Values[index_SN]=(double)(ByteValue);
						}

						sprintf(ErrMSG, "(Pixie_Scan_Crate_Slots): Module # %i SERIAL NUMBER = %i", k, (U8)ByteValue);
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
					}
				}
				else { // if failed to find
					sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): PlxPci_DeviceFind failed for PCI Device Number (%d) at Bus Number %d; rc=%d", PCIDeviceNum[PXISlotMap[k]], PCIBusNum[PXISlotMap[k]] + BusNumOffset, rc);
					Pixie_Print_MSG(ErrMSG,1);
					// Before return, we need to close those PCI devices that are already opened
					for(m=0; m<k; m++) {
						rc = PlxPci_DeviceClose(&Sys_hDevice[m]);
						if (rc != ApiSuccess) {
							sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots): Unable to close the PLX device for module %d; rc=%d", k, rc);
							Pixie_Print_MSG(ErrMSG,1);
						}
					}
					return(-8);
				} // if found and opened
			} // for all devices
			break;
		case EXPRESS_PCI: // working with WINDRIVER_API
#ifdef WINDRIVER_API
			dwNumDevices = scanResult.dwNumDevices;
			if (!dwNumDevices) {
				sprintf(ErrMSG,"*ERROR* (Pixie_Scan_Crate_Slots): No matching device was found for search criteria (Vendor ID 0x%lX, Device ID 0x%lX)\n", PIXIE500E_DEFAULT_VENDOR_ID, PIXIE500E_DEFAULT_DEVICE_ID);
				Pixie_Print_MSG(ErrMSG,1);
				return (-4);
			}
			if (dwNumDevices != NumModules) {
				sprintf(ErrMSG,"*ERROR* (Pixie_Scan_Crate_Slots): Found more or less modules (%d) than defined (%d).", dwNumDevices, NumModules);
				Pixie_Print_MSG(ErrMSG,1);
				return (-4);
			}
			for (k = 0; k < dwNumDevices; k++) {
				BZERO(pSlot);
				pSlot = scanResult.deviceSlot[k];
				/* Retrieve the device's resources information */
				BZERO(deviceInfo);
				deviceInfo.pciSlot = pSlot;
				dwStatus = WDC_PciGetDeviceInfo(&deviceInfo);
				if (WD_STATUS_SUCCESS != dwStatus) {
					sprintf(ErrMSG,"*ERROR* (Pixie_Scan_Crate_Slots): DeviceOpen, failed retrieving the device's resources information.\n Error 0x%lx - %s\n", dwStatus, Stat2Str(dwStatus));
					Pixie_Print_MSG(ErrMSG,1);
					return (-5);
				}
				/* NOTE: You can modify the device's resources information here, if
				necessary (mainly the deviceInfo.Card.Items array or the items number -
				deviceInfo.Card.dwItems) in order to register only some of the resources
				or register only a portion of a specific address space, for example. */
				for(i=0; i < deviceInfo.Card.dwItems; i++)
				{
					/* Enable shared interrupts on device */
                                  // Making interrupts not shareble seems to lead to resource conflicts (same IRQ registered with WinDriver)
                                  // when using more than 3 modules
                                  // Logging items for debug: normally see Bus, 3 memory items (BAR0, BAR2, BAR4), interrupt.
					if (deviceInfo.Card.Item[i].item == ITEM_INTERRUPT) {
						deviceInfo.Card.Item[i].fNotSharable = FALSE; 
						sprintf(ErrMSG, "*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item INTERRUPT");
						Pixie_Print_MSG(ErrMSG, PrintDebugMsg_Boot);
					} else if (deviceInfo.Card.Item[i].item == ITEM_MEMORY) {
						deviceInfo.Card.Item[i].fNotSharable = TRUE; 
						sprintf(ErrMSG, "*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item MEMORY");
						Pixie_Print_MSG(ErrMSG, PrintDebugMsg_Boot);
					} else if (deviceInfo.Card.Item[i].item == ITEM_BUS) {
						deviceInfo.Card.Item[i].fNotSharable = TRUE; 
						sprintf(ErrMSG, "*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item BUS");
						Pixie_Print_MSG(ErrMSG, PrintDebugMsg_Boot);
					} else {
						sprintf(ErrMSG, "*DEBUG* (Pixie_Scan_Crate_Slots_GN): found card item %d", deviceInfo.Card.Item[i].item);
						Pixie_Print_MSG(ErrMSG, PrintDebugMsg_Boot);
					}
				}
				/* Open a handle to the device */
				hDev[k] = PIXIE500E_DeviceOpen(&deviceInfo);
				if (hDev[k]) { // if succesfully opened
					sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots_GN): Gennum device %d on Bus=%d", k, pSlot.dwBus);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
				}
				else { // Failed to open one device
					sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots_GN): DeviceOpen, failed opening a handle to the device. Power cycle of module required");
					Pixie_Print_MSG(ErrMSG,1);
					// Close previously opened devices
					if(k>0) {
						for (m=0; m<k; m++) {   
							if (!PIXIE500E_DeviceClose(hDev[m])) {
								sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots_GN): DeviceClose: Failed closing PIXIE5000E device: %s", PIXIE500E_GetLastErr());
								Pixie_Print_MSG(ErrMSG,1);
							}		
						}
					}
					return (-5);
				} // if done opening

				// Map Virtual Addresses:
				// Find BAR0, get dwUserDirectAddr.
				pDev = (PWDC_DEVICE)hDev[k];
				// Map BAR0, BAR2, BAR4
				AddrDesc[0] = pDev->pAddrDesc[AD_PCI_BAR0];
				AddrDesc[1] = pDev->pAddrDesc[AD_PCI_BAR2];
				AddrDesc[2] = pDev->pAddrDesc[AD_PCI_BAR4];
				VAddr[k] = WDC_MEM_DIRECT_ADDR((WDC_ADDR_DESC*)&AddrDesc[0]);
				
				// For Pixie500e each device is on different bus, and that number can be different on different PCs.
				// At first we set the hDev, VAddr, BOARD_VERSION, SERIAL_NUMBER using enumeration
				// that WDC_PciScanDevices() has provided. Then rearrange them according to SLOT_WAVE (containing serial numbers).
			
				// Get Revision and Serial Number from EEPROM
				Pixie_ReadVersion((U8)k, &BoardInfo); // read board version and serial number from Gennum EEPROM
				// Workaround for devices with uninitialized EEPROM: change Revision to 0xA101 and Serial Number to 0x0000
				if (BoardInfo==0xFFFFFFFF) BoardInfo =  0x0000A101; //0x00C8A550;

				Pixie_Devices[k].Module_Parameter_Values[index_BV]= (double)(BoardInfo & 0x0000FFFF);
				Pixie_Devices[k].Module_Parameter_Values[index_SN]= (double)((BoardInfo & 0xFFFF0000) >> 16);

		

				sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots_GN): Device %d, Board version= 0x%04X, S/N = %d", 
					k,
					(U16)Pixie_Devices[k].Module_Parameter_Values[index_BV], 
					(U16)Pixie_Devices[k].Module_Parameter_Values[index_SN]);
				Pixie_Print_MSG(ErrMSG,1);
			}// for each found module k in scanResult


			// loop over found devices and arrange in hDev[] array according to serial numbers
			for (k =0; k < dwNumDevices; k++) {
				// find match in Slots[] serial numbers
				m = 0;
				while (m < NumModules) {
					if (Pixie_Devices[k].Module_Parameter_Values[index_SN] == PXISlotMap[m]) {
						break;
					};
					m++;
				}
			
				if (m==NumModules) { // checked all in Slots[], but didn't find the matching serial number
						sprintf(ErrMSG, "*ERROR* (Pixie_Scan_Crate_Slots_GN): Could not find device with S/N=%d. Check list of serial numbers.", PXISlotMap[k]);
						Pixie_Print_MSG(ErrMSG,1);
						return (-9);
				}
			}// for found devices
	
			// loop over specified serial numbers  arrange device pointers in temp array
			// assume a one-to-one match
			for (m =0; m < NumModules; m++) {
				for (k =0; k < dwNumDevices; k++) {
					// FIXME: KS: when serial number is not set
					// hDev[] is not passed and device handle is lost.
					// Explanation: unprogrammed EEPROM returns 0xFFFF for revision and serial number.
					// In that case revision is changed to 0xA101 (P500e) and Serial to 0x0000, and user should use 0 for serial number.
					if (Pixie_Devices[k].Module_Parameter_Values[index_SN] == PXISlotMap[m]) {
						tmp_hDev[m] = hDev[k];
						tmp_SerialNumber[m] = Pixie_Devices[k].Module_Parameter_Values[index_SN];
						tmp_BoardVersion[m] = Pixie_Devices[k].Module_Parameter_Values[index_BV];
						tmp_VAddr[m] = VAddr[k];
					}
				} // for found modules
			}	// for specified modules

			// copy temp array back to device pointers
			for (m =0; m < NumModules; m++) {
				hDev[m] = tmp_hDev[m];
				Pixie_Devices[m].Module_Parameter_Values[index_SN] = tmp_SerialNumber[m];
				Pixie_Devices[m].Module_Parameter_Values[index_BV] = tmp_BoardVersion[m];
				VAddr[m] = tmp_VAddr[m] ;
         } // for specified modules

         // Set Module parameters for ADC BITS and ADC RATE
         for (k =0; k < NumModules; k++) {
            Pixie_ReadVersion((U8)k, &BoardInfo); // read board version and serial number from Gennum EEPROM
				value16 = (U16)BoardInfo & 0x00000FF0;
				switch (value16) {
					case MODULETYPE_P500e:
					case MODULETYPE_P4e_14_500:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 14.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 500.0;
						break;
					case MODULETYPE_P4e_16_125:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 16.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 125.0;
						break;
					case MODULETYPE_P4e_14_125:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 14.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 125.0;
						break;
					case MODULETYPE_P4e_12_125:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 12.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 125.0;
						break;
					case MODULETYPE_P4e_16_250:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 16.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 250.0;
						break;
					case MODULETYPE_P4e_14_250:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 14.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 250.0;
						break;
					case MODULETYPE_P4e_12_250:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 12.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 250.0;
						break;					
					case MODULETYPE_P4e_16_500:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 16.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 500.0;
						break;
					case MODULETYPE_P4e_12_500:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 12.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 500.0;
						break;
					default:
						Pixie_Devices[k].Module_Parameter_Values[index_AB] = 16.0;
						Pixie_Devices[k].Module_Parameter_Values[index_AR] = 125.0;
						break;
				}
			
			} // for specified modules

			// TODO: if dwNumDevices > NumModules it would be nice to free resources for modules not listed in PXISlotMap[].
			// TODO: graceful exit if no matching serial number in PXISlotMap[].
			for (k = 0; k < dwNumDevices; k++) {
				if (k >= NumModules) {
					sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots_GN): DEVICE %d IS NOT USED!", k);
					Pixie_Print_MSG(ErrMSG,1);
				}
				sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots_GN): UPDATED Device %d, Board version= 0x%04X, S/N = %d", 
					k,
					(U16)Pixie_Devices[k].Module_Parameter_Values[index_BV], 
					(U16)Pixie_Devices[k].Module_Parameter_Values[Find_Xact_Match("SERIAL_NUMBER", 
					Module_Parameter_Names, N_MODULE_PAR)]);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);
			}
			
			sprintf(ErrMSG, "*INFO* (Pixie_Scan_Crate_Slots_GN) Finished scanning all modules");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_Boot);			
#endif
			break;

		default:
			break;
	}

	return(0);
}


/****************************************************************
*	Pixie_Close_PCI_Devices function:
*		Unmap PCI BAR and close PLX PCI devices.
*
*		Return Value:
*			 0 - successful
*			-1 - Unable to unmap the PCI BAR
*			-2 - Unable to close the PCI device
*
****************************************************************/

S32 Pixie_Close_PCI_Devices (
							 U16 ModNum )		// Pixie module number
{

	PLX_STATUS	rc;

	/* Unmaps a previously mapped PCI BAR from user virtual space */
	rc = PlxPci_PciBarUnmap(&Sys_hDevice[ModNum], (VOID **)&VAddr[ModNum]);
	if(rc != ApiSuccess)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Close_PCI_Devices): Unable to unmap the PCI BAR; rc=%d", rc);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	/* Release the PLX device */
	rc = PlxPci_DeviceClose(&Sys_hDevice[ModNum]);
	if (rc != ApiSuccess)
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Close_PCI_Devices): Unable to close the PLX device; rc=%d", rc);
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}

	return(0);
}


/****************************************************************
*	Pixie_Init_Globals: 
*		Initialize some frequently used globals. 
* 
*		Return Value: 
*			0  - successful 
****************************************************************/

S32 Pixie_Init_Globals(void) {

	U8     i;
	S8     str[128];
	U16    idx, value16;
	double value;

	U16 SYSTEM_CLOCK_MHZ = P4_SYSTEM_CLOCK_MHZ;
	U16 FILTER_CLOCK_MHZ = P4_FILTER_CLOCK_MHZ;
	U16 ADC_CLOCK_MHZ = P4_ADC_CLOCK_MHZ;
	U16 DSP_CLOCK_MHZ = P4_DSP_CLOCK_MHZ;
	U16 CTscale = P4_CTSCALE;			// The scaling factor for count time counters

	/* Set frequently used indices */
	Run_Task_Index=Find_Xact_Match("RUNTASK", DSP_Parameter_Names, N_DSP_PAR);
	Control_Task_Index=Find_Xact_Match("CONTROLTASK", DSP_Parameter_Names, N_DSP_PAR);
	Resume_Index=Find_Xact_Match("RESUME", DSP_Parameter_Names, N_DSP_PAR);
	RUNTIME_Index=Find_Xact_Match("RUNTIMEA", DSP_Parameter_Names, N_DSP_PAR);
	NUMEVENTS_Index=Find_Xact_Match("NUMEVENTSA", DSP_Parameter_Names, N_DSP_PAR);
	SYNCHDONE_Index=Find_Xact_Match("SYNCHDONE", DSP_Parameter_Names, N_DSP_PAR);
	BoardVersion_Index=Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR);

	for(i=0; i<NUMBER_OF_CHANNELS; i++)
	{
		sprintf(str,"FASTPEAKSA%d",i);
		FASTPEAKS_Index[i]=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);

		sprintf(str,"COUNTTIMEA%d",i);
		COUNTTIME_Index[i]=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	}

	/* Make SGA gain table */
	Make_SGA_Gain_Table();

	/* Initialize energy filter interval */
	for(i=0; i<Number_Modules; i++)
	{
		// Define clock constants according to BoardRevision 
		Pixie_Define_Clocks (i,0,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale,&DSP_CLOCK_MHZ );

		idx=Find_Xact_Match("FILTERRANGE", DSP_Parameter_Names, N_DSP_PAR);
		value=(double)Pixie_Devices[i].DSP_Parameter_Values[idx];

		Filter_Int[i]=pow(2.0, value)/FILTER_CLOCK_MHZ;

	}

	// update Ccontrol options stored in settings file (one of 3 places)
	idx = Find_Xact_Match("CCONTROL", DSP_Parameter_Names, N_DSP_PAR);
	value16 = Pixie_Devices[0].DSP_Parameter_Values[idx];
	//AutoProcessLMData	= (0xF & value16);			// To control if the LM parse routine processes compressed LM data
	PrintDebugMsg_Boot	  = TstBit(4,value16);		// if 1, print debug messages during booting
	PrintDebugMsg_QCerror = TstBit(5,value16);		// if 1, print error debug messages during LM buffer quality check
	PrintDebugMsg_QCdetail= TstBit(6,value16);		// if 1, print detail debug messages during LM buffer quality check
	PrintDebugMsg_other	  = TstBit(7,value16);		// if 1, print other debug messages 
	Polling				= TstBit(8,value16);		// if 1, use (old style) polling of module in LM runs, if 0 use intertupts from module  
	BufferQC			= TstBit(9,value16);		// if 1, execute data quality control check before writing LM data to file
	//KeepCW			= tstbit(10,value16);		// To control update and enforced minimum of coincidence wait
	PollForNewData		= TstBit(11,value16);		// if 1, return new data in DMA buffer during polling
	MultiThreadDAQ		= TstBit(12,value16);		// if 1, run 0x400, and 0x10x as a separate thread
	return(0);
}








