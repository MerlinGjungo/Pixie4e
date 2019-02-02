/**************************************************************************/
/*	SampleBoot.c							  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*		This is a sample program based on the Pixie-4 C library.  */
/*		It can be used to boot Pixie-4 modules.			  */
/*									  */
/**************************************************************************/

//#include "Sample.h"
// Headers that were included in Sample.h
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "PlxTypes.h"
#include "Plx.h"
#include "globals.h"
#include "sharedfiles.h"
#include "utilities.h"


int main(int argc, char** argv)
{


	const U8     NumberOfModules = 1; // hardcoded for just one module.
	S16     Slots[] = {0}; // if SN was not initialized, use 0, if SN from default EEPROM use 0xBEEF. SN is provided as the argument
	S32    retval          = 0;
	S32    i, j;

	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};


	DWORD dwStatus, dwData;
	DWORD val;

	char *parname;
	U16 offset, idx;
	U8 ModNum, ChanNum;

	char c;

	unsigned char eepromEntry[6];// = {EEPROM_ENTRY(0xF104, 0xBEEFA101)};

	/* System configuration */
	//#include "SystemConfig.c" // almost running SystemConfig.c, but ignore boof_file_names here
	SystemParameterValues[Find_Xact_Match("NUMBER_MODULES",     System_Parameter_Names, N_SYSTEM_PAR)] = NumberOfModules;
	SystemParameterValues[Find_Xact_Match("OFFLINE_ANALYSIS",   System_Parameter_Names, N_SYSTEM_PAR)] = 0;
	SystemParameterValues[Find_Xact_Match("AUTO_PROCESSLMDATA", System_Parameter_Names, N_SYSTEM_PAR)] = 0;
	SystemParameterValues[Find_Xact_Match("MAX_NUMBER_MODULES", System_Parameter_Names, N_SYSTEM_PAR)] = 7;//13;
	SystemParameterValues[Find_Xact_Match("KEEP_CW",            System_Parameter_Names, N_SYSTEM_PAR)] = 1;

	/* Download boot file names */
	//Pixie_Hand_Down_Names(boot_file_names, "ALL_FILES");
	/* Call library function Pixie_User_Par_IO to initialize SystemParameterValues */
	Pixie_User_Par_IO(SystemParameterValues, "NUMBER_MODULES",     "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "OFFLINE_ANALYSIS",   "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "AUTO_PROCESSLMDATA", "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "MAX_NUMBER_MODULES", "SYSTEM", MOD_WRITE, 0, 0);



	fflush(stdin);
	printf("*******************************************************\n");
	printf("* Usage: Pixie500e_program_EEPROM.exe                 *\n");
	printf("* Power up single module with shunt on EEPROM jumper  *\n");
	printf("* then remove shunt before running this program       *\n");
	printf("* ----------------------------------------            *\n");
	printf("* ... then enter the serial number when prompted.     *\n");
	printf("* If board with provided serial number is not found,  *\n");
	printf("* use S/N value reported by scan (could be            *\n");
	printf("* 0x0000 if EEPROM has not been programmed yet, or    *\n");
	printf("* 0xBEEF if default EEPROM was loaded.                *\n");
	printf("* Use with a single module in the crate!              *\n");
	printf("*******************************************************\n");
	printf("                                                       \n");
	printf("Enter 3-digit serial number of the module to be programmed:\n");
	printf("(use 0 if the EEPROM was not initialized yet, 0xBEEF if it has default EEPROM)\n");
	if (scanf("%d", &val)!=1) {
		sprintf(ErrMSG, "*ERROR*: not a valid serial number entered.");
		Pixie_Print_MSG(ErrMSG,1);
		fflush(stdin);
		printf("...Press any key to quit...\n");
		getchar();
		return (-1);
	};
	if ( val < 0 || val > 65535 || Number_Modules !=1) {
		sprintf(ErrMSG, "*ERROR*: Please provide valid serial number in range from 0 to 65536.");
		Pixie_Print_MSG(ErrMSG,1);

		printf("...Press any key to quit...\n");
		getchar();
		return (-1);
	} // if entered serial number was not ok

	Slots[0] = val;


	/* The PXI crate slot number where the Pixie module is installed */
	for (i = 0; i < NumberOfModules; i++) {
		SystemParameterValues[Find_Xact_Match("SLOT_WAVE", System_Parameter_Names, N_SYSTEM_PAR) + i] = Slots[i];
	}

	Pixie_User_Par_IO(SystemParameterValues, "SLOT_WAVE",          "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "KEEP_CW",            "SYSTEM", MOD_WRITE, 0, 0);
	// End of system configuration


	// Need to scan crate for devices, then proceed to boot offline mode.
	/* Scan all crate slots and find the address for each slot where a PCI device is installed */
	retval=Pixie_Scan_Crate_Slots(Number_Modules, &Phy_Slot_Wave[0]);
	if(retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie500e_program_EEPROM): Scanning crate slots unsuccessful, error %d.", retval);
		Pixie_Print_MSG(ErrMSG,1);
		PIXIE500E_LibUninit();
		return(-1);
	}
	sprintf(ErrMSG, "*INFO* (Pixie500e_program_EEPROM): Scanning crate slots successful.");
	Pixie_Print_MSG(ErrMSG,1);

	retval = Pixie_Boot(0x0);
	if (retval < 0) {
		sprintf(ErrMSG, "*ERROR* (Pixie500e_program_EEPROM): Offline module boot unsuccessful, error %d.", retval);
		Pixie_Print_MSG(ErrMSG,1);
		PIXIE500E_LibUninit();
		return(-2);
	}
	sprintf(ErrMSG, "*INFO* (Pixie500e_program_EEPROM): Offline module boot successful.");
	Pixie_Print_MSG(ErrMSG,1);

	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		sprintf(ErrMSG, "*INFO* (Pixie500e_program_EEPROM): MODULE %d dump...", ModNum);
		Pixie_Print_MSG(ErrMSG,1);

		dwStatus = PIXIE500E_DumpI2CPROM(hDev[ModNum]);
		Pixie_Sleep(10);

		fflush(stdin);
		printf("Load default EEPROM? [y/N]:");
		c = getchar();
		if (c=='y') {
			sprintf(ErrMSG, "*INFO* (Pixie500e_program_EEPROM): Loading default EEPROM...");
			Pixie_Print_MSG(ErrMSG,1);

			dwStatus = PIXIE500E_LoadDefaultI2CPROM(hDev[ModNum]);
			Pixie_Sleep(10);

			if (dwStatus == WD_STATUS_SUCCESS) {
				sprintf(ErrMSG, "*INFO* (Pixie500e_program_EEPROM): Loading default EEPROM DONE");
				Pixie_Print_MSG(ErrMSG,1);
			}
			else {
				sprintf(ErrMSG, "*ERROR* (Pixie500e_program_EEPROM): Loading default EEPROM FAILED!");
				Pixie_Print_MSG(ErrMSG,1);
				PIXIE500E_LibUninit();
				return (-3);
			}
		}
		

		fflush(stdin);
		printf("Set serial number? [y/N]:");
		c = getchar();
		if (c=='y') {
			fflush(stdin);
			printf("Enter 3-digit serial number: ");
			scanf("%d", &val);
			if (val > 0 && val <1000) {
				printf("Good number %d\n", val);
				PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, 7);
				printf("changing from %d (0x%X) to %d (0x%X)\n", (dwData >> 16),(dwData >> 16),val,val);
				// replace serial number in the 7th EEPROM word (upper 2 bytes):
				dwData = ((val & 0xFFFF) << 16) + (dwData & 0x0000FFFF);
				eepromEntry[0] = (0xF104) & 0xFF;
				eepromEntry[1] = (0xF104 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 7);
			}
			else {
				printf("Bad number\n");
			}
		}

		fflush(stdin);
		printf("Set Module type? [y/N]:");
		c = getchar();
		if (c=='y') {
			fflush(stdin);
			printf("Enter 4-digit module type \n");
			printf(" for example A550 for P4e, A101 for P500e\n");
			printf(" in P4e, 2nd last digit is ADC/HW variant as follows:\n");
			printf("  0 = 14bit, 500 MHz, original P500e \n");
			printf("  1 = reserved\n");
			printf("  2 = reserved\n");
			printf("  3 = reserved\n");
			printf("  4 = reserved\n");
			printf("  5 = 16bit, 125 MHz\n");
			printf("  6 = 14bit, 125 MHz\n");
			printf("  7 = 12bit, 125 MHz\n");
			printf("  8 = reserved\n");
			printf("  9 = 16bit, 250 MHz\n");
			printf("  A = 14bit, 250 MHz\n");
			printf("  B = 12bit, 250 MHz\n");
			printf("  C = reserved\n");
			printf("  D = 16bit, 500 MHz\n");
			printf("  E = 14bit, 500 MHz\n");
			printf("  F = 12bit, 500 MHz\n");
			printf(" last digit is HW revision (0=A, 1=B, etc)\n");
			printf("Now enter 4-digit module type (16/125: A551, 14/500: A5E1) \n");
			scanf("%X", &val);

			if (val > 0xA000 && val < 0xAFFF) {
				printf("Good number %X\n", val);
				
				printf("changing entry 7 from 0x%X to 0x%X\n", (dwData & 0xFFFF), val);
				// replace module type in the 7th EEPROM word (lower 2 bytes):
				dwData = (dwData & 0xFFFF0000) + (val & 0x0000FFFF);
				eepromEntry[0] = (0xF104) & 0xFF;
				eepromEntry[1] = (0xF104 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 7);

				PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, 5);
				printf("changing entry 5 from 0x%X to 0x%X\n", ((dwData >> 16) & 0xFFFF), val);
				// replace module type in the 5th EEPROM word (upper 2 bytes):
				dwData = ((val << 16) & 0xFFFF0000) + (dwData & 0x0000FFFF);
				eepromEntry[0] = (0xF02C) & 0xFF;
				eepromEntry[1] = (0xF02C >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 5);
			}
			else {
				printf("Bad number\n");
			}
		}

		fflush(stdin);
		printf("Overwrite defaults from utility (rather than calling P500e.lib function) [y/N]:");
		c = getchar();
		if (c=='y') {
		
				// set the 0th EEPROM word:
				dwData = 0x0809F04C;
				eepromEntry[0] = (0xF808) & 0xFF;			//addr
				eepromEntry[1] = (0xF808 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 0);

								// set the 1st EEPROM word:
				dwData = 0x0001F04C;
				eepromEntry[0] = (0xF808) & 0xFF;			//addr
				eepromEntry[1] = (0xF808 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 1);

								// set the 2nd EEPROM word:
				dwData = 0x00008002;
				eepromEntry[0] = (0xF05C) & 0xFF;			//addr
				eepromEntry[1] = (0xF05C >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 2);

								// set the 3rd EEPROM word:
				dwData = 0x00005840;
				eepromEntry[0] = (0xF060) & 0xFF;			//addr
				eepromEntry[1] = (0xF060 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 3);

								// set the 4th EEPROM word:
				dwData = 0x00000028;
				eepromEntry[0] = (0xF850) & 0xFF;			//addr
				eepromEntry[1] = (0xF850 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 4);

								// set the 5th EEPROM word:
				dwData = 0xA5511C40;
				eepromEntry[0] = (0xF02C) & 0xFF;			//addr
				eepromEntry[1] = (0xF02C >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 5);

								// set the 6th EEPROM word:
				dwData = 0x00065000;
				eepromEntry[0] = (0xF800) & 0xFF;			//addr
				eepromEntry[1] = (0xF800 >> 8) & 0xFF;
				eepromEntry[2] = (dwData) & 0xFF;			// data
				eepromEntry[3] = (dwData >> 8) & 0xFF;
				eepromEntry[4] = (dwData >> 16) & 0xFF;
				eepromEntry[5] = (dwData >> 24) & 0xFF;
				PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 6);
		}


		printf("Updated EEPROM\n");
		for (i = 0; i < 13; i++) {
			dwData = 0xDEADBEEF;
			PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, i);
			sprintf(ErrMSG, "EEPROM%d: 0x%08X", i, dwData);
			Pixie_Print_MSG(ErrMSG,1);
		}

		// KS: here is the proper way to do error check. ErrLog() in pixie500e_lib.c
		// will also call Pixie_Print_MSG().
		retval = PIXIE500E_ReadWriteReg(hDev[ModNum], CLK_CSR, WDC_READ, &val, FALSE);
		if (retval != WD_STATUS_SUCCESS) {
			sprintf(ErrMSG, "*ERROR* (Main): Failed reading CLK_CSR");
			Pixie_Print_MSG(ErrMSG,1);
			return(WD_WINDRIVER_STATUS_ERROR);
		}
		sprintf(ErrMSG, "*DEBUG* (Main): Module %d CLK_CSR=0x%08X\n", ModNum, val);
		Pixie_Print_MSG(ErrMSG,1);

		// read word 1 of EEPROM (should match CLK_CSR)
		dwData = 0;
		PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, 1);
		val = GN4124_EEPROM_DEFAULT[1];
		//if ( (dwData && 0x000FFFFF) != (GN4124_EEPROM_DEFAULT[1] && 0x000FFFFF)) {
		//	sprintf(ErrMSG, "*INFO* Need to reboot the module %d to apply CLK_CRS settings from EEPROM", ModNum);
		//	Pixie_Print_MSG(ErrMSG,1);
		//}


	} // for module


	//*eepromEntry = EEPROM_ENTRY(0xF104, 0xDEADA100);

	//PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 11);

	PIXIE500E_LibUninit();

	sprintf(ErrMSG, "*INFO* EEPROM programming done");
	Pixie_Print_MSG(ErrMSG,1);
	fflush(stdin);
	printf("...Press any key to quit...");
	getchar();

	return(retval);
}

