#ifndef __PIXIE4_IFACE_H
#define __PIXIE4_IFACE_H

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
 *     pixie4_iface.h
 *
 * Description:
 *
 *     Pixie-4 Igor inteface header
 *
 * Revision:
 *
 *     3-11-2004
 *
 ******************************************************************************/


/* If this is compiled by a C++ compiler, make it */
/* clear that these are C routines.               */
#ifdef __cplusplus
extern "C" {
#endif


#if !defined(S32)
    typedef signed long            S32;
#endif

#define REQUIRES_IGOR_200 1 + FIRST_XOP_ERR

/* Prototypes */

S32 Pixie4_Hand_Down_Names (
		struct {
			Handle Name;		// a string indicating which type of names to be downloaded
			waveHndl Names;		// a two dimensional string array
			double result;		// return value
		}* p );


S32 Pixie4_Boot_System (
		struct {
			double Boot_Pattern;	// Pixie-4 boot patten
			double result;			// return value
		}* p );


S32 Pixie4_User_Par_IO (
		struct {
			double ChanNum;					// channel number of the Pixie module
			double ModNum;					// number of the module to work on
			double direction;				// transfer direction (read or write)
			Handle User_Par_Type;			// user parameter type 
			Handle User_Par_Name;			// user parameter name
			waveHndl User_Par_Values;		// user parameters to be transferred
			double result;					// return value
		}* p );


S32 Pixie4_Acquire_Data (
		struct {
			double ModNum;			// Pixie module number
			Handle file_name;		// file name
			waveHndl User_data;		// array holding transferred data
			double Run_Type;		// data acquisition type
			double result;			// return value
		}* p );


S32 Pixie4_Set_Current_ModChan (
		struct {
			double Module;
			double Channel;
			double result;
	}* p );


S32 Pixie4_Buffer_IO (
		struct {
			double ModNum;			// number of the module to work on 
			Handle file_name;		// settings file name
			double direction;		// I/O direction
			double type;			// I/O type
			waveHndl Values;		// parameter values
			double result;			// return value
		}* p );


#ifdef __cplusplus
}
#endif

#endif	/* End of pixie4_iface.h */
