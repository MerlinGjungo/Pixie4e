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
*      reader.c
*
* Description:
*
*      This file contains format reader functions for Pixie.
*
* Revision:
*
*		01-21-2015
*
* Member functions:
*					P4/500e
* 					P500E_Format_Map ()			- "lookup table" of header values from file with meaningful names
*					CheckSums ()				- compute checksum on channel header
*					ErrorChecking ()			- checks file header and following/previous trace length for errors
* 					Pixie_List_Mode_Parser()	- executes runtasks 0x7001-7007, 7009, 7020-7021
* 					Pixie_Event_Browser()		- executes runtasks 0x7008
*
*					P4/500
*					PixieListModeReader							- (Pixie-4) Parse list mode data file using analysis logic 
*					Pixie_Parse_List_Mode_Events				-  task 7001
*					PixieParseListModeEventsBufferLevel				- buffer-level user function for the list mode reader
*					PixieParseListModeEventsChannelLevel			-  channel-level user function for the list mode reader 
*					PixieParseListModeEventsPostAnalysisLevel
*					Pixie_Locate_List_Mode_Traces				- task 7002
*					PixieLocateListModeTracesChannelLevel
*					Pixie_Read_List_Mode_Traces					- task 7003
*					Pixie_Read_Energies							- task 7004
*					PixieReadEnergiesChannelLevel
*					PixieReadEnergiesAuxChannelLevel
*					Pixie_Read_Event_PSA						- task 7005
*					PixieReadEventPSAChannelLevel
*					PixieReadEventPSAAuxChannelLevel
*					Pixie_Read_Long_Event_PSA					- task 7006
*					PixieReadLongEventPSAChannelLevel
*					PixieReadLongEventPSAAuxChannelLevel
*					Pixie_Locate_List_Mode_Events				- task 7007
*					PixieLocateListModeEventsBufferLevel
*					PixieLocateListModeEventsEventLevel
*					PixieLocateListModeEventsChannelLevel
*					Pixie_User_List_Mode_Reader					- task 7010
*					PixieUserListModeReaderPreProcess
*					PixieUserListModeReaderBufferLevel
*					PixieUserListModeReaderEventLevel
*					PixieUserListModeReaderChannelLevel
*					PixieUserListModeReaderAuxChannelLevel
*					PixieUserListModeReaderPostProcess
*					Pixie_Read_List_Mode_Events					- task 7008	
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

#include "reader.h"

/*
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
*/
/****************************************************************
* Pixie-500 Express List Mode Format Mapping
*	P500E_Format_Map function:
*		Creates mapping between run header and event header 
*       values and meaningful names used for convenience of programming
*
*		Return Values: none
*
****************************************************************/

void P500E_Format_Map_400 (LMR_t LMP5, P500E_t P500E) 
{

	P500E->BlockSize			= &LMP5->RunHeader[0]; 
	P500E->ModNum  				= &LMP5->RunHeader[1];    
	P500E->RunType				= &LMP5->RunHeader[2];
	P500E->ChanHeadLen			= &LMP5->RunHeader[3];
	P500E->SumChanLen			= &LMP5->RunHeader[6];
	P500E->BoardVersion			= &LMP5->RunHeader[7];
	P500E->ChanLen0				= &LMP5->RunHeader[8];
	P500E->ChanLen1				= &LMP5->RunHeader[9];
	P500E->ChanLen2				= &LMP5->RunHeader[10];
	P500E->ChanLen3				= &LMP5->RunHeader[11];
	P500E->ADCrate				= &LMP5->ADCrate;
	P500E->EvtPattern			= &LMP5->ChannelHeader[0];
	P500E->EvtInfo				= &LMP5->ChannelHeader[1];
	P500E->NumTraceBlks			= &LMP5->ChannelHeader[2];
	P500E->NumTraceBlksPrev		= &LMP5->ChannelHeader[3];
	P500E->TrigTimeLO			= &LMP5->ChannelHeader[4];
	P500E->TrigTimeMI			= &LMP5->ChannelHeader[5];
	P500E->TrigTimeHI			= &LMP5->ChannelHeader[6];
	P500E->TrigTimeX			= &LMP5->ChannelHeader[7];
	P500E->Energy				= &LMP5->ChannelHeader[8];
	P500E->ChanNo				= &LMP5->ChannelHeader[9];
	P500E->UserPSA				= &LMP5->ChannelHeader[10];
	P500E->XIAPSA				= &LMP5->ChannelHeader[11];
	P500E->ExtendedPSA0			= &LMP5->ChannelHeader[12];
	P500E->ExtendedPSA1			= &LMP5->ChannelHeader[13];
	P500E->ExtendedPSA2			= &LMP5->ChannelHeader[14];
	P500E->ExtendedPSA3			= &LMP5->ChannelHeader[15];
	P500E->CheckSum0			= &LMP5->ChannelHeader[28];
	P500E->CheckSum1			= &LMP5->ChannelHeader[29];
	P500E->WaterMark0			= &LMP5->ChannelHeader[WATERMARKINDEX16];
	P500E->WaterMark1			= &LMP5->ChannelHeader[WATERMARKINDEX16+1];
}

void P500E_Format_Map_402 (LMR_t LMP5, P500E_t P500E) 
{

	P500E->BlockSize			= &LMP5->RunHeader[0]; 
	P500E->ModNum  				= &LMP5->RunHeader[1];    
	P500E->RunType				= &LMP5->RunHeader[2];
	P500E->ChanHeadLen			= &LMP5->RunHeader[3];
	P500E->SumChanLen			= &LMP5->RunHeader[6];
	P500E->BoardVersion			= &LMP5->RunHeader[7];
	P500E->ChanLen0				= &LMP5->RunHeader[8];
	P500E->ChanLen1				= &LMP5->RunHeader[9];
	P500E->ChanLen2				= &LMP5->RunHeader[10];
	P500E->ChanLen3				= &LMP5->RunHeader[11];
	P500E->ADCrate				= &LMP5->ADCrate;
	P500E->EvtPattern			= &LMP5->ChannelHeader[0];
	P500E->EvtInfo				= &LMP5->ChannelHeader[1];
	P500E->NumTraceBlks			= &LMP5->ChannelHeader[2];
	P500E->NumTraceBlksPrev		= &LMP5->ChannelHeader[3];
	P500E->TrigTimeLO			= &LMP5->ChannelHeader[26];	// from ch.3 flatch for event as a whole
	P500E->TrigTimeMI			= &LMP5->ChannelHeader[27];	// from ch.3 flatch for event as a whole
	P500E->TrigTimeHI			= &LMP5->ChannelHeader[4];
	P500E->TrigTimeX			= &LMP5->ChannelHeader[5];
	P500E->Esum					= &LMP5->ChannelHeader[6];
	P500E->TrigTimeLO_0			= &LMP5->ChannelHeader[8];
	P500E->TrigTimeMI_0			= &LMP5->ChannelHeader[9];
	P500E->Energy_0				= &LMP5->ChannelHeader[10];
	P500E->NumTraceBlks_0		= &LMP5->ChannelHeader[11];
	P500E->TrigTimeLO_1			= &LMP5->ChannelHeader[12];
	P500E->TrigTimeMI_1			= &LMP5->ChannelHeader[13];
	P500E->Energy_1				= &LMP5->ChannelHeader[14];
	P500E->NumTraceBlks_1		= &LMP5->ChannelHeader[15];	
	P500E->TrigTimeLO_2			= &LMP5->ChannelHeader[16];
	P500E->TrigTimeMI_2			= &LMP5->ChannelHeader[17];
	P500E->Energy_2				= &LMP5->ChannelHeader[18];
	P500E->NumTraceBlks_2		= &LMP5->ChannelHeader[19];
	P500E->TrigTimeLO_3			= &LMP5->ChannelHeader[20];
	P500E->TrigTimeMI_3			= &LMP5->ChannelHeader[21];
	P500E->Energy_3				= &LMP5->ChannelHeader[22];
	P500E->NumTraceBlks_3		= &LMP5->ChannelHeader[23];
	P500E->EvtInfo_01			= &LMP5->ChannelHeader[24];
	P500E->EvtInfo_23			= &LMP5->ChannelHeader[25];
	P500E->CheckSum0			= &LMP5->ChannelHeader[28];
	P500E->CheckSum1			= &LMP5->ChannelHeader[29];
	P500E->WaterMark0			= &LMP5->ChannelHeader[WATERMARKINDEX16];
	P500E->WaterMark1			= &LMP5->ChannelHeader[WATERMARKINDEX16+1];
}


/****************************************************************
* Pixie-500e checksum computation.
*	CheckSum function:
*		Computes checksum of the channel header
*       Reads DSP computed checksum from the channel header.
*
*		Return Values: none
*
****************************************************************/

void CheckSums (U32 *Computed, U32 *Recorded, U16 *ChannelHeader) {
	/* Compute checksum */
	*Computed = ChannelHeader[0] + ChannelHeader[1]*65536;
	*Computed = *Computed^(ChannelHeader[2] +  ChannelHeader[3]*65536);
	*Computed = *Computed^(ChannelHeader[4] +  ChannelHeader[5]*65536);
	*Computed = *Computed^(ChannelHeader[6] +  ChannelHeader[7]*65536);
	*Computed = *Computed^(ChannelHeader[8] +  ChannelHeader[9]*65536);
	*Computed = *Computed^(ChannelHeader[10] + ChannelHeader[11]*65536);
	*Computed = *Computed^(ChannelHeader[12] + ChannelHeader[13]*65536);
	*Computed = *Computed^(ChannelHeader[14] + ChannelHeader[15]*65536);
	/* End of checksum computation */

	/* Read recorded checksum */
	*Recorded = ChannelHeader[29] * 65536 + ChannelHeader[28];
	if (*Recorded == 0) *Computed = 0;		// older files may have no checksum. So if zero, suppress check sum check 
	/* End reading recorded checksum */
}



/****************************************************************
* Pixie-500 channel header error checking.
*	ErrorChecking function:
*		- check for watermark, if not found look in specified "direction"
*		- check for watermark, if necessary correct channel number
*		- check for prev. trace length, correct if necessary
*		- check for current trace length, correct if necessary
*		- final file pointer at beginning of currwent trace 
*
*		Return Values: 0 if ok
*					   1 if unrecoverable error
*
****************************************************************/

U32 ErrorChecking (U16 *ChannelHeader, U16 direction, U16 RunType,  FILE *ListModeFile, P500E_t P500E) {

	S64  i = 0;
	S64  CurrentFilePos = 0;
	U32  Computed = 0; /* Check sum */
	U32  Recorded = 0; /* Check sum */
	U32  WaterMark = 0;
	U32  Skipped16=0;
	U16  Words[2] = {0};
	S64	 skipbytes;
	U16  hit;
	U16 CHL = *P500E->ChanHeadLen;
	int errorf =0;
 
	CurrentFilePos = Pixie_ftell(ListModeFile);	// positioned just after current header
	if(direction==1)						// 1 means search to right, towards next event
		skipbytes = CHL*(-2)+4;				// in searching WM, move pointer in file a channel header back and then 2 16 bit words forward
	else
		skipbytes = CHL*(-2)-4;				// in searching WM, move pointer in file a channel header back and then 2 16 bit words backward

	WaterMark = (U32)ChannelHeader[WATERMARKINDEX16] + (U32)ChannelHeader[WATERMARKINDEX16+1]*65536;
	while (WaterMark != WATERMARK) { 
		sprintf(ErrMSG, "*ERROR* (ErrorChecking): Bad watermark in current event: 0x%X",WaterMark);
		Pixie_Print_MSG(ErrMSG,(PrintDebugMsg_QCerror && Skipped16==0) );	// if bad watermark and we did not skip in the previous cycle, it's a new error: print

		// scan for next WM
		Pixie_fseek (ListModeFile , skipbytes, SEEK_CUR);	// skip from previous read and try again. 
		Skipped16++;										// count how many words skipped
		Skipped16++;										// count how many words skipped
		fread(ChannelHeader, sizeof(U16), CHL, ListModeFile);
		WaterMark = (U32)ChannelHeader[WATERMARKINDEX16] + (U32)ChannelHeader[WATERMARKINDEX16+1]*65536;

		// give up if too many skips (4* the max waveform length). Note: not in while condition so we can print this message
		if(Skipped16 > MAXFIFOBLOCKS * BLOCKSIZE * 4) {		
			sprintf(ErrMSG, "*ERROR* (ErrorChecking): list file badly damaged");
			Pixie_Print_MSG(ErrMSG,1);
			return (1);				    
		}
	}
	// once we found a watermark: print how many words skipped (if any) and set skip counter to zero
	if (Skipped16 > 0) {
		sprintf(ErrMSG, "*DEBUG* (ErrorChecking): Skipped %d words before finding event",Skipped16);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
		Skipped16=0;
		CurrentFilePos = Pixie_ftell(ListModeFile); // positioned just after current header
	}
	
	/* Check checksum */
	CheckSums (&Computed, &Recorded, ChannelHeader);
	if (Computed != Recorded) {
		sprintf(ErrMSG, "*ERROR* (ErrorChecking): Checksum mismatch.");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
		ChannelHeader[1] |= 0x8000; /* Mark as bad event.*/
	} // end checking checksum


	/* checking channel number ( ChannelHeader[9] )*/
	// no matter if checksum is good or bad, 
	if((RunType & 0xFF0F) != 0x402) {		// no channel number in 0x402 record
		if (ChannelHeader[9] > (NUMBER_OF_CHANNELS - 1)) { 
			sprintf(ErrMSG, "*ERROR* (ErrorChecking): wrong channel number: %hu",ChannelHeader[9]);
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
			ChannelHeader[1] |= 0x8000; /* Mark as bad event.*/

			// try to recover from hit pattern
			hit = (ChannelHeader[0] & 0x000F);
			switch(hit)
			{  
				case 0x1:
					ChannelHeader[9] = 0;
					break;
				case 0x2:
					ChannelHeader[9] = 1;
					break;
				case 0x4:
					ChannelHeader[9] = 2;
					break;
				case 0x8:
					ChannelHeader[9] = 3;
					break;
				default: 
					ChannelHeader[9] = 0;		// default to zero if both header and hit are bad 
					break;
			}
		} /* end checking channel number */
	}

	/* trace length check */
	// no matter if checksum is good or bad, 
	// - check previous trace length by looking for prev. watermark
	// - check  following trace length by looking for next watermark 
	//    except if we have an end-of-run record

	/* check previous trace length. no known value from prev. event, so have to look for it */
	// note: corrected value may be shorter than actual length due to block size coarseness. Must use "direction" parameter to find correctly
	skipbytes = (CHL*2+ChannelHeader[3]*BLOCKSIZE)*(-2)+WATERMARKINDEX16*2;	// one CHL back to beginning of current event, CHL+trace back to prev. event, WMindex forward to WM
	if( ((CurrentFilePos+skipbytes)/2-WATERMARKINDEX16) < RUN_HEAD_LENGTH ) {
		errorf = 1;														// indicate "errorf" which means prev. event outside file
	}
	else {
		Pixie_fseek (ListModeFile ,skipbytes , SEEK_CUR);						//  go back to (nominal) WM of prev event (previously check this is in range)
		fread (Words, sizeof(U16), 2, ListModeFile);
		WaterMark = (U32)Words[0] + (U32)Words[1]*65536; 
		if (WaterMark != WATERMARK) { 
			sprintf(ErrMSG, "*ERROR* (ErrorChecking): wrong previous trace size ");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);				// if bad watermark, the prev. trace size is bad
			ChannelHeader[1] |= 0x8000;									// Mark as bad event

			errorf = Pixie_fseek (ListModeFile , CurrentFilePos-CHL*2-4, SEEK_SET); // set to known position of beginning of current header, then 2 16bit words back.  errorf is zero if ok
			
			while ( (WaterMark != WATERMARK) && (!errorf) ) { 
				fread (Words, sizeof(U16), 2, ListModeFile);			// read and move 2 words forward
				WaterMark = (U32)Words[0] + (U32)Words[1]*65536;
				errorf = Pixie_fseek (ListModeFile , -8, SEEK_CUR);			//  go back 4 16bit words
				Skipped16++;
				Skipped16++;

				// give up if too many skips (4* the max waveform length). Note: not in while condition so we can print this message
				if(Skipped16 > MAXFIFOBLOCKS * BLOCKSIZE * 4) {		
					sprintf(ErrMSG, "*ERROR* (ErrorChecking): list file badly damaged");
					Pixie_Print_MSG(ErrMSG,1);
					return (1);				    
				}
			} // end while loop. file pointer just after WM position of previous event

			i = Pixie_ftell(ListModeFile);									// get current position at just after WM position of previous event
			i = i+2*(CHL-WATERMARKINDEX16);								// adjust to beginning of prev. trace
			i = (S64)fabs(CurrentFilePos - i);							// difference to beginning of current trace
			ChannelHeader[3] = (U16)floor(2 * i / BLOCKSIZE) - 1;		// update prev. trace length in blocks
			sprintf(ErrMSG, "*DEBUG* (ErrorChecking) Trace length: %d i: %d", ChannelHeader[3], i);
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
		}
	}		
	if(errorf) {	//fseek outside file or other error
		ChannelHeader[3] = 0;		// set prev. TL to zero
		sprintf(ErrMSG, "*DEBUG* (ErrorChecking): previous event would be outside file, assuming current is the first with previous trace size = 0");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
	}
	/* end check previous trace length */


	/* check current trace length; have to look for it. If this is the EOR record, it must be zero */
	// note: corrected value may be shorter than actual length due to block size coarseness. Must use "direction" parameter to find correctly
	Pixie_fseek (ListModeFile , CurrentFilePos, SEEK_SET);	// go back to known position at beginnin of current trace
	skipbytes = (ChannelHeader[2]*BLOCKSIZE)*(2)+WATERMARKINDEX16*2;
	errorf = Pixie_fseek (ListModeFile ,skipbytes , SEEK_CUR); //  go forward trace (nominal), then forward to WM. 
	if( ((U32)ChannelHeader[0] + (U32)ChannelHeader[1]*65536) == EORMARK)	//If there are trailing zeros in the file, the above errorf checking does not catch the last event, so check for EOR
		errorf=1;												// indicate "errorf" which means next event outside file
	if(!errorf) {
		fread (Words, sizeof(U16), 2, ListModeFile);
		WaterMark = (U32)Words[0] + (U32)Words[1]*65536; 
		if (WaterMark != WATERMARK) { 
			sprintf(ErrMSG, "*ERROR* (ErrorChecking): wrong current trace size ");
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);		// if bad watermark, the prev. trace size is bad
			ChannelHeader[1] |= 0x8000;							// Mark as bad event.

			Pixie_fseek (ListModeFile , CurrentFilePos, SEEK_SET);	// set to known position of beginning of current trace
			
			while ( (WaterMark != WATERMARK) && !(errorf) ) { 
				errorf = fread (Words, sizeof(U16), 2, ListModeFile);		// read and move 2 words forward
				errorf = 2-errorf;								// zero if successfully read the 2 words
				WaterMark = (U32)Words[0] + (U32)Words[1]*65536;
				Skipped16++;
				Skipped16++;

				// give up if too many skips (4* the max waveform length). Note: not in while condition so we can print this message
				if(Skipped16 > MAXFIFOBLOCKS * BLOCKSIZE * 4) {		
					sprintf(ErrMSG, "*ERROR* (ErrorChecking): list file badly damaged");
					Pixie_Print_MSG(ErrMSG,1);
					return (1);				    
				}
			} // end while loop. file pointer just after WM position of previous event

			i = Pixie_ftell(ListModeFile);							// get current position at just after WM position of next event
			i = i-2*(WATERMARKINDEX16);							// adjust to beginning of next header
			i = (S64)fabs(CurrentFilePos - i);					// difference to beginning of current trace
			ChannelHeader[3] = (U16)floor(2 * i / BLOCKSIZE);	// update current trace length in blocks
			sprintf(ErrMSG, "*DEBUG* (ErrorChecking) Trace length: %d i: %d", ChannelHeader[3], i);
			Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);

		}
	}
	if(errorf) {	//fseek outside file or other error. 
		ChannelHeader[2] = 0;		// set current TL to zero
		sprintf(ErrMSG, "*DEBUG* (ErrorChecking): next event would be outside file, assuming this is the last event (EOR) with trace size = 0");
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
	}
	/* end check current trace length */

	Pixie_fseek (ListModeFile , CurrentFilePos, SEEK_SET); // set file position to beginning of trace

	return (0);
	/* End error checking */
}

/****************************************************************
*	Pixie_List_Mode_Parser function (P4e/500e):
*		Parse the list mode files to get various information.
*       Task 0x7001 Mode 0: report the total number of events.
*       Task 0x7001 Mode 1: report the total number of events
*                           and create an ASCII file with some 
*                           information for every event like 
*                           Energy and time stamp.
*       Task 0x7001 Mode 2: report the total number of events
*                           and create an ASCII file with more
*                           detailed information for every 
*                           event like Energy and time stamp.
*       Task 0x7001 Mode 3: report the total number of events
*                           and create an ASCII file with all
*                           the information available for every 
*                           event.             
*       Task 0x7002: report trace position, trace length and
*                    energy for every event.
*       Task 0x7004: report energy for every event.
*       Task 0x7005: report XIA PSA and User PSA for every event.
*       Task 0x7006: report 32-bit time stamp, 
*                           Energy,
*                           XIA PSA, 
*                           User PSA, 
*                           Extended PSA #0, 
*                           Extended PSA #1, 
*                           Extended PSA #2, 
*                           Extended PSA #3.
*       Task 0x7007: report event position in the binary file
*                    and the event length.
*		Task 0x7011: for 0x400 and 0x402 types, write to ASCII file,
*		    first line, header data: event, channel, Trigger times (LO, MI, Hi), trace length
*		    second line, trace: waveform samples
*		Task 0x7020: write data corrected by QC process
*		Task 0x7021: write data in Pixie-4 style .bin file 
*		Taks 0x7030 Mode 3: for 0x400 write to ASCII file, similar to 0x7001,
*		                Event, channel, time stamp, Energy,
*		                computed from trace Rise Time,
*		                computed from trace Amplitude,
*		                computed from trace pre-peak base line,
*		                computed from trace Q0 sum,
*		                computed from trace Q1 sum,
*		                computed from trace PSA value
*		                UserData should be of length 17:
*		                word 0: number of processed events,
*		                word 1: input Q0 length,
*		                word 2: input Q1 length,
*		                word 3: input Q0 delay,
*		                word 4: input Q1 delay,
*		                word 5: input rise-time start, percent
*		                word 6: input rise-time stop, percent
*		                word 7: input PSA compuptation 0: Q1/Q0, 1: (Q1-Q0)/Q0
*		                word 8: input 1: divide Q-sums by 8
*		                word 9: input 1: use leading edge trigger
*		                word 10: input leading edge trigger threshold in ADC steps
*		                word 11: output Rise Time
*		                word 12: output Amplitude
*		                word 13: output Baseline
*		                word 14: output Q0 sum
*		                word 15: output Q1 sum
*		                word 16: output 1000*(Q1-Q0/Q0 or 1000*Q1/Q0
*
*		Return Value:
*			 0 - success
*			-1 - can't open list mode data file
*			-2 - memory allocation error
*			-3 - no valid watermark found or other invalid data in file
*			-4 - invalid data pointer for return data
*			-5 - invalid run type in file
*
* KS NB: TracePos is still 32-bit, so, expect the reader to fail (not show correct data) for big (>2GB) files!

****************************************************************/


S32 Pixie_List_Mode_Parser(S8 *filename, U32 *UserData, U16 TaskNum )
{
	U8   mode[3] = {"w"};
	U16  i = 0;
	U16  Words[2] = {0};
	U16  hit;
	U16  MyNumTraceBlksPrev=0;
	U16  P4hsize16 = BUFFER_HEAD_LENGTH + EVENT_HEAD_LENGTH + P4_MAX_CHAN_HEAD_LENGTH;
	U16  RunType;
	U16  ChannelNo = 0;
	U32  TraceNum = 0;
	U16  EventLengthRH;
	U16  ReadMoreFileData = 1;
	S32	 ReturnValue = 0;
	U32  CheckSumComputed = 0;
	U32  CheckSumRecorded = 0;
	U32  WaterMark = 0;
	U32  *ShiftFromStart = NULL;
	U32  EHR;
	U16  *P4headers = NULL;
	U32  TotalShift      =  0;
	S64  EventPos = RUN_HEAD_LENGTH;
	S64  GoodHeaderPos = 0;
	S64  offset16 = 0;
	S64  Skipped32 = 0;
	double	RunStartTime = 0;
	BOOL nextWMfound = FALSE;
	/* Pointers to data structures for the list mode reader */
	LMR_t		LMP5 = NULL;
	P500E_t		P500E = NULL;
	size_t		bytesRead = 0;

	sprintf(ErrMSG, "*INFO* (Pixie_List_Mode_Parser): Start processing LM file");
	Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);

	 /* Make sure UserData is not NULL */
	if(!UserData) {
		sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): NULL pointer *UserData");
		Pixie_Print_MSG(ErrMSG,1);
		return(-4);
	}
	/* Reserve memory for the data structures for the list mode reader */
	if(!(LMP5 = calloc(1, sizeof(*LMP5)))) {
			sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): not enough memory for LMP5");
			Pixie_Print_MSG(ErrMSG,1);
			return(-2);
	}
	if(!(P500E = calloc(1, sizeof(*P500E)))) {
			sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): not enough memory for P500E");
			Pixie_Print_MSG(ErrMSG,1);
			free(LMP5);
			return(-2);
	}
	 /* Create ShiftFromStart array needed for 0x7001 and 0x7007 */
	if(!(ShiftFromStart = calloc(PRESET_MAX_MODULES, sizeof(U32)))) {
			sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): not enough memory for ShiftFromStart");
			Pixie_Print_MSG(ErrMSG,1);
			free(LMP5);
			free(P500E); 
			return(-2);
	}
	 /* Create P4headers array needed for 0x7011  */
	if(!(P4headers = calloc(P4hsize16, sizeof(U16)))) {
			sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): not enough memory for P4headers");
			Pixie_Print_MSG(ErrMSG,1);
			free(LMP5);
			free(P500E); 
			free(ShiftFromStart);
			return(-2);
	}

	/* Open the list mode file if exists */
	LMP5->ListModeFileName = filename;
	if(LMP5->ListModeFile) fclose(LMP5->ListModeFile);
	if(!(LMP5->ListModeFile = fopen(LMP5->ListModeFileName, "rb"))) { 
		sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open list mode data file %s", LMP5->ListModeFileName);
		Pixie_Print_MSG(ErrMSG,1);
		free(LMP5);
		free(P500E); 
		free(ShiftFromStart);
		free(P4headers);
		return(-1);
	}

	/* Read the first buffer/run/channel header to glean initial information about the type of the list mode file */
	fread (LMP5->FirstHeader, sizeof(U16), FIRST_HEAD_LENGTH, LMP5->ListModeFile);
	RunType = LMP5->FirstHeader[2];

		// Check for valid RunType
	if (RunType < 0x400 || RunType > 0x4F3) { /* If run type is bogus then finish processing */
		sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong run type 0x%x", RunType);
		Pixie_Print_MSG(ErrMSG,1);
		fclose(LMP5->ListModeFile);
		free(LMP5);
		free(P500E); 
		free(ShiftFromStart);
		free(P4headers);
		return (-5);
	} 
	else {
		sprintf(ErrMSG, "*INFO* (Pixie_List_Mode_Parser): RunType = 0x%x", RunType);
		Pixie_Print_MSG(ErrMSG,PrintDebugMsg_other);
	}

	/* Copy first header to the corresponding run header and first channel header */
	memcpy (LMP5->RunHeader,        &LMP5->FirstHeader[0],		RUN_HEAD_LENGTH * sizeof(U16) );
	rewind(LMP5->ListModeFile);

	/* Pixie-500 Express List Mode Format Mapping */
	if((RunType & 0xFF0F) == 0x402)
		P500E_Format_Map_402 (LMP5, P500E);
	else
		P500E_Format_Map_400 (LMP5, P500E);
	
	/* Read run header */
	fread (LMP5->RunHeader, sizeof(U16), RUN_HEAD_LENGTH, LMP5->ListModeFile);
	/* Remember the end position of the last header */
	GoodHeaderPos = (S64)Pixie_ftell (LMP5->ListModeFile);		// This eliminated unneccesary parsing of the first 32 words?

	/* Read the list mode file and do the processing */
	/* Loop over channel headers */
	while ( ReadMoreFileData) {
		
			bytesRead = fread (LMP5->ChannelHeader, sizeof(U16), *P500E->ChanHeadLen, LMP5->ListModeFile);
			if(bytesRead != *P500E->ChanHeadLen) {
				sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): Less than a channel header remaining in file, exiting file");
				Pixie_Print_MSG(ErrMSG,1);
				ReadMoreFileData=0;
				continue;
			}
			
			  // if the event pattern is all zero, exit the loop over events
			  // (except in the first event, where we might have leading zeros which are handled by error checking)
			  if( (*P500E->EvtPattern==0) && (LMP5->TotalEvents>=0) )	 {
				sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): Found all zero event pattern, exiting file");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
				ReadMoreFileData=0;
				continue;
			  }

				// fill run type dependent event parameters
				if((RunType & 0xFF0F) == 0x402) {	
					ChannelNo = 0;// default to zero
					EventLengthRH = *P500E->SumChanLen - 3; // EventLengthTotal = 4x (header + TL), in blocks; so subtract 3 for actual length in runtype 0x402
				}
				else {
					// for other run types, this default may be overwritten in channel error check
					ChannelNo = *P500E->ChanNo;
					EventLengthRH = LMP5->RunHeader[8+ ChannelNo];
				}

			/* ERROR CHECKING */
			/* Begin check watermark */
			WaterMark = *P500E->WaterMark0 + *P500E->WaterMark1 * 65536;

			if (WaterMark != WATERMARK) { 
				sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): Bad watermark: 0x%X, event %d",WaterMark, LMP5->TotalEvents);
				Pixie_Print_MSG(ErrMSG,(PrintDebugMsg_QCerror && Skipped32==0) );	// if bad watermark and we did not skip in the previous cycle, it's a new error: print

				Pixie_fseek (LMP5->ListModeFile , (S64)(*P500E->ChanHeadLen)*(-2)+4, SEEK_CUR); // move pointer in file 2 16 bit words ahead from previous read and try again. 
				Skipped32++; // count how many words skipped
				
				if(Skipped32 > (MAXFIFOBLOCKS * BLOCKSIZE)/2 * 4) {		// give up if too many skips (4* the max waveform length
						sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): list file badly damaged. Event: %d", LMP5->TotalEvents);
						Pixie_Print_MSG(ErrMSG,1);
						fclose(LMP5->ListModeFile);
						free(LMP5);
						free(P500E); 
						free(ShiftFromStart);
						free(P4headers);
						return (-3);				    
					}
				continue;		//next cycle
			}
			
			// once we found a watermark: print how many words skipped (if any) and set skip counter to zero
			if (Skipped32 > 0) {
				sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): Skipped %d words before finding event %d",Skipped32, LMP5->TotalEvents);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
				Skipped32=0;
			}
			
			// Remember the end position of the last header 
			GoodHeaderPos = (S64)Pixie_ftell (LMP5->ListModeFile);		// WH_TODO: check. This eliminated unneccesary parsing of the first 32 words?

			/* End check watermark */
		
			/* Begin check checksums */
			// The idea is that all values make sense if the checksum is correct.
			// For example, a channel number can only be wrong if the checksum is wrong (DSP would not write a bad value on purpose)
			// The exception is the trace length, which may be correct but different from actual recorded langth, e.g. if some words are just missing
			CheckSums (&CheckSumComputed, &CheckSumRecorded, LMP5->ChannelHeader);
			if (CheckSumComputed != CheckSumRecorded) { 
				// If checksums do not match 
				sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): Checksums do not match. Computed: 0x%X, Recorded: 0x%X Event: %d",CheckSumComputed, CheckSumRecorded, LMP5->TotalEvents);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				if (LMP5->TotalEvents == 0) {
					sprintf(ErrMSG, "TaskNum: 0x%X", TaskNum);
					Pixie_Print_MSG(ErrMSG,1);
				}
				*P500E->EvtInfo |= 0x8000; // Mark as bad event. 
				LMP5->BadEvent++;
			} /* end check checksums */
			else {	// progress report for specific event
				// sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): checksum ok");
				// Pixie_Print_MSG(ErrMSG,(LMP5->TotalEvents==0));
			}

			/* checking channel number, even if checksums are ok */
			if((RunType & 0xFF0F) != 0x402) {	
				if (ChannelNo > (NUMBER_OF_CHANNELS - 1)) { 
					sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong channel number: %hu, event %d",*P500E->ChanNo, LMP5->TotalEvents);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					*P500E->EvtInfo |= 0x8000; // Mark as bad event. 

					// try to recover from hit pattern
					hit = (*P500E->EvtPattern & 0x000F);
					switch(hit)
					{  
						case 0x1:
							ChannelNo = 0;
							break;
						case 0x2:
							ChannelNo = 1;
							break;
						case 0x4:
							ChannelNo = 2;
							break;
						case 0x8:
							ChannelNo = 3;
							break;
						default: 
							ChannelNo = 0;		// default to zero if both header and hit are bad 
							break;
					}
				}
				EventLengthRH = LMP5->RunHeader[8+ ChannelNo];
			} /* end checking channel number */

			/* trace length check */
			// no matter if checksum is good or bad, 
			// - check previous trace length by comparing to what was actually written. this is actually not important, not used below
			// - check  following trace length by looking for next watermark 
			//    except if we have an end-of-run record

			/* check previous trace length against known value from processing */
			if (*P500E->NumTraceBlksPrev !=MyNumTraceBlksPrev) {
				sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong previous trace size in blocks: %hu, event %d",*P500E->NumTraceBlksPrev, LMP5->TotalEvents);
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				*P500E->EvtInfo |= 0x8000;								// Mark as bad event. 
				*P500E->NumTraceBlksPrev  = MyNumTraceBlksPrev;			// try to recover from previous processing
			} /* end check previous trace length */
		
			/* check following trace length by looking for next watermark */
			WaterMark = *P500E->EvtPattern + *P500E->EvtInfo * 65536;	// borrow this variable temporarily for end of run mark check
			if (WaterMark==EORMARK) {
				// no need to check for tracelength
				sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): reached end of run");
				Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCdetail);
				*P500E->NumTraceBlks = 0;
				ReadMoreFileData=0;
			}	
			else {
				nextWMfound = FALSE;
				// 1. try place of next watermark per channel header
				// move file pointer 
				offset16 = *P500E->NumTraceBlks * *P500E->BlockSize + WATERMARKINDEX16;	// offset from current position (end of channel header) to place of next watermark (in 16bit words)
				Pixie_fseek (LMP5->ListModeFile , GoodHeaderPos+offset16*2, SEEK_SET); // move pointer in file 2 16 bit words ahead from previous read and try again. 
				// read 
				bytesRead = fread (Words, sizeof(U16), 2, LMP5->ListModeFile);
				if ( bytesRead == 0 ) {
					sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): unexpected end of file");
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					ReadMoreFileData=0;
				} else {
					// compare
					WaterMark = (U32)Words[0] + (U32)Words[1]*65536;
					//sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): watermark try 1, %X at offset %d",WaterMark, offset16);
					//Pixie_Print_MSG(ErrMSG,(LMP5->TotalEvents==0));
					if (WaterMark == WATERMARK)
						nextWMfound = TRUE;
				}

				// 2. try zero (some special code may suppress traces)
				// move file pointer 
				if (!nextWMfound) {
					offset16 = WATERMARKINDEX16;	// offset from current position (end of channel header) to place of next watermark (in 16bit words)
					Pixie_fseek (LMP5->ListModeFile , GoodHeaderPos+offset16*2, SEEK_SET); // move pointer in file 2 16 bit words ahead from previous read and try again. 			
					// read 
					bytesRead = fread (Words, sizeof(U16), 2, LMP5->ListModeFile);
					if ( bytesRead == 0 ) {
						sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): unexpected end of file");
						Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
						ReadMoreFileData=0;
					}	else {
						// compare
						WaterMark = (U32)Words[0] + (U32)Words[1]*65536;
						if (WaterMark == WATERMARK)
							nextWMfound = TRUE;
					}
				}

				// 3. try using file header value (assumes ChannelNo is correct. Make sure it's at least in range)
				// move file pointer 
				if (!nextWMfound) {
					if( (ChannelNo < NUMBER_OF_CHANNELS) && (EventLengthRH>0) && (EventLengthRH<MAXFIFOBLOCKS+1) ) {
						offset16 = (EventLengthRH -1) * *P500E->BlockSize + WATERMARKINDEX16;	 // offset from current position (end of channel header) to place of next watermark (in 16bit words)
						//RunHeader 8-11 have event size in blocks (header+trace)
						Pixie_fseek (LMP5->ListModeFile , GoodHeaderPos+offset16*2, SEEK_SET); // move pointer in file 2 16 bit words ahead from previous read and try again. 				
						// read 
						bytesRead = fread (Words, sizeof(U16), 2, LMP5->ListModeFile);
						if ( bytesRead == 0 ) {
							sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): unexpected end of file");
							Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
							ReadMoreFileData=0;
						} 	else {
							// compare
							WaterMark = (U32)Words[0] + (U32)Words[1]*65536;
							if (WaterMark == WATERMARK)
								nextWMfound = TRUE;
						}
					}
				} // end try 3


				// no point finding true value now. Just use file header value for processing below. 
				// But later advance file pointer only by channel header (not trace), so next cycle starts looking for watermark
				// update header info
				if(!nextWMfound) {
					*P500E->EvtInfo |= 0x8000; // Mark as bad event. 
					LMP5->BadEvent++;
					sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong following trace size in blocks: %hu, event %d",*P500E->NumTraceBlks, LMP5->TotalEvents);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
				}
				*P500E->NumTraceBlks = (U16)(offset16  - (S64)WATERMARKINDEX16) / *P500E->BlockSize;	// update trace blocks to follow
				*P500E->NumTraceBlks = MAX(*P500E->NumTraceBlks, 0);	// ensure it's in legal limits if some really crazy values have been read
				*P500E->NumTraceBlks = MIN(*P500E->NumTraceBlks, *P500E->SumChanLen);
				MyNumTraceBlksPrev = *P500E->NumTraceBlks;									// update "known" value of last event for next event

				// move back file pointer
				Pixie_fseek (LMP5->ListModeFile , GoodHeaderPos, SEEK_SET);
			}
			/* end check following trace length */

	/*		// Some more checks. Can't harm, but maybe not necessary
			if (!(*P500E->EvtInfo & 0x8000)) { // if not a bad event 
				if (*P500E->NumTraceBlks > *P500E->SumChanLen) { // If trace length is bogus then finish processing 
					sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong trace size in blocks: %hu, event %d,   %d",*P500E->NumTraceBlks, LMP5->TotalEvents, LMP5->ChannelHeader[2]);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					fclose(LMP5->ListModeFile);
					free(LMP5);
					free(P500E); 
					free(ShiftFromStart);
					free(P4headers);
					return (-3);
				}
				if (*P500E->NumTraceBlksPrev > *P500E->SumChanLen) { // If previous trace length is bogus then finish processing 
					sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong previous trace size in blocks: %hu, event %d",	*P500E->NumTraceBlksPrev, LMP5->TotalEvents);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					fclose(LMP5->ListModeFile);
					free(LMP5);
					free(P500E); 
					free(ShiftFromStart);
					free(P4headers);
					return (-3);
				}
				if (ChannelNo > (NUMBER_OF_CHANNELS - 1)) { // If channel number is bogus then finish processing 
					sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): wrong channel number: %hu, event %d",ChannelNo, LMP5->TotalEvents);
					Pixie_Print_MSG(ErrMSG,PrintDebugMsg_QCerror);
					fclose(LMP5->ListModeFile);
					free(LMP5);
					free(P500E); 
					free(ShiftFromStart);
					free(P4headers);
					return (-3);
				}   
				// END OF OTHER ERROR CHECKING 
			}		*/

			/* Read trace if it is run 0x400, 0x402, or 0x403*/
			if ((RunType & 0xFF0F) == 0x400 || (RunType & 0xFF0F) == 0x402 || (RunType & 0xFF0F) == 0x403) {
				if(*P500E->NumTraceBlks>0)
					fread (LMP5->Trace, sizeof(U16), (U32)*P500E->BlockSize * (U32)*P500E->NumTraceBlks, LMP5->ListModeFile);
				// if the watermark was not found (bad TL), move back file pointer to end of header so next cycles starts a search

				if(!nextWMfound) Pixie_fseek (LMP5->ListModeFile , GoodHeaderPos, SEEK_SET);
			}

			/* Analysis logic here */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7001 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7001) {
				if (AutoProcessLMData > 0) {

					// file creation and header
					if (!LMP5->TotalEvents) {
						if (!LMP5->OutputFile) {
							/* Determine the name of the output file */
							strcpy(LMP5->OutputFileName, filename);
							*strstr(LMP5->OutputFileName, ".") = '\0';
							/* Check if the user requests outputing parsed data to a file */
							if(AutoProcessLMData == 1)			// short, traditional output data file, incomplete timestamp
								sprintf(LMP5->OutputFileName, "%s_m%hu.dat", LMP5->OutputFileName, *P500E->ModNum); 							
							if(AutoProcessLMData == 2)		// Long output data file with full timestamp and hit pattern 
								sprintf(LMP5->OutputFileName, "%s_m%hu.dt2", LMP5->OutputFileName,  *P500E->ModNum); 							
							if(AutoProcessLMData == 3){		// Long output data file with Energy, Time and various PSA values
								if(UserData[0] == 0) {
									sprintf(LMP5->OutputFileName, "%s_m%hu.dt3", LMP5->OutputFileName, *P500E->ModNum);
								}
								else {
									sprintf(LMP5->OutputFileName, "%s.dt3", LMP5->OutputFileName);		// special case, we are in a (host) loop over modules, append to file without module suffix
									sprintf(mode,"a");
									sprintf(ErrMSG, "*DEBUG* (Pixie_Parse_List_Mode_Events): output file %s", LMP5->OutputFileName);
									Pixie_Print_MSG(ErrMSG,1);
								}
							}

							if(!(LMP5->OutputFile = fopen(LMP5->OutputFileName, mode))) {
								fclose(LMP5->ListModeFile);
								sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open output file", LMP5->OutputFileName);
								Pixie_Print_MSG(ErrMSG,1);
								free(LMP5);
								free(P500E); 
								free(ShiftFromStart);
								free(P4headers);
								return(-1);
							}
						}

						/* First header contains 64 records of the run header and the first event header */
						RunStartTime =  ((double)*P500E->TrigTimeHI * 65536.0 * 65536.0 + 
							(double)*P500E->TrigTimeMI * 65536.0 + 
							(double) *P500E->TrigTimeLO) / 
							(double)P500E_ADC_CLOCK_MHZ * 1.0e-6;
						fprintf(LMP5->OutputFile, "\nModule:\t%hu\n",         *P500E->ModNum);
						fprintf(LMP5->OutputFile, "Run Type:\t%hu\n",         *P500E->RunType);
						fprintf(LMP5->OutputFile, "Run Start Time (s) :\t%f\n\n", RunStartTime);
						if(AutoProcessLMData == 1) {
							if ((*P500E->RunType & 0xFF0F) < 0x402) fprintf(LMP5->OutputFile, "Event No\tChannel No\tEnergy\tTrig Time\tXIA_PSA\tUser_PSA\n");
							if ((*P500E->RunType & 0xFF0F) > 0x401) fprintf(LMP5->OutputFile, "Event No\tChannel No\tEnergy\tTrig Time\n");
						}
						if(AutoProcessLMData == 2) 
							fprintf(LMP5->OutputFile, "Event No\tChannel No\tHit Pattern\tEvent_Time_A\tEvent_Time_B\tEvent_Time_C\tEnergy\tTrig Time\tXIA_PSA\tUser_PSA\n");
						if(AutoProcessLMData == 3) 
							fprintf(LMP5->OutputFile, "Event\tChannel\tTimeStamp\tEnergy\tRT\tApeak\tBsum\tQ0\tQ1\tPSAval\n");
					}
					
					// event processing
					if(AutoProcessLMData == 1) 
						if ((RunType & 0xFF0F) == 0x402) {		// 4-channel records
							fprintf(LMP5->OutputFile, "%-9d%-12d%-9d%-15d%-9d%-6d\n", 
								LMP5->Events[*P500E->ModNum], 
								0,						// channel
								*P500E->Energy_0, 
								*P500E->TrigTimeLO_0, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%-9d%-12d%-9d%-15d%-9d%-6d\n", 
								LMP5->Events[*P500E->ModNum], 
								1,						// channel
								*P500E->Energy_1, 
								*P500E->TrigTimeLO_1, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%-9d%-12d%-9d%-15d%-9d%-6d\n", 
								LMP5->Events[*P500E->ModNum], 
								2,						// channel
								*P500E->Energy_2, 
								*P500E->TrigTimeLO_2, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%-9d%-12d%-9d%-15d%-9d%-6d\n", 
								LMP5->Events[*P500E->ModNum], 
								3,						// channel
								*P500E->Energy_3, 
								*P500E->TrigTimeLO_3, 
								0, 
								0);
						}
						else {		// other run types have single channel records
							fprintf(LMP5->OutputFile, "%-9d%-12d%-9d%-15d%-9d%-6d\n", 
								LMP5->Events[*P500E->ModNum], 
								ChannelNo, 
								*P500E->Energy, 
								*P500E->TrigTimeLO, 
								*P500E->XIAPSA, 
								*P500E->UserPSA);
						}
					if(AutoProcessLMData == 2) 
						if ((RunType & 0xFF0F) == 0x402) {		// 4-channel records
							fprintf(LMP5->OutputFile, "%d\t %d\t 0x%X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n", 
								LMP5->Events[*P500E->ModNum], 
								0, 
								(U32)(65536.0 * (double)*P500E->EvtInfo + (double)*P500E->EvtPattern), 
								*P500E->TrigTimeHI, 
								*P500E->TrigTimeMI_0, 
								*P500E->TrigTimeLO_0, 
								*P500E->Energy_0, 
								*P500E->TrigTimeLO_0, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%d\t %d\t 0x%X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n", 
								LMP5->Events[*P500E->ModNum], 
								1, 
								(U32)(65536.0 * (double)*P500E->EvtInfo + (double)*P500E->EvtPattern), 
								*P500E->TrigTimeHI, 
								*P500E->TrigTimeMI_1, 
								*P500E->TrigTimeLO_1, 
								*P500E->Energy_1, 
								*P500E->TrigTimeLO_1, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%d\t %d\t 0x%X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n", 
								LMP5->Events[*P500E->ModNum], 
								2, 
								(U32)(65536.0 * (double)*P500E->EvtInfo + (double)*P500E->EvtPattern), 
								*P500E->TrigTimeHI, 
								*P500E->TrigTimeMI_2, 
								*P500E->TrigTimeLO_2, 
								*P500E->Energy_2, 
								*P500E->TrigTimeLO_2, 
								0, 
								0);
							fprintf(LMP5->OutputFile, "%d\t %d\t 0x%X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n", 
								LMP5->Events[*P500E->ModNum], 
								3, 
								(U32)(65536.0 * (double)*P500E->EvtInfo + (double)*P500E->EvtPattern), 
								*P500E->TrigTimeHI, 
								*P500E->TrigTimeMI_3, 
								*P500E->TrigTimeLO_3, 
								*P500E->Energy_3, 
								*P500E->TrigTimeLO_3, 
								0, 
								0);
						}
						else {		// other run types have single channel records
							fprintf(LMP5->OutputFile, "%d\t %d\t 0x%X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n", 
								LMP5->Events[*P500E->ModNum], 
								ChannelNo, 
								(U32)(65536.0 * (double)*P500E->EvtInfo + (double)*P500E->EvtPattern), 
								*P500E->TrigTimeHI, 
								*P500E->TrigTimeMI, 
								*P500E->TrigTimeLO, 
								*P500E->Energy, 
								*P500E->TrigTimeLO, 
								*P500E->XIAPSA, 
								*P500E->UserPSA);
						}
					if(AutoProcessLMData == 3) 
						if ((RunType & 0xFF0F) == 0x402) {		// 4-channel records
							fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
								LMP5->Events[*P500E->ModNum], 
								0,
								(unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
															   65536.0 * (double)*P500E->TrigTimeMI_0 + 
																		 (double)*P500E->TrigTimeLO_0),
								*P500E->Energy_0,
								0,0,0,0,0,0);
							fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
								LMP5->Events[*P500E->ModNum], 
								1,
								(unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
															   65536.0 * (double)*P500E->TrigTimeMI_1 + 
																		 (double)*P500E->TrigTimeLO_1),
								*P500E->Energy_1,
								0,0,0,0,0,0);
							fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
								LMP5->Events[*P500E->ModNum], 
								2,
								(unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
															   65536.0 * (double)*P500E->TrigTimeMI_2 + 
																		 (double)*P500E->TrigTimeLO_2),
								*P500E->Energy_2,
								0,0,0,0,0,0);
							fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
								LMP5->Events[*P500E->ModNum], 
								3,
								(unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
															   65536.0 * (double)*P500E->TrigTimeMI_3 + 
																		 (double)*P500E->TrigTimeLO_3),
								*P500E->Energy_3,
								0,0,0,0,0,0);
						}
						else {
							fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n", 
								LMP5->Events[*P500E->ModNum], 
								ChannelNo,
								(unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
															   65536.0 * (double)*P500E->TrigTimeMI + 
																		 (double)*P500E->TrigTimeLO),
								*P500E->Energy,
								*P500E->XIAPSA,
								*P500E->UserPSA,
								*P500E->ExtendedPSA0,
								*P500E->ExtendedPSA1,
								*P500E->ExtendedPSA2,
								*P500E->ExtendedPSA3);
						}
				}
				//sprintf(ErrMSG, "*INFO* (Pixie_List_Mode_Parser): Fill up ModuleEvents");
				//Pixie_Print_MSG(ErrMSG,(LMP5->TotalEvents==0));

				/* Fill up ModuleEvents */
			//	MODULE_EVENTS[*P500E->ModNum] = LMP5->TotalEvents+1;	// BAD! this remembers total events from previous files if module number is not present in this file
				UserData[*P500E->ModNum] = LMP5->TotalEvents + 1;
			//	MODULE_EVENTS[*P500E->ModNum+PRESET_MAX_MODULES] = LMP5->TotalEvents+1;
				// KS DEBUG
				// FIXME: Unless in 0x7001 UserData is set to be array of PRESET_MAX_MODULES + 1 (plus one!),
				// we get "HEAP CORRUPTION ERROR" -- buffer overflow on UserData.
				UserData[*P500E->ModNum+PRESET_MAX_MODULES] = LMP5->TotalEvents + 1;
			} /* End of 0x7001 */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7002 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7002) {
				if ((RunType & 0xFF0F) != 0x402) {	// not supported in runtask 0x402
					U32  TraceLen       = (U32)*P500E->NumTraceBlks * (U32)*P500E->BlockSize;
					U32  TracePos       = (U32)(Pixie_ftell(LMP5->ListModeFile) + 1) / 2 - TraceLen;
				//	U32		i; 

				TraceNum = LMP5->Traces[*P500E->ModNum];
				UserData[3*TraceNum+0] = TracePos;
				UserData[3*TraceNum+1] = TraceLen; 
				UserData[3*TraceNum+2] =  *P500E->Energy;


				/* P500e has only one file per module. So no need to remember events from "lower" modules,
				   they are all zero and any shift is zero
					if (!LMP5->TotalEvents) {// Prepare the array of module-dependent shifts 
						for(i = 1; i < PRESET_MAX_MODULES; i++)
							ShiftFromStart[i] = (TotalShift += MODULE_EVENTS[i+PRESET_MAX_MODULES-1]);
					}
					UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Traces[*P500E->ModNum])+0] = TracePos;
					UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Traces[*P500E->ModNum])+1] = TraceLen; 
					UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Traces[*P500E->ModNum])+2] = *P500E->Energy;  */
				}
				

			} /* End of 0x7002 */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7004 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7004) {
				TraceNum = LMP5->Traces[*P500E->ModNum];
				if ((RunType & 0xFF0F) == 0x402) {		// 4-channel records			
					UserData[4*TraceNum+0] = *P500E->Energy_0;
					UserData[4*TraceNum+1] = *P500E->Energy_1;
					UserData[4*TraceNum+2] = *P500E->Energy_2;
					UserData[4*TraceNum+3] = *P500E->Energy_3;
				}
				else {
					UserData[4*TraceNum+ChannelNo] = *P500E->Energy;
				}
			} /* End of 0x7004 */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7005 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7005) {
				// not supported in runtask 0x402
				if ( (*P500E->RunType & 0xFF0F) == 0x400 || 
					 (*P500E->RunType & 0xFF0F) == 0x401 || 
					 (*P500E->RunType & 0xFF0F) == 0x403  ) {
					UserData[8*LMP5->Traces[*P500E->ModNum]+2 * ChannelNo+0] = *P500E->XIAPSA;
					UserData[8*LMP5->Traces[*P500E->ModNum]+2 * ChannelNo+1] = *P500E->UserPSA;
				}
			} /* End of 0x7005 */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7006 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7006) {
				if ((RunType & 0xFF0F) != 0x402) {	// not supported in runtask 0x402
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+0] = 
						65536 * (U32)*P500E->TrigTimeMI + (U32)*P500E->TrigTimeLO; /* 32-bit time stamp */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+1] = *P500E->Energy; /* Energy */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+2] = *P500E->XIAPSA; /* XIA PSA */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+3] = *P500E->UserPSA; /* User PSA */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+4] = *P500E->ExtendedPSA0; /* User 2 */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+5] = *P500E->ExtendedPSA1; /* User 3 */
					UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+6] = *P500E->ExtendedPSA2; /* User 4 */
				UserData[32*LMP5->Traces[*P500E->ModNum]+8 * ChannelNo+7] = *P500E->ExtendedPSA3; /* User 5 */
				}
			} /* End of 0x7006 */
			/*************************************************************************************************************/
			/**************************************** TASK 0x7007 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7007) {
				U32  EventLen =  (U32)*P500E->SumChanLen * (U32)*P500E->BlockSize; /* Sum of lengths of 4 channels. New definition.  */
			//	U32		i; 

				TraceNum = LMP5->Traces[*P500E->ModNum];
				EventPos = GoodHeaderPos/2 - (S64)*P500E->ChanHeadLen;
				UserData[3*TraceNum+0] = (U32)EventPos;
				UserData[3*TraceNum+1] = (U32)EventPos; 
				UserData[3*TraceNum+2] =  EventLen;

				//debug
				//if(TraceNum<10)	
				//{
				//	sprintf(ErrMSG, "*DEBUG* (Pixie_List_Mode_Parser): TraceNum %d, EventPos %d, EventLen %d",TraceNum,EventPos,EventLen);
				//	Pixie_Print_MSG(ErrMSG,1);
				//}

				/* P500e has only one file per module. So no need to remember events from "lower" modules,
				   they are all zero and any shift is zero
				if (!LMP5->TotalEvents) {// Prepare the array of module-dependent shifts 
					for(i = 1; i < PRESET_MAX_MODULES; i++) 
						ShiftFromStart[i] = (TotalShift += MODULE_EVENTS[i-1]);
				}
				EventPos = GoodHeaderPos/2 - (S64)*P500E->ChanHeadLen;
				UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Events[*P500E->ModNum])+0] = (U32)EventPos;
				UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Events[*P500E->ModNum])+1] = (U32)EventPos; 
				UserData[3*(ShiftFromStart[*P500E->ModNum]+LMP5->Events[*P500E->ModNum])+2] =  EventLen;
				*/


			} /* End of 0x7007 */
			
			/*************************************************************************************************************/
			/**************************************** TASK 0x7020 ********************************************************/
			/*************************************************************************************************************/
			if (TaskNum == 0x7020) {
				if ((RunType & 0xFF0F) != 0x402) {	// not supported in runtask 0x402
					// file creation and header
					if (!LMP5->TotalEvents) {
						if (!LMP5->OutputFile) {
							// Determine the name of the output file 
							strcpy(LMP5->OutputFileName, filename);
							*strstr(LMP5->OutputFileName, ".") = '\0';
							sprintf(LMP5->OutputFileName,"%s_QC.b%02d", LMP5->OutputFileName, *P500E->ModNum); 
							
							if(!(LMP5->OutputFile = fopen(LMP5->OutputFileName, "wb"))) {
								fclose(LMP5->ListModeFile);
								sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open output file", LMP5->OutputFileName);
								Pixie_Print_MSG(ErrMSG,1);
								free(LMP5);
								free(P500E); 
								free(ShiftFromStart);
								free(P4headers);
								return(-3);
							}
						}

						// write run header, RUN_HEAD_LENGTH 16 bit words
						fwrite(LMP5->RunHeader, RUN_HEAD_LENGTH, 2, LMP5->OutputFile);
					}
					
					// event processing: write corrected event
					fwrite(LMP5->ChannelHeader, *P500E->ChanHeadLen, 2, LMP5->OutputFile);							// header
					fwrite(LMP5->Trace, (U32)*P500E->BlockSize * (U32)*P500E->NumTraceBlks, 2, LMP5->OutputFile);		// trace
				}	// runtype
			}	// End of 0x7020 
			 
			/*************************************************************************************************************/
			/**************************************** TASK 0x7021 ********************************************************/
			/*************************************************************************************************************/
			if (TaskNum == 0x7021) {
				if ((RunType & 0xFF0F) != 0x402) {	// not YET supported in runtask 0x402
					// not yet supported in runtask 0x402
					// file creation and NO header
					if (!LMP5->TotalEvents) {
						if (!LMP5->OutputFile) {
							// Determine the name of the output file 
							strcpy(LMP5->OutputFileName, filename);
							*strstr(LMP5->OutputFileName, ".") = '\0';
							sprintf(LMP5->OutputFileName, "%s_m%hu.bin", LMP5->OutputFileName, *P500E->ModNum); 
							
							if(!(LMP5->OutputFile = fopen(LMP5->OutputFileName, "wb"))) {
								fclose(LMP5->ListModeFile);
								sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open output file", LMP5->OutputFileName);
								Pixie_Print_MSG(ErrMSG,1);
								free(LMP5);
								free(P500E); 
								free(ShiftFromStart);
								free(P4headers);
								return(-3);
							}
						}
					}
					
					// event processing: for simplicity, treat each event as a buffer (BH+EH+CH+trace)
					P4headers[0] = P4hsize16 + *P500E->BlockSize * *P500E->NumTraceBlks;
					P4headers[1] = *P500E->ModNum;
					P4headers[2] =  0x7100;			// fake runtype 0x100, for module type 7 = P4e
					P4headers[3] = *P500E->TrigTimeHI;
					P4headers[4] = *P500E->TrigTimeMI;
					P4headers[5] = *P500E->TrigTimeLO;
					P4headers[BUFFER_HEAD_LENGTH+0] = *P500E->EvtPattern;
					P4headers[BUFFER_HEAD_LENGTH+1] = *P500E->TrigTimeMI;
					P4headers[BUFFER_HEAD_LENGTH+2] = *P500E->TrigTimeLO;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+0] = P4_MAX_CHAN_HEAD_LENGTH+ *P500E->BlockSize * *P500E->NumTraceBlks;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+1] = *P500E->TrigTimeLO;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+2] = *P500E->Energy;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+3] = *P500E->XIAPSA;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+4] = *P500E->UserPSA;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+5] = *P500E->ExtendedPSA0;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+6] = *P500E->ExtendedPSA1;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+7] = *P500E->ExtendedPSA2;
					P4headers[BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+8] = *P500E->ExtendedPSA3;		// usually time stamp high, but that is already in the buffer header above

					fwrite(P4headers, P4hsize16*2, 1, LMP5->OutputFile);							// header
					fwrite(LMP5->Trace, (U32)*P500E->BlockSize * (U32)*P500E->NumTraceBlks *2, 1, LMP5->OutputFile);		// trace
				}	// runtype
			}	// End of 0x7021 

			/*************************************************************************************************************/
			/**************************************** TASK 0x7009 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7009) {
				if ((RunType & 0xFF0F) == 0x402) {	// only supported in runtask 0x402. copy the event header without the trace blocks
					EHR = 32;
					TraceNum = LMP5->Traces[*P500E->ModNum];
					// overall event
					UserData[EHR*TraceNum + 0] = (U32)*P500E->EvtPattern;		// Event  hit pattern 
					UserData[EHR*TraceNum + 1] = (U32)*P500E->EvtInfo;		// Event info 
					UserData[EHR*TraceNum + 2] = 0;
					UserData[EHR*TraceNum + 3] = 0;
					UserData[EHR*TraceNum + 4] = (U32)*P500E->TrigTimeHI;		// 24-bit high time stamp 
					UserData[EHR*TraceNum + 5] = (U32)*P500E->TrigTimeX;  
					UserData[EHR*TraceNum + 6] = (U32)*P500E->Esum;			// Energy sum 
					UserData[EHR*TraceNum + 7] = 0;
					UserData[EHR*TraceNum + 8] = (U32)*P500E->TrigTimeLO_0;	// 32-bit  low time stamp 
					UserData[EHR*TraceNum + 9] = (U32)*P500E->TrigTimeMI_0;
					UserData[EHR*TraceNum +10] = (U32)*P500E->Energy_0;		// energy
					UserData[EHR*TraceNum +11] = 0;
					UserData[EHR*TraceNum +12] = (U32)*P500E->TrigTimeLO_1;	// 32-bit  low time stamp 
					UserData[EHR*TraceNum +13] = (U32)*P500E->TrigTimeMI_1;
					UserData[EHR*TraceNum +14] = (U32)*P500E->Energy_1;		// energy
					UserData[EHR*TraceNum +15] = 0;
					UserData[EHR*TraceNum +16] = (U32)*P500E->TrigTimeLO_2;	// 32-bit  low time stamp 
					UserData[EHR*TraceNum +17] = (U32)*P500E->TrigTimeMI_2;
					UserData[EHR*TraceNum +18] = (U32)*P500E->Energy_2;		// energy
					UserData[EHR*TraceNum +19] = 0;
					UserData[EHR*TraceNum +20] = (U32)*P500E->TrigTimeLO_3;	// 32-bit  low time stamp 
					UserData[EHR*TraceNum +21] = (U32)*P500E->TrigTimeMI_3;
					UserData[EHR*TraceNum +22] = (U32)*P500E->Energy_3;		// energy
					UserData[EHR*TraceNum +23] = 0;
					UserData[EHR*TraceNum +24] = (U32)*P500E->EvtInfo_01; // channel specific event info 
					UserData[EHR*TraceNum +35] = (U32)*P500E->EvtInfo_23;
					UserData[EHR*TraceNum +26] = (U32)*P500E->TrigTimeLO; // 32-bit  low time stamp 
					UserData[EHR*TraceNum +27] = (U32)*P500E->TrigTimeMI;
					UserData[EHR*TraceNum +28] = 0;
					UserData[EHR*TraceNum +29] = 0;
					UserData[EHR*TraceNum +30] = 0;
					UserData[EHR*TraceNum +31] = 0;
				}
			} /* End of 0x7009 */

			// KS DEBUG
			// New task: line one is event header info, line two is waveform data
			/************************************************************************************************************/
			/**************************************** TASK 0x7011 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7011) {
				if (!LMP5->TotalEvents) { // only on start of processing
					if (!LMP5->OutputFile) { // create output file
						// Determine the name of the output file 
						strcpy(LMP5->OutputFileName, filename);
						*strstr(LMP5->OutputFileName, ".") = '\0';
						sprintf(LMP5->OutputFileName,"%s_m%d.out", LMP5->OutputFileName, *P500E->ModNum); 
						if(!(LMP5->OutputFile = fopen(LMP5->OutputFileName, "w"))) {
							fclose(LMP5->ListModeFile);
							sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open output file", LMP5->OutputFileName);
							Pixie_Print_MSG(ErrMSG,1);
							free(LMP5);
							free(P500E); 
							free(ShiftFromStart);
							free(P4headers);
							return(-3);
						}
					}
				}
				switch ((RunType & 0xFF0F)) {
					case 0x400:
						// line one
						fprintf(LMP5->OutputFile, "%d %d %d %d %d %d\n",
							LMP5->Events[*P500E->ModNum], 
							ChannelNo,
							*P500E->TrigTimeLO,
							*P500E->TrigTimeMI,
							*P500E->TrigTimeHI,
							(U32)*P500E->NumTraceBlks * (U32)*P500E->BlockSize
							);
						// line two
						for (i = 0; i < (U32)*P500E->NumTraceBlks * (U32)*P500E->BlockSize; i++) 
							fprintf(LMP5->OutputFile, "%d ", LMP5->Trace[i]);
						fprintf(LMP5->OutputFile, "\n");
						break;
					case 0x402:
						// header channel 0
						fprintf(LMP5->OutputFile, "%d %d %d %d %d %d\n",
							LMP5->Events[*P500E->ModNum], 
							0,
							*P500E->TrigTimeLO_0,
							*P500E->TrigTimeMI_0,
							*P500E->TrigTimeHI,
							(U32)*P500E->NumTraceBlks_0 * (U32)*P500E->BlockSize
							);
						// header channel 1
						fprintf(LMP5->OutputFile, "%d %d %d %d %d %d\n",
							LMP5->Events[*P500E->ModNum], 
							1,
							*P500E->TrigTimeLO_1,
							*P500E->TrigTimeMI_1,
							*P500E->TrigTimeHI,
							(U32)*P500E->NumTraceBlks_1 * (U32)*P500E->BlockSize
							);
						// header channel 2
						fprintf(LMP5->OutputFile, "%d %d %d %d %d %d\n",
							LMP5->Events[*P500E->ModNum], 
							2,
							*P500E->TrigTimeLO_2,
							*P500E->TrigTimeMI_2,
							*P500E->TrigTimeHI,
							(U32)*P500E->NumTraceBlks_2 * (U32)*P500E->BlockSize
							);
						// header channel 3
						fprintf(LMP5->OutputFile, "%d %d %d %d %d %d\n",
							LMP5->Events[*P500E->ModNum], 
							3,
							*P500E->TrigTimeLO_3,
							*P500E->TrigTimeMI_3,
							*P500E->TrigTimeHI,
							(U32)*P500E->NumTraceBlks_3 * (U32)*P500E->BlockSize
							);
						// trace channel 0
						for (i = 0; i < (U32)*P500E->NumTraceBlks_0 * (U32)*P500E->BlockSize; i++) 
							fprintf(LMP5->OutputFile, "%d ", LMP5->Trace[i]);
						fprintf(LMP5->OutputFile, "\n");
						// trace channel 1
						for (i = 0; i < (U32)*P500E->NumTraceBlks_1 * (U32)*P500E->BlockSize; i++) 
							fprintf(LMP5->OutputFile, "%d ", LMP5->Trace[i+(U32)*P500E->NumTraceBlks_0 * (U32)*P500E->BlockSize]);
						fprintf(LMP5->OutputFile, "\n");
						// trace channel 2
						for (i = 0; i < (U32)*P500E->NumTraceBlks_2 * (U32)*P500E->BlockSize; i++) 
							fprintf(LMP5->OutputFile, "%d ", LMP5->Trace[i+((U32)*P500E->NumTraceBlks_0+(U32)*P500E->NumTraceBlks_1) * (U32)*P500E->BlockSize]);
						fprintf(LMP5->OutputFile, "\n");
						// trace channel 3
						for (i = 0; i < (U32)*P500E->NumTraceBlks_3 * (U32)*P500E->BlockSize; i++) 
							fprintf(LMP5->OutputFile, "%d ", LMP5->Trace[i+((U32)*P500E->NumTraceBlks_0+(U32)*P500E->NumTraceBlks_1+(U32)*P500E->NumTraceBlks_2) * (U32)*P500E->BlockSize]);
						fprintf(LMP5->OutputFile, "\n");
						break;
					default:
						sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): run type %d not supported for task 0x7011", RunType);
						Pixie_Print_MSG(ErrMSG,1);
						fclose(LMP5->ListModeFile);
						free(LMP5);
						free(P500E); 
						free(ShiftFromStart);
						free(P4headers);
						return(-3);
						break;
				} // RunType

				UserData[*P500E->ModNum] = LMP5->TotalEvents + 1;
			} /* End of 0x7011 */
			
			// Compute PSA values.
			/************************************************************************************************************/
			/**************************************** TASK 0x7030 **************************************************/
			/************************************************************************************************************/
			if (TaskNum == 0x7030) {
                if (AutoProcessLMData == 3) {
                    if (!LMP5->TotalEvents) { // only on start of processing
                        if (!LMP5->OutputFile) { // create output file
                            // Determine the name of the output file 
                            strcpy(LMP5->OutputFileName, filename);
                            *strstr(LMP5->OutputFileName, ".") = '\0';
                            sprintf(LMP5->OutputFileName,"%s_PSA_m%d.dt3", LMP5->OutputFileName, *P500E->ModNum); 
                            if(!(LMP5->OutputFile = fopen(LMP5->OutputFileName, "w"))) {
                                fclose(LMP5->ListModeFile);
                                sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): can't open output file", LMP5->OutputFileName);
                                Pixie_Print_MSG(ErrMSG,1);
                                free(LMP5);
                                free(P500E); 
                                free(ShiftFromStart);
                                free(P4headers);
                                return(-3);
                            }
                        }  
                        /* First header contains 64 records of the run header and the first event header */
                        RunStartTime =  ((double)*P500E->TrigTimeHI * 65536.0 * 65536.0 + 
                            (double)*P500E->TrigTimeMI * 65536.0 + 
                            (double) *P500E->TrigTimeLO) / 
                            (double)P500E_ADC_CLOCK_MHZ * 1.0e-6;
                        fprintf(LMP5->OutputFile, "\nModule:\t%hu\n",         *P500E->ModNum);
                        fprintf(LMP5->OutputFile, "Run Type:\t%hu\n",         *P500E->RunType);
                        fprintf(LMP5->OutputFile, "Run Start Time (s) :\t%f\n\n", RunStartTime);                    
                        fprintf(LMP5->OutputFile, "Event\tChannel\tTimeStamp\tEnergy\tRT\tApeak\tBsum\tQ0\tQ1\tPSAval\n");
                    } // if header
                    switch ((RunType & 0xFF0F)) {
                        case 0x400:
                            if (ComputePSA(LMP5->Trace, (U32)*P500E->NumTraceBlks * (U32)*P500E->BlockSize, UserData) != 0) {
                                // Not quitting processing, just reporting bad PSA calculation.
                                /*
                                fclose(LMP5->ListModeFile);
                                sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): Failed calculating PSA.");
                                Pixie_Print_MSG(ErrMSG,1);
                                free(LMP5);
                                free(P500E); 
                                free(ShiftFromStart);
                                free(P4headers);
                                return(-3);
                                */
                                sprintf(ErrMSG, "*WARNING* (Pixie_List_Mode_Parser): Failed calculating PSA for event %ld.", LMP5->Events[*P500E->ModNum]);
                                Pixie_Print_MSG(ErrMSG, PrintDebugMsg_QCdetail);
								UserData[11] = 0;
								UserData[12] = 0;
								UserData[13] = 0;
								UserData[14] = 0;
								UserData[15] = 0;
								UserData[16] = 0;
                            }
							
                            fprintf(LMP5->OutputFile, "%u   %hu   %llu   %hu   %hu   %hu   %hu   %hu  %hu  %hu\n",
                                    LMP5->Events[*P500E->ModNum], 
                                    ChannelNo,
                                    (unsigned long long)(65536.0 * 65536.0 * (double)*P500E->TrigTimeHI + 
                                                                   65536.0 * (double)*P500E->TrigTimeMI + 
                                                                             (double)*P500E->TrigTimeLO),
                                    *P500E->Energy,
                                    UserData[11], UserData[12], UserData[13], UserData[14], UserData[15], UserData[16]);
                            break;

                        case 0x402:
                            // fall through to default.
                        default:
                            sprintf(ErrMSG, "*ERROR* (Pixie_List_Mode_Parser): run type %d not supported for task 0x7030", RunType);
                            Pixie_Print_MSG(ErrMSG,1);
                            fclose(LMP5->ListModeFile);
                            free(LMP5);
                            free(P500E); 
                            free(ShiftFromStart);
                            free(P4headers);
                            return(-3);
                            break;
                    } // switch RunType

                    UserData[0] = LMP5->TotalEvents + 1;
                    } // AutoProcessLMData = 3
			} /* End of 0x7030 */

			/*************************************************************************************************************/
			/**************************************** End of TASKs ******************************************************/
			/************************************************************************************************************/

	
		/* End of analysis logic */
		LMP5->Traces[*P500E->ModNum]++;		/* Count traces in each module */
		LMP5->TotalTraces++;				/* Count all traces */
		LMP5->Events[*P500E->ModNum]++;		/* Count events in each module. Same as traces for Pixie-500 Express */
		LMP5->TotalEvents++;				/* Count all events.  Same as traces for Pixie-500 Express */
	}	// end of while loop over events


	sprintf(ErrMSG, "*INFO* (Pixie_List_Mode_Parser): Processed %d events, %d are marked as bad.", LMP5->TotalEvents, LMP5->BadEvent);
	Pixie_Print_MSG(ErrMSG,1);
	/* Close files */
	fclose(LMP5->ListModeFile);
	if (LMP5->OutputFile) fclose(LMP5->OutputFile);
	/* Free memory */
	free(LMP5);
	free(P500E); 
	free(ShiftFromStart);
	free(P4headers);
	/* Done */
	return (0);
}


/****************************************************************
*	Pixie_Event_Browser function:
*		Lookup events by position in the binary file and if desired
*       display other events in coincidence on other channels. 
*       Event coincidence is controlled by the pre-defined coincidence 
*       window.
*
*		Return Value:
*			 0 - success
*			-1 - can't open list mode data file
*			-2 - memory allocation error
*			-3 - error checking failed
*			-4 - invalid pointer to UserData
*
*
****************************************************************/


S32 Pixie_Event_Browser(S8 *filename, U32 *UserData)
{
	S32	ReturnValue = 0;
	U32 *ShiftFromStart = NULL;
	U32 TotalShift =  0;
	S64 EventPos = RUN_HEAD_LENGTH;
	U16 EvtChanNum = 0;
	U16 CurrentChanNum;
	U16 AdvanceBeyondCounter = 0;
	U16 *TimeHighWord = NULL;
	U16 *ChanHeader = NULL;
	U16 *Traces[NUMBER_OF_CHANNELS] = {NULL};
	U16 EvtLength = 0;
	U16 RunType;
	U16	BHL = BUFFER_HEAD_LENGTH; /* Historic block header length used for UserData to comply to the old Pixie-4 rules */
	U16 EHL = EVENT_HEAD_LENGTH; /* Historic event header length used for UserData to comply to the old Pixie-4 rules */
	U16 CHL = P4_MAX_CHAN_HEAD_LENGTH; /* Historic channel header length used for UserData to comply to the old Pixie-4 rules */
	U16 pattern, val16;
	U32 pattern32, val32;
	U32 j,k;
	U32 EventFound[NUMBER_OF_CHANNELS] ={0};
	S64 EvtPos = 0;
	U32 PrevEvtSize = 0;
	U32	CheckSumComputed = 0;
	U32	CheckSumRecorded = 0;
	U32 TraceSize  = 0;		// trace size per channel header, corrected by error checking
	U32 TraceSizeR = 0;		// trace size to be read from file (safe value)
	U32 TraceSizeB = 0;		// trace size in blocks (temp)
	U32 CurrentPosition = 0;
	U32 EvtsFromStart = 0;
	S32 status = 0;
	S64 PrevEvtPos = 0;
	double TimeStamp = 0;
	double Time = 0;
	double TimeWindow;
	double LeftTimeBoundary = 0;
	double RightTimeBoundary = 0;
	double PreviousTime = 0;
	double TimeJitter = 0;
	double RunStartTime = 0;
	/* Pointers to data structures for the list mode reader */
	LMR_t		LMP5 = NULL;
	P500E_t		P500E = NULL;

	/* Make sure UserData is not NULL */
	if(!UserData) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): NULL pointer *UserData");
		Pixie_Print_MSG(ErrMSG,1);
		return(-4);
	}
	/* Reserve memory for the data structures for the list mode reader */
	if(!(LMP5 = calloc(1, sizeof(*LMP5)))) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): not enough memory for LMP5");
		Pixie_Print_MSG(ErrMSG,1);
		return(-2);
	}
	if(!(P500E = calloc(1, sizeof(*P500E)))) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): not enough memory for P500E");
		Pixie_Print_MSG(ErrMSG,1);
		free(LMP5);
		return(-2);
	}

	/* Open the list mode file if exists */
	LMP5->ListModeFileName = filename;
	if(!(LMP5->ListModeFile = fopen(LMP5->ListModeFileName, "rb"))) {
		free(LMP5);
		free(P500E);
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): can't open list mode data file %s", filename);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	/* Read the first buffer/run/channel header to glean initial information about the type of the list mode file */
	fread (LMP5->FirstHeader, sizeof(U16), FIRST_HEAD_LENGTH, LMP5->ListModeFile);
	/* Copy first header to the corresponding run header and first channel header */
	memcpy (LMP5->RunHeader, &LMP5->FirstHeader[0], RUN_HEAD_LENGTH * sizeof(U16) );
	memcpy (LMP5->ChannelHeader, &LMP5->FirstHeader[MAX_CHAN_HEAD_LENGTH], MAX_CHAN_HEAD_LENGTH * sizeof(U16));
	rewind(LMP5->ListModeFile);

	RunType = LMP5->FirstHeader[2];
	/* Pixie-500 Express List Mode Format Mapping */
	if((RunType & 0xFF0F)==0x402)
		P500E_Format_Map_402 (LMP5, P500E);
	else
		P500E_Format_Map_400 (LMP5, P500E);

	// get input values from UserData. UserData[1] is unused as input
	EvtPos		= UserData[0]*2; /* In bytes */
	EvtLength	= (U16)UserData[2]; /* In 16-bit words */
	TimeWindow	= (double)UserData[3];
	/* Make sure the trace length entries UserData are initialized to zero */
	UserData[3] = 0;
	UserData[4] = 0;
	UserData[5] = 0;
	UserData[6] = 0;
	/* invites crash if channel info is incorrect
	FirstHeader[6] is the sum of channel lengths in 32 16-bit words 
	memset(&UserData[3], 0, *P500E->ChanHeadLen*sizeof(U32)*(*P500E->SumChanLen-3)); */

	/* Create temporary storage to store event information */
	if(!(ChanHeader = calloc(*P500E->ChanHeadLen, sizeof(U16)))) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): not enough memory for ChanHeader");
		Pixie_Print_MSG(ErrMSG,1);
		fclose(LMP5->ListModeFile);
		free(LMP5);
		free(P500E);
		return(-2);
	}
	/* Create temporary storage to store high word of the time stamp */
	/* This information used to be only available for the entire event */
	/* In P500e it is recorded for every channel */
	if(!(TimeHighWord = calloc(NUMBER_OF_CHANNELS, sizeof(U16)))) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): not enough memory for TimeHighWord");
		Pixie_Print_MSG(ErrMSG,1);
		fclose(LMP5->ListModeFile);
		free(LMP5);
		free(P500E);
		free(ChanHeader);
		return(-2);
	}

	/* Allocate memory for traces */
	for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
		/* *P500E->SumChanLen is the sum of channel lengths in 32 16-bit words. */
		/* It is a safe optimum storage size estimation. */
		if(!Traces[k]) Traces[k] = calloc(*P500E->BlockSize*(U32)*P500E->SumChanLen, sizeof(U16)); 
		if(!Traces[k]) {
			sprintf(ErrMSG, "*ERROR* (Pixie_Event_Browser): not enough memory for Traces: %u", *P500E->SumChanLen);
			Pixie_Print_MSG(ErrMSG,1);
			fclose(LMP5->ListModeFile);
			free(LMP5);
			free(P500E);
			free(ChanHeader);
			free(TimeHighWord);
			return(-2);
		}
	}

	/* now read event header */ 
	Pixie_fseek(LMP5->ListModeFile, EvtPos, SEEK_SET);								// Position ListModeFile to the beginning of this event 
	fread(ChanHeader, sizeof(U16), *P500E->ChanHeadLen, LMP5->ListModeFile);	// Read event information 

	/*	Even though location handed down from UI is the location found after error checking in runtask 0x7007
		error checking is still necessary to catch bad tracelengths etc 
		Values of ChanHeader may change, so use these, not *P500E->XXX for the return values */
	if(ErrorChecking (ChanHeader, 1, RunType, LMP5->ListModeFile, P500E)) {				// Error checking. search forward (right) if event not found (unlikely)
		fclose(LMP5->ListModeFile);
		free(LMP5);
		free(P500E);
		free(ChanHeader);
		free(TimeHighWord);
		for (k = 0; k < NUMBER_OF_CHANNELS; k++) free(Traces[k]);
		return(-3);
	}

	/* Populate event information array from current event */
	pattern = *P500E->BoardVersion & 0x0FF0;
	switch (pattern) {
		case MODULETYPE_P500e:
		case MODULETYPE_P4e_16_500:
		case MODULETYPE_P4e_14_500:
		case MODULETYPE_P4e_12_500:
			*P500E->ADCrate = 500;
			break;
		case MODULETYPE_P4e_16_250:
		case MODULETYPE_P4e_14_250:
		case MODULETYPE_P4e_12_250:
      case MODULETYPE_PN_12_250:
      case MODULETYPE_PN_12_250P:
			*P500E->ADCrate = 250;
			break;
		case MODULETYPE_P4e_16_125:
		case MODULETYPE_P4e_14_125:
		case MODULETYPE_P4e_12_125:
			*P500E->ADCrate = 125;
			break;
		default:
			*P500E->ADCrate = 500;		// old files without BoardVersion are likely from P500e
			break;
	}

	UserData[1]   = *P500E->ADCrate;
	UserData[7+1] = *P500E->ModNum;
	UserData[7+2] = *P500E->RunType;	


	if((RunType & 0xFF0F)==0x402){
		// populate headers and (temp) traces
		UserData[7+3] = ChanHeader[4];								// Equivalent to Buffer Start Time High Word  
		UserData[7+4] = ChanHeader[8];								// Equivalent to Buffer Start Time Middle Word  
		UserData[7+5] = ChanHeader[9];								// Equivalent to Buffer Start Time Low Word 
		UserData[7+BHL] = ChanHeader[0] + 65536 * ChanHeader[1]; ;	// Event status 
		UserData[7+BHL+2] = ChanHeader[26];							// Time low 
		UserData[7+BHL+1] = ChanHeader[27];							// Time medium 
		for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
			UserData[7+BHL+EHL+CHL*k+1] = 65536 * (U32)ChanHeader[9+4*k] + (U32)ChanHeader[8+4*k]; /* Time medium and low*/
			UserData[7+BHL+EHL+CHL*k+2] = (U32)ChanHeader[10+4*k]; /* Energy */
			UserData[7+BHL+EHL+CHL*k+3] = 0; /* XIA PSA */
			UserData[7+BHL+EHL+CHL*k+4] = 0; /* User PSA */
			UserData[7+BHL+EHL+CHL*k+5] = 0; /* Extended Uretval */
			UserData[7+BHL+EHL+CHL*k+6] = 0; /* Extended Uretval */
			UserData[7+BHL+EHL+CHL*k+7] = 0; /* Extended Uretval */
			UserData[7+BHL+EHL+CHL*k+8] = 0; /* Extended Uretval */
			TraceSizeB = ChanHeader[11+4*k];
			if(  (LMP5->RunHeader[8+ k]>0) && (LMP5->RunHeader[8+ k]<MAXFIFOBLOCKS+1) )  // if within legal limits, check against run header
				TraceSizeB = MIN(TraceSizeB, LMP5->RunHeader[8+ k] -1 );	// never longer than the value from file header (DSP setting) (if valid. in particular, older files may have event size field = 0, which would imply TL<0)
			TraceSizeB = MIN(TraceSizeB, MAXFIFOBLOCKS );								// never longer than the MAXFIFOBLOCKS. (negative not possible for U32)
			TraceSizeR = *P500E->BlockSize * TraceSizeB;							// trace size in words16 for reading and returning to User, 
			TraceSizeR = MIN(TraceSizeR, EvtLength );								// never longer than event length
			UserData[3+k] = TraceSizeR;	
			fread(Traces[k], 2, TraceSizeR, LMP5->ListModeFile);
		}
	}
	else {		// other run types are single event records, search for 4 closest
		UserData[7+3] = ChanHeader[6];								// Equivalent to Buffer Start Time High Word  
		UserData[7+4] = ChanHeader[5];								// Equivalent to Buffer Start Time Middle Word  
		UserData[7+5] = ChanHeader[4];								// Equivalent to Buffer Start Time Low Word 
		UserData[7+BHL] = ChanHeader[0] + 65536 * ChanHeader[1]; ;	// Event status 
		UserData[7+BHL+2] = ChanHeader[4];							// Time low 
		UserData[7+BHL+1] = ChanHeader[5];							// Time medium 
		EvtChanNum = ChanHeader[9];
		TraceSizeB = ChanHeader[2];												// trace size in blocks from header
		if(   (EvtChanNum < NUMBER_OF_CHANNELS)									// if within legal limits, check against run header
		   && (LMP5->RunHeader[8+ EvtChanNum]>0) 
		   && (LMP5->RunHeader[8+ EvtChanNum]<MAXFIFOBLOCKS+1) ) {
			TraceSizeB = MIN(TraceSizeB, LMP5->RunHeader[8+ EvtChanNum] -1 );	// never longer than the value from file header (DSP setting) (if valid. in particular, older files may have event size field = 0, which would imply TL<0)
		}
		TraceSizeB = MIN(TraceSizeB, MAXFIFOBLOCKS );								// never longer than the MAXFIFOBLOCKS. (negative not possible for U32)
		TraceSizeR = *P500E->BlockSize * TraceSizeB;							// trace size in words16 for reading and returning to User, 
		TraceSizeR = MIN(TraceSizeR, EvtLength );								// never longer than event length
		TimeHighWord[EvtChanNum] = ChanHeader[6];								// Save high word of the time stamp 
		TimeStamp =  (double)ChanHeader[4] + 65536.0 * (double)ChanHeader[5] + 4294967296.0 * (double)ChanHeader[6]; // Compute time stamp  in ticks  

		/* Search to the left in the time window to extract up to 4 channel records in coincidence with the selected event */
		Time = TimeStamp;
		CurrentChanNum = EvtChanNum;
		PrevEvtPos = (S32)EvtPos;
		TimeJitter = 20.0;
		if (TimeStamp >= TimeWindow / 2.0) 
			LeftTimeBoundary = TimeStamp - TimeWindow / 2.0;
		else                                                       
			LeftTimeBoundary = 0.0;
		AdvanceBeyondCounter = NUMBER_OF_CHANNELS;
		while (AdvanceBeyondCounter && (PrevEvtPos > 0)) {
			/* Record the closest event in time to the left of the current event */
			if (!EventFound[CurrentChanNum] && (fabs(Time - TimeStamp) <= TimeWindow / 2.0)) { /* Only one record per channel. No overwriting  */
				EventFound[CurrentChanNum] = 1;										// can not use UserData[3+CurrentChanNum] because the TL may be zero
				UserData[3+CurrentChanNum] = TraceSizeR;				
				UserData[7+BHL+EHL+CHL*CurrentChanNum+1] = 65536 * (U32)ChanHeader[5] + (U32)ChanHeader[4]; /* Time medium and low*/
				UserData[7+BHL+EHL+CHL*CurrentChanNum+2] = ChanHeader[8]; /* Energy */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+3] = ChanHeader[11]; /* XIA PSA */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+4] = ChanHeader[10]; /* User PSA */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+5] = ChanHeader[12]; /* Extended Uretval */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+6] = ChanHeader[13]; /* Extended Uretval */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+7] = ChanHeader[14]; /* Extended Uretval */
				UserData[7+BHL+EHL+CHL*CurrentChanNum+8] = ChanHeader[15]; /* Extended Uretval */
				TimeHighWord[CurrentChanNum] = ChanHeader[6]; /* Time High */
				fread(Traces[CurrentChanNum], 2, TraceSizeR, LMP5->ListModeFile);
			}
			if (TimeWindow > 65535.0) {/* Larger than 64k coincidence window disables searching for the neighboring events */
				/* Copy found traces into external data array UserData and release dynamic memory */
				CurrentPosition = 7+BHL+EHL+NUMBER_OF_CHANNELS*CHL;
				for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
					if (Traces[k]) {
						for(j = 0; j < UserData[3+k]; j++) 
							UserData[CurrentPosition+j] = (U32)*(Traces[k] + j); // TODO: does this need a check if CurrentPosition+j > EvtLength
						free(Traces[k]);
						CurrentPosition += UserData[3+k];
					}
				}
				fclose(LMP5->ListModeFile);
				free(LMP5);
				free(P500E);
				free(ChanHeader);
				free(TimeHighWord);

				return (0); /* Return peacefully */
			}
			
			/* read another channel header to the left */
			PrevEvtSize =(U32)(ChanHeader[3] + 1) * *P500E->BlockSize;
			PrevEvtPos -= (S64)(2 * PrevEvtSize);
			if (PrevEvtPos > 0) {
				Pixie_fseek(LMP5->ListModeFile, PrevEvtPos, SEEK_SET);
				fread(ChanHeader, sizeof(U16), *P500E->ChanHeadLen, LMP5->ListModeFile);
				if(ErrorChecking (ChanHeader, 0, RunType, LMP5->ListModeFile, P500E)) {			// check prev. event, if not found (prev length rounded down), look to left
					fclose(LMP5->ListModeFile);
					free(LMP5);
					free(P500E);
					free(ChanHeader);
					free(TimeHighWord);
					for (k = 0; k < NUMBER_OF_CHANNELS; k++) free(Traces[k]);
					return(-3);
				}
				CurrentChanNum = ChanHeader[9];
				TraceSizeB = ChanHeader[2];												// trace size in blocks from header
				if(   (CurrentChanNum < NUMBER_OF_CHANNELS)									// if within legal limits, check against run header
				   && (LMP5->RunHeader[8+ CurrentChanNum]>0) 
				   && (LMP5->RunHeader[8+ CurrentChanNum]<MAXFIFOBLOCKS+1) ) {
					TraceSizeB = MIN(TraceSizeB, LMP5->RunHeader[8+ CurrentChanNum] -1 );	// never longer than the value from file header (DSP setting) (if valid. in particular, older files may have event size field = 0, which would imply TL<0)
				}
				TraceSizeB = MIN(TraceSizeB, MAXFIFOBLOCKS );								// never longer than the MAXFIFOBLOCKS. (negative not possible for U32)
				TraceSizeR = *P500E->BlockSize * TraceSizeB;							// trace size in words16 for reading and returning to User, 
				TraceSizeR = MIN(TraceSizeR, EvtLength );								// never longer than event length
				Time =  (double)ChanHeader[4] + 65536.0 * (double)ChanHeader[5] + 4294967296.0 * (double)ChanHeader[6]; /* Time in ticks */ 
				
				if (Time < LeftTimeBoundary - TimeJitter) AdvanceBeyondCounter--;		// decrement loop counter when outside window 
				// WH_TODO: this loop runs until 4 events are outside window. Better run until each channel is outside window (what if one channel has 10 events while the others have 1?)
			}
		}

		/* Now search to the right in the time window to extract up to 4 channel records in coincidence with the selected event */
		/* Position LMP5->ListModeFile to the beginning of the current event */
		if(Pixie_fseek(LMP5->ListModeFile, EvtPos, SEEK_SET)) {
			for (j = 0; j < NUMBER_OF_CHANNELS; j++) if (Traces[j]) free(Traces[j]);
			fclose(LMP5->ListModeFile);
			free(LMP5);
			free(P500E);
			free(ChanHeader);
			free(TimeHighWord);
			return (0); /* Return peacefully if it does not exist. unlikely */
		}

		/* Read the header of the current event. WH_TODO: why again?*/
		fread(ChanHeader, 2, (size_t)*P500E->ChanHeadLen, LMP5->ListModeFile);
		if(ErrorChecking (ChanHeader, 1, RunType, LMP5->ListModeFile, P500E)) {
			fclose(LMP5->ListModeFile);
			free(LMP5);
			free(P500E);
			free(ChanHeader);
			free(TimeHighWord);
			for (k = 0; k < NUMBER_OF_CHANNELS; k++) free(Traces[k]);
			return(-3);
		}
		TraceSize  = (U32)ChanHeader[2] * *P500E->BlockSize;					// length in file
		EvtChanNum = ChanHeader[9];
		TraceSizeB = ChanHeader[2];												// trace size in blocks from header, corrected by error checking
		if(   (EvtChanNum < NUMBER_OF_CHANNELS)									// if within legal limits, check against run header
		   && (LMP5->RunHeader[8+ EvtChanNum]>0) 
		   && (LMP5->RunHeader[8+ EvtChanNum]<MAXFIFOBLOCKS+1) ) {
			TraceSizeB = MIN(TraceSizeB, LMP5->RunHeader[8+ EvtChanNum] -1 );	// never longer than the value from file header (DSP setting) (if valid. in particular, older files may have event size field = 0, which would imply TL<0)
		}
		TraceSizeB = MIN(TraceSizeB, MAXFIFOBLOCKS );								// never longer than the MAXFIFOBLOCKS. (negative not possible for U32)
		TraceSizeR = *P500E->BlockSize * TraceSizeB;							// trace size in words16 for reading and returning to User, 
		TraceSizeR = MIN(TraceSizeR, EvtLength );								// never longer than event length

		Time = TimeStamp;
		RightTimeBoundary = TimeStamp + TimeWindow / 2.0;
		CurrentChanNum = EvtChanNum;
		AdvanceBeyondCounter = NUMBER_OF_CHANNELS;
		status = (S32)*P500E->ChanHeadLen; 
		while (AdvanceBeyondCounter && (status == (S32)*P500E->ChanHeadLen)) {
			/* Record the closest event in time to the right of the current event */
			if (CurrentChanNum > (NUMBER_OF_CHANNELS-1)) break; /* If an error occurs, stop scanning to the right */
			if (CurrentChanNum != EvtChanNum) {
				if ((!EventFound[CurrentChanNum] && (fabs(Time - TimeStamp) <= TimeWindow / 2.0)) 
					|| ((PreviousTime > 0) && (fabs(Time-TimeStamp) < fabs(PreviousTime-TimeStamp)))) { /* No overwriting unless closer in time  */					
						EventFound[CurrentChanNum] =1;
						UserData[3+CurrentChanNum] = TraceSizeR;
						UserData[7+BHL+EHL+CHL*CurrentChanNum+1] = 65536 * (U32)ChanHeader[5] + (U32)ChanHeader[4]; /* Time medium and low*/
						UserData[7+BHL+EHL+CHL*CurrentChanNum+2] = ChanHeader[8]; /* Energy */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+3] = ChanHeader[11]; /* XIA PSA, low word first ordering */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+4] = ChanHeader[10]; /* User PSA */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+5] = ChanHeader[13]; /* Extended Uretval */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+6] = ChanHeader[12]; /* Extended Uretval */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+7] = ChanHeader[15]; /* Extended Uretval */
						UserData[7+BHL+EHL+CHL*CurrentChanNum+8] = ChanHeader[14]; /* Extended Uretval */
						TimeHighWord[CurrentChanNum] = ChanHeader[6]; /* Time High */
						fread(Traces[CurrentChanNum], 2, TraceSizeR, LMP5->ListModeFile);
						if(TraceSizeR<TraceSize)
							Pixie_fseek(LMP5->ListModeFile, 2*(S64)(TraceSize-TraceSizeR), SEEK_CUR);		// skip over "trace" that exceeds DSP setting
				}
				else Pixie_fseek(LMP5->ListModeFile, 2*(S64)TraceSize, SEEK_CUR);		// skip trace of channel we don't want
			}
			else Pixie_fseek(LMP5->ListModeFile, 2*(S64)TraceSize, SEEK_CUR);			// skip trace of this event's channel, which we already have

			// get next event
			status = fread(ChanHeader, 2, (size_t)*P500E->ChanHeadLen, LMP5->ListModeFile);
			if(ErrorChecking (ChanHeader, 1, RunType, LMP5->ListModeFile, P500E)) {
				fclose(LMP5->ListModeFile);
				free(LMP5);
				free(P500E);
				free(ChanHeader);
				free(TimeHighWord);
				for (k = 0; k < NUMBER_OF_CHANNELS; k++) free(Traces[k]);
				return(-3);
			}
			Time =  (double)ChanHeader[4] + 65536.0 * (double)ChanHeader[5] + 4294967296.0 * (double)ChanHeader[6]; /* Time in ticks */
			CurrentChanNum = ChanHeader[9];
			PreviousTime = (double)UserData[7+BHL+EHL+CHL*CurrentChanNum+1] + 4294967296.0 * (double)TimeHighWord[CurrentChanNum]; /* Time High. Time in ticks */
			TraceSize = (U32)ChanHeader[2] * *P500E->BlockSize;
			TraceSizeB = ChanHeader[2];												// trace size in blocks from header
			if(   (EvtChanNum < NUMBER_OF_CHANNELS)									// if within legal limits, check against run header
			   && (LMP5->RunHeader[8+ CurrentChanNum]>0) 
			   && (LMP5->RunHeader[8+ CurrentChanNum]<MAXFIFOBLOCKS+1) ) {
				TraceSizeB = MIN(TraceSizeB, LMP5->RunHeader[8+ CurrentChanNum] -1 );	// never longer than the value from file header (DSP setting) (if valid. in particular, older files may have event size field = 0, which would imply TL<0)
			}
			TraceSizeB = MIN(TraceSizeB, MAXFIFOBLOCKS );								// never longer than the MAXFIFOBLOCKS. (negative not possible for U32)
			TraceSizeR = *P500E->BlockSize * TraceSizeB;							// trace size in words16 for reading and returning to User, 
			TraceSizeR = MIN(TraceSizeR, EvtLength );								// never longer than event length

			if (Time > RightTimeBoundary + TimeJitter) AdvanceBeyondCounter--;		// decrement loop counter when outside window
		}
	}	// run type

	/* Special records: extract values from special data to populate return values */
	if(UserData[7+BHL] == RSRMARK)	// check event pattern for RS record ID
	// special run stats record. 
	// Run stats in trace0 block. Arranged in parA.ch0,parA.ch1,parA.ch2,...,parB.ch0,parB.ch1 etc  
	{
		j = 4;	// User PSA offset in UserData
		for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
			val16 = *(Traces[0]+17*NUMBER_OF_CHANNELS+k) ;	// GCOUNT [31:0] in words 17, 16 of the runstats block
			UserData[7+BHL+EHL+CHL*k+j] = val16; 	// User PSA
			val16 = *(Traces[0]+16*NUMBER_OF_CHANNELS+k);
			UserData[7+BHL+EHL+CHL*k+j-1] = val16; 	// XIA PSA
		}	
		j = 1;	// timestamp offset in UserData
		for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
			val32 = *(Traces[0]+27*NUMBER_OF_CHANNELS+k) * 65536 + *(Traces[0]+28*NUMBER_OF_CHANNELS+k) ;	// timestamp [31:0] of RS latch  in words 27, 28 of the runstats block
			UserData[7+BHL+EHL+CHL*k+j] = val32; 
		}	
	}


	/* Copy found traces into external data array UserData and release dynamic memory */
	CurrentPosition = 7+BHL+EHL+NUMBER_OF_CHANNELS*CHL;
	for (k = 0; k < NUMBER_OF_CHANNELS; k++) {
		for(j = 0; j < UserData[3+k]; j++) 
			UserData[CurrentPosition+j] = (U32)*(Traces[k] + j);				// TODO: does this need a check if CurrentPosition+j > EvtLength
		free(Traces[k]);
		CurrentPosition += UserData[3+k];
	}
	/* Close data file */
	fclose(LMP5->ListModeFile);
	/* Free memory */
	free(ChanHeader);
	free(TimeHighWord);
	free(LMP5);
	free(P500E); 
	return(0);
}

/************************************************************************************************************/
/*************************************************************************************************************/
/************************** P4 READER FOR COMPATIBILITY **************************************/
/************************************************************************************************************/
/************************************************************************************************************/
/* Global data structure for the list mode reader and related functions */
struct LMReaderDataStruct ListModeDataStructure;
/* Pointer to the global data structure for the list mode reader and related functions */
LMR_t LMA = &ListModeDataStructure;

/****************************************************************
 *	PixieListModeReader function:
 *		Parse list mode data file using analysis logic 
 *              defined in six functions. These functions contain 
 *              logic to be applied:
 *              1) before the file processing begins;
 *              2) after reading buffer header;
 *              3) after reading event header;
 *              4) after reading channel data (header + trace if applicable);
 *              5) for a channel not in the read pattern;
 *              6) after the file processing is finished.
 *
 *              The reader uses a data structure (LMA) globally defined in 
 *              globals.c and typedefed in utilities.h. This structure contains all
 *              the necessary variables to read list mode files as well as function
 *              and void pointers for the user analysis functions to be linked in and 
 *              additional user parameters to be exchanged between the functions.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *                      -2 - buffer length zero; corrupted file
 *                      -3 - wrong run type
 *
 ****************************************************************/

int PixieListModeReader (LMR_t LMA)
{

	U16 RunType                = 0;
	U16 ModuleType = 0;
	U16 ChannelHeaderLengths[] = {P4_MAX_CHAN_HEAD_LENGTH, P4_MAX_CHAN_HEAD_LENGTH, 4, 2};
	U16 MaxChanLen = 0;
	U16 MaxBufLen = 8192;
	S32 ReturnValue            = 0;

	/* Open the list mode file if exists */
	if(!(LMA->ListModeFile = fopen(LMA->ListModeFileName, "rb"))) { 
		sprintf(ErrMSG, "*ERROR* (PixieListModeReader): can't open list mode data file %s", LMA->ListModeFileName);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	/* Read the first buffer header to glean initial information about the type of the list mode file */
	fread (LMA->BufferHeader, sizeof(U16), BUFFER_HEAD_LENGTH, LMA->ListModeFile);
	/* User requests only first header for identification if LMA->ReadFirstBufferHeader is set */
	if (LMA->ReadFirstBufferHeader) {
		fclose(LMA->ListModeFile);
		return (0);
	}
	/* Otherwise rewind to the beginning of the file */
	rewind(LMA->ListModeFile);
	/* Determine the module type and the coresponding channel length limitation */
	/* The upper 4 bits of the record indicate the module type */
	ModuleType = LMA->BufferHeader[2] & 0xF000;
	if (ModuleType == 2) MaxChanLen = 1024;
	else MaxChanLen = 8192;
	/* Determine the run type and coresponding channel header length */
	/* The lower 12 bits of the record indicate the run task number */
	RunType = LMA->BufferHeader[2] & 0x0F0F; 
	if (RunType < 0x100 || RunType > 0x103) { /* If run type is bogus then finish processing */
		fclose(LMA->ListModeFile);
		sprintf(ErrMSG, "*ERROR* (PixieListModeReader): wrong run type");
		Pixie_Print_MSG(ErrMSG,1);
		return (-3);
	}
	/* Chose the right channel header length from the list */
	LMA->ChanHeadLen = ChannelHeaderLengths[RunType-0x100];
	/* If defined call pre-analysis user logic function */
	if (LMA->PreAnalysisAction) LMA->PreAnalysisAction(LMA);
	/* Read the list mode file and do the processing */
	while (!feof(LMA->ListModeFile)) { /* Loop over buffer headers */
		fread (LMA->BufferHeader, sizeof(U16), BUFFER_HEAD_LENGTH, LMA->ListModeFile);
		/* If buffer header length is zero, things went awry */
		if (!LMA->BufferHeader[0] || LMA->BufferHeader[0] > MaxBufLen) break;
		/* If defined call buffer-level user logic function */
		if (LMA->BufferLevelAction) LMA->BufferLevelAction(LMA);
		LMA->BufferHeader[0] -= BUFFER_HEAD_LENGTH;
		while (LMA->BufferHeader[0]) {/* Loop over event headers */
			fread (LMA->EventHeader, sizeof(U16), EVENT_HEAD_LENGTH, LMA->ListModeFile);
			LMA->BufferHeader[0] -= EVENT_HEAD_LENGTH;
			/* If defined call event-level user logic function */
			if (LMA->EventLevelAction) LMA->EventLevelAction(LMA);
			for (LMA->Channel = 0; LMA->Channel < NUMBER_OF_CHANNELS; LMA->Channel++) { /* Loop over channel headers and traces if present */
				if (LMA->EventHeader[0] & (0x1 << LMA->Channel)) { /* Only go over channels defined in read pattern */
					fread (LMA->ChannelHeader, sizeof(U16), LMA->ChanHeadLen, LMA->ListModeFile);
					if (LMA->ChannelHeader[0] > MaxChanLen && RunType < 0x102) {
						sprintf(ErrMSG, "*ERROR* (PixieListModeReader): channel header corruption encountered CH[0]=%d",LMA->ChannelHeader[0]);
						Pixie_Print_MSG(ErrMSG,1);
						fclose(LMA->ListModeFile);
						return (-2);
					}
					if (RunType == 0x100) { /* Read trace if it is run 0x100 */
						fread (LMA->Trace, sizeof(U16), LMA->ChannelHeader[0] - LMA->ChanHeadLen, LMA->ListModeFile);
						LMA->BufferHeader[0] -= LMA->ChannelHeader[0];
					}
					else LMA->BufferHeader[0] -= LMA->ChanHeadLen;
					/* If defined call channel-level user logic function */
					if (LMA->ChannelLevelAction) LMA->ChannelLevelAction(LMA);
					LMA->Traces[LMA->BufferHeader[1]]++; /* Count traces in each module */
					LMA->TotalTraces++; /* Count all traces */
				}
				else { /* If the channel is not in the read pattern some action may still be needed */
					/* If defined call auxiliary channel action function */
					if (LMA->AuxChannelLevelAction) LMA->AuxChannelLevelAction(LMA);
				}
			}
			LMA->Events[LMA->BufferHeader[1]]++; /* Count events in each module */
			LMA->TotalEvents++;  /* Count all events */
		}
		LMA->Buffers[LMA->BufferHeader[1]]++;    /* Count buffers in each module */
		LMA->TotalBuffers++;  /* Count all buffers */
	}
	/* Check for premature EOF due to an error */
	if (!feof(LMA->ListModeFile)) {
		sprintf(ErrMSG, "*ERROR* (PixieListModeReader): buffer header corruption encountered");
		Pixie_Print_MSG(ErrMSG,1);
		ReturnValue = -2; /* If so return a corresponding error code */
	}
	/* If defined call post-analysis user logic function */
	if (LMA->PostAnalysisAction) LMA->PostAnalysisAction(LMA);
	/* Close list mode file */
	fclose(LMA->ListModeFile);
	/* Done */
	return(ReturnValue);

}

/****************************************************************
 *	Pixie_Parse_List_Mode_Events function:
 *		Parse the list mode events from the list mode data file,
 *		write values of time stamp, energy, XIA_PSA, user_PSA
 *		of each event into a text file, and send the number of events
 *		and traces in each module to the host.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - NULL pointer *ModuleEvents
 *			-2 - can't open list mode data file
 *			-3 - can't open output file
 *			-4 - found BufNdata = 0
 *
 ****************************************************************/

void *allocate_array (size_t drow, size_t dcol, char   type)
{
	/* Create an array of different types */

	char **m        = NULL;
	char **mfirst   = NULL;
	char  *auxptr   = NULL;
	size_t dcolsize = 0;
	size_t ptrsize  = sizeof(char *);
	size_t varsize  = 0;

	if (type == 'c' ) dcolsize = dcol * (varsize = sizeof(char));
	if (type == 's' ) dcolsize = dcol * (varsize = sizeof(unsigned short));
	if (type == 'u' ) dcolsize = dcol * (varsize = sizeof(unsigned int));
	if (type == 'f' ) dcolsize = dcol * (varsize = sizeof(float));
	if (type == 'd' ) dcolsize = dcol * (varsize = sizeof(double));
	if (varsize == 0) return NULL; // Wrong type specified

	mfirst = m = calloc(drow, ptrsize);
	if (m)
	{
		auxptr = *m = calloc(drow, dcolsize);
		if (*m)
		{
			while (--drow) *(++m) = (auxptr += dcolsize);
			return (void *)mfirst;
		}
		free(m);
	}

	return NULL;
}

void free_array (void **a)
{
    /* Destroy a previously created array */

    free (*a);
    free (a);

}

S32 PixieParseListModeEventsBufferLevel (LMR_t LMA)
{ 

	/* This is a buffer-level user function for the list mode reader */
	U16 Module           = LMA->BufferHeader[1];
	U16 ModuleID         = LMA->BufferHeader[2] & 0xF000; /* The upper 4 bits of the record indicate the module type: DGF, Pixie-4, P500 */
	U16 System_Clock_MHz = 0;

	if (ModuleID == 0x2000) System_Clock_MHz = P4_SYSTEM_CLOCK_MHZ;
	//if (ModuleID == 0x4000) System_Clock_MHz = P500_SYSTEM_CLOCK_MHZ;		 P500 not supported in this SW

	if(AutoProcessLMData > 0 && LMA->Buffers[Module] == 0) { /* Print out the Module header if first buffer */
		FILE  *OutputFilePerModule = ((FILE **)LMA->par1)[Module];
		FILE *OutputFile                    =   (FILE *)LMA->par0;
		U16    RunType    = LMA->BufferHeader[2] & 0x0FFF;
		double RunStartTime = ((double)LMA->BufferHeader[3] * 65536.0 * 65536.0 + (double)LMA->BufferHeader[4] * 65536.0 + (double)LMA->BufferHeader[5]) / (double)System_Clock_MHz * 1.0e-6;
		if(AutoProcessLMData != 4) fprintf(OutputFilePerModule, "\nModule:\t%d\n",         Module);
		if(AutoProcessLMData != 4) fprintf(OutputFilePerModule, "Run Type:\t%d\n",         RunType);
		if(AutoProcessLMData != 4) fprintf(OutputFilePerModule, "Run Start Time:\t%f\n\n", RunStartTime);
		if(AutoProcessLMData == 1) {
			if (RunType < 0x102) fprintf(OutputFilePerModule, "Event No\tChannel No\tEnergy\tTrig Time\tXIA_PSA\tUser_PSA\n");
			if (RunType > 0x101) fprintf(OutputFilePerModule, "Event No\tChannel No\tEnergy\tTrig Time\n");
		}
		if(AutoProcessLMData == 2) fprintf(OutputFilePerModule, "Event No\tChannel No\tHit Pattern\tEvent_Time_A\tEvent_Time_B\tEvent_Time_C\tEnergy\tTrig Time\tXIA_PSA\tUser_PSA\n");
		if(AutoProcessLMData == 3) fprintf(OutputFilePerModule, "Event\tChannel\tTimeStamp\tEnergy\tRT\tApeak\tBsumC\tCsum\tPsum\tPSAval\n");
		if(AutoProcessLMData == 4 && Module == 0) fprintf(OutputFile, "Event\tMulti\tEa\tEb\tEc\tTa\tTb\tTc\tChanA\tChanB\tChanC\tMa\tMb\tMc\n");
	}

	return (0);
}

S32 PixieParseListModeEventsChannelLevel (LMR_t LMA) 
{
    
    /* This is a channel-level user function for the list mode reader */
	if(AutoProcessLMData > 0) {
		U8 *(*Format)[5]          = LMA->par2;
		S32 **DT4Data				= (S32 **)LMA->par3;
		U16   Module              = LMA->BufferHeader[1];
		U16   RunType             = LMA->BufferHeader[2] & 0x0FFF;
		U16   chl                 = LMA->ChanHeadLen;
		U16   Channel             = LMA->Channel;
		U16   TrigTime            = 0;
		U16   Energy              = 0;
		U16   XIA_PSA             = LMA->ChannelHeader[3];    
		U16   User_PSA            = LMA->ChannelHeader[4];
		U16   HT                  = LMA->ChannelHeader[8];
		U16   EvtPattern          = LMA->EventHeader[0];
		U16   ETH                 = LMA->EventHeader[1];    
		U16   ETL                 = LMA->EventHeader[2];
		U32   NumEvents           = LMA->Events[Module];
		FILE *OutputFilePerModule = ((FILE **)LMA->par1)[Module];

		if (RunType < 0x102) {
			TrigTime            = LMA->ChannelHeader[1];
			Energy              = LMA->ChannelHeader[2];
		}
		if (RunType > 0x101) {
			TrigTime            = LMA->ChannelHeader[0];
			Energy              = LMA->ChannelHeader[1];
		}
		if(chl == 4) HT = 0;
		if(chl == 2) HT = XIA_PSA = User_PSA = 0;
		if(AutoProcessLMData == 1) fprintf(OutputFilePerModule, Format[chl][1], NumEvents, Channel, Energy, TrigTime, XIA_PSA, User_PSA);
		if(AutoProcessLMData == 2) fprintf(OutputFilePerModule, Format[chl][2], NumEvents, Channel, EvtPattern, HT, ETH, ETL, Energy, TrigTime, XIA_PSA, User_PSA); 
		if(AutoProcessLMData == 3) fprintf(OutputFilePerModule, 
			"%d   %d   %d   %d   %d   %d   %d   %d  %d  %d\n", 
			NumEvents, 
			Channel,
			LMA->ChannelHeader[1],
			LMA->ChannelHeader[2],
			LMA->ChannelHeader[3],
			LMA->ChannelHeader[4],
			LMA->ChannelHeader[5],
			LMA->ChannelHeader[6],
         LMA->ChannelHeader[7],
			LMA->ChannelHeader[8]);
		if (AutoProcessLMData == 4) {
			/* If multiplicity is >=3 and event numbers coincide fill up the third record */
			if (DT4Data[NumEvents][1] >= 3 && Energy>0) 
				DT4Data[NumEvents][1] ++ ; /* Multiplicity becomes two */			

			 /* If multiplicity is two and event numbers coincide fill up the third record */
			if (DT4Data[NumEvents][1] == 2 && Energy>0 ) {
					DT4Data[NumEvents][1] = 3; /* Multiplicity becomes 3 */
					DT4Data[NumEvents][4] = Energy; /* Energy C */
					DT4Data[NumEvents][7] = ETL; /* Time C */
					DT4Data[NumEvents][10] = Channel + 4 * Module; /* Channel C */
					DT4Data[NumEvents][13] = Module; /* Module C */ 

				if ( (abs(DT4Data[NumEvents][5]-ETL)>10) && (abs(DT4Data[NumEvents][6]-ETL)<=10) ) {
					// close to B and different from A -> move A to C, store new in A
					DT4Data[NumEvents][4] = DT4Data[NumEvents][2]; /* Energy C */
					DT4Data[NumEvents][7] = DT4Data[NumEvents][5]; /* Time C */
					DT4Data[NumEvents][10] = DT4Data[NumEvents][8]; /* Channel C */
					DT4Data[NumEvents][13] = DT4Data[NumEvents][11]; /* Module C */ 

					DT4Data[NumEvents][2] = Energy; /* Energy A */
					DT4Data[NumEvents][5] = ETL; /* Time A */
					DT4Data[NumEvents][8] = Channel + 4 * Module; /* Channel A */
					DT4Data[NumEvents][11] = Module; /* Module A */
				}
				if ( (abs(DT4Data[NumEvents][5]-ETL)<=10) && (abs(DT4Data[NumEvents][6]-ETL)>10) ) {
					// close to A and different from B -> move B to C, store new in B
					DT4Data[NumEvents][4] = DT4Data[NumEvents][3]; /* Energy C */
					DT4Data[NumEvents][7] = DT4Data[NumEvents][6]; /* Time C */
					DT4Data[NumEvents][10] = DT4Data[NumEvents][9]; /* Channel C */
					DT4Data[NumEvents][13] = DT4Data[NumEvents][12]; /* Module C */ 

					DT4Data[NumEvents][3] = Energy; /* Energy B */
					DT4Data[NumEvents][6] = ETL; /* Time B */
					DT4Data[NumEvents][9] = Channel + 4 * Module; /* Channel B */
					DT4Data[NumEvents][12] = Module; /* Module B */
				}
			

			}
			/* If multiplicity is one and event numbers coincide fill up the second record */
			if (DT4Data[NumEvents][1] == 1 && Energy>0 ) {
				if(DT4Data[NumEvents][5]<=ETL){
					DT4Data[NumEvents][1] = 2; /* Multiplicity becomes two */
					DT4Data[NumEvents][3] = Energy; /* Energy B */
					DT4Data[NumEvents][6] = ETL; /* Time B */
					DT4Data[NumEvents][9] = Channel + 4 * Module; /* Channel B */
					DT4Data[NumEvents][12] = Module; /* Module B */
				}
				else{
					DT4Data[NumEvents][1] = 2; /* Multiplicity becomes two */
					DT4Data[NumEvents][3] = DT4Data[NumEvents][2]; /* Energy B */
					DT4Data[NumEvents][6] = DT4Data[NumEvents][5]; /* Time B */
					DT4Data[NumEvents][9] = DT4Data[NumEvents][8]; /* Channel B */
					DT4Data[NumEvents][12] = DT4Data[NumEvents][11]; /* Module B */

					DT4Data[NumEvents][2] = Energy; /* Energy A */
					DT4Data[NumEvents][5] = ETL; /* Time A */
					DT4Data[NumEvents][8] = Channel + 4 * Module; /* Channel A */
					DT4Data[NumEvents][11] = Module; /* Module A */
				}
			}
			 /* If multiplicity is zero fill up the first record */
			if (!DT4Data[NumEvents][1] && Energy>0 ) {
				DT4Data[NumEvents][0] = NumEvents;
				DT4Data[NumEvents][1] = 1; /* Multiplicity becomes one */
				DT4Data[NumEvents][2] = Energy; /* Energy A */
				DT4Data[NumEvents][5] = ETL; /* Time A */
				DT4Data[NumEvents][8] = Channel + 4 * Module; /* Channel A */
				DT4Data[NumEvents][11] = Module; /* Module A */
			}
		}
	}
	return (0);
}


S32 PixieParseListModeEventsPostAnalysisLevel (LMR_t LMA) 
{
    
	if(AutoProcessLMData == 4) {
		S32 **DT4Data				= (S32 **)LMA->par3;
		U16   Module              = LMA->BufferHeader[1];
		U16   Channel             = LMA->Channel;
		U16   TrigTime            = 0;
		U16   Energy              = 0;
		U32   NumEvents           = LMA->Events[Module];
		U32 i, j;
		FILE *OutputFile                    =   (FILE *)LMA->par0;

		for (i = 0; i < NumEvents; i++) {
			if ((DT4Data[i][1] == 2 && /* If multiplicity is two */
				abs(DT4Data[i][5]-DT4Data[i][6]) < 10)  /* If time stamps match */
				|| /* or */ 
				(DT4Data[i][1] >= 3 && /* If multiplicity is three */
					(abs(DT4Data[i][5]-DT4Data[i][6]) < 10 ||	/* If time stamps match */
					abs(DT4Data[i][7]-DT4Data[i][6]) < 10 ||
					abs(DT4Data[i][5]-DT4Data[i][7]) < 10))  
				) {
						for (j = 0; j < 14; j++) {
							fprintf(OutputFile, "%d\t", DT4Data[i][j]);
						}
						fprintf(OutputFile, "\n");
			}
		}
	
	}
	return (0);
}


S32 Pixie_Parse_List_Mode_Events (S8  *filename,      /* the list mode data file name (with complete path) */
			          U32 *ModuleEvents ) /* receives number of events & traces for each module */
{
    U8   			  *Format[10][5] = {NULL};
    U8                 OutputFileName[256]           = {'\0'};
    U8                 TempFileName  [256]           = {'\0'};
	S32				**DT4Data							= NULL;
    U16                i												=  0;
    FILE              *OutputFile						=  NULL;
    FILE              *TempFiles[PRESET_MAX_MODULES] = {NULL};
    
    /* Check if pointer *ModuleEvents has been initialized */
	if(!ModuleEvents) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Events): NULL pointer *ModuleEvents");
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
    
	//sprintf(ErrMSG, "*INFO* (Pixie_Parse_List_Mode_Events): Auto variable: %d", AutoProcessLMData);
	//Pixie_Print_MSG(ErrMSG,1);

	Format[9][1] = "%-9d%-12d%-9d%-15d%-9d%-6d\n";
    Format[9][2] = "%d\t %d\t %X\t %d\t %d\t %d\t %d\t %d\t %d\t %d\n";
    Format[4][1] = Format[9][1];
    Format[4][2] = "%-9d%-12d%-12x%-12d%-15d%-15d%-9d%-12d%-9d%-6d\n";
    Format[2][1] = "%-9d%-12d%-9d%-15d\n";
    Format[2][2] = Format[4][2];
	
    /* Zero the list mode reader arguments structure */ 
    memset(LMA, 0, sizeof(*LMA)); 
    /* If a record file is requested */
	if (AutoProcessLMData > 0) {
		if ( AutoProcessLMData == 4) { /* For AutoProcessLMData = 4 determine the number of recorded traces*/
			LMA->ListModeFileName = filename;
			/* Do the processing */
			if (PixieListModeReader(LMA) == -1) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Traces): Cannot open list mode file");
				Pixie_Print_MSG(ErrMSG,1);
				return (-2); /* Error means "cannot open file" */
			}
			sprintf(ErrMSG, "*INFO* (Pixie_Parse_List_Mode_Events): Traces: %d", LMA->Events[0]);
			Pixie_Print_MSG(ErrMSG,1);
			/* Create a 2D array of run data */
			if (!(DT4Data = allocate_array (LMA->Events[0]+100, 14, 'u'))) {
				sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Traces): Cannot allocate 2D array");
				Pixie_Print_MSG(ErrMSG,1);
				return (-3);
			}
			/* Zero the list mode reader arguments structure */ 
			memset(LMA, 0, sizeof(*LMA));
		}
		/* Determine the name of the output file */
		strcpy(OutputFileName, filename); 
		/* Check if the user requests outputing parsed data to a file */
		if(AutoProcessLMData == 1) strcpy(&OutputFileName[strlen(OutputFileName)-3], "dat");
		/* Long output data file with full timestamp and hit pattern */
		if(AutoProcessLMData == 2) strcpy(&OutputFileName[strlen(OutputFileName)-3], "dt2");
		/* Long output data file with Energy, Time and various PSA values*/
		if(AutoProcessLMData == 3) strcpy(&OutputFileName[strlen(OutputFileName)-3], "dt3");
		/* Long output data file with Xe coincidence event data */
		if(AutoProcessLMData == 4) strcpy(&OutputFileName[strlen(OutputFileName)-3], "dt4");
		if(!(OutputFile = fopen(OutputFileName, "w"))) {
			sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Events): can't open output file", OutputFileName);
			Pixie_Print_MSG(ErrMSG,1);
			if (AutoProcessLMData == 4) free_array (DT4Data);
			return(-3);
		}
		/* Open temporary files for each module for writing if not AutoProcess variant 4*/
		if (AutoProcessLMData != 4) {
			for(i = 0; i < PRESET_MAX_MODULES; i++) {
				sprintf(TempFileName, "_TempFile_Module_%hu", i);
				if(!(TempFiles[i] = fopen(TempFileName, "w"))) {
					sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Events): can't open output temporary file", TempFileName);
					Pixie_Print_MSG(ErrMSG,1);
				}
			}
		}
		/* Populate data structure for AutoProcessLMData > 0 */
		LMA->par0               = OutputFile;
		LMA->par1               = TempFiles;
		LMA->par2               = Format;
		LMA->par3               = DT4Data;
		LMA->BufferLevelAction  = PixieParseListModeEventsBufferLevel;
		LMA->ChannelLevelAction = PixieParseListModeEventsChannelLevel;
		LMA->PostAnalysisAction = PixieParseListModeEventsPostAnalysisLevel;
	}
    
    /* Add file name to the data structure */
    LMA->ListModeFileName = filename;
    
    /* Do the processing */
	if (PixieListModeReader(LMA) == -1) {
		sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Traces): Cannot open list mode file");
		Pixie_Print_MSG(ErrMSG,1);
		if (AutoProcessLMData == 4) free_array (DT4Data);
		return (-2); /* Error means "cannot open file" */
	}

    /* Fill up ModeluEvents. If requested: close temp files for writing; */ 
    /* combine temp files into one output file; close and delete temp files */
	for(i = 0; i < PRESET_MAX_MODULES; i++) {
		MODULE_EVENTS[i]                    = ModuleEvents[i]                    = LMA->Events[i];
		MODULE_EVENTS[i+PRESET_MAX_MODULES] = ModuleEvents[i+PRESET_MAX_MODULES] = LMA->Traces[i];
		if (AutoProcessLMData > 0) {
			if (AutoProcessLMData != 4) {
				fclose(TempFiles[i]);
				sprintf(TempFileName, "_TempFile_Module_%hu", i);
				if(!(TempFiles[i] = fopen(TempFileName, "r"))) {
					sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Events): can't open temporary file", TempFileName);
					Pixie_Print_MSG(ErrMSG,1);
				}
				while (!feof(TempFiles[i])) putc(getc(TempFiles[i]), OutputFile);
				fclose(TempFiles[i]);
				remove(TempFileName);
			}
		}
	}
    
    /* Close output file */
    if (AutoProcessLMData > 0) fclose(OutputFile);
	if (AutoProcessLMData == 4) free_array (DT4Data);
    
    return(0);
    
}

/****************************************************************
 *	Pixie_Locate_List_Mode_Traces function:
 *		Parse the list mode file and locate the starting point,
 *		length, energy of each trace.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL pointer *ModuleTraces
 *			-4 - Run type is not 0x100
 *
 ****************************************************************/

S32 PixieLocateListModeTracesChannelLevel (LMR_t LMA) 
{
    
    /* This is a channel-level user function for the list mode reader */
    U16  ModNum         = LMA->BufferHeader[1];
    U32 *ModuleTraces   = (U32*)LMA->par0;
    U32 *ShiftFromStart = (U32*)LMA->par1;
    U32  TraceLen       = (U32)(LMA->ChannelHeader[0] - LMA->ChanHeadLen);
    U32  Energy         = (U32) LMA->ChannelHeader[2];
    U32  TracePos       = (U32)(Pixie_ftell(LMA->ListModeFile) + 1) / 2 - TraceLen;

    ModuleTraces[3*(ShiftFromStart[ModNum]+LMA->Traces[ModNum])+0] = TracePos;
    ModuleTraces[3*(ShiftFromStart[ModNum]+LMA->Traces[ModNum])+1] = TraceLen; 
    ModuleTraces[3*(ShiftFromStart[ModNum]+LMA->Traces[ModNum])+2] = Energy;
    
    return (0);
    
}

S32 Pixie_Locate_List_Mode_Traces (S8  *filename,	// the list mode data file name (with complete path)
			           U32 *ModuleTraces)	// receives trace length and location for each module
{
    
    S32                status                             =  0;
    S32                i                                  =  0;
    U32                ShiftFromStart[PRESET_MAX_MODULES] = {0};
    U32                TotalShift                         =  0;
    
    /* Check if pointer *ModuleEvents has been initialized */
    if(!ModuleTraces) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Traces): NULL pointer *ModuleTraces");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
    
    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA)); 
    
    /* Prepare the array of module-dependent shifts */
    for(i = 1; i < PRESET_MAX_MODULES; i++) {
	ShiftFromStart[i] = (TotalShift += MODULE_EVENTS[i+PRESET_MAX_MODULES-1]);
    }
    
    /* Determine the run type reading only the first buffer */
    LMA->ListModeFileName      = filename;
    LMA->ReadFirstBufferHeader = 1;
    if (PixieListModeReader(LMA) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Traces): problems opening list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    
    /* If run type is not 0x100 then there is no reason to do any processing */
    if ((LMA->BufferHeader[2] & 0x0FFF) != 0x100) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Parse_List_Mode_Traces): wrong run type");
	Pixie_Print_MSG(ErrMSG,1);
	return (-4);
    }

    /* Do the processing */
    LMA->ReadFirstBufferHeader = 0;
    LMA->par0                  = ModuleTraces;
    LMA->par1                  = ShiftFromStart;
    LMA->ChannelLevelAction    = PixieLocateListModeTracesChannelLevel;
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Traces): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Traces): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }
    
    return(0);
    
}


/****************************************************************
 *	Pixie_Read_List_Mode_Traces function:
 *		Read specfic trace events from the list mode file.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *
 ****************************************************************/

S32 Pixie_Read_List_Mode_Traces (S8  *filename,		// the list mode data file name (with complete path)
				 U32 *ListModeTraces )	// receives list mode trace data
{
	U16 i, idx;
	U32 j;
	FILE  *ListModeFile = NULL;
	
	/* Open the list mode file */
	ListModeFile = fopen(filename, "rb");
	if(ListModeFile != NULL) {
		idx = NUMBER_OF_CHANNELS * 2;
		/* Read list mode traces from the file */
		for( i = 0; i < NUMBER_OF_CHANNELS; i ++ ) {
			if( ( ListModeTraces[i*2] != 0 ) && (ListModeTraces[i*2+1] != 0 ) ) {
				/* Position ListModeFile to the requested trace location */
				Pixie_fseek(ListModeFile, ListModeTraces[i*2]*2, SEEK_SET);
				/* Read trace */
				for(j=0; j<ListModeTraces[i*2+1]; j++) {
					fread(&ListModeTraces[idx++], 2, 1, ListModeFile);
				}
			}
		}
		fclose(ListModeFile);
	}
	else {
		sprintf(ErrMSG, "*ERROR* (Pixie_Read_List_Mode_Traces): can't open list mode file %s", filename);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}

	return(0);
}


/****************************************************************
 *	Pixie_Read_Energies function:
 *		Read energy values from the list mode file for a Pixie module.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL EventEnergies pointer
 *
 ****************************************************************/
 
S32 PixieReadEnergiesChannelLevel (LMR_t LMA) 
{
    /* This is a channel-level user function for the list mode reader */
    U8  *ModNum        = (U8*) LMA->par1;
    U16  RunTask       =       LMA->BufferHeader[2] & 0x0FFF;
    U32 *EventEnergies = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		if (RunTask > 0x101) EventEnergies[TraceNum] = LMA->ChannelHeader[1];
		else                 EventEnergies[TraceNum] = LMA->ChannelHeader[2];
	}

    return (0);  
} 

S32 PixieReadEnergiesAuxChannelLevel (LMR_t LMA) 
{

    /* This is an auxiliary channel-level user function for the list mode reader */
    U8  *ModNum        = (U8*) LMA->par1;
    U32 *EventEnergies = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		EventEnergies[TraceNum] = 0;
	}
    return (0);
    
} 

S32 Pixie_Read_Energies (S8  *filename,			// list mode data file name
			 U32 *EventEnergies,		// receive event energy values
			 U8   ModNum )			// Pixie module number
{

    int status = 0;
    
    /* Check if pointer *ModuleEvents has been initialized */
    if(!EventEnergies) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Energies): NULL pointer *EventEnergies");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
    
    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA)); 

    /* Do the processing */
    LMA->ListModeFileName   = filename;
    LMA->par0               = EventEnergies;
    LMA->par1               = &ModNum;
    LMA->ChannelLevelAction = PixieReadEnergiesChannelLevel;
	LMA->AuxChannelLevelAction = PixieReadEnergiesAuxChannelLevel;
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Energies): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Energies): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }

    return (0);
    
}

/****************************************************************
 *	Pixie_Read_Event_PSA function:
 *		Read event PSA values from the list mode file for a Pixie module.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can not open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL EventPSA array pointer
 *			-4 - wrong run type
 *
 ****************************************************************/
 
S32 PixieReadEventPSAChannelLevel (LMR_t LMA) 
{
    /* This is a channel-level user function for the list mode reader */
    U8  *ModNum   = (U8*) LMA->par1;
    U16  RunTask  =       LMA->BufferHeader[2] & 0x0FFF;
    U32 *EventPSA = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		if (RunTask == 0x100 || RunTask == 101) {
			EventPSA[2*TraceNum+0] = LMA->ChannelHeader[3]; /* XIA PSA */
			EventPSA[2*TraceNum+1] = LMA->ChannelHeader[4]; /* User PSA */
		}
		if (RunTask == 0x102) {
			EventPSA[2*TraceNum+0] = LMA->ChannelHeader[2]; /* XIA PSA */
			EventPSA[2*TraceNum+1] = LMA->ChannelHeader[3]; /* User PSA */
		}
	}
    
    return (0);    
} 

S32 PixieReadEventPSAAuxChannelLevel (LMR_t LMA) 
{
    /* This is a channel-level user function for the list mode reader */
    U8  *ModNum   = (U8*) LMA->par1;
    U32 *EventPSA = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		EventPSA[2*TraceNum+0] = 0; /* XIA PSA */
		EventPSA[2*TraceNum+1] = 0; /* User PSA */
	}

    return (0);   
} 

S32 Pixie_Read_Event_PSA (S8  *filename,	// list mode data file name
			  U32 *EventPSA,	// receive event PSA values
			  U8   ModNum )		// Pixie module number
{

    int status;
    
    /* Check if pointer *ModuleEvents has been initialized */
    if(!EventPSA) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Event_PSA): NULL pointer *EventPSA");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
    
    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA));

    /* Determine the run type reading only the first buffer */
    LMA->ListModeFileName      = filename;
    LMA->ReadFirstBufferHeader = 1;
    if (PixieListModeReader(LMA) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Event_PSA): problem opening list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    
    /* If run type is higher than 0x102 then there is no reason to do any processing */
    if ((LMA->BufferHeader[2] & 0x0FFF) > 0x102) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Event_PSA): wrong run type");
	Pixie_Print_MSG(ErrMSG,1);
	return (-4);
    }

    /* Do the processing */
    LMA->ReadFirstBufferHeader = 0;
    LMA->par0                  = EventPSA;
    LMA->par1                  = &ModNum;
    LMA->ChannelLevelAction    = PixieReadEventPSAChannelLevel;
	LMA->AuxChannelLevelAction = PixieReadEventPSAAuxChannelLevel;
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Event_PSA): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Event_PSA): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }

    return (0);
    
}

/****************************************************************
 *	Pixie_Read_Long_Event_PSA function:
 *		Read the extended event PSA values from the list mode file for a Pixie module:
 *		- timestamp
 *      - energy
 *      - XIA_PSA
 *		- User_PSA
 *      - User_2
 *		- User_3
 *      - User_4
 *		- User_5
 *	 
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can not open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL EventPSA array pointer
 *			-4 - wrong run type
 *
 ****************************************************************/

S32 PixieReadLongEventPSAChannelLevel (LMR_t LMA) 
{
    /* This is a channel-level user function for the list mode reader */
	U8  *ModNum   = (U8*) LMA->par1;
    U32 *EventPSA = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		EventPSA[8*TraceNum+0] = LMA->ChannelHeader[1]; /* Time stamp */
		EventPSA[8*TraceNum+1] = LMA->ChannelHeader[2]; /* Energy */
		EventPSA[8*TraceNum+2] = LMA->ChannelHeader[3]; /* XIA PSA */
		EventPSA[8*TraceNum+3] = LMA->ChannelHeader[4]; /* User PSA */
		EventPSA[8*TraceNum+4] = LMA->ChannelHeader[5]; /* User 2 */
		EventPSA[8*TraceNum+5] = LMA->ChannelHeader[6]; /* User 3 */
		EventPSA[8*TraceNum+6] = LMA->ChannelHeader[7]; /* User 4 */
		EventPSA[8*TraceNum+7] = LMA->ChannelHeader[8]; /* User 5 */
	}
    return (0);
} 

S32 PixieReadLongEventPSAAuxChannelLevel (LMR_t LMA) 
{
    /* This is an auxiliary channel-level user function for the list mode reader */
    U8  *ModNum   = (U8*) LMA->par1;
    U32 *EventPSA = (U32*)LMA->par0;
	U32  TraceNum = 4*LMA->Events[*ModNum] + (U32)LMA->Channel;
    
	if (LMA->BufferHeader[1] == (U16)*ModNum) { // Only events for this module
		EventPSA[8*TraceNum+0] = 0; /* Time stamp */
		EventPSA[8*TraceNum+1] = 0; /* Energy */
		EventPSA[8*TraceNum+2] = 0; /* XIA PSA */
		EventPSA[8*TraceNum+3] = 0; /* User PSA */
		EventPSA[8*TraceNum+4] = 0; /* User 2 */
		EventPSA[8*TraceNum+5] = 0; /* User 3 */
		EventPSA[8*TraceNum+6] = 0; /* User 4 */
		EventPSA[8*TraceNum+7] = 0; /* User 5 */
	}

    return (0);   
} 

S32 Pixie_Read_Long_Event_PSA (S8  *filename,			// list mode data file name
			       U32 *EventPSA,			// receive event PSA values
			       U8   ModNum )			// Pixie module number
{

    int status = 0;
    
    /* Check if pointer *ModuleEvents has been initialized */
    if(!EventPSA) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Long_Event_PSA): NULL pointer *EventPSA");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }

    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA)); 

    /* Determine the run type reading only the first buffer */
    LMA->ListModeFileName      = filename;
    LMA->ReadFirstBufferHeader = 1;
    if (PixieListModeReader(LMA) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Long_Event_PSA): problem opening list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    
    /* If run type is not 0x100 or 0x101 then there is no reason to do any processing */
    if ((LMA->BufferHeader[2] & 0x0FFF) > 0x101) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Long_Event_PSA): wrong run type");
	Pixie_Print_MSG(ErrMSG,1);
	return (-4);
    }

    /* Do the processing */
    LMA->ReadFirstBufferHeader = 0;
    LMA->par0                  = EventPSA;
    LMA->par1                  = &ModNum;
    LMA->ChannelLevelAction    = PixieReadLongEventPSAChannelLevel;
	LMA->AuxChannelLevelAction = PixieReadLongEventPSAAuxChannelLevel;
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Long_Event_PSA): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Read_Long_Event_PSA): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }

    return (0);
    
}

/****************************************************************
 *	Pixie_Locate_List_Mode_Events function:
 *		Parse the list mode file and find 
 *                              every event start, 
 *                              corresponding buffer start and 
 *                              number of words in the event
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL ModuleTraces array pointer
 *
 ****************************************************************/

S32 PixieLocateListModeEventsBufferLevel (LMR_t LMA) 
{
    
    /* This is a buffer-level user function for the list mode reader */
    /* Buffer start */
    *(S32*)LMA->par2 = (Pixie_ftell(LMA->ListModeFile) + 1) / 2 - BUFFER_HEAD_LENGTH;
    
    return (0);
    
} 

S32 PixieLocateListModeEventsEventLevel (LMR_t LMA) 
{
    
    /* This is a event-level user function for the list mode reader */
    U16  ModNum         =        LMA->BufferHeader[1];
    U16 *LastChannel    =  (U16*)LMA->par4;
    U32 *ModuleEvents   =  (U32*)LMA->par0;
    U32 *ShiftFromStart =  (U32*)LMA->par1;
    U32 *EventLen       =  (U32*)LMA->par3;
    S32  BufferPos      = *(S32*)LMA->par2;
    S32  EventPos       =       (Pixie_ftell(LMA->ListModeFile) + 1) / 2 - EVENT_HEAD_LENGTH;
    
    if (LMA->EventHeader[0] & 0x1) *LastChannel = 0;
    if (LMA->EventHeader[0] & 0x2) *LastChannel = 1;
    if (LMA->EventHeader[0] & 0x4) *LastChannel = 2;
    if (LMA->EventHeader[0] & 0x8) *LastChannel = 3;
    
    *EventLen = EVENT_HEAD_LENGTH; /* Initialize EventLen to event buffer length */
    ModuleEvents[3*(ShiftFromStart[ModNum]+LMA->Events[ModNum])+0] = (U32)EventPos;
    ModuleEvents[3*(ShiftFromStart[ModNum]+LMA->Events[ModNum])+1] = (U32)BufferPos; 
    
    return (0);
    
} 

S32 PixieLocateListModeEventsChannelLevel (LMR_t LMA) 
{
    
    /* This is a channel-level user function for the list mode reader */
    U16  ModNum         =        LMA->BufferHeader[1];
    U16  RunType        =        LMA->BufferHeader[2] & 0x0F0F;
    U16  LastChannel    = *(U16*)LMA->par4;
    U32 *ModuleEvents   =  (U32*)LMA->par0;
    U32 *ShiftFromStart =  (U32*)LMA->par1;
    U32 *EventLen       =  (U32*)LMA->par3;
    
    if (RunType == 0x100) *EventLen += (U32)LMA->ChannelHeader[0]; /* Sum channel lengths to get event length */
    else                  *EventLen += (U32)LMA->ChanHeadLen;
    if (LMA->Channel == LastChannel) {              /* If no more channels to sum report event length */
	ModuleEvents[3*(ShiftFromStart[ModNum]+LMA->Events[ModNum])+2] = *EventLen;
    }
    
    return (0);
    
} 

S32 Pixie_Locate_List_Mode_Events (S8  *filename,		// the list mode data file name (with complete path)
			           U32 *ModuleEvents)	// receives event length and location for each module
{

    U16                LastChannel                        =  0;
    U32                EventLen                           =  0;
    U32                ShiftFromStart[PRESET_MAX_MODULES] = {0};
    U32                TotalShift                         =  0;
    S32                BufferPos                          =  0;
    S32                status                             =  0;
    S32                i                                  =  0;
    
    /* Check if pointer *ModuleEvents has been initialized */
    if(!ModuleEvents) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Events): NULL pointer *EventPSA");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
    
    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA)); 
    
    /* Prepare the array of module-dependent shifts */
    for(i = 1; i < PRESET_MAX_MODULES; i++) ShiftFromStart[i] = (TotalShift += MODULE_EVENTS[i-1]);

    /* Do the processing */
    LMA->ListModeFileName      = filename;
    LMA->par0                  = ModuleEvents;
    LMA->par1                  = ShiftFromStart;
    LMA->par2                  = &BufferPos;
    LMA->par3                  = &EventLen;
    LMA->par4                  = &LastChannel;
    LMA->BufferLevelAction     = PixieLocateListModeEventsBufferLevel;
    LMA->EventLevelAction      = PixieLocateListModeEventsEventLevel;
    LMA->ChannelLevelAction    = PixieLocateListModeEventsChannelLevel;
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Events): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Events): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }

    return (0);
    
}


/****************************************************************
 *	Pixie_User_List_Mode_Reader function:
 *		Parse the list mode file and conduct 
 *      any user defined analysis using six action  
 *      functions of the list mode reader. The user is 
 *      responsible to populate the six functions as well as 
 *      the calling function with the code and recompile.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *			-2 - found BufNdata = 0
 *			-3 - NULL UserData array pointer
 *
 ****************************************************************/

S32 PixieUserListModeReaderPreProcess (LMR_t LMA) 
{
	/* This function can be used to conduct any computation before a
	 * list mode file parsing begins. In this example the array UserData 
	 * is initialized to zero */
	 
	U32 *UserData     =  (U32*)LMA->par2;
	U32  UserDataSize = *(U32*)LMA->par3 ;
	memset(UserData, 0, sizeof(U32) * UserDataSize);
    return (0);
} 

S32 PixieUserListModeReaderBufferLevel (LMR_t LMA) 
{
	/* This function can be used to conduct any computation right after
	 * a buffer header has been read into LMA->BufferHeader[]. In this 
	 * example the position of the buffer start is determined and kept 
	 * in the fourth parameter */
	 
	*(S32*)LMA->par4 = (Pixie_ftell(LMA->ListModeFile) + 1) / 2 - BUFFER_HEAD_LENGTH;
    return (0);
} 

S32 PixieUserListModeReaderEventLevel (LMR_t LMA) 
{
	/* This function can be used to conduct any computation right after
	 * an event header has been read into LMA->EventHeader[]. In this 
	 * example UserVariable is incremented by one if all channels trigger
	 * simultaneously */
	
	U16    HitPattern   = LMA->EventHeader[0];
	double UserVariable = *(double *)LMA->par1;
	if (HitPattern & 0x0F00) UserVariable += 1.0;
    return (0);
} 

S32 PixieUserListModeReaderChannelLevel (LMR_t LMA) 
{
	/* This function can be used to conduct any computation right after
	 * a channel header has been read into LMA->ChannelHeader[]. Only 
	 * channels defined in the read pattern of an event (bits 3-0 of LMA->EventHeader[0])
	 * are considered. If a certain action is required for a channel not in the
     * read pattern	the user must use the function PixieUserListModeReaderAuxChannelLevel().
	 * In this example the first element of the UserData array is populated with 
	 * different channel header values depending on the run type */
	
	U16  RunType  =       LMA->BufferHeader[2] & 0x0FFF;
    U32 *UserData = (U32*)LMA->par2;
    if (RunType == 0x100) UserData[0] = (U32)LMA->ChannelHeader[3]; /* XIA PSA */
    if (RunType == 0x102) UserData[0] = (U32)LMA->ChannelHeader[2]; /* XIA PSA */
    return (0);
} 

S32 PixieUserListModeReaderAuxChannelLevel (LMR_t LMA) 
{
	/* This function can be used to conduct any computation for channels
	 * which are not defined in the read pattern. In this example the first 
	 * element of the UserData array is initiated to zero */
	
	U32 *UserData = (U32*)LMA->par2;
    UserData[0] = 0;
    return (0);
} 

S32 PixieUserListModeReaderPostProcess (LMR_t LMA)
{
	/* This function can be used to conduct any computation after
	 * a list mode file parsing is over. In this example total event 
	 * and trace tallies are printed to the screen */
	 
	S8 MSG[256] = {'\0'};
	sprintf(MSG, "Total Number of Events: %u\n", LMA->TotalEvents);
	Pixie_Print_MSG(ErrMSG,1);
	sprintf(MSG, "Total Number of Traces: %u\n", LMA->TotalTraces);
	Pixie_Print_MSG(ErrMSG,1);
    return (0);
} 

S32 Pixie_User_List_Mode_Reader (S8  *filename,		// the list mode data file name (with complete path)
			           U32 *UserData)	// receives event length and location for each module
{
	U32    UserDataSize = *UserData; /* The user must provide the size of the array in the zeroth element */
	S32    status       = 0;
    double UserVariable = 2.718281828459045;
	double UserArray[5] = {3.141592653589793};
	
    /* Check if pointer has been initialized */
    if(!UserData) {
	sprintf(ErrMSG, "*ERROR* (Pixie_User_List_Mode_Reader): NULL pointer *UserData");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
    /* Make sure the size of the user array is not zero */
    if(!UserDataSize) {
	sprintf(ErrMSG, "*ERROR* (Pixie_User_List_Mode_Reader): UserDataSize is zero");
	Pixie_Print_MSG(ErrMSG,1);
	return(-3);
    }
	
    /* Zero the list mode reader arguments structure */
    memset(LMA, 0, sizeof(*LMA)); 

    /* Do the processing */
    LMA->ListModeFileName      = filename;
    LMA->par0                  = UserArray;
    LMA->par1                  = &UserVariable;
    LMA->par2                  = UserData;
    LMA->par3                  = &UserDataSize;
    LMA->PreAnalysisAction     = PixieUserListModeReaderPreProcess(LMA);
    LMA->BufferLevelAction     = PixieUserListModeReaderBufferLevel(LMA);
    LMA->EventLevelAction      = PixieUserListModeReaderEventLevel(LMA);
    LMA->ChannelLevelAction    = PixieUserListModeReaderChannelLevel(LMA);
    LMA->AuxChannelLevelAction = PixieUserListModeReaderAuxChannelLevel(LMA);
    LMA->PostAnalysisAction    = PixieUserListModeReaderPostProcess(LMA);
    if ((status = PixieListModeReader(LMA)) == -1) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Events): Cannot open list mode file");
	Pixie_Print_MSG(ErrMSG,1);
	return (-1);
    }
    if (status == -2) {
	sprintf(ErrMSG, "*ERROR* (Pixie_Locate_List_Mode_Events): Corrupted file: module# %hu, buffer# %u", LMA->BufferHeader[1], LMA->Buffers[LMA->BufferHeader[1]]);
	Pixie_Print_MSG(ErrMSG,1);
	return (-2);
    }

    return (0);   
}





/****************************************************************
 *	Pixie_Read_List_Mode_Events function:
 *		Read specfic event header, channel headers and trace from the list mode file.
 *
 *		Return Value:
 *			 0 - success
 *			-1 - can't open list mode data file
 *
 ****************************************************************/

S32 Pixie_Read_List_Mode_Events (
			S8  *filename,		// the list mode data file name (with complete path)
			U32 *ListModeTraces )	// receives list mode trace data
{						// first three words contain location of event, location of buffer, and length of event
	U16 idx, RunTask, traceindex;
	U16 EvtPattern, ChanNData, chl;
	U32 j,k;
	FILE  *ListModeFile = NULL;
	
	/* Open the list mode file */
	ListModeFile = fopen(filename, "rb");

	ListModeTraces[3] = 0;	// initialize tracelengths -- word 3 is now also an input parameter
	ListModeTraces[4] = 0;	// initialize tracelengths 
	ListModeTraces[5] = 0;	// initialize tracelengths 
	ListModeTraces[6] = 0;	// initialize tracelengths 
	
	if(ListModeFile != NULL) {
	    /* Read list mode traces from the file */
	    if((ListModeTraces[0] != 0 ) && (ListModeTraces[2] != 0)) {
        	idx = 7;	// buffer header is written starting from loc. 7
			/* Position ListModeFile to the beginning of this buffer */
			Pixie_fseek(ListModeFile, ListModeTraces[1]*2, SEEK_SET);
			/* Read buffer header */
			for(j=0; j<BUFFER_HEAD_LENGTH; j++) 
				fread(&ListModeTraces[idx++], 2, 1, ListModeFile);
			
			/* Determine Run Task */				
			RunTask = (U16)(ListModeTraces[9] & 0x0FFF);			
			/* Determine Channel Header Length */
			chl=9;	//default
			switch(RunTask) {
				case 0x100:
				case 0x101:
				case 0x200:
				case 0x201:
					chl = 9;
					break;
				case 0x102:
				case 0x202:
					chl = 4;
					break;
				case 0x103:
				case 0x203:
					chl = 2;
					break;
				default:
					break;
			}
        	
			/* Position ListModeFile to the requested event location */
			Pixie_fseek(ListModeFile, ListModeTraces[0]*2, SEEK_SET);	
			/* Read event header */
			for(j=0; j<EVENT_HEAD_LENGTH; j++) 
				fread(&ListModeTraces[idx++], 2, 1, ListModeFile);

       		EvtPattern = (U16)ListModeTraces[13];

			/* position trace write pointer to channel j header entry */
      		traceindex = 7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+9*NUMBER_OF_CHANNELS;
   			for(j = 0; j < NUMBER_OF_CHANNELS; j ++) {
				if( TstBit((U16)j, EvtPattern) ) {
					/*position write pointer to channel j header entry */
					/* idx = 7+BUFFER_HEAD_LENGTH+EVENT_HEAD_LENGTH+9*j; */
					if( chl == 9 ) {
						/* Read channel header */
						for(k=0; k<chl; k++) 
							fread(&ListModeTraces[idx++], 2, 1, ListModeFile);

						// if traces, read and store
        				ChanNData = (U16)ListModeTraces[idx-chl];
        				if ( ChanNData > chl) {
            				/* store traces */
            				for(k=0; k<(U16)(ChanNData - chl); k++) 
								fread(&ListModeTraces[traceindex++], 2, 1, ListModeFile);
            				/* store tracelength */
            				ListModeTraces[3+j] = ChanNData-chl;
						}
					}
					else {
                		/* for easier processing, store the channel header length */
                		ListModeTraces[idx++] = chl;  
    					
						/* Read channel header */
						for(k=0; k<chl; k++) 
							fread(&ListModeTraces[idx++], 2, 1, ListModeFile);
                		/* store tracelength */
                		ListModeTraces[3+j] = 0;	// report zero tracelength for channels without trace		
                		idx += (9-1-chl);    
					}
				}
				else {
					idx += 9;	// skip the channel header if channel not present
				}
			}	// endfor (traces)
			fclose(ListModeFile);
	    }
		else {
			sprintf(ErrMSG, "*ERROR* (Pixie_Read_List_Mode_Events): UserData contains no valid locastion information");
			Pixie_Print_MSG(ErrMSG,1);
			fclose(ListModeFile);
			return(-1);
		}	    
	}
	else {
	    sprintf(ErrMSG, "*ERROR* (Pixie_Read_List_Mode_Events): can't open list mode file %s", filename);
	    Pixie_Print_MSG(ErrMSG,1);
	    return(-1);
	}
	if((ListModeTraces[9] & 0xF000) == 0x4000)
		ListModeTraces[1] = 500;	// Pixie-500: module ID 4 (500 MHz)
	else
		ListModeTraces[1] = 75;		// default Pixie-4 (75 MHz)
	return(0);
}



