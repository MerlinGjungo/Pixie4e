/**************************************************************************/
/*	SampleBoot.c							  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*		This is a sample program based on the Pixie-4 C library.  */
/*		It can be used to boot Pixie-4 modules.			  */
/*									  */
/**************************************************************************/

#include "Sample.h"
//#define BOOT_ONLY
//#define LOAD_DEFAULT_EEPROM
//#define TEST_BAR0
//#define TEST_DMA
//#define TEST_IRQ
//#define TEST_DSPPAR
#define TEST_ADCTRACE 
//#define TEST_OFFSETDAC
#define TEST_LISTMODE
//#define TEST_DMAPROG
//#define TEST_READER


unsigned int gIntCounter = 0;

int PIXIE500E_Cleanup();
//static void IntHandler_INT3(WDC_DEVICE_HANDLE hDev, PIXIE500E_INT_RESULT *pIntResult);

int main(int argc, char **argv)
{

	U8     NumberOfModules = 1;
	S16     Slots[]         = {200};
	S32    retval          = 0;
	S32    i, j;

	WD_PCI_CARD_INFO deviceInfo;



	double SystemParameterValues [N_SYSTEM_PAR]                                        = {0};
	double ModuleParameterValues [PRESET_MAX_MODULES*N_MODULE_PAR]                     = {0};
	double ChannelParameterValues[PRESET_MAX_MODULES*N_CHANNEL_PAR*NUMBER_OF_CHANNELS] = {0};


	DWORD dwStatus, dwData;
	DWORD val;
	DWORD repCnt = 0;
	DWORD failCnt = 0;
	U32 *ADC_Data;//[IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS]; // DMA frame-buffer  
	U32 *LM_Data; // List mode DMA frame buffer
	U32 *dummy;
	U32 *tmpBuffer;
	char *parname;
	U16 offset, idx;
	U8 ModNum, ChanNum;
	double ADCsum;
	DWORD offsetDACsteps = 1024;

	FILE *pLMData = NULL;
	FILE *pTraceData = NULL;
	U32 cntFrameBuffer = 0;
	S32 dmaTimedOut = 0; // 0 DMA finished OK, -1 DMA timed out
	char c;
	char LMOutputFileName[PRESET_MAX_MODULES][128];
	char TraceOutputFileName[128];
	U32 isDSPActive = 0; // bit 13 of CSR: 1 run active, 0 run stopped
	U32 isDMAIdle = 0;   // bit 14 of CSR: 1 data ready (DMA is idle), 0 DMA is still processing the SG list
	unsigned char eepromEntry[6] = {EEPROM_ENTRY(0x0000, 0x00000000)};

	// Bandwidth test
	clock_t startClock0, endClock0;
	unsigned long int startClock1, endClock1;
	double timeDMA, volDMA;

	U32 dmaIdleTO = 100;

	U32* ModuleEvents = NULL;
	U32 numSpills = 0;
	U32 pollTimeMS = 0;
	U32 traceLengthSamples = 0;

	// Parameter I/O debug
	// KS DEBUG
	U16 idxArr[4];
	LARGE_INTEGER startTime, stopTime, duSec;
	LARGE_INTEGER timerFrequency;

	U16 RunType;


	/* System configuration */
#include "SystemConfig.c"

	//numSpills = atoi(argv[1]);
	//pollTimeMS = atoi(argv[2]);
	//traceLengthSamples = atoi(argv[3]);


	/* Boot all modules in the crate */
#ifdef TEST_READER
	Pixie_Boot_System(0x0);
#else
	if((retval = Pixie_Boot_System(0xE)) < 0) { // 0x2 FPGA only, 0x6 FPGA and DSP, 0xE for FPGA, DSP, and DSP parameters
		printf("Boot Pixie-4 failed, retval=%d\n", retval);
		//return(retval);
		failCnt++;
		sprintf(ErrMSG,"*ERROR* (Main): ****** Cycle %d, failure %d\n", repCnt, failCnt);
		Pixie_Print_MSG(ErrMSG,1);
		return(-1);
	}
	else {
		printf("... ");
		Pixie_Sleep(1);
		sprintf(ErrMSG, "*INFO* (Main) Boot Pixie-4 succeeded");
		Pixie_Print_MSG(ErrMSG,1);
		//sprintf(ErrMSG, "******* Cycle %d finished", repCnt);
		//Pixie_Print_MSG(ErrMSG,1);
	}
#endif
#ifdef BOOT_ONLY
	return(7);
#endif
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		sprintf(ErrMSG, "*INFO* (Main): MODULE %d dump...", ModNum);
		Pixie_Print_MSG(ErrMSG,1);

		for (i = 0; i < 64; i++) {
			val=(U32)0x0;
			Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+i , MOD_READ, 1, &val);
			printf("DSPpar(%d)=%d\n", i, val);
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

		for (i = 0; i < N_SYSTEM_PAR; i++) {
			printf("System parameter %s = %f\n", System_Parameter_Names[i], System_Parameter_Values[i]);
		}

		printf("dev. %d=0x%08X\n", ModNum, hDev[ModNum]);
	} // for module




#ifdef TEST_EEPROM
	printf("Read EEPROM\n");
	for (i = 0; i < 13; i++) {
		dwData = 0xDEADBEEF;
		PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, i);
		sprintf(ErrMSG, "EEPROM: 0x%08X", dwData);
		Pixie_Print_MSG(ErrMSG,1);
	}
	//*eepromEntry = EEPROM_ENTRY(0xF104, 0xDEADA100);

	PIXIE500E_WriteI2C_Word(hDev[ModNum], eepromEntry, 11);

	printf("Read updated EEPROM\n");
	for (i = 0; i < 13; i++) {
		dwData = 0xDEADBEEF;
		PIXIE500E_ReadI2C_Word(hDev[ModNum], &dwData, i);
		sprintf(ErrMSG, "EEPROM: 0x%08X", dwData);
		Pixie_Print_MSG(ErrMSG,1);
	}
	return(3);
#endif
#ifdef LOAD_DEFAULT_EEPROM
	return(42);
#endif

#ifdef TEST_BAR0
	// check interrupts
	PIXIE500E_InterruptTest(hDev[0]);
	for (repCnt = 0; repCnt < 1; repCnt++) {

		//if (repCnt % 10 ==0 ) printf("... in cycle %d\n", repCnt);

		////
		////
		//// Assert RSTOUT
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		//i &= ~(0xC000);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_WRITE, &i, FALSE);
		////Read back to ensure write completion
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		//// Disable Local Bus (the problem with BAR0?)
		//i &=~(0x3000); 
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_WRITE, &i, FALSE);
		////Read back to ensure write completion
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		//// Enable Local Bus
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		//i |=0x1000; 
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_WRITE, &i, FALSE);
		////Read back to ensure write completion
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		//// De-assert RSTOUT
		//i |= 0x4000;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_WRITE, &i, FALSE);
		////Read back to ensure write completion
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], PCI_SYS_CFG_SYSTEM, WDC_READ, &i, FALSE);
		////

		///******************************/
		//Pixie_Sleep(10); // Having this delay after LB reset if important for BAR0 staying alive
		//for (j = 0; j < 10; j++) {
		//	dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CLK_CSR, WDC_READ, &i, FALSE);
		//	Pixie_Sleep(1);
		//	if (i != 0xE001f07c) {
		//		sprintf(ErrMSG, "*ERROR* CLK_CSR=0x%08X value FAIL, before BAR0 read, cycle %0d", i, repCnt);
		//		Pixie_Print_MSG(ErrMSG,1);
		//		//PIXIE500E_IntDisable(hDev[ModNum]);
		//		PIXIE500E_DeviceClose(hDev[ModNum]);
		//		exit(7);
		//	}
		//}
		///******************************/



		//// As a test, once FPGA is booted, BAR0 registers can be acceseed with Pixie_Register_IO()
		////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//Pixie_Register_IO(0, 0x60, MOD_READ, &dwData);
		////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//if (dwData == 0xFFFFFFFF) {
		//	sprintf(ErrMSG, "*ERROR* BAR0 read FAIL, cycle %0d", repCnt);
		//	Pixie_Print_MSG(ErrMSG,1);
		//	//PIXIE500E_IntDisable(hDev[ModNum]);
		//	PIXIE500E_DeviceClose(hDev[ModNum]);
		//	exit(666);
		//}
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], DMA_PAYLOAD_SIZE, WDC_READ, &i, FALSE);
		////$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		//if (dwData == 0xFFFFFFFF) {
		//	sprintf(ErrMSG, "*ERROR* BAR0 read FAIL, cycle %0d", repCnt);
		//	Pixie_Print_MSG(ErrMSG,1);
		//	//PIXIE500E_IntDisable(hDev[ModNum]);
		//	PIXIE500E_DeviceClose(hDev[ModNum]);
		//	exit(999);
		//}

		//for (j = 0; j < 10; j++) {
		//	dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], CLK_CSR, WDC_READ, &i, FALSE);
		//	Pixie_Sleep(1);
		//	if (i != 0xE001f07c) {
		//		sprintf(ErrMSG, "*ERROR* CLK_CSR=0x%08X value FAIL, after BAR0 read, cycle %0d", i, repCnt);
		//		Pixie_Print_MSG(ErrMSG,1);
		//		//PIXIE500E_IntDisable(hDev[ModNum]);
		//		PIXIE500E_DeviceClose(hDev[ModNum]);
		//		exit(8);
		//	}
		//}
		///******************************/




		// Do the DMA P2L-SDRAM-L2P test
		// Trying to evaluate throughput:
		// VDMA sequencer has 2K-work instruction memory;
		// We keep both P2L and L2P SG list in there, so only half is used for one type.)
		// Plus, because some data is left in FIFOs, so L2P has to be forced to time out, while waiting
		// for L2P to become idle, thus I'm tricking it to use only half of L2P pages.
		// Assume that 1800 instructions, 900 for only one type, 300 SG entries.
		// Pushing 300 * 4KB = 1200 KB = 1.17 MB P2L, 150 * 4KB = 600 KB = 0.59 MB L2P
		//PIXIE500E_DMA_SDRAM_Test(hDev[ModNum],4096/* 4096*300*/, TRUE, TRUE);

	} // for repCnt
	sprintf(ErrMSG,"*INFO* (Main) Success, %d cycles", repCnt);
	Pixie_Print_MSG(ErrMSG,1);
	//PIXIE500E_IntDisable(hDev[ModNum]);



#endif


#ifdef TEST_BAR2
	(U32*) tmpBuffer = malloc(65536*sizeof(U32));
	memset(tmpBuffer, 0, 65536*sizeof(U32));
	for (i = 0; i < 65536; i++) *(tmpBuffer+i) = i;
	Pixie_Acquire_Data(0x9002, (U32*)tmpBuffer, "", 0); // write
	memset(tmpBuffer, 0xFFFF, 65536*sizeof(U32));
	Pixie_Acquire_Data(0x9001, tmpBuffer, "", 0); // read
	for (i = 0; i < 65536; i++) {
		printf("EM data %08X 0x%08X\n", i, tmpBuffer[i]);
		if (tmpBuffer[i] != i ) printf("****************************************************\n");
	}

#endif

	//dwData  =0x851E;
	//Pixie_IODM(0, DATA_MEMORY_ADDRESS+Find_Xact_Match("TRACKDAC0", DSP_Parameter_Names, N_DSP_PAR), MOD_WRITE, 1, &dwData);

	//retval = Control_Task_Run(0, SET_DACS, 10000);
	//if(retval < 0)
	//{
	//	sprintf(ErrMSG, "*ERROR* (MAIN): failure to set DACs in Module %d", 0);
	//	Pixie_Print_MSG(ErrMSG,1);
	//	return(-3);
	//}
	//Pixie_Sleep(100);

	///* Program signal processing FPGAs */
	//retval = Control_Task_Run(0, PROGRAM_FIPPI, 1000);
	//if(retval < 0)
	//{
	//	sprintf(ErrMSG, "*ERROR* (MAIN): failure to program Fippi in Module %d", 0);
	//	Pixie_Print_MSG(ErrMSG,1);
	//	return(-4);
	//}

	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		// change polarity
		idx = Find_Xact_Match("CHANCSRA0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("chanCSRA=: %d\n", dwData);
		//// Clear INVERT bit (bit 5)
		//dwData &= ~(0x20);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("New chanCSRA=: %d\n", dwData);	
		// For NaI with Vega need to invert:
		//Set INVERT bit (bit 5)
		//dwData |= (0x20);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("New chanCSRA=: %d\n", dwData);

		/* Program signal processing FPGAs */
		retval = Control_Task_Run(ModNum, PROGRAM_FIPPI, 1000);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (MAIN): failure to program Fippi in Module %d", 0);
			Pixie_Print_MSG(ErrMSG,1);
			return(-4);
		}
	} // for modules
	Pixie_Sleep(100);


	//// test pattern for GET_TRACES
	//idx = Find_Xact_Match("HOSTIO", DSP_Parameter_Names, N_DSP_PAR);
	//dwData = 0x2;
	//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
	////retval = Start_Run(0, NEW_RUN, 0, GET_TRACES);
	//Pixie_Sleep(2);

	// ramp for GET_TRACES
	//idx = Find_Xact_Match("HOSTIO", DSP_Parameter_Names, N_DSP_PAR);
	//dwData = 0x1;
	//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);

#ifdef TEST_DSPPAR

	for (repCnt = 0; repCnt < 1; repCnt++) {
		for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
			idxArr[0] = Pixie_Get_Par_Idx("ENERGY_RISETIME", "CHANNEL");
			idxArr[1] = Pixie_Get_Par_Idx("ENERGY_FLATTOP", "CHANNEL");
			idxArr[2] = Pixie_Get_Par_Idx("TRIGGER_RISETIME", "CHANNEL");
			idxArr[3] = Pixie_Get_Par_Idx("TRIGGER_FLATTOP", "CHANNEL");

			printf("$$$ DSPpar test %d\n", repCnt);
			QueryPerformanceFrequency(&timerFrequency);


			for (i = 0; i < 1000000; i++) {
				//val=(U32)0x0000FFFF;

				// READ
				QueryPerformanceCounter(&startTime);
				Pixie_IODM(0, DATA_MEMORY_ADDRESS/*+i*/ , MOD_READ, 1, &val);
				QueryPerformanceCounter(&stopTime);
				duSec.QuadPart = stopTime.QuadPart - startTime.QuadPart; // elapsed number of ticks.
				duSec.QuadPart *= 1000000;
				duSec.QuadPart /= timerFrequency.QuadPart;
				//printf("READ: %ld usec (QPC)\n", duSec);

				val = (U32)0x0000FFFF;

				// WRITE
				QueryPerformanceCounter(&startTime);
				Pixie_IODM(0, DATA_MEMORY_ADDRESS/*+i*/ , MOD_WRITE, 1, &val);
				QueryPerformanceCounter(&stopTime);
				duSec.QuadPart = stopTime.QuadPart - startTime.QuadPart; // elapsed number of ticks.
				duSec.QuadPart *= 1000000;
				duSec.QuadPart /= timerFrequency.QuadPart;
				//printf("WRITE: %ld usec (QPC)\n", duSec);

				wait_for_a_short_time(1000); // getting 4 us using QPC for 1000 cycles on Z200.
				val = (U32)0x0;
				Pixie_IODM(0, DATA_MEMORY_ADDRESS/*+i*/ , MOD_READ, 1, &val);
				//printf("READ DSPpar(%d)=0x%08X\n", i, val);
				if (val!=0x0000FFFF) printf("\t\t\tBit 13 error\n");


				// KS DEBUG: Test of ENERGY_RISETIME write time.
				//startClock0 = clock();
				//UA_CHANNEL_PAR_IO (ChannelParameterValues, "ENERGY_RISETIME", MOD_WRITE, ModNum, 0);
				//endClock0 = clock();
				//printf("\n");
				//printf("\t@@ WRITE  %s :%f\n", "ENERGY_RISETIME", ((double)(endClock0-startClock0)/CLOCKS_PER_SEC));
			} // end for parameters


		} // for modules
	} // for repCnt

#endif


#ifdef TEST_ADCTRACE
	//Upon boot, try to start runs, etc...
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		ChanNum = 0;
		printf("\nAdjusting offsets for module %d channel %d ...", ModNum, ChanNum);



		offset  = ModNum*N_CHANNEL_PAR*NUMBER_OF_CHANNELS + ChanNum * N_CHANNEL_PAR;
		idx = Pixie_Get_Par_Idx("BASELINE_PERCENT", "CHANNEL");

		Pixie_User_Par_IO(ChannelParameterValues, "BASELINE_PERCENT", "CHANNEL", MOD_READ, ModNum, ChanNum);
		printf("Initial offset %f\n", ChannelParameterValues[offset+idx]);

	
		// Trace before offset adjust
		sprintf(TraceOutputFileName, "ADCdata%02d_0.txt", ModNum);
		pTraceData = fopen(TraceOutputFileName, "w");

		ADC_Data = malloc(IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));

		printf("************************* TRACE0 ********************\n");
		memset(ADC_Data, 0x0000, IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
		// Set up DMA
		// Start GET_TRACES DSP task, put data into SDRAM 
		// Perform DMA transfer
		retval = Pixie_Acquire_Data(ACQUIRE_ADC_TRACES, ADC_Data, "", ModNum); // get into a control task, push data into SDRAM
		if (retval < 0) {
			sprintf(ErrMSG, "*ERROR* (MAIN) Pixie-500e failed to ADC trace for module %d", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			fclose(pTraceData);
			free(ADC_Data);
			return (retval);
		}
		else {
			for (i = 0; i < IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS; i++) {
				//sprintf(ErrMSG, "*INFO* (MAIN) Successfully acquired trace, ADC[0x%08X]= 0x%08X", i, ADC_Data[i]);
				//Pixie_Print_MSG(ErrMSG,1);
				fprintf(pTraceData, /*"0x%08X\n"*/"%d\n", ADC_Data[i]);
			}
		}
		fclose(pTraceData);
		free(ADC_Data);

		// ADJUST OFFSET


		if((retval = Pixie_Acquire_Data(ADJUST_OFFSETS_DSP, NULL, "", ModNum)) < 0){
			printf("Adjust offsets failed, retval=%d\n", retval);
			return(retval);
		}
		printf(" Done!\n");

		Pixie_User_Par_IO(ChannelParameterValues, "BASELINE_PERCENT", "CHANNEL", MOD_READ, ModNum, ChanNum);
		printf("Final offset %f\n", ChannelParameterValues[offset+idx]);

		// Trace after offset adjust
		sprintf(TraceOutputFileName, "ADCdata%02d_1.txt", ModNum);
		pTraceData = fopen(TraceOutputFileName, "w");

		ADC_Data = malloc(IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));

		printf("************************* TRACE1 ********************\n");
		memset(ADC_Data, 0x0000, IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
		// Set up DMA
		// Start GET_TRACES DSP task, put data into SDRAM 
		// Perform DMA transfer
		retval = Pixie_Acquire_Data(ACQUIRE_ADC_TRACES, ADC_Data, "", ModNum); // get into a control task, push data into SDRAM
		if (retval < 0) {
			sprintf(ErrMSG, "*ERROR* (MAIN) Pixie-500e failed to ADC trace for module %d", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			fclose(pTraceData);
			free(ADC_Data);
			return (retval);
		}
		else {
			for (i = 0; i < IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS; i++) {
				//sprintf(ErrMSG, "*INFO* (MAIN) Successfully acquired trace, ADC[0x%08X]= 0x%08X", i, ADC_Data[i]);
				//Pixie_Print_MSG(ErrMSG,1);
				fprintf(pTraceData, /*"0x%08X\n"*/"%d\n", ADC_Data[i]);
			}
		}
		fclose(pTraceData);
		free(ADC_Data);

	} // for modules
#endif


#ifdef TEST_DMA


	//pTraceData = fopen("AAA.txt","wb");
	tmpBuffer = malloc(DMA_LM_FRAMEBUFFER_LENGTH);
	//ADC_Data = malloc(DMA_LM_FRAMEBUFFER_LENGTH*128);
	pDmaTrace = NULL;
	retval = PIXIE500E_DMA_Trace_Setup(hDev[0], DMA_LM_FRAMEBUFFER_LENGTH, tmpBuffer, &pDmaTrace);
	retval = PIXIE500E_DMA_Init(hDev[0]);
	retval = WDC_DMASyncCpu(pDmaTrace);

	while (1) {
		pTraceData = fopen("AAA.txt","wb");
		cntFrameBuffer = 0;
		timeDMA = 0;
		volDMA = 0;
		printf("Push to start ramp..."); getchar();
		for (j = 0; j < 1; j++) {
			retval = Start_Run(0, NEW_RUN, 0, 0x200);
			Pixie_Sleep(100);
			//retval = End_Run(0); //(stops anyway)
			printf("128 MB of Data in SDRAM\n");
		}
		printf("Push to start DMA..."); getchar(); 

		VDMADriver_SetDPTR(hDev[0], MAIN_START);
		startClock0 = clock();
		startClock1 = GetTickCount();
		do { // Reading out data from SDRAM
			memset(tmpBuffer, 0, DMA_LM_FRAMEBUFFER_LENGTH);
			//startClock0 = clock();
			VDMADriver_Go(hDev[0]);
			retval = PIXIE500E_DMA_WaitForCompletion(hDev[0], TRUE); // poll for idle, halt if busy too long

			//fwrite(tmpBuffer, DMA_LM_FRAMEBUFFER_LENGTH, 1, pTraceData);

			if (retval==WD_STATUS_SUCCESS) { // DMA is now idle, no timeout
				cntFrameBuffer++;

				// Throughput measurement
				endClock0 = clock();
				endClock1 = GetTickCount();
				timeDMA = ((double)(endClock1-startClock1)/CLOCKS_PER_SEC);
				volDMA = cntFrameBuffer*DMA_LM_FRAMEBUFFER_LENGTH/(1024.*1024); // in MB
				printf("\t%03d: ", cntFrameBuffer);
				printf("@ %10u\t", (unsigned long)(cntFrameBuffer*DMA_LM_FRAMEBUFFER_LENGTH)); // position in bytes
				// Check SDRAM status
				dwStatus = PIXIE500E_ReadWriteReg(hDev[0], APP_SDRAM_STATUS, WDC_READ, &dwData, FALSE);
				printf("diff=%10d,Full %d\t", (dwData & 0x3FFFFF00) >> 8, (dwData & 0x1));
				printf("%10f MB/%f sec, speed= %f MB/sec\n", volDMA, timeDMA, volDMA/timeDMA);

				//printf("Press to rewind sequencer..."); getchar();
				VDMADriver_SetDPTR(hDev[0], MAIN_START);
				// save data (!!!)
				fwrite(tmpBuffer, DMA_LM_FRAMEBUFFER_LENGTH, 1, pTraceData);

				//memcpy((ADC_Data+DMA_LM_FRAMEBUFFER_LENGTH*cntFrameBuffer), tmpBuffer, DMA_LM_FRAMEBUFFER_LENGTH);
				//fwrite(ADC_Data, DMA_LM_FRAMEBUFFER_LENGTH, 1, pTraceData);

			}
		} while (retval != WD_WINDRIVER_STATUS_ERROR && cntFrameBuffer < 63);

		endClock0 = clock();
		endClock1 = GetTickCount();
		//fwrite(ADC_Data, DMA_LM_FRAMEBUFFER_LENGTH, cntFrameBuffer, pTraceData);
		timeDMA = ((double)(endClock1-startClock1)/CLOCKS_PER_SEC);
		volDMA = cntFrameBuffer*DMA_LM_FRAMEBUFFER_LENGTH/(1024.*1024); // in MB
		printf("%10f MB/%f sec, speed= %f MB/sec\n", volDMA, timeDMA, volDMA/timeDMA);

		printf("DMA all done\n");
		fclose(pTraceData);
		printf("Look at data and press to continue...");getchar();
	}; // while going in circles
	retval = WDC_DMASyncIo(pDmaTrace);
	retval = WDC_DMABufUnlock(pDmaTrace);
	// To make the flush data (64 words at the end of the run from DSP) disappear.
	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], APP_SDRAM_STATUS, WDC_READ, &dwData, FALSE);
	// Re-init SDRAM, assert bit 3
	dwData |= 0x8;
	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], APP_SDRAM_STATUS, WDC_WRITE, &dwData, FALSE);
	// Reset FIFOs
	VDMADriver_EventSet(hDev[0], 0x400);

	free(tmpBuffer);
	//free(ADC_Data);
	fclose(pTraceData);

	return(8);
#endif

#ifdef TEST_IRQ

	//PIXIE500E_InterruptTest_SWI0(hDev[0]);
	PIXIE500E_InterruptTest_INT3(hDev[0]);

	//VDMADriver_SetEventEnable(hDev[0], 0x0); // Event emitted by the sequencer, all disabled
	//// Clear all INT_CFG
	//i = 0x0;
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG0, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG1, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG2, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG3, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG4, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG5, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG6, WDC_WRITE, &i, FALSE);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG7, WDC_WRITE, &i, FALSE);

	//i = 0xFF; // Disable interrupt on GPIO[7:0]
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_MASK_SET, WDC_WRITE, &i, FALSE);

	//i = 0x0; // Not in bypass mode
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_BYPASS_MODE, WDC_WRITE, &i, FALSE);

	//i = 0x0;  // Disable all GPIO output
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_OUTPUT_ENABLE, WDC_WRITE, &i, FALSE);

	//i = 0x04; // GPIO[7:0] as input
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_DIRECTION_MODE, WDC_WRITE, &i, FALSE);

	//i = 0; // Set GPIO[7:0] to edge trigger
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_TYPE, WDC_WRITE, &i, FALSE);

	//i = 0; // rising edge trigger
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_ON_ANY, WDC_WRITE, &i, FALSE);

	//i = 0x4; // Set GPIO[7:0] to rising edge trigger
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_VALUE, WDC_WRITE, &i, FALSE);

	//VDMADriver_EventClear(hDev[0], 0x2);

	//i = VDMADriver_GetEventStatus(hDev[0]); // Read VDMA_EVENT register to flush the settings

	//VDMADriver_SetEventEnable(hDev[0], 0x2);

	/* Read to clear GPIO_INT_STATUS.  There is a delay in initializing the
	GPIO signal level from the FPGA, thus a loop read is required. */
	//do
	//{
	//	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_STATUS, WDC_READ, &i, FALSE);
	//}while(i);

	//Read INT_STAT to clear up existing interrupts
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_READ, &i, FALSE);

	//i = 0x4; // Enable interrupt on GPIO[7:0]
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_MASK_CLR, WDC_WRITE, &i, FALSE);



	// GPIO IRQ test
	//i = 0x8000; // GPIO enable to CFG0 (CFG4--7 are routed to GPIO block) (really?) (only CFG0 works)
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG0, WDC_WRITE, &i, FALSE);

	// SWI0 test
	//i = 0x4; // SWI0
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_CFG0, WDC_WRITE, &i, FALSE);

	//PIXIE500E_IntEnable(hDev[0], TestIntHandler_SWI0);

	/* Unmask GPIO interrupts and re-enable VDMA_EVENT. This step is required
	because we always mask out GPIO interrupt and/or external interrupt (via
	VDMA_EVENT_EN register) in the kernel level ISR when these interrupts are
	enabled.  For the current test design, the previous trigger of SW interrupt
	has mask out the GPIO / external interrupts.
	See the kernel level ISR implementation for details.
	*/
	//i = 0x04;
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_MASK_CLR, WDC_WRITE, &i, FALSE);
	//VDMADriver_SetEventEnable(hDev[0], 0x02);


	//printf("********SETTING EVENT **************\n");
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_READ, &i, FALSE);
	//printf("Before event set, INT_STAT=0x%08X\n", i);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INPUT_VALUE, WDC_READ, &i, FALSE);
	//printf("Before event set, GPIO_INPUT_VALUE=0x%08X\n", i);
	//Pixie_Sleep(10);
	//i = VDMADriver_GetEventStatus(hDev[0]);
	//printf("Before event set, EventStatus=0x%08X\n", i);
	//// Set the FPGA output HI by setting VDMA_EVENT 
	//VDMADriver_EventSet(hDev[0], 0x2); //assert GPIO interrupt
	////i = 0x8; // assert SW interrupt
	////dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_WRITE, &i, FALSE);
	//Pixie_Sleep(10);
	//i = VDMADriver_GetEventStatus(hDev[0]);
	//printf("After event set, EventStatus=0x%08X\n", i);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_READ, &i, FALSE);
	//printf("After event set, INT_STAT=0x%08X\n", i);
	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INPUT_VALUE, WDC_READ, &i, FALSE);
	//printf("After event set, GPIO_INPUT_VALUE=0x%08X\n", i);

	//printf("Press to start..."); getchar();
	//pDev = (PWDC_DEVICE)hDev[0];
	//while (gIntCounter < 10) {
	//	// Assert SWI0
	//	i = 0x0;
	//	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_READ, &i, FALSE);
	//	i |=(0x4);
	//	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_WRITE, &i, FALSE);

	//	// Main Interrupt
	//	dwStatus = PIXIE500E_ReadWriteReg(hDev[0], INT_STAT, WDC_READ, &i, FALSE);
	//	printf("MAIN: loop INT_STAT=0x%08X\n", i);
	//	printf("MAIN: loop gIntCounter = %d\n", gIntCounter);
	//	printf("Press for next..."); getchar();
	//	LMBufferCounter[0] = pDev->Int.dwCounter;
	//	gIntCounter = LMBufferCounter[0];

	//	//// clear GPIO IRQ by reading GPIO_INT_STATUS
	//	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INT_STATUS, WDC_READ, &i, FALSE);
	//	//printf("MAIN: GPIO_INT_STATUS=0x%08X\n", i);

	//	//dwStatus = PIXIE500E_ReadWriteReg(hDev[0], GPIO_INPUT_VALUE, WDC_READ, &i, FALSE);
	//	//printf("MAIN: GPIO_INPUT_VALUE=0x%08X\n", i);
	//}

#endif

#ifdef TEST_OFFSETDAC
	//Upon boot, try to start runs, etc...
	pTraceData = fopen("OffsetDAC.txt", "w");
	for (repCnt = 0; repCnt <offsetDACsteps; repCnt++) {
		dwData  = repCnt*0xFFFF/offsetDACsteps;
		idx = Find_Xact_Match("TRACKDAC0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);

		retval = Control_Task_Run(0, SET_DACS, 1000);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (MAIN): failure to set DACs in Module %d", 0);
			Pixie_Print_MSG(ErrMSG,1);
			return(-3);
		}
		Pixie_Sleep(1);

		//// change polarity
		//	idx = Find_Xact_Match("CHANCSRA0", DSP_Parameter_Names, N_DSP_PAR);
		//	Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//	printf("chanCSRA=: %d\n", dwData);
		//	// Clear INVERT bit (bit 5)
		//	dwData &= ~(0x20);
		//	Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//	Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//	printf("New chanCSRA=: %d\n", dwData);	

		//	/* Program signal processing FPGAs */
		//	retval = Control_Task_Run(0, PROGRAM_FIPPI, 1000);
		//	if(retval < 0)
		//	{
		//		sprintf(ErrMSG, "*ERROR* (MAIN): failure to program Fippi in Module %d", 0);
		//		Pixie_Print_MSG(ErrMSG,1);
		//		return(-4);
		//	}






		printf("************************* OFFSETDAC CYCLE %d ********************\n", repCnt);
		ADCsum = 0;
		ADC_Data = malloc(IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
		memset(ADC_Data, 0x0000, IO_BUFFER_LENGTH*NUMBER_OF_CHANNELS*sizeof(U32));
		// Set up DMA
		// Start GET_TRACES DSP task, put data into SDRAM 
		// Perform DMA transfer
		retval = Pixie_Acquire_Data(ACQUIRE_ADC_TRACES, ADC_Data, "", 0); // get into a control task, push data into SDRAM
		if (retval < 0) {
			sprintf(ErrMSG, "*ERROR* (MAIN) Pixie-500e failed to ADC trace");
			Pixie_Print_MSG(ErrMSG,1);
		}
		else {
			for (i = 0; i < IO_BUFFER_LENGTH; i++) {
				//sprintf(ErrMSG, "*INFO* (MAIN) Successfully acquired trace, ADC[0x%08X]= 0x%08X", i, ADC_Data[i]);
				//Pixie_Print_MSG(ErrMSG,1);
				//fprintf(pTraceData, "%d\n", ADC_Data[i]);
				ADCsum += ADC_Data[i];
			}
		}
		ADCsum  = ADCsum/IO_BUFFER_LENGTH;
		fprintf(pTraceData, "%d %f\n", repCnt*0xFFFF/offsetDACsteps, ADCsum);
		printf("%d %f\n", repCnt*0xFFFF/offsetDACsteps, ADCsum);
	} // for DAC ramp
	fclose(pTraceData);
#endif




#ifdef TEST_LISTMODE
	printf("List Mode Test\n");
	// Prepare for List Mode Run 0x400
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		//idx = Find_Xact_Match("FASTTHRESH0", DSP_Parameter_Names, N_DSP_PAR);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("Threshold= %d\n", dwData);
		//dwData = 1000;
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("New Threshold= %d\n", dwData);



		idx = Find_Xact_Match("FASTLENGTH0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d Fast Length= %d\n", ModNum, dwData);
		//dwData = 4;
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("new Fast Length= %d\n", dwData);

		idx = Find_Xact_Match("FASTGAP0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d Fast Gap= %d\n", ModNum, dwData);
		//dwData = 0;
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("new Fast Gap= %d\n", dwData);

		idx = Find_Xact_Match("SLOWLENGTH0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d Slow Length= %d\n", ModNum, dwData);
		//dwData = 4;
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("new Slow Length= %d\n", dwData);

		idx = Find_Xact_Match("SLOWGAP0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d Slow Gap= %d\n", ModNum, dwData);
		//dwData = 0;
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("new Slow Gap= %d\n", dwData);

		idx = Find_Xact_Match("FILTERRANGE", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d FILTERRANGE= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("COINCPATTERN", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d COINCPATTERN= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("USERDELAY0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d USERDELAY0= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("PEAKSEP0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d PEAKSEP0= %d\n", ModNum, dwData);
/*************************************************************************/
/* TRACE LENGTH */
		idx = Find_Xact_Match("TRACELENGTH0", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d TRACELENGTH0= %d\n", ModNum, dwData);
		dwData = traceLengthSamples;
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d new TRACELENGTH0= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("TRACELENGTH1", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d TRACELENGTH1= %d\n", ModNum, dwData);
		dwData = traceLengthSamples;
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d new TRACELENGTH1= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("TRACELENGTH2", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d TRACELENGTH2= %d\n", ModNum, dwData);
		dwData = traceLengthSamples;
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d new TRACELENGTH2= %d\n", ModNum, dwData);

		idx = Find_Xact_Match("TRACELENGTH3", DSP_Parameter_Names, N_DSP_PAR);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d TRACELENGTH3= %d\n", ModNum, dwData);
		dwData = traceLengthSamples;
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_WRITE, 1, &dwData);
		Pixie_IODM(ModNum, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		printf("%d new TRACELENGTH3= %d\n", ModNum, dwData);
/******************************************************************/
		//idx = Find_Xact_Match("CHANCSRA0", DSP_Parameter_Names, N_DSP_PAR);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("CHANCSRA0= %d\n", dwData);
		//idx = Find_Xact_Match("CHANCSRA1", DSP_Parameter_Names, N_DSP_PAR);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("CHANCSRA1= %d\n", dwData);
		//idx = Find_Xact_Match("CHANCSRA2", DSP_Parameter_Names, N_DSP_PAR);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("CHANCSRA2= %d\n", dwData);
		//idx = Find_Xact_Match("CHANCSRA3", DSP_Parameter_Names, N_DSP_PAR);
		//Pixie_IODM(0, DATA_MEMORY_ADDRESS+idx, MOD_READ, 1, &dwData);
		//printf("CHANCSRA3= %d\n", dwData);

		/* Program signal processing FPGAs */
		retval = Control_Task_Run(ModNum, PROGRAM_FIPPI, 1000);
		if(retval < 0)
		{
			sprintf(ErrMSG, "*ERROR* (MAIN): failure to program Fippi in Module %d", ModNum);
			Pixie_Print_MSG(ErrMSG,1);
			return(-4);
		}

		dummy = NULL;
		// Allocate memory for parsing results.
		ModuleEvents = calloc(PRESET_MAX_MODULES, sizeof(U32));

		// PIXIE500E_InterruptSetup_INT3 and IntHandler_INT3
		// MOVED TO pixie500e_lib.c
		//// Prepare for interrupt processing
		//// Clear all INT_CFG
		//i = 0x0;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG0, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG1, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG2, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG3, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG4, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG5, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG6, WDC_WRITE, &i, FALSE);
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG7, WDC_WRITE, &i, FALSE);

		//// For interrupt type GPIO_BYPASS_INT_EDGE

		//// All for only one INT0
		//// Disable generation of GPIO_INT in INT_STAT to prevent interrupt detection by the GPIO block.
		//// (masked by GPIO_INT_MASK, GPIO_INT is disabled)
		//i = 0xFFFF;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], GPIO_INT_MASK_SET, WDC_WRITE, &i, FALSE);

		//// Rising edge trigger in INT3--0 (INT3, actually)
		//i = 0x8;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CTRL, WDC_WRITE, &i, FALSE);

		//// GPIO INT3--0 in bypass mode. Only INT3!
		//i = 0x8;//0xF;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], GPIO_BYPASS_MODE, WDC_WRITE, &i, FALSE);

		//VDMADriver_EventClear(hDev[ModNum], 0xFF);
		//VDMADriver_GetEventStatus(hDev[ModNum]); 

		//VDMADriver_SetEventEnable(hDev[ModNum], 0x08); // enable VDMA_EVENT[7:0] to allow output on GPIO[7:0]

		//// clear interrupts (not really...)
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_STAT, WDC_READ, &i, FALSE);
		//i = 0xFFFFFFF3;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_STAT, WDC_WRITE, &i, FALSE);

		//// Enable INT0-3 on CFG0
		//i = 0x80; // Bit 7: INT3
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_CFG0, WDC_WRITE, &i, FALSE);

		//// General interrupt setup and ISR init
		//// TODO: but how IntHandler_INT3 gets its arguments???
		//PIXIE500E_IntEnable(hDev[ModNum], IntHandler_INT3);

		//i = 0xFFFF;
		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], GPIO_INT_MASK_CLR, WDC_WRITE, &i, FALSE);
		//VDMADriver_SetEventEnable(hDev[ModNum], 0xFF);

		////myEvent = 0x08; // use event 0x08 with INT3

		//dwStatus = PIXIE500E_ReadWriteReg(hDev[ModNum], INT_STAT, WDC_READ, &i, FALSE);
		//printf("PIXIE500E_InterruptTest: ready to assert INT_STAT=0x%08X\n", i);


	} // for modules


	for (repCnt = 0; repCnt<1; repCnt++) {

		printf("...Run # %d...\n", repCnt);
		cntFrameBuffer = 0;
		/*
		THIS IS NEEDED FOR CLEAN OUT EXTRA ZEROES AT THE BEGINNING?
		Without asserted bit 3 in APP_SDRAM_STATUS and no DMA event 0x400:
		the very first list-mode run each DMA buffer has 8 32-bit zeroes at start.
		With only bit 3 set in APP_SDRAM_STATUS: same.
		With only event 0x400: OK -- move VDMA_Driver_EventSet(hDev, 0x400) to DMA_Init()
		--- seems OK with DSP SDRAM preload.
*/
	// To make the flush data (64 words at the end of the run from DSP) disappear.
		//retval = PIXIE500E_ReadWriteReg(hDev[0], APP_SDRAM_STATUS, WDC_READ, &val, FALSE);
		// Re-init SDRAM, assert bit 3
		//val |= 0x8;
		//retval = PIXIE500E_ReadWriteReg(hDev[0], APP_SDRAM_STATUS, WDC_WRITE, &val, FALSE);
		//Pixie_Sleep(10);
		// Reset FIFOs
		//VDMADriver_EventSet(hDev[0], 0x400);
		//Pixie_Sleep(10);
//// ZEROES?
		Pixie_Sleep(10);
// NB: testing threshold effect on number of errors.
/*
Changing lo and hi threshold with step of 32.
CAP_WR_BRG_WR CAP_WR_BRG_RD CAP_RD_BRG_WR CAP_RD_BRG_RD Err_Event
480/20        480/16-256    480/20        480/20        62
480/20        480-256/16    480/20        480/20        62, 190
480/20        480-256/20    480/20        480/20        62, 190
460/20        500-276/20    460/20        460/20        none
460/20        500/20-244    460/20        460/20        none
--not CAP_WR_BRG_RD?
460/20        460/20        500/20-244    460/20        lots with each setting
460/20        460/20        500-276/20    460/20        62, 273, 321 only at 500
460/20        460/20        460/2-226     460/20        none 
460/20        460/20        460/20        460/20        none short (but junk in beginning!)
460/20        460/20        500-276/20    460/20        
*/
		gThr_lo = 20;//+repCnt*32;
		gThr_hi =460;//500-repCnt*32;
		printf("%%%%%%%%%%%%%%%%%%%%%%%\t thr: %d %d\n", gThr_lo, gThr_hi);
		// create a file to assure that for module 0 it is always fresh.
		for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
			sprintf(LMOutputFileName[ModNum], "AAA_LMdata_run%04d.b%02d", repCnt, ModNum);
			pLMData = fopen(LMOutputFileName[ModNum], "wb"); // create  file for appending list mode data
			fclose(pLMData);
		} // for modules
		Pixie_Sleep(10);
		RunType = 0x400;
		AutoProcessLMData = 3;
			// Init DMA, open bin file for appending,
			// Start LM run in DSP.
			// Start DMA.
		sprintf(ErrMSG, "*INFO* (MAIN): Into 0x1000!");
		Pixie_Print_MSG(ErrMSG,1);
		// Start run in all modules (LMOutputFileName will be set in Pixie_Acquire_Data for each module)
		retval = Pixie_Acquire_Data(0x1000+RunType, dummy, LMOutputFileName[0], Number_Modules);

		Pixie_Sleep(100);

		if (retval < 0) {
			sprintf(ErrMSG, "*ERROR* (MAIN): failed to start run!");
			Pixie_Print_MSG(ErrMSG,1);
			PIXIE500E_Cleanup();
			return (-1);
		}


		QueryPerformanceFrequency(&timerFrequency);
		startClock0 = clock();
		QueryPerformanceCounter(&startTime);


		do  { // +++++++++++++++++++++ POLLING LOOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			Pixie_Sleep(pollTimeMS); // polling time

			// If using interrutps, data is recorded by the ISR, and we are only checking number
			// of framebuffers here using 0x4403
//#ifdef POLLING
//			if(Polling) 
			// Polling hardware for framebuffer completion, dump to disk
			retval = Pixie_Acquire_Data(0x4000+RunType, dummy, LMOutputFileName[0], Number_Modules);
//#else
			// else
//			// Getting framebuffer data and dump to disk is done by ISR, polling only updates UI 
//			retval = Pixie_Acquire_Data(0x4403, dummy, LMOutputFileName[0], Number_Modules);
//#endif
			// FIXME Sort out what happens if a module doesn't have any data coming out--check DMA timeouts!

			// retval is the accumulative number of frame buffers
			if (retval > 0) {
				cntFrameBuffer = retval;
				// throughput benchmark
				QueryPerformanceCounter(&stopTime);
				duSec.QuadPart = stopTime.QuadPart - startTime.QuadPart; // elapsed number of ticks.
				duSec.QuadPart *= 1000000;
				duSec.QuadPart /= timerFrequency.QuadPart;
				timeDMA = ((double)duSec.QuadPart / 1000000); // in sec
				volDMA = (cntFrameBuffer/1024.)*(DMA_LM_FRAMEBUFFER_LENGTH/1024.); // in MB
				if (/*cntFrameBuffer%10==0 && */cntFrameBuffer>=0) {
					printf("\t\t\t%d: ", cntFrameBuffer);
					printf("%f MB/%f sec, speed= %f MB/sec\n", volDMA, timeDMA, volDMA/timeDMA);
					sprintf(ErrMSG, "*INFO* (MAIN): done buffer %d ###########################", cntFrameBuffer);
					Pixie_Print_MSG(ErrMSG,1);
				}
			}

			// Condition for end of run: N spills
			if (cntFrameBuffer >= numSpills) {
				sprintf(ErrMSG, "*DEBUG* (MAIN): Going to stop run %d", repCnt);
				Pixie_Print_MSG(ErrMSG,1);
				//printf("Press to stop run...");getchar();

				retval = Pixie_Acquire_Data(0x3000+RunType, dummy,LMOutputFileName[0], Number_Modules); // stop run

				if (retval < 0) {
					sprintf(ErrMSG, "*ERROR* (MAIN): failed to stop run.");
					Pixie_Print_MSG(ErrMSG,1);
					PIXIE500E_Cleanup();
					return (-2);
				} // failure to stop
				else {
					// Normal run stop.
					sprintf(ErrMSG, "*INFO* (Main): run %d stopped.", repCnt);
					Pixie_Print_MSG(ErrMSG,1);

					// Check auto data processing here.
					AutoProcessLMData = 0;
					retval = Pixie_Acquire_Data(0x7001, ModuleEvents, LMOutputFileName[0], Number_Modules);
					sprintf(ErrMSG, "*INFO* (Main): run %d parsed.", repCnt);
					Pixie_Print_MSG(ErrMSG,1);

					break;
				} // if stop ok
			}
		} while (1); // ++++++++++++++++++++++++ END POLLING LOOP ++++++++++++++++++++++++++++++++++++++++++++++++++++++++


		endClock0 = clock();
		timeDMA = ((double)(endClock0-startClock0)/CLOCKS_PER_SEC);
		volDMA = cntFrameBuffer*DMA_LM_FRAMEBUFFER_LENGTH/(1024.*1024); // in MB
		printf("%f MB/%f sec, speed= %f MB/sec\n", volDMA, timeDMA, volDMA/timeDMA);
		printf("DMA done\n...");//getchar();
	} //repCnt, new run.
	//free(ModuleEvents); // release resources for parse results (getting errors on this?)

#endif

#ifdef TEST_READER
	// Allocate memory for parsing results.
	ModuleEvents = calloc(PRESET_MAX_MODULES, sizeof(U32));
	sprintf(LMOutputFileName[0], "AAA_LMdata_run0001_badTraceBlocks.b00");
	pLMData = fopen(LMOutputFileName[0], "rb"); // create  file for appending list mode data
	// Check auto data processing here.
	AutoProcessLMData = 0;
	retval = Pixie_Acquire_Data(0x7001, ModuleEvents, LMOutputFileName[0], 1);
	sprintf(ErrMSG, "*INFO* (Main): file %s parsed.", LMOutputFileName[0]);
	Pixie_Print_MSG(ErrMSG,1);
	fclose(pLMData);
#endif
#ifdef TEST_DMAPROG


	failCnt = 0;
	// Do setup/teardown only once:
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {
		// DMA setup
		pDmaList[ModNum] = NULL;
		LMBuffer[ModNum] = malloc(DMA_LM_FRAMEBUFFER_LENGTH);
		if (!LMBuffer[ModNum]) {
			sprintf(ErrMSG, "*ERROR* (MAIN): Memory allocation for list mode buffer failure");
			Pixie_Print_MSG(ErrMSG,1);
			return(-0x13);
		}
		memset(LMBuffer[ModNum], 0x69, DMA_LM_FRAMEBUFFER_LENGTH); 
		if (!LMBuffer[ModNum]) {
			sprintf(ErrMSG, "*ERROR* (MAIN): Memory allocation for list mode buffer copy failure");
			Pixie_Print_MSG(ErrMSG,1);
			return(-0x13);
		}

		//retval = PIXIE500E_DMA_Trace_Setup(hDev[ModNum],  DMA_LM_FRAMEBUFFER_LENGTH, LMBuffer[ModNum], &pDmaList[ModNum]);
		//if (retval != 0) {
		//	sprintf(ErrMSG, "*ERROR* (MAIN): Run start, DMA setup failed, %d", retval);
		//	Pixie_Print_MSG(ErrMSG,1);
		//	PIXIE500E_Cleanup();
		//	return(-0x13);
		//}

	} // for modules


	// TODO: check if more than 4KB page is the problem.
	// Errors appear without DSP Start/Stop: yes, still some, when 116,118. Only 116 OK.

	// Without VDMADriver_Go/Halt:(no errors for 116 only)
	// only 118, no Go/Halt:
	// With only 4KB pages: TBD.
	for (repCnt = 0; repCnt<10000; repCnt++) {
		sprintf(ErrMSG, "*INFO* (MAIN): Rep %d, errors %d", repCnt, failCnt);
		Pixie_Print_MSG(ErrMSG,1);


		for (ModNum = 0; ModNum < Number_Modules; ModNum++) {

			// PIXIE500E_DMA_Trace_Setup() does
			// 1. WDC_DMASGBufLock()
			// 2. PIXIE500E_VDMACodeGen_TraceOut()
			// 3. PIXIE500E_DMA_ProgramSequencer()
			// NB: failures to program DMA sequencer in PIXIE500E_DMA_Trace_Setup()
			retval = PIXIE500E_DMA_Trace_Setup(hDev[ModNum],  DMA_LM_FRAMEBUFFER_LENGTH, LMBuffer[ModNum], &pDmaList[ModNum]);
			if (retval != 0) {
				sprintf(ErrMSG, "*ERROR* (MAIN): Run start, DMA setup failed, %d, ModNum=%d, device 0x%08X", retval, ModNum, hDev[ModNum]);
				Pixie_Print_MSG(ErrMSG,1);
				printf("Press..."); getchar();
				//PIXIE500E_Cleanup();
				//return(-0x13);
				failCnt++;
				// and repeat the setup
				retval = PIXIE500E_DMA_Trace_Setup(hDev[ModNum],  DMA_LM_FRAMEBUFFER_LENGTH, LMBuffer[ModNum], &pDmaList[ModNum]);
			}

			// RUN
			retval = WDC_DMASyncCpu(pDmaList[ModNum]); // SyncCpu needed before performing DMA transfers.
			VDMADriver_SetDPTR(hDev[ModNum], MAIN_START);
			//VDMADriver_Go(hDev[ModNum]);

			//sprintf(ErrMSG, "*INFO* (MAIN): Run start, DMA setup DONE for module %d", ModNum);
			//Pixie_Print_MSG(ErrMSG,1);
			//Pixie_Sleep(10);
			//VDMADriver_Halt(hDev[ModNum]);
			retval = WDC_DMASyncIo(pDmaList[ModNum]);
		} // for modules

	} // for reps

	// Tear down
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {


		// Clean up after the run
		if (pDmaList[ModNum] != NULL) {
			retval = WDC_DMASyncIo(pDmaList[ModNum]);
			retval = WDC_DMABufUnlock(pDmaList[ModNum]);
			pDmaList[ModNum] = NULL;
		}
		if (LMBuffer[ModNum] != NULL) {
			free(LMBuffer[ModNum]);
			LMBuffer[ModNum] = NULL;

		}
		sprintf(ErrMSG, "*INFO* (MAIN): Run stop, DMA teardown DONE for module %d", ModNum);
		Pixie_Print_MSG(ErrMSG,1);
	} // for modules



#endif


	PIXIE500E_Cleanup();
	//PIXIE500E_LibUninit();

	return(retval);
}


int PIXIE500E_Cleanup()
{
	U8 ModNum;
	S32 retval;

	//End_Run(Number_Modules); // Once DMA programming fails, no communication with DSP
	for (ModNum = 0; ModNum < Number_Modules; ModNum++) {

		VDMADriver_Halt(hDev[ModNum]);
		// Clean up after the run
		if(listFile[ModNum] != NULL) fclose(listFile[ModNum]);

		if (pDmaList[ModNum] != NULL) {
			retval = WDC_DMASyncIo(pDmaList[ModNum]);
			retval = WDC_DMABufUnlock(pDmaList[ModNum]);
			pDmaList[ModNum] = NULL;
		}

		if (LMBuffer[ModNum] != NULL) {
			free(LMBuffer[ModNum]);
			LMBuffer[ModNum] = NULL;
		}

		PIXIE500E_DeviceClose(hDev[ModNum]);
		hDev[ModNum] = NULL;
	}
	//free(listFile);
	//free(pDmaList);
	//free(LMBuffer);
	//free(hDev);
	PIXIE500E_LibUninit();
	exit(1);
}

//static void IntHandler_INT3(WDC_DEVICE_HANDLE hDev, PIXIE500E_INT_RESULT *pIntResult)
//{
//	DWORD dwStatus, i;
//	//U8 ModNum;
//	//U16 RunType;
//	PPIXIE500E_DEV_CTX pDevCtx;	
//	PWDC_DEVICE pDev = (PWDC_DEVICE)hDev;
//	pDevCtx = (PPIXIE500E_DEV_CTX)WDC_GetDevContext(pDev);
//	//ModNum = (U8)(pDevCtx->dModNum);
//	//RunType = (U16)(pDevCtx->dRunType);
//
//	//ModNum = 0;
//	//RunType = 0x400;
//
//	// Setup for the next interrupt: enable event 0x8 (all events were disabled in kernel ISR)
//	// and clear INT_STAT
//	VDMADriver_EventClear(hDev, 0x08);
//	VDMADriver_SetEventEnable(hDev, 0x08);
//	i = 0xFFFFFFF3;
//	dwStatus = PIXIE500E_ReadWriteReg(hDev, INT_STAT, WDC_WRITE, &i, FALSE);
//	// Then do the work: buffer QC, file dump, DMA restart
//	dwStatus = Write_DMA_List_Mode_File ((U8)(pDevCtx->dModNum), "", (U16)(pDevCtx->dRunType));
//	if (dwStatus <0) printf("$$$$$$$$$$$$$$$$$ Problems with Write_DMA_List_Mode_File() $$$$$$$$$$$$$$$$$\n");
//
//	//// Bypass Write_DMA_List_Mode_File()
//	//LMBufferCounter[ModNum]++;
//	//VDMADriver_SetDPTR(hDev, MAIN_START);	// rewind DMA sequencer
//	//VDMADriver_Go(hDev);
//
//}
