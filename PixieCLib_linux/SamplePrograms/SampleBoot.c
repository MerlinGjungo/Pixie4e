/**************************************************************************/
/*	SampleBoot.c							  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*		This is a sample program based on the Pixie-4 C library.  */
/*		It can be used to boot Pixie-4 modules.			  */
/*									  */
/**************************************************************************/

#include "Sample.h"
#include "globals.c"

int main(int argc, char **argv)
{
	U8     NumberOfModules = 1;
//	S8 		*ParName = NULL;
        S8 ParName[MAX_PAR_NAME_LENGTH];
	S16     Slots[]         = {231};
	S32    retval          = 0;
	S32    i, j;
//	WD_PCI_CARD_INFO deviceInfo;
	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};
	/* System configuration */
	#include "SystemConfig.c"

        PrintDebugMsg_Boot=1;
	/* Boot all modules in the crate */
	if((retval = Pixie_Boot_System(0x2)) < 0) { // 0x2 FPGA only, 0x6 FPGA and DSP, 0xE for FPGA, DSP, and DSP parameters
		printf("Boot Pixie-4 failed, retval=%d\n", retval);
		return(retval);
	}
	else {
		printf("... ");
		printf("*INFO* (Main) Boot Pixie-4 succeeded\n");
	}

    return (0);
//	PIXIE500E_LibUninit();


        // SYSTEM
        retval = Pixie_User_Par_IO(SystemParameterValues, "ALL_SYSTEM_PARAMETERS", "SYSTEM", MOD_READ, 0, 0);
        if (retval !=0) {
          printf("Problems reading system parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_SYSTEM_PAR; i++) {
          Pixie_Get_Par_Name(i, "SYSTEM", ParName);
          printf("system %d: %s 0x%04X\n", i, ParName, (UINT16)SystemParameterValues[i]);
        }
        getchar();

        // MODULE
        retval = Pixie_User_Par_IO(ModuleParameterValues, "ALL_MODULE_PARAMETERS", "MODULE", MOD_READ, 0, 0);
        if (retval !=0) {
          printf("Problems reading module parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_MODULE_PAR; i++) {
          Pixie_Get_Par_Name(i, "MODULE", ParName);
          printf("module %d: %s 0x%04X\n", i, ParName, (UINT16)ModuleParameterValues[i]);
        }
        getchar();

        // CHANNEL 0
        retval = Pixie_User_Par_IO(ChannelParameterValues, "ALL_CHANNEL_PARAMETERS", "CHANNEL", MOD_READ, 0, 0);
        if (retval !=0) {
          printf("Problems reading channel parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_CHANNEL_PAR; i++) {
          Pixie_Get_Par_Name(i, "CHANNEL", ParName);
          printf("ch0 %d: %s 0x%04X\n", i, ParName, (UINT16)ChannelParameterValues[i]);
        }

        // CHANNEL 1
        retval = Pixie_User_Par_IO(ChannelParameterValues, "ALL_CHANNEL_PARAMETERS", "CHANNEL", MOD_READ, 0, 1);
        if (retval !=0) {
          printf("Problems reading channel parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_CHANNEL_PAR; i++) {
          Pixie_Get_Par_Name(i, "CHANNEL", ParName);
          printf("ch1 %d: %s 0x%04X\n", i, ParName, (UINT16)ChannelParameterValues[i + N_CHANNEL_PAR]);
        }

        // CHANNEL 2
        retval = Pixie_User_Par_IO(ChannelParameterValues, "ALL_CHANNEL_PARAMETERS", "CHANNEL", MOD_READ, 0, 2);
        if (retval !=0) {
          printf("Problems reading channel parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_CHANNEL_PAR; i++) {
          Pixie_Get_Par_Name(i, "CHANNEL", ParName);
          printf("ch2 %d: %s 0x%04X\n", i, ParName, (UINT16)ChannelParameterValues[i + N_CHANNEL_PAR*2]);
        }

        // CHANNEL 3
        retval = Pixie_User_Par_IO(ChannelParameterValues, "ALL_CHANNEL_PARAMETERS", "CHANNEL", MOD_READ, 0, 3);
        if (retval !=0) {
          printf("Problems reading channel parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_CHANNEL_PAR; i++) {
          Pixie_Get_Par_Name(i, "CHANNEL", ParName);
          printf("ch3 %d: %s 0x%04X\n", i, ParName, (UINT16)ChannelParameterValues[i + N_CHANNEL_PAR*3]);
        }

	return(0);
}
