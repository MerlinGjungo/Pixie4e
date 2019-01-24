#pragma rtGlobals=1		// Use modern global access method.

Menu "&XIA"
	"Pixie Panel",Pixie_Panel_Main()
	"&Expert Panel/F8",Pixie_EX_ExpertPanel()
	"&User Panel",User_Control()
	"-"
	"&Parameter Setup/F2",Pixie_Panel_Parameters(0)
	"&Oscilloscope/F3",Pixie_Plot_Oscilloscope()
	"&Chassis Setup",Pixie_Panel_ChassisSettings()
	"&Files,Paths/F5",Pixie_Panel_AllFiles()
	"-"
	"&MCA Spectrum/F9",Pixie_Plot_MCADisplay()
	"&List Mode Traces/F10",Pixie_Plot_LMTracesDisplay()
	"List Mode Spectrum/F11",Pixie_Plot_LMSpectrumDisplay()
	"&Run Statistics/F12",Pixie_Panel_SystemRunStats()
	"-"
	"&About Pixie Viewer", Pixie_Panel_About("")
	
End


//########################################################################
//
//	Pixie_Panel_StartUp:
//		Specifiy totoal number of modules
//		Specify the slot number, serial number and filter range for each module
//		Specify the controller type
//
//########################################################################
Window Pixie_Panel_StartUp() : Panel
	
	// Check if this panel has already been opened
	DoWindow/F Pixie_Panel_StartUp
	if(V_Flag != 1)

		// Create the new panel
		NewPanel/K=1/W=(150, 250, 600, 600) as "Pixie4 Start Up Panel"
		DoWindow/C Pixie_Panel_StartUp
		ModifyPanel fixedSize=1
		ModifyPanel cbRGB=(51456,44032,58880)
		
		// Draw the title
		SetDrawEnv/W=Pixie_Panel_StartUp fillfgc= (32768, 65280, 65280)
		DrawRect 25, 5, 425, 45
		SetDrawEnv/W=Pixie_Panel_StartUp fsize=20
		DrawText 75, 37, "Pixie Viewer"
		
		PopupMenu ChoosePXIChasisType, mode=Pixie_GetPXIChasisType(), pos={75, 65}, proc=Pixie_Ctrl_CommonPopup
		PopupMenu ChoosePXIChasisType,title="Select PXI chassis type",bodywidth=200,size={275,19},fsize=12//,font="Arial"
		PopupMenu ChoosePXIChasisType, value="4, 5, 6 or 8 slot chassis;NI 14 or 18-slot chassis (3U);XIA 14-slot chassis (6U);8-slot PXIe-1062"
		
		GroupBox ModuleNumberBox pos={25, 105}, fsize=12, size={400, 170}, title="Please specify the modules",fcolor=(1,1,1)
		
		// Specify the total number of modules
		SetVariable NumberOfPixie4Modules pos={15, 130}, size={250,20},bodywidth=50,title="Number of Pixie4 Modules"
		SetVariable NumberOfPixie4Modules limits={1,root:pixie4:MaxNumModules,1}, fsize=12, value=root:pixie4:NumberOfModules
		SetVariable NumberOfPixie4Modules proc=Pixie_Ctrl_CommonSetVariable
		
		// Initialize Module List Data
		Pixie_MakeList_Modules()
		
		// Create the Module List Box
		ListBox ModuleBox pos={40,155},size={370,100},frame=2,listWave=root:pixie4:ModuleListData
		ListBox ModuleBox widths={100,100,100},selwave=root:pixie4:ModuleSListData,fsize=12
		ListBox ModuleBox mode=0,proc=Pixie_ListProc_Modules
	
		Button OfflineAnalysis,pos={20,285},size={100,55},title="Offline Analysis",proc=Pixie_Ctrl_CommonButton
		Button StartUpSystemNow,pos={185,285},size={100,55},title="Start Up System",proc=Pixie_Ctrl_CommonButton
		Button HelpInitial_Startup,pos={350,285},size={75,55},title="Help",proc=Pixie_CallHelp
		//Button RebootDSP,pos={20,315},size={100,25},proc=Pixie_Boot1C,title="Reboot DSP",fsize=11
		
	endif
	
EndMacro




//########################################################################
//
//	Pixie_Panel_AllFiles:
//		Specifiy boot file names and paths
//		Specify list mode or MCA data file names or paths
//
//########################################################################
Window Pixie_Panel_AllFiles() : Panel

	PauseUpdate; Silent 1		// building window...
	
	DoWindow/F AllFilesPanel
	if (V_Flag!=1)

		NewPanel/K=1 /W=(150,110,755,650) as "All Files Panel"
		DoWindow/C AllFilesPanel
		ModifyPanel cbRGB=(51456,44032,58880)
		ModifyPanel fixedSize=1
		SetDrawLayer UserBack
		SetDrawEnv fsize= 16,fstyle= 1
		DrawText 200,36,"File Names and Paths"
		SetDrawEnv linethick= 2,linefgc= (65280,32768,58880)
	
		Variable bw = 350
		Variable dy  = 22
		Variable by = 60
		Variable sx=492
		Variable px =40
		GroupBox FirmwareGroup pos={10, 40}, fsize=12, fstyle=1, size={585,130}, title="Firmware",fcolor=(1,1,1)
	
	//	SetVariable FirmwareDirectory,pos={30,60},size={500,18},title="Firmware Directory",noedit=0,proc=Pixie_File_PathDialog
	//	SetVariable FirmwareDirectory,limits={-Inf,Inf,0},value= root:pixie4:FirmwareFileDirectory,font=Arial,fsize=10,bodywidth=bw
			
		SetVariable CommFPGAFile4,pos={px,by},size={sx,18},title="Comm. FPGA Pixie-4"
		SetVariable CommFPGAFile4,limits={-Inf,Inf,0},value= root:pixie4:CommFPGAFile4,proc=Pixie_File_PathDialog
		SetVariable CommFPGAFile4,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetCommFPGAFile4, pos={535,by},size={50,18},title="Find",proc=Pixie_File_Dialog
		
//		SetVariable CommFPGAFile500,pos={px,by+dy},size={sx,18},title="Comm. FPGA Pixie-500"
//		SetVariable CommFPGAFile500,limits={-Inf,Inf,0},value= root:pixie4:CommFPGAFile500,proc=Pixie_File_PathDialog
//		SetVariable CommFPGAFile500,font=Arial,fsize=10,bodywidth=bw,noedit=0
//		Button SetCommFPGAFile500, pos={535,by+dy},size={50,18},title="Find",proc=Pixie_File_Dialog
		
		SetVariable SPFPGAFile4,pos={px,by+1*dy},size={sx,18},title="Sig. Proc. FPGA Pixie-4"
		SetVariable SPFPGAFile4,limits={-Inf,Inf,0},value= root:pixie4:SPFPGAFile4,proc=Pixie_File_PathDialog
		SetVariable SPFPGAFile4,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetSPFPGAFile4, pos={535,by+1*dy},size={50,18},title="Find",proc=Pixie_File_Dialog
		
		SetVariable FPGAFile4e,pos={px,by+2*dy},size={sx,18},title="FPGA Pixie-4e (16/125)"
		SetVariable FPGAFile4e,limits={-Inf,Inf,0},value= root:pixie4:FPGAFile4e,proc=Pixie_File_PathDialog
		SetVariable FPGAFile4e,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetFPGAFile4e, pos={535,by+2*dy},size={50,18},title="Find",proc=Pixie_File_Dialog
		
		SetVariable SPFPGAFile500,pos={px,by+3*dy},size={sx,18},title="FPGA Pixie-4e (14/500)" //"Sig. Proc. FPGA Pixie-500"
		SetVariable SPFPGAFile500,limits={-Inf,Inf,0},value= root:pixie4:SPFPGAFile500,proc=Pixie_File_PathDialog
		SetVariable SPFPGAFile500,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetSPFPGAFile500, pos={535,by+3*dy},size={50,18},title="Find",proc=Pixie_File_Dialog
	
		SetVariable FPGAFile500e,pos={px,by+4*dy},size={sx,18},title="Combined FPGA Pixie-500e"
		SetVariable FPGAFile500e,limits={-Inf,Inf,0},value= root:pixie4:FPGAFile500e,proc=Pixie_File_PathDialog
		SetVariable FPGAFile500e,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetFPGAFile500e, pos={535,by+4*dy},size={50,18},title="Find",proc=Pixie_File_Dialog
	
		by = by+6*dy
		GroupBox DSPGroup pos={10, by}, fsize=12, fstyle=1, size={585,140}, title="DSP",fcolor=(1,1,1)
		
		SetVariable DSPCodeFile4,pos={px,by+dy},size={sx,18},title="DSP Code Pixie-4 (.bin)"
		SetVariable DSPCodeFile4,limits={-Inf,Inf,1},value= root:pixie4:DSPCodeFile4,proc=Pixie_File_PathDialog
		SetVariable DSPCodeFile4,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetDSPCodeFile4, pos={535,by+dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
		
//		SetVariable DSPCodeFile500,pos={px,by+2*dy},size={sx,18},title="DSP Code Pixie-500 (.bin)"
//		SetVariable DSPCodeFile500,limits={-Inf,Inf,1},value= root:pixie4:DSPCodeFile500,proc=Pixie_File_PathDialog
//		SetVariable DSPCodeFile500,font=Arial,fsize=10,bodywidth=bw,noedit=0
//		Button SetDSPCodeFile500, pos={535,by+2*dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
		
		SetVariable DSPCodeFile500e,pos={px,by+2*dy},size={sx,18},title="DSP Code Pixie-4e/500e (.ldr)"
		SetVariable DSPCodeFile500e,limits={-Inf,Inf,1},value= root:pixie4:DSPCodeFile500e,proc=Pixie_File_PathDialog
		SetVariable DSPCodeFile500e,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetDSPCodeFile500e, pos={535,by+2*dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
			
		SetVariable DSPVarFile4,pos={px,by+3*dy},size={sx,18},title="DSP I/O Parameter Names (.var)",noedit=0,proc=Pixie_File_PathDialog
		SetVariable DSPVarFile4,limits={-Inf,Inf,1},value=  root:pixie4:DSPVarFile4,font=Arial,fsize=10,bodywidth=bw
		Button SetDSPVarFile4, pos={535,by+3*dy},size={50,18},title="Find",proc=Pixie_File_Dialog		
		
		SetVariable DSPListFile4,pos={px,by+4*dy},size={sx,18},title="Memory Names Pixie-4 (.lst)",noedit=0,proc=Pixie_File_PathDialog
		SetVariable DSPListFile4,limits={-Inf,Inf,1},value= root:pixie4:DSPListFile4,font=Arial,fsize=10,bodywidth=bw
		Button SetDSPListFile4, pos={535,by+4*dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
		
		SetVariable DSPListFile500e,pos={px,by+5*dy},size={sx,18},title="Memory Names Pixie-4e/500e (.lst)",noedit=0,proc=Pixie_File_PathDialog
		SetVariable DSPListFile500e,limits={-Inf,Inf,1},value= root:pixie4:DSPListFile500e,font=Arial,fsize=10,bodywidth=bw
		Button SetDSPListFile500e, pos={535,by+5*dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
	
		by = by+7*dy
		GroupBox SettingsGroup pos={10, by}, fsize=12, fstyle=1, size={585,45}, title="Settings File",fcolor=(1,1,1)
		
		SetVariable DSPParaValueFile,pos={px,by+dy},size={sx,18},title="DSP Parameter Values File"
		SetVariable DSPParaValueFile,limits={-Inf,Inf,1},value= root:pixie4:DSPParaValueFile,proc=Pixie_File_PathDialog
		SetVariable DSPParaValueFile,font=Arial,fsize=10,bodywidth=bw,noedit=0
		Button SetDSPParaValueFile, pos={535,by+dy},size={50,18},title="Find",proc=Pixie_File_Dialog		
	
		by = by+3*dy
		GroupBox OutputGroup pos={10, by}, fsize=12, fstyle=1, size={585,70}, title="Output File Directories",fcolor=(1,1,1)
		
		SetVariable MCASpecDirectory,pos={px,by+dy},size={sx,18},title="MCA Spectrum Directory",noedit=0,proc=Pixie_File_PathDialog
		SetVariable MCASpecDirectory,limits={-Inf,Inf,1},value= root:pixie4:MCAFileDirectory,font=Arial,fsize=10,bodywidth=bw
		Button SetMCASpecDirectory, pos={535,by+dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
	
		SetVariable PulseShapeDirectory,pos={px,by+2*dy},size={sx,18},title="Pulse Shape Directory",noedit=0,proc=Pixie_File_PathDialog
		SetVariable PulseShapeDirectory,limits={-Inf,Inf,1},value= root:pixie4:EventFileDirectory,font=Arial,fsize=10,bodywidth=bw
		Button SetPulseShapeDirectory, pos={535,by+2*dy},size={50,18},title="Find",proc=Pixie_File_Dialog	
		
		by = by+4*dy-10
		Button CallUseHomePath,pos={px,by},size={90,40},proc=Pixie_AnyPanelClose,title="Home Paths",proc=Pixie_Ctrl_CommonButton
	//	Button RebootDSP,pos={px+100,by},size={90,40},proc=Pixie_Boot1C,title="Reboot DSP",fsize=11
		Button StartUpSystemNow,pos={px+150,by},size={110,40},title="Start Up System",proc=Pixie_Ctrl_CommonButton,fsize=11, fstyle=1
		Button OfflineAnalysis,pos={px+320,by},size={90,40},title="Offline Analysis",proc=Pixie_Ctrl_CommonButton

		Button HelpAllFilesPanel,pos={px+460,by},size={60,40},proc=Pixie_CallHelp,title="Help"
	
	endif
	
EndMacro





//########################################################################
//
//	Pixie_Panel_CopySettings:
//		Set settings copy parameters
//
//########################################################################
Window Pixie_Panel_CopySettings() : Panel

	DoWindow/F CopySettingsPanel
	if (V_Flag!=1)

		NewPanel/K=1/W=(150,50,570,385) as "Copy Settings Panel"
		DoWindow/C CopySettingsPanel
	
		ModifyPanel cbRGB=(51456,44032,58880)
		ModifyPanel fixedSize=1
		SetDrawLayer UserBack
		SetDrawEnv fsize= 16,fstyle= 1
		
		SetVariable SettingsSourcModule,pos={25,15},size={120,18},title="Source Module"
		SetVariable SettingsSourcModule,limits={0,root:pixie4:NumberofModules-1,1},value= root:pixie4:CopySettingsSourceMod
		SetVariable SettingsSourcModule,font=Arial,fsize=10,bodywidth=40
		
		SetVariable SettingsSourcChannel,pos={180,15},size={125,18},title="Source Channel"
		SetVariable SettingsSourcChannel,limits={0,root:pixie4:NumberofChannels-1,1},value= root:pixie4:CopySettingsSourceChan
		SetVariable SettingsSourcChannel,font=Arial,fsize=10,bodywidth=40
		
		SetDrawLayer UserBack
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 20,55,"Items to copy"
		
		Pixie_MakeList_CopyItems()
		
		ListBox CopyItemsListBox,pos={20,60},size={120,213},frame=2,listWave=root:pixie4:CopyItemsList
		ListBox CopyItemsListBox,widths= {65},selwave=root:pixie4:CopyItemsListData,fsize=10,mode=3
		ListBox CopyItemsListBox,colorWave=root:pixie4:CopyItemsColorWave,font="arial",proc=Pixie_ListProc_CopyItems	
		
		SetDrawLayer UserBack
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 175,55,"Destination Modules and Channels"
		
		root:pixie4:CopySettingsModChanData = 0x20
		root:pixie4:CopySettingsDataWave = 0
		Pixie_MakeList_CopySettings()
		
		ListBox DestModChanListBox,pos={175,60},size={225,213},frame=2,listWave=root:pixie4:CopySettingsModChan
		ListBox DestModChanListBox,widths= {50,30,30,30,30},selwave=root:pixie4:CopySettingsModChanData,fsize=10,mode=8
		ListBox DestModChanListBox,colorWave=root:pixie4:CopyItemsColorWave,font="arial",proc=Pixie_ListProc_Copy		
		
		Button CopySettings,pos={250,285},size={60,40},title="Copy",proc=Pixie_Ctrl_CommonButton
		Button CopySettingsPanelClose,pos={325,285},size={60,40},title="Close",proc=Pixie_AnyPanelClose
		
	endif
	
EndMacro


//########################################################################
//
//	Pixie_Panel_ExtractSettings:
//		Set settings extract parameters
//
//########################################################################
Window Pixie_Panel_ExtractSettings() : Panel

	DoWindow/F ExtractSettingsPanel
	if (V_Flag!=1)

		NewPanel/K=1/W=(150,50,570,400) as "Extract Settings Panel"
		DoWindow/C ExtractSettingsPanel
	
		ModifyPanel cbRGB=(51456,44032,58880)
		ModifyPanel fixedSize=1
		SetDrawLayer UserBack
		SetDrawEnv fsize= 16,fstyle= 1
	
		SetVariable SelectSettingsFile,pos={20,10},size={320,18},title="Settings File"
		SetVariable SelectSettingsFile,limits={-Inf,Inf,1},value= root:pixie4:ExtractSettingsFile
		SetVariable SelectSettingsFile,font=Arial,fsize=10,bodywidth=250,proc=Pixie_File_PathDialog
	
		Button FindSettingsFile, pos={350,10},size={50,18},title="Find",proc=Pixie_File_Dialog
	
		SetVariable SettingsSourcModule,pos={25,35},size={120,18},title="Source Module"
		SetVariable SettingsSourcModule,limits={0,root:pixie4:NumberofModules-1,1},value= root:pixie4:CopySettingsSourceMod
		SetVariable SettingsSourcModule,font=Arial,fsize=10,bodywidth=40
		
		SetVariable SettingsSourcChannel,pos={180,35},size={125,18},title="Source Channel"
		SetVariable SettingsSourcChannel,limits={0,root:pixie4:NumberofChannels-1,1},value= root:pixie4:CopySettingsSourceChan
		SetVariable SettingsSourcChannel,font=Arial,fsize=10,bodywidth=40
		
		SetDrawLayer UserBack
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 20,75,"Items to extract"
		
		Pixie_MakeList_CopyItems()
		
		ListBox CopyItemsListBox,pos={20,85},size={120,185},frame=2,listWave=root:pixie4:CopyItemsList
		ListBox CopyItemsListBox,widths= {65},selwave=root:pixie4:CopyItemsListData,fsize=10,mode=3
		ListBox CopyItemsListBox,colorWave=root:pixie4:CopyItemsColorWave,font="arial",proc=Pixie_ListProc_CopyItems	
		
		SetDrawLayer UserBack
		SetDrawEnv fsize= 12,fstyle= 1
		DrawText 175,75,"Extract to Modules and Channels"
		
		root:pixie4:CopySettingsModChanData = 0x20
		root:pixie4:CopySettingsDataWave = 0
		Pixie_MakeList_CopySettings()
		
		ListBox DestModChanListBox,pos={175,85},size={225,185},frame=2,listWave=root:pixie4:CopySettingsModChan
		ListBox DestModChanListBox,widths= {50,30,30,30,30},selwave=root:pixie4:CopySettingsModChanData,fsize=10,mode=8
		ListBox DestModChanListBox,colorWave=root:pixie4:CopyItemsColorWave,font="arial",proc=Pixie_ListProc_Copy		
		
		Button ExtractSettings,pos={250,300},size={60,40},title="Extract",proc=Pixie_Ctrl_CommonButton
		Button ExtractSettingsPanelClose,pos={325,300},size={60,40},title="Close",proc=Pixie_AnyPanelClose
		
	endif
	
EndMacro


//########################################################################
//
//	Pixie_Panel_DataRecOption:
//		Set list mode or MCA data recording options
//
//########################################################################
Window Pixie_Panel_DataRecOption() : Panel

	DoWindow/F DataRecOptionPanel
	if (V_Flag!=1)

		NewPanel/K=1/W=(675,40,1000,475) as "Data Record Options"
		DoWindow/C DataRecOptionPanel
		variable dy
	
		ModifyPanel cbRGB=(51456,44032,58880)
		ModifyPanel fixedSize=1
		SetDrawLayer UserBack
		SetDrawEnv fsize= 16,fstyle= 1
		DrawText 30,30,"Select data record options"
		
		SetDrawEnv fsize= 11,fstyle= 2
		DrawText 22,50,"These options are saved in Igor only, not in the .set files"
	
		CheckBox UpdateRunNumber,pos={20,65},size={60,20},title="Auto increment run number"
		CheckBox UpdateRunNumber,variable=root:pixie4:AutoRunNumber,fsize=11	// ,proc=Pixie_Ctrl_CommonCheckBox  no proc for Igor only variables, use "variable" to store in globals
		
		CheckBox StoreSpectrum,pos={20,90},size={60,20},title="Auto store spectrum data as binary .mca file"
		CheckBox StoreSpectrum,variable=root:pixie4:AutoStoreSpec,fsize=11 // ,proc=Pixie_Ctrl_CommonCheckBox  no proc for Igor only variables, use "variable" to store in globals
		
		CheckBox StoreSettings,pos={20,115},size={60,20},title="Auto store settings after run in .set file"
		CheckBox StoreSettings,variable=root:pixie4:AutoStoreSettings,fsize=11 // ,proc=Pixie_Ctrl_CommonCheckBox  no proc for Igor only variables, use "variable" to store in globals
				
		CheckBox AutoProcessCompressedLMData,pos={20,140},size={60,20},proc=Pixie_Ctrl_CommonCheckBox,title="Auto process list mode data into .dat file type"
		CheckBox AutoProcessCompressedLMData,variable=root:pixie4:AutoProcessLMData,fsize=11	// this value is part of system params, updated by the proc. 
		SetVariable AutoProcessLMDataVar,pos={252,140},size={38,18},title=" ", value=root:pixie4:AutoProcessLMData, limits={0,3,1}
		SetVariable AutoProcessLMDataVar,fsize=11,proc=Pixie_Ctrl_CommonSetVariable
		
		CheckBox StoreStatistics,pos={20,165},size={60,20},title="Auto store statistics after run in .ifm file"
		CheckBox StoreStatistics,variable=root:pixie4:AutoStoreStatistics,fsize=11 // ,proc=Pixie_Ctrl_CommonCheckBox  no proc for Igor only variables, use "variable" to store in globals		
		
		CheckBox AutoUpdateMCA,pos={20,190},size={60,20},title="Update MCA"
		CheckBox AutoUpdateMCA,variable=root:pixie4:AutoUpdateMCA,fsize=11 // ,proc=Pixie_Ctrl_CommonCheckBox  no proc for Igor only variables, use "variable" to store in globals	
		CheckBox AutoUpdateRS,pos={105,190},size={60,20},title="Statistics   every"
		CheckBox AutoUpdateRS,variable=root:pixie4:AutoUpdateRunStats,fsize=11
		SetVariable Tupdate,pos={212,187},size={38,18},title=" "
		SetVariable Tupdate,limits={5,inf,1},value= root:pixie4:Tupdate
		SetVariable Tupdate,fsize=11
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 255,202,"seconds"
		
		
				
		CheckBox AutoNewFile,pos={20,215},size={60,20},title="New files every"
		CheckBox AutoNewFile,variable=root:pixie4:AutoNewFile,fsize=11
		SetVariable TNewFile,pos={125,212},size={45,18},title=" "
		SetVariable TNewFile,limits={10,inf,1},value= root:pixie4:TNewFile
		SetVariable TNewFile,fsize=11
		SetDrawEnv fsize= 11,fstyle= 0
		DrawText 185,229,"spills   (0x301: seconds)"
		
		CheckBox DisableLMparsing,pos={20,240},size={60,20},title="Do not parse list mode file after run"
		CheckBox DisableLMparsing,variable=root:pixie4:DisableLMparsing,fsize=11
		
		Checkbox StartDelay, pos={20,265},size={60,20},title="Delay runstart until",variable=root:pixie4:StartDTreq,fsize=11
		SetVariable StartDelayDate, pos={140,265},size={160,20},title="Date",fsize=11, value = root:pixie4:ReqStartDate
		SetVariable StartDelayDate, help={"Run start date as in Igor 'date()' function"}
		SetVariable StartDelayTime, pos={140,285},size={160,20},title="Time",font=Arial,fsize=11, value = root:pixie4:ReqStartTime
		SetVariable StartDelayTime, help={"Run start time as in Igor 'time()' function"}
		
		Checkbox StopDelay, pos={20,310},size={60,20},title="Force run stop at",variable=root:pixie4:StopDTreq,fsize=11
		SetVariable StopDelayDate, pos={140,310},size={160,20},title="Date",fsize=11, value = root:pixie4:ReqStopDate
		SetVariable StopDelayDate, help={"Run stop date as in Igor 'date()' function"}
		SetVariable StopDelayTime, pos={140,335},size={160,20},title="Time",fsize=11, value = root:pixie4:ReqStopTime
		SetVariable StopDelayTime, help={"Run stop time as in Igor 'time()' function"}
	
	
		dy = 110
		Button PrintDateTime, pos={20,265+dy},size={66,40},title="Date/Time",proc=Pixie_Ctrl_CommonButton
		Button HelpData_Record_Options, pos={120,265+dy},size={60,40},title="Help",proc=Pixie_CallHelp
		Button DataRecOptionPanelClose,pos={220,265+dy},size={60,40},title="Close",proc=Pixie_AnyPanelClose
	
	endif
	
EndMacro


//########################################################################
//
//	Pixie_Panel_PleaseWait:
//		Remind the user about a short wait
//
//########################################################################
Window Pixie_Panel_PleaseWait() : Panel
	PauseUpdate; Silent 1		// building window...
	NewPanel /K=1/W=(400,300,820,360) as "Please wait ..."
	DoWindow/C PleaseWaitPanel
	SetDrawLayer UserBack
	SetDrawEnv fsize= 14
	DrawText 29,34,"Please wait for a few seconds until this task is finished ..."
	DoUpdate
EndMacro


//########################################################################
//
//	Pixie_EX_ExpertPanel:
//		Provides several advanced options
//
//########################################################################
Window Pixie_EX_ExpertPanel() : Panel
	PauseUpdate; Silent 1		// building window...
	DoWindow/F ExpertPanel
	if (V_flag!=1)

		NewPanel /W=(700,25,960,560) /K=1
		DoWindow/C ExpertPanel
		DoWindow/T ExpertPanel,"Expert Panel"
		ModifyPanel cbRGB=(51456,44032,58880)
		ModifyPanel fixedSize=1
	
		SetDrawLayer UserBack
		SetDrawEnv fsize= 14
		
		GroupBox ShowValues pos={10, 5}, fsize=10, fstyle=1, size={240, 100}, title="Show Values",fcolor=(1,1,1)
		Button ExpertShowModuleGlobalValues,pos={20,25},size={105,20},proc=Pixie_EX_GetSystemParameters,title="System Parameters",fsize=11
		Button ExpertShowGlobalDataValues,pos={20,50},size={105,20},proc=Pixie_EX_GetModuleParameters,title="Module Parameters",fsize=11
		Button ExpertShowUserValues,pos={20,75},size={105,20},proc=Pixie_EX_GetChanParameters,title="Channel Parameters",fsize=11
		Button ExpertShowDSPValues,pos={135,25},size={105,20},proc=Pixie_EX_GetDSPValues,title="DSP I/O",fsize=11
		Button ExpertShowMemoryValues,pos={135,50},size={105,20},proc=Pixie_EX_GetMemoryValues,title="Memory",fsize=11
		
		GroupBox DownloadValues pos={10, 115}, fsize=10, fstyle=1, size={240, 50}, title="Download Values",fcolor=(1,1,1)
		Button CallPutModPar,pos={20,135},size={105,20},proc=Pixie_EX_CallPutModPar,title="DSP I/O",fsize=11
		
		
		
		GroupBox CcontrolOptions pos={10, 178}, fsize=10, fstyle=1, size={240, 290}, title="Advanced Options (saved in .set)",fcolor=(1,1,1)		
		CheckBox RUX_CControl04_1,pos={20,198},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages (boot)"
		CheckBox RUX_CControl04_1,variable=root:pixie4:PrintDebugMsg_Boot,font="Arial", fsize=11,mode=0
		CheckBox RUX_CControl05_1,pos={20,218},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages (QC error)"
		CheckBox RUX_CControl05_1,variable=root:pixie4:PrintDebugMsg_QCerror,font="Arial", fsize=11,mode=0
		CheckBox RUX_CControl06_1,pos={20,238},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages (QC detail)"
		CheckBox RUX_CControl06_1,variable=root:pixie4:PrintDebugMsg_QCdetail,font="Arial", fsize=11,mode=0
		CheckBox RUX_CControl07_1,pos={20,258},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages (other)"
		CheckBox RUX_CControl07_1,variable=root:pixie4:PrintDebugMsg_other,font="Arial", fsize=11,mode=0
		
		CheckBox RUX_CControl13_1,pos={20,278},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages (DAQ)"
		CheckBox RUX_CControl13_1,variable=root:pixie4:PrintDebugMsg_daq,font="Arial", fsize=11,mode=0
		CheckBox RUX_CControl14_1,pos={20,298},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Print debug messages to file"
		CheckBox RUX_CControl14_1,variable=root:pixie4:PrintDebugMsg_file,font="Arial", fsize=11,mode=0
		
		CheckBox RUX_CControl08_1,pos={20,318},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Igor polls DMA (no interrupt)"
		CheckBox RUX_CControl08_1,variable=root:pixie4:Polling,font="Arial", fsize=11,mode=0
		CheckBox RUX_CControl09_1,pos={20,338},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Apply QC during DMA readout"
		CheckBox RUX_CControl09_1,variable=root:pixie4:BufferQC,font="Arial", fsize=11,mode=0
		CheckBox RUX_MCSRA11_1,pos={20,358},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Bypass SDRAM (debug only)"
		CheckBox RUX_MCSRA11_1,variable=root:pixie4:SDRAMbypass,font="Arial", fsize=11,mode=0
		
		CheckBox RUX_MCSRA08_1,pos={20,378},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Record Run stats in LM"
		CheckBox RUX_MCSRA08_1,variable=root:pixie4:RS2LM,font="Arial", fsize=11,mode=0
		CheckBox RUX_MCSRA07_1,pos={20,398},size={60,20},proc=Pixie_IO_CheckBoxControl,title="RS->LM on falling edge"
		CheckBox RUX_MCSRA07_1,variable=root:pixie4:RS2LMedge,font="Arial", fsize=11,mode=0
		
		CheckBox RUX_CControl11_1,pos={20,418},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Poll for new Data"
		CheckBox RUX_CControl11_1,variable=root:pixie4:PollForNewData,font="Arial", fsize=11,mode=0
		
		CheckBox RUX_CControl12_1,pos={20,438},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Multithreaded DAQ"
		CheckBox RUX_CControl12_1,variable=root:pixie4:MultiThreadDAQ,font="Arial", fsize=11,mode=0


		Button RebootDSP,pos={20,480},size={105,20},proc=Pixie_EX_CloseHandle,title="Release Handle",fsize=11
		Button ResetPLL,pos={140,480},size={105,20},proc=Pixie_EX_ResetPLL,title="Reset PLL",fsize=11
		Button ReadRawLMdata,pos={20,505},size={105,20},proc=Pixie_EX_ReadRawLMdata,title="Read raw LM data",fsize=11
		Button SaveRawLMdata,pos={140,505},size={105,20},proc=Pixie_EX_SaveRawLMdata,title="Save raw LM data",fsize=11
	endif
	
EndMacro


Window Pixie_Panel_SystemRunStats() : Panel
	PauseUpdate; Silent 1		// building window...
	DoWindow/F SystemRunStatsPanel
	if (V_flag!=1)

		Variable height = min(root:pixie4:NumberOfModules,5)
		NewPanel /W=(700,25,1075,height*100+270) /K=1
		DoWindow/C SystemRunStatsPanel
		DoWindow/T SystemRunStatsPanel,"All Run Statistics"
		ModifyPanel cbRGB=(51456,44032,58880)
	
		SetDrawLayer UserBack
		SetDrawEnv fsize= 14
		
		Pixie_MakeList_AllRunStats(0)

		SetVariable StartTime,bodywidth=270,pos={50,10},size={275,19},disable=0
		SetVariable StartTime,noedit=1,font="Arial",format="%.03f",fsize=10,title="Run Start"
		SetVariable StartTime,value=root:pixie4:StartTime,limits={0,Inf,0}
		
		SetVariable StopTime,bodywidth=270,pos={50,35},size={275,19},disable=0
		SetVariable StopTime,noedit=1,font="Arial",format="%.03f",fsize=10,title="Run End"
		SetVariable StopTime,value=root:pixie4:StopTime,limits={0,Inf,0}
	
		SetVariable RunStatsSource,bodywidth=270,pos={50,60},size={275,19},disable=0
		SetVariable RunStatsSource,noedit=1,font="Arial",format="%.03f",fsize=10,title="Source"
		SetVariable RunStatsSource,value=root:pixie4:InfoSource	,limits={0,Inf,0}
		
		PopupMenu RunStatsFileIO, pos={118,85},bodywidth=110,proc=Pixie_File_RunStats,title="Files"
		PopupMenu RunStatsFileIO, value="Save to text file (.ifm);Read from text file (.ifm)",mode=0,disable=0	
		Button UpdateRunStats,pos={180,85},size={70,20},title="Update",proc=Pixie_Ctrl_CommonButton,disable=0
		
		ListBox AllModStatisticsBox pos={10,120}
		ListBox AllModStatisticsBox size={350,15*height+45},frame=2,listWave=root:pixie4:AllModRunStats
		ListBox AllModStatisticsBox widths={45,70,85,70,75,95,105,110},selwave=root:pixie4:AllModRunStats_S,fsize=10
		ListBox AllModStatisticsBox mode=0,colorWave=root:pixie4:ListColorWave,font="Arial"
		
		ListBox AllChStatisticsBox pos={10, 180+15*height}
		ListBox AllChStatisticsBox size={350,62*height+35},frame=2,listWave=root:pixie4:AllChRunStats
		ListBox AllChStatisticsBox widths={45,50,80,55,55,55,100,80,85},selwave=root:pixie4:AllChRunStats_S,fsize=10
		ListBox AllChStatisticsBox mode=0,colorWave=root:pixie4:ListColorWave,font="Arial"
		
		Button ShowTrackRates, pos={50,225+77*height},size={80,40},title="History",proc=Pixie_Ctrl_CommonButton
		Button HelpRun_Statistics, pos={160,225+77*height},size={60,40},title="Help",proc=Pixie_CallHelp
		Button SystemRunStatsPanelClose,pos={260,225+77*height},size={60,40},title="Close",proc=Pixie_AnyPanelClose
		
	endif
	
EndMacro


Window Pixie_Panel_ChassisSettings() : Panel
	PauseUpdate; Silent 1		// building window...
	DoWindow/F ChassisEditPanel
	if (V_flag!=1)

		Variable width = min(root:pixie4:NumberOfModules,4)
		NewPanel /W=(120,100,760,570) /K=1
		DoWindow/C ChassisEditPanel
		DoWindow/T ChassisEditPanel,"Chassis Register Panel "
		ModifyPanel cbRGB=(51456,44032,58880)
	
		Pixie_MakeList_Chassis(0)
			
		SetDrawLayer UserBack
		SetDrawEnv fsize= 16,fstyle= 1
		DrawText 20,30,"Front Panel and Backplane Options"
		
		ListBox ChassisBox pos={20,40}
		ListBox ChassisBox size={520+width*30,150},frame=2,listWave=root:pixie4:ChassisSetup
		ListBox ChassisBox selwave=root:pixie4:ChassisSetup_S,fsize=12, widths={240,30}, proc=Pixie_ListProc_Chassis
		ListBox ChassisBox mode=8,colorWave=root:pixie4:ListColorWave,font="Arial"
		
		SetDrawEnv fsize= 16,fstyle= 1
		DrawText 20,250,"Module Coincidence Setup"
		
		ListBox CoincBox pos={20,255}
		ListBox CoincBox size={520+width*30,135},frame=2,listWave=root:pixie4:CoincSetup
		ListBox CoincBox selwave=root:pixie4:CoincSetup_S,fsize=12, widths={240,30}, proc=Pixie_ListProc_Chassis
		ListBox CoincBox mode=8,colorWave=root:pixie4:ListColorWave,font="Arial"	//use same color scheme
				
		Button HelpChassisRegisterPanel,pos={130,420},size={60,40},proc=Pixie_CallHelp,title="Help", help={"Open help entry for ChassisEditPanel"}	
		Button ChassisEditPanelClose,pos={60,420},size={60,40},title="Close",proc=Pixie_AnyPanelClose	
		
		SetVariable MultiplyAllGains pos={270, 440}, size={140,20},title="Multiply all gains by "
		SetVariable MultiplyAllGains limits={0,100,0}, fsize=12, value=root:pixie4:GainMultiplier, fstyle=0	
		Button MultiplyApply,pos={420,440},size={50,20},title="Apply",proc=Pixie_Ctrl_CommonButton	
		Button MultiplyApply, help={"When clicking ''apply'' the gains of all modules/channels will be multiplyed by the factor"}	
		
		Button MCAGainMatch,pos={270,410},size={200,20},proc=Pixie_Ctrl_CommonButton,title="Gain Match Current Module",help={"Using current peak positions from fit, adjust gains of ch.1-3 to match ch. 0"},fsize=11



	endif
	
EndMacro


//########################################################################
//
//	Pixie_Panel_About:
//		Display version information.
//
//########################################################################
Window Pixie_Panel_About(ctrlName) : ButtonControl
String ctrlName					

	// Check if this panel has already been opened
	DoWindow/F VersionPanel
	if(V_Flag == 1)
		return 0
	endif	

	Silent 1
	NewPanel/K=1 /W=(150,50,585,310) as "Version Information"
	DoWindow/C VersionPanel
	ModifyPanel cbRGB=(51456,44032,58880)
	SetDrawLayer UserBack
	DrawPICT 230,12,0.859649122807017,0.941176470588235,xialogo_bmp
	DrawText 232,113,"XIA LLC"
	DrawText 230,133,"31057 Genstar Road"
	DrawText 230,153,"Hayward, CA 94544"
	DrawText 230,173,"Phone: ...... (510)401-5760"
	DrawText 230,193,"Fax: ............ (510)401-5761"
	DrawText 230,213,"Support: .... support@xia.com"
	DrawText 230,233,"Web: .......... www.xia.com"
	DrawText 230,253,"Updates: ... support.xia.com"

	ValDisplay ViewerVersion,pos={15,20},size={180,18},bodyWidth=80,title="Pixie Viewer release"
	ValDisplay ViewerVersion,format="%.04X",limits={0,0,0},barmisc={0,1000}
	ValDisplay ViewerVersion,value= #"root:pixie4:ViewerVersion"
	
	
	SetVariable ViewerType,pos={15,40},size={180,18},bodyWidth=80,title="for Pixie-"
	SetVariable ViewerType,value= root:pixie4:MTsuffix, noproc, noedit=1

	ValDisplay CLibraryRelease,pos={15,60},size={180,18},bodyWidth=80,title="C-library release"
	ValDisplay CLibraryRelease,format="%.04X",limits={0,0,0},barmisc={0,1000}
	ValDisplay CLibraryRelease,value= #"root:pixie4:CLibraryRelease"

	ValDisplay CLibraryBuild,pos={15,80},size={180,18},bodyWidth=80,title="C-library build"
	ValDisplay CLibraryBuild,format="%04X",limits={0,0,0},barmisc={0,1000}
	ValDisplay CLibraryBuild,value= #"root:pixie4:CLibraryBuild"
	
	ValDisplay DSPRelease,pos={15,100},size={180,18},bodyWidth=80,title="DSP code release"
	ValDisplay DSPRelease,format="%.04X",limits={0,0,0},barmisc={0,1000}
	ValDisplay DSPRelease,value= root:pixie4:Display_Module_Parameters[ root:pixie4:index_DSPrelease]

	ValDisplay DSPBuild,pos={15,120},size={180,18},bodyWidth=80,title="DSP code build"
	ValDisplay DSPBuild,format="%04X",limits={0,0,0},barmisc={0,1000}
	ValDisplay DSPBuild,value= root:pixie4:Display_Module_Parameters[ root:pixie4:index_DSPbuild]
	
	ValDisplay FiPPIversion,pos={15,140},size={180,18},bodyWidth=80,title="FiPPI version"
	ValDisplay FiPPIversion,format="%04X",limits={0,0,0},barmisc={0,1000},value=  root:pixie4:Display_Module_Parameters[ root:pixie4:index_FippiID]
	
	ValDisplay Hardversion,pos={15,160},size={180,18},bodyWidth=80,title="System version"
	ValDisplay Hardversion,format="%04X",limits={0,0,0},barmisc={0,1000},value=  root:pixie4:Display_Module_Parameters[ root:pixie4:index_SystemID]

	ValDisplay Boardversion,pos={15,180},size={180,18},bodyWidth=80,title="Board version"
	ValDisplay Boardversion,format="%04X",limits={0,0,0},barmisc={0,1000},value=  root:pixie4:Display_Module_Parameters[ root:pixie4:index_BoardVersion]

	ValDisplay SerialNumber,pos={15,200},size={180,18},bodyWidth=80,title="Serial number"
	ValDisplay SerialNumber,format="%d",limits={0,0,0},barmisc={0,1000},value=  root:pixie4:Display_Module_Parameters[ root:pixie4:index_SerialNum]
	
	Button VersionInformationFile,pos={15,225},size={85,30},proc=Pixie_File_GenerateVersionText,title="Output to file"
	Button PVupdates,pos={110,225},size={100,30},proc=Pixie_CheckForPVupdates,title="Check for Updates"
	//Button VersionPanelClose,pos={135,225},size={60,30},proc=Pixie_AnyPanelClose,title="Close"
End




//########################################################################
//	Pixie_Panel_Main
//	Create the Pixie4 main control panel
//########################################################################
Function Pixie_Panel_Main()

	DoWindow/F Pixie4MainPanel
	if(V_Flag == 1)
		return(0)
	endif
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_EvRate = root:pixie4:index_EvRate
	Nvar index_ICR = root:pixie4:index_ICR
	Nvar ncp =  root:pixie4:NumChannelPar
		
	NewPanel/K=1/W=(2, 2, 135, 600) as "Pixie"
	DoWindow/C Pixie4MainPanel
	ModifyPanel fixedSize=1
	ModifyPanel cbRGB=(51456,44032,58880)
	
	SetVariable SelectedPixie4Module pos={18, 6}, size={90,20},title="Module "
	SetVariable SelectedPixie4Module limits={0,inf,1}, fsize=12, value=root:pixie4:ChosenModule, fstyle=1
	SetVariable SelectedPixie4Module proc=Pixie_IO_SelectModChan
	
	GroupBox Main_Setup pos={10,33},size={110,86},fsize=12,fstyle=1,title="Setup",disable=0,fcolor=(1,1,1)
	PopupMenu CallSetup, pos={100,58},bodywidth = 90,proc=Pixie_Ctrl_CommonPopup,title=" Open Panels",size = {10,90}
	PopupMenu CallSetup, value="Parameter Setup;Oscilloscope;Chassis Setup;Files/Paths",mode=0

	Button chooseControllerType pos={20,87},size={90,24},title="Start System",proc=Pixie_Ctrl_CommonButton,fsize=12

	
	Variable dy =88
	GroupBox Main_RunControl pos={10,45+dy},size={110,215},fsize=12,fstyle=1,title="Run Control",disable=0,fcolor=(1,1,1)
	
	TitleBox WhichRun, title="Run Type", pos={20,65+dy},size={65,90},frame=0,fsize=12
	PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), pos={22,80+dy}, proc=Pixie_Ctrl_CommonPopup
	PopupMenu RUN_WhichRun,title="",size={80,19},fsize=11,font="Arial", bodywidth = 82
	PopupMenu RUN_WhichRun, value="0x100;0x101;0x102;0x103;0x301;0x400;0x401;0x402;0x403"
	PopupMenu RUN_WhichRun, help={"0x100: General purpose; 0x101: No trace; 0x102: No aux data; 0x103: energy/time only; 0x301: MCA mode; 0x400: General purpose P500 Express"}
	dy+=40

	TitleBox Truntime, title="Run Time [s]", pos={20,70+dy},size={65,90},frame=0,fsize=12
	SetVariable Main_TimeOut,pos={22,85+dy},size={80,19}, title=" "
	SetVariable Main_TimeOut,help={"The run is stopped after # seconds in all run types. "},format="%.2f"
	SetVariable Main_TimeOut,limits={0,Inf,1},value= root:pixie4:RunTimeOut,fsize=12,font="Arial"	
	
	TitleBox Trepcount, title="Number of Spills", pos={20,110+dy},size={65,90},frame=0,fsize=12
	SetVariable Main_RepCount,pos={22,125+dy},size={80,19}, title=" "
	SetVariable Main_RepCount,format="%d",fsize=12,font="Arial"
	SetVariable Main_RepCount,limits={1,Inf,1},value= root:pixie4:RepCount
	
	Button MainStartRun,pos={20,154+dy},size={90,24},title="Start Run",proc=Pixie_RC_StartRun,title="Start Run",help={"Start Run"},disable=0,fsize=12
	Button MainStopRun,pos={20,187+dy},size={90,24},title="Stop Run",proc=Pixie_RC_StopRun,title="Stop Run",help={"Stop Run"},disable=2,fsize=12

	
	
	dy-=50
	GroupBox Main_Results pos={10,285+dy},size={110,221},fsize=12,fstyle=1,title="Results",disable=0,fcolor=(1,1,1)
	
	Button UpdateRunStats title="Update",size={90,24}, pos={20,307+dy}
	Button UpdateRunStats proc=Pixie_Ctrl_CommonButton,fSize=11
	
	dy+=32
	TitleBox EventRate, title="Event Rate [cps]", pos={20,307+dy},size={65,90},frame=0,fsize=12
	SetVariable ER,pos={22,324+dy},size={80,19}, title=" "
	SetVariable ER,help={"Event rate for current module "},format="%.05g",  noedit = 1
	SetVariable ER,value= Display_Module_Parameters[index_EvRate],fsize=12,font="Arial",limits={0,Inf,0}	
	
	dy+=40
	TitleBox InputRate, title="Input Rates [cps]", pos={20,307+dy},size={65,90},frame=0,fsize=12
	SetVariable ICR0,pos={22,324+dy},size={80,19}, title=" 0"
	SetVariable ICR0,help={"Input count rate for channel 0 of current module "},format="%.05g",  noedit = 1
	SetVariable ICR0,value= Display_Channel_Parameters[index_ICR+0*ncp],fsize=11,font="Arial",limits={0,Inf,0}	
	dy+=20
	SetVariable ICR1,pos={22,324+dy},size={80,19}, title=" 1"
	SetVariable ICR1,help={"Input count rate for channel 0 of current module "},format="%.05g",  noedit = 1
	SetVariable ICR1,value= Display_Channel_Parameters[index_ICR+1*ncp],fsize=11,font="Arial",limits={0,Inf,0}	
	dy+=20
	SetVariable ICR2,pos={22,324+dy},size={80,19}, title=" 2"
	SetVariable ICR2,help={"Input count rate for channel 0 of current module "},format="%.05g",  noedit = 1
	SetVariable ICR2,value= Display_Channel_Parameters[index_ICR+2*ncp],fsize=11,font="Arial",limits={0,Inf,0}	
	dy+=20
	SetVariable ICR3,pos={22,324+dy},size={80,19}, title=" 3"
	SetVariable ICR3,help={"Input count rate for channel 0 of current module "},format="%.05g",  noedit = 1
	SetVariable ICR3,value= Display_Channel_Parameters[index_ICR+3*ncp],fsize=11,font="Arial",limits={0,Inf,0}	
	
	dy+=35
	PopupMenu CallResults, pos={100,312+dy},bodywidth=90,proc=Pixie_Ctrl_CommonPopup,title="Open Panels",size = {10,90}, fsize=12
	PopupMenu CallResults, value="MCA Spectrum;List Mode Traces;List Mode Spectrum;Run Statistics;File Series",mode=0



End

//########################################################################
//	Pixie_Panel_Parameters
//	Create the Pixie4 Parameter Setup panel
//########################################################################
Function Pixie_Panel_Parameters(tabNum)
	Variable tabNum
	Nvar NumberOfModules = root:pixie4:NumberOfModules
	Nvar NumberOfChannels = root:pixie4:NumberOfChannels 
	
	DoWindow/F Pixie_Parameter_Setup
	if(V_Flag == 1)
		return(0)
	endif
	
	NewPanel/K=1/W=(145, 2, 830, 270) as "Parameter Setup"
	DoWindow/C Pixie_Parameter_Setup
	ModifyPanel fixedSize=1
	ModifyPanel cbRGB=(51456,44032,58880)
	
	// Common
	TabControl cmnPixie4Tab pos={5,6},size={670,190},fsize=12,font="Chicago"
	TabControl cmnPixie4Tab tabLabel(0)="Trigger",tabLabel(1)="Energy",tabLabel(2)="Waveform",tabLabel(3)="Gate"
	TabControl cmnPixie4Tab tabLabel(4)="Coincidence",tabLabel(5)="Advanced",tabLabel(6)="Run Control"
	TabControl cmnPixie4Tab value= tabNum,proc=Pixie_Tab_Switch, labelBack=(51456,44032,58880)
	
	
	SetVariable SelectedPixie4Module pos={580, 4}, size={80,20},title="Module"
	SetVariable SelectedPixie4Module limits={0,inf,1}, fsize=12, value=root:pixie4:ChosenModule
	SetVariable SelectedPixie4Module proc=Pixie_IO_SelectModChan
	
//	SetVariable SelectedPixie4Channel pos={450, 4}, size={200,20},bodywidth=50,title="Channel"
//	SetVariable SelectedPixie4Channel limits={0,NumberOfChannels-1,1}, fsize=12, value=root:pixie4:ChosenChannel
//	SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
	
	Button chooseControllerType pos={10,205},size={90,50},title="Start System",proc=Pixie_Ctrl_CommonButton,fsize=12
	Button cmnOscilloscope,pos={120,205},size={85,50},title="Oscilloscope",proc=Pixie_Ctrl_SetupButtonControl,help={"Display ADC traces"},fsize=12
	
	Button cmnLoad,pos={265,205},size={60,23},title="Load",proc=Pixie_Ctrl_SetupButtonControl,help={"Load settings from a file"},fsize=12
	Button cmnSave,pos={265,231},size={60,23},title="Save",proc=Pixie_Ctrl_SetupButtonControl,help={"Save settings to a file"},fsize=12
	Button cmnCopy,pos={340,205},size={60,23},title="Copy",proc=Pixie_Ctrl_SetupButtonControl,help={"Copy settings from one channel to other channels and modules"},fsize=12
	Button cmnExtract,pos={340,231},size={60,23},title="Extract",proc=Pixie_Ctrl_SetupButtonControl,help={"Extract settings from a settings file to destination channels and modules"},fsize=12
	

	Button moreless, pos={478,205},size={60,50},title="Less",proc=Pixie_Ctrl_MoreLess,fsize=12
	Button cmnHelpButton pos={612,205},size={60,50},title="Help",proc=Pixie_CallHelp,fsize=12
	Button displayAbout pos={545,205},size={60,50},title="About",proc=Pixie_Panel_About,fsize=12
	Nvar HideDetail = root:pixie4:HideDetail
	HideDetail =0
//	DrawText/W=Pixie_Parameter_Setup 114,250,"� XIA LLC   		 http://www.xia.com"

	Pixie_Tab_Trigger()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",0)	// hide it
	Pixie_Tab_Energy()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",1)	// hide it
	Pixie_Tab_Waveform()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",2)	// hide it
	Pixie_Tab_Gate()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",3)	// hide it
	Pixie_Tab_Coincidence()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",4)	// hide it
	Pixie_Tab_Advanced()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",5)	// hide it
	Pixie_Tab_RunControl()
	Pixie_Tab_HideControls("Pixie_Parameter_Setup",6)	// hide it
	
//	 Pixie_Tab_ShowControls("Pixie_Parameter_Setup",tabnum)//showcurrent

End


// ******************************************************************************************************************************
//		Tab panel functions
//		Checkbox Controls for DSP parameters must be named XXX_namebt_#
//           where 	"XXX" is an identifier for tabs				
//				"name" is a key word for the I/O variable  (e.g. CCSRA for CHANNEL_CSRA)
//				"bt" is the bit number
//				"#" is the channel number (or other indicator for module variables)
//
//		SetVariable controls for DSP parameters must be named XXX_name#
//           where 	"XXX" is an identifier for tabs
//				"name" is the name of the I/O variable as in Channel_Parameter_Names (e.g. ENERGY_RISETIME)
//				"#" is the channel number
//
//		Any control and title must be named XXX_name
//           where 	"XXX" is an identifier for tabs (used to enable/disable controls)	
//				an X as the third letter indicates this is an advanced control to be hidden with more/less
// ***************************************************************************************************************************************

Function Pixie_Tab_Trigger()
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType

	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_FL = root:pixie4:index_FL
	Nvar index_FG = root:pixie4:index_FG
	Nvar index_TH = root:pixie4:index_TH
	Nvar ncp =  root:pixie4:NumChannelPar
	
	
	Variable y0=70
	Variable y1= 90
	Variable y2= 110
	Variable y3 = 130
	Variable y4 = 150
	
	Variable xpos=20
	GroupBox TRG_Channel title="", pos={xpos-5,35+12},size={55,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1,font=Arial
	TitleBox TRG_group0 title="Channel", pos={xpos+5,y0},size={65,90},frame=0,fsize=11,disable=status,font=Arial,fsize=12
	TitleBox TRG_ch0, title="      0", pos={xpos+5,y1+3},size={65,90},frame=0,fsize=11,disable=status,font=Arial,fsize=12
	TitleBox TRG_ch1, title="      1", pos={xpos+5,y2+3},size={65,90},frame=0,fsize=11,disable=status,font=Arial,fsize=12
	TitleBox TRG_ch2, title="      2", pos={xpos+5,y3+3},size={65,90},frame=0,fsize=11,disable=status,font=Arial,fsize=12
	TitleBox TRG_ch3, title="      3", pos={xpos+5,y4+3},size={65,90},frame=0,fsize=11,disable=status,font=Arial,fsize=12
	xpos+=65
	
	Variable wid = 68
	GroupBox TRG_Filter title="Filter", pos={xpos-5,35},size={235,145},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1

	TitleBox TRG_group1a title="Rise Time (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	SetVariable TRG_TRIGGER_RISETIME0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_FL+0*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_RISETIME0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_RISETIME1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_FL+1*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_RISETIME1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_RISETIME2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_FL+2*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_RISETIME2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_RISETIME3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_FL+3*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_RISETIME3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)	

	xpos+=(wid+12)
	TitleBox TRG_group2a title="Flat Top (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11,font=Arial
	SetVariable TRG_TRIGGER_FLATTOP0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_FG+0*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_FLATTOP0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_FLATTOP1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_FG+1*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_FLATTOP1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_FLATTOP2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_FG+2*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_FLATTOP2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_FLATTOP3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_FG+3*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_FLATTOP3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)	

	xpos+=(wid+12)
	wid = 58
	TitleBox TRG_group3a title="Threshold",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	SetVariable TRG_TRIGGER_THRESHOLD0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_TH+0*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_THRESHOLD0 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.2f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_THRESHOLD1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_TH+1*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_THRESHOLD1 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.2f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_THRESHOLD2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_TH+2*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_THRESHOLD2 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.2f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable TRG_TRIGGER_THRESHOLD3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_TH+3*ncp],disable =status,font=Arial,fsize=12
	SetVariable TRG_TRIGGER_THRESHOLD3 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.2f",  frame=0, labelBack=(60928,60928,60928)	

	xpos+=(wid+12)
	xpos+=15
	GroupBox TRG_Options title="Options", pos={xpos-5,35},size={330,145},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1

	TitleBox TRG_group4 title="Enable",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=status,fsize=11
	TitleBox TRG_group4a title="Trigger",size={50,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	Checkbox TRG_CCSRA04_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit04, disable =status
	Checkbox TRG_CCSRA04_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit04, disable =status
	Checkbox TRG_CCSRA04_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit04, disable =status
	Checkbox TRG_CCSRA04_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit04, disable =status
	

	xpos+=45
	TitleBox TRG_group5 title="Respond to",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=status,fsize=11
	TitleBox TRG_group5a title="Group only",size={50,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	Checkbox TRG_CCSRA00_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit00, disable =status
	Checkbox TRG_CCSRA00_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit00, disable =status
	Checkbox TRG_CCSRA00_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit00, disable =status
	Checkbox TRG_CCSRA00_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit00, disable =status


	xpos+=65
	TitleBox TRX_group6 title="Good",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox TRX_group6a title="Channel",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox TRX_CCSRA02_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit02, disable =detailstatus
	Checkbox TRX_CCSRA02_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit02, disable =detailstatus
	Checkbox TRX_CCSRA02_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit02, disable =detailstatus
	Checkbox TRX_CCSRA02_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit02, disable =detailstatus
	
	xpos+=50
	TitleBox TRX_groupF title="16x longer",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox TRX_groupFa title="Trigger",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox TRX_CCSRA15_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit15, disable =detailstatus
	Checkbox TRX_CCSRA15_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit15, disable =detailstatus
	Checkbox TRX_CCSRA15_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit15, disable =detailstatus
	Checkbox TRX_CCSRA15_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit15, disable =detailstatus

	
	if(ModuleType ==4)
	xpos+=50
	TitleBox TRX_group7 title="Read",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox TRX_group7a title="Always",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox TRX_CCSRA03_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit03, disable =detailstatus
	Checkbox TRX_CCSRA03_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit03, disable =detailstatus
	Checkbox TRX_CCSRA03_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit03, disable =detailstatus
	Checkbox TRX_CCSRA03_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit03, disable =detailstatus
	

	xpos+=50
	
	TitleBox TRX_group8 title="Timestamp from",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox TRX_group8a title="local trigger only",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox TRX_CCSRA13_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit13, disable =detailstatus
	Checkbox TRX_CCSRA13_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit13, disable =detailstatus
	Checkbox TRX_CCSRA13_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit13, disable =detailstatus
	Checkbox TRX_CCSRA13_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit13, disable =detailstatus
	endif

End

Function Pixie_Tab_Energy()
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
		Nvar ModuleType = root:pixie4:ModuleType
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_SL = root:pixie4:index_SL
	Nvar index_SG = root:pixie4:index_SG
	Nvar index_TAU = root:pixie4:index_TAU
	Nvar index_INTEGRATOR = root:pixie4:index_INTEGRATOR
	Nvar ncp =  root:pixie4:NumChannelPar
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_FilterRange =  root:pixie4:index_FilterRange
	
	Variable FR = Display_Module_Parameters[index_FilterRange]
	
	Variable y0=70
	Variable y1= 90
	Variable y2= 110
	Variable y3 = 130
	Variable y4 = 150
	
	Variable xpos=20

	GroupBox EGY_Channel title="", pos={xpos-5,35+12},size={55,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1
	TitleBox EGY_group0 title="Channel", pos={xpos+5,y0},size={65,90},frame=0,fsize=11,disable=status
	TitleBox EGY_ch0, title="      0", pos={xpos+5,y1+3},size={65,90},frame=0,fsize=11,disable=status
	TitleBox EGY_ch1, title="      1", pos={xpos+5,y2+3},size={65,90},frame=0,fsize=11,disable=status
	TitleBox EGY_ch2, title="      2", pos={xpos+5,y3+3},size={65,90},frame=0,fsize=11,disable=status
	TitleBox EGY_ch3, title="      3", pos={xpos+5,y4+3},size={65,90},frame=0,fsize=11,disable=status

	Variable wid = 73
	xpos+=65	
	GroupBox EGY_Filter title="Filter", pos={xpos-5,35},size={170,145},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1
	SetVariable EGX_FilterRange,pos={xpos+25,51},size={100,19},disable=detailstatus,proc=Pixie_IO_ModVarControl
	SetVariable EGX_FilterRange,font="Arial",format="%d",fsize=11,title="Filter Range ",value= Display_Module_Parameters[index_FilterRange]
	SetVariable EGX_FilterRange,limits={1,6,1}

	TitleBox EGY_group1a title="Rise Time (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	SetVariable EGY_ENERGY_RISETIME0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_SL+0*ncp],disable =status,font=Arial,fsize=12
	SetVariable EGY_ENERGY_RISETIME0 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_RISETIME1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_SL+1*ncp],disable =status,font=Arial,fsize=12
	SetVariable EGY_ENERGY_RISETIME1 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_RISETIME2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_SL+2*ncp],disable =status,font=Arial,fsize=12
	SetVariable EGY_ENERGY_RISETIME2 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_RISETIME3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_SL+3*ncp],disable =status,font=Arial,fsize=12
	SetVariable EGY_ENERGY_RISETIME3 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)	
		
	xpos+=(wid+12)
	 wid = 68
	TitleBox EGY_group2a title="Flat Top (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	SetVariable EGY_ENERGY_FLATTOP0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_SG+0*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_ENERGY_FLATTOP0 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_FLATTOP1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_SG+1*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_ENERGY_FLATTOP1 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_FLATTOP2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_SG+2*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_ENERGY_FLATTOP2 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_ENERGY_FLATTOP3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_SG+3*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_ENERGY_FLATTOP3 ,title=" ",  limits={-inf,inf,2^FR/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)	
	
	
	xpos+=(wid+12)
	xpos+=15
	GroupBox EGY_Comp title="Computation", pos={xpos-5,35},size={138,145},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1

	wid = 58
	TitleBox EGY_group3 title="Tau (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11
	SetVariable EGY_TAU0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_TAU+0*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_TAU0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_TAU1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_TAU+1*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_TAU1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_TAU2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_TAU+2*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_TAU2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGY_TAU3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_TAU+3*ncp],fsize=12,font=Arial, disable =status
	SetVariable EGY_TAU3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g",  frame=0, labelBack=(60928,60928,60928)	

	xpos+=(wid+12)
	wid = 50
	TitleBox EGX_group4 title="Integrator",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox EGX_group4a title="Mode",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	SetVariable EGX_INTEGRATOR0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_INTEGRATOR+0*ncp],fsize=12,font=Arial, disable =detailstatus
	SetVariable EGX_INTEGRATOR0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGX_INTEGRATOR1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_INTEGRATOR+1*ncp],fsize=12,font=Arial, disable =detailstatus
	SetVariable EGX_INTEGRATOR1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGX_INTEGRATOR2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_INTEGRATOR+2*ncp],fsize=12,font=Arial, disable =detailstatus
	SetVariable EGX_INTEGRATOR2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3g",  frame=0, labelBack=(60928,60928,60928)
	SetVariable EGX_INTEGRATOR3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_INTEGRATOR+3*ncp],fsize=12,font=Arial, disable =detailstatus
	SetVariable EGX_INTEGRATOR3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3g",  frame=0, labelBack=(60928,60928,60928)	

	xpos+=(wid+12)
	xpos+=15
	GroupBox EGX_Advanced title="Options", pos={xpos-5,35},size={110,145},frame=1,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1
	wid = 43

	xpos+=5
	TitleBox EGX_group6 title="Estimate E",size={50,90}, pos={xpos,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox EGX_group6a title="if not hit",size={50,90}, pos={xpos,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox EGX_CCSRA14_0, title=" ", pos={xpos+15,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit14, disable =detailstatus
	Checkbox EGX_CCSRA14_1, title=" ", pos={xpos+15,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit14, disable =detailstatus
	Checkbox EGX_CCSRA14_2, title=" ", pos={xpos+15,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit14, disable =detailstatus
	Checkbox EGX_CCSRA14_3, title=" ", pos={xpos+15,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit14, disable =detailstatus

	xpos+=55
	TitleBox EGX_group7 title="Allow",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11
	TitleBox EGX_group7a title="E<0",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11
	Checkbox EGX_CCSRA09_0, title=" ", pos={xpos+15,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit09, disable =detailstatus
	Checkbox EGX_CCSRA09_1, title=" ", pos={xpos+15,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit09, disable =detailstatus
	Checkbox EGX_CCSRA09_2, title=" ", pos={xpos+15,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit09, disable =detailstatus
	Checkbox EGX_CCSRA09_3, title=" ", pos={xpos+15,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit09, disable =detailstatus

	xpos+=60
	GroupBox EGY_Buttons title="", pos={xpos-6,35+12},size={115,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1
	Button EGY_TauFinder,pos={xpos,52},size={100,25},title="Auto Find Tau",proc=Pixie_Ctrl_SetupButtonControl,fsize=11,disable=status
//	Button EGX_OptimizeTau,pos={xpos,82},size={100,25},title="Optimize Tau",proc=Pixie_Ctrl_SetupButtonControl,disable=detailstatus,fsize=11
//	Button EGX_OptimizeE,pos={xpos,112},size={100,25},title="Optimize Filter",proc=Pixie_Ctrl_SetupButtonControl,disable=detailstatus,fsize=11
	Button EGX_Scan,pos={xpos,142},size={100,25},title="Scan Settings",proc=Pixie_Ctrl_SetupButtonControl,disable=detailstatus,fsize=11


End

Function Pixie_Tab_Waveform()

	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_TL = root:pixie4:index_TL
	Nvar index_TD = root:pixie4:index_TD
	Nvar index_PSA_START = root:pixie4:index_PSA_START
	Nvar index_PSA_END = root:pixie4:index_PSA_END
	Nvar index_CFD_TH = root:pixie4:index_CFD_TH
	Nvar ncp =  root:pixie4:NumChannelPar
	

	Variable y0=70
	Variable y1= 90
	Variable y2= 110
	Variable y3 = 130
	Variable y4 = 150
	
	Variable xpos=20
	GroupBox WAV_Channel title="", pos={xpos-5,35+12},size={55,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	TitleBox WAV_group0 title="Channel", pos={xpos+5,y0},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox WAV_ch0, title="      0", pos={xpos+5,y1+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox WAV_ch1, title="      1", pos={xpos+5,y2+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox WAV_ch2, title="      2", pos={xpos+5,y3+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox WAV_ch3, title="      3", pos={xpos+5,y4+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	
	Variable wid = 73
	xpos+=65
	GroupBox WAV_TC title="Trace Capture", pos={xpos-5,35},size={235,145},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup

	TitleBox WAV_group1 title="Trace",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=status,fsize=11, win = Pixie_Parameter_Setup
	TitleBox WAV_group1a title="Length (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_TL+0*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH0 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_TL+1*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH1 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_TL+2*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH2 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_TL+3*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_LENGTH3 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	

	xpos+=(wid+12)
	TitleBox WAV_group2 title="Trace",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=status,fsize=11, win = Pixie_Parameter_Setup
	TitleBox WAV_group2a title="Delay (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=status,fsize=11, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_TD+0*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY0 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_TD+1*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY1 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_TD+2*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY2 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_TD+3*ncp],fsize=12,font=Arial, disable =status, win = Pixie_Parameter_Setup
	SetVariable WAV_TRACE_DELAY3 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup

	xpos+=(wid+12)
//	if(ModuleType ==4)
	TitleBox WAX_group7 title="4x longer",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox WAX_group7a title="traces",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRC11_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit11, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRC11_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit11, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRC11_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit11, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRC11_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit11, disable =detailstatus, win = Pixie_Parameter_Setup
//	endif

	xpos+=(wid+12)
	//xpos+=15
	GroupBox WAX_PSA title="Pulse Shape Analysis", pos={xpos-5,35},size={320,145},frame=1,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup

	TitleBox WAX_group3 title="PSA Start (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_PSA_START+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_PSA_START+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_PSA_START+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_PSA_START+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_START3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup

	xpos+=(wid+12)
	TitleBox WAX_group4 title="PSA End (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_PSA_END+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_PSA_END+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_PSA_END+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_PSA_END+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_PSA_END3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup

	xpos+=(wid+12)
	TitleBox WAX_group5 title="CFD",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox WAX_group5a title="Threshold (%)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_CFD_TH+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_CFD_TH+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_CFD_TH+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_CFD_TH+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable WAX_CFD_THRESHOLD3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	

	xpos+=(wid+12)
	TitleBox WAX_group6 title="Compute",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox WAX_group6a title="CFD time",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRA10_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit10, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRA10_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit10, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRA10_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit10, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox WAX_CCSRA10_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit10, disable =detailstatus, win = Pixie_Parameter_Setup

End



Function Pixie_Tab_Gate()

	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_GW = root:pixie4:index_GW
	Nvar index_GD = root:pixie4:index_GD	
	Nvar ncp =  root:pixie4:NumChannelPar
		
	Variable y0=70
	Variable y1= 90
	Variable y2= 110
	Variable y3 = 130
	Variable y4 = 150
	Variable wid
	
	Variable xpos=20
	GroupBox GAX_Channel title="", pos={xpos-5,35+12},size={55,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	TitleBox GAX_group0 title="Channel", pos={xpos+5,y0},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox GAX_ch0, title="      0", pos={xpos+5,y1+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox GAX_ch1, title="      1", pos={xpos+5,y2+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox GAX_ch2, title="      2", pos={xpos+5,y3+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox GAX_ch3, title="      3", pos={xpos+5,y4+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	

	if(ModuleType ==4)
		xpos+=65
		GroupBox GAX_GFLTtitle title="Veto", frame=1, pos={xpos-5,35},size={91,145},fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
		TitleBox GAX_group3a title="required",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=45
		TitleBox GAX_group4a title="invert",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup

		xpos+=56
		GroupBox GAX_Gateetitle title="Gate", frame=1, pos={xpos-5,35},size={355,145},frame=0,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
		TitleBox GAX_group5a title="required",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=45
		TitleBox GAX_group6a title="invert",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=38
		TitleBox GAX_group7 title="invert",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group7a title="edge",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC07_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC07_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC07_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC07_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=35
		wid = 68
		TitleBox GAX_group1a title="Window (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_GW+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_GW+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_GW+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_GW+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	
	
		xpos+=(wid+12)
		TitleBox GAX_group2a title="Delay (탎)",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_GD+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_GD+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_GD+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_GD+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	
	
		xpos+=(wid+10)
		TitleBox GAX_group8 title="copy",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group8a title="Veto",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=40
		TitleBox GAX_group11 title="no",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group11a title="pulse",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_0, title=" ", pos={xpos+5,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_1, title=" ", pos={xpos+5,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_2, title=" ", pos={xpos+5,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_3, title=" ", pos={xpos+5,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=50
		GroupBox GAX_Statstitle title="Statistics", frame=1, pos={xpos-5,35},size={95,145},fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
		TitleBox GAX_group9 title="Gate",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group9a title="statistics",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_0, title=" ", pos={xpos+15,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_1, title=" ", pos={xpos+15,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_2, title=" ", pos={xpos+15,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_3, title=" ", pos={xpos+15,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=45
		TitleBox GAX_group10 title="GDT=",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group10a title="'allow'",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC09_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC09_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC09_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC09_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup
	else
		xpos+=65
		GroupBox GAX_GFLTtitle title="Veto", frame=1, pos={xpos-5,35},size={115,145},fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
		TitleBox GAX_group3c title="Reject if",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group3a title="High",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC00_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit00, disable =detailstatus, win = Pixie_Parameter_Setup
		
		xpos+=30
		TitleBox GAX_group44a title="Low",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA06_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit06, disable =detailstatus, win = Pixie_Parameter_Setup
	
		xpos+=30
		TitleBox GAX_group55 title="Count",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group5a title="@ Fall",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	//	TitleBox GAX_group56a title="R   F",size={50,90}, pos={xpos+12,y0+12}, frame=0,disable=detailstatus, font="Arial",fSize=9, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC09_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC09_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC09_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC09_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit09, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		xpos+=30

	
		xpos+=35
		GroupBox GAX_Gateetitle title="Gate ", frame=1, pos={xpos-5,35},size={353,145},frame=0,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
		TitleBox GAX_group8 title="Use",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group8a title="Veto",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC02_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit02, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=35	

		wid = 45
		TitleBox GAX_group2a title="Delay",size={53,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group2b title="by (탎)",size={53,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_GD+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_GD+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_GD+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_GD+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_DELAY3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	
		xpos+=(wid+12)
		
		TitleBox GAX_group11 title="Make",size={50,90}, pos={xpos+2,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group11a title="Pulse",size={50,90}, pos={xpos+2,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_0, title=" ", pos={xpos+5,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_1, title=" ", pos={xpos+5,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_2, title=" ", pos={xpos+5,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC10_3, title=" ", pos={xpos+5,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit10, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=35
		
		TitleBox GAX_group18 title="Start",size={50,90}, pos={xpos+2,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group18a title="@ Fall",size={50,90}, pos={xpos+2,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC15_0, title=" ", pos={xpos+5,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit15, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC15_1, title=" ", pos={xpos+5,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit15, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC15_2, title=" ", pos={xpos+5,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit15, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC15_3, title=" ", pos={xpos+5,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit15, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=32
	
		wid = 45
		TitleBox GAX_group1 title="Width",size={58,90}, pos={xpos+10,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group1a title="(탎)",size={58,90}, pos={xpos+10,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_GW+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW0 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_GW+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW1 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_GW+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW2 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_GW+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
		SetVariable GAX_GATE_WINDOW3 ,title=" ",  limits={-inf,inf,1/FilterClockMHz}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	
		xpos+=(wid+7)
	
		TitleBox GAX_group7 title="FP",size={50,90}, pos={xpos+10,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group7a title="out",size={50,90}, pos={xpos+10,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC13_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit13, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC13_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit13, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC13_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit13, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC13_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit13, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=30
	
		TitleBox GAX_group3d title="Reject if",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group54a title="High",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC01_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit01, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=28
		
		TitleBox GAX_group64a title="Low",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRA12_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit12, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=30
	
		TitleBox GAX_group68a title="Count",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group6a title="@ Fall",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	//	TitleBox GAX_group66a title="R   F",size={50,90}, pos={xpos+12,y0+12}, frame=0,disable=detailstatus, font="Arial",fSize=9, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC07_0, title=" ", pos={xpos+12,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC07_1, title=" ", pos={xpos+12,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC07_2, title=" ", pos={xpos+12,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		Checkbox GAX_CCSRC07_3, title=" ", pos={xpos+12,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit07, disable =detailstatus, win = Pixie_Parameter_Setup, mode=0
		xpos+=33	
		
		xpos+=30
		GroupBox GAX_Statstitle title="Statistics", frame=1, pos={xpos-5,35},size={90,145},fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
		TitleBox GAX_group9 title="Gate",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group9a title="mode",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_0, title=" ", pos={xpos+15,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_1, title=" ", pos={xpos+15,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_2, title=" ", pos={xpos+15,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC08_3, title=" ", pos={xpos+15,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit08, disable =detailstatus, win = Pixie_Parameter_Setup
		xpos+=35
	
		TitleBox GAX_group101 title="Gcount",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		TitleBox GAX_group101a title="in LM",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC14_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit14, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC14_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit14, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC14_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit14, disable =detailstatus, win = Pixie_Parameter_Setup
		Checkbox GAX_CCSRC14_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit14, disable =detailstatus, win = Pixie_Parameter_Setup
	endif

		
End


Function Pixie_Tab_Coincidence()

	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	Nvar index_ActCW =  root:pixie4:index_ActCW
	Nvar index_MinCW =  root:pixie4:index_MinCW
	Nvar index_CoincPat =  root:pixie4:index_CoincPat
	Nvar index_CD = root:pixie4:index_CD
	Nvar SystemClockMHz = root:pixie4:SystemClockMHz
	Nvar ncp =  root:pixie4:NumChannelPar


	Variable xpos=25
	Variable wid = 68
	GroupBox COI_HPtitle title="Allowed Hit Patterns", frame=1, pos={xpos-10,35},size={420,145},frame=0,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	SetVariable COI_Coinc, pos={200,35}, value =  Display_Module_Parameters[index_CoincPat],fsize=12,fstyle=1, disable =status, win = Pixie_Parameter_Setup
	SetVariable COI_Coinc ,title="0x",  limits={-inf,inf,0}, proc = Pixie_IO_ModVarControl,size={60,20}, format="%04X",  frame=0, win = Pixie_Parameter_Setup//, labelBack=(60928,60928,60928)	

	TitleBox COI_group1a title="Channels", pos={xpos+26,55},size={65,90},frame=0,fsize=12,disable=status, win = Pixie_Parameter_Setup
	TitleBox COI_group1b title="3 2 1 0", pos={xpos+26,72},size={65,90},frame=0,fsize=10,disable=status,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit00_4,pos={xpos,95},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 0 0 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit00_4,variable=root:pixie4:Coincbit00,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit01_4,pos={xpos,115},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 0 0 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit01_4,variable=root:pixie4:Coincbit01,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit02_4,pos={xpos,135},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 0 1 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit02_4,variable=root:pixie4:Coincbit02,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit03_4,pos={xpos,155},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 0 1 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit03_4,variable=root:pixie4:Coincbit03,disable=status, win = Pixie_Parameter_Setup
	
	xpos+=105
	TitleBox COI_group2a title="Channels", pos={xpos+26,55},size={65,90},frame=0,fsize=12,disable=status, win = Pixie_Parameter_Setup
	TitleBox COI_group2b title="3 2 1 0", pos={xpos+26,72},size={65,90},frame=0,fsize=10,disable=status,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit04_4,pos={xpos,95},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 1 0 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit04_4,variable=root:pixie4:Coincbit04,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit05_4,pos={xpos,115},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 1 0 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit05_4,variable=root:pixie4:Coincbit05,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit06_4,pos={xpos,135},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 1 1 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit06_4,variable=root:pixie4:Coincbit06,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit07_4,pos={xpos,155},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 0 1 1 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit07_4,variable=root:pixie4:Coincbit07,disable=status, win = Pixie_Parameter_Setup
	
	xpos+=105
	TitleBox COI_group3a title="Channels", pos={xpos+26,55},size={65,90},frame=0,fsize=12,disable=status, win = Pixie_Parameter_Setup
	TitleBox COI_group3b title="3 2 1 0", pos={xpos+26,72},size={65,90},frame=0,fsize=10,disable=status,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit08_4,pos={xpos,95},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 0 0 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit08_4,variable=root:pixie4:Coincbit08,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit09_4,pos={xpos,115},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 0 0 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit09_4,variable=root:pixie4:Coincbit09,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit10_4,pos={xpos,135},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 0 1 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit10_4,variable=root:pixie4:Coincbit10,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit11_4,pos={xpos,155},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 0 1 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit11_4,variable=root:pixie4:Coincbit11,disable=status, win = Pixie_Parameter_Setup
	
	xpos+=105
	TitleBox COI_group4a title="Channels", pos={xpos+26,55},size={65,90},frame=0,fsize=12,disable=status, win = Pixie_Parameter_Setup
	TitleBox COI_group4b title="3 2 1 0", pos={xpos+26,72},size={65,90},frame=0,fsize=10,disable=status,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit12_4,pos={xpos,95},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 1 0 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit12_4,variable=root:pixie4:Coincbit12,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit13_4,pos={xpos,115},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 1 0 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit13_4,variable=root:pixie4:Coincbit13,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit14_4,pos={xpos,135},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 1 1 0",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit14_4,variable=root:pixie4:Coincbit14,disable=status, win = Pixie_Parameter_Setup
	CheckBox COI_hit15_4,pos={xpos,155},size={60,20},proc=Pixie_IO_CheckBoxControl,title=" 1 1 1 1",fsize=12,font="courier", win = Pixie_Parameter_Setup
	CheckBox COI_hit15_4,variable=root:pixie4:Coincbit15,disable=status, win = Pixie_Parameter_Setup
	
	xpos+=120
	GroupBox COI_CWtitle title="Coincidence Window", frame=1, pos={xpos-15,35},size={215,90},frame=0,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	SetVariable COI_ActualCoincWait,pos={xpos-9,49},size={195,17},title=" Window width (ns): 27 + ",bodywidth=60, win = Pixie_Parameter_Setup
	SetVariable COI_ActualCoincWait,fsize=12, font="Arial",format="%.5g",proc=Pixie_IO_ModVarControl, win = Pixie_Parameter_Setup//,  frame=0, labelBack=(60928,60928,60928)
	SetVariable COI_ActualCoincWait,limits={-inf,inf,0},value=Display_Module_Parameters[index_ActCW],disable=status, win = Pixie_Parameter_Setup
	
	//SetVariable COI_MinCoincWait,pos={xpos-6,68},size={195,17},title="Required by energy filters:", bodywidth=60, win = Pixie_Parameter_Setup
	//SetVariable COI_MinCoincWait,fsize=11, font="Arial",format="%.5g",limits={-inf,inf,0} , frame=0, labelBack=(51456,44032,58880), win = Pixie_Parameter_Setup
	//SetVariable COI_MinCoincWait,value=Display_Module_Parameters[index_MinCW], noedit=1,disable=status, win = Pixie_Parameter_Setup
	
	//PopupMenu COI_KeepCW, mode=Pixie_GetKeepCW(), pos={xpos-6,88}, proc=Pixie_Ctrl_CommonPopup, win = Pixie_Parameter_Setup
	//PopupMenu COI_KeepCW,title="",disable=status,size={198,20},bodywidth=195,fsize=10,font="Arial", win = Pixie_Parameter_Setup
	//PopupMenu COI_KeepCW, value="Increase for E filter, never decrease;Keep at required E filter width;Disregard E filter requirement", win = Pixie_Parameter_Setup
	//PopupMenu COI_KeepCW, help={"Energy filter differences require a minimum width; it can be applied as lower limit, dafault width, or ignored"}, win = Pixie_Parameter_Setup

	if(ModuleType ==4)
	Checkbox COX_KeepCW00_5, pos={xpos+97,72}, proc=Pixie_IO_CheckBoxControl, win = Pixie_Parameter_Setup
	Checkbox COX_KeepCW00_5, variable=root:pixie4:KeepCW, Title = "Adjust for E filter", disable=detailstatus
	endif
	
	TitleBox COI_group5a title="Channel delays (ns)", pos={xpos,72},size={65,90},frame=0,fsize=12,disable=detailstatus, win = Pixie_Parameter_Setup
//	TitleBox COI_group5b title="Delays", pos={xpos,135},size={65,90},frame=0,fsize=10,disable=status, win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY0, pos={xpos+20,89}, value =  Display_Channel_Parameters[index_CD+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY0 ,title="0 ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g"//,  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY1, pos={xpos+20,106}, value =  Display_Channel_Parameters[index_CD+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY1 ,title="1 ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g"//,  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY2, pos={xpos+100,89}, value =  Display_Channel_Parameters[index_CD+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY2 ,title="2 ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g"//,  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY3, pos={xpos+100,106}, value =  Display_Channel_Parameters[index_CD+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable COX_COINC_DELAY3 ,title="3 ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%.5g"//,  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup	



	GroupBox COX_Clovertitle title="Advanced", frame=1, pos={xpos-15,130},size={215,50},frame=0,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	

	CheckBox COX_MCSRA03_4,pos={xpos-8,148},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Sum channels for addback MCA", win = Pixie_Parameter_Setup
	CheckBox COX_MCSRA03_4,variable = root:pixie4:CloverAdd,fsize=11,font="Arial",disable=detailstatus, win = Pixie_Parameter_Setup
	
	if(ModuleType ==4)	
		CheckBox COX_MCSRA04_4,pos={xpos-8,163},size={60,20},proc=Pixie_IO_CheckBoxControl,title="No coincidences in channel MCAs", win = Pixie_Parameter_Setup
		CheckBox COX_MCSRA04_4,variable = root:pixie4:CloverSingleOnly,fsize=11,font="Arial",disable=detailstatus, win = Pixie_Parameter_Setup
	else
		CheckBox COX_MCSRA00_4,pos={xpos-8,163},size={60,20},proc=Pixie_IO_CheckBoxControl,title="Allow only one record per CW", win = Pixie_Parameter_Setup
		CheckBox COX_MCSRA00_4,variable = root:pixie4:CWgroup,fsize=11,font="Arial",disable=detailstatus, win = Pixie_Parameter_Setup
	endif

End

Function Pixie_Tab_Advanced()

	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType
	Variable status,detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Wave Display_Channel_Parameters = root:pixie4:Display_Channel_Parameters
	//Nvar index_EMIN = root:pixie4:index_EMIN
	Nvar index_BINFACTOR = root:pixie4:index_BINFACTOR	
	Nvar index_CCSRC = root:pixie4:index_CCSRC	
	Nvar index_CCSRA = root:pixie4:index_CCSRA	
	Nvar index_BLCUT = root:pixie4:index_BLCUT
	Nvar index_BLAVG = root:pixie4:index_BLAVG
	Nvar ncp =  root:pixie4:NumChannelPar
	
	Variable ygb=35
	Variable y0=70
	Variable y1= 90
	Variable y2= 110
	Variable y3 = 130
	Variable y4 = 150
	
	Variable xpos=20
	GroupBox ADX_Channel title="", pos={xpos-5,35+12},size={55,133},frame=1,fsize=12,disable=status,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	TitleBox ADX_group0 title="Channel", pos={xpos+5,y0},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox ADX_ch0, title="      0", pos={xpos+5,y1+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox ADX_ch1, title="      1", pos={xpos+5,y2+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox ADX_ch2, title="      2", pos={xpos+5,y3+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	TitleBox ADX_ch3, title="      3", pos={xpos+5,y4+3},size={65,90},frame=0,fsize=11,disable=status, win = Pixie_Parameter_Setup
	
	xpos+=65
	GroupBox ADX_Histotitle title="MCA Control", frame=1, pos={xpos-5,ygb},size={115,145},fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	TitleBox ADX_group1 title="Enable",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRA07_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit07, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRA07_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit07, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRA07_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit07, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRA07_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit07, disable =detailstatus, win = Pixie_Parameter_Setup
	
//	xpos+=40
	Variable wid = 38
//	TitleBox ADX_group2 title="Minimum",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
//	TitleBox ADX_group2a title="Energy",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN0 pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_EMIN+0*ncp],fsize=11, disable =detailstatus, win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN0 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_EMIN+1*ncp],fsize=11, disable =detailstatus, win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN1 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_EMIN+2*ncp],fsize=11, disable =detailstatus, win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN2 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_EMIN+3*ncp],fsize=11, disable =detailstatus, win = Pixie_Parameter_Setup
//	SetVariable ADX_EMIN3 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)	, win = Pixie_Parameter_Setup

	xpos+=(wid+12)
	wid = 43
	TitleBox ADX_group3 title="Binning",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group3a title="Factor",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_BINFACTOR+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_BINFACTOR+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_BINFACTOR+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_BINFACTOR+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BINFACTOR3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup

	xpos+=(wid+10)
	xpos+=20
	GroupBox ADX_Pileuptitle title="Out of Range and Pileup", frame=1, pos={xpos-5,ygb},size={245,145},frame=0,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	
	TitleBox ADX_group9 title="Allow out",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group9a title="of range",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC04_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit04, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC04_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit04, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC04_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit04, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC04_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit04, disable =detailstatus, win = Pixie_Parameter_Setup
	xpos+=52
	
	if(ModuleType ==4)
	TitleBox ADX_group10 title="Out of range",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group10a title="to Veto",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC12_0, title=" ", pos={xpos+20,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit12, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC12_1, title=" ", pos={xpos+20,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit12, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC12_2, title=" ", pos={xpos+20,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit12, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC12_3, title=" ", pos={xpos+20,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit12, disable =detailstatus, win = Pixie_Parameter_Setup
	endif
	xpos+=66


	TitleBox ADX_group4 title="Disable",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group4a title="pileup",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC03_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit03, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC03_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit03, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC03_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit03, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC03_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit03, disable =detailstatus, win = Pixie_Parameter_Setup

	xpos+=42
	TitleBox ADX_group5 title="Invert",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group5a title="pileup",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC05_0, title=" ", pos={xpos+10,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit05, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC05_1, title=" ", pos={xpos+10,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit05, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC05_2, title=" ", pos={xpos+10,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit05, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC05_3, title=" ", pos={xpos+10,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit05, disable =detailstatus, win = Pixie_Parameter_Setup
	
	xpos+=36
	TitleBox ADX_group6 title="Pause",size={50,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group6a title="pileup",size={50,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC06_0, title=" ", pos={xpos+15,y1+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRCbit06, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC06_1, title=" ", pos={xpos+15,y2+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRCbit06, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC06_2, title=" ", pos={xpos+15,y3+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRCbit06, disable =detailstatus, win = Pixie_Parameter_Setup
	Checkbox ADX_CCSRC06_3, title=" ", pos={xpos+15,y4+3}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRCbit06, disable =detailstatus, win = Pixie_Parameter_Setup
	
	xpos+=(wid+5)
	xpos+=7
	GroupBox ADX_Basline title="Baseline Settings", pos={xpos-5,35},size={210,145},frame=1,fsize=12,disable=detailstatus,fcolor=(1,1,1),fstyle=1, win = Pixie_Parameter_Setup
	wid = 53
	TitleBox ADX_group7 title="Baseline",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group7a title="Cut",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_BLCUT+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_BLCUT+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_BLCUT+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_BLCUT+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLCUT3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)	, win = Pixie_Parameter_Setup


	xpos+=(wid+12)
	wid = 53
	TitleBox ADX_group8 title="Baseline",size={58,90}, pos={xpos+5,y0-15}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	TitleBox ADX_group8a title="Averaging",size={58,90}, pos={xpos+5,y0}, frame=0,disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG0, pos={xpos+5,y1}, value =  Display_Channel_Parameters[index_BLAVG+0*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG0 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG1, pos={xpos+5,y2}, value =  Display_Channel_Parameters[index_BLAVG+1*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG1 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG2, pos={xpos+5,y3}, value =  Display_Channel_Parameters[index_BLAVG+2*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG2 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG3, pos={xpos+5,y4}, value =  Display_Channel_Parameters[index_BLAVG+3*ncp],fsize=12,font=Arial, disable =detailstatus, win = Pixie_Parameter_Setup
	SetVariable ADX_BLAVG3 ,title=" ",  limits={-inf,inf,1}, proc = Pixie_IO_ChanVarControl,size={wid,20}, format="%d",  frame=0, labelBack=(60928,60928,60928), win = Pixie_Parameter_Setup

	xpos+=(wid+12)
	Button ADX_BLcutUpdate,pos={xpos,y1},size={65,25},proc=Pixie_Ctrl_SetupButtonControl,title="Recompute",disable=detailstatus,fsize=11,help={"Recompute BLcut for all channels"}, win = Pixie_Parameter_Setup
	Button ADX_I2ECalib,pos={xpos,y3+15},size={65,25},proc=Pixie_Ctrl_SetupButtonControl,title="Calib. ADCs",disable=detailstatus,fsize=11,help={"Run the ADC gain/offset calibration"}, win = Pixie_Parameter_Setup
	Checkbox ADX_CControl15_1, title="Lock", pos={xpos+5,y2+10}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:KeepBLcut, disable =detailstatus, win = Pixie_Parameter_Setup


End



//########################################################################
//	Pixie4 Run Tab: user can start/stop/poll data runs here
//########################################################################
Function Pixie_Tab_RunControl()
	
	Nvar RunInProgress = root:pixie4:RunInProgress
	Nvar HideDetail = root:pixie4:HideDetail
	Nvar ModuleType = root:pixie4:ModuleType
	Variable status, detailstatus
	if (RunInProgress)
		status=2		//grayed out
		detailstatus = 2-HideDetail // invisible if hidden, gray if not
	else
		status=0		// normal
		detailstatus = HideDetail
	endif	
	
	Wave Display_Module_Parameters = root:pixie4:Display_Module_Parameters
	Nvar index_MaxEv =  root:pixie4:index_MaxEv
	
	Variable dy = 22
	Variable xr = 35
	GroupBox RUN_ControlGroup pos={xr-20,35},size={245,150},fsize=12,fstyle=1,title="Run Control",disable=0,fcolor=(1,1,1), win = Pixie_Parameter_Setup

	PopupMenu RUN_WhichRun, mode=Pixie_GetRunType(), pos={xr-18,52}, proc=Pixie_Ctrl_CommonPopup, win = Pixie_Parameter_Setup
	PopupMenu RUN_WhichRun,title="Run Type ",disable=status,bodywidth=165,size={230,19},fsize=12,font="Arial", win = Pixie_Parameter_Setup
	PopupMenu RUN_WhichRun, value="0x100: General ;0x101: No trace out ;0x102: No aux data ;0x103: Energy and time only;0x301: MCA Mode ;0x400: General (Express);0x401: Text, no trace ;0x402: Group Mode ;0x403: unused", win = Pixie_Parameter_Setup
	PopupMenu RUN_WhichRun, help={"0x100: Energy, timestamps, PSA and traces; 0x101: No trace; 0x102: No aux data; 0x103: energy/time only; 0x301: MCA mode; 0x400: Energy, timestamps, PSA and traces (Express); others: not yet supported"}, win = Pixie_Parameter_Setup
		
	SetVariable RUN_TimeOut,pos={xr,57+dy},size={150,19},title="Run Time [s]",bodywidth=85, win = Pixie_Parameter_Setup
	SetVariable RUN_TimeOut,help={"The run is stopped after # seconds in all runs. "},format="%.2f",disable=status, win = Pixie_Parameter_Setup
	SetVariable RUN_TimeOut,limits={0,Inf,1},value= root:pixie4:RunTimeOut,fsize=12,font="Arial"	, win = Pixie_Parameter_Setup

	SetVariable RUN_PollingTime,pos={xr,57+2*dy},size={150,19},title="Poll Time [s]", win = Pixie_Parameter_Setup
	SetVariable RUN_PollingTime,format="%.2f",fsize=12,bodywidth=85,disable=status,font="Arial", win = Pixie_Parameter_Setup
	SetVariable RUN_PollingTime,limits={0,1800,0.1},value= root:pixie4:PollingTime, win = Pixie_Parameter_Setup
	
	TitleBox RUN_FileTitle, title="Output File Name: Base Name + Run Number",fstyle=0,frame=0,pos={xr-8,55+3*dy+6},fsize=11,font="Arial",disable=status, win = Pixie_Parameter_Setup
	
	SetVariable RUN_FileBase,pos={xr+8,53+4*dy},size={205,19},title="Base Name ",fsize=12,font="Arial",disable=detailstatus, win = Pixie_Parameter_Setup
	SetVariable RUN_FileBase,limits={-Inf,Inf,1},value= root:pixie4:OutBaseName,bodywidth=150, win = Pixie_Parameter_Setup
	
	SetVariable RUN_Number,pos={xr,53+5*dy},size={150,19},title="Run Number",fsize=12,font="Arial",disable=detailstatus, win = Pixie_Parameter_Setup
	SetVariable RUN_Number,format="%04d",limits={1,Inf,1},value= root:pixie4:RunNumber,bodywidth=85, win = Pixie_Parameter_Setup

	Variable xl =277
	GroupBox RUN_ListModeGroup pos={xl-8,35},size={170,150},fsize=12,fstyle=1,title="List Mode Spill Settings",disable=status,fcolor=(1,1,1), win = Pixie_Parameter_Setup
	
	SetVariable RUN_RepCount,pos={xl,55},size={150,19},title="No. of Spills", win = Pixie_Parameter_Setup
	SetVariable RUN_RepCount,format="%d",fsize=12,bodywidth=70,disable=status,font="Arial", win = Pixie_Parameter_Setup
	SetVariable RUN_RepCount,limits={1,Inf,1},value= root:pixie4:RepCount, win = Pixie_Parameter_Setup

	if(ModuleType ==4)		
	SetVariable RUX_SpillTimeOut,pos={xl,55+dy},size={150,19},title="Spill Timeout [s]",bodywidth=70, win = Pixie_Parameter_Setup
	SetVariable RUX_SpillTimeOut, format="%.2f",disable=status, win = Pixie_Parameter_Setup
	SetVariable RUX_SpillTimeOut,limits={0,Inf,1},value= root:pixie4:SpillTimeOut,fsize=12,font="Arial"	, win = Pixie_Parameter_Setup

	SetVariable RUX_SetMaxEvents,pos={xl,55+2*dy},size={150,19},proc=Pixie_IO_ModVarControl,title="Events / Buffer", win = Pixie_Parameter_Setup
	SetVariable RUX_SetMaxEvents,font="Arial", fsize=12,disable=detailstatus,win = Pixie_Parameter_Setup
	SetVariable RUX_SetMaxEvents,format="%d",limits={0,Inf,1},value= Display_Module_Parameters[index_MaxEv],bodywidth=70, win = Pixie_Parameter_Setup

	CheckBox RUX_MCSRA01_5,pos={xl+25,55+3*dy},size={60,20},proc=Pixie_IO_CheckBoxControl,title="1 buffer per spill", win = Pixie_Parameter_Setup
	CheckBox RUX_MCSRA01_5,variable=root:pixie4:NoRepeatLMrun,disable=detailstatus,font="Arial", fsize=12,mode=1, win = Pixie_Parameter_Setup

	CheckBox RUX_MCSRA01_4,pos={xl+25,55+4*dy},size={60,20},proc=Pixie_IO_CheckBoxControl,title="32 buffers per spill", win = Pixie_Parameter_Setup
	CheckBox RUX_MCSRA01_4,variable=root:pixie4:RepeatLMrun,disable=detailstatus,font="Arial", fsize=12,mode=1, win = Pixie_Parameter_Setup
	endif
		
	//CheckBox RUX_DblBufCSR00_4,pos={xl+25,55+5*dy},size={60,20},proc=Pixie_IO_CheckBoxControl,title="16/16 buffers (cont.)", win = Pixie_Parameter_Setup
	//CheckBox RUX_DblBufCSR00_4,variable=root:pixie4:PingPongRun,disable=detailstatus,font="Arial", fsize=11,mode=1, win = Pixie_Parameter_Setup
	
	
	Variable xs = 455
	GroupBox RUX_SynchronizeGroup pos={xs-7,35},size={127,150},fsize=12,fstyle=1,title="Synchronization",disable=HideDetail,fcolor=(1,1,1)	, win = Pixie_Parameter_Setup
	
	CheckBox RUX_SynchWait00_4,pos={xs,55},size={120,22},proc=Pixie_IO_CheckBoxControl,fsize=11,disable=status, win = Pixie_Parameter_Setup
	CheckBox RUX_SynchWait00_4,title="Simultaneously start",variable=root:pixie4:SynchWait,font="Arial", win = Pixie_Parameter_Setup
	TitleBox RUX_SWTitle, title="and stop modules",frame=0,pos={xs+20,55+dy-8},disable=status, fsize=11,font="Arial",win = Pixie_Parameter_Setup


	CheckBox RUX_RunSynchro00_4,pos={xs,55+2*dy-8},size={120,22},proc=Pixie_IO_CheckBoxControl,fsize=11, win = Pixie_Parameter_Setup
	CheckBox RUX_RunSynchro00_4,title="Re-synchronize",variable=root:pixie4:InSynch,disable=detailstatus,font="Arial", win = Pixie_Parameter_Setup
	TitleBox RUX_RSTitle, title="clocks at runstart",frame=0,pos={xs+20,55+3*dy-16},disable=detailstatus,fsize=11,font="Arial", win = Pixie_Parameter_Setup

	if(ModuleType ==4)
		CheckBox RUX_SyncPersist00_5,pos={xs+20,55+4*dy-16},size={120,22},fsize=11,win = Pixie_Parameter_Setup // Igor only variable, no procedure needed
		CheckBox RUX_SyncPersist00_5,title="every new run",variable=root:pixie4:SynchAlways,disable=detailstatus,font="Arial", win = Pixie_Parameter_Setup
	endif
	
	if(ModuleType ==500)
		CheckBox RUX_MCSRA10_4,pos={xs,55+5*dy-8},size={120,22},proc=Pixie_IO_CheckBoxControl,fsize=11,disable=status, win = Pixie_Parameter_Setup
		CheckBox RUX_MCSRA10_4,title="500 MHz Timestamps",variable=root:pixie4:TS500,font="Arial", win = Pixie_Parameter_Setup
	endif
	if(ModuleType ==400)
		CheckBox RUX_MCSRA10_4,pos={xs,55+5*dy-8},size={120,22},proc=Pixie_IO_CheckBoxControl,fsize=11,disable=status, win = Pixie_Parameter_Setup
		CheckBox RUX_MCSRA10_4,title="400 MHz Timestamps",variable=root:pixie4:TS500,font="Arial", win = Pixie_Parameter_Setup
	endif
	
	Button RUN_BeginDAQ,pos={587,50},size={72,40},proc=Pixie_RC_StartRun,title="Start Run",help={"Start Run"},disable=status,fsize=11, win = Pixie_Parameter_Setup
	Button RUN_EndDAQ,pos={587,95},size={72,40},proc=Pixie_RC_StopRun,title="Stop Run",help={"Stop Run"},disable=((status==2?0:2)),fsize=11, win = Pixie_Parameter_Setup
	Button RUX_RecordOption,pos={587,140},size={72,40},proc=Pixie_Ctrl_SetupButtonControl,title="Record",help={"Data record options"},disable=detailstatus,fsize=11, win = Pixie_Parameter_Setup
	
End



//########################################################################
//	Oscilloscope display
//########################################################################
Window Pixie_Plot_Oscilloscope() : Graph

	DoWindow/F Pixie4Oscilloscope
	if (V_Flag!=1)
	
		PauseUpdate; Silent 1	// building window...
		Display/K=1 /W=(250,200,650,525) root:pixie4:ADCch0,root:pixie4:ADCch1,root:pixie4:ADCch2,root:pixie4:ADCch3 as "Oscilloscope"
		DoWindow/C Pixie4Oscilloscope
	
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph mode=6
		ModifyGraph grid=2
		ModifyGraph mirror(bottom)=2
		ModifyGraph mirror(left)=2
		ModifyGraph rgb(ADCch1)=(0,65280,0),rgb(ADCch2)=(0,15872,65280),rgb(ADCch3)=(0,26112,0)
		Label left "ADC Units"
		Label bottom "Time"
		SetAxis/A/N=1 left
		ControlBar 140
		ShowInfo
		
		Variable ncp = root:pixie4:NumChannelPar
		
		Button ADCRefresh,pos={10,105},size={65,30},proc=Pixie_Ctrl_CommonButton,title="Refresh",help={"Refresh graph with new data"},fsize=11
		Button AdjustDC,pos={90,105},size={80,30},proc=Pixie_Ctrl_CommonButton,title="Adjust Offsets",disable=0,fsize=11,help={"Automatically adjust offsets in all modules"}
		Button HelpOscilloscope, pos={185,105},size={55,30},proc=Pixie_CallHelp,title="Help",fsize=11
		Button ADCDisplayClose,pos={255,105},size={70,30},proc=Pixie_AnyGraphClose,title="Close",fsize=11
		
			
		TabControl ScopeTab pos={347,3},size={180,133},fsize=12	//180,125
		TabControl ScopeTab tabLabel(0)="Buttons",tabLabel(1)="Tau",tabLabel(2)="Rates"
		TabControl ScopeTab value= 0,proc=Pixie_Tab_ScopeSwitch, labelBack=(51456,44032,58880)	
		
		// Button Tab
		Button FFTDisplay,pos={365,30},size={60,27},proc=Pixie_Ctrl_CommonButton,title="FFT",fsize=11,help={"Open plot with FFT analysis of trace"}, disable=0
		Button ADCFilterDisplay,pos={440,30},size={60,27},proc=Pixie_Ctrl_CommonButton,title="Filters",fsize=11,help={"Open plot with energy filter and trigger filter response"}, disable=0
		Button ADCDisplayCapture,pos={365,100},size={60,27},proc=Pixie_Ctrl_CommonButton,title="Capture",fsize=11
		Button ADCDisplayCapture,help={"Refresh graph with new data and repeat until a pulse is found"}, disable=0
		Button ADCDisplaySave,pos={440,100},size={60,27},proc=Pixie_Ctrl_CommonButton,title="Save",fsize=11
		Button ADCDisplaySave,help={"Save Oscilloscope waveforms to a file (Igor text format with time scale)"}, disable=0
		Button ADX_I2ECalib,pos={365,65},size={60,27},proc=Pixie_Ctrl_SetupButtonControl,title="Calibrate",disable=0
		Button ADX_I2ECalib,fsize=11,help={"Run the calibration routine to match the ADC cores' gain and offset"}
		Button ADX_I2EReset,pos={440,65},size={60,27},proc=Pixie_Ctrl_SetupButtonControl,title="Reset",disable=0
		Button ADX_I2EReset,fsize=11,help={"Reset the ADC to re-initialize cores' gain and offset"}

		//Tau Tab	
//		GroupBox Tau title="Manually Fit Decay Time", pos={355,5},size={168,95},frame=1,fsize=11,fcolor=(1,1,1),fstyle=0
		Popupmenu TauFit, pos={392,35},bodywidth=80,mode=0,title="Fit Trace",proc=Pixie_Ctrl_CommonPopup
		PopupMenu TauFit, value="Channel 0;Channel 1;Channel 2;Channel 3",fsize=11, disable=1
		PopupMenu TauFit, help={"Fit an oscilloscope trace between cursors to determine the decay time tau"}
			
		Popupmenu TauOK, pos={472,35},bodywidth=70,mode=0,title="Accept",proc=Pixie_Ctrl_CommonPopup
		PopupMenu TauOK, value="Channel 0;Channel 1;Channel 2;Channel 3",fsize=11
		PopupMenu TauOK, help={"Apply fit value found for tau to selected channel"}, disable=1
		
		SetVariable LastTau, pos={362,63}, value =  root:pixie4:LastTau, fsize=11, help={"Result of Tau fit"}, disable=1
		SetVariable LastTau ,title="Tau (탎)",size={105,20}, format="%.4f", noedit=1, limits={-inf,inf,0}
		SetVariable TauDeviation, pos={467,63}, value =  root:pixie4:TauDeviation, fsize=11, help={"Standard deviation of Tau fit"}
		SetVariable TauDeviation ,title="�",size={55,20}, format="%.4f", noedit=1, limits={-inf,inf,0}, disable=1
		
		Button TauClear,pos={362,90},size={155,25},proc=Pixie_Ctrl_CommonButton,title="Remove Tau Fit From Graph",fsize=11, disable=1

		// Rates Tab
		TitleBox ICRtitle, title="ICR (cps)",size={58,90}, pos={365,30}, frame=0, disable=1
		SetVariable ADC_ICR0, pos={365,46}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CICR+0*ncp], noedit=1, disable=1
		SetVariable ADC_ICR0, title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_ICR0,  help={"ICR measured in the last ~3ms. Precise to about 10% or 50 cps"}
		SetVariable ADC_ICR1, pos={365,66}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CICR+1*ncp], noedit=1, disable=1
		SetVariable ADC_ICR1, title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_ICR1,  help={"ICR measured in the last ~3ms. Precise to about 10% or 50 cps"}
		SetVariable ADC_ICR2, pos={365,86}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CICR+2*ncp], noedit=1, disable=1
		SetVariable ADC_ICR2, title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_ICR2,  help={"ICR measured in the last ~3ms. Precise to about 10% or 50 cps"}
		SetVariable ADC_ICR3, pos={365,106}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_CICR+3*ncp], noedit=1, disable=1
		SetVariable ADC_ICR3, title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_ICR3,  help={"ICR measured in the last ~3ms. Precise to about 10% or 50 cps"}

		TitleBox OORFtitle, title="Out of Range (%)",size={58,90}, pos={430,30}, frame=0, disable=1
		SetVariable ADC_OORF0, pos={430,46}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OORF+0*ncp], noedit=1, disable=1
		SetVariable ADC_OORF0 ,title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_OORF0,  help={"Fraction the signal was out of ADC range in the last ~3ms. Precise to about 10%"}
		SetVariable ADC_OORF1, pos={430,66}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OORF+1*ncp], noedit=1, disable=1
		SetVariable ADC_OORF1 ,title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_OORF1,  help={"Fraction the signal was out of ADC range in the last ~3ms. Precise to about 10%"}
		SetVariable ADC_OORF2, pos={430,86}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OORF+2*ncp], noedit=1, disable=1
		SetVariable ADC_OORF2 ,title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_OORF2,  help={"Fraction the signal was out of ADC range in the last ~3ms. Precise to about 10%"}
		SetVariable ADC_OORF3, pos={430,106}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OORF+3*ncp], noedit=1, disable=1
		SetVariable ADC_OORF3 ,title=" ",  limits={-inf,inf,0},size={48,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_OORF3,  help={"Fraction the signal was out of ADC range in the last ~3ms. Precise to about 10%"}


		Variable xpos = 10

		TitleBox group0 title="Channel",size={65,90}, pos={xpos+5,5}, frame=0
		Checkbox ADCch0, title="\K(65280,0,0)\f01||||||  0", pos={xpos+5,21}, proc=Pixie_Ctrl_CheckTrace,value=1
		Checkbox ADCch1, title="\K(0,65280,0)\f01||||||  1", pos={xpos+5,41}, proc=Pixie_Ctrl_CheckTrace,value=1
		Checkbox ADCch2, title="\K((0,15872,65280)\f01||||||  2", pos={xpos+5,61}, proc=Pixie_Ctrl_CheckTrace,value=1
		Checkbox ADCch3, title="\K(0,26112,0)\f01||||||  3", pos={xpos+5,81}, proc=Pixie_Ctrl_CheckTrace,value=1
		
		xpos = 80
		TitleBox group2 title="dT (탎)",size={58,90}, pos={xpos+5,5}, frame=0
		SetVariable ADC_XDT0, pos={xpos+5,21}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_XDT+0*ncp]
		SetVariable ADC_XDT0 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={48,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_XDT1, pos={xpos+5,41}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_XDT+1*ncp]
		SetVariable ADC_XDT1 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={48,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_XDT2, pos={xpos+5,61}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_XDT+2*ncp]
		SetVariable ADC_XDT2 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={48,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_XDT3, pos={xpos+5,81}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_XDT+3*ncp]
		SetVariable ADC_XDT3 ,title=" ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={48,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)	
				
		xpos = 130
		TitleBox group3 title="Offset (%)",size={68,90}, pos={xpos+5,5}, frame=0
		SetVariable ADC_BASELINE_PERCENT0, pos={xpos+5,21}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_BASEPC+0*ncp]
		SetVariable ADC_BASELINE_PERCENT0 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_BASELINE_PERCENT1, pos={xpos+5,41}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_BASEPC+1*ncp]
		SetVariable ADC_BASELINE_PERCENT1 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_BASELINE_PERCENT2, pos={xpos+5,61}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_BASEPC+2*ncp]
		SetVariable ADC_BASELINE_PERCENT2 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_BASELINE_PERCENT3, pos={xpos+5,81}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_BASEPC+3*ncp]
		SetVariable ADC_BASELINE_PERCENT3 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%d",  frame=0, labelBack=(60928,60928,60928)

		xpos = 183
		TitleBox group4 title="Gain (V/V)",size={68,90}, pos={xpos+5,5}, frame=0
		SetVariable ADC_VGAIN0, pos={xpos+5,21}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_GAIN+0*ncp]
		SetVariable ADC_VGAIN0 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={58,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VGAIN1, pos={xpos+5,41}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_GAIN+1*ncp]
		SetVariable ADC_VGAIN1 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={58,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VGAIN2, pos={xpos+5,61}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_GAIN+2*ncp]
		SetVariable ADC_VGAIN2 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={58,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VGAIN3, pos={xpos+5,81}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_GAIN+3*ncp]
		SetVariable ADC_VGAIN3 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={58,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		
		xpos = 243
		TitleBox group5 title="Offset (V)",size={68,90}, pos={xpos+5,5}, frame=0
		SetVariable ADC_VOFFSET0, pos={xpos+5,21}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OFFSET+0*ncp]
		SetVariable ADC_VOFFSET0 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VOFFSET1, pos={xpos+5,41}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OFFSET+1*ncp]
		SetVariable ADC_VOFFSET1 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VOFFSET2, pos={xpos+5,61}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OFFSET+2*ncp]
		SetVariable ADC_VOFFSET2 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)
		SetVariable ADC_VOFFSET3, pos={xpos+5,81}, value =  root:pixie4:Display_Channel_Parameters[root:pixie4:index_OFFSET+3*ncp]
		SetVariable ADC_VOFFSET3 ,title="  ",  limits={-inf,inf,0}, proc = Pixie_IO_ChanVarControl,size={51,20}, format="%.3f",  frame=0, labelBack=(60928,60928,60928)

		xpos = 301
		TitleBox group1 title="Invert",size={50,90}, pos={xpos+5,5}, frame=0
		Checkbox ADC_CCSRA05_0, title=" ", pos={xpos+10,21}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan0CSRAbit05
		Checkbox ADC_CCSRA05_1, title=" ", pos={xpos+10,41}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan1CSRAbit05
		Checkbox ADC_CCSRA05_2, title=" ", pos={xpos+10,61}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan2CSRAbit05
		Checkbox ADC_CCSRA05_3, title=" ", pos={xpos+10,81}, proc = Pixie_IO_CheckBoxControl,variable =root:pixie4:Chan3CSRAbit05
	endif
	
EndMacro


//########################################################################
//	Display List Mode Traces
//########################################################################
Window Pixie_Plot_LMTracesDisplay() : Graph

	DoWindow/F ListModeTracesDisplay
	if (V_Flag!=1)

		PauseUpdate; Silent 1		// building window...
		Display/K=1 /W=(250,175,700,500) root:pixie4:trace0,root:pixie4:trace1,root:pixie4:trace2,root:pixie4:trace3 as "List Mode Traces"
		DoWindow/C ListModeTracesDisplay
	
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph mode=6
		ModifyGraph grid=1
		ModifyGraph mirror(bottom)=2
		ModifyGraph mirror(left)=2
		ModifyGraph rgb(trace1)=(0,65280,0),rgb(trace2)=(0,15872,65280),rgb(trace3)=(0,26112,0)
		SetAxis/A/N=1 left
		Label bottom "Time from first sample"
		Label left "Pulse Height (ADC steps)"
		ControlBar 135
		
		Variable dfy = 33
		Variable evtinfox = 580
		Variable cby = 95
		Variable bux=501
		
		
		SetVariable TraceDataFile, value=root:pixie4:DataFile, pos={298,dfy+18},size={170,18},title=" "
		SetVariable TraceDataFile, fsize=11,proc=Pixie_Ctrl_CommonSetVariable,font=Arial//,bodywidth=100
		
		Button FindTraceDataFile, pos={298,dfy+37},size={40,18},proc=Pixie_Ctrl_CommonButton,title="Find",fsize=11
	
		SetVariable CallReadEvents,pos={300,12},size={170,18},proc=Pixie_Ctrl_CommonSetVariable,title="Event Number    "
		SetVariable CallReadEvents,format="%d",fsize=12, font=Arial//,bodywidth=70
		SetVariable CallReadEvents,limits={0,Inf,1},value= root:pixie4:ChosenEvent, fstyle=1

		if( (root:pixie4:ModuleType ==501) | (root:pixie4:ModuleType ==5) )
			GroupBox Files pos={290,dfy},size={187,92},fsize=12,fstyle=0,title="Data File",disable=0,fcolor=(1,1,1)
			Button FindTraceDataFile, pos={298,dfy+37},size={40,18},proc=Pixie_Ctrl_CommonButton,title="File",fsize=11
			Checkbox Return4, title="Show 4 pulses within", pos={300,cby+2}, variable = root:pixie4:Show4LMtraces, fsize=10
			SetVariable DisplayCW,pos={420,cby},size={50,18},proc=Pixie_Ctrl_CommonSetVariable,title=" "
			SetVariable DisplayCW,format="%d",fsize=11,font=Arial//,bodywidth=70
			SetVariable DisplayCW,limits={0,65535,1},value= root:pixie4:DisplayTimeWindow
			//SetVariable text1, pos = {500,50}, title = "    clock ticks", frame=0,fsize=10, size={50,20}, noedit=1, noproc, limits={0,65535,0}, valueColor=(51456,44032,58880)
			TitleBox title1 title="clock ticks",frame=0, pos = {420,cby+17}
			Titlebox title1, fsize=9, font="Arial"
			
			//SetDrawEnv fsize= 8,fstyle= 0
			//DrawText 520,cby+14,"clock ticks"
			
			ValDisplay Hitpattern,  pos = {evtinfox,5}, title = "Hit Pattern 0x", format ="%8.8X", value = root:Pixie4:EventHitpattern, size={150,20},fsize=10
			TitleBox title2 title="Module",frame=0, pos = {400,dfy+40},font=Arial,fsize=10
			Button LMT_NextMod, pos={440,dfy+37},size={25,18},proc=Pixie_Ctrl_CommonButton,title=">",fsize=11
			Button LMT_PrevMod, pos={370,dfy+37},size={25,18},proc=Pixie_Ctrl_CommonButton,title="<",fsize=11
		else
			GroupBox Files pos={290,dfy},size={187,45},fsize=12,fstyle=0,title="Data File",disable=0,fcolor=(1,1,1)
			ValDisplay Hitpattern,  pos = {300,90}, title = "Hit Pattern 0x", format ="%4.4X", value = root:Pixie4:EventHitpattern, size={120,20},fsize=10
			ValDisplay TimeStampHI,  pos = {300,110}, title = "Event Time", value = root:Pixie4:EventTimeHI, size={100,20},fsize=10
			ValDisplay TimeStampLO,  pos = {405,110}, value = root:Pixie4:EventTimeLO, size={40,20},fsize=10
			Button FindTraceDataFile, pos={bux,110},size={55,22},proc=Pixie_Ctrl_CommonButton,title="File",fsize=11

		endif
		
		
		// Initialize Channel Energy List Data
		Pixie_MakeList_Traces(0)
		
		// Create the Channel Energy List Box
		ListBox ChannelEnergyBox pos={20,10},size={250,115},frame=2,listWave=root:pixie4:ListModeEnergyListData
		ListBox ChannelEnergyBox widths={50,75,75,75,75},selwave=root:pixie4:ListModeEnergySListData,fsize=10,font="arial"
		ListBox ChannelEnergyBox mode=8,disable=0,colorWave=root:pixie4:ListColorWave,proc=Pixie_ListProc_Traces
	
		// control buttons
		Button EventFilterDisplay, pos={bux,20},size={55,22},proc=Pixie_Plot_FilterDisplay,title="Filter",fsize=11
		PopupMenu LMRefSelect, pos={bux,50},proc=Pixie_Ctrl_CommonPopup,title="Ref",size={55,22}, bodywidth= 55
		PopupMenu LMRefSelect, value="Set ch.0 as ref;Set ch.1 as ref;Set ch.2 as ref;Set ch.3 as ref",mode=0,disable=0	
		Button HelpList_Mode_Traces, pos={bux,80},size={55,22},proc=Pixie_CallHelp,title="Help",fsize=11
		
		// event info spelled out
		Checkbox Evinfo5, title = "Overall accept", pos ={evtinfox, 18}, fsize=9, variable = root:Pixie4:EvHit_Accept, disable=2
		Checkbox Evinfo16, title = "Local coincidence test ok", pos ={evtinfox, 32}, fsize=9, variable = root:Pixie4:EvHit_CoincOK, disable=2
		Checkbox Evinfo20, title = "Local channel hit", pos ={evtinfox, 46}, fsize=9, variable = root:Pixie4:EvHit_ChannelHit, disable=2
		Checkbox Evinfo18, title = "Pulse piled up", pos ={evtinfox, 60}, fsize=9, variable = root:Pixie4:EvHit_PiledUp, disable=2
		Checkbox Evinfo21, title = "Pulse out of range", pos ={evtinfox, 74}, fsize=9, variable = root:Pixie4:EvHit_OOR, disable=2
		Checkbox Evinfo19, title = "Waveform FIFO full", pos ={evtinfox, 88}, fsize=9, variable = root:Pixie4:EvHit_WvFifoFull, disable=2
		Checkbox Evinfo17, title = "Veto logic high", pos ={evtinfox, 102}, fsize=9, variable = root:Pixie4:EvHit_Veto, disable=2
		Checkbox Evinfo22, title = "Data Transmission error", pos ={evtinfox, 116}, fsize=9, variable = root:Pixie4:EvHit_Derror, disable=2
		
		Checkbox Evinfo23, title = "Gate logic high", pos ={evtinfox+120, 32}, fsize=9, variable = root:Pixie4:EvHit_Gate, disable=2
		Checkbox Evinfo24, title = "PSA sum overflow", pos ={evtinfox+120, 46}, fsize=9, variable = root:Pixie4:EvHit_PSAovr, disable=2
		Checkbox Evinfo4, title = "Front panel input high", pos ={evtinfox+120, 60}, fsize=11, variable = root:Pixie4:EvHit_Front, disable=2
		Checkbox Evinfo6, title = "Backplane Status input high", pos ={evtinfox+120, 74}, fsize=11, variable = root:Pixie4:EvHit_Status, disable=2
		Checkbox Evinfo7, title = "Backplane Token input high", pos ={evtinfox+120, 88}, fsize=11, variable = root:Pixie4:EvHit_Token, disable=2

	endif
	
EndMacro


//########################################################################
//	Display MCA spectrum
//########################################################################
Window Pixie_Plot_MCADisplay() : Graph

	DoWindow/F MCASpectrumDisplay
	if (V_Flag!=1)

		PauseUpdate; Silent 1 // building window...
		
		Display/K=1 /W=(150,150,725,500) root:pixie4:MCAch0,root:pixie4:MCAch1,root:pixie4:MCAch2,root:pixie4:MCAch3 as "MCA Spectrum"
		DoWindow/C MCASpectrumDisplay	
		//AppendToGraph root:pixie4:MCAsum
		//ModifyGraph rgb(MCAsum)=(0,0,0)
		Label left "\\Z10Counts"
		Label bottom "\\Z10Channel"
		
		ModifyGraph cbRGB=(51456,44032,58880),mode=6,grid=1,mirror(bottom)=2,mirror(left)=2
		ModifyGraph mode=6,grid=1
		ModifyGraph grid=1
		ModifyGraph mirror(bottom)=2
		ModifyGraph mirror(left)=2
		ModifyGraph rgb(MCAch1)=(0,65280,0),rgb(MCAch2)=(0,15872,65280),rgb(MCAch3)=(0,26112,0)
		SetAxis/A/N=1 left
		ShowInfo
		ControlBar 150	
	
		// Initialize MCA Spectrum List Data
		Pixie_MakeList_MCA(0)
		
		// Create the MCA Spectrum List Box
		ListBox MCASpectrumBox pos={20,10},size={515,100},frame=2,listWave=root:pixie4:MCASpecListData
		ListBox MCASpectrumBox widths={40,45, 40,40,40,45,45,45,50},selwave=root:pixie4:MCASpecSListData,fsize=10,font="arial"
		ListBox MCASpectrumBox mode=8,disable=0,colorWave=root:pixie4:ListColorWave,proc=Pixie_ListProc_MCA
	
		PopupMenu MCAFitOptions, pos={710,10},bodywidth=150,mode=Pixie_GetFitOption(),title=" ",proc=Pixie_Ctrl_CommonPopup
		PopupMenu MCAFitOptions, value="Fit tallest peaks in spectra;Fit peaks with highest E;Fit peaks between min/max;Fit peaks between cursors"
		PopupMenu MCAFitOptions, help={"Fit options"}
	
		PopupMenu GaussFitMCA, pos={555,10},bodywidth=60,mode=0,title="Fit",proc=Pixie_Math_GaussFit
		PopupMenu GaussFitMCA, value="Channel 0;Channel 1;Channel 2;Channel 3;All Single Channels;Reference;Addback;Cum.0;Cum.1;Cum.2;Cum.3"
		PopupMenu GaussFitMCA, help={"Do Gauss fit between limits defined in Fit Option"}
		
		PopupMenu SumHistoMCAch, pos={555,45},bodywidth=60,mode=0,title="Sum",proc=Pixie_Math_SumHisto
		PopupMenu SumHistoMCAch, help={"Sum histogram"}
		PopupMenu SumHistoMCAch, value="Sum-Background between limits;Sum between limits;Sum complete MCA"
		
		PopupMenu StoreMCA, pos={625,45},bodywidth=60,proc=Pixie_File_MCA,title="Files", help={"Save, read and extract from file"}
		PopupMenu StoreMCA, value="Save MCA to Igor text file;Read MCA from Igor text file;Extract MCA from binary file (.mca);Export ch.0 as CHN;Export ch.1 as CHN;Export ch.2 as CHN;Export ch.3 as CHN;Export ALL ch. as CHN;Export ALL ch. as CSV",mode=0
		
		PopupMenu MCARefSelect, pos={695,45},proc=Pixie_Ctrl_CommonPopup,title="Ref",bodywidth=60
		PopupMenu MCARefSelect, value="Set ch.0 as ref;Set ch.1 as ref;Set ch.2 as ref;Set ch.3 as ref",mode=0,disable=0	

	
		Button AutoScaleMCA, pos={18,120},size={65,20},proc=Pixie_Ctrl_CommonButton,title="Auto zoom", help={"Zoom to full range"},fsize=11
		Button ZoomInMCA, pos={90,120},size={52,20},proc=Pixie_Ctrl_CommonButton,title="Zoom in", help={"Zoom in MCA"},fsize=11
		Button ZoomOutMCA, pos={150,120},size={57,20},proc=Pixie_Ctrl_CommonButton,title="Zoom out", help={"Zoom out MCA"},fsize=11
		Button ZoomMCAToCursors, pos={214,120},size={90,20},proc=Pixie_Ctrl_CommonButton,title="Zoom to cursors", help={"Zoom MCA to Cursors"},fsize=11
		Button ResetScaleMCA, pos={312,120},size={70,20},proc=Pixie_Ctrl_CommonButton,title="Reset Scale", help={"Reset MCA scale to 1/bin"},fsize=11
	
		
		Button UpdateMCA, pos={545,80},size={60,30},proc=Pixie_RC_UpdateMCAData,title="Update", help={"Update MCA for the current module"},fsize=11
		Button HelpMCA_Spectrum,pos={615,80},size={60,30},proc=Pixie_CallHelp,title="Help", help={"Show details of Gauss fit"},fsize=11
		Button MCASpecClose,pos={685,80},size={60,30},proc=Pixie_AnyGraphClose,title="Close",help={"Close Panel"},fsize=11
	
		SetVariable MCASpectrumFile,pos={390,123},size={360,20},disable=0,title=" MCA  source"
		SetVariable MCASpectrumFile,font="Arial",fSize=10, noedit=1,limits={-inf,inf,0}
		SetVariable MCASpectrumFile,value=  root:pixie4:MCASource	
	endif
	
EndMacro

Window Pixie_Plot_LMEScatter()
			DoWindow/F LMEScatter
			if(V_Flag != 1)
				Display/K=1/W=(150, 250, 600, 600) root:pixie4:EnergyWave0,root:pixie4:EnergyWave1,root:pixie4:EnergyWave2,root:pixie4:EnergyWave3 as "Pixie4 LM E Scatter plot"
				DoWindow/C LMEScatter
				ModifyGraph mode=2
				ModifyGraph rgb(EnergyWave2)=(0,15872,65280),rgb(EnergyWave1)=(0,65280,0),rgb(EnergyWave3)=(0,26112,0)
			
			endif
EndMacro

//########################################################################
//	Display List Mode spectrum
//########################################################################
Window Pixie_Plot_LMSpectrumDisplay() : Graph

	DoWindow/F ListModeSpectrumDisplay
	if (V_Flag!=1)

		PauseUpdate; Silent 1  // building window...
		Display/K=1 /W=(100,150,735,500) root:pixie4:Spectrum0,root:pixie4:Spectrum1,root:pixie4:Spectrum2,root:pixie4:Spectrum3 as "List Mode Spectrum"
		DoWindow/C ListModeSpectrumDisplay	
		
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph mode=6
		ModifyGraph grid=1
		ModifyGraph mirror(bottom)=2
		ModifyGraph mirror(left)=2
		ModifyGraph rgb(Spectrum2)=(0,15872,65280),rgb(Spectrum1)=(0,65280,0),rgb(Spectrum3)=(0,26112,0)
		SetAxis/A/N=1 left
		ControlBar 150
		ShowInfo
	
				
		// Initialize List Mode Spectrum List Data
		Pixie_MakeList_LMHisto()
		
		// Create the List Mode Spectrum List Box
		ListBox ListModeSpectrumBox pos={20,15},size={425,95},frame=2,listWave=root:pixie4:ListModeSpecListData
		ListBox ListModeSpectrumBox widths={35,45},selwave=root:pixie4:ListModeSpecSListData,fsize=10,font="arial"
		ListBox ListModeSpectrumBox mode=8,disable=0,colorWave=root:pixie4:ListColorWave,proc=Pixie_ListProc_LMHisto
	
	
		Variable chx=462
		GroupBox HistogrammingGroup pos={chx,5},size={180,105},fsize=12,title="Channel Settings",  disable=0,fcolor=(1,1,1)

		SetVariable SelectedPixie4Channel pos={chx+20,25}, size={150,20},title="Channel",font="arial"
		SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=10, value=root:pixie4:ChosenChannel
		SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan, bodywidth=100
					
		SetVariable SetHistoEmin,pos={chx+20,45},size={150,18},proc=Pixie_Ctrl_CommonSetVariable,title="Emin"
		SetVariable SetHistoEmin,limits={1,65535,1},value= root:pixie4:HistoEmin,fsize=10,font="arial",bodywidth=100
		
		SetVariable SetHistoDE,pos={chx+20,65},size={150,18},proc=Pixie_Ctrl_CommonSetVariable,title="Delta E"
		SetVariable SetHistoDE,limits={1,Inf,1},value= root:pixie4:HistoDE,fsize=10,font="arial",bodywidth=100
		
		SetVariable SetNbin,pos={chx+20,85},size={150,18},proc=Pixie_Ctrl_CommonSetVariable,title="No. of Bins"
		SetVariable SetNbin,format="%d",limits={2,Inf,1},value= root:pixie4:NHistoBins,fsize=10,font="arial",bodywidth=100
		
		Variable dfx=660
		GroupBox FileGroup pos={dfx,5},size={170,105},fsize=12,title="File Settings",  disable=0,fcolor=(1,1,1)
	
		SetVariable HistoDataFile, value=root:pixie4:DataFile, pos={dfx+10,25},size={130,18},title="Name",font="arial"
		SetVariable HistoDataFile, fsize=10,proc=Pixie_Ctrl_CommonSetVariable
		Button FindTraceDataFile, pos={dfx+145,24},size={15,18},proc=Pixie_Ctrl_CommonButton,title=":",fsize=11
		
		SetVariable SetFirstEvent,pos={dfx+10,45},size={150,18},title="Event range: First"
		SetVariable SetFirstEvent,format="%d",limits={0,Inf,1},value= root:pixie4:HistoFirstEvent,fsize=10,font="arial"
		SetVariable SetLastEvent,pos={dfx+70,65},size={90,18},title="Last "
		SetVariable SetLastEvent,format="%d",limits={1,Inf,1},value= root:pixie4:HistoLastEvent,fsize=10,font="arial"
		
		Button CallLoadListModeSpec,pos={dfx+8,85},size={62,20},proc=Pixie_Ctrl_CommonButton,title="Read File", help={"Read data from file"},fsize=11, fstyle=1
		Button CallListModeSpecHisto,pos={dfx+82,85},size={80,20},proc=Pixie_Ctrl_CommonButton,title="Histogram", help={"Histogram"},fsize=11, fstyle=1
		TitleBox arrow, pos={dfx+72,88}, title=">", fstyle=1, frame=0
	
		Button AutoScaleListmodeSpectrum, pos={18,120},size={65,20},proc=Pixie_Ctrl_CommonButton,title="Auto zoom", help={"Zoom to full range"},fsize=11
		Button ZoomInListmodeSpectrum, pos={90,120},size={52,20},proc=Pixie_Ctrl_CommonButton,title="Zoom in", help={"Zoom in MCA"},fsize=11
		Button ZoomOutListmodeSpectrum, pos={150,120},size={57,20},proc=Pixie_Ctrl_CommonButton,title="Zoom out", help={"Zoom out MCA"},fsize=11
		Button ZoomListModeSpectrumToCursors, pos={214,120},size={90,20},proc=Pixie_Ctrl_CommonButton,title="Zoom to cursors", help={"Zoom MCA to Cursors"},fsize=11
		Button ResetScaleListModeSpectrum, pos={312,120},size={70,20},proc=Pixie_Ctrl_CommonButton,title="Reset Scale", help={"Reset MCA scale to 1/bin"},fsize=11
	
		PopupMenu LMSRefSelect, pos={480,120},proc=Pixie_Ctrl_CommonPopup,title="Ref",bodywidth=60
		PopupMenu LMSRefSelect, value="Set ch.0 as ref;Set ch.1 as ref;Set ch.2 as ref;Set ch.3 as ref",mode=0,disable=0	
		PopupMenu GaussFitListModech, pos={550,120},bodywidth=60,mode=0,title="Fit",proc=Pixie_Math_GaussFit
		PopupMenu GaussFitListModech, value="Channel 0;Channel 1;Channel 2;Channel 3;All Channels;Reference", help={"Do Gauss fit between min and max"}
		PopupMenu SumHistoListModech, pos={620,120},bodywidth=60,mode=0,title="Sum",proc=Pixie_Math_SumHisto
		PopupMenu SumHistoListModech, help={"Sum histogram"}
		PopupMenu SumHistoListModech, value="Sum-Background between limits;Sum between limits;Sum complete MCA"	
		Button CallListModeSpecEScatter, pos={680,120}, size = {70,20},proc=Pixie_Ctrl_CommonButton,title="Scatter Plot", help={"Open new scatetr plot of energies"},fsize=11, fstyle=0
		Button HelpList_Mode_Spectrum,pos={760,120},size={60,20},proc=Pixie_CallHelp,title="Help", help={"Show details of Gauss fit"},fsize=11
		//		Button ListModeSpecClose,pos={755,80},size={60,30},proc=Pixie_AnyGraphClose,title="Close",help={"Close Panel"},fsize=11


	endif
	
EndMacro


//########################################################################
//	Display FFT of ADC traces
//########################################################################
Window Pixie_Plot_FFTdisplay() : Graph

	DoWindow/F FFTDisplay
	if (V_Flag!=1)

		PauseUpdate; Silent 1  // building window...
		Display/K=1 /W=(100,150,450,450) root:pixie4:TraceFFT as "ADC Trace FFT"
		DoWindow/C FFTDisplay
	
		if(root:pixie4:ChosenChannel == 0)
			ModifyGraph rgb(TraceFFT)=(65280,0,0)
		endif
		if(root:pixie4:ChosenChannel == 1)
			ModifyGraph rgb(TraceFFT)=(0,65280,0)
		endif
		if(root:pixie4:ChosenChannel == 2)
			ModifyGraph rgb(TraceFFT)=(0,15872,65280)
		endif
		if(root:pixie4:ChosenChannel == 3)
			ModifyGraph rgb(TraceFFT)=(0,26112,0)
		endif
	
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph mode=6
		ModifyGraph grid=1
		ModifyGraph mirror=2
		Label left "Signal Amplitude, ADC units"
		Label bottom "Frequency"
		SetAxis/A/N=1 left
		ShowInfo
		ControlBar 80
		
		SetVariable SelectedPixie4Channel pos={18,7}, size={100,20},title="Channel"
		SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=11, value=root:pixie4:ChosenChannel
		SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
	
	
		ValDisplay valdisp0,pos={18,35},title="Frequency bin width, Hz "
		ValDisplay valdisp0,limits={0,0,0},barmisc={0,1000},fsize=11
		ValDisplay valdisp0,value= #"root:pixie4:FFTbin", size={200,20}
		
		Button FilterFFT,pos={235,15},size={65,50},proc=Pixie_Ctrl_CommonButton,title="Apply Filter",fsize=11
		Button FilterFFT,help={"Apply the current energy filter to the Fourier spectrum"}
		Button HelpFFTDisplay, pos={312,15},size={62,50},proc=Pixie_CallHelp,title="Help",fsize=11
		Button FFTDisplayClose,pos={385,15},size={62,50},proc=Pixie_AnyGraphClose,title="Close",fsize=11
	endif
	
EndMacro




//########################################################################
//	Display Filters on ADC traces
//########################################################################
Window Pixie_Plot_ADCFilter() : Graph

	DoWindow/F ADCFilterDisplay
	if (V_Flag!=1)

		PauseUpdate; Silent 1  // building window...
		Display/K=1 /W=(100,150,450,450) root:pixie4:TraceFilter as "ADC Trace Filter Display"
		DoWindow/C ADCFilterDisplay
		AppendtoGraph root:pixie4:TraceFilterSFMarkers
		AppendtoGraph/L=L1 root:pixie4:TraceFilterSF
		AppendtoGraph/L=L2 root:pixie4:TraceFilterFF, root:pixie4:TraceTH
		AppendtoGraph/L=L3 root:pixie4:TraceGate
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph mode=6
		ModifyGraph rgb(TraceFilter)=(0,26112,0),rgb(TraceFilterFF)=(0,0,0)
		ModifyGraph grid=1
		ModifyGraph mirror=2
		ModifyGraph standoff=0
		ModifyGraph lblPos(left)=47,lblPos(L1)=47,lblPos(L2)=48,lblPos(L3)=48
		ModifyGraph lblLatPos(left)=-2,lblLatPos(L1)=-2
		ModifyGraph freePos(L1)=0
		ModifyGraph freePos(L2)=0
		ModifyGraph freePos(L3)=0
		ModifyGraph axisEnab(left)={0.05,0.35}
		ModifyGraph axisEnab(L1)={0.38,0.68}
		ModifyGraph axisEnab(L2)={0.71,1}
		ModifyGraph axisEnab(L3)={0,0.03}
		ModifyGraph grid(L3)=0
		ModifyGraph manTick(L3)={0,1,0,1},manMinor(L3)={0,50}
		ModifyGraph lstyle(TraceTH)=11,lsize(TraceTH)=2,rgb(TraceTH)=(0,0,65280)
		ModifyGraph rgb(TraceFilterSF)=(65280,43520,0)
		Label left "Signal"
		Label bottom "Time"
		Label L1 "Slow Filter "
		Label L2 "Fast Filter "
		Label L3 "Gate"
		
		if(root:pixie4:ChosenChannel == 0)
			ModifyGraph rgb(TraceFilter)=(65280,0,0)
		endif
		if(root:pixie4:ChosenChannel == 1)
			ModifyGraph rgb(TraceFilter)=(0,65280,0)
		endif
		if(root:pixie4:ChosenChannel == 2)
			ModifyGraph rgb(TraceFilter)=(0,15872,65280)
		endif
		if(root:pixie4:ChosenChannel == 3)
			ModifyGraph rgb(TraceFilter)=(0,26112,0)
		endif
	
		
		SetAxis/A
		ShowInfo
		ControlBar 70
		
		SetVariable SelectedPixie4Channel pos={10,7}, size={135,20},bodywidth=75,title="Channel"
		SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=11, value=root:pixie4:ChosenChannel
		SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
	
		
		Titlebox M1,pos={10,30},size={208,16},title="To compute properly, filter lengths should", frame=0,fsize=10
		Titlebox M2,pos={10,45},size={208,16},title="be multiples of the ADC sampling interval dT     ", frame=0,fsize=10
	
		Button ADCFilterDisplayClose,pos={375,10},size={60,40},proc=Pixie_AnyGraphClose,title="Close"
		Button ADCFilterDisplayRefresh,pos={225,10},size={60,40},proc=Pixie_Ctrl_CommonButton,title="Refresh"
		Button HelpADCFilterDisplay, pos={300,10},size={60,40},proc=Pixie_CallHelp,title="Help"
	
	endif
	
EndMacro


//########################################################################
//
//	Pixie_Plot_FilterDisplay:
//		Display digital trapezoidal filter response of a list mode trace.
//
//########################################################################
Function Pixie_Plot_FilterDisplay(CtrlName) : ButtonControl
String CtrlName

	DoWindow/F Pixie4FilterDisplay
	if(V_Flag == 1)
		return(0)
	endif

	PauseUpdate; Silent 1		// building window...
	String fldrSav0= GetDataFolder(1)
	SetDataFolder root:pixie4:
	Display/K=1 /W=(275.25,42.5,600,293.75) seltrace
	DoWindow/C Pixie4FilterDisplay
	AppendToGraph/L=L2 ff,th
	AppendToGraph/L=L1 sf
	SetDataFolder fldrSav0
	ModifyGraph cbRGB=(51456,44032,58880)
	ModifyGraph mode(seltrace)=6,mode(ff)=6,mode(sf)=6
	ModifyGraph lSize(th)=2
	ModifyGraph lStyle(th)=7
	ModifyGraph rgb(ff)=(0,15872,65280),rgb(th)=(0,12800,52224),rgb(sf)=(0,52224,0)
	ModifyGraph grid=1
	ModifyGraph mirror=2
	ModifyGraph standoff=0
	ModifyGraph lblPos(left)=66,lblPos(L2)=68,lblPos(L1)=68
	ModifyGraph lblLatPos(left)=4,lblLatPos(L2)=-3
	ModifyGraph freePos(L2)=0
	ModifyGraph freePos(L1)=0
	ModifyGraph axisEnab(left)={0,0.3}
	ModifyGraph axisEnab(L2)={0.66,1}
	ModifyGraph axisEnab(L1)={0.33,0.63}
	Label left "Signal"
	Label bottom "Time"
	Label L2 "Fast Filter "
	Label L1 "Slow Filter "
	SetAxis/A/N=1 left
	ControlBar 60
	
	SetVariable SelectedPixie4Channel pos={20,7}, size={135,20},bodywidth=75,title="Channel"
	SetVariable SelectedPixie4Channel limits={0,3,1}, fsize=11, value=root:pixie4:ChosenChannel
	SetVariable SelectedPixie4Channel proc=Pixie_IO_SelectModChan	
	
	SetVariable DigitalFilterSelectEvent,pos={20,32},size={135,20},proc=Pixie_Ctrl_CommonSetVariable,title="Trace Number"
	SetVariable DigitalFilterSelectEvent,format="%d",bodywidth=75, fsize=11
	SetVariable DigitalFilterSelectEvent,limits={0,Inf,1},value= root:pixie4:ChosenEvent
	Button HelpFilter_Display,  pos={250,5}, size={60,35},title="Help",proc=Pixie_CallHelp
	Button FilterDisplayClose,pos={320,5},size={60,35},proc=Pixie_AnyGraphClose,title="Close"
	
End

//########################################################################
//
//	Pixie_Plot_FileSeries:
//		Display resolution etc from a series of files
//
//########################################################################
Window Pixie_Plot_FileSeries() 


	DoWindow/F PixieFileSeries
	if(V_Flag != 1)
		String fldrSav0= GetDataFolder(1)
		SetDataFolder root:results:
		Display/K=1 /W=(100,100,600,500) tau0,tau1,tau2,tau3 vs runnumber as "File Series Results"
		DoWindow/C PixieFileSeries
		AppendToGraph/L=L1 Tgap0,Tgap1,Tgap2,Tgap3,Tpeak0,Tpeak1,Tpeak2,Tpeak3 vs runnumber  
		AppendToGraph/L=L2 peak0,peak1,peak2,peak3 vs runnumber 
		AppendToGraph/L=L3 FWHM0,FWHM1,FWHM2,FWHM3 vs runnumber 
		SetDataFolder fldrSav0
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph lblPos(left)=52,lblPos(L1)=50,lblPos(L2)=49,lblPos(L3)=48
		ModifyGraph lblLatPos(L3)=-6
		ModifyGraph freePos(L1)={0,bottom}
		ModifyGraph freePos(L2)={0,bottom}
		ModifyGraph freePos(L3)={0,bottom}
		ModifyGraph axisEnab(left)={0,0.22}
		ModifyGraph axisEnab(L1)={0.26,0.48}
		ModifyGraph axisEnab(L2)={0.52,0.75}
		ModifyGraph axisEnab(L3)={0.78,1}
		
		ModifyGraph rgb(tau1)=(0,65280,0),rgb(tau2)=(0,15872,65280),rgb(tau3)=(0,26112,0)
		ModifyGraph rgb(Tgap1)=(0,65280,0),rgb(Tgap2)=(0,15872,65280),rgb(Tgap3)=(0,26112,0)
		ModifyGraph rgb(Tpeak1)=(0,65280,0),rgb(Tpeak2)=(0,15872,65280),rgb(Tpeak3)=(0,26112,0)
		ModifyGraph rgb(peak1)=(0,65280,0),rgb(peak2)=(0,15872,65280),rgb(peak3)=(0,26112,0)
		ModifyGraph rgb(FWHM1)=(0,65280,0),rgb(FWHM2)=(0,15872,65280),rgb(FWHM3)=(0,26112,0)
		
		Label left "Tau (탎)"
		Label bottom "Run number"
		Label L1 "Filter (탎)"
		Label L2 "Position"
		Label L3 "FWHM (%)"
		ControlBar 90
	
		ModifyGraph grid=1
		ModifyGraph mirror=2
		ModifyGraph mode=4
		ModifyGraph marker=16
		ModifyGraph marker(tau0)=7,marker(tau1)=7,marker(tau2)=7,marker(tau3)=7
		ModifyGraph marker(Tgap0)=6,marker(Tgap1)=6,marker(Tgap2)=6,marker(Tgap3)=6
		ModifyGraph marker(FWHM0)=19,marker(FWHM1)=19,marker(FWHM2)=19,marker(FWHM3)=19
		ModifyGraph marker(Tpeak0)=22,marker(Tpeak1)=22,marker(Tpeak2)=22,marker(Tpeak3)=22
		ShowInfo
		
		String txt =  "\s(FWHM0) \s(FWHM1) \s(FWHM2) \s(FWHM3) FWHM\r\s(peak0) \s(peak1) \s(peak2) \s(peak3) Peak position\r\s(Tpeak0) \s(Tpeak1) \s(Tpeak2) \s(Tpeak3) Trise\r"
		txt+= "\s(Tgap0) \s(Tgap1) \s(Tgap2) \s(Tgap3) Tflat\r\s(tau0) \s(tau1) \s(tau2) \s(tau3) Tau"
		Legend/C/N=text0 txt

	
		Variable dy = 8
		Variable xx=25
		
		Variable xpos = 170
		Checkbox ch0, title="\K(65280,0,0)\f01Include channel 0", pos={xpos+5,dy}, variable = root:auto:chan0
		Checkbox ch1, title="\K(0,65280,0)\f01Include channel 1", pos={xpos+5,dy+20},  variable = root:auto:chan1
		Checkbox ch2, title="\K((0,15872,65280)\f01Include channel 2", pos={xpos+5,dy+40},  variable = root:auto:chan2
		Checkbox ch3, title="\K(0,26112,0)\f01Include channel 3", pos={xpos+5,dy+60},  variable = root:auto:chan3
			
		dy = 10
		SetVariable SVbn,pos={xx,dy},size={130,18},title="Basename"
		SetVariable SVbn,value= root:auto:basename, fsize=11
			
		SetVariable SVvon,pos={xx,dy+25},size={130,18},title="Start run no. "
		SetVariable SVvon,value= root:auto:von, fsize=11
		
		SetVariable SVbis,pos={xx,dy+50},size={130,18},title="End run no.   "
		SetVariable SVbis,value= root:auto:bis, fsize=11
	
		Button ParseSeries, pos = {xpos+150,15}, title = "Parse Files", size = {85,35}, proc = Pixie_Ctrl_CommonButton
		Button HelpFile_Series,  pos={xpos+245,15}, size={60,35},title="Help",proc=Pixie_CallHelp
		Button SeriesClose,pos={xpos+315,15},size={60,35},proc=Pixie_AnyGraphClose,title="Close"
	endif
	
End




//########################################################################
//	Display to track event rates during run 
//########################################################################
Window Pixie_Plot_TrackRates() : Graph

	DoWindow/F Pixie4TrackRates
	if (V_Flag!=1)
		Display/K=1 /W=(250,200,550,420) root:pixie4:ER0 vs root:pixie4:ERtimestamp as "Track Rates"
		
		DoWindow/C Pixie4TrackRates
	
		ModifyGraph cbRGB=(51456,44032,58880)
		ModifyGraph grid=2
		
		ModifyGraph mirror=2
		Label left "Rate (counts/s)"
		Label bottom "Time since runstart (s)"
		SetAxis/A/N=1 left
		ControlBar 85
		SetVariable Mod0, pos ={10,5}, size={100,20}, title="Rate0: Mod", value= root:pixie4:TrackMod[0], fsize=11, limits={0,root:pixie4:PRESET_MAX_MODULES,1}
		SetVariable Chan0, pos ={117,5}, size={70,20}, title="Chan", value= root:pixie4:TrackChan[0], fsize=11, limits={0,3,1}
		SetVariable Mod1, pos ={10,25}, size={100,20}, title="Rate1: Mod", value= root:pixie4:TrackMod[1], fsize=11, limits={0,root:pixie4:PRESET_MAX_MODULES,1}
		SetVariable Chan1, pos ={117,25}, size={70,20}, title="Chan", value= root:pixie4:TrackChan[1], fsize=11, limits={0,3,1}
		SetVariable Mod2, pos ={10,45}, size={100,20}, title="Rate2: Mod", value= root:pixie4:TrackMod[2], fsize=11, limits={0,root:pixie4:PRESET_MAX_MODULES,1}
		SetVariable Chan2, pos ={117,45}, size={70,20}, title="Chan", value= root:pixie4:TrackChan[2], fsize=11, limits={0,3,1}
		SetVariable Mod3, pos ={10,65}, size={100,20}, title="Rate3: Mod", value= root:pixie4:TrackMod[3], fsize=11, limits={0,root:pixie4:PRESET_MAX_MODULES,1}
		SetVariable Chan3, pos ={117,65}, size={70,20}, title="Chan", value= root:pixie4:TrackChan[3], fsize=11, limits={0,3,1}
		Button TrackClear,pos={210,30},size={60,35},proc=Pixie_Ctrl_CommonButton,title="Clear"
		Button TrackClose,pos={300,30},size={60,35},proc=Pixie_AnyGraphClose,title="Close"
		Checkbox TrackRate, pos={210,5},size={60,20},title="Track Event rates during updates",variable=root:pixie4:trackER,fsize=12

		AppendtoGraph root:pixie4:ER1 vs root:pixie4:ERtimestamp
		AppendtoGraph root:pixie4:ER2 vs root:pixie4:ERtimestamp
		AppendtoGraph root:pixie4:ER3 vs root:pixie4:ERtimestamp
		ModifyGraph rgb(ER1)=(0,65280,0),rgb(ER2)=(0,15872,65280),rgb(ER3)=(0,26112,0)
		Legend/C/N=text0/J "\\s(ER0) Rate0 \r\\s(ER1) Rate1\r\\s(ER2) Rate2 \r\\s(ER3) Rate3"
		ModifyGraph mode=4,marker=19
	endif
EndMacro