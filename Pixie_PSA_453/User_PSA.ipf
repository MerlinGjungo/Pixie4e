#pragma rtGlobals=1		// Use modern global access method.
#include ":Time_Analysis"

// This procedure file contains templates for user functions.
// The functions are called at certain points in the standard operation, e.g. 
// at the beginning or end of a run. Users can modify the functions to 
// execute custom routines at these points. 
// Output data is located in root:results 
// The variable root:user:UserVersion specifies the version of XIA's user calls in the main code, defined in main code
// The variable root:user:UserVariant specifies the user code variant (=application)
// The variable root:user:UserCode specifies the user code version (=updates, builds for a given application)

// ****************************************************************************************
// User Functions called by general code
// ****************************************************************************************


Function User_Globals()
	// called from Pixie_InitGlobals. Use to define and create global variables

	Variable/G root:user:UserVariant = 0x0400	// the user code variant (= application). variant numbers 0x0000-0x7FFF reserved for code written by XIA 
	// Variant 0x0000 = generic
	// Variant 0x0100 = Xe PW
	// Variant 0x0110 = Xe SAUNA
	// Variant 0x0200 = MPI
	// Variant 0x0300 = gamma neutron
	// Variant 0x0400 = general PSA
	
	Variable/G root:user:UserCode= 0x000E	// the user code version = updates and modifications of code for the same application
	String/G root:user:UserTitle = "User"	// This string may be used in a number of panels and displays. Replace for a particular application
	Variable/G  root:user:ProcessBeforeNewRun
	
	// add additional user global variables below
	NewDataFolder/O root:PW

	// constants
	Variable/G  root:PW:PWrecordlength =8 //= 8 // number of PSA return values per channel, incl. energy but not NchanDat 
	
	//  processing variables and options
	Variable/G root:PW:PWchannel = 3		// channel to be analyzed
	Variable/G root:PW:Nevents 		   	// number of events 
	Variable/G root:PW:oldNevents = 0	// remember previous number of events
	make/d/o/n=29 root:PW:Eventvalues
	Variable/G root:PW:source = 0
//	Variable/G root:PW:PSAoption = 0			// 0: Q0/Q1; 1: (Q0-Q1)/Q1
//	Variable/G root:PW:Q1startoption = 1	// relative to high or low threshold
//	Variable/G root:pixie4:ModCSRBbit03 = 0 			// divide result by 8
//	Variable/G root:pixie4:ModCSRBbit02 = 1 		// correct for arrival within the 4 samples per FPGA cycle
	Variable/G root:PW:PSAletrig =1			// use leading edge trigger
	Variable/G root:PW:PSAth = 40				// trigger threshold in % (CFD) or ADC steps (LE)
	Variable/G root:PW:CompDSP2Igor =0		// go into debug mode when reading/computing PSA values for comarison between DSP/FPGA and Igor
	Variable/G  root:PW:Allchannels =0		// loop over all channels and modules when processing file(s)
//	Variable/G root:PW:ALMproc3 =1			// variables for radio button to switch between AutoLMprocess 3 or 5
//	Variable/G root:PW:ALMproc5 =0
	Variable/G root:user:AWEpanels =0
	
	// Run statistics
	String/G root:PW:IFMStartTime
	String/G root:PW:IFMStopTime
	Make/o/n=18  root:PW:RT_tot, root:PW:Nev_tot
	Make/o/n=(18,4)  root:PW:LT_tot, root:PW:Ntrig_tot
	Make/o/n=18  root:PW:RT_ifm, root:PW:Nev_ifm
	Make/o/n=(18,4) root:PW:LT_ifm, root:PW:Ntrig_ifm
	
	//Igor "acquisition parameters" when computing PSA values from traces
	Variable/G root:PW:LoQ1 //= 12 // length of sum P and its baseline
	Variable/G root:PW:LoQ0 //= 12 // length of sum C and its baseline
	Variable/G root:PW:SoQ1 //= 0	// starting point of P relative to 10% or 90% level
	Variable/G root:PW:SoQ0 //= 24	// starting point of C relative to 10% level	
	Variable/G  root:PW:RTlow = 0.1
	Variable/G  root:PW:RThigh =0.9
	Variable/G  root:PW:RTrise = 1		// 1 = search rising edge from 0 to max. 0 = search falling edge from max to end
	Variable/G  root:PW:CFDoffset = 100
	
	// names for waves in configurable plots
	String/G  root:PW:destwavenamex = "energy"
	String/G  root:PW:destwavenamey = "PSAval"
	String/G  root:PW:destwavenamez = "Ratio0"
	String/G  root:PW:destwavenameN0 = "Q0sm"
	String/G  root:PW:destwavenameD0 = "energy"
	String/G  root:PW:destwavenameN1 = "Q1sm"
	String/G  root:PW:destwavenameD1 = "energy"
	
	PW_process_makethewaves(1)	// makes waves for the event result parameters
	
	// histograms
	Variable/G root:PW:nbins =256
	Nvar nbins = root:PW:nbins
	make/o/n=(nbins) root:PW:RT_histo			// waves for histograms
	make/o/n=(nbins) root:PW:Q0_histo
	make/o/n=(nbins) root:PW:Q1_histo
	make/o/n=(nbins) root:PW:Bsm_histo
	make/o/n=(nbins) root:PW:PSAval_histo
	make/o/n=(nbins) root:PW:Ratio0_histo
	make/o/n=(nbins) root:PW:Ratio1_histo
	make/o/n=(nbins) root:PW:amp_histo
	make/o/n=(nbins*nbins) x2D, y2D, z2D
	Variable/G root:PW:scalex = (nbins)
	Variable/G root:PW:scaley = (nbins)
	Variable/G root:PW:offx = 0
	Variable/G root:PW:offy = 0
	Variable/G root:PW:ngth = 1000
	make/o/n=(nbins) th2D
	wave th2D
	th2D = 0
	
	PSA2D_globals()			// initialize also the online 2D PSA functions
	
	Time_Globals()			// initializa also globals for timing

End

Function User_StartRun()
	//called at end of Pixie_StartRun (executed at beginning of run) for runs with polling time>0
	//Use to prepare runs	
	
	Nvar AWEpanels = root:user:AWEpanels 

	if(AWEpanels)
		ShowHideAWEcontrols()
	endif
	
End

Function User_NewFileDuringRun(Runtype)
	String Runtype	// "List" for list mode runs, "MCA" for MCA runs

	//called from Pixie_CheckRun (executed every polling period) in those occasions 
	//when Igor automatically saves files and increments run file number after N spills or seconds. 
	// called after data is saved to current files and new files have been made and run resumed
	// (current run number  = last saved run number + 1)
	
	//Use to process output data, modify parameters, or add comments in between auto saves
	// use if statements below to distinguish between run types
	
	// if(cmpstr(Runtype, "List")==0)
	//
	// endif
	
	// if(cmpstr(Runtype, "MCA")==0)
	//
	// endif
	
	// for example, uncomment below to save Igor
	//saveExperiment
	
	// to process output data
	Nvar MaxNumModules = root:pixie4:MaxNumModules
	Nvar PRESET_MAX_MODULES = root:pixie4:PRESET_MAX_MODULES	
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Svar OutBaseName = root:pixie4:OutBaseName
	Nvar RunNumber = root:pixie4:RunNumber
	Nvar ProcessBeforeNewRun = root:user:ProcessBeforeNewRun
	
	Variable ret, fn, num, m
	String filename, LMdatafilename, ErrMSG
	
	if(ProcessBeforeNewRun)
		// roll back current run number by one
		num = RunNumber-1
		
		for(m=0;m<NumberOfModules;m+=1)
			sprintf filename, "%s%04d.b%02d", OutBaseName, num,m		// for P500e, each module has a separate file
			Open/R/P=EventPath fn as filename
			PathInfo EventPath
		
			if(fn == 0)
				print "Error: can not open file %s\r", filename
				// sprintf ErrMSG, "Error: can not open file %s\r", filename		// can not use DoAlert during background task,user response too slow
				// DoAlert 0,ErrMSG
				 return(-1)
			else
				close fn
			endif
			LMdatafilename = S_Path + filename
			LMdatafilename = Pixie_FileNameConversion(LMdatafilename)	
		
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
			listmodewave[0] = 1		// indicates we are in a loop, one output file to which all modules are appended
			ret = Pixie4_Acquire_Data(0x7001, listmodewave, LMdatafilename, NumberOfModules)
			if(ret < 0)
				print "Can not parse list mode event file."
				//Doalert 0, "Can not parse list mode event file." // can not use DoAlert during background task,user response too slow
				return(ret)
			endif
		endfor
	endif

	
End

Function User_UpdateMCA()	
	//called from Pixie_UpdateMCAData, i.e. after updating spectra (and in most cases, run statistics)
	
	// in principle one could call the duplication routine to copy MCA data to the user copies in root:results
	// but this may take time and possibly claim more memory every time. Not recommended	
	//User_DuplicateResults()	
	
	PSA2D_UpdateMCAData("module")
	
	
End

Function User_StopRun()
	//called at end of Pixie_StopRun (executed at end of run)
	//Use to process output data

	User_DuplicateResults() //copy output data into root:results data folder
	
	// add custom code below
	
	// PSA2D
	// latest MCA has been loaded as part of updateMCA routine
	// here we save it to file
	Nvar AutoStoreSpec = root:pixie4:AutoStoreSpec
	if(AutoStoreSpec)
		PSA2D_buttons("savebin")
	endif
	
	
	Nvar AWEpanels = root:user:AWEpanels 

	if(AWEpanels)
		ShowHideAWEcontrols()
	endif

	
End


Function User_ChangeChannelModule()
	//called when changing Module Number or Channel Number

	User_GetDSPValues("")	//read data back from DSP of current module
	
	//add custom code below
	DoWindow/F PSA_2D
	if(V_flag==1)
		Nvar ChosenChannel = root:pixie4:ChosenChannel
		ModifyImage/W=PSA_2D psa2dz plane = ChosenChannel
		
		PSA2D_UpdateMCAData("") //update projections only
	endif
	
End

Function User_TraceDataFile()
	//called when changing file name in list mode trace display
	
	//add custom code below
	
End

Macro User_HomePaths()
	// called from macro UseHomePaths to set default path and filenames
	
	// customize code below
	
	// Communication FPGA files
// root:pixie4:FPGAFile4e = root:pixie4:FirmwareFileDirectory + "P4e_16_125_vdo.bin"
// root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "P4e_14_500_vdo.bin"
// root:pixie4:CommFPGAFile4 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
// root:pixie4:CommFPGAFile500 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
	
	// Signal processing FPGA files	
//	root:pixie4:SPFPGAFile4 = root:pixie4:FirmwareFileDirectory + "pixie.bin"
//	root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "FippiP500.bin"
//	root:pixie4:FPGAFile500e = root:pixie4:FirmwareFileDirectory + "p500e_zdt.bin"
	
	// DSP I/O variable values file
	root:pixie4:DSPParaValueFile = root:pixie4:ConfigFileDirectory + "EJ309_500.set"
	
	// DSP code files
	//root:pixie4:DSPCodeFile4 = root:pixie4:DSPFileDirectory + "PXIcode.bin"
	//root:pixie4:DSPCodeFile500 = root:pixie4:DSPFileDirectory + "P500code.bin"
	//root:pixie4:DSPCodeFile500e = root:pixie4:DSPFileDirectory + "P500e.ldr"     // for both P4e and P500e
		
	// DSP I/O variable names file
	//root:pixie4:DSPVarFile4 = root:pixie4:DSPFileDirectory + "P500e.var"
	
	// DSP all variable names file
	//root:pixie4:DSPListFile4 = root:pixie4:DSPFileDirectory + "PXIcode.lst"
	//root:pixie4:DSPListFile500e = root:pixie4:DSPFileDirectory + "P500e.lst"

	// set default values that are hidden, Clib or Igor only, and may interfere with acquisition unexpectedly
	root:pixie4:AutoProcessLMData = 3

	
End

// ****************************************************************************************
// User Control Panels 
// ****************************************************************************************


Window User_Control() : Panel
	PauseUpdate; Silent 1		// building window...
	// sample user control panel
	User_GetDSPValues("")		//update values
	
	DoWindow/F User_Control		// bring panel to front, if open
	if (V_flag!=1)					// only if not already open, make new panel
	
		Variable xout = 475
		Variable xopt = 250

		NewPanel /K=1/W=(400,10,850,420) 
		ModifyPanel cbRGB=(65280,59904,48896)
		DoWindow/C User_Control
		SetDrawLayer UserBack
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 5,25,"DSP Input Parameters"
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText xopt-4,26,"Options"
		SetDrawEnv fsize= 11,fstyle=0
		DrawText xopt-4,40,"Toggle Checkboxes to change CSRB's"
//		SetDrawEnv fsize= 14,fstyle= 1
//		DrawText xout-4,25,"DSP Output Values"
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 5,175,"PSA Input Values"
		

		
//		SetVariable ModCSRB,pos={7,32},size={120,16},title="ModCSRB",help={"Module CSRB - set to 1 to enable User code"}
//		SetVariable ModCSRB,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ModCSRB")], proc=User_VariableIO
//		SetVariable CCSRB0,pos={7,52},size={120,16},title="CCSRB0   ",help={"Channel 0 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB0,fSize=10,format="%X",value=root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB0")], proc=User_VariableIO
//		SetVariable CCSRB1,pos={7,72},size={120,16},title="CCSRB1   ",help={"Channel 1 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB1,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB1")], proc=User_VariableIO
//		SetVariable CCSRB2,pos={7,92},size={120,16},title="CCSRB2   ",help={"Channel 2 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB2,fSize=10,format="%X",value=root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB2")], proc=User_VariableIO
//		SetVariable CCSRB3,pos={7,112},size={120,16},title="CCSRB3   ",help={"Channel 3 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB3,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB3")], proc=User_VariableIO
      		
      // better: using module/channel parameters from C library
		Variable ncp =  root:pixie4:NumChannelPar
		SetVariable ADV_ModCSRB,pos={7,32},size={120,16},title="ModCSRB",help={"Module CSRB - set bit 0 to enable User code"},font=Arial
		SetVariable ADV_ModCSRB,fSize=12,format="%X",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_MCSRB], proc=Pixie_IO_ModVarControl
		SetVariable ADV_CHANNEL_CSRB0,pos={7,52},size={120,16},title="CCSRB0   ",help={"Channel 0 CSRB - set bit 0 to enable User code"},font=Arial
		SetVariable ADV_CHANNEL_CSRB0,fSize=12,format="%X",value=root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+0*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB1,pos={7,72},size={120,16},title="CCSRB1   ",help={"Channel 1 CSRB - set bit 0 to enable User code"},font=Arial
		SetVariable ADV_CHANNEL_CSRB1,fSize=12,format="%X",value= root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+1*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB2,pos={7,92},size={120,16},title="CCSRB2   ",help={"Channel 2 CSRB - set bit 0 to enable User code"},fsize=10,font=Arial
		SetVariable ADV_CHANNEL_CSRB2,fSize=12,format="%X",value=root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+2*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB3,pos={7,112},size={120,16},title="CCSRB3   ",help={"Channel 3 CSRB - set bit 0 to enable User code"},font=Arial
		SetVariable ADV_CHANNEL_CSRB3,fSize=12,format="%x",value= root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+3*ncp], proc=Pixie_IO_ChanVarControl

		Variable wid = 140
		SetDrawEnv fsize= 12,fstyle= 2
		DrawText 7,195,"Channel                 0                1                2                3"
		SetVariable USX_QDC0_LENGTH0, pos={7,200}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0LEN+0*ncp]
		SetVariable USX_QDC0_LENGTH0 ,title="QDC0 Length   ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_LENGTH0, pos={7,218}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1LEN+0*ncp]
		SetVariable USX_QDC1_LENGTH0 ,title="QDC1 Length   ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC0_DELAY0, pos={7,236}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0DEL+0*ncp]
		SetVariable USX_QDC0_DELAY0 ,title="QDC0 Delay     ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_DELAY0, pos={7,254}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1DEL+0*ncp]
		SetVariable USX_QDC1_DELAY0 ,title="QDC1 Delay     ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable WAX_CFD_THRESHOLD0,pos={7,272}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CFD_TH+0*ncp]
		SetVariable WAX_CFD_THRESHOLD0 ,title="PSA Threshold", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		
		SetVariable USX_CH_EXTRA_IN0,pos={7,327}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+0*ncp]
		SetVariable USX_CH_EXTRA_IN0 ,title="CFD Delay         ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable UXX_CH_EXTRA_IN0,pos={7,345}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+0*ncp+1]
		SetVariable UXX_CH_EXTRA_IN0 ,title="CFD Scale         ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_FCFD_THRESHOLD0, pos={7,363}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_FCFD_TH+0*ncp]
		SetVariable USX_FCFD_THRESHOLD0 ,title="CFD Threshold ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12

		
		wid = 45
		Variable xi = 155
		SetVariable USX_QDC0_LENGTH1, pos={xi,200}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0LEN+1*ncp]
		SetVariable USX_QDC0_LENGTH1, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_LENGTH1, pos={xi,218}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1LEN+1*ncp]
		SetVariable USX_QDC1_LENGTH1, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC0_DELAY1, pos={xi,236}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0DEL+1*ncp]
		SetVariable USX_QDC0_DELAY1, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_DELAY1, pos={xi,254}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1DEL+1*ncp]
		SetVariable USX_QDC1_DELAY1, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable WAX_CFD_THRESHOLD1,pos={xi,272}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CFD_TH+1*ncp]
		SetVariable WAX_CFD_THRESHOLD1 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		
		SetVariable USX_CH_EXTRA_IN1,pos={xi,327}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+1*ncp]
		SetVariable USX_CH_EXTRA_IN1 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable UXX_CH_EXTRA_IN1,pos={xi,345}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+1*ncp+1]
		SetVariable UXX_CH_EXTRA_IN1 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_FCFD_THRESHOLD1, pos={xi,363}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_FCFD_TH+1*ncp]
		SetVariable USX_FCFD_THRESHOLD1 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12


		xi = 205
		SetVariable USX_QDC0_LENGTH2, pos={xi,200}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0LEN+2*ncp]
		SetVariable USX_QDC0_LENGTH2, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_LENGTH2, pos={xi,218}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1LEN+2*ncp]
		SetVariable USX_QDC1_LENGTH2, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC0_DELAY2, pos={xi,236}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0DEL+2*ncp]
		SetVariable USX_QDC0_DELAY2, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_DELAY2, pos={xi,254}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1DEL+2*ncp]
		SetVariable USX_QDC1_DELAY2, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable WAX_CFD_THRESHOLD2,pos={xi,272}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CFD_TH+2*ncp]
		SetVariable WAX_CFD_THRESHOLD2 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		
		SetVariable USX_CH_EXTRA_IN2,pos={xi,327}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+2*ncp]
		SetVariable USX_CH_EXTRA_IN2 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable UXX_CH_EXTRA_IN2,pos={xi,345}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+2*ncp+1]
		SetVariable UXX_CH_EXTRA_IN2 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_FCFD_THRESHOLD2, pos={xi,363}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_FCFD_TH+2*ncp]
		SetVariable USX_FCFD_THRESHOLD2 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12

		xi = 260
		SetVariable USX_QDC0_LENGTH3, pos={xi,200}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0LEN+3*ncp]
		SetVariable USX_QDC0_LENGTH3, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_LENGTH3, pos={xi,218}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1LEN+3*ncp]
		SetVariable USX_QDC1_LENGTH3, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC0_DELAY3, pos={xi,236}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q0DEL+3*ncp]
		SetVariable USX_QDC0_DELAY3, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_QDC1_DELAY3, pos={xi,254}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_Q1DEL+3*ncp]
		SetVariable USX_QDC1_DELAY3, title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable WAX_CFD_THRESHOLD3,pos={xi,272}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CFD_TH+3*ncp]
		SetVariable WAX_CFD_THRESHOLD3 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		
		SetVariable USX_CH_EXTRA_IN3,pos={xi,327}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+3*ncp]
		SetVariable USX_CH_EXTRA_IN3 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable UXX_CH_EXTRA_IN3,pos={xi,345}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CH_EXTRA_IN+3*ncp+1]
		SetVariable UXX_CH_EXTRA_IN3 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		SetVariable USX_FCFD_THRESHOLD3, pos={xi,363}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_FCFD_TH+3*ncp]
		SetVariable USX_FCFD_THRESHOLD3 ,title=" ", proc = Pixie_IO_ChanVarControl,size={wid,16},font=Arial,fsize=12
		
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 5,321,"Slow CFD Input Values"


		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,215,"in ADC samples"
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,233,"in ADC samples"
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,251,"in ADC samples"
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,269,"in ADC samples"
		
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,342,"in ADC samples"
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 320,360,"1-N/8 (e.g. 3 = 62.5%)"




		// change name and appareance, but not the index for DSPValues
//		SetVariable UserIn0,pos={7,182},size={80,16},title="LoQ0 ",help={"User Input 0"}
//		SetVariable UserIn0,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")], proc=User_VariableIO
//		SetVariable UserIn1,pos={7,200},size={80,16},title="LoQ1 ",help={"User Input 1"}
//		SetVariable UserIn1,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+1], proc=User_VariableIO
//		SetVariable UserIn2,pos={7,218},size={80,16},title="SoQ1 ",help={"User Input 2"}
//		SetVariable UserIn2,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+2], proc=User_VariableIO
//		SetVariable UserIn3,pos={7,236},size={80,16},title="SoQ0 ",help={"User Input 3"}
//		SetVariable UserIn3,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+3], proc=User_VariableIO
//		SetVariable UserIn4,pos={10,254},size={140,16},title="Threshold               ",help={"User Input 4"}
//		SetVariable UserIn4,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+4], proc=User_VariableIO
//		SetVariable UserIn5,pos={10,272},size={140,16},title="Threshold               ",help={"User Input 5"}
//		SetVariable UserIn5,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+5], proc=User_VariableIO
		//etc, max 15 inputs 

            // better: using module parameters from C library
				// change name and appareance, but not the index for DSPValues
//		SetVariable UserIn00,pos={10,142},size={120,16},title="User Input 0 ",help={"User Input 0"}
//		SetVariable UserIn00,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn01,pos={10,160},size={120,16},title="User Input 1 ",help={"User Input 1"}
//		SetVariable UserIn01,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+1], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn02,pos={10,182},size={120,16},title="User Input 2 ",help={"User Input 2"}
//		SetVariable UserIn02,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+2], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn03,pos={10,200},size={120,16},title="User Input 3 ",help={"User Input 3"}
//		SetVariable UserIn03,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+3], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn04,pos={10,222},size={120,16},title="User Input 4 ",help={"User Input 4"}
//		SetVariable UserIn04,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+4], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn05,pos={10,240},size={120,16},title="User Input 5 ",help={"User Input 5"}
//		SetVariable UserIn05,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+5], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn06,pos={10,262},size={120,16},title="User Input 6 ",help={"User Input 6"}
//		SetVariable UserIn06,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+6], proc=Pixie_IO_ModVarControl
//		SetVariable UserIn07,pos={10,280},size={120,16},title="User Input 7 ",help={"User Input 7"}
//		SetVariable UserIn07,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+7], proc=Pixie_IO_ModVarControl
		//etc, max 15 inputs UserIn08 .. UserIn15
		
//		ValDisplay UserOut0,pos={xout,32},size={100,16},  title="Code Version ",help={"User Output 0"}
//		ValDisplay UserOut0,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")]
//		ValDisplay UserOut1,pos={xout,52},size={85,16},  title="PHend  ",help={"User Output 1"}
//		ValDisplay UserOut1,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+1]
//		ValDisplay UserOut2,pos={xout,72},size={85,16},  title="PHpeak",help={"User Output 2"}
//		ValDisplay UserOut2,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+2]
//		ValDisplay UserOut3,pos={xout,92},size={85,16},  title="Bsum0  ",help={"User Output 3"}
//		ValDisplay UserOut3,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+3]
//		ValDisplay UserOut4,pos={xout,112},size={85,16},title="Bsum1  ",help={"User Output 4"}
//		ValDisplay UserOut4,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+4]
//		ValDisplay UserOut5,pos={xout,132},size={85,16},title="Apeak   ",help={"User Output 5"}
//		ValDisplay UserOut5,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+5]	
//		ValDisplay UserOut6,pos={xout,152},size={85,16},title="10%Thr ",help={"User Output 6"}
//		ValDisplay UserOut6,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+6]	
//		ValDisplay UserOut7,pos={xout,172},size={85,16},title="90%Thr ",help={"User Output 7"}
//		ValDisplay UserOut7,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+7]	
//		ValDisplay UserOut8,pos={xout,192},size={85,16},title="RT        ",help={"User Output 8"}
//		ValDisplay UserOut8,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+8]	
//		ValDisplay UserOut9,pos={xout,212},size={85,16},title="Energy  ",help={"User Output 9"}
//		ValDisplay UserOut9,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+9]	
//		ValDisplay UserOut10,pos={xout,232},size={85,16},title="QDC0    ",help={"User Output 10"}
//		ValDisplay UserOut10,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+10]	
//		ValDisplay UserOut11,pos={xout,252},size={85,16},title="QDC1    ",help={"User Output 11"}
//		ValDisplay UserOut11,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+11]	
//		ValDisplay UserOut12,pos={xout,272},size={85,16},title="Q1/Q0      ",help={"User Output 12"}
//		ValDisplay UserOut12,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+12]
//		ValDisplay UserOut13,pos={xout,292},size={85,16},title="(Q1-Q0)/Q0 ",help={"User Output 13"}
//		ValDisplay UserOut13,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+13]			
		//etc, max 15 outputs
      		// better: using module parameters from C library
//		ValDisplay UserOut0,pos={405,142},size={120,16},title="UserOut[0] ",help={"User Output 0"}
//		ValDisplay UserOut0,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT]
//		ValDisplay UserOut1,pos={405,162},size={120,16},title="UserOut[1] ",help={"User Output 1"}
//		ValDisplay UserOut1,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+1]
//		ValDisplay UserOut2,pos={405,182},size={120,16},title="UserOut[2] ",help={"User Output 2"}
//		ValDisplay UserOut2,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+2]
//		ValDisplay UserOut3,pos={405,202},size={120,16},title="UserOut[3] ",help={"User Output 3"}
//		ValDisplay UserOut3,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+3]	
		//etc, max 15 outputs
		
		Checkbox EnaUserDSP, pos = {xopt,44}, size={60,20},proc=User_CheckBoxControl,title=" Enable PSA & CFD DSP code"
		Checkbox EnaUserDSP, fsize=12,font="Arial",help={"Bit 0 must be set to enable the special functions"}, variable= root:pixie4:ModCSRBbit00
		Checkbox PSAdiv8, pos = {xopt,62}, size={60,20},proc=User_CheckBoxControl,title=" Divide result by 8 (for long sums)"
		Checkbox PSAdiv8, fsize=12,font="Arial",help={"Long sums may become > 64K, so divide to fit into 16bit number"}, variable= root:pixie4:ModCSRBbit03
		Checkbox Correct4x, pos = {xopt,80}, size={60,20},proc=User_CheckBoxControl,title=" Correct within 4-sample block"
		Checkbox Correct4x, fsize=12,font="Arial",help={"Without this option, PSA sum location is only precise to 4 samples"}, variable= root:pixie4:ModCSRBbit02
		Checkbox SlowCFD, pos = {xopt,98}, size={60,20},proc=User_CheckBoxControl,title=" Use Slow CFD logic (125 MHz only)"
		Checkbox SlowCFD, fsize=12,font="Arial",help={"Use an alternative CFD algoriths that accommodates slow rising pulses. Not implemented for 500 MHz Pixie-4e"}, variable= root:pixie4:ModCSRBbit10


		
		Button RepeatDMA,pos={xopt,120},size={160,20},proc=Pixie_RepeatDMA,title="Start Fast Multi-File Run",fsize=11
		Button RepeatDMA,  help={"Fastest polling, no MCA or Run Statistics updates"}
		SetDrawEnv fsize= 12,fstyle= 0
		DrawText xopt+10,142+14,"Press ESC to stop"
		
		Variable ANFy = 160
	//	CheckBox AutoNewFile,pos={xopt,ANFy},size={60,20},title="New files every"
	//	CheckBox AutoNewFile,variable=root:pixie4:AutoNewFile,fsize=11
		SetVariable TNewFile,pos={xopt+10,ANFy-3},size={130,18},title="New files every "
		SetVariable TNewFile,limits={10,inf,1},value= root:pixie4:TNewFile
		SetVariable TNewFile,font=Arial,fsize=12
		SetDrawEnv fsize= 12,fstyle= 0
		DrawText xopt+145,ANFy+14,"spills"
		
//		Checkbox EnableUserCode, pos = {207,32}, size={60,20},proc=User_CheckBoxControl,title="Enable User DSP code"
//		Checkbox EnableUserCode, fsize=11,font="Arial",help={"Sets bits in Module CSRB and Channel CSRB to switch on user code in DSP"}
		
	// 	no longer needed when using Module Parameterd from C library		
//		Button Apply,pos={20,315},size={90,25},proc=Pixie_EX_CallPutModPar,title="Apply to DSP",fsize=11
//		Button Refresh,pos={130,315},size={90,25},proc=User_GetDSPValues,title="Read from DSP",fsize=11
		Button About,pos={340,372},size={90,25},proc=User_Version,title="Version",fsize=11
	endif
EndMacro

// ****************************************************************************************
// User Control Panel Functions 
// ****************************************************************************************

Window User_Version(ctrlName) : ButtonControl
	String ctrlName

	DoWindow/F User_Version		// bring panel to front, if open
	if (V_flag!=1)					// only if not already open, make new panel
		//NewPanel /K=1/W=(446,197,1103,556) 
		NewPanel /K=1/W=(20,20,250,150) as "User Version"
		ModifyPanel cbRGB=(65285,59904,48896)
		DoWindow/C User_Version
		
		ValDisplay UserV0,pos={10,20},size={140,16},title="Igor User Calls:     0x",help={"Version of Igor function calls from main code to user code"}
		ValDisplay UserV0,fSize=10,format="%X",value= root:user:UserVersion
		ValDisplay UserV1,pos={10,45},size={140,16},title="Igor User Variant: 0x",help={"Variant of Igor user functions (application)"}
		ValDisplay UserV1,fSize=10,format="%X",value= root:user:UserVariant 
		ValDisplay UserV2,pos={10,95},size={140,16},title="DSP User Code:   0x",help={"Version of user DSP code"}
		ValDisplay UserV2,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")]
		ValDisplay UserV3,pos={10,70},size={140,16},title="Igor User Code:    0x",help={"Version of Igor user functions (build number)"}
		ValDisplay UserV3,fSize=10,format="%X",value= root:user:UserCode
	endif
End

Function User_GetDSPValues(ctrlName) : ButtonControl
	String ctrlName

	Wave/T DSPNames = root:pixie4:DSPNames
	Wave DSPValues = root:pixie4:DSPValues
	Nvar ChosenModule = root:pixie4:ChosenModule
	
	Pixie4_Buffer_IO(DSPValues, 0, 1,"", ChosenModule)
	root:pixie4:DSPValues=DSPValues
	
	//update controls
	DoWindow/F User_Control		// bring panel to front, if open
	if (V_flag==1)		
		//only checkboxes need explicit updating		
	endif

End


Function User_CheckBoxControl(ctrlName, checked) : CheckBoxControl
	String ctrlName
	Variable checked
	
	Wave DSPvalues = root:pixie4:DSPvalues
	Variable index
	Variable bitmod, bit, MCSRB
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar index_MCSRB=root:pixie4:index_MCSRB
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Wave Module_Parameter_Values = root:pixie4:Module_Parameter_Values
	
	StrSwitch(ctrlName)
//		Case "EnableUserCode":	
//			bitmod = 0
//			if (checked)
//				index = Pixie_Find_DSPname("MODCSRB")
//				DSPValues[index] = setbit(0,DSPValues[index])
//			else
//				index = Pixie_Find_DSPname("MODCSRB")
//				DSPValues[index] = clrbit(0,DSPValues[index])
//			endif
//			break	
		
//	Case "PSAoption":	
//		bitmod =3
//		break	


		Case "EnaUserDSP":	
			bitmod = 16
			bit = 0
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
			
		Case "Correct4x":	
			bitmod = 16
			bit = 2
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

	
		Case "PSAdiv8":	
			bitmod = 16
			bit = 3
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
			
		Case "SlowCFD":	
			bitmod = 16
			bit = 10
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
			
			// here, also  copy to channel CSRA
			
			Variable CCSRA, ch
			Wave Channel_Parameter_Values = root:pixie4:Channel_Parameter_Values
			Nvar NumberOfChannels = root:pixie4:NumberOfChannels
			for(ch=0;ch<NumberOfChannels;ch+=1)
				CCSRA = Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRA")][ch][ChosenModule]
				if(checked)
					CCSRA = SETbit(bit, CCSRA)
				else
					CCSRA = CLRbit(bit, CCSRA)
				endif		
				Channel_Parameter_Values[Pixie_Find_ChanParInWave("CHANNEL_CSRA")][ch][ChosenModule]=CCSRA
				Pixie4_User_Par_IO(Channel_Parameter_Values, "CHANNEL_CSRA", "CHANNEL", 0, ChosenModule, ch)

			endfor
			// update display variables with values changed in C library 
			Pixie_IO_ReadChanParFromMod(ChosenModule)
			Pixie_IO_SendChanParToIgor()
			
			//some changes module parameters, so update 
			Pixie_IO_ReadModParFromMod(ChosenModule)
			Pixie_IO_SendModParToIgor()
			

			break	

					
		Default:
					
	EndSwitch
	
	if(bitmod < 16)
		if (checked)
			index = Pixie_Find_DSPname("CHANCSRB0")
			DSPValues[index] = setbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB1")
			DSPValues[index] = setbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB2")
			DSPValues[index] = setbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB3")
			DSPValues[index] = setbit(bitmod,DSPValues[index])
		else
			index = Pixie_Find_DSPname("CHANCSRB0")
			DSPValues[index] = clrbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB1")
			DSPValues[index] = clrbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB2")
			DSPValues[index] = clrbit(bitmod,DSPValues[index])
			index = Pixie_Find_DSPname("CHANCSRB3")
			DSPValues[index] = clrbit(bitmod,DSPValues[index])
		endif
		
		User_VariableIO("",0,"","") 
	endif
		
	
	

End


Function User_VariableIO(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum
	//after changing parameters, call this function to download to DSP
 	 
	Pixie_EX_PutModPar()
End



//**************************************************************************************************
// Other subroutines
//**************************************************************************************************

Function User_DuplicateResults()
	// duplicate selected run output data to variables and waves in the "results" data folder
	
	//statistics
	
	if(0)	// only execute when necessary	
		Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
		Nvar index_EvRate = root:pixie4:index_EvRate
		Nvar index_RunTime = root:pixie4:index_RunTime
		Nvar index_NumEv = root:pixie4:index_NumEv
		Svar StartTime = root:pixie4:StartTime
		Svar StopTime = root:pixie4:StopTime
		
		Nvar RunTimeU = root:results:RunTime
		Nvar EventRateU = root:results:EventRate
		Nvar NumEventsU = root:results:NumEvents		
		Svar StartTimeU = root:results:StartTime
		Svar StopTimeU = root:results:StopTime
		
		RunTimeU = Display_Module_Parameters[index_RunTime]
		EventRateU = Display_Module_Parameters[index_EvRate]
		NumEventsU = Display_Module_Parameters[index_NumEv]		
		StartTimeU = StartTime
		StopTimeU = StopTime
		
		Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
		Nvar index_COUNTTIME = root:pixie4:index_COUNTTIME
		Nvar index_ICR = root:pixie4:index_ICR
		Nvar ncp = root:pixie4:NumChannelPar
		Wave ChannelCountTime = root:results:ChannelCountTime
		Wave ChannelInputCountRate =  root:results:ChannelInputCountRate
		ChannelCountTime[0] = Display_Channel_Parameters[index_COUNTTIME+0*ncp]
		ChannelCountTime[1] = Display_Channel_Parameters[index_COUNTTIME+1*ncp]
		ChannelCountTime[2] = Display_Channel_Parameters[index_COUNTTIME+2*ncp]
		ChannelCountTime[3] = Display_Channel_Parameters[index_COUNTTIME+3*ncp]

		ChannelInputCountRate[0] = Display_Channel_Parameters[index_ICR+0*ncp]
		ChannelInputCountRate[1] = Display_Channel_Parameters[index_ICR+1*ncp]
		ChannelInputCountRate[2] = Display_Channel_Parameters[index_ICR+2*ncp]
		ChannelInputCountRate[3] = Display_Channel_Parameters[index_ICR+3*ncp]
		
		
		//MCAs
		Wave MCAch0 =  root:pixie4:MCAch0
		Duplicate/o MCAch0, root:results:MCAch0
		Wave MCAch1 =  root:pixie4:MCAch1
		Duplicate/o MCAch1, root:results:MCAch1
		Wave MCAch2 =  root:pixie4:MCAch2
		Duplicate/o MCAch2, root:results:MCAch2
		Wave MCAch2 =  root:pixie4:MCAch2
		Duplicate/o MCAch2, root:results:MCAch2
		Wave MCAsum =  root:pixie4:MCAsum
		Duplicate/o MCAsum, root:results:MCAsum
		
		
		//traces and list mode data
		Wave trace0 =  root:pixie4:trace0
		Duplicate/o trace0, root:results:trace0
		Wave trace1 =  root:pixie4:trace1
		Duplicate/o trace1, root:results:trace1
		Wave trace2 =  root:pixie4:trace2
		Duplicate/o trace2, root:results:trace2
		Wave trace3 =  root:pixie4:trace3
		Duplicate/o trace3, root:results:trace3
		Wave eventposlen =  root:pixie4:eventposlen
		Duplicate/o eventposlen, root:results:eventposlen
		Wave eventwave =  root:pixie4:eventwave
		Duplicate/o eventwave, root:results:eventwave
		
		//make/o/u/i/n=(len*2*NumberOfChannels) root:pixie4:EventPSAValues 
	endif
End		

Function User_ReadEvent()
	//called when changing event number in list mode trace display or digital filter display
	
	
	if(1)	// only execute when necessary	
		//duplicate traces and list mode data
		Wave trace0 =  root:pixie4:trace0
		Duplicate/o trace0, root:results:trace0
		Wave trace1 =  root:pixie4:trace1
		Duplicate/o trace1, root:results:trace1
		Wave trace2 =  root:pixie4:trace2
		Duplicate/o trace2, root:results:trace2
		Wave trace3 =  root:pixie4:trace3
		Duplicate/o trace3, root:results:trace3
		Wave eventposlen =  root:pixie4:eventposlen
		Duplicate/o eventposlen, root:results:eventposlen
		Wave eventwave =  root:pixie4:eventwave
		Duplicate/o eventwave, root:results:eventwave
	endif
	
	//add custom code below
	
	Nvar PWchannel = root:PW:PWchannel
	Nvar ChosenEvent = root:pixie4:ChosenEvent	
	wave dummy = root:pixie4:dummy
	Wave Eventvalues = root:PW:Eventvalues		// wave of return values for this event
	Eventvalues =0
	// 0 energy (DSP)				
	// 1 rise time or CFD time (DSP/FPGA)		UserRetVal
	// 2 amplitude (DSP/FPGA)				UserRetVal +1
	// 3 baseline (DSP/FPGA)					UserRetVal +2
	// 4 Q1 sum (DSP/FPGA)					UserRetVal +4
	// 5 Q0 sum (DSP/FPGA)					UserRetVal +3
	// 6 PSA value (DSP)						UserRetVal +5
	// 7 timestamp (FPGA)
	
	// 10 timestamp refined by CFD time  (DSP/FPGA/Igor)
	// 11 timestamp refined by CFD time  (Igor)
	// 12 CFD time fraction (DSP/FPGA)
	// 13 CFD time fraction (Igor)

	// 15 rise time (Igor)
	// 16 Q1 sum (Igor)
	// 17 Q0 sum (Igor
	// 18 amplitude (Igor)
	// 19 ratio Q1/Q0 (Igor
	// 20 base (Igor)
	
	// 21-28 debug
	
	
	//  Mode 0: Extract DSP values for "Event Detail"
	Eventvalues[0] = eventwave[7+6+3+PWchannel*9+2]	//energy  from DSP
	Eventvalues[1] = PW_restoresign(eventwave[7+6+3+PWchannel*9+3])	//rise time or CFD from DSP  			UserRetVal
	Eventvalues[2] = PW_restoresign(eventwave[7+6+3+PWchannel*9+4])	// amplitude from DSP       	UserRetVal +1
	Eventvalues[3] = PW_restoresign(eventwave[7+6+3+PWchannel*9+5])	// baseline sum from DSP  	UserRetVal +2
	Eventvalues[5] = PW_restoresign(eventwave[7+6+3+PWchannel*9+6])	// Q0 sum from DSP			UserRetVal +3
	Eventvalues[4] = PW_restoresign(eventwave[7+6+3+PWchannel*9+7])	// Q1 sum from DSP			UserRetVal +4
	Eventvalues[6] = eventwave[7+6+3+PWchannel*9+8]					// PSA value from DSP		UserRetVal +5
	Eventvalues[7] = eventwave[7+6+3+PWchannel*9+1]					// time stamp
		
	Variable ch
	// in plain P500, first 4 samples may hold FPGA values. 
	// here, traceN is unsigned, nan = 5e9
//	for(ch=0;ch<4;ch+=1)
//		Wave trace = $("root:pixie4:trace"+num2str(ch))
//		wavestats/q dummy	// to set V_npnts to 1
//		wavestats/q/z	trace
//		if(V_npnts>1)
//			trace[0] = nan				// clear trace values for display
//			trace[1] = nan
//			trace[2] = nan
//			trace[3] = nan
//		endif
//	endfor
					
	//  Mode 1: Compute Igor PSA values for "event Detail" and/or processing
	Wave trace = $("root:pixie4:trace"+num2str(PWchannel))
	Nvar LoQ1 = root:PW:LoQ1 // = 12 // length of sum Q1 and its baseline
	Nvar LoQ0 = root:PW:LoQ0 // = 12 // length of sum Q0	 and its baseline
	Nvar SoQ1 = root:PW:SoQ1 // = 0	// starting point of Q1 relative to high or low RT level
	Nvar SoQ0 =root:PW:SoQ0 //= 24	// starting point of Q0 relative to low RT level
	Nvar RTlow = root:PW:RTlow //= 0.1
	Nvar RThigh = root:PW:RThigh //=0.9	
	Nvar RTrise = root:PW:RTrise
	Nvar PSAoption = root:PW:PSAoption			// 0: Q1/Q0; 1: (Q1-Q0)/Q0
	Nvar Q1startoption = root:PW:Q1startoption			// relative to high or low threshold
	Nvar PSAdiv8 = root:PW:PSAdiv8  			// divide result by 8
	Nvar PSAletrig = root:PW:PSAletrig			// use leading edge trigger
	Nvar PSAth = root:PW:PSAth 		// trigger threshold in % (CFD) or ADC steps (LE)
	Nvar CFDoffset = root:PW:CFDoffset
//	Nvar ADCClockMHz = root:pixie4:ADCClockMHz
	Nvar wftimescale = root:pixie4:wftimescale			// sample clock in MHz as read from the file

	Variable dbgbase = 	1	// debug: set base to zero
		


	
	wavestats/q dummy	// to set V_npnts to 1
	wavestats/q/z	trace
	
	if(V_npnts>1)
	
		Variable k,j, base, ampl, Q1sum, Q0sum, RT, lev10, lev90	
		Variable normQ0, normQ1, Q1start, Q0start
		Variable LEthreshold = PSAth*1.27*4	// threshold for leading edge (not CFD) trigger, matching DSP/FPGA scaling
	
		
		if(PSAdiv8)
			normQ0 = 32//2^( floor(log(LoQ0)/log(2) )) 	// find closest power of 2 for normalization, matching DSP
			normQ1 = 32//2^( floor(log(LoQ1)/log(2) ))		
		else
			normQ0 = 4//2^( floor(log(LoQ0)/log(2) )) 
			normQ1 = 4//2^( floor(log(LoQ1)/log(2) ))		
		endif
		// capture FPGA values embedded in trace
		Eventvalues[22] =22
		Eventvalues[23] =23
		Eventvalues[24] =24
		Eventvalues[25] =25
		Eventvalues[26] =26
		Eventvalues[27] =27
		Eventvalues[28] =28
		
			

		
		// ***************  calculate base and amplitude  ***************
		// baseline
		base = 0
		for(j=4;j<4+8;j+=1)
			base+=trace[j]
		endfor
		base=base/8				// in single sample units
		Eventvalues[20] =base	
						
		//find max
		wavestats/q	trace
		ampl = V_max-base* dbgbase	// optionally set to zero for debug purposes
		Eventvalues[18] =ampl
		
		// debug: add base again to DSP/FPGA Qi sums
		//Eventvalues[25] =  Eventvalues[5]+LoQ0*Eventvalues[3]/normQ0
		//Eventvalues[26] =  Eventvalues[4]+LoQ1*Eventvalues[3]/normQ1
	

			
		// ***************  calculate RiseTime  ***************
		// find 10% level before max, searching back from maximum
		Variable endpoint
		if(RTrise==1)
			endpoint=0
		else
			endpoint = 1000000
		endif
		findlevel/q/R=(V_maxloc,endpoint) trace, (base+ampl*RTlow)
		lev10 = V_levelX	//in x units
	
		// find 90% level before max, searching back from maximum
		findlevel/q/R=(V_maxloc,endpoint) trace, (base+ampl*RThigh) 
		lev90 = V_levelX	//in x units
		// compute rise time
		RT = abs(lev90-lev10) //in s
		RT = RT/wftimescale *16 	// in 1/16 clock cycles
		
		Eventvalues[15] = RT

		// *************** compute Q1sum  ***************
		
		Q1start = x2pnt(trace, lev10)		// in points
		
		//optional: leading edge trigger for Q1
		if(PSAletrig)
			findlevel/p/q trace, (base+LEthreshold)
			Q1start = ceil(V_levelX)	//in points 
			// use ceil here because FPGA picks first sample over threshold.  Findlevel/p reports e.g. point "50.2" so point 51 is over TH
			//	Q1start = ceil(Q1start/4)*4	// for 4 sample granularity
		endif
	
		Q1sum = 0
		for(j=Q1start+SoQ1;j<Q1start+SoQ1+LoQ1;j+=1)
			Q1sum = Q1sum + trace[j] - base* dbgbase	// optionally set to zero for debug purposes  // normal integration
			//Q1sum = Q1sum + 0.5* trace[j] + 0.5* trace[j+1] - base // trapeziod
		endfor
			
		Q1sum /= normQ1
		Eventvalues[16] = Q1sum 
		
//		// repeat without base (debug)
//		Q1sum = 0
//		for(j=Q1start+SoQ1;j<Q1start+SoQ1+LoQ1;j+=1)
//			Q1sum = Q1sum + trace[j] 	// optionally set to zero for debug purposes  // normal integration
//			//Q1sum = Q1sum + 0.5* trace[j] + 0.5* trace[j+1] - base // trapeziod
//		endfor	
//		Q1sum /= normQ1
//		Eventvalues[23] = Q1sum 
		
		
		// *************** compute Q0sum  ***************
				
		Q0start = x2pnt(trace, lev10)		// in points

		//optional: leading edge trigger for C	with fixed threshold of LEthreshold
		if(PSAletrig)
			findlevel/q/p trace, (base+LEthreshold)
			Q0start = ceil(V_levelX)	//in points
	//		Q0start = ceil(Q0start/4)*4	// for 4 sample granularity
		endif
		
		Q0sum = 0
		for(j=Q0start+SoQ0;j<Q0start+SoQ0+LoQ0;j+=1)
			Q0sum = Q0sum + trace[j] - base * dbgbase	// optionally set to zero for debug purposes		
		endfor
		Q0sum /= normQ0
		Eventvalues[17] = Q0sum
		
//		// repeat without base (debug)
//		Q0sum = 0
//		for(j=Q0start+SoQ0;j<Q0start+SoQ0+LoQ0;j+=1)
//			Q0sum = Q0sum + trace[j] 	// optionally set to zero for debug purposes		
//		endfor
//		Q0sum /= normQ0
//		Eventvalues[22] = Q0sum 
		
		
		
		// *************** compute PSAvalue  ***************

		Eventvalues[19] = Eventvalues[16]/Eventvalues[17]

	
		// *************** compute 50% CFD level  ***************
		
		// Igor compute from trace
		variable cfdlowp, cfdhighp
		variable cfdlow, cfdhigh, cfdlevel, cfdfracI, reltimeI
		cfdlevel = (base+ 0.5*ampl)
		findlevel/q/p trace, cfdlevel
		cfdlowp = floor(V_levelX)
		cfdlow = trace[cfdlowp]
		cfdhighp = ceil(V_levelX)
		cfdhigh = trace[cfdhighp]
		cfdfracI = 1 - (cfdlevel-cfdlow) / (cfdhigh - cfdlow)
			
	
		//if((wftimescale*1e9)<=5)	// comparison wftimescale == 2e-9 fails!
		 if (eventwave[1] == 500)
			// if 500MHz waveforms, cfdtime and offsets are in units of 2ns and also TS is in units of 2ns			
			reltimeI = (Eventvalues[7] & 0xFFFFFFFF) +43 - cfdhighp - cfdfracI
		endif
		if (eventwave[1] == 125)
			// if 125MHz waveforms, cfdtime and offsets are in units of 8ns but TS is in units of 2ns
			reltimeI = (Eventvalues[7] & 0xFFFFFFFF) + 4*cfdhighp - 4*cfdfracI -324
		endif
		if (eventwave[1] == 250)
			// if 250 MHz waveforms from PN, cfdtime and offsets are in units of 4ns but TS is in units of 1ns
			reltimeI = Eventvalues[7] - 4*cfdfracI  + 4*cfdhighp
		endif
		
			
		// read from DSP	/FPGA		
		// Eventvalues[1] includes sample offset and fractional part, scaled differently for 125/500 MHz
		Variable  cfdticksF, cfdfracF, reltimeF
		//if((wftimescale*1e9)<=5)	// comparison wftimescale == 2e-9 fails!
		 if (eventwave[1] == 500)
			cfdfracF = (Eventvalues[1] & 0xFF)/256
			reltimeF = Eventvalues[7]-Eventvalues[1]/256		
		endif
		if (eventwave[1] == 125)
			cfdfracF = (Eventvalues[1] & 0x3FF)/1024
			reltimeF = Eventvalues[7]-Eventvalues[1]/256		
		endif	
		if (eventwave[1] == 250)
			cfdticksF = floor((Eventvalues[1] & 0xFC00) /256/4)		// timestamp of max in units of 4ns
			cfdfracF =  (Eventvalues[1] & 0x3FF)/256/4
			reltimeF = Eventvalues[7]+ 200 - Eventvalues[1]/256  	
		endif		
		
		// report
		Eventvalues[10] = reltimeF			// 10 timestamp refined by CFD time  (DSP/FPGA/Igor)	
		Eventvalues[11] = reltimeI	- CFDoffset	// 11 timestamp refined by CFD time  (Igor)
		Eventvalues[12] = cfdfracF			// 12 CFD time fraction (DSP/FPGA)
		Eventvalues[13] = cfdfracI				// 13 CFD time fraction (Igor)
		
		// debug
	//	Eventvalues[12] = (Eventvalues[1] & 0x3FF)/1024	// 12 CFD time fraction (DSP/FPGA)
	//	Eventvalues[12] = (Eventvalues[1] & 0xFF)/256	// 12 CFD time fraction (DSP/FPGA)
		
					
		// raw values from FPGA	
//		Variable Fcfdlow, Fcfdhigh, Fcfdlevel, Freltime_off, Fcfdtime_off,Fcfd_cfdticks
//		Wave wfarray	
//		Variable Fcfdhighp = 106 - (wfarray[19][ChosenEvent] & 0xFF)/4 - floor(wfarray[19][ChosenEvent]/256)
//		Fcfdlow = wfarray[16][ChosenEvent]
//		Fcfdhigh = wfarray[17][ChosenEvent]
//		Fcfdlevel =  wfarray[18][ChosenEvent]	
//		Fcfd_cfdticks = (wfarray[19][ChosenEvent]	 & 0x000F)	
//		Fcfdtime_off = 1 - (Fcfdlevel-Fcfdlow) / (Fcfdhigh - Fcfdlow)
//		print " "
//		print " Igor CFD level:",floor(cfdlevel), "near points #",  cfdlowp, cfdhighp, "(y=",cfdlow, cfdhigh,")"
//		print " FPGA CFD level:", Fcfdlevel,"near points #", Fcfdhighp-1, Fcfdhighp,"(y=",Fcfdlow,Fcfdhigh,")",Fcfd_cfdticks
//		print " FPGA cfd ticks: ",(wfarray[19][ChosenEvent] & 0xFF), "ft ticks:", 4*floor(wfarray[19][ChosenEvent]/256)
//		print "cfd time Igor:",cfdtime, "Igor from FPGA", Fcfdtime_off, "DSP from FPGA", (Eventvalues[1] & 0xFF)/256
//		Freltime_off =  Eventvalues[7]-Fcfdtime_off - (wfarray[19][ChosenEvent] & 0xFF) - 4*floor(wfarray[19][ChosenEvent]/256)
//		print "relative to TS  Igor:"
//		print/D reltime, "Igor" 
//		print/D Freltime_off, "Igor from FPGA"
//		print/D Freltime_on, "DSP from FPGA"	
		
			 
//		Eventvalues[25] = floor(Eventvalues[1]/1024	)		// 25 FPGA: # samples after flatch
//		Eventvalues[26] = 4*floor(cfdhighp/4)					// 26 Igor: # samples after start of trace
//		Eventvalues[25] = eventvalues[10] - eventvalues[11]
//		Eventvalues[25] = wfarray[21][ChosenEvent]
//		Eventvalues[26] = wfarray[22][ChosenEvent]
		Eventvalues[28] = round(reltimeI-reltimeF)
			 

	endif
	
	
End

Function PW_restoresign(int) // correct for overflow/negative results
	Variable int
	if(int>63000)
		int=int-65536
	endif
	return int
End

Function PW_process_makethewaves(nevents)
	Variable nevents // D or I = 0 for DSP, 1 for Igor, 2 for both 

	String text, combwaves, Csiwaves,plasticwaves,otherwaves, allwaves
	Variable k
	
	Nvar oldNevents =   root:PW:oldNevents 
	if(nevents != oldNevents)
		print  "Making new waves, N events:", nevents
	endif
		
	// Waves for all events
	allwaves = "rt;energy;Q1sm;Q0sm;Bsm;PSAval;Amp;Ratio0;Ratio1;Channel;TrigTime;type"
	
			
	NewDataFolder/O/S root:PW	
	for(k=0;k<ItemsInList(allwaves);k+=1)
		text = StringFromList(k,allwaves)
		if(nevents != oldNevents)
			KillWaves/Z $(text)
			Make/o/n=(Nevents) $(text)
		endif
		wave wav = $(text)
		wav = NaN
	endfor
	
	SetDataFolder root:
		
End




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// get data from files
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Function PW_process_allchannels()
	// a function to process all channels from all modules for the PSAval vs E scatter plot
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar PWchannel	= root:PW:PWchannel 	
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar  Nevents = root:PW:Nevents 				// number of events
	
	Svar OutputFileName = root:pixie4:OutputFileName
	Svar OutBaseName = root:pixie4:OutBaseName
	Nvar RunNumber = root:pixie4:RunNumber
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename
	
	String	text = "root:PW"
	String filename, basefilename
	Variable mo, ch, fn, len, savePWchannel

	//Generate2DMCA(0)					// clear histogram
	//Execute "MCA_2D()"				// show histogram
	PW_AddCombinePSA(0)					// create/clear cumulative waves
	savePWchannel = PWchannel
	
	len = strlen(DataFile)
	basefilename = DataFile[0,len-3]
	
	for(mo=0;mo<NumberOfModules;mo+=1)
	
		ChosenModule = mo
		 Pixie_IO_SelectModChan("SelectedPixie4Module",0," "," ")
		
		// set file for current module
		sprintf filename, "%s%02d", basefilename,mo		// for P500e, each module has a separate file
		datafile = filename
		Pixie_Ctrl_CommonSetVariable("TraceDataFile",0,"","")
//		Open/R/P=EventPath fn as filename
//		PathInfo EventPath
//
//		if(fn == 0)
//			printf "Error: can not open file %s\r", filename
//			break
//		else
//			close fn
//		endif
//		DataFile = filename
//		longDataFilename = S_Path + filename
//		longDataFilename = Pixie_FileNameConversion(longDataFilename)	

		for(ch=0;ch<NumberOfChannels;ch+=1)
			PWchannel = ch				// set channel
		 	PW_file_getPSAdata()		// process file for this channel
		 	DoUpdate
		 	PW_AddCombinePSA(1)			// add to cumulative waves
		endfor
	endfor
	
	PW_AddCombinePSA(2)					// copy cumulative waves back to displayed waves
	PWchannel = savePWchannel
	
	PSA_histo()

End

// Function to combine PSA value and energy for multiple channels/modules/files
// into cumulative final waves
Function PW_AddCombinePSA(app)
Variable app // 0- start new, 1 --append, 2 -- copy back cumulative arrays


	String	text = "root:PW"

	// work on DSP specific waves
	Wave rt = $(text+":rt")					// rise time
	Wave energy = $(text+":energy")		// DSP energy
	Wave Q0sm = $(text+":Q0sm")			// PSA sum (Q0-B)
	Wave Q1sm = $(text+":Q1sm")			// PSA sum (Q1-B)
	Wave Bsm = $(text+":Bsm")			// PSA sum (B)
	Wave PSAval = $(text+":PSAval")		// PSA value (Q1/Q0 ratio)
	Wave Amp = $(text+":Amp")			// amplitude
	Wave Ratio0 = $(text+":Ratio0")		// arbitrary ratio 0
	Wave Ratio1 = $(text+":Ratio1")		// arbitrary ratio 1
	Wave TrigTime = $(text+":TrigTime")	// trigger time
	Wave Chan = $(text+":Channel")		// channel

//	Svar destwavenamex = root:PW:destwavenamex
//	Svar destwavenamey = root:PW:destwavenamey
//	Svar destwavenamez = root:PW:destwavenamez
//	Wave displayx = $("root:PW:"+destwavenamex)		// display wave for x, filled with PSA data of one parameter
//	Wave displayy = $("root:PW:"+destwavenamey)		// display wave for y
//	if(cmpstr(destwavenamez,"none")!=0)
//		Wave displayz = $("root:PW:"+destwavenamez)		// display wave for z
//	else 
//		Wave displayz = $("root:PW:"+destwavenamex)		// if not defined, use x wave instead
//	endif
	
	Nvar  Nevents = root:PW:Nevents 				// number of events
	Variable k,m
	
	if(app==0)	// create new waves and set to zero
		//killwaves/Z  cumx, cumy, cumz
		// make/o/n=1 cumx, cumy, cumz
		killwaves/Z  cumRT, cumE, cumQ0, cumQ1, cumB, cumPSA, cumA, cumR0, cumR1, cumTT, cumCH
		make/d/o/n=1  cumRT, cumE, cumQ0, cumQ1, cumB, cumPSA, cumA, cumR0, cumR1, cumTT, cumCH
		Wave cumRT 
		Wave cumE
		Wave cumQ0
		Wave cumQ1
		Wave cumB
		Wave cumPSA
		Wave cumA
		Wave cumR0
		Wave cumR1
		Wave cumTT
		Wave cumCH
		cumRT = 0
		cumE = 0
		cumQ0 = 0
		cumQ1 = 0
		cumB = 0
		cumPSA = 0
		cumA = 0
		cumR0 = 0
		cumR1 = 0
		cumTT = 0 
		cumCH = 0
	endif
	
	if(app==1)	// copy nonzero elements into cumulative waves
		Wave cumRT 
		Wave cumE
		Wave cumQ0
		Wave cumQ1
		Wave cumB
		Wave cumPSA
		Wave cumA
		Wave cumR0
		Wave cumR1
		Wave cumTT
		Wave cumCH
		wavestats/q cumRT
		m = V_npnts
		for(k=0;k<Nevents;k+=1)
			if( !(Bsm[k]==0) )		// Bsm[k]==0 would indicate an empty record
				Insertpoints m, 1, cumRT, cumE, cumQ0, cumQ1, cumB, cumPSA, cumA, cumR0, cumR1, cumTT, cumCH
				m+=1
				cumRT[m] = rt[k]
				cumE[m] = energy[k]
				cumQ0[m] = Q0sm[k]
				cumQ1[m] = Q1sm[k]
				cumB[m] = Bsm[k]
				cumPSA[m] = PSAval[k]
				cumA[m] = Amp[k]
				cumR0[m] = Ratio0[k]
				cumR1[m] = Ratio1[k]
				cumTT[m] = TrigTime[k]
				cumCH[m] = Chan[k]
			endif
		endfor
	endif
		
	if(app==2)	// copy cumulative waves back into original ones 
		duplicate/o cumRT, $(text+":rt")
		duplicate/o cumE, $(text+":energy")
		duplicate/o cumQ0, $(text+":Q0sm")		
		duplicate/o cumQ1, $(text+":Q1sm")		
		duplicate/o cumB, $(text+":Bsm")
		duplicate/o cumPSA, $(text+":PSAval")
		duplicate/o cumA, $(text+":Amp")
		duplicate/o cumR0, $(text+":Ratio0")	
		duplicate/o cumR1, $(text+":Ratio1")	
		duplicate/o cumTT,  $(text+":TrigTime")
		duplicate/o cumCH, $(text+":Channel")		
		killwaves/Z  cumRT, cumE, cumQ0, cumQ1, cumB, cumPSA, cumA, cumR0, cumR1, cumTT, cumCH
	endif

	
End



Function PW_file_getPSAdata()
	
	// processing parameters
	Nvar PWchannel	= root:PW:PWchannel 			// P4 channel to analyze
	Nvar PWrecordlength	= root:PW:PWrecordlength 	// number of PSA data words for all 4 channels
	Nvar  oldNevents = root:PW:oldNevents 				// remember previous number of events
	Nvar  Nevents = root:PW:Nevents 				// number of events
	Nvar source = root:PW:source
	Nvar CompDSP2Igor = root:PW:CompDSP2Igor
	Wave eventvalues = root:PW:Eventvalues
	Variable ch, NbadeventsIgor, NbadeventsDSP
	Variable k,m, len,n, index, E, ret, cnt, j
	String text
	
	// *** 1 *** get data from file
	Svar DataFile = root:pixie4:DataFile
	Svar longDataFilename = root:pixie4:longDataFilename	
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar ChosenEvent = root:pixie4:ChosenEvent
	wave listmodewave = root:pixie4:listmodewave
	
	Nevents=listmodewave[ChosenModule]
	
	if(source==1)
		print " "
		print "Computing PSA data from traces, file", DataFile
	
		Execute "Pixie_Plot_LMTracesDisplay()"	// open LM trace display since Igor processing needs it open
		PW_process_makethewaves(nevents)
		oldNevents = nevents	
	else
		print " "
		print "Reading PSA values from file", DataFile,"(DSP computed)"		
			
		if (nevents!= oldNevents)
			Killwaves/Z root:pixie4:EventPSAValues
			make/o/u/i/n=(nevents*PWrecordlength*NumberOfChannels) root:pixie4:EventPSAValues
		endif
		
		// event by event, always 8 words for ch 0-1-2-3: energy, XIA, user, user_2, user_3, user_4, user_5
		Wave PSAvalues = root:pixie4:eventpsavalues
		// parse the list mode event file
		ret = Pixie4_Acquire_Data(0x7006, PSAvalues, longDataFilename, ChosenModule)
		if(ret < 0)
			Doalert 0, "Can not read event PSA values from the list mode event file."
			return(-1)
		endif
		
		PW_process_makethewaves(nevents)
		oldNevents = nevents			
	endif
	
	// *** 2 *** sort data from file or memory into waves, do some error checks
	
	text = "root:PW"

	// work on DSP specific waves
	Wave rt = $(text+":rt")					// rise time
	Wave energy = $(text+":energy")		// DSP energy
	Wave Q0sm = $(text+":Q0sm")			// PSA sum (Q0-B)
	Wave Q1sm = $(text+":Q1sm")			// PSA sum (Q1-B)
	Wave Bsm = $(text+":Bsm")			// PSA sum (B)
	Wave PSAval = $(text+":PSAval")		// PSA value (Q1/Q0 ratio)
	Wave Amp = $(text+":Amp")			// amplitude
	Wave Ratio0 = $(text+":Ratio0")		// arbitrary ratio 0
	Wave Ratio1 = $(text+":Ratio1")		// arbitrary ratio 1
	Wave TrigTime = $(text+":TrigTime")	// trigger time
	Wave Chan = $(text+":Channel")		// channel


	NbadeventsIgor = 0
	NbadeventsDSP = 0
	for(m=0;m<nevents;m+=1)		
//	for(m=0;m<10000;m+=1)		
		if(source==1)
			if (mod(m,10000)==0)
				print "processing event #",m
				DoUpdate
			endif
			ChosenEvent = m
			Pixie_IO_ReadEvent()
			
			energy[m]    	= Eventvalues[0]	// energy 
			rt[m]           	= Eventvalues[15] //RT
			Q0sm[m]      = Eventvalues[17] //Q0sum
			Q1sm[m]     	= Eventvalues[16] //Q1sum
			Bsm[m]  	= Eventvalues[20] //PSA sum (B)
			PSAval[m]  	= Eventvalues[19] //PSA value computed by Igor
			if(PSAval[m] <0)
				NbadeventsIgor+=1
			endif
			Amp[m] 		= Eventvalues[18] //Amplitude
			Chan[m]  	= PWchannel
			TrigTime[m] 	=  Eventvalues[7]			
			// debug -- use this to quantify difference betwen DSP and Igor results
			if (CompDSP2Igor)
				rt[m]           	= Eventvalues[4] // Q1 by DSP
				Chan[m]  	= Eventvalues[5] // Q0 by DSP
				TrigTime[m]	= Eventvalues[3] //base by DSP
				ratio0[m]     	= Eventvalues[2] //amplitude by DSP
				ratio1[m]     	= Eventvalues[6] //ratio by DSP
				if(ratio1[m] >65530)
					NbadeventsDSP+=1
				endif
			endif
		
		else	// source 0				
			index = (m*NumberOfChannels+PWchannel)*PWrecordlength			
			// read E, P, C, RT
			// index+0 is timestamp
			energy[m]   	= PSAvalues[index+1]		
			rt[m]           	= PW_restoresign(PSAvalues[index+2])
			Q0sm[m]      = PW_restoresign(PSAvalues[index+5])
			Q1sm[m]      = PW_restoresign(PSAvalues[index+6])
			Bsm[m]  	= PW_restoresign(PSAvalues[index+4])
			PSAval[m]  	= PW_restoresign(PSAvalues[index+7])
			if(PSAval[m] >65530)
				NbadeventsDSP+=1
			endif
			Amp[m] 		= PSAvalues[index+3]
			Chan[m]  	= PWchannel
			TrigTime[m] 	= PSAvalues[index]
		endif	
	endfor	
	print Nevents, "total,", NbadeventsDSP,"bad (DSP),", NbadeventsIgor,"bad (Igor)"
	// *** 3 *** read run statistics from .ifm file
	String filename
	len = strlen(DataFile)
	filename = DataFile[0,len-5]+".ifm"
	PW_file_readIFM(filename)
		
	// *** 4 *** compute histograms and user defined ratios 

	Svar  destwavenameN0 = root:PW:destwavenameN0 
	Svar  destwavenameD0 = root:PW:destwavenameD0
	Svar  destwavenameN1 = root:PW:destwavenameN1 
	Svar  destwavenameD1 = root:PW:destwavenameD1
	
	if (!CompDSP2Igor)
		wave numer0 = $("root:PW:"+destwavenameN0)
		wave denom0 = $("root:PW:"+destwavenameD0)
		Ratio0 = numer0 / denom0
		wave numer1 = $("root:PW:"+destwavenameN1)
		wave denom1 = $("root:PW:"+destwavenameD1)
		Ratio1 = numer1 / denom1
	endif
	
 	PSA_histo()
 	
 	
		
End


	

Function PW_file_readIFM(filename)
String filename

	Svar IFMStartTime = root:PW:IFMStartTime
	Svar IFMStopTime = root:PW:IFMStopTime
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels
	Nvar PWchannel = root:PW:PWchannel

	Wave RT_ifm =  root:PW:RT_ifm
	Wave Nev_ifm =  root:PW:Nev_ifm
	Wave LT_ifm =  root:PW:LT_ifm
	Wave Ntrig_ifm =  root:PW:Ntrig_ifm
	RT_ifm =0 
	Nev_ifm =0
	LT_ifm =0
	Ntrig_ifm =0

	
	Variable filenum,i,len,m, j,k
	String line
	Variable Mnum, Chnum, RT, LT, ER, ICR, NumberMod
		
	Open/R/P=EventPath/T="????" filenum as filename 
	if (cmpstr(S_fileName,"")!=0)		// if file defined succesfully
			
		FReadline filenum, line	// line 1
		if (cmpstr(line[0,8], "XIA Pixie") !=0)
			DoAlert 0, "Not a valid .ifm file, exiting"
			close/a
			return (0)
		endif
		
		FReadline filenum, line  // line 2
		len = strlen(line)
		IFMStartTime = line[23,len-2]
		print "start",IFMStartTime
		
		FReadline filenum, line // line 3
		len = strlen(line)
		IFMStopTime = line[11,len-2]
		print "stop",IFMStopTime
		
		FReadline filenum, line	// line 4: blank
		FReadline filenum, line	// line 5
		sscanf line, "Number of Modules: %d\r", NumberMod
		FReadline filenum, line	//module header
		k=0
		do
			FReadline filenum, line
			sscanf line, "%d %g %g", Mnum, RT, ER
			RT_ifm[k] = RT
			Nev_ifm[k] = RT*ER
			k+=1
		while (k<NumberMod)
		
		FReadline filenum, line	// line after module results: blank
		FReadline filenum, line	// channel results header: 
		k=0
		do
			for(i=0;i<NumberOfChannels;i+=1)
				FReadline filenum, line
				sscanf line, "%d %d %g %g", Mnum, Chnum, LT, ICR
				LT_ifm[k][i] =LT
				Ntrig_ifm[k][i] =ICR*LT
			endfor
			k+=1
		while (k<4)
	
		close filenum
		print "Runtime", RT_ifm[0],"s ;  Livetime", LT_ifm[0][PWchannel],"s"
		
	else		// if file opened not successfully
		printf "PW_file_readIFM: open statistics file failed, skipping ...\r" 
		
	endif
		
End



Function PSA_histo()
		histogram/B=1 root:PW:RT,  root:PW:RT_histo
		histogram/B={0,4,16384} root:PW:Q0sm,  root:PW:Q0_histo
		histogram/B={0,4,16384} root:PW:Q1sm,  root:PW:Q1_histo
		histogram/B={0,4,16384} root:PW:Bsm,  root:PW:Bsm_histo
		histogram/B={0,4,16384} root:PW:PSAval,  root:PW:PSAval_histo
		histogram/B=1 root:PW:Ratio0,  root:PW:Ratio0_histo
		histogram/B=1 root:PW:Ratio1,  root:PW:Ratio1_histo
		histogram/B={0,4,16384} root:PW:amp,  root:PW:amp_histo
End




Function PW_Panel_Call_Buttons(ctrlname)
String ctrlname

	Nvar source = root:PW:source
	Nvar Allchannels = root:PW:Allchannels
	
	if(cmpstr(ctrlname,"clr")==0)	
		Generate2DMCA(0)
		return 0
	endif
	
	if(cmpstr(ctrlname,"app")==0)	
		Generate2DMCA(1)
		return 0
	endif
	
	if(cmpstr(ctrlname,"shw")==0)	
		ShowROICursors()
		return 0
	endif
	
	if(cmpstr(ctrlname,"roi")==0)	
		Analyze_sumROIcsr()
		return 0
	endif
	
	if(cmpstr(ctrlname,"ngcount")==0)	
		Analyze_sumPSATH()
		return 0
	endif
	
	
	if(cmpstr(ctrlname,"ReadDSPPSA")==0)	
		source = 0
		if(Allchannels)
			PW_process_allchannels()
		else
			PW_file_getPSAdata()
		endif
		return 0 
	endif
	
	if(cmpstr(ctrlname,"ReadDT3")==0)	
		
		LoadWave/G/O/P=EventPath/N=dt3import
		duplicate/o root:dt3import0,  root:PW:Ratio0	// resize to current length
		duplicate/o root:dt3import0,  root:PW:Ratio1
		duplicate/o root:dt3import1,  root:PW:Channel
		duplicate/o root:dt3import2,  root:PW:Trigtime
		duplicate/o root:dt3import3,  root:PW:energy
		duplicate/o root:dt3import4,  root:PW:rt
		duplicate/o root:dt3import5,  root:PW:Amp
		duplicate/o root:dt3import6,  root:PW:Bsm
		duplicate/o root:dt3import7,  root:PW:Q0sm
		duplicate/o root:dt3import8,  root:PW:Q1sm
		duplicate/o root:dt3import9,  root:PW:PSAval
		
		Svar  destwavenameN0 = root:PW:destwavenameN0 
		Svar  destwavenameD0 = root:PW:destwavenameD0
		Svar  destwavenameN1 = root:PW:destwavenameN1 
		Svar  destwavenameD1 = root:PW:destwavenameD1
		
		wave Ratio0 =  root:PW:Ratio0
		wave numer0 = $("root:PW:"+destwavenameN0)
		wave denom0 = $("root:PW:"+destwavenameD0)
		Ratio0 = numer0 / denom0
		wave Ratio1 =  root:PW:Ratio1
		wave numer1 = $("root:PW:"+destwavenameN1)
		wave denom1 = $("root:PW:"+destwavenameD1)
		Ratio1 = numer1 / denom1
		
		PSA_histo()

		
		killwaves/Z dt3import0, dt3import1, dt3import2, dt3import3, dt3import4, dt3import5
		killwaves/Z dt3import6, dt3import7, dt3import8, dt3import9
		return(0)
	endif
	
	
	if(cmpstr(ctrlname,"ComputeIgorPSA")==0)	
		source = 1
		if(Allchannels)
			PW_process_allchannels()
		else
			PW_file_getPSAdata()
		endif
		source = 0
		return 0 
	endif
	
	if(cmpstr(ctrlname,"ComputeClibPSA")==0)	
		Variable ret
		Nvar LoQ1 = root:PW:LoQ1 // = 12 // length of sum Q1 and its baseline
		Nvar LoQ0 = root:PW:LoQ0 // = 12 // length of sum Q0	 and its baseline
		Nvar SoQ1 = root:PW:SoQ1 // = 0	// starting point of Q1 relative to high or low RT level
		Nvar SoQ0 =root:PW:SoQ0 //= 24	// starting point of Q0 relative to low RT level
		Nvar RTlow = root:PW:RTlow //= 0.1
		Nvar RThigh = root:PW:RThigh //=0.9	
		Nvar RTrise = toot:PW:RTrise		// ignored in C code
		Nvar PSAoption = root:PW:PSAoption			// 0: Q1/Q0; 1: (Q1-Q0)/Q0
		Nvar Q1startoption = root:PW:Q1startoption			// relative to high or low threshold
		Nvar PSAdiv8 = root:PW:PSAdiv8    			// divide result by 8
		Nvar PSAletrig = root:PW:PSAletrig			// use leading edge trigger
		Nvar PSAth = root:PW:PSAth 		// trigger threshold in % (CFD) or ADC steps (LE)
		
		Nvar ChosenModule = root:pixie4:ChosenModule
		Svar longDataFilename = root:pixie4:longDataFilename
		make/o/u/i/n=(20) root:PW:PSAval4C
		wave  PSAval4C =  root:PW:PSAval4C
		PSAval4C[1] = LoQ0
		PSAval4C[2] = LoQ1
		PSAval4C[3] = SoQ0
		PSAval4C[4] = SoQ1
		PSAval4C[5] = floor(RTlow*100)	// must be integer
		PSAval4C[6] = floor(RThigh*100)
		PSAval4C[7] = PSAoption
		PSAval4C[8] = PSAdiv8
		PSAval4C[9] = PSAletrig
		PSAval4C[10] = PSAth
		
		ret = Pixie4_Acquire_Data(0x7030, PSAval4C, longDataFilename, ChosenModule)
		if(ret < 0)
			Doalert 0, "Can not parse list mode event file."
			return(-1)
		endif
		return 0 
	endif
	
	
	
	Execute (ctrlname+"()")
	
End


Window PW_PSAList() : Table
	DoWindow/F PSAList
	if (V_Flag!=1)
		Edit/W=(280,50,800,450)/K=1 root:PW:energy,root:PW:rt,root:PW:Q0sm,root:PW:Q1sm, root:PW:Bsm
		DoWindow/C PSAList
		AppendToTable root:PW:PSAval,root:PW:Amp
		AppendToTable root:PW:Ratio0,  root:PW:Ratio1, root:PW:Trigtime,  root:PW:Channel
		ModifyTable width=40
		ModifyTable width( root:PW:Ratio0)=60
		ModifyTable width( root:PW:Ratio1)=60
		ModifyTable width( root:PW:rt)=60
		ModifyTable width( root:PW:PSAval)=60
		ModifyTable width( root:PW:amp)=60
		
		ModifyTable title(root:PW:energy)="Energy"
		ModifyTable title(root:PW:rt)="Rise Time"
		ModifyTable title(root:PW:Q0sm)="Q0-B"
		ModifyTable title(root:PW:Q1sm)="Q1-B"
		ModifyTable title(root:PW:Bsm)="B"
		ModifyTable title(root:PW:amp)="Amplitude"
		ModifyTable title(root:PW:PSAval)="PSA value"
		ModifyTable title(root:PW:Ratio0)="Ratio0"
		ModifyTable title(root:PW:Ratio1)="Ratio1"
		ModifyTable title(root:PW:Channel)="Channel"
		ModifyTable title(root:PW:Trigtime)="Trigtime"
	endif
EndMacro

Function PW_Panel_PopProc(ctrlName,popNum,popStr) : PopupMenuControl
	String ctrlName
	Variable popNum	
	String popStr
	
	if(cmpstr(ctrlName,"scattery")==0)
		Svar destwavename = root:PW:destwavenamey
	endif
	if(cmpstr(ctrlName,"scatterx")==0)
		Svar destwavename = root:PW:destwavenamex
	endif
	if(cmpstr(ctrlName,"scatterz")==0)
		Svar destwavename = root:PW:destwavenamez
	endif
	if(cmpstr(ctrlName,"ratioD0")==0)
		Svar destwavename = root:PW:destwavenameD0
	endif
	if(cmpstr(ctrlName,"ratioN0")==0)
		Svar destwavename = root:PW:destwavenameN0
	endif
	if(cmpstr(ctrlName,"ratioD1")==0)
		Svar destwavename = root:PW:destwavenameD1
	endif
	if(cmpstr(ctrlName,"ratioN1")==0)
		Svar destwavename = root:PW:destwavenameN1
	endif
	
	if(popNum==1)	
		destwavename = "<select>"	
	endif
		
	if(popNum==2)
		destwavename = "energy"
	endif
	
	if(popNum==3)
		destwavename = "rt"
	endif
	
	if(popNum==4)
		destwavename = "Q0sm"
	endif
	
	if(popNum==5)
		destwavename = "Q1sm"
	endif
	
	if(popNum==6)
		destwavename = "Bsm"
	endif
	
	if(popNum==7)
		destwavename = "PSAval"
	endif
	
	if(popNum==8)
		destwavename = "Amp"
	endif
	
	if(popNum==9)
		destwavename = "Ratio0"
	endif
	
	if(popNum==10)
		destwavename = "Ratio1"
	endif
	
	if(popNum==11)
		destwavename = "none"
	endif

	



	
End


Window PW_PSA_scatterplot() : Graph
	DoWindow/F PSAscatter	
	if (V_Flag!=1)
		SetDataFolder  root:PW
		Display /K=1/W=(485.25,248.75,911.25,475.25) $(root:PW:destwavenamey) vs  $(root:PW:destwavenamex)
		DoWindow/C PSAscatter
		ModifyGraph mode=2
		ModifyGraph lSize=1	
		ModifyGraph zero=1
		ModifyGraph mirror=2
		ModifyGraph lblMargin(left)=2,lblMargin(bottom)=3
		ModifyGraph lblLatPos(bottom)=-18
		Label left root:PW:destwavenamey
		Label bottom root:PW:destwavenamex
		if(cmpstr(root:PW:destwavenamez,"none")!=0)
			ModifyGraph zColor($(root:PW:destwavenamey))={$(root:PW:destwavenamez),*,*,Rainbow}
			ColorScale/C/N=text0/F=0/A=RC/X=0.8 trace=$(root:PW:destwavenamey)
		endif
		ShowInfo
		SetDataFolder  root:
	endif
EndMacro

Menu "&User PSA"
	"&User Control Panel", User_Control()
	"&PSA Analysis",PSA_Analysis()
	"&2D MCA [Igor offline]",MCA_2D()
	"&ROI MCAs",MCA_ROI_1D()
	"&2D MCA [DSP online]",PSA2D_plot()
End


Function PSA_Analysis() : Panel
	PauseUpdate; Silent 1		// building window...
	
	DoWindow/F PSA__Analysis
	if (V_flag!=1)
		NewPanel /K=1/W=(350,70,585,730) as "PSA Results"
		ModifyPanel cbRGB=(65280,59904,48896)
		DoWindow/C PSA__Analysis	

		variable buttonx = 200
		Variable ctrlx = 16
		Variable sety = 27

		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 10,20,"General Analysis Settings"
		
		SetVariable Igor1a,pos={ctrlx,sety-2},size={140,16},title="Channel to process",help={"select 0..3"}, limits={0,3,1}
		SetVariable Igor1a,fSize=12,format="%g",value= root:PW:PWchannel, font=Arial
		Checkbox Igor1,pos={ctrlx+155,sety},size={160,16},title="all",help={"loop over all channels and modules when reading from file"}
		Checkbox Igor1,fSize=12,variable= root:PW:Allchannels, font=Arial
		sety+=2

		
		SetDrawEnv fsize= 16,fstyle= 0
		DrawText ctrlx+120,sety+40,"  /    "
		DrawText ctrlx+120,sety+60,"  /    "
		
		popupmenu ratioN0,  pos={ctrlx,sety+20}, title = "Ratio0 = ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu ratioN0, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude", mode=1,font=Arial
		popupmenu ratioN0, help={"Define a new parameter 'ratio0' from one PSA value divided by another"} 
		popupmenu ratioD0,  pos={ctrlx+130,sety+20}, title = " ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu ratioD0, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude", mode=1,font=Arial
		popupmenu ratioD0, help={"Define a new parameter 'ratio0' from one PSA value divided by another"} 

		popupmenu ratioN1,  pos={ctrlx,sety+45}, title = "Ratio1 = ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu ratioN1, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude;Ratio0", mode=1,font=Arial
		popupmenu ratioN1, help={"Define a new parameter 'ratio1' from one PSA value divided by another"} 
		popupmenu ratioD1,  pos={ctrlx+130,sety+45}, title = " ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu ratioD1, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude;Ratio0", mode=1,font=Arial
		popupmenu ratioD1, help={"Define a new parameter 'ratio1' from one PSA value divided by another"} 


		
		Variable filey = 117
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 10,filey,"Review PSA results from file"
		
		SetVariable TraceDataFile, value=root:pixie4:DataFile, pos={ctrlx, filey+3},size={165,18},title="File"
		SetVariable TraceDataFile, fsize=12,proc=Pixie_Ctrl_CommonSetVariable,font=Arial//,bodywidth=100
		Button FindTraceDataFile, pos={ctrlx+170,filey+1},size={30,20},proc=Pixie_Ctrl_CommonButton,title="Find",fsize=11,font=Arial
		
		Button PW_eventPanel,proc=PW_Panel_Call_Buttons,title="Show PSA results with LM traces",font=Arial
		Button PW_eventPanel,pos={ctrlx, filey +25},size={buttonx,19}, fsize=11, help={"Open panel with PSA results computed by DSP and Igor for each trace"} 
		
		SetDrawEnv linefgc= (39168,0,31232)
		DrawLine 10, filey+51, 220, filey+51
		
		filey+=10
		
		Button ReadDSPPSA,pos={ctrlx, filey+50},size={buttonx,19},proc=PW_Panel_Call_Buttons,title="Read DSP PSA Data from binary file"
		Button ReadDSPPSA,help={"Read list data from file, extract PSA values computed by DSP"},fsize=11,font=Arial
		
		SetDrawEnv fsize= 8,fstyle= 0
		DrawText 105,filey+82,"-- or --"
		
		Button ComputeIgorPSA,pos={ctrlx, filey+84},size={buttonx,19},proc=PW_Panel_Call_Buttons,title="Compute PSA Data from traces (Igor)"
		Button ComputeIgorPSA,help={"Read list data from file, compute PSA values from traces in Igor"},fsize=11,font=Arial
		
		filey+=34
		SetDrawEnv fsize= 8,fstyle= 0
		DrawText 105,filey+82,"-- or --"
		
		Button ReadDT3,pos={ctrlx, filey+84},size={buttonx,19},proc=PW_Panel_Call_Buttons,title="Read DSP PSA Data from text file"
		Button ReadDT3,help={"Read dt3 file, extract PSA values computed by DSP. All channels in one module"},fsize=11,font=Arial

		
		SetDrawEnv linefgc= (39168,0,31232)
		DrawLine 10, filey+115, 220, filey+115

		filey +=124
		Button PW_PSAList,pos={ctrlx, filey},size={buttonx,19},proc=PW_Panel_Call_Buttons,title="Open PSA Table"
		Button PW_PSAList,help={"Open Table with PSA raw data"}, fsize=11,font=Arial
		
		Button PW_PSA_scatterplot,pos={ctrlx, filey+26},size={buttonx,19},proc=PW_Panel_Call_Buttons,title="Display scatter plot"
		Button PW_PSA_scatterplot,help={"Create scatter plot of one parameter vs another, select below"}, fsize=11,font=Arial
		
		popupmenu scattery,  pos={ctrlx+20,filey+50}, title = "y wave      ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu scattery, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude;Ratio0;Ratio1", mode=1,font=Arial
		
		popupmenu scatterx,  pos={ctrlx+20,filey+74}, title = "x wave      ", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu scatterx, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude;Ratio0;Ratio1", mode=1,font=Arial
		
		popupmenu scatterz,  pos={ctrlx+20,filey+98}, title = "color wave", proc = PW_Panel_PopProc, size ={120,20}
		popupmenu scatterz, value="<select>;energy;risetime;Q0sum;Q1sum;Bsum;PSAvalue;Amplitude;Ratio0;Ratio1;none", mode=1,font=Arial
		
		
		variable igory = 430
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 10,igory,"Parameters for Igor (offline) PSA"

		
		SetVariable Igor2,pos={ctrlx,igory+7},size={180,16},title="Sum Q0 length (LoQ0) ",help={"number of samples in earlier sum"}
		SetVariable Igor2,fSize=12,format="%g",value= root:PW:LoQ0,fsize=11,font=Arial	
		SetVariable Igor0,pos={ctrlx,igory+27},size={180,16},title="Sum Q1 length (LoQ1) ",help={"number of samples in later sum"}
		SetVariable Igor0,fSize=12,format="%g",value= root:PW:LoQ1,	fsize=11,font=Arial
	
		SetVariable Igor5,pos={ctrlx,igory+47},size={180,16},title="Sum Q0 delay (SoQ0)  ",help={"starting point of Q0 relative to low RT level"}
		SetVariable Igor5,fSize=12,format="%g",value= root:PW:SoQ0,fsize=11,font=Arial
		SetVariable Igor4,pos={ctrlx,igory+67},size={180,16},title="Sum Q1 delay (SoQ1)  ",help={"starting point of Q1 relative to low or high RT level"}
		SetVariable Igor4,fSize=12,format="%g",value= root:PW:SoQ1,fsize=11,font=Arial
				
		SetVariable Igor20,pos={ctrlx,igory+87},size={90,16},title="RT low",help={"lower threshold for RT computation (0.1 = 10%)"}
		SetVariable Igor20,fSize=12,format="%g",value= root:PW:RTlow,fsize=11,font=Arial
		SetVariable Igor21,pos={ctrlx+100,igory+87},size={80,16},title="high ",help={"upper threshold for RT computation (0.7 = 70%)"}
		SetVariable Igor21,fSize=12,format="%g",value= root:PW:RThigh,fsize=11,font=Arial
		
		
		ctrlx+=15
		igory+=30
		Checkbox Igor22,pos={ctrlx,igory+79},size={160,16},title="Compute rise, not fall  ",help={"if checked, look for rise from start to maximum, else for fall from maximum to end"}
		Checkbox Igor22,fSize=12,variable= root:PW:RTrise,fsize=11,font=Arial
	//	Checkbox Igor6,pos={ctrlx,igory+97},size={160,16},title="Q1 start relative to low RT",help={"0 - start at high RT level, 1- start at low RT level"}
	//	Checkbox Igor6,fSize=10,variable= root:PW:Q1startoption			
	//	Checkbox Igor9,pos={ctrlx,igory+115},size={160,16},title="PSA value = Q1/Q0    ",help={"0 - Q1/Q0, 1- (Q1-Q0)/Q0"}
	//	Checkbox Igor9,fSize=10,variable= root:PW:PSAoption
		Checkbox Igor10,pos={ctrlx,igory+97},size={160,16},title="Divide result by 8    ",help={"for long sums"}
		Checkbox Igor10,fSize=12,variable= root:PW:PSAdiv8,fsize=11,font=Arial
		Checkbox Igor11,pos={ctrlx,igory+115},size={160,16},title="Leading edge trigger    ",help={"use leading edge trigger instead of CFD"}
		Checkbox Igor11,fSize=12,variable= root:PW:PSAletrig,fsize=11,font=Arial

		ctrlx-=15
		SetVariable Igor12,pos={ctrlx,igory+133},size={180,16},title="PSA Threshold         ",help={"in % for CFD or in ADC steps for LE"}
		SetVariable Igor12,fSize=12,format="%g",value= root:PW:PSAth, limits={0,65535,1},fsize=11,font=Arial
		SetVariable Igor13,pos={ctrlx,igory+151},size={180,16},title="CFD offset (ns)        ",help={"Offset between Igor counting from start of trace vs FPGA counting from trigger"}
		SetVariable Igor13,fSize=12,format="%g",value= root:PW:CFDoffset, limits={0,65535,1},fsize=11,font=Arial
		
		Button ComputeClibPSA,pos={ctrlx, igory+173},size={buttonx,20},proc=PW_Panel_Call_Buttons,title="Compute PSA Data from traces (C)"
		Button ComputeClibPSA,help={"Compute PSA values from traces in C, generating .dt3 from .b00"},fsize=11,font=Arial



	endif
End

Function Eventtype()	// Assign a type ID according to wave characteristics

	Nvar  Nevents = root:PW:Nevents 				// number of events
	String text
	Variable m
	text = "root:PW"
	Wave rt = $(text+":rt")					// rise time
	Wave energy = $(text+":energy")		// DSP energy
	Wave Q0sm = $(text+":Q0sm")			// PSA sum (Q0-B)
	Wave Q1sm = $(text+":Q1sm")			// PSA sum (Q1-B)
	Wave Bsm = $(text+":Bsm")			// PSA sum (B)
	Wave PSAval = $(text+":PSAval")		// PSA value computed by DSP
	Wave Amp = $(text+":Amp")			// amplitude
	Wave Ratio0 = $(text+":Ratio0")			// arbitrary ratio
	Wave Ratio1 = $(text+":Ratio1")			// arbitrary ratio
	Wave TrigTime = $(text+":TrigTime")	// trigger time
	Wave Chan = $(text+":Channel")		// channel
	Wave type = $(text+":type")		// channel
	
	Variable QE0lowRn = 8.4
	Variable QE0highRn = 15
	Variable QE1lowRn =  2.35
	Variable QE1highRn =  5.3
	Variable QE0lowPl = 18
	Variable QE0highCo = QE0lowPl
	Variable QE0lowCo = 2.5
	Variable QE0highCs = QE0lowCo
	
	type = nan

	for(m=0;m<nevents;m+=1)
		if(Ratio0[m] <QE0highCs)
			type[m] = 10
		endif
		
		if(Ratio0[m] >QE0lowPl)
			type[m] = 20
		endif
		
		if( (Ratio0[m] >QE0lowCo) && (Ratio0[m] <QE0highCo) )
			type[m] = 30
		endif
		
		if( (Ratio0[m] >QE0lowRn) && (Ratio0[m] <QE0highRn) && (Ratio1[m] <QE1highRn) && (Ratio1[m] >QE1lowRn)  )
//		if(  (Ratio[m] >QE1lowRn)  )
	
			type[m] = 50
		endif
	endfor


End

Window PW_EventPanel() : Panel
	PauseUpdate; Silent 1		// building window...
	Pixie_Plot_LMTracesDisplay()
	Dowindow/F PW_Events
	if (V_flag!=1)
		NewPanel/K=1 /W=(600,450,920,750)
		Dowindow/C PW_Events
		ModifyPanel cbRGB=(65280,59904,48896)
		
		//ValDisplay valdisp0,pos={8,8},size={180,15},title="Event Number in File    "
		//ValDisplay valdisp0,limits={0,0,0},barmisc={0,1000}, value= root:pixie4:ChosenEvent
		
		SetVariable CallReadEvents,pos={8,8},size={180,15},proc=Pixie_Ctrl_CommonSetVariable,title="Event Number                "
		SetVariable CallReadEvents,format="%d",fsize=12,font=Arial//,bodywidth=70
		SetVariable CallReadEvents,limits={0,Inf,1},value= root:pixie4:ChosenEvent

		
		Variable xio=145	
		Variable xfo=220	

		Variable xdo=18
		
		SetDrawEnv fsize= 11,fstyle= 2
		DrawText 12,42,"                       DSP/FPGA              Igor                   debug"
		
		ValDisplay valdisp1,pos={xdo,45},size={125,15},title="Energy       ", fsize=12, font=Arial
		ValDisplay valdisp1,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[0]
		 
		ValDisplay valdisp2,pos={xdo,65},size={125,15},title="Rise Time ", fsize=12, font=Arial
		ValDisplay valdisp2,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[1],valueColor=(50000,50000,50000 )
		ValDisplay valdisp2a,pos={xio,65},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp2a,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[15]
	
		
		ValDisplay valdisp3,pos={xdo,85},size={125,15},title="Amplitude  ", fsize=12, font=Arial
		ValDisplay valdisp3,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[2]
		ValDisplay valdisp3a,pos={xio,85},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp3a,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[18]
		
		ValDisplay valdisp4,pos={xdo,105},size={125,15},title="B                 ", fsize=12, font=Arial
		ValDisplay valdisp4,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[3]
		ValDisplay valdisp4a,pos={xio,105},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp4a,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[20]
		ValDisplay valdisp4b,pos={xfo,105},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp4b,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[24] ,valueColor=(50000,50000,50000 )

		
		ValDisplay valdisp5b,pos={xdo,125},size={125,15},title="Q0 sum     ", fsize=12, font=Arial
		ValDisplay valdisp5b,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[5]
		ValDisplay valdisp5a,pos={xio,125},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp5a,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[17]
		ValDisplay valdisp5c,pos={xfo,125},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp5c,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[25] ,valueColor=(40000,40000,40000 )

		
				
		ValDisplay valdisp7,pos={xdo,145},size={125,15},title="Q1 sum     ", fsize=12, font=Arial
		ValDisplay valdisp7,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[4]
		ValDisplay valdisp11,pos={xio,145},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp11,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[16]
		ValDisplay valdisp7c,pos={xfo,145},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp7c,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[26] ,valueColor=(40000,40000,40000 )
				
					
		ValDisplay valdisp12a,pos={xdo,165},size={125,15},title="PSA value  ", fsize=12, font=Arial
		ValDisplay valdisp12a,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[6]//	,valueColor=(50000,50000,50000 )
		ValDisplay valdisp10,pos={xio,165},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp10,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[19]
		
	//	ValDisplay valdisp13,pos={xdo,185},size={200,15},title="Ratio                                       "
	//	ValDisplay valdisp13,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[21]
	
	// 10 timestamp refined by CFD time  (DSP/FPGA/Igor)
	// 11 timestamp refined by CFD time  (Igor)
	// 12 CFD time fraction (DSP/FPGA)
	// 13 CFD time fraction (Igor)
		ValDisplay valdisp20,pos={xdo,185},size={125,15},title="CFD fract.  ", fsize=12, font=Arial
		ValDisplay valdisp20,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[12]//	,valueColor=(50000,50000,50000 )
		ValDisplay valdisp21,pos={xio,185},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp21,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[13]

		ValDisplay valdisp22,pos={xdo,225},size={220,15},title="TS                          ", fsize=12, font=Arial
		ValDisplay valdisp22,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[7], format="%10.0f"
		ValDisplay valdisp23,pos={xdo,245},size={220,15},title="TS-CFD  (DSP)   ", fsize=12, font=Arial
		ValDisplay valdisp23,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[10], format="%10.5f"
		ValDisplay valdisp24,pos={xdo,265},size={220,15},title="TS-CFD  (Igor)    ", fsize=12, font=Arial
		ValDisplay valdisp24,limits={0,0,0},barmisc={0,1000}, value=root:PW:Eventvalues[11], format="%10.5f"

		// debug	
		ValDisplay valdisp32,pos={xfo,65},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp32,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[22] ,valueColor=(50000,50000,50000 )	
		ValDisplay valdisp33,pos={xfo,85},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp33,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[23] ,valueColor=(50000,50000,50000 )
	
		ValDisplay valdisp34,pos={xfo,165},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp34,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[27] ,valueColor=(50000,50000,50000 )	
		ValDisplay valdisp35,pos={xfo,185},size={75,15},title="    ", fsize=12, font=Arial
		ValDisplay valdisp35,limits={0,0,0},barmisc={0,1000}, value= root:PW:Eventvalues[28] ,valueColor=(50000,50000,50000 )
	endif
	
EndMacro



Function Generate2DMCA(app)
Variable app	// append: 0= clear, recreate waves, 1 = append, 2 = rescale
	
	Nvar scalex = root:PW:scalex
	Nvar scaley = root:PW:scaley
	Nvar offx = root:PW:offx
	Nvar offy = root:PW:offy
	
	Svar destwavenamex = root:PW:destwavenamex
	Svar destwavenamey = root:PW:destwavenamey
	
	wave energy1list = $("root:PW:"+destwavenamex) 
	wave energy2list =$("root:PW:"+destwavenamey)


	Variable Nevents, Ex, Ey, xy,k
	Nvar nbins = root:PW:nbins
	
	make/o/n=(nbins*nbins) x2D, y2D, z2D
	wave x2D
	wave y2D
	wave z2D

	// 2D spectra
	wavestats/q energy1list
	Nevents = V_npnts
			
	if (app==0)
		z2D=0
	endif
	
	if(app==1)	
		for(k=0;k<Nevents;k+=1)
			Ex = floor((energy1list[k]-offx)/scalex) 
			Ey = floor((energy2list[k]-offy)/scaley) 
			if( (Ey<nbins) && (Ex <nbins) ) // if in range ...
				xy = Ex+(Ey*nbins)
				z2D[xy]+=1						
			endif
		endfor
			
		z2D[0]=0			
	endif
	
	// always
	x2d = mod(p,nbins)
	y2d = floor(p/nbins)
	x2D=x2D*scalex+offx
	y2D=y2D*scaley+offy
		
End

Window MCA_ROI_1D() : Graph
	PauseUpdate; Silent 1		// building window...
	
	DoWindow/F MCA_ROI
	if(V_flag!=1)	
		display/K=1/W=(200,500,650,620)  E1_roi  as "1D ROI spectra projections"
		DoWindow/C MCA_ROI
		AppendToGraph E2_roi
		ModifyGraph mode=6,rgb(E2_roi)=(0,15872,65280)
		Label bottom root:PW:destwavenamex+" and/or "+root:PW:destwavenamey
		Label left "N counts"
	endif
EndMacro

Window MCA_2D() : Graph
	PauseUpdate; Silent 1		// building window...
	
	DoWindow/F MCA_2D
	if(V_flag!=1)
		Display /K=1/W=(200,70,650,460) root:y2D vs root:x2D as "2D spectrum"
		DoWindow/C MCA_2D
		ControlBar 75
		SetVariable scx,pos={10,10},size={130,16},title="x scaling factor",fsize=11,value= root:PW:scalex,font=Arial
		SetVariable scx, help = {"Bin size in x axis. Use 256 for full range of DSP parameters. Possibly <0.1 for 'ratio'"}
		SetVariable ofx,pos={155,10},size={100,16},title="x offset",fsize=11,value= root:PW:offx,font=Arial
		SetVariable ofx, help = {"Offset in x axis (left limit)"}
		SetVariable scy,pos={10,35},size={130,16},title="y scaling factor",fsize=11,value= root:PW:scaley,font=Arial
		SetVariable scy, help = {"Bin size in y axis Use 256 for full range of DSP parameters. Possibly <0.1 for 'ratio'"}
		SetVariable ofy,pos={155,35},size={100,16},title="y offset",fsize=11,value= root:PW:offy,font=Arial
		SetVariable ofy, help = {"Offset in y axis (lower limit)"}
		Button clr, pos={270,10}, size = {80,20}, title="Clear/Rescale", proc=PW_Panel_Call_Buttons
		Button clr, help={"Clear the spectrum and apply current bin size"}
		Button app,  pos={270,35}, size = {80,20}, title="Append", proc=PW_Panel_Call_Buttons
		Button app, help={"Add the PSA waves from scatter plot to MCA"}
		Button shw,  pos={360,10}, size = {100,20}, title="Show cursor", proc=PW_Panel_Call_Buttons
		Button roi,  pos={360,35}, size = {100,20}, title="Sum ROI", proc=PW_Panel_Call_Buttons
		Button roi, help={"Sum the counts within the cursor box"} 
		
		SetVariable ngthr,pos={470,10},size={120,16},title="PSA TH",fsize=11,value= root:PW:ngth,font=Arial
		SetVariable ngthr, help = {"Threshold in 'y' to sum events above and below (e.g, gammas and neutrons)"}
		Button ngcount,  pos={470,35}, size = {120,20}, title="Sum above/below TH", proc=PW_Panel_Call_Buttons
		Button ngcount, help={"Sum the counts above and below the PSA TH"} 
 
		ModifyGraph mode=2
		ModifyGraph marker=18
		ModifyGraph lSize=2
		ModifyGraph rgb=(0,0,0)
		ModifyGraph msize=2
		ModifyGraph logZColor=1,zColor(y2D)={z2D,1,*,Spectrum,0}
		ModifyGraph zColorMin(y2D)=(65535,65535,65535)
		ModifyGraph mirror=2
		Label left root:PW:destwavenamey
		Label bottom root:PW:destwavenamex
		ColorScale/N=text0/A=MC/X=38.45/Y=11.00/E=2 trace=y2D, nticks=8, log =1, minor=1
		
		Appendtograph root:th2D vs root:x2D
		ModifyGraph rgb=(0,0,0),lstyle(th2D)=8
	endif
EndMacro



Function Analyze_sumPSATH()
	
	Nvar nbins = root:PW:nbins
	Wave  x2D = root:x2D
	Wave  y2D = root:y2D
	Wave  z2D = root:z2D
	Nvar ngth = root:PW:ngth
	Nvar scaley = root:PW:scaley
	
	make/o/n=(nbins,nbins) root:MCA_2Dcount
	Wave  MCA_2Dcount =  root:MCA_2Dcount
	MCA_2Dcount = z2D		// MCA_2D[p][q]: p = x, q = y
	
	make/o/n=(nbins)  root:E1_roi,  root:E2_roi
	Wave E1_roi =  root:E1_roi
	Wave E2_roi =  root:E2_roi
	E1_roi = 0
	E2_roi = 0 
	
	Variable k, j, sumup, sumdown, updownth
	Variable x1,y1,x2,y2
	
	x1 = 0
	x2 = nbins
	y1 = 0
	y2 = nbins
	updownth = floor(ngth/scaley)
	
	sumup = 0
	sumdown = 0
	for (k=x1;k<=x2;k+=1)
		for(j=y1;j<=y2;j+=1)
			if(j>updownth)
				sumup += MCA_2Dcount[k][j]		
				E1_roi[k] += MCA_2Dcount[k][j]	
			else
				sumdown += MCA_2Dcount[k][j]	
				E2_roi[k] += MCA_2Dcount[k][j]		
			endif
		endfor
	endfor
	
	setscale/P x,0,16, E1_roi	// PARAMETER 32 is scaling of 2D spectrum
	setscale/P x,0,16, E2_roi
	
	print "Counts above threshold:", sumup
	print "Counts below/on threshold:", sumdown
	
	wave th2D
	th2D = ngth

End

Function Analyze_sumROIcsr()

	Variable p1,p2, p3, ongraph
	
	ongraph=1
	if (cmpstr(csrinfo(A,  "MCA_2D"),"")==0)
		ongraph=0
	endif
	if (cmpstr(csrinfo(B,  "MCA_2D"),"")==0)
		ongraph=0
	endif
	if(ongraph==0)
		return(0)
	endif
	
	p1 = pcsr(A, "MCA_2D")
	p2 = pcsr(B, "MCA_2D")
	
	if(p1>p2)
		p3 = p1
		p1 = p2
		p2 = p3
	endif
	
	//print "points:", p1,p2
	Analyze_sumROIpts(p1,p2)
End


Function Analyze_sumROIpts(p1,p2)
Variable p1,p2
	
	Nvar nbins = root:PW:nbins
	Wave  x2D = root:x2D
	Wave  y2D = root:y2D
	Wave  z2D = root:z2D
	
	make/o/n=(nbins,nbins) root:MCA_2Dcount
	Wave  MCA_2Dcount =  root:MCA_2Dcount
	MCA_2Dcount = z2D		// MCA_2D[p][q]: p = x, q = y
	
	make/o/n=(nbins)  root:E1_roi,  root:E2_roi
	Wave E1_roi =  root:E1_roi
	Wave E2_roi =  root:E2_roi
	E1_roi = 0
	E2_roi = 0 
	
	Variable k, j, sumROI
	Variable x1,y1,x2,y2
	
	x1 = mod(p1,nbins)
	x2 = mod(p2,nbins)
	y1 = floor(p1/nbins)
	y2 = floor(p2/nbins)
	
//	print x1,y1,x2,y2
//	print x2D(p1)/x1, y2D(p1)/y1, x2D(p2)/x2, y2D(p2)/y2
	
	sumROI = 0
	for (k=x1;k<=x2;k+=1)
		for(j=y1;j<=y2;j+=1)
			sumROI += MCA_2Dcount[k][j]		
			E1_roi[k] += MCA_2Dcount[k][j]	
			E2_roi[j] += MCA_2Dcount[k][j]	
		endfor
	endfor
	
	setscale/P x,0,16, E1_roi	// PARAMETER 32 is scaling of 2D spectrum
	setscale/P x,0,16, E2_roi
	
	print "Counts in ROI:", sumROI
	return(sumROI)

End

Function ShowROICursors()
		Execute "MCA_2D()"
		ShowInfo
		Cursor/H=1 A y2D 2316
		Cursor/H=1 B y2D 11588
End


Proc CompareDSP2Igor(process)
Variable process		// if 1, reprocess file. if 0, just build ratios and histograms
	PauseUpdate; Silent 1	
//	Nvar CompDSP2Igor = root:PW:CompDSP2Igor
//	Nvar  Nevents = root:PW:Nevents 				// number of events
//	Nvar source = root:PW:source
	
	Variable k,m, len
	
//	Nvar LoQ1 = root:PW:LoQ1 // = 12 // length of sum Q1 and its baseline
//	Nvar LoQ0 = root:PW:LoQ0 // = 12 // length of sum Q0	 and its baseline
//	Nvar SoQ1 = root:PW:SoQ1 // = 0	// starting point of Q1 relative to high or low RT level
//	Nvar SoQ0 =root:PW:SoQ0 //= 24	// starting point of Q0 relative to low RT level

if(process)
	root:PW:CompDSP2Igor = 1	// go into debug mode
	root:PW:source =1			// "Computing PSA data from traces "
	
	PW_file_getPSAdata()	// process file, keeping arrays of online and offline results
  	root:PW:CompDSP2Igor = 0	// exit debug mode
	root:PW:source = 0			// Reading PSA values from file (default)
endif
	 
	make/o/n=( root:PW:nevents) root:PW:DiffQ0, root:PW:DiffQ1,  root:PW:DiffAmpl,  root:PW:DiffBase,  root:PW:DiffRatio

	 // in CompDSP2Igor mode, channel, rt, ratio0, TrigTime and ratio1 contain online result of Q0, Q1, ampl, base, PSAval=Q1/Q0
	//  root:PW:DiffQ0 =     (root:PW:Channel -  root:PW:Q0sm 	) /root:PW:Q0sm 	*100 //Q0(DSP) - Q0(Igor)
	//  root:PW:DiffQ1 =     (root:PW:rt -  root:PW:Q1sm)	 /root:PW:Q1sm 	*100		//Q1(DSP) - Q1(Igor)
	  root:PW:DiffAmpl = (root:PW:ratio0 -  root:PW:Amp)  /root:PW:Amp 	*100	//Ampl(DSP) - Ampl(Igor)
	  root:PW:DiffBase = (root:PW:TrigTime -  root:PW:Bsm) /root:PW:Bsm 	*100 	//base(DSP) - base(Igor)
	  root:PW:DiffRatio = (root:PW:ratio1/1000 -  root:PW:PSAval)  /root:PW:PSAval 	*100	//ratio(DSP) - ratio(Igor)
	 
	 // Q0, Q1 and ratio are strongly affected by error in B, try here to revert for Qs
	 duplicate/o root:PW:Channel, Qdsp
	 duplicate/o root:PW:Q0sm, Qigor
	 duplicate/o root:PW:TrigTime, Bdsp
	 duplicate/o root:PW:Bsm, Bigor
	  Qdsp = Qdsp-(root:PW:LoQ0*Bdsp/4)		// undo baseline subtraction
	  Qigor = Qigor-(root:PW:LoQ0*Bigor/4)
	  root:PW:DiffQ0 =    (root:Qdsp-root:Qigor)/root:Qdsp*100 
	  
	 duplicate/o root:PW:rt, Qdsp
	 duplicate/o root:PW:Q1sm, Qigor
	  Qdsp = Qdsp-(root:PW:LoQ1*Bdsp/4)		// undo baseline subtraction
	  Qigor = Qigor-(root:PW:LoQ1*Bigor/4)
	  root:PW:DiffQ1 =    (root:Qdsp-root:Qigor)/root:Qdsp*100 
	
	 
	 // another computation of ratio by Igor with DSP values
	  duplicate/o root:PW:ratio1, DSPIgorratio
	  DSPIgorratio = root:PW:rt / root:PW:Channel
	  

	 Variable bstart = -100
	 variable bsize = 0.1
	 Variable bnum = 2000
	 make/o/n=1 root:PW:histo_DiffQ0
	 histogram/B={bstart,bsize,bnum} root:PW:DiffQ0,  root:PW:histo_DiffQ0
	 make/o/n=1 root:PW:histo_DiffQ1
	 histogram/B={bstart,bsize,bnum} root:PW:DiffQ1,  root:PW:histo_DiffQ1
	 make/o/n=1 root:PW:histo_DiffAmpl
	 histogram/B={bstart,bsize,bnum} root:PW:DiffAmpl,  root:PW:histo_DiffAmpl
	 make/o/n=1 root:PW:histo_DiffBase
	 histogram/B={bstart,bsize,bnum} root:PW:DiffBase,  root:PW:histo_DiffBase
	 make/o/n=1 root:PW:histo_DiffRatio
	 histogram/B={bstart,bsize,bnum} root:PW:DiffRatio,  root:PW:histo_DiffRatio
	 
	   // histogram the ratios with specific bins
	   bstart = -0.1
	   bsize = 0.004
	   bnum = 1024
	   make/o/n=1 root:PW:histo_Q1Q0_DSP
	   make/o/n=1 root:PW:histo_Q1Q0_Igor
	   make/o/n=1 root:PW:histo_Q1Q0_DSPIgor
	   histogram/B={bstart,bsize,bnum} root:PW:PSAval,   root:PW:histo_Q1Q0_Igor
	   histogram/B={bstart,bsize,bnum} DSPIgorratio,   root:PW:histo_Q1Q0_DSPIgor
	   bstart *=1000
	   bsize *= 1000
	   histogram/B={bstart,bsize,bnum} root:PW:ratio1,   root:PW:histo_Q1Q0_DSP
		 
	 killwaves/Z Qdsp, Qigor, Bdsp, Bigor, DSPIgorratio

End





Window Q1Q0Histograms() : Graph
	PauseUpdate; Silent 1		// building window...
	Display /W=(366.75,125.75,761.25,347) :PW:histo_Q1Q0_Igor
	AppendToGraph/T  :PW:histo_Q1Q0_DSP
	AppendToGraph :PW:histo_Q1Q0_DSPIgor
	ModifyGraph mode(histo_Q1Q0_Igor)=6,mode(histo_Q1Q0_DSP)=6,mode(histo_Q1Q0_DSPIgor)=6
	ModifyGraph rgb(histo_Q1Q0_DSP)=(0,0,65280),rgb(histo_Q1Q0_DSPIgor)=(26368,0,52224)
	ModifyGraph log(left)=1
	SetAxis bottom -0.1,1
	SetAxis top -100,1000
	Cursor/P A histo_Q1Q0_DSP 49;Cursor/P B histo_Q1Q0_DSP 149
	ShowInfo
EndMacro


Window PSADiffHisttograms() : Graph
	PauseUpdate; Silent 1		// building window...
	String fldrSav0= GetDataFolder(1)
	SetDataFolder root:PW:
	Display/K=1 /W=(150.75,140,637.5,520.25) histo_DiffQ0,histo_DiffQ1,histo_DiffAmpl,histo_DiffBase
	AppendToGraph histo_DiffRatio
	SetDataFolder fldrSav0
	ModifyGraph mode=6
	ModifyGraph rgb(histo_DiffQ1)=(8704,8704,8704),rgb(histo_DiffAmpl)=(0,0,65280),rgb(histo_DiffBase)=(0,52224,0)
	ModifyGraph rgb(histo_DiffRatio)=(52224,0,41728)
	ModifyGraph log(left)=1
	ModifyGraph mirror=2
	Label left "N events"
	Label bottom "(Online - Offline)/Offline)*100 (%)"
	Legend/C/N=text0/J/F=0/A=MC/X=32.31/Y=35.12 "\\s(histo_DiffQ0) histo_DiffQ0\r\\s(histo_DiffQ1) histo_DiffQ1\r\\s(histo_DiffAmpl) histo_DiffAmpl"
	AppendText "\\s(histo_DiffBase) histo_DiffBase\r\\s(histo_DiffRatio) histo_DiffRatio"
EndMacro

Function ShowHideAWEcontrols()

	Nvar RunInProgress = root:pixie4:RunInProgress
	
	if(RunInProgress)
		SetVariable RUN_FileBase,disable=2,win = Pixie4MainPanel_AWE
		SetVariable RUN_Number, disable=2,win = Pixie4MainPanel_AWE
		Button cmnLoad, disable=2, win = Pixie4MainPanel_AWE	
		Button cmnSave, disable=2, win = Pixie4MainPanel_AWE	
		Button MainStartRun, disable=2, win = Pixie4MainPanel_AWE	
		Button MainStopRun, disable=0, win = Pixie4MainPanel_AWE			
		popupmenu RUN_WhichRun, disable=2, win = Pixie4MainPanel_AWE	
	//	Button ADCRefresh, disable=2, win = AWEOscilloscope	
	//	Button AdjustDC, disable=2, win = AWEOscilloscope	
	else
		SetVariable RUN_FileBase,disable=0,win = Pixie4MainPanel_AWE
		SetVariable RUN_Number, disable=0,win = Pixie4MainPanel_AWE
		Button cmnLoad, disable=0, win = Pixie4MainPanel_AWE
		Button cmnSave, disable=0, win = Pixie4MainPanel_AWE	
		Button MainStartRun, disable=0, win = Pixie4MainPanel_AWE
		Button MainStopRun, disable=2, win = Pixie4MainPanel_AWE	
		popupmenu RUN_WhichRun, disable=0, win = Pixie4MainPanel_AWE		
	//	Button ADCRefresh, disable=0, win = AWEOscilloscope	
	//	Button AdjustDC, disable=0, win = AWEOscilloscope	
	endif
End


// ****************************************************************************************
// User Functions related to online 2D spectra
// ****************************************************************************************


// Online PSA 2D spectra
// Igor
// new graph with "Imageplot", show chosenchannel
// link to array 4x 256x256 from P4e memory
// controls
// - update from P4e
// - load/save from/to file 
// - export PN csv file (later)
// - show total counts above/below, all 4 channels, User_out C variables 0-7
// save extra MCA to binary file .mca2d during end run routine
// think: scaling

// DSP
// input variables
// - threshold n/g <- UserIn 4x
// - scaling factor x, y <- UserIn 8x 
// output variables
// - counts above TH -> UserOut 8x
// - counts below TH -> UserOut 8x


Function PSA2D_globals()
	make/u/i/o/n=(256,256,4) psa2dz
	make/u/i/o/n=256 psa2dn, psa2dg
	make/u/i/o/n=(1024*256) mca2dwave
	make/o/n=4 countn, countg
	
End

Function PSA2D_UpdateMCAData(mode)
String mode


	Nvar ChosenModule = root:pixie4:ChosenModule
	Svar MCAFileName = root:pixie4:MCAFileName
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Wave mca2dwave
	wave psa2dz
	
	Variable RunType, ret
	String filename = MCAFileName+"2d"
	
	if(cmpstr(mode, "module")==0)	
		RunType=0x9005  // Read MCA data from external memory
		ret = Pixie4_Acquire_Data(RunType,mca2dwave, filename, ChosenModule)
		if(ret < 0)
			DoAlert 0, "Failed to read MCA data from module."
			return(ret)
		endif
		
		// copy to display waves
		psa2dz = mca2dwave		// TOD: check if re-ordering is required
	endif
	
	// file read is implemented in calling function
	//if(cmpstr(mode, "file")==0)	
	//	print "readback from file not yet supported"
	//endif
	
	// read parameters and compute 32bit output values
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Pixie_IO_ReadModParFromMod(NumberOfModules)
	Pixie_IO_SendModParToIgor()
	DoUpdate
	Wave countn
	Wave countg
	wave dmp = root:pixie4:Display_Module_Parameters 
	Nvar index_UserOUT = root:pixie4:index_UserOUT
	Nvar index_UserIn  = root:pixie4:index_UserIn
	countn[0] = dmp[index_UserOUT+ 1] + 2^16*dmp[index_UserOUT+ 0] 
	countn[1] = dmp[index_UserOUT+ 2] + 2^16*dmp[index_UserOUT+ 2] 
	countn[2] = dmp[index_UserOUT+ 5] + 2^16*dmp[index_UserOUT+ 4] 
	countn[3] = dmp[index_UserOUT+ 7] + 2^16*dmp[index_UserOUT+ 6] 
	countg[0] = dmp[index_UserOUT+ 9] + 2^16*dmp[index_UserOUT+ 8] 
	countg[1] = dmp[index_UserOUT+11] + 2^16*dmp[index_UserOUT+10] 
	countg[2] = dmp[index_UserOUT+13] + 2^16*dmp[index_UserOUT+12] 
	countg[3] = dmp[index_UserOUT+15] + 2^16*dmp[index_UserOUT+14] 
	
	
	Wave psa2dn
	Wave psa2dg
	Variable k, j, sumup, sumdown, updownth
	Variable x1,y1,x2,y2, ch, nbins
	ch = ChosenChannel
	nbins = 256
	
	x1 = 0
	x2 = nbins
	y1 = 0
	y2 = nbins
	updownth = dmp[index_UserIn+ch]
	
	psa2dn = 0
	psa2dg = 0
	for (k=x1;k<x2;k+=1)
		for(j=y1;j<y2;j+=1)
			if(j>updownth)	
				psa2dn[k] += psa2dz[k][j][ch]	
			else	
				psa2dg[k] += psa2dz[k][j][ch]		
			endif
		endfor
	endfor	
	DoUpdate
		

End

Function PSA2D_buttons(ctrlName) : ButtonControl
	String ctrlName
	
	wave psa2dz
	Svar MCAFileName = root:pixie4:MCAFileName
	String filename = MCAFileName+"2d"
	variable filenum
	Wave mca2dwave
	
		StrSwitch(ctrlName)
		Case "update":
		
			// load data from P4e
			Pixie_RC_UpdateMCAData("UpdateMCA")
			// PSA2D_UpdateMCAData()
			// now load from 2nd memory block and copy to psa2d globals
			// and compute projections			
			// common subfunction with Update MCA
			
			break
		
		Case "load":
			Open/R/P=MCAPath filenum
			Fbinread/F=3/U filenum, mca2dwave
			close filenum
			psa2dz = mca2dwave
			 PSA2D_UpdateMCAData("") // compute projections only
			// load data from file, then update
			// common subfunction with Update MCA
			break
			
		Case "savecsv":
			print "not yet implemented"
			// save psa2d to csv file (like PN)
			break
			
		Case "savebin":
			mca2dwave = psa2dz
			Open /P=MCAPath filenum as filename
			Fbinwrite/F=3/U filenum, mca2dwave
			close filenum
			break

		Default:
			break
	EndSwitch
	
End


Window PSA2D_plot() : Graph
	DoWindow/F PSA_2D
	if(V_flag!=1)
		PauseUpdate; Silent 1		// building window...
		Display/K=1/W=(0,100,400,500)/L=L1 psa2dn,psa2dg as "2D PSA MCA from DSP"
		ModifyPanel cbRGB=(65280,59904,48896)
		DoWindow/C PSA_2D
		AppendImage/G=1 psa2dz
		ModifyImage psa2dz ctab= {1,*,Spectrum,0}
		ModifyImage psa2dz plane= 1
		ModifyImage psa2dz minRGB=(65535,65535,65535)
		ModifyImage psa2dz log=1
		ModifyGraph mirror(left)=2,mirror(bottom)=2,mirror(L1)=2
		ModifyGraph lblPos(left)=52
		ModifyGraph freePos(L1)={0,bottom}
		ModifyGraph axisEnab(left)={0,0.74}
		ModifyGraph axisEnab(L1)={0.76,1}
		ModifyGraph mode=6,rgb(psa2dg)=(0,0,65535)
		ModifyGraph log(L1)=1
		ShowInfo
		ControlBar 153

		TitleBox title0 title="DSP settings",pos={10,3},frame=0, fstyle=1, fsize=14
		TitleBox title2 title="DSP results",pos={10,68},frame=0, fstyle=1, fsize=14
		Legend/C/N=text0/J/A=LT/X=63.48/Y=4.58 "Projections (offline)\r\\s(psa2dn)  above TH (neutrons)\r\\s(psa2dg)  below TH (gammas)"
		
		Variable ctrly = 130
		Button update,pos={110,ctrly},size={80,20},title="Update", help={"Read from Pixie module"}, proc = PSA2D_buttons
		Button load,pos={400,ctrly},size={50,20},title="Load", help={"Load from file"}, proc = PSA2D_buttons
		Button savebin,pos={470,ctrly},size={50,20},title="Save", help={"Save to file in binary format"}, proc = PSA2D_buttons
		SetVariable setvar0,pos={20,ctrly+2},size={80,16},title="Channel", limits={0,3,1}, fsize=11
		SetVariable setvar0, proc=Pixie_IO_SelectModChan, value=root:pixie4:ChosenChannel
		
		//Variable ncp = root:pixie4:NumChannelPar
	
		ctrly = 12
		SetVariable UserIn04,pos={120,ctrly},size={140,16},title="x scale [2^-n]  0 ",help={"User Input 4"},limits={0,8,1}
		SetVariable UserIn04,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+4], proc=Pixie_IO_ModVarControl
		SetVariable UserIn06,pos={265,ctrly},size={60,16},title="1 ",help={"User Input 6"},limits={0,8,1}
		SetVariable UserIn06,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+6], proc=Pixie_IO_ModVarControl
		SetVariable UserIn08,pos={330,ctrly},size={60,16},title="2 ",help={"User Input 8"},limits={0,8,1}
		SetVariable UserIn08,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+8], proc=Pixie_IO_ModVarControl
		SetVariable UserIn10,pos={395,ctrly},size={60,16},title="3 ",help={"User Input 10"},limits={0,8,1}
		SetVariable UserIn10,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+10], proc=Pixie_IO_ModVarControl
		
		ctrly = 32
		SetVariable UserIn05,pos={120,ctrly},size={140,16},title="y scale [2^-n]  0 ",help={"User Input 5"},limits={0,8,1}
		SetVariable UserIn05,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+5], proc=Pixie_IO_ModVarControl
		SetVariable UserIn07,pos={265,ctrly},size={60,16},title="1 ",help={"User Input 7"},limits={0,8,1}
		SetVariable UserIn07,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+7], proc=Pixie_IO_ModVarControl
		SetVariable UserIn09,pos={330,ctrly},size={60,16},title="2 ",help={"User Input 9"},limits={0,8,1}
		SetVariable UserIn09,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+9], proc=Pixie_IO_ModVarControl
		SetVariable UserIn11,pos={395,ctrly},size={60,16},title="3 ",help={"User Input 11"},limits={0,8,1}
		SetVariable UserIn11,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+11], proc=Pixie_IO_ModVarControl

		ctrly = 52
		SetVariable UserIn00,pos={120,ctrly},size={140,16},title="N/G threshold 0 ",help={"User Input 0"},limits={0,256,1}
		SetVariable UserIn00,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn], proc=Pixie_IO_ModVarControl
		SetVariable UserIn01,pos={265,ctrly},size={60,16},title="1 ",help={"User Input 1"},limits={0,256,1}
		SetVariable UserIn01,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+1], proc=Pixie_IO_ModVarControl
		SetVariable UserIn02,pos={330,ctrly},size={60,16},title="2 ",help={"User Input 2"},limits={0,256,1}
		SetVariable UserIn02,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+2], proc=Pixie_IO_ModVarControl
		SetVariable UserIn03,pos={395,ctrly},size={60,16},title="3 ",help={"User Input 3"},limits={0,256,1}
		SetVariable UserIn03,fSize=12,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+3], proc=Pixie_IO_ModVarControl
		TitleBox title1 title="<256",pos={460,58},frame=0, fsize=11
		//etc, max 15 inputs UserIn08 .. UserIn15

		
		
		ctrly = 85
		SetVariable dis0,format="%4.5g",limits={0,Inf,0},value= root:countn[0]
		SetVariable dis0, pos={20,ctrly},size={180,14},title="Above TH Total 0", noedit=1,fSize=12
		SetVariable dis1,format="%4.5g",limits={0,Inf,0},value= root:countg[0]
		SetVariable dis1, pos={20,ctrly+20},size={180,14},title="Below  TH Total 0", noedit=1,fSize=12
		SetVariable dis2,format="%4.5g",limits={0,Inf,0},value= root:countn[1]
		SetVariable dis2, pos={210,ctrly},size={100,14},title=" 1", noedit=1,fSize=12
		SetVariable dis3,format="%4.5g",limits={0,Inf,0},value= root:countg[1]
		SetVariable dis3, pos={210,ctrly+20},size={100,14},title=" 1", noedit=1,fSize=12
		SetVariable dis4,format="%4.5g",limits={0,Inf,0},value=root:countn[2]
		SetVariable dis4, pos={320,ctrly},size={100,14},title=" 2", noedit=1,fSize=12
		SetVariable dis5,format="%4.5g",limits={0,Inf,0},value=  root:countg[2]
		SetVariable dis5, pos={320,ctrly+20},size={100,14},title=" 2", noedit=1,fSize=12
		SetVariable dis6,format="%4.5g",limits={0,Inf,0},value= root:countn[3]
		SetVariable dis6, pos={430,ctrly},size={100,14},title=" 3", noedit=1,fSize=12
		SetVariable dis7,format="%4.5g",limits={0,Inf,0},value= root:countg[3]
		SetVariable dis7, pos={430,ctrly+20},size={100,14},title=" 3", noedit=1,fSize=12

		
		Label left "PSA Value"
		Label bottom "Energy"
		Label L1 "N counts"
		ModifyGraph lblPosMode(L1)=2
	endif
EndMacro

