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
*      Utilities.c
*
* Description:
*
*      This file contains all the boot, control and other utilities functions
*		for Pixie.
*
* Revision:
*
*		12-1-2004
*
* Member functions:
*	1) Run control functions:
*		Check_Run_Status, Control_Task_Run, End_Run, Get_Traces, Get_Slow_Traces
*		Start_Run, Run_Enable_Set, Run_Enable_Clear, Read_Resume_Run
*
*	2) Pixie memory and file I/O functions:
*		Pixie_IODM, Pixie_IOEM, Create_List_Mode_File
*		Read_Spectrum_File, Write_List_Mode_File, Write_Spectrum_File, Write_DMA_List_Mode_File 
*		Pixie_Register_IO, Pixie_RdWrdCnt, Pixie_ReadCSR , Pixie_WrtCSR, Pixie_ReadVersion.
*
*	3) Pixie User Parameter I/O functions used not only in ua_par_io:
*	    BLcut_Finder, Make_SGA_Gain_Table, Pixie_CopyExtractSettings :		 	
*
*	4) Pixie automatic optimization functions:
*		Phi_Value, Linear_Fit, RandomSwap, Tau_Finder,
*		Tau_Fit, Thresh_Finder, Adjust_Offsets, Adjust_Offsets_DSP 
*
*	5) Utility functions:
*		ClrBit, Find_Xact_Match, RoundOff, FlushIgorMSG, Pixie_Print_MSG
*		SetBit, TglBit, TstBit, Pixie_Sleep	
*
*   6) Functions that apply differences for different board types and variants
*       Pixie_Define_Clocks, Pixie_Check_Moduletypes
*
*	7) Functions communicating with devices on the board
*		ADCSPI, Apply_default_I2E, 
*
******************************************************************************/


#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef XIA_WINDOZE
#include <io.h>
#endif
#ifdef XIA_LINUX
#include <unistd.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "PlxTypes.h"
#include "PciTypes.h"
#include "Plx.h"

#include "globals.h"
#include "sharedfiles.h"
#include "utilities.h"

#include "Reg9054.h"
#include "PexApi.h"


#ifdef COMPILE_IGOR_XOP
#include <XOPStandardHeaders.h>
#endif

/****************************************************************
*	Start_Run function:
*		Starts a new run or resumes a run in a Pixie module or multiple
*		modules.
*
*		Don't check Run Task or Control Task for validity.
*		The DSP code takes care of that. There are two legal
*		values for Type: NEW_RUN and RESUME_RUN.
*
*		Return Value:
*			 0 - success
*			-1 - failure to stop a previous run
*
****************************************************************/

S32 Start_Run (
			   U8  ModNum,			// Pixie module number
			   U8  Type,			// run type (NEW_RUN or RESUME_RUN)
			   U16 Run_Task,		// run task
			   U16 Control_Task )	// control task
{

	S32 retval;
	U32 address, value;
	U32 *buffer=NULL;
	U8  k;
	U32 dwStatus, CSR; 

	buffer = malloc(MCA2D_MEMORY_LENGTH * sizeof(U32));		// temp array to clear MCA memory. choose the larger of 2D or Nch x MCA memory
	if(!buffer){
		sprintf(ErrMSG, "*ERROR* (Start_Run): Memory allocation failure");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	memset(buffer, 0,MCA2D_MEMORY_LENGTH * sizeof(U32));	// set to zero

	if(ModNum == Number_Modules)  /* Start run in all modules */
	{
		/* Prepare for a run: ending a previous run, clear external memory, set parameters */
		for(k = 0; k < Number_Modules; k ++)
		{
			/* Check if there is a run in progresss; if so, end it */
			retval = Check_Run_Status(k);
			if(retval == 1)
			{
				retval = End_Run(k);
				/* Check if End_Run returned without errors */
				if(retval < 0)
				{
					sprintf(ErrMSG, "*ERROR* (Start_Run): End_Run failed in Module %d; return value=%d", k, retval);
					Pixie_Print_MSG(ErrMSG,1);
					return(-1);
				}
			}

			/* Clear external memory first before starting a new data acquisition run */
			if((Type == NEW_RUN) && (Run_Task != 0) && (Control_Task == 0))
			{
				// Standard MCA memory
				Pixie_IOEM(k, HISTOGRAM_MEMORY_ADDRESS, MOD_WRITE, MAX_HISTOGRAM_LENGTH*NUMBER_OF_CHANNELS, buffer);

				// Extra 2D memory
#ifdef WINDRIVER_API
				if (PCIBusType==EXPRESS_PCI) {
					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
					// Assert bit 6: high bit of MCA memory range
					CSR=(U32)SetBit(BIT_MCAUPPERA, (U16)CSR);	/* Set bit 6 of APP_HOST_CTL to select upper MCA range */
					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_READ, &CSR, FALSE);

					Pixie_IOEM(k, MCA2D_MEMORY_ADDRESS, MOD_WRITE, MCA2D_MEMORY_LENGTH, buffer);

					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
					// De-Assert bit 6: high bit of MCA memory range
					CSR=(U32)ClrBit(BIT_MCAUPPERA, (U16)CSR);	/* Set bit 6 of APP_HOST_CTL to select upper MCA range */
					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
					dwStatus = PIXIE500E_ReadWriteReg(hDev[k], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
				}
#endif

				/* Set RunTask */
				address=Run_Task_Index+DATA_MEMORY_ADDRESS;
				value=(U32)Run_Task;
				Pixie_IODM(k, address, MOD_WRITE, 1, &value);
			}
			if(Type == NEW_RUN)
			{
				/* Set RunTask */
			//	address=Run_Task_Index+DATA_MEMORY_ADDRESS;		control tasks should not modify runtask (any more)
			//	value=(U32)Run_Task;
			//	Pixie_IODM(k, address, MOD_WRITE, 1, &value);

				/* Set ControlTask */
				address=Control_Task_Index+DATA_MEMORY_ADDRESS;
				value=(U32)Control_Task;
				Pixie_IODM(k, address, MOD_WRITE, 1, &value);
			}

			/* Set RESUME */
			address=Resume_Index+DATA_MEMORY_ADDRESS;
			value=(U32)Type;
			Pixie_IODM(k, address, MOD_WRITE, 1, &value);
		}
		/* Set CSR to start run in all modules */
		for(k = 0; k < Number_Modules; k ++)
		{
			Run_Enable_Set(k,Run_Task);
		}
	}
	else if(ModNum < Number_Modules)  /* Start run in one module only */		
	{
		/* Check if there is a run in progresss; if so, end it */
		retval = Check_Run_Status(ModNum);
		if(retval == 1)
		{
			retval = End_Run(ModNum);
			/* Check if End_Run returned without errors */
			if(retval < 0)
			{
				sprintf(ErrMSG, "*ERROR* (Start_Run): End_Run failed in Module %d; return value=%d", ModNum, retval);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}
		}

		/* Clear external memory first before starting a new data acquisition run */
		if((Type == NEW_RUN) && (Run_Task != 0) && (Control_Task == 0))
		{
			Pixie_IOEM(ModNum, 0, MOD_WRITE, MAX_HISTOGRAM_LENGTH*NUMBER_OF_CHANNELS, buffer);

			// Extra 2D memory
#ifdef WINDRIVER_API
			if (PCIBusType==EXPRESS_PCI) {
				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
				// Assert bit 6: high bit of MCA memory range
				CSR=(U32)SetBit(BIT_MCAUPPERA, (U16)CSR);	/* Set bit 6 of APP_HOST_CTL to select upper MCA range */
				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);

				Pixie_IOEM(ModNum, MCA2D_MEMORY_ADDRESS, MOD_WRITE, MCA2D_MEMORY_LENGTH, buffer);

				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
				// De-Assert bit 6: high bit of MCA memory range
				CSR=(U32)ClrBit(BIT_MCAUPPERA, (U16)CSR);	/* Set bit 6 of APP_HOST_CTL to select upper MCA range */
				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
				dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
			}
#endif

			/* Set RunTask */
			address=Run_Task_Index+DATA_MEMORY_ADDRESS;
			value=(U32)Run_Task;
			Pixie_IODM(ModNum, address, MOD_WRITE, 1, &value);
		}

		if(Type == NEW_RUN)
		{

			/* Set ControlTask */
			address=Control_Task_Index+DATA_MEMORY_ADDRESS;
			value=(U32)Control_Task;
			Pixie_IODM(ModNum, address, MOD_WRITE, 1, &value);
		}

		/* Set RESUME */
		address=Resume_Index+DATA_MEMORY_ADDRESS;
		value=(U32)Type;
		Pixie_IODM(ModNum, address, MOD_WRITE, 1, &value);

		Run_Enable_Set(ModNum,Run_Task);
	}

	free(buffer);
	return(0);
}


/****************************************************************
*	End_Run function:
*		Ends run in a single Pixie module or multiple modules.
*
*		Return Value:
*			 0 - success
*			-1 - failure to end the run
*
****************************************************************/

S32 End_Run (
			 U8 ModNum )		// Pixie module number
{
	U32 retval, tcount;
	U32 address, value;
	U16 sumActive;
	U8 k;
	U16 Active[PRESET_MAX_MODULES];

	/* Set RESUME in DSP to 2 to signal a run-stop in all cards */
	for(k = 0; k < Number_Modules; k++)
	{
		address = Resume_Index + DATA_MEMORY_ADDRESS;
		value   = 2;
		Pixie_IODM((U8)k, address, MOD_WRITE, 1, &value);
	}

	/* Set SYNCHDONE in DSP to 1 to cause a synch-loop break-up in all cards */
	/* Conflicts with limits in IODM. Is ignored in P500e anyway because only input variables are copied to FPGA and read by DSP 
	for(k = 0; k < Number_Modules; k++)
	{
		address = SYNCHDONE_Index + DATA_MEMORY_ADDRESS;
		value   = 1;
		Pixie_IODM(k, address, MOD_WRITE, 1, &value);
	}  */

	if(ModNum == Number_Modules)  /* Stop run in all modules */
	{
		for(k = 0; k < Number_Modules; k ++)
		{
			Run_Enable_Clear(k);

			Active[k] = 1;  /* To be used for checking run status below */ 
		}

		/* Check if the run has been really ended */
		tcount = 0;
		sumActive = Number_Modules;

		do
		{
			for(k = 0; k < Number_Modules; k ++)
			{
				if(Active[k] == 1)
				{
					retval = Check_Run_Status((U8)k);
					if( retval == 0)
					{
						Active[k] = 0;	/* Clear Active for a module whose run is done */
						sumActive --;
					}
				}
			}

			if(sumActive == 0)
			{
				break;		/* Run in all modules is done */
			}

			Pixie_Sleep(1);
			tcount++;

		} while(tcount < 50); /* TimeOut = 50 ms */

		/* Check if there is any module whose run has not ended */
		for(k = 0; k < Number_Modules; k ++)
		{
			if(Active[k] == 1)
			{
				sprintf(ErrMSG, "*ERROR* (End_Run): Module %d failed to stop its run", k);
				Pixie_Print_MSG(ErrMSG,1);
			}
		}

		if(sumActive != 0)
		{
			return(-1);		/* Not all modules stopped the run successfully */
		}
		else
		{
			return(0);		/* All modules stopped the run successfully */
		}
	}
	else if(ModNum < Number_Modules)  /* Stop run in one module only */
	{		
		Run_Enable_Clear(ModNum);

		/* Check if the run has been really ended. */
		tcount=0;
		do
		{
			retval = Check_Run_Status(ModNum);
			if( retval == 0)
			{
				break;
			}

			Pixie_Sleep(1);
			tcount++;
		} while(tcount < 100); /* TimeOut = 100 ms */

		if(tcount == 100)  /* Timed out */
		{
			sprintf(ErrMSG, "*ERROR* (End_Run): Module %d failed to stop its run", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}
		else
		{
			return(0);		/* The module stopped the run successfully */
		}
	}

	return(0);
}

/****************************************************************
*	Read_Resume_Run function:
*		Reads data and resums run in Pixie modules after memory buffers are filled.
*
*	An attempt to minimize readout dead time, currently not fully tested
*		Return Value:
*			 0 - success
*			-1 - failure to end the run
*			-2 - invalid word count
*
****************************************************************/

S32 Read_Resume_Run (
			U8  ModNum,			// Pixie module number
			U8  Stop,		    // If 0, do not stop and resume if stopped (poll in any spill but last) 
								// if 1, do not stop but do not resume if stopped (last spill)
								// If 2, stop, then resume (spill timeout)
								// If 3, stop and do not resume (run timeout, new file). 
			S8  *FileName    )  // list mode file name
{

	U8    k;
	U32   CSR, RunActive, DataReady;
	S32	  retval=0;

	//sprintf(ErrMSG, "*INFO* (Read_Resume_Run): file %s",FileName);
	//Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

/**** End/Poll run part ******************************************************************/
	if(Stop>=2)
	{
		End_Run(Number_Modules);
	}

	Pixie_ReadCSR(ModNum, &CSR);
	RunActive = ((CSR & 0x2000) >0);
	DataReady = ((CSR & 0x4000) >0);
	
	
/**** Check end run part ***************************************************************/
	
	/* If stopped, check if the run has really ended in all modules */
	if(RunActive==0)
	{
		for(k = 0; k < Number_Modules; k++)
		{   
			if(Check_Run_Status((U8)k) != 0)  /* run not stopped */
			{
				sprintf(ErrMSG, "*ERROR* (Read_Resume_Run): Module %d failed to stop its run", k);
				Pixie_Print_MSG(ErrMSG,1);
				return(-3);
			}
		}
	}
	

/**** Write LMF part *******************************************************************/
	//if( (RunActive==0) | (DataReady ==1))
	if(DataReady ==1)
	{
		retval=Write_List_Mode_File(FileName);
		if(retval<0)
		{
			return(retval);
		}
		retval=1;		// set to 1 indicating one spill has been read
	}

	
/**** Resume run part ******************************************************************/

	/* Resume run in all modules */
	//if( ((Stop==2) | (Stop==0)) & (RunActive ==0))
	if( (RunActive==0) & (DataReady!=0))		// resume if module was stopped and data was ready. (if no data ready, must be a host stop)
	{
		for(k = 0; k < Number_Modules; k++)
		{
			/* Read CSR */
			Pixie_ReadCSR(k, &CSR);
			CSR = (U32)SetBit(0, (U16)CSR);	/* Set bit 0 of CSR to enable run */
			Pixie_WrtCSR(k, CSR);
		}
	}

/***** Return info ************************************************************************/
	if( (RunActive==0) & (DataReady==0))		// end MT polling if module was stopped and no data was ready. (must be a host stop)
	{
		MT_KeepPolling = 0; //stop the infinite polling loop
		sprintf(ErrMSG, "*INFO* (Read_Resume_Run): stopping infinite polling loop run");
		Pixie_Print_MSG(ErrMSG,1);
	}

	return(retval);		//0 = polled ok, no read, 1 = polled and read one spill
}
	


/****************************************************************
*	Check_Run_Status function:
*		Check if a run is in progress. Bit 13.
*
*		Return Value:
*			0:	no run in progress
*			1:	a run in progress
*
****************************************************************/

S32 Check_Run_Status (
					  U8 ModNum )			// Pixie module number
{

	U32 CSR;
	S32 isRunInProgress = -1;
	S32 dwStatus = -1;

	dwStatus = Pixie_ReadCSR(ModNum, &CSR);

	isRunInProgress = ((U16)CSR & 0x2000) >> 13;

	sprintf(ErrMSG, "*DEBUG* (Check_Run_Status): dwStatus =  %d, CSR = %x, active = %d",dwStatus,CSR,isRunInProgress);
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_daq);
	
	if (dwStatus<0 || CSR==0xFFFFFFFF) return(REGIO_ERR);

	return(isRunInProgress);
}


/****************************************************************
*	Control_Task_Run function:
*		Perform a control task run.
*
*		Return Value:
*			 0 - successful
*			-1 - failed to start the control task run
*			-2 - control task run timed out
*
****************************************************************/

S32 Control_Task_Run (
					  U8 ModNum,			// Pixie module number
					  U8 ControlTask,		// Control task number
					  U32 Max_Poll )		// Timeout control in unit of ms for control task run
{
	S32 retval;
	U32 count;
	
	if (Offline == 1) {
		sprintf(ErrMSG, "(Control_Task_Run): Offline mode. No I/O operations possible");
		Pixie_Print_MSG(ErrMSG,1);
		return (0);
	}

	/* Start control task run: NEW_RUN and RunTask = 0 */
	retval = Start_Run(ModNum, NEW_RUN, 0, ControlTask);
	if(retval < 0)
	{
		sprintf(ErrMSG, "*ERROR* (Control_Task_Run): Failure to start control task %d in Module %d", ControlTask, ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	/* A short wait before polling the run status */
	Pixie_Sleep(1);

	count=0;
	while(Check_Run_Status(ModNum) && (count<Max_Poll))
	{
		count++; /* The maximal waiting time is set by Max_Poll */ 
		Pixie_Sleep(1);
	}

	if(count>=Max_Poll)
	{
		sprintf(ErrMSG, "*ERROR* (Control_Task_Run): Control task %d in Module %d timed out", ControlTask, ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-2); /* Time Out */
	}
	else
	{
		return(0); /* Normal finish */
	}
}

/******************************************************************
* Set Bit 0 of  Pixie-4 CSR, or Set bit 0 of P4e/500e APP_HOST_CTL
*******************************************************************/
S32 Run_Enable_Set ( U8 ModNum, U16 task )
{
	S32 dwStatus, CSR;
	U16 startbit;
	dwStatus = -1;

	if(task<0x100)
		startbit = BIT_CTRLENA;		// controltasks start with this bit
	else
		startbit = BIT_RUNENA;		// DAQ tasks start with this bit

	switch (PCIBusType) {
		case REGULAR_PCI:
			/* Read CSR */
			dwStatus = Pixie_ReadCSR(ModNum, &CSR);
			CSR=(U32)SetBit(startbit, (U16)CSR);	/* Set bit 0 or 1 of CSR to enable run */
			Pixie_WrtCSR(ModNum, CSR);
			break;
		case EXPRESS_PCI:
#ifdef WINDRIVER_API
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
			// Assert bit 0
			CSR=(U32)SetBit(startbit, (U16)CSR);	/* Set bit 0 or 1 of APP_HOST_CTL to enable run */
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
#endif
			break;
		default:
			break;
	}
	return dwStatus;
}


/****************************************************************************
* Clear Bit 0,1 of  Pixie-4 CSR, or Clear bit 0,1 of APP_HOST_CTL of P4/500e
*****************************************************************************/
S32 Run_Enable_Clear ( U8 ModNum )
{
	S32 dwStatus, CSR;

	switch (PCIBusType) {
		case REGULAR_PCI:
			/* Read CSR */
			dwStatus = Pixie_ReadCSR(ModNum, &CSR);
			CSR=(U32)ClrBit(BIT_RUNENA, (U16)CSR);	/* Clear bit 0 of CSR to disable DAQ run */
			CSR=(U32)ClrBit(BIT_CTRLENA, (U16)CSR);	/* Clear bit 1 of CSR to disable ctrl run */
			Pixie_WrtCSR(ModNum, CSR);
			break;
		case EXPRESS_PCI:
#ifdef WINDRIVER_API
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
			// De-assert bits {BIT_CTRLENA,BIT_RUNENA} 
			CSR &= ~(0x3);
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
#endif
			break;
		default:
			break;
	}
	return dwStatus;
}

/****************************************************************
*	Pixie_Register_IO function:
*		Used for single-word I/O communications between Host
*		and Pixie registers.
*
****************************************************************/

S32 Pixie_Register_IO (
					   U16 ModNum,	// the Pixie module to communicate to
					   U32 address,	// register address
					   U16 direction,	// either MOD_READ or MOD_WRITE
					   U32 *value )	// holds or receives the data
{
	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);

	if (VAddr[ModNum] == 0) {            
		sprintf(ErrMSG, "*ERROR* (Pixie_Register_IO) - module address not mapped. Module=%d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-6);
	}


	/* Write to and read from register */
	if(direction == MOD_WRITE) *(U32*)(VAddr[ModNum]+address) = *value;
	if(direction == MOD_READ ) *value = *(U32*)(VAddr[ModNum]+address);

	return(0);
}


/****************************************************************
*	Pixie_RdWrdCnt:
*		Read word count register of the selected Pixie module to
*		obtain the number of 16-bit words in the linear I/O buffer.
*
****************************************************************/

S32 Pixie_RdWrdCnt (
					U8 ModNum,			// Pixie module number
					U32 *WordCount )	// word count value
{
	
	S32 dwStatus = -1;
	switch (PCIBusType) {
		case REGULAR_PCI:
			dwStatus = Pixie_Register_IO(ModNum, PCI_WCR, MOD_READ, WordCount);
			break;
		case EXPRESS_PCI:
#ifdef WINDRIVER_API
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, WordCount, FALSE);
#endif
			break;
		default:
			break;
	}
	return(dwStatus);

}


/****************************************************************
*	Pixie_ReadCSR:
*		Read Control Status Register (CSR) of the selected Pixie module.
*
****************************************************************/

S32 Pixie_ReadCSR (
				   U8 ModNum,			// Pixie module number
				   U32 *CSR )			// CSR value
{
	S32 dwStatus = -1;
	switch (PCIBusType) {
		case REGULAR_PCI:
			dwStatus = Pixie_Register_IO(ModNum, PCI_CSR, MOD_READ, CSR);
			break;
		case EXPRESS_PCI:
#ifdef WINDRIVER_API
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS, WDC_READ, CSR, FALSE);
			if(dwStatus==WD_WINDRIVER_STATUS_ERROR) // or FFFFFFFF
			{
			//	sprintf(ErrMSG, "*ERROR* (Pixie_ReadCSR): Fatal error in Module I/O");
			//	Pixie_Print_MSG(ErrMSG,1);
				return REGIO_ERR;		// fatal error causing system crash

			}
#endif
			break;
		default:
			break;
	}
	return(dwStatus);
}


/****************************************************************
*	Pixie_WrtCSR:
*		Write Control Status Register (CSR) to the selected Pixie module.
*
****************************************************************/

S32 Pixie_WrtCSR (
				  U8 ModNum,			// Pixie module number
				  U32 CSR )			// CSR value
{
	Pixie_Register_IO(ModNum, PCI_CSR, MOD_WRITE, &CSR);
	return(0);
}


/****************************************************************
*	Pixie_ReadVersion:
*		Read hardware version of the selected Pixie module.
*
****************************************************************/
S32 Pixie_ReadVersion (
					   U8 ModNum,			// Pixie module number
					   U32 *Version )		// Version number
{

	U32 offset, value, value2;
	UINT8 EEPROMbuffer[6];
	S32 retval = 0;
	switch (PCIBusType) {
		case REGULAR_PCI: // Pixie-4, PLX
			Pixie_Register_IO(ModNum, PCI_VERSION, MOD_READ, Version);
			break;
		case EXPRESS_PCI: // Pixie500e, Gennum
#ifdef WINDRIVER_API
			// Read directly from EEPROM
			offset = EEPROM_XIA_OFFSET*EEPROM_WD_SIZE; // EEPROM_WD_SIZE = 6 words, 7th entry is the revision and serial number
			retval = PIXIE500E_ReadI2C(hDev[ModNum], EEPROMbuffer, SLAVE_ADDR, offset, EEPROM_WD_SIZE);
			if( retval != EEPROM_WD_SIZE) {
				sprintf(ErrMSG, "*ERROR* (Pixie_ReadVersion): Error reading EEPROM at offset 0x%x", offset);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}
			value = EEPROMbuffer[0] | (EEPROMbuffer[1] << 8);
			value2 = EEPROMbuffer[2] | (EEPROMbuffer[3] << 8) | (EEPROMbuffer[4] << 16) | (EEPROMbuffer[5] << 24);
			//printf("EEPROM: 0x%04x = 0x%08x\r\n", value, value2);
			*Version = value2;
#endif
			break;
		default:
			sprintf(ErrMSG, "*ERROR* (Pixie_ReadVersion) - PCI type not defined. Module=%d", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
			break;
	}

	return(0);
}




/****************************************************************
*	Pixie_IODM function:
*		Used for all I/O communications between Host and Pixie data memory.
*
****************************************************************/
S32 Pixie_IODM (
				U8  ModNum,	// the Pixie module to communicate to
				U32 address,	// data memory address
				U8  direction,	// either MOD_READ or MOD_WRITE
				U16 nWords,	// the number of 32-bit words to be transferred
				U32 *buffer )	// holds or receives the data
{
	U16 k = 0;
	U32 dwData, dwStatus;
	U32 i, index;
	U32 timeoutPollDSP = 1000;		// normally, one polling cycle is sufficient

	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);

	if (VAddr[ModNum] == 0) {		// rlk
		sprintf(ErrMSG, "*ERROR* (Pixie_IODM) - module address not mapped. Module=%d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
		return(-6);
	}

	switch (PCIBusType) {
		case REGULAR_PCI:
			/* Set initial address to talk to */
			*(U32*)(VAddr[ModNum]+PCI_IDMAADDR) = address;
			/* Write or read to data memory */
			if (direction == MOD_WRITE) 
				for (k = 0; k < nWords; k++) 
					*(U32*)(VAddr[ModNum] + PCI_IDMADATA) = buffer[k];
			if (direction == MOD_READ ) 
				for (k = 0; k < nWords; k++) 
					buffer[k]                             = *(U32*)(VAddr[ModNum] + PCI_IDMADATA);
			break;
			/* Done */
		case EXPRESS_PCI:
#ifdef WINDRIVER_API
			
		
			if (direction == MOD_WRITE) {
//			sprintf(ErrMSG, "*INFO* (Pixie_IODM) write: address %d, nWords %d",address,nWords);
//			Pixie_Print_MSG(ErrMSG,1);
				//  Write only to the DSP input parameters
				if (address >= DATA_MEMORY_ADDRESS && address < DATA_MEMORY_ADDRESS+DSP_IO_BORDER) {
				
					// put the "bad value" index of the parameter to be read out into APP_HOST_CTL_DATA as a write request
					dwData = 0xFFFFFFFF; 
					dwStatus =  PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL_DATA, WDC_WRITE, &dwData, FALSE);

					// TODO: IODM should not change values in Pixie_Devices, but if it does not here, GetTraces DMA times out
								
					// Put the values to be changed into  DSP_Parameter_Values
					for (k = 0; k < nWords; k++) 
						Pixie_Devices[ModNum].DSP_Parameter_Values[address-DATA_MEMORY_ADDRESS+k] = (U16)buffer[k];
					
					// Fill the DSP parameter block RAM with the updated DSP_Parameter_Values

					for (k = 0; k < DSP_IO_BORDER; k++) 

#ifdef XIA_WINDOZE
					*(U32*)(VAddr[ModNum] + FPGA_PARAM_RAM + 4*k) = (U32)Pixie_Devices[ModNum].DSP_Parameter_Values[k]; 
#endif
#ifdef XIA_LINUX
					// KS DEBUG
					// NOTE: looks like VAddr mapping is not working right under Linux. Using just regular memory I/O
					WDC_WriteAddr32(hDev[ModNum], AD_PCI_BAR0, FPGA_PARAM_RAM + 4*k, Pixie_Devices[ModNum].DSP_Parameter_Values[k]);
#endif

					// Set bit 5 of APP_HOST_CTL to enable Parameter IO 
					dwStatus =  PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &dwData, FALSE);
					dwData=(U32)SetBit(BIT_PARIO, (U16)dwData);		
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &dwData, FALSE);

					// Wait for DSP to move data to DSP memory
					i=0;
					dwData = 0x0;
					do {
						// MUST have a delay, but sleep is too long
						wait_for_a_short_time(60000);   // 60 us -- may go faster than this, but usually does not matter				
						dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS, WDC_READ, &dwData, FALSE);
						i++;
					} while ( i < (timeoutPollDSP) && (TstBit(BIT_PARIO_ACTIVE,(U16)dwData)==1) );// while DSP is copying parameters
//					sprintf(ErrMSG, "*INFO* (Pixie_IODM) DSP parameter WRITE: APP_STATUS=0x%08X, cycle %d", dwData,i);
//					Pixie_Print_MSG(ErrMSG,1);

					// here: timed out or DSP set bit 14 of APP_STATUS				
					if (i >= timeoutPollDSP) {  // if timed out
						sprintf(ErrMSG, "*ERROR* (Pixie_IODM) DSP parameter WRITE timed out, APP_STATUS=0x%08X", dwData);
						Pixie_Print_MSG(ErrMSG,1);
						return(-2);
					}
					
					// APP_HOST_CTL.5 should be cleared automatically, by DSP's ParIO active going low.
					// here we read it (all bits) just in case???
					dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &dwData, FALSE);

				}// if MOD_WRITE to DSP IO address range	
			
				else if (address == IO_BUFFER_ADDRESS) {
					// not implemented yet
					sprintf(ErrMSG, "*ERROR* (Pixie_IODM) IO_BUFFER write not implemented yet.");
					Pixie_Print_MSG(ErrMSG,1);
					return(-1);
				}	// if MOD_WRITE to IO buffer --  not implemented 
				
				else { // wrong address
					sprintf(ErrMSG, "*ERROR* (Pixie_IODM) Write to wrong address %d.", address);
					Pixie_Print_MSG(ErrMSG,1);
					return(-1);
				} // if wrong address
			} //if MOD_WRITE 

			if (direction == MOD_READ ) 
			{
			//sprintf(ErrMSG, "*INFO* (Pixie_IODM) read: address %d, nWords %d",address,nWords);
			//Pixie_Print_MSG(ErrMSG,1);

				// check for valid address range
				if ( (address - DATA_MEMORY_ADDRESS + nWords) > DATA_MEMORY_LENGTH )
				{
					sprintf(ErrMSG, "*ERROR* (Pixie_IODM) DSP parameter READ request to illegal address range (address %d, nWords %d", address, nWords);
					Pixie_Print_MSG(ErrMSG,1);
					return(-2);
				}

				for (k=0; k < nWords; k++ ) {
					// put the index of the parameter to be read into APP_HOST_CTL_DATA
					index = (address - DATA_MEMORY_ADDRESS) + k;


					//if( (index > (DSP_IO_BORDER+64) ) && (index<=N_DSP_PAR)  || (index < DSP_IO_BORDER )  )
					if( (index <  DSP_IO_BORDER ) ||									// all input parameters [< DSP_IO_BORDER],  
						(index > (DSP_IO_BORDER+64) ) && (index < N_DSP_PAR)  ||		// the channel output parameters [DSP_IO_BORDER+64 .. N_DSP_PAR], and
						(index > (DSP_IO_BORDER+2) )  && (index < DSP_IO_BORDER+28)  )	// some module output parameters can be read directly from DSP RAM
					{


#ifdef XIA_WINDOZE
						dwData = *(U32*)(VAddr[ModNum] + FPGA_PARAM_RAM + 4*index);
#endif
#ifdef XIA_LINUX
						// KS DEBUG
						// NOTE: looks like VAddr mapping is not working right under Linux. Using just regular memory I/O
						WDC_ReadAddr32(hDev[ModNum], AD_PCI_BAR0, FPGA_PARAM_RAM + 4 * index, &dwData);
#endif
						buffer[k] = dwData;
					}
					else
					{
						dwData = index; 	
						dwStatus =  PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL_DATA, WDC_WRITE, &dwData, FALSE);
						
						// Set bit 5 of APP_HOST_CTL to enable Parameter IO 
						dwStatus =  PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &dwData, FALSE);
						dwData=(U32)SetBit(BIT_PARIO, (U16)dwData);	
						dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &dwData, FALSE);

						// wait for DSP to write data word into mailbox register
						i=0;
						dwData = 0x0;
						do {
							// NOTE: MUST have a delay, but sleep is too long
							wait_for_a_short_time(60000);   // 60 us -- may go faster than this, but usually does not matter	
							dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS, WDC_READ, &dwData, FALSE);
							i++;
						} while ( i < (timeoutPollDSP) && (TstBit(BIT_PARIO_ACTIVE,(U16)dwData)==1) );// while DSP is copying parameters
						
						// here: timed out or DSP set bit 14 of APP_STATUS
						if (i>= timeoutPollDSP) {
							sprintf(ErrMSG, "*ERROR* (Pixie_IODM) DSP parameter READ timed out, mailbox=0x%08X", dwData);
							Pixie_Print_MSG(ErrMSG,1);
							return(-2);
						}
						//Now the data is at APP_STATUS_DATA
						dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_STATUS_DATA, WDC_READ, &dwData, FALSE);
						buffer[k] = dwData;

						//sprintf(ErrMSG, "*INFO* (Pixie_IODM) READ DSPpar(%d)=0x%08X",index, dwData);
						//Pixie_Print_MSG(ErrMSG,1);
					} //if DSPRAM
				} // for
			} // if MOD_READ

#endif

			break;


		default:
			sprintf(ErrMSG, "*ERROR* (Pixie_IODM) - bus type not defined");
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
			break;
	}
	return(0);
}


/****************************************************************
*	Pixie_IOEM function:
*		Pixie external memory I/O communication (burst Read/Write)
*
*		Return Value:
*			 0 - success or offline analysis
*			-1 - external memory locked by DSP 
*			-2 - unable to open DMA channel
*			-3 - unable to read dummy words
*			-4 - unable to read external memory data
*			-5 - unable to close DMA channel
*			-6 - module address not mapped
*
****************************************************************/

S32 Pixie_IOEM (
				U8  ModNum,			// the Pixie module to communicate to
				U32 address,		// external memory address
				U8  direction,		// either MOD_READ or MOD_WRITE
				U32 nWords,			// the number of 32-bit words to be transferred
				U32 *buffer )		// holds or receives the data
{
	U32            k; 
	U32			   dummy[5]={5*0};
	U32			   count;
	U32            CSR;
	PLX_STATUS     rc = ApiSuccess;
	PLX_DMA_PROP   DmaDesc;
	PLX_DMA_PARAMS DmaData;
	U32 localAddress = PCI_EMDATA;
	S32 retval, error;
	S32 dwStatus;
	U32 u32Data;

	/* Returns immediately for offline analysis */
	if(Offline == 1) return(0);


	switch(PCIBusType) {
		case REGULAR_PCI:

			if (VAddr[ModNum] == 0) {		// rlk
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM) - module address not mapped. Module=%d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-6);
			}


			error = 0;
			/* Set bit 2 of CSR to indicate that PCI wants to access the external memory */
			Pixie_ReadCSR(ModNum, &CSR);
			CSR = SetBit(2, (U16)CSR);
			/* Write back CSR */
			Pixie_WrtCSR(ModNum, (U16)CSR);

			/* Read CSR and check if DSP is still busy with the external memory */
			count = 10000;	/* Check DSPWRTRACE 10000 times */
			do
			{
				Pixie_ReadCSR(ModNum, &CSR);
				retval = CSR & 0x0080;	/* Check bit 7 of CSR (DSPWRTRACE) */
				count --;
			} while ((retval == 0x80) && (count > 0));

			if(count == 0)  /* Timed out */
			{
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM) - External memory locked by DSP in Module %d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}


			if(direction==MOD_WRITE)  /* Write to external memory */
			{
				*(U32*)(VAddr[ModNum] + PCI_EMADDR) = address;  
				for (k=0; k<nWords; k++)
				{
					*(U32*)(VAddr[ModNum] + localAddress) = *buffer++;
				}
			}
			else if(direction==MOD_READ)  /* Read from external memory */
			{
				/* Clear DMA descriptor structure */
				memset( &DmaDesc, 0, sizeof(PLX_DMA_PROP));

				/* Set up DMA configuration structure */
				DmaDesc.ReadyInput 	      = 1;
				DmaDesc.Burst             = 1;
				DmaDesc.BurstInfinite	  = 1;
				DmaDesc.FastTerminateMode = 0;
				DmaDesc.LocalBusWidth     = 2; /* 32 bit bus */

				rc = PlxPci_DmaChannelOpen(&Sys_hDevice[ModNum], 0, &DmaDesc);    
				if (rc != ApiSuccess)
				{
					sprintf(ErrMSG, "*ERROR* (Pixie_IOEM) - Unable to open DMA in Module %d, rc=%d", ModNum, rc);
					Pixie_Print_MSG(ErrMSG,1);
					error = -2;

				}


				if(error>=0)
				{
					memset(&DmaData, 0, sizeof(PLX_DMA_PARAMS));
					DmaData.UserVa	     = (PLX_UINT_PTR)dummy;
					DmaData.LocalAddr    = PCI_EMDATA;
					DmaData.ByteCount	 = 3*4;	/* Read three dummy words; each word is 32-bt (4 bytes) */
					DmaData.Direction    = PLX_DMA_LOC_TO_PCI;

					*(U32*)(VAddr[ModNum] + PCI_EMADDR) = address;  
					rc = PlxPci_DmaTransferUserBuffer(&Sys_hDevice[ModNum], 0, &DmaData, DMATRANSFER_TIMEOUT);
					if (rc != ApiSuccess)
					{	
						sprintf(ErrMSG, "*ERROR* (Pixie_IOEM) - Unable to read dummy words in Module %d, rc=%d", ModNum, rc);
						Pixie_Print_MSG(ErrMSG,1);
						error = -3;
					}
				}

				if(error>=0) {
					memset(&DmaData, 0, sizeof(PLX_DMA_PARAMS));
					DmaData.UserVa			= (PLX_UINT_PTR)buffer;
					DmaData.LocalAddr       = PCI_EMDATA;
					DmaData.ByteCount		= nWords*4;	/* Read external memory data */
					DmaData.Direction       = PLX_DMA_LOC_TO_PCI;

					rc = PlxPci_DmaTransferUserBuffer(&Sys_hDevice[ModNum], 0, &DmaData, DMATRANSFER_TIMEOUT);
					if (rc != ApiSuccess) {
						sprintf(ErrMSG, "*ERROR* (Pixie_IOEM) - Unable to read external memory data in Module %d, rc=%d", ModNum, rc);
						Pixie_Print_MSG(ErrMSG,1);
						error = -4;
					}
				}

				//ALWAYS close when opened successfully
				if(error!=-2)
				{

					rc = PlxPci_DmaChannelClose(&Sys_hDevice[ModNum], 0); 
					if (rc != ApiSuccess)
					{
						error = -5;

						/* But try to reset the device if a DMA is in-progress */
						if (rc == ApiDmaInProgress)
						{
							PlxPci_DeviceReset(&Sys_hDevice[ModNum]);
							/* Attempt to close again */
							rc = PlxPci_DmaChannelClose(&Sys_hDevice[ModNum], 0);
							if (rc == ApiSuccess)
							{
								error=0;
							}
						}

						if(error == -5)
						{
							sprintf(ErrMSG, "ERROR (Pixie_IOEM) - Unable to close the DMA channel in Module %d, rc=%d", ModNum, rc);
							Pixie_Print_MSG(ErrMSG,1);
						}
					}

				}
			}


			/* ALWAYS Clear bit 2 of CSR to release PCI's control of external memory */
			CSR &= 0xFFFB;
			*(U32*)(VAddr[ModNum] + PCI_CSR) = CSR;

			break;

		case (EXPRESS_PCI):

			
#ifdef WINDRIVER_API
			/* Set bit 2 of CSR to indicate that PCI wants to access the external memory */
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-1): read APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}
			CSR=(U32)SetBit(2, (U16)CSR);	
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-2): write  APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);// read back to make sure next I/O does not come too fast
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-3): read APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}

			if(direction==MOD_WRITE) { /* Write to external memory */
				// NB: Fast consecutive writes to memory lead to errors (the new value is not written),
				// it shows with looped WDC_WriteAddr() without readback, or with WDC_WriteAddrBlock();
				// thus using here loop with WDC_WriteAddr() and reading data back (returning -7 if descrepancy is found).
				//error = WDC_WriteAddrBlock32(hDev[ModNum], AD_PCI_BAR2, PCIE_EMDATA+4*address, nWords*sizeof(U32), buffer, WDC_ADDR_RW_DEFAULT);
				for (k=0; k<nWords; k++) {
					error = WDC_WriteAddr32(hDev[ModNum], AD_PCI_BAR2, PCIE_EMDATA+4*(address+k), buffer[k]);
					wait_for_a_short_time(500);
					//WDC_ReadAddr32(hDev[ModNum],AD_PCI_BAR2, PCIE_EMDATA+4*(address+k), &u32Data);
					//WDC_ReadAddr32(hDev[ModNum],AD_PCI_BAR2, PCIE_EMDATA+4*(address+k), &u32Data);
					//if (buffer[k] != u32Data) error = -7;
					// Note: data correct in memory with just one read, but need 2 read back to get correct data to compare
				} // for nWords
			} // if WRITE
			else if(direction==MOD_READ) {  /* Read from external memory */
				// a single read sets address, the following read returns valid data from that address. 
				// NB: instead of looping with WDC_ReadAddr, using WDC_ReadAddrBlock() to read the buffer.
				// But need a dummy read of address 0 (to set address 0),
				// then read addresses 1 through N-1 via ReadAddrBlock() (into buffer[0] through buffer[N-2]),
				// then read address N-1 to read buffer[N-1].

				error = WDC_ReadAddr32(hDev[ModNum],AD_PCI_BAR2, PCIE_EMDATA+4*address, &u32Data);

				error = WDC_ReadAddrBlock32(hDev[ModNum], AD_PCI_BAR2, PCIE_EMDATA+4*(address+1), (nWords-1)*sizeof(U32), buffer, WDC_ADDR_RW_DEFAULT);

				error = WDC_ReadAddr32(hDev[ModNum],AD_PCI_BAR2, PCIE_EMDATA+4*(address+nWords-1), (buffer+nWords-1));


			} // if READ


			// read or write: clear bit 2 to release memory
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-4): read APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}
			CSR=(U32)ClrBit(2, (U16)CSR);	/* Clear bit 2 of APP_HOST_CTL to claim PCI's control of external memory */
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_WRITE, &CSR, FALSE);
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-5): write APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}
			dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], APP_HOST_CTL, WDC_READ, &CSR, FALSE);// read back to make sure next I/O does not come too fast
			if (dwStatus != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Pixie_IOEM IO-6): read APP_HOST_CTL not succesful (%d)", dwStatus);
				Pixie_Print_MSG(ErrMSG,1);
			}

#endif
			break;

		default:
			break;
			
	} // PCI bus type
	return(error);
}


/****************************************************************
*	Find_Xact_Match function:
*		Looks for an exact match between str and a name in the
*		array Names. It uses Names_Array_Len to know how many names
*		there are. Note that all elements in Names have all-uppercase
*		names.
*
*		Return Value:
*			index of str in Names if found
*			Names_Array_Len-1 if not found
*
****************************************************************/

U16 Find_Xact_Match (S8 *str,				// the string to be searched
					 S8 Names[][MAX_PAR_NAME_LENGTH],	// the array which contains the string
					 U16 Names_Array_Len )			// the length of the array
{
	U16	k = 0;

	while (strcmp(str, Names[k]) != 0 && k < Names_Array_Len) { k++; }

	if (k == Names_Array_Len) 
	{
		/* If str is not found in Names */
		sprintf(ErrMSG, "*ERROR* (Find_Xact_Match): %s was not found", str);
		Pixie_Print_MSG(ErrMSG,1);
		return (Names_Array_Len - 1);
	}
	else return (k);

}

/****************************************************************
*	Write_Spectrum_File function:
*		Read histogram data from each Pixie module then save it into a
*		32-bit integer binary file. The data from Module #0 is saved first,
*		then the data from Module #1 is appended to the end of the file,
*		and so on.
*
*		Return Value:
*			 0 - success
*			-1 - failure to open MCA spectrum file
*
****************************************************************/

S32 Write_Spectrum_File (
						 S8 *FileName )		// histogram data file name
{
	U16  i;
	FILE *specFile = NULL;
	S32  retval;
	U32 *specdata=NULL;

	

	specFile = fopen(FileName, "ab"); /* Append to a binary file */
	if(specFile != NULL)
	{
		specdata = malloc(MAX_HISTOGRAM_LENGTH*NUMBER_OF_CHANNELS * sizeof(U32));
		if(!specdata){
			sprintf(ErrMSG, "*ERROR* (Write_Spectrum_File): Memory allocation failure");
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}
		for(i=0; i<Number_Modules; i++)
		{
			retval = Pixie_IOEM((U8)i, HISTOGRAM_MEMORY_ADDRESS, MOD_READ, HISTOGRAM_MEMORY_LENGTH, specdata);
			if(retval < 0)  /* Check if reading is successful */
			{
				sprintf(ErrMSG, "*ERROR* (Write_Spectrum_File): failure to read histogram data from Module %d", i);
				Pixie_Print_MSG(ErrMSG,1);
			}
			else  /* Write to file */
			{
				fwrite(specdata, 4, HISTOGRAM_MEMORY_LENGTH, specFile);
			}
		}
		fclose(specFile);
		free(specdata);
	}
	else
	{
		sprintf(ErrMSG, "*ERROR* (Write_Spectrum_File): can't open MCA spectrum file %s", FileName);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	return(0);
}


/****************************************************************
*	Write_List_Mode_File function:
*		Read list mode data from each Pixie module then append the data to
*		a 16-bit integer binary file. The data from Module #0 is saved first,
*		then the data from Module #1 is appended to the end of the file,
*		and so on.
*		The assumption is that all modules are in the same mode, i.e. 32x buffer, ping
*      pong or single buffer mode, and that they take data synchronously, i.e they
*      have the same number of buffers.
*
*		Return Value:
*			 0 - success
*			-1 - can't open list mode data file
*			-2 - invalid word count
*
****************************************************************/

S32 Write_List_Mode_File (
						  S8  *FileName )		// List mode data file name
{
	U16 i, j, MCSRA, MCSRA_index, EMwords_index;
	U16 EMwords2_index, DblBufCSR_index, DblBufCSR;
	U32 Aoffset[2], WordCountPP[2];
	U32 WordCount, NumWordsToRead, CSR;
	U32 dsp_word[2];
	U32 WCRs[PRESET_MAX_MODULES], CSRs[PRESET_MAX_MODULES];
	FILE *ListModeFile = NULL;
	U32 *buffer=NULL;
	U16 *shortbuffer=NULL;

	//sprintf(ErrMSG, "*INFO* (Write_List_Mode_File): file %s",FileName);
	//Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

	buffer = malloc(LIST_MEMORY_LENGTH * sizeof(U32));
	if(!buffer){
		sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File): Memory allocation failure");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	shortbuffer = malloc(IO_BUFFER_LENGTH * sizeof(U16));
	if(!shortbuffer){
		sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File): Memory allocation failure");
		Pixie_Print_MSG(ErrMSG,1);
		free(buffer);
		return(-1);
	}

	if(!MultiThreadDAQ) {		// in multi thread, use file pointer opened in runstart
		ListModeFile = fopen(FileName, "ab");  // Append to a binary file 
		if(ListModeFile == NULL) {
			sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File): can't open list mode file %s", FileName);
			Pixie_Print_MSG(ErrMSG,1);
			free(buffer);
			free(shortbuffer);
			return(-1);
		}
	}

	// Locate DSP variables MODCSRA and EMWORDS 
	MCSRA_index = Find_Xact_Match("MODCSRA", DSP_Parameter_Names, N_DSP_PAR);
	EMwords_index = Find_Xact_Match("EMWORDS", DSP_Parameter_Names, N_DSP_PAR);
	EMwords2_index = Find_Xact_Match("EMWORDS2", DSP_Parameter_Names, N_DSP_PAR);
	DblBufCSR_index = Find_Xact_Match("DBLBUFCSR", DSP_Parameter_Names, N_DSP_PAR);

	Pixie_IODM(0, (U16)DATA_MEMORY_ADDRESS + MCSRA_index, MOD_READ, 1, dsp_word);
	MCSRA = (U16)dsp_word[0];
	Pixie_IODM(0, (U16)DATA_MEMORY_ADDRESS + DblBufCSR_index, MOD_READ, 1, dsp_word);
	DblBufCSR = (U16)dsp_word[0];

	// ----------- single buffer mode, List mode data in DM ------------------------------

	if( (TstBit(MODCSRA_EMWORDS, MCSRA) == 0) & (TstBit(DBLBUFCSR_ENABLE, DblBufCSR) == 0) )	
	{
		// Read out list mode data module by module 
		for(i=0; i<Number_Modules; i++)
		{

			// Read Pixie's word count register => the number of 16-bit words to read 
			Pixie_RdWrdCnt((U8)i, &WordCount);

			if(WordCount > IO_BUFFER_LENGTH)
			{
				sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File):invalid word count %d", WordCount);
				Pixie_Print_MSG(ErrMSG,1);
				if(!MultiThreadDAQ) fclose(ListModeFile);
				if( MultiThreadDAQ) fclose(listFile[0]);
				free(buffer);
				free(shortbuffer);
				return(-2);
			}

			// Read out the list mode data 
			Pixie_IODM((U8)i, IO_BUFFER_ADDRESS, MOD_READ, (U16)WordCount, buffer);

			for(j=0; j<WordCount; j++)
			{
				shortbuffer[j] = (U16)buffer[j];
			}

			// Append to the file 
			
			if(!MultiThreadDAQ) fwrite(shortbuffer, 2, WordCount, ListModeFile);
			if( MultiThreadDAQ) fwrite(shortbuffer, 2, WordCount, listFile[0]);
		}

		if(!MultiThreadDAQ) fclose(ListModeFile);		// only normal mode closes file every time
		free(buffer);
		free(shortbuffer);
		return(0);

	}

	// ----------- 32x buffer mode, List mode data in EM ------------------------------

	if( (TstBit(MODCSRA_EMWORDS, MCSRA) == 1) & (TstBit(DBLBUFCSR_ENABLE, DblBufCSR) == 0) )	
	{
		// Read out list mode data module by module 
		for(i=0; i<Number_Modules; i++)
		{

			// A dummy read of Pixie's word count register 
			Pixie_RdWrdCnt((U8)i, &WordCount);

			// The number of 16-bit words to read is in EMwords 
			Pixie_IODM((U8)i, (U16)DATA_MEMORY_ADDRESS + EMwords_index, MOD_READ, 2, dsp_word);
			WordCount = dsp_word[0] * 65536 + dsp_word[1];

			// Check if it is an odd or even number 
			if(fmod(WordCount, 2.0) == 0.0) 
			{
				NumWordsToRead = WordCount / 2;
			}
			else
			{
				NumWordsToRead = WordCount / 2 + 1;
			}

			if( (NumWordsToRead > LIST_MEMORY_LENGTH) || (NumWordsToRead ==0) ) {
				sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File):invalid word count %d", NumWordsToRead);
				Pixie_Print_MSG(ErrMSG,1);
				if(!MultiThreadDAQ) fclose(ListModeFile);
				if( MultiThreadDAQ) fclose(listFile[0]);
				free(buffer);
				free(shortbuffer);
				return(-2);
			}

			// Read out the list mode data 
			Pixie_IOEM((U8)i, LIST_MEMORY_ADDRESS, MOD_READ, NumWordsToRead, buffer);

			if(!MultiThreadDAQ) fwrite(buffer, 2, WordCount, ListModeFile);
			if( MultiThreadDAQ) fwrite(buffer, 2, WordCount, listFile[0]);

		}

		if(!MultiThreadDAQ) fclose(ListModeFile);		// only normal mode closes file every time
		free(buffer);
		free(shortbuffer);
		return(0);

	}

	// ----------- double buffer mode, List mode data in EM ------------------------------

	else
	{
	
		Aoffset[0] = 0;
		Aoffset[1] = LM_DBLBUF_BLOCK_LENGTH;
		
		for(i=0; i<Number_Modules; i++)
		{
			// read the CSR
			Pixie_ReadCSR((U8)i, &CSR);
			CSRs[i] = CSR;

			// A read of Pixie's word count register 
			// This also indicates to the DSP that a readout has begun 
			Pixie_RdWrdCnt((U8)i, &WordCount);
			WCRs[i] = WordCount;
		}	// CSR for loop

		// Read out list mode data module by module 
		for(i=0; i<Number_Modules; i++)
		{

			// The number of 16-bit words to read is in EMwords or EMwords2
			Pixie_IODM((U8)i, (U16)DATA_MEMORY_ADDRESS + EMwords_index, MOD_READ, 2, dsp_word);
			WordCountPP[0] = dsp_word[0] * 65536 + dsp_word[1];
			Pixie_IODM((U8)i, (U16)DATA_MEMORY_ADDRESS + EMwords2_index, MOD_READ, 2, dsp_word);
			WordCountPP[1] = dsp_word[0] * 65536 + dsp_word[1];

					
			if(TstBit(CSR_128K_FIRST, (U16)CSRs[i]) == 1) 
			{
				j=0;			
			}
			else		// block at 128K+64K was first
			{
				j=1;
			}
	
			if  (TstBit(CSR_DATAREADY, (U16)CSRs[i]) == 0 )		
			// function called after a readout that cleared WCR => run stopped => read other block
			{
				j=1-j;			
				sprintf(ErrMSG, "*INFO* (Write_List_Mode_File): Module %d: Both memory blocks full (block %d older). Run paused (or finished).",i,1-j);
				Pixie_Print_MSG(ErrMSG,1);
			}

//sprintf(ErrMSG, "*INFO* (Write_List_Mode_File): Module %d: CSR = %x, j=%d",i,CSRs[i],j );
//Pixie_Print_MSG(ErrMSG,1);
			
			if (WordCountPP[j] >0)
			{
				// Check if it is an odd or even number 
				if(fmod(WordCountPP[j], 2.0) == 0.0)
				{
					NumWordsToRead = WordCountPP[j] / 2;
				}
				else
				{
					NumWordsToRead = WordCountPP[j] / 2 + 1;
				}
	
				if(NumWordsToRead > LIST_MEMORY_LENGTH)
				{
					sprintf(ErrMSG, "*ERROR* (Write_List_Mode_File):invalid word count %d", NumWordsToRead);
					Pixie_Print_MSG(ErrMSG,1);
					if(!MultiThreadDAQ) fclose(ListModeFile);
					if( MultiThreadDAQ) fclose(listFile[0]);
					free(buffer);
					free(shortbuffer);
					return(-2);
				}
	
				// Read out the list mode data 
				Pixie_IOEM((U8)i, LIST_MEMORY_ADDRESS+Aoffset[j], MOD_READ, NumWordsToRead, buffer);
	
				// save to file
				if(!MultiThreadDAQ) fwrite(buffer, 2, WordCountPP[j], ListModeFile);
				if( MultiThreadDAQ) fwrite(buffer, 2, WordCountPP[j], listFile[0]);

			}
						
		}	// readout for loop
	
	
		if(!MultiThreadDAQ) fclose(ListModeFile);		// only normal mode closes file every time

		for(i=0; i<Number_Modules; i++)
		{
			// A second read of Pixie's word count register to clear the DBUF_1FULL bit
			// indicating to the DSP that the read is complete
			Pixie_RdWrdCnt((U8)i, &WordCount);
		}	// third for loop

		free(buffer);
		free(shortbuffer);
		return(0);
	}

	
}


/****************************************************************
*	Read_Spectrum_File function:
*		Read MCA spectrum data from a previously-saved binary file.
*
*		Return Value:
*			 0 - success
*			-1 - can't open the MCA spectrum file
*			-2 - the spectrum file doesn't contain data for this module
*
****************************************************************/

S32 Read_Spectrum_File (
						U8  ModNum,			// Pixie module number
						U32 *Data,			// Receives histogram data
						S8  *FileName )		// previously-saved histogram file
{
	U32 offset, TotalBytes;

	FILE *specFile = NULL;

	specFile = fopen(FileName, "rb"); /* Read a binary file */
	if(specFile != NULL)
	{
		/* Get the file length */
		Pixie_fseek(specFile, 0, SEEK_END);
		TotalBytes = Pixie_ftell(specFile) + 1;

		/* Calculate the offset to point the file pointer to the module histogram data */
		offset = ModNum*HISTOGRAM_MEMORY_LENGTH*NUMBER_OF_CHANNELS;

		/* Check offset to make sure histogram data is available for this Pixie module */
		if(offset < (TotalBytes - HISTOGRAM_MEMORY_LENGTH*NUMBER_OF_CHANNELS))
		{
			Pixie_fseek(specFile, offset, SEEK_SET);
			fread(Data, 4, HISTOGRAM_MEMORY_LENGTH, specFile);
			fclose(specFile);
			return(0);
		}
		else
		{
			sprintf(ErrMSG, "*ERROR* (Read_Spectrum_File): the spectrum file %s doesn't contain data for Module %d", FileName, ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);
		}
	}
	else
	{
		sprintf(ErrMSG, "*ERROR* (Read_Spectrum_File): can't open MCA spectrum file %s", FileName);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
}


/****************************************************************
*	Get_Traces function:
*		Acquire ADC traces for one or all channels of a Pixie module.
*			for ocsilloscope, all channels are read out.
*			for tau_finder, individual channels are read out).
*
*		Return Value:
*			 0 - success
*			-1 - failure to start the GET_TRACES run
*			-2 - GET_TRACES run timed out
*			
****************************************************************/


S32 Get_Traces (
				U32 *Trace_Buffer,		// ADC trace data
				U8  ModNum,				// Pixie module number
				U8  ChanNum )			// Pixie channel number
{

	U16 ch, idx, Wcount;
	U32 value;
	S32 retval;
	U32 dwStatus;


	// DMA stuff for Pixie500e
#ifdef WINDRIVER_API
	pDmaTrace = NULL;
#endif

	/* Check whether only only one channel or all channels are requested to acquring traces */
	if(ChanNum == NUMBER_OF_CHANNELS)
	{
		
		// Pixie-4e, Pixie-500e return 4 channel's ADC traces via the DMA buffer
		if(PCIBusType==EXPRESS_PCI) {
#ifdef WINDRIVER_API
			// Prepare for DMA even before the data starts showing up in SDRAM
			// program sequencer and lock SG buffer
			retval = PIXIE500E_DMA_Trace_Setup(hDev[ModNum],  IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32), Trace_Buffer, &pDmaTrace);
			if(retval != WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*ERROR* (Get_Traces): failure to set up ADC trace DMA for module %d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-5);
			}
			dwStatus = PIXIE500E_DMA_Init(hDev[ModNum]);
			if (dwStatus!=0) {
				sprintf(ErrMSG, "*ERROR* (Get_Traces): Failure to init DMA engine %d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-5);
			}
			
			retval = WDC_DMASyncCpu(pDmaTrace);	
#endif
			//	sprintf(ErrMSG, "*DEBUG* (Get_Traces): start run", ModNum);
			//	Pixie_Print_MSG(ErrMSG,1);
	
			/* Start GET_TRACES run to get ADC traces */
			retval = Start_Run(ModNum, NEW_RUN, 0, GET_TRACES);
			if(retval < 0) {
				sprintf(ErrMSG, "*ERROR* (Get_Traces): failure to start GET_TRACES run in Module %d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}

			/* Check Run Status */
			Wcount=0;
			do{
				Pixie_Sleep(10);
				if(Check_Run_Status(ModNum)==0)
				{
				//	sprintf(ErrMSG, "*INFO* (Get_Traces): Check_Run_Status=0: Module %d", ModNum);
				//	Pixie_Print_MSG(ErrMSG,1);
					break;
				}

				Wcount++;
			} while(Wcount<100); /* The maximum allowed waiting time is 1 s */

			if(Wcount>=100)	{
				sprintf(ErrMSG, "*ERROR* (Get_Traces): Acquiring ADC traces in Module %d timed out", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-2); /* Time Out */
			}
			// N.B. At this point, for Pixie500e, the data is in SDRAM or waiting to be written into it 

#ifdef WINDRIVER_API
				//sprintf(ErrMSG, "*DEBUG* (Get_Traces): starting DMA readout");
				//Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);	
				VDMADriver_Go(hDev[ModNum]);
		
				dwStatus = PIXIE500E_DMA_WaitForCompletion(hDev[ModNum], TRUE);
				if (dwStatus == 0) {
					//	sprintf(ErrMSG, "*INFO* (Pixie_IODM) IO_BUFFER read: DMA transfer completed successfully.");
					//	Pixie_Print_MSG(ErrMSG,1);
					//	return(dwStatus);
				} else {
						sprintf(ErrMSG, "*ERROR* (Get_Traces) IO_BUFFER read: DMA transfer timed out.");
						Pixie_Print_MSG(ErrMSG,1);
						return(-2);
				}

				// sprintf(ErrMSG, "*DEBUG* (Get_Traces): finishing up DMA");
				// Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);	
				retval = WDC_DMASyncIo(pDmaTrace);
				retval = WDC_DMABufUnlock(pDmaTrace);
#endif
		} // endif PCI bus type == PCIe

		// Pixie-4 returns a single channel's ADC trace via Memory I/O, loop over all channels
		if(PCIBusType==REGULAR_PCI) {
			for (ch=0; ch<NUMBER_OF_CHANNELS; ch++ )
			{
				/* Set DSP parameter CHANNUM (channel number) */
				idx = Find_Xact_Match("CHANNUM", DSP_Parameter_Names, N_DSP_PAR);
				Pixie_Devices[ModNum].DSP_Parameter_Values[idx] = ch;
				/* Download to the data memory */
				value = (U32)Pixie_Devices[ModNum].DSP_Parameter_Values[idx] ;
				Pixie_IODM(ModNum, (DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

				/* Start GET_TRACES run to get ADC traces */
				retval = Start_Run(ModNum, NEW_RUN, 0, GET_TRACES);
				if(retval < 0) {
					sprintf(ErrMSG, "*ERROR* (Get_Traces): failure to start GET_TRACES run in Module %d, Channel %d", ModNum, ch);
					Pixie_Print_MSG(ErrMSG,1);
					return(-1);
				}

				/* Check Run Status */
				Wcount=0;
				do{
					Pixie_Sleep(1);
					if(Check_Run_Status(ModNum)==0) {
						break;
					}

					Wcount++;
				} while(Wcount<10000); /* The maximum allowed waiting time is 10 s */

				if(Wcount>=10000) 	{
					sprintf(ErrMSG, "*ERROR* (Get_Traces): Acquiring ADC traces in Module %d Channel %d timed out", ModNum, ch);
					Pixie_Print_MSG(ErrMSG,1);
					return(-2); /* Time Out */
				}

				/* Read out the I/O buffer */
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS, MOD_READ, IO_BUFFER_LENGTH, Trace_Buffer);

				/* Update pointer *Trace_Buffer */
				Trace_Buffer += IO_BUFFER_LENGTH;
			}

		}


	}
	else if(ChanNum < NUMBER_OF_CHANNELS) 	{
		/* Set DSP parameter CHANNUM (channel number) */
		idx = Find_Xact_Match("CHANNUM", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[ModNum].DSP_Parameter_Values[idx] = ChanNum;
		/* Download to the data memory */
		value = (U32)Pixie_Devices[ModNum].DSP_Parameter_Values[idx] ;
		Pixie_IODM(ModNum, (DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

		/* Start GET_TRACES run to get ADC traces */
		switch(PCIBusType) {
			case (REGULAR_PCI):
				retval = Start_Run(ModNum, NEW_RUN, 0, GET_TRACES);
  				break;
		
			case (EXPRESS_PCI):
				retval = Start_Run(ModNum, NEW_RUN, 0, 39);		// controltask 39: traditional single channel to I/O buffer
  				break;

			default:
				break;
			
		} // PCI bus type

		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Get_Traces): failure to start GET_TRACES run in Module %d Channel %d", ModNum, ChanNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}

		/* Check Run Status */
		Wcount=0;
		do
		{
			Pixie_Sleep(10);
			if(Check_Run_Status(ModNum)==0)
				break;

			Wcount++;
		} while(Wcount<100); /* The maximum allowed waiting time is 2 s */

		if(Wcount>=100)
		{
			sprintf(ErrMSG, "*ERROR* (Get_Traces): Acquiring ADC traces in Module %d Channel %d timed out", ModNum, ChanNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-2); /* Time Out */
		}
		/* Read out the I/O buffer */
		switch(PCIBusType) {
			case (REGULAR_PCI):
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS, MOD_READ, IO_BUFFER_LENGTH, Trace_Buffer);
  				break;
		
			case (EXPRESS_PCI):
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS_P500E+DATA_MEMORY_ADDRESS , MOD_READ, IO_BUFFER_LENGTH, Trace_Buffer);
  				break;

			default:
				break;
			
		} // PCI bus type
	}	// N channels
	return(0);
}



/****************************************************************
*	Adjust_Offsets function:
*      if called with a module number < current number of modules:
*			Adjust the DC-offset of all channels of a Pixie module.
*		if called with current number of modules:
*			Adjust the DC-offset of all channels of all Pixie modules.
*
*		Return Value:
*			 0 - success
*			-1 - failure to start RampTrackDAC run
*			-2 - RampTrackDAC run timed out
*			-3 - failure to set DACs
*			-4 - failure to program Fippi
*
****************************************************************/

S32 Adjust_Offsets (
					U8 ModNum )		// module number
{

	U32    Wcount, j, CurrentModNum, Twait, MNstart, MNend;
	S32    retval;
	U32    sTDACwave[IO_BUFFER_LENGTH], IOBuffer[DSP_IO_BORDER];
	double TDACwave[IO_BUFFER_LENGTH], DACcenter, DACfifty;
	double a, b, abdiff, abmid, coeff[2], low, high, ADCtarget,baselinepercent;
	U16    adcMax, TDACwaveLen, TDACStepSize, TRACKDAC, idx, ChanCSRA;
	S8     str[256];

	/* Initialize several constants */
	TDACwaveLen = 2048;	/* 8192 words shared by 4 channels */
	TDACStepSize = 32;	/* 32 Ramp TrackDAC steps; each step covers 2048 TDAC values */

	adcMax = 4096;	/* P4 Fippi still reports 12-bit ADC numbers */
	if(PCIBusType==EXPRESS_PCI) {
		adcMax = 16384;	/* P500e Fippi reports 14-bit ADC numbers. WH_TODO: check for P4e, though normally use 0x85 instead */
	}

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


		// start ramping
		for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
		{
			/* Start to ramp TRACK DACs */
			retval = Start_Run((U8)CurrentModNum, NEW_RUN, 0, RAMP_TRACKDAC);
			if(retval < 0)
			{
				sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): failure to start RampTrackDAC run in Module %d", CurrentModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1);
			}
		}

		// wait and poll
		Twait=0;
		for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
		{
			/* Check run status */
			Wcount=0;
			do
			{
				Pixie_Sleep(1);
				if(Check_Run_Status((U8)CurrentModNum)==0)		
				{
					break;
				}

				Wcount++;
			}while(Wcount<10000); /* The maximum allowed waiting time is 10 s */

			if(Wcount>=10000)
			{
				sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): RampTrackDACs timed out in Module %d", CurrentModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-2); /* Time Out */
			}
		}

		//readout and calculate offsets
		for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
		{
			/* Read out the I/O buffer */
			Pixie_IODM ((U8)CurrentModNum, IO_BUFFER_ADDRESS, MOD_READ, IO_BUFFER_LENGTH, sTDACwave);

			/* cast the tdac data into doubles */
			for (j = 0; j < IO_BUFFER_LENGTH; j++) 
			{
				TDACwave[j] = (double)sTDACwave[j];
			}

			/*	clean up the array by removing entries that are out of bounds, or on a constant level */
			for (j = (IO_BUFFER_LENGTH - 1); j > 0; j --)
			{
				if ((TDACwave[j] > (adcMax-1)) || (TDACwave[j] == TDACwave[j-1]))
				{
					TDACwave[j] = 0;
				}
			}

			/* take care of the 0th element last, always lose it */
			TDACwave[0] = 0.0;

			/*	Another pass through the array, removing any element that is surrounded by ZEROs */
			for(j = 1; j < (IO_BUFFER_LENGTH - 1); j ++)
			{
				if(TDACwave[j] != 0)  /* remove out-of-range points and failed measurements */
				{
					if ((TDACwave[j - 1] == 0) && (TDACwave[j + 1] == 0))
					{
						TDACwave[j] = 0;
					}
				}
			}

			for( j = 0; j < NUMBER_OF_CHANNELS; j ++ )
			{
				/* Get DSP parameter CHANCSRA */
				sprintf(str,"CHANCSRA%d",j);
				idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
				ChanCSRA=Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];
				if( TstBit(2, ChanCSRA) )  /* Check if this is a good channel */
				{
					/* Perform a linear fit to these data */
					low = j*TDACwaveLen;
					coeff[0] = low;
					high = low+TDACwaveLen-1;
					coeff[1] = high;
					retval = Linear_Fit(TDACwave, coeff);
					if ( retval == 0 ) 	{
						a = -coeff[0] / coeff[1];
						a = MIN(MAX(low, a), high);
						b = (adcMax - coeff[0]) / coeff[1];
						b = MIN(MAX(low, b), high);
						abdiff = (double) (fabs(b - a) / 2.);
						abmid = (b + a) / 2.;
						a = (double) (ceil(abmid - (0.25 * abdiff)));
						b = (double) (floor(abmid + (0.25 * abdiff)));
						coeff[0] = a;
						coeff[1] = b;

						retval = Linear_Fit(TDACwave, coeff);
						if ( retval == 0 )
						{
							a = -coeff[0] / coeff[1];
							a = MIN(MAX(low, a), high);
							b = (adcMax - coeff[0]) / coeff[1];
							b = MIN(MAX(low, b), high);
							abdiff = (double) (fabs(b - a) / 2.);
							abmid = (b + a) / 2.;
							a = (double) (ceil(abmid - (0.9 * abdiff)));
							b = (double) (floor(abmid + (0.9 * abdiff)));
							coeff[0] = a;
							coeff[1] = b;

							retval = Linear_Fit(TDACwave, coeff);
							if( retval == 0 ) 	{
								DACfifty = (adcMax/2 - coeff[0]) / coeff[1];
								DACcenter = (DACfifty - low) * TDACStepSize;

								/* Find baseline percent and calculate ADCtarget */
								sprintf(str,"BASELINEPERCENT%d",j);
								idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
								baselinepercent=(double)Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];
								ADCtarget=adcMax*baselinepercent/100.0;

								TRACKDAC = (U16)RoundOff(DACcenter+TDACStepSize*(ADCtarget-adcMax/2)/(coeff[1]));

								/* Update DSP parameter TRACKDAC */
								sprintf(str,"TRACKDAC%d", j);
								idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
								Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx]=TRACKDAC;
							}
							else {
								sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): linear fit error in Module %d Channel %d", CurrentModNum, j);
								Pixie_Print_MSG(ErrMSG,1);
							}
						}
						else {
							sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): linear fit error in Module %d Channel %d", CurrentModNum, j);
							Pixie_Print_MSG(ErrMSG,1);
						}
					}
					else {
						sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): linear fit error in Module %d Channel %d", CurrentModNum, j);
						Pixie_Print_MSG(ErrMSG,1);
					}
				}
			}
		}

		for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
		{
			/* Download to the data memory */
			for(j=0; j<DSP_IO_BORDER; j++)
			{
				IOBuffer[j] = (U32)Pixie_Devices[CurrentModNum].DSP_Parameter_Values[j];
			}
			Pixie_IODM((U8)CurrentModNum, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, IOBuffer);

			/* Set DACs */
			retval = Control_Task_Run((U8)CurrentModNum, SET_DACS, 10000);
			if(retval < 0) 	{
				sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): failure to set DACs in Module %d", CurrentModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-3);
			}

			/* Program signal processing FPGAs */
			retval = Control_Task_Run((U8)CurrentModNum, PROGRAM_FIPPI, 1000);
			if(retval < 0) 	{
				sprintf(ErrMSG, "*ERROR* (Adjust_Offsets): failure to program Fippi in Module %d", CurrentModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-4);
			}
		}

	return(0);

}



/****************************************************************
*	Adjust_Offsets_DSP function:
*      if called with a module number < current number of modules:
*			Adjust the DC-offset of all channels of a Pixie module.
*		if called with current number of modules:
*			Adjust the DC-offset of all channels of all Pixie modules.
*
*		Return Value:
*			 0 - success
*			-1 - failure to start DSP controltask run
*			-4 - failure to program Fippi
*
****************************************************************/

S32 Adjust_Offsets_DSP (
						U8 ModNum )		// module number
{
	U32 CurrentModNum, MNstart, MNend;
	U32 retval, value, ChanNum;
	U16 TrackDAC, idx_HOD, idx_TDAC;
	S8  str[256];

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

	idx_HOD=Find_Xact_Match("HOSTODATA", DSP_Parameter_Names, N_DSP_PAR);

	for (CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
	{
		// Find offset in DSP, set DACs to target value, update TRACKDAC values in DSP memory
		retval = Control_Task_Run((U8)CurrentModNum, FIND_OFFSET, 1000);
		if(retval < 0) {
			sprintf(ErrMSG, "*ERROR* (Adjust_Offsets_DSP): failure to start ADJUST_OFFSETS run in Module %d", CurrentModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}

		// FPGA RAM not updated by DSP. Starting another run sets ParIOActive (for Controltask, Resume, etc) so DSP
		// would read FPGA RAM again and overwrite TRACKDAC values. So now read from DSP (output block!)
		// and apply back to the FPGA RAM. Can not on GUI to read from DSP to update display variables because
		// GUI reads from FPGA
		for( ChanNum = 0; ChanNum < NUMBER_OF_CHANNELS; ChanNum ++ )
		{
			retval = Pixie_IODM(CurrentModNum, DATA_MEMORY_ADDRESS+idx_HOD+ChanNum, MOD_READ, 1, &value );
			TrackDAC = (U16)value;
			//sprintf(ErrMSG, "*INFO* (Adjust_Offsets_DSP): TrackDAC from HostOData: %d.", TrackDAC);
			//Pixie_Print_MSG(ErrMSG,1);
			if (retval != 0) {
				sprintf(ErrMSG, "*ERROR* (Adjust_Offsets_DSP): Cannot get TrackDAC cut for module %d.", CurrentModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-2);
			}

			/* Set DSP parameter TrackDAC */
			sprintf(str,"TRACKDAC%d",ChanNum);
			idx_TDAC=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
			Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx_TDAC]=TrackDAC;
			/* Download TrackDAC to the DSP data memory */
			value = (U32)TrackDAC;
			Pixie_IODM(CurrentModNum, (U16)(DATA_MEMORY_ADDRESS+idx_TDAC), MOD_WRITE, 1, &value);
		}

		// No ProgramFippi required here, DACs are not part of it

	} // modules
	return(0);
}

/****************************************************************
*	Linear_Fit function:
*		This routine performs a linear fit to positive definite
*		data, any negative or zero point will be ignored!
*
*		Return Value:
*			 0 - fit ok
*			-1 - no fit
*
****************************************************************/

S32 Linear_Fit (
				double *data,			// source data for linear fit
				double coeff[2] )		// coefficients
{
	U32 i;
	U32 ndata  = 0;
	double sxx = 0;
	double sx  = 0;
	double sy  = 0;
	double syx = 0;

	for(i = (U32)coeff[0]; i < (U32)coeff[1]; i++)
	{
		if(data[i] <= 0)
		{
			continue;
		}

		sx  += i;
		sxx += i*i;
		sy  += data[i];
		syx += data[i]*i;
		ndata++;
	}

	if(ndata > 1)
	{
		coeff[1] = (syx - ((sx * sy) / ((double) ndata))) / (sxx - ((sx * sx) / ((double) ndata)));
		coeff[0] = (sy - (coeff[1] * sx)) / ((double) ndata);
		return(0); /* fit ok */
	}
	else
	{
		return(-1); /* no fit */
	}
}


/****************************************************************
*	Pixie_Sleep function:
*		This routine serves as a wrapper for a system dependent
*              sleep-like function
*
*		Return Value:
*			 0 - good sleep 
*		      
*
****************************************************************/

S32 Pixie_Sleep (
				 double ms )			// Time in milliseconds
{

#ifdef XIA_WINDOZE
	Sleep((U32)ms);
#endif
#ifdef XIA_LINUX
	usleep((unsigned long)(ms * 1000));
#endif

	return(0);
}

/****************************************************************
*	Pixie_fseek function:
*		This routine serves as a wrapper for a system dependent
*              fseek function, with Large-File Support.
*
*		Return Value:
*			 0 - success 
*		      
*
****************************************************************/

S32 Pixie_fseek (
				 FILE *stream,			// Pointer to FILE structure
				 S64 offset,             // Number of bytes from origin
				 S32 origin)				// Initial position
{
	S32 retval;

#ifdef XIA_WINDOZE
#ifdef WI64
	retval = _fseeki64(stream, offset, origin);
#else
	retval = fseek(stream, offset, origin);
#endif
#elif XIA_LINUX
#ifdef x86_64
	retval = fseeko(stream, offset, origin);
#else
	retval = fseek(stream, offset, origin);
#endif
#endif

	return(retval);
}

/****************************************************************
*	Pixie_ftell function:
*		This routine serves as a wrapper for a system dependent
*              ftell function, with Large-File Support.
*
*		Return Value:
*			 Current position of a file pointer 
*		      
*
****************************************************************/

S64 Pixie_ftell (
				 FILE *stream)			// Pointer to FILE structure

{
	S64 retval;
#ifdef XIA_WINDOZE
#ifdef WI64
	retval = _ftelli64(stream);
#else
	retval = ftell(stream);
#endif
#elif XIA_LINUX
#ifdef x86_64
	retval = ftello(stream);
#else
	retval = ftell(stream);
#endif
#endif

	return(retval);
}

/****************************************************************
*	Get_Slow_Traces function:
*		Acquire slow ADC traces for one channel of a Pixie module.
*
*		Return Value:
*			 0 - success
*			-1 - failure to start the  run
*			-2 - run timed out
*			-3 - no file found
*			
****************************************************************/

S32 Get_Slow_Traces (
					 U32 *UserData,			// input data
					 U8  ModNum,				// Pixie module number
					 S8 *FileName )			// data file name
{



	U16 Wcount, shortbuffer[4096];
	U32 buffer[4096], k,j, CSR, WCR;
	S32 retval;
	FILE *slowFile = NULL;

	sprintf(ErrMSG, "*MESSAGE* (Get_Slow_Traces): starting, please wait");
	Pixie_Print_MSG(ErrMSG,1);

	slowFile = fopen(FileName, "ab");  /* Append to a binary file */
	if(slowFile != NULL)
	{

		/* Start GET_SLOW_TRACES run to get ADC traces */
		retval = Start_Run(ModNum, NEW_RUN, 0, 24);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Get_Slow_Traces): failure to start run");
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}

		for(k=0;k<UserData[0];k+=1)
		{
			/* Check Run Status */
			Wcount=0;
			do
			{
				//Pixie_Sleep(1); don't sleep, do dummy reads to keep up stealing cycles from DSP
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS-4096, MOD_READ, 1024, buffer);

				//poll
				Pixie_ReadCSR(ModNum, &CSR);
				if ((CSR & 0x4000)>0)		// poll LAM bit
				{
					break;
				}

				Wcount++;
			} while(Wcount<10000); /* The maximum allowed waiting time is 10 s */

			if(Wcount>=10000)
			{
				sprintf(ErrMSG, "*ERROR* (Get_Slow_Traces): Acquiring ADC traces timed out");
				Pixie_Print_MSG(ErrMSG,1);
				return(-2); /* Time Out */
			}

			Pixie_RdWrdCnt(ModNum, &WCR);	// WCR is 0 or 1, indicating DM block


			if(WCR==0)
			{
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS, MOD_READ, 4096, buffer);
			}
			else
			{
				Pixie_IODM (ModNum, IO_BUFFER_ADDRESS+4096, MOD_READ, 4096, buffer);
			}

			for(j=0; j<4096; j++)
			{
				shortbuffer[j] = (U16)buffer[j];
			}

			/* Append to a file */
			fwrite(shortbuffer, 2, 4096, slowFile);

		}

		retval=End_Run(ModNum);  /* Stop the run */
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (Get_Slow_Traces): failure to end the run, retval=%d", retval);
			Pixie_Print_MSG(ErrMSG,1);
			Pixie_Sleep(5000);
			return(-4);
		}
	}
	else
	{
		sprintf(ErrMSG, "*ERROR* (Get_Slow_Traces): Could not open output file");
		Pixie_Print_MSG(ErrMSG,1);
		return(-3); /* No file  */
	}

	fclose(slowFile);

	sprintf(ErrMSG, "*MESSAGE* (Get_Slow_Traces): done taking traces");
	Pixie_Print_MSG(ErrMSG,1);

	return(0);
}


/****************************************************************
*	Tau_Finder function:
*		Find the exponential decay constant of the detector/preamplifier
*		signal connected to one channel of a Pixie module.
*			
*		Tau is both an input and output parameter: it is used as the
*		initial guess of Tau, then used for returning the new Tau value.
*
*		Return Value:
*			 0 - success
*			-1 - failure to acquire ADC traces
*
****************************************************************/

S32 Tau_Finder (
				U8 ModNum,			// Pixie module number
				U8 ChanNum,			// Pixie channel number
				double *Tau )		// Tau value
{

	U32 Trace[IO_BUFFER_LENGTH];
	U16 idx, FL, FG, Xwait, TFcount; /* fast filter times are set here */
	U16 ndat, k, kmin, kmax, n, tcount, MaxTimeIndex;
	U16 Trig[IO_BUFFER_LENGTH];
	S8  str[256];
	double localAmplitude, s1, s0; /* used to determine which tau fit was best */
	double dt; /* dt is the time between Trace samples */
	double threshold, t0, t1, TriggerLevelShift, avg, MaxTimeDiff, fitted_tau;
	double FF[IO_BUFFER_LENGTH], FF2[IO_BUFFER_LENGTH], TimeStamp[IO_BUFFER_LENGTH/4];
	double input_tau;
	S32 retval;

	U16 SYSTEM_CLOCK_MHZ = 75;
	U16 FILTER_CLOCK_MHZ = 75;
	U16	ADC_CLOCK_MHZ = 75;
	U16	DSP_CLOCK_MHZ = 75;
	U16 CTscale = 16;
	U16 BoardVersion;

	// Define clock constants according to BoardRevision 
	Pixie_Define_Clocks (ModNum,ChanNum,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale, &DSP_CLOCK_MHZ );
	BoardVersion = (U16)Pixie_Devices[ModNum].Module_Parameter_Values[BoardVersion_Index];


	/* Save input Tau value */
	input_tau=*Tau;

	/* Generate random indices */
	RandomSwap();

	/* Get DSP parameters FL, FG and XWAIT */
	sprintf(str,"FASTLENGTH%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	FL=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	sprintf(str,"FASTGAP%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	FG=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	sprintf(str,"XWAIT%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	Xwait=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	if( ((BoardVersion & 0x0F00) == MODULETYPE_P500e ) ||   ((BoardVersion & 0x0F00) == MODULETYPE_P4e )    )  // any Pixie-500e or Pixie-4e
		dt = (double)Xwait*5 / (double)DSP_CLOCK_MHZ*1.0e-6;
	else
		dt=(double)Xwait/(double)DSP_CLOCK_MHZ*1.0e-6;

			ndat=IO_BUFFER_LENGTH;
			localAmplitude=0;

			TFcount=0;  /* Initialize TFcount */
			do
			{
				/* get ADC-trace */
				retval = Get_Traces(Trace, ModNum, ChanNum);
				if(retval < 0)
				{
					sprintf(ErrMSG, "*ERROR* (Tau_Finder): failure to get ADC traces in Module %d Channel %d", ModNum, ChanNum);
					Pixie_Print_MSG(ErrMSG,1);
					return(-1);
				}
				/* Find threshold */
				threshold=Thresh_Finder(Trace, Tau, FF, FF2, FL, FG, ModNum, ChanNum);

				kmin=2*FL+FG;

				for(k=0;k<kmin;k+=1)
				{
					Trig[k]= 0;
				}

				/* Find average FF shift */
				avg=0.0;
				n=0;
				for(k=kmin;k<(ndat-1);k+=1)
				{
					if((FF[k+1]-FF[k])<threshold)
					{
						avg+=FF[k];
						n+=1;
					}
				}

				avg/=n;
				for(k=kmin;k<(ndat-1);k+=1)
				{
					FF[k]-=avg;
				}

				for(k=kmin;k<(ndat-1);k+=1)  /* look for rising edges */
				{
					Trig[k]= (FF[k]>threshold)?1:0;
				}

				tcount=0;
				for(k=kmin;k<(ndat-1);k+=1)  /* record trigger times */
				{
					if((Trig[k+1]-Trig[k])==1)
					{
						TimeStamp[tcount++]=k+2;  /* there are tcount triggers */
					}
				}
				//sprintf(ErrMSG, "*INFO* (Tau_Finder): found  %d triggers", tcount);
				//Pixie_Print_MSG(ErrMSG,1);

				if(tcount>2)
				{
					TriggerLevelShift=0.0;
					for(n=0; n<(tcount-1); n+=1)
					{
						avg=0.0;
						kmin=(U16)(TimeStamp[n]+2*FL+FG);
						kmax=(U16)(TimeStamp[n+1]-1);
						if((kmax-kmin)>0)
						{
							for(k=kmin;k<kmax;k+=1)
							{
								avg+=FF2[k];
							}
						}
						TriggerLevelShift+=avg/(kmax-kmin);
					}
					TriggerLevelShift/=tcount;
				}

				switch(tcount)
				{
				case 0:
					/* Increment TFcount */
					TFcount ++;
					continue;
				case 1:
					t0=TimeStamp[0]+2*FL+FG;
					t1=ndat-2;
					break;
				default:
					MaxTimeDiff=0.0;
					for(k=0;k<(tcount-1);k+=1)
					{
						if((TimeStamp[k+1]-TimeStamp[k])>MaxTimeDiff)
						{
							MaxTimeDiff=TimeStamp[k+1]-TimeStamp[k];
							MaxTimeIndex=k;
						}
					}

					if((ndat-TimeStamp[tcount-1])<MaxTimeDiff)
					{
						t0=TimeStamp[MaxTimeIndex]+2*FL+FG;
						t1=TimeStamp[MaxTimeIndex+1]-1;
					}
					else
					{
						t0=TimeStamp[tcount-1]+2*FL+FG;
						t1=ndat-2;
					}

					break;
				}

				//sprintf(ErrMSG, "*INFO* (Tau_Finder): boundaries at %d  and %d ", t0,t1);
				//Pixie_Print_MSG(ErrMSG,1);
				if(((t1-t0)*dt)<3*(*Tau))
				{
					/* Increament TFcount */
					TFcount ++;
					continue;
				}

				t1=MIN(t1,(t0+RoundOff(6*(*Tau)/dt+4)));

				s0=0;	s1=0;
				kmin=(U16)t0-(2*FL+FG)-FL-1;
				for(k=0;k<FL;k++)
				{
					s0+=Trace[kmin+k];
					s1+=Trace[(U16)(t0+k)];
				}
				if((s1-s0)/FL > localAmplitude)
				{
					fitted_tau=Tau_Fit(Trace, (U16)t0, (U16)t1, dt);
					if(fitted_tau > 0)	/* Check if returned Tau value is valid */
					{
						*Tau=fitted_tau;
					}

					localAmplitude=(s1-s0)/FL;
				}

				/* Increament TFcount */
				TFcount ++;

			} while((*Tau == input_tau) && (TFcount < 10)); /* Try 10 times at most to get a valid Tau value */

	return(0);

}


/****************************************************************
*	Tau_Fit function:
*		Exponential fit of the ADC trace.
*
*		Return Value:
*			Tau value if successful
*			-1 - Geometric search did not find an enclosing interval
*			-2 - Binary search could not find small enough interval
*
****************************************************************/

double Tau_Fit (
				U32 *Trace,		// ADC trace data
				U32 kmin,		// lower end of fitting range
				U32 kmax,		// uuper end of fitting range
				double dt )		// sampling interval of ADC trace data
{
	double mutop,mubot,valtop,valbot,eps,dmu,mumid,valmid;
	U32 count;

	eps=1e-3;
	mutop=10e6; /* begin the search at tau=100ns (=1/10e6) */
	valtop=Phi_Value(Trace,exp(-mutop*dt),kmin,kmax);
	mubot=mutop;
	count=0;
	do  /* geometric progression search */
	{
		mubot=mubot/2.0;
		valbot=Phi_Value(Trace,exp(-mubot*dt),kmin,kmax);
		count+=1;
		if(count>20)
		{
			sprintf(ErrMSG, "*ERROR* (Tau_Fit): geometric search did not find an enclosing interval");
			Pixie_Print_MSG(ErrMSG,1);
			return(-1);
		}	/* Geometric search did not find an enclosing interval */ 
	} while(valbot>0);	/* tau exceeded 100ms */

	mutop=mubot*2.0;
	valtop=Phi_Value(Trace,exp(-mutop*dt),kmin,kmax);
	count=0;
	do  /* binary search */
	{
		mumid=(mutop+mubot)/2.0;
		valmid=Phi_Value(Trace,exp(-mumid*dt),kmin,kmax);
		if(valmid>0)
		{
			mutop=mumid;
		}
		else
		{
			mubot=mumid;
		}

		dmu=mutop-mubot;
		count+=1;
		if(count>20)
		{
			sprintf(ErrMSG, "*ERROR* (Tau_Fit): Binary search could not find small enough interval");
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);  /* Binary search could not find small enough interval */
		}

	} while(fabs(dmu/mubot) > eps);

	return(1/mutop);  /* success */
}


/****************************************************************
*	Phi_Value function:
*		geometric progression search.
*
*		Return Value:
*			search result
*
****************************************************************/

double Phi_Value (
				  U32 *ydat,		// source data for search
				  double qq,		// search parameter
				  U32 kmin,		// search lower limit
				  U32 kmax )		// search upper limit
{
	S32 ndat;
	double s0,s1,s2,qp;
	double A,B,Fk,F2k,Dk,Ek,val;
	U32 k;

	ndat=kmax-kmin+1;
	s0=0; s1=0; s2=0;
	qp=1;

	for(k=kmin;k<=kmax;k+=1)
	{
		s0+=ydat[k];
		s1+=qp*ydat[k];
		s2+=qp*ydat[k]*(k-kmin)/qq;
		qp*=qq;
	}

	Fk=(1-pow(qq,ndat))/(1-qq);
	F2k=(1-pow(qq,(2*ndat)))/(1-qq*qq);
	Dk=-(ndat-1)*pow(qq,(2*ndat-1))/(1-qq*qq)+qq*(1-pow(qq,(2*ndat-2)))/pow((1-qq*qq),2);
	Ek=-(ndat-1)*pow(qq,(ndat-1))/(1-qq)+(1-pow(qq,(ndat-1)))/pow((1-qq),2);
	A=(ndat*s1-Fk*s0)/(ndat*F2k-Fk*Fk) ;
	B=(s0-A*Fk)/ndat;

	val=s2-A*Dk-B*Ek;

	return(val);

} 

/****************************************************************
*	Thresh_Finder function:
*		Threshold finder used for Tau Finder function.
*
*		Return Value:
*			Threshold
*
****************************************************************/

double Thresh_Finder (
					  U32 *Trace,		// ADC trace data
					  double *Tau,		// Tau value
					  double *FF,		// return values for fast filter
					  double *FF2,		// return values for fast filter
					  U32 FL,			// fast length
					  U32 FG,			// fast gap
					  U8  ModNum,		// Pixie module number
					  U8  ChanNum )		// Pixie channel number
{

	U32 ndat,kmin,k,idx,ndev,n,m;
	double Xwait,dt,xx,c0,sum0,sum1,deviation,threshold;
	S8  str[256];

	U16 SYSTEM_CLOCK_MHZ = 75;
	U16 FILTER_CLOCK_MHZ = 75;
	U16	ADC_CLOCK_MHZ = 75;
	U16	DSP_CLOCK_MHZ = 75;
	U16 CTscale = 16;

	// Define clock constants according to BoardRevision 
	Pixie_Define_Clocks (ModNum,ChanNum,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale, &DSP_CLOCK_MHZ );


	ndev=8;		/* threshold will be 8 times sigma */
	ndat=IO_BUFFER_LENGTH;

	sprintf(str,"XWAIT%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	Xwait=(double)Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	dt=Xwait/SYSTEM_CLOCK_MHZ*1e-6;
	xx=dt/(*Tau);
	c0=exp(-xx*(FL+FG));

	kmin=2*FL+FG;
	/* zero out the initial part,where the true filter values are unknown */
	for(k=0;k<kmin;k+=1)
	{
		FF[k]=0;
	}

	for(k=kmin;k<ndat;k+=1)
	{
		sum0=0;	sum1=0;
		for(n=0;n<FL;n++)
		{
			sum0+=Trace[k-kmin+n];
			sum1+=Trace[k-kmin+FL+FG+n];
		}
		FF[k]=sum1-sum0*c0;
	}

	/* zero out the initial part,where the true filter values are unknown */
	for(k=0;k<kmin;k+=1)
	{
		FF2[k]=0;
	}

	for(k=kmin;k<ndat;k+=1)
	{
		sum0=0;	sum1=0;
		for(n=0;n<FL;n++)
		{
			sum0+=Trace[k-kmin+n];
			sum1+=Trace[k-kmin+FL+FG+n];
		}
		FF2[k]=(sum0-sum1)/FL;
	}

	deviation=0;
	for(k=0;k<ndat;k+=2)
	{
		deviation+=fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]]);
	}

	deviation/=(ndat/2);
	threshold=ndev/2*deviation/2;

	m=0; deviation=0;
	for(k=0;k<ndat;k+=2)
	{
		if(fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]])<threshold)
		{
			m+=1;
			deviation+=fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]]);
		}
	}
	deviation/=m;
	deviation*=sqrt(PI)/2;
	threshold=ndev*deviation;

	m=0; deviation=0;
	for(k=0;k<ndat;k+=2)
	{
		if(fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]])<threshold)
		{
			m+=1;
			deviation+=fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]]);
		}
	}

	deviation/=m;
	deviation*=sqrt(PI)/2;
	threshold=ndev*deviation;

	m=0; deviation=0;
	for(k=0;k<ndat;k+=2)
	{
		if(fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]])<threshold)
		{
			m+=1;
			deviation+=fabs(FF[Random_Set[k]]-FF[Random_Set[k+1]]);
		}
	}
	deviation/=m;
	deviation*=sqrt(PI)/2;
	threshold=ndev*deviation;

	return(threshold);
}


/****************************************************************
*	BLcut_Finder function:
*		Find the BLcut value for the selected channel and return it
*		using pointer *BLcut.
*
*		Return Value:
*			 0 - successful
*			-1 - failed to start collecting baselines
*			-2 - baseline collection timed out
*
****************************************************************/

S32 BLcut_Finder (
				  U8 ModNum,			// Pixie module number
				  U8 ChanNum,			// Pixie channel number
				  double *BLcut )		// BLcut return value
{

	U16 idx, KeepLog, FilterRange, SL, SG, KeepChanNum, k, l,localBlCut;
	U32 BadBaselines, value, Wcount, buffer[IO_BUFFER_LENGTH];
	S8  str[256];
	double tim, tau, Bnorm, BLave, BLsigma, ExpFactor, b0, b1, b2, b3, baseline[IO_BUFFER_LENGTH/6];
	U16 BLCmin, BLCmax;
	S32 retval;

	U16 SYSTEM_CLOCK_MHZ = 75;
	U16 FILTER_CLOCK_MHZ = 75;
	U16	ADC_CLOCK_MHZ = 75;
	U16	DSP_CLOCK_MHZ = 75;
	U16 CTscale = 16;
	U16 BoardVersion;

	if (Offline == 1) {
		sprintf(ErrMSG, "(BLcut_Finder): Offline mode. No I/O operations possible");
		Pixie_Print_MSG(ErrMSG,1);
		return (0);
	}

	// Define clock constants according to BoardRevision 
	Pixie_Define_Clocks (ModNum,ChanNum,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale, &DSP_CLOCK_MHZ );



	/*****************************************************
	*
	*	Set proper DSP parameters for collecting baselines 
	*
	*****************************************************/

	/* Store the DSP parameter Log2BWeight value */
	sprintf(str,"LOG2BWEIGHT%d", ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	KeepLog=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];
	/* Temporarily set Log2BWeight to 0 */
	value=0;
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=(U16)value;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	/* Get the values of DSP parameters FilterRange, SlowLength, SlowGap and BLCut */
	idx=Find_Xact_Match("FILTERRANGE", DSP_Parameter_Names, N_DSP_PAR);
	FilterRange=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	sprintf(str,"SLOWLENGTH%d", ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	SL=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	sprintf(str,"SLOWGAP%d", ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	SG=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

	/* Temporarily set BLcut to 0 */
	sprintf(str,"BLCUT%d", ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	localBlCut=0;
	value=(U32)localBlCut;
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=localBlCut;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	/* Set the DSP parameter ChanNum */
	idx=Find_Xact_Match("CHANNUM", DSP_Parameter_Names, N_DSP_PAR);
	KeepChanNum=Pixie_Devices[ModNum].DSP_Parameter_Values[idx];
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=ChanNum;
	value=(U32)ChanNum;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	BoardVersion = (U16)Pixie_Devices[ModNum].Module_Parameter_Values[BoardVersion_Index];
	switch (PCIBusType) {
		case REGULAR_PCI: /* Pixie-4, Pixie-500 */


			/*****************************************************
			*
			*	Start to collect baselines 
			*
			*****************************************************/

			tim=(double)(SL+SG)*pow(2.0, (double)FilterRange);

			/* Get DSP parameter PreampTau */
			sprintf(str,"PREAMPTAUA%d", ChanNum);
			idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
			tau=(double)Pixie_Devices[ModNum].DSP_Parameter_Values[idx];

			sprintf(str,"PREAMPTAUB%d", ChanNum);
			idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
			tau+=(double)Pixie_Devices[ModNum].DSP_Parameter_Values[idx]/65536;

			/* Calculate necessary bit shift for the baseline sums */
			/* baseline sums are truncated (FilterRange-2) bits in FPGA if FilterRange is */
			/* greater than 2; for all FilterRanges, baseline sums are upshifted by 6 bits. */
			/* no shift or truncation for P500e @ FilterRange 0-5, 2bit shift for FilterRange 6,7 */
			/* but 4 samples averaged into one for filter input */

			// Note: Pixie-500e  (not used in this mode)
			Bnorm = pow(2.0, (double)(MAX(0,(FilterRange-2))-6))/(SL*pow(2.0, (double)(FilterRange+1)));

			/* Start Control Task COLLECT_BASES to collect 1365 baselines */
			retval=Start_Run(ModNum, NEW_RUN, 0, COLLECT_BASES);
			if(retval < 0)
			{
				sprintf(ErrMSG, "BLcut_Finder: failed to collect baselines in Module %d", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-1); /* failed to start collecting baselines */
			}

			/* Check Run Status */
			Wcount=0;
			Pixie_Sleep(1); /* A short wait before polling */
			while(Check_Run_Status(ModNum) && (Wcount<10000))
			{
				Pixie_Sleep(1);
				Wcount++; /* The maximal waiting time is 10 s */ 
			}
			if(Wcount>=10000)
			{
				sprintf(ErrMSG, "BLcut_Finder: Module %d timed out", ModNum);
				Pixie_Print_MSG(ErrMSG,1);
				return(-2); /* Time Out */
			}

			/* Read the data memory */
			Pixie_IODM(ModNum, IO_BUFFER_ADDRESS, MOD_READ, IO_BUFFER_LENGTH, buffer);

			/* Calculate BLsigma */
			BadBaselines = 0;
			l            = 0;
			BLsigma      = 0; // Baseline sigma^2
			BLave        = 0; // Baseline average
			ExpFactor    = exp(-tim / (tau * FILTER_CLOCK_MHZ));
			for(k = 0; k < IO_BUFFER_LENGTH-6; k += 6)
			{
				b0 = (double)buffer[k];
				b1 = (double)buffer[k+1];
				b2 = (double)buffer[k+2];
				b3 = (double)buffer[k+3];
				baseline[l]        = ((b2 + b3 * 65536.0) - ExpFactor * (b0 + b1 * 65536.0)) * Bnorm;
				if (l > 0) BLsigma = (BLsigma * (1.0 - 1.0 / (double)l) + 1.0 / (1.0 + (double)l) * (baseline[l] - BLave) * (baseline[l] - BLave));
				BLave              = (baseline[l] / (1.0 + (double)l) + (double)l / (1.0 + (double)l) * BLave);
				if (fabs(baseline[l] - BLave) > 4 * sqrt(BLsigma)) BadBaselines++;
				l++;
			}
			BLsigma = sqrt(BLsigma); // sigma = sqrt(sigma^2)

			/* Calculate BLcut */
			localBlCut = (U16)floor(4.0 * BLsigma);
			BLCmin = 15;			// for P4/P500, BLcut is in arbitrary units, limits are more generous
			BLCmax = 32767;			//comparison as signed integer in DSP, so must be positive


			if(localBlCut < BLCmin)		//ensure min. BLcut
			{
				localBlCut = BLCmin;
			}
			if(localBlCut > BLCmax)		
			{
				localBlCut = BLCmax;
			}
			/* Report progress or an alarming channel */
			if (BadBaselines > IO_BUFFER_LENGTH / 12) { // More than 600
				sprintf(ErrMSG, "*WARNING* Mod. %d Chan.: %d  BLave: %e BLsigma: %e BLcut: %u Bad Baselines: %u", ModNum, ChanNum, BLave, BLsigma, localBlCut, BadBaselines);
				Pixie_Print_MSG(ErrMSG,1);
			}
			else {
				//	    sprintf(ErrMSG, "Mod. %d Chan.: %d  BLave: %e BLsigma: %e BLcut: %u Bad Baselines: %u", ModNum, ChanNum, BLave, BLsigma, localBlCut, BadBaselines);
				//	    Pixie_Print_MSG(ErrMSG,1);
			}
			break;
		case EXPRESS_PCI: /* Pixie-500e */
#ifdef WINDRIVER_API
			Control_Task_Run(ModNum, FIND_BLCUT, 1000);
			idx=Find_Xact_Match("HOSTODATA", DSP_Parameter_Names, N_DSP_PAR);
			idx += ChanNum; // DSP reports BLcut values in HOSTODATA+ChanNum

			retval = Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &value );
			localBlCut = (U16)value;
			if (retval != 0) {
				sprintf(ErrMSG, "*ERROR* (BLcut_Finder): Cannot get baseline cut for module %d channel %d.", ModNum, ChanNum, localBlCut);
				Pixie_Print_MSG(ErrMSG,1);
				localBlCut = 10;
			}
			if (localBlCut == 0xFFFE) {
				sprintf(ErrMSG, "*INFO* (BLcut_Finder): Skipped baseline cut computation for module %d channel %d (not 'good').", ModNum, ChanNum,localBlCut);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
				localBlCut = 10;
			}
			if (localBlCut <10 || localBlCut == 0xFFFF) {
				sprintf(ErrMSG, "*INFO* (BLcut_Finder): Computed invalid baseline cut for module %d channel %d (%d).", ModNum, ChanNum,localBlCut);
				Pixie_Print_MSG(ErrMSG,1);
				localBlCut = 10;
			}
#endif
			break;
		default:
			break;
	} // switch

	/*****************************************************
	*
	*	Set BLcut in DSP and restore several parameters
	*
	*****************************************************/

	/* Set DSP parameter BLcut */
	sprintf(str,"BLCUT%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=localBlCut;
	/* Download BLcut to the DSP data memory */
	value = (U32)localBlCut;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	/* Restore DSP parameter Log2BWeight */
	sprintf(str,"LOG2BWEIGHT%d",ChanNum);
	idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=KeepLog;
	/* Download Log2BWeight to the DSP data memory */
	value = (U32)KeepLog;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	/* Restore DSP parameter ChanNum */
	idx=Find_Xact_Match("CHANNUM", DSP_Parameter_Names, N_DSP_PAR);
	Pixie_Devices[ModNum].DSP_Parameter_Values[idx]=KeepChanNum;
	/* Download CHANNUM to the DSP data memory */
	value = (U32)KeepChanNum;
	Pixie_IODM(ModNum, (U16)(DATA_MEMORY_ADDRESS+idx), MOD_WRITE, 1, &value);

	/* Update user value BLCUT */
	idx=Find_Xact_Match("BLCUT", Channel_Parameter_Names, N_CHANNEL_PAR);
	Pixie_Devices[ModNum].Channel_Parameter_Values[ChanNum][idx]=localBlCut;

	/* Update BLcut return value */
	*BLcut = (double)localBlCut;

	return(0);
}


/****************************************************************
*	RandomSwap function:
*		Generate a random set. The size of the set is IO_BUFFER_LENGTH.
*
****************************************************************/

S32 RandomSwap(void)
{

	U32 rshift,Ncards;
	U32 k,MixLevel,imin,imax;
	U16 a;

	for(k=0; k<IO_BUFFER_LENGTH; k++) Random_Set[k]=(U16)k;

	Ncards=IO_BUFFER_LENGTH;
	rshift= (U32)(log(((double)RAND_MAX+1.0)/(double)IO_BUFFER_LENGTH)/log(2.0));
	MixLevel=5;

	for(k=0; k<MixLevel*Ncards; k++)
	{
		imin=(rand()>>rshift); 
		imax=(rand()>>rshift);
		a=Random_Set[imax];
		Random_Set[imax]=Random_Set[imin];
		Random_Set[imin]=a;
	}

	return(0);

}




/****************************************************************
*	RoundOff function:
*		Round a floating point number to the nearest integer.
*
*		Return Value:
*			rounded 32-bit integer
*
****************************************************************/

U32 RoundOff(double x) { return((U32)floor(x+0.5)); }

/****************************************************************
*	SetBit function:
*		Set Bit function (for 16-bit words only).
*
*		Return Value:
*			16-bit integer after setting the bit
*
****************************************************************/

U16 SetBit(U16 bit, U16 value)
{
	return(value | (U16)(pow(2,bit)));
}

/****************************************************************
*	ClrBit function:
*		Clear Bit function (for 16-bit words only).
*
*		Return Value:
*			16-bit integer after clearing the bit
*
****************************************************************/

U16 ClrBit(U16 bit, U16 value)
{
	value=SetBit(bit, value);
	return(value ^ (U16)(pow(2,bit)));
}

/****************************************************************
*	TglBit function:
*		Toggle Bit function (for 16-bit words only).
*
*		Return Value:
*			16-bit integer after toggling the bit
*
****************************************************************/

U16 TglBit(U16 bit, U16 value)
{
	return(value ^ (U16)(pow(2,bit)));
}

/****************************************************************
*	TstBit function:
*		Test Bit function (for 16-bit words only).
*
*		Return Value:
*			bit value
*
****************************************************************/

U16 TstBit(U16 bit, U16 value)
{
	return(((value & (U16)(pow(2,bit))) >> bit));
}



/****************************************************************
*	Pixie_CopyExtractSettings function:
*		Copy or extract settings to the specific modules and
*		channels.
*
****************************************************************/

S32 Pixie_CopyExtractSettings (
							   U8 SourceChannel,			// source Pixie channel
							   U16 BitMask,				// copy/extract bit mask pattern
							   U8 DestinationModule,		// destination module number
							   U8 DestinationChannel,		// destination channel number
							   U16 *DSPSourceSettings )	// DSP settings of the source channel
{
	S8  str[256];
	U32 idxS, idxD, idx;
	U16 value;

	U16 SYSTEM_CLOCK_MHZ = 75;
	U16 FILTER_CLOCK_MHZ = 75;
	U16	ADC_CLOCK_MHZ = 75;
	U16	DSP_CLOCK_MHZ = 75;
	U16 CTscale = 16;

	/* Gain */
	if( TstBit(0, (U16)BitMask) )
	{
		/* Copy SGA */
		sprintf(str,"SGA%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"SGA%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy DIGGAIN */
		sprintf(str,"DIGGAIN%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"DIGGAIN%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy GAINDAC */
		sprintf(str,"GAINDAC%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"GAINDAC%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
   }

	/* Offset */
	if( TstBit(1, (U16)BitMask) )
	{
		/* Copy TRACKDAC */
		sprintf(str,"TRACKDAC%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"TRACKDAC%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy BASELINE_PERCENT */
		sprintf(str,"BASELINEPERCENT%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"BASELINEPERCENT%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* Filter */
	if( TstBit(2, (U16)BitMask) )
	{
		/* Copy FILTERRANGE */
		idx=Find_Xact_Match("FILTERRANGE", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Update energy filter interval */
		Pixie_Define_Clocks ((U8)DestinationModule,(U8)0,&SYSTEM_CLOCK_MHZ,&FILTER_CLOCK_MHZ,&ADC_CLOCK_MHZ,&CTscale,&DSP_CLOCK_MHZ );
		Filter_Int[DestinationModule]=pow(2.0, (double)Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx])/FILTER_CLOCK_MHZ;

		/* Copy SLOWLENGTH */
		sprintf(str,"SLOWLENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"SLOWLENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy SLOWGAP */
		sprintf(str,"SLOWGAP%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"SLOWGAP%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy PEAKSEP */
		sprintf(str,"PEAKSEP%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"PEAKSEP%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy LOG2BWEIGHT */
		sprintf(str,"LOG2BWEIGHT%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"LOG2BWEIGHT%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy BLCUT */
		sprintf(str,"BLCUT%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"BLCUT%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* Trigger */
	if( TstBit(3, (U16)BitMask) )
	{
		/* Copy FASTLENGTH */
		sprintf(str,"FASTLENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"FASTLENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy FASTGAP */
		sprintf(str,"FASTGAP%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"FASTGAP%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy FASTTHRESH */
		sprintf(str,"FASTTHRESH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"FASTTHRESH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* FIFO */
	if( TstBit(4, (U16)BitMask) )
	{
		/* Copy RESETDELAY */
		sprintf(str,"RESETDELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"RESETDELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy TRACELENGTH */
		sprintf(str,"TRACELENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"TRACELENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy XWAIT */
		sprintf(str,"XWAIT%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"XWAIT%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy XAVG */
		sprintf(str,"XAVG%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"XAVG%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy USERDELAY */
		sprintf(str,"USERDELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"USERDELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* CSR */
	if( TstBit(5, (U16)BitMask) )
	{
		/* Copy CHANCSRA */
		sprintf(str,"CHANCSRA%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"CHANCSRA%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy CHANCSRB */
		sprintf(str,"CHANCSRB%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"CHANCSRB%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy CHANCSRC */
		sprintf(str,"CHANCSRC%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"CHANCSRC%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* Coincidence Pattern */
	if( TstBit(6, (U16)BitMask) )
	{
		/* Copy COINCPATTERN */
		idx=Find_Xact_Match("COINCPATTERN", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy COINCWAIT */
		idx=Find_Xact_Match("COINCWAIT", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy COINCDELAY */
		sprintf(str,"COINCDELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"COINCDELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* MCA */
	if( TstBit(7, (U16)BitMask) )
	{
		/* Copy LOG2EBIN */
		sprintf(str,"LOG2EBIN%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"LOG2EBIN%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* TAU */
	if( TstBit(8, (U16)BitMask) )
	{
		/* Copy PREAMPTAUA */
		sprintf(str,"PREAMPTAUA%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"PREAMPTAUA%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy PREAMPTAUB */
		sprintf(str,"PREAMPTAUB%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"PREAMPTAUB%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* INTEGRATOR */
	if( TstBit(9, (U16)BitMask) )
	{
		/* Copy INTEGRATOR */
		sprintf(str,"INTEGRATOR%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"INTEGRATOR%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	/* ModCSR and ModulePattern */
	if( TstBit(10, (U16)BitMask) )
	{
		/* Copy MODCSRA */
		idx=Find_Xact_Match("MODCSRA", DSP_Parameter_Names, N_DSP_PAR);
		value=DSPSourceSettings[idx];
		value = ClrBit(5, value);		// clear the bit for front panel veto input, only one module can be enabled
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=value;

		/* Copy MODCSRB */
		idx=Find_Xact_Match("MODCSRB", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy MODCSRC */
		idx=Find_Xact_Match("MODCSRC", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy DBLBUFCSR */
		idx=Find_Xact_Match("DBLBUFCSR", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy ModulePattern */
		idx=Find_Xact_Match("MODULEPATTERN", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];

		/* Copy NNSHAREPATTERN */
		idx=Find_Xact_Match("NNSHAREPATTERN", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idx]=DSPSourceSettings[idx];
	}

	/* GATE */
	if( TstBit(11, (U16)BitMask) )
	{
		/* Copy GATEWINDOW */
		sprintf(str,"GATEWINDOW%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"GATEWINDOW%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy GATEDELAY */
		sprintf(str,"GATEDELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"GATEDELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

   /* PSA */
	if( TstBit(12, (U16)BitMask) )
	{
		/* Copy PSAOFFSET */
		sprintf(str,"PSAOFFSET%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"PSAOFFSET%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy PSALENGTH */
		sprintf(str,"PSALENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"PSALENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

		/* Copy CFDTHR */
		sprintf(str,"CFDTHR%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"CFDTHR%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

  		/* Copy QDC0LENGTH */
		sprintf(str,"QDC0LENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"QDC0LENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

  		/* Copy QDC1LENGTH */
		sprintf(str,"QDC1LENGTH%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"QDC1LENGTH%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

  		/* Copy QDC0DELAY */
		sprintf(str,"QDC0DELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"QDC0DELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];

  		/* Copy QDC1DELAY */
		sprintf(str,"QDC1DELAY%d",SourceChannel);
		idxS=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		sprintf(str,"QDC1DELAY%d",DestinationChannel);
		idxD=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);
		Pixie_Devices[DestinationModule].DSP_Parameter_Values[idxD]=DSPSourceSettings[idxS];
	}

	return(0);
}




/****************************************************************
*	Make_SGA_Gain_Table function:
*		This routine generates the SGA gain table for Pixie module's
*		gain setting.
*
****************************************************************/


S32 Make_SGA_Gain_Table(void)
{

	U32 k;
	double RG,RF;

	for(k=0;k<N_SGA_GAIN;k++)
	{
		/* RF = 1200 + 120 (if bit 0 not set) + 270 (if bit 1 not set) + 560 (if bit 2 not set) */ 
		RF = 2150 - 120*((k>>0)&1) - 270*((k>>1)&1) - 560*((k>>2)&1);
		/* RG = 100 + 100 (if bit 3 not set) + 100 (if bit 4 not set) + 150 (if bit 5 not set) + 820 (if bit 6 not set) */ 
		RG =  1320 - 0*((k>>3)&1) - 100*((k>>4)&1) - 300*((k>>5)&1) - 820*((k>>6)&1);   
		SGA_Computed_Gain[k]=(1 + RF/RG)/2;
	}
	return(0);
}




/****************************************************************
*	Pixie_Print_MSG function:
*		This routine prints error message or other message
*		either to Igor history window or a text file.
*
****************************************************************/

S32 Pixie_Print_MSG (
					 S8 *message, 	// message to be printed 
					 U32 enable )	// print it or not
{
	FILE *PIXIEmsg = NULL;
		
	if(!enable)	return(0);



#ifdef COMPILE_IGOR_XOP
	if(PrintDebugMsg_file) {
		// debug: optionally print the message to file
		

		PIXIEmsg = fopen("PIXIEmsg.txt", "a");
		if(PIXIEmsg != NULL)
		{
			fprintf(PIXIEmsg, "%s\n", message);
			fclose(PIXIEmsg);
		}
		Pixie_Sleep(2);
	} else {


		//If polling thread (current thread ID is pollingThreadId which is set in Pixie4_Acquire_Data(0x1403),
		//don't call XOPNotice, but write into msgBuffer.
		//And don't forget to dump msgBuffer into XOPNotice and clear it on Igor poll with Pixie_Acquire_Data(0x4403).

		//if (GetCurrentThreadId()==pollingThreadId) {
		if (GetCurrentThreadId() != gMainThreadId) { // if not main thread
				// Add carriage return character '\r' and flag 'MT' for XOPNotice from multithread or interrupt
			strcat(message, " MT\r");
			 //TODO
			// It is possible to get msgBuffer overflow if too many messages from the polling thread that are not XOPNotice'd to Igor.
			strcat(msgBuffer, message);
			return(0);
		} else {
			// Add carriage return character '\r' for XOPNotice
			strcat(message, "\r");
			XOPNotice(message);
		}
	}
		
#else
		// for LV dll etc
		//FILE *PIXIEmsg = NULL;

		PIXIEmsg = fopen("PIXIEmsg.txt", "a");
		if(PIXIEmsg != NULL)
		{
			/* Add new line character '\n' for printf */
			strcat(message, "\n");
			printf(message);

			fprintf(PIXIEmsg, "%s", message);
			fclose(PIXIEmsg);
		}

#endif

	return(0);
}

/****************************************************************
*	FlushIgorMSG function:
*		This routine prints accumulated messages in Igor multithread runs
*       or interrupt driven readout and clears the buffer afterwards
*
****************************************************************/
S32 FlushIgorMSG()
{
#ifdef COMPILE_IGOR_XOP
					// Print out msgBuffer in Igor
					XOPNotice(msgBuffer);
#endif
					// clean the last message buffer
					memset(msgBuffer, 0, sizeof(msgBuffer));
	return (0);
}


/****************************************************************
*	Pixie_Define_Clocks function:
*		Define clock constants according to BoardRevision .
*
*		Return Value:
*			 0 - success
*			-1 - invalid BoardRevision
*
****************************************************************/


S32 Pixie_Define_Clocks (

						 U8  ModNum,				// Pixie module number
						 U8  ChanNum,			// Pixie channel number
						 U16 *SYSTEM_CLOCK_MHZ,	// system clock -- coincidence window, 
						 U16 *FILTER_CLOCK_MHZ,	// filter/processing clock -- filter lengths, runs statistics
						 U16 *ADC_CLOCK_MHZ,		// sampling clock of the ADC
						 U16 *CTscale,			// The scaling factor for count time counters
						 U16 *DSP_CLOCK_MHZ	// DSP clock -- DSP time stamps
					 )		// 
{

	U16	BoardRevision;


	// Define clock constants according to BoardRevision 
	BoardRevision = (U16)Pixie_Devices[ModNum].Module_Parameter_Values[BoardVersion_Index];

	//	sprintf(ErrMSG, "*INFO* (Pixie_Define_Clocks): BoardRevision (Xilinx) for Module %d is %X", ModNum,BoardRevision);
	//	Pixie_Print_MSG(ErrMSG,1);

	if((BoardRevision & 0x0F00) == MODULETYPE_P4) 
	{
		*SYSTEM_CLOCK_MHZ = P4_SYSTEM_CLOCK_MHZ; // For Pixie-4
		*FILTER_CLOCK_MHZ = P4_FILTER_CLOCK_MHZ;
		*ADC_CLOCK_MHZ    = P4_ADC_CLOCK_MHZ;
		*DSP_CLOCK_MHZ    = P4_DSP_CLOCK_MHZ;
		*CTscale		  = P4_CTSCALE;
	}
	else if((BoardRevision & 0x0FF0) == MODULETYPE_P4e_16_125) 
	{
		*SYSTEM_CLOCK_MHZ = P4E_SYSTEM_CLOCK_MHZ; // For Pixie-4 express (125 MHz version)
		*FILTER_CLOCK_MHZ = P4E_FILTER_CLOCK_MHZ;
		*ADC_CLOCK_MHZ    = P4E125_ADC_CLOCK_MHZ;
		*CTscale		  = P4E_CTSCALE;
		*DSP_CLOCK_MHZ    = P4E_DSP_CLOCK_MHZ;		//
	}
	else if((BoardRevision & 0x0FF0) == MODULETYPE_P4e_14_500) 
	{
		*SYSTEM_CLOCK_MHZ = P4E_SYSTEM_CLOCK_MHZ; // For Pixie-4 express (125 MHz version)
		*FILTER_CLOCK_MHZ = P4E_FILTER_CLOCK_MHZ;
		*ADC_CLOCK_MHZ    = P4E500_ADC_CLOCK_MHZ;
		*CTscale		  = P4E_CTSCALE;
		*DSP_CLOCK_MHZ    = P4E_DSP_CLOCK_MHZ;		//
	}

	else if((BoardRevision & 0x0F00)  == MODULETYPE_P500e) 
	{
		*SYSTEM_CLOCK_MHZ = P500E_SYSTEM_CLOCK_MHZ; // For P500e
		*FILTER_CLOCK_MHZ = P500E_FILTER_CLOCK_MHZ;
		*ADC_CLOCK_MHZ    = P500E_ADC_CLOCK_MHZ;
		*CTscale		  = P500E_CTSCALE;
		*DSP_CLOCK_MHZ    = P500E_DSP_CLOCK_MHZ;		//
	}
	else
	{
		sprintf(ErrMSG, "*Error* (Pixie_Define_Clocks): Unknown BoardRevision %X (M %d Ch %d), time constants defaulting to Pixie-500e", BoardRevision, ModNum, ChanNum );
		Pixie_Print_MSG(ErrMSG,1);
		*SYSTEM_CLOCK_MHZ = P500E_SYSTEM_CLOCK_MHZ; // For P500e
		*FILTER_CLOCK_MHZ = P500E_FILTER_CLOCK_MHZ;
		*ADC_CLOCK_MHZ    = P500E_ADC_CLOCK_MHZ;
		*CTscale		  = P500E_CTSCALE;
		*DSP_CLOCK_MHZ    = P500E_DSP_CLOCK_MHZ;	
		return(-1); // Possible that EEPROM has not been programmed yet. Fall back on defaults and carry on.
	}

	return(0);
}


/****************************************************************
*	Pixie_Check_Moduletypes function:
*		Check if all modules are Express type or not per BoardRevision .
*
*		Return Value:
*			 2 - all non-Express
*			 1 - all Express
*		     0 - mixed
*			-1 - invalid BoardRevision
*
****************************************************************/


S32 Pixie_Check_Moduletypes (						 
						 U8  ModNum)				// Pixie module number
{
	U16	BoardRevision;
	U16 allexpress, allnonexpress, ret;
	U32 CurrentModNum, MNstart, MNend;	// for looping over modules if ModNum==Number_Modules

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

	allexpress=1;
	allnonexpress=1;
	for(CurrentModNum = MNstart; CurrentModNum < MNend ; CurrentModNum ++)
	{	
		BoardRevision = (U16)Pixie_Devices[CurrentModNum].Module_Parameter_Values[Find_Xact_Match("BOARD_VERSION", Module_Parameter_Names, N_MODULE_PAR)];
	//	sprintf(ErrMSG, "*INFO* (Pixie_Check_Moduletypes): BoardRevision 0x %x",BoardRevision);
	//	Pixie_Print_MSG(ErrMSG,1);
		if ( ((BoardRevision & 0x0F00) == MODULETYPE_P500e) || ((BoardRevision & 0x0F00) == MODULETYPE_P4e)  )// Pixie-500e or P4e
		{
			allnonexpress = 0;
		}
		else
		{
			allexpress = 0;
		}

	} // for modules

	ret = 2*allnonexpress+allexpress;
	return(ret);
}

/****************************************************************
*	Write_DMA_List_Mode_File function:
*		Read out data from DMA buffer to file, one module
*		return values
*			<0: error
*			 0: ok
*		     1: end of run detected
****************************************************************/


S32 Write_DMA_List_Mode_File (						 
							  U8  ModNum, 				// Pixie module number
							  S8  *FileName ,		// List mode data file name
							  U16 RunType)          // Run type (binary vs ASCII file dump), lower 12 bits



{
#define DUMP
//#define MAKE_CHAN_HEAD_ERRORS

	size_t elementsWritten = 0;
	U32 value, i, j;
	U32 bufPtr, currentDWord, EvStart;
	U32 traceBlocksFollow, traceBlocksPrev, traceBlocksMismatchCount; // traceBlocksMismatchCount: TRACELENGTH != trace blocks to follow in channnel header
	U32 goodEventCount;			// actually TOTAL events written
	U32 badEventCount;			// badEventCount: event with missing 64-bit word or other incorrect trace
	U32 splitHeaderCount, splitTraceCount; // split[Header,Trace]Count: event split over two framebuffers
	U32 offsetWordCount;		// event does not start at beginning of framebuffer (because of split trace/header)
	U32 badChanNumCount;		// wrong channel number
	U32 badWatermarkCount;		// corrupted watermark
	S32 checkSum, checkSumComputed;
	U32 checkSumMismatchCount;	// corrupted checksum
	BOOL nextWMfound = FALSE;	// indicates we found a watermark
	BOOL nextWMoutside = FALSE;	// indicates WM location ouside buffer
	//S32 EndRunFound =0;
	U16 EventLengthDSP;

// 32-bit words of channel header, defined in reader.h, written by DSP in main.asm (LMprocessing)
/*	const U32 chanHeadEventStatusIdx = 0;
	const U32 chanHeadNumBlocksIdx = 1;
	const U32 chanHeadLoMidTrigTimeIdx = 2;
	const U32 chanHeadHiTrigTimeIdx = 3;
	const U32 chanHeadEnChanIdx = 4;
	const U32 chanHeadPSAIdx = 5;
	const U32 chanHeadPSA0Idx = 6;
	const U32 chanHeadPSA1Idx = 7;
	const U32 chanHeadCheckSumIdx = 14;
	const U32 chanHeadWatermarkIdx = 15; // positions in Channel Header, counted in 32-bit words	 
*/
	size_t eventsWritten;

	U32 numDWordsChanHead = MAX_CHAN_HEAD_LENGTH * sizeof(U16)/sizeof(U32); // size of channel header in 32-bit words
	U32 numDWordsTrace; // size of trace in 32-bit words, traceBlocksFollow*16
	U32 numDWordsBuf = DMA_LM_FRAMEBUFFER_LENGTH/sizeof(U32); // size of the DMA framebuffer in 32-bit words
	U32 goodEventBytes = 0;
	U32 numDWordsRemaining;
	U32 numDWordsToWrite;
	U32 numDWordsToAdvance;
	U16 ChanNum, hit; // current event channel number and hit pattern
	U32 traceBlocksFollow_QC; // trace length to follow according to QC
	U32 *dumpBuffer = NULL; // copy of the DMA framebuffer, used for writing to the file with "FileName"

	U32 *pLMBufferCopy; // local pointer to current module LMBufferCopy
	

	if(listFile[ModNum] == NULL)
	{
		sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): file not found error");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	} // if binary file opened


#ifdef WINDRIVER_API
		// BUFFER QUALITY CHECK:
		// Once in a while we are dopping 8 bytes from the data stream.
		// This leads to misaligned list mode structure and reader not being able to parse the events.
		// Check and correct events of wrong size.

		// Check the frame buffer for incomplete events (we have known tags (watermarks) in channel header),
		// and for missing 2 32-bit words in end of channel header or trace.
		// 1. Read the buffer until channel header is detected.
		// 2. Read trace, confirm that it is full (next channel header in the right place).
		// 3a. If cool, write the event to disk.
		// 3b. If not, write as if header's trace length is correct (likely junk at end of trace), mark event as bad
	    // 4. go back to 1. 
		// Events that start/end not on buffer boundary are discarded and counted as split. WH_TODO: improve that


		bufPtr = 0;
		currentDWord = 0;
		traceBlocksFollow = 0;
		traceBlocksFollow_QC = 0;
		checkSum = 0;
		checkSumComputed = 0;

		goodEventCount = 0;
		badEventCount = 0;
		splitHeaderCount = 0;
		splitTraceCount = 0;
		offsetWordCount = 0;
		traceBlocksMismatchCount = 0;
		badWatermarkCount = 0;
		badChanNumCount = 0;
		checkSumMismatchCount = 0;

//		EndRunFound[ModNum] = 0;		// global: initialize to zero every time we process a new buffer

		if (!LMBuffer[ModNum]) {
			sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): LMBuffer = 0");
			Pixie_Print_MSG(ErrMSG,1);
			return (-1);
		}
		if (!LMBufferCopy[ModNum]) {
			sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): LMBufferCopy = 0");
			Pixie_Print_MSG(ErrMSG,1);
			return (-1);
		}

		pLMBufferCopy = LMBufferCopy[ModNum];

		if(!BufferQC)
		{
#ifdef DUMP
			eventsWritten = fwrite(LMBuffer[ModNum], DMA_LM_FRAMEBUFFER_LENGTH, 1, listFile[ModNum]);
#endif		
			// Set the last element to a known pattern, change of which will be used as DMA idle indicator.
			LMBuffer[ModNum][DMA_LM_FRAMEBUFFER_LENGTH/(sizeof(U32))-1] = 0xA5A5A5A5;

			VDMADriver_SetDPTR(hDev[ModNum], MAIN_START);		// rewind DMA sequencer
			VDMADriver_Go(hDev[ModNum]);						// resume DMA (that was halted by finishing the SG list)
			sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): Sequencer restarted, no QC");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);

			LMBufferCounter[ModNum]++;
			sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): Done Write_DMA_List_Mode_File with buffer %d, no QC",LMBufferCounter[ModNum]);
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);

			if(MakeNewFile)	{	// if a top level call asked for new file in multi-file runs, make/switch files now for all modules
				// TODO: insert an EOR block at the end of the file
				for(j=0; j < Number_Modules; j++) {
					Create_List_Mode_File(j, next_base_name, RunType);
				}
			}

			return(0);
		}

		// if BufferQC
		sprintf(ErrMSG, "*INFO*  (Write_DMA_List_Mode_File): Starting BUFFER QUALITY CHECK, module %d",ModNum);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);

		// first, write leftover from previous buffer to file
		// but then start looking for watermark of next event from beginning of file, in case the leftover is a short trace
		if(numDWordsLeftover[ModNum]>0) {
			eventsWritten = fwrite(LMBuffer[ModNum], (numDWordsLeftover[ModNum])*sizeof(U32), 1, listFile[ModNum]);
		}

		while (bufPtr < numDWordsBuf) {

			// check if we can read complete channel header
			numDWordsRemaining = numDWordsBuf - bufPtr;
			if ( numDWordsChanHead > numDWordsRemaining ) 
			{ 
				splitHeaderCount++;
				sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B discarding multi-buffer event (split channel header), split count %d", bufPtr*sizeof(U32), splitHeaderCount);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				//bufPtr += numDWordsChanHead; // to jump out of while loop
				break;
			} // end if cannot read complete channel header

			if  (RunType==0x402) 	{	// run type 0x402 does not have the channel number in the usual place
					ChanNum = 0;		// default to zero 
					EventLengthDSP = EventLengthTotal[ModNum]; // EventLengthTotal = 1x header + 4x TL (in blocks) in runtype 0x402
			} else {
					ChanNum = (U16)((LMBuffer[ModNum][bufPtr+chanHeadEnChanIdx] & 0xFFFF0000) >> 16); 
					ChanNum = ChanNum & 0x00FF;		// upper bits of channel number reserved for special records
					EventLengthDSP = EventLength[ModNum][ChanNum];
			}

			// WATERMARK CHECK BEGIN
			// if watermark in place, event starts here
			currentDWord = LMBuffer[ModNum][bufPtr+chanHeadWatermarkIdx]; // watermark word

			if(	currentDWord == WATERMARK)	// first try, watermark might just be in place and correct
			{
				value=8;
			}
			else	// if not, maybe just a few digits are wrong: check
			{					
				// give a point for a number at correct position: 0 (no match) to 8 points (full match)
				value = 0;
				for (i = 0; i < 8; i++) {
					if ( ((currentDWord >> (i*4)) & 0xF) == ((WATERMARK >> (i*4)) & 0xF)) 
						value++;
				}
				
				// if watermark word is almost correct, but not full match: still count as OK, fix and continue.
				if (value > 6 &&  value < 8) {  // for more robust watermark word, change acceptance level.
					sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B bad watermark 0x%08X, fixed. (module %d, event %d)", bufPtr*4, currentDWord, ModNum, goodEventCount);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					badWatermarkCount++;
					LMBuffer[ModNum][bufPtr+chanHeadWatermarkIdx] = WATERMARK; // corrected
					LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad
				}
			}

			if (value<=6)  // if more than two digits are wrong: assume that this is not the watermark, go to next word
			{
				bufPtr++;
				offsetWordCount++;
				continue; // next word
			} 
			// WATERMARK CHECK END

#ifdef MAKE_CHAN_HEAD_ERRORS
			// DEBUG Breaking some channel header words
			if (goodEventCount %2 == 1) {
				LMBuffer[ModNum][bufPtr+1] = LMBuffer[ModNum][bufPtr+1]+ 0x10000;
			}
#endif

			traceBlocksFollow = (LMBuffer[ModNum][bufPtr+chanHeadNumBlocksIdx] & 0x0000FFFF);
			traceBlocksPrev   = (LMBuffer[ModNum][bufPtr+chanHeadNumBlocksIdx] & 0xFFFF0000) >> 16;
			numDWordsTrace    = traceBlocksFollow*BLOCKSIZE/2;

			// CHECK SUM CHECK BEGIN
			checkSum = LMBuffer[ModNum][bufPtr+chanHeadCheckSumIdx];
			checkSumComputed = 0;
			for (i = 0; i < numDWordsChanHead; i++) { // checksum computation from channel header
				// Checksum only contains words written in main.sam(LMprocessing())
				if (i == chanHeadEventStatusIdx ||
					i == chanHeadNumBlocksIdx ||
					i == chanHeadLoMidTrigTimeIdx ||
					i == chanHeadHiTrigTimeIdx ||
					i == chanHeadEnChanIdx ||
					i == chanHeadPSAIdx ||
					i == chanHeadPSA0Idx ||
					i == chanHeadPSA1Idx) {
						checkSumComputed = (checkSumComputed^LMBuffer[ModNum][bufPtr+i]);
				}
			} // end for channel header words used for checksum

			// If channel header corrupted, mark it as bad event
			// then, check and correct a few key values
			// channel number: 
			//		- can be compared to hit pattern, use that if header value is unreasonable
			// previous trace length: 
			//		- known from before
			// current trace length: 
			//		- can be expected to be DSP parameter value (or less in special code)
			//		  this channel's DSP parameter value is stored in file header (TL#_DSP)
			//		- try reasonable values then ([maybe] search for next watermark to verify header value (up to TL#_DSP)
			//		- if true length less than TL#_DSP, correct header with that value and store that much trace
			//		- if true length more than TL#_DSP (ie, watermark not found), correct header with TL#_DSP and store that much trace
			//		  (excess will be discarded in watermark search next time around)
			//		- if end of buffer earlier than TL#_DSP, treat as split event (ie currently: discard)
			// Note: if checksum is ok and tracelength written is incorrect (more or less data), 
			// event should be marked bad and saved with trace length as in header. Missing data will be 
			// filled with junk (actually next channel header), excess data will be truncated in watermark search next time around
			// This is applied below

			if (checkSumComputed != checkSum) {
				sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B bad checksum, expected 0x%08X, calculated 0x%08X (module %d event %d)", bufPtr*4, checkSum, checkSumComputed, ModNum, goodEventCount);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				checkSumMismatchCount++;
				LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad
			} // CHECK SUM CHECK END

			// CHANNEL NUMBER CHECK BEGIN
			if  (RunType!=0x402) 	{	// run type 0x402 does not have the channel number in the usual place, can't check
				ChanNum = (U16)((LMBuffer[ModNum][bufPtr+chanHeadEnChanIdx] & 0xFFFF0000) >> 16); 
				ChanNum = ChanNum & 0x00FF;		// upper bits of channel number reserved for special records
				if (ChanNum >= NUMBER_OF_CHANNELS) {
					badChanNumCount++;								
					sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B wrong channel number %d", bufPtr*4, ChanNum);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					// uncomment 2 lines below to reject such events
					//bufPtr+=numDWordsChanHead; // skip forward
					//continue; // no further processing of this event
					LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad

					// try to recover
					hit = (U16)(LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] & 0x000F);
					switch(hit)
					{  
						case 0x1:
							ChanNum = 0;
							break;
						case 0x2:
							ChanNum = 1;
							break;
						case 0x4:
							ChanNum = 2;
							break;
						case 0x8:
							ChanNum = 3;
							break;
						default: 
							ChanNum = 0;		// default to zero if both header and hit are bad 
							break;
					}
					// reconstruct energy (lo), channel (hi)
					value = (LMBuffer[ModNum][bufPtr+chanHeadEnChanIdx] & 0xFFFF) + (ChanNum << 16);
					LMBuffer[ModNum][bufPtr+chanHeadEnChanIdx] = value;
				}
				EventLengthDSP = EventLength[ModNum][ChanNum];
			} // CHANNEL NUMBER CHECK END



			
			// EVENT LENGTH CHECK  A) trace length previously
			if(traceBlocksPrev != traceBlocksPrev_QC[ModNum]) {
				printf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B prev. tracelength mismatch (fixed). measured %d, header %d (module %d, event %d)", bufPtr*4, traceBlocksPrev_QC[ModNum], traceBlocksPrev, ModNum, goodEventCount);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				traceBlocksPrev = traceBlocksPrev_QC[ModNum]; // use the value remembered
				traceBlocksMismatchCount++;
				LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad
				LMBuffer[ModNum][bufPtr+chanHeadNumBlocksIdx] = traceBlocksFollow + (traceBlocksPrev << 16); // update with correct length
			} // END EVENT LENGTH CHECK A)


			// BEGIN CHECK FOR END OF RUN 
			if ((LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] &0x0F00000F )==EORMARK) {	// special record: end run
				sprintf(ErrMSG, "*INFO*  (Write_DMA_List_Mode_File): END of data, last TimeStamp=%u", LMBuffer[ModNum][bufPtr+2]);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
#ifdef DUMP	
				memcpy(pLMBufferCopy+goodEventBytes/sizeof(U32), &LMBuffer[ModNum][bufPtr], (numDWordsChanHead)*sizeof(U32));
				goodEventBytes += (numDWordsChanHead)*sizeof(U32);
#endif
				EndRunFound[ModNum] = 1;
				bufPtr += (numDWordsChanHead); // advance only to trace, then step to next event
				break; // no further processing of buffer
			} 
			else {
				// EVENT LENGTH CHECK  B) trace length to follow. j will return the true trace length to write
				nextWMfound = FALSE;	
				nextWMoutside = FALSE;	// if _any_ of the checks find WM would be outside buffer, we assume it is. not 100% correct considering header value may be way off
				// first, try if recorded value is ok
				j = traceBlocksFollow*BLOCKSIZE/2 + chanHeadWatermarkIdx;				// operating on 32bit variables, but blocks are measured for 16 bit numbers
				if(numDWordsRemaining-numDWordsChanHead > j) {							// only if there are enough words left
					if(LMBuffer[ModNum][bufPtr+numDWordsChanHead+j] == WATERMARK)		// check for watermark
						nextWMfound = TRUE;
				}
				else {
					nextWMoutside = TRUE;
				}

				// second, try if zero (special code removing trace or special record with no trace and wrong channel number)
				if(!nextWMfound) {
					j = 0 + chanHeadWatermarkIdx; // 
					if(numDWordsRemaining-numDWordsChanHead > j) {						// only if there are enough words left
						if(LMBuffer[ModNum][bufPtr+numDWordsChanHead+j] == WATERMARK)	// check for watermark
							nextWMfound = TRUE;
					}
					else {
						nextWMoutside = TRUE;
					}
				}

				// third, try if nominal value of DSP parameter is ok (should be so, unless special code recording shorter waveform or channel number wrong)
				if(!nextWMfound) {
					j = EventLengthDSP*BLOCKSIZE/2 - numDWordsChanHead + chanHeadWatermarkIdx;	// event length is trace length plus header, in blocks, so subtract that for tracelength
					if(numDWordsRemaining-numDWordsChanHead > j) {										// only if there are enough words left
						if(LMBuffer[ModNum][bufPtr+numDWordsChanHead+j] == WATERMARK)					// check for watermark
							nextWMfound = TRUE;
					}
					else {
						nextWMoutside = TRUE;
					}
				}

				// If nextWMfound, j is the offset to the next watermark (minus header, but including watermarkindex).
				// if not found,   j is the last try, which is the DSP setting
				//					this may be a short event, but we store a full TL anyway


				// correct trace length if neccesary
				numDWordsTrace = j - chanHeadWatermarkIdx;									// trace: number of Dwords
				traceBlocksFollow_QC = (U32)ceil( (double)numDWordsTrace/BLOCKSIZE*2);		// trace: number of blocks (rounded up)
				if ((traceBlocksFollow_QC != traceBlocksFollow) && !nextWMoutside ) {		// this only checks for mismatch between header value and value used to record. short events (!nextWMfound but channel header TL matches run header TL) are handled below
					if (nextWMfound)  sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B tracelength mismatch (fixed). Measured %d, header %d, (module %d event %d)", bufPtr*sizeof(U32), traceBlocksFollow_QC, traceBlocksFollow, ModNum, goodEventCount);
					else			  sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B tracelength mismatch (fixed). WM not found, using %d, header %d, (module %d, event %d)", bufPtr*sizeof(U32), traceBlocksFollow_QC, traceBlocksFollow, ModNum, goodEventCount);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					traceBlocksMismatchCount++;
					LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad
					LMBuffer[ModNum][bufPtr+chanHeadNumBlocksIdx] = traceBlocksFollow_QC + (traceBlocksPrev << 16);	// update with correct length
				}
				else {
					if (nextWMoutside) {
						sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): @ 0x%08X B tracelength could not be verified, next WM beyond buffer.  Using %d, header %d, (module %d, event %d)", bufPtr*sizeof(U32), traceBlocksFollow_QC, traceBlocksFollow, ModNum, goodEventCount);
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
					}
				}
				
				traceBlocksPrev_QC[ModNum] = (U16)traceBlocksFollow_QC;		// current follow becomes next event's previous
				// END EVENT LENGTH CHECK B)
			}	// END CHECK FOR END OF RUN 

			
			// Now prepare to write to file					
			numDWordsToWrite   = numDWordsChanHead + numDWordsTrace;		// set up defaults for good event
			numDWordsToAdvance = numDWordsToWrite;
			numDWordsLeftover[ModNum]  = 0; 

			if ( numDWordsToWrite > numDWordsRemaining) {		// split trace
				splitTraceCount++;
//LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x10000000; // debug: mark event as split
				numDWordsLeftover[ModNum]  = numDWordsToWrite - numDWordsRemaining;		// numberwords to write = lesser of numDWordsChanHead+numDWordsTrace and numDWordsRemaining	
				numDWordsToAdvance = numDWordsRemaining;
				numDWordsToWrite   = numDWordsRemaining;
				goodEventCount++;
				sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): @ 0x%08X B found multi-buffer event (split current trace), split count %d, remaining words %d (event %d)", bufPtr*sizeof(U32), splitTraceCount, numDWordsLeftover[ModNum],goodEventCount-1);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
			}
			else {
				if (numDWordsToWrite + numDWordsChanHead > numDWordsRemaining) {	// split (or no) next header. no need to worry here, current event is ok, next will be investigated next cycle
					goodEventCount++;
					sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): @ 0x%08X B found split next header (or none if remainder 0), remaining words %d (event %d)", bufPtr*sizeof(U32), numDWordsLeftover[ModNum],goodEventCount-1);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
				}
				else {
					if(nextWMfound)		// truly good
						goodEventCount++;
					else				// short trace, "bad"
					{
						badEventCount++;
						goodEventCount++;	// we are counting all written to file in the goodEventCount						
						numDWordsToAdvance = numDWordsChanHead;		// to search for watermark of next event in next loop
						sprintf(ErrMSG, "*ERROR* (Write_DMA_List_Mode_File): @ 0x%08X B found short event record or corrupt next header, (module %d, event %d) ", bufPtr*sizeof(U32), ModNum, goodEventCount-1);
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
						LMBuffer[ModNum][bufPtr+chanHeadEventStatusIdx] |= 0x80000000; // mark event as bad
					}
				}
			}
		
#ifdef DUMP
			// Now finally write to file (actually, fill output buffer to write) 
			memcpy(pLMBufferCopy+goodEventBytes/sizeof(U32), &LMBuffer[ModNum][bufPtr], (numDWordsToWrite)*sizeof(U32));
			goodEventBytes += (numDWordsToWrite)*sizeof(U32);
#endif		
			bufPtr += (numDWordsToAdvance); // increment to next event (most cases)
		//	continue;
		} // END WHILE INSIDE BUFFER


		// Report things that should not have happened
		// if detail print is on, print always. if not, only print when there have been errors
		if (PrintDebugMsg_QCdetail || PrintDebugMsg_other) {
			sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): Spill %d: Mod %d total=%d, short=%d, splitH=%d, splitT=%d, offset=%d,  traceMismatch=%d, badWM=%d, badChanNum=%d, badCheckSum=%d", LMBufferCounter[ModNum], ModNum, goodEventCount, badEventCount, splitHeaderCount, splitTraceCount, offsetWordCount, traceBlocksMismatchCount, badWatermarkCount, badChanNumCount, checkSumMismatchCount);
			Pixie_Print_MSG(ErrMSG,1);
		}
		else {
			if (badEventCount>0 || traceBlocksMismatchCount>0 || badWatermarkCount>0 || badChanNumCount>0 || checkSumMismatchCount>0 || splitHeaderCount>0 ) {
				sprintf(ErrMSG, "*WARNING* (Write_DMA_List_Mode_File): Spill %d, Mod %d: total=%d, short=%d, splitH=%d, splitT=%d, offset=%d,  traceMismatch=%d, badWM=%d, badChanNum=%d, badCheckSum=%d", LMBufferCounter[ModNum], ModNum, goodEventCount, badEventCount, splitHeaderCount, splitTraceCount, offsetWordCount, traceBlocksMismatchCount, badWatermarkCount, badChanNumCount, checkSumMismatchCount);
				Pixie_Print_MSG(ErrMSG,1);
			}
		}
	//	if(!EndRunFound[ModNum]) {			// only if the run is not over anyway 
			// Set the last element to a known pattern, change of which will be used as DMA idle indicator.
			
			LMBuffer[ModNum][DMA_LM_FRAMEBUFFER_LENGTH/(sizeof(U32))-1] = 0xA5A5A5A5;

			VDMADriver_SetDPTR(hDev[ModNum], MAIN_START);			// rewind DMA sequencer
			VDMADriver_Go(hDev[ModNum]);							// resume DMA (that was halted by finishing the SG list)
			sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): Sequencer restarted");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_daq);
	//	}

#ifdef DUMP
			switch (RunType) {
				case 0x400: // Binary file
				case 0x402: // Binary file
				case 0x403:
					eventsWritten = fwrite(pLMBufferCopy, goodEventBytes, 1, listFile[ModNum]);
					break;
				case 0x401: // ASCII file, no trace (like AutoPRocessLMData=3)
					EvStart = 0;
					while (EvStart < goodEventBytes/sizeof(U16)) {
						fprintf(listFile[ModNum], 
							"%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
							dt3EventCounter[ModNum]++, 
							((U16*)pLMBufferCopy)[9+EvStart],
							(unsigned long long)(65536.0 * 65536.0 * (double)((U16*)pLMBufferCopy)[6+EvStart] + 
							65536.0 * (double)((U16*)pLMBufferCopy)[5+EvStart] + 
							(double)((U16*)pLMBufferCopy)[4+EvStart]),
							((U16*)pLMBufferCopy)[8+EvStart],
							((U16*)pLMBufferCopy)[11+EvStart],
							((U16*)pLMBufferCopy)[10+EvStart],
							((U16*)pLMBufferCopy)[12+EvStart],
							((U16*)pLMBufferCopy)[13+EvStart],
							((U16*)pLMBufferCopy)[14+EvStart],
							((U16*)pLMBufferCopy)[15+EvStart]);
						EvStart += (((U16*)pLMBufferCopy)[2]+1)*32;
					}
					break;
				default:
					break;
			} // switch RunType

#endif // if DUMP
		
		LMBufferCounter[ModNum]++;
		sprintf(ErrMSG, "*DEBUG* (Write_DMA_List_Mode_File): Done Write_DMA_List_Mode_File with spill %d",LMBufferCounter[ModNum]);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);


#endif // if WINDRIVER_API

	if(MakeNewFile)	{	// if a top level call asked for new file in multi-file runs, make/switch files now for all modules
		// first write any left overs. (cleared in Create_List_Mode_File) 
		if(numDWordsLeftover[ModNum]>0) {
			eventsWritten = fwrite(LMBuffer[ModNum], (numDWordsLeftover[ModNum])*sizeof(U32), 1, listFile[ModNum]);
		}
		// TODO: insert EOR block at the end of the old files
		// then make new files
		for(j=0; j < Number_Modules; j++) {
			Create_List_Mode_File(j, next_base_name, RunType);
		}
	}

	return(EndRunFound[ModNum]);
}

/****************************************************************
*	Apply_default_I2E function:
*		Enable ADC's I2E, apply gain/offset/phase stored in EEPROM
*
****************************************************************/

S32 Apply_default_I2E (						 
						 U8  ModNum) 				// Pixie module number
{
	U16 data[4];
	U32 value, i, ch;
	U16 I2Edata[48];

	// disable I2E (to reset)
	data[0] = 0x20; 
	data[1] = 0x20; 
	data[2] = 0x20; 
	data[3] = 0x20; 
	ADCSPI(ModNum, 0x31, data, MOD_WRITE);
	

	// read data from EEPROM 
#ifdef WINDRIVER_API		
	for (i=0;i < 12; i++) {	
		PIXIE500E_ReadI2C_Word(hDev[ModNum], &value, i+9);		// read a 32 bit word staring from word 9
		I2Edata[4*i+0] = (U16)((value & 0x000000FF));				// unpack into bytes 
		I2Edata[4*i+1] = (U16)((value & 0x0000FF00) >> 8);
		I2Edata[4*i+2] = (U16)((value & 0x00FF0000) >> 16);
		I2Edata[4*i+3] = (U16)((value & 0xFF000000) >> 24);
	}
#endif

	if(I2Edata[0]==0) {
		sprintf(ErrMSG, "*ERROR* (Apply_default_I2E): suspecting blank EEPROM, I2E data not applied");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	// write to ADC
	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+0];
	ADCSPI(ModNum, 0x20, data, MOD_WRITE);		//write  coarse offset, core 0

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+6];
	ADCSPI(ModNum, 0x26, data, MOD_WRITE);		//write  coarse offset, core 1

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+1];
	ADCSPI(ModNum, 0x21, data, MOD_WRITE);		//write  fine offset, core 0

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+7];
	ADCSPI(ModNum, 0x27, data, MOD_WRITE);		//write  fine offset, core 1

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+2];
	ADCSPI(ModNum, 0x22, data, MOD_WRITE);		//write  coarse gain, core 0

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+8];
	ADCSPI(ModNum, 0x28, data, MOD_WRITE);		//write  coarse gain, core 1

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+3];
	ADCSPI(ModNum, 0x23, data, MOD_WRITE);		//write  medium gain, core 0

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+9];
	ADCSPI(ModNum, 0x29, data, MOD_WRITE);		//write  medium gain, core 1

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+4];
	ADCSPI(ModNum, 0x24, data, MOD_WRITE);		//write  fine gain, core 0

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+10];
	ADCSPI(ModNum, 0x2A, data, MOD_WRITE);		//write  fine gain, core 1

	for (ch=0;ch < 4; ch++) data[ch] = I2Edata[12*ch+5];
	ADCSPI(ModNum, 0x70, data, MOD_WRITE);		//write skew, 


	// freeze I2E (so that it does not recalibrate on disable), then enable (activates downloaded values)
	data[0] = 0x22; 
	data[1] = 0x22; 
	data[2] = 0x22; 
	data[3] = 0x22; 
	ADCSPI(ModNum, 0x31, data, MOD_WRITE);
	data[0] = 0x23; 
	data[1] = 0x23; 
	data[2] = 0x23; 
	data[3] = 0x23; 
	ADCSPI(ModNum, 0x31, data, MOD_WRITE);

	return(0);
}


/****************************************************************
*	ADCSPI function:
*		read/write to ADC SPI registers
*
***************************************************************/
S32 ADCSPI (
	U8 ModNum,				// module number
	U16 addr,				//SPI register address, same for all channels
	U16 *data,				// SPI data (r/w)
	U16 read	)			// 1 for read, 0 for write
{
	U32 buffer[N_DSP_PAR];
	U16 idx_HIO, idx_HIOD, idx_HOD;
	S32 retval;

	// prepare indices and address words
	idx_HIO = Find_Xact_Match("HOSTIO", DSP_Parameter_Names, N_DSP_PAR);
	idx_HIOD = Find_Xact_Match("HOSTIODATA", DSP_Parameter_Names, N_DSP_PAR);	// input only
	idx_HOD = Find_Xact_Match("HOSTODATA", DSP_Parameter_Names, N_DSP_PAR);		// output only

	if(read)
		addr = SetBit(15, addr);
	else
		addr = ClrBit(15, addr);


	// get data from DSP
	Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_READ, N_DSP_PAR, buffer);

	//specify addr and data for DSP
	buffer[idx_HIO+0]  = addr;
	buffer[idx_HIO+1]  = addr;
	buffer[idx_HIO+2]  = addr;
	buffer[idx_HIO+3]  = addr;
	buffer[idx_HIOD+0] = data[0];
	buffer[idx_HIOD+1] = data[1];
	buffer[idx_HIOD+2] = data[2];
	buffer[idx_HIOD+3] = data[3];

	// write data to DSP
	Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_WRITE, DSP_IO_BORDER, buffer);

	// start control run to read/write to ADC
	retval = Control_Task_Run(ModNum, 34, 10000);

	// get read data from DSP
	Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS, MOD_READ, N_DSP_PAR, buffer);
	data[0] = (U16)buffer[idx_HOD+0];
	data[1] = (U16)buffer[idx_HOD+1];
	data[2] = (U16)buffer[idx_HOD+2];
	data[3] = (U16)buffer[idx_HOD+3];

	return(retval);
}

/****************************************************************
*	Create_List_Mode_File function:
*		create a file for run tasks 0x40#, populate header
*
***************************************************************/
S32 Create_List_Mode_File (
	U32 CurrentModNum,		// module number
	S8	*base_name,			// filename without .xxx 
	U16 runtask	)			// 
{

	S32 retval = 0;
	S8  current_name[256];
	U8  str[256];
	U8  ChanNum;
	U16 *Run_Header = NULL;
	U16 idx;
	U16 TL, CW, CP, CSRC;
	
	if (listFile[CurrentModNum])			// if open, 
		fclose(listFile[CurrentModNum]);	// close currently open file
	
	// update/initialize global variables
	LMBufferCounter[CurrentModNum] = 0; // reset spill counter for new run
	dt3EventCounter[CurrentModNum] = 0;	// reset spill counter for new run
	MakeNewFile = 0;					// clear any pending request to make new files
	// MakeNewFile is set to 0 by start run, set to 1 by resume run and then cleared again when
	// Write_DMA_List_Mode_File calls Create_List_Mode_File to make new files. 
	// So at start of Create_List_Mode_File, MakeNewFile is 0 at the first execution, 1 thereafter

	// open files
	if(runtask==0x401)	{	// 0x401 is special in several ways ... 
		sprintf(current_name,"%s_m%d.dt3",base_name,CurrentModNum); //file name: .dt3
		listFile[CurrentModNum] = fopen(current_name, "w"); // create .dt3 file
	}
	else {
		sprintf(current_name,"%s.b%02d",base_name,CurrentModNum);// add module number as last 2 characters
		listFile[CurrentModNum] = fopen(current_name, "wb"); // create empty file
	}	
	if (!listFile[CurrentModNum]) {
		sprintf(ErrMSG, "*ERROR* (Create_List_Mode_File): cannot open list mode file");
		Pixie_Print_MSG(ErrMSG,1);
		retval = -1;
	} 

	// define the file header
	if(!(Run_Header = calloc(RUN_HEAD_LENGTH,sizeof(U16)))) {
		fclose(listFile[CurrentModNum]);
		sprintf(ErrMSG, "*ERROR* (Create_List_Mode_File): Insufficient memory");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	};
	Run_Header[0] = BLOCKSIZE;
	Run_Header[1] = (U16)CurrentModNum;
//	Run_Header[2] = runtask;	
	Run_Header[3] = MAX_CHAN_HEAD_LENGTH; // Channel header length. To be changed to a constant in defs.h
	idx=Find_Xact_Match("COINCPATTERN", DSP_Parameter_Names, N_DSP_PAR);				
	CP = Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];		// from local copy
	Run_Header[4] = CP;	
	idx=Find_Xact_Match("COINCWAIT", DSP_Parameter_Names, N_DSP_PAR);				
	CW = Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];		// from local copy
	Run_Header[5] = CW;	
	Run_Header[7] = (U16)Pixie_Devices[CurrentModNum].Module_Parameter_Values[BoardVersion_Index];	// board version, e.g. 0xA550 For P4e, 16/125 Rev A
	idx = Find_Xact_Match("SERIAL_NUMBER", Module_Parameter_Names, N_MODULE_PAR);
	Run_Header[12] = (U16)Pixie_Devices[CurrentModNum].Module_Parameter_Values[idx];	// serial number

	//sprintf(ErrMSG, "*DEBUG* (Create_List_Mode_File): runtask %x", runtask);
	//Pixie_Print_MSG(ErrMSG,1);	
	
	// Determining the sum of four channel lengths. Used to compute array size in parsing routines 
	for(ChanNum = 0; ChanNum < NUMBER_OF_CHANNELS; ChanNum++) {
		sprintf(str,"TRACELENGTH%d",ChanNum);			
		idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);				
		TL = Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];		// from local copy
		Run_Header[6] +=(U16)(TL + MAX_CHAN_HEAD_LENGTH) / BLOCKSIZE;
		Run_Header[8+ChanNum] =(U16)(TL + MAX_CHAN_HEAD_LENGTH) / BLOCKSIZE;			// each channel's event length, in blocks
	}
	if(runtask==0x402)
		Run_Header[6] -=  (3*MAX_CHAN_HEAD_LENGTH) / BLOCKSIZE;		// only one header for 4 channels in Runtype 0x402


	// Determining the Trace4x options for each channel. Sets a flag in the runformat field
	Run_Header[2] = runtask;
	for(ChanNum = 0; ChanNum < NUMBER_OF_CHANNELS; ChanNum++) {
		sprintf(str,"CHANCSRC%d",ChanNum);			
		idx=Find_Xact_Match(str, DSP_Parameter_Names, N_DSP_PAR);				
		CSRC = Pixie_Devices[CurrentModNum].DSP_Parameter_Values[idx];		// from local copy
		if(TstBit(CSR_TRACE4X,CSRC)==1)
			Run_Header[2] = SetBit(4+ChanNum,Run_Header[2]);
//sprintf(ErrMSG, "*DEBUG* (Create_List_Mode_File): CSRC %x, RunHeader[2] %x", CSRC, Run_Header[2]);
//Pixie_Print_MSG(ErrMSG,1);
	}



	// write the file header
	if(runtask==0x401)	{	// 0x401 is special in several ways ... 
		fprintf(listFile[CurrentModNum], "\nModule:\t%hu\n",         CurrentModNum);
		fprintf(listFile[CurrentModNum], "Run Type:\t%hu\n",         0x401);
		fprintf(listFile[CurrentModNum], "Run Start Time :\t <see .ifm file> \n\n");
		fprintf(listFile[CurrentModNum], "Event\tChannel\tTimeStamp\tEnergy\tRT\tApeak\tBsum\tQ0\tQ1\tPSAval\n");
		EventLengthTotal[CurrentModNum] = 4;	// save event lengths for later into globals
		EventLength[CurrentModNum][0]   = 1;	// 0x401 never writes traces
		EventLength[CurrentModNum][1]   = 1;
		EventLength[CurrentModNum][2]   = 1;
		EventLength[CurrentModNum][3]   = 1;
	}
	else {
		fwrite(Run_Header, sizeof(U16), RUN_HEAD_LENGTH, listFile[CurrentModNum]); // write to file	
		EventLengthTotal[CurrentModNum] = Run_Header[6];	// save event lengths for later into globals
		EventLength[CurrentModNum][0]   = Run_Header[8];
		EventLength[CurrentModNum][1]   = Run_Header[9];
		EventLength[CurrentModNum][2]   = Run_Header[10];
		EventLength[CurrentModNum][3]   = Run_Header[11];
	}

	traceBlocksPrev_QC[CurrentModNum] = 0;		// initialize QC trace block length
	numDWordsLeftover[CurrentModNum] = 0;		// initialize carry over from LM buffer to next
	free(Run_Header);

	return(retval);
}

/****************************************************************
*	FindNewDMAData function:
*		parse through the DMA buffer (copy) and return the position of the last new RS block
*
***************************************************************/
S32 FindNewDMAData (
		)			
{

	U32 pos, lastRSpos, recordfound;
	
	// copy DMA buffer to a local (unchanging) block
	//memcpy(LMBufferCopy, &LMBuffer[0][0], DMA_LM_FRAMEBUFFER_LENGTH);

	lastRSpos   = DMADataPos;
	recordfound = 1;
	pos			= DMADataPos;		// should be first word of new header after last RS block

	while (recordfound==1 && pos < DMA_LM_FRAMEBUFFER_LENGTH)
	{
		if(LMBuffer[0][pos+chanHeadWatermarkIdx] == WATERMARK)			// check if a WM is in the right location, if so extract event info
		{
			if(LMBuffer[0][pos+chanHeadEventStatusIdx] == RSRMARK)	// if it's a RS block, save this position
			{
				lastRSpos = pos+5*BLOCKSIZE/2;									// should read up to RS record and include the 5 blocks of RS header+data. 
				sprintf(ErrMSG, "*DEBUG* (FindNewDMAData): lastRSpos = %d, evtpattern %x.",pos*2,LMBuffer[0][pos+chanHeadEventStatusIdx]);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
			}

			pos = pos+BLOCKSIZE/2+BLOCKSIZE/2*(0xFFFF & LMBuffer[0][pos+chanHeadNumBlocksIdx]);	// advance by event length
		}
		else
		{
			recordfound = 0;											// if there is no WM in the right place, there is no new data
		}		// end valid record found

		

	}

	return(lastRSpos);
}

/****************************************************************
*	ComputePSA function:
*	    calculate extended PSA values from waveforms (not DSP or FPGA)
*	    TODO: use separate arrays for PSA control and output.
*
***************************************************************/
S32 ComputePSA(U16 *trace, U32 traceLen, U32 *PSAval)
{
    // PSAval: sum length and location in words 1, 2, 3, 4
    // Other control options: words 5--10.
    // Output into word 0, 11--16.
    S32 retval;
    double base;
    double ampl;
    U32 V_maxloc; // location of trace maximum
    U16 V_max; // Value of trace maximum
    U32 lev10, lev90;  // locations of 10% and 90% crossings
    U32 i;
    double RTlow; //= 0.1;
    double RThigh; // = 0.9;
    double RT;
    double Q0sum, Q1sum;
    U32 Q0start, SoQ0, LoQ0;
    U32 Q1start, SoQ1, LoQ1;
    U32 normQ0;
    U32 normQ1;
    U32 PSAoption, PSAdiv8, PSAletrig, PSAth;
    double LEthreshold;
	U32 BLlen = 8;


    LoQ0 = PSAval[1]; //12;
    LoQ1 = PSAval[2]; //64;
    SoQ0 = PSAval[3]; // 0;
    SoQ1 = PSAval[4]; //32;
    RTlow = PSAval[5]/100.0;
    RThigh = PSAval[6]/100.0;
    PSAoption = PSAval[7];
    PSAdiv8 = PSAval[8];
    PSAletrig = PSAval[9];
    PSAth = PSAval[10];

    LEthreshold = PSAth*1.27*4; // threshold for leading edge (not CFD) trigger, matching DSP/FPGA scaling

    retval = -1;

    if (PSAdiv8) {
        normQ0 = 32;
        normQ1 = 32;
    }
    else {
        normQ0 = 4;
        normQ1 = 4;
    }

    

    // Baseline
    base = 0.0;
    for (i = 4; i < 4+BLlen; i++) {
        base += trace[i];
    }
    base /= BLlen;

    // Amplitude
    ampl = 0.0;
    V_max = 0;
    V_maxloc = 0;
    for (i = 0; i < traceLen; i++) {
        if (trace[i] > V_max) {
            V_max = trace[i];
            V_maxloc = i;
        }
    } // for trace
    ampl = (double)V_max - base;
    
    // TODO: make sure that only consider rise time levels close to V_maxloc
    lev10 = 0;
    for (i = (V_maxloc-LoQ0 > 0) ? V_maxloc-LoQ0 : 0; i < traceLen; i++) {
        if (trace[i] > (base + ampl*RTlow) ) {
            lev10 = i;
            break;
        }
    } // for trace

    lev90 = 0;
    for (i = (V_maxloc-LoQ0 > 0) ? V_maxloc-LoQ0 : 0; i < traceLen; i++) {
        if (trace[i] > (base + ampl*RThigh) ) {
            lev90 = i;
            break;
        }
    } // for trace

    RT  = (lev90-lev10)*16;	// in 1/16 clock cycles
	// TODO: for proper calculation, would need interpolation between 2 closest samples
	// but currently DSP/FPGa also don't compute RT, so this coarse value is ok for now

    // TODO leadging edge trigger
    // Q0start, Q1start could be from LEthreshold
    if (PSAletrig) {
		Q0start = Q1start = lev10;	// set a default in case the LE trigger doesn't find any
        for (i = (V_maxloc-LoQ0 > 0) ? V_maxloc-LoQ0 : 0; i < traceLen; i++) {
            if (trace[i] > (base + LEthreshold) ) {
                Q0start = Q1start = i;
                break;
            }
        } // for trace
    }
    else Q0start = Q1start = lev10;
    
    
    // Compute Q1 sum
    Q1sum = 0.0;
    for (i = Q1start + SoQ1; i <Q1start + SoQ1 + LoQ1; i++) {
        Q1sum += (trace[i] - base);
    } // for Q1 sum
    Q1sum /= normQ1;
    
    // Compute Q0 sum
    Q0sum = 0.0;
    for (i = Q0start + SoQ0; i <Q0start + SoQ0 + LoQ0; i++) {
        Q0sum += (trace[i] - base);
    } // for Q0 sum
    Q0sum /= normQ0;

    // Output
    PSAval[11] = (int)RT;
    PSAval[12] = (int)ampl;
    PSAval[13] = (int)base;
    PSAval[14] = (int)Q0sum;
    PSAval[15] = (int)Q1sum;
    if (Q0sum > 0) 
		PSAval[16] = (PSAoption>0) ? (int)(1000.*((Q1sum-Q0sum)/Q0sum)) : (int)(1000.*(Q1sum/Q0sum));
    else 
		PSAval[16] = 0;
    retval = 0;

    if (lev10 >= lev90 || lev10 >=V_maxloc || lev90 > V_maxloc)  {
        sprintf (ErrMSG, "*WARNING* (ComputePSA): problems with finding rising edge: lev10 x=%d, peak x=%d, lev90 x=%d", lev10, V_maxloc, lev90);
        Pixie_Print_MSG(ErrMSG, PrintDebugMsg_QCdetail);
        retval = -1;
    }

	if (Q0sum<=0 || Q1sum<=0 )  {
        sprintf (ErrMSG, "*WARNING* (ComputePSA): problems with PSA sums: lev10 Q0=%2.2f, Q1=%2.2f", Q0sum, Q1sum);
        Pixie_Print_MSG(ErrMSG, PrintDebugMsg_QCdetail);
        retval = -1;
    }

    return(retval);
}


