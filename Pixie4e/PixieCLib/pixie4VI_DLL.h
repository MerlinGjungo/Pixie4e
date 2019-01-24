#ifndef __PIXIE4VI_DLL_H
#define __PIXIE4VI_DLL_H

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
 * File Name:
 *
 *     pixie4VI_DLL.h
 *
 * Description:
 *
 *     This file contains all the Pixie-4 DLL function prototypes to be called by
 *     the LabVIEW VIs.
 *
 * Revision:
 *
 *     5-3-2006
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


_declspec(dllexport) S32 _stdcall Pixie_Download_FileNames (
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
  char *reserved6);

_declspec(dllexport) S32 _stdcall Pixie_Boot_Modules (
	U16 TotalNumberModules,
	U16 OfflineAnalysis,
	U16 MaxNumberOfModules,
	U16 SerialMap[PRESET_MAX_MODULES],
	U16 Boot_Pattern );	// bit mask

_declspec(dllexport) S32 _stdcall Pixie_Adjust_DCOffsets (
	double BaselinePercent,
	U16 ModNum );

_declspec(dllexport) S32 _stdcall Pixie_Acquire_ADCWaveforms (
	double Xdt,
	U16 *ADCWaveforms,
	U16 ModNum );

_declspec(dllexport) S32 _stdcall Pixie_Read_MCAHistograms (
	U32 *MCAHistograms,
	U16 ModNum );

_declspec(dllexport) S32 _stdcall Pixie_Read_8KofMCAHistograms (
  U32 *MCAHistograms,
  U32 Nwords,
  U16 ModNum );

_declspec(dllexport) S32 _stdcall Pixie_Start_MCARun (
	U16 Mode );

_declspec(dllexport) S32 _stdcall Pixie_Stop_Run_400 (	
   U16 Run_Type,
	char  *ListModeDataFile );

_declspec(dllexport) S32 _stdcall Pixie_Stop_Run ();

_declspec(dllexport) S32 _stdcall Pixie_Save_MCAHistograms (
	char *MCAHistogramFile );

_declspec(dllexport) S32 _stdcall Pixie_Save_Settings (
	char *DSPSettingsFile );

_declspec(dllexport) S32 _stdcall Pixie_Start_ListModeRun (
	U16 Run_Type,
	U16 Mode);

_declspec(dllexport) S32 _stdcall Pixie_Start_ListModeRun_400 (
	U16 Run_Type,
	U16 Mode, 
	char *ListModeDataFile);  

_declspec(dllexport) S32 _stdcall Pixie_Check_RunStatus (
	U16 ModNum);

_declspec(dllexport) S32 _stdcall Pixie_Check_RunStatus_400 (
	U16 ModNum, 
  	U16 Run_Type,
	char *ListModeDataFile);

_declspec(dllexport) S32 _stdcall Pixie_Check_RunData(
 	 U16 ModNum, 
  	 U16 Run_Type,
    U32 *NewLMdata,		   // array for new LM data.
 	 char *ListModeDataFile );

_declspec(dllexport) S32 _stdcall Pixie_Get_New_LMdata (
 	 U16 ModNum, 
	 U32 fileposread,		
	 U32 *NewLMdata,		  
 	 char *ListModeDataFile);

_declspec(dllexport) S32 _stdcall Pixie_Save_ListModeData (
	char *ListModeDataFile );

_declspec(dllexport) S32 _stdcall Pixie_Write_User_Par (
			double User_Par_Value,		// user parameter to be transferred
			char *User_Par_Name,		// user parameter name
			char *User_Par_Type,		// user parameter type
			U16  ModNum,				// number of the module to work on
			U16  ChanNum );				// channel number of the Pixie module

_declspec(dllexport) S32 _stdcall Pixie_Read_User_Par (
			double *User_Par_Value,		// user parameter to be transferred
			char *User_Par_Name,		// user parameter name
			char *User_Par_Type,		// user parameter type
			U16  ModNum,				// number of the module to work on
			U16  ChanNum );				// channel number of the Pixie module

_declspec(dllexport) S32 _stdcall Pixie_Read_ListModeData (
	char *ListModeDataFile,
	U32 *ListModeData,
	U16 type );

_declspec(dllexport) S32 _stdcall Pixie_ControlTaskRun (
	U16 ModNum,			// number of the module to work on
	U16 ControlTask,	// control task type
	U32 MaxPoll );		// maximum polling time before timed out

#ifdef __cplusplus
}
#endif

#endif   /* End of pixie4VI_DLL.h */
