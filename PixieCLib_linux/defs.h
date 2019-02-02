#ifndef __DEFS_H
#define __DEFS_H


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
 *     Defs.h
 *
 * Description:
 *
 *     Constant definitions
 *
 * Revision:
 *
 *     11-30-2004
 *
 ******************************************************************************/


/* If this is compiled by a C++ compiler, make it */
/* clear that these are C routines.               */
#ifdef __cplusplus
extern "C" {
#endif

// ***********************************************************
//		Definitions required to be set by makefile
// ***********************************************************
// Set COMPILE_IGOR_XOP if compiling Igor XOP code
// Set XIA_WINDOZE for Windows
// Set XIA_LINUX for Linux. Example code only, not tested/supported by XIA
// Set COMPILE_TOOL_MINGW32 when compiling with mingw32 compiler rather than MS Visual Studio

// ***********************************************************
//		Basic data type definitions
// ***********************************************************

#ifndef PCI_CODE
	#define PCI_CODE
#endif

#ifndef LITTLE_ENDIAN			// LITTLE_ENDIAN: least significant byte stored first
	#define LITTLE_ENDIAN		// BIG_ENDIAN:    most significant byte stored first
#endif

#ifndef PI
	#define PI					3.14159265358979
#endif

// ***********************************************************
//		C Library version information
// ***********************************************************

#define C_LIBRARY_RELEASE		0x450	// C Library release number
#ifdef WI64  
	#define C_LIBRARY_BUILD			0x6402		// C Library build number
#else
	#define C_LIBRARY_BUILD			0x3202		// C Library build number
#endif

// ***********************************************************
//		PCI bus specifications
// ***********************************************************
#define	REGULAR_PCI				0xDEAD
#define	EXPRESS_PCI				0xBEEF

// ***********************************************************
//		Module Version definitions
// ***********************************************************
#define MODULETYPE_P4			0x0700	// any P4	
#define MODULETYPE_P4B			0xA701	// also 0xA721
#define MODULETYPE_P4C			0xA702
#define MODULETYPE_P4D			0xA703
#define MODULETYPE_P4E			0xA704
#define MODULETYPE_P4C2E		0xA702
#define MODULETYPE_P500e		0x0100	// any P500e
#define MODULETYPE_P500eA		0xA100
#define MODULETYPE_P500eB		0xA101
#define MODULETYPE_P4e			0x0500	// any P4e
#define MODULETYPE_P4e_16_125	0x0550	// any P4e revision, xx bit, yy MHz
#define MODULETYPE_P4e_14_125	0x0560	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_12_125	0x0570	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_16_250	0x0590	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_14_250	0x05A0	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_12_250	0x05B0	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_16_500	0x05D0	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_14_500	0x05E0	// any P4e revision, xx bit, yy MHz 
#define MODULETYPE_P4e_12_500	0x05F0	// any P4e revision, xx bit, yy MHz 

#define MODULETYPE_PN_12_250	0x0990	// any std PN revision, 12 bit, 250 MHz 
#define MODULETYPE_PN_12_250P	0x0980	// any ptp PN revision, 12 bit, 250 MHz 
#define MODULETYPE_P16			0xA912	// P16 
#define MODULETYPE_P32        0x0F00   // HiDen (not 0xAF00, because for ProgramFPGA type=MODULETYPE & 0x0FF0

	
// ***********************************************************
//		Module specifications
// ***********************************************************

#define PRESET_MAX_MODULES		17			// Preset total number of modules allowed in the system
#define PRESET_MAX_PXI_SLOTS	18			// Preset maximum number of PXI slots for one PXI chassis
#define NUMBER_OF_CHANNELS		4			// Number of channels for each module

#define GAIN_FACTOR				1			// Pixie voltage gain adjust factor
#define V_OFFSET_MAX			2.5			// Pixie voltage offset maximum
#define P500E_HIGH_GAIN			2.9			// high/low gain for Pixie-500 Rev B boards
#define P500E_HIGH_SGA			0x0001
#define P500E_LOW_GAIN			1.0			// high/low gain for Pixie-500 Rev B boards
#define P500E_LOW_SGA			0x0000
#define P4E500_HIGH_GAIN		5.0			// high/low gain for Pixie-4e 14/500 boards
#define P4E500_LOW_GAIN			2.0			// high/low gain for Pixie-4e 14/500 boards
#define DEFAULT_GAIN			1.0
#define DEFAULT_SGA				0x0000

// ***********************************************************
//		Number of variables and parameters
// ***********************************************************

#define DSP_IO_BORDER			256		// Number of DSP I/O parameters
#define N_DSP_PAR				512		// Number of all DSP parameters
#define N_MEM_PAR				16384	// Number of DSP internal memory parameters
#define N_CHANNEL_PAR			64		// Number of channel dependent parameters
#define N_MODULE_PAR			128		// Number of module dependent parameters
#define N_SYSTEM_PAR			64		// Number of system dependent parameters
#define N_SGA_GAIN				128		// Number of SGA gains
#define N_BOOT_FILES			16		// Number of boot files
#define N_USER_PAR_IO			16		// Number of parameters for custom DSP or FPGA code (input and output each)



// ***********************************************************
//		Data memory, circular and output buffer, histogram
// ***********************************************************

#define DATA_MEMORY_ADDRESS			0x4000	// DSP data memory address
#define USER_MEMORY_ADDRESS			0x41A0	// Address of user data block in DSP data memory 
#define DATA_MEMORY_LENGTH			16384	// DSP data memory length
#define USER_MEMORY_LENGTH			1100	// Length of user data block in DSP data memory 
#define EVENT_BUFFER_LENGTH			4060	// Circular buffer length
#define IO_BUFFER_ADDRESS_P500E		1792	// Address of I/O output buffer (relative to MODNUM for P500e) TODO: this should be read from module
#define IO_BUFFER_ADDRESS			24540//	// Address of I/O output buffer (absolute DSP address for P4/500) TODO: this should be read from module
#define IO_BUFFER_LENGTH			8192	// Length of I/O output buffer
#define DMA_LM_FRAMEBUFFER_LENGTH	0x200000 // Length of DMA buffer in LM runs. 2MB for Win32 and Win64.
#define RUN_HEAD_LENGTH				32		// Run header length in Pixie-500 Express list mode files
#define FIRST_HEAD_LENGTH			64		// Run header length + first event header lengthin Pixie-500 Express list mode files
#define BUFFER_HEAD_LENGTH			6		// Output buffer header length
#define EVENT_HEAD_LENGTH			3		// Event header length
#define P4_MAX_CHAN_HEAD_LENGTH		9	// Largest channel header length for Pixie-4
#define MAX_CHAN_HEAD_LENGTH		32		// Largest channel header length
#define BLOCKSIZE					32		// Number of 16-bit words per data block
#define MAX_HISTOGRAM_LENGTH		32768	// Maximum MCA histogram length
#define MAX_TRACE_LENGTH			65536	// Maximum trace length
#define UEI_OFFSET					1536	// Address of User Extra In block in DSP data memory 
#define UEO_OFFSET					1664	// Address of User Extra Out block in DSP data memory 

#define FPGA_PARAM_RAM				0x2000	// address of RAM in FPGA containing the DSP parameters (input)


#define HISTOGRAM_MEMORY_LENGTH		131072	// external histogram memory length (32bit wide)
#define HISTOGRAM_MEMORY_ADDRESS	0x0		// histogram memory start location in external memory  
#define LIST_MEMORY_LENGTH			131072	// external list mode memory length (32-bit wide)
#define LIST_MEMORY_ADDRESS			0x20000	// list mode buffer start location in external memory
#define MCA2D_MEMORY_LENGTH			262144	// external 2D spectrum memory length (32-bit wide)
#define MCA2D_MEMORY_ADDRESS		0x0		// external 2D spectrum memory start location in external memory (high bit set elsewhere)  
#define LM_DBLBUF_BLOCK_LENGTH		65536  	// length of one block in external memory in double buffer mode


#define PCI_CFDATA				  0x00		// PCI address for Data Register in Config FPGA (write only)
#define PCI_CFCTRL				  0x04		// PCI address for Control register in Config FPGA (write only)
#define PCI_CFSTATUS			  0x08		// PCI address for Status Register in Config FPGA (read only)
#define PCI_VERSION				  0x0C		// PCI address for Version Register in Config FPGA/PROM)
#define PCI_I2C				      0x10		// PCI address for I2C I/O and test bits
#define PCI_PULLUP				  0x14		// PCI address for register controlling backplane pullups (write only)
#define PCI_CSR					  0x48		// PCI address for Cnotrol/Status Register
#define PCI_WCR					  0x4C		// PCI address for Word Count Register
#define PCI_IDMAADDR			  0x80		// PCI address for IDMA address write (prior to read/write from/to DSP memory)
#define PCI_IDMADATA			  0x84		// PCI address for IDMA data (read/write from/to DSP memory)
#define PCI_SP1				      0x88		// PCI address reserved for special purposes
#define PCI_EMADDR				  0xC0		// PCI address for external memory address write (prior to read/write from/to EM)
#define PCI_EMDATA				  0x100000	// PCI address for external memory data (read/write from/to EM)
#define PCIE_EMDATA				  0x0	// PCIe address for external memory data (read/write from/to BAR2)

// ***********************************************************
//		Length of communication FPGA, Fippi, and DSP configurations
// ***********************************************************

#define N_COMFPGA_BYTES			166980	// Communication FPGA file
#define N_FIPPI_BYTES			166980	// FIPPI file
#define N_DSP_CODE_BYTES		65536	// DSP code file
#define EEPROM_MEMORY_SIZE		2048	// Memory size in bytes of P4/P500 EEPROM chip 
#define N_P4E_BYTES			    10071302// Fippi (P4E Rev A, B)
#define N_P500E_BYTES			1873114 // Fippi (P500E rev B)
#define N_P32_BYTES             9730652   // P32 configuration
#define EEPROM_XIA_OFFSET		7		// starting address for XIA content in "words"
// EEPROM content:	0-6		GN configuration
//					7		revision and serial number
//					8-11		analog channel HW modification (0=default)
//					12-20	ADC I2E calibration data
										




// ***********************************************************
//		Limits of parameter and file name length
// ***********************************************************

#define MAX_FILE_NAME_LENGTH	1024	// Maximum length of file names
#define MAX_PAR_NAME_LENGTH		65		// Maximum length of parameter names

// ***********************************************************
//		Switches for downloading configurations
// ***********************************************************

#define FIPPI_CONFIG				1		// P4 FIPPI configuration
#define DSP_CODE					2		// P4 DSP code
#define DSP_PARA_VAL				3		// any DSP parameter values
#define COM_FPGA_CONFIG_REV_B		4		// P4 Communication FPGA configuration (Rev. B)
#define COM_FPGA_CONFIG_REV_C		5		// P4 Communication FPGA configuration (Rev. C-E)
#define P4E_FPGA_CONFIG				6		// P4e Artix configuration
#define P500E_FPGA_CONFIG			7		// P500e Virtex configuration
#define P500E_DSP_CODE				8		// P500e/P4e DSP code
#define COM_FPGA_CONFIG_P500_REV_B	9		// P500 Communication FPGA configuration 
#define P500_DSP_CODE				10		// P500 DSP code
#define P4E14500_FPGA_CONFIG		11		// P4e Artix configuration (14/500 only)
#define P32_FPGA_CONFIG             12				// P32 Artix configuration
#define P32_DSP_CODE                13              // P32 DSP code

#define DSP_PARA_NAM		0		// DSP parameter names
//#define DSP_MEM_NAM		1		// DSP internal memory parameter names


// ***********************************************************
//		Data transfer direction
// ***********************************************************

#define DMABUFREFILL_TIMEOUT	50		// 2MB refill timeout limit in ms
#define DMATRANSFER_TIMEOUT		500	// DMA transfer timeout limit in ms
#define MOD_READ				1		// Host read from modules
#define MOD_WRITE				0		// Host write to modules  


// ***********************************************************
//		Frequently used Control Tasks
// ***********************************************************

#define SET_DACS					0	// DSP task Set DACs
#define ENABLE_INPUT				1	// DSP task Enable detector signal input
#define RAMP_TRACKDAC				3	// DSP task  TrackDAC
#define GET_TRACES					4	// DSP task Acquire ADC traces
#define PROGRAM_FIPPI				5	// DSP task Program FIPPIs
#define COLLECT_BASES				6	// DSP task Collect baselines
#define FIND_OFFSET					13	// DSP task Find offset called by ADJUST_OFFSETS_DSP
#define FIND_BLCUT					40	// DSP task Compute BLcut called by ADJUST_BLCUT
#define FIND_TAU					41	// DSP task Compute tau called by ADJUST_TAU_DSP

#define ADJUST_OFFSETS				0x83	// Adjust module offsets in C-library
#define ACQUIRE_ADC_TRACES			0x84	// Acquire ADC traces from the module
#define ADJUST_OFFSETS_DSP  		0x85	// Adjust module offsets (new method in DSP)
#define ADJUST_BLCUT				0x80	// Compute BLcut in C-library for P4/P500 and in DSP for P500e
#define ADJUST_TAU					0x81	// Compute tau in C-library
#define ADJUST_TAU_DSP				0x89	// Compute tau in DSP

#define READ_EEPROM_MEMORY			0x100	// Read the entire contents of EEPROM memory
#define WRITE_EEPROM_MEMORY			0x101	// Write the entire contents of EEPROM memory
#define WRITE_EEPROM_MEMORY_SHORT	0x102	// Write only 64 bytes to EEPROM memory
#define WRITE_RAMP_SDRAM			0x200  

// ***********************************************************
//		Run Type and Polling
// ***********************************************************

#define NEW_RUN					1		// New data run
#define RESUME_RUN				0		// Resume run

#define NO_POLL					0		// No polling
#define AUTO_POLL				1		// Auto polling

#define BIT_RUNENA				0		// Host Control bit to request DAQ run
#define BIT_CTRLENA				1		// Host Control bit to request control run
#define BIT_PARIO				5		// Host Control bit to request parameter I/O
#define BIT_MCAUPPERA			6		// Host Control bit to specify upper MCA address range
#define BIT_EORR				7		// Host Control bit to indicate to DSP that EOR has been received
#define BIT_PARIO_ACTIVE		12		// Host Statuis bit indicating par I/O in progress
#define BIT_RUN_ACTIVE			13		// Host Statuis bit indicating DAQ or control run in progress
#define BIT_DATA_READY			14		// Host Statuis bit indicating data ready in mailbox


// ***********************************************************
//		MODULE CSRA, CSR and DBLBUFCSR bit masks
// ***********************************************************

#define MODCSRA_EMWORDS			1		// Write list mode data to EM
#define MODCSRA_PULLUP			2		// Enable pullups on the trigger line
#define DBLBUFCSR_ENABLE		0		// Enable double buffer mode
#define DBLBUFCSR_READ          0x0002	// OR in this to notify DSP host has read
#define DBLBUFCSR_128K_FIRST    3		// Set by DSP to indicate whick block to read first
#define CSR_128K_FIRST			10		// Set by DSP to indicate whick block to read first in CSR
#define CSR_ODD_WORD			11		// Set by DSP to indicate odd number of words in EM
#define CSR_ACTIVE				13		// Set by DSP to indicate run in progress
#define CSR_DATAREADY			14		// Set by DSP to indicate data is ready. Cleared by reading WCR
#define CSR_TRACE4X			    11		// bit controlling the 4x trace option


// ***********************************************************
//		Pixie-4/P500(e) variant definitions
// ***********************************************************

#define P4_CTSCALE				16			// The scaling factor for count time counters
#define P4_SYSTEM_CLOCK_MHZ		75			// System clock in MHz
#define P4_FILTER_CLOCK_MHZ		75			// clock for pulse processing in FPGA in MHz
#define P4_ADC_CLOCK_MHZ		75			// digitization clock in ADC. 
#define P4_DSP_CLOCK_MHZ		75			// DSP clock in MHz
#define P4E_CTSCALE				32			// The scaling factor for count time counters
#define P4E_SYSTEM_CLOCK_MHZ	125			// System, DSP clock in MHz
#define P4E_FILTER_CLOCK_MHZ	125			// clock for pulse processing in FPGA in MHz
#define P4E125_ADC_CLOCK_MHZ	125			// digitization clock in ADC.  
#define P4E500_ADC_CLOCK_MHZ	500			// digitization clock in ADC.  
#define P4E_DSP_CLOCK_MHZ		75			// DSP clock in MHz (P4e has 300 MHz clock, but timers are 1/4)
#define P500E_CTSCALE			32			// The scaling factor for count time counters
#define P500E_SYSTEM_CLOCK_MHZ	125			// System clock in MHz
#define P500E_FILTER_CLOCK_MHZ	125			// clock for pulse processing in FPGA in MHz
#define P500E_ADC_CLOCK_MHZ		500			// digitization clock in ADC. 
#define P500E_DSP_CLOCK_MHZ		75			// DSP clock in MHz (P500e has 300 MHz clock, but timers are 1/4)

// ***********************************************************
//		LM data constants
// ***********************************************************
#define P4_LIST_FILE			4				// List mode file is Pixie-4 format
#define P500E_LIST_FILE			500				// List mode file is Pixie-500 Express format
#define WATERMARK				0x12345678		// watermark to identify event in P500e LM data format
#define EORMARK					0x01000002		// hit pattern for end of run record
#define RSRMARK					0x01000004		// hit pattern for end of run record
#define WATERMARKINDEX16		30				// location of watermark in header in 16bit words
#define MAXFIFOBLOCKS			1024			// Trace FIFO length in 32-word blocks (128 max for P4e, but P16 may be longer)

// 32-bit words positions in Channel Header,
#define chanHeadEventStatusIdx			0
#define chanHeadNumBlocksIdx			1
#define chanHeadLoMidTrigTimeIdx		2
#define chanHeadHiTrigTimeIdx			3
#define chanHeadEnChanIdx				4
#define chanHeadPSAIdx					5
#define chanHeadPSA0Idx					6
#define chanHeadPSA1Idx					7
#define chanHeadCheckSumIdx				14
#define chanHeadWatermarkIdx			15 

// ***********************************************************
//		Error codes
// ***********************************************************
#define REGIO_ERR			-17 
#define SCAN_CRATE_ERR		-1
#define RD_SYS_B_ERR		-2
#define RD_SYS_C_ERR		-3
#define RD_FIP_ERR			-4
#define RD_DSP_EXE_ERR		-5
#define RD_DSP_PAR_ERR		-6
#define INIT_DSP_PAR_ERR	-7
#define BOOT_SYS_ERR		-81
#define BOOT_FIPPI_ERR		-82
#define BOOT_DSP_ERR		-83
#define SET_DAC_ERR			-84
#define PROG_FIPPI_ERR		-85
#define ENA_INTS_ERR		-86
#define BOOT_PATTERN_ERR	-87
#define CLOSE_HDLE_ERR		-13


#pragma warning(disable : 4996)		// sprintf warnings (TODO: there may be a better place for this)
#ifdef XIA_WINDOZE
	#define PIXIE_EXPORT _declspec(dllexport)
	#define PIXIE_API _stdcall
#elif XIA_LINUX
	#define PIXIE_EXPORT
	#define PIXIE_API
#endif

#ifdef __cplusplus
}
#endif

#endif	/* End of defs.h */


