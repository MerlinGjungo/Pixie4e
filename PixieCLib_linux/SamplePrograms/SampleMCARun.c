/**************************************************************************/
/*	SampleMCARun.c							  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*	This is a sample program based on the Pixie-4 C library.          */
/*	It can be used to collect MCA spectra with Pixie-4 modules.       */
/*									  */
/**************************************************************************/

#include "Sample.h"

int main(void){

	U8     ModNum          = 0;
	U8     ChanNum         = 0;
	U8     NumberOfModules = 1;
	U8     Slots[]         = {200}; 
	U16    offset          = 0;
	U16    idx             = 0;
	U32   *UserData        = NULL;
	S32    retval          = 0;
	S32    i               = 0, j;
	double timeout;
	double poll_interval;
	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};
	FILE  *bufferdata = NULL;
	FILE  *ofp        = NULL;

	/* System configuration */
	#include "SystemConfig.c"
	/* Initialize modules */
	if((retval = Pixie_Boot_System(0xE)) < 0) {
	    printf("*ERROR* (Pixie_Boot_System): boot Pixie-4 failed, retval = %d\n", retval);
	    return(retval);
	}
	/***********************************************/
	/*  Simple task: make a 10-second MCA run      */
	/*  and save the spectrum into a file.         */
	/***********************************************/	
	printf("\nStart a 10-second MCA run ...\n");
	timeout       = 10;                                     /* make a 10 sec MCA run */
	poll_interval = 1.0;                                    /* Polling interval */
	Pixie_Acquire_Data(0x1301, NULL, "", NumberOfModules);  /* start a new MCA run (0x301) */
	Pixie_Sleep(1);                                         /* a short delay here to avoid polling too early */
	do {
	    /* wait until run has ended (0x4000 == poll) */
	    retval = Pixie_Acquire_Data(0x4301, NULL, "", 0);
	    /* Wait for one poll_interval */
	    Pixie_Sleep((U32)(poll_interval * 1000));
	    timeout -= poll_interval;
	} while ((retval == 1) && (timeout > 0));
	/* stop run */
	Pixie_Acquire_Data(0x3301, NULL, "", NumberOfModules);
	/* remove the old spectrum file */
	system ("rm MCAspectrum.bin");
	/* readout and save MCA spectrum */
	Pixie_Acquire_Data(0x5301, NULL, "MCAspectrum.bin", NumberOfModules);
	UserData = malloc(270000 * sizeof(U32));
	/* read MCA spectrum from file */
	if((retval = Pixie_Acquire_Data(0x8000, UserData, "MCAspectrum.bin", 0)) < 0) {
	    printf("*ERROR* (Pixie_Acquire_Data(0x8000)): retval = %d\n", retval);
	    return(retval);
	}
	system ("rm MCAUserData");
	ofp = fopen("MCAUserData", "w");
	for (i = 0; i < 270000; i++) fprintf(ofp, "%u\n", UserData[i]);
	fclose (ofp);
	free(UserData);

	printf("\nFinish a 10-second MCA run ...\n");

	return(0);
}

