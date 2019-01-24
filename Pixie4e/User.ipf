#pragma rtGlobals=1		// Use modern global access method.

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

	Variable/G root:user:UserVariant = 0x0000	// the user code variant (= application). variant numbers 0x0000-0x7FFF reserved for code written by XIA 
	// Variant 0x0000 = generic
	// Variant 0x0100 = Xe PW
	// Variant 0x0110 = Xe SAUNA
	// Variant 0x0200 = MPI
	// Variant 0x0300 = gamma neutron
	// Variant 0x0400 = general PSA
	
	Variable/G root:user:UserCode= 0x0007// the user code version = updates and modifications of code for the same application
	String/G root:user:UserTitle = "User"	// This string may be used in a number of panels and displays. Replace for a particular application
	
	// add additional user global variables below
End

Function User_StartRun()
	//called at end of Pixie_StartRun (executed at beginning of run) for runs with polling time>0
	//Use to prepare runs	
End

Function User_NewFileDuringRun(Runtype)
String Runtype	// "List" for list mode runs, "MCA" for MCA runs

	//called from Pixie_CheckRun (executed every polling period) in those occasions 
	//when Igor automatically saves files and increments run file number after N spills or seconds. 
	// called after data is saved to current files, before new files are made and run resumes
	
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
End

Function User_UpdateMCA()	
	//called from Pixie_UpdateMCAData, i.e. after updating spectra (and in most cases, run statistics)
	
	// in principle one could call the duplication routine to copy MCA data to the user copies in root:results
	// but this may take time and possibly claim more memory every time. Not recommended	
	//User_DuplicateResults()	
	
	
End

Function User_StopRun()
	//called at end of Pixie_StopRun (executed at end of run)
	//Use to process output data

	User_DuplicateResults() //copy output data into root:results data folder
	
	// add custom code below
	
End


Function User_ChangeChannelModule()
	//called when changing Module Number or Channel Number

	User_GetDSPValues("")	//read data back from DSP of current module
	
	//add custom code below
	
End

Function User_ReadEvent()
	//called when changing event number in list mode trace display or digital filter display
	
	if(0)	// only execute when necessary	
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
	
End

Function User_TraceDataFile()
	//called when changing file name in list mode trace display
	
	//add custom code below
	
End

Macro User_HomePaths()
	// called from macro UseHomePaths to set default path and filenames
	
	// customize code below
	
//	// Communication FPGA files
// root:pixie4:FPGAFile4e = root:pixie4:FirmwareFileDirectory + "P4e_16_125_vdo.bin"
// root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "P4e_14_500_vdo.bin"
// root:pixie4:CommFPGAFile4 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
// root:pixie4:CommFPGAFile500 = root:pixie4:FirmwareFileDirectory + "syspixie_RevC.bin"
//	
//	// Signal processing FPGA files	
//	root:pixie4:SPFPGAFile4 = root:pixie4:FirmwareFileDirectory + "pixie.bin"
//	root:pixie4:SPFPGAFile500 = root:pixie4:FirmwareFileDirectory + "FippiP500.bin"
//	root:pixie4:FPGAFile500e = root:pixie4:FirmwareFileDirectory + "p500e_zdt.bin"
//	
//	// DSP I/O variable values file
//	root:pixie4:DSPParaValueFile = root:pixie4:ConfigFileDirectory + "default_125.set"
//	
//	// DSP code files
//	root:pixie4:DSPCodeFile4 = root:pixie4:DSPFileDirectory + "PXIcode.bin"
//	root:pixie4:DSPCodeFile500 = root:pixie4:DSPFileDirectory + "P500code.bin"
//	root:pixie4:DSPCodeFile500e = root:pixie4:DSPFileDirectory + "P500e.ldr"			// for both P4e and P500e
//		
//	// DSP I/O variable names file
//	root:pixie4:DSPVarFile4 = root:pixie4:DSPFileDirectory + "P500e.var"				// P4 has same var list, but the map2var produces only 416 names, so only use P4e/500e var file
//	
//	// DSP all variable names file
//	root:pixie4:DSPListFile4 = root:pixie4:DSPFileDirectory + "PXIcode.lst"
//	root:pixie4:DSPListFile500e = root:pixie4:DSPFileDirectory + "P500e.lst"
	
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
		NewPanel /K=1/W=(446,197,1000,556) 
		ModifyPanel cbRGB=(65280,59904,48896)
		DoWindow/C User_Control
		SetDrawLayer UserBack
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 5,25,"DSP Input Parameters"
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 200,26,"Options"
		SetDrawEnv fsize= 14,fstyle= 1
		DrawText 400,25,"DSP Output Values"
		
//		SetVariable ModCSRB,pos={7,32},size={120,16},title="ModCSRB",help={"Module CSRB - set to 1 to enable User code"}
//		SetVariable ModCSRB,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ModCSRB")], proc=User_VariableIO
//		SetVariable CCSRB0,pos={7,52},size={120,16},title="CCSRB0   ",help={"Channel 0 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB0,fSize=10,format="%X",value=root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB0")], proc=User_VariableIO
//		SetVariable CCSRB1,pos={7,72},size={120,16},title="CCSRB1   ",help={"Channel 1 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB1,fSize=10,format="%X",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB1")], proc=User_VariableIO
//		SetVariable CCSRB2,pos={7,92},size={120,16},title="CCSRB2   ",help={"Channel 2 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB2,fSize=10,format="%X",value=root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB2")], proc=User_VariableIO
//		SetVariable CCSRB3,pos={7,112},size={120,16},title="CCSRB3   ",help={"Channel 3 CSRB - set to 1 to enable User code"}
//		SetVariable CCSRB3,fSize=10,format="%x",value= root:pixie4:DSPValues[Pixie_Find_DSPname("ChanCSRB3")], proc=User_VariableIO
		
		// better: using module/channel parameters from C library
		Variable ncp =  root:pixie4:NumChannelPar
		SetVariable ADV_ModCSRB,pos={7,32},size={120,16},title="ModCSRB",help={"Module CSRB - set bit 0 to enable User code"}
		SetVariable ADV_ModCSRB,fSize=10,format="%X",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_MCSRB], proc=Pixie_IO_ModVarControl
		SetVariable ADV_CHANNEL_CSRB0,pos={7,52},size={120,16},title="CCSRB0   ",help={"Channel 0 CSRB - set bit 0 to enable User code"}
		SetVariable ADV_CHANNEL_CSRB0,fSize=10,format="%X",value=root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+0*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB1,pos={7,72},size={120,16},title="CCSRB1   ",help={"Channel 1 CSRB - set bit 0 to enable User code"}
		SetVariable ADV_CHANNEL_CSRB1,fSize=10,format="%X",value= root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+1*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB2,pos={7,92},size={120,16},title="CCSRB2   ",help={"Channel 2 CSRB - set bit 0 to enable User code"}
		SetVariable ADV_CHANNEL_CSRB2,fSize=10,format="%X",value=root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+2*ncp], proc=Pixie_IO_ChanVarControl
		SetVariable ADV_CHANNEL_CSRB3,pos={7,112},size={120,16},title="CCSRB3   ",help={"Channel 3 CSRB - set bit 0 to enable User code"}
		SetVariable ADV_CHANNEL_CSRB3,fSize=10,format="%x",value= root:pixie4:Display_Channel_Parameters[root:pixie4:index_CCSRB+3*ncp], proc=Pixie_IO_ChanVarControl
		

//		SetVariable UserIn0,pos={10,142},size={120,16},title="User Input 0 ",help={"User Input 0"}
//		SetVariable UserIn0,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")], proc=User_VariableIO
//		SetVariable UserIn1,pos={10,160},size={120,16},title="User Input 1 ",help={"User Input 1"}
//		SetVariable UserIn1,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+1], proc=User_VariableIO
//		SetVariable UserIn2,pos={10,182},size={120,16},title="User Input 2 ",help={"User Input 2"}
//		SetVariable UserIn2,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+2], proc=User_VariableIO
//		SetVariable UserIn3,pos={10,200},size={120,16},title="User Input 3 ",help={"User Input 3"}
//		SetVariable UserIn3,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+3], proc=User_VariableIO
//		SetVariable UserIn4,pos={10,222},size={120,16},title="User Input 4 ",help={"User Input 4"}
//		SetVariable UserIn4,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+4], proc=User_VariableIO
//		SetVariable UserIn5,pos={10,240},size={120,16},title="User Input 5 ",help={"User Input 5"}
//		SetVariable UserIn5,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+5], proc=User_VariableIO
//		SetVariable UserIn6,pos={10,262},size={120,16},title="User Input 6 ",help={"User Input 6"}
//		SetVariable UserIn6,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+6], proc=User_VariableIO
//		SetVariable UserIn7,pos={10,280},size={120,16},title="User Input 7 ",help={"User Input 7"}
//		SetVariable UserIn7,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserIn")+7], proc=User_VariableIO

		// better: using module parameters from C library
				// change name and appareance, but not the index for DSPValues
		SetVariable UserIn00,pos={10,142},size={120,16},title="User Input 0 ",help={"User Input 0"}
		SetVariable UserIn00,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn], proc=Pixie_IO_ModVarControl
		SetVariable UserIn01,pos={10,160},size={120,16},title="User Input 1 ",help={"User Input 1"}
		SetVariable UserIn01,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+1], proc=Pixie_IO_ModVarControl
		SetVariable UserIn02,pos={10,182},size={120,16},title="User Input 2 ",help={"User Input 2"}
		SetVariable UserIn02,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+2], proc=Pixie_IO_ModVarControl
		SetVariable UserIn03,pos={10,200},size={120,16},title="User Input 3 ",help={"User Input 3"}
		SetVariable UserIn03,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+3], proc=Pixie_IO_ModVarControl
		SetVariable UserIn04,pos={10,222},size={120,16},title="User Input 4 ",help={"User Input 4"}
		SetVariable UserIn04,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+4], proc=Pixie_IO_ModVarControl
		SetVariable UserIn05,pos={10,240},size={120,16},title="User Input 5 ",help={"User Input 5"}
		SetVariable UserIn05,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+5], proc=Pixie_IO_ModVarControl
		SetVariable UserIn06,pos={10,262},size={120,16},title="User Input 6 ",help={"User Input 6"}
		SetVariable UserIn06,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+6], proc=Pixie_IO_ModVarControl
		SetVariable UserIn07,pos={10,280},size={120,16},title="User Input 7 ",help={"User Input 7"}
		SetVariable UserIn07,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserIn+7], proc=Pixie_IO_ModVarControl
		//etc, max 15 inputs UserIn08 .. UserIn15
		
//		ValDisplay UserOut0,pos={405,142},size={120,16},title="UserOut[0] ",help={"User Output 0"}
//		ValDisplay UserOut0,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")]
//		ValDisplay UserOut1,pos={405,162},size={120,16},title="UserOut[1] ",help={"User Output 1"}
//		ValDisplay UserOut1,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+1]
//		ValDisplay UserOut2,pos={405,182},size={120,16},title="UserOut[2] ",help={"User Output 2"}
//		ValDisplay UserOut2,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+2]
//		ValDisplay UserOut3,pos={405,202},size={120,16},title="UserOut[3] ",help={"User Output 3"}
//		ValDisplay UserOut3,fSize=10,format="%g",value= root:pixie4:DSPValues[Pixie_Find_DSPname("UserOut")+3]	
		// better: using module parameters from C library
		ValDisplay UserOut0,pos={405,142},size={120,16},title="UserOut[0] ",help={"User Output 0"}
		ValDisplay UserOut0,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT]
		ValDisplay UserOut1,pos={405,162},size={120,16},title="UserOut[1] ",help={"User Output 1"}
		ValDisplay UserOut1,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+1]
		ValDisplay UserOut2,pos={405,182},size={120,16},title="UserOut[2] ",help={"User Output 2"}
		ValDisplay UserOut2,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+2]
		ValDisplay UserOut3,pos={405,202},size={120,16},title="UserOut[3] ",help={"User Output 3"}
		ValDisplay UserOut3,fSize=10,format="%g",value= root:pixie4:Display_Module_Parameters[root:pixie4:index_UserOUT+3]	
		//etc, max 15 outputs
		
		Checkbox EnableUserCode, pos = {207,32}, size={60,20},proc=User_CheckBoxControl,title="Enable User DSP code"
		Checkbox EnableUserCode, fsize=11,font="Arial",help={"Sets bits in Module CSRB and Channel CSRB to switch on user code in DSP"}
		
	// 	no longer needed when using Module Parameterd from C library
	//	Button Apply,pos={20,310},size={90,25},proc=Pixie_EX_CallPutModPar,title="Apply to DSP",fsize=11
	//	Button Refresh,pos={130,310},size={90,25},proc=User_GetDSPValues,title="Read from DSP",fsize=11
		Button About,pos={240,310},size={90,25},proc=User_Version,title="Version",fsize=11
		Button User_ControlClose,pos={350,310},size={90,25},title="Close",proc=Pixie_AnyPanelClose,fsize=11
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
		ModifyPanel cbRGB=(65280,59904,48896)
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
	
	StrSwitch(ctrlName)
		Case "EnableUserCode":	
			if (checked)
				index = Pixie_Find_DSPname("MODCSRB")
				DSPValues[index] = setbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB0")
				DSPValues[index] = setbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB1")
				DSPValues[index] = setbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB2")
				DSPValues[index] = setbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB3")
				DSPValues[index] = setbit(0,DSPValues[index])
			else
				index = Pixie_Find_DSPname("MODCSRB")
				DSPValues[index] = clrbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB0")
				DSPValues[index] = clrbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB1")
				DSPValues[index] = clrbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB2")
				DSPValues[index] = clrbit(0,DSPValues[index])
				index = Pixie_Find_DSPname("CHANCSRB3")
				DSPValues[index] = clrbit(0,DSPValues[index])
			endif
			break	
					
		Default:
					
	EndSwitch
	
	User_VariableIO("",0,"","") 

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
		
		//make/o/u/i/n=(len*2*NumberOfChannels) root:pixie4:EventPSAValues //XXX
	endif
	
End
