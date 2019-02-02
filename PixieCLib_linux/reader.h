#ifndef __READER_H
#define __READER_H

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
 *     reader.h
 *
 * Description:
 *
 *     Definitions of global structure for the list mode reader
 *
 * Revision:
 *
 *     12-14-2012
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

#include "globals.h"

/* This structure is used to read and analyze list mode data files.
 * It contains a number of variables and pointers necessary to create
 * an arbitrary analysis application making use of data in Pixie list 
 * mode files. */

 struct LMReaderDataStruct {
    void  *par0; /* These void parameter pointers can carry  */
    void  *par1; /* any variable, array, structure or function */
    void  *par2; /* necessary for a particular analysis */
    void  *par3;
    void  *par4;
    void  *par5;
    void  *par6;
    void  *par7;
    void  *par8;
    void  *par9;
    FILE  *ListModeFile;	/* List mode file pointer used in file handling routines */
    U8    *ListModeFileName;	/* List mode file name pointer */
	FILE  *OutputFile; 
	U8	  OutputFileName[256];
	U16    FirstHeader   [FIRST_HEAD_LENGTH]; /* An array to read the first header of the list mode file for the file definitiono purposes */
	U16    RunHeader   [RUN_HEAD_LENGTH];/*
    
						RunHeader[0]:     Block size in 16-bit words 
						RunHeader[1]:     Module Number     
						RunHeader[2]:     Run Type
						RunHeader[3]		 Channel Header Length					 */
	U16    BufferHeader    [BUFFER_HEAD_LENGTH]; 
										/*  BufferHeader[0]: Buffer Length
											BufferHeader[1]: Module Number
											BufferHeader[2]: Run Type
											BufferHeader[3]: Run Start High Word 
											BufferHeader[4]: Run Start Middle Word  
											BufferHeader[5]: Run Start Low Word
											 */
					     
	U16    EventHeader     [EVENT_HEAD_LENGTH];   
										/* EventHeader[0]: Hit Pattern
											EventHeader[1]: Event Time High Word
											EventHeader[2]: Event Time Low Word  
											 */
					     
	U16    ChannelHeader   [MAX_CHAN_HEAD_LENGTH];
	/* (Types 0x100 and 0x101)                                           (Type 0x102)        (Type 0x103) for Pixie-4
    
						ChannelHeader[0]:     Channel Length         Fast Trigger Time   Fast Trigger Time
						ChannelHeader[1]:     Trigger Time              Energy                     Energy
						ChannelHeader[2]:     Energy                       XIA PSA Value
						ChannelHeader[3]:     XIA PSA Value          User PSA Value
						ChannelHeader[4]:     User PSA Value
						ChannelHeader[5]:     Unused
						ChannelHeader[6]:     Unused
						ChannelHeader[7]:     Unused
						ChannelHeader[8]:     Real Time High Word

		 (Types 0x400 and 0x401)      (Type 0x402)        (Type 0x403) for Pixie-500 Express
    
					ChannelHeader[0]	Event status info e.g. backplane lines
					ChannelHeader[1]	reserved
					ChannelHeader[2]	Number of blocks of Trace data to follow
					ChannelHeader[3]	Number of blocks of the previous Trace data
					ChannelHeader[4]	Lower 16 bits  of Trigger time 
					ChannelHeader[5]	Middle 16 bits of Trigger time
					ChannelHeader[6]	Upper 16 bits of Trigger time
					ChannelHeader[7]	Reserved
					ChannelHeader[8]	Energy
					ChannelHeader[9]	Channel number 
					ChannelHeader[10]	User PSA value
					ChannelHeader[11]	XIA PSA value
					ChannelHeader[12	- 32]  Unused/Reserved

											 */
    U16    Trace[MAX_TRACE_LENGTH];		/* An array to contain individual trace data */
    U16    Channel;				/* Current channel number when reading an event */
    U16    ChanHeadLen;                 	/* Length of the channel header for the current run type */
	U16    ReadFirstBufferHeader;
	U16	   ADCrate;
    U32    Buffers[PRESET_MAX_MODULES];		/* Buffer counters for every module */
    U32    Events [PRESET_MAX_MODULES];		/* Event  counters for every module */
    U32    Traces [PRESET_MAX_MODULES];		/* Trace  counters for every module */
    U32    TotalBuffers;			/* Buffer counter for all modules */
    U32    TotalEvents;				/* Event  counter for all modules */
    U32    TotalTraces;    			/* Trace  counter for all modules */
	U32	   BadEvent;
#ifdef	COMPILE_TOOL_MINGW32
    S32  (*PreAnalysisAction) (LMReaderDataStruct *); /* Pointer to the user function containing logic before file parsing starts */
    S32  (*BufferLevelAction) (LMReaderDataStruct *);	/* Pointer to the user function containing logic at the buffer  level */
    S32  (*EventLevelAction)  (LMReaderDataStruct *);	/* Pointer to the user function containing logic at the event   level */
    S32  (*ChannelLevelAction) (LMReaderDataStruct *);	/* Pointer to the user function containing logic at the channel level */
    S32  (*AuxChannelLevelAction) (LMReaderDataStruct *); /* Pointer to the user function containing logic for a channel not in the read pattern */
    S32  (*PostAnalysisAction) (LMReaderDataStruct *);	/* Pointer to the user function containing logic after file parsing ends */
#else
    S32  (*PreAnalysisAction) (); /* Pointer to the user function containing logic before file parsing starts */
    S32  (*BufferLevelAction) ();	/* Pointer to the user function containing logic at the buffer  level */
    S32  (*EventLevelAction)  ();	/* Pointer to the user function containing logic at the event   level */
    S32  (*ChannelLevelAction) ();	/* Pointer to the user function containing logic at the channel level */
	S32  (*AuxChannelLevelAction) (); /* Pointer to the user function containing logic for a channel not in the read pattern */
    S32  (*PostAnalysisAction) ();	/* Pointer to the user function containing logic after file parsing ends */
#endif
};

typedef struct LMReaderDataStruct * LMR_t;

 struct P500E_ListModeFormatStruct {
	// common
	U16		*BlockSize;				/* RunHeader[0]: Block size in 16-bit words */ 
	U16		*ModNum; 				/* RunHeader[1]: Module Number */     
	U16		*RunType;				/* RunHeader[2]: Run Type */
	U16		*ChanHeadLen;			/* RunHeader[3]: Channel Header Length */
	U16		*SumChanLen;			/* RunHeader[6]: Sum of channel lengths */
	U16		*BoardVersion;			/* RunHeader[7]: Board Version */
	U16		*ADCrate;				/* ADC rate extracted from RunHeader[7]: Board Version */
	U16		*ChanLen0;				/* RunHeader[8]: length (header + trace) of ch.0 */
	U16		*ChanLen1;				/* RunHeader[9]: length (header + trace) of ch.0 */
	U16		*ChanLen2;				/* RunHeader[10]: length (header + trace) of ch.0 */
	U16		*ChanLen3;				/* RunHeader[11]: length (header + trace) of ch.0 */
	U16		*EvtPattern;			/* ChannelHeader[0] */
	U16		*EvtInfo;				/* ChannelHeader[1] */
	U16		*NumTraceBlks;			/* ChannelHeader[2] */
	U16		*NumTraceBlksPrev;		/* ChannelHeader[3] */
	U16		*CheckSum0;				/* ChannelHeader[28] */
	U16		*CheckSum1;				/* ChannelHeader[29] */
	U16		*WaterMark0;			/* ChannelHeader[30] */
	U16		*WaterMark1;			/* ChannelHeader[31] */
	// run type 0x400
	U16		*TrigTimeLO;			/* ChannelHeader[4] */
	U16		*TrigTimeMI;			/* ChannelHeader[5] */
	U16		*TrigTimeHI;			/* ChannelHeader[6] */
	U16		*TrigTimeX;				/* ChannelHeader[7] */
	U16		*Energy;				/* ChannelHeader[8] */
	U16		*ChanNo;				/* ChannelHeader[9] */
	U16		*UserPSA;				/* ChannelHeader[10] */
	U16		*XIAPSA;				/* ChannelHeader[11] */
	U16		*ExtendedPSA0;			/* ChannelHeader[12] */
	U16		*ExtendedPSA1;			/* ChannelHeader[13] */
	U16		*ExtendedPSA2;			/* ChannelHeader[14] */
	U16		*ExtendedPSA3;			/* ChannelHeader[15] */
	// run type 0x402
	U16		*Esum;				
	U16		*TrigTimeLO_0;		
	U16		*TrigTimeMI_0;		
	U16		*Energy_0;		
	U16		*NumTraceBlks_0;		
	U16		*TrigTimeLO_1;		
	U16		*TrigTimeMI_1;		
	U16		*Energy_1;			
	U16		*NumTraceBlks_1;		
	U16		*TrigTimeLO_2;		
	U16		*TrigTimeMI_2;		
	U16		*Energy_2;			
	U16		*NumTraceBlks_2;		
	U16		*TrigTimeLO_3;		
	U16		*TrigTimeMI_3;		
	U16		*Energy_3;			
	U16		*NumTraceBlks_3;		
	U16		*EvtInfo_01;			
	U16		*EvtInfo_23;			
};

typedef struct P500E_ListModeFormatStruct * P500E_t;


#ifdef __cplusplus
}
#endif	/* End of notice for C++ compilers */

#endif	/* End of reader.h */
