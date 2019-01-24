
/******************************************************************************/
/*	Sample.h						              */
/*		last updated: 4/05/11 DB			              */
/*								              */
/******************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "PlxTypes.h"
#include "Plx.h"
#include "globals.h"
#include "sharedfiles.h"
#include "utilities.h"



 // OLD
/*--------------------------------------------------------------------------------*/
/* The boot_file_names has 7 entries which include:                               */
/*                                               communication FPGA Rev. B file,  */
/*						 communication FPGA Rev. C file,  */
/*						 the Fippi FPGA file,             */
/*						 the DSP code bianry file,        */
/*						 DSP parameter values file,       */
/*						 DSP parameter names file,        */
/*                                               all parameter names file.        */
/*
  "C:\\XIA\\PIXIE4\\firmware\\FippiP500.bin",
  "C:\\XIA\\PIXIE4\\firmware\\syspixie_RevC.bin",
//  "C:\\XIA\\PIXIE4\\firmware\\pixie.bin",
//"C:\\Users\\xia\\Documents\\Visual\ Studio\ 2005\\Projects\\pixie500e\\pixie500e_fw\\Redshift_test\\redshift_pair.bin",
//"C:\\XIA\\Redshift\\redshift_v13.4\\redshift_pair.bin",
"C:\\XIA\\Redshift\\Redshift_B\\ZZ_redshift_pair.bin",
"C:\\XIA\\Pixie500e\\DSP\\p500e.ldr",
//"C:\\XIA\\Pixie500e\\DSP\\P500dsp.ldr",
  "C:\\XIA\\Pixie500e\\Configuration\\default500_test2.set",
  "C:\\XIA\\Pixie500e\\DSP\\p500e.var",
  "C:\\XIA\\Pixie500e\\DSP\\PXIcode.lst"
  */
/*--------------------------------------------------------------------------------*/

// NEW
/*--------------------------------------------------------------------------------*/
/* The boot_file_names has 7 entries which include:                               */
/*                 the communication FPGA P4 Rev. B file,                         */
/*                 the communication FPGA P4 Rev. C file,                         */
/*                 the P4 Fippi FPGA file,                                        */
/*                 the P4 DSP code binary file,                                   */
/*                 DSP parameter values file,                                     */
/*                 DSP parameter names file,                                      */
/*                 the P4/P500 all parameter names file.                          */
/*                 the communication FPGA P500 Rev. B file,                       */
/*                 the P500 Fippi FPGA file,                                      */
/*                 the P500e FPGA file,                                           */
/*                 the P500 DSP code binary file,                                 */
/*                 the P500e DSP code binary file,                                */
/*                 the P500e all parameter names file.                            */
/*--------------------------------------------------------------------------------*/

S8 *boot_file_names[] = { /* for one Pixie-4 module */
#ifdef XIA_WINDOZE
  "C:\\XIA\\P4eSW\\firmware\\P4e_16_125_vdo.bin",
  "C:\\XIA\\P4eSW\\firmware\\syspixie_RevC.bin",
  "C:\\XIA\\P4eSW\\firmware\\pixie.bin",
  "C:\\XIA\\P4eSW\\DSP\\PXIcode.bin",
  "C:\\XIA\\P4eSW\\Configuration\\default_125.set",
  "C:\\XIA\\P4eSW\\DSP\\p500e.var",
  "C:\\XIA\\P4eSW\\DSP\\PXIcode.lst",
  "C:\\XIA\\P4eSW\\firmware\\syspixie_RevC.bin",
  "C:\\XIA\\P4eSW\\firmware\\P4e_14_500_vdo.bin",
  //"C:\\XIA\\P4eSW\\firmware\\FippiP500.bin",
  "C:\\XIA\\P4eSW\\firmware\\p500e_zdt.bin",
  "C:\\XIA\\P4eSW\\DSP\\P500code.bin",
  "C:\\XIA\\P4eSW\\DSP\\p500e.ldr",
  "C:\\XIA\\P4eSW\\DSP\\P500e.lst",
  "",
  "",
  "",
  ""
#else
  "/home/xia/Documents/Pixie4e/Firmware/P4e_16_125_vdo.bin",
  "/home/xia/Documents/Pixie4e/firmware/syspixie_RevC.bin",
  "/home/xia/Documents/Pixie4e/firmware/pixie.bin",
  "/home/xia/Documents/Pixie4e/DSP/PXIcode.bin",
  "/home/xia/Documents/Pixie4e/Configuration/default_125.set",
  "/home/xia/Documents/Pixie4e/DSP/p500e.var",
  "/home/xia/Documents/Pixie4e/DSP/PXIcode.lst",
  "/home/xia/Documents/Pixie4e/firmware/syspixie_RevC.bin",
  "/home/xia/Documents/Pixie4e/Firmware/P4e_14_500_vdo.bin",
  "/home/xia/Documents/Pixie4e/Firmware/p500e_zdt.bin",
  "/home/xia/Documents/Pixie4e/DSP/P500code.bin",
  "/home/xia/Documents/Pixie4e/DSP/p500e.ldr",
  "/home/xia/Documents/Pixie4e/DSP/p500e.lst",
 
#endif
};

       
