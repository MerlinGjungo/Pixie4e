/*************************************************************************/
/*	SampleADCTrace.c						 */
/*		last updated: 4/26/05 HT			         */
/*									 */
/*	This is a sample program based on the Pixie-4 C library.         */
/*	It can be used to collect ADC traces with Pixie-4 modules.       */
/*								         */
/*************************************************************************/

#include "Sample.h"

int main(void){

	U8     ModNum          = 0;
	U8     ChanNum         = 1;
	U8     NumberOfModules = 1;
	U8     Slots[]         = {200};
	U16    offset          = 0;
	U16    idx             = 0;
	U32    *ADC_Data;
//	U32    ADC_Data[IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS];
	S32    retval;
	S32    i, j;
	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};
	FILE  *bufferdata = NULL;
	U32 val;

	/* System configuration */
	#include "SystemConfig.c"
	
	/* Initialize modules */
	if((retval = Pixie_Boot_System(0xE)) < 0) {
		printf("*ERROR* (Pixie_Boot_System): boot Pixie-4 failed, retval = %d\n", retval);
		return(retval);
	}
	
	/***********************************************/
	/*  Simple task 0: adjust offsets.             */
	/***********************************************/
	printf("\nAdjusting offsets ...\n");
	ModNum = 0;
	ChanNum = 0;
	offset  = ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;

	idx = Pixie_Get_Par_Idx("CHANNEL_CSRA", "CHANNEL");
        Pixie_User_Par_IO(ChannelParameterValues, "CHANNEL_CSRA", "CHANNEL", MOD_READ, ModNum, ChanNum);
	val = (U32)ChannelParameterValues[offset+idx];
       	val &= ~(0x20);
	ChannelParameterValues[offset+idx] = (double)val;
        Pixie_User_Par_IO(ChannelParameterValues, "CHANNEL_CSRA", "CHANNEL", MOD_WRITE, ModNum, ChanNum);

	idx = Pixie_Get_Par_Idx("BASELINE_PERCENT", "CHANNEL");
	ChannelParameterValues[offset+idx] = 10;
	if((retval = Pixie_User_Par_IO(ChannelParameterValues, "BASELINE_PERCENT", "CHANNEL", MOD_WRITE, ModNum, ChanNum)) < 0) {
	    printf("Pixie_User_Par_IO() failed, retval=%d\n", retval);
	    return(retval);
	}

	if((retval = Pixie_Acquire_Data(ADJUST_OFFSETS, NULL, "", NumberOfModules)) < 0){ // Use both modules
	    printf("Adjust offsets failed, retval=%d\n", retval);
	    return(retval);
	}
	/***********************************************/
	/*  Simple task 1: acquire ADC trace for the   */
	/*  current channel. File_Name can be blank.   */
	/***********************************************/
	printf("\nAcquire ADC trace ...\n");
	ADC_Data = malloc(IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
	/***************/
	/*  Module 0   */
	/***************/
	if((retval = Pixie_Acquire_Data(ACQUIRE_ADC_TRACES, ADC_Data, "", 0)) < 0) {
	    printf("Acquire ADC trace failed, retval=%d\n", retval);
	    return(retval);
	}
	else {  /* Write to a file */
	    if((bufferdata = fopen("adctrace0.dat", "w")) != NULL) {
		for(i = 0; i < IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS; i++) fprintf(bufferdata, "%d\n", ADC_Data[i]);
		fclose(bufferdata);
	    }
	}

	return(0);
}

