/**************************************************************************/
/*	SampleListMode.c						  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*	This is a sample program based on the Pixie-4 C library.          */
/*	It can be used to record list mode data with Pixie-4 modules.     */
/*									  */
/**************************************************************************/

#include "Sample.h"

int main(void){

	U8     ModNum          = 0;
	U8     ChanNum         = 0;
	U8     NumberOfModules = 1;
	U8     Slots[]         = {231};
	U16    offset          = 0;
	U16    idx             = 0;
	S32    retval          = 0;
	S32    i, j;

	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};

	FILE  *bufferdata = NULL;
        U16 value16 = 0;
        S8 ParName[MAX_PAR_NAME_LENGTH];

	/* System configuration */
	#include "SystemConfig.c"

	/* Initialize modules */
	if((retval = Pixie_Boot_System(0xE)) < 0) {
	    printf("*ERROR* (Pixie_Boot_System): boot Pixie-4 failed, retval = %d\n", retval);
	    return(retval);
	}
	/* Set synchronization: needed for multiple modules */
	idx = Pixie_Get_Par_Idx("SYNCH_WAIT", "MODULE");
	ModNum  = 0;
	offset  = ModNum*N_MODULE_PAR;
	ModuleParameterValues[offset+idx] = 1;
	if((retval = Pixie_User_Par_IO(ModuleParameterValues, "SYNCH_WAIT", "MODULE", MOD_WRITE, ModNum, ChanNum)) < 0){
	    printf("Pixie_User_Par_IO() failed, retval=%d\n", retval);
	    return(retval);
	}

	/***********************************************/
	/*  Simple task 0: adjust offsets.             */
	/***********************************************/
	printf("\nAdjusting offsets ...\n");
	ModNum = 0;
	ChanNum = 0;
	offset  = ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;

        Pixie_User_Par_IO(ChannelParameterValues, "CHANNEL_CSRA", "CHANNEL", MOD_READ, ModNum, ChanNum);
	idx = Pixie_Get_Par_Idx("CHANNEL_CSRA", "CHANNEL");
	value16 = (U16)ChannelParameterValues[offset+idx];
       	value16 &= ~(0x20);
	ChannelParameterValues[offset+idx] = (double)value16;
        Pixie_User_Par_IO(ChannelParameterValues, "CHANNEL_CSRA", "CHANNEL", MOD_WRITE, ModNum, ChanNum);

	idx = Pixie_Get_Par_Idx("BASELINE_PERCENT", "CHANNEL");
	ChannelParameterValues[offset+idx] = 10;
	if((retval = Pixie_User_Par_IO(ChannelParameterValues, "BASELINE_PERCENT", "CHANNEL", MOD_WRITE, ModNum, ChanNum)) < 0) {
	    printf("Pixie_User_Par_IO() failed, retval=%d\n", retval);
	    return(retval);
	}

	if((retval = Pixie_Acquire_Data(ADJUST_OFFSETS, NULL, "", NumberOfModules)) < 0){ 
	    printf("Adjust offsets failed, retval=%d\n", retval);
	    return(retval);
        } else printf("Offsets adjusted.\n");	


	/******************************************************************/
	/*  Simple task 1: change some module and channel parameters.     */
	/******************************************************************/

        // C_CONTROL
	ModNum = 0;
	ChanNum = 0;
        // Read values
        Pixie_User_Par_IO(ModuleParameterValues, "C_CONTROL", "MODULE", MOD_READ, ModNum, ChanNum);
        idx = Pixie_Get_Par_Idx("C_CONTROL", "MODULE");
        offset = ModNum*N_MODULE_PAR;
        value16 = (U16)ModuleParameterValues[offset+idx];
	// Bit 4: PrintDebugMsg_Boot (setting)
	// Bit 5: PrintDebugMsg_QCerror (setting)
	// Bit 6: PrintDebugMsg_QCdetail (setting)
	// Bit 7: PrintDebugMsg_other (setting)
	// Bit 8: Polling (clearing)
	// Bit 9: BufferQC (setting)
	// Bit 11: PollForNewData (clearing)
	// Bit 12: MultiThreadDAQ (clearing)
	value16 |= 0x2F0;
	value16 &= ~(0x1900);
        ModuleParameterValues[offset+idx]=(double)value16;
        Pixie_User_Par_IO(ModuleParameterValues, "C_CONTROL", "MODULE", MOD_WRITE, ModNum, ChanNum);

	// Trace length:
	// recorded waveform length in multiples of 32-sample blocks (i.e. 32*8 ns = 0.256 us time steps)
	ModNum = 0;
	idx = Pixie_Get_Par_Idx("TRACE_LENGTH", "CHANNEL");

	ChanNum = 0;
	offset = ModNum * N_CHANNEL_PAR * NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
	ChannelParameterValues[offset + idx] = (double)3.328;
	Pixie_User_Par_IO(ChannelParameterValues, "TRACE_LENGTH", "CHANNEL", MOD_WRITE, ModNum, ChanNum);

	ChanNum = 1;
	offset = ModNum * N_CHANNEL_PAR * NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
	ChannelParameterValues[offset + idx] = (double)3.584;
	Pixie_User_Par_IO(ChannelParameterValues, "TRACE_LENGTH", "CHANNEL", MOD_WRITE, ModNum, ChanNum);

	ChanNum = 2;
	offset = ModNum * N_CHANNEL_PAR * NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
	ChannelParameterValues[offset + idx] = (double)3.840;
	Pixie_User_Par_IO(ChannelParameterValues, "TRACE_LENGTH", "CHANNEL", MOD_WRITE, ModNum, ChanNum);

	ChanNum = 3;
	offset = ModNum * N_CHANNEL_PAR * NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
	ChannelParameterValues[offset + idx] = (double)4.096;
	Pixie_User_Par_IO(ChannelParameterValues, "TRACE_LENGTH", "CHANNEL", MOD_WRITE, ModNum, ChanNum);


        // Print parameters
        // MODULE
        retval = Pixie_User_Par_IO(ModuleParameterValues, "ALL_MODULE_PARAMETERS", "MODULE", MOD_READ, ModNum, ChanNum);
        if (retval !=0) {
          printf("Problems reading module parameters, %d\n", retval);
          return (1);
        }
        for (i = 0; i < N_MODULE_PAR; i++) {
          Pixie_Get_Par_Name(i, "MODULE", ParName);
          printf("module: par %d %s = 0x%04X\n", i, ParName, (U16)ModuleParameterValues[i]);
        }

        // CHANNEL
	idx = Pixie_Get_Par_Idx("TRACE_LENGTH", "CHANNEL");
	for  (ChanNum=0; ChanNum < NUMBER_OF_CHANNELS; ChanNum++)  {
	        retval = Pixie_User_Par_IO(ChannelParameterValues, "ALL_CHANNEL_PARAMETERS", "CHANNEL", MOD_READ, ModNum, ChanNum);
	        if (retval !=0) {
	          printf("Problems reading channel parameters, %d\n", retval);
	          return (1);
	        }
		Pixie_Get_Par_Name(idx, "CHANNEL", ParName);
		offset = ModNum * N_CHANNEL_PAR * NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
	        for (i = 0; i < N_CHANNEL_PAR; i++) {
	          Pixie_Get_Par_Name(i, "CHANNEL", ParName);
	          printf("chan %d: par %d %s = %f\n", ChanNum, i, ParName, ChannelParameterValues[offset + i]);
	        } // for parameter
	}// for channel	

	printf("TRACE_LENGTH: %f %f %f %f", 
		ChannelParameterValues[ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + N_CHANNEL_PAR*0+idx],
		ChannelParameterValues[ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + N_CHANNEL_PAR*1+idx],
		ChannelParameterValues[ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + N_CHANNEL_PAR*2+idx],
		ChannelParameterValues[ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + N_CHANNEL_PAR*3+idx]);



	/***********************************************/
	/*  Simple task: list mode run for pulse       */
	/*  shape analysis.                            */
	/***********************************************/
	printf("\nMake 5-sec general list mode run ...\n");
	/* List mode run 0x400 */
	/* Use all modules */


	Pixie_Acquire_Data(0x1400, NULL, "ListModeData.bin", NumberOfModules); // Start run
	Pixie_Sleep(5000);
	Pixie_Acquire_Data(0x3400, NULL, "ListModeData.bin", NumberOfModules); // Stop run
	printf("Info: list mode run 0x400 ended\n");

	return(0);
}

