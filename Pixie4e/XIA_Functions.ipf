#pragma rtGlobals=1		// Use modern global access method.

//****************************************************************************************//
//  Pixie_InitGlobals:                                                                                            //
//     Initialize global variables, waves and paths.                                           //
//****************************************************************************************//

Function Pixie_InitType()
	Variable mt
	Prompt mt,"Use '4' for Pixie-4, '5' for Pixie-4e and Pixie500e"// '500'('501') for Pixie-500(e), '400' for Pixie-500/400MHz"
	DoPrompt "Enter Pixie Module type", mt
	
	SVAR MTsuffix = root:pixie4:MTsuffix
	
	switch(mt)	// numeric switch
		case 4:		// execute if case matches expression
			MTsuffix = "4"
			break						// exit from switch
		case 5:		// execute if case matches expression
			MTsuffix = "4e"
			break						// exit from switch
		case 501:		// execute if case matches expression
			MTsuffix = "500e"
			break						// exit from switch
		default:							// optional default expression executed
			MTsuffix = "4"					// when no case matches
	endswitch
	
	return mt
End

Function Pixie_VersionCheck()
	// Check release numbers for consistency
	
	Nvar ViewerVersion = root:pixie4:ViewerVersion
	Nvar Clib = root:pixie4:CLibraryRelease
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	// root:pixie4:Display_Module_Parameters[ root:pixie4:index_DSPrelease]
	wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_DSPrelease = root:pixie4:index_DSPrelease
	Nvar index_FippiID = root:pixie4:index_FippiID
	Nvar index_SystemID = root:pixie4:index_SystemID
	Nvar index_BoardVersion = root:pixie4:index_BoardVersion
	
	Variable DSP = Display_Module_Parameters[index_DSPrelease]
	Variable Fippi = Display_Module_Parameters[index_FippiID] 
	Variable System = Display_Module_Parameters[index_SystemID]				// from FPGA
	Variable BoardVersion = Display_Module_Parameters[index_BoardVersion]	
	Variable board_mismatch =1
	
	// supported versions in this release
	// these should all be the same for major releases (e.g. 0x410)
	// For minor releases, some may advance, other stay the same, e.g. DSP, C -> 0x401, Fippi = 0x400 
	// (different "builds" are not checked, they must be compatible within a version)
	// DSP and Fippi varies for P4e, P4, P500e
	// for now, manually enter legal version numbers
	ViewerVersion = 0x451			// specify here the Pixie Viewer version 
	Variable Cversion = 0x450
	Variable Dversion_P4e = 0x450
	Variable Fversion_P4e500 = 0x451
	Variable Fversion_P4e125 = 0x451
	
	Variable Dversion_P4 = 0x1473	// this should never need updating
	Variable Fversion_P500e = 0x431		// this should never need updating
	Variable Fversion_P4 = 0x00CA		// this should never need updating
	Variable Fversion_P4nec = 0x0001		// NEC continues to probe the system in new ways, so there may be updates

	// check code versions - applied to online and offline mode
	if(Clib != Cversion) 
		print "*** Error *** API or Igor code version mismatch. Please open the ''About'' Panel and compare to release notes"
	endif

	if(!Pixie4Offline)	// modules are present, check what's read back from them
		
		//check DSP
		if(  !(  (DSP == Dversion_P4e)  || (DSP == Dversion_P4)  )        )
			print "*** Error *** DSP code version mismatch. Please open the ''About'' Panel and compare to release notes"
		endif

		// check Fippi
		if(  !( (Fippi == Fversion_P4e125)  || (Fippi == Fversion_P4e500)  || (Fippi == Fversion_P500e)  || (Fippi == Fversion_P4) || (Fippi == Fversion_P4nec)  )        )
			print "*** Error *** Signal processing FPGA code version mismatch. Please open the ''About'' Panel and compare to release notes"
		endif
	
	
		// check board versions 
		if(System == BoardVersion)	// in most cases, these should be identical. Exceptions below
			board_mismatch = 0
		endif
			
			if((BoardVersion & 0xFFF0) == 0xA700) 				// P4, all Revs (Rev B not really supported, but not really in use) 
				if(  (System == 0xA71C)  ||  (System == 0xA72E) )	//generic or NEC System FW
					board_mismatch = 0
				endif
			endif
			
			if(BoardVersion == 0xA100) 						// P500e 14/500, Rev A 
				if(System == 0xA101) 						// ok with Rev B FW
					board_mismatch = 0
				endif
			endif
			
			if(BoardVersion == 0xA550) 						// P4e 16/125, Rev A 
				if(System == 0xA551) 						// ok with Rev B FW
					board_mismatch = 0
				endif
			endif
			
			if(BoardVersion == 0xA551) 						// P4e 16/125, Rev B 
				if(System == 0xA550) 						// ok with Rev A FW
					board_mismatch = 0
				endif
			endif
					
		if(board_mismatch)
			print "*** Error *** Board Version mismatch. Please check if the firmware file is correct for this module version"
		endif
	endif
End

Macro Pixie_InitGlobals()

	// Disable the display of Macro commands
	Silent 1
	
	// Create a new folder pixie4
	NewDataFolder/o root:pixie4
	
	Variable/G root:pixie4:ViewerVersion// = 0x400		// Pixie4 Viewer version set above in version check
	
	///////////////////////////////////////////////////////////// 
	//Device selection                                 //
	////////////////////////////////////////////////////////////
	
	// ModuleType =	4 	Pixie-4
	//				5 	Pixie-4 Express, Pixie-500 Express
	// 				500	Pixie-500 (no longer supported)
	//				400 Pixie-500 [14/400] (no longer supported)
	// 				501 Pixie-500 Express (legacy)
	
	String/G root:pixie4:MTsuffix = "-4"
	Variable/G root:pixie4:ModuleType = 4	// default Pixie-4
	Variable mt
	mt = Pixie_InitType()
	if(mt==400 || mt==500  || mt==501 || mt==5)
		root:pixie4:ModuleType = mt
	endif
	
	// close windows that are module type dependant
	DoWindow/K ListModeTracesDisplay
	DoWindow/K Pixie_Parameter_Setup
	
	
	///////////////////////////////////////////////////////////// 
	// Constants definitions                         //
	////////////////////////////////////////////////////////////
	
	//default
		Variable/G root:pixie4:SystemClockMHz = 75		// system clock 
		Variable/G root:pixie4:FilterClockMHz = 75			// clock for pulse processing and traces clock applied in the /4 time scale
//		Variable/G root:pixie4:ADCClockMHz = 75			// ADC clock
		Variable/G root:pixie4:FIFOLength = 1023			// FIFO length
		Variable/G root:pixie4:ADCrange = 16384			//14 bit ADC
	if(root:pixie4:ModuleType ==5)
		Variable/G root:pixie4:SystemClockMHz = 125		// system clock 
		Variable/G root:pixie4:FilterClockMHz = 125			// clock for pulse processing and traces clock applied in the /4 time scale
//		Variable/G root:pixie4:ADCClockMHz = 125			// ADC clock
		Variable/G root:pixie4:FIFOLength = 4096			// FIFO length
		Variable/G root:pixie4:ADCrange = 65536			//16bit ADC
	endif	
	if(root:pixie4:ModuleType ==500)
		Variable/G root:pixie4:SystemClockMHz = 75		// system clock 
		Variable/G root:pixie4:FilterClockMHz = 125			// clock for pulse processing and traces clock applied in the /4 time scale
//		Variable/G root:pixie4:ADCClockMHz = 500			// ADC clock
		Variable/G root:pixie4:FIFOLength = 8191			// FIFO length
		Variable/G root:pixie4:ADCrange = 4096				//12 bit ADC
	endif
	if(root:pixie4:ModuleType ==501)
		Variable/G root:pixie4:SystemClockMHz = 125		// system clock 
		Variable/G root:pixie4:FilterClockMHz = 125			// clock for pulse processing and traces clock applied in the /4 time scale
//		Variable/G root:pixie4:ADCClockMHz = 500			// ADC clock
		Variable/G root:pixie4:FIFOLength = 4096			// FIFO length
		Variable/G root:pixie4:ADCrange = 16384			//14 bit ADC
	endif
	if(root:pixie4:ModuleType ==400)
		Variable/G root:pixie4:SystemClockMHz = 75		// system clock 
		Variable/G root:pixie4:FilterClockMHz = 100			// clock for pulse processing and traces clock applied in the /4 time scale
//		Variable/G root:pixie4:ADCClockMHz = 400			// ADC clock
		Variable/G root:pixie4:FIFOLength = 8191			// FIFO length
		Variable/G root:pixie4:ADCrange = 16384			//14 bit ADC
	endif

	Variable/G root:pixie4:NumberOfChannels = 4		// number of channels	
	Variable/G root:pixie4:ADCTraceLen = 8192			// maximum ADC trace length
	Variable/G root:pixie4:MCALen = 32768				// maximum MCA histogram length
	Variable/G root:pixie4:DSPVarLen = 512				// total number of memory variables
	Variable/G root:pixie4:MemVarLen = 16384			// total number of memory variables
	Variable/G root:pixie4:NumSystemPar = 64			// total number of module globals (applicable to all modules)
	Variable/G root:pixie4:NumChannelPar = 64			// total number of user variables (applicable to each channel)
	Variable/G root:pixie4:NumModulePar = 128			// total number of global variables (applicable to each module)
	Variable/G root:pixie4:EFMinRiseTimeTicks = 2		// the minimum energy filter rise time (in decimated clock ticks)
	Variable/G root:pixie4:EFMaxRiseTimeTicks = 124		// the maximum energy filter rise time (in decimated clock ticks)
	Variable/G root:pixie4:EFMinFlatTopTicks = 3			// the minimum energy filter flat top (in decimated clock ticks)
	Variable/G root:pixie4:EFMaxFlatTopTicks = 125		// the maximum energy filter flat top (in decimated clock ticks)	
	Variable/G root:pixie4:TFMinRiseTimeTicks = 2		// the minimum trigger filter rise time (in clock ticks)
	Variable/G root:pixie4:TFMaxRiseTimeTicks = 63		// the maximum trigger filter rise time (in clock ticks)
	Variable/G root:pixie4:TFMinFlatTopTicks = 0			// the minimum trigger filter flat top (in clock ticks)
	Variable/G root:pixie4:TFMaxFlatTopTicks = 61		// the maximum trigger filter flat top (in clock ticks)	
	Variable/G root:pixie4:PRESET_MAX_MODULES = 17	// the maximum number of modules possible in the system	
	
	

		
	///////////////////////////////////////////////////////////// 
	// Run Control Interface global variables  //
	////////////////////////////////////////////////////////////
	
	Variable/G root:pixie4:ChosenModule
	Variable/G root:pixie4:ChosenChannel
	Variable/G root:pixie4:PanelTabNumber
	Variable/G root:pixie4:PrevTabNumber
	Variable/G root:pixie4:HideDetail // how many controls to show
	Variable/G root:pixie4:FirstTimeUse	
	Variable/G root:pixie4:GainMultiplier = 1
	
	// Allowed run tasks and copy items
	Variable/G root:Pixie4:Nruntypes = 9
	Make/o/n=( root:Pixie4:Nruntypes) root:pixie4:RunTasks = {0x100,0x101,0x102,0x103,0x301,0x400,0x401,0x402,0x403}	
	Variable/G root:Pixie4:MCArunListNumber = 5
	Variable/G root:Pixie4:DMAListNumber = 6
	
	Make/o/t/n=13 root:pixie4:CopyItemsList={"Gain", "Offset", "Filter", "Trigger", "FIFO", "ChanCSR",  "COINC.", "MCA", "TAU", "Integrator","ModCSR", "Gate","PSA"}
	Make/o/u/i/n=13 root:pixie4:CopyItemsListData
	
	///////////////////////////////////////////////////////////// 
	// System Setup global variables		//
	////////////////////////////////////////////////////////////
		
	Variable/G root:pixie4:NumberOfModules // = 1	// initialize at very first init_globals only
	Variable/G root:pixie4:Pixie4Offline
	Variable/G root:pixie4:AutoProcessLMData 
	Variable/G root:pixie4:KeepCW 
	Variable/G root:pixie4:MaxNumModules				// acts as crate type ID
	Make/o/n=(root:pixie4:NumberOfModules) root:pixie4:ModuleSlotNumber
	
	Variable/G root:pixie4:CLibraryRelease
	Variable/G root:pixie4:CLibraryBuild
	
	Make/o/n=(root:pixie4:NumberOfModules) root:pixie4:ModuleSerialNumber
	Make/o/t/n=(root:pixie4:NumberOfModules,3) root:pixie4:ModuleListData
	Make/o/b/n=(root:pixie4:NumberOfModules,3) root:pixie4:ModuleSListData
	Make/o/u/w/n=(root:pixie4:NumberOfModules) root:pixie4:ModuleSerialNumber_UINT16	
	
	///////////////////////////////////////////////////////////// 
	// Dummy waves                      		//
	////////////////////////////////////////////////////////////
	make/o/u/i/n=1 root:pixie4:dummy
	make/o/u/w/n=1 root:pixie4:dummysettings
	
	///////////////////////////////////////////////////////////// 
	// I/O global variables                            //
	////////////////////////////////////////////////////////////
	Make/o/d/n=(root:pixie4:NumSystemPar) root:pixie4:System_Parameter_Values
	Make/o/t/n=(root:pixie4:NumSystemPar) root:pixie4:System_Parameter_Names
	root:pixie4:System_Parameter_Names[0] = {"NUMBER_MODULES","OFFLINE_ANALYSIS","AUTO_PROCESSLMDATA", "MAX_NUMBER_MODULES"}
	root:pixie4:System_Parameter_Names[4] ={"C_LIBRARY_RELEASE","C_LIBRARY_BUILD", "KEEP_CW","SLOT_WAVE"}
		
	Make/o/d/n=(root:pixie4:NumModulePar,root:pixie4:PRESET_MAX_MODULES) root:pixie4:Module_Parameter_Values
	Make/o/t/n=(root:pixie4:NumModulePar) root:pixie4:Module_Parameter_Names
	root:pixie4:Module_Parameter_Names[0]= {"MODULE_NUMBER","MODULE_CSRA","MODULE_CSRB","C_CONTROL","MAX_EVENTS","COINCIDENCE_PATTERN"}
	root:pixie4:Module_Parameter_Names[6]= {"ACTUAL_COINCIDENCE_WAIT","MIN_COINCIDENCE_WAIT","SYNCH_WAIT","IN_SYNCH","RUN_TYPE"}
 	root:pixie4:Module_Parameter_Names[11]= {"FILTER_RANGE","MODULEPATTERN","NNSHAREPATTERN","DBLBUFCSR","MODULE_CSRC","BUFFER_HEAD_LENGTH"}
 	root:pixie4:Module_Parameter_Names[17]= {"EVENT_HEAD_LENGTH","CHANNEL_HEAD_LENGTH","","NUMBER_EVENTS"}
	root:pixie4:Module_Parameter_Names[21]= {"RUN_TIME","EVENT_RATE","TOTAL_TIME","BOARD_VERSION","SERIAL_NUMBER","DSP_RELEASE","DSP_BUILD","FIPPI_ID"}
	root:pixie4:Module_Parameter_Names[29]= {"SYSTEM_ID","XET_DELAY","PDM_MASKA","PDM_MASKB","PDM_MASKC"}
	root:pixie4:Module_Parameter_Names[34]= {"USER_IN","","","","","","","","","","","","","","",""}	// 16 values for USER_IN
	root:pixie4:Module_Parameter_Names[50]= {"USER_OUT","","","","","","","","","","","","","","",""}	// 16 values for USER_OUT
	root:pixie4:Module_Parameter_Names[66]= {"ADC_BITS","ADC_RATE","NUM_COINC_TRIG","COINC_SFDT"}
	root:pixie4:Module_Parameter_Names[70]= {"COINC_COUNT_TIME","COINC_INPUT_RATE","EXTRA_IN","","","","","","",""}
	root:pixie4:Module_Parameter_Names[80]= {"EXTRA_OUT","","","","","","","","",""}		// 48 values for EXTRA_OUT, but can insert specific names if necessary
	root:pixie4:Module_Parameter_Names[90]= {"","","","","","","","","",""}
	root:pixie4:Module_Parameter_Names[100]= {"","","","","","","","","",""}
	root:pixie4:Module_Parameter_Names[110]= {"","","","","","","","","",""}
	root:pixie4:Module_Parameter_Names[120]= {"","","","","","","",""}
	

	Make/o/d/n=(root:pixie4:NumChannelPar,root:pixie4:NumberOfChannels,root:pixie4:PRESET_MAX_MODULES) root:pixie4:Channel_Parameter_Values
	Make/o/t/n=(root:pixie4:NumChannelPar) root:pixie4:Channel_Parameter_Names
	root:pixie4:Channel_Parameter_Names[0]= {"CHANNEL_CSRA","CHANNEL_CSRB","ENERGY_RISETIME","ENERGY_FLATTOP","TRIGGER_RISETIME","TRIGGER_FLATTOP"}
	root:pixie4:Channel_Parameter_Names[6]= {"TRIGGER_THRESHOLD","VGAIN","VOFFSET","TRACE_LENGTH","TRACE_DELAY","PSA_START"}
  	root:pixie4:Channel_Parameter_Names[12]= {"PSA_END","FCFD_THRESHOLD","BINFACTOR","TAU","BLCUT","XDT","BASELINE_PERCENT","CFD_THRESHOLD","INTEGRATOR"}
  	root:pixie4:Channel_Parameter_Names[21] = {"CHANNEL_CSRC","GATE_WINDOW","GATE_DELAY","COINC_DELAY","BLAVG","COUNT_TIME","INPUT_COUNT_RATE","FAST_PEAKS"}
	root:pixie4:Channel_Parameter_Names[29]= {"OUTPUT_COUNT_RATE","NOUT","GATE_RATE","GATE_COUNTS","FTDT","SFDT"}
	root:pixie4:Channel_Parameter_Names[35] = {"GDT","CURRENT_ICR","CURRENT_OORF","PSM_GAIN_AVG","PSM_GAIN_AVG_LEN","PSM_TEMP_AVG","PSM_TEMP_AVG_LEN","PSM_GAIN_CORR"}
	root:pixie4:Channel_Parameter_Names[43] = {"QDC0_LENGTH","QDC1_LENGTH","QDC0_DELAY","QDC1_DELAY","NPPI","PASS_PILEUP_RATE","CH_EXTRA_IN","","","","","","","","","","","","","",""}
	Variable/G root:pixie4:NumChannelParIFM = 49		//  number of input user variables (applicable to each channel) for .ifm file
	Variable/G root:pixie4:NumModuleInParIFM = 72		//  number of input global variables (applicable to each module) for .ifm file
	Variable/G root:pixie4:NumSystemInParIFM = 7		//  number of input global variables (applicable to system) for .ifm file

	


	//////////////////////////////////////////////////////////////////////////// 
	// Module global variables for display and control     //
	////////////////////////////////////////////////////////////////////////////
	
	make/o/d/n=(4* root:pixie4:NumModulePar) root:pixie4:Display_Module_Parameters

	//IN (mostly) 
	// no global for ModuleNumber		
	Variable/G root:pixie4:index_MCSRA = Pixie_Find_ModuleParInWave("MODULE_CSRA")
	Variable/G root:pixie4:index_MCSRB = Pixie_Find_ModuleParInWave("MODULE_CSRB")
	Variable/G root:pixie4:index_MCSRC = Pixie_Find_ModuleParInWave("MODULE_CSRC")
	Variable/G root:pixie4:index_CC = Pixie_Find_ModuleParInWave("C_CONTROL")
	// no global for Module Format
	Variable/G root:pixie4:index_MaxEv = Pixie_Find_ModuleParInWave("MAX_EVENTS")
	Variable/G root:pixie4:index_CoincPat = Pixie_Find_ModuleParInWave("COINCIDENCE_PATTERN")
	Variable/G root:pixie4:index_ActCW = Pixie_Find_ModuleParInWave("ACTUAL_COINCIDENCE_WAIT")
	Variable/G root:pixie4:index_MinCW = Pixie_Find_ModuleParInWave("MIN_COINCIDENCE_WAIT")
//	Variable/G root:pixie4:index_SyncWait = Pixie_Find_ModuleParInWave("SYNCH_WAIT")	// syncwait uses checkbox only
//	Variable/G root:pixie4:index_InSync = Pixie_Find_ModuleParInWave("IN_SYNCH")			// InSync uses checkbox only
	Variable/G root:pixie4:index_RunType = Pixie_Find_ModuleParInWave("RUN_TYPE")
	Variable/G root:pixie4:index_FilterRange = Pixie_Find_ModuleParInWave("FILTER_RANGE")
	Variable/G root:pixie4:index_ModPat = Pixie_Find_ModuleParInWave("MODULEPATTERN")
	Variable/G root:pixie4:index_NNSharePat = Pixie_Find_ModuleParInWave("NNSHAREPATTERN")
	Variable/G root:pixie4:index_DBCSR = Pixie_Find_ModuleParInWave("DBLBUFCSR")


	//OUT
	Variable/G root:pixie4:index_BHL = Pixie_Find_ModuleParInWave("BUFFER_HEAD_LENGTH")
	Variable/G root:pixie4:index_EHL = Pixie_Find_ModuleParInWave("EVENT_HEAD_LENGTH")
	Variable/G root:pixie4:index_CHL = Pixie_Find_ModuleParInWave("CHANNEL_HEAD_LENGTH")
	
	Variable/G root:pixie4:index_NumEv = Pixie_Find_ModuleParInWave("NUMBER_EVENTS")
	Variable/G root:pixie4:index_RunTime = Pixie_Find_ModuleParInWave("RUN_TIME")
	Variable/G root:pixie4:index_EvRate = Pixie_Find_ModuleParInWave("EVENT_RATE")
	Variable/G root:pixie4:index_TotTime = Pixie_Find_ModuleParInWave("TOTAL_TIME")
	Variable/G root:pixie4:index_BoardVersion = Pixie_Find_ModuleParInWave("BOARD_VERSION")
	Variable/G root:pixie4:index_SerialNum = Pixie_Find_ModuleParInWave("SERIAL_NUMBER")
	Variable/G root:pixie4:index_DSPrelease = Pixie_Find_ModuleParInWave("DSP_RELEASE")
	Variable/G root:pixie4:index_DSPbuild = Pixie_Find_ModuleParInWave("DSP_BUILD")
	Variable/G root:pixie4:index_FippiID = Pixie_Find_ModuleParInWave("FIPPI_ID")
	Variable/G root:pixie4:index_SystemID = Pixie_Find_ModuleParInWave("SYSTEM_ID")
	Variable/G root:pixie4:index_XETdelay = Pixie_Find_ModuleParInWave("XET_DELAY")
	Variable/G root:pixie4:index_UserIn = Pixie_Find_ModuleParInWave("USER_IN")
	Variable/G root:pixie4:index_UserOUT = Pixie_Find_ModuleParInWave("USER_OUT")
	Variable/G root:pixie4:index_NCT = Pixie_Find_ModuleParInWave("NUM_COINC_TRIG")
	Variable/G root:pixie4:index_CSFDT = Pixie_Find_ModuleParInWave("COINC_SFDT")
	Variable/G root:pixie4:index_CCT = Pixie_Find_ModuleParInWave("COINC_COUNT_TIME")
	Variable/G root:pixie4:index_CIR = Pixie_Find_ModuleParInWave("COINC_INPUT_RATE")



	//Coinc. Pattern bits
	Variable/G root:pixie4:Coincbit00	// variables for Coinc checkboxes
	Variable/G root:pixie4:Coincbit01
	Variable/G root:pixie4:Coincbit02
	Variable/G root:pixie4:Coincbit03
	Variable/G root:pixie4:Coincbit04
	Variable/G root:pixie4:Coincbit05
	Variable/G root:pixie4:Coincbit06
	Variable/G root:pixie4:Coincbit07
	Variable/G root:pixie4:Coincbit08
	Variable/G root:pixie4:Coincbit09
	Variable/G root:pixie4:Coincbit10
	Variable/G root:pixie4:Coincbit11
	Variable/G root:pixie4:Coincbit12
	Variable/G root:pixie4:Coincbit13
	Variable/G root:pixie4:Coincbit14
	Variable/G root:pixie4:Coincbit15
	
	//MCSRA bits
	Variable/G root:pixie4:CloverAdd
	Variable/G root:pixie4:CloverSingleOnly
	Variable/G root:pixie4:RepeatLMrun
	Variable/G root:pixie4:NoRepeatLMrun
	Variable/G root:pixie4:TS500
	Variable/G root:pixie4:SDRAMbypass
	Variable/G root:pixie4:RS2LM
	Variable/G root:pixie4:RS2LMedge
	Variable/G root:pixie4:CWgroup
	
	//CControl bits
	Variable/G root:pixie4:Polling
	Variable/G root:pixie4:BufferQC
	Variable/G root:pixie4:PrintDebugMsg_Boot	
	Variable/G root:pixie4:PrintDebugMsg_QCerror	
	Variable/G root:pixie4:PrintDebugMsg_QCdetail
	Variable/G root:pixie4:PrintDebugMsg_other
	Variable/G root:pixie4:PrintDebugMsg_daq
	Variable/G root:pixie4:PrintDebugMsg_file
	Variable/G root:pixie4:PollForNewData
	Variable/G root:pixie4:MultiThreadDAQ
	Variable/G root:pixie4:KeepBLcut

	
	//MCSRB bits
	Variable/G root:pixie4:ModCSRBbit00	// variables for ModCSRB checkboxes
	Variable/G root:pixie4:ModCSRBbit01
	Variable/G root:pixie4:ModCSRBbit02
	Variable/G root:pixie4:ModCSRBbit03
	Variable/G root:pixie4:ModCSRBbit04
	Variable/G root:pixie4:ModCSRBbit05
	Variable/G root:pixie4:ModCSRBbit06
	Variable/G root:pixie4:ModCSRBbit07
	Variable/G root:pixie4:ModCSRBbit08
	Variable/G root:pixie4:ModCSRBbit09
	Variable/G root:pixie4:ModCSRBbit10
	Variable/G root:pixie4:ModCSRBbit11
	Variable/G root:pixie4:ModCSRBbit12
	Variable/G root:pixie4:ModCSRBbit13
	Variable/G root:pixie4:ModCSRBbit14
	Variable/G root:pixie4:ModCSRBbit15
	
	//DblBufCSR bits
	Variable/G root:pixie4:PingPongRun
	
	//other checkbox variables
	Variable/G root:pixie4:synchwait		//XXX check: duplicate in comm. globals above?
	Variable/G root:pixie4:InSynch			// set to 0 if you want to synchronize clocks


	
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Channel global variables for display and control                                 //
	// use a wave, not variables, to make access/updating more convenient //
	/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	make/o/d/n=(4* root:pixie4:NumChannelPar) root:pixie4:Display_Channel_Parameters
	
	Variable/G root:pixie4:index_CCSRA = Pixie_Find_ChanParInWave("CHANNEL_CSRA")
	Variable/G root:pixie4:index_CCSRB = Pixie_Find_ChanParInWave("CHANNEL_CSRB")
	Variable/G root:pixie4:index_SL = Pixie_Find_ChanParInWave("ENERGY_RISETIME")
	Variable/G root:pixie4:index_SG = Pixie_Find_ChanParInWave("ENERGY_FLATTOP")
	Variable/G root:pixie4:index_FL = Pixie_Find_ChanParInWave("TRIGGER_RISETIME")
	Variable/G root:pixie4:index_FG = Pixie_Find_ChanParInWave("TRIGGER_FLATTOP")
	Variable/G root:pixie4:index_TH = Pixie_Find_ChanParInWave("TRIGGER_THRESHOLD")
	Variable/G root:pixie4:index_GAIN = Pixie_Find_ChanParInWave("VGAIN")
	Variable/G root:pixie4:index_OFFSET = Pixie_Find_ChanParInWave("VOFFSET")
	Variable/G root:pixie4:index_TL = Pixie_Find_ChanParInWave("TRACE_LENGTH")
	Variable/G root:pixie4:index_TD = Pixie_Find_ChanParInWave("TRACE_DELAY")
	Variable/G root:pixie4:index_PSA_START = Pixie_Find_ChanParInWave("PSA_START")
	Variable/G root:pixie4:index_PSA_END = Pixie_Find_ChanParInWave("PSA_END")
//	Variable/G root:pixie4:index_EMIN = Pixie_Find_ChanParInWave("EMIN")
	Variable/G root:pixie4:index_BINFACTOR = Pixie_Find_ChanParInWave("BINFACTOR")
	Variable/G root:pixie4:index_TAU = Pixie_Find_ChanParInWave("TAU")
	Variable/G root:pixie4:index_BLCUT = Pixie_Find_ChanParInWave("BLCUT")
	Variable/G root:pixie4:index_BLAVG = Pixie_Find_ChanParInWave("BLAVG")	
	Variable/G root:pixie4:index_XDT = Pixie_Find_ChanParInWave("XDT")
	Variable/G root:pixie4:index_BASEPC = Pixie_Find_ChanParInWave("BASELINE_PERCENT")
	Variable/G root:pixie4:index_CFD_TH = Pixie_Find_ChanParInWave("CFD_THRESHOLD")
	Variable/G root:pixie4:index_INTEGRATOR = Pixie_Find_ChanParInWave("INTEGRATOR")
	Variable/G root:pixie4:index_CCSRC = Pixie_Find_ChanParInWave("CHANNEL_CSRC")
	Variable/G root:pixie4:index_GW = Pixie_Find_ChanParInWave("GATE_WINDOW")
	Variable/G root:pixie4:index_GD = Pixie_Find_ChanParInWave("GATE_DELAY")
	Variable/G root:pixie4:index_CD = Pixie_Find_ChanParInWave("COINC_DELAY")
	Variable/G root:pixie4:index_COUNTTIME = Pixie_Find_ChanParInWave("COUNT_TIME")
	Variable/G root:pixie4:index_ICR = Pixie_Find_ChanParInWave("INPUT_COUNT_RATE")
	Variable/G root:pixie4:index_FP = Pixie_Find_ChanParInWave("FAST_PEAKS")
	Variable/G root:pixie4:index_OCR = Pixie_Find_ChanParInWave("OUTPUT_COUNT_RATE")
	Variable/G root:pixie4:index_GCR = Pixie_Find_ChanParInWave("GATE_RATE")
	Variable/G root:pixie4:index_FTDT = Pixie_Find_ChanParInWave("FTDT")
	Variable/G root:pixie4:index_SFDT = Pixie_Find_ChanParInWave("SFDT")
	Variable/G root:pixie4:index_GDT = Pixie_Find_ChanParInWave("GDT")
	Variable/G root:pixie4:index_CICR = Pixie_Find_ChanParInWave("CURRENT_ICR")
	Variable/G root:pixie4:index_OORF = Pixie_Find_ChanParInWave("CURRENT_OORF")
	Variable/G root:pixie4:index_Q0LEN = Pixie_Find_ChanParInWave("QDC0_LENGTH")
	Variable/G root:pixie4:index_Q1LEN = Pixie_Find_ChanParInWave("QDC1_LENGTH")
	Variable/G root:pixie4:index_Q0DEL = Pixie_Find_ChanParInWave("QDC0_DELAY")
	Variable/G root:pixie4:index_Q1DEL = Pixie_Find_ChanParInWave("QDC1_DELAY")
	Variable/G root:pixie4:index_PPR = Pixie_Find_ChanParInWave("PASS_PILEUP_RATE")
	Variable/G root:pixie4:index_FCFD_TH = Pixie_Find_ChanParInWave("FCFD_THRESHOLD")
	Variable/G root:pixie4:index_CH_EXTRA_IN = Pixie_Find_ChanParInWave("CH_EXTRA_IN")
	Variable/G root:pixie4:index_FCFD_SC = Pixie_Find_ChanParInWave("FCFD_SCALE")

	
	Variable/G root:pixie4:ChanCSRAbit00	// variables for ChanCSRA checkboxes
	Variable/G root:pixie4:ChanCSRAbit01
	Variable/G root:pixie4:ChanCSRAbit02
	Variable/G root:pixie4:ChanCSRAbit03
	Variable/G root:pixie4:ChanCSRAbit04
	Variable/G root:pixie4:ChanCSRAbit05
	Variable/G root:pixie4:ChanCSRAbit06
	Variable/G root:pixie4:ChanCSRAbit07
	Variable/G root:pixie4:ChanCSRAbit08
	Variable/G root:pixie4:ChanCSRAbit09
	Variable/G root:pixie4:ChanCSRAbit10
	Variable/G root:pixie4:ChanCSRAbit11
	Variable/G root:pixie4:ChanCSRAbit12
	Variable/G root:pixie4:ChanCSRAbit13
	Variable/G root:pixie4:ChanCSRAbit14
	Variable/G root:pixie4:ChanCSRAbit15
	
	Variable/G root:pixie4:Chan0CSRAbit00	// variables for ChanCSRA checkboxes
	Variable/G root:pixie4:Chan0CSRAbit01
	Variable/G root:pixie4:Chan0CSRAbit02
	Variable/G root:pixie4:Chan0CSRAbit03
	Variable/G root:pixie4:Chan0CSRAbit04
	Variable/G root:pixie4:Chan0CSRAbit05
	Variable/G root:pixie4:Chan0CSRAbit06
	Variable/G root:pixie4:Chan0CSRAbit07
	Variable/G root:pixie4:Chan0CSRAbit08
	Variable/G root:pixie4:Chan0CSRAbit09
	Variable/G root:pixie4:Chan0CSRAbit10
	Variable/G root:pixie4:Chan0CSRAbit11
	Variable/G root:pixie4:Chan0CSRAbit12
	Variable/G root:pixie4:Chan0CSRAbit13
	Variable/G root:pixie4:Chan0CSRAbit14
	Variable/G root:pixie4:Chan0CSRAbit15
	
	Variable/G root:pixie4:Chan1CSRAbit00	// variables for ChanCSRA checkboxes
	Variable/G root:pixie4:Chan1CSRAbit01
	Variable/G root:pixie4:Chan1CSRAbit02
	Variable/G root:pixie4:Chan1CSRAbit03
	Variable/G root:pixie4:Chan1CSRAbit04
	Variable/G root:pixie4:Chan1CSRAbit05
	Variable/G root:pixie4:Chan1CSRAbit06
	Variable/G root:pixie4:Chan1CSRAbit07
	Variable/G root:pixie4:Chan1CSRAbit08
	Variable/G root:pixie4:Chan1CSRAbit09
	Variable/G root:pixie4:Chan1CSRAbit10
	Variable/G root:pixie4:Chan1CSRAbit11
	Variable/G root:pixie4:Chan1CSRAbit12
	Variable/G root:pixie4:Chan1CSRAbit13
	Variable/G root:pixie4:Chan1CSRAbit14
	Variable/G root:pixie4:Chan1CSRAbit15
	
	Variable/G root:pixie4:Chan2CSRAbit00	// variables for ChanCSRA checkboxes
	Variable/G root:pixie4:Chan2CSRAbit01
	Variable/G root:pixie4:Chan2CSRAbit02
	Variable/G root:pixie4:Chan2CSRAbit03
	Variable/G root:pixie4:Chan2CSRAbit04
	Variable/G root:pixie4:Chan2CSRAbit05
	Variable/G root:pixie4:Chan2CSRAbit06
	Variable/G root:pixie4:Chan2CSRAbit07
	Variable/G root:pixie4:Chan2CSRAbit08
	Variable/G root:pixie4:Chan2CSRAbit09
	Variable/G root:pixie4:Chan2CSRAbit10
	Variable/G root:pixie4:Chan2CSRAbit11
	Variable/G root:pixie4:Chan2CSRAbit12
	Variable/G root:pixie4:Chan2CSRAbit13
	Variable/G root:pixie4:Chan2CSRAbit14
	Variable/G root:pixie4:Chan2CSRAbit15
	
	Variable/G root:pixie4:Chan3CSRAbit00	// variables for ChanCSRA checkboxes
	Variable/G root:pixie4:Chan3CSRAbit01
	Variable/G root:pixie4:Chan3CSRAbit02
	Variable/G root:pixie4:Chan3CSRAbit03
	Variable/G root:pixie4:Chan3CSRAbit04
	Variable/G root:pixie4:Chan3CSRAbit05
	Variable/G root:pixie4:Chan3CSRAbit06
	Variable/G root:pixie4:Chan3CSRAbit07
	Variable/G root:pixie4:Chan3CSRAbit08
	Variable/G root:pixie4:Chan3CSRAbit09
	Variable/G root:pixie4:Chan3CSRAbit10
	Variable/G root:pixie4:Chan3CSRAbit11
	Variable/G root:pixie4:Chan3CSRAbit12
	Variable/G root:pixie4:Chan3CSRAbit13
	Variable/G root:pixie4:Chan3CSRAbit14
	Variable/G root:pixie4:Chan3CSRAbit15
	
	Variable/G root:pixie4:Chan0CSRBbit00	// variables for ChanCSRB checkboxes
	Variable/G root:pixie4:Chan0CSRBbit01
	Variable/G root:pixie4:Chan0CSRBbit02
	Variable/G root:pixie4:Chan0CSRBbit03
	Variable/G root:pixie4:Chan0CSRBbit04
	Variable/G root:pixie4:Chan0CSRBbit05
	Variable/G root:pixie4:Chan0CSRBbit06
	Variable/G root:pixie4:Chan0CSRBbit07
	Variable/G root:pixie4:Chan0CSRBbit08
	Variable/G root:pixie4:Chan0CSRBbit09
	Variable/G root:pixie4:Chan0CSRBbit10
	Variable/G root:pixie4:Chan0CSRBbit11
	Variable/G root:pixie4:Chan0CSRBbit12
	Variable/G root:pixie4:Chan0CSRBbit13
	Variable/G root:pixie4:Chan0CSRBbit14
	Variable/G root:pixie4:Chan0CSRBbit15
	
	Variable/G root:pixie4:Chan1CSRBbit00	// variables for ChanCSRB checkboxes
	Variable/G root:pixie4:Chan1CSRBbit01
	Variable/G root:pixie4:Chan1CSRBbit02
	Variable/G root:pixie4:Chan1CSRBbit03
	Variable/G root:pixie4:Chan1CSRBbit04
	Variable/G root:pixie4:Chan1CSRBbit05
	Variable/G root:pixie4:Chan1CSRBbit06
	Variable/G root:pixie4:Chan1CSRBbit07
	Variable/G root:pixie4:Chan1CSRBbit08
	Variable/G root:pixie4:Chan1CSRBbit09
	Variable/G root:pixie4:Chan1CSRBbit10
	Variable/G root:pixie4:Chan1CSRBbit11
	Variable/G root:pixie4:Chan1CSRBbit12
	Variable/G root:pixie4:Chan1CSRBbit13
	Variable/G root:pixie4:Chan1CSRBbit14
	Variable/G root:pixie4:Chan1CSRBbit15
	
	Variable/G root:pixie4:Chan2CSRBbit00	// variables for ChanCSRB checkboxes
	Variable/G root:pixie4:Chan2CSRBbit01
	Variable/G root:pixie4:Chan2CSRBbit02
	Variable/G root:pixie4:Chan2CSRBbit03
	Variable/G root:pixie4:Chan2CSRBbit04
	Variable/G root:pixie4:Chan2CSRBbit05
	Variable/G root:pixie4:Chan2CSRBbit06
	Variable/G root:pixie4:Chan2CSRBbit07
	Variable/G root:pixie4:Chan2CSRBbit08
	Variable/G root:pixie4:Chan2CSRBbit09
	Variable/G root:pixie4:Chan2CSRBbit10
	Variable/G root:pixie4:Chan2CSRBbit11
	Variable/G root:pixie4:Chan2CSRBbit12
	Variable/G root:pixie4:Chan2CSRBbit13
	Variable/G root:pixie4:Chan2CSRBbit14
	Variable/G root:pixie4:Chan2CSRBbit15
	
	Variable/G root:pixie4:Chan3CSRBbit00	// variables for ChanCSRB checkboxes
	Variable/G root:pixie4:Chan3CSRBbit01
	Variable/G root:pixie4:Chan3CSRBbit02
	Variable/G root:pixie4:Chan3CSRBbit03
	Variable/G root:pixie4:Chan3CSRBbit04
	Variable/G root:pixie4:Chan3CSRBbit05
	Variable/G root:pixie4:Chan3CSRBbit06
	Variable/G root:pixie4:Chan3CSRBbit07
	Variable/G root:pixie4:Chan3CSRBbit08
	Variable/G root:pixie4:Chan3CSRBbit09
	Variable/G root:pixie4:Chan3CSRBbit10
	Variable/G root:pixie4:Chan3CSRBbit11
	Variable/G root:pixie4:Chan3CSRBbit12
	Variable/G root:pixie4:Chan3CSRBbit13
	Variable/G root:pixie4:Chan3CSRBbit14
	Variable/G root:pixie4:Chan3CSRBbit15
	
	Variable/G root:pixie4:Chan0CSRCbit00	// variables for ChanCSRC checkboxes
	Variable/G root:pixie4:Chan0CSRCbit01
	Variable/G root:pixie4:Chan0CSRCbit02
	Variable/G root:pixie4:Chan0CSRCbit03
	Variable/G root:pixie4:Chan0CSRCbit04
	Variable/G root:pixie4:Chan0CSRCbit05
	Variable/G root:pixie4:Chan0CSRCbit06
	Variable/G root:pixie4:Chan0CSRCbit07
	Variable/G root:pixie4:Chan0CSRCbit08
	Variable/G root:pixie4:Chan0CSRCbit09
	Variable/G root:pixie4:Chan0CSRCbit10
	Variable/G root:pixie4:Chan0CSRCbit11
	Variable/G root:pixie4:Chan0CSRCbit12
	Variable/G root:pixie4:Chan0CSRCbit13
	Variable/G root:pixie4:Chan0CSRCbit14
	Variable/G root:pixie4:Chan0CSRCbit15
	
	Variable/G root:pixie4:Chan1CSRCbit00	// variables for ChanCSRC checkboxes
	Variable/G root:pixie4:Chan1CSRCbit01
	Variable/G root:pixie4:Chan1CSRCbit02
	Variable/G root:pixie4:Chan1CSRCbit03
	Variable/G root:pixie4:Chan1CSRCbit04
	Variable/G root:pixie4:Chan1CSRCbit05
	Variable/G root:pixie4:Chan1CSRCbit06
	Variable/G root:pixie4:Chan1CSRCbit07
	Variable/G root:pixie4:Chan1CSRCbit08
	Variable/G root:pixie4:Chan1CSRCbit09
	Variable/G root:pixie4:Chan1CSRCbit10
	Variable/G root:pixie4:Chan1CSRCbit11
	Variable/G root:pixie4:Chan1CSRCbit12
	Variable/G root:pixie4:Chan1CSRCbit13
	Variable/G root:pixie4:Chan1CSRCbit14
	Variable/G root:pixie4:Chan1CSRCbit15
	
	Variable/G root:pixie4:Chan2CSRCbit00	// variables for ChanCSRC checkboxes
	Variable/G root:pixie4:Chan2CSRCbit01
	Variable/G root:pixie4:Chan2CSRCbit02
	Variable/G root:pixie4:Chan2CSRCbit03
	Variable/G root:pixie4:Chan2CSRCbit04
	Variable/G root:pixie4:Chan2CSRCbit05
	Variable/G root:pixie4:Chan2CSRCbit06
	Variable/G root:pixie4:Chan2CSRCbit07
	Variable/G root:pixie4:Chan2CSRCbit08
	Variable/G root:pixie4:Chan2CSRCbit09
	Variable/G root:pixie4:Chan2CSRCbit10
	Variable/G root:pixie4:Chan2CSRCbit11
	Variable/G root:pixie4:Chan2CSRCbit12
	Variable/G root:pixie4:Chan2CSRCbit13
	Variable/G root:pixie4:Chan2CSRCbit14
	Variable/G root:pixie4:Chan2CSRCbit15
	
	Variable/G root:pixie4:Chan3CSRCbit00	// variables for ChanCSRC checkboxes
	Variable/G root:pixie4:Chan3CSRCbit01
	Variable/G root:pixie4:Chan3CSRCbit02
	Variable/G root:pixie4:Chan3CSRCbit03
	Variable/G root:pixie4:Chan3CSRCbit04
	Variable/G root:pixie4:Chan3CSRCbit05
	Variable/G root:pixie4:Chan3CSRCbit06
	Variable/G root:pixie4:Chan3CSRCbit07
	Variable/G root:pixie4:Chan3CSRCbit08
	Variable/G root:pixie4:Chan3CSRCbit09
	Variable/G root:pixie4:Chan3CSRCbit10
	Variable/G root:pixie4:Chan3CSRCbit11
	Variable/G root:pixie4:Chan3CSRCbit12
	Variable/G root:pixie4:Chan3CSRCbit13
	Variable/G root:pixie4:Chan3CSRCbit14
	Variable/G root:pixie4:Chan3CSRCbit15
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////// 
	// Other global variables (Igor only, not related to settings file		//
	///////////////////////////////////////////////////////////////////////////////////////////////////
	
	///////////////////////////////////////////////////////////// 
	// Calibrate global variables	                          //
	////////////////////////////////////////////////////////////	

	Variable/G root:pixie4:LastTau
	Variable/G root:pixie4:TauDeviation

	
	
	///////////////////////////////////////////////////////////// 
	// Run global variables	                          //
	////////////////////////////////////////////////////////////	
	Variable/G root:pixie4:PollingTime
	Variable/G root:pixie4:RunTimeOut
	Variable/G root:pixie4:KeepRunTimeOut
	Variable/G root:pixie4:SpillTimeOut
	Variable/G root:pixie4:KeepSpillTimeOut
	Variable/G root:pixie4:WhichRun
	Variable/G root:pixie4:RepCount
	Variable/G root:pixie4:KeepRepCount
	Variable/G root:pixie4:TotalSpillCount
	Variable/G root:pixie4:SynchAlways
	Variable/G root:pixie4:RunNumber
	Variable/G root:pixie4:RunActive
	Variable/G root:pixie4:RunInProgress
	Variable/G root:pixie4:AutoRunNumber
	Variable/G root:pixie4:AutoStoreSpec
	Variable/G root:pixie4:AutoStoreSettings
	Variable/G root:pixie4:AutoStoreStatistics
	Variable/G root:pixie4:AutoUpdateMCA
	Variable/G root:pixie4:AutoUpdateRunStats
	Variable/G root:pixie4:Tupdate
	Variable/G root:pixie4:AutoNewFile
	Variable/G root:pixie4:TNewFile
	Variable/G root:pixie4:DisableLMparsing
	String/G root:pixie4:OutBaseName
	String/G root:pixie4:OutputFileName
	String/G root:pixie4:SettingsFileName
	String/G root:pixie4:StatisticsFileName
	String/G root:pixie4:MCAFileName	
	String/G root:pixie4:ReqStopDate	
	String/G root:pixie4:ReqStopTime
	String/G root:pixie4:ReqStartDate	
	String/G root:pixie4:ReqStartTime
	Variable/G root:pixie4:StartDTreq
	Variable/G root:pixie4:StopDTreq
	Variable/G root:pixie4:SpillCountNewFile
	Variable/G root:pixie4:OldCSR

	///////////////////////////////////////////////////////////// 
	// Analyze global variables	                   //
	////////////////////////////////////////////////////////////
	
	Variable/G root:pixie4:StartTime_s		//start time/date in seconds from 1904
	Variable/G root:pixie4:SeriesStartTime_s		//start time/date in seconds from 1904
	Variable/G root:pixie4:StopTime_s		//stop time/date in seconds from 1904
	Variable/G root:pixie4:CurrentTime_s	//last statistics readout time/date in seconds from 1904
	String/G root:pixie4:InfoSource		//can be file or read from module
	String/G root:pixie4:MCASource	//can be file or read from module
	String/G root:pixie4:StartTime
	String/G root:pixie4:SeriesStartTime
	String/G root:pixie4:StopTime
	
	Make/o/n=2000 root:pixie4:ER0, root:pixie4:ER1, root:pixie4:ER2, root:pixie4:ER3, root:pixie4:ER4
	Make/o/n=2000 root:pixie4:ERtimestamp
	Make/o/n=4 root:pixie4:lastNE, root:pixie4:lastRT, root:pixie4:TrackMod, root:pixie4:TrackChan
	Variable/G root:pixie4:TrackER
	Variable/G root:pixie4:TrackNum
	
	///////////////////////////////////////////////////////////// 
	// Graph global variables	                   //
	////////////////////////////////////////////////////////////

	Variable/G root:pixie4:FFTbin
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TauTrace
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceFFT
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceFilter
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceFilterSF
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceFilterFF
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceFilterSFMarkers
	Make/o/n=(root:pixie4:ADCTraceLen) root:pixie4:TraceTH
	// Added /i/u to avoid "bit errors" obvious with test patterns
	Make/o/i/u/n=(root:pixie4:ADCTraceLen) root:pixie4:ADCch0
	Make/o/i/u/n=(root:pixie4:ADCTraceLen) root:pixie4:ADCch1
	Make/o/i/u/n=(root:pixie4:ADCTraceLen) root:pixie4:ADCch2
	Make/o/i/u/n=(root:pixie4:ADCTraceLen) root:pixie4:ADCch3
	Make/o/n=1 root:pixie4:sf
	Make/o/n=1 root:pixie4:ff
	Make/o/n=1 root:pixie4:seltrace
	Make/o/n=1 root:pixie4:th
	Make/o/n=1 root:pixie4:sfmarkers
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAch0
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAch1
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAch2
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAch3
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAch4		// 4 is ref 
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotal0	 // cumulative for multi-file runs
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotal1
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotal2
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotal3
	root:pixie4:MCAtotal0 = 0 	 // cumulative for multi-file runs
	root:pixie4:MCAtotal1 = 0
	root:pixie4:MCAtotal2 = 0
	root:pixie4:MCAtotal3 = 0
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotalold0	 // cumulative for multi-file runs
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotalold1
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotalold2
	Make/o/i/u/n=(root:pixie4:MCALen) root:pixie4:MCAtotalold3
	root:pixie4:MCAtotalold0 = 0 	 // cumulative for multi-file runs
	root:pixie4:MCAtotalold1 = 0
	root:pixie4:MCAtotalold2 = 0
	root:pixie4:MCAtotalold3 = 0
	Make/o/i/u/n=16384 root:pixie4:MCAtotalsum
	Make/o/i/u/n=16384 root:pixie4:MCAsum	
	Make/o/i/u/n=1 root:pixie4:Spectrum0
	Make/o/i/u/n=1 root:pixie4:Spectrum1
	Make/o/i/u/n=1 root:pixie4:Spectrum2
	Make/o/i/u/n=1 root:pixie4:Spectrum3
	Make/o/i/u/n=1 root:pixie4:Spectrum4					// 4 is ref 
	Make/o/u/i/n=(root:pixie4:MCALen*root:pixie4:NumberOfChannels) root:pixie4:mcawave	
	Variable/G root:pixie4:xmax
	Variable/G root:pixie4:xmin	
	Make/o/i/u/n=1 root:pixie4:trace0
	Make/o/i/u/n=1 root:pixie4:trace1
	Make/o/i/u/n=1 root:pixie4:trace2
	Make/o/i/u/n=1 root:pixie4:trace3
	Make/o/i/u/n=1 root:pixie4:trace4					// 4 is ref 
	//make/o/u/i/n=(len*2*NumberOfChannels) root:pixie4:EventPSAValues //XXX
	make/o/u/i/n=(1*3*root:pixie4:NumberOfChannels) root:pixie4:traceposlen // make wave to store trace location (in list mode file), trace length, and energy of each event
	make/o/u/i/n=(2) root:pixie4:eventposlen // make wave to store event location (in list mode file) and event length
	Variable/G root:pixie4:wftimescale	// sampling rate of LM traces (MHz)
	
	///////////////////////////////////////////////////////////// 
	// Settings copy/extract global variables  //
	////////////////////////////////////////////////////////////
	Variable/G root:pixie4:CopySettingsSourceMod
	Variable/G root:pixie4:CopySettingsSourceChan
	Variable/G root:pixie4:CopySettingsBitMask
	String/G root:pixie4:ExtractSettingsFile
	Make/o/u/w/n=(2,3) root:pixie4:CopyItemsColorWave
	make/o/t/n=(1,5) root:pixie4:CopySettingsModChan
	make/o/n=(1,5,2) root:pixie4:CopySettingsModChanData
	Make/o/u/w/n=1 root:pixie4:CopySettingsDataWave

	///////////////////////////////////////////////////////////// 
	// Gauss fit global variables                    //
	////////////////////////////////////////////////////////////
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAStartFitChannel
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAEndFitChannel
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAFitRange
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAscale
	root:pixie4:MCAStartFitChannel = 1
	root:pixie4:MCAEndFitChannel = 32768
	root:pixie4:MCAFitRange = 1
	root:pixie4:MCAscale = 1
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAChannelPeakPos
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAChannelPeakEnergy
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAChannelFWHMPercent
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAChannelFWHMAbsolute
	Make/o/n=(root:pixie4:NumberOfChannels*2+2) root:pixie4:MCAChannelPeakArea
	Make/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListStartFitChannel
	Make/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListEndFitChannel
	Make/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListChannelPeakPos
	Make/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListChannelFWHMPercent
	Make/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListChannelPeakArea
	
	Variable/G root:pixie4:MCAfitOption

	
	///////////////////////////////////////////////////////////// 
	// Histogram global variables                  //
	////////////////////////////////////////////////////////////	
	Variable/G root:pixie4:NHistoBins
	Variable/G root:pixie4:HistoDE
	Variable/G root:pixie4:HistoEmin
	Variable/G root:pixie4:HistoFirstEvent
	Variable/G root:pixie4:HistoLastEvent
	Make/o/n=(root:pixie4:NumberOfChannels) root:pixie4:Emink
	Make/o/n=(root:pixie4:NumberOfChannels) root:pixie4:dxk	
	Make/o/n=(root:pixie4:NumberOfChannels) root:pixie4:Nbink	

	///////////////////////////////////////////////////////////// 
	// Debug global variables                       //
	////////////////////////////////////////////////////////////
	Make/o/t/n=(root:pixie4:DSPVarLen) root:pixie4:DSPNames
	Make/o/u/w/n=(root:pixie4:DSPVarLen) root:pixie4:DSPValues
	Make/o/t/n=(root:pixie4:MemVarLen) root:pixie4:MemoryNames4
	Make/o/t/n=(root:pixie4:MemVarLen) root:pixie4:MemoryNames500e
	Make/o/u/w/n=(root:pixie4:MemVarLen) root:pixie4:MemoryValues

	///////////////////////////////////////////////////////////// 
	// File/Path global variables                    //
	////////////////////////////////////////////////////////////
	String/G root:pixie4:FPGAFile4e
	String/G root:pixie4:CommFPGAFile4
	String/G root:pixie4:CommFPGAFile500
	String/G root:pixie4:SPFPGAFile4
	String/G root:pixie4:SPFPGAFile500
	String/G root:pixie4:FPGAFile500e
	
	String/G root:pixie4:DSPCodeFile4
	String/G root:pixie4:DSPVarFile4
	String/G root:pixie4:DSPListFile4
	String/G root:pixie4:DSPCodeFile500
	String/G root:pixie4:DSPCodeFile500e
	String/G root:pixie4:DSPListFile500e
	
	String/G root:pixie4:DSPParaValueFile
	
	String/G root:pixie4:DataFile
	String/G root:pixie4:longDataFileName
	String/G root:pixie4:DSPFileDirectory
	String/G root:pixie4:FirmwareFileDirectory
	String/G root:pixie4:ConfigFileDirectory
	String/G root:pixie4:DSPFileDirectory
	String/G root:pixie4:DSPFileDirectory
	String/G root:pixie4:MCAFileDirectory
	String/G root:pixie4:EventFileDirectory
	Make/o/t/n=16 root:pixie4:AllFiles
	
	///////////////////////////////////////////////////////////// 
	// Pulse Shape Analysis global variables //
	////////////////////////////////////////////////////////////
	Variable/G root:pixie4:ChosenEvent
	Variable/G root:pixie4:DisplayTimeWindow
	Variable/G root:pixie4:Show4LMtraces
	variable/G root:Pixie4:EventHitpattern
	variable/G root:Pixie4:EventTimeHI
	variable/G root:Pixie4:EventTimeLO
	Make/u/i/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListModeChannelEnergy
	Make/u/i/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListModeChannelTrigger
	Make/u/i/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListModeChannelXIA
	Make/u/i/o/n=(root:pixie4:NumberOfChannels+1) root:pixie4:ListModeChannelUser
	Variable/G root:Pixie4:EvHit_Front
	Variable/G root:Pixie4:EvHit_Accept
	Variable/G root:Pixie4:EvHit_Status
	Variable/G root:Pixie4:EvHit_Token
	Variable/G root:Pixie4:EvHit_CoincOK
	Variable/G root:Pixie4:EvHit_Veto
	Variable/G root:Pixie4:EvHit_PiledUp
	Variable/G root:Pixie4:EvHit_WvFifoFull
	Variable/G root:Pixie4:EvHit_ChannelHit
	Variable/G root:Pixie4:EvHit_OOR
	Variable/G root:Pixie4:EvHit_Gate
	Variable/G root:Pixie4:EvHit_PSAovr
	Variable/G root:Pixie4:EvHit_Derror


	
	///////////////////////////////////////////////////////////// 
	//color wave for lists			             //
	////////////////////////////////////////////////////////////
	Make/o/u/w/n=(9,3) root:pixie4:ListColorWave
	root:pixie4:ListColorWave[0][0]=51456 // color 0: unused?: purple 
	root:pixie4:ListColorWave[0][1]=44032
	root:pixie4:ListColorWave[0][2]=58880
	root:pixie4:ListColorWave[1][0]=51456 // color 1: default non-editable cells: purple
	root:pixie4:ListColorWave[1][1]=44032
	root:pixie4:ListColorWave[1][2]=58880
	root:pixie4:ListColorWave[2][0]=60928 // color 2: editable cells: light gray
	root:pixie4:ListColorWave[2][1]=60928
	root:pixie4:ListColorWave[2][2]=60928
	root:pixie4:ListColorWave[3][0]=65280 // color 3: ch.0: red
	root:pixie4:ListColorWave[3][1]=0
	root:pixie4:ListColorWave[3][2]=0
	root:pixie4:ListColorWave[4][0]=0 // color 4: ch.1: bright green
	root:pixie4:ListColorWave[4][1]=58880
	root:pixie4:ListColorWave[4][2]=0
	root:pixie4:ListColorWave[5][0]=0 // color 5: ch.2: blue
	root:pixie4:ListColorWave[5][1]=15872
	root:pixie4:ListColorWave[5][2]=65280
	root:pixie4:ListColorWave[6][0]=0 // color 6: ch.3: dark green
	root:pixie4:ListColorWave[6][1]=26112
	root:pixie4:ListColorWave[6][2]=0
	root:pixie4:ListColorWave[7][0]=30000 // color 7: ref channel/other: dark gray (for black)
	root:pixie4:ListColorWave[7][1]=30000
	root:pixie4:ListColorWave[7][2]=30000
	root:pixie4:ListColorWave[8][0]=36864 // color 8: addback/other purple
	root:pixie4:ListColorWave[8][1]=14592
	root:pixie4:ListColorWave[8][2]=58880
	
	// initialize lists
	Pixie_MakeList_Chassis(0)
	Pixie_MakeList_AllRunStats(0)
	Pixie_MakeList_LMHisto()
	Pixie_MakeList_MCA(0)
	Pixie_MakeList_Traces(0)
	
	///////////////////////////////////////////////////////////// 
	// User accessible output data	             //
	////////////////////////////////////////////////////////////	
	
	// Create a new folder for output data
	NewDataFolder/o root:results
	
	//statistics
	Variable/G root:results:RunTime
	Variable/G root:results:EventRate
	Variable/G root:results:NumEvents		
	Make/o/n=(root:pixie4:NumberOfChannels) root:results:ChannelCountTime
	Make/o/n=(root:pixie4:NumberOfChannels) root:results:ChannelInputCountRate
	String/G root:results:StartTime
	String/G root:results:StopTime
	
	//MCAs
	Make/o/n=(root:pixie4:MCALen) root:results:MCAch0
	Make/o/n=(root:pixie4:MCALen) root:results:MCAch1
	Make/o/n=(root:pixie4:MCALen) root:results:MCAch2
	Make/o/n=(root:pixie4:MCALen) root:results:MCAch3
	make/o/n=16384 root:results:MCAsum
	
	//traces and list mode data
	Make/o/n=1 root:results:trace0
	Make/o/n=1 root:results:trace1
	Make/o/n=1 root:results:trace2
	Make/o/n=1 root:results:trace3
	make/o/u/i/n=(1*3*root:pixie4:NumberOfChannels) root:results:eventposlen // make wave to store trace location (in list mode file), trace length, and energy of each event
	make/o/u/i/n=(10) root:results:eventwave
	
	
	// File series variables
	make/o/n=10 root:results:tau0, root:results:peak0, root:results:Tgap0, root:results:Tpeak0, root:results:FWHM0
	make/o/n=10 root:results:tau1, root:results:peak1, root:results:Tgap1, root:results:Tpeak1, root:results:FWHM1
	make/o/n=10 root:results:tau2, root:results:peak2, root:results:Tgap2, root:results:Tpeak2, root:results:FWHM2
	make/o/n=10 root:results:tau3, root:results:peak3, root:results:Tgap3, root:results:Tpeak3, root:results:FWHM3
	
	NewDataFolder/o root:auto	
	Pixie_Scan_InitGlobals()

	///////////////////////////////////////////////////////////// 
	// Call to user routine			             //
	////////////////////////////////////////////////////////////	
	
	NewDataFolder/o root:user		//create the user data folder
	Variable/G root:user:UserVersion = 0x0105	// the version of the user function calls defined by XIA
	User_Globals()
	
	
	SetDataFolder root:
	
EndMacro


Macro Pixie_InitPixie4Files()

	///////////////////////////////////////// 
	// Set up file paths		//
	/////////////////////////////////////////
	
	// Home path
	String HomePathString, S_Path
	PathInfo home
	HomePathString = S_Path

	// DSP file directory
	NewPath/Z/O DSPPath root:pixie4:DSPFileDirectory
	PathInfo	DSPPath
	If(cmpstr(S_Path,root:pixie4:DSPFileDirectory)!=0)	// If the old directory no longer valid, use the default
		root:pixie4:DSPFileDirectory = HomePathString + "DSP:"
		NewPath/O DSPPath root:pixie4:DSPFileDirectory
		PathInfo	DSPPath
		root:pixie4:DSPFileDirectory=S_Path
	endif
	
	// Firmware file directory
	NewPath/Z/O FirmwarePath root:pixie4:FirmwareFileDirectory
	PathInfo	FirmwarePath
	If(cmpstr(S_Path,root:pixie4:FirmwareFileDirectory)!=0)	// If the old directory no longer valid, use the default
		root:pixie4:FirmwareFileDirectory = HomePathString + "Firmware:"
		NewPath/O FirmwarePath root:pixie4:FirmwareFileDirectory
		PathInfo	FirmwarePath
		root:pixie4:FirmwareFileDirectory=S_Path
	endif
	
	// Configuration file directory
	NewPath/Z/O ConfigurationPath root:pixie4:ConfigFileDirectory
	PathInfo	ConfigurationPath
	If(cmpstr(S_Path,root:pixie4:ConfigFileDirectory)!=0)	// If the old directory no longer valid, use the default
		root:pixie4:ConfigFileDirectory = HomePathString + "Configuration:"
		NewPath/O ConfigurationPath root:pixie4:ConfigFileDirectory
		PathInfo	ConfigurationPath
		root:pixie4:ConfigFileDirectory=S_Path
	endif	
	
	// MCA file directory
	NewPath/Z/O MCAPath root:pixie4:MCAFileDirectory
	PathInfo	MCAPath
	If(cmpstr(S_Path,root:pixie4:MCAFileDirectory)!=0)	// If the old directory no longer valid, use the default
		root:pixie4:MCAFileDirectory = HomePathString + "Data:"
		NewPath/O MCAPath root:pixie4:MCAFileDirectory
		PathInfo	MCAPath
		root:pixie4:MCAFileDirectory=S_Path
	endif
	
	// Pulse shape file directory
	NewPath/Z/O EventPath root:pixie4:EventFileDirectory
	PathInfo	EventPath
	If(cmpstr(S_Path,root:pixie4:EventFileDirectory)!=0)	// If the old directory no longer valid, use the default
		root:pixie4:EventFileDirectory = HomePathString + "Data:"
		NewPath/O EventPath root:pixie4:EventFileDirectory
		PathInfo	EventPath
		root:pixie4:EventFileDirectory=S_Path
	endif

EndMacro


Macro Pixie_InitVarNames()

	Pixie_IO_GetDSPNames()
	Pixie_IO_GetMemoryNames()

EndMacro


Macro Pixie_UseHomePaths()

	///////////////////////////////////////// 
	// Set up file paths		//
	/////////////////////////////////////////
	
	// Home path
	String HomePathString, S_Path
	PathInfo home
	HomePathString = S_Path

	// DSP file directory
	root:pixie4:DSPFileDirectory = HomePathString + "DSP:"
	NewPath/O DSPPath root:pixie4:DSPFileDirectory
	PathInfo	DSPPath
	root:pixie4:DSPFileDirectory=S_Path

	// Firmware file directory
	root:pixie4:FirmwareFileDirectory = HomePathString + "Firmware:"
	NewPath/O FirmwarePath root:pixie4:FirmwareFileDirectory
	PathInfo	FirmwarePath
	root:pixie4:FirmwareFileDirectory=S_Path
	
	// Configuration file directory
	root:pixie4:ConfigFileDirectory = HomePathString + "Configuration:"
	NewPath/O ConfigurationPath root:pixie4:ConfigFileDirectory
	PathInfo	ConfigurationPath
	root:pixie4:ConfigFileDirectory=S_Path
	
	// MCA file directory
	root:pixie4:MCAFileDirectory = HomePathString + "Data:"
	NewPath/O MCAPath root:pixie4:MCAFileDirectory
	PathInfo	MCAPath
	root:pixie4:MCAFileDirectory=S_Path
	
	// Pulse shape file directory
	root:pixie4:EventFileDirectory = HomePathString + "Data:"
	NewPath/O EventPath root:pixie4:EventFileDirectory
	PathInfo	EventPath
	root:pixie4:EventFileDirectory=S_Path
	
	///////////////////////////////////////// 
	// Initialize boot files		//
	/////////////////////////////////////////
	
	String setname
	
	//default
			setname = "default_125.set"
	if(root:pixie4:ModuleType ==500)		// legacy, not loaded
			setname = "default500.set"
	endif
	if(root:pixie4:ModuleType ==501)	
			setname = "default_500.set"
	endif
	if(root:pixie4:ModuleType ==5)	
			setname = "default_125.set"
	endif
	if(root:pixie4:ModuleType ==4)	
			setname = "default_75.set"
	endif
	
	// Communication FPGA files
	root:pixie4:FPGAFile4e = root:pixie4:FirmwareFileDirectory + "P4e_16_125_vdo.bin"
	root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "P4e_14_500_vdo.bin"
	root:pixie4:CommFPGAFile4 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
 	root:pixie4:CommFPGAFile500 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
	
	// Signal processing FPGA files	
	root:pixie4:SPFPGAFile4 = root:pixie4:FirmwareFileDirectory + "pixie.bin"
	//root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "FippiP500.bin"
	root:pixie4:FPGAFile500e = root:pixie4:FirmwareFileDirectory + "p500e_zdt.bin"
	
	// DSP I/O variable values file
	root:pixie4:DSPParaValueFile = root:pixie4:ConfigFileDirectory + setname
	
	// DSP code files
	root:pixie4:DSPCodeFile4 = root:pixie4:DSPFileDirectory + "PXIcode.bin"
	root:pixie4:DSPCodeFile500 = root:pixie4:DSPFileDirectory + "P500code.bin"
	root:pixie4:DSPCodeFile500e = root:pixie4:DSPFileDirectory + "P500e.ldr"			// for both P4e and P500e
		
	// DSP I/O variable names file
	root:pixie4:DSPVarFile4 = root:pixie4:DSPFileDirectory + "P500e.var"				// P4 has same var list, but the map2var produces only 416 names, so only use P4e/500e var file
	
	// DSP all variable names file
	root:pixie4:DSPListFile4 = root:pixie4:DSPFileDirectory + "PXIcode.lst"
	root:pixie4:DSPListFile500e = root:pixie4:DSPFileDirectory + "P500e.lst"
	
	////////////////////////////////////////////////////
	// Call to User function                 //
	///////////////////////////////////////////////////
	User_HomePaths()

EndMacro

Proc Pixie_Cleanup()

	// set default values that are hidden, Clib or Igor only, and may interfere with acquisition unexpectedly
	root:pixie4:SpillTimeOut = 10000000
	root:pixie4:repcount = 10000000
	root:pixie4:AutoRunNumber = 1
	root:pixie4:AutoStoreSpec = 1
	root:pixie4:AutoStoreSettings = 1
	root:pixie4:AutoProcessLMData = 0
	root:pixie4:KeepCW = 1
	root:pixie4:AutoStoreStatistics = 1
	root:pixie4:AutoUpdateMCA = 0
	root:pixie4:AutoNewFile = 0
	root:pixie4:DisableLMparsing = 0
	root:pixie4:StartDTreq = 0	
	root:pixie4:StopDTreq = 0		
	root:pixie4:SynchAlways = 0
	root:pixie4:RunTimeOut = 1000000
	root:pixie4:PollingTime = 0.1
	Pixie_Ctrl_CommonButton("ResetScaleMCA")
	Pixie_Ctrl_CommonButton("ResetScaleListModeSpectrum")
	
	
	KillDataFolder/Z root:test
	
	Killwaves/Z exmem, buffer
	Killwaves/Z LMdata0, wfarray, LMheader

End

//########################################################################
//	Pixie_GetRunType: Retrieve run type
//########################################################################
Function Pixie_GetRunType()
	Nvar WhichRun=root:pixie4:WhichRun
	return(WhichRun)
End

//########################################################################
//	Pixie_GetKeepCW: Retrieve option for coincidence wait minimum
//########################################################################
Function Pixie_GetKeepCW()
	Nvar KeepCW=root:pixie4:KeepCW
	return(KeepCW+1)	// add 1 for popup menu
End


//########################################################################
//	Pixie_GetFitOption: Retrieve MCA fit option
//########################################################################
Function Pixie_GetFitOption()
	Nvar MCAfitOption =  root:pixie4:MCAfitOption

	
	variable ret
	
	ret = 0
	ret = MCAfitOption
	
	if(ret==0)			// pick default value if nothing is selected
		MCAfitOption=4
		ret=4
	endif
	
	return(ret)
End



//########################################################################
//
//	Pixie_AnyGraphClose:
//		Close any graph.
//
//########################################################################
Function Pixie_AnyGraphClose(ctrlName) : ButtonControl
String ctrlName					
	DoWindow/K $WinName(0,1)
End


//########################################################################
//
//	Pixie_AnyPanelClose:
//		Close any panel.
//
//########################################################################
Function Pixie_AnyPanelClose(ctrlName) : ButtonControl
String ctrlName					
	DoWindow/K $WinName(0,64)
End







//########################################################################
//	Pixie_CallHelp: Display help messages
//########################################################################
Function Pixie_CallHelp(ctrlName): ButtonControl
String ctrlname

	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	String topic
	Variable len,k
	
	if (cmpstr(ctrlname,"cmnHelpButton")==0)
		switch(PanelTabNumber)	// numeric switch
			case 0:		
				DisplayHelpTopic "Trigger"
				break						// exit from switch
			case 1:		// execute if case matches expression
				DisplayHelpTopic "Energy"
				break
			case 2:
				DisplayHelpTopic "Waveform"
				break
			case 3:
				DisplayHelpTopic "Gate"
				break
			case 4:
				DisplayHelpTopic "Coincidence"
				break
			case 5:
				DisplayHelpTopic "Advanced"
				break
			case 6:
				DisplayHelpTopic "Run Control"
				break
		endswitch
	else	
		len = strlen(ctrlname)
		topic = ctrlname[4,len-1] // Control Name must be "help" + (help topic); use "_" for space
	
		do 									// replace '_' by ' '
			k=strsearch(topic,"_",0)
			if (k>=0)
				topic[k,k]=" "
			endif
		while(k>=0)

		DisplayHelpTopic topic
		
	endif
	
End


//########################################################################
//	Pixie_GetPXIChasisType: Retrieve PXI chasis type
//########################################################################
Function Pixie_GetPXIChasisType()
	Nvar MaxNumModules=root:pixie4:MaxNumModules
	
	Switch(MaxNumModules)
		Case 7:
			return(1)
			break
		Case 17:
			return(2)
			break
		Case 13:
			return(3)
			break
		Case 62:
			return(4)
			break
		Default:
			return(1)
			break
	EndSwitch
	
End




//########################################################################
//
//	Pixie_Ctrl_CommonSetVariable:
//		Handle variable value changes.
//
//########################################################################
Function Pixie_Ctrl_CommonSetVariable(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum

	// global variables and waves
	Nvar HistoEmin = root:pixie4:HistoEmin
	Nvar HistoDE = root:pixie4:HistoDE
	Nvar NHistoBins = root:pixie4:NHistoBins			
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename
	Nvar AutoProcessLMData=root:pixie4:AutoProcessLMData
	Nvar ChosenModule = root:pixie4:ChosenModule
	
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave Emink=root:pixie4:Emink
	Wave dxk=root:pixie4:dxk
	Wave Nbink=root:pixie4:Nbink	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	
	// local variables
	Variable i,j,len,NumTraces,ModLoc,index,filnum,totaltraces,dt,ret
	Variable tmin, tmax
	String wav
	
	Nvar RunInProgress = root:pixie4:RunInProgress

	
	StrSwitch(ctrlName)
		Case "NumberOfPixie4Modules":
		
			// Set the total number of modules and update the Module List Data
			Pixie_MakeList_Modules()
			DoWindow/F Pixie_Parameter_Setup
			if(V_flag==1)
				SetVariable SelectedPixie4Module limits={0,NumberOfModules-1,1},win=Pixie_Parameter_Setup
			endif
			DoWindow/F Pixie4MainPanel
			if(V_flag==1)
				SetVariable SelectedPixie4Module limits={0,NumberOfModules-1,1},win=Pixie4MainPanel
			endif
			DoWindow/F Pixie_Panel_StartUp
			break
			
			
		Case "HistoDataFile":
		Case "TraceDataFile":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else
			
				// open list mode data file for reading		
				Open/R/P=EventPath filnum as DataFile
				if(cmpstr(S_fileName, "")!= 0) // File opened successfully
					longDataFilename = Pixie_FileNameConversion(S_fileName)
					close filnum
				else
					doalert 0, "Can not open list mode event file."
					return(-1)
				endif
	
				Pixie_ExtractNumPosLen()
				
				//////////////////////////////////////////////////////////// 
				// Call to user routine			             //
				User_TraceDataFile()
				////////////////////////////////////////////////////////////	
			endif
			break
			
		Case "CallReadEvents":
		Case "DigitalFilterSelectEvent":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else
				Pixie_IO_Readevent()
			endif
			break

		Case "SetHistoEmin":
		
			Emink[ChosenChannel]=HistoEmin
			break
			
		Case "SetHistoDE":
		
			dxk[ChosenChannel]=HistoDE		
			break
			
		Case "SetNbin":
		
			Nbink[ChosenChannel]=NHistoBins		
			break
			
		Case "AutoProcessLMDataVar":
			System_Parameter_Values[Pixie_Find_SysParInWave("AUTO_PROCESSLMDATA")]=AutoProcessLMData
			Pixie4_User_Par_IO(System_Parameter_Values, "AUTO_PROCESSLMDATA", "SYSTEM", 0, ChosenModule, ChosenChannel)			
			break
						
		Default:
			break
	EndSwitch

End


//########################################################################
//
//	Pixie_Ctrl_CommonCheckBox:
//		Handle check box changes.
//
//########################################################################
Function Pixie_Ctrl_CommonCheckBox(ctrlName, checked) : CheckBoxControl
	String ctrlName
	Variable checked

	Nvar AutoProcessLMData=root:pixie4:AutoProcessLMData
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values

	StrSwitch(ctrlName)
				
		Case "AutoProcessCompressedLMData":
			AutoProcessLMData = Checked
			System_Parameter_Values[Pixie_Find_SysParInWave("AUTO_PROCESSLMDATA")]=AutoProcessLMData
			Pixie4_User_Par_IO(System_Parameter_Values, "AUTO_PROCESSLMDATA", "SYSTEM", 0, ChosenModule, ChosenChannel)			
			break
			
		Default:
					
	EndSwitch

End

Function Pixie_Ctrl_CheckTrace(ctrlName,checked) : CheckBoxControl
	String ctrlName
	Variable checked
	Variable k,n
	String wav
	Silent 1
	k=strlen(ctrlName)
	n=str2num(ctrlName[k-1,k-1])
	wav=ctrlName
	if(checked)
		AppendToGraph $("root:pixie4:"+wav)
		do
			if(n==1)
				ModifyGraph rgb($wav)=(0,65280,0)
			endif
			if(n==2)
				ModifyGraph rgb($wav)=(0,15872,65280)
			endif
			if(n==3)
				ModifyGraph rgb($wav)=(0,26112,0)
			endif
			if(cmpstr(wav,"MCASum")==0)
				ModifyGraph rgb($wav)=(0,0,0)
			endif
		while(0)
		ModifyGraph mode=6
		ModifyGraph grid=1
		ModifyGraph mirror(bottom)=2
		ModifyGraph mirror(left)=2
	else
		RemoveFromGraph/Z $(wav)
		wav="fit_"+wav
		RemoveFromGraph/Z $(wav)
	endif

End



//########################################################################
//
//	Pixie_Ctrl_SetupButtonControl:
//		Handle button click routines from Parameter Setup Panel.
//
//########################################################################
Function Pixie_Ctrl_SetupButtonControl(ctrlName) : ButtonControl
	String ctrlName

	// global variables and waves
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	
	Nvar chEFRTLow = root:auto:chEFRTLow
	Nvar chEFRTHigh = root:auto:chEFRTHigh
	Nvar chEFFTLow = root:auto:chEFFTLow
	Nvar chEFFTHigh = root:auto:chEFFTHigh
	

	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave EFRTLow = root:auto:EFRTLow
	Wave EFRTHigh = root:auto:EFRTHigh
	Wave EFFTLow = root:auto:EFFTLow
	Wave EFFTHigh = root:auto:EFFTHigh

	// local variables
	Variable len,filnum,i,j,ret,dt,tmax,tmin
	
	Nvar RunInProgress = root:pixie4:RunInProgress

	// none of the controls below should be used during the run
	if(RunInProgress)
		print "Run in progress. Please stop run before attempting this task"
		return(0)
	endif

	StrSwitch(ctrlName)

		//Common	
			
		Case "cmnLoad":
		Case "cmnSave":
			Pixie_File_Settings(ctrlName)
			break	
			
		Case "cmnCopy":
			Execute "Pixie_Panel_CopySettings()"
			break
			
		Case "cmnExtract":
			Execute "Pixie_Panel_ExtractSettings()"
			break	
			
		Case "cmnOscilloscope":
			Execute "Pixie_Plot_Oscilloscope()"
			break
		
		//Energy Tab			
	//	Case "EGX_OptimizeE":
	//		chEFRTLow = EFRTLow[ChosenChannel]
	//		chEFRTHigh = EFRTHigh[ChosenChannel]
	//		chEFFTLow = EFFTLow[ChosenChannel]
	//		chEFFTHigh = EFFTHigh[ChosenChannel]
	//		Pixie_Auto_EFOptimization()
	//		break	

		Case "EGY_TauFinder":
			Execute "Pixie_Panel_PleaseWait()"
			make/o/n=4 root:pixie4:oldtau
			Make/o/u/i/n=0 offset1
			Wave oldtau = root:pixie4:oldtau
			Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
			Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
			Nvar index_TAU = root:pixie4:index_TAU 
			Nvar ncp = root:pixie4:NumChannelPar
			Variable ch, error
			String ch_error
			String str1 = ""
	
			for(ch=0;ch<4;ch+=1)
				oldtau[ch] =  Channel_Parameter_Values[Pixie_Find_ChanParInWave("TAU")][ch][ChosenModule]		
			endfor
			//Pixie4_User_Par_IO(Channel_Parameter_Values,"FIND_TAU", "CHANNEL", 1, ChosenModule, 0)	// any channel will do, C lib loops
			Pixie4_Acquire_Data(0x0081, offset1, str1,  NumberOfModules)
			Pixie_IO_ReadChanParFromMod( NumberOfModules)
			Pixie_IO_SendChanParToIgor()
			
			DoWindow/K PleaseWaitPanel
			
			// Check if the Tau value returned by Tau_Finder is the same as the old Tau value (to 4 digits)
			ch_error = " "
			error=0
			for(ch=0;ch<4;ch+=1)
				if(floor(oldtau[ch]*10000) ==  floor(Channel_Parameter_Values[Pixie_Find_ChanParInWave("TAU")][ch][ChosenModule] *10000) )
					ch_error +=(num2str(ch)+" ")
					error = 1
				endif
			endfor
			if (error)	
				DoAlert 0, "The automatic Tau Finder failed to determine the decay time for channel(s)"+ch_error+".\nPlease verify that the signal is in range."
			endif

			break	
			
			break

	//	Case "EGX_OptimizeTau":
	//		Pixie_Auto_TauOptimization()	
	//		break
			
		Case "EGX_Scan":
			Pixie_Scan_ControlPanel()
			break		
			
		// Advanced Tab	
		Case "ADX_BLcutUpdate":
			Execute "Pixie_Panel_PleaseWait()"
			Make/o/u/i/n=0 offset
			String str=""
			Pixie4_Acquire_Data(0x0080, offset, str,  NumberOfModules)
			Pixie_IO_ReadChanParFromMod( NumberOfModules)
			Pixie_IO_SendChanParToIgor()
			DoWindow/K PleaseWaitPanel
			break	
			
		Case "ADX_I2ECalib":
			Pixie_I2E_CalibrateADC()
			break
			
		Case "ADX_I2EReset":
			Pixie_I2E_ResetADC()
			break
		
		//Run Tab	
		Case "RUX_RecordOption":
			Execute "Pixie_Panel_DataRecOption()"
			break

			
		Default:
			break
	EndSwitch			

End





//########################################################################
//
//	Pixie_Ctrl_CommonButton:
//		Handle button click routines.
//
//########################################################################
Function Pixie_Ctrl_CommonButton(ctrlName) : ButtonControl
	String ctrlName

	// global variables and waves
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar PRESET_MAX_MODULES = root:pixie4:PRESET_MAX_MODULES
	Nvar CopySettingsBitMask = root:pixie4:CopySettingsBitMask
	Nvar CopySettingsSourceMod = root:pixie4:CopySettingsSourceMod
	Nvar CopySettingsSourceChan = root:pixie4:CopySettingsSourceChan
	Nvar LastTau = root:pixie4:LastTau
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar EFMinRiseTimeTicks = root:pixie4:EFMinRiseTimeTicks
	Nvar EFMaxRiseTimeTicks = root:pixie4:EFMaxRiseTimeTicks
	Nvar EFMinFlatTopTicks = root:pixie4:EFMinFlatTopTicks
	Nvar EFMaxFlatTopTicks = root:pixie4:EFMaxFlatTopTicks
//	Nvar chEFRTLow = root:auto:chEFRTLow
//	Nvar chEFRTHigh = root:auto:chEFRTHigh
//	Nvar chEFFTLow = root:auto:chEFFTLow
//	Nvar chEFFTHigh = root:auto:chEFFTHigh
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	Nvar ADCTraceLen = root:pixie4:ADCTraceLen
	
	Svar ExtractSettingsFile = root:pixie4:ExtractSettingsFile
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename
	
	Wave/T Module_Parameter_Names = root:pixie4:Module_Parameter_Names
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave/T Channel_Parameter_Names = root:pixie4:Channel_Parameter_Names	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave/T System_Parameter_Names = root:pixie4:System_Parameter_Names	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	Wave/T DSPNames = root:pixie4:DSPNames
	Wave DSPValues = root:pixie4:DSPValues

	Wave CopySettingsDataWave = root:pixie4:CopySettingsDataWave
	Wave Emink = root:pixie4:Emink
	Wave dxk = root:pixie4:dxk
	Wave Nbink = root:pixie4:Nbink
	Nvar HistoFirstEvent =  root:pixie4:HistoFirstEvent
	Nvar HistoLastEvent =  root:pixie4:HistoLastEvent
//	
//	Wave EFRTLow = root:auto:EFRTLow
//	Wave EFRTHigh = root:auto:EFRTHigh
//	Wave EFFTLow = root:auto:EFFTLow
//	Wave EFFTHigh = root:auto:EFFTHigh
	
	Wave ADCch0 = root:pixie4:ADCch0
	Wave ADCch1 = root:pixie4:ADCch1
	Wave ADCch2 = root:pixie4:ADCch2
	Wave ADCch3 = root:pixie4:ADCch3

	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_FilterRange =  root:pixie4:index_FilterRange
	
	// local variables
	Variable len,filnum,i,j,totaltraces,ret,searchStr,lastcolonPos,dt,tmax,tmin,old_tau
	Variable 	xleftold, xrightold, xdiff, xleftnew, xrightnew, foundpulse, FR
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar ModuleType = root:pixie4:ModuleType

	StrSwitch(ctrlName)
		Case "chooseControllerType":
		
			Execute "Pixie_Panel_StartUp()"
			break
		
		Case "OfflineAnalysis":	
		Case "StartUpSystemNow":
			
			// Check if offline analysis is requested
			if(cmpstr(ctrlName, "OfflineAnalysis") == 0)
				Pixie4Offline	= 1
			else
				Pixie4Offline = 0
			endif
			

			// Ready to boot the system up, and kill the Pixie4 StartupSystem Panel
			ret=Pixie_StartUp()
			if(ret<0)  // Download names failure
				DoAlert 0, "Failed to download system configuration file names and global variable names."
				return (ret)
			endif
			
			ret=Pixie4_Boot_System(0x1F)
			if(ret<0)  // Boot failed
				DoAlert 0, "Module boot error. Please see details in history window"
				return (ret)
			endif
		//	return(0)

			// Update system parameters
			Pixie_IO_ReadSysParFromMod()
			Pixie_IO_SendSysParToIgor()

			// Update module parameters
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor()
			
			// Update channel parameters
			Pixie_IO_ReadChanParFromMod(NumberOfModules)
			Pixie_IO_SendChanParToIgor()
			
			// Set the Same Run Type to all modules globally and update their corresponding MaxEvents
			Pixie_Ctrl_CommonPopup("RunWhichRun", WhichRun, "")

			// Set the current module and channel
			if(ChosenModule > (NumberOfModules-1))
				ChosenModule = NumberOfModules - 1
			elseif(ChosenModule < 0)
				ChosenModule = 0
			endif							
			Pixie4_Set_Current_ModChan(ChosenChannel,ChosenModule)	
 
 			// Update Energy Filter time limits
 			
 			DoWindow/F Pixie_Parameter_Setup
 			if(V_Flag == 1)
				dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
				SetVariable EGY_ENERGY_FLATTOP0, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP1, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP2, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP3, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME0, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME1, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME2, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME3, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
			endif
			
			// Update DSP values
			Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
			root:pixie4:DSPValues=DSPValues
			
//			// Update energy filter auto optimization limits
//			for(j=0; j<NumberOfChannels; j+=1)
//				dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
//				EFRTLow[j] = EFMinRiseTimeTicks*dt
//				EFRTHigh[j] = EFMaxRiseTimeTicks*dt
//				EFFTLow[j] = EFMinFlatTopTicks*dt
//				EFFTHigh[j] = EFMaxFlatTopTicks*dt
//			endfor
			
			
			//////////////////////////////////////////////////////////// 
			// Call to user routine			             //
			User_ChangeChannelModule()
			////////////////////////////////////////////////////////////	

			// Close the StartUp panel
			DoWindow/K Pixie_Panel_StartUp
			
			Pixie_VersionCheck()
			
			if(Pixie4Offline == 0)
				Pixie_IO_ADCTrace()		// update Oscilloscope trace once (may prevent later crash in startrun?)
			endif
			
			break
			
	
			
		Case "AdjustDC":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else			
				Make/o/u/i/n=0 offset
				String str=""
				if(ModuleType==4)
					Execute "Pixie_Panel_PleaseWait()"
					Pixie4_Acquire_Data(0x0083, offset, str, NumberOfModules)
				else
					Pixie4_Acquire_Data(0x0085, offset, str, NumberOfModules)
				endif
				
				Pixie_IO_ReadChanParFromMod(NumberOfModules)
				Pixie_IO_SendChanParToIgor()
				
				Pixie_IO_ADCTrace()
				Pixie_IO_ADCTrace()		// the first time after adjust, the traces may look odd. 
	
				if(ModuleType==4)
					DoWindow/K PleaseWaitPanel		
				endif
			endif	
			break
			
			
		Case "ADCRefresh":
		Case "ADCFilterDisplayRefresh":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				Pixie_IO_ADCTrace()
			endif
			break
			
		Case "ADCDisplayCapture":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				foundpulse = 0
				do
					Pixie_IO_ADCTrace()
					DoUpdate
					wavestats/q ADCch0 
					if( (V_max - V_min) > 100)
						foundpulse =1
					endif
					wavestats/q ADCch1 
					if( (V_max - V_min) > 100)
						foundpulse =1
					endif
					wavestats/q ADCch2 
					if( (V_max - V_min) > 100)
						foundpulse =1
					endif
					wavestats/q ADCch3 
					if( (V_max - V_min) > 100)
						foundpulse =1
					endif
				while (foundpulse==0)
			endif
			break
			
		Case "ADCDisplaySave":
			save/t root:pixie4:adcch0, root:pixie4:adcch1, root:pixie4:adcch2, root:pixie4:adcch3 as "Oscilloscope.itx"
			break	
			
			

		Case "CopySettings":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				CopySettingsDataWave[0]=CopySettingsSourceMod
				CopySettingsDataWave[1]=CopySettingsSourceChan
				CopySettingsDataWave[2]=CopySettingsBitMask
				Pixie4_Buffer_IO(CopySettingsDataWave, 3, 1,"", ChosenModule)
	
				Pixie_IO_ReadModParFromMod(NumberOfModules)
				Pixie_IO_SendModParToIgor()
	
				Pixie_IO_ReadChanParFromMod(NumberOfModules)
				Pixie_IO_SendChanParToIgor()
			endif
			break
			
		Case "ExtractSettings":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
			
				CopySettingsDataWave[0]=CopySettingsSourceMod
				CopySettingsDataWave[1]=CopySettingsSourceChan
				CopySettingsDataWave[2]=CopySettingsBitMask
				Pixie4_Buffer_IO(CopySettingsDataWave, 3, 0,Pixie_FileNameConversion(ExtractSettingsFile), ChosenModule)			
	
				Pixie_IO_ReadModParFromMod(NumberOfModules)
				Pixie_IO_SendModParToIgor()
	
				Pixie_IO_ReadChanParFromMod(NumberOfModules)
				Pixie_IO_SendChanParToIgor()
			endif
			break
			

		Case "FFTDisplay":
		
			Execute "Pixie_Plot_FFTdisplay()"
			break
			
		Case "ADCFilterDisplay":
		
			Execute "Pixie_Plot_ADCFilter()"
			break
			
		Case "FilterFFT":
		
			Pixie_FilterFFT()
			break
			
		
		Case "CallLoadListModeSpec":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				// open list mode data file for reading
				Open/R/P=EventPath filnum as DataFile
				if(cmpstr(S_fileName, "")!= 0) // File opened successfully
					longDataFilename = Pixie_FileNameConversion(S_fileName)
					close filnum
				else
					Doalert 0, "Can not open list mode event file."
					return(-1)
				endif
			
				// wait ...
				Execute "Pixie_Panel_PleaseWait()"
				
				// make wave to store list mode data information, two 32-bit words for each module
				
				//**************************************************************************************
				//	Special considerations: for a large wave, we kill it before making a new one
				//	even though we intend to overwrite the previous one. According to WaveMetrics,
				//	killwaves should let Windows OS to release the memory.
				//**************************************************************************************
				killwaves/Z root:pixie4:listmodewave	
				make/o/u/i/n=(PRESET_MAX_MODULES*2) root:pixie4:listmodewave
				wave listmodewave = root:pixie4:listmodewave
				
				// clear listmodewave
				listmodewave = 0
				
				// parse the list mode event file
				ret = Pixie4_Acquire_Data(0x7001, listmodewave, longDataFilename, ChosenModule)
				if(ret < 0)
					Doalert 0, "Can not parse list mode event file."
					return(-1)
				endif
				
				// force update
				root:pixie4:listmodewave = listmodewave
				
				// read event energies
				len=listmodewave[ChosenModule]	// len contains number of events for current module
				if(len > 0)
	
					HistoLastEvent = max(1,len-1)
					 
					//**************************************************************************************
					//	Special considerations: for a large wave, we kill it before making a new one
					//	even though we intend to overwrite the previous one. According to WaveMetrics,
					//	killwaves should let Windows OS to release the memory.
					//**************************************************************************************
					killwaves/Z root:pixie4:EventEnergies		
				
					make/o/u/i/n=(len*NumberOfChannels) root:pixie4:EventEnergies
					wave EventEnergies = root:pixie4:EventEnergies
					ret = Pixie4_Acquire_Data(0x7004, EventEnergies, longDataFilename, ChosenModule)
					if(ret == 0x70)
					
						//**************************************************************************************
						//	Special considerations: for a large wave, we kill it before making a new one
						//	even though we intend to overwrite the previous one. According to WaveMetrics,
						//	killwaves should let Windows OS to release the memory.
						//**************************************************************************************
						killwaves/Z root:pixie4:EnergyWave0, root:pixie4:EnergyWave1, root:pixie4:EnergyWave2, root:pixie4:EnergyWave3		
					
						make/o/n=(len) root:pixie4:EnergyWave0
						make/o/n=(len) root:pixie4:EnergyWave1
						make/o/n=(len) root:pixie4:EnergyWave2
						make/o/n=(len) root:pixie4:EnergyWave3
						wave EnergyWave0 = root:pixie4:EnergyWave0
						wave EnergyWave1 = root:pixie4:EnergyWave1
						wave EnergyWave2 = root:pixie4:EnergyWave2
						wave EnergyWave3 = root:pixie4:EnergyWave3
						EnergyWave0 = EventEnergies[p*NumberOfChannels]
						EnergyWave1 = EventEnergies[p*NumberOfChannels+1]
						EnergyWave2 = EventEnergies[p*NumberOfChannels+2]
						EnergyWave3 = EventEnergies[p*NumberOfChannels+3]
					endif
				endif
				endif
				
				DoWindow/K PleaseWaitPanel
			break
			
		Case "CallListModeSpecEScatter":
			Execute "Pixie_Plot_LMEScatter()"		
			break
			
		Case "CallListModeSpecHisto":
		
			Variable ch
			ch=0
			do			
				String wavS, wavD
				wavD="root:pixie4:Spectrum"+num2str(Ch)

				//**************************************************************************************
				//	Special considerations: for a large wave, we kill it before making a new one
				//	even though we intend to overwrite the previous one. According to WaveMetrics,
				//	killwaves should let Windows OS to release the memory.
				//**************************************************************************************
				killwaves/Z $wavD		
				
				Make/o/n=(Nbink[ch]) $wavD
				wavS="root:pixie4:EnergyWave"+num2str(Ch)
				histogram/R=[HistoFirstEvent,HistoLastEvent]/b={Emink[ch], dxk[ch], Nbink[ch]} $wavS, $wavD
				ch+=1
			while(ch<4)
			break
			
		Case "FindTraceDataFile":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		

				Open/R/D/T="????"/P=EventPath filnum
				if(cmpstr(S_fileName, "")!= 0) // File opened successfully
					longDataFilename = Pixie_FileNameConversion(S_fileName)
					// Extract DataFile from S_fileName
					len=strlen(S_fileName)
					searchStr=strsearch(S_fileName, ":", 0)
					lastcolonPos=searchStr				
					do
						searchStr=strsearch(S_fileName, ":", searchStr+1)
						if(searchStr==-1)
							break
						else
							lastcolonPos=searchStr
						endif
					while(searchStr!=-1)
					DataFile=S_fileName[lastcolonPos+1, len-1]
				else
					doalert 0, "Can not open list mode event file."
					return(-1)
				endif
			
			
				Pixie_ExtractNumPosLen()
				
				//////////////////////////////////////////////////////////// 
				// Call to user routine			             //
				User_TraceDataFile()
				////////////////////////////////////////////////////////////	
			endif
			break
			
		Case "LMT_NextMod":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				// change module number
				ChosenModule+=1
				Pixie_IO_SelectModChan("SelectedPixie4Module",0,"","")	// also applies test for ChosenModule > N modules
				
				// change LM data file
				len = strlen(DataFile)
				sprintf DataFile,"%s%02d",Datafile[0,len-3], ChosenModule
				
				// open/parse LM data file
				 Pixie_Ctrl_CommonSetVariable("TraceDataFile",0,"","") 
				 
				 // update event
				  Pixie_Ctrl_CommonSetVariable("CallReadEvents",0,"","") 
				 endif
		
			break
			
		Case "LMT_PrevMod":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				// change module number
				ChosenModule=max(0,ChosenModule-1)
				Pixie_IO_SelectModChan("SelectedPixie4Module",0,"","")	// does not apply test for ChosenModule <0			
				// change LM data file
				len = strlen(DataFile)
				sprintf DataFile,"%s%02d",Datafile[0,len-3], ChosenModule
				
				// open/parse LM data file
				 Pixie_Ctrl_CommonSetVariable("TraceDataFile",0,"","") 
				 
				  // update event
				  Pixie_Ctrl_CommonSetVariable("CallReadEvents",0,"","") 
			endif
			break
				
			
		Case "AutoScaleMCA":
			
			SetAxis/A/W=MCASpectrumDisplay
			break
			
		Case "ResetScaleMCA":
			
			wave MCAch0 = root:pixie4:MCAch0
			wave MCAch1 = root:pixie4:MCAch1
			wave MCAch2 = root:pixie4:MCAch2
			wave MCAch3 = root:pixie4:MCAch3
			wave MCAsum = root:pixie4:MCAsum
			wave MCAscale = root:pixie4:MCAscale
			wave MCAStartFitChannel = root:pixie4:MCAStartFitChannel
			wave MCAEndFitChannel = root:pixie4:MCAEndFitChannel
			wave MCAChannelPeakPos = root:pixie4:MCAChannelPeakPos
			wave MCAChannelFWHMAbsolute = root:pixie4:MCAChannelFWHMAbsolute
			setscale/P x, 0,1, MCAch0
			setscale/P x, 0,1, MCAch1
			setscale/P x, 0,1, MCAch2
			setscale/P x, 0,1, MCAch3
			setscale/P x, 0,1, MCAsum
			
			MCAStartFitChannel/=MCAscale
			MCAEndFitChannel/=MCAscale
			MCAChannelPeakPos/=MCAscale
			MCAChannelFWHMAbsolute/=MCAscale
			
			MCAscale = 1
			Pixie_MakeList_MCA(1)
			break
			
		Case "ZoomInMCA":
		
			GetAxis/Q/W=MCASpectrumDisplay bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold + xdiff*0.1
			xrightnew = xrightold - xdiff*0.1
			
			if(xrightnew > xleftnew)
				SetAxis/W=MCASpectrumDisplay bottom xleftnew, xrightnew
				SetAxis/A/W=MCASpectrumDisplay left
			endif
			break		
			
		Case "ZoomOutMCA":
		
			GetAxis/Q/W=MCASpectrumDisplay bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold - xdiff*0.1
			xrightnew = xrightold + xdiff*0.1
			
			SetAxis/W=MCASpectrumDisplay bottom xleftnew, xrightnew
			SetAxis/A/W=MCASpectrumDisplay left
			break	
			
		Case "MCAGainMatch":	
			Wave dcp = root:pixie4:Display_Channel_Parameters
			Nvar  ncp = root:pixie4:NumChannelPar
			Nvar index_GAIN = root:pixie4:index_GAIN
			Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
			Variable gf
			gf = MCAChannelPeakPos[0]/MCAChannelPeakPos[1]					//old gain * ch.0 pos / ch.x pos 
			dcp[index_GAIN+1*ncp] =  dcp[index_GAIN+1*ncp] * gf					// update
			Pixie_IO_ChanVarControl("ADC_VGAIN1",0,"","")  						// apply
			gf = MCAChannelPeakPos[0]/MCAChannelPeakPos[2]					//old gain * ch.0 pos / ch.x pos 
			dcp[index_GAIN+2*ncp] =  dcp[index_GAIN+2*ncp] * gf
			Pixie_IO_ChanVarControl("ADC_VGAIN2",0,"","")  	
			gf = MCAChannelPeakPos[0]/MCAChannelPeakPos[3]					//old gain * ch.0 pos / ch.x pos 
			dcp[index_GAIN+3*ncp] =  dcp[index_GAIN+3*ncp] * gf
			Pixie_IO_ChanVarControl("ADC_VGAIN3",0,"","")  		
			break

		Case "ZoomMCAToCursors":
		
			if(xcsr(A) < xcsr(B))
				xleftold = xcsr(A)
				xrightold = xcsr(B)
			else
				xleftold = xcsr(B)
				xrightold = xcsr(A)
			endif
			
			GetAxis/Q/W=MCASpectrumDisplay bottom
			// Make sure Cursors are really on the graph
			if((xleftold >= V_min) && (xrightold <= V_max))
			
				xdiff = xrightold - xleftold
				
				xleftnew = xleftold - xdiff*0.05
				xrightnew = xrightold + xdiff*0.05
				
				SetAxis/W=MCASpectrumDisplay bottom xleftnew, xrightnew
				SetAxis/A/W=MCASpectrumDisplay left 
			endif
					
			break
			
		Case "AutoScaleListmodeSpectrum":
			
			SetAxis/A/W=ListModeSpectrumDisplay
			break
			
		Case "ZoomInListmodeSpectrum":
			
			GetAxis/Q/W=ListModeSpectrumDisplay bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold + xdiff*0.1
			xrightnew = xrightold - xdiff*0.1
			
			if(xrightnew > xleftnew)
				SetAxis/W=ListModeSpectrumDisplay bottom xleftnew, xrightnew
				SetAxis/A/W=ListModeSpectrumDisplay left
			endif
			
			break		
			
		Case "ZoomOutListmodeSpectrum":

			GetAxis/Q/W=ListModeSpectrumDisplay bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold - xdiff*0.1
			xrightnew = xrightold + xdiff*0.1
			
			SetAxis/W=ListModeSpectrumDisplay bottom xleftnew, xrightnew
			SetAxis/A/W=ListModeSpectrumDisplay left
			break	
			
		Case "ZoomListModeSpectrumToCursors":
		
			if(xcsr(A) < xcsr(B))
				xleftold = xcsr(A)
				xrightold = xcsr(B)
			else
				xleftold = xcsr(B)
				xrightold = xcsr(A)
			endif
			
			GetAxis/Q/W=ListModeSpectrumDisplay bottom
			// Make sure Cursors are really on the graph
			if((xleftold >= V_min) && (xrightold <= V_max))
			
				xdiff = xrightold - xleftold
				
				xleftnew = xleftold - xdiff*0.05
				xrightnew = xrightold + xdiff*0.05
				
				SetAxis/W=ListModeSpectrumDisplay bottom xleftnew, xrightnew
				SetAxis/A/W=ListModeSpectrumDisplay left 
			endif
					
			break
			
		Case "ResetScaleListModeSpectrum":
			
			wave Spectrum0 = root:pixie4:Spectrum0
			wave Spectrum1 = root:pixie4:Spectrum1
			wave Spectrum2 = root:pixie4:Spectrum2
			wave Spectrum3 = root:pixie4:Spectrum3
			//wave MCAsum = root:pixie4:MCAsum
			setscale/P x, 0,1, Spectrum0
			setscale/P x, 0,1, Spectrum1
			setscale/P x, 0,1, Spectrum2
			setscale/P x, 0,1, Spectrum3
			//setscale/P x, 0,1, MCAsum
			break	
		
		Case "AutoScaleOscilloscope":
			
			SetAxis/A/W=Pixie4Oscilloscope
			break
			
		Case "ZoomInOscilloscope":
			
			GetAxis/Q/W=Pixie4Oscilloscope bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold + xdiff*0.1
			xrightnew = xrightold - xdiff*0.1
			
			if(xrightnew > xleftnew)
				SetAxis/W=Pixie4Oscilloscope bottom xleftnew, xrightnew
				SetAxis/A/W=Pixie4Oscilloscope left 
			endif
			
			break		
			
		Case "ZoomOutOscilloscope":
		
			GetAxis/Q/W=Pixie4Oscilloscope bottom
			xleftold = V_min
			xrightold = V_max
			xdiff = xrightold - xleftold
			
			xleftnew = xleftold - xdiff*0.1
			xrightnew = xrightold + xdiff*0.1
			
			SetAxis/W=Pixie4Oscilloscope bottom xleftnew, xrightnew
			SetAxis/A/W=Pixie4Oscilloscope left 
			
			break	
			
		Case "TauClear":
			Nvar LastTau = root:pixie4:LastTau	// Clear LastTau and TauDeviation
			Nvar TauDeviation = root:pixie4:TauDeviation
			TauDeviation = 0 
			LastTau = 0
			RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch0
			RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch1
			RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch2
			RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch3
			RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch0
			RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch1
			RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch2
			RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch3
			break
			
		Case "FindPattern":	
			Execute "Pixie_PatternEditPanel()"	
			break	
			
		Case "ParseSeries":
			Pixie_Series_Process()	
			break			
			
		Case "OpenSeriesPlot":
			Execute "Pixie_Plot_FileSeries()"
			break		
			
		Case "SetScanRunConditions":
			Pixie_Scan_RunConditions()
			break	
			
		Case "SetScanToStart":
			Pixie_Scan_StartParameters()
			break
			
		Case "CallUseHomePath":
			Execute "Pixie_UseHomePaths()"
			break
			
		Case "PrintDateTime":
			print "Date:",date()
			print "Time:",time()
			break
			
		Case "ShowTrackRates":
			Execute "Pixie_Plot_TrackRates()"
			break
			
		Case "TrackClear":
			Pixie_RC_ClearTrack()
			break
			
		Case "UpdateRunStats":
			Pixie_RC_UpdateRunstats("module")
			break
			
		Case "MultiplyApply":
			if(RunInProgress)
				print "Run in progress. Please stop run before attempting this task"
			else		
				Nvar GainMultiplier =root:pixie4:GainMultiplier
				Nvar index_GAIN = root:pixie4:index_GAIN
				for(ChosenModule=0;ChosenModule<NumberOfModules;ChosenModule+=1)
					Pixie4_Set_Current_ModChan(0,ChosenModule)
					Pixie_IO_SendModParToIgor()
					for(ch=0;ch<NumberOfChannels;ch+=1)
						Channel_Parameter_Values[index_GAIN][ch][ChosenModule] *= GainMultiplier
						Pixie4_User_Par_IO(Channel_Parameter_Values,"VGAIN","CHANNEL", 0, ChosenModule, ch)
						// update display variables with values changed in C library 
						Pixie_IO_ReadChanParFromMod(ChosenModule)
						Pixie_IO_SendChanParToIgor()
					endfor
				endfor
				GainMultiplier = 1
				ChosenModule = 0
			endif
			break
			
					
		Default:
			break
	EndSwitch
	
End


//########################################################################
//
//	Pixie_Ctrl_CommonPopup:
//		Handle popup menu changes.
//
//########################################################################
Function Pixie_Ctrl_CommonPopup(ctrlName,popNum,popStr) : PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr

	// global variables and waves
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave RunTasks = root:pixie4:RunTasks
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar NumChannelPar = root:pixie4:NumChannelPar
	Nvar NumModulePar = root:pixie4:NumModulePar
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar FirstTimeUse = root:pixie4:FirstTimeUse
	Svar ConfigFileDirectory = root:pixie4:ConfigFileDirectory
	Svar DSPParaValueFile = root:pixie4:DSPParaValueFile
	
	Nvar MCAfitOption =  root:pixie4:MCAfitOption
	
	// local variables
	Variable k, direction, channum
	
	StrSwitch(ctrlName)
		Case "RUN_WhichRun":
		
			WhichRun=popNum
			direction=0	// Download to module
			channum=0	// Channel number is not important here
			
			// Set Run Type globally in all modules and update their corresponding MaxEvents
			for(k=0; k<NumberOfModules; k+=1)
				// Update Run Type and MaxEvents
				Module_Parameter_Values[Pixie_Find_ModuleParInWave("RUN_TYPE")][k]=RunTasks[WhichRun-1]
				Pixie4_User_Par_IO(Module_Parameter_Values,"RUN_TYPE", "MODULE", direction, k, channum)
			endfor
			
			// Update current module parameters
			Pixie_IO_ReadModParFromMod(ChosenModule)
			Pixie_IO_SendModParToIgor()	// includes chassis list
			
			DoWindow/F Pixie_Parameter_Setup
			if(V_flag==1)
				PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie_Parameter_Setup
			endif
			DoWindow/F Pixie4MainPanel
			if(V_flag==1)
				PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie4MainPanel
			endif
			break
		
		Case "ChoosePXIChasisType":
			
			if(popNum == 1)
				MaxNumModules = 7
			elseif(popNum == 2)
				MaxNumModules = 17
			elseif(popNum == 3)
				MaxNumModules = 13
			elseif(popNum == 4)
				MaxNumModules = 62		// for PXIe1062
			else
				MaxNumModules = 7
			endif
			
			// Update lmits for control NumberOfPixie4Modules
			SetVariable NumberOfPixie4Modules limits={1,MaxNumModules,1}, win=Pixie_Panel_StartUp
			
			if(FirstTimeUse == 0)				
				// Reset the FirstTimeUse
				FirstTimeUse = 2
			endif
			
			break
			
		Case "CallSetup":
			if(popNum == 1)
				Pixie_Panel_Parameters(0)
			elseif(popNum == 2)
				Execute "Pixie_Plot_Oscilloscope()"
			elseif(popNum == 3)
				Execute "Pixie_Panel_ChassisSettings()"
			elseif(popNum == 4)
				Execute "Pixie_Panel_AllFiles()"
			endif
			break
			
		Case "CallResults":
			if(popNum == 1)
				Execute "Pixie_Plot_MCADisplay()"
			elseif(popNum == 2)
				Execute "Pixie_Plot_LMTracesDisplay()"
			elseif(popNum == 3)
				Execute "Pixie_Plot_LMSpectrumDisplay()"
			elseif(popNum == 4)
				Execute "Pixie_Panel_SystemRunStats()"
			elseif(popNum == 5)
				Execute "Pixie_Plot_FileSeries()"
			endif
			break	
			
		Case "MCAFitOptions":
			MCAfitOption = popnum
			PopupMenu MCAFitOptions, mode=popnum, win = MCASpectrumDisplay
			break				
			
		Case "TauFit":
			Pixie_Math_TauFit(popnum-1)
			break	
			
		Case "TauOK":
			Nvar LastTau=root:pixie4:LastTau
			Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
			Nvar index_TAU = root:pixie4:index_TAU
			Nvar ncp = root:pixie4:NumChannelPar
			Display_Channel_Parameters[index_TAU+(popnum-1)*ncp] = LastTau
			Pixie_IO_ChanVarControl("EGY_TAU"+num2str(popnum-1),0,"","")
			break	
			
		Case "LMRefSelect":
			Wave src = $("root:pixie4:trace"+num2str(popnum-1))
			duplicate/o src, root:pixie4:trace4
			Wave ListModeChannelEnergy=root:pixie4:ListModeChannelEnergy
			Wave ListModeChannelTrigger=root:pixie4:ListModeChannelTrigger
			Wave ListModeChannelXIA=root:pixie4:ListModeChannelXIA
			Wave ListModeChannelUser=root:pixie4:ListModeChannelUser
			ListModeChannelEnergy[4] = ListModeChannelEnergy[popnum-1]
			ListModeChannelTrigger[4] = ListModeChannelTrigger[popnum-1]
			ListModeChannelXIA[4] = ListModeChannelXIA[popnum-1]
			ListModeChannelUser[4] = ListModeChannelUser[popnum-1]
			Pixie_MakeList_Traces(1)
			break
			
		Case "MCARefSelect":
			Wave src = $("root:pixie4:mcach"+num2str(popnum-1))
			duplicate/o src, root:pixie4:mcach4
			Wave MCAStartFitChannel=root:pixie4:MCAStartFitChannel
			Wave MCAEndFitChannel=root:pixie4:MCAEndFitChannel
			Wave MCAFitRange=root:pixie4:MCAFitRange
			Wave MCAscale=root:pixie4:MCAscale
			Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
			Wave MCAChannelPeakEnergy=root:pixie4:MCAChannelPeakEnergy
			Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
			Wave MCAChannelFWHMAbsolute=root:pixie4:MCAChannelFWHMAbsolute
			Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea
			MCAStartFitChannel[4] = MCAStartFitChannel[popnum-1]
			MCAEndFitChannel[4] = MCAEndFitChannel[popnum-1]
			MCAFitRange[4] = MCAFitRange[popnum-1]
			MCAscale[4] = MCAscale[popnum-1]			
			MCAChannelPeakPos[4] = MCAChannelPeakPos[popnum-1]
			MCAChannelPeakEnergy[4] = MCAChannelPeakEnergy[popnum-1]
			MCAChannelFWHMPercent[4] = MCAChannelFWHMPercent[popnum-1]
			MCAChannelFWHMAbsolute[4] = MCAChannelFWHMAbsolute[popnum-1]
			MCAChannelPeakArea[4] = MCAChannelPeakArea[popnum-1]		
			
			Pixie_MakeList_MCA(1)
			break
			
		Case "LMSRefSelect":
			Wave src = $("root:pixie4:spectrum"+num2str(popnum-1))
			duplicate/o src, root:pixie4:spectrum4
			Wave ListStartFitChannel=root:pixie4:ListStartFitChannel
			Wave ListEndFitChannel=root:pixie4:ListEndFitChannel
			Wave ListChannelPeakPos=root:pixie4:ListChannelPeakPos
			Wave ListChannelFWHMPercent=root:pixie4:ListChannelFWHMPercent
			Wave ListChannelPeakArea=root:pixie4:ListChannelPeakArea
			ListStartFitChannel[4] = ListStartFitChannel[popnum-1]
			ListEndFitChannel[4] = ListEndFitChannel[popnum-1]
			ListChannelPeakPos[4] = ListChannelPeakPos[popnum-1]
			ListChannelFWHMPercent[4] = ListChannelFWHMPercent[popnum-1]			
			ListChannelPeakArea[4] = ListChannelPeakArea[popnum-1]
			
			Pixie_MakeList_LMHisto()
			break
			
			
						
	//	Case "COI_KeepCW":
	//		Nvar KeepCW=root:pixie4:KeepCW
	//		Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	//		KeepCW = popnum-1
	//		System_Parameter_Values[Pixie_Find_SysParInWave("KEEP_CW")]=KeepCW
	//		Pixie4_User_Par_IO(System_Parameter_Values, "KEEP_CW", "SYSTEM", 0, ChosenModule, ChosenChannel)			
	//		break
							
		Default:
			break
	Endswitch
	
End

Function Pixie_Ctrl_MoreLess(ctrlName) : ButtonControl
String ctrlName

	Nvar HideDetail = root:pixie4:HideDetail 
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	Nvar prevTab= root:pixie4:PrevTabNumber
	
	//If button is clicked, change from full detail to minimum detail and vice versa
	If (HideDetail)
		HideDetail = 0
//		Button moreless, title="Less",win = Pixie4RunControlPanel
		DoWindow/F Pixie_Parameter_Setup
		if(V_flag==1)
			Button moreless, title="Less",win = Pixie_Parameter_Setup
		endif
	else
		HideDetail = 1
//		Button moreless, title="More",win = Pixie4RunControlPanel
		DoWindow/F Pixie_Parameter_Setup
		if(V_flag==1)
			Button moreless, title="More",win = Pixie_Parameter_Setup
		endif
	endif
	
	//update controls
	prevTab=-1
	Pixie_Tab_Switch("",PanelTabNumber)

End






//########################################################################
//	Pixie_ListProc_Modules: Track and update Module List Data
//########################################################################
Function Pixie_ListProc_Modules(ctrlName,row,col,event)
	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.
	
	Wave ModuleSerialNumber = root:pixie4:ModuleSerialNumber
	Wave ModuleSlotNumber = root:pixie4:ModuleSlotNumber
	Wave/T ModuleListData = root:pixie4:ModuleListData
	Wave ModuleSListData = root:pixie4:ModuleSListData
	Nvar MaxNumModules = root:pixie4:MaxNumModules

	if((event==7) && (col==0))  // ModuleSerialNumber was changed
		ModuleSerialNumber[row]=str2num(ModuleListData[row][col])
	endif
	
	if((event==7) && (col==1))  // ModuleSlotNumber was changed
	// TODO: KS commented it out so for board version A101 (P500e revB) we are using serial numbers, not physical slots
	// Might want to put a better switch here and check for serial number valid range
		//if(str2num(ModuleListData[row][col]) < 2 || str2num(ModuleListData[row][col]) >(MaxNumModules+1))
		//	doalert 0, "Slot_Number should be in the range of 2 to " + num2str(MaxNumModules+1) + "!"
		//else
			ModuleSlotNumber[row]=str2num(ModuleListData[row][col])
		//endif
	endif

	return 0			// other return values reserved
end


//########################################################################
//	Pixie_MakeList_Modules: Initialize Module List Data
//########################################################################
Function Pixie_MakeList_Modules()
	
	Nvar  NumberOfModules=root:pixie4:NumberOfModules
	Variable i
	String strLabel

	Make/o/t/n=(NumberOfModules,2) root:pixie4:ModuleListData
	Make/o/b/n=(NumberOfModules,2) root:pixie4:ModuleSListData
	Wave/T ModuleListData = root:pixie4:ModuleListData
	Wave ModuleSListData = root:pixie4:ModuleSListData	

	Make/o/n=(NumberOfModules) root:pixie4:ModuleSerialNumber
	Make/o/n=(NumberOfModules) root:pixie4:ModuleSlotNumber
	Make/o/u/w/n=(NumberOfModules) root:pixie4:ModuleSerialNumber_UINT16
	
	Wave ModuleSerialNumber = root:pixie4:ModuleSerialNumber
	Wave ModuleSerialNumber_UINT16 = root:pixie4:ModuleSerialNumber_UINT16
	for(i=0; i<NumberOfModules; i+=1)
		ModuleSerialNumber[i]=i
		ModuleSerialNumber_UINT16[i]=ModuleSerialNumber[i]
	endfor

	Wave ModuleSerialNumber = root:pixie4:ModuleSerialNumber
	Wave ModuleSlotNumber = root:pixie4:ModuleSlotNumber
	
	ModuleListData[][0]=num2str(ModuleSerialNumber[p])	
	ModuleListData[][1]=num2str(ModuleSlotNumber[p])	
	
	//* Module Number is not editable; All other data are editable *//
	ModuleSListData[][0]=0
	ModuleSListData[][1]=2

	strLabel="Module Number"
	setdimlabel 1,0,$strLabel,ModuleListData
	strLabel="Slot or Serial Number"
	setdimlabel 1,1,$strLabel,ModuleListData

End




//########################################################################
//	Pixie_MakeList_AllRunStats: Initialize Run Statistics List Data for Whole System
//########################################################################
Function Pixie_MakeList_AllRunStats(mode)
Variable mode	//0 - initialize when opening panel
				//1 - update values only

	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar CurrentTime_s = root:pixie4:CurrentTime_s
	Nvar StartTime_s = root:pixie4:StartTime_s
	Nvar StopTime_s = root:pixie4:StopTime_s
	
	if(mode==0)
		Make/o/t/n=(4*NumberOfModules,9) root:pixie4:AllChRunStats
		Make/o/b/n=(4*NumberOfModules,9,2) root:pixie4:AllChRunstats_S
		
		Make/o/t/n=(NumberOfModules,8) root:pixie4:AllModRunStats
		Make/o/b/n=(NumberOfModules,8,2) root:pixie4:AllModRunstats_S
	endif
		
	
	Wave/T AllChRunStats=root:pixie4:AllChRunStats
	Wave AllChRunstats_S=root:pixie4:AllChRunstats_S
	Wave/T AllModRunStats=root:pixie4:AllModRunStats
	Wave AllModRunstats_S=root:pixie4:AllModRunstats_S
	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	
	Variable index0, index1, index,DAQtime
	
	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
	Nvar index_ICR = root:pixie4:index_ICR
	Nvar index_OCR = root:pixie4:index_OCR
	Nvar index_SFDT = root:pixie4:index_SFDT
	Nvar index_GCR = root:pixie4:index_GCR
	Nvar index_EvRate = root:pixie4:index_EvRate
	Nvar index_RunTime = root:pixie4:index_RunTime
	Nvar index_TotTime = root:pixie4:index_TotTime
	Nvar index_GDT = root:pixie4:index_GDT
	Nvar index_PPR = root:pixie4:index_PPR
	Nvar index_CSFDT = root:pixie4:index_CSFDT
	Nvar index_CCT = root:pixie4:index_CCT
	Nvar index_CIR = root:pixie4:index_CIR
	
	
	
	// Channel statistics list
	AllChRunStats[][0]=num2str(floor(p/4))					//module
	AllChRunStats[][1]=num2str(mod(p,4))					//channel
	AllChRunStats[][2]=num2str(Channel_Parameter_Values[index_COUNTTIME][mod(p,4)][floor(p/4)])		//channel count time
	AllChRunStats[][3]=num2str(Channel_Parameter_Values[index_ICR][mod(p,4)][floor(p/4)])		// channel ICR
	AllChRunStats[][4]=num2str(Channel_Parameter_Values[index_OCR][mod(p,4)][floor(p/4)])		// channel OCR
	AllChRunStats[][5]=num2str(Channel_Parameter_Values[index_PPR][mod(p,4)][floor(p/4)])		// channel PPR
	AllChRunStats[][6]=num2str(Channel_Parameter_Values[index_SFDT][mod(p,4)][floor(p/4)])		// channel filter dead time
	AllChRunStats[][7]=num2str(Channel_Parameter_Values[index_GCR][mod(p,4)][floor(p/4)])		// channel gate rate
	AllChRunStats[][8]=num2str(Channel_Parameter_Values[index_GDT][mod(p,4)][floor(p/4)])		// channel gate dead time

	
	if(mode==0)
		// All list data are not editable
		AllChRunstats_S=0
		
		// Set up for background color
		AllChRunstats_S[][][1]=1
	
		String labelStr1="Count Time [s]"
		String labelStr2="ICR [cps]"
		String labelStr3="OCR [cps]"
		String labelStr4="PPR [cps]"
		String labelStr5="Filter Dead Time [s]"
		String labelStr6="Gate Rate [cps]"
		String labelStr7="Gate Time [s]"
		setdimlabel 1,0,Module,AllChRunStats
		setdimlabel 1,1,Channel,AllChRunStats
		setdimlabel 1,2,$labelStr1,AllChRunStats
		setdimlabel 1,3,$labelStr2,AllChRunStats
		setdimlabel 1,4,$labelStr3,AllChRunStats
		setdimlabel 1,5,$labelStr4,AllChRunStats
		setdimlabel 1,6,$labelStr5,AllChRunStats
		setdimlabel 1,7,$labelStr6,AllChRunStats
		setdimlabel 1,8,$labelStr7,AllChRunStats
		setdimlabel 2,1,backColors,AllChRunStats_S
	endif
	
	
	
	// Module statistics list
	AllModRunStats[][0]=num2str(p)					//module
	AllModRunStats[][1]=num2str(Module_Parameter_Values[index_RunTime][p])		//module run time
	AllModRunStats[][2]=num2str(Module_Parameter_Values[index_EvRate][p])			// module event rate = NumEvents/Runtime
	// calculate fraction of lab time the module is active
	// IMPROVE ME: this chould be based on module's realtime for better than 1s precision
//	if(StopTime_s >0)	//set to -1 while run in progress
//		DAQtime =  StopTime_s  - StartTime_s 
//	else
//		DAQtime =  CurrentTime_s  - StartTime_s 
//	endif
//	AllModRunStats[][3]= num2str(  Module_Parameter_Values[index_RunTime][p] / DAQtime  *100 )		//fraction of lab time the module is active
	AllModRunStats[][3]= num2str(  Module_Parameter_Values[index_RunTime][p] / Module_Parameter_Values[index_TotTime][p]  *100 )		//fraction of lab time the module is active
	AllModRunStats[][4]=num2str(Module_Parameter_Values[index_TotTime][p])		// module event rate = NumEvents/Runtime
	AllModRunStats[][5]=num2str(Module_Parameter_Values[index_CSFDT][p])		// module event rate = NumEvents/Runtime
	AllModRunStats[][6]=num2str(Module_Parameter_Values[index_CCT][p])		// module event rate = NumEvents/Runtime
	AllModRunStats[][7]=num2str(Module_Parameter_Values[index_CIR][p])		// module event rate = NumEvents/Runtime
	
	if(mode==0)
		// All list data are not editable
		AllModRunstats_S=0
		
		// Set up for background color
		AllModRunstats_S[][][1]=1
	
		labelStr1="Run Time [s]"
		labelStr2="Event Rate [cps]"
		labelStr3="DAQ Fraction [%]"
		labelStr4="Total Time [s]"
		labelStr5="Coinc Filter DT [s]"
		labelStr6="Coinc Count Time [s]"
		labelStr7="Coinc Input Rate [cps]"
		setdimlabel 1,0,Module,AllModRunStats
		setdimlabel 1,1,$labelStr1,AllModRunStats
		setdimlabel 1,2,$labelStr2,AllModRunStats
		setdimlabel 1,3,$labelStr3,AllModRunStats
		setdimlabel 1,4,$labelStr4,AllModRunStats
		setdimlabel 1,5,$labelStr5,AllModRunStats
		setdimlabel 1,6,$labelStr6,AllModRunStats
		setdimlabel 1,7,$labelStr7,AllModRunStats
		setdimlabel 2,1,backColors,AllModRunstats_S
	endif
	
	
End


//########################################################################
//	Pixie_MakeList_Chassis: Initialize Chassis List Data for Whole System
//########################################################################
Function Pixie_MakeList_Chassis(mode)
Variable mode	//0 - initialize when opening panel
				//1 - update values only

	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	
	if(mode==0)
		Make/o/t/n=(7,NumberOfModules+1) root:pixie4:ChassisSetup
		Make/o/b/n=(7,NumberOfModules+1,2) root:pixie4:ChassisSetup_S
		Make/o/t/n=(6,NumberOfModules+1) root:pixie4:CoincSetup
		Make/o/b/n=(6,NumberOfModules+1,2) root:pixie4:CoincSetup_S
	endif
		
	
	Wave/T ChassisSetup=root:pixie4:ChassisSetup
	Wave ChassisSetup_S=root:pixie4:ChassisSetup_S
	Wave/T CoincSetup=root:pixie4:CoincSetup
	Wave CoincSetup_S=root:pixie4:CoincSetup_S
		
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	
	Variable index0, index1, index2, index3,index4,runtime,k, m,MCSRA,MCSRB, MPat, index5, index6
	String labelStr1, value
	
	index0=Pixie_Find_ModuleParInWave("MODULE_CSRA")
	index1=Pixie_Find_ModuleParInWave("MODULEPATTERN")
	index2=Pixie_Find_ModuleParInWave("COINCIDENCE_PATTERN")
	index3=Pixie_Find_ModuleParInWave("ACTUAL_COINCIDENCE_WAIT")	
	index4=Pixie_Find_ModuleParInWave("NNSHAREPATTERN")	
	index5=Pixie_Find_ModuleParInWave("XET_DELAY")	
//	index6=Pixie_Find_ModuleParInWave("C_CONTROL")
	
	// first column are descriptions, rest are checkboxes
	if(mode==0)
		ChassisSetup_S[][][]=0	
		ChassisSetup_S[0][][]=0x02		//row 0 is editable text
		ChassisSetup_S[0][0][]=0
		ChassisSetup_S[5][][]=0x02		//row 5 is editable text
		ChassisSetup_S[5][0][]=0
		ChassisSetup_S[6][][]=0x02		//row 6 is editable text
		ChassisSetup_S[6][0][]=0
//		ChassisSetup_S[7][][]=0x02		//row 7 is editable text
//		ChassisSetup_S[7][0][]=0
		
		CoincSetup_S[][][]=0	
//		CoincSetup_S[1][][]=0x02	//row 1 is editable text
//		CoincSetup_S[1][0][]=0
//		CoincSetup_S[7][][]=0x02 //row 7 is editable text
//		CoincSetup_S[7][0][]=0
		
		ChassisSetup[][]=""
		CoincSetup[][]=""
		
		// Set up for background color
		ChassisSetup_S[][][1]=2	//all gray
		CoincSetup_S[][][1]=2		//all gray
		
		ChassisSetup_S[][0][1]=1	//1st column is purple
		CoincSetup_S[][0][1]=1	//1st column is purple
		CoincSetup_S[4][][1]=1	//row with coincpattern is purple
		CoincSetup_S[5][][1]=1	//row with coincpattern is purple
	endif
	
	
	
	for(k=1;k<=NumberOfModules;k+=1)
		MCSRA = Module_Parameter_Values[index0][k-1]
		MPat = Module_Parameter_Values[index1][k-1]
			//chassis list
			
			//bits2,15:  bit		module only    trad. wire-OR    NN slave   NN master
			//		     2			0				1			0			1
			//		   15			0				0			1			1	

			if(TSTBIT(2,MCSRA)>0)
				if(TSTBIT(15,MCSRA)>0)
					ChassisSetup[0][k]="3"
				else
					ChassisSetup[0][k]="1"
				endif
			else
				if(TSTBIT(15,MCSRA)>0)
					ChassisSetup[0][k]="2"
				else
					ChassisSetup[0][k]="0"
				endif
			endif
		
			
			if(TSTBIT(5,MCSRA)>0)
				ChassisSetup_S[1][k][0]=0x30
			else
				ChassisSetup_S[1][k][0]=0x20
			endif
			
	//		if(TSTBIT(11,MCSRA)>0)
	//			ChassisSetup_S[8][k][0]=0x30
	//		else
	//			ChassisSetup_S[8][k][0]=0x20
	//		endif
			
			if(TSTBIT(14,MCSRA)>0)	
				ChassisSetup_S[2][k][0]=0x30
			else
				ChassisSetup_S[2][k][0]=0x20
			endif
			
			if(TSTBIT(9,MCSRA)>0)	
				ChassisSetup_S[3][k][0]=0x30
			else
				ChassisSetup_S[3][k][0]=0x20
			endif
			
			if(TSTBIT(13,MCSRA)>0)	
				ChassisSetup_S[4][k][0]=0x30
			else
				ChassisSetup_S[4][k][0]=0x20
			endif
			
			sprintf value, "0x%04X", Module_Parameter_Values[index4][k-1]
			ChassisSetup[5][k]= value
			
			value = num2str(Module_Parameter_Values[index5][k-1])
			ChassisSetup[6][k]= value
			
	//		sprintf value, "0x%04X", Module_Parameter_Values[index6][k-1]
	//		ChassisSetup[7][k]= value
	
	
						
			
			//coinc list
					
			if(TSTBIT(5,MPat)>0)
				CoincSetup_S[0][k][0]=0x30
			else
				CoincSetup_S[0][k][0]=0x20
			endif
			
			if(TSTBIT(6,MCSRA)>0)
				CoincSetup_S[3][k][0]=0x30
			else
				CoincSetup_S[3][k][0]=0x20
			endif
			
			if(TSTBIT(7,MPat)>0)
				CoincSetup_S[1][k][0]=0x30
			else
				CoincSetup_S[1][k][0]=0x20
			endif
			
			if(TSTBIT(12,MCSRA)>0)
				CoincSetup_S[2][k][0]=0x30
			else
				CoincSetup_S[2][k][0]=0x20
			endif
				
			sprintf value, "0x%04X", Module_Parameter_Values[index2][k-1]
			CoincSetup[4][k]= value
			
			sprintf value, "%d", Module_Parameter_Values[index3][k-1]
			CoincSetup[5][k]= value
	endfor
	
	ChassisSetup_S[0][][]=0x02
	ChassisSetup_S[0][0][0]=0
	ChassisSetup_S[0][0][1]=1
	
	if(mode==0)
		
		ChassisSetup[0][0]="Trigger share mode"
		ChassisSetup[1][0]="Front panel drives Veto line (one module only)"
		ChassisSetup[2][0]="Front panel contributes to STATUS line (wire-OR)"
		ChassisSetup[3][0]="Module writes control pattern to PDM to immediate left"
		ChassisSetup[4][0]="Send local hit pattern to PDM in slot 2"
		ChassisSetup[5][0]="PDM control pattern"
		ChassisSetup[6][0]="Validation delay for external fast trigger (ns)"
	//	ChassisSetup[7][0]="C control options"
	//	ChassisSetup[8][0]="Bypass SDRAM"

		CoincSetup[0][0]="Accept event if local hit pattern passes local test"
		CoincSetup[1][0]="Accept event if global hit pattern passes global test (in PDM)"
		CoincSetup[2][0]="Module's local test adds to global test (local fail causes global fail)"
		CoincSetup[3][0]="Channel 3 Hit contributes to STATUS line (wire-OR)"
		CoincSetup[4][0]="Coincidence Pattern for local test (from Coinc. Tab)"
		CoincSetup[5][0]="Coincidence Window for both tests (from Coinc. Tab)"
		
			
		for(k=1;k<=NumberOfModules;k+=1)
			labelstr1 = " "+num2str(k-1)
			setdimlabel 1,k,$labelStr1,ChassisSetup
			setdimlabel 1,k,$labelStr1,CoincSetup
		endfor
		setdimlabel 1,0,Module,ChassisSetup
		setdimlabel 2,1,backColors,ChassisSetup_S
		setdimlabel 1,0,Module,CoincSetup
		setdimlabel 2,1,backColors,CoincSetup_S
	endif

		
		
End

//########################################################################
//	Pixie_ListProc_Chassis: set bits in ModCSRA and ModulePattern according to list box selections
//########################################################################
Function Pixie_ListProc_Chassis(ctrlName,row,col,event)
	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.
	
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar  NumberOfModules=root:pixie4:NumberOfModules
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave ChassisSetup_S=root:pixie4:ChassisSetup_S
	Wave CoincSetup_S=root:pixie4:CoincSetup_S
	Wave/T ChassisSetup=root:pixie4:ChassisSetup
	Wave/T CoincSetup=root:pixie4:CoincSetup
		
	Variable ratio, dx, ModNum, MCSRA, MPat, val, value, applyMPat
	String wav, entry
		
	if(col==0)
		return(0)
	endif
	applyMPat = 0
	ModNum = col-1
	MCSRA = Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULE_CSRA")][ModNum]
	MPat = Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULEPATTERN")][ModNum]
	
	if( event==1)   // Checkbox was selected/deselected
		
		if (cmpstr(CtrlName,"ChassisBox")==0)
			Switch(row)
				Case 0:	//connect to backplane triggers  - only at mouse up
				
					break
					
				Case 1: //Veto to backplane
					if( (ChassisSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(5, MCSRA)
					else
						MCSRA = CLRbit(5, MCSRA)
					endif
					break	
				
				Case 2: //Status to backplane
					if( (ChassisSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(14, MCSRA)
					else
						MCSRA = CLRbit(14, MCSRA)
					endif
					break		
					
				Case 3: //program PDM
					if( (ChassisSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(9, MCSRA)
					else
						MCSRA = CLRbit(9, MCSRA)
					endif		
					break		
				
				Case 4: //signal to PDM
					if( (ChassisSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(13, MCSRA)					// C library prevents setting this bit in slot 2, avoiding conflict with PCI CLK in
					else
						MCSRA = CLRbit(13, MCSRA)
					endif		
					break	
//					
//				Case 8: //SDRAM bypass
//					if( (ChassisSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
//						MCSRA = SETbit(11, MCSRA)					// C library prevents setting this bit in slot 2, avoiding conflict with PCI CLK in
//					else
//						MCSRA = CLRbit(11, MCSRA)
//					endif		
//					break	
							
				Default:
					break
			EndSwitch			
		endif
				
		if (cmpstr(CtrlName,"CoincBox")==0)	
			Switch(row)
										
				Case 0: //local accept
					if( (CoincSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MPat = SETbit(5, MPat)
					else
						MPat = CLRbit(5, MPat)
					endif	
					applyMPat = 1	
					break	
					
				Case 1: //global accept (token)
					if( (CoincSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MPat = SETbit(7, MPat)
					else
						MPat = CLRbit(7, MPat)
					endif		
					applyMPat = 1
					break		
					
				Case 2: //local to token
					if( (CoincSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(12, MCSRA)
					else
						MCSRA = CLRbit(12, MCSRA)
					endif		
					break		
					
								
				Case 3: //Hit3 to status
					if( (CoincSetup_S[row][col][0] & 0x10) == 0 )		//value is taken before checkbox is changed
						MCSRA = SETbit(6, MCSRA)
					else
						MCSRA = CLRbit(6, MCSRA)
					endif		
					break		
				
			
						
				Default:
					break
			EndSwitch
		endif	
		
		Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULE_CSRA")][ModNum]=MCSRA
		Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRA", "MODULE", 0, ModNum, 0)	
		
		if (applyMPat ==1)		
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULEPATTERN")][ModNum]=MPat
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULEPATTERN", "MODULE", 0, ModNum, 0)
		endif
				
	endif
	
	if(event==7)
	
		if (cmpstr(CtrlName,"ChassisBox")==0)	
			if(row==0) //trigger sharing - only at mouse up
				value  = str2num(ChassisSetup[row][col])	
				if( (value<=0) | (value>3) )		
					MCSRA = CLRbit(2, MCSRA)
					MCSRA = CLRbit(15, MCSRA)
					ChassisSetup[row][col]="0"
				endif	
				
				if( value== 1 )		
					MCSRA = SETbit(2, MCSRA)
					MCSRA = CLRbit(15, MCSRA)
				endif	
				
				if( value== 2 )		
					MCSRA = CLRbit(2, MCSRA)
					MCSRA = SETbit(15, MCSRA)
				endif	
				
				if( value== 3 )		
					MCSRA = SETbit(2, MCSRA)
					MCSRA = SETbit(15, MCSRA)
				endif	
				//print "MCSRA = ", MCSRA
				Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULE_CSRA")][ModNum]=MCSRA
				Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRA", "MODULE", 0, ModNum, 0)	
			endif
			
			if(row==5) //PDM control pattern
				sscanf ChassisSetup[row][col], "0x%x", val
				Module_Parameter_Values[Pixie_Find_ModuleParInWave("NNSHAREPATTERN")][col-1] = val
				Pixie4_User_Par_IO(Module_Parameter_Values, "NNSHAREPATTERN", "MODULE", 0, ModNum, 0)				
			endif	
			
			if(row==6) //External Trigger Delay
				val = str2num( ChassisSetup[row][col])
				Module_Parameter_Values[Pixie_Find_ModuleParInWave("XET_DELAY")][col-1] = val
				Pixie4_User_Par_IO(Module_Parameter_Values, "XET_DELAY", "MODULE", 0, ModNum, 0)				
			endif	
			
//			if(row==7) //C control pattern
//				sscanf ChassisSetup[row][col], "0x%x", val
//				Module_Parameter_Values[Pixie_Find_ModuleParInWave("C_CONTROL")][col-1] = val
//				Pixie4_User_Par_IO(Module_Parameter_Values, "C_CONTROL", "MODULE", 0, ModNum, 0)				
//			endif			
		
			
		endif
		
	endif
		
	//if(event==2)   // Mouse up	(follows event 4)
		//some changes affect all modules, so update 
		Pixie_IO_ReadModParFromMod(NumberOfModules)
		
		Pixie_MakeList_Chassis(1)	//update list
		
	//endif
	
	// deselect cell
	if (cmpstr(CtrlName,"CoincBox")==0)	
		CoincSetup_S[row][col][0] = CLRBIT(0,CoincSetup_S[row][col][0])
	endif
	if (cmpstr(CtrlName,"ChassisBox")==0)	
		ChassisSetup_S[row][col][0] = CLRBIT(0,ChassisSetup_S[row][col][0])
	endif
	return 0  // other return values reserved
end




//########################################################################
//	Pixie_MakeList_Traces: Initialize Channel Energy List Data
//########################################################################
Function Pixie_MakeList_Traces(init)
Variable init	//0 - first open graph
			//1 - update fit values - do not change checkboxes.

	String strLabel
	
	if (init==0)
		Make/o/t/n=(5,5) root:pixie4:ListModeEnergyListData
		Make/o/b/n=(5,5,2) root:pixie4:ListModeEnergySListData
	endif
	
	Wave/T ListModeEnergyListData=root:pixie4:ListModeEnergyListData
	Wave ListModeEnergySListData=root:pixie4:ListModeEnergySListData
	
	Wave ListModeChannelEnergy=root:pixie4:ListModeChannelEnergy
	Wave ListModeChannelTrigger=root:pixie4:ListModeChannelTrigger
	Wave ListModeChannelXIA=root:pixie4:ListModeChannelXIA
	Wave ListModeChannelUser=root:pixie4:ListModeChannelUser

	ListModeEnergyListData[][0]=num2istr(p)	
	ListModeEnergyListData[4][0]="Ref"
	ListModeEnergyListData[][1]=num2istr(ListModeChannelEnergy[p])	
	ListModeEnergyListData[][2]=num2istr(ListModeChannelTrigger[p])	
	ListModeEnergyListData[][3]=num2istr(ListModeChannelXIA[p])	
	ListModeEnergyListData[][4]=num2istr(ListModeChannelUser[p])	
	
	if (init==0)	
		// Most list data fields are not editable except channel numbers are checkboxes
		ListModeEnergySListData=0
		ListModeEnergySListData[][0][]=0x30
		ListModeEnergySListData[4][0][]=0x20	// unchecked
			
		// Set up color
		ListModeEnergySListData[][][1]=1	//default non-editable: purple
		ListModeEnergySListData[0][0][1]=3	//ch.0: red
		ListModeEnergySListData[1][0][1]=4	//ch.1: bright green
		ListModeEnergySListData[2][0][1]=5	//ch.2: blue
		ListModeEnergySListData[3][0][1]=6	//ch.3: dark green
		ListModeEnergySListData[4][0][1]=7	//ref: black
			
		setdimlabel 1,0,Channel,ListModeEnergyListData
		strLabel="Energy (16bit)"
		setdimlabel 1,1,$strLabel,ListModeEnergyListData
		setdimlabel 1,2,TimeStamp,ListModeEnergyListData
		setdimlabel 1,3,XIA_PSA,ListModeEnergyListData
		setdimlabel 1,4,User_PSA,ListModeEnergyListData
		setdimlabel 2,1,backColors,ListModeEnergySListData
	endif
	
	
End

//########################################################################
//	Pixie_ListProc_Traces: Select List Mode Traces in Trace Display
//########################################################################
Function Pixie_ListProc_Traces(ctrlName,row,col,event)
	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.

	
	Wave/T ListModeEnergyListData=root:pixie4:ListModeEnergyListData
	Wave ListModeEnergySListData=root:pixie4:ListModeEnergySListData
	String wav
		
	if((event==2) && (col==0))  // Channel was selected/deselected
		wav="Trace"+num2str(row)
		
		if((ListModeEnergySListData[row][col] & 0x10) ==0x10 )
			AppendToGraph $("root:pixie4:"+wav)
			do
				if(row==1)
					ModifyGraph rgb($wav)=(0,65280,0)
				endif
				if(row==2)
					ModifyGraph rgb($wav)=(0,15872,65280)
				endif
				if(row==3)
					ModifyGraph rgb($wav)=(0,26112,0)
				endif
				if(row==4)
					ModifyGraph rgb($wav)=(0,0,0)
				endif
			while(0)
			ModifyGraph mode=6
			ModifyGraph grid=1
			ModifyGraph mirror(bottom)=2
			ModifyGraph mirror(left)=2
		else
			RemoveFromGraph/Z $(wav)
			wav="fit_"+wav
			RemoveFromGraph/Z $(wav)
		endif
	endif

	return 0			// other return values reserved
end






//########################################################################
//	Pixie_MakeList_MCA: Initialize MCA Spectrum List Data
//########################################################################
Function Pixie_MakeList_MCA(init)
	Variable init	//0 - first open graph
				//1 - update fit values - do not change checkboxes.

	String strLabel
	
	if(init==0)
		make/o/t/n=(10,9) root:pixie4:MCASpecListData
		make/o/b/n=(10,9,2) root:pixie4:MCASpecSListData
	endif
	
	Wave/T MCASpecListData=root:pixie4:MCASpecListData
	Wave MCASpecSListData=root:pixie4:MCASpecSListData
	
	Wave MCAStartFitChannel=root:pixie4:MCAStartFitChannel
	Wave MCAEndFitChannel=root:pixie4:MCAEndFitChannel
	Wave MCAFitRange=root:pixie4:MCAFitRange
	Wave MCAscale=root:pixie4:MCAscale
	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
	Wave MCAChannelFWHMAbsolute=root:pixie4:MCAChannelFWHMAbsolute
	Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea				
	
	
	MCASpecListData[][0]=num2str(p)
	MCASpecListData[4][0]="Ref"
	MCASpecListData[5][0]="Addback"
	MCASpecListData[6][0]="Cum0"
	MCASpecListData[7][0]="Cum1"
	MCASpecListData[8][0]="Cum2"
	MCASpecListData[9][0]="Cum3"
	MCASpecListData[][1]=num2str(MCAFitRange[p])
	MCASpecListData[][2]=num2str(MCAStartFitChannel[p])
	MCASpecListData[][3]=num2str(MCAEndFitChannel[p])
	MCASpecListData[][4]=num2str(MCAscale[p])
	MCASpecListData[][5]=num2str(MCAChannelPeakPos[p])
	MCASpecListData[][6]=num2str(MCAChannelFWHMPercent[p])
	MCASpecListData[][7]=num2str(MCAChannelFWHMAbsolute[p])
	MCASpecListData[][8]=num2str(MCAChannelPeakArea[p])
		
	if (init==0)
		// Most list data are not editable except StartFitChannel, EndFitChannel, FitRange, and Scale
		// channel number are checkboxes
		MCASpecSListData=0
		MCASpecSListData[][0][]=0x30
		MCASpecSListData[4][0][]=0x20
		MCASpecSListData[5][0][]=0x20
		MCASpecSListData[6][0][]=0x20
		MCASpecSListData[7][0][]=0x20
		MCASpecSListData[8][0][]=0x20
		MCASpecSListData[9][0][]=0x20
		MCASpecSListData[][1][]=2
		MCASpecSListData[][2][]=2
		MCASpecSListData[][3][]=2
		MCASpecSListData[][4][]=2
		
		// Set up color
		MCASpecSListData[][][1]=1	//default non-editable: purple
		MCASpecSListData[][1,4][1]=2
		MCASpecSListData[0][0][1]=3	//ch.0: red
		MCASpecSListData[1][0][1]=4	//ch.1: bright green
		MCASpecSListData[2][0][1]=5	//ch.2: blue
		MCASpecSListData[3][0][1]=6	//ch.3: dark green
		MCASpecSListData[4][0][1]=7	//other: black
		MCASpecSListData[5][0][1]=8	//other: purple
		MCASpecSListData[6][0][1]=3	//ch.0: red
		MCASpecSListData[7][0][1]=4	//ch.1: bright green
		MCASpecSListData[8][0][1]=5	//ch.2: blue
		MCASpecSListData[9][0][1]=6	//ch.3: dark green
			
						
		setdimlabel 1,0,Channel,MCASpecListData
		
		strLabel="Range [%]"
		setdimlabel 1,1,$strLabel,MCASpecListData	
		
		setdimlabel 1,2,Min,MCASpecListData
		setdimlabel 1,3,Max,MCASpecListData
		
		strLabel="keV/bin"
		setdimlabel 1,4,$strLabel,MCASpecListData
		
		setdimlabel 1,5,Peak,MCASpecListData
		
		strLabel="FWHM [%]"
		setdimlabel 1,6,$strLabel,MCASpecListData
	
		strLabel="FWHM [abs]"
		setdimlabel 1,7,$strLabel,MCASpecListData
				
		strLabel="Peak Area"
		setdimlabel 1,8,$strLabel,MCASpecListData
			
		setdimlabel 2,1,backColors,MCASpecSListData
	endif
End


//########################################################################
//	Pixie_ListProc_MCA: Track and update MCA Spectrum List Data
//########################################################################
Function Pixie_ListProc_MCA(ctrlName,row,col,event)
	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.
	
	Wave MCAStartFitChannel=root:pixie4:MCAStartFitChannel
	Wave MCAEndFitChannel=root:pixie4:MCAEndFitChannel
	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
	Wave MCAFitRange=root:pixie4:MCAFitRange
	Wave MCAscale=root:pixie4:MCAscale
	Wave MCAChannelPeakEnergy=root:pixie4:MCAChannelPeakEnergy
	Wave MCAChannelFWHMAbsolute=root:pixie4:MCAChannelFWHMAbsolute
	Wave/T MCASpecListData=root:pixie4:MCASpecListData
	Wave MCASpecSListData=root:pixie4:MCASpecSListData
	Nvar  NumberOfModules=root:pixie4:NumberOfModules
	Variable ratio, dx
	String wav, fitwav
	
	if(row==5)
		wav="MCAsum"
	else
		if(row<5)
			wav="MCAch"+num2str(row)
		else
			wav="MCAtotal"+num2str(row-6)
		endif
	endif
	fitwav = "fit_"+wav
		
	if((event==7) && (col==1))  // Fit Range was changed
		MCAFitRange[row]=str2num(MCASpecListData[row][col])
	endif	
		
	if((event==7) && (col==2))  // StartFitChannel was changed
		MCAStartFitChannel[row]=str2num(MCASpecListData[row][col])
	endif
	
	if((event==7) && (col==3))  // EndFitChannel was changed
		MCAEndFitChannel[row]=str2num(MCASpecListData[row][col])
	endif
	
	if((event==7) && (col==4))  // kev/bin was changed
	
		ratio = str2num(MCASpecListData[row][col])/MCAscale[row]		// new/old
		
		Wave w=$("root:pixie4:"+wav)
		dx=str2num(MCASpecListData[row][col])
		SetScale/P x,0,dx," ",w
		
		MCAscale[row]*=ratio
		MCAStartFitChannel[row]*=ratio
		MCAEndFitChannel[row]*=ratio
		MCAChannelFWHMAbsolute[row]*=ratio
		MCAChannelPeakPos[row]*=ratio
		
		// Update display
		MCASpecListData[row][2]=num2str(MCAStartFitChannel[row])
		MCASpecListData[row][3]=num2str(MCAEndFitChannel[row])
		MCASpecListData[row][5]=num2str(MCAChannelPeakPos[row])
		MCASpecListData[row][7]=num2str(MCAChannelFWHMAbsolute[row])
		
		RemoveFromGraph/z $fitwav
	endif
	
	if((event==2) && (col==0))  // Channel was selected/deselected
		Silent 1

		if((MCASpecSListData[row][col] & 0x10) ==0x10 )
			AppendToGraph $("root:pixie4:"+wav)
			do
				if(row==1)
					ModifyGraph rgb($wav)=(0,65280,0)
				endif
				if(row==2)
					ModifyGraph rgb($wav)=(0,15872,65280)
				endif
				if(row==3)
					ModifyGraph rgb($wav)=(0,26112,0)
				endif
				if(row==4)	//ref
					ModifyGraph rgb($wav)=(0,0,0)
				endif
				if(row==5)	//sum
					ModifyGraph rgb($wav)=(36864,14592,58880)
				endif
				if(row==7)
					ModifyGraph rgb($wav)=(0,65280,0)
				endif
				if(row==8)
					ModifyGraph rgb($wav)=(0,15872,65280)
				endif
				if(row==9)
					ModifyGraph rgb($wav)=(0,26112,0)
				endif
				if(row>5)
					ModifyGraph lsize($wav)=2
				endif
			while(0)
			ModifyGraph mode=6
			ModifyGraph grid=1
			ModifyGraph mirror(bottom)=2
			ModifyGraph mirror(left)=2
		else
			RemoveFromGraph/Z $(wav)
			wav="fit_"+wav
			RemoveFromGraph/Z $(wav)
		endif
	endif

	return 0  // other return values reserved
end


//########################################################################
//	Pixie_MakeList_LMHisto: Initialize List Mode Spectrum List Data
//########################################################################
Function Pixie_MakeList_LMHisto()

	String strLabel
	
	make/o/t/n=(5,6) root:pixie4:ListModeSpecListData
	make/o/b/n=(5,6,2) root:pixie4:ListModeSpecSListData
	
	Wave/T ListModeSpecListData=root:pixie4:ListModeSpecListData
	Wave ListModeSpecSListData=root:pixie4:ListModeSpecSListData
	
	Wave ListStartFitChannel=root:pixie4:ListStartFitChannel
	Wave ListEndFitChannel=root:pixie4:ListEndFitChannel
	Wave ListChannelPeakPos=root:pixie4:ListChannelPeakPos
	Wave ListChannelFWHMPercent=root:pixie4:ListChannelFWHMPercent
	Wave ListChannelPeakArea=root:pixie4:ListChannelPeakArea				
	
	ListModeSpecListData[][0]=num2str(p)
	ListModeSpecListData[4][0]="Ref"
	ListModeSpecListData[][1]=num2str(ListStartFitChannel[p])
	ListModeSpecListData[][2]=num2str(ListEndFitChannel[p])
	ListModeSpecListData[][3]=num2str(ListChannelPeakPos[p])
	ListModeSpecListData[][4]=num2str(ListChannelFWHMPercent[p])
	ListModeSpecListData[][5]=num2str(ListChannelPeakArea[p])

	// Most list data are not editable except StartFitChannel and EndFitChannel
	ListModeSpecSListData=0
	ListModeSpecSListData[][0][]=0x30
	ListModeSpecSListData[4][0][]=0x20
	ListModeSpecSListData[][1][]=2
	ListModeSpecSListData[][2][]=2
	
	// Set up color
	ListModeSpecSListData[][][1]=1	//default non-editable: purple
	ListModeSpecSListData[][1,3][1]=2
	ListModeSpecSListData[0][0][1]=3	//ch.0: red
	ListModeSpecSListData[1][0][1]=4	//ch.1: bright green
	ListModeSpecSListData[2][0][1]=5	//ch.2: blue
	ListModeSpecSListData[3][0][1]=6	//ch.3: dark green
	ListModeSpecSListData[4][0][1]=7	//ref: dark gray


	setdimlabel 1,0,Channel,ListModeSpecListData
	setdimlabel 1,1,Min,ListModeSpecListData
	setdimlabel 1,2,Max,ListModeSpecListData
	setdimlabel 1,3,Peak,ListModeSpecListData
	
	strLabel="FWHM [%]"
	setdimlabel 1,4,$strLabel,ListModeSpecListData

	strLabel="Peak Area"
	setdimlabel 1,5,$strLabel,ListModeSpecListData
		
	setdimlabel 2,1,backColors,ListModeSpecSListData

End









//########################################################################
//	Pixie_ListProc_LMHisto: Track and update List Mode Spectrum List Data
//########################################################################
Function Pixie_ListProc_LMHisto(ctrlName,row,col,event)
	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.

	Wave ListStartFitChannel=root:pixie4:ListStartFitChannel
	Wave ListEndFitChannel=root:pixie4:ListEndFitChannel
	Wave ListChannelPeakPos=root:pixie4:ListChannelPeakPos

	Wave/T ListModeSpecListData=root:pixie4:ListModeSpecListData
	Wave ListModeSpecSListData=root:pixie4:ListModeSpecSListData
	String wav
	
	if((event==7) && (col==1))  // StartFitChannel was changed
		ListStartFitChannel[row]=str2num(ListModeSpecListData[row][col])
		DoWindow ListModeSpectrumDisplay
		if (V_flag!=0)
			wav = "Spectrum"+num2str(row)
			cursor/W=ListModeSpectrumDisplay A $wav ListStartFitChannel[row]		
		endif		
	endif
	
	if((event==7) && (col==2))  // EndFitChannel was changed
		ListEndFitChannel[row]=str2num(ListModeSpecListData[row][col])
		DoWindow ListModeSpectrumDisplay
		if (V_flag!=0)
			wav = "Spectrum"+num2str(row)
			cursor/W=ListModeSpectrumDisplay B $wav ListEndFitChannel[row]
		endif		
	endif
	
	if((event==7) && (col==3))  // ChannelPeakPos was changed
		ListChannelPeakPos[row]=str2num(ListModeSpecListData[row][col])
	endif
	
	if((event==2) && (col==0))  // Channel was selected/deselected
		Silent 1
		wav="Spectrum"+num2str(row)
	//	if (row ==4) //sum
	//		wav="MCAsum"
	//	endif	
		if((ListModeSpecSListData[row][col] & 0x10) ==0x10 )
			AppendToGraph $("root:pixie4:"+wav)
			do
				if(row==1)
					ModifyGraph rgb($wav)=(0,65280,0)
				endif
				if(row==2)
					ModifyGraph rgb($wav)=(0,15872,65280)
				endif
				if(row==3)
					ModifyGraph rgb($wav)=(0,26112,0)
				endif
				if(row==4)	//ref
					ModifyGraph rgb($wav)=(0,0,0)
				endif
		//		if(row==4)	//sum
		//			ModifyGraph rgb($wav)=(0,0,0)
		//		endif
			while(0)
			ModifyGraph mode=6
			ModifyGraph grid=1
			ModifyGraph mirror(bottom)=2
			ModifyGraph mirror(left)=2
		else
			RemoveFromGraph/Z $(wav)
			wav="fit_"+wav
			RemoveFromGraph/Z $(wav)
		endif
	endif

	return 0			// other return values reserved
end


//########################################################################
//	Pixie_MakeList_CopyItems: Create copy items list
//########################################################################
Function Pixie_MakeList_CopyItems()

	Nvar CopySettingsBitMask = root:pixie4:CopySettingsBitMask
	Wave/T CopyItemsList = root:pixie4:CopyItemsList
	Wave CopyItemsListData = root:pixie4:CopyItemsListData
	Wave CopyItemsColorWave = root:pixie4:CopyItemsColorWave

	CopySettingsBitMask = 0x1FFF
	CopyItemsListData=0x30
	// Initialize the color wave
	CopyItemsColorWave[][0]=51456
	CopyItemsColorWave[][1]=44032
	CopyItemsColorWave[][2]=58880
End


//########################################################################
//	Pixie_ListProc_CopyItems: Set bits mask
//########################################################################
Function Pixie_ListProc_CopyItems(ctrlName,row,col,event)

	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.
	
	Wave CopyItemsListData = root:pixie4:CopyItemsListData
	Nvar CopySettingsBitMask = root:pixie4:CopySettingsBitMask
		
	if((CopyItemsListData[row] == 48) || (CopyItemsListData[row] == 49))
		CopySettingsBitMask = SETbit(row, CopySettingsBitMask)
	else
		CopySettingsBitMask =CLRbit(row, CopySettingsBitMask)
	endif
end


//########################################################################
//	Pixie_MakeList_CopySettings: Create source and destination modules and channels
//########################################################################
Function Pixie_MakeList_CopySettings()

	Nvar NumberOfModules = root:pixie4:NumberOfModules

	make/o/t/n=(NumberOfModules,5) root:pixie4:CopySettingsModChan
	make/o/n=(NumberOfModules,5,2) root:pixie4:CopySettingsModChanData
	Make/o/u/w/n=(NumberOfModules+3) root:pixie4:CopySettingsDataWave
	
	Wave/T CopySettingsModChan=root:pixie4:CopySettingsModChan
	Wave CopySettingsModChanData=root:pixie4:CopySettingsModChanData
	Wave CopyItemsColorWave = root:pixie4:CopyItemsColorWave
	Wave ModuleSerialNumber = root:pixie4:ModuleSerialNumber

	CopySettingsModChan = ""
	CopySettingsModChan[][0]=num2str(ModuleSerialNumber[p])

	CopySettingsModChanData = CopySettingsModChanData %| 0x20
	
	// Set up for background color
	CopySettingsModChanData[][][1]=1

	// Initialize the color wave
	CopyItemsColorWave[][0]=48896
	CopyItemsColorWave[][1]=52992
	CopyItemsColorWave[][2]=65280

	setdimlabel 1,0,Module,CopySettingsModChan
	setdimlabel 1,1,Ch0,CopySettingsModChan
	setdimlabel 1,2,Ch1,CopySettingsModChan
	setdimlabel 1,3,Ch2,CopySettingsModChan
	setdimlabel 1,4,Ch3,CopySettingsModChan	

End


//########################################################################
//	Pixie_ListProc_Copy: Copy settings
//########################################################################
Function Pixie_ListProc_Copy(ctrlName,row,col,event)

	String ctrlName		// name of this control
	Variable row			// row if click in interior, -1 if click in title
	Variable col			// column
	Variable event		// event code: 6 is begin edit, 7 is finish edit.
	Variable i
	
	Wave CopySettingsDataWave = root:pixie4:CopySettingsDataWave
	Wave CopySettingsDataWave=root:pixie4:CopySettingsDataWave
	Wave CopySettingsModChanData=root:pixie4:CopySettingsModChanData
	
	if(event == 2) // only act on Mouse Up event
		if(col == 0) // Module column - all channels are selected automatically
			if((CopySettingsModChanData[row][0][0] == 48) || (CopySettingsModChanData[row][0][0] == 49))
				CopySettingsDataWave[row+3] = 0xF
				for(i = 1; i < 5; i += 1 )
					CopySettingsModChanData[row][i][0] =SETbit(4, CopySettingsModChanData[row][i][0])
				endfor
				Pixie_MakeList_CopySettings()
			else
				CopySettingsDataWave[row+3] = 0x0
				for(i = 1; i < 5; i += 1)
					CopySettingsModChanData[row][i][0] = CLRbit(4, CopySettingsModChanData[row][i][0])
				endfor
				Pixie_MakeList_CopySettings()
			endif			
		else
			if((CopySettingsModChanData[row][col][0] == 48) || (CopySettingsModChanData[row][col][0] == 49))
				CopySettingsDataWave[row+3] = SETbit((col-1), CopySettingsDataWave[row+3])
			else
				CopySettingsDataWave[row+3] = CLRbit((col-1), CopySettingsDataWave[row+3])
			endif
		endif
	endif
end




//########################################################################
//	Pixie_File_Dialog: Boot files dialog for "Find" button
//########################################################################
Function Pixie_File_Dialog(ctrlName) :ButtonControl
String ctrlName

	Svar FPGAFile4e = root:pixie4:FPGAFile4e
	Svar CommFPGAFile4 = root:pixie4:CommFPGAFile4
	Svar CommFPGAFile500 = root:pixie4:CommFPGAFile500
	Svar SPFPGAFile4 = root:pixie4:SPFPGAFile4
	Svar SPFPGAFile500 = root:pixie4:SPFPGAFile500
	Svar FPGAFile500e = root:pixie4:FPGAFile500e
	Svar DSPCodeFile4 = root:pixie4:DSPCodeFile4
	Svar DSPCodeFile500 = root:pixie4:DSPCodeFile500
	Svar DSPCodeFile500e = root:pixie4:DSPCodeFile500e
	Svar DSPVarFile4 = root:pixie4:DSPVarFile4
	Svar DSPListFile4 = root:pixie4:DSPListFile4
	Svar DSPListFile500e = root:pixie4:DSPListFile500e
	Svar ExtractSettingsFile = root:pixie4:ExtractSettingsFile
	
	Svar DSPParaValueFile = root:pixie4:DSPParaValueFile
	
	Svar MCAFileDirectory = root:pixie4:MCAFileDirectory
	Svar EventFileDirectory = root:pixie4:EventFileDirectory
	
	Variable filenum
	
	if(cmpstr(ctrlName,"SetFPGAFile4e")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			FPGAFile4e = S_FileName
		endif
		Return(0)
	endif

	if(cmpstr(ctrlName,"SetCommFPGAFile4")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			CommFPGAFile4 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetCommFPGAFile500")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			CommFPGAFile500 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetSPFPGAFile4")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			SPFPGAFile4 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetSPFPGAFile500")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			SPFPGAFile500 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetFPGAFile500e")==0)
		Open/R/D/P=FirmwarePath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			FPGAFile500e = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPCodeFile4")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPCodeFile4 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPCodeFile500")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPCodeFile500 = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPCodeFile500e")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPCodeFile500e = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPVarFile4")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPVarFile4 = S_FileName
			Pixie_IO_GetDSPNames()
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPListFile4")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPListFile4 = S_FileName
			Pixie_IO_GetMemoryNames()			
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SetDSPListFile500e")==0)
		Open/R/D/P=DSPPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			DSPListFile500e = S_FileName
			Pixie_IO_GetMemoryNames()			
		endif
		Return(0)
	endif

	if(cmpstr(ctrlName,"SetDSPParaValueFile")==0)
		Open/R/D/P=ConfigurationPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)		
			DSPParaValueFile = S_FileName
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"FindSettingsFile")==0)
		Open/R/D/P=ConfigurationPath/T="????" filenum
		if(cmpstr(S_FileName,"") != 0)
			ExtractSettingsFile = S_FileName
		endif
		Return(0)
	endif	
	
	if(cmpstr(ctrlName,"SetMCASpecDirectory")==0)
		NewPath/O MCAPath 
		PathInfo	MCAPath		
		If(cmpstr(S_Path,"") !=0 )  // Test if the new directory is valid
			MCAFileDirectory = S_Path
		else
			DoAlert 0, "Invalid directory."
		endif
		Return(0)
	endif	
	
	if(cmpstr(ctrlName,"SetPulseShapeDirectory")==0)
		NewPath/O EventPath 
		PathInfo	EventPath		
		If(cmpstr(S_Path,"") !=0 )  // Test if the new directory is valid
			EventFileDirectory = S_Path
		else
			DoAlert 0, "Invalid directory."
		endif
		Return(0)
	endif		

End


//########################################################################
//	Pixie_File_PathDialog: Boot file directories dialog for filename string
//########################################################################
Function Pixie_File_PathDialog(ctrlName,varNum,varStr,varName) : SetVariableControl
String ctrlName	// if file, identical to file name variable; if not, ends with "directory"
Variable varNum
String varStr
String varName

	Svar FPGAFile4e = root:pixie4:FPGAFile4e
	Svar CommFPGAFile4 = root:pixie4:CommFPGAFile4
	Svar CommFPGAFile500 = root:pixie4:CommFPGAFile500
	Svar SPFPGAFile4 = root:pixie4:SPFPGAFile4
	Svar SPFPGAFile500 = root:pixie4:SPFPGAFile500
	Svar FPGAFile500e = root:pixie4:FPGAFile500e
	Svar DSPCodeFile4 = root:pixie4:DSPCodeFile4
	Svar DSPCodeFile500 = root:pixie4:DSPCodeFile500
	Svar DSPCodeFile500e = root:pixie4:DSPCodeFile500e
	Svar DSPVarFile4 = root:pixie4:DSPVarFile4
	Svar DSPListFile4 = root:pixie4:DSPListFile4
	Svar DSPListFile500e = root:pixie4:DSPListFile500e
	
	
	Svar ExtractSettingsFile = root:pixie4:ExtractSettingsFile
//	Svar FirmwareFileDirectory = root:pixie4:FirmwareFileDirectory
	Svar MCAFileDirectory = root:pixie4:MCAFileDirectory
	Svar EventFileDirectory = root:pixie4:EventFileDirectory
	
	Variable filenum, len
	String ctrlend
	len = strlen(ctrlName)
	ctrlend = ctrlName[len-9,len-1]
	
	if(cmpstr(ctrlend,"Directory")!=0)
		Svar fnam = $("root:pixie4:"+ctrlName)
		Open/R filenum as fnam		// open will complain if file does not exist
		if(filenum != 0)
			close filenum
		endif
		Return(0)
	endif
	
	if(cmpstr(ctrlName,"SelectSettingsFile")==0)
		Open/R filenum as ExtractSettingsFile
		if(filenum != 0)
			close filenum
		endif
		Return(0)
	endif	

	if(cmpstr(ctrlName,"MCASpecDirectory")==0)
		NewPath/Z/O MCAPath MCAFileDirectory
		PathInfo	MCAPath
		If(cmpstr(S_Path, MCAFileDirectory) !=0 )  // Test if the new directory is valid
			DoAlert 0, "The directory you just input doesn't exist."
		endif
		Return(0)
	endif
		
	if(cmpstr(ctrlName,"PulseShapeDirectory")==0)
		NewPath/Z/O EventPath EventFileDirectory
		PathInfo	EventPath
		If(cmpstr(S_Path, EventFileDirectory) !=0 )  // Test if the new directory is valid
			DoAlert 0, "The directory you just input doesn't exist."
		endif
		Return(0)
	endif		

//	if(cmpstr(ctrlName,"FirmwareDirectory")==0)
//		NewPath/Z/O FirmwarePath FirmwareFileDirectory
//		PathInfo	FirmwarePath
//		If(cmpstr(S_Path, FirmwareFileDirectory) !=0 )  // Test if the new directory is valid
//			DoAlert 0, "The directory you just input doesn't exist."
//		endif
//		Return(0)
//	endif
	
//	if(cmpstr(ctrlName,"FPGAFile4e")==0)
//		Open/R filenum as FPGAFile4e
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif
	
//	if(cmpstr(ctrlName,"CommFPGAFile4")==0)
//		Open/R filenum as CommFPGAFile4
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif
//	
//	if(cmpstr(ctrlName,"SPFPGAFile")==0)
//		Open/R filenum as SPFPGAFile
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif
//	
//	if(cmpstr(ctrlName,"DSPCodeFile")==0)
//		Open/R filenum as DSPCodeFile
//		if(filenum == 0)
//			DoAlert 0, "This file doesn't exist."
//		else
//			close filenum
//		endif
//		Return(0)
//	endif
//
//	if(cmpstr(ctrlName,"DSPVarFile")==0)
//		Open/R filenum as DSPVarFile
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif
//	
//	if(cmpstr(ctrlName,"DSPListFile")==0)
//		Open/R filenum as DSPListFile
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif	
//
//	if(cmpstr(ctrlName,"DSPParaValueFile")==0)
//		Open/R filenum as DSPParaValueFile
//		if(filenum != 0)
//			close filenum
//		endif
//		Return(0)
//	endif
	

	
End


//########################################################################
//	Pixie_FileNameConversion: Convert file name from Igor format to Windows/DOS format
//########################################################################
Function/S Pixie_FileNameConversion(FileName)
String FileName
	
	Variable i, searchStr

	searchStr=strsearch(FileName, ":", 0)
	FileName[searchStr+1]="\\"
	do 
		searchStr=strsearch(FileName, ":", searchStr+1)
		if(searchStr==-1)
			break
		endif
		FileName[searchStr,searchStr]="\\"
	while(searchStr!=-1)
	
	return(FileName)

End


//########################################################################
//	Pixie_File_Settings: Save or load Pixie settings
//########################################################################
Function Pixie_File_Settings(ControlName) 
String ControlName

	Wave ModuleSerialNumber = root:pixie4:ModuleSerialNumber
	Wave ModuleSerialNumber_UINT16 = root:pixie4:ModuleSerialNumber_UINT16
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar DSPParaValueFile = root:pixie4:DSPParaValueFile
	
	String filename,filepath
	Variable filenum, strlenfile, i, searchStr, LastColonPos, type, direction, modnum
	
	// Prompt to open a file
	if(  (cmpstr(ControlName, "cmnSave") == 0) )
		Open/D/P=ConfigurationPath/T="????" filenum as "*.set"
	endif
	if(  (cmpstr(ControlName, "cmnLoad") == 0) )
		Open/R/D/P=ConfigurationPath/T="????" filenum
	endif
		
	filename = s_filename
	if (cmpstr(filename,"")!=0)	// if file opened successfully
		DSPParaValueFile=filename
		strlenfile=strlen(DSPParaValueFile)
		searchStr=strsearch(DSPParaValueFile, ":", 0)
		do 
			searchStr=strsearch(DSPParaValueFile, ":", searchStr+1)
			if(searchStr==-1)
				break
			endif
			LastColonPos = searchStr
		while(searchStr!=-1)
		
		filepath=DSPParaValueFile[0,LastColonPos]
		NewPath/O/Q/Z ConfigurationPath, filepath
		filename=Pixie_FileNameConversion(DSPParaValueFile)
		
		type=2	// Settings I/O
		modnum=0	// module number is not important here
		if(  (cmpstr(ControlName, "cmnSave") == 0) )
			direction=0	// Save settings
			Pixie4_Buffer_IO(ModuleSerialNumber_UINT16, type, direction, filename, modnum)
		elseif( (cmpstr(ControlName, "cmnLoad") == 0) )
			direction=1	// Load settings
			Pixie4_Buffer_IO(ModuleSerialNumber_UINT16, type, direction, filename, modnum)

			// Update module parameters
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			// Pixie_IO_SendModParToIgor() not required, part of Pixie_IO_SelectModChan
			
			// Update channel parameters
			Pixie_IO_ReadChanParFromMod(NumberOfModules)
			// Pixie_IO_SendChanParToIgor() not required, part of Pixie_IO_SelectModChan

			// Update Igor controls
			Pixie_IO_SelectModChan("LoadSettings", 0, "", "")
		endif		
	endif
End



//########################################################################
//
//	Pixie_File_RunStats:
//		Store/Read run statistics
//
//########################################################################

Function Pixie_File_RunStats(ctrlName,popNum,popStr) : PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr
	
	
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
//	Nvar CloverAdd = root:pixie4:CloverAdd

	Nvar NumModulePar = root:pixie4:NumModulePar
	Nvar NumChannelPar = root:pixie4:NumChannelPar
	Wave/T Channel_Parameter_Names = root:pixie4:Channel_Parameter_Names
	Wave/T Module_Parameter_Names = root:pixie4:Module_Parameter_Names
	Wave CPV = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave/T System_Parameter_Names = root:pixie4:System_Parameter_Names
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	
	Nvar NumChannelParIFM = root:pixie4:NumChannelParIFM 		//  number of input user variables (applicable to each channel) for .ifm file
	Nvar NumModuleInParIFM = root:pixie4:NumModuleInParIFM 		//  number of input global variables (applicable to each module) for .ifm file
	Nvar NumSystemInParIFM = root:pixie4:NumSystemInParIFM 		//  number of input global variables (applicable to system) for .ifm file


	Svar StartTime = root:pixie4:StartTime
	Svar StopTime = root:pixie4:StopTime
	Svar InfoSource = root:pixie4:InfoSource
	Svar StatisticsFileName = root:pixie4:StatisticsFileName
	Svar OutBaseName = root:pixie4:OutBaseName
	
	Variable filenum,i,k,index0, index1, index2, index3,len,m
	String filename,wav, line, text
	Variable Mnum, Chnum, RT, LT, ER, ICR, NumberMod, TT, OCR, GCR, SFDT, GDT, PPR
	Variable NCT, CSFDT, CCT, CIR
	
	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
	Nvar index_ICR = root:pixie4:index_ICR
	Nvar index_OCR = root:pixie4:index_OCR
	Nvar index_GCR = root:pixie4:index_GCR
	Nvar index_SFDT = root:pixie4:index_SFDT
	Nvar index_GDT = root:pixie4:index_GDT
	Nvar index_FTDT = root:pixie4:index_FTDT
	Nvar index_PPR = root:pixie4:index_PPR

		
	Nvar index_RunTime = root:pixie4:index_RunTime
	Nvar index_TotTime = root:pixie4:index_TotTime
	Nvar index_EvRate = root:pixie4:index_EvRate
	Nvar index_NumEv = root:pixie4:index_NumEv
	
	Nvar index_NCT = root:pixie4:index_NCT
	Nvar index_CSFDT = root:pixie4:index_CSFDT
	Nvar index_CCT = root:pixie4:index_CCT
	Nvar index_CIR = root:pixie4:index_CIR
	
	Nvar  ViewerVersion =  root:pixie4:ViewerVersion
		
	if(popnum == 1) // save as .ifm file	
 			
		if (cmpstr(ctrlName,"StopRun")==0)
			Open filenum as StatisticsFileName
			filename=S_fileName	// full path and name of file opened, or "" if cancelled
		else
			Open/D/T="????"/M="Save Run Statistics in Text File (.ifm)"/P=MCAPath filenum as "*.ifm"
			filename=S_fileName	// full path and name of file opened, or "" if cancelled
		endif
		
		if (cmpstr(filename,"")!=0)		// if file defined succesfully
			Open/Z/T="????" filenum as filename
			
			len = strlen(filename)
			text = filename[0,len-4]
			fprintf filenum, "XIA Pixie DAQ run saved to files %sxxx \r", text
			fprintf filenum, "Acquisition started at %s \r",StartTime
			fprintf filenum, "stopped at %s\r\r", StopTime
			
			fprintf filenum, "Number of Modules: %d\r", NumberOfModules
	
			fprintf filenum, "Module\t Run Time(s)\t Event Rate (cps)\t Total Time (s)\t Num Coinc. Trig\t Coinc. SFDT (s)\t Coinc. Count Time (s)\t Coinc. Input Rate (cps)\r" 
			for(k=0;k<NumberOfModules;k+=1)
				fprintf filenum, "%d\t %g\t %g\t %g\t", k, Module_Parameter_Values[index_RunTime][k],Module_Parameter_Values[index_EvRate][k], Module_Parameter_Values[index_TotTime][k]
				fprintf filenum, "%g\t %g\t %g\t %g\r", Module_Parameter_Values[index_NCT][k],Module_Parameter_Values[index_CSFDT][k], Module_Parameter_Values[index_CCT][k], Module_Parameter_Values[index_CIR][k]
			endfor	
			
			fprintf filenum, "\rModule\t Channel\t Count Time(s)\t Input Count Rate (cps)\t Output Count Rate (cps)\t Pass Pileup Rate (cps)\t Filter Dead Time (s)\t Gate Rate (cps)\t Gate Time (s)\r" 
			for(k=0;k<NumberOfModules;k+=1)
				for(i=0; i<NumberOfChannels; i+=1)
					fprintf filenum, "%d\t %d\t %g\t %g\t %g\t %g\t %g\t %g\t %g\r", k,i, CPV[index_COUNTTIME][i][k], CPV[index_ICR][i][k], CPV[index_OCR][i][k], CPV[index_PPR][i][k], CPV[index_SFDT][i][k], CPV[index_GCR][i][k], CPV[index_GDT][i][k]
				endfor
			endfor		
			
			fprintf filenum, "\r\r***** System Settings ****** \r\r"
			
			fprintf filenum, "Pixie Viewer release %x\r", ViewerVersion
			
			for(m=0;m<(NumSystemInParIFM+NumberOfModules);m+=1)  // number of input system parameters (or useful output variables)
				text = System_Parameter_Names[m]
				fprintf filenum, "%s\t", text
				fprintf filenum, "%g\t", System_Parameter_Values[m]
				fprintf filenum, "\r"
			endfor
			fprintf filenum, "\r"
			
			
			for(m=0;m<NumModuleInParIFM;m+=1)  // number of input module parameters (or useful output variables)
				text = Module_Parameter_Names[m]
				fprintf filenum, "%s\t", text
				for(k=0;k<NumberOfModules;k+=1)	
					fprintf filenum, "%g\t", Module_Parameter_Values[m][k]
				endfor	
				fprintf filenum, "\r"
			endfor
			
			//channel header
			fprintf filenum, "\r%s\t", "Module Number"
			for(k=0;k<NumberOfModules;k+=1)	
				for(i=0;i<NumberOfChannels;i+=1)	
					fprintf filenum, "%d\t", k
				endfor
			endfor	
			fprintf filenum, "\r"
			
			fprintf filenum, "%s\t", "Channel Number"
			for(k=0;k<NumberOfModules;k+=1)	
				for(i=0;i<NumberOfChannels;i+=1)	
					fprintf filenum, "%d\t", i
				endfor
			endfor	
			fprintf filenum, "\r"
						
			for(m=0;m<NumChannelParIFM;m+=1)  // number of  input channel parameters (or useful output variables)
				text = Channel_Parameter_Names[m]
				fprintf filenum, "%s\t", text
				for(k=0;k<NumberOfModules;k+=1)	
					for(i=0;i<NumberOfChannels;i+=1)	
						fprintf filenum, "%g\t", CPV[m][i][k]
					endfor
				endfor	
				fprintf filenum, "\r"
			endfor
			
			close filenum
		else		// if file opened not successfully
			printf "Pixie_File_RunStats: open statistics file failed, exiting ...\r" 
			return(0)
		endif
	endif
	
	if(popnum == 2) // read from ifm file
		
		Open/D/R/T="????"/M="Read Run Statistics from Text File (.ifm)"/P=MCAPath filenum as "*.ifm"
		filename=S_fileName	// full path and name of file opened, or "" if cancelled
		
		if (cmpstr(filename,"")!=0)		// if file defined succesfully
			Open/R/Z/T="????" filenum as filename
			
			FReadline filenum, line
			if (cmpstr(line[0,8], "XIA Pixie") !=0)
				DoAlert 0, "Not a valid .ifm file, exiting"
				close/a
				return (0)
			endif
			
			FReadline filenum, line
			len = strlen(line)
			StartTime = line[23,len-2]
			
			FReadline filenum, line
			len = strlen(line)
			StopTime = line[11,len-2]
			
			FReadline filenum, line	//blank
			FReadline filenum, line
			sscanf line, "Number of Modules: %d\r", NumberMod
			FReadline filenum, line	//module header
			k=0
			do
				FReadline filenum, line
				sscanf line, "%d %g %g %g %g %g %g %g", Mnum, RT, ER, TT, NCT, CSFDT, CCT, CIR
				Module_Parameter_Values[index_RunTime][Mnum] = RT
				Module_Parameter_Values[index_NumEv][Mnum] = ER*RT
				Module_Parameter_Values[index_TotTime][Mnum] = TT
				Module_Parameter_Values[index_EvRate][Mnum] = ER
				Module_Parameter_Values[index_NCT][Mnum] = NCT
				Module_Parameter_Values[index_CSFDT][Mnum] = CSFDT
				Module_Parameter_Values[index_CCT][Mnum] = CCT
				Module_Parameter_Values[index_CIR][Mnum] = CIR			
				k+=1
			while (k<NumberMod)
			
			FReadline filenum, line	//blank
			FReadline filenum, line	//channel header
			k=0
			do
				for(i=0;i<NumberOfChannels;i+=1)
					FReadline filenum, line
					sscanf line, "%d %d %g %g %g %g %g %g %g", Mnum, Chnum, LT, ICR, OCR, PPR, SFDT, GCR, GDT
					CPV[index_COUNTTIME][Chnum][Mnum] = LT
					CPV[index_ICR][Chnum][Mnum] = ICR
					CPV[index_OCR][Chnum][Mnum] = OCR
					CPV[index_PPR][Chnum][Mnum] = PPR
					CPV[index_SFDT][Chnum][Mnum] = SFDT
					CPV[index_GCR][Chnum][Mnum] = GCR
					CPV[index_GDT][Chnum][Mnum] = GDT
				endfor
				k+=1
			while (k<NumberMod)
			close filenum
			InfoSource = filename
			Pixie_RC_UpdateRunstats("ifmfile")
		else		// if file opened not successfully
			printf "Pixie_File_RunStats: open statistics file failed, exiting ...\r" 
			return(0)
		endif
	endif
	
End	

//########################################################################
//
//	Pixie_File_ExportCHN:
//		Export MCA spectrum to a ORTEC format .CHN file.
//
//########################################################################
Function Pixie_File_ExportCHN(ch, truncate)
Variable ch, truncate


	Variable HistogramLength, filenum, k, len, hour, binratio
	Variable data, numch, firstch, pos
	Variable CHN =-1
	String headerchar, datestring, timestring, strng, Msg, filename

	Svar StartTime = root:pixie4:StartTime
	Svar StopTime = root:pixie4:StopTime
	timestring = StartTime[0,8]
	truncate = 0

	wave mcawave=$("root:pixie4:MCACh"+num2str(ch))
	wavestats/q mcawave
	HistogramLength =V_npnts
	
	// Check if HistogramLength exceeds 16K channels
	if( HistogramLength> 16384)
//		Sprintf Msg, "To be compatible with ORTEC® .CHN file format, histogram length should not exceed 16384. Current histogram length is %d. Click Yes below to rebin the histogram to 16384 bins and save as a .CHN file. Click No to truncate", HistogramLength
//		DoAlert 1, Msg
		if(truncate==1)	
			print "Truncating spectrum during export"
		else		
			print "Rebinning spectrum during export"
			// Rebin the histogram
			Make/o/d/n=16384 CHNformatMCAWave
			
			binratio = HistogramLength / 16384
			if(binratio == 4)	// 64K spectrum
				for(k=0; k<16384; k+=1)
					CHNformatMCAWave[k] = mcawave[k*binratio] + mcawave[k*binratio+1] + mcawave[k*binratio+2] + mcawave[k*binratio+3]
				endfor
			elseif(binratio == 2)	// 32K spectrum
				for(k=0; k<16384; k+=1)
					CHNformatMCAWave[k] = mcawave[k*binratio] + mcawave[k*binratio+1]
				endfor
			endif						
		endif
	endif

	sprintf filename,"spectrum%d.chn",ch
	Sprintf Msg, "Save MCA %d as .CHN file",ch
	Open/D/T="????"/M=Msg/P=MCAPath filenum as filename
	if(cmpstr(S_filename, "") == 0)
		return(-1)
	endif
	Open/P=MCAPath filenum as S_filename

	FbinWrite/F=2 filenum, CHN		// data format -1 for .CHN files
	data =1
	FbinWrite/U/F=2 filenum, data		// MCA number = 1
	FbinWrite/U/F=2 filenum, data		// Segment number = 1
	
	strng =StringfromList( 2,timestring,":") 	// ss AM
	headerchar = strng[0,1]				// ss 
	FbinWrite filenum, headerchar			// sec
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_RunTime = root:pixie4:index_RunTime
	Nvar index_LIVETIME = root:pixie4:index_LIVETIME
	Nvar ncp = root:pixie4:NumChannelPar
	
	Variable Realtime = Display_Module_Parameters[index_RunTime]		// TODO find equivalent P4 value 
	data = Realtime*50
	FbinWrite/F=3 filenum, data		// in 20ms ticks
	Variable Livetime = Display_Channel_Parameters[ index_LIVETIME+ch*ncp]		// TODO find equivalent P4 value 
	data = Livetime*50
	FbinWrite/F=3 filenum, data		// in 20ms ticks

	headerchar = "xxxxxxxxxxxx"
	strng = StringfromList( 0,timestring,":")	
	headerchar[8,9]=strng						// hour
	strng = StringfromList( 1,timestring,":")		// min
	headerchar[10,11] = strng
	strng=headerchar[0,11]
	FbinWrite filenum, strng

	firstch = 0
	FbinWrite/U/F=2 filenum, firstch	// Starting Channel
	
	// Check if HistogramLength exceeds 16K channels
	if(HistogramLength <= 16384)

		Wavestats/q mcawave
		numch = V_npnts
		FbinWrite/U/F=2 filenum, numch	// Number of Channels 	
		
		for(k=firstch;k<firstch+numch;K+=1)
			data  = mcawave[k]
			FbinWrite/F=3 filenum, data
		endfor
		
	else		// Histogram length > 16384
	
		numch = 16384
		FbinWrite/U/F=2 filenum, numch	// Number of Channels 	
		
		for(k=firstch;k<firstch+numch;K+=1)
			if(truncate)
				data  = mcawave[k]
			else
				data  = CHNformatMCAWave[k]
			endif
			FbinWrite/F=3 filenum, data
		endfor
	
		// CHNformatMCAWave no longer needed
		KillWaves/Z CHNformatMCAWave
		
	endif
	
	// add footer -- 256 unknown words
	data=0
	for(k=0;k<256;k+=1)
		Switch(k)
			Case 0:
				data = 0xFF9A    // required by some programs, though not Ortec Gammavision
				break
			Case 5:
				data = 0x3F80    // required by some programs, though not Ortec Gammavision
				break
			Case 160:   
				data = 0x581D    // A text string can be encoded starting from word 160 X
				break
			Case 161:   
				data = 0x4149	// AI
				break
			Case 162:   
				data = 0x5020   // P_
				break
			Case 163:   
				data = 0x7869  //xi	//0x6C6F   //lo
				break
			Case 164:   
				data = 0x6569  //ei 	0x7261   //ra
				break
			Case 165:   
				data = 0  // 0x7369   //si
				break
			default:
				data = 0
		EndSwitch

  		FbinWrite/U/F=2 filenum, data
  	endfor

	Close filenum
	
	Return(0)
	
End

//########################################################################
//
//	Pixie_File_MCA:
//		Read MCA histogram from a previously saved MCA file.
//
//########################################################################

Function Pixie_File_MCA(ctrlName,popNum,popStr) : PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr
	
	Svar MCAsource = root:pixie4:MCASource
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar CloverAdd = root:pixie4:CloverAdd
	Svar StartTime = root:pixie4:StartTime
	Svar StopTime = root:pixie4:StopTime
	Svar SeriesStartTime = root:pixie4:SeriesStartTime
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_RunTime = root:pixie4:index_RunTime
	Nvar index_EvRate = root:pixie4:index_EvRate
	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
	Nvar index_ICR = root:pixie4:index_ICR
	Nvar ncp = root:pixie4:NumChannelPar
	
	Wave dummy0=dummy0
	Wave MCAch0 = root:pixie4:MCAch0
	Wave MCAch1 = root:pixie4:MCAch1
	Wave MCAch2 = root:pixie4:MCAch2
	Wave MCAch3 = root:pixie4:MCAch3
	Wave MCAsum = root:pixie4:MCAsum
	Nvar ChosenModule = root:pixie4:ChosenModule
	
	variable MCA16nSum, RunType
	Wave RunTasks = root:pixie4:RunTasks
	Nvar WhichRun = root:pixie4:WhichRun
	RunType=RunTasks[WhichRun-1]
	MCA16nSum = ( (RunType==0x402) || (CloverAdd==1) )
	
	Variable filenum,i, truncate
	Variable HistogramLength
	String filename,wav,Msg

	
	if(popnum == 1) // save as itx file
	
		
		Open/T="IGTX"/M="Save MCA as Igor Text File (.itx)"/P=MCAPath filenum
		filename=S_fileName	// full path and name of file opened, or "" if cancelled
		if (cmpstr(filename,"")!=0)		// if file opened succesfully
			fprintf filenum, "IGOR\r"
			fprintf filenum, "X // XIA Pixie4 MCA data saved %s, %s; Run started %s, ended %s\r",date(),time(),StartTime, StopTime
			fprintf filenum, "X // File series started %s, \r",SeriesStartTime
	
			Variable RunTime = Display_Module_Parameters[index_RunTime]
			fprintf filenum, "X // Run Time [s]= %g\r", RunTime
		
			Variable EventRate = Display_Module_Parameters[index_EvRate]
			fprintf filenum, "X // Event Rate [cps]= %g\r", EventRate
			
			for(i=0; i<NumberOfChannels; i+=1)
				fprintf filenum, "X // Channel %g\r", i
				fprintf filenum, "X // Count Time [s]= %g\r", Display_Channel_Parameters[index_COUNTTIME+i*ncp]
				fprintf filenum, "X // Input Count Rate [cps] = %g\r", Display_Channel_Parameters[ index_ICR+i*ncp]
			endfor
			close filenum
			
			for(i=0; i<NumberOfChannels; i+=1)
				wav="root:pixie4:MCACh"+num2str(i)
				Save/A/t/p=MCAPath $wav as filename
			endfor
			
			if (MCA16nSum==1)	//Addback spectrum
				wav="root:pixie4:MCAsum"
				Save/A/t/p=MCAPath $wav as filename
			endif
			
			for(i=0; i<NumberOfChannels; i+=1)
				wav="root:pixie4:MCAtotal"+num2str(i)
				Save/A/t/p=MCAPath $wav as filename
			endfor
					
		else		// if file opened not successfully
			printf "Pixie_SaveMCA: open MCA spectrum file failed, exiting ..." 
			return(0)
		endif
	endif
	
	if(popnum == 2) // read from itx file
		String fldr
		
		fldr = GetDataFolder(1)
		SetDataFolder "root:pixie4"
		LoadWave/o/t/p=MCAPath
		SetDataFolder fldr
		
	
		MCASource = S_fileName
	endif
	
	if(popnum == 3) // read from auto saved binary file
		Variable fil
		open/p=MCAPath/D/R/T="????" fil
		if(cmpstr(S_fileName,"")==0)
			print "Error reading file or user cancelled"
			return(0)
		endif
		//Svar S_fileName = root:S_fileName
		Svar MCAFileName = root:pixie4:MCAFileName
		MCAFileName = Pixie_FileNameConversion(S_fileName)
		Pixie_RC_UpdateMCAData("file")
	endif
	
	if( (popnum > 3) && (popnum <8) ) // Export CHN
	
		// Check if HistogramLength exceeds 16K channels
		wave mcawave=$("root:pixie4:MCACh"+num2str(popnum-4))
		wavestats/q mcawave
		HistogramLength =V_npnts
		if( HistogramLength> 16384)
			Sprintf Msg, "To be compatible with ORTEC® .CHN file format, histogram length should not exceed 16384. Current histogram length is %d. Click Yes below to rebin the histogram to 16384 bins and save as a .CHN file. Click No to truncate", HistogramLength
			DoAlert 1, Msg
			if(V_flag == 2)	// No is clicked
				truncate=1
			else
				truncate=0
			endif	
		endif
			
		Pixie_File_ExportCHN(popnum-4,truncate)
	endif
	
	if(popnum ==8)  // Export CHN all ch. 
		// Check if HistogramLength exceeds 16K channels, same action for all 4 channels
		wave mcawave=$("root:pixie4:MCACh"+num2str(0))
		wavestats/q mcawave
		HistogramLength = V_npnts
		wave mcawave=$("root:pixie4:MCACh"+num2str(1))
		wavestats/q mcawave
		HistogramLength = max(HistogramLength,V_npnts)
		wave mcawave=$("root:pixie4:MCACh"+num2str(2))
		wavestats/q mcawave
		HistogramLength = max(HistogramLength,V_npnts)
		wave mcawave=$("root:pixie4:MCACh"+num2str(3))
		wavestats/q mcawave
		HistogramLength = max(HistogramLength,V_npnts)
		if( HistogramLength> 16384)
			Sprintf Msg, "To be compatible with ORTEC® .CHN file format, histogram length should not exceed 16384. At least one histogram length is %d. Click Yes below to rebin the histogram to 16384 bins and save as a .CHN file. Click No to truncate", HistogramLength
			DoAlert 1, Msg
			if(V_flag == 2)	// No is clicked
				truncate=1
			else
				truncate=0
			endif	
		endif
	
		Pixie_File_ExportCHN(0,truncate)
		Pixie_File_ExportCHN(1,truncate)
		Pixie_File_ExportCHN(2,truncate)
		Pixie_File_ExportCHN(3,truncate)
	endif
	
	if(popnum ==9)  // Export CSV all ch. 
		Variable nbins = 32768
				
		Open/T=".csv"/M="Save MCA as csv File (.csv)"/P=MCAPath filenum
		filename=S_fileName	// full path and name of file opened, or "" if cancelled
		if (cmpstr(filename,"")!=0)		// if file opened succesfully
			fprintf filenum, "bin,MCAch0,MCAch1,MCAch2,MCAch3,MCAsum\n"
		
			
			for(i=0; i<nbins; i+=1)
				fprintf filenum, "%d,%d,%d,%d,%d,%d\n",i,MCAch0[i],MCAch1[i],MCAch2[i],MCAch3[i],MCAsum[i]
			endfor
			
			close filenum
					
		else		// if file opened not successfully
			printf "Pixie_SaveMCA: open MCA spectrum file failed, exiting ..." 
			return(0)
		endif
		
		
	endif
	
End	



//########################################################################
//
//	Pixie_File_GenerateVersionText:
//		Create a text file containing version information.
//
//########################################################################
Function Pixie_File_GenerateVersionText(ctrlName) : ButtonControl
	String ctrlName

	Nvar ViewerVersion = root:pixie4:ViewerVersion
	Nvar CLibraryRelease = root:pixie4:CLibraryRelease
	Nvar CLibraryBuild = root:pixie4:CLibraryBuild
	Nvar index_DSPrelease = root:pixie4:index_DSPrelease
	Nvar index_DSPbuild = root:pixie4:index_DSPbuild
	Nvar index_FippiID = root:pixie4:index_FippiID
	Nvar index_SystemID = root:pixie4:index_SystemID
	Nvar index_BoardVersion = root:pixie4:index_BoardVersion
	Nvar index_SerialNum = root:pixie4:index_SerialNum
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
		
	Variable filnum
	Open/T="TEXT" filnum
	if(cmpstr(S_fileName, "") != 0)
		fprintf filnum, "Pixie4-Viewer=%04x\r", ViewerVersion
		fprintf filnum, "C-library release=%04x\r", CLibraryRelease
		fprintf filnum, "C-library build=%04X\r", CLibraryBuild
		fprintf filnum, "DSP code release=%04x\r", Display_Module_Parameters[index_DSPrelease]
		fprintf filnum, "DSP code build=%04X\r", Display_Module_Parameters[index_DSPbuild]
		fprintf filnum, "FiPPI version=%04X\r", Display_Module_Parameters[index_FippiID]
		fprintf filnum, "System version=%04X\r", Display_Module_Parameters[index_SystemID]
		fprintf filnum, "Board version=%04X\r", Display_Module_Parameters[index_BoardVersion]
		fprintf filnum, "Serial number=%d\r", Display_Module_Parameters[index_SerialNum]			
		
		close(filnum)
	else
		DoAlert 0,"Version information file can not be opened"
	endif
End

//########################################################################
//	Pixie_Tab_ShowControls: Disable the controls in a tab
//########################################################################
Function Pixie_Tab_ShowControls(win,newTab)
	String win
	Variable newTab
	
	DoWindow/F $win
	if(V_flag!=1)
		return(0)
	endif
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	
	String listOfControlNames=ControlNameList(win)
	
	String controlName,matchStr,type
	Variable index=0
	
	switch(newTab)	// numeric switch
		case 0:		// execute if case matches expression
			matchStr = "TR*"
			break
		case 1:		// execute if case matches expression
			matchStr = "EG*"
			break
		case 2:		// execute if case matches expression
			matchStr = "WA*"
			break
		case 3:		// execute if case matches expression
			matchStr = "GA*"
			break
		case 4:		// execute if case matches expression
			matchStr = "CO*"
			break
		case 5:		// execute if case matches expression
			matchStr = "AD*"
			break
		case 6:		// execute if case matches expression
			matchStr = "RU*"
			break
		case 7:		// execute if case matches expression
			matchStr = "RE*"
			break
		default:
			matchStr = ""
			break
	endswitch
	

	do
		controlName= StringFromList(index,listOfControlNames)
		if( strlen(controlName) == 0 )
			break
		endif
		if( stringmatch(controlName,matchStr) )
			ControlInfo/W=$win $controlName
			if( V_Flag )
				type=StringFromList(abs(V_Flag),";Button;Checkbox;PopupMenu;ValDisplay;SetVariable;Chart;Slider;TabControl;GroupBox;TitleBox;ListBox;")
				if( strlen(type) )
					if(RunInProgress)
						if( cmpstr(Controlname,"RUN_EndDAQ")==0)
							Execute type+" "+controlName+" win="+win+", disable=0"
						else
							if(  (cmpstr(Controlname[2],"X")==0) && (HideDetail==1) )
								Execute type+" "+controlName+" win="+win+", disable=1"
							else
								Execute type+" "+controlName+" win="+win+", disable=2"
							endif
						endif
					else
						if( cmpstr(Controlname,"RUN_EndDAQ")==0)
							Execute type+" "+controlName+" win="+win+", disable=2"
						else
							if(  (cmpstr(Controlname[2],"X")==0) && (HideDetail==1) )
								Execute type+" "+controlName+" win="+win+", disable=1"
							else
								Execute type+" "+controlName+" win="+win+", disable=0"
							endif
						endif
					endif
				endif
			endif
		endif
		index += 1
	while( 1 )
End

//########################################################################
//	Pixie_Tab_HideControls: Disable the controls in a tab
//########################################################################
Function Pixie_Tab_HideControls(win,prevTab)
	String win
	Variable prevTab
	
	
	String listOfControlNames=ControlNameList(win)
	
	String controlName,matchStr,type
	Variable index=0
	
	switch(prevTab)	// numeric switch
		case 0:		// execute if case matches expression
			matchStr = "TR*"
			break
		case 1:		// execute if case matches expression
			matchStr = "EG*"
			break
		case 2:		// execute if case matches expression
			matchStr = "WA*"
			break
		case 3:		// execute if case matches expression
			matchStr = "GA*"
			break
		case 4:		// execute if case matches expression
			matchStr = "CO*"
			break
		case 5:		// execute if case matches expression
			matchStr = "AD*"
			break
		case 6:		// execute if case matches expression
			matchStr = "RU*"
			break
		default:
			matchStr = ""
			break
	endswitch
	
	
	
	
	do
		controlName= StringFromList(index,listOfControlNames)
		if( strlen(controlName) == 0 )
			break
		endif
		if( stringmatch(controlName,matchStr) )
			ControlInfo/W=$win $controlName
			if( V_Flag )
				type=StringFromList(abs(V_Flag),";Button;Checkbox;PopupMenu;ValDisplay;SetVariable;Chart;Slider;TabControl;GroupBox;TitleBox;ListBox;")
				if( strlen(type) )
					Execute type+" "+controlName+" win="+win+", disable=1"
				endif
			endif
		endif
		index += 1
	while( 1 )
End




//########################################################################
//	Pixie_Tab_Switch: Switch between Tabs 
//########################################################################

Function Pixie_Tab_Switch(name,tabNum)
	String name
	Variable tabNum
	
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	Nvar prevTab= root:pixie4:PrevTabNumber
	Nvar RunInProgress = root:pixie4:RunInProgress

	if( tabNum != -1 && tabNum != prevTab )
		Pixie_Tab_HideControls("Pixie_Parameter_Setup",prevTab)	// hide previous
	endif
	
	 Pixie_Tab_ShowControls("Pixie_Parameter_Setup",tabnum)//showcurrent

	prevTab= tabNum
	PanelTabNumber=tabNum
	
	if(RunInProgress)
		Button/Z MainStartRun,disable=2,win = Pixie4MainPanel
		Button/Z MainStopRun,disable=0,win = Pixie4MainPanel
		SetVariable/Z Main_TimeOut, disable=2,win = Pixie4MainPanel
		SetVariable/Z Main_RepCount, disable=2,win = Pixie4MainPanel
		PopupMenu/Z RUN_WhichRun, disable=2, win = Pixie4MainPanel	
	else
		Button/Z MainStartRun,disable=0,win = Pixie4MainPanel
		Button/Z MainStopRun,disable=2,win = Pixie4MainPanel
		SetVariable/Z Main_TimeOut, disable=0,win = Pixie4MainPanel
		SetVariable/Z Main_RepCount, disable=0,win = Pixie4MainPanel
		PopupMenu/Z RUN_WhichRun, disable=0, win = Pixie4MainPanel
	endif

End

Function Pixie_Tab_ScopeSwitch(name,tabNum)
	String name
	Variable tabNum
	
	if(tabnum==0)
		Button FFTDisplay, disable=0, win = Pixie4Oscilloscope
		Button ADCFilterDisplay, disable=0, win = Pixie4Oscilloscope
		Button ADCDisplayCapture, disable=0, win = Pixie4Oscilloscope
		Button ADCDisplaySave, disable=0, win = Pixie4Oscilloscope
		Button ADX_I2ECalib,disable=0, win = Pixie4Oscilloscope
		Button ADX_I2EReset,disable=0, win = Pixie4Oscilloscope
	
		PopupMenu TauFit, disable=1, win = Pixie4Oscilloscope
		Popupmenu TauOK,disable=1, win = Pixie4Oscilloscope
		SetVariable LastTau,disable=1, win = Pixie4Oscilloscope
		SetVariable TauDeviation,disable=1, win = Pixie4Oscilloscope
		Button TauClear,disable=1, win = Pixie4Oscilloscope
		
		TitleBox ICRtitle, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR0, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR1, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR2, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR3, disable=1, win = Pixie4Oscilloscope
		TitleBox OORFtitle, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF0, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF1, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF2, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF3, disable=1, win = Pixie4Oscilloscope
	endif
	
	if(tabnum==1)
		Button FFTDisplay, disable=1, win = Pixie4Oscilloscope
		Button ADCFilterDisplay, disable=1, win = Pixie4Oscilloscope
		Button ADCDisplayCapture, disable=1, win = Pixie4Oscilloscope
		Button ADCDisplaySave, disable=1, win = Pixie4Oscilloscope
		Button ADX_I2ECalib,disable=1, win = Pixie4Oscilloscope
		Button ADX_I2EReset,disable=1, win = Pixie4Oscilloscope
	
		PopupMenu TauFit, disable=0, win = Pixie4Oscilloscope
		Popupmenu TauOK,disable=0, win = Pixie4Oscilloscope
		SetVariable LastTau,disable=0, win = Pixie4Oscilloscope
		SetVariable TauDeviation,disable=0, win = Pixie4Oscilloscope
		Button TauClear,disable=0, win = Pixie4Oscilloscope
		
		TitleBox ICRtitle, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR0, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR1, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR2, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_ICR3, disable=1, win = Pixie4Oscilloscope
		TitleBox OORFtitle, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF0, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF1, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF2, disable=1, win = Pixie4Oscilloscope
		SetVariable ADC_OORF3, disable=1, win = Pixie4Oscilloscope
	endif
	
	if(tabnum==2)
		Button FFTDisplay, disable=1, win = Pixie4Oscilloscope
		Button ADCFilterDisplay, disable=1, win = Pixie4Oscilloscope
		Button ADCDisplayCapture, disable=1, win = Pixie4Oscilloscope
		Button ADCDisplaySave, disable=1, win = Pixie4Oscilloscope
		Button ADX_I2ECalib,disable=1, win = Pixie4Oscilloscope
		Button ADX_I2EReset,disable=1, win = Pixie4Oscilloscope
	
		PopupMenu TauFit, disable=1, win = Pixie4Oscilloscope
		Popupmenu TauOK,disable=1, win = Pixie4Oscilloscope
		SetVariable LastTau,disable=1, win = Pixie4Oscilloscope
		SetVariable TauDeviation,disable=1, win = Pixie4Oscilloscope
		Button TauClear,disable=1, win = Pixie4Oscilloscope
		
		TitleBox ICRtitle, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_ICR0, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_ICR1, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_ICR2, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_ICR3, disable=0, win = Pixie4Oscilloscope
		TitleBox OORFtitle, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_OORF0, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_OORF1, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_OORF2, disable=0, win = Pixie4Oscilloscope
		SetVariable ADC_OORF3, disable=0, win = Pixie4Oscilloscope
	endif
	
End


////########################################################################
////	Initialize global variables for auto optimization routines
////########################################################################
//Macro Pixie_InitAutoScanVar()
//
//	// Disable the display of Macro commands
//	Silent 1
//	
//	// Create a new folder data
//	NewDataFolder/o root:auto
//	
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:lastFWHM	// used for monitoring the best FWHM	
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:EFRTLow		// starting energy filter rise time to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:EFRTHigh	// last energy filter rise time to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:EFFTLow		// starting energy filter flat top to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:EFFTHigh		// last energy filter flat top to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:TauHigh		// last tau to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:TauLow		// starting tau to be scanned
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:TauStep		// scanning step size of tau
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:BestEFRT	// best energy filter rise time
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:BestEFFT	// best energy filter flat top
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:BestTau		// best Tau value
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:keepEFRT	// temporary storage of best energy filter rise time
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:keepEFFT	// temporary storage of best energy filter flat top
//	make/o/n=(root:pixie4:NumberOfChannels) root:auto:keepTau		// temporary storage of best Tau
//	
//	Variable/G root:auto:chEFRTLow		// starting energy filter rise time to be scanned for the current channel
//	Variable/G root:auto:chEFRTHigh		// last energy filter rise time to be scanned for the current channel
//	Variable/G root:auto:chEFFTLow		// starting energy filter flat top to be scanned for the current channel
//	Variable/G root:auto:chEFFTHigh		// last energy filter flat top to be scanned for the current channel
//
//	Variable/G root:auto:chTauLow			// starting Tau value to be scanned for the current channel
//	Variable/G root:auto:chTauHigh			// last Tau value to be scanned for the current channel
//	Variable/G root:auto:chTauStep		// step size used for Tau auto-scan for the current channel
//	
//	Variable/G root:auto:chEFScanMask	// bit mask for determining whether scanning a channel
//	Variable/G root:auto:chTauScanMask	// bit mask for determining whether scanning a channel
//	
//EndMacro
//
//
////########################################################################
////	Create auto energy filter optimization panel
////########################################################################
//Function Pixie_Auto_EFOptimization()
//	
////	Nvar FilterRange = root:pixie4:FilterRange
//	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
//	Nvar EFMinRiseTimeTicks = root:pixie4:EFMinRiseTimeTicks
//	Nvar EFMaxRiseTimeTicks = root:pixie4:EFMaxRiseTimeTicks
//	Nvar EFMinFlatTopTicks = root:pixie4:EFMinFlatTopTicks
//	Nvar EFMaxFlatTopTicks = root:pixie4:EFMaxFlatTopTicks
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar chEFScanMask = root:auto:chEFScanMask
//	
//	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
//	Nvar index_FilterRange = root:pixie4:index_FilterRange
//	Variable Filterrange = Display_Module_Parameters[index_FilterRange]
//	
//	PauseUpdate; Silent 1		// building window...
//	DoWindow/F Pixie_Auto_EFOptimizationPanel
//	if (V_flag==1)
//		return(0)
//	endif
//
//	PauseUpdate; Silent 1		// building window...
//	NewPanel/K=1/W=(281.25,292.25,684.75,600) as "Auto Optimization of Energy Filter"
//	ModifyPanel cbRGB=(51456,44032,58880)
//	DoWindow/C Pixie_Auto_EFOptimizationPanel
//	SetDrawLayer UserBack
//	SetDrawEnv fsize= 14,fstyle= 1
//	DrawText 42,23,"Automatic scanning of Energy Filter settings"
//	SetDrawEnv fsize= 11,fstyle= 1
//	DrawText 24,49,"Before starting scan, please confirm the following conditions:"
//	
//	SetVariable SelectedPixie4Channel pos={280,165}, size={100,20},title="Channel"
//	SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=11, value=root:pixie4:ChosenChannel
//	SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
//	
//
//	Button DoScan,pos={296,195},size={75,28},proc=Pixie_Auto_ScanFilterTime,title="Start Scan",win=Pixie_Auto_EFOptimizationPanel
//	Button HelpOptimize_Energy_Filter, pos={296,230},size={75,28},proc=Pixie_CallHelp,title="Help"
//	Button CloseAutoScan,pos={296,265},size={75,28},proc=Pixie_AnyPanelClose,title="Close",win=Pixie_Auto_EFOptimizationPanel
//	
//	GroupBox EnergyFilterLmitsGroup,pos={15,155},size={235,140},title="Auto scanning limits",fcolor=(1,1,1)
//	GroupBox EnergyFilterLmitsGroup,fSize=11,fStyle=1
//	
//	drawtext 75,200,"Rise Time"
//	
//	SetVariable RTScanLowerLimit,pos={25,210},size={125,18},title="Start [µs]",win=Pixie_Auto_EFOptimizationPanel
//	SetVariable RTScanLowerLimit,font="Arial",format="%.03f",proc=Pixie_Auto_SetEFLimits
//	SetVariable RTScanLowerLimit,value= root:auto:chEFRTLow,bodyWidth= 75
//	SetVariable RTScanUpperLimit,pos={25,235},size={125,18},title="End [µs]",win=Pixie_Auto_EFOptimizationPanel
//	SetVariable RtScanUpperLimit,font="Arial",format="%.03f",proc=Pixie_Auto_SetEFLimits
//	SetVariable RTScanUpperLimit,value= root:auto:chEFRTHigh,bodyWidth= 75
//
//	drawtext 160,200,"Flat Top"
//	
//	SetVariable FTScanLowerLimit,pos={110,210},size={125,18},title=" ",win=Pixie_Auto_EFOptimizationPanel
//	SetVariable FTScanLowerLimit,font="Arial",format="%.03f",proc=Pixie_Auto_SetEFLimits
//	SetVariable FTScanLowerLimit,value= root:auto:chEFFTLow,bodyWidth= 75
//	SetVariable FTScanUpperLimit,pos={110,235},size={125,18},title=" ",win=Pixie_Auto_EFOptimizationPanel
//	SetVariable FTScanUpperLimit,font="Arial",format="%.03f",proc=Pixie_Auto_SetEFLimits
//	SetVariable FTScanUpperLimit,value= root:auto:chEFFTHigh,bodyWidth= 75
//
//	SetVariable RTScanLowerLimit,limits={EFMinRiseTimeTicks*2^FilterRange/FilterClockMHz,EFMaxRiseTimeTicks*2^FilterRange/FilterClockMHz,2^FilterRange/FilterClockMHz}
//	SetVariable RTScanUpperLimit,limits={EFMinRiseTimeTicks*2^FilterRange/FilterClockMHz,EFMaxRiseTimeTicks*2^FilterRange/FilterClockMHz,2^FilterRange/FilterClockMHz}
//	SetVariable FTScanLowerLimit,limits={EFMinFlatTopTicks*2^FilterRange/FilterClockMHz,EFMaxFlatTopTicks*2^FilterRange/FilterClockMHz,2^FilterRange/FilterClockMHz}
//	SetVariable FTScanUpperLimit,limits={EFMinFlatTopTicks*2^FilterRange/FilterClockMHz,EFMaxFlatTopTicks*2^FilterRange/FilterClockMHz,2^FilterRange/FilterClockMHz}
//
//	CheckBox DoScanThisChEF,pos={125,265},size={239,18},title="Scan this channel?", proc=Pixie_Auto_SetChScanBitMask
//	CheckBox DoScanThisChEF,value= TSTbit(ChosenChannel, chEFScanMask), win=Pixie_Auto_EFOptimizationPanel
//
//	CheckBox CheckADCTraces,pos={40,55},size={229,14},title="ADC traces on the Oscilloscope are in range?"
//	CheckBox CheckADCTraces,value= 0,win=Pixie_Auto_EFOptimizationPanel
//	CheckBox CheckRunTime,pos={40,75},size={253,14},title="Run time and polling time have been set properly?"
//	CheckBox CheckRunTime,value= 0,win=Pixie_Auto_EFOptimizationPanel
//	CheckBox CheckMCAROI,pos={40,95},size={311,14},title="MCA Gauss fit range has been set properly for each channel?"
//	CheckBox CheckMCAROI,value= 0,win=Pixie_Auto_EFOptimizationPanel
//	CheckBox CheckScanLimits,pos={40,115},size={239,14},title="Auto scan limits have been set properly below for each channel?"
//	CheckBox CheckScanLimits,value= 0,win=Pixie_Auto_EFOptimizationPanel
//
//End
//
//
////########################################################################
////	Set scanning limits of energy filter length
////########################################################################
//Function Pixie_Auto_SetEFLimits(ctrlName,varNum,varStr,varName) : SetVariableControl
//String ctrlName
//Variable varNum
//String varStr
//String varName
//	
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar chEFRTLow = root:auto:chEFRTLow
//	Nvar chEFRTHigh = root:auto:chEFRTHigh
//	Nvar chEFFTLow = root:auto:chEFFTLow
//	Nvar chEFFTHigh = root:auto:chEFFTHigh
//
//	wave EFRTLow = root:auto:EFRTLow
//	wave EFRTHigh = root:auto:EFRTHigh
//	wave EFFTLow = root:auto:EFFTLow
//	wave EFFTHigh = root:auto:EFFTHigh
//	
//	Strswitch(ctrlName)
//
//		case "RTScanLowerLimit":
//			if(chEFRTLow > EFRTHigh[ChosenChannel])
//				chEFRTLow = EFRTHigh[ChosenChannel]
//				EFRTLow[ChosenChannel] = EFRTHigh[ChosenChannel]
//				DoAlert 0, "Rise Time Start can not be greater than Rise Time End."
//				return(-1)
//			endif
//				
//			EFRTLow[ChosenChannel] = chEFRTLow
//			break			
//	
//		case "RTScanUpperLimit":
//			if(chEFRTHigh < EFRTLow[ChosenChannel])
//				chEFRTHigh = EFRTLow[ChosenChannel]
//				EFRTHigh[ChosenChannel] = EFRTLow[ChosenChannel]
//				DoAlert 0, "Rise Time End can not be smaller than Rise Time Start."
//				return(-2)
//			endif
//				
//			EFRTHigh[ChosenChannel] = chEFRTHigh
//			break			
//		
//		case "FTScanLowerLimit":
//			if(chEFFTLow > EFFTHigh[ChosenChannel])
//				chEFFTLow = EFFTHigh[ChosenChannel]
//				EFFTLow[ChosenChannel] = EFFTHigh[ChosenChannel]
//				DoAlert 0, "Flat Top Start can not be greater than Flat Top End."
//				return(-2)
//			endif
//
//			EFFTLow[ChosenChannel] = chEFFTLow
//			break			
//				
//		case "FTScanUpperLimit":
//			if(chEFFTHigh < EFFTLow[ChosenChannel])
//					chEFFTHigh = EFFTLow[ChosenChannel]
//					EFFTHigh[ChosenChannel] = EFFTLow[ChosenChannel]
//					DoAlert 0, "Flat Top End can not be smaller than Flat Top Start."
//					return(-2)
//				endif
//				
//			EFFTHigh[ChosenChannel] = chEFFTHigh
//			break
//	EndSwitch
//End
//
//
////########################################################################
////	Start scanning of energy filter lengths
////########################################################################
//Function Pixie_Auto_ScanFilterTime(ctrlName) : ButtonControl
//	String ctrlName
//
//
//	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
//	Nvar index_FilterRange = root:pixie4:index_FilterRange
//	Nvar index_RunTime = root:pixie4:index_RunTime
//	Nvar index_EvRate = root:pixie4:index_EvRate
//	Variable Filterrange = Display_Module_Parameters[index_FilterRange]
//	Variable RunTime = Display_Module_Parameters[index_RunTime]
//	Variable EventRate = Display_Module_Parameters[index_EvRate]
//
//
//	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
//	Nvar RunTimeOut = root:pixie4:RunTimeOut
//	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
//	Nvar ChosenModule = root:pixie4:ChosenModule
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar EFMinRiseTimeTicks = root:pixie4:EFMinRiseTimeTicks
//	Nvar EFMaxRiseTimeTicks = root:pixie4:EFMaxRiseTimeTicks
//	Nvar EFMinFlatTopTicks = root:pixie4:EFMinFlatTopTicks
//	Nvar EFMaxFlatTopTicks = root:pixie4:EFMaxFlatTopTicks
//	Nvar chEFScanMask = root:auto:chEFScanMask
//	Wave EFRTLow = root:auto:EFRTLow
//	Wave EFRTHigh = root:auto:EFRTHigh
//	Wave EFFTLow = root:auto:EFFTLow
//	Wave EFFTHigh = root:auto:EFFTHigh
//	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
//	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
//	Nvar index_ICR = root:pixie4:index_ICR
//	Nvar ncp = root:pixie4:NumChannelPar
//	Nvar index_SL = root:pixie4:index_SL
//	Nvar index_SG = root:pixie4:index_SG
//	Variable EnergyRiseTime =Display_Channel_Parameters[index_SL+ChosenChannel*ncp]
//	Variable EnergyFlatTop =Display_Channel_Parameters[index_SG+ChosenChannel*ncp]
//
//	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
//	Wave mcawave = root:pixie4:mcawave
//	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
//	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
//	Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea
//	Wave keepEFRT = root:auto:keepEFRT
//	Wave keepEFFT = root:auto:keepEFFT
//	Wave BestEFRT = root:auto:BestEFRT
//	Wave BestEFFT = root:auto:BestEFFT
//	Wave lastFWHM = root:auto:lastFWHM
//		
//	Variable ch, tmp, controlvalue
//	Variable StartTime, TimeDiff, RunActive, filnum1, filnum2, maxFilterLen
//	Variable doneRT, doneFT
//	String tmpfilnam
//
//	Controlinfo/w=Pixie_Auto_EFOptimizationPanel CheckADCTraces
//	controlvalue = V_Value
//	Controlinfo/w=Pixie_Auto_EFOptimizationPanel CheckRunTime
//	controlvalue += V_Value
//	Controlinfo/w=Pixie_Auto_EFOptimizationPanel CheckMCAROI
//	controlvalue += V_Value
//	Controlinfo/w=Pixie_Auto_EFOptimizationPanel CheckScanLimits
//	controlvalue += V_Value
//	
//	if(controlvalue != 4)
//		DoAlert 0, "Please confirm all conditions first."
//		return(0)
//	endif
//	
//	if(chEFScanMask == 0)
//		DoAlert 0, "Please select 'Scan this channel?' for at least one channel."
//		return(0)
//	endif
//	
//	// File dialog to open output file	
//	open/T="TEXT"/M="Create a text file to store scanning results" filnum1
//	if(filnum1 == 0)
//		DoAlert 0,"Failed to open output file"
//		return(-1)
//	endif
//	// Temp file uses the same file name as the output file but with extension .tmp
//	tmpfilnam = S_filename + ".tmp"
//	open filnum2 as tmpfilnam
//	if(filnum2 == 0)
//		DoAlert 0,"Failed to open temp file"
//		return(-1)
//	endif	
//	
//	Button DoScan,disable=2,win=Pixie_Auto_EFOptimizationPanel
//	
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//
//		// Store the current filter time
//		keepEFRT[ch]=Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_RISETIME")][ch][ChosenModule]
//		keepEFFT[ch]=Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_FLATTOP")][ch][ChosenModule]
//		
//		// Initialize lastFWHM
//		lastFWHM[ch]=100
//		
//	endfor		
//
//	// Print file header for output file
//	fprintf filnum1, "Automatic scanning of energy filter rise time and flat top started at %s, %s\r\r",date(),time()
//	fprintf filnum1, "Filter range = %d\r\r", FilterRange
//	fprintf filnum1,"Ch.#     Rise Time [µs]  Flat Top [µs]      FWHM [%%]\r"
//	
//	// Print file header for temp file
//	fprintf filnum2, "Automatic scanning of energy filter rise time and flat top started at %s, %s\r\r",date(),time()
//	fprintf filnum2, "Filter range = %d\r\r", FilterRange
//
//	// Variables used to store energy filter times locally
//	make/o/n=(NumberOfChannels) root:auto:EFRTlen, root:auto:EFFTlen
//	wave EFRTlen = root:auto:EFRTlen
//	wave EFFTlen = root:auto:EFFTlen
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		EFRTlen[ch] = EFRTLow[ch]  // Initialize rise times
//	endfor
//	
//	maxFilterLen = (EFMinRiseTimeTicks+EFMaxFlatTopTicks)*2^FilterRange/FilterClockMHz
//	
//	// Determine if a channel needs to be scanned
//	doneRT = 0
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		if(TSTbit(ch, chEFScanMask) == 0)
//			doneRT = SETBit(ch, doneRT)
//		endif		
//	endfor
//
//	// Scan all possible combinations of energy filter times
//	do
//		// Set energy filter rise time for each channel
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//		
//			if(TSTbit(ch, doneRT) == 0)
//				// Intentionally keep EnergyFlatTop to minimum so that EnergyRiseTime can be set accordingly
//				EnergyFlatTop=EFMinFlatTopTicks*2^FilterRange/FilterClockMHz
//				Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_FLATTOP")][ch][ChosenModule]=EnergyFlatTop
//				Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, ch)
//
//				// Set EnergyRiseTime
//				EnergyRiseTime = EFRTlen[ch]
//				Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_RISETIME")][ch][ChosenModule] = EnergyRiseTime
//				Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, ch)
//				// Read back EnergyRiseTime
//				EFRTlen[ch] = Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_RISETIME")][ch][ChosenModule]
//			endif
//			
//		endfor
//		
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			EFFTlen[ch] = EFFTLow[ch]
//			if(TSTbit(ch, doneRT) == 0)
//				doneFT = CLRbit(ch, doneFT)
//			else
//				doneFT = SETbit(ch, doneFT)
//			endif
//		endfor
//		
//		make/o/u/i/n=1 dummy	
//		do
//			for(ch=0; ch<NumberOfChannels; ch+=1)
//				if( (TSTbit(ch, doneRT) == 0) && (TSTbit(ch, doneFT) == 0) )
//					// Check energy filter times limit
//					if((EFFTlen[ch] + EFRTlen[ch]) > maxFilterLen)
//						doneFT = SETbit(ch, doneFT)
//					else
//						// Set EnergyFlatTop
//						EnergyFlatTop = EFFTlen[ch]
//						Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_FLATTOP")][ch][ChosenModule] = EnergyFlatTop
//						Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, ch)
//						// Read back EnergyFlatTop
//						EFFTlen[ch] = Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_FLATTOP")][ch][ChosenModule]
//					endif
//				endif
//			endfor
//
//			
//			Pixie4_Acquire_Data(0x1301, dummy, "", ChosenModule)  // Start a new MCA run
//			
//			StartTime=DateTime
//			do
//				Sleep/T 60
//				TimeDiff=DateTime - StartTime
//			while( TimeDiff < RunTimeOut )
//		
//			// stop the MCA run
//			Pixie4_Acquire_Data(0x3301, root:pixie4:mcawave, "", ChosenModule)
//			// read out the MCA spectra
//		//	Pixie4_Acquire_Data(0x9001, root:pixie4:mcawave, "", ChosenModule)
//		//	mcawave[0] = mcawave[0]		// make sure IGOR updates the displays
//			Pixie_RC_UpdateMCAData("module")
//		//	Pixie_StopRun("")
//
//			// Gauss fit
//			for(ch=0; ch<NumberOfChannels; ch+=1)
//				if( (TSTbit(ch, doneRT) == 0) && (TSTbit(ch, doneFT) == 0) )
//					Pixie_Math_GaussFit("GaussFitMCA", ch+1, "")
//				
//					// print out tau values and FWHM to output file
//					fprintf filnum1,"%d %14.4f %18.4f %14.4f\r",ch, EFRTlen[ch], EFFTlen[ch], MCAChannelFWHMPercent[ch]
//					
//					// print out to Igor history window
//					printf "Ch#=%d\tRT=%8.4f\tFT=%8.4f\tFWHM=%8.4f\r",ch, EFRTlen[ch], EFFTlen[ch], MCAChannelFWHMPercent[ch]
//				endif
//			endfor	
//				
//			// print out all relevant infomation to temp file
//			fprintf filnum2, "\tRun Time [s]= %g\r", Display_Module_Parameters[index_RunTime]
//			fprintf filnum2, "\tEvent Rate [cps]= %g\r\r", Display_Module_Parameters[index_EvRate]
//		
//			for(ch=0; ch<NumberOfChannels; ch+=1)
//				if( (TSTbit(ch, doneRT) == 0) && (TSTbit(ch, doneFT) == 0) )
//					fprintf filnum2, "\tChannel %g\r", ch
//					fprintf filnum2, "\tEnergy Filter Rise Time [µs]= %g\r", EFRTlen[ch]
//					fprintf filnum2, "\tEnergy Filter Flat Top [µs]= %g\r", EFFTlen[ch]		
//					fprintf filnum2, "\t\tCount Time [s]= %g\r", Display_Channel_Parameters[index_COUNTTIME+ch*ncp]
//					fprintf filnum2, "\t\tInput Count Rate [cps] = %g\r", Display_Channel_Parameters[index_ICR+ch*ncp]
//					fprintf filnum2, "\t\tPeak Position = %g\r", MCAChannelPeakPos[ch]
//					fprintf filnum2, "\t\tResolution (FWHM) [%%] = %g\r", MCAChannelFWHMPercent[ch]
//					fprintf filnum2, "\t\tPeak Area = %g\r\r", MCAChannelPeakArea[ch]
//					
//					if(lastFWHM[ch] > MCAChannelFWHMPercent[ch])
//						lastFWHM[ch] = MCAChannelFWHMPercent[ch]
//						BestEFRT[ch] = EFRTlen[ch]
//						BestEFFT[ch] = EFFTlen[ch]
//					endif
//				endif
//			endfor
//			
//			for(ch=0; ch<NumberOfChannels; ch+=1)
//				if( (TSTbit(ch, doneRT) == 0) && (TSTbit(ch, doneFT) == 0) )
//					EFFTlen[ch] += 2^FilterRange/FilterClockMHz
//					if( (EFFTlen[ch] -  EFFTHigh[ch]) >= (2^FilterRange/(2*FilterClockMHz)) )
//						doneFT = SETBit(ch, doneFT)
//					endif
//				endif
//			endfor
//	
//		while( doneFT < (2^NumberOfChannels - 1) )
//		
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			if(TSTbit(ch, doneRT) == 0)
//				EFRTlen[ch] += 2^FilterRange/FilterClockMHz
//				if( (EFRTlen[ch] - EFRTHigh[ch]) > (2^FilterRange/(2*FilterClockMHz)) )
//					doneRT = SETBit(ch, doneRT)
//				endif
//			endif
//		endfor
//
//	while ( doneRT < (2^NumberOfChannels - 1) )
//	
//	// Restore the current filter time
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//
//		EnergyRiseTime=keepEFRT[ch]
//		Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_RISETIME")][ch][ChosenModule]=EnergyRiseTime
//		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, ch)
//		
//		EnergyFlatTop=keepEFFT[ch]
//		Channel_Parameter_Values[Pixie_Find_ChanParInWave("ENERGY_FLATTOP")][ch][ChosenModule]=EnergyFlatTop
//		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, ch)
//	endfor		
//
//	// Print file header for output file
//	fprintf filnum1, "\r\rAutomatic scanning of energy filter rise time and flat top finished at %s, %s\r\r",date(),time()
//	
//	fprintf filnum1, "\rThis automatic scanning program found the following best energy filter settings.\r"
//	fprintf filnum1, "-----------------------------------------------------------------------------------------------------------------\r"
//	fprintf filnum1, "Channel      Best Rise Time [µs]       Best Flat Top [µs]       Best FWHM[%]\r"	
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		if(TSTbit(ch, chEFScanMask) == 1)
//			fprintf filnum1, "\t%d", ch
//			fprintf filnum1, "%25.4f%25.4f%20.4f\r", BestEFRT[ch], BestEFFT[ch], lastFWHM[ch]
//			
//			// print out to Igor history window
//			printf "Ch#=%d\tBest RT=%8.4f\tBest FT=%8.4f\tBest FWHM=%8.4f\r",ch, BestEFRT[ch], BestEFFT[ch], lastFWHM[ch]
//		endif
//	endfor
//	
//	// Print file header for temp file
//	fprintf filnum2, "Automatic scanning of energy filter rise time and flat top finished at %s, %s\r\r",date(),time()
//
//	close(filnum1)
//	close(filnum2)
//	
//	Button DoScan,disable=0,win=AutoEFOptimizationPanel
//End
//
//
////########################################################################
////	Create auto Tau optimization panel
////########################################################################
//Function Pixie_Auto_TauOptimization()
//	
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar chTauScanMask = root:auto:chTauScanMask
//	
//	PauseUpdate; Silent 1		// building window...
//	DoWindow/F AutoTauOptimizationPanel
//	if (V_flag==1)
//		return(0)
//	endif
//		
//	NewPanel/K=1/W=(81.25,292.25,486.25,600) as "Auto Optimization of Decay Time"
//	ModifyPanel cbRGB=(51456,44032,58880)
//	DoWindow/C AutoTauOptimizationPanel
//	
//	SetDrawLayer UserBack
//	SetDrawEnv fsize= 14,fstyle= 1
//	DrawText 50,23,"Automatic scanning of Decay Time"
//	
//	SetDrawLayer UserBack
//	SetDrawEnv fsize= 11,fstyle= 1
//	DrawText 24,49,"Before starting scan, please confirm the following conditions:"
//
//	CheckBox TauCheckADCTraces,pos={40,55},size={229,14},title="ADC traces on the Oscilloscope are in range?"
//	CheckBox TauCheckADCTraces,value= 0,win=AutoTauOptimizationPanel
//	CheckBox TauCheckRunTime,pos={40,75},size={253,14},title="Run time and polling time have been set properly?"
//	CheckBox TauCheckRunTime,value= 0,win=AutoTauOptimizationPanel
//	CheckBox TauCheckMCAROI,pos={40,95},size={311,14},title="MCA Gauss fit range has been set properly for each channel?"
//	CheckBox TauCheckMCAROI,value= 0,win=AutoTauOptimizationPanel
//	CheckBox TauCheckScanLimits,pos={40,115},size={239,14},title="Auto scan limits have been set properly below for each channel?"
//	CheckBox TauCheckScanLimits,value= 0,win=AutoTauOptimizationPanel
//	
//	SetVariable SelectedPixie4Channel pos={280,160}, size={100,20},title="Channel"
//	SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=11, value=root:pixie4:ChosenChannel
//	SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
//	
//
//
//	Button DoTauScan,pos={290,190},size={75,28},proc=Pixie_Auto_ScanTau,title="Start Scan"
//	Button HelpOptimize_Tau, pos={290,225},size={75,28},proc=Pixie_CallHelp,title="Help"
//	Button CloseAutoTauScan,pos={290,260},size={75,28},proc=Pixie_AnyPanelClose,title="Close"
//	GroupBox TauLmitsGroup,pos={20,150},size={194,130},title="Decay time limits",fcolor=(1,1,1)
//	GroupBox TauLmitsGroup,fSize=12,fStyle=1
//	SetVariable TauLowerLimit,pos={65,175},size={124,18},title="Start [µs]",limits={0,inf,0}, proc=Pixie_Auto_SetTauLimits
//	SetVariable TauLowerLimit,font="Arial",format="%.03f",value= root:auto:chTauLow,bodyWidth= 75
//	SetVariable TauUpperLimit,pos={65,200},size={124,18},title="End [µs]",value= root:auto:chTauHigh,bodyWidth= 75
//	SetVariable TauUpperLimit,font="Arial",format="%.03f",limits={0, inf, 0}, proc=Pixie_Auto_SetTauLimits
//	SetVariable TauStepSize,pos={65,225},size={124,18},title="Step Size [µs]",value= root:auto:chTauStep,bodyWidth= 75
//	SetVariable TauStepSize,font="Arial",format="%.03f",limits={0,inf,0}, proc=Pixie_Auto_SetTauLimits
//	CheckBox DoScanThisChTau,pos={80,255},size={239,18},title="Scan this channel?", proc=Pixie_Auto_SetChScanBitMask
//	CheckBox DoScanThisChTau,value= TSTbit(ChosenChannel, chTauScanMask), win=AutoTauOptimizationPanel
//
//End
//
//
////########################################################################
////	Set scanning limits of Tau
////########################################################################
//Function Pixie_Auto_SetTauLimits(ctrlName,varNum,varStr,varName) : SetVariableControl
//String ctrlName
//Variable varNum
//String varStr
//String varName
//	
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar chTauLow = root:auto:chTauLow
//	Nvar chTauHigh = root:auto:chTauHigh
//	Nvar chTauStep = root:auto:chTauStep
//
//	wave TauLow = root:auto:TauLow
//	wave TauHigh = root:auto:TauHigh
//	wave TauStep = root:auto:TauStep
//	
//	Strswitch(ctrlName)
//
//		case "TauLowerLimit":
//			if(chTauLow > TauHigh[ChosenChannel])
//				chTauLow = TauHigh[ChosenChannel]
//				TauLow[ChosenChannel] = TauHigh[ChosenChannel]
//				DoAlert 0, "Tau Start can not be greater than Tau End."
//				return(-1)
//			endif
//				
//			TauLow[ChosenChannel] = chTauLow
//			break			
//	
//		case "TauUpperLimit":
//			if(chTauHigh < TauLow[ChosenChannel])
//				chTauHigh = TauLow[ChosenChannel]
//				TauHigh[ChosenChannel] = TauLow[ChosenChannel]
//				DoAlert 0, "Tau End can not be smaller than Tau Start."
//				return(-2)
//			endif
//				
//			TauHigh[ChosenChannel] = chTauHigh
//			break			
//		
//		case "TauStepSize":
//			TauStep[ChosenChannel] = chTauStep
//			break
//					
//	EndSwitch
//End
//
//
////########################################################################
////	Start scanning of Tau
////########################################################################
//Function Pixie_Auto_ScanTau(ctrlName) : ButtonControl
//	String ctrlName
//
//	Nvar RunTimeOut = root:pixie4:RunTimeOut
//	Nvar PollingTime = root:pixie4:PollingTime
//	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
//	Nvar ChosenModule = root:pixie4:ChosenModule
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
//	Nvar index_RunTime = root:pixie4:index_RunTime
//	Nvar index_EvRate = root:pixie4:index_EvRate
//
//	Nvar chTauLow = root:auto:chTauLow
//	Nvar chTauHigh = root:auto:chTauHigh
//	Nvar chTauStep = root:auto:chTauStep
//	Nvar chTauScanMask = root:auto:chTauScanMask
//	
//	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
//	Wave mcawave = root:pixie4:mcawave
//	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
//	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
//	Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea
//	Wave TauLow = root:auto:TauLow
//	Wave TauHigh = root:auto:TauHigh
//	Wave TauStep = root:auto:TauStep
//	Wave keepTau = root:auto:keepTau
//	Wave BestTau = root:auto:BestTau
//	Wave lastFWHM = root:auto:lastFWHM
//	
//	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
//	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
//	Nvar index_ICR = root:pixie4:index_ICR
//	Nvar ncp = root:pixie4:NumChannelPar
//		
//	Variable ch, tmp, controlvalue
//	Variable StartTime, TimeDiff, filnum1, filnum2, Tau
//	Variable doneTau
//	
//	String tmpfilnam
//
//	Controlinfo/w=AutoTauOptimizationPanel TauCheckADCTraces
//	controlvalue = V_Value
//	Controlinfo/w=AutoTauOptimizationPanel TauCheckRunTime
//	controlvalue += V_Value
//	Controlinfo/w=AutoTauOptimizationPanel TauCheckMCAROI
//	controlvalue += V_Value
//	Controlinfo/w=AutoTauOptimizationPanel TauCheckScanLimits
//	controlvalue += V_Value
//	
//	if(controlvalue != 4)
//		DoAlert 0, "Please confirm all conditions first."
//		return(0)
//	endif
//
//	if(chTauScanMask == 0)
//		DoAlert 0, "Please select 'Scan this channel?' for at least one channel."
//		return(0)
//	endif
//	
//	// File dialog to open output file	
//	open/T="TEXT"/M="Create a text file to store scanning results" filnum1
//	if(filnum1 == 0)
//		DoAlert 0,"Failed to open output file"
//		return(-1)
//	endif
//	// Temp file uses the same file name as the output file but with extension .tmp
//	tmpfilnam = S_filename + ".tmp"
//	open filnum2 as tmpfilnam
//	if(filnum2 == 0)
//		DoAlert 0,"Failed to open temp file"
//		return(-1)
//	endif	
//	
//	Button DoTauScan,disable=2,win=AutoTauOptimizationPanel
//	
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//
//		// Store the current Tau
//		keepTau[ch]=Channel_Parameter_Values[Pixie_Find_ChanParInWave("TAU")][ch][ChosenModule]
//		
//		// Initialize lastFWHM
//		lastFWHM[ch]=100
//		
//	endfor		
//
//	// Print file header for output file
//	fprintf filnum1, "Automatic scanning of decay time started at %s, %s\r\r",date(),time()
//	fprintf filnum1,"Ch.#        Tau [µs]      FWHM [%%]\r"
//	
//	// Print file header for temp file
//	fprintf filnum2, "Automatic scanning of decay time started at %s, %s\r\r",date(),time()
//
//	// Variables used to store tau values locally
//	make/o/n=(NumberOfChannels) root:auto:TauValue
//	wave TauValue = root:auto:TauValue
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		TauValue[ch] = TauLow[ch]  // Initialize tau
//	endfor
//
//	// Determine if a channel needs to be scanned
//	doneTau = 0
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		if(TSTbit(ch, chTauScanMask) == 0)
//			doneTau = SETBit(ch, doneTau)
//		endif		
//	endfor
//	
//	make/o/u/i/n=1 dummy
//	// Scan all possible tau values
//	do
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			if(TSTbit(ch, doneTau) == 0)
//				// Set Tau
//				Tau = TauValue[ch]
//				Channel_Parameter_Values[Pixie_Find_ChanParInWave("TAU")][ch][ChosenModule]=Tau
//				Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, ch)
//			endif
//		endfor
//			
//		
//		Pixie4_Acquire_Data(0x1301, dummy, "", ChosenModule)  // Start a new MCA run
//			
//		StartTime=DateTime
//		do
//			Sleep/T 60
//			TimeDiff=DateTime - StartTime
//		while( TimeDiff < RunTimeOut )
//		
//		// stop the MCA run
//		Pixie4_Acquire_Data(0x3301, root:pixie4:mcawave, "", ChosenModule)
//		// read out the MCA spectra
//		//Pixie4_Acquire_Data(0x9001, root:pixie4:mcawave, "", ChosenModule)
//		//mcawave[0] = mcawave[0]		// make sure IGOR updates the displays
//		//Pixie_StopRun("")
//		Pixie_RC_UpdateMCAData("module")
//
//		// Gauss fit
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			if( TSTbit(ch, doneTau) == 0 )
//				Pixie_Math_GaussFit("GaussFitMCA", ch+1, "")
//
//				// print out tau values and FWHM to output file
//				fprintf filnum1,"%d %14.2f %14.4f\r",ch, TauValue[ch], MCAChannelFWHMPercent[ch]
//				
//				// print out to Igor history window
//				printf "Ch#=%d\tTau=%14.2f\tFWHM=%8.4f\r",ch, TauValue[ch], MCAChannelFWHMPercent[ch]
//			endif
//		endfor	
//					
//		// print out all relevant infomation to temp file
//		fprintf filnum2, "\tRun Time [s]= %g\r", Display_Module_Parameters[index_RunTime]
//		fprintf filnum2, "\tEvent Rate [cps]= %g\r\r", Display_Module_Parameters[index_EvRate]
//
//		
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			if( (TSTbit(ch, doneTau) == 0) )
//				fprintf filnum2, "\tChannel %g\r", ch
//				fprintf filnum2, "\tTau [µs]= %g\r", TauValue[ch]
//				fprintf filnum2, "\t\tCount Time [s]= %g\r", Display_Channel_Parameters[index_COUNTTIME+ch*ncp]
//				fprintf filnum2, "\t\tInput Count Rate [cps] = %g\r", Display_Channel_Parameters[index_ICR+ch*ncp]
//				fprintf filnum2, "\t\tPeak Position = %g\r", MCAChannelPeakPos[ch]
//				fprintf filnum2, "\t\tResolution (FWHM) [%%] = %g\r", MCAChannelFWHMPercent[ch]
//				fprintf filnum2, "\t\tPeak Area = %g\r\r", MCAChannelPeakArea[ch]
//					
//				if(lastFWHM[ch] > MCAChannelFWHMPercent[ch])
//					lastFWHM[ch] = MCAChannelFWHMPercent[ch]
//					BestTau[ch] = TauValue[ch]
//				endif
//			endif
//		endfor
//			
//		for(ch=0; ch<NumberOfChannels; ch+=1)
//			if( (TSTbit(ch, doneTau) == 0) )
//				TauValue[ch] += TauStep[ch]
//				if( TauValue[ch] > TauHigh[ch] )
//					doneTau = SETBit(ch, doneTau)
//				endif
//			endif
//		endfor
//	
//	while( doneTau < (2^NumberOfChannels - 1) )
//	
//	// Restore the current filter time
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		Tau=keepTau[ch]
//		Channel_Parameter_Values[Pixie_Find_ChanParInWave("TAU")][ch][ChosenModule]=Tau
//		Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, ch)
//	endfor		
//
//	// Print file header for output file
//	fprintf filnum1, "\r\rAutomatic scanning of decay time finished at %s, %s\r\r",date(),time()
//	
//	fprintf filnum1, "\rThis automatic scanning program found the following best decay times.\r"
//	fprintf filnum1, "-----------------------------------------------------------------------------------------------------------------\r"
//	fprintf filnum1, "Channel      Best Decay Time [µs]       Best FWHM[%]\r"	
//	for(ch=0; ch<NumberOfChannels; ch+=1)
//		if(TSTbit(ch, chTauScanMask) == 1)
//			fprintf filnum1, "\t%d", ch
//			fprintf filnum1, "%25.4f%20.4f\r", BestTau[ch], lastFWHM[ch]
//			
//			// print out to Igor history window
//			printf "Ch#=%d\tBest Tau=%8.4f\tBest FWHM=%8.4f\r",ch, BestTau[ch], lastFWHM[ch]
//		endif
//	endfor
//	
//	// Print file header for temp file
//	fprintf filnum2, "Automatic scanning of decay time finished at %s, %s\r\r",date(),time()
//
//	close(filnum1)
//	close(filnum2)
//	
//	Button DoTauScan,disable=0,win=AutoTauOptimizationPanel
//End
//
//
////########################################################################
////	Set the bit mask for determining whether a channel will be scanned
////########################################################################
//Function Pixie_Auto_SetChScanBitMask(ctrlName, checked) : CheckBoxControl
//	String ctrlName
//	Variable checked
//
//	Nvar ChosenChannel = root:pixie4:ChosenChannel
//	Nvar chTauScanMask = root:auto:chTauScanMask
//	Nvar chEFScanMask = root:auto:chEFScanMask
//	
//	StrSwitch(ctrlName)
//		Case "DoScanThisChTau":
//			if(checked)
//				chTauScanMask = SETBit(ChosenChannel, chTauScanMask)
//			else
//				chTauScanMask = CLRBit(ChosenChannel, chTauScanMask)
//			endif
//					
//			break
//			
//		Case "DoScanThisChEF":
//			if(checked)
//				chEFScanMask = SETBit(ChosenChannel, chEFScanMask)
//			else
//				chEFScanMask = CLRBit(ChosenChannel, chEFScanMask)
//			endif
//		
//			break
//	EndSwitch
//
//End


//########################################################################
//	Functions to scan through parameter values
//########################################################################

Function Pixie_Scan_InitGlobals()

	Variable/G root:auto:startSL 
	Variable/G root:auto:endSL
	Variable/G root:auto:dSL 
	
	Variable/G root:auto:startSG 
	Variable/G root:auto:endSG
	Variable/G root:auto:dSG
	
	Variable/G root:auto:startTau 
	Variable/G root:auto:endTau
	Variable/G root:auto:dTau
	
	Variable/G root:auto:MaxSGL
	
	Variable/G root:auto:von
	Variable/G root:auto:bis
	String/G root:auto:basename
	
	Variable/G root:auto:startTau
	Variable/G root:auto:endTau
	Variable/G root:auto:dTau
	
	Variable/G root:auto:chan0
	Variable/G root:auto:chan1
	Variable/G root:auto:chan2
	Variable/G root:auto:chan3
	
	make/o/n=(1) root:results:tau0, root:results:peak0, root:results:Tgap0, root:results:Tpeak0, root:results:FWHM0
	make/o/n=(1) root:results:tau1, root:results:peak1, root:results:Tgap1, root:results:Tpeak1, root:results:FWHM1
	make/o/n=(1) root:results:tau2, root:results:peak2, root:results:Tgap2, root:results:Tpeak2, root:results:FWHM2
	make/o/n=(1) root:results:tau3, root:results:peak3, root:results:Tgap3, root:results:Tpeak3, root:results:FWHM3
	make/o/n=(1) root:results:runnumber


End


Function Pixie_Scan_ControlPanel() : Panel
	DoWindow/F ScanControlPanel
	if (V_flag==1)
		return 0
	endif
	NewPanel /W=(700,25,1080,275) /K=1
	DoWindow/C ScanControlPanel
	DoWindow/T ScanControlPanel,"File Series Scan"
	ModifyPanel cbRGB=(51456,44032,58880)
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_FilterRange =  root:pixie4:index_FilterRange
	
	variable xx = 30
	variable dy=20
	
	GroupBox Filter title="Filter limits", pos={xx-15,10},size={170,190},frame=1,fsize=12,fcolor=(1,1,1),fstyle=1
	
	SetVariable EGX_FilterRange,pos={xx,10+dy},size={140,19},proc=Pixie_IO_ModVarControl
	SetVariable EGX_FilterRange,help={"Set filter range to get different energy filter rise time or flat top limits"}
	SetVariable EGX_FilterRange,format="%d",fsize=11,title="Filter Range ",value=  Display_Module_Parameters[index_FilterRange]
	SetVariable EGX_FilterRange,limits={1,6,1}, bodywidth = 70
		
	SetVariable SLstart,pos={xx,40+dy},size={140,18},title="Start Trise [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SLstart,value= root:auto:startSL, fsize=11, format="%.3f",bodywidth = 70
	SetVariable SLend,pos={xx,60+dy},size={140,18},title="End Trise [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SLend,value= root:auto:endSL, fsize=11, format="%.3f",bodywidth = 70
	SetVariable SLd,pos={xx,80+dy},size={140,18},title="Step size [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SLd,value= root:auto:dSL, fsize=11, format="%.3f",bodywidth = 70
	
	SetVariable SGstart,pos={xx,110+dy},size={140,18},title="Start Tflat [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SGstart,value= root:auto:startSG, fsize=11, format="%.3f",bodywidth = 70
	SetVariable SGend,pos={xx,130+dy},size={140,18},title="End Tflat [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SGend,value= root:auto:endSG, fsize=11, format="%.3f",bodywidth = 70
	SetVariable SGd,pos={xx,150+dy},size={140,18},title="Step size [us] ", proc=Pixie_Scan_SetVariable 
	SetVariable SGd,value= root:auto:dSG, fsize=11, format="%.3f",bodywidth = 70
	
	GroupBox Tau title="Tau limits", pos={xx-15+180,10},size={160,122},frame=1,fsize=12,fcolor=(1,1,1),fstyle=1
		
	SetVariable Taustart,pos={xx+180,40+dy},size={130,18},title="Start Tau [us] "
	SetVariable Taustart,value= root:auto:startTau, fsize=11, format="%.3f",bodywidth = 65
	SetVariable Tauend,pos={xx+180,60+dy},size={130,18},title="End Tau [us] " 
	SetVariable Tauend,value= root:auto:endTau, fsize=11, format="%.3f",bodywidth = 65
	SetVariable Taud,pos={xx+180,80+dy},size={130,18},title="Step size [us] "
	SetVariable Taud,value= root:auto:dTau, fsize=11, format="%.3f",bodywidth = 65
	
	Button SetScanToStart pos = {200,dy+122}, title = "Set Parameters to Start", size = {150,22}, proc = Pixie_Ctrl_CommonButton
	Button SetScanToStart, help={"Set filter settings and tau to 'start' limit, unless step size = 0"}, fsize=11
	Button SetScanRunConditions pos = {200,dy+152}, title = "Set Scan Run Conditions", size = {150,22}, proc = Pixie_Ctrl_CommonButton
	Button SetScanRunConditions, help={"Set 'Data Record' options, Run Time, Run Type as required for file series run"}, fsize=11
	
	dy = 85
	Button StartScanRun, pos = {20,dy+125}, title = "Start Scan", size = {75,30}, proc = Pixie_RC_StartRun
	Button OpenSeriesPlot, pos = {110,dy+125}, title ="File Series", size = {75,30}, proc = Pixie_Ctrl_CommonButton
	Button HelpFile_Series_Scan, pos = {200,dy+125}, title = "Help", size = {75,30}, proc = Pixie_CallHelp	
	Button CloseScan_Settings, pos = {290,dy+125}, title = "Close", size = {75,30}, proc = Pixie_AnyPanelClose	
End

Function Pixie_Scan_StartParameters()
	
	Nvar startSL  = root:auto:startSL
	Nvar startSG  = root:auto:startSG
	Nvar startTau  = root:auto:startTau
	
	Nvar dSL  = root:auto:dSL
	Nvar dSG  = root:auto:dSG
	Nvar dTau  = root:auto:dTau
				
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Nvar index_SL = root:pixie4:index_SL
	Nvar index_SG = root:pixie4:index_SG
	Nvar index_TAU = root:pixie4:index_TAU
	Nvar index_ActCW = root:pixie4:index_ActCW
	Nvar ChosenModule = root:pixie4:ChosenModule
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	
	variable m
	string name1,name2, name3
	
	for(m=0;m<4;m+=1)			
		name1 = "EGY_ENERGY_RISETIME"+num2str(m)
		name2 = "EGY_ENERGY_FLATTOP"+num2str(m)
		name3 = "EGY_TAU"+num2str(m)				

	if(dSL>0)
		Channel_Parameter_Values[index_SL][m][ChosenModule] = startSL
		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, m)
		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, m)
	endif
	
	if(dSG>0)
		Channel_Parameter_Values[index_SG][m][ChosenModule] = startSG
		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, m)
		Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, m)
	endif
	
	if(dTau>0)
		Channel_Parameter_Values[index_TAU][m][ChosenModule] = startTau
		Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, m)
		Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, m)	
	endif	
	
	endfor
	
	// update display variables
	Pixie_IO_ReadChanParFromMod(ChosenModule)
	Pixie_IO_SendChanParToIgor()
	
	// force coincwait back to minimum, it may have increased while channels had different filter times
	Nvar SystemClockMHz = root:pixie4:SystemClockMHz
	Display_Module_Parameters[index_ActCW] = round(1000/SystemClockMHz)
	Pixie_IO_ModVarControl("COI_ActualCoincWait",0,"","")
End


Function Pixie_Scan_RunConditions()
	Nvar AutoRunNumber = root:pixie4:AutoRunNumber
	Nvar AutoStoreSpec = root:pixie4:AutoStoreSpec
	Nvar AutoStoreSettings = root:pixie4:AutoStoreSettings
	Nvar AutoStoreStatistics = root:pixie4:AutoStoreStatistics
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar TNewFile = root:pixie4:TNewFile
	Nvar MCArunListNumber = root:Pixie4:MCArunListNumber
	
	Nvar dSL  = root:auto:dSL
	Nvar startSL  = root:auto:startSL
	Nvar endSL  = root:auto:endSL
	Nvar dSG  = root:auto:dSG
	Nvar startSG  = root:auto:startSG
	Nvar endSG  = root:auto:endSG
	Nvar dTau  = root:auto:dTau
	Nvar startTau  = root:auto:startTau
	Nvar endTau  = root:auto:endTau
	
	// set up Record options
	AutoRunNumber = 1
	AutoStoreSpec = 1
	AutoStoreSettings =1
	AutoStoreStatistics =1
	
	//Set up runtime
	Variable Nsl, Nsg, Ntau
	
	if(dSL>0)
		Nsl =  abs(startSL - endSL)/dSL	// number of steps in SL
	else
		Nsl = 1
	endif
	if(dSG>0)
		Nsg =  abs(startSG - endSG)/dSG	// number of steps in SG
	else
		Nsg = 1				
	endif
	if(dTau>0)
		Ntau =  abs(startTau - endTau)/dTau	// number of steps in tau
	else
		Ntau = 1
	endif
	RunTimeOut = TNewFile * Nsl * Nsg * Ntau
	
	// Set up runtype
	Pixie_Ctrl_CommonPopup("RUN_WhichRun",MCArunListNumber,"") 
	DoWindow/F Pixie_Parameter_Setup
	if(V_flag==1)
		PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie_Parameter_Setup
	endif
	DoWindow/F Pixie4MainPanel
	if(V_flag==1)
		PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie4MainPanel
	endif
	
			
End			

Function Pixie_Scan_SetVariable(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_FilterRange =  root:pixie4:index_FilterRange
	Nvar FilterClockMHz =  root:pixie4:FilterClockMHz
	Nvar EFMinRiseTimeTicks = root:pixie4:EFMinRiseTimeTicks
	Nvar EFMaxRiseTimeTicks = root:pixie4:EFMaxRiseTimeTicks
	Nvar EFMinFlatTopTicks = root:pixie4:EFMinFlatTopTicks
	Nvar EFMaxFlatTopTicks = root:pixie4:EFMaxFlatTopTicks

	
	Nvar MaxSGL  = root:auto:MaxSGL
	
	Nvar dSL  = root:auto:dSL
	Nvar startSL  = root:auto:startSL
	Nvar endSL  = root:auto:endSL
	
	Nvar dSG  = root:auto:dSG
	Nvar startSG  = root:auto:startSG
	Nvar endSG  = root:auto:endSG
	
	Variable FR  = Display_Module_Parameters[index_FilterRange]
	variable dt = 2^FR/FilterClockMHz
	
	MaxSGL = 127* dt
	
	if(dSL!=0)
		dSL = max(dt, dSL)
		dSL = round(dSL/dt) *dt
	endif
	if(dSG!=0)
		dSG = max(dt, dSG)
		dSG = round(dSG/dt) *dt
	endif
	
	
	startSL = round(startSL/dt) *dt
	startSL = max(EFMinRiseTimeTicks* dt, startSL)
	startSL = min(EFMaxRiseTimeTicks, startSL)
	
	startSG = round(startSG/dt) *dt
	startSG = max(EFMinFlatTopTicks* dt, startSG)
	startSG = min(EFMaxFlatTopTicks*dt, startSG)
	
	endSL = round(endSL/dt) *dt
	endSL = max(EFMinRiseTimeTicks* dt, endSL)
	endSL = min(EFMaxRiseTimeTicks* dt, endSL)
	
	endSG = round(endSG/dt) *dt
	endSG = max(EFMinFlatTopTicks* dt, endSG)
	endSG = min(EFMaxFlatTopTicks* dt, endSG)
End


//########################################################################
//
//	Pixie_Scan_CheckRun:
//		Check the run status in auto scans of parameters (MCA only).      
//
//########################################################################
Function Pixie_Scan_CheckRun(s)		// This is the function that will be called periodically
	STRUCT WMBackgroundStruct &s

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar RunActive = root:pixie4:RunActive
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar AutoUpdateMCA= root:pixie4:AutoUpdateMCA
	Nvar Tupdate = root:pixie4:Tupdate
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	Nvar TNewFile = root:pixie4:TNewFile
	Nvar PanelTabNumber = root:pixie4:PanelTabNumber
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar OutputFileName = root:pixie4:OutputFileName
	Wave RunTasks = root:pixie4:RunTasks
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave dummy = root:pixie4:dummy
	Nvar StartTime_s = root:pixie4:StartTime_s
	
	// local variables
	Variable RunType,ret,direction,channum,i,index0,index1, updated, saved, CSR, DataReady, stoppedrun, m, elapsed
		
	updated=0
	saved=0
	stoppedrun=0
	
	// poll module	
	RunType = 0x40FF	
	CSR = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, ChosenModule)	//polling
	RunActive = ((CSR & 0x2000) >0)
	DataReady =  ((CSR & 0x4000) >0)
	
	if(CSR < 0)	// failed to poll
		DoAlert 0, "Failed to check run status due to invalid module number."
		ret = Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
		if(ret < 0)  // failed to stop the run
			DoAlert 0, "Failed to stop the run. Please reboot the Pixie modules."
		endif
					
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(RunActive)
	endif
	
	
	// check for run finished or data ready
	elapsed = DateTime - StartTime_s
	RunTimeOut = KeepRunTimeOut - elapsed
	//RunTimeOut -= PollingTime
	//SpillTimeOut -= PollingTime
		
	if( (SpillTimeOut<=0) || (RunActive==0) || (RunTimeOut<=0) )	
		stoppedrun =1
		RepCount -= 1
		if (RunTimeOut<=0)	// overall run time over: no more spills
			RepCount = 0
		endif
	
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		Pixie_RC_StopRun(" repeat_scan")  
	endif
	
	
	// 2b. Run still in progress (MCA mode only)
	// when run active, update spectrum occasionally
	if( (AutoUpdateMCA==1)  & (updated==0) )
	 	if( ( mod(KeepRunTimeOut-RunTimeOut,Tupdate)>=Tupdate-PollingTime)  & (RunTimeOut>Tupdate)  )
	 		Pixie_RC_UpdateMCAData("UpdateMCA")
		endif
	endif
	
//	if(AutoNewFile==1) 	// in scan, always autonewfile
	if(1)
		 if( ( mod(KeepRunTimeOut-RunTimeOut,TNewFile)>=TNewFile-PollingTime)  & (RunTimeOut>TNewFile)  )
		 	Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
			Pixie_RC_AutoSaveDAQFiles()
			
			
			///////////////////////////////////////////////////////////// ///////////////////////////////////////////////////////
			// 	Modify acquisition parameters			      //
			Nvar MaxSGL  = root:auto:MaxSGL
			
			Nvar dSL  = root:auto:dSL
			Nvar startSL  = root:auto:startSL
			Nvar endSL  = root:auto:endSL
			
			Nvar dSG  = root:auto:dSG
			Nvar startSG  = root:auto:startSG
			Nvar endSG  = root:auto:endSG
			
			Nvar dTau  = root:auto:dTau
			Nvar startTau  = root:auto:startTau
			Nvar endTau  = root:auto:endTau
			
			// old variables
			//Nvar EnergyRiseTime = root:pixie4:EnergyRiseTime
			//Nvar EnergyFlatTop = root:pixie4:EnergyFlatTop
			//Nvar Tau = root:pixie4:Tau
			//Nvar ActualCoincWait = root:pixie4:ActualCoincWait
			
			Nvar ChosenChannel = root:pixie4:ChosenChannel
						
			Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
			Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
			Nvar index_SL = root:pixie4:index_SL
			Nvar index_SG = root:pixie4:index_SG
			Nvar index_TAU = root:pixie4:index_TAU
			Nvar ncp =  root:pixie4:NumChannelPar	
			Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
			Nvar index_ActCW = root:pixie4:index_ActCW
			
			Variable EnergyRiseTime 
			Variable EnergyFlatTop
			Variable Tau
			variable inc
			string name1,name2, name3
			
			for(m=0;m<4;m+=1)
				//ChosenChannel = m
				//Pixie_IO_SelectModChan("",0,"","")
				
				name1 = "EGY_ENERGY_RISETIME"+num2str(m)
				name2 = "EGY_ENERGY_FLATTOP"+num2str(m)
				name3 = "EGY_TAU"+num2str(m)
					
				EnergyRiseTime = Display_Channel_Parameters[index_SL+ncp*m]
				EnergyFlatTop = Display_Channel_Parameters[index_SG+ncp*m]
				Tau = Display_Channel_Parameters[index_TAU+ncp*m]
				
				inc=0
				
		
				if(dSL>0)
					EnergyRiseTime += dSL	
					if( (EnergyRiseTime > endSL)   |   ((EnergyRiseTime + EnergyFlatTop) > MaxSGL)  )
						EnergyRiseTime = startSL
						inc=1
					endif
				//	Display_Channel_Parameters[index_SL+ncp*ChosenChannel] = EnergyRiseTime
				//	Pixie_IO_ChanVarControl(name1,0,"","")
				//	Pixie_IO_ChanVarControl(name1,0,"","")
				// 	directly modify I/O wave to save time with updates etc
					Channel_Parameter_Values[index_SL][m][ChosenModule] = EnergyRiseTime
					Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, m)
					Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_RISETIME", "CHANNEL", 0, ChosenModule, m)
				else 	
					inc=1
				endif
				
		
				if (dSG>0) 
					if (inc==1) 
						EnergyFlatTop += dSG
						if(  (EnergyFlatTop > endSG)  |   ((EnergyRiseTime + EnergyFlatTop) > MaxSGL)  )
							EnergyFlatTop = startSG
						else 
							inc = 0
						endif	
				//		Display_Channel_Parameters[index_SG+ncp*m] = EnergyFlatTop
				//		Pixie_IO_ChanVarControl(name2,0,"","")
				//		Pixie_IO_ChanVarControl(name2,0,"","")
						Channel_Parameter_Values[index_SG][m][ChosenModule] = EnergyFlatTop
						Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, m)
						Pixie4_User_Par_IO(Channel_Parameter_Values,"ENERGY_FLATTOP", "CHANNEL", 0, ChosenModule, m)
					endif
				endif
			
				if( dTau>0) 
					if (inc==1) 
						Tau += dTau
						if(  (Tau > endTau)   )
							Tau = startTau
						endif
				//		Display_Channel_Parameters[index_TAU+ncp*m] = Tau
				//		Pixie_IO_ChanVarControl(name3,0,"","")
				//		Pixie_IO_ChanVarControl(name3,0,"","")
						Channel_Parameter_Values[index_TAU][m][ChosenModule] = Tau
						Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, m)
						Pixie4_User_Par_IO(Channel_Parameter_Values,"TAU", "CHANNEL", 0, ChosenModule, m)
					endif
				endif
				
			endfor
			
			// update display variables
			Pixie_IO_ReadChanParFromMod(ChosenModule)
			Pixie_IO_SendChanParToIgor()
			
			// force coincwait back to minimum, it may have increased while channels had different filter times
			Nvar SystemClockMHz = root:pixie4:SystemClockMHz
			Display_Module_Parameters[index_ActCW] = round(1000/SystemClockMHz)
			Pixie_IO_ModVarControl("COI_ActualCoincWait",0,"","")
			//ActualCoincWait = 13.3
			//Pixie_Ctrl_CommonSetVariable("ActualCoincWait",0,"","")
							
			////////////////////////////////////////////////////////////	////////////////////////////////////////////////////
							
			RunType = RunTasks[WhichRun-1]+0x1000 //New Run after changing file names
			Pixie_RC_MakeRunFiles(RunType)
			Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
		endif
	endif

	
	return(0)		// IGOR 5 needs a return value for Background task to work
			
End


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// P500e ADC calibration -- user accessible
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


Function Pixie_I2E_ResetADC()
// reset the ADC to start over with new I2E initial calibration

	Nvar ChosenModule = root:pixie4:ChosenModule
	make/o/n=4 data
	Wave data
	
	// start control task for DSp to reset ADC and wait until finished *******************
	Variable runtype = 35
	make/u/i/o/n=1 dummy
	pixie4_Acquire_Data(runtype, buffer, "",ChosenModule)	
	do
		Sleep/t 5
	while (pixie4_Acquire_Data(0x4000, dummy, "",ChosenModule))
	
	Sleep/t 60	// ADC reset may take up to 550ms to recalibrate
	
	// re-initialize I2E ***************************************************************
	
	// disable I2E 
	data = 0x1C
	Pixie_I2E_ADCSPI(0x31, data, 0)	
		
	// freeze I2E 
	data = 0x22
	Pixie_I2E_ADCSPI(0x31, data, 0)		// write back to ADC
	
	// enable I2E (keep frozen)
	data = 0x23
	Pixie_I2E_ADCSPI(0x31, data, 0)		// write back to ADC
	

	DoAlert 0, "ADC was reset. Please re-run ADC calibration."

	
End

Function Pixie_I2E_CalibrateADC()

	// ********** initialize I2E data arrays	 ********** 
	//
	Make/o/n=48 root:auto:I2Edata
	Make/o/n=48/t root:auto:I2Edatanames
	Wave I2Edata =  root:auto:I2Edata
	Wave/t I2Edatanames =  root:auto:I2Edatanames
	make/o/n=(4) root:auto:I2Epolarity 
	Wave I2Epolarity = root:auto:I2Epolarity 
	//I2Epolarity = {-1,1,-1,-1}		// for P4e-14/500, Fippi 0x409 early compiles
	I2Epolarity = {1,1,1,1}		// for P500e

	I2Edatanames[0] = {"offset_coarse00","offset_fine00","gain_coarse00","gain_medium00","gain_fine00","skew_diff0"}  //final 2 numbers are ch and core # 
	I2Edatanames[6] = {"offset_coarse01","offset_fine01","gain_coarse01","gain_medium01","gain_fine01",""}
	I2Edatanames[12] = {"offset_coarse10","offset_fine10","gain_coarse10","gain_medium10","gain_fine10","skew_diff1"}  //final 2 numbers are ch and core # 
	I2Edatanames[18] = {"offset_coarse11","offset_fine11","gain_coarse11","gain_medium11","gain_fine11",""}
	I2Edatanames[24] = {"offset_coarse20","offset_fine20","gain_coarse20","gain_medium20","gain_fine20","skew_diff2"}  //final 2 numbers are ch and core # 
	I2Edatanames[30] = {"offset_coarse21","offset_fine21","gain_coarse21","gain_medium21","gain_fine21",""}
	I2Edatanames[36] = {"offset_coarse30","offset_fine30","gain_coarse30","gain_medium30","gain_fine30","skew_diff3"}  //final 2 numbers are ch and core # 
	I2Edatanames[42] = {"offset_coarse31","offset_fine31","gain_coarse31","gain_medium31","gain_fine31",""}	
	
//	DoWindow/F I2Etable
//	if (V_flag ==0)
//		Edit/K=1/W=(753.75,47,947.25,695) I2Edatanames,I2Edata
//		ModifyTable format(Point)=1,width(Point)=42,width(I2Edata)=56
//		DoWindow/C I2Etable
//	endif
	
	// ********** initialize Module settings ********** 
	//

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar ncp = root:pixie4:NumChannelPar
	Nvar index_OFFSET = root:pixie4:index_OFFSET
	Nvar index_BASEPC = root:pixie4:index_BASEPC
	Nvar index_GAIN = root:pixie4:index_GAIN
	Nvar  index_XDT  =  root:pixie4:index_XDT 
	
	Variable m, ch
	String name
	
	make/o/n=4 tempgain, tempbasepc, tempxdt, temppol
	
	for(m=0;m<4;m+=1)
		// set gain
		tempgain[m] = Display_Channel_Parameters[index_GAIN+m*ncp]
		Display_Channel_Parameters[index_GAIN+m*ncp] = 1
		name = "ADC_VGAIN"+num2str(m) 
		Pixie_IO_ChanVarControl(name,0,"","")
		
		// set polarity
		Nvar  ChanCSRAbit05 =  $("root:pixie4:Chan"+num2str(m)+"CSRAbit05")
		temppol[m] = ChanCSRAbit05
		ChanCSRAbit05 = 1
		name = "ADC_CCSRA05_"+num2str(m) 
		Pixie_IO_CheckBoxControl(name,ChanCSRAbit05)
		
//		// set initial offset to 49%
//		tempbasepc[m] = Display_Channel_Parameters[ index_BASEPC+m*ncp]
//		Display_Channel_Parameters[ index_BASEPC+m*ncp] = 49
//		name = "ADC_BASELINE_PERCENT"+num2str(m)
//		Pixie_IO_ChanVarControl(name,0,"","")
//				
//		// set dt to 0.067us (minimum)
//		tempxdt[m] = Display_Channel_Parameters[ index_XDT+m*ncp] 
//		Display_Channel_Parameters[ index_XDT+m*ncp] = 0
//		name = "ADC_XDT"+num2str(m)
//		Pixie_IO_ChanVarControl(name,0,"","")
	endfor

//	Pixie_Ctrl_CommonButton("AdjustDC")	// adjust offsets
//	Pixie_Ctrl_CommonButton("ADCRefresh")	// refresh

	
	// ********** run calibration test **********
	//
	
	wave slopes = root:auto:slopes		// waves containing offset and slope of ADC mismatch
	wave offsets = root:auto:offsets
	Variable co = 4.24	// offset correction factor
	Variable cs = -0.13	// slope/gain correction factor
	variable dbg
	
	Variable mis, minmis	// mismatch
	mis = 10000
	minmis = 10000

	// read calibration values
	Pixie_I2E_ReadCalibFromADC()
	DoUpdate
	
	mis = Pixie_I2E_caltest()

	//for(m=0;m<1;m+=1)
	for(m=0;m<4;m+=1)
		// run test and analysis routines
		//mis = Pixie_I2E_caltest()
			
		// read calibration values
		//Pixie_I2E_ReadCalibFromADC()
		//DoUpdate
		
		//if( (mis<minmis) | (mis<2) )
		if( (mis>2) )
			minmis = 2*mis
			// make changes
			// fine adjustment is only 50-80% of the theoretical required change, to allow slow creeping up to optimum, not overshooting
			for(ch=0;ch<4;ch+=1)
				if(abs(offsets[ch]/co) >1)
					//print "old coarse offset ch",ch,I2Edata[6+ch*12]
					I2Edata[6+ch*12] = floor(I2Edata[6+ch*12]+ offsets[ch]/co*I2Epolarity[ch])		// adjust coarse offset
					I2Edata[6+ch*12] = min(I2Edata[6+ch*12], 255)
					I2Edata[6+ch*12] = max(I2Edata[6+ch*12], 0)
				else
					//print "old fine offset ch",ch,I2Edata[7+ch*12]
					I2Edata[7+ch*12] = floor(I2Edata[7+ch*12]+ offsets[ch]/co*20*I2Epolarity[ch])	// adjust fine offset
					if (I2Edata[7+ch*12] <=0)									// if out of range, adjust coarse instead
						I2Edata[7+ch*12] = 50
						I2Edata[6+ch*12] -= 2
					endif	
					if (I2Edata[7+ch*12] >=255)
						I2Edata[7+ch*12] = 205
						I2Edata[6+ch*12] += 2
					endif	
				endif
				
				if(abs(slopes[ch]/cs) >1)
					//print "old medium gain ch",ch,I2Edata[9+ch*12]
					I2Edata[9+ch*12] = floor(I2Edata[9+ch*12]+slopes[ch]/cs*I2Epolarity[ch])		// adjust medium gain
					I2Edata[9+ch*12] = min(I2Edata[9+ch*12], 255)
					I2Edata[9+ch*12] = max(I2Edata[9+ch*12], 0)
	
				else
					//print "old fine gain ch",ch,I2Edata[10+ch*12]
					I2Edata[10+ch*12] = floor(I2Edata[10+ch*12]+slopes[ch]/cs*5*I2Epolarity[ch])	// adjust fine gain
					if (I2Edata[10+ch*12] <=0)									// if out of range, adjust coarse instead
						I2Edata[10+ch*12] = 20
						I2Edata[9+ch*12] -= 2
					endif	
					if (I2Edata[10+ch*12] >=255)
						I2Edata[10+ch*12] = 235
						I2Edata[9+ch*12] += 2
					endif
				endif
			
			endfor
			Pixie_I2E_WriteGainOffset1ToADC()
			Sleep/T 60		// a pause seems to help here to not lock up the ADC in a bad state
		
			mis = Pixie_I2E_caltest()
		endif
	endfor
	
//	mis = Pixie_I2E_caltest()		

	
	// ********** restore Module settings ********** 
	//

	for(m=0;m<4;m+=1)
		// set gain
		Display_Channel_Parameters[index_GAIN+m*ncp] = tempgain[m]
		name = "ADC_VGAIN"+num2str(m) 
		Pixie_IO_ChanVarControl(name,0,"","")
		
		// set polarity
		ChanCSRAbit05 = temppol[m]
		name = "ADC_CCSRA05_"+num2str(m) 
		Pixie_IO_CheckBoxControl(name,ChanCSRAbit05)
		
//		// set initial offset to 49%
//		Display_Channel_Parameters[ index_BASEPC+m*ncp] = tempbasepc[m]
//		name = "ADC_BASELINE_PERCENT"+num2str(m)
//		Pixie_IO_ChanVarControl(name,0,"","")
//				
//		// set dt to 0.067us (minimum)
//		Display_Channel_Parameters[ index_XDT+m*ncp] = tempxdt[m]
//		name = "ADC_XDT"+num2str(m)
//		Pixie_IO_ChanVarControl(name,0,"","")
	endfor
	
	
	Pixie_Ctrl_CommonButton("AdjustDC")	// adjust offsets
	Pixie_Ctrl_CommonButton("ADCRefresh")	// refresh
	
	if(mis>2)
		DoAlert 0, "ADC calibration did not converge well. Please repeat. In bad cases, please reset ADC. In extreme cases, please reboot."
	else
		// write to file
		//save/t I2Edata as "ADCcaldata.itx"  // write to file is quite pointless
	endif

	
End

Function Pixie_I2E_caltest()

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_OFFSET = root:pixie4:index_OFFSET
	Nvar ncp = root:pixie4:NumChannelPar
	String name
	Variable basepc
	Variable ndc = 19
		
	make/o/n=(ndc) root:auto:I2Ediff0
	wave I2Ediff0 = root:auto:I2Ediff0
	I2Ediff0 = nan
	make/o/n=(ndc) root:auto:I2Ediff1
	wave I2Ediff1 = root:auto:I2Ediff1
	I2Ediff1 = nan
	make/o/n=(ndc) root:auto:I2Ediff2
	wave I2Ediff2 = root:auto:I2Ediff2
	I2Ediff2 = nan
	make/o/n=(ndc) root:auto:I2Ediff3
	wave I2Ediff3 = root:auto:I2Ediff3
	I2Ediff3 = nan
	
	variable  ADCTraceLen = 400
	make/o/n=(ADCTraceLen/2) traceA, traceB, diff
	wave traceA
	wave traceB
	wave diff	
	variable m, k, j
	
	// set HostIO to 3 for I2E special ADC traces	
	wave DSPValues = root:pixie4:DSPvalues
	Nvar ChosenModule = root:pixie4:ChosenModule
	Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)		// get DSP data	
	root:pixie4:DSPValues=DSPValues	// update 
	Variable HostIOindex = Pixie_Find_DSPname("HostIO")
	DSPValues[HostIOindex] = 3					// set HostIO
	Pixie4_Buffer_IO(DSPValues, 0, 0,"", ChosenModule)		// apply to module

	Pixie_Ctrl_CommonButton("ADCRefresh")	// refresh ADC traces
	DoUpdate

	for(k=0;k<ndc;k+=1)		// repeat for ndc different offsets
				
		for(m=0;m<4;m+=1)		// repeat for 4 channels
			wave wav = $("root:pixie4:adcch"+num2str(m))
			wave I2Ediff = $("root:auto:I2Ediff"+num2str(m))
			traceA = wav[2*p+k*ADCTraceLen]
			traceB = wav[2*p+k*ADCTraceLen+1]
			traceA[0] = traceA[1]	// remove possible glitches
			traceB[0] = traceB[1]
			diff = traceA-traceB
			wavestats/q diff
			I2Ediff[k] = V_avg
		endfor		
	endfor
	
	// restore HostIO
	DSPValues[HostIOindex] = 0					// set HostIO
	Pixie4_Buffer_IO(DSPValues, 0, 0,"", ChosenModule)		// apply to module

	Variable mis
	mis = Pixie_I2E_CalAnalyze(0)
	return(mis)

End

Function Pixie_I2E_CalAnalyze(report)
Variable report //0	- no suggestions for changes, 1 - suggestions
	
	// analyze
	make/n=4/o root:auto:slopes, root:auto:offsets
	wave slopes = root:auto:slopes
	wave offsets = root:auto:offsets
	Wave W_coef
	Variable co, cs
	
	wave I2Ediff0 = root:auto:I2Ediff0
	wave I2Ediff1 = root:auto:I2Ediff1
	wave I2Ediff2 = root:auto:I2Ediff2
	wave I2Ediff3 = root:auto:I2Ediff3
	
	DoWindow/F I2Ediffs
	If(V_flag!=1)
		String fldrSav0= GetDataFolder(1)
		SetDataFolder root:auto:
		Display/K=1/W=(6,300,300,500) I2Ediff0,I2Ediff1,I2Ediff2,I2Ediff3
		DoWindow/C I2Ediffs
		SetDataFolder fldrSav0
		ModifyGraph mode=4
		ModifyGraph marker=19
		ModifyGraph rgb(I2Ediff1)=(0,65280,0),rgb(I2Ediff2)=(0,15872,65280),rgb(I2Ediff3)=(0,39168,0)
		Label left "Core mismatch (ADC steps)"
		Label bottom "ADC range (a.u.)"
	endif

	wavestats/q I2Ediff0
	offsets[0] = V_avg
	wavestats/q I2Ediff1
	offsets[1] = V_avg
	wavestats/q I2Ediff2
	offsets[2] = V_avg	
	wavestats/q I2Ediff3
	offsets[3] = V_avg
	print " "
	print "offset differences:", offsets[0], offsets[1], offsets[2], offsets[3]	
	// 1 coarse offset unit changes ADC diff by ~4.24steps
	co = 4.24
	if(report)
		print "suggested adjustment (coarse)", offsets[0]/co, offsets[1]/co, offsets[2]/co, offsets[3]/co
		print "suggested adjustment (fine)", offsets[0]/co*25, offsets[1]/co*25, offsets[2]/co*25, offsets[3]/co*25
	endif
	
	CurveFit/Q/NTHR=0 line  root:auto:I2Ediff0 /D 
	slopes[0] = W_coef[1]
	CurveFit/Q/NTHR=0 line  root:auto:I2Ediff1 /D 
	slopes[1] = W_coef[1]
	CurveFit/Q/NTHR=0 line  root:auto:I2Ediff2 /D 
	slopes[2] = W_coef[1]
	CurveFit/Q/NTHR=0 line  root:auto:I2Ediff3 /D 
	slopes[3] = W_coef[1]

	print "gain differences:", slopes[0], slopes[1], slopes[2], slopes[3]
	// 1 medium gain unit changes slope by ~0.13
	cs = -0.13
	if(report)
		print "suggested adjustment (coarse)", slopes[0]/cs, slopes[1]/cs, slopes[2]/cs, slopes[3]/cs
		print "suggested adjustment (fine)", slopes[0]/cs*10, slopes[1]/cs*10, slopes[2]/cs*10, slopes[3]/cs*10
	endif
	
	//overall mismatch
	Variable mis
	mis = sqrt(offsets[0]^2+ offsets[1]^2+ offsets[2]^2+ offsets[3]^2 + 100*(slopes[0]^2+ slopes[1]^2+ slopes[2]^2+ slopes[3]^2))
	print "mismatch overall", mis
	return (mis)	

End

Function Pixie_I2E_WriteGainOffset1ToADC()
// a shorter version of I2E_WriteCalibToADC, only gain and offset for core 1

	wave I2Edata =  root:auto:I2Edata
	make/o/n=4 data
	variable ch,k

	// first disable I2E 
	data = 0x20
	Pixie_I2E_ADCSPI(0x31, data, 0)	
	
	// second, write data
	for(ch=0;ch<4;ch+=1)
		data[ch] = I2Edata[12*ch+6]
	endfor
	Pixie_I2E_ADCSPI(0x26, data, 0)		//write  coarse offset, core 1

	for(ch=0;ch<4;ch+=1)
		data[ch] = I2Edata[12*ch+7]
	endfor
	Pixie_I2E_ADCSPI(0x27, data, 0)		//write  fine offset, core 1

	for(ch=0;ch<4;ch+=1)
		data[ch] = I2Edata[12*ch+9]
	endfor
	Pixie_I2E_ADCSPI(0x29, data, 0)		//writemedium gain, core 1

	for(ch=0;ch<4;ch+=1)
		data[ch] = I2Edata[12*ch+10]
	endfor
	Pixie_I2E_ADCSPI(0x2A, data, 0)		//write fine gain, core 1
	
	// third, freeze I2E (data not applied if frozen, but if enabled and not frozen, I2E will recalibrate) 
	Pixie_I2E_ADCSPI(0x31, data, 1)		//read current values in I2E control
	for(ch=0;ch<4;ch+=1)
		data[ch] = data[ch] | 2					// set bit to freeze
	endfor
	Pixie_I2E_ADCSPI(0x31, data, 0)		// write back to ADC
	
	// fourth, enable I2E (keep frozen)
	Pixie_I2E_ADCSPI(0x31, data, 1)		//read current values in I2E control
	for(ch=0;ch<4;ch+=1)
		data[ch] = data[ch] | 1				// set bit to enable
	endfor
	Pixie_I2E_ADCSPI(0x31, data, 0)		// write back to ADC

End

Function Pixie_I2E_ADCSPI(addr, data, read)
variable addr
Wave data
Variable read

	wave DSPValues = root:pixie4:DSPvalues
	Nvar ChosenModule = root:pixie4:ChosenModule
	make/u/i/o/n=8192 buffer
	make/u/i/o/n=1 dummy
	
	// get DSP data	
	Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
	root:pixie4:DSPValues=DSPValues	// update 

	Variable HostIOindex = Pixie_Find_DSPname("HostIO")
	Variable HostIODataindex = Pixie_Find_DSPname("HostIOData")
	Variable HostODataindex = Pixie_Find_DSPname("HostOData")
	variable ch
	
	if(read)
		addr = setbit(15,addr)
		data = 0
	else
		addr = clrbit(15,addr)
	endif
	DSPValues[HostIODataindex+0] = data[0]
	DSPValues[HostIOindex+0] = addr
	DSPValues[HostIODataindex+1] = data[1]
	DSPValues[HostIOindex+1] = addr
	DSPValues[HostIODataindex+2] = data[2]
	DSPValues[HostIOindex+2] = addr
	DSPValues[HostIODataindex+3] = data[3]
	DSPValues[HostIOindex+3] = addr
	
	// apply to module and talk to ADC	
	//Pixie_EX_PutModPar()	
	Pixie4_Buffer_IO(DSPValues, 0, 0,"", ChosenModule)
	
	//Callcontroltask(34)
	pixie4_Acquire_Data(34, buffer, "",ChosenModule)	
	do
		Sleep/t 5
	while (pixie4_Acquire_Data(0x4000, dummy, "",ChosenModule))
	
	if(read)
		// get DSP data	
		Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
		root:pixie4:DSPValues=DSPValues	// update 
	
		for(ch=0;ch<4;ch+=1)
			data[ch] =  DSPValues[HostODataindex+ch]
//			if(data[ch] >256)
//				print "bad write, channel",ch, "to"
//				ph(addr)
//			endif
		endfor
	endif
End

Function Pixie_I2E_ReadCalibFromADC()

	wave I2Edata =  root:auto:I2Edata
	wave/t I2Edatanames =  root:auto:I2Edatanames
	make/o/n=4 data
	variable ch
	
//	DoWindow/F I2Etable
//	if (V_flag ==0)
//		Edit/K=1/W=(753.75,47,947.25,695) I2Edatanames,I2Edata
//		ModifyTable format(Point)=1,width(Point)=42,width(I2Edata)=56
//		DoWindow/C I2Etable
//	endif
	
	Pixie_I2E_ADCSPI(0x20, data, 1)		//read current values in coarse offset, core 0
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+0] = data[ch]
	endfor
	Pixie_I2E_ADCSPI(0x26, data, 1)		//read current values in coarse offset, core 1
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+6] =  data[ch]
	endfor
	
	Pixie_I2E_ADCSPI(0x21, data, 1)		//read current values in fine offset, core 0
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+1] =  data[ch]
	endfor
	Pixie_I2E_ADCSPI(0x27, data, 1)		//read current values in fine offset, core 1
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+7] =  data[ch]
	endfor
	
	Pixie_I2E_ADCSPI(0x22, data, 1)		//read current values in coarse gain, core 0
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+2] =  data[ch]
	endfor
	Pixie_I2E_ADCSPI(0x28, data, 1)		//read current values in  coarse gain, core 1
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+8] =  data[ch]
	endfor
	
	Pixie_I2E_ADCSPI(0x23, data, 1)		//read current values in medium gain, core 0
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+3] =  data[ch]
	endfor
	Pixie_I2E_ADCSPI(0x29, data, 1)		//read current values in medium gain, core 1
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+9] =  data[ch]
	endfor
	
	Pixie_I2E_ADCSPI(0x24, data, 1)		//read current values in fine gain, core 0
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+4] =  data[ch]
	endfor
	Pixie_I2E_ADCSPI(0x2A, data, 1)		//read current values in fine gain, core 1
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+10] =  data[ch]
	endfor
	
	Pixie_I2E_ADCSPI(0x70, data, 1)		//read current values in skew
	for(ch=0;ch<4;ch+=1)
		I2Edata[12*ch+5] =  data[ch]
	endfor

End


//########################################################################
//
//	Pixie_IO_ChanVarControl:
//		Handle variable value changes for I/O parameters.
//		SetVariableControls must be named XXX_name#
//           where 	"XXX" is an identifier for tabs
//				"name" is the name of the I/O variable as in Channel_Parameter_Names (e.g. ENERGY_RISETIME)
//				"#" is the channel number
//
//########################################################################
Function Pixie_IO_ChanVarControl(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum
	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar NumChannelPar = root:pixie4:NumChannelPar
	
	String IOvarname
	Variable ch, len,k,m,j
	Variable direction
	
	direction=0	// Write to Pixie modules
	len = strlen(CtrlName)
	IOvarname = CtrlName[4,len-2]
	ch = str2num(CtrlName[len-1,len])
	//print CtrlName, len, IOvarname, ch
	
	
	// copy display value into I/O value 
	// copy all values of this channel - allows special routines to modify more than one value, 
	// but not for multiple channels because the IO only handles one channel at a time
	j=ch*NumChannelPar
//	j=0
//	for(m=0;m<NumberOfChannels;m+=1)
		for(k=0;k<(NumChannelPar);k+=1)	
			Channel_Parameter_Values[k][ch][ChosenModule] = Display_Channel_Parameters[j]
//			Channel_Parameter_Values[k][m][ChosenModule] = Display_Channel_Parameters[j]
			j+=1
		endfor
//	endfor


	//Channel_Parameter_Values[ChosenModule*4*NumChannelPar, (ChosenModule+1)*4*NumChannelPar-1] = Display_Channel_Parameters[p]
	Pixie4_User_Par_IO(Channel_Parameter_Values,IOvarname, "CHANNEL", direction, ChosenModule, ch)
	

	// update display variables with values changed in C library 
	Pixie_IO_ReadChanParFromMod(ChosenModule)
	Pixie_IO_SendChanParToIgor()
	
	// Update Module variables in case coincidence wait changed
	if( (cmpstr(IOvarname,"ENERGY_RISETIME")==0)  |  (cmpstr(IOvarname,"ENERGY_FLATTOP")==0)  | (cmpstr(IOvarname,"CHANNEL_CSRA")==0)        )
		Pixie_IO_ReadModParFromMod(ChosenModule)
		Pixie_IO_SendModParToIgor()
	endif

End


//########################################################################
//
//	Pixie_IO_ModVarControl:
//		Handle variable value changes for I/O parameters.
//		SetVariableControls must be named XXX_name#
//           where 	"XXX" is an identifier for tabs
//				"name" is a short version of the variable name as in Module_Parameter_Names (e.g. MODULE_CSRA)
//						to be looked up by case statement below
//
//########################################################################
Function Pixie_IO_ModVarControl(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum
		
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumModulePar = root:pixie4:NumModulePar
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_MCSRA = root:pixie4:index_MCSRA
	Nvar index_MCSRB = root:pixie4:index_MCSRB
	Nvar index_MaxEv = root:pixie4:index_MaxEv
	Nvar index_CoincPat = root:pixie4:index_CoincPat
	Nvar index_ActCW = root:pixie4:index_ActCW
	Nvar index_FilterRange = root:pixie4:index_FilterRange
	Nvar index_MCSRC = root:pixie4:index_MCSRC
	Nvar index_ModPat = root:pixie4:index_ModPat
	Nvar index_NNSharePat = root:pixie4:index_NNSharePat
	Nvar index_UserIn = root:pixie4:index_UserIn
	
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Variable dt,j,k
	
	// copy display value into I/O value 
	j=0
	for(k=0;k<(NumModulePar);k+=1)	
		Module_Parameter_Values[k][ChosenModule] = Display_Module_Parameters[j]
		j+=1
	endfor


	StrSwitch(ctrlName)			
		Case "ADV_MCSRA":
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRA", "MODULE", 0, ChosenModule, 0)
			break	
			
		Case "ADV_ModCSRB":
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRB", "MODULE", 0, ChosenModule, 0)
			break	
			
		Case "ADV_MCSRC":
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRC", "MODULE", 0, ChosenModule, 0)
			break	
		
		Case "RUX_SetMaxEvents":
			Pixie4_User_Par_IO(Module_Parameter_Values, "MAX_EVENTS", "MODULE", 0, ChosenModule, 0)		
			break
					
		Case "COI_Coinc":
			Pixie4_User_Par_IO(Module_Parameter_Values, "COINCIDENCE_PATTERN", "MODULE", 0, ChosenModule, 0)
			break
	
		Case "COI_ActualCoincWait":
			Pixie4_User_Par_IO(Module_Parameter_Values, "ACTUAL_COINCIDENCE_WAIT", "MODULE", 0, ChosenModule, 0)
			break
			
		Case "ADV_ModulePat":
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULEPATTERN", "MODULE", 0, ChosenModule, 0)
			break	
			
			
		Case "ADV_NNshare":
			Pixie4_User_Par_IO(Module_Parameter_Values, "NNSHAREPATTERN", "MODULE", 0, ChosenModule, 0)
			break			
			
		Case "EGX_FilterRange":
			//------------------------------------------------------------------------------------------------------------
			// Changing filter range would affect the following parameters:
			//	energy filter rise time and flat top, baseline cut, peaksample, peaksep, 
			//	trigger delay, paflength, and coincidence wait.
			//------------------------------------------------------------------------------------------------------------
		
			Execute "Pixie_Panel_PleaseWait()"
			
			// First, download the new filter range to the driver and the DSP
			Pixie4_User_Par_IO(Module_Parameter_Values, "FILTER_RANGE", "MODULE", 0, ChosenModule, 0)

			// Then, update all affected parameters
			// Need to loop over all the channels of a Pixie module
			// A call to UPDATE_FILTERRANGE_PARAMS will update energy filter, and call Compute_FIFO, BLCut_Finder and Program_Fippi
			//Pixie4_User_Par_IO(Channel_Parameter_Values, "UPDATE_FILTERRANGE_PARAMS", "MODULE", 0, ChosenModule, 0)
			
			// Update channel parameters
			Pixie_IO_ReadChanParFromMod(ChosenModule)
			Pixie_IO_SendChanParToIgor()
			
			DoWindow/K PleaseWaitPanel
						
			break
			
		Case "UserIn00":
		Case "UserIn01":
		Case "UserIn02":
		Case "UserIn03":
		Case "UserIn04":
		Case "UserIn05":
		Case "UserIn06":
		Case "UserIn07":
		Case "UserIn08":
		Case "UserIn09":
		Case "UserIn10":
		Case "UserIn11":
		Case "UserIn12":
		Case "UserIn13":
		Case "UserIn14":
		Case "UserIn15":	
			Pixie4_User_Par_IO(Module_Parameter_Values, "USER_IN", "MODULE", 0, ChosenModule, 0)
			break	
	
		Case "EXTRA_IN0":
		Case "EXTRA_IN1":	
		Case "EXTRA_IN2":
		Case "EXTRA_IN3":
		Case "EXTRA_IN4":	
		Case "EXTRA_IN5":
		Case "EXTRA_IN6":
		Case "EXTRA_IN7":	
			Pixie4_User_Par_IO(Module_Parameter_Values, "EXTRA_IN", "MODULE", 0, ChosenModule, 0)
			break	
		
						
		Default:
			break
			
	EndSwitch
		
	// Update module parameters
	Pixie_IO_ReadModParFromMod(ChosenModule)
	Pixie_IO_SendModParToIgor()	// includes chassis list
			

	StrSwitch(ctrlName)		// control specific updates:	
		Case "EGX_FilterRange":	
			// Update Energy Filter time limits
			DoWindow/F Pixie_Parameter_Setup
			if(V_flag==1)
				dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
				SetVariable EGY_ENERGY_RISETIME0, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME1, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME2, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME3, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP0, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
				SetVariable EGY_ENERGY_FLATTOP1, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
				SetVariable EGY_ENERGY_FLATTOP2, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
				SetVariable EGY_ENERGY_FLATTOP3, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup			
			endif
			break
								
		Default:
			break
			
	EndSwitch	
End



//########################################################################
//
//	Pixie_IO_CheckBoxControl:
//		Handle variable value changes for I/O parameters from checkboxes
//		Check box controls must be named XXX_namebt_#
//           where 	"XXX" is an identifier for tabs
//				"name" is a key word for the I/O variable  (e.g. CCSRA for CHANNEL_CSRA)
//				"bt" is the bit number
//				"#" is the channel number (or other indicator for module variables)
//
//########################################################################
Function Pixie_IO_CheckBoxControl(ctrlName, checked) : CheckBoxControl
	String ctrlName
	Variable checked
	
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	
	Nvar index_CoincPat=root:pixie4:index_CoincPat
	Nvar index_MCSRA=root:pixie4:index_MCSRA
	Nvar index_MCSRB=root:pixie4:index_MCSRB
	Nvar index_DBCSR=root:pixie4:index_DBCSR
	Nvar index_CC=root:pixie4:index_CC
	
	String IOvarname
	Variable ch, len,k,m,j,bit,i
	Variable direction, CCSRA, CCSRC, MCSRA, MCSRB, DBCSR, CoincPat, CC
	
	direction=0	// Write to Pixie modules
	len = strlen(CtrlName)
	IOvarname = CtrlName[4,len-5]
	bit = str2num(CtrlName[len-4,len-2])
	ch = str2num(CtrlName[len-1,len])
//	print CtrlName, len, IOvarname, ch, bit

	Nvar RepeatLMrun = root:pixie4:RepeatLMrun
	Nvar NoRepeatLMrun = root:pixie4:NoRepeatLMrun
	Nvar PingPongRun = root:pixie4:PingPongRun
	Nvar synchwait=root:pixie4:synchwait
	Nvar InSynch=root:pixie4:InSynch

	StrSwitch(IOvarname)
		Case "hit":
			CoincPat = Display_Module_Parameters[index_CoincPat] // start with what is displayed
			CoincPat =CLRbit(bit,CoincPat)						// modify
			CoincPat +=checked*2^bit
			// Download coincidence pattern to the current module
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("COINCIDENCE_PATTERN")][ChosenModule]=CoincPat
			Pixie4_User_Par_IO(Module_Parameter_Values, "COINCIDENCE_PATTERN", "MODULE", direction, ChosenModule, 0)
			
			//these changes affect only current module
			Pixie_IO_ReadModParFromMod(ChosenModule)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break
	
		Case "SynchWait":
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("SYNCH_WAIT")][ChosenModule]=SynchWait	// we do not use the display wave since only one bit is active, use check box variable only
			Pixie4_User_Par_IO(Module_Parameter_Values, "SYNCH_WAIT", "MODULE", 0, ChosenModule, 0)		
			// broadcasted to all modules in C library
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break	
	
		Case "RunSynchro":
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("IN_SYNCH")][ChosenModule]=1-InSynch  // we do not use the display wave since only one bit is active, use check box variable only
			Pixie4_User_Par_IO(Module_Parameter_Values, "IN_SYNCH", "MODULE", 0, ChosenModule, 0)
			// broadcasted to all modules in C library
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break

		Case "DblBufCSR":	
			DBCSR = Display_Module_Parameters[index_DBCSR]	// start with what is displayed
			if(bit==0) // multiple buffer/spill radio buttons
				if(checked)										// modify
					NoRepeatLMrun = 0
					RepeatLMrun = 0
					DBCSR = SETbit(bit, DBCSR)
				else
					DBCSR = CLRbit(bit, DBCSR)
				endif
			endif
			
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("DBLBUFCSR")][ChosenModule]=DBCSR	// download
			Pixie4_User_Par_IO(Module_Parameter_Values, "DBLBUFCSR", "MODULE", 0, ChosenModule, 0)
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break
	
		Case "MCSRA":	
			MCSRA = Display_Module_Parameters[index_MCSRA]	// start with what is displayed
			DBCSR = Display_Module_Parameters[index_DBCSR]
			if(bit==1) // multiple buffer/spill radio buttons				// modify
				if(ch==4) //set 32 buffers
					if(checked)
						NoRepeatLMrun = 0
						PingPongRun = 0
						MCSRA = SETbit(bit, MCSRA)
						DBCSR = CLRbit(0, DBCSR)	
					endif
				else //set 1buffer
					if(checked)
						RepeatLMrun = 0
						PingPongRun = 0
						MCSRA = CLRbit(bit, MCSRA)
						DBCSR = CLRbit(0, DBCSR) 
					endif
				endif
				// need to explicitely clear DblBufCSR because C lib can not tell if  a change to "NoRepeatRun" is different
				// from changing a different MCRSA bit and leaving buffer mode unchanged in double buffer mode. 
				// (in both cases C lib is called with "MCSRA" and bit1 = 0) 				
				Module_Parameter_Values[Pixie_Find_ModuleParInWave("DBLBUFCSR")][ChosenModule]=DBCSR
				Pixie4_User_Par_IO(Module_Parameter_Values, "DBLBUFCSR", "MODULE", 0, ChosenModule, 0)

			else		// all other MCSRA bits	
				if(checked)
					MCSRA = SETbit(bit, MCSRA)
				else
					MCSRA = CLRbit(bit, MCSRA)
				endif		
			endif
	
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULE_CSRA")][ChosenModule]=MCSRA
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRA", "MODULE", 0, ChosenModule, 0)
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break
			
		Case "MCSRB":	
			MCSRB = Display_Module_Parameters[index_MCSRB]	// start with what is displayed
		
			if(checked)
				MCSRB = SETbit(bit, MCSRB)
			else
				MCSRB = CLRbit(bit, MCSRB)
			endif		
	
	
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("MODULE_CSRB")][ChosenModule]=MCSRB
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_CSRB", "MODULE", 0, ChosenModule, 0)
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break	
	
		Case "CCSRA":	
			CCSRA = Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRA")][ch][ChosenModule]
			if(checked)
				CCSRA = SETbit(bit, CCSRA)
			else
				CCSRA = CLRbit(bit, CCSRA)
			endif		
			Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRA")][ch][ChosenModule]=CCSRA
			Pixie4_User_Par_IO(Channel_Parameter_Values, "CHANNEL_CSRA", "CHANNEL", 0, ChosenModule, ch)
			
			// update display variables with values changed in C library 
			Pixie_IO_ReadChanParFromMod(ChosenModule)
			Pixie_IO_SendChanParToIgor()
			
			//some changes module parameters, so update 
			Pixie_IO_ReadModParFromMod(ChosenModule)
			Pixie_IO_SendModParToIgor()
			break
			
		Case "CCSRC":	
			CCSRC = Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRC")][ch][ChosenModule]
			if(checked)
				CCSRC = SETbit(bit, CCSRC)
			else
				CCSRC = CLRbit(bit, CCSRC)
			endif		
			Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRC")][ch][ChosenModule]=CCSRC
			Pixie4_User_Par_IO(Channel_Parameter_Values, "CHANNEL_CSRC", "CHANNEL", 0, ChosenModule, ch)
			
			// update display variables with values changed in C library 
			Pixie_IO_ReadChanParFromMod(ChosenModule)
			Pixie_IO_SendChanParToIgor()
			break
			
		Case "KeepCW":	
			Nvar KeepCW=root:pixie4:KeepCW
			Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
			Nvar ChosenChannel = root:pixie4:ChosenChannel
			CCSRC = Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRC")][ch][ChosenModule]
			if(checked)
				KeepCW = 1
			else
				KeepCW = 0
			endif		
			System_Parameter_Values[Pixie_Find_SysParInWave("KEEP_CW")]=KeepCW
			Pixie4_User_Par_IO(System_Parameter_Values, "KEEP_CW", "SYSTEM", 0, ChosenModule, ChosenChannel)		
			break
			
		Case "CControl":	
			CC = Display_Module_Parameters[index_CC]	// start with what is displayed
			if(checked)
				CC = SETbit(bit, CC)
			else
				CC = CLRbit(bit, CC)
			endif		
		
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("C_CONTROL")][ChosenModule]=CC
			Pixie4_User_Par_IO(Module_Parameter_Values, "C_CONTROL", "MODULE", 0, ChosenModule, 0)
			
			//some changes affect all modules, so update 
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor() 	//includes also the chassis list
			break
	
		Default:
					
	EndSwitch
	
End




//########################################################################
//
//	Pixie_IO_ReadChanParFromMod:
//		Read the values of channel parameters from every Pixie module.
//
//########################################################################
Function Pixie_IO_ReadChanParFromMod(ModNum)
Variable ModNum
	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Variable direction, channum, k
	
	// This function can handle two cases:
	//	1. ModNum < NumberOfModules: read channel parameters from the selected module
	//	2. ModNum = NumberOfModules: read channel parameters from all modules present in the system

	direction=1	// Read from the Pixie module
	
	if(ModNum < NumberOfModules)	// read channel parameters from the selected module
	//	for(channum=0; channum<NumberOfChannels; channum+=1) // XXX loop over channels implemented in C library, but needs to be debugged
			Pixie4_User_Par_IO(Channel_Parameter_Values, "ALL_CHANNEL_PARAMETERS", "CHANNEL", direction, ModNum, channum)
	//	endfor
	endif
	
	if(ModNum == NumberOfModules)	// read channel parameters from all modules present in the system
		for(k=0; k<NumberOfModules; k+=1)
		//	for(channum=0; channum<NumberOfChannels; channum+=1)	// loop over channels implemented in C library
				Pixie4_User_Par_IO(Channel_Parameter_Values, "ALL_CHANNEL_PARAMETERS", "CHANNEL", direction, k, channum)
		//	endfor
		endfor
	endif
	
	// force update
	root:pixie4:Channel_Parameter_Values = Channel_Parameter_Values
	
End


//########################################################################
//
//	Pixie_IO_SendChanParToIgor:
//		Update the Igor control displays using the latest channel parameters.
//
//########################################################################
Function Pixie_IO_SendChanParToIgor()
	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
		
	wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar NumChannelPar = root:pixie4:NumChannelPar
	Display_Channel_Parameters = Channel_Parameter_Values[p+NumberOfChannels*NumChannelPar*ChosenModule]
	
	// update CCSRA bit variables
	variable k,ch
	string vrname0, vrname1, vrname2
	for(ch=0;ch<4;ch+=1)
		for(k=0;k<16;k+=1)
			sprintf vrname0, "root:pixie4:Chan%dCSRAbit%02d",ch,k
			sprintf vrname1, "root:pixie4:Chan%dCSRBbit%02d",ch,k
			sprintf vrname2, "root:pixie4:Chan%dCSRCbit%02d",ch,k
			NVAR CCSRAbit = $(vrname0)
			CCSRAbit = tstbit(k,Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRA")][ch][ChosenModule])	
			NVAR CCSRBbit = $(vrname1)
			CCSRBbit = tstbit(k,Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRB")][ch][ChosenModule])		
			NVAR CCSRCbit = $(vrname2)
			CCSRCbit = tstbit(k,Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRC")][ch][ChosenModule])		
		endfor
	endfor
	
End


//########################################################################
//
//	Pixie_IO_ReadModParFromMod:
//		Read the values of module parameters from every Pixie module.
//
//########################################################################
Function Pixie_IO_ReadModParFromMod(ModNum)
Variable ModNum

	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Variable direction, channum, k
	
	// This function can handle two cases:
	//	1. ModNum < NumberOfModules: read module parameters from the selected module
	//	2. ModNum = NumberOfModules: read module parameters from all modules present in the system
	
	direction=1	// Read from the Pixie module
	channum=0	// Channel number is not important here

	if(ModNum < NumberOfModules)	// read module parameters from the selected module
		Pixie4_User_Par_IO(Module_Parameter_Values, "ALL_MODULE_PARAMETERS", "MODULE", direction, ModNum, channum)
	endif
	
	if(ModNum == NumberOfModules)	// read module parameters from all modules present in the system
		for(k=0; k<NumberOfModules; k+=1)
			Pixie4_User_Par_IO(Module_Parameter_Values, "ALL_MODULE_PARAMETERS", "MODULE", direction, k, channum)
		endfor
	endif
	
	// force update
	root:pixie4:Module_Parameter_Values = Module_Parameter_Values
	
End


//########################################################################
//
//	Pixie_IO_SendModParToIgor:
//		Update the Igor control displays using the latest module parameters.
//
//########################################################################
Function Pixie_IO_SendModParToIgor()
	
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Nvar ChosenModule = root:pixie4:ChosenModule
	

	Nvar SynchWait = root:pixie4:SynchWait
	Nvar InSynch = root:pixie4:InSynch
	Nvar SynchAlways  = root:pixie4:SynchAlways

	
	Nvar RepeatLMrun = root:pixie4:RepeatLMrun
	Nvar NoRepeatLMrun = root:pixie4:NoRepeatLMrun

	Nvar CloverAdd =  root:pixie4:CloverAdd
	Nvar CloverSingleOnly =  root:pixie4:CloverSingleOnly
	Nvar CWgroup = root:pixie4:CWgroup
	Nvar PingPongRun = root:pixie4:PingPongRun
	Nvar TS500 = root:pixie4:TS500
	Nvar SDRAMbypass = root:pixie4:SDRAMbypass
	Nvar RS2LM = root:pixie4:RS2LM
	Nvar RS2LMedge = root:pixie4:RS2LMedge
	Nvar Polling = root:pixie4:Polling
	Nvar BufferQC = root:pixie4:BufferQC
	Nvar PrintDebugMsg_Boot = root:pixie4:PrintDebugMsg_Boot
	Nvar PrintDebugMsg_QCerror = root:pixie4:PrintDebugMsg_QCerror
	Nvar PrintDebugMsg_QCdetail = root:pixie4:PrintDebugMsg_QCdetail
	Nvar PrintDebugMsg_other = root:pixie4:PrintDebugMsg_other
	Nvar PrintDebugMsg_daq = root:pixie4:PrintDebugMsg_daq
	Nvar PrintDebugMsg_file = root:pixie4:PrintDebugMsg_file
	Nvar PollForNewData = root:pixie4:PollForNewData
	Nvar MultiThreadDAQ = root:pixie4:MultiThreadDAQ
	Nvar KeepBLcut = root:pixie4:KeepBLcut
	
	
	wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar NumModulePar = root:pixie4:NumModulePar
	Nvar index_DBCSR = root:pixie4:index_DBCSR
	Nvar index_MCSRA = root:pixie4:index_MCSRA
	Nvar index_MCSRB = root:pixie4:index_MCSRB
	Nvar index_CC = root:pixie4:index_CC
	
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar MCArunListNumber = root:pixie4:MCArunListNumber
	Nvar Nruntypes = root:pixie4:Nruntypes
	Wave RunTasks = root:pixie4:RunTasks
	Nvar index_RunType = root:pixie4:index_RunType
	
	Variable k
	String vrname

	Display_Module_Parameters = Module_Parameter_Values[p+NumModulePar*ChosenModule]

	// update variables for checkboxes
	SynchWait = Module_Parameter_Values[Pixie_Find_ModuleParInWave("SYNCH_WAIT")][ChosenModule]
	InSynch = 1 - Module_Parameter_Values[Pixie_Find_ModuleParInWave("IN_SYNCH")][ChosenModule]
	if (InSynch==0)
		SynchAlways=0
	endif
	
	Pixie_IO_SetHitPattern()
	
	PingPongRun = tstbit( 0,Display_Module_Parameters[index_DBCSR])
	RepeatLMrun = tstbit(1,Display_Module_Parameters[index_MCSRA])
	if( (PingPongRun==0) & (RepeatLMrun==0) )
		NoRepeatLMrun = 1
	else 
		NoRepeatLMrun = 0
	endif 
	CloverAdd =  tstbit(3,Display_Module_Parameters[index_MCSRA])
	CloverSingleOnly =  tstbit(4,Display_Module_Parameters[index_MCSRA])
	TS500 = tstbit(10,Display_Module_Parameters[index_MCSRA])
	SDRAMbypass  = tstbit(11,Display_Module_Parameters[index_MCSRA])
	RS2LM  = tstbit(8,Display_Module_Parameters[index_MCSRA])
	RS2LMedge  = tstbit(7,Display_Module_Parameters[index_MCSRA])
	CWgroup =  tstbit(0,Display_Module_Parameters[index_MCSRA])
	
	// MCSRB checkboxes
	for(k=0;k<15;k+=1)	
			sprintf vrname, "root:pixie4:ModCSRBbit%02d",k
			NVAR MSRBbit = $(vrname)
			MSRBbit = tstbit(k,Module_Parameter_Values[index_MCSRB][ChosenModule])	
	endfor
	
	Pixie_MakeList_Chassis(1)
	
	// CControl checkboxes
	PrintDebugMsg_Boot 		= tstbit( 4,Display_Module_Parameters[index_CC])
	PrintDebugMsg_QCerror	= tstbit( 5,Display_Module_Parameters[index_CC])
	PrintDebugMsg_QCdetail	= tstbit( 6,Display_Module_Parameters[index_CC])
	PrintDebugMsg_other		= tstbit( 7,Display_Module_Parameters[index_CC])
	Polling 					= tstbit( 8,Display_Module_Parameters[index_CC])
	BufferQC					= tstbit( 9,Display_Module_Parameters[index_CC])
	PollForNewData 			= tstbit( 11,Display_Module_Parameters[index_CC])
	MultiThreadDAQ			= tstbit( 12,Display_Module_Parameters[index_CC])
	PrintDebugMsg_daq		= tstbit( 13,Display_Module_Parameters[index_CC])
	PrintDebugMsg_file		= tstbit( 14,Display_Module_Parameters[index_CC])
	KeepBLcut					= tstbit( 15,Display_Module_Parameters[index_CC])
	
	// run type
	Variable runtype
	runtype = Display_Module_Parameters[index_RunType]
	Whichrun=MCArunListNumber	// default
	for(k=0;k<Nruntypes;k+=1)
		if(runtype==RunTasks[k])
			Whichrun = k+1
		endif	
	endfor

//	DoWindow/F Pixie_Parameter_Setup
//	if(V_flag==1)
		PopupMenu/Z RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie_Parameter_Setup
//	endif
//	DoWindow/F Pixie4MainPanel
//	if(V_flag==1)
		PopupMenu/Z RUN_WhichRun, mode=Pixie_GetRunType(), win = Pixie4MainPanel
//	endif
End


//########################################################################
//
//	Pixie_IO_ReadSysParFromMod:
//		Read the values of system parameters from the XOP.
//
//########################################################################
Function Pixie_IO_ReadSysParFromMod()
	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values

	Variable direction, modnum, channum
	direction=1	// Read from the Pixie module
	modnum=0	// Module number is not important here
	channum=0	// Channel number is not important here
	
	// Read system parameters
	Pixie4_User_Par_IO(System_Parameter_Values, "ALL_SYSTEM_PARAMETERS", "SYSTEM", direction, modnum, channum)
	
		
	// force update
	root:pixie4:System_Parameter_Values = System_Parameter_Values

	
End


//########################################################################
//
//	Pixie_IO_SendSysParToIgor:
//		Update the Igor control displays using the latest system parameters.
//
//########################################################################
Function Pixie_IO_SendSysParToIgor()

	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	Nvar CLibraryRelease = root:pixie4:CLibraryRelease
	Nvar CLibraryBuild = root:pixie4:CLibraryBuild
	Nvar KeepCW=root:pixie4:KeepCW
	
	CLibraryRelease = System_Parameter_Values[Pixie_Find_SysParInWave("C_LIBRARY_RELEASE")]
	CLibraryBuild = System_Parameter_Values[Pixie_Find_SysParInWave("C_LIBRARY_BUILD")]
	KeepCW =  System_Parameter_Values[Pixie_Find_SysParInWave("KEEP_CW")]
	//PopupMenu/Z COI_KeepCW, mode=Pixie_GetKeepCW(), win = Pixie_Parameter_Setup

End


//########################################################################
//
//	Pixie_IO_SetHitPattern:
//		Set hit pattern.
//
//########################################################################
Function Pixie_IO_SetHitPattern()

	//Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_CoincPat = root:pixie4:index_CoincPat
	Variable CoincPattern
	CoincPattern=Display_Module_Parameters[index_CoincPat]
	
	Nvar Coincbit00 = root:pixie4:Coincbit00	// variables for ChanCSRA checkboxes
	Nvar Coincbit01 = root:pixie4:Coincbit01
	Nvar Coincbit02 = root:pixie4:Coincbit02
	Nvar Coincbit03 = root:pixie4:Coincbit03
	Nvar Coincbit04 = root:pixie4:Coincbit04
	Nvar Coincbit05 = root:pixie4:Coincbit05
	Nvar Coincbit06 = root:pixie4:Coincbit06
	Nvar Coincbit07 = root:pixie4:Coincbit07
	Nvar Coincbit08 = root:pixie4:Coincbit08
	Nvar Coincbit09 = root:pixie4:Coincbit09
	Nvar Coincbit10 = root:pixie4:Coincbit10
	Nvar Coincbit11 = root:pixie4:Coincbit11
	Nvar Coincbit12 = root:pixie4:Coincbit12
	Nvar Coincbit13 = root:pixie4:Coincbit13
	Nvar Coincbit14 = root:pixie4:Coincbit14
	Nvar Coincbit15 = root:pixie4:Coincbit15
		
	Coincbit00 = tstbit(0,CoincPattern)	
	Coincbit01 = tstbit(1,CoincPattern)	
	Coincbit02 = tstbit(2,CoincPattern)	
	Coincbit03 = tstbit(3,CoincPattern)	
	Coincbit04 = tstbit(4,CoincPattern)	
	Coincbit05 = tstbit(5,CoincPattern)	
	Coincbit06 = tstbit(6,CoincPattern)	
	Coincbit07 = tstbit(7,CoincPattern)	
	Coincbit08 = tstbit(8,CoincPattern)	
	Coincbit09 = tstbit(9,CoincPattern)	
	Coincbit10 = tstbit(10,CoincPattern)	
	Coincbit11 = tstbit(11,CoincPattern)	
	Coincbit12 = tstbit(12,CoincPattern)	
	Coincbit13 = tstbit(13,CoincPattern)	
	Coincbit14 = tstbit(14,CoincPattern)	
	Coincbit15 = tstbit(15,CoincPattern)	
			
End

//########################################################################
//	Pixie_IO_SelectModChan: Switch between modules or channels
//########################################################################
Function Pixie_IO_SelectModChan(ctrlName,varNum,varStr,varName) : SetVariableControl
String ctrlName
Variable varNum
String varStr
String varName

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar NumberOfModules = root:pixie4:NumberOfModules

	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Nvar EFMinRiseTimeTicks = root:pixie4:EFMinRiseTimeTicks
	Nvar EFMaxRiseTimeTicks = root:pixie4:EFMaxRiseTimeTicks
	Nvar EFMinFlatTopTicks = root:pixie4:EFMinFlatTopTicks
	Nvar EFMaxFlatTopTicks = root:pixie4:EFMaxFlatTopTicks
//	Nvar chEFRTLow = root:auto:chEFRTLow
//	Nvar chEFRTHigh = root:auto:chEFRTHigh
//	Nvar chEFFTLow = root:auto:chEFFTLow
//	Nvar chEFFTHigh = root:auto:chEFFTHigh
//	Nvar chTauLow = root:auto:chTauLow
//	Nvar chTauHigh = root:auto:chTauHigh
//	Nvar chTauStep = root:auto:chTauStep
//	Nvar chTauScanMask = root:auto:chTauScanMask
//	Nvar chEFScanMask = root:auto:chEFScanMask
	Nvar HistoEmin = root:pixie4:HistoEmin
	Nvar HistoDE = root:pixie4:HistoDE
	Nvar NHistoBins = root:pixie4:NHistoBins
			
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
//	Wave RunTasks = root:pixie4:RunTasks
	Wave mcawave = root:pixie4:mcawave
	Wave DSPValues = root:pixie4:DSPValues
//	Wave EFRTLow = root:auto:EFRTLow
//	Wave EFRTHigh = root:auto:EFRTHigh
//	Wave EFFTLow = root:auto:EFFTLow
//	Wave EFFTHigh = root:auto:EFFTHigh
//	wave TauLow = root:auto:TauLow
//	wave TauHigh = root:auto:TauHigh
//	wave TauStep = root:auto:TauStep
	Wave Emink = root:pixie4:Emink
	Wave dxk = root:pixie4:dxk
	Wave Nbink = root:pixie4:Nbink
	
	Nvar index_FilterRange = root:pixie4:index_FilterRange
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	

	Variable dt, tmin, tmax,i,strpos
	String str
	
	if(ChosenModule>(NumberOfModules-1))
		ChosenModule = (NumberOfModules-1)
	endif
	
	
	// Set the current module and channel
	Pixie4_Set_Current_ModChan(ChosenChannel,ChosenModule)
	
	// Update Spectra -- if open, the graph update takes some noticable amount of time, but the data updated itself does not
	if(cmpstr(ctrlName, "SelectedPixie4Module") == 0)
		Pixie_RC_UpdateMCAData("UpdateMCA")
		Pixie_RC_UpdateMCAData("EndRun")
	endif


	// Update Igor environment
	Pixie_IO_SendModParToIgor()
	Pixie_IO_SendChanParToIgor()
	
	// Update Energy Filter time limits
	if(cmpstr(ctrlName, "SelectedPixie4Module") == 0)
		DoWindow/F Pixie_Parameter_Setup
	 	if(V_Flag == 1)
			dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
			SetVariable EGY_ENERGY_RISETIME0, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
			SetVariable EGY_ENERGY_RISETIME1, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
			SetVariable EGY_ENERGY_RISETIME2, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
			SetVariable EGY_ENERGY_RISETIME3, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup
			SetVariable EGY_ENERGY_FLATTOP0, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
			SetVariable EGY_ENERGY_FLATTOP1, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
			SetVariable EGY_ENERGY_FLATTOP2, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup	
			SetVariable EGY_ENERGY_FLATTOP3, limits={-inf,inf,dt}, win=Pixie_Parameter_Setup			
		endif
	endif

	if(cmpstr(ctrlName, "SelectedPixie4Module") != 0)	// only when changing channel
		// Update TraceFFT display
		String wav
		wav="root:pixie4:ADCch"+num2str(ChosenChannel)
		Pixie_FFTtrace($wav)
		DoWindow/F FFTDisplay	
		if (V_Flag==1)
			Switch(ChosenChannel)
				Case 0:
					ModifyGraph rgb(TraceFFT)=(65280,0,0)
					break				
				Case 1:
					ModifyGraph rgb(TraceFFT)=(0,65280,0)
					break
				Case 2:
					ModifyGraph rgb(TraceFFT)=(0,15872,65280)
					break			
				Case 3:
					ModifyGraph rgb(TraceFFT)=(0,26112,0)
					break
			EndSwitch
		endif
	
	
		// Update ADC Filter display
		wav="root:pixie4:ADCch"+num2str(ChosenChannel)
		Pixie_FilterADCTraceCalc($wav)
		DoWindow/F ADCFilterDisplay	
		if (V_Flag==1)
			Switch(ChosenChannel)
				Case 0:
					ModifyGraph rgb(TraceFilter)=(65280,0,0)
					break				
				Case 1:
					ModifyGraph rgb(TraceFilter)=(0,65280,0)
					break
				Case 2:
					ModifyGraph rgb(TraceFilter)=(0,15872,65280)
					break			
				Case 3:
					ModifyGraph rgb(TraceFilter)=(0,26112,0)
					break
			EndSwitch
		endif
	endif
	
	// Update Digital Filter (list mode trace) display
	DoWindow/F Pixie4FilterDisplay
	if(V_Flag == 1)
		Pixie_FilterLMTraceCalc()
	endif
		
//	// Update energy filter auto optimization limits
//	chEFRTLow = EFRTLow[ChosenChannel]
//	chEFRTHigh = EFRTHigh[ChosenChannel]
//	chEFFTLow = EFFTLow[ChosenChannel]
//	chEFFTHigh = EFFTHigh[ChosenChannel]
//
//	// Update auto optimization limits
//	chTauLow = TauLow[ChosenChannel]
//	chTauHigh = TauHigh[ChosenChannel]
//	chTauStep = TauStep[ChosenChannel]
//
//	// Update controls
//	DoWindow/F AutoEFOptimizationPanel
//	if (V_flag==1)
//		dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
//		SetVariable FTScanLowerLimit,limits={EFMinFlatTopTicks*dt,EFMaxFlatTopTicks*dt,dt},win=AutoEFOptimizationPanel
//		SetVariable FTScanUpperLimit,limits={EFMinFlatTopTicks*dt,EFMaxFlatTopTicks*dt,dt},win=AutoEFOptimizationPanel
//		SetVariable RTScanLowerLimit,limits={EFMinRiseTimeTicks*dt,EFMaxRiseTimeTicks*dt,dt},win=AutoEFOptimizationPanel
//		SetVariable RTScanUpperLimit,limits={EFMinRiseTimeTicks*dt,EFMaxRiseTimeTicks*dt,dt},win=AutoEFOptimizationPanel
//		CheckBox DoScanThisChEF,value= TSTbit(ChosenChannel, chEFScanMask), win=AutoEFOptimizationPanel
//	endif
//	
//	DoWindow/F AutoTauOptimizationPanel
//	if (V_flag==1)
//		CheckBox DoScanThisChTau,value= TSTbit(ChosenChannel, chTauScanMask), win=AutoTauOptimizationPanel
//	endif
	
	// Update histogram controls on list mode spectrum display
	HistoEmin = Emink[ChosenChannel]
	HistoDE = dxk[ChosenChannel]
	NHistoBins = Nbink[ChosenChannel]
	
	//////////////////////////////////////////////////////////// 
	// Call to user routine			             //
		User_ChangeChannelModule()
	////////////////////////////////////////////////////////////	

End


//########################################################################
//
//	Retrieve DSP I/O parameter names from the DSP .var file
//
//########################################################################
Function Pixie_IO_GetDSPNames()

	Wave/T DSPNames = root:pixie4:DSPNames
	Svar DSPVarFile4 = root:pixie4:DSPVarFile4
	String line
	Variable filin,k,len
	
	Open/R/P=DSPPath filin as DSPVarFile4
	k=0
	do
		FReadLine filin, line
		len=strlen(line)
		if(len>6)
			DSPNames[k]=line[5, (len-2)]
		else
			DSPNames[k]=""
		endif
		k+=1
	 	Fstatus filin
	while(V_logEOF>V_filepos)
	
End


//########################################################################
//
//	Retrieve DSP memory parameter names from the DSP .lst file
//
//########################################################################
Function Pixie_IO_GetMemoryNames()

	Wave/T MemoryNames4 = root:pixie4:MemoryNames4
	Wave/T MemoryNames500e = root:pixie4:MemoryNames500e
	Svar DSPListFile4 = root:pixie4:DSPListFile4
	Svar DSPListFile500e = root:pixie4:DSPListFile500e
	String line
	Variable filin,index,len
	
	MemoryNames4=""	// Clear MemoryNames	
	Open/R/P=DSPPath filin as DSPListFile4
	do
		FReadLine filin, line
		len=strlen(line)
		index=str2num(line[0,4])
		MemoryNames4[index]=line[7, (len-2)]
	 	Fstatus filin
	while(V_logEOF>V_filepos)
	close filin
	
	MemoryNames500e=""	// Clear MemoryNames
	Open/R/P=DSPPath filin as DSPListFile500e
	do
		FReadLine filin, line
		len=strlen(line)
		index=str2num(line[0,4])
		MemoryNames500e[index]=line[7, (len-2)]
	 	Fstatus filin
	while(V_logEOF>V_filepos)
	close filin
	
End


//########################################################################
//
//	Pixie_FilterLMTraceCalc:
//		Calculate digital trapezoidal filter response of a list mode trace.
//
//########################################################################
Function Pixie_FilterLMTraceCalc()


	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar wftimescale = root:pixie4:wftimescale			// sampling interval in MHz as read from the file
	
	Nvar index_SL = root:pixie4:index_SL
	Nvar index_SG = root:pixie4:index_SG
	Nvar index_FL = root:pixie4:index_FL
	Nvar index_FG = root:pixie4:index_FG	
	Nvar index_TH = root:pixie4:index_TH
	Nvar index_INTEGRATOR = root:pixie4:index_INTEGRATOR
	Nvar ncp =  root:pixie4:NumChannelPar	
	
	Wave sf=root:pixie4:sf
	Wave ff=root:pixie4:ff
	Wave seltrace=root:pixie4:seltrace
	Wave SFmarkers=root:pixie4:SFmarkers
	Wave th = root:pixie4:th

	Variable k, dt, len, rate, s0,m, s1, s2, s3
	Variable SL,SG,FL,FG, INT

	String wav
	wav="root:pixie4:Trace"+num2str(ChosenChannel)
	//Duplicate/o $wav,root:pixie4:seltrace,root:pixie4:sf,root:pixie4:ff,root:pixie4:th,root:pixie4:sfmarkers
	wave Trace = $(wav)
	
	len=numpnts(Trace)
	// can not use duplicate since source wave is unsigned integer and results are floats
	make/o/n=(len) root:pixie4:seltrace, root:pixie4:sf, root:pixie4:ff, root:pixie4:th, root:pixie4:sfmarkers,root:pixie4:cfd
	Wave seltrace=root:pixie4:seltrace
	Wave sf=root:pixie4:sf
	Wave ff=root:pixie4:ff
	Wave cfd=root:pixie4:cfd
	Wave sfmarkers=root:pixie4:sfmarkers
	Wave th = root:pixie4:th
	CopyScales Trace, seltrace, sf, ff, sfmarkers, th, cfd
	seltrace = Trace
	ff  = Trace
	sf  = Trace
	th  = Trace
	cfd  = Trace
	sfmarkers = Nan
	rate = 1e-6/wftimescale
	
	SL=Display_Channel_Parameters[index_SL+ncp*ChosenChannel]*rate
	SG=Display_Channel_Parameters[index_SG+ncp*ChosenChannel]*rate
	FL=Display_Channel_Parameters[index_FL+ncp*ChosenChannel]*rate
	FG=Display_Channel_Parameters[index_FG+ncp*ChosenChannel]*rate
	INT = Display_Channel_Parameters[index_INTEGRATOR+ncp*ChosenChannel]
	
	Variable FLpFG
	FLpFG = FL + FG

	Variable ndat, off, x0,x1,x2,x3
	Variable cfd_B = 4
	Variable cfd_D = 4
	
	// slow filter *************************************************************************
	off=2*SL+SG-1
	ndat=numpnts(seltrace)
	if(ndat > 0)  // we won't calculate digital filter value for null trace
		sf=nan
		k=off
		do
			if(INT!=1)		// INT==0 is so simplified (no gap, no tau) that it is the same as INT=2
				s0=0
				for(m=0;m<SL;m+=1)
					s0 -= seltrace[k+m-off]
					s0 += seltrace[SL+SG+k+m-off]
				endfor
//				x0=pnt2x(seltrace,k+SL+SG-off)
//				x1=pnt2x(seltrace,k+SG+2*SL-1-off)
//				x2=pnt2x(seltrace,k-off)
//				x3=pnt2x(seltrace,k+SL-1-off)
//				sf[k]=sum(seltrace,x0,x1)-sum(seltrace,x2,x3)
				sf[k]=s0
				
			else
				s0=0
				for(m=0;m<SG;m+=1)
					s0 += seltrace[k+m-SG]
				endfor
				sf[k]=s0
			endif
//	if(k>=127)
//		print x0,x1,x2,x3, sum(seltrace,x0,x1), sum(seltrace,x2,x3), sf[k]
//	endif
			
			k+=1
		while(k<ndat)
		
		// fast filter *************************************************************************
		off=2*FL+FG-1
		ff=nan
		k=off
		do
			s0=0
			for(m=0;m<FL;m+=1)
				s0 -= seltrace[k+m-off]
				s0 += seltrace[FL+FG+k+m-off]
			endfor
			ff[k]=s0
//			x0=pnt2x(seltrace,k+FLpFG-off)
//			x1=pnt2x(seltrace,k+FLpFG+FL-1-off)
//			x2=pnt2x(seltrace,k-off)
//			x3=pnt2x(seltrace,k+FL-1-off)
//			ff[k]=sum(seltrace,x0,x1)-sum(seltrace,x2,x3)
			k+=1
		while(k<ndat)
		
		// CFD *************************************************************************
		off=cfd_B+cfd_D+4
		cfd=nan
		k=off
		do

			s0 = seltrace[k]+seltrace[k-1]							// sample avg (length L = 2)
			s1 =  seltrace[k-cfd_B]+seltrace[k-cfd_B-1]				// sample avg (delayed)
			s2 =  seltrace[k-cfd_D]+seltrace[k-cfd_D-1]				
			s3 =  seltrace[k-cfd_B-cfd_D]+seltrace[k-cfd_B-cfd_D-1]	
			
//			k-=2
//			s0 += seltrace[k]+seltrace[k-1]							// add to sample avg (length L = 4)
//			s1 +=  seltrace[k-cfd_B]+seltrace[k-cfd_B-1]				// sample avg (delayed)
//			s2 +=  seltrace[k-cfd_D]+seltrace[k-cfd_D-1]				
//			s3 +=  seltrace[k-cfd_B-cfd_D]+seltrace[k-cfd_B-cfd_D-1]	
//			k+=2

			cfd[k]=((s0-s1) - (s2-s3))/2
			k+=1
		while(k<ndat)
		
	endif

	Variable THfactor
	if(rate>150)
		THfactor = 4
	else
		THfactor = 16
	endif
	th = Display_Channel_Parameters[index_TH+ncp*ChosenChannel]*THfactor*(FL)
	SFmarkers = NaN
	
End




//########################################################################
//
//	Pixie_FilterTrace:
//		Display filters and threshold on an ADC trace.
//
//########################################################################
Function Pixie_FilterADCTraceCalc(Trace)
Wave Trace

	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar FilterClockMHz=root:pixie4:FilterClockMHz
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_SL = root:pixie4:index_SL
	Nvar index_SG = root:pixie4:index_SG
	Nvar index_FL = root:pixie4:index_FL
	Nvar index_FG = root:pixie4:index_FG	
	Nvar index_TH = root:pixie4:index_TH
	Nvar index_XDT = root:pixie4:index_XDT
	Nvar ncp =  root:pixie4:NumChannelPar	
	Nvar ModuleType = root:pixie4:ModuleType


	Variable len,df
	
	len=numpnts(Trace)
	// can not use duplicate since source wave is unsigned integer and results are floats
	make/o/n=(len) root:pixie4:TraceFilter, root:pixie4:TraceFilterSF, root:pixie4:TraceFilterFF, root:pixie4:TraceFilterSFMarkers, root:pixie4:TraceTH, root:pixie4:TraceGate
	Wave TraceFilter=root:pixie4:TraceFilter
	Wave TraceFilterSF=root:pixie4:TraceFilterSF
	Wave TraceFilterFF=root:pixie4:TraceFilterFF
	Wave TraceFilterSFMarkers=root:pixie4:TraceFilterSFMarkers
	Wave TraceTH = root:pixie4:TraceTH
	Wave TraceGate = root:pixie4:TraceGate
	CopyScales Trace, TraceFilter, TraceFilterSF, TraceFilterFF, TraceFilterSFMarkers, TraceTH, TraceGate
	TraceFilter = Trace
	TraceFilterFF  = Trace
	TraceFilterSF  = Trace
	TraceTH  = Trace
	TraceFilterSFMarkers = Nan
	TraceGate = Trace & 0x0001
	
	
	Variable k, dt, xdt
	Variable SL,SG,FL,FG
	
	xdt = Display_Channel_Parameters[index_XDT+ncp*ChosenChannel]
	
	SL=Display_Channel_Parameters[index_SL+ncp*ChosenChannel]/xdt
	SG=Display_Channel_Parameters[index_SG+ncp*ChosenChannel]/xdt
	FL=Display_Channel_Parameters[index_FL+ncp*ChosenChannel]/xdt 
	FG=Display_Channel_Parameters[index_FG+ncp*ChosenChannel]/xdt


		
	Variable FLpFG
	FLpFG = FL + FG
	
	

	Variable ndat, off, x0,x1,x2,x3
	off=2*SL+SG-1
	ndat=numpnts(TraceFilter)
	if(ndat > 0)  // we won't calculate digital filter value for null trace
		TraceFilterSF=nan
		k=off
		do
			x0=pnt2x(TraceFilter,k+SL+SG-off)
			x1=pnt2x(TraceFilter,k+SG+2*SL-1-off)
			x2=pnt2x(TraceFilter,k-off)
			x3=pnt2x(TraceFilter,k+SL-1-off)
			TraceFilterSF[k]=sum(TraceFilter,x0,x1)-sum(TraceFilter,x2,x3)
			k+=1
		while(k<ndat)
		TraceFilterSF*=xdt/0.0133
	
		off=2*FL+FG-1
		TraceFilterFF=nan
		k=off
		do
			x0=pnt2x(TraceFilter,k+FLpFG-off)
			x1=pnt2x(TraceFilter,k+FLpFG+FL-1-off)
			x2=pnt2x(TraceFilter,k-off)
			x3=pnt2x(TraceFilter,k+FL-1-off)
			TraceFilterFF[k]=sum(TraceFilter,x0,x1)-sum(TraceFilter,x2,x3)
			k+=1
		while(k<ndat)
		TraceFilterFF*=xdt*FilterClockMHz
	endif
	if(ModuleType ==500)
		TraceTH = Display_Channel_Parameters[index_TH+ncp*ChosenChannel]* (FL)*xdt*FilterClockMHz 
	else
		TraceTH = Display_Channel_Parameters[index_TH+ncp*ChosenChannel]* (FL)*xdt*FilterClockMHz *4 //(TriggerRiseTime/13.33e-3)*8/FL
	endif
		
//	print FL, FG, FLpFG, xdt, ncp, index_TH+ncp*ChosenChannel
End

// threshold entered in control panel is about 1/4 of amplitude that is still just triggered on, but only for a square pulse (fast rise time, long decay, TF flat top large)


//########################################################################
//
//	Pixie_Math_SumHisto:
//		Sum counts under the MCA histogram (minus background).
//
//########################################################################
Function Pixie_Math_SumHisto(ctrlName,popNum,popStr): PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr
	
	Wave MCAStartFitChannel=root:pixie4:MCAStartFitChannel
	Wave MCAEndFitChannel=root:pixie4:MCAEndFitChannel
	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
	Wave MCAChannelPeakEnergy=root:pixie4:MCAChannelPeakEnergy
	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
	Wave MCAChannelFWHMAbsolute=root:pixie4:MCAChannelFWHMAbsolute
	Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea
		
	Wave ListStartFitChannel=root:pixie4:ListStartFitChannel
	Wave ListEndFitChannel=root:pixie4:ListEndFitChannel
	Wave ListChannelPeakPos=root:pixie4:ListChannelPeakPos
	Wave ListChannelFWHMPercent=root:pixie4:ListChannelFWHMPercent
	Wave ListChannelPeakArea=root:pixie4:ListChannelPeakArea				
	
	String waveCursorAIsOn, waveCursorBIsOn, wav
	Variable len,a,b,de,bckgnd,ChanNum, ListNum
	
	waveCursorAIsOn = CsrWave(A)
	waveCursorBIsOn = CsrWave(B)

	if(cmpstr(ctrlName, "SumHistoMCAch") == 0)
		ListNum=0
		do
		
			// 0..4 normal channels and ref spectrum 
				ChanNum=ListNum							
				wav="root:pixie4:MCAch"+num2str(ChanNum)
			if (ListNum==5)	
				ChanNum= 0	
				wav="root:pixie4:MCAsum"		// addback
			endif
			if (ListNum>5)						// cumulative
				ChanNum= ListNum-6					// 
				wav = "root:pixie4:MCAtotal"+num2str(ChanNum)
			endif

		
			//if cursor is anywhere (any channel) on graph, use it as fit limit for selected channel
			// else use min/max set in user control for selected channel
			variable xa, xb
			if(cmpstr(CsrWave(A),"")==0)
			 	xa=MCAStartFitChannel[ListNum]
			 else 
			 	xa = xcsr(A)
			 endif
			
			 if(cmpstr(CsrWave(B),"")==0)
			 	xb=MCAEndFitChannel[ListNum]
			 else 
			 	xb = xcsr(B)
			 endif
			
			if(xa <= xb)
				MCAStartFitChannel[ListNum]=xa
				MCAEndFitChannel[ListNum]=xb
			else
				MCAStartFitChannel[ListNum]=xb
				MCAEndFitChannel[ListNum]=xa
			endif
			xa=MCAStartFitChannel[ListNum]		// use xa, xb as shorthand
			xb=MCAEndFitChannel[ListNum]
				
			wavestats/q $wav
			if (popNum==3) // sum all
				xa = 0
				xb=(V_npnts)*deltax($wav)-1
			endif
		
			// compute area
			MCAChannelPeakArea[ListNum]=sum($wav,xa,xb)		
			
			// determine FWHM
			WaveStats/Q/R=(xa,xb) $wav
			FindLevel/q  /R=(V_maxLoc,xa) $wav,(V_min+(V_max-V_min)/2)
			a=V_LevelX
			FindLevel/q  /R=(V_maxLoc,xb) $wav,(V_min+(V_max-V_min)/2)		
			b=V_LevelX
			MCAChannelFWHMAbsolute[ListNum]=(b-a)
			MCAChannelFWHMPercent[ListNum]=(b-a)/V_maxLoc*100
			
			// subtract background
			de=deltax($wav)
			a=sum($wav,xa,xa+2*de)/3	// average around start
			b=sum($wav,xb,xb-2*de)/3		// average around end
			bckgnd=(b+a)/2*(xb-xa)/de		// compute trapezoid under averaged end points
			if((popNum==2) || (popNum==3))
				bckgnd=0
			endif
			MCAChannelPeakArea[ListNum]-=bckgnd
			MCAChannelPeakPos[ListNum]=V_maxloc
			MCAChannelPeakEnergy[ListNum]=MCAChannelPeakEnergy[ListNum]		

	ListNum+=1
	while (ListNum<10)		// include ref
	Pixie_MakeList_MCA(1)	



	elseif(cmpstr(ctrlName, "SumHistoListModech") == 0)
		ChanNum=0
		do
			//if cursor is anywhere (any channel) on graph, use it as fit limit for selected channel
			// else use min/max set in user control for selected channel
			if(cmpstr(CsrWave(A),"")==0)
			 	xa=ListStartFitChannel[ChanNum]
			 else 
			 	xa = xcsr(A)
			 endif
			
			 if(cmpstr(CsrWave(B),"")==0)
			 	xb=ListEndFitChannel[ChanNum]
			 else 
			 	xb = xcsr(B)
			 endif
			
			if(xa <= xb)
				ListStartFitChannel[ChanNum]=xa
				ListEndFitChannel[ChanNum]=xb
			else
				ListStartFitChannel[ChanNum]=xb
				ListEndFitChannel[ChanNum]=xa
			endif
			xa=ListStartFitChannel[ChanNum]	// use xa, xb as shorthand
			xb=ListEndFitChannel[ChanNum]
				
			wav="root:pixie4:Spectrum"+num2str(ChanNum)
			wavestats/q $wav
			if (popNum==3) // sum all
				xa = 0
				xb=(V_npnts)*deltax($wav)-1
			endif
			
			// compute area
			ListChannelPeakArea[ChanNum]=sum($wav,xa,xb)		
			
			// determine FWHM
			WaveStats/Q/R=(xa,xb) $wav
			FindLevel/q  /R=(V_maxLoc,xa) $wav,(V_min+(V_max-V_min)/2)
			a=V_LevelX
			FindLevel/q  /R=(V_maxLoc,xb) $wav,(V_min+(V_max-V_min)/2)		
			b=V_LevelX
			ListChannelFWHMPercent[ChanNum]=(b-a)/V_maxLoc*100			
			
			// subtract background
			de=deltax($wav)
			a=sum($wav,xa,xa+2*de)/3
			b=sum($wav,xb,xb-2*de)/3
			bckgnd=(b+a)/2*(xb-xa)/de
			if((popNum==2) || (popNum==3))
				bckgnd=0
			endif
			ListChannelPeakArea[ChanNum]-=bckgnd
			ListChannelPeakPos[ChanNum]=V_maxloc		
			ChanNum+=1
		while (Channum<5)		// include ref
		Pixie_MakeList_LMHisto()
	endif
End

//########################################################################
//
//	Pixie_Math_GaussFit:
//		Gaussian fit of energy spectrum.
//
//########################################################################
Function Pixie_Math_GaussFit(ctrlName,popNum,popStr) : PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr

	Wave MCAStartFitChannel=root:pixie4:MCAStartFitChannel
	Wave MCAEndFitChannel=root:pixie4:MCAEndFitChannel
	Wave MCAFitRange=root:pixie4:MCAFitRange
	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
	Wave MCAChannelPeakEnergy=root:pixie4:MCAChannelPeakEnergy
	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
	Wave MCAChannelFWHMAbsolute=root:pixie4:MCAChannelFWHMAbsolute
	Wave MCAChannelPeakArea=root:pixie4:MCAChannelPeakArea
	
	Wave ListStartFitChannel=root:pixie4:ListStartFitChannel
	Wave ListEndFitChannel=root:pixie4:ListEndFitChannel
	Wave ListChannelPeakPos=root:pixie4:ListChannelPeakPos
	Wave ListChannelFWHMPercent=root:pixie4:ListChannelFWHMPercent
	Wave ListChannelPeakArea=root:pixie4:ListChannelPeakArea				
		
	Nvar MCAfitOption =  root:pixie4:MCAfitOption
	
	String waveCursorAIsOn, waveCursorBIsOn, wavname, foldername, wvn
	Variable ChanNum, EndNum, xa, xb, peakloc, xtemp, Listnum
	foldername="root:pixie4:"
	
	ChanNum=popNum-1					// channel number in loop and wave ref
	EndNum=ChanNum+1				// end of loop number
	Listnum=ChanNum					// entry in the MCA resutls array
	wavname = "MCAch"
	if (popNum==5)	
		ChanNum= 0	
		EndNum = 4						// run through 0..3
		Listnum=0
		wavname = "MCAch"
	endif
	if (popNum==6)
		ChanNum= 4						// "channel 4" is the ref spectrum
		EndNum=ChanNum+1
		Listnum=4
		wavname = "MCAch"
	endif
	if (popNum==7)						// 7 is addback
		ChanNum= -1					// indicate no channel suffix
		EndNum=ChanNum+1
		Listnum=5
		wavname = "MCAsum"
	endif
	if (popNum>7)
		ChanNum=popNum-8
		EndNum=ChanNum+1
		Listnum= ChanNum+6
		wavname = "MCAtotal"
	endif


	if(cmpstr(ctrlName, "GaussFitMCA") == 0)


		DoWindow/F MCASpectrumDisplay
		do	
				 
			Wave W_Coef=W_Coef
			if(ChanNum>=0)
				wvn = foldername+wavname+num2str(ChanNum)
			else
				wvn = foldername+wavname
			endif
			wave wav =$(wvn)
			wavestats/q wav
				
			if(MCAfitOption==1)
				peakloc = V_maxloc	//in x
				//xa = x2pnt(wav,peakloc*(1-MCAFitRange[Listnum]/100))
				//xb = x2pnt(wav,peakloc*(1+MCAFitRange[Listnum]/100))	 
				xa = peakloc*(1-MCAFitRange[Listnum]/100)
				xb = peakloc*(1+MCAFitRange[Listnum]/100)	 
			endif
			
			if(MCAfitOption==2)
				findlevel/q/R=[32767,0] wav, V_max/10
				peakloc = V_LevelX	//in x
				//xa = x2pnt(wav,peakloc*(1-MCAFitRange[Listnum]/100))
				//xb = x2pnt(wav,peakloc*(1+MCAFitRange[Listnum]/100))
				xa = peakloc*(1-MCAFitRange[Listnum]/100)
				xb = peakloc*(1+MCAFitRange[Listnum]/100)	 	 
			endif
			
			if(MCAfitOption==3)
				xa = MCAStartFitChannel[Listnum]
				xb = MCAEndFitChannel[Listnum]
			endif
			
			if(MCAfitOption==4)
				//if cursor is anywhere (any channel) on graph, use it as fit limit for selected channel
				// else use min/max set in user control for selected channel
				if(cmpstr(CsrWave(A),"")==0)
				 	xa=MCAStartFitChannel[Listnum]
				 else 
				 	xa = xcsr(A)
				 endif
				
				 if(cmpstr(CsrWave(B),"")==0)
				 	xb=MCAEndFitChannel[Listnum]
				 else 
				 	xb = xcsr(B)
				 endif
				
				if(xa > xb)
					xtemp=xa
					xa=xb
					xb=xtemp
				endif
			endif
			
			W_Coef = nan
			CurveFit/Q gauss wav(xa,xb) /D
			MCAChannelPeakPos[Listnum]=W_Coef[2]
			MCAChannelPeakEnergy[Listnum]=MCAChannelPeakPos[Listnum]
			MCAChannelFWHMPercent[Listnum]=100*W_coef[3]*2*sqrt(ln(2))/W_coef[2]
			MCAChannelFWHMAbsolute[Listnum]=W_coef[3]*2*sqrt(ln(2))
			MCAChannelPeakArea[Listnum]=W_coef[3]*W_coef[1]*sqrt(Pi)/deltax(wav)
			
			ChanNum+=1
			Listnum+=1
		while (ChanNum<EndNum)
		
		Pixie_MakeList_MCA(1)
		
		
		
	elseif(cmpstr(ctrlName, "GaussFitListModech") == 0)
		do	
			//if cursor is anywhere (any channel) on graph, use it as fit limit for selected channel
			// else use min/max set in user control for selected channel
			if(cmpstr(CsrWave(A),"")==0)
			 	xa=ListStartFitChannel[Listnum]
			 else 
			 	xa = xcsr(A)
			 endif
			
			 if(cmpstr(CsrWave(B),"")==0)
			 	xb=ListEndFitChannel[Listnum]
			 else 
			 	xb = xcsr(B)
			 endif
			
			if(xa <= xb)
				ListStartFitChannel[Listnum]=xa
				ListEndFitChannel[Listnum]=xb
			else
				ListStartFitChannel[Listnum]=xb
				ListEndFitChannel[Listnum]=xa
			endif
		
			wavname="root:pixie4:Spectrum"+num2str(ChanNum)
			wave wav =$wavname
			Wave W_Coef=W_Coef
			CurveFit/Q gauss wav(ListStartFitChannel[Listnum],ListEndFitChannel[Listnum]) /D
			ListChannelPeakPos[Listnum]=W_Coef[2]
			ListChannelFWHMPercent[Listnum]=100*W_coef[3]*2*sqrt(ln(2))/W_coef[2]
			ListChannelPeakArea[Listnum]=W_coef[3]*W_coef[1]*sqrt(Pi)/deltax(wav)	
			
			ChanNum+=1
			Listnum+=1
		while (ChanNum<EndNum)
	
		Pixie_MakeList_LMHisto()
			
	endif
End



//########################################################################
//
//	Pixie_Math_TauFit:
//		Single exponential fit of a ADC trace.
//
//########################################################################
Function Pixie_Math_TauFit(channel) 
Variable channel
	
	Wave TauTrace=$("root:pixie4:ADCch"+num2str(channel)) 
	Wave TauReswave=$("Res_ADCch"+num2str(channel)) 
	Wave TauFitwave=$("fit_ADCch"+num2str(channel)) 
	Nvar LastTau=root:pixie4:LastTau
	Nvar TauDeviation=root:pixie4:TauDeviation
	Variable xa,xb
	
	// ensure target waves exist and are clean
	duplicate/o TauTrace, TauReswave
	duplicate/o TauTrace, TauFitwave
	TauReswave = 0
	TauFitwave = nan
	
	//if cursor is anywhere on graph, use it as fit limit
	if( (cmpstr(CsrWave(A),"")==0) || (cmpstr(CsrWave(B),"")==0) )
		DoAlert 0, "Please define the fit range with the cursors"
		ShowInfo
		return(-1)
	endif
	xa = xcsr(A)/1e-6
	xb = xcsr(B)/1e-6
	
	CurveFit/Q exp_XOffset TauTrace(xa*1e-6,xb*1e-6) /D/R
	LastTau=1e6/K2
	
	Wave W_sigma=W_sigma
	Wave W_coef=W_coef
	TauDeviation=W_sigma[2]*1e6
	LastTau=W_coef[2]*1e6
End



//########################################################################
//
//	Pixie_FilterFFT:
//		Simulate the effect of energy filter on the FFT noise spectrum.
//
//########################################################################
Function Pixie_FilterFFT()

	Wave TraceFFT=root:pixie4:TraceFFT
	Nvar ChosenChannel=root:pixie4:ChosenChannel
	Nvar FilterClockMHz=root:pixie4:FilterClockMHz
	
	// New parameter modification scheme
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_SL = root:pixie4:index_SL
	Nvar index_SG = root:pixie4:index_SG
	Nvar ncp = root:pixie4:NumChannelPar
	Variable EnergyRiseTime = Display_Channel_Parameters[index_SL+ChosenChannel*ncp]
	Variable EnergyFlatTop = Display_Channel_Parameters[index_SG+ChosenChannel*ncp]
	Variable L,G,dt,dec
	String cc,wav

	cc=num2str(ChosenChannel)
	wav = "root:pixie4:ADCch"+cc
	Pixie_FFTtrace($wav)	 // recompute before applying a different filter
	
	TraceFFT*=2*abs(sin(Pi*x*EnergyRiseTime)*sin(Pi*x*(EnergyRiseTime+EnergyFlatTop))/((EnergyRiseTime*FilterClockMHz)*sin(Pi*x*1e-6/FilterClockMHz)))
	TraceFFT[0]=0

End


//########################################################################
//
//	Pixie_FFTtrace:
//		Calculate the FFT noise spectrum of the ADC trace.
//
//########################################################################
Function Pixie_FFTtrace(Trace)
Wave Trace

	
	Nvar FFTbin=root:pixie4:FFTbin
	Variable len,df
	Duplicate/o Trace, TauFFT
	len=numpnts(TauFFT)
	if(mod(len,2)==1)
		DeletePoints len-2,1, TauFFT
	endif
	len/=2
	make/o/n=(len) root:pixie4:TraceFFT
	Wave TraceFFT=root:pixie4:TraceFFT
	
	FFT/DEST=TauFFT Trace
	TraceFFT=sqrt(magsqr(TauFFT))/len // normalize to peak amplitude
	df=deltax(TauFFT)
	CopyScales TauFFT, TraceFFT	 // copy scales halves df
	SetScale/P x,0,df, TraceFFT
	TraceFFT[0]=0
	DeletePoints len,len, root:pixie4:TraceFFT
	FFTbin=df
	KillWaves TauFFT
	
End



//########################################################################
//
//	Pixie_Series_Process:
//		Extract key settings and resolutions from a series of MCA runs
//
//########################################################################
Function Pixie_Series_Process()

	Svar basename=  root:auto:basename 
	Nvar von = root:auto:von
	Nvar bis = root:auto:bis

	make/o/n=(bis+1) root:results:tau0, root:results:peak0, root:results:Tgap0, root:results:Tpeak0, root:results:FWHM0
	make/o/n=(bis+1) root:results:tau1, root:results:peak1, root:results:Tgap1, root:results:Tpeak1, root:results:FWHM1
	make/o/n=(bis+1) root:results:tau2, root:results:peak2, root:results:Tgap2, root:results:Tpeak2, root:results:FWHM2
	make/o/n=(bis+1) root:results:tau3, root:results:peak3, root:results:Tgap3, root:results:Tpeak3, root:results:FWHM3
	make/o/n=(bis+1) root:results:runnumber
	
	wave tau0 = root:results:tau0
	wave peak0 = root:results:peak0
	wave Tgap0 = root:results:Tgap0
	wave Tpeak0 = root:results:Tpeak0
	wave FWHM0 = root:results:FWHM0
	FWHM0 = nan
	tau0 = nan
	peak0 = nan
	Tgap0 = nan
	Tpeak0 = nan
	
	wave tau1 = root:results:tau1
	wave peak1 = root:results:peak1
	wave Tgap1 = root:results:Tgap1
	wave Tpeak1 = root:results:Tpeak1
	wave FWHM1 = root:results:FWHM1
	FWHM1 = nan
	tau1 = nan
	peak1 = nan
	Tgap1=nan
	Tpeak1=nan
		
	wave tau2 = root:results:tau2
	wave peak2 = root:results:peak2
	wave Tgap2 = root:results:Tgap2
	wave Tpeak2 = root:results:Tpeak2
	wave FWHM2 = root:results:FWHM2
	FWHM2 = nan
	tau2 = nan
	peak2 = nan
	Tgap2=nan
	Tpeak2=nan
	
	wave tau3 = root:results:tau3
	wave peak3 = root:results:peak3
	wave Tgap3 = root:results:Tgap3
	wave Tpeak3 = root:results:Tpeak3
	wave FWHM3 = root:results:FWHM3
	FWHM3 = nan
	tau3 = nan
	peak3 = nan
	Tgap3=nan
	Tpeak3=nan
	
	wave runnumber = root:results:runnumber
	runnumber = nan
	
	Variable k, maxloc, filenum,m, j
	String filename, runn, line
	Variable v0,v1,v2,v3, chan
	
	Wave MCAch0 = root:pixie4:MCAch0
	Wave MCAch1 = root:pixie4:MCAch1
	Wave MCAch2 = root:pixie4:MCAch2
	Wave MCAch3 = root:pixie4:MCAch3
	Wave MCAsum = root:pixie4:MCAsum
	Nvar CloverAdd = root:pixie4:CloverAdd
	Nvar ChosenModule = root:pixie4:ChosenModule
	Wave MCAChannelPeakPos=root:pixie4:MCAChannelPeakPos
	Wave MCAChannelFWHMPercent=root:pixie4:MCAChannelFWHMPercent
	duplicate/o MCAch0, root:pixie4:MCAtotal0
	duplicate/o MCAch1, root:pixie4:MCAtotal1
	duplicate/o MCAch2, root:pixie4:MCAtotal2
	duplicate/o MCAch3, root:pixie4:MCAtotal3
	duplicate/o MCAsum, root:pixie4:MCAtotalsum
	Wave MCAtotal0 = root:pixie4:MCAtotal0
	Wave MCAtotal1 = root:pixie4:MCAtotal1
	Wave MCAtotal2 = root:pixie4:MCAtotal2
	Wave MCAtotal3 = root:pixie4:MCAtotal3
	Wave MCAtotalsum = root:pixie4:MCAtotalsum
	MCAtotal0 = 0
	MCAtotal1 = 0
	MCAtotal2 = 0
	MCAtotal3 = 0
	MCAtotalsum =0
	
	Nvar  chan0 = root:auto:chan0
	Nvar  chan1 = root:auto:chan1
	Nvar  chan2 = root:auto:chan2
	Nvar  chan3 = root:auto:chan3
	Svar MCAsource = root:pixie4:MCASource
	
	variable MCA16nSum, RunType
	Wave RunTasks = root:pixie4:RunTasks
	Nvar WhichRun = root:pixie4:WhichRun
	RunType=RunTasks[WhichRun-1]
	MCA16nSum = ( (RunType==0x402) || (CloverAdd==1) )
		
	Execute " Pixie_Plot_MCADisplay()"	// open window (or bring to front) so that fit cursors from MCA are used
	for(k=von;k<=bis;k+=1)
	
		runnumber[k] = k
		
		// load MCA
		sprintf runn,"%04d",k
		filename = basename+runn+".mca"
		GBLoadWave/Q/O/B/N=dummy/T={96,96}/W=1/p=MCAPath filename
		MCASource = S_fileName
		
		Wave dummy0=dummy0
	
		
		wavestats/q dummy0
		if (V_npnts < (4* 32768*ChosenModule))
			DoAlert 0, "There is no data in the file for module "+num2str(ChosenModule)
		else
			if (MCA16nSum==1)	//Addback spectrum
				MCAch0 = dummy0[p+4*32768*ChosenModule]
				MCAch1 = dummy0[p+16384+4*32768*ChosenModule]
				MCAch2 = dummy0[p+2*16384+4*32768*ChosenModule]
				MCAch3 = dummy0[p+3*16384+4*32768*ChosenModule]
				MCAsum= dummy0[p+4*16384+4*32768*ChosenModule]
			else
				MCAch0 = dummy0[p+4*32768*ChosenModule]
				MCAch1 = dummy0[p+32768+4*32768*ChosenModule]
				MCAch2 = dummy0[p+2*32768+4*32768*ChosenModule]
				MCAch3 = dummy0[p+3*32768+4*32768*ChosenModule]
				MCAsum=0
			endif
		endif
		MCAtotal0 +=  MCAch0
		MCAtotal1 +=  MCAch1
		MCAtotal2 +=  MCAch2
		MCAtotal3 +=  MCAch3
		MCAtotalsum +=  MCAsum
	
		// fit and store results
		if (chan0)
			Pixie_Math_GaussFit("GaussFitMCA",1,"")
			peak0[k] =MCAChannelPeakPos[0]
			FWHM0[k] = MCAChannelFWHMPercent[0]
		endif
		
		if (chan1)
			Pixie_Math_GaussFit("GaussFitMCA",2,"")
			peak1[k] =MCAChannelPeakPos[1]
			FWHM1[k] = MCAChannelFWHMPercent[1]
		endif
		
		if (chan2)
			Pixie_Math_GaussFit("GaussFitMCA",3,"")
			peak2[k] =MCAChannelPeakPos[2]
			FWHM2[k] = MCAChannelFWHMPercent[2]
		endif
		
		if (chan3)
			Pixie_Math_GaussFit("GaussFitMCA",4,"")
			peak3[k] =MCAChannelPeakPos[3]
			FWHM3[k] = MCAChannelFWHMPercent[3]
		endif
			

		// load ifm file and extract settings
		filename = basename+runn+".ifm"
		Open/R/Z/P=MCApath/T="????" filenum as filename
		for(m=1;m<37;m+=1)
			FReadline filenum, line
		endfor
		//print line
		// next line is beginning of settings: Module number header
		
		for(m=1;m<7;m+=1)
			FReadline filenum, line
		endfor
	//	print line
		sscanf line, "ENERGY_RISETIME %g %g %g %g", v0, v1, v2, v3
		if (chan0)
			Tpeak0[k] = v0
		endif
		if (chan1)
			Tpeak1[k] = v1
		endif
		if (chan2)
			Tpeak2[k] = v2
		endif
		if (chan3)
			Tpeak3[k] = v3
		endif
		
		FReadline filenum, line
	//	print line
		sscanf line, "ENERGY_FLATTOP %g %g %g %g", v0, v1, v2, v3
		if (chan0)
			Tgap0[k] = v0
		endif
		if (chan1)
			Tgap1[k] = v1
		endif
		if (chan2)
			Tgap2[k] = v2
		endif
		if (chan3)
			Tgap3[k] = v3
		endif
		
		for(m=1;m<12;m+=1)
			FReadline filenum, line
		endfor
		FReadline filenum, line
	//	print line
		sscanf line, "TAU %g %g %g %g", v0, v1, v2, v3
		if (chan0)
			tau0[k] = v0
		endif
		if (chan1)
			tau1[k] = v1
		endif
		if (chan2)
			tau2[k] = v2
		endif
		if (chan3)
			tau3[k] = v3
		endif
		
		close/a
	
	endfor
	
	MCAch0 = MCAtotal0 
	MCAch1 = MCAtotal1 
	MCAch2 = MCAtotal2 
	MCAch3 = MCAtotal3 
	MCAsum = MCAtotalsum
	sprintf MCAsource, "Files %s%04d.mca to %s%04d.mca", basename, von, basename, bis
	Killwaves dummy0
End


//########################################################################
//
//	Pixie_RC_StartRun:
//		Start a run in all modules.      
//
//########################################################################
Function Pixie_RC_StartRun(ctrlName) : ButtonControl
String ctrlName

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar RunActive = root:pixie4:RunActive
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	Nvar NumberOfModules=root:pixie4:NumberOfModules
	Nvar ChosenModule=Root:pixie4:ChosenModule
	Wave RunTasks = root:pixie4:RunTasks
	Wave dummy = root:pixie4:dummy
	Nvar MCArunListNumber = root:Pixie4:MCArunListNumber
	Nvar DMAListNumber = root:Pixie4:DMAListNumber
	Svar OutputFileName = root:pixie4:OutputFileName
	Nvar OldCSR = root:pixie4:OldCSR

	
	Svar ReqStartDate = root:pixie4:ReqStartDate	
	Svar ReqStartTime = root:pixie4:ReqStartTime
	Nvar StartDTreq = root:pixie4:StartDTreq
	Nvar StartTime_s = root:pixie4:StartTime_s
	Nvar SeriesStartTime_s = root:pixie4:SeriesStartTime_s
	Svar SeriesStartTime = root:pixie4:SeriesStartTime
	Nvar  SpillCountNewFile =  root:pixie4:SpillCountNewFile
	Nvar  TotalSpillCount =  root:pixie4:TotalSpillCount
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	
	if(AutoNewFile)
		Wave MCAtotal0 = root:pixie4:MCAtotal0
		Wave MCAtotal1 = root:pixie4:MCAtotal1
		Wave MCAtotal2 = root:pixie4:MCAtotal2
		Wave MCAtotal3 = root:pixie4:MCAtotal3
		Wave MCAtotalsum = root:pixie4:MCAtotalsum
		Wave MCAtotalold0 = root:pixie4:MCAtotalold0
		Wave MCAtotalold1 = root:pixie4:MCAtotalold1
		Wave MCAtotalold2 = root:pixie4:MCAtotalold2
		Wave MCAtotalold3 = root:pixie4:MCAtotalold3
		MCAtotal0 = 0
		MCAtotal1 = 0
		MCAtotal2 = 0
		MCAtotal3 = 0
		MCAtotalsum =0
		MCAtotalold0 = 0
		MCAtotalold1 = 0
		MCAtotalold2 = 0
		MCAtotalold3 = 0
	endif
	OldCSR = 0		



	// local variables
	Variable RunType, filenum, SpillStartTime, TimeDiff, ret,repeat, SpillTimeDiff, StartTime, i
	String filename, ErrMSG
	
	
	
	// The first run is always a new run
	RunType = RunTasks[WhichRun-1]+0x1000
		
	// disable controls
	RunInProgress = 1
	Pixie_Tab_Switch("",PanelTabNumber)
	
	//make all the new run files
	Pixie_RC_MakeRunFiles(RunType)
	SeriesStartTime_s = datetime
	SeriesStartTime  = time()+" "+date()
	
	// initialize rate tracking variables
	Pixie_RC_ClearTrack()
	
	//Ensure polling time is not too small
	if( (PollingTime<0.01) && (PollingTime!=0) )
		PollingTime = 0.01
	endif
	
	KeepRePCount = RepCount
	KeepRunTimeOut = RunTimeOut
	RunTimeOut += PollingTime	// Add 1 PollingTime to offset the time spent before a run really starts
	KeepSpillTimeOut = SpillTimeOut
	SpillCountNewFile = 0
	TotalSpillCount = 0
	
	if(RunType == 0x1301)  // MCA run
		RepCount = 0  // Force only one run for MCA run
	endif
	
	if(WhichRun != MCArunListNumber)	
		SpillTimeOut += PollingTime	// Add 1 PollingTime to offset the time spent before a run really starts
	endif
	
	if(PollingTime==0)  // Poll at maximum frequency
		DoAlert 0, "Runs with zero polling time currently not supported"
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(-1)		
	else  // leisurely polling
	
		//////////////////////////////////////////////////////////// 
		// Call to user routine			             //
			User_StartRun()
		////////////////////////////////////////////////////////////	
		

		if (StartDTreq)		// wait until start time/date is reached
			print "Waiting until", ReqStartDate, ReqStartTime
			do
			i=0
			 	if( (cmpstr(ReqStartDate,Date()) == 0) && (cmpstr(ReqStartTime,Time()) == 0) )
			 		i=2
				endif
				sleep/T 1
			while( i<=1)
		endif
		
		//make/o/u/i/n=1 dummy
		ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
		if(ret < 0)  // Failed to start a new run
			DoAlert 0, "Failed to start a new run."
			Pixie_RC_RestoreRunTab()  //restore run tab settings
			return(ret)
		endif

		Nvar PollForNewData = root:pixie4:PollForNewData
		Nvar MultiThreadDAQ = root:pixie4:MultiThreadDAQ
		
		Sleep/T 10		
		if(cmpstr("StartScanRun",ctrlName)==0)
			//SetBackground Pixie_Scan_CheckRun()
			CtrlNamedBackground CheckRun, period=(PollingTime*60), proc=Pixie_Scan_CheckRun
			print "polling routine is modifying filter settings"
			print "This routine works only for Rev.C and higher, only for MCA runs"
			print "Run option 'New file' is enabled automatically for duration of scan, but new file interval must be set by user"
		else
			if(WhichRun >= DMAListNumber)	
				if(PollForNewData)
					//SetBackground Pixie_RC_DMACheckRun_DataPoll()
					CtrlNamedBackground CheckRun, period=(PollingTime*60), proc=Pixie_RC_DMACheckRun_DataPoll
				else
					//SetBackground Pixie_RC_DMACheckRun()
					CtrlNamedBackground CheckRun, period=(PollingTime*60), proc=Pixie_RC_DMACheckRun
				endif
			else
				if(MultiThreadDAQ & (WhichRun != MCArunListNumber))
					//SetBackground Pixie_RC_DMACheckRun()
					CtrlNamedBackground CheckRun, period=(PollingTime*60), proc=Pixie_RC_DMACheckRun
				else
					//SetBackground Pixie_RC_CheckRun()
					CtrlNamedBackground CheckRun, period=(PollingTime*60), proc=Pixie_RC_CheckRun
				endif
			endif
		endif		
	
		//CtrlBackground start,period=(PollingTime*60)
		CtrlNamedBackground CheckRun start

	endif
End




//########################################################################
//
//	Pixie_RC_StopRun:
//		Stop the run in all modules and read out run data from modules.      
//
//########################################################################
Function Pixie_RC_StopRun(ctrlName) : ButtonControl
	String ctrlName

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar DisableLMparsing = root:pixie4:DisableLMparsing
	Svar OutputFileName = root:pixie4:OutputFileName
	Wave RunTasks = root:pixie4:RunTasks
	Wave dummy = root:pixie4:dummy
	Nvar MCArunListNumber = root:Pixie4:MCArunListNumber
	Nvar DMAListNumber = root:Pixie4:DMAListNumber
	Svar OutBaseName = root:pixie4:OutBaseName
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar AutoRunNumber = root:pixie4:AutoRunNumber
	Nvar PRESET_MAX_MODULES = root:pixie4:PRESET_MAX_MODULES
	Nvar AutoProcessLMData = root:pixie4:AutoProcessLMData
	Nvar MultiThreadDAQ = root:pixie4:MultiThreadDAQ


	Svar RunStopTime = root:pixie4:StopTime
	Nvar StopTime_s = root:pixie4:StopTime_s
		
	// local variables
	Variable RunType, totaltraces,ret,len,i, totalevents
	Variable num, fn, m
	String filename,wav, LMdatafilename
	
	if( (cmpstr(ctrlName, "RunEndDAQ1") == 0) | (cmpstr(ctrlName, "RunEndDAQ2a") == 0) | (cmpstr(ctrlName, "RUN_EndDAQ") == 0) | (cmpstr(ctrlName, "MainStopRun") == 0)) 
	// Stop run from all processes except AutoScanFilterTime and Fast Polling
		if(PollingTime != 0) 		// there is no background task in polling = 0
			//CtrlBackground Stop  // Stop the background first
			CtrlNamedBackground CheckRun kill
		endif

		//make/o/u/i/n=1 dummy
		RunType=RunTasks[WhichRun-1]+0x3000  // 
		ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules (DSP only)
		//print" Igor: Pixie_RC_StopRun: extra stop at runstop causes Igor crash, skipping"
		
		if(ret<0)	// check for end run error
			if(ret==-17)	// some are serious
				DoAlert 0, "Failed to stop run, potentially fatal error communicating to module. The PC may lock up in future I/O. Save all open files now, then try a PLL reset and reboot for a chance to recover."	
				Pixie_RC_RestoreRunTab()	// Restore original settings
				RunNumber += 1  // Increment run number by 1
				Pixie_File_RunStats("StopRun",1,"")	// save some diagnostic data
				return (ret)
			else	// some are not
				printf "Failed to stop run (error %d, -0x%0X), probably save to continue\n", ret, abs(ret)		
			endif
			
		endif
		// default action for saving run data 
		if( (WhichRun != MCArunListNumber) & (WhichRun < DMAListNumber) & !MultiThreadDAQ)		// if not mca run, it must be a listmode run
			RunType=RunTasks[WhichRun-1]+0x6000  // Save list mode event data. Not required for MCA runs and DMA runs  in which the final readout happens at endrun
			Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
		endif

		Pixie_RC_AutoSaveDAQFiles()
		Pixie_RC_AutoRestoreSync()
		Pixie_RC_RestoreRunTab()	// Restore original settings
		
		
		
	else // for ctrlName repeat_scan, from repeat or scall check run routines
		RunStopTime = time()+" "+date()
		StopTime_s = datetime
	endif
	

	if ( (WhichRun != MCArunListNumber)   )
		if ( DisableLMparsing || (WhichRun==7) )
			if ( DisableLMparsing)
				print "WARNING: Skipped parsing of list mode file. Manually load file to view traces"
			else
				print "WARNING: No binary file with traces created for Runtask 0x401"
			endif
		else 
			if(WhichRun==DMAListNumber)	// for DMA runs, do once per module (each file), end with 0
				if(AutoRunNumber)
					num = RunNumber-1
				else
					num = RunNumber
				endif
									
				make/o/u/i/n=(PRESET_MAX_MODULES*2) root:pixie4:listmodewave
				wave listmodewave = root:pixie4:listmodewave
				listmodewave = 0
				
				
				if(AutoProcessLMData)
					for(m=0;m<NumberOfModules;m+=1)
						sprintf filename, "%s%04d.b%02d", OutBaseName, num,m		// for P500e, each module has a separate file
						Open/R/P=EventPath fn as filename
						PathInfo EventPath
			
						if(fn == 0)
							print "Error: can not open file", filename
							break
						else
							close fn
						endif
						LMdatafilename = S_Path + filename
						LMdatafilename = Pixie_FileNameConversion(LMdatafilename)	
	
						// Parse the list mode event file
						listmodewave[0] = 1		// indicates we are in a loop, one output file to which all modules are appended
						ret = Pixie4_Acquire_Data(0x7001, listmodewave, LMdatafilename, NumberOfModules)
						if(ret < 0)
							Doalert 0, "Can not parse list mode event file."
							break
						endif
					endfor
				endif
			endif
			Pixie_ExtractNumPosLen()	// once more, for current module
		endif
	endif

	
	// Update all modules
	Pixie_IO_ReadModParFromMod(NumberOfModules)
	Pixie_IO_SendModParToIgor()
	
	// Update MCA traces in all channels and update all globals
	Pixie_RC_UpdateMCAData("endrun")

	 ////////////////////////////////////////////////////////////// 
	// Call to user routine			             ///
     //////////////////////////////////////////////////////////////	
	User_StopRun()

End


//########################################################################
//
//	Pixie_RestoreRunTab:
//		Restore the status of run control tabs when the run is done.      
//
//########################################################################
Function Pixie_RC_RestoreRunTab()

	Nvar RunActive = root:pixie4:RunActive
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber

	// Restore original settings
	RunTimeOut = KeepRunTimeOut
	SpillTimeOut=KeepSpillTimeOut
	RepCount = KeepRepCount
	RunActive = 0
	RunInProgress = 0
			
	Pixie_Tab_Switch("",PanelTabNumber)
	
	
End



//########################################################################
//
//	Pixie_UpdateMCAData:
//		Update MCA spectra and run statistics.
//
//########################################################################
Function Pixie_RC_UpdateMCAData(mode)
String mode	// file - read from disk and update
			// endrun - clean up at end of run
			// UpdateMCA - read during run, push button in MCApanel, also update statistics
			// module - read from module, don't worry about statistics

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave mcawave = root:pixie4:mcawave
	Wave RunTasks = root:pixie4:RunTasks
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar WhichRun = root:pixie4:WhichRun
	Svar MCAFileName = root:pixie4:MCAFileName
	Nvar CloverAdd = root:pixie4:CloverAdd
	Svar  MCASource =  root:pixie4:MCASource
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	String wav, ErrMSG, filename,wv2
	Variable index0, index1, index, i, k, dx, x0, direction, channum, RunType, ret, filenum


	//----------------------------------------------------------------
	// + Update MCA spctra for all channels of current module
	//----------------------------------------------------------------
	
	variable MCA16nSum
	RunType=RunTasks[WhichRun-1]
	MCA16nSum = ( (RunType==0x402) || (CloverAdd==1) )
	
	
	if(cmpstr(mode, "file")==0)		// read from file after end of run
		RunType=RunTasks[WhichRun-1]+0x8000  // Read MCA data from file
		ret = Pixie4_Acquire_Data(RunType, root:pixie4:mcawave, MCAFileName, ChosenModule)
		if(ret < 0)
			DoAlert 0, "Failed to read MCA data from file."
			return(ret)
		endif
		MCASource = MCAFileName
	endif
	
	if(cmpstr(mode, "module")==0)		// read from EM
		RunType=0x9001  // Read MCA data
		ret = Pixie4_Acquire_Data(RunType, root:pixie4:mcawave, MCAFileName, ChosenModule)
		if(ret < 0)
			DoAlert 0, "Failed to read MCA data from module."
			return(ret)
		endif
		MCASource = "<module>"
	endif
	
	if(cmpstr(mode, "UpdateMCA")==0)		// read from EM
		RunType=0x9001  // Read MCA data
		ret = Pixie4_Acquire_Data(RunType, root:pixie4:mcawave, MCAFileName, ChosenModule)
		if(ret < 0)
			DoAlert 0, "Failed to read MCA data from module."
			return(ret)
		endif
		MCASource = "<module>"
		Pixie_RC_UpdateRunstats("module")
	endif

	// in multi-file LM runs, the online MCA are cumulative
	String mcaname
	RunType=RunTasks[WhichRun-1]
	if((RunType==0x400) && (AutoNewFile==1) )
		mcaname = "MCAtotal"					// choose totals to update from the module mca wave below
		
		for(i=0; i<NumberOfChannels; i+=1)			// update current by subtraction totals-to-last-filechange from module mca wave
			wav = "root:pixie4:MCAch"+num2str(i)
			wv2 = "root:pixie4:MCAtotalold"+num2str(i)
			wave tracewav = $wav
			wave oldMCA = $wv2
				// 32k spectra
				tracewav = mcawave[p+32768*i]-oldMCA[p]		// new current MCA is total minus previous total
				tracewav[0]=0
		endfor
	else
		mcaname = "MCAch"					// choose current to update from the module mca wave below
	endif
	
	
	if( (cmpstr(mode, "UpdateMCA")==0)	|  (cmpstr(mode, "file")==0) |  (cmpstr(mode, "module")==0)  )		//when reading, update waves
		Wave MCAsum = root:pixie4:MCAsum
		index=0
		mcawave[0] = mcawave[0]		// ensure update of data read 
		for(i=0; i<NumberOfChannels; i+=1)
			wav = "root:pixie4:"+mcaname+num2str(i)
			wave tracewav = $wav
				
			
			if(MCA16nSum==1)				// 16k spectra with addback
				tracewav[16384,32767]=0
				tracewav[0,16383] = mcawave[p+16384*i]
				tracewav[0]=0
			else								// 32k spectra
				tracewav = mcawave[p+32768*i]
				tracewav[0]=0
				MCAsum = 0
			endif
		endfor
			
		if(MCA16nSum==1)					// 16k addback spectrum
			MCAsum = mcawave[p+16384*4]
			MCAsum[0]=0
		endif

		///////////////////////////////////////////////////////////// 
		// 	Call to user routine			      //
			User_UpdateMCA()				//
		////////////////////////////////////////////////////////////	
		
	endif

	if(cmpstr(mode,"endrun")==0)		// no reading, remove fits 
	
		for(i=0; i<NumberOfChannels; i+=1)
			// Remove fit
			DoWindow/F MCASpectrumDisplay
			if(V_Flag != 0)
				wav="fit_MCAch"+num2str(i)
				RemoveFromGraph/W=MCASpectrumDisplay/z $wav
			endif
		endfor
	endif

End

//########################################################################
//
//	Pixie_RC_UpdateRunstats:
//		Update Run statistics.
//
//########################################################################
Function Pixie_RC_UpdateRunstats(mode)
String mode	//modes: 	
			//			module: 		from module
			//			ifmfile:		from .ifm file	
			//			setfile:		from .set file	
			
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar CurrentTime_s = root:pixie4:CurrentTime_s
	Svar  InfoSource=  root:pixie4:InfoSource
	Svar DSPParaValueFile = root:pixie4:DSPParaValueFile
	Svar SettingsFileName = root:pixie4:SettingsFileName
	Svar StatisticsFileName = root:pixie4:StatisticsFileName
	
	Nvar TrackER = root:pixie4:TrackER
	Nvar TrackNum = root:pixie4:TrackNum
	Wave TrackMod = root:pixie4:TrackMod
	Wave TrackChan = root:pixie4:TrackChan
	Nvar index_FP = root:pixie4:index_FP
	Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
	Wave ERtimestamp = root:pixie4:ERtimestamp
	Wave lastNE = root:pixie4:lastNE
	Wave lastRT = root:pixie4:lastRT
	Nvar StartTime_s = root:pixie4:StartTime_s

	String wav, ErrMSG, filename	
	Variable index0, index1, index, i, k, dx, x0, direction, channum, RunType, ret, filenum

	//----------------------------------------------------------------
	// + Update run statistics in all modules
	//----------------------------------------------------------------
	
	
	if (cmpstr(mode,"module")==0)
		// First, channel run statistics
		direction=1	// Read from Pixie modules
		channum = 0	// Channel number is not important here
		for(i=0; i<NumberOfModules; i+=1)
			Pixie4_User_Par_IO(Channel_Parameter_Values, "CHANNEL_RUN_STATISTICS", "CHANNEL", direction, i, channum)
		endfor
		
		// Second, module run statistics		
		direction=1	// Read from Pixie modules
		channum = 0	// Channel number is not important here
		for(i=0; i<NumberOfModules; i+=1)
			Pixie4_User_Par_IO(Module_Parameter_Values, "MODULE_RUN_STATISTICS", "MODULE", direction, i, channum)
		endfor
		InfoSource = "<module>"
		CurrentTime_s = datetime
	endif
	
	if (cmpstr(mode,"setfile")==0)
		InfoSource = DSPParaValueFile
	endif
		
	Pixie_IO_SendChanParToIgor()
	Pixie_IO_SendModParToIgor()	
		
	Pixie_MakeList_AllRunStats(1)
	
	if(TrackER &&  (cmpstr(mode,"module")==0)  )
	
		variable diff, mo,ch, fp, lt
		for(i=0; i< 4; i+=1)
			wave wv = $("root:pixie4:ER"+num2str(i))
			mo = TrackMod[i]
			ch = TrackChan[i]
			fp = Channel_Parameter_Values[index_FP][ch][mo]
			lt = Channel_Parameter_Values[index_COUNTTIME][ch][mo]
			diff =  (-lastNE[i] + fp) / (- lastRT[i] + lt )
			wv[TrackNum] = diff
			lastNE[i] = fp
			lastRT[i] = lt
		endfor
		ERtimestamp[trackNum] = CurrentTime_s- StartTime_s
		trackNum +=1
		
		Variable jump=1000
		if (trackNum>=jump*2)
			for(i=0; i< 4; i+=1)
			wave wv = $("root:pixie4:ER"+num2str(i))
				wv[0,jump-1] = wv[jump+p]
				wv[jump,2*jump-1] = nan
			endfor
			ERtimestamp[0,jump-1] = ERtimestamp[jump+p]
			ERtimestamp[jump,2*jump-1] = nan
			trackNum=jump
		endif

	endif

End


Function	Pixie_RC_ClearTrack()
		
	Nvar TrackNum = root:pixie4:TrackNum
	Wave ERtimestamp = root:pixie4:ERtimestamp
	Wave lastNE = root:pixie4:lastNE
	Wave lastRT = root:pixie4:lastRT
	
	Variable i

	TrackNum = 0
	ERtimestamp = nan
	for(i=0; i<4; i+=1)
		wave wv = $("root:pixie4:ER"+num2str(i))
		wv = nan
	endfor
	lastNE = 0
	lastRT = 0
End





Function Pixie_RC_AutoSaveDAQFiles()

// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar AutoRunNumber = root:pixie4:AutoRunNumber
	Nvar AutoStoreSettings = root:pixie4:AutoStoreSettings	
	Nvar AutoStoreStatistics = root:pixie4:AutoStoreStatistics	
	Nvar AutoStoreSpec = root:pixie4:AutoStoreSpec
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar MCAFileName = root:pixie4:MCAFileName	
	Svar OutputFileName = root:pixie4:OutputFileName
	Svar SettingsFileName = root:pixie4:SettingsFileName
	Svar StatisticsFileName = root:pixie4:StatisticsFileName
	Svar LongDataFileName = root:pixie4:LongDataFileName
	Svar InfoSource = root:pixie4:InfoSource
	Wave RunTasks = root:pixie4:RunTasks
	Svar RunStopTime = root:pixie4:StopTime
	Nvar StopTime_s = root:pixie4:StopTime_s
	Wave dummy = root:pixie4:dummy
	Wave dummysettings = root:pixie4:dummysettings

	Variable RunType
	
	RunStopTime = time()+" "+date()
	StopTime_s = datetime

	if(AutoStoreSpec == 1)  // Automatically save spectrum in a file
		RunType=RunTasks[WhichRun-1]+0x5000 //save to file
		Pixie4_Acquire_Data(RunType, dummy, MCAFileName, NumberOfModules)
		Pixie_RC_UpdateMCAData("file")	// read from file and update
	else  // Manually read out MCA spectrum for the current module
		Pixie_RC_UpdateMCAData("module")	// read from EM
		//Pixie4_Acquire_Data(0x9001, root:pixie4:mcawave,"", ChosenModule)
	endif
	Pixie_RC_UpdateRunstats("module") 	// always update Runstats

	if(AutoRunNumber == 1)
		RunNumber += 1  // Increment run number by 1
	endif
			
	if(AutoStoreSettings == 1)
		// Save settings
		//make/o/u/w/n=1 dummysettings
		Pixie4_Buffer_IO(dummysettings, 2, 0, SettingsFileName, ChosenModule)
		InfoSource = SettingsFileName		//statistics are saved in .set file
	endif
	
	if(AutoStoreStatistics == 1)
		Pixie_File_RunStats("StopRun",1,"")
		InfoSource = StatisticsFileName		//statistics are saved in .ifm file
		// includes update of runstats
	endif

End



Function Pixie_RC_MakeRunFiles(RunType)
Variable RunType

// global variables and waves
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar AutoRunNumber = root:pixie4:AutoRunNumber
	Nvar AutoStoreSpec = root:pixie4:AutoStoreSpec
	Nvar AutoStoreSettings = root:pixie4:AutoStoreSettings
	Nvar AutoStoreStatistics = root:pixie4:AutoStoreStatistics
	Svar OutBaseName = root:pixie4:OutBaseName
	Svar OutputFileName = root:pixie4:OutputFileName
	Svar SettingsFileName = root:pixie4:SettingsFileName
	Svar StatisticsFileName = root:pixie4:StatisticsFileName
	Svar MCAFileName = root:pixie4:MCAFileName
	Svar DataFile = root:pixie4:DataFile
	Svar LongDataFileName = root:pixie4:LongDataFileName
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar AutoProcessLMData = root:pixie4:AutoProcessLMData
	Nvar ChosenModule = root:pixie4:ChosenModule
	
	Svar RunStartTime = root:pixie4:StartTime
	Svar RunStopTime = root:pixie4:StopTime
	Nvar StartTime_s = root:pixie4:StartTime_s
	Nvar StopTime_s = root:pixie4:StopTime_s
	
	Variable  filenum, noconflict, Ntries, m
	String filename, ErrMSG
	
	noconflict=1
	Ntries=0
	close/a
	
	if (AutoRunNumber==1)	//we assume, if auto increment is selected, we don't want to overwrite existing files
		do	
			if(( RunType & 0x0FFF) ==0x0301)  // MCA run
				sprintf filename, "%s%04d.mca", OutBaseName, RunNumber
				Open/R/Z/P=MCAPath filenum as filename
			endif
			if(( RunType & 0x0FF0) ==0x0100)  // List mode run (P4/P500)
				sprintf filename, "%s%04d.bin", OutBaseName, RunNumber
				Open/R/Z/P=EventPath filenum as filename
			endif
			if(( RunType & 0x0FF0) ==0x0400)  // List mode run (P500e)
				sprintf filename, "%s%04d.b00", OutBaseName, RunNumber
				Open/R/Z/P=EventPath filenum as filename
			endif
			if(V_flag==0) //file exists
				print "WARNING: File",filename,"already exists ... incrementing run number."
				RunNumber+=1
				noconflict=0
				Ntries+=1
				close filenum
			else
				noconflict=1
			endif
		while ( (noconflict==0) && (Ntries<20) )
		
		if(noconflict==0)
			OutBaseName=OutBaseName+"_new"
			print "too many tries ... modifying base name"
		endif
	endif
		

		
	//  Make settings file name
	if(AutoStoreSettings == 1)
		sprintf filename, "%s%04d.set", OutBaseName, RunNumber
		if(( RunType & 0x0FFF) ==0x0301)  // MCA run
			Open/P=MCAPath filenum as filename
			PathInfo MCAPath
		else  // List Mode run
			Open/P=EventPath filenum as filename
			PathInfo EventPath
		endif
		if(filenum == 0)
			sprintf ErrMSG, "Error: can not open file %s\r", filename
			 DoAlert 0,ErrMSG
			 return(-1)
		else
			close filenum
		endif
		SettingsFileName = S_Path + filename
		SettingsFileName = Pixie_FileNameConversion(SettingsFileName)
	endif
	
	//  Make statistics file name	
	// since this file contains Igor run start/stop time, it is written by Igor (others by C lib)
	if(AutoStoreStatistics == 1)
		sprintf filename, "%s%04d.ifm", OutBaseName, RunNumber
		if(( RunType & 0x0FFF) ==0x0301)  // MCA run
			Open/P=MCAPath filenum as filename
			PathInfo MCAPath
		else  // List Mode run
			Open/P=EventPath filenum as filename
			PathInfo EventPath
		endif
		if(filenum == 0)
			sprintf ErrMSG, "Error: can not open file %s\r", filename
			 DoAlert 0,ErrMSG
			 return(-1)
		else
			close filenum
		endif
		StatisticsFileName = S_Path + filename
	endif
	
	// Make MCA filename
	if(AutoStoreSpec == 1)
		sprintf filename, "%s%04d.mca", OutBaseName, RunNumber
		if(( RunType & 0x0FFF) ==0x0301)  // MCA run
			Open/P=MCAPath filenum as filename
			PathInfo MCAPath
		else  // List Mode run
			Open/P=EventPath filenum as filename
			PathInfo EventPath
		endif
		if(filenum == 0)
			sprintf ErrMSG, "Error: can not open file %s\r", filename
			 DoAlert 0,ErrMSG
			 return(-1)
		else
			close filenum
		endif
		MCAFileName = S_Path + filename
		MCAFileName = Pixie_FileNameConversion(MCAFileName)
	endif
	
//	// Make list mode output filename
//	if(( RunType & 0x0FFF) !=0x0301)   // List Mode run
//		sprintf filename, "%s%04d.bin", OutBaseName, RunNumber
//		Open/P=EventPath filenum as filename
//		PathInfo EventPath
//	
//		if(filenum == 0)
//			sprintf ErrMSG, "Error: can not open file %s\r", filename
//			 DoAlert 0,ErrMSG
//			 return(-1)
//		else
//			close filenum
//		endif
//		OutputFileName = S_Path + filename
//		OutputFileName = Pixie_FileNameConversion(OutputFileName)
//		DataFile = filename
//		LongDataFileName = OutputFileName
//	endif

	// Make list mode output filename
	if(( RunType & 0x0FF0) ==0x0100)   // List Mode run P4/P500
		sprintf filename, "%s%04d.bin", OutBaseName, RunNumber
		Open/P=EventPath filenum as filename
		PathInfo EventPath
	
		if(filenum == 0)
			sprintf ErrMSG, "Error: can not open file %s\r", filename
			 DoAlert 0,ErrMSG
			 return(-1)
		else
			close filenum
		endif
		OutputFileName = S_Path + filename
		OutputFileName = Pixie_FileNameConversion(OutputFileName)
		DataFile = filename
		LongDataFileName = OutputFileName
	endif
	if(( RunType & 0x0FF0) ==0x0400)   // List Mode run P500e
	// one file per module, extension .b##
//		if(AutoProcessLMData!=5)		// no binary files when AutoProcessLMData=5
//			for(m=0;m<NumberOfModules;m+=1)
//				sprintf filename, "%s%04d.b%02d", OutBaseName, RunNumber, m
//				Open/P=EventPath filenum as filename
//				PathInfo EventPath
//		
//				if(filenum == 0)
//					sprintf ErrMSG, "Error: can not open file %s\r", filename
//					 DoAlert 0,ErrMSG
//					 return(-1)
//				else
//					close filenum
//				endif
//				if(m==0)
//					OutputFileName = S_Path + filename
//					OutputFileName = Pixie_FileNameConversion(OutputFileName)
//					DataFile = filename
//					LongDataFileName = OutputFileName
//				endif
//			endfor
//		else
			// make only filename
			sprintf filename, "%s%04d.b%02d", OutBaseName, RunNumber, ChosenModule
			PathInfo EventPath
			OutputFileName = S_Path + filename
			OutputFileName = Pixie_FileNameConversion(OutputFileName)
			DataFile = filename
			LongDataFileName = OutputFileName
//		endif
	endif

	
	RunStartTime = time()+" "+date()
	StartTime_s = datetime
	StopTime_s = -1
	RunStopTime = "<in progress>"
	
End

Function Pixie_RC_AutoRestoreSync()

	Nvar SynchAlways = root:pixie4:SynchAlways
	Nvar InSynch = root:pixie4:InSynch
	
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar ChosenModule = root:pixie4:ChosenModule

	Variable i
	
// In DSP V4.xx, timers are synced at boot time. So the InSync checkbox really means clear it for every run, no need to have it checked for first run only
// Therefore no longer clear it automatically
//	// For Pixie-500e, InSynch is unchanged by DSP. Here we clear it for the next run unless user wants it to persist
//	if(!SynchAlways)	
//		InSynch=0
//		for(i=0; i<NumberOfModules; i+=1)
//			Module_Parameter_Values[Pixie_Find_ModuleParInWave("IN_SYNCH")][i]=1-InSynch
//		endfor
//		Pixie4_User_Par_IO(Module_Parameter_Values, "IN_SYNCH", "MODULE", 0, ChosenModule, 0)
//	endif
	
	// For Pixie-4/500, InSynch is cleared after clocks have been synchronized in module, here we set it back if user wants it so
	if(SynchAlways)	
		InSynch=1
		for(i=0; i<NumberOfModules; i+=1)
			Module_Parameter_Values[Pixie_Find_ModuleParInWave("IN_SYNCH")][i]=1-InSynch
		endfor
		Pixie4_User_Par_IO(Module_Parameter_Values, "IN_SYNCH", "MODULE", 0, ChosenModule, 0)
	endif	
End




//########################################################################
//
//	Pixie_CheckRun:
//		Check the run status in all modules.      
//
//########################################################################
Function Pixie_RC_CheckRun(s)		// This is the function that will be called periodically
	STRUCT WMBackgroundStruct &s

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar RunActive = root:pixie4:RunActive
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar AutoUpdateMCA= root:pixie4:AutoUpdateMCA
	Nvar AutoUpdateRunStats = root:pixie4:AutoUpdateRunStats
	Nvar Tupdate = root:pixie4:Tupdate
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	Nvar TNewFile = root:pixie4:TNewFile
	Nvar PanelTabNumber = root:pixie4:PanelTabNumber
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar OutputFileName = root:pixie4:OutputFileName
	Wave RunTasks = root:pixie4:RunTasks
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave dummy = root:pixie4:dummy
	Nvar MCArunListNumber = root:Pixie4:MCArunListNumber
	Nvar PingPongRun = root:pixie4:PingPongRun
		
	Svar ReqStopDate = root:pixie4:ReqStopDate	
	Svar ReqStopTime = root:pixie4:ReqStopTime
	Nvar StopDTreq = root:pixie4:StopDTreq
	Nvar StartTime_s = root:pixie4:StartTime_s
	Nvar SeriesStartTime_s = root:pixie4:SeriesStartTime_s
	
	// local variables
	Variable RunType,ret,direction,channum,i,index0,index1, updated, saved, CSR, DataReady, stoppedrun, elapsed
		
	updated=0
	saved=0
	stoppedrun=0
	
	// poll module	
	RunType = 0x40FF	
	CSR = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, ChosenModule)	//polling
	//ph(csr)

	RunActive = ((CSR & 0x2000) >0)
	DataReady =  ((CSR & 0x4000) >0)
	
	if(CSR < 0)	// failed to poll
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		DoAlert 0, "Failed to check run status."
		ret = Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
		if(ret < 0)  // failed to stop the run
			print "Failed to stop the run. Please reboot the Pixie modules."
		endif
					
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(RunActive)
	endif
		
	// check for run finished or data ready
	
	// decrease timers
	
	elapsed = DateTime - SeriesStartTime_s

	//print/d elapsed
	RunTimeOut = KeepRunTimeOut - elapsed
	//RunTimeOut -= PollingTime
	if(WhichRun != MCArunListNumber)	
		SpillTimeOut = KeepSpillTimeOut - elapsed
		//SpillTimeOut -= PollingTime
	endif
	
	// check if target date/time reached	
 	if(StopDTreq)
 		if( (cmpstr(ReqStopDate,Date()) == 0) && (cmpstr(ReqStopTime,Time()) == 0) )
			RunTimeOut =0
		endif	 	
	endif
	
	// when run active, update spectrum and/or runstats occasionally
  	if( ( mod(KeepRunTimeOut-RunTimeOut,Tupdate)>=Tupdate-PollingTime)  & (RunTimeOut>Tupdate)  )
 		if( AutoUpdateMCA==1) 
 			Pixie_RC_UpdateMCAData("UpdateMCA")
 		else
			if (AutoUpdateRunStats)
				Pixie_RC_UpdateRunstats("module")
			endif	
		endif 			
	endif
		
	// 1. Run stopped (finished and/or data ready)	
	if( (SpillTimeOut<=0) || (RunActive==0) || (RunTimeOut<=0) )	
		stoppedrun =1
		
		//printf "1 %X, Repcount = %d\r", csr, repcount
		RepCount -= 1
		if (RunTimeOut<=0)	// overall run time over: no more spills
			RepCount = 0
		endif
		
		if(RepCount > 0)  // list mode repeat run with a) 32x buffer/spill with all EM filled, b) 16x ping pong with both EM blocks filled or c) single buffer/spill with DM filled 
		
			// Stop and read out modules
			Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
			RunType=RunTasks[WhichRun-1]+0x6000  // Save list mode event data
			ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
			//print "reading data"
			if(ret < 0)	// failed to save list mode event data
				//CtrlBackground Stop  // Stop the background first
				CtrlNamedBackground CheckRun kill
				DoAlert 0, "Failed to save list mode event data."
				Pixie_RC_RestoreRunTab()  //restore run tab settings
				return(ret)
			endif
						
			//occasionally save all auto files and go to next run number
			if(AutoNewFile==1) 
			 	if( (mod(KeepRepCount-RepCount-1,TNewFile) >=TNewFile-1) && (RepCount >= TNewFile)  )
			 		
			 		Pixie_RC_AutoSaveDAQFiles()
			 		Pixie_RC_UpdateMCATotals()
			 		saved=1
			 		Pixie_RC_AutoRestoreSync()
			 							
			 		Pixie_RC_MakeRunFiles(RunType)
			 		Pixie_RC_ClearTrack()
			 		RunType = RunTasks[WhichRun-1]+0x1000 //New Run after changing file names
			 	endif
			endif
			
			if (saved)
				///////////////////////////////////////////////////////////// 
				// 	Call to user routine			      //
					User_NewFileDuringRun("List")		//
				////////////////////////////////////////////////////////////	
			endif	
			
			//Resume/Restart Run
			if (saved==0)
				RunType=RunTasks[WhichRun-1]+0x2000  // Resume run
			endif
			ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
			if(ret < 0)	// failed to resume run
				//CtrlBackground Stop  // Stop the background first
				CtrlNamedBackground CheckRun kill
				DoAlert 0, "Failed to resume run."
				Pixie_RC_RestoreRunTab()  //restore run tab settings
				return(ret)
			endif
			
			SpillTimeOut=KeepSpillTimeOut	
	
		else  // MCA run or list mode run w/o spills left 
			
			//CtrlBackground Stop  // Stop the background first
			CtrlNamedBackground CheckRun kill
			Pixie_RC_StopRun("RunEndDAQ1")  // includes last readout
			
		endif
	endif
	
		// 2a. Run still in progress (Double buffer List mode only)
	if(WhichRun != MCArunListNumber)
		if ( (DataReady==1) && (stoppedrun==0) )	//one EM block has data -> readout, but don't stop run
			RepCount -= 1
			if(RepCount > 0)  	// still more spills to come
			
				//occasionally save all auto files and go to next run number
				if( (AutoNewFile==1) && (mod(KeepRepCount-RepCount-1,TNewFile) >=TNewFile-1) && (RepCount >= TNewFile)  )
				
					// Stop and read out modules
					Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
					RunType=RunTasks[WhichRun-1]+0x6000  // Save list mode event data
					ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
					if(ret < 0)	// failed to save list mode event data
						//CtrlBackground Stop  // Stop the background first
						CtrlNamedBackground CheckRun kill
						DoAlert 0, "Failed to save list mode event data."
						Pixie_RC_RestoreRunTab()  //restore run tab settings
						return(ret)
					endif
				 		
				 	// make new files
				 	Pixie_RC_AutoSaveDAQFiles()
				 	Pixie_RC_UpdateMCATotals()
				 	Pixie_RC_AutoRestoreSync()
				 							
				 	Pixie_RC_MakeRunFiles(RunType)
				 	Pixie_RC_ClearTrack()
				
					// restart
				 	RunType = RunTasks[WhichRun-1]+0x1000 //New Run after changing file names
				 	ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
					if(ret < 0)	// failed to resume run
						//CtrlBackground Stop  // Stop the background first
						CtrlNamedBackground CheckRun kill
						DoAlert 0, "Failed to resume run."
						Pixie_RC_RestoreRunTab()  //restore run tab settings
						return(ret)
					endif
									
					///////////////////////////////////////////////////////////// 
					// 	Call to user routine			      //
						User_NewFileDuringRun("List")		//
					////////////////////////////////////////////////////////////	
							 	
				 else	
				 	// read without stopping
				 	RunType=RunTasks[WhichRun-1]+0x6000  // Save list mode event data
				 	//print "Igor: saving data to file"
					ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
					if(ret < 0)	// failed to save list mode event data
						//CtrlBackground Stop  // Stop the background first
						CtrlNamedBackground CheckRun kill
						DoAlert 0, "Failed to save list mode event data."
						Pixie_RC_RestoreRunTab()  //restore run tab settings
						return(ret)
					endif				
				endif
			
					
			else	// took all we wanted, host stop
				//CtrlBackground Stop  // Stop the background first
				CtrlNamedBackground CheckRun kill
				Pixie_RC_StopRun("RunEndDAQ2a")  
	
				// Restore original settings
				//Pixie_RC_RestoreRunTab()  //restore run tab settings
				//print "checkrun: duplicate restore runtab"
			endif
			
			SpillTimeOut=KeepSpillTimeOut	// restore timeout for next spill
		endif
		
	else		// 2b. Run still in progress (MCA mode only)
		if(AutoNewFile==1) 
			 if( ( mod(KeepRunTimeOut-RunTimeOut,TNewFile)>=TNewFile-PollingTime)  & (RunTimeOut>TNewFile)  )
			 	Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
				Pixie_RC_AutoSaveDAQFiles()
				Pixie_RC_UpdateMCATotals()
				
				///////////////////////////////////////////////////////////// 
				// 	Call to user routine			      //
					User_NewFileDuringRun("MCA")	//
				////////////////////////////////////////////////////////////	
								
				RunType = RunTasks[WhichRun-1]+0x1000 //New Run after changing file names
				Pixie_RC_MakeRunFiles(RunType)
				Pixie_RC_ClearTrack()
				Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
			endif
		endif
	endif
	

		
	return(0)		// IGOR 5 needs a return value for Background task to work
			
End

	



// This function tries to avoid memory overflow problems in multiple MCA runs (avoiding background task)
// to be called from the command line only
Function Pixie_RepeatMCA(ctrlName) : ButtonControl
String ctrlName

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	Nvar NumberOfModules=root:pixie4:NumberOfModules
	Svar OutputFileName = root:pixie4:OutputFileName
	Nvar RunInProgress = root:pixie4:RunInProgress
	Wave RunTasks = root:pixie4:RunTasks	
	Nvar TNewFile = root:pixie4:TNewFile
	Wave dummy = root:pixie4:dummy

	// local variables
	Variable RunType, filenum, WaitStartTime, TimeDiff, ret,repeat, aborted, StartTime, currentTime
	String filename, ErrMSG
	
	// The first run is always a new run
	RunType = RunTasks[WhichRun-1]+0x1000

	// disable controls
	RunInProgress = 1
	Pixie_Tab_Switch("",PanelTabNumber)
	
	//make all the new run files
	Pixie_RC_MakeRunFiles(RunType)
	
	KeepRePCount = RepCount
	KeepRunTimeOut =RunTimeOut

	ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Start a new run in all modules
	if(ret < 0)  // Failed to start a new run
		DoAlert 0, "Failed to start a new run. Please reboot the Pixie modules."
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(ret)
	endif

	Sleep/T 10
	PollingTime = 0
	WaitStartTime=DateTime
	StartTime=DateTime
	repeat = 1
	aborted=0
	
	print "Press ''ESC'' to abort, ''Shift'' to update spectra"
	
	do	// loop over total run time
		do	// wait MCA save interval
			Sleep/T 10		
			TimeDiff=DateTime - WaitStartTime
			RunTimeOut= KeepRunTimeOut - ( DateTime - StartTime)
			if (GetKeyState(0) & 32)
				repeat=0
			endif	
			if (GetKeyState(0) & 4)
				Pixie_RC_UpdateMCAData("UpdateMCA")
			endif	
			DoUpdate
		while ( (TimeDiff < TNewFile) && (repeat==1) )
		
		currentTime = DateTime
		if( ((currentTime - StartTime) < KeepRunTimeOut) && (repeat==1)  )
			WaitStartTime=DateTime
			Pixie4_Acquire_Data(0x3000, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
			Pixie_RC_AutoSaveDAQFiles()
			RunType = RunTasks[WhichRun-1]+0x1000 //New Run after changing file names
			Pixie_RC_MakeRunFiles(RunType)
			Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
		else
			repeat = 0
		endif
	while(repeat==1)		
	
	Pixie_RC_StopRun(" repeat_scan")
	PollingTime = 0.1
	
End




//########################################################################
//
//	Pixie_RC_DMACheckRun:
//		Check the run status in all modules and store data
// 		most of functionality in C library
//		ionly for Pixie-500e DMA runs.      
//
//########################################################################
Function Pixie_RC_DMACheckRun(s)		// This is the function that will be called periodically
	STRUCT WMBackgroundStruct &s

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar AutoUpdateMCA= root:pixie4:AutoUpdateMCA
	Nvar AutoUpdateRunStats = root:pixie4:AutoUpdateRunStats
	Nvar Tupdate = root:pixie4:Tupdate
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	Nvar TNewFile = root:pixie4:TNewFile
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar OldCSR = root:pixie4:OldCSR
	Svar OutputFileName = root:pixie4:OutputFileName
	Wave RunTasks = root:pixie4:RunTasks
	Wave dummy = root:pixie4:dummy
		
	Svar ReqStopDate = root:pixie4:ReqStopDate	
	Svar ReqStopTime = root:pixie4:ReqStopTime
	Nvar StopDTreq = root:pixie4:StopDTreq
	Nvar  SpillCountNewFile =  root:pixie4:SpillCountNewFile
	Nvar TotalSpillCount = root:Pixie4:TotalSpillCount
	Nvar StartTime_s = root:pixie4:StartTime_s
	Nvar SeriesStartTime_s = root:pixie4:SeriesStartTime_s
	
	// local variables
	Variable RunType,ret,direction,channum,i,index0,index1, updated, saved, CSR, DataReady, stoppedrun, elapsed
		
	updated=0
	saved=0
	stoppedrun=0
	
	// poll module	
	RunType=RunTasks[WhichRun-1]+0x4000		// polling actions depend on runtype 
	CSR = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)	//poll and if DMA idle, save data
	// returns total number of spills saved, or negative number if something went wrong
	//print CSR

	if(CSR < 0)	// failed to poll
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		DoAlert 0, "Failed to check run status. Please reboot the Pixie modules"		
		// For P4e, better not talk to the module again, it may lock up PC
			//RunType=RunTasks[WhichRun-1]+0x3000  //  Stop  run and save list mode event data
			//ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules		
			//if(ret < 0)  // failed to stop the run
			//	print "Failed to stop the run. Please reboot the Pixie modules."
			//endif				
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(CSR)
	endif
	
	RepCount = KeepRepCount - CSR	- TotalSpillCount // decrease spill count by C lib accumulated total
	SpillTimeOut=inf//KeepSpillTimeOut	// restore timeout for next spill
	// spill time out does not apply to DMA runs
	
	// decrease timers
	//elapsed = DateTime - StartTime_s
	elapsed = DateTime - SeriesStartTime_s
	
	RunTimeOut = KeepRunTimeOut - elapsed
	SpillTimeOut = KeepSpillTimeOut - elapsed
	//RunTimeOut -= PollingTime
	//SpillTimeOut -= PollingTime
	
	// check if target date/time reached	
 	if(StopDTreq)
 		if( (cmpstr(ReqStopDate,Date()) == 0) && (cmpstr(ReqStopTime,Time()) == 0) )
			RunTimeOut =0
		endif	 	
	endif
	
	if((RepCount<=0) || (RunTimeOut<=0)  )	// TODO need to restart run after spill timeout
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		Pixie_RC_StopRun("RunEndDAQ2a")  
	else	
		// when run active, update spectrum and/or runstats occasionally
	  	if( ( mod(KeepRunTimeOut-RunTimeOut,Tupdate)>=Tupdate-PollingTime)  & (RunTimeOut>Tupdate)  )
	 		if( AutoUpdateMCA==1) 
	 			Pixie_RC_UpdateMCAData("UpdateMCA")
	 		else
				if (AutoUpdateRunStats)
					Pixie_RC_UpdateRunstats("module")
				endif	
			endif 			
		endif
	
		//occasionally save all auto files and go to next run number
		if( (AutoNewFile==1) && (CSR >= TNewFile-1)  && (RepCount >= TNewFile-1) && (CSR != OldCSR) )
			TotalSpillCount += CSR
			OldCSR = CSR	
			//print TotalSpillCount
		 		
		 	// make new files
		 	Pixie_RC_AutoSaveDAQFiles()
		 	Pixie_RC_UpdateMCATotals()		// MCA are cumulative in DMA runs 0x400-403 
		 	Pixie_RC_AutoRestoreSync()
		 							
		 	Pixie_RC_MakeRunFiles(RunType)
		 	Pixie_RC_ClearTrack()
		
			// restart
		 	RunType = RunTasks[WhichRun-1]+0x2000 //0x2000 informs LM polling/recording routine to go to new file name after next execution
		 	ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)
			if(ret < 0)	// failed to resume run
				//CtrlBackground Stop  // Stop the background first
				CtrlNamedBackground CheckRun kill
				DoAlert 0, "Failed to resume run."
				Pixie_RC_RestoreRunTab()  //restore run tab settings
				return(ret)
			endif
							
			///////////////////////////////////////////////////////////// 
			// 	Call to user routine			      //
				User_NewFileDuringRun("List")		//
			////////////////////////////////////////////////////////////	
		endif
	endif
						 		
	return(0)		// IGOR 5 needs a return value for Background task to work
			
End

Function Pixie_RC_UpdateMCATotals()

	Wave RunTasks = root:pixie4:RunTasks
	Nvar WhichRun = root:pixie4:WhichRun
	Svar MCAFileName = root:pixie4:MCAFileName

	Variable Runtype
	RunType=RunTasks[WhichRun-1]

	Wave MCAtotal0 = root:pixie4:MCAtotal0
	Wave MCAtotal1 = root:pixie4:MCAtotal1
	Wave MCAtotal2 = root:pixie4:MCAtotal2
	Wave MCAtotal3 = root:pixie4:MCAtotal3
	Wave MCAtotalsum = root:pixie4:MCAtotalsum
	Wave MCAch0 = root:pixie4:MCAch0
	Wave MCAch1 = root:pixie4:MCAch1
	Wave MCAch2 = root:pixie4:MCAch2
	Wave MCAch3 = root:pixie4:MCAch3
	Wave MCAsum = root:pixie4:MCAsum
	
	Wave MCAtotalold0 = root:pixie4:MCAtotalold0
	Wave MCAtotalold1 = root:pixie4:MCAtotalold1
	Wave MCAtotalold2 = root:pixie4:MCAtotalold2
	Wave MCAtotalold3 = root:pixie4:MCAtotalold3
	
	if(Runtype==0x301)
		MCAtotal0 +=  MCAch0
		MCAtotal1 +=  MCAch1
		MCAtotal2 +=  MCAch2
		MCAtotal3 +=  MCAch3
		MCAtotalsum +=  MCAsum
	endif
	
	if(Runtype==0x400)
		MCAtotalold0 = MCAtotal0
		MCAtotalold1 = MCAtotal1
		MCAtotalold2 = MCAtotal2
		MCAtotalold3 = MCAtotal3
	endif
	

End

// This function tries to avoid crashes in multi-file DMA runs (avoiding background task)
// to be called from the command line only
Function Pixie_RepeatDMA(ctrlName) : ButtonControl
String ctrlName

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar PanelTabNumber=root:pixie4:PanelTabNumber
	Nvar NumberOfModules=root:pixie4:NumberOfModules
	Svar OutputFileName = root:pixie4:OutputFileName
	Nvar RunInProgress = root:pixie4:RunInProgress
	Wave RunTasks = root:pixie4:RunTasks	
	Nvar TNewFile = root:pixie4:TNewFile
	Wave dummy = root:pixie4:dummy
	Nvar  SpillCountNewFile =  root:pixie4:SpillCountNewFile
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar RunNumber = root:pixie4:RunNumber
	Svar OutBaseName = root:pixie4:OutBaseName
	Svar OutputFileName = root:pixie4:OutputFileName
	Svar DataFile = root:pixie4:DataFile
	Svar LongDataFileName = root:pixie4:LongDataFileName
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar TotalSpillCount = root:Pixie4:TotalSpillCount

	if(RunInProgress)
		print "Run in progress. Please stop run before attempting this task"
		return(0)
	endif
	
//	if(WhichRun !=7)
//		print "Fast Multi-File run currently only supported in Mode 0x401"
//		return(0)
//	endif
	
	SpillTimeOut = inf

	// local variables
	Variable RunType, filenum, WaitStartTime, TimeDiff, ret,repeat, aborted, StartTime, currentTime
	String filename, ErrMSG
	Variable CSR
	
	// The first run is always a new run
	RunType = RunTasks[WhichRun-1]+0x1000

	// disable controls
	RunInProgress = 1
	Pixie_Tab_Switch("",PanelTabNumber)
	
	//make all the new run files
	Pixie_RC_MakeRunFiles(RunType)
	
	KeepRepCount = RepCount
	KeepRunTimeOut =RunTimeOut

	ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Start a new run in all modules
	if(ret < 0)  // Failed to start a new run
		DoAlert 0, "Failed to start a new run. Please reboot the Pixie modules."
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(ret)
	endif

	Sleep/T 10
	PollingTime = 0
	WaitStartTime=DateTime
	StartTime=DateTime
	repeat = 1
	aborted=0
	TotalSpillCount = 0
	
	print "Press ''ESC'' to abort"// ''Shift'' to update spectra"
	
	do	// loop over total number of spilles
		do	// spill save period 
			
			// wait and count time
			Sleep/T 1		
			RunTimeOut= KeepRunTimeOut - ( DateTime - StartTime)
			
			// check for abort
			if (GetKeyState(0) & 32)
				repeat=0
			endif	
//			if (GetKeyState(0) & 4)
//				Pixie_RC_UpdateMCAData("UpdateMCA")
//			endif	
//			DoUpdate

			// check for timeout
			currentTime = DateTime
			if( (currentTime - StartTime) > KeepRunTimeOut) 
				repeat=0	
			endif 
			
			// poll module	
			RunType=RunTasks[WhichRun-1]+0x4000		// polling actions depend on runtype 
			CSR = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)	//poll and if DMA idle, save data, CSR is spill counted since last file change
			if(CSR < 0)	// failed to poll
				print "Failed to check run status."
				RunType=RunTasks[WhichRun-1]+0x3000  //  Stop  run and save list mode event data
				ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules
				if(ret < 0)  // failed to stop the run
					print "Failed to stop the run. Please reboot the Pixie modules."
				endif
					
				Pixie_RC_RestoreRunTab()  // restore run tab settings
				return(CSR)
			endif
			
			RepCount = KeepRepCount -  CSR	- TotalSpillCount				// decrease spill count		
			if(RepCount<=0) 
				repeat = 0	
			endif		
			DoUpdate

		while ( (CSR < TNewFile) && (repeat==1) )
	//	print repcount, CSR
		
		// make new files
		if( (repeat==1)  && (CSR >= TNewFile) && (RepCount >= TNewFile) )
			TotalSpillCount +=CSR			// update spill count at this file change
			
			// save mca, set, ifm files	(cumulative), increment run number
			Pixie_RC_AutoSaveDAQFiles()
			
			// make new files
			RunType=RunTasks[WhichRun-1]+0x2000
			Pixie_RC_MakeRunFiles(RunType)
			
			// just change filenames
			print "Changing file names to",OutputFileName, "at spill count ",RepCount
			ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)		// "resume task" to change filenames
			if(ret < 0)	// failed to resume run
				print "Failed to resume run."
				repeat = 0	
				Pixie_RC_RestoreRunTab()  //restore run tab settings
				return(ret)
			endif
							
			///////////////////////////////////////////////////////////// 
			// 	Call to user routine			      //
				User_NewFileDuringRun("List")		//
			////////////////////////////////////////////////////////////	
		endif
		
	while(repeat==1)		
	
	Pixie_RC_StopRun("RUN_EndDAQ")
	PollingTime = 0.1
	//SpillTimeOut = KeepSpillTimeOut
	
End

//########################################################################
//
//	Find the index of  a system parameter
//
//########################################################################
Function Pixie_Find_SysParInWave(name)
String name

	Wave/T System_Parameter_Names = root:pixie4:System_Parameter_Names
	Variable i, wavelen,index
	
	i=0
	wavelen = numpnts(System_Parameter_Names)
	do	
		if(cmpstr(name, System_Parameter_Names[i]) == 0)
			index=i
			break
		endif
		i=i+1	
	while(i<wavelen)

	return(index)
End


//########################################################################
//
//	Find the index of  a module parameter
//
//########################################################################
Function Pixie_Find_ModuleParInWave(name)
String name

	Wave/T Module_Parameter_Names = root:pixie4:Module_Parameter_Names
	Variable i, wavelen,index
	
	i=0
	wavelen = numpnts(Module_Parameter_Names)
	do	
		if(cmpstr(name, Module_Parameter_Names[i]) == 0)
			index=i
			break
		endif
		i=i+1	
	while(i<wavelen)

	return(index)
End


//########################################################################
//
//	Find the index of  a channel parameter
//
//########################################################################
Function Pixie_Find_ChanParInWave(name)
String name

	Wave/T Channel_Parameter_Names = root:pixie4:Channel_Parameter_Names
	Variable i, wavelen,index
	
	i=0
	wavelen = numpnts(Channel_Parameter_Names)
	do	
		if(cmpstr(name, Channel_Parameter_Names[i]) == 0)
			index=i
			break
		endif
		i=i+1	
	while(i<wavelen)

	return(index)
End


//########################################################################
//
//	Some bit manipulations, for 16-bit words
//
//########################################################################
Function CLRbit(bit,value)
	Variable value,bit
	value=SETbit(bit,value)
	return(value %^ (2^bit) )
End

Function SETbit(bit,value)
	Variable value,bit
	return(value %| ( 2^bit) )
End

Function TGLbit(bit,value)
	Variable value,bit
	return(value %^ ( 2^bit) )
End

Function TSTbit(bit,value)
	Variable value,bit
	return((value %& ( 2^bit))/2^bit )
End


//########################################################################
//
//	Read memory buffer content of a Pixie module
//
//########################################################################
Function Pixie_EX_MemoryScan(ModNum)
Variable ModNum

	Wave MemoryValues = root:pixie4:MemoryValues	
	Variable direction, type, channum
	
	direction = 1	// Read from a Pixie module
	type = 1	// DSP memory parameters
	channum = 0	// Channel number can be any channel here
	
	Pixie4_Set_Current_ModChan(channum, ModNum)
	Pixie4_Buffer_IO(MemoryValues, type, direction, "", ModNum)
	root:pixie4:MemoryValues=MemoryValues
End


//########################################################################
//
//	Find the value of a memory buffer location
//
//########################################################################
Function Pixie_EX_FindVar(ModNum, str)
String str
Variable ModNum
	
	
	Wave MemoryValues = root:pixie4:MemoryValues
	Variable k,addr,val,n,ndat,len
	
	Pixie_EX_MemoryScan(ModNum)
	
	Wave/T MemoryNames=root:pixie4:MemoryNames4
	ndat=numpnts(MemoryNames)
	str=UpperStr(str)
	print "Pixie- 4 or Pixie-500"
	
	for(k=0; k<ndat; k+=1)
		n=strsearch(MemoryNames[k],str,0)
		if(n!=-1)
			val=MemoryValues[k]
			Print MemoryNames[k],"@",k,"=",val
		endif
	endfor
	
	// repeat for P500e names
	Wave/T MemoryNames=root:pixie4:MemoryNames500e
	ndat=numpnts(MemoryNames)
	str=UpperStr(str)	
	print "Pixie-500e"

	for(k=0; k<ndat; k+=1)
		n=strsearch(MemoryNames[k],str,0,2)
		if(n!=-1)
			val=MemoryValues[k]
			Print MemoryNames[k],"@",k,"=",val
		endif
	endfor
	
	return val
End

//########################################################################
//
//	Find the location of a memory buffer location called by naem
//
//########################################################################
Function Pixie_EX_FindVarLoc(ModNum, str)
String str
Variable ModNum
	
	
	Wave MemoryValues = root:pixie4:MemoryValues
	Variable k,addr,val,n,ndat,len, kvar
	
	Pixie_EX_MemoryScan(ModNum)
	
	Wave/T MemoryNames=root:pixie4:MemoryNames4
	ndat=numpnts(MemoryNames)
	str=UpperStr(str)
	print "Pixie- 4 or Pixie-500"
	
	for(k=0; k<ndat; k+=1)
		n=strsearch(MemoryNames[k],str,0)
		if(n!=-1)
			val=MemoryValues[k]
			Print MemoryNames[k],"@",k,"=",val
		endif
	endfor
	
	// repeat for P500e names
	Wave/T MemoryNames=root:pixie4:MemoryNames500e
	ndat=numpnts(MemoryNames)
	str=UpperStr(str)	
	print "Pixie-500e"

	for(k=0; k<ndat; k+=1)
		n=strsearch(MemoryNames[k],str,0,2)
		if(n!=-1)
			val=MemoryValues[k]
			Print MemoryNames[k],"@",k,"=",val
			kvar = k
		endif
	endfor
	
	return kvar
End


//########################################################################
//
//	Functions used to download DSP parameters to a Pixie module
//
//########################################################################
Function Pixie_EX_PutModPar()

	Wave DSPValues = root:pixie4:DSPValues
	Nvar ChosenModule = root:pixie4:ChosenModule
	Variable direction, type
	
	direction = 0	// Download to a Pixie module
	type = 0	// DSP I/O parameters
	
	Pixie4_Buffer_IO(DSPValues, type, direction,"", ChosenModule)

	// Update module parameters
	Pixie_IO_ReadModParFromMod(ChosenModule)
	// Update channel parameters
	Pixie_IO_ReadChanParFromMod(ChosenModule)

	// Update Igor controls
	Pixie_IO_SelectModChan("LoadSettings", 0, "", "")
End

Function Pixie_EX_CallPutModPar(CtrlName):ButtonControl
String CtrlName
	Pixie_EX_PutModPar()
End


//########################################################################
//
//	Find the index of a DSP parameter
//
//########################################################################
Function Pixie_Find_DSPname(str)
String str
	
	Wave/T DSPnames = root:pixie4:DSPnames

	Variable k,addr,val,n,ndat,len
		
	ndat=numpnts(DSPnames)
	str=UpperStr(str)

	for(k=0; k<ndat; k+=1)
		n=strsearch(DSPnames[k],str,0)
		if(n!=-1)
			return(k)
		endif
	endfor
	return(-1)
End





//########################################################################
//
//	Print a decimal number in hexadecimal format
//
//########################################################################
Function ph(num)
Variable num

	printf "0x%X\r",num

End



//########################################################################
//
//	Initialize global variables and prepare for booting Pixie modules
//
//########################################################################
Function Pixie_StartUp()

	Wave/T AllFiles=root:pixie4:AllFiles

	Svar FPGAFile4e = root:pixie4:FPGAFile4e
	Svar CommFPGAFile4 = root:pixie4:CommFPGAFile4
	Svar CommFPGAFile500 = root:pixie4:CommFPGAFile500
	Svar SPFPGAFile4 = root:pixie4:SPFPGAFile4
	Svar SPFPGAFile500 = root:pixie4:SPFPGAFile500
	Svar FPGAFile500e = root:pixie4:FPGAFile500e
	Svar DSPCodeFile4 = root:pixie4:DSPCodeFile4
	Svar DSPCodeFile500 = root:pixie4:DSPCodeFile500
	Svar DSPCodeFile500e = root:pixie4:DSPCodeFile500e
	Svar DSPVarFile4 = root:pixie4:DSPVarFile4
	Svar DSPListFile4 = root:pixie4:DSPListFile4
	Svar DSPListFile500e = root:pixie4:DSPListFile500e
	
	Svar DSPParaValueFile = root:pixie4:DSPParaValueFile

	Nvar NumberOfModules=root:pixie4:NumberOfModules
	Nvar Pixie4Offline=root:pixie4:Pixie4Offline
	Nvar AutoProcessLMData=root:pixie4:AutoProcessLMData
	Nvar MaxNumModules=root:pixie4:MaxNumModules
	Nvar KeepCW =  root:pixie4:KeepCW
	Wave ModuleSlotNumber=root:pixie4:ModuleSlotNumber
	Wave/T System_Parameter_Names=root:pixie4:System_Parameter_Names
	Wave/T Module_Parameter_Names=root:pixie4:Module_Parameter_Names
	Wave/T Channel_Parameter_Names=root:pixie4:Channel_Parameter_Names
	Wave System_Parameter_Values=root:pixie4:System_Parameter_Values

	Variable retval, index, k, direction, modnum, channum

	//***********************************************************	
	// Convert all file names from Igor format to Windows/DOS format
	//***********************************************************
	
	allfiles[0] = Pixie_FileNameConversion(FPGAFile4e)			// used for P4e-16/125 or other variants
	allfiles[1] = Pixie_FileNameConversion(CommFPGAFile4)
	allfiles[2] = Pixie_FileNameConversion(SPFPGAFile4)
	allfiles[3] = Pixie_FileNameConversion(DSPCodeFile4)
	allfiles[4] = Pixie_FileNameConversion(DSPParaValueFile)
	allfiles[5] = Pixie_FileNameConversion(DSPVarFile4)
	allfiles[6] = Pixie_FileNameConversion(DSPListFile4)
	allfiles[7] = Pixie_FileNameConversion(CommFPGAFile500)
	allfiles[8] = Pixie_FileNameConversion(SPFPGAFile500)		// also used for P4e-14/500
	allfiles[9] = Pixie_FileNameConversion(FPGAFile500e)
	allfiles[10] = Pixie_FileNameConversion(DSPCodeFile500)
	allfiles[11] = Pixie_FileNameConversion(DSPCodeFile500e)
	allfiles[12] = Pixie_FileNameConversion(DSPListFile500e)

	
	//***********************************************************	
	//  Initialize system parameter values
	//***********************************************************
	System_Parameter_Values[Pixie_Find_SysParInWave("NUMBER_MODULES")]=NumberOfModules	
	System_Parameter_Values[Pixie_Find_SysParInWave("OFFLINE_ANALYSIS")]=Pixie4Offline
	System_Parameter_Values[Pixie_Find_SysParInWave("AUTO_PROCESSLMDATA")]=AutoProcessLMData
	System_Parameter_Values[Pixie_Find_SysParInWave("KEEP_CW")]=KeepCW
	System_Parameter_Values[Pixie_Find_SysParInWave("MAX_NUMBER_MODULES")]=MaxNumModules
	index=Pixie_Find_SysParInWave("SLOT_WAVE")
	for(k=0; k<NumberOfModules; k+=1)
		System_Parameter_Values[index+k]=ModuleSlotNumber[k]
	endfor

	//***********************************************************	
	//  Download global parameter names -- really should be the other way round, reading names from C lib
	//  Now we still have 2 sets of definitions that must be matched manually
	//  Plus, for some reason Hand_Down_Names crshes with sys/mod/chan unless called step by step in debugger mode (on 64 bit Win only?)
	//***********************************************************
//	retval = Pixie4_Hand_Down_Names(System_Parameter_Names, "SYSTEM")
//	if( retval < 0 )
//		return(retval)  // download failure
//	endif
//		
//	retval = Pixie4_Hand_Down_Names(Module_Parameter_Names, "MODULE")
//	if( retval < 0 )
//		return(retval)  // download failure
//	endif
//	
//	retval = Pixie4_Hand_Down_Names(Channel_Parameter_Names, "CHANNEL")	
//	if( retval < 0 )
//		return(retval)  // download failure
//	endif
	
	//***********************************************************	
	// Download system parameter values
	//***********************************************************
	direction = 0	// download to XOP
	modnum = 0	// module number is not used for system parameter values
	channum = 0	// channel number is not used for system parameter values
	
	retval = Pixie4_User_Par_IO(System_Parameter_Values, "NUMBER_MODULES", "SYSTEM", direction, modnum, channum)
	if( retval < 0 )
		return(retval)  // download failure
	endif
	retval = Pixie4_User_Par_IO(System_Parameter_Values, "OFFLINE_ANALYSIS", "SYSTEM", direction, modnum, channum)
	if( retval < 0 )
		return(retval)  // download failure
	endif
	retval = Pixie4_User_Par_IO(System_Parameter_Values, "AUTO_PROCESSLMDATA", "SYSTEM", direction, modnum, channum)
	if( retval < 0 )
		return(retval)  // download failure
	endif
	retval = Pixie4_User_Par_IO(System_Parameter_Values, "MAX_NUMBER_MODULES", "SYSTEM", direction, modnum, channum)
	if( retval < 0 )
		return(retval)  // download failure
	endif
	retval = Pixie4_User_Par_IO(System_Parameter_Values, "SLOT_WAVE", "SYSTEM", direction, modnum, channum)
	if( retval < 0 )
		return(retval)  // download failure
	endif
	
	//***********************************************************	
	// Download file names
	//***********************************************************
	
	// Verify files exist first
	Variable filesok =1
	Variable fnum	
	
	open/Z/R fnum as FPGAFile4e
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif

	open/Z/R fnum as CommFPGAFile4
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
//	open/Z/R fnum as CommFPGAFile500
//	if(V_flag!=0)
//		filesok=0
//	else
//		close fnum
//	endif
	
	open/Z/R fnum as SPFPGAFile4
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
//	open/Z/R fnum as SPFPGAFile500
//	if(V_flag!=0)
//		filesok=0
//	else
//		close fnum
//	endif
	
	open/Z/R fnum as FPGAFile500e
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	open/Z/R fnum as DSPCodeFile4
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
//	open/Z/R fnum as DSPCodeFile500
//	if(V_flag!=0)
//		filesok=0
//	else
//		close fnum
//	endif
	
	open/Z/R fnum as DSPCodeFile500e
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	open/Z/R fnum as DSPParaValueFile
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	open/Z/R fnum as DSPVarFile4
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	open/Z/R fnum as DSPListFile4
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	open/Z/R fnum as DSPListFile500e
	if(V_flag!=0)
		filesok=0
	else
		close fnum
	endif
	
	if(filesok==0)
		DoAlert 1, "One or more of the boot files specified does not exist. Run macro ''UseHomePaths'' to set to default?"
		if(V_flag==1)
			Execute "Pixie_UseHomePaths()"
			allfiles[0] = Pixie_FileNameConversion(FPGAFile4e)
			allfiles[1] = Pixie_FileNameConversion(CommFPGAFile4)
			allfiles[2] = Pixie_FileNameConversion(SPFPGAFile4)
			allfiles[3] = Pixie_FileNameConversion(DSPCodeFile4)
			allfiles[4] = Pixie_FileNameConversion(DSPParaValueFile)
			allfiles[5] = Pixie_FileNameConversion(DSPVarFile4)
			allfiles[6] = Pixie_FileNameConversion(DSPListFile4)
			allfiles[7] = Pixie_FileNameConversion(CommFPGAFile500)
			allfiles[8] = Pixie_FileNameConversion(SPFPGAFile500)
			allfiles[9] = Pixie_FileNameConversion(FPGAFile500e)
			allfiles[10] = Pixie_FileNameConversion(DSPCodeFile500)
			allfiles[11] = Pixie_FileNameConversion(DSPCodeFile500e)
			allfiles[12] = Pixie_FileNameConversion(DSPListFile500e)
		else
			print "Bootfiles not found Please open the Files panel and correct the path and/or file name"
			return(-1)  // name failure
		endif
	endif
	
	
	Pixie4_Hand_Down_Names(AllFiles, "ALL_FILES")
	if( retval < 0 )
		return(retval)  // download failure
	endif
	
End


//########################################################################
//
//	Acquire ADC traces from 4 channels of a Pixie module
//
//########################################################################
Function Pixie_IO_ADCTrace()

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar ADCTraceLen = root:pixie4:ADCTraceLen
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Variable k,dt,ret
	String wav, msg

	//**************************************************************************************
	//	Special considerations: for a large wave, we kill it before making a new one
	//	even though we intend to overwrite the previous one. According to WaveMetrics,
	//	killwaves should let Windows OS to release the memory.
	//**************************************************************************************
	killwaves/Z root:pixie4:ADCWave
	
	Make/o/u/i/n=(ADCTraceLen*NumberOfChannels) root:pixie4:ADCWave
	Wave ADCWave = root:pixie4:ADCWave

	// Acquire ADC traces
	ret = Pixie4_Acquire_Data(0x0084,ADCWave,"", ChosenModule)
	if(ret < 0)
		msg="Acquire ADC trace for Module "+num2str(ChosenModule)+" failed."
		DoAlert 0, msg
		return(ret)  // ADC trace acquisition failed
	endif

	// Update ADC trace for each channel
	for(k=0; k<NumberOfChannels; k+=1)
		wav="root:pixie4:ADCch"+num2str(k)
		Wave wv=$wav
		wv = ADCWave[p+k*ADCTraceLen]
	
		dt = Channel_Parameter_Values[Pixie_Find_ChanParInWave("XDT")][k][ChosenModule]*1e-6
		//print dt
		SetScale/P x,0,dt,"s",wv
		wv[0] = wv[3]	// hide spike from data transfer in first few words
		wv[1] = wv[3]
		wv[2] = wv[3]
	endfor

	// FFT analysis on ChosenChannel's ADC trace
	wav="root:pixie4:ADCch"+num2str(ChosenChannel)
	Pixie_FFTtrace($wav)
	
	// recompute filter
	Pixie_FilterADCTraceCalc($wav)
	

	// remove tau fit
	DoWindow/F Pixie4Oscilloscope
	if(V_flag==1)
		RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch0
		RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch1
		RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch2
		RemoveFromGraph/W= Pixie4Oscilloscope/Z Res_ADCch3
		RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch0
		RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch1
		RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch2
		RemoveFromGraph/W= Pixie4Oscilloscope/Z fit_ADCch3
	endif
	
	// update ICR, OORF
	Pixie_RC_UpdateRunstats("module")
	
End






//########################################################################
//
//	Pixie_EX_GetSystemParameters:
//		Retrieve system parameters
//
//########################################################################
Function Pixie_EX_GetSystemParameters(ctrlName) : ButtonControl
String ctrlName

	Wave/T System_Parameter_Names = root:Pixie4:System_Parameter_Names
	Wave System_Parameter_Values = root:Pixie4:System_Parameter_Values
	Nvar ChosenModule = root:pixie4:ChosenModule
	Variable direction, channum

	Pixie_IO_ReadSysParFromMod()
	
	DoWindow/F ModuleGlobalView
	if(V_Flag == 1)
		return(0)
	endif
	
	Edit/K=1/W=(5,45,375,500) System_Parameter_Names,System_Parameter_Values
	DoWindow/C ModuleGlobalView
	Execute "ModifyTable width(root:pixie4:System_Parameter_Names)=135"
	Execute "ModifyTable width(root:pixie4:System_Parameter_Values)=135"
End


//########################################################################
//
//	Pixie_EX_GetModuleParameters:
//		Retrieve module parameters from all Pixie modules.
//
//########################################################################
Function Pixie_EX_GetModuleParameters(ctrlName) : ButtonControl
String ctrlName

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberofChannels = root:pixie4:NumberofChannels
	Wave/T Module_Parameter_Names = root:Pixie4:Module_Parameter_Names
	Wave Module_Parameter_Values = root:Pixie4:Module_Parameter_Values
	
	Variable i
	
	Pixie_IO_ReadModParFromMod(NumberOfModules)
	
	DoWindow/F GlobalDataView
	if(V_Flag == 1)
		return(0)
	endif
	
	Edit/K=1/W=(5,45,600,500) Module_Parameter_Names,Module_Parameter_Values
	DoWindow/C GlobalDataView
	Execute "ModifyTable width(root:pixie4:Module_Parameter_Names)=155"
	Execute "ModifyTable width(root:pixie4:Module_Parameter_Values)=140"
	
End


//########################################################################
//
//	Pixie_EX_GetChanParameters:
//		Read channel parameters from all Pixie modules.
//
//########################################################################
Function Pixie_EX_GetChanParameters(ctrlName) : ButtonControl
String ctrlName

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberofChannels = root:pixie4:NumberofChannels
	Wave/T Channel_Parameter_Names = root:Pixie4:Channel_Parameter_Names
	Wave Channel_Parameter_Values = root:Pixie4:Channel_Parameter_Values
	
	Variable i,j
	
	Pixie_IO_ReadChanParFromMod(NumberOfModules)
	
	DoWindow/F UserView
	if(V_Flag == 1)
		return(0)
	endif
	
	Edit/K=1/W=(5,45,575,525) Channel_Parameter_Names,Channel_Parameter_Values
	DoWindow/C UserView
	Execute "ModifyTable width(root:pixie4:Channel_Parameter_Names)=125"
	Execute "ModifyTable width(root:pixie4:Channel_Parameter_Values)=155"
End


//########################################################################
//
//	Pixie_EX_GetDSPValues:
//		Read DSP I/O parameters from a Pixie module
//
//########################################################################
Function Pixie_EX_GetDSPValues(ctrlName) : ButtonControl
String ctrlName

	Wave/T DSPNames = root:pixie4:DSPNames
	Wave DSPValues = root:pixie4:DSPValues
	Nvar ChosenModule = root:pixie4:ChosenModule
	
	Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
	root:pixie4:DSPValues=DSPValues
	
	DoWindow/F DSPView
	if(V_Flag == 1)
		return(0)
	endif
	
	Edit/K=1/W=(50,100,350,450) DSPNames,DSPValues
	DoWindow/C DSPView
	Execute "ModifyTable width(:pixie4:DSPNames)=120"
	
End


//########################################################################
//
//	Pixie_EX_GetMemoryValues:
//		Read DSP memory values from a Pixie module
//
//########################################################################
Function Pixie_EX_GetMemoryValues(ctrlName) : ButtonControl
String ctrlName

	Wave/T MemoryNames4 = root:pixie4:MemoryNames4
	Wave/T MemoryNames500e = root:pixie4:MemoryNames500e
	Wave MemoryValues = root:pixie4:MemoryValues
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	
	if(Pixie4Offline == 1)
		MemoryValues=0
	else
		Pixie4_Buffer_IO(MemoryValues, 1, 1,"", ChosenModule)
		root:pixie4:MemoryValues=MemoryValues
	endif
	
	DoWindow/F MemoryView
	if(V_Flag == 1)
		return(0)
	endif
	
	Edit/K=1/W=(50,100,375,450) MemoryNames4,MemoryNames500e,MemoryValues
	DoWindow/C MemoryView
	Execute "ModifyTable width(:pixie4:MemoryNames4)=150"
	Execute "ModifyTable width(:pixie4:MemoryNames500e)=150"
	
End




Function Pixie_IO_ReadEvent()

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar ChosenEvent = root:pixie4:ChosenEvent
//	Nvar ADCClockMHz = root:pixie4:ADCClockMHz
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_BHL = root:pixie4:index_BHL
	Nvar index_EHL = root:pixie4:index_EHL
	Nvar index_CHL = root:pixie4:index_CHL
	
	Nvar EventHitpattern = root:Pixie4:EventHitpattern
	Nvar EventTimeHI = root:Pixie4:EventTimeHI
	Nvar EventTimeLO = root:Pixie4:EventTimeLO
	Nvar DisplayTimeWindow = root:pixie4:DisplayTimeWindow
	Nvar Show4LMtraces =  root:pixie4:Show4LMtraces
	Nvar wftimescale = root:pixie4:wftimescale			// sample clock in MHz as read from the file

	
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename
	
	wave listmodewave = root:pixie4:listmodewave
	Wave ListModeChannelEnergy=root:pixie4:ListModeChannelEnergy
	Wave ListModeChannelTrigger=root:pixie4:ListModeChannelTrigger
	Wave ListModeChannelXIA=root:pixie4:ListModeChannelXIA
	Wave ListModeChannelUser=root:pixie4:ListModeChannelUser
	wave eventposlen = root:pixie4:eventposlen
	
	// local variables
	Variable i,j,len,NumTraces,NumEvents,ModLoc,index,filnum,totaltraces,dt,ret
	Variable tmin, tmax, ew_header1, ew_header2
	String wav
	Variable BufHeadLen = 6
	Variable EventHeadLen = 3
	Variable RunFormat, t0
	
		
	ew_header1 = 7+BufHeadLen+EventHeadLen+4*9 //buffer/event/channel headers plus 7 words
	ew_header2 = 7+BufHeadLen+EventHeadLen       //buffer/event/channel headers plus 7 words

	// Check if there are events in this list mode file
	NumEvents = listmodewave[ChosenModule]
	if(NumEvents == 0)
		DoAlert 0, "There are no events in this list mode file for this module"
		return(-1)
	endif
			
	// ensure event is in range
	if(ChosenEvent >= NumEvents)
		ChosenEvent = NumEvents-1
	endif
	
	// locate starting point of event for the current module
	ModLoc=0
	for(i=0; i<ChosenModule; i+=1)
		ModLoc += 3*listmodewave[i]		// 3 entries per event * number of events in "previous" modules
	endfor
	
	//**************************************************************************************
	//	Special considerations: for a large wave, we kill it before making a new one
	//	even though we intend to overwrite the previous one. According to WaveMetrics,
	//	killwaves should let Windows OS to release the memory.
	//**************************************************************************************
	killwaves/Z root:pixie4:eventwave		
	len = eventposlen[ModLoc+ChosenEvent*3+2]+7+36 // ensure enough room for full channel headers
	make/o/u/i/n=(len) root:pixie4:eventwave
	wave eventwave = root:pixie4:eventwave
	eventwave=0
	eventwave[0] = eventposlen[ModLoc+ChosenEvent*3]		// event location
	eventwave[1] = eventposlen[ModLoc+ChosenEvent*3+1]	// corresponding buffer header location, gets the ADC rate
	eventwave[2] = eventposlen[ModLoc+ChosenEvent*3+2]	// length of event
	if(Show4LMtraces)
		DisplayTimeWindow =  floor(DisplayTimeWindow)
		DisplayTimeWindow = max(0,DisplayTimeWindow)	
		DisplayTimeWindow = min(65535,DisplayTimeWindow)	
		eventwave[3] = DisplayTimeWindow	// coincidence window
	else
		eventwave[3] = 100000	// >64K to switch off looking for related pulses
	endif
					
	// read event
	ret = Pixie4_Acquire_Data(0x7008, eventwave, longDataFilename, ChosenModule)
	if(ret < 0)
		DoAlert 0, "Error reading requested event from file"
		return(ret)
	endif
	
	// event header data
	RunFormat = eventwave[7+2+0]
	EventHitpattern = eventwave[7+6+0]
	EventTimeHI = eventwave[7+6+1]
	EventTimeLO = eventwave[7+6+2]
	
	// distribute traces and header values to the four channels and scale
	index=0
	t0=0
	for(i=0; i<NumberOfChannels; i+=1)
		wav="root:pixie4:trace"+num2str(i)
		
		//**************************************************************************************
		//	Special considerations: for a large wave, we kill it before making a new one
		//	even though we intend to overwrite the previous one. According to WaveMetrics,
		//	killwaves should let Windows OS to release the memory.
		//**************************************************************************************
		killwaves/Z $wav		
		len = eventwave[i+3]
		if(len != 0)
			make/u/i/o/n=(len ) $wav
			wave listtrace = $wav
			for(j=0; j<(len ); j+=1)
				listtrace[j]=eventwave[index+ew_header1]
				index += 1
			endfor
		else
			make/u/i/o/n=0 $wav
			wave listtrace = $wav
			listtrace=nan
		endif
		
	
		if((RunFormat & (2^(4+i))) >0)
			wftimescale = 1e-6/eventwave[1] *4		// eventwave[1] now returns ADC rate in MHz
		else
			wftimescale = 1e-6/eventwave[1] 
		endif	
		
		SetScale/P x,0,wftimescale,"s",$wav	
		
		//channel header data					
		ListModeChannelEnergy[i]=eventwave[ew_header2+i*9+2]
		ListModeChannelTrigger[i]=eventwave[ew_header2+i*9+1]
		ListModeChannelXIA[i]=eventwave[ew_header2+i*9+3]
		ListModeChannelUser[i]=eventwave[ew_header2+i*9+4]
		
	endfor
	
	Pixie_MakeList_Traces(1)
				
	Pixie_FilterLMTraceCalc()  // Calculate digital filter for the trace in the ChosenChannel
	
	variable tn				// make trace from selected event thicker than "coincident" ones
	tn = log(EventHitpattern&0xF)/log(2)
	DoWindow/F ListModeTracesDisplay
	if (V_Flag==1)
		ModifyGraph/Z/W=ListModeTracesDisplay lsize=1
		ModifyGraph/Z/W=ListModeTracesDisplay lsize($("trace"+num2str(tn)))=2
	endif
	
	// populate event info variables
	Nvar EvHit_Front = root:Pixie4:EvHit_Front
	Nvar EvHit_Accept = root:Pixie4:EvHit_Accept
	Nvar EvHit_Status = root:Pixie4:EvHit_Status
	Nvar EvHit_Token = root:Pixie4:EvHit_Token
	Nvar EvHit_CoincOK = root:Pixie4:EvHit_CoincOK
	Nvar EvHit_Veto = root:Pixie4:EvHit_Veto
	Nvar EvHit_PiledUp = root:Pixie4:EvHit_PiledUp
	Nvar EvHit_WvFifoFull = root:Pixie4:EvHit_WvFifoFull
	Nvar EvHit_ChannelHit = root:Pixie4:EvHit_ChannelHit
	Nvar EvHit_OOR = root:Pixie4:EvHit_OOR
	Nvar EvHit_Gate = root:Pixie4:EvHit_Gate
	Nvar EvHit_PSAovr = root:Pixie4:EvHit_PSAovr
	Nvar EvHit_Derror = root:Pixie4:EvHit_Derror
	EvHit_Front = (EventHitpattern&(2^4))>0
	EvHit_Accept =  (EventHitpattern&(2^5))>0
	EvHit_Status = (EventHitpattern&(2^6))>0
	EvHit_Token =  (EventHitpattern&(2^7))>0
	EvHit_CoincOK =  (EventHitpattern&(2^16))>0
	EvHit_Veto = (EventHitpattern&(2^17))>0
	EvHit_PiledUp = (EventHitpattern&(2^18))>0
	EvHit_WvFifoFull = (EventHitpattern&(2^19))>0
	EvHit_ChannelHit =  (EventHitpattern&(2^20))>0
	EvHit_OOR =  (EventHitpattern&(2^21))>0
	EvHit_Gate =  (EventHitpattern&(2^22))>0
	EvHit_PSAovr =  (EventHitpattern&(2^23))>0		
	EvHit_Derror =(EventHitpattern&(2^31))>0 
	
	//////////////////////////////////////////////////////////// 
	// Call to user routine			             //
	User_ReadEvent()
	////////////////////////////////////////////////////////////	
			
End




Function Pixie_ExtractNumPosLen()
	//Scan list mode data file and extract number of events and traces, as well as position and length of events
	// mode = "endrun" - only do for non- MCA runs
	
	
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar PRESET_MAX_MODULES = root:pixie4:PRESET_MAX_MODULES	
	Nvar NumberOfModules = root:pixie4:NumberOfModules

	
	Variable totaltraces,ret,len,i, totalevents
	
	
	// make wave to store list mode data information, two 32-bit words for each module
	
	//**************************************************************************************
	//	Special considerations: for a large wave, we kill it before making a new one
	//	even though we intend to overwrite the previous one. According to WaveMetrics,
	//	killwaves should let Windows OS to release the memory.
	//**************************************************************************************
	killwaves/Z root:pixie4:listmodewave
		
	make/o/u/i/n=(PRESET_MAX_MODULES*2) root:pixie4:listmodewave
	wave listmodewave = root:pixie4:listmodewave
	listmodewave = 0
	
	
	// Parse the list mode event file
	ret = Pixie4_Acquire_Data(0x7001, listmodewave, LongDataFileName, NumberOfModules)
	if(ret < 0)
		Doalert 0, "Can not parse list mode event file."
		return(ret)
	endif
	
	// force update
	root:pixie4:listmodewave = listmodewave

	 
	// prepare for reading events
	totalevents = 0
	for(i=0; i<PRESET_MAX_MODULES; i+=1)
		totalevents += listmodewave[i]
	endfor
			
	if(totalevents <= 0)
		print "There are no events in this list mode data file."
	else

		// make wave to store trace location (in list mode file), trace length, and energy of each event
		
		//**************************************************************************************
		//	Special considerations: for a large wave, we kill it before making a new one
		//	even though we intend to overwrite the previous one. According to WaveMetrics,
		//	killwaves should let Windows OS to release the memory.
		//**************************************************************************************
		killwaves/Z root:pixie4:eventposlen
		
		make/o/u/i/n=(totalevents*3) root:pixie4:eventposlen
		wave eventposlen = root:pixie4:eventposlen
		eventposlen = 0
		// Locate traces
		ret = Pixie4_Acquire_Data(0x7007, eventposlen, longDataFilename, NumberOfModules)
		if(ret < 0)
			Doalert 0, "Can not locate events in list mode event file."
			print ret
			return(ret)
		endif
	endif



End

Function Pixie_Boot1C(CtrlName):ButtonControl	// Boot DSP etc, but not the FPGA
String CtrlName

// global variables and waves
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar PRESET_MAX_MODULES = root:pixie4:PRESET_MAX_MODULES
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	
	Wave/T Module_Parameter_Names = root:pixie4:Module_Parameter_Names
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	Wave/T Channel_Parameter_Names = root:pixie4:Channel_Parameter_Names	
	Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
	Wave/T System_Parameter_Names = root:pixie4:System_Parameter_Names	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	Wave/T DSPNames = root:pixie4:DSPNames
	Wave DSPValues = root:pixie4:DSPValues
	

	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_FilterRange =  root:pixie4:index_FilterRange
	
	// local variables
	Variable len,filnum,i,j,totaltraces,ret,searchStr,lastcolonPos,dt,tmax,tmin,old_tau
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	
	Pixie4Offline = 0

			ret=Pixie_StartUp()
			if(ret<0)  // Download names failure
				DoAlert 0, "Failed to download system configuration file names and global variable names."
			endif
			
			ret=Pixie4_Boot_System(0x1C)
			if(ret<0)  // Download names failure
				DoAlert 0, "Failed to boot 1C."
			endif

			// Update system parameters
			Pixie_IO_ReadSysParFromMod()
			Pixie_IO_SendSysParToIgor()

			// Update module parameters
			Pixie_IO_ReadModParFromMod(NumberOfModules)
			Pixie_IO_SendModParToIgor()
			
			// Update channel parameters
			Pixie_IO_ReadChanParFromMod(NumberOfModules)
			Pixie_IO_SendChanParToIgor()
			
			// Set the Same Run Type to all modules globally and update their corresponding MaxEvents
			Pixie_Ctrl_CommonPopup("RunWhichRun", WhichRun, "")

			// Set the current module and channel
			if(ChosenModule > (NumberOfModules-1))
				ChosenModule = NumberOfModules - 1
			elseif(ChosenModule < 0)
				ChosenModule = 0
			endif							
			Pixie4_Set_Current_ModChan(ChosenChannel,ChosenModule)	
 
 			// Update Energy Filter time limits
 			
 			DoWindow/F Pixie_Parameter_Setup
 			if(V_Flag == 1)
				dt =2^Display_Module_Parameters[index_FilterRange]/FilterClockMHz
				SetVariable EGY_ENERGY_FLATTOP0, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP1, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP2, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_FLATTOP3, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME0, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME1, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME2, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
				SetVariable EGY_ENERGY_RISETIME3, limits={-inf,inf,dt}, win = Pixie_Parameter_Setup
			endif
			
			// Update DSP values
			Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
			root:pixie4:DSPValues=DSPValues
			
			// Close the StartUp panel
			DoWindow/K Pixie_Panel_StartUp
End

Function Pixie_EX_CloseHandle(CtrlName):ButtonControl	
String CtrlName
	// Special boot code to close handle of P4e devices
	// Seems to be required for LV to use the card after a Igor boot (and vice versa)

	// global variables and waves
	Nvar Pixie4Offline = root:pixie4:Pixie4Offline
	Wave/T System_Parameter_Names = root:pixie4:System_Parameter_Names	
	Wave System_Parameter_Values = root:pixie4:System_Parameter_Values
	
	// local variables
	Variable ret, direction, modnum, channum
	
	direction = 0	// download to XOP
	modnum = 0	// module number is not used for system parameter values
	channum = 0	// channel number is not used for system parameter values
		
	ret=Pixie4_Boot_System(0x20) 
	if(ret<0)  
		DoAlert 0, "Failed to Release Handle. This is pretty bad. A power cycle of the module is probably required"
	else
		DoAlert 0, "Handle to Pixie-4e module(s) was released. A reboot is required to continue using the Pixie Viewer with the module(s). Now switching to offline mode"
	endif
	
	Pixie4Offline = 1
	System_Parameter_Values[Pixie_Find_SysParInWave("OFFLINE_ANALYSIS")]=Pixie4Offline
	ret = Pixie4_User_Par_IO(System_Parameter_Values, "OFFLINE_ANALYSIS", "SYSTEM", direction, modnum, channum)
	if( ret < 0 )
		return(ret)  // download failure
	endif

	// Open the StartUp panel
	Execute "Pixie_Panel_StartUp()"
End

Function Pixie_EX_ResetPLL(CtrlName):ButtonControl	
String CtrlName

	Nvar ChosenModule = root:pixie4:ChosenModule
	Variable ret
	make/o/u/i/n=1 dummy
	//Wave dummy = dummy
	ret = Pixie4_Acquire_Data(0x40F7,dummy,"", ChosenModule)
	printf "CSR = 0x%x\r", ret
	
End

Function Pixie_EX_ReadRawLMdata(CtrlName):ButtonControl	
String CtrlName

	// load currently specified LM file as a wave LMdata0
	Svar longDataFilename = root:pixie4:longDataFilename
	GBLoadWave/B/O/A=LMData/T={80,4}/W=1 longDataFilename
	Wave LMData0
	
	Variable eventlength, numwords, numevents
	if(LMData0[2] == 0x400)
		eventlength = LMData0[8]	* LMData0[0]		// assuming all are the same as ch.0 in run type 0x400
		Pixie_EX_LMheadernames400()					// repopulate header names for runtype 0x400
	else
		eventlength = LMData0[6]	* LMData0[0]		// always all 4 channels are read
		Pixie_EX_LMheadernames402()					// repopulate header names for runtype 0x402
	endif
	
	// copy first 32 words into a header array
	make/o/n=32 LMheader
	Wave LMheader
	LMheader = LMdata0
	DoWindow/F FileHeaderTable
	if(V_flag!=1)
		PauseUpdate; Silent 1		// building window...
		edit/K=1/W=(5,30,220,400) root:pixie4:LMheadernames, LMheader
		DoWindow/C FileHeaderTable
		ModifyTable format(Point)=1,width(LMheader)=50
	endif
	DeletePoints 0,32, LMData0 					// remove header from data wave
	
	// sort LMData into a 2D array
	wavestats/q LMData0
	numwords = V_npnts
	numevents = numwords/eventlength +1	// just in case there is an end run record
	make/o/n=(eventlength,numevents) wfarray
	wfarray = LMData0	// this essentially sorts the 1D data into a 2D array, first index is # word in event, second index is event #
	make/t/o/n=32 root:pixie4:EVheadernames
	Wave/t EVheadernames =  root:pixie4:EVheadernames
	Wave/t LMheadernames =  root:pixie4:LMheadernames
	EVheadernames = LMheadernames[p+32]
	
	DoWindow/F EventArrayTable
	if(V_flag!=1)
		PauseUpdate; Silent 1		// building window...
		Edit/K=1/W=(230,30,840,400) root:pixie4:EVheadernames, wfarray
		DoWindow/C EventArrayTable
		ModifyTable format(Point)=1,width(wfarray)=50
	endif
	killwaves LMData0
	
End

Function Pixie_EX_SaveRawLMdata(CtrlName):ButtonControl	
String CtrlName

	Wave LMheader
	Wave wfarray
	
	Variable filelength, fnum
	wavestats/q wfarray
	filelength = V_npnts	// number of points in wfarray  
	make/o/n=(filelength) LMdata0
	LMdata0 = wfarray		// 2D into 1D wave
	insertpoints 0,32, LMdata0
	LMdata0[0,31] = LMheader
	
	open/M="Select new binary file nale" fnum 
	fbinwrite/F=2/U fnum, LMdata0
	close fnum
	

	DoWindow/K EventArrayTable
	Killwaves/Z LMdata0, wfarray, LMheader

End



//*********************************************************************************
// LM data file analysis functions
//*********************************************************************************

Function Pixie_EX_LMheadernames400()

	make/t/o/n=64 root:pixie4:LMheadernames
	wave/t LMheadernames = root:pixie4:LMheadernames
	LMheadernames[0] = "BlkSize"
	LMheadernames[1] = "ModNum "
	LMheadernames[2] = "RunFormat "
	LMheadernames[3] = "ChanHeadLen "
	LMheadernames[4] = "CoincPat "
	LMheadernames[5] = "CoincWin "
	LMheadernames[6] = "MaxCombEventLen"
	LMheadernames[7] = "Module Type "
	LMheadernames[8] = "EventLength0 "
	LMheadernames[9] = "EventLength1 "
	LMheadernames[10] = "EventLength2 "
	LMheadernames[11] = "EventLength3 "
	LMheadernames[12] = "Serial Number "
	
	LMheadernames[32] = "EvtPattern "
	LMheadernames[33] = "EvtInfo  "
	LMheadernames[34] = "NumTraceBlks "
	LMheadernames[35] = "NumTraceBlksPrev "
	LMheadernames[36] = "TrigTimeLO "
	LMheadernames[37] = "TrigTimeMI "
	LMheadernames[38] = "TrigTimeHI "
	LMheadernames[39] = "TrigTimeX "
	LMheadernames[40] = "Energy "
	LMheadernames[41] = "ChanNo "
	LMheadernames[42] = "User PSA Value (A)"
	LMheadernames[43] = "XIA PSA Value (CFD)"
	LMheadernames[44] = "Extended PSA Values0 (B)"
	LMheadernames[45] = "Extended PSA Values1 (Q0)"
	LMheadernames[46] = "Extended PSA Values2 (Q1)"
	LMheadernames[47] = "Extended PSA Values3 (R)"
	LMheadernames[48] = "8 lo"
	LMheadernames[49] = "8 hi"
	LMheadernames[50] = "9 lo"
	LMheadernames[51] = "9 hi"
	LMheadernames[52] = "10 lo"
	LMheadernames[53] = "10 hi"
	LMheadernames[54] = "11 lo"
	LMheadernames[55] = "11 hi"
	LMheadernames[56] = "12 lo"
	LMheadernames[57] = "12 hi"
	LMheadernames[58] = "13 lo"
	LMheadernames[59] = "13 hi"
	LMheadernames[60] = "checksum"
	LMheadernames[61] = "checksum"
	LMheadernames[62] = "watermark"
	LMheadernames[63] = "watermark"
End

Function Pixie_EX_LMheadernames402()

	make/t/o/n=64 root:pixie4:LMheadernames
	wave/t LMheadernames = root:pixie4:LMheadernames
	LMheadernames[0] = "BlkSize"
	LMheadernames[1] = "ModNum "
	LMheadernames[2] = "RunFormat "
	LMheadernames[3] = "ChanHeadLen "
	LMheadernames[4] = "CoincPat "
	LMheadernames[5] = "CoincWin "
	LMheadernames[6] = "MaxCombEventLen "
	LMheadernames[7] = "Module Type "
	LMheadernames[8] = "EventLength0 "
	LMheadernames[9] = "EventLength1 "
	LMheadernames[10] = "EventLength2 "
	LMheadernames[11] = "EventLength3 "
	LMheadernames[12] = "Serial Number "
	
	LMheadernames[32] = "EvtPattern "
	LMheadernames[33] = "EvtInfo  "
	LMheadernames[34] = "NumTraceBlks "
	LMheadernames[35] = "NumTraceBlksPrev "
	LMheadernames[36] = "TrigTimeHI "
	LMheadernames[37] = "TrigTimeX "
	LMheadernames[38] = "E sum "
	LMheadernames[39] = " "
	LMheadernames[40] = "TrigTimeLO_0 "
	LMheadernames[41] = "TrigTimeMI_0 "
	LMheadernames[42] = "Energy_0"
	LMheadernames[43] = "NumTraceBlks_0 "
	LMheadernames[44] = "TrigTimeLO_1"
	LMheadernames[45] = "TrigTimeMI_1"
	LMheadernames[46] = "Energy_1"
	LMheadernames[47] = "NumTraceBlks_1"
	LMheadernames[48] = "TrigTimeLO_2"
	LMheadernames[49] = "TrigTimeMI_2"
	LMheadernames[50] = "Energy_2"
	LMheadernames[51] = "NumTraceBlks_2"
	LMheadernames[52] = "TrigTimeLO_3"
	LMheadernames[53] = "TrigTimeMI_3"
	LMheadernames[54] = "Energy_3"
	LMheadernames[55] = "NumTraceBlks_3"
	LMheadernames[56] = "Evt info 0,1"
	LMheadernames[57] = "Evt info 2,3"
	LMheadernames[58] = "EventTimeLO"
	LMheadernames[59] = "EventTimeMI"
	LMheadernames[60] = "checksum"
	LMheadernames[61] = "checksum"
	LMheadernames[62] = "watermark"
	LMheadernames[63] = "watermark"
End


//******************************************************************************************************************************
//******************************************************************************************************************************
// check for updates
//******************************************************************************************************************************
//******************************************************************************************************************************
Function Pixie_CheckForPVupdates(ctrlName) : ButtonControl
	String ctrlName

	Nvar ViewerVersion = root:pixie4:ViewerVersion
	variable ret

	// connect to XIA support page
	String releasepage, searchstr, vn
	releasepage = fetchURL("http://support.xia.com/default.asp?W365")
	Variable error = GetRTError(1)		// Check for error before using response
	if (error != 0)
		// FetchURL produced an error
		// so don't try to use the response.
		if (cmpstr(ctrlname,"Noalert")==0)
			print "Could not connect to the XIA support web page."
		else
			DoAlert 0, "Could not connect to the XIA support web page."
		endif
		return(-1)
	endif
	
	// check response
	sprintf searchstr, "Pixie-4e_4.%02x_setup.exe", (ViewerVersion-0x400)	// release package file name (two version digits filled in)
	ret= strsearch(releasepage,searchstr,0)		// -1 if not found
	if(ret<0)
		// if not found, the current release is not on the webpage, so there must be a newer one (or, during development, an older one)
		if (cmpstr(ctrlname,"Noalert")==0)
			print "There is a new release on XIA's support web page http://support.xia.com/default.asp?W365"
		else
			DoAlert 1, "There is a new release on XIA's support web page. Open in web browser?"
			if(V_flag==1)
				BrowseURL "http://support.xia.com/default.asp?W365"
			endif
		endif
	else
		if (cmpstr(ctrlname,"Noalert")==0)
			print "This PixieViewer software is up to date"
		else
			DoAlert 0, "This PixieViewer software is up to date"
		endif
	endif
End




//########################################################################
//
//	Pixie_RC_DMACheckRun_DataPoll:
//		Check the run status in all modules and store data
// 		most of functionality in C library
//		only for Pixie-500e DMA runs.      
//		returns "new" data from DMA buffer
//
//########################################################################
Function Pixie_RC_DMACheckRun_DataPoll(s)		// This is the function that will be called periodically
	STRUCT WMBackgroundStruct &s

	// global variables and waves
	Nvar WhichRun = root:pixie4:WhichRun
	Nvar PollingTime = root:pixie4:PollingTime
	Nvar RunTimeOut = root:pixie4:RunTimeOut
	Nvar KeepRunTimeOut = root:pixie4:KeepRunTimeOut
	Nvar SpillTimeOut = root:pixie4:SpillTimeOut
	Nvar KeepSpillTimeOut = root:pixie4:KeepSpillTimeOut
	Nvar RepCount = root:pixie4:RepCount
	Nvar KeepRePCount = root:pixie4:KeepRePCount
	Nvar AutoUpdateMCA= root:pixie4:AutoUpdateMCA
	Nvar AutoUpdateRunStats = root:pixie4:AutoUpdateRunStats
	Nvar Tupdate = root:pixie4:Tupdate
	Nvar AutoNewFile = root:Pixie4:AutoNewFile
	Nvar TNewFile = root:pixie4:TNewFile
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar OutputFileName = root:pixie4:OutputFileName
	Wave RunTasks = root:pixie4:RunTasks
	Wave dummy = root:pixie4:dummy
		
	Svar ReqStopDate = root:pixie4:ReqStopDate	
	Svar ReqStopTime = root:pixie4:ReqStopTime
	Nvar StopDTreq = root:pixie4:StopDTreq
	Nvar  SpillCountNewFile =  root:pixie4:SpillCountNewFile
	Nvar TotalSpillCount = root:Pixie4:TotalSpillCount
	Nvar StartTime_s = root:pixie4:StartTime_s
	
	// local variables
	Variable RunType,ret,direction,channum,i,index0,index1, updated, saved, CSR, DataReady, stoppedrun, elapsed
		
	updated=0
	saved=0
	stoppedrun=0
	
	Variable Nof2MB = 524288*2			// allocate 2 buffer's worth, just in case. Max return should be 1x 2MB
	Make/o/u/i/n=(Nof2MB) userData		// 2MB buffer, 4bytes each for U32
	//Make/o/u/i/n=(2*Nof2MB) userData16
	//Make/o/u/i/n=(100000) userData16
	wave userData
	//wave userData16
	userData=0
	
	// poll module	
	RunType=RunTasks[WhichRun-1]+0x4000		// polling actions depend on runtype 
	CSR = Pixie4_Acquire_Data(RunType, userData, OutputFileName, NumberOfModules)	//poll and if DMA idle, save data
	// returns total number of spills saved, or negative number if something went wrong
	//print CSR

	if(CSR < 0)	// failed to poll
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		DoAlert 0, "Failed to check run status."		
		RunType=RunTasks[WhichRun-1]+0x3000  //  Stop  run and save list mode event data
		ret = Pixie4_Acquire_Data(RunType, dummy, OutputFileName, NumberOfModules)  // Stop run in all modules		
		if(ret < 0)  // failed to stop the run
			print "Failed to stop the run. Please reboot the Pixie modules."
		endif				
		Pixie_RC_RestoreRunTab()  // restore run tab settings
		return(CSR)
	endif
	
	RepCount = KeepRepCount - CSR	- TotalSpillCount // decrease spill count by C lib accumulated total
	SpillTimeOut=inf//KeepSpillTimeOut	// restore timeout for next spill
	// spill time out does not apply to DMA runs
	
	// decrease timers
	elapsed = DateTime - StartTime_s
	RunTimeOut = KeepRunTimeOut - elapsed
	//RunTimeOut -= PollingTime
	//SpillTimeOut -= PollingTime
	
	
	if((RepCount<=0) || (RunTimeOut<=0)  )	// TODO need to restart run after spill timeout
		//CtrlBackground Stop  // Stop the background first
		CtrlNamedBackground CheckRun kill
		Pixie_RC_StopRun("RunEndDAQ2a")  
	endif
	
//	Variable k
//		for(k=0;k<25000;k+=1)
//			userData16[2*k] = userData[k] & 0xFFFF
//			userData16[2*k+1] = floor(userData[k] / 65536)
//		endfor
	//	userdata=0
	
	//Variable/G fileposwords
//	Nvar fileposwords =fileposwords
//	PollDMAbuffer(fileposwords)
							 		
	return(0)		// IGOR 5 needs a return value for Background task to work
			
End