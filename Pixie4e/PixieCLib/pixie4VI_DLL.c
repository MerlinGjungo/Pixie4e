/*----------------------------------------------------------------------
 * Copyright (c) 2004, X-ray Instrumentation Associates
 * Copyright (c) 2005, 2006, XIA LLC
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
 *   * Neither the name of X-ray Instrumentation Associates (or
 *     XIA LLC) nor the names of its contributors may be used
 *     to endorse or promote products derived from this
 *     software without specific prior written permission.
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
 *      pixie4VI_DLL.c
 *
 * Description:
 *
 *      This file contains all the wrapper funtions which
 *		can be used to control the Pixie modules through LabVIEW VIs
 *		and this DLL file.
 *
 * Member functions:
 *
 *		Pixie_Download_FileNames
 *		Pixie_Boot_Modules
 *		Pixie_Adjust_DCOffsets
 *		Pixie_Acquire_ADCWaveforms
 *		Pixie_Start_MCARun
 *      Pixie_Start_ListModeRun
 *		Pixie_Start_ListModeRun_400
 *      Pixie_Check_RunStatus
 *		Pixie_Check_RunStatus_400
 *      Pixie_Check_RunData
 *		Pixie_Get_New_LMdata
 *		Pixie_Stop_Run
 *		Pixie_Stop_Run_400
 *		Pixie_Read_MCAHistograms
 *		Pixie_Read_8KofMCAHistograms
 *		Pixie_Save_MCAHistograms
 *		Pixie_Save_ListModeData
 *		Pixie_Write_User_Par
 *		Pixie_Read_User_Par
 *		Pixie_Read_ListModeData
 *		Pixie_ControlTaskRun
 *
 ******************************************************************************/

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "PlxTypes.h"
#include "Plx.h"

#include "pixie4VI_DLL.h"
#include "globals.h"
#include "defs.h"
#include "sharedfiles.h"
#include "utilities.h"
#include "time.h"


/****************************************************************
 *	Pixie_Download_FileNames:
 *		Download file names from the host to this DLL.
 *
 *		Return Value:
 *			0 - download successful
 *
 ****************************************************************/
/*
 * Pass firmware and DSP file names from UI to C-library.
 * 0. Combined FPGAs P4e (p4e_zdt.bin)                       				Boot_File_Name_List[0]
 * 1. Communication FPGA, P4 rev.C, (syspixie_RevC.bin)      				Boot_File_Name_List[1]
 * 2. P4 FIPPI, (pixie.bin)                                  				Boot_File_Name_List[2]
 * 3. P4 DSP, (PXIcode.bin)                                  				Boot_File_Name_List[3]
 * 4. DSP parameters, (default500e.set)                      				Boot_File_Name_List[4]
 * 5. P4 DSP parameter values (PXIcode.var)                  				Boot_File_Name_List[5]
 * 6. P4 DSP parameter names (PXIcode.lst)  					    		Boot_File_Name_List[6]
 * 7. Communication FPGA P500 Rev.B (syspixie_RevC.bin) (unused now)		Boot_File_Name_List[7]
 * 8. P500 FIPPI (FippiP500.bin)                             				Boot_File_Name_List[8]
 * 9. P500e (pixie500e.bin)                                  				Boot_File_Name_List[9]
 * 10. DSP P500 binary (P500code.bin)                        				Boot_File_Name_List[10]
 * 11. DSP P500e binary (p500e.ldr)                          				Boot_File_Name_List[11]
 * 12. DSP P500e parameter names (P500e.lst) (unused now)    				Boot_File_Name_List[12]

 */

S32 _stdcall Pixie_Download_FileNames (
 
  char *ComFPGAFileNameRevC, //1
  char *SPFPGAFileName,     //2
  char *SPFPGAP4eFileName,	// 0 P4e 16/125
  char *SPFPGAP500FileName, // 8 P4e 14/500
  char *CombinedFPGAP500e,  // 9 P500e
  char *DSPCodeFileName, // 3 P4
  char *DSPP500eCodeFileName, // 11 P4e/500e
  char *DSPVarFileName, //5 
  char *DSPLstFileName,		//6 P4
  char *DSPP500eLstFileName,// 12 P4e/500e
  char *DSPParFileName, // 4
  char *reserved2,
  char *reserved1,
  char *reserved3,
  char *reserved5,
  char *reserved6
  )

{
  
  U32 len, n;
  char *pSource, *pDest;

 /* FGPA Pixie-4e File */
  len = MIN(strlen(SPFPGAP4eFileName), MAX_FILE_NAME_LENGTH);
  pSource = SPFPGAP4eFileName;
  pDest = Boot_File_Name_List[0];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;

  /* P4 ComFPGAFile_RevC */
  len = MIN(strlen(ComFPGAFileNameRevC), MAX_FILE_NAME_LENGTH);
  pSource = ComFPGAFileNameRevC;
  pDest = Boot_File_Name_List[1];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
  
  /* FGPA Pixie-4 File */
  len = MIN(strlen(SPFPGAFileName), MAX_FILE_NAME_LENGTH);
  pSource = SPFPGAFileName;
  pDest = Boot_File_Name_List[2];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;

  /* DSP Pixie-4 Binary File */
  len = MIN(strlen(DSPCodeFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPCodeFileName;
  pDest = Boot_File_Name_List[3];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;

 /* DSP Par File (.set) */
  len = MIN(strlen(DSPParFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPParFileName;
  pDest = Boot_File_Name_List[4];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;

  /* DSP Pixie-4/500/500e Var File */
  len = MIN(strlen(DSPVarFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPVarFileName;
  pDest = Boot_File_Name_List[5];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;

  /* DSP Pixie-4/500 LstFile */
  len = MIN(strlen(DSPLstFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPLstFileName;
  pDest = Boot_File_Name_List[6];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
    
  /* Comm. FPGA P500 revB File 
  len = MIN(strlen(ComFPGAFileNameP500RevB), MAX_FILE_NAME_LENGTH);
  pSource = ComFPGAFileNameP500RevB;
  pDest = Boot_File_Name_List[7];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0; */
  
  /* SPFPGA P500 File */
  len = MIN(strlen(SPFPGAP500FileName), MAX_FILE_NAME_LENGTH);
  pSource = SPFPGAP500FileName;
  pDest = Boot_File_Name_List[8];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
  
  /* SPFPGA P500e File */
  len = MIN(strlen(CombinedFPGAP500e), MAX_FILE_NAME_LENGTH);
  //sprintf(ErrMSG," Within Pixie4VI_DLL.c the file name is %s\n", CombinedFPGAP500e); 
  //Pixie_Print_MSG(ErrMSG,1);
  pSource = CombinedFPGAP500e;
  pDest = Boot_File_Name_List[9];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
  

  /* DSP Pixe-500 File 
  len = MIN(strlen(DSPP500CodeFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPP500CodeFileName;
  pDest = Boot_File_Name_List[10];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;*/
  
  /* DSP Pixe-500e File */
  len = MIN(strlen(DSPP500eCodeFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPP500eCodeFileName;
  pDest = Boot_File_Name_List[11];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
  
  /* DSP Pixie-500e Lst File */
  len = MIN(strlen(DSPP500eLstFileName), MAX_FILE_NAME_LENGTH);
  pSource = DSPP500eLstFileName;
  pDest = Boot_File_Name_List[12];
  n = 0;
  while( n++ < len ) *pDest++ = *pSource++;			
  *pDest = 0;
  
  return (0);
}


/****************************************************************
 *	Pixie_Boot_Modules:
 *		Boot all the Pixie modules in the system.
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
 *			-8 - failed to boot all modules present in the system
 *			-9 - failed to initialize NUMBER_MODULES
 *			-10- failed to initialize OFFLINE_ANALYSIS
 *			-11- failed to initialize MAX_NUMBER_MODULES
 *			-12- failed to initialize SLOT_WAVE
 *
 ****************************************************************/

S32 _stdcall Pixie_Boot_Modules (
  U16 TotalNumberModules,
  U16 OfflineAnalysis,
  U16 MaxNumberOfModules,
  U16 SerialMap[PRESET_MAX_MODULES],
  U16 Boot_Pattern )	// bit mask
{
  
  S32 retval;
  U16 k, idx;
  double user_par_values[N_SYSTEM_PAR];


   char buff[100];
   time_t now = time (0);
   strftime (buff, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	sprintf(ErrMSG, "*INFO* (Pixie_Boot_Modules): %s", buff);
   Pixie_Print_MSG(ErrMSG,1);
  
  /* Initialize a few SYSTEM parametetrs in the global level */
  user_par_values[Find_Xact_Match("NUMBER_MODULES", System_Parameter_Names, N_SYSTEM_PAR)] = TotalNumberModules;
  retval = Pixie_User_Par_IO(user_par_values, "NUMBER_MODULES", "SYSTEM", MOD_WRITE, 0, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Boot_Modules): failed to initialize NUMBER_MODULES.");
    Pixie_Print_MSG(ErrMSG,1);
    return(-9);
  }
  user_par_values[Find_Xact_Match("OFFLINE_ANALYSIS", System_Parameter_Names, N_SYSTEM_PAR)] = OfflineAnalysis;
  retval = Pixie_User_Par_IO(user_par_values, "OFFLINE_ANALYSIS", "SYSTEM", MOD_WRITE, 0, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Boot_Modules): failed to initialize OFFLINE_ANALYSIS.");
    Pixie_Print_MSG(ErrMSG,1);
    return(-10);
  }
  user_par_values[Find_Xact_Match("MAX_NUMBER_MODULES", System_Parameter_Names, N_SYSTEM_PAR)] = MaxNumberOfModules;
  retval = Pixie_User_Par_IO(user_par_values, "MAX_NUMBER_MODULES", "SYSTEM", MOD_WRITE, 0, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Boot_Modules): failed to initialize MAX_NUMBER_MODULES.");
    Pixie_Print_MSG(ErrMSG,1);
    return(-11);
  }
  idx = Find_Xact_Match("SLOT_WAVE", System_Parameter_Names, N_SYSTEM_PAR);
  for(k=0; k<TotalNumberModules; k++)
  {
    user_par_values[idx + k] = SerialMap[k];
  }
  retval = Pixie_User_Par_IO(user_par_values, "SLOT_WAVE", "SYSTEM", MOD_WRITE, 0, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Boot_Modules): failed to initialize SLOT_WAVE.");
    Pixie_Print_MSG(ErrMSG,1);
    return(-12);
  }
  
  /* Boot all modules present in the system */
  retval = Pixie_Boot_System(Boot_Pattern);
  return(retval);
}


/****************************************************************
 *	Pixie_Adjust_DCOffsets:
 *		Adjust DC-offsets in one Pixie module.
 *
 *		Return Value:
 *			 0 - adjusting dc-offset successful
 *			-1 - invalid module number
 *			-2 - failed to adjust dc-offsets
 *			-3 - failed to set BASELINE_PERCENT
 *
 ****************************************************************/

S32 _stdcall Pixie_Adjust_DCOffsets (
  double BaselinePercent,
  U16 ModNum )
{
  
  S32 retval;
  U16 k, idx, offset;
  double user_par_values[N_CHANNEL_PAR * NUMBER_OF_CHANNELS * PRESET_MAX_MODULES];
  U32 dummy[4];
  
  /* First, check if ModNum is valid */
  if(ModNum >= Number_Modules)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Adjust_DCOffsets): invalid Module # = %d (should be less than %d)", ModNum, Number_Modules);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  
  /* First, download baseline percent to the module */
  offset = N_CHANNEL_PAR * NUMBER_OF_CHANNELS * ModNum;
  idx = Find_Xact_Match("BASELINE_PERCENT", Channel_Parameter_Names, N_CHANNEL_PAR);
  for(k=0; k<NUMBER_OF_CHANNELS; k++)
  {
    user_par_values[offset + k * N_CHANNEL_PAR + idx] = BaselinePercent;
    retval = Pixie_User_Par_IO(user_par_values, "BASELINE_PERCENT", "CHANNEL", MOD_WRITE, (U8)ModNum, (U8)k);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Adjust_DCOffsets): failed to set BASELINE_PERCENT in Module %d Channel %d", ModNum, k);
      Pixie_Print_MSG(ErrMSG,1);
      return(-3);
    }
  }
  
  /* Now, adjust offsets in the module */
  retval = Pixie_Acquire_Data(0x0083, dummy, "", (U8)ModNum);
  return(retval);
}


/****************************************************************
 *	Pixie_Acquire_ADCWaveforms:
 *		Acquire ADC waveforms in one Pixie module.
 *
 *		Return Value:
 *			 0 - acquiring ADC waveforms successful
 *			-1 - invalid module number
 *			-2 - NULL ADCWaveforms pointer
 *			-3 - can't find parameter XDT
 *			-4 - failed to set XDT
 *			-5 - failed to acquire ADC waveforms
 *
 ****************************************************************/

S32 _stdcall Pixie_Acquire_ADCWaveforms (
  double Xdt,
  U16 *ADCWaveforms,
  U16 ModNum )
{
  
  S32 retval;
  U16 k, idx, offset;
  double user_par_values[N_CHANNEL_PAR * NUMBER_OF_CHANNELS * PRESET_MAX_MODULES];
  U32 tempdata[IO_BUFFER_LENGTH * NUMBER_OF_CHANNELS];
  
  /* First, check if ModNum is valid */
  if(ModNum >= Number_Modules)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_ADCWaveforms): invalid Module # = %d (should be less than %d)", ModNum, Number_Modules);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  
  if(ADCWaveforms == NULL)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_ADCWaveforms): NULL ADCWaveforms pointer");
    Pixie_Print_MSG(ErrMSG,1);
    return(-2);
  }
  
  /* Download Xdt to the module */
  offset = N_CHANNEL_PAR * NUMBER_OF_CHANNELS * ModNum;
  idx = Find_Xact_Match("XDT", Channel_Parameter_Names, N_CHANNEL_PAR);
  if(idx == (N_CHANNEL_PAR - 1))
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_ADCWaveforms): can't find parameter XDT");
    Pixie_Print_MSG(ErrMSG,1);
    return(-3);
  }
  for(k=0; k<NUMBER_OF_CHANNELS; k++)
  {
    user_par_values[offset + k * N_CHANNEL_PAR + idx] = Xdt;
    retval = Pixie_User_Par_IO(user_par_values, "XDT", "CHANNEL", MOD_WRITE, (U8)ModNum, (U8)k);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_ADCWaveforms): failed to set XDT in Module %d Channel %d", ModNum, k);
      Pixie_Print_MSG(ErrMSG,1);
      return(-4);
    }
  }
  
  /* Now, acquire ADC waveforms in the module */
  retval = Pixie_Acquire_Data(0x0004, tempdata, "", (U8)ModNum);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Acquire_ADCWaveforms): failed to acquire ADC waveforms in Module %d", ModNum);
    Pixie_Print_MSG(ErrMSG,1);
    return(-5);
  }
  /* Transfer data to *ADCWaveforms */
  for(k=0; k<(IO_BUFFER_LENGTH * NUMBER_OF_CHANNELS); k++)
  {
    ADCWaveforms[k] = (U16)tempdata[k];
  }	
  return(0);
}


/****************************************************************
 *	Pixie_Start_MCARun:
 *		Start or resume MCA run in all Pixie modules.
 *
 *		Return Value:
 *			 0 - starting MCA run successful
 *			-1 - failed to start MCA run
 *			-2 - failed to resume MCA run
 *			-3 - wrong Mode specified
 *
 ****************************************************************/

S32 _stdcall Pixie_Start_MCARun (
  U16 Mode )
{
  
  S32 retval;
  U32 dummy[4];
  
  if(Mode == NEW_RUN)
  {
    retval = Pixie_Acquire_Data(0x1301, dummy, "", (U8)Number_Modules);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Start_MCARun): failed to start MCA run, retval = %d", retval);
      Pixie_Print_MSG(ErrMSG,1);
      return(-1);
    }
  }
  else if(Mode == RESUME_RUN)
  {
    retval = Pixie_Acquire_Data(0x2301, dummy, "", (U8)Number_Modules);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Start_MCARun): failed to resume MCA run, retval = %d", retval);
      Pixie_Print_MSG(ErrMSG,1);
      return(-2);
    }
  }
  else
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Start_MCARun): wrong Mode specified %d (should be either NEW_RUN(1) or RESUME_RUN(0))", Mode);
    Pixie_Print_MSG(ErrMSG,1);
    return(-3);
  }
  return(0);
  
}

/****************************************************************
 *	Pixie_Start_ListModeRun:
 *		Start or resume list mode run in all Pixie modules.
 *    Run types 0x100-103 for Pixie-4
 *
 *		Return Value:
 *			 0 - starting list mode run successful
 *			-1 - invalid list mode run type
 *			-2 - failed to start list mode run
 *			-3 - failed to resume list mode run
 *			-4 - wrong Mode specified
 *
 ****************************************************************/

S32 _stdcall Pixie_Start_ListModeRun (
	U16 Run_Type,
	U16 Mode)
{

	S32 retval;
	U32 dummy[4];

	/* First, check if Run_Type is valid */
	if( (Run_Type != 0x100) && (Run_Type != 0x101) && (Run_Type != 0x102) && (Run_Type != 0x103)  )
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun): invalid list mode run type %d", Run_Type);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	if(Mode == NEW_RUN)
	{
		retval = Pixie_Acquire_Data((U16)(0x1000 + Run_Type), dummy, "", (U8)Number_Modules);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun): failed to start list mode run, retval = %d", retval);
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);
		}
	}
	else if(Mode == RESUME_RUN)
	{
		retval = Pixie_Acquire_Data((U16)(0x2000 + Run_Type), dummy, "", (U8)Number_Modules);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun): failed to resume list mode run, retval = %d", retval);
			Pixie_Print_MSG(ErrMSG,1);
			return(-3);
		}
	}
	else
	{
		sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun): wrong Mode specified %d (should be either NEW_RUN(1) or RESUME_RUN(0))", Mode);
		Pixie_Print_MSG(ErrMSG,1);
		return(-4);
	}
	return(0);

}



/****************************************************************
 *	Pixie_Start_ListModeRun_400:
 *		Start or resume list mode run in all Pixie modules.
 *
 *		Return Value:
 *			 0 - starting list mode run successful
 *			-1 - invalid list mode run type
 *			-2 - failed to start list mode run
 *			-3 - failed to resume list mode run
 *			-4 - wrong Mode specified
 *
 ****************************************************************/

S32 _stdcall Pixie_Start_ListModeRun_400 (
  U16 Run_Type,
  U16 Mode,
  char *ListModeDataFile)
{
  
  S32 retval;
  U32 dummy[4];

//sprintf(ErrMSG, "*DEBUG* (Pixie_Start_ListModeRun): about to start run");
//Pixie_Print_MSG(ErrMSG,1);  
 
  /* First, check if Run_Type is valid */
  if(Run_Type != 0x400) 
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun_400): invalid list mode run type %d", Run_Type);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  

    retval = Pixie_Acquire_Data((U16)(0x1000 + Run_Type), dummy, ListModeDataFile, (U8)Number_Modules);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Start_ListModeRun_400): failed to start list mode run, retval = %d", retval);
      Pixie_Print_MSG(ErrMSG,1);
      return(-2);
    }

  return(0);
  
}

/****************************************************************
 *	Pixie_Check_RunStatus:
 *		Check run status in one Pixie module.
 *
 *		Return Value:
 *			 0 - no run is in progress
 *			 1 - run is still in progress
 *			-1 - failed to check run status
 *
 ****************************************************************/

S32 _stdcall Pixie_Check_RunStatus( 
	U16 ModNum
	)
{
  
  S32 retval;
  U32 dummy[4];
  
  retval = Pixie_Acquire_Data(0x4000, dummy, "", 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Check_RunStatus): failed to check run status, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(retval);
  
}




/****************************************************************
 *	Pixie_Check_RunStatus_400:
 *		Check run status in one Pixie module.
 *
 *		Return Value:
 *			 0 - no run is in progress
 *			 1 - run is still in progress
 *			-1 - failed to check run status
 *
 ****************************************************************/

S32 _stdcall Pixie_Check_RunStatus_400(
 	 U16 ModNum, 
  	 U16 Run_Type,
 	 char *ListModeDataFile
  )
{
   
  S32 retval;
  U32 dummy[4];

  retval = Pixie_Acquire_Data(0x4400, dummy, ListModeDataFile, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Check_RunStatus_400): failed to check run status, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }

//   sprintf(ErrMSG, "*DEBUG* (Pixie_Check_RunStatus_400): check run status: %d", retval);
//	  Pixie_Print_MSG(ErrMSG,1);  
   return(retval);
  
}


/****************************************************************
 *	Pixie_Check_RunData:
 *		Check run status in one Pixie module and return new LM data 
 *    Pixie-4e only
 *    C-CONTROL bits must be set for polling and polling with data before the run
 *	  The array for the LM data should be at least 2MB.  
 *	  The module must be set up to record special Run Statistics records in 
 *       list mode, triggered by an external Veto signal 
 *
 *		Return Value:
 *			 0 - no run is in progress
 *			 1 - run is still in progress
 *			-1 - failed to check run status
 *
 ****************************************************************/

S32 _stdcall Pixie_Check_RunData(
 	 U16 ModNum, 
  	 U16 Run_Type,
    U32 *NewLMdata,		   // array for new LM data.
 	 char *ListModeDataFile
  )
{
   
  S32 retval;
 
  retval = Pixie_Acquire_Data(0x4400, NewLMdata, ListModeDataFile, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Check_RunData): failed to check run status, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }

//   sprintf(ErrMSG, "*DEBUG* (Pixie_Check_RunData): check run status: %d", retval);
//	  Pixie_Print_MSG(ErrMSG,1);  
   return(retval);
  
}




/****************************************************************
 *	Pixie_Get_New_LMdata:
 *		Read the new data in the LM file while a run is in progress.
 *		The array for the LM data should be at least 10MB.  
 *		It must handle N 2MB buffers at polling interval; N is a function of count rate and event data length
 *		Bad things WILL happen if there is more new data than the length of the array
 *
 *		Return Value:
 *			>=0 pointer (in 16bit words) to the file location last read from
 *			< 0 some kind of error
 *
 ****************************************************************/

S32 _stdcall Pixie_Get_New_LMdata(
 	 U16 ModNum, 
	 U32 fileposread,		// file position (16bit words) to start read from
	 U32 *NewLMdata,		   // array for new LM data. 
 	 char *ListModeDataFile
  )
{
   
  S32 retval; 

  NewLMdata[0] = fileposread;
  retval = Pixie_Acquire_Data(0x40FA, NewLMdata, ListModeDataFile, ModNum);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Get_New_LMdata): failed to read data, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
	 return(-1);
  }
 
  return(retval);
  
}


/****************************************************************
 *	Pixie_Stop_Run:
 *		Stop data acquisition run in all Pixie modules.
 *
 *		Return Value:
 *			 0 - stopping run successful
 *			-1 - failed to start MCA run
 *			-2 - failed to resume MCA run
 *			-3 - wrong Mode specified
 *
 ****************************************************************/

S32 _stdcall Pixie_Stop_Run()
{
  
  S32 retval;
  U32 dummy[4];
//  sprintf(ErrMSG, "*INFO* (Pixie_Stop_Run): Pixie_Stop_Run"); 
//  Pixie_Print_MSG(ErrMSG,1);
  
  retval = Pixie_Acquire_Data(0x3000 , dummy, "", (U8)Number_Modules);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Stop_Run): failed to stop run, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}

/****************************************************************
 *	Pixie_Stop_Run_400:
 *		Stop data acquisition run in all Pixie modules for typ 0x400.
 *
 *		Return Value:
 *			 0 - stopping run successful
 *			-1 - failed to start MCA run
 *			-2 - failed to resume MCA run
 *			-3 - wrong Mode specified
 *
 ****************************************************************/

S32 _stdcall Pixie_Stop_Run_400 (
	U16 Run_Type,
 	char *ListModeDataFile	)
{
  
  S32 retval;
  U32 dummy[4];
//  sprintf(ErrMSG, "*INFO* (Pixie_Stop_Run): Pixie_Stop_Run"); 
//  Pixie_Print_MSG(ErrMSG,1);
  
  retval = Pixie_Acquire_Data(0x3000 + Run_Type, dummy, ListModeDataFile, (U8)Number_Modules);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Stop_Run): failed to stop run, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}

/****************************************************************
 *	Pixie_Read_MCAHistograms:
 *		Read MCA histograms from the external memory in one Pixie module.
 *		All 4 channels are read out at the same time, and the host
 *		needs to split them into 4 individual histograms.
 *
 *		Return Value:
 *			 0 - reading MCA histograms successful
 *			-1 - Invalid module number
 *			-2 - NULL MCAHistograms pointer
 *			-3 - failed to read MCA histograms
 *
 ****************************************************************/

S32 _stdcall Pixie_Read_MCAHistograms (
  U32 *MCAHistograms,
  U16 ModNum )
{
  
  S32 retval;
  
  /* First, check if ModNum is valid */
  if(ModNum >= Number_Modules)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_MCAHistograms): invalid Module # = %d (should be less than %d)", ModNum, Number_Modules);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  
  if(MCAHistograms == NULL)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_MCAHistograms): NULL MCAHistograms pointer");
    Pixie_Print_MSG(ErrMSG,1);
    return(-2);
  }
  
  /* Now, read histograms from the module */
  retval = Pixie_Acquire_Data(0x9001, MCAHistograms, "", (U8)ModNum);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_MCAHistograms): failed to read MCA histograms in Module # %d, retval = %d", ModNum, retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-3);
  }
  return(0);
  
}

/****************************************************************
 *	Pixie_Read_8KofMCAHistograms:
 *		Read first 8K of MCA histograms from the external memory in one Pixie module.
 *
 *		Return Value:
 *			 0 - reading MCA histograms successful
 *			-1 - Invalid module number
 *			-2 - NULL MCAHistograms pointer
 *			-3 - failed to read MCA histograms
 *
 ****************************************************************/

S32 _stdcall Pixie_Read_8KofMCAHistograms (
  U32 *MCAHistograms,
  U32 Nwords,
  U16 ModNum )
{
  
  S32 retval;
  
  /* First, check if ModNum is valid */
  if(ModNum >= Number_Modules)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_8KofMCAHistograms): invalid Module # = %d (should be less than %d)", ModNum, Number_Modules);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  
  if(MCAHistograms == NULL)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_8KofMCAHistograms): NULL MCAHistograms pointer");
    Pixie_Print_MSG(ErrMSG,1);
    return(-2);
  }
  
  /* Now, read histograms from the module */
  MCAHistograms[0] = Nwords;
  retval = Pixie_Acquire_Data(0x9007, MCAHistograms, "", (U8)ModNum);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_8KofMCAHistograms): failed to read MCA histograms in Module # %d, retval = %d", ModNum, retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-3);
  }
  return(0);
  
}



/****************************************************************
 *	Pixie_Save_MCAHistograms:
 *		Read MCA histograms from the external memory in all Pixie modules
 *		and save the histograms to a file in binary format.
 *
 *		Return Value:
 *			 0 - saving MCA histograms to file successful
 *			-1 - failed to save MCA histograms to file
 *
 ****************************************************************/

S32 _stdcall Pixie_Save_MCAHistograms (
  char *MCAHistogramFile )
{
  
  S32 retval;
  U32 dummy[4];
  
  /* Read histograms from modules and save them to a file */
  retval = Pixie_Acquire_Data(0x5000, dummy, MCAHistogramFile, (U8)Number_Modules);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Save_MCAHistograms): failed to save MCA histograms to file, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}


/****************************************************************
 *	Pixie_Save_ListModeData:
 *		Read list mode data from the external memory in all Pixie modules
 *		and save it to a file in binary format.
 *
 *		Return Value:
 *			 0 - saving list mode data to file successful
 *			-1 - failed to save list mode data to file
 *
 ****************************************************************/

S32 _stdcall Pixie_Save_ListModeData (
  	char *ListModeDataFile )
{
  
  S32 retval;
  U32 dummy[4];
  
  /* Read list mode data from modules and save it to a file */
  /* 0x100-103 are treated the same here */
  retval = Pixie_Acquire_Data(0x6100, dummy, ListModeDataFile, (U8)Number_Modules);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Save_ListModeData): failed to save list mode data to file, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}

/****************************************************************
 *	Pixie_Save_Settings:
 *		Read DSP parameters from all Pixie modules
 *		and save these parameters to a file in binary format.
 *
 *		Return Value:
 *			 0 - saving DSP parameters to file successful
 *			-1 - failed to save DSP parameters to file
 *
 ****************************************************************/

S32 _stdcall Pixie_Save_Settings (
  char *DSPSettingsFile )
{
  
  S32 retval;
  U16 dummy[4];
  
  
  /* Read histograms from modules and save them to a file */
  retval = Pixie_Buffer_IO(dummy, 2, 0, DSPSettingsFile, 0);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Save_Settings): failed to save DSP parameters to file, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}


/****************************************************************
 *	Pixie_Write_User_Par:
 *		Write user parameter values to one Pixie module.
 *
 *		Return Value:
 *			 0 - writing user parameter to module successful
 *			-1 - failed to write SYSTEM parameter
 *			-2 - failed to write MODULE parameter
 *			-3 - failed to write CHANNEL parameter
 *			-4 - invalid user parameter type
 *
 ****************************************************************/


S32 _stdcall Pixie_Write_User_Par (
  double User_Par_Value,		// user parameter to be transferred
  char *User_Par_Name,		// user parameter name
  char *User_Par_Type,		// user parameter type
  U16  ModNum,				// number of the module to work on
  U16  ChanNum )				// channel number of the Pixie module
{
  
  S32 retval;
  double user_par_values[NUMBER_OF_CHANNELS*N_CHANNEL_PAR*PRESET_MAX_MODULES];
  U16 offset;
  
  if(strcmp(User_Par_Type,"SYSTEM")==0)
  {
    user_par_values[Find_Xact_Match(User_Par_Name, System_Parameter_Names, N_SYSTEM_PAR)] = User_Par_Value;
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "SYSTEM", MOD_WRITE, 0, 0);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Write_User_Par): failed to write SYSTEM parameter %s to Pixie-4 modules", User_Par_Name);
      Pixie_Print_MSG(ErrMSG,1);
      return(-1);
    }
  }
  else if(strcmp(User_Par_Type,"MODULE")==0)
  {
    offset = ModNum * N_MODULE_PAR;
    user_par_values[Find_Xact_Match(User_Par_Name, Module_Parameter_Names, N_MODULE_PAR)+offset] = User_Par_Value;
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "MODULE", MOD_WRITE, (U8)ModNum, 0);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Write_User_Par): failed to write MODULE parameter %s to Pixie-4 module # %d", User_Par_Name, ModNum);
      Pixie_Print_MSG(ErrMSG,1);
      return(-2);
    }
  }
  else if(strcmp(User_Par_Type,"CHANNEL")==0)
  {
    offset = ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + ChanNum*N_CHANNEL_PAR;
    user_par_values[Find_Xact_Match(User_Par_Name, Channel_Parameter_Names, N_CHANNEL_PAR)+offset] = User_Par_Value;
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "CHANNEL", MOD_WRITE, (U8)ModNum, (U8)ChanNum);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Write_User_Par): failed to write CHANNEL parameter %s to Pixie-4 module %d channel %d", User_Par_Name, ModNum, ChanNum);
      Pixie_Print_MSG(ErrMSG,1);
      return(-3);
    }
  }
  else
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Write_User_Par): invalid user parameter type, type=%s", User_Par_Type);
    Pixie_Print_MSG(ErrMSG,1);
    return(-4);
  }
  return(0);
}


/****************************************************************
 *	Pixie_Read_User_Par:
 *		Read user parameter values from one Pixie module.
 *
 *		Return Value:
 *			 0 - reading user parameter from module successful
 *			-1 - failed to read SYSTEM parameter
 *			-2 - failed to read MODULE parameter
 *			-3 - failed to read CHANNEL parameter
 *			-4 - invalid user parameter type
 *
 ****************************************************************/

S32 _stdcall Pixie_Read_User_Par (
  double *User_Par_Value,		// user parameter to be transferred
  char *User_Par_Name,		// user parameter name
  char *User_Par_Type,		// user parameter type
  U16  ModNum,				// number of the module to work on
  U16  ChanNum )				// channel number of the Pixie module
{
  
  S32 retval;
  double user_par_values[NUMBER_OF_CHANNELS*N_CHANNEL_PAR*PRESET_MAX_MODULES];
  U16 offset;
  
  if(strcmp(User_Par_Type,"SYSTEM")==0)
  {
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "SYSTEM", MOD_READ, 0, 0);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Read_User_Par): failed to read SYSTEM parameter %s from Pixie-4 modules", User_Par_Name);
      Pixie_Print_MSG(ErrMSG,1);
      return(-1);
    }
    *User_Par_Value = user_par_values[Find_Xact_Match(User_Par_Name, System_Parameter_Names, N_SYSTEM_PAR)];
  }
  else if(strcmp(User_Par_Type,"MODULE")==0)
  {
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "MODULE", MOD_READ, (U8)ModNum, 0);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Read_User_Par): failed to read MODULE parameter %s from Pixie-4 module # %d", User_Par_Name, ModNum);
      Pixie_Print_MSG(ErrMSG,1);
      return(-2);
    }
    offset = ModNum * N_MODULE_PAR;
    *User_Par_Value = user_par_values[Find_Xact_Match(User_Par_Name, Module_Parameter_Names, N_MODULE_PAR)+offset];
  }
  else if(strcmp(User_Par_Type,"CHANNEL")==0)
  {
    retval = Pixie_User_Par_IO(user_par_values, User_Par_Name, "CHANNEL", MOD_READ, (U8)ModNum, (U8)ChanNum);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_Read_User_Par): failed to read CHANNEL parameter %s from Pixie-4 module %d channel %d", User_Par_Name, ModNum, ChanNum);
      Pixie_Print_MSG(ErrMSG,1);
      return(-3);
    }
    offset = ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + ChanNum*N_CHANNEL_PAR;
    *User_Par_Value = user_par_values[Find_Xact_Match(User_Par_Name, Channel_Parameter_Names, N_CHANNEL_PAR)+offset];
  }
  else
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_User_Par): invalid user parameter type, type=%s", User_Par_Type);
    Pixie_Print_MSG(ErrMSG,1);
    return(-4);
  }
  return(0);
}


/****************************************************************
 *	Pixie_Read_ListModeData:
 *		Parse and read list mode data from a list mode data file.
 *		Definitions of input variable "type":
 *			0x7001: parse the list mode data file to get the number
 *			        of events for each module.
 *			0x7007: locate the starting point, length and energy of
 *			        each waveform.
 *			0x7008: read specific event header, channel headers and
 *			        waveform.			
 *
 *		Return Value:
 *			 0 - reading list mode data successful
 *			-1 - failed to read list mode data
 *
 ****************************************************************/

S32 _stdcall Pixie_Read_ListModeData (
  char *ListModeDataFile,
  U32 *ListModeData,
  U16 type )
{
  
  S32 retval;
  
  /* Parse or read list mode data from a list mode data file */
  retval = Pixie_Acquire_Data(type, ListModeData, ListModeDataFile, (U8)Number_Modules);
  if(retval < 0)
  {
    sprintf(ErrMSG, "*ERROR* (Pixie_Read_ListModeData): failed to read list mode data, retval = %d", retval);
    Pixie_Print_MSG(ErrMSG,1);
    return(-1);
  }
  return(0);
  
}


/****************************************************************
 *	Pixie_ControlTaskRun:
 *
 *		Return Value:
 *			 0 - execute successful
 *			-1 - failed to to execute
 *
 ****************************************************************/

S32 _stdcall Pixie_ControlTaskRun (
  U16 ModNum,			// number of the module to work on
  U16 ControlTask,	// control task type
  U32 MaxPoll )		// maximum polling time before timed out
{
  
  S32 retval;
  unsigned long dummy[2];
  
  if(ControlTask == ADJUST_BLCUT)
  {
    /* Find BLCut values for all channels in all modules */
    retval = Pixie_Acquire_Data (ADJUST_BLCUT, dummy, "", Number_Modules);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_ControlTaskRun): failed to execute control task %d in module %d, retval = %d", ControlTask, ModNum, retval);
      Pixie_Print_MSG(ErrMSG,1);
      return(-1);
    }
  }
  else
  {
    retval = Control_Task_Run((U8)ModNum, (U8)ControlTask, MaxPoll);
    if(retval < 0)
    {
      sprintf(ErrMSG, "*ERROR* (Pixie_ControlTaskRun): failed to execute control task %d in module %d, retval = %d", ControlTask, ModNum, retval);
      Pixie_Print_MSG(ErrMSG,1);
      return(-1);
    }
  }
  return(0);
  
}
