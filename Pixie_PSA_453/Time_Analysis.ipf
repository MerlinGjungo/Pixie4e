#pragma rtGlobals=1		// Use modern global access method.

// This procedure contains functions to compute time-of-arrival differences between 2 Pixie-4/500 channels
// After loading it into the Pixie Viewer, this is how to operate it:
//- click "compile" at the bottom of the procedure window
//- execute "Time_Globals()" from the command line to initialize variables
//- execute "Time_Panel()" from the command line to open a small control panel

// Brief description of controls:
// - in the upper left, choose event increment = 2 for data files with run type 0x400 with 2
//   active channels. Use 3 for 3 active channels etc.
// - The Max. events control is used to limit processing to a portion of a large file 
// - To accommodate differen module types, time scale of time stamps and waveforms samples
//   are user variables. See help text in lower left of  the Igor .pxp window for guidance. Defaults are 2ns/2ns 
//- below the time units you set the modules and channels to compute timing for 
//  (e.g. Mod A = 0, B=1, Chan A=0, B=0 for M0C0 and M1C0).
//- below that are parameters for the baseline (how many points to use at the beginning 
//  of the trace to define the DC level), CFD threshold (0.5=50% of amplitude), and a default
//  trigger position (the earliest point the code starts looking for a rising edge). These values should 
// default to 12, 0.5, 20 
//- below that are thresholds to apply energy and rise time cuts before histogramming, e.g. 
//  to focus only on pulses with 1.3 MeV. The cuts can be enabled or disabled by the checkboxes. 
//  Both cut and uncut values are reported. Ignore for at first.
//
//- On the right side, you can choose file and event number, similar to the list mode trace display.
//  There will probably be an error about a null or missing wave the first time you change event 
//  number, that can be ignored.
//- The "traces" button opens a window with the 2 waveforms of the current event
//- below that are a number of results:
//  base = the average of the first N samples of the trace
//  amplitude = max pulse height found
//  energy = energy computed by P4
//  rise time = pulse rise time
//  CFD pos = time from beginning of trace to CFD level
//  ev time high/low = event time stamps from file
//  ch time stamp = channel time stamp
//  event # = should be same as the one selected above
//  T difference is the difference of CFD time and channel time stamps
//
// Results from the online (DSP/FPGa) computation are available if data has been taken in mode 0x400 with the PSA firmware
// Results are
//	DSP off = offset from trigger point
//	DSP dt = fractional time from lower closest sample to CFD level
//	DSP T difference is the difference of CFD time and channel time stamps
//
//Normally I click through event by event to see what is going on. Clicking the "process" button 
//at the bottom runs over all events in the file, computes the time differences, applies cuts and histograms them 
// into the dT spectrum with the histogramming parameters defined in in control for start/size, and number of bins
// "Cut E, histo" reapplies any cut and repeats the histogramming. "Show Histo" 
// and "Show Tdiff" opens plots of the dT spectrum and the dT plotted vs event number. "Tdiff vs E" and
//  "Tdiff vs RT" open scatter plots of these properties (one for each channel) so one can look at any 
// dependency and figure out if/where to cut.
//"Fit histo" fits the dT spectrum with a Gaussian (use cursors to define range) and reports the results in the command line history window

Menu "&Timing"
	"-"
	"Time Panel",Time_Panel()
End

Function Time_Globals()	
	NewDataFolder/O root:tim
	make/d/o/n=30 root:tim:Eventvalues
	make/o/n=10 root:tim:traceA
	make/o/n=10 root:tim:traceB
	make/o/n=10 root:tim:triglocsA
	make/o/n=10 root:tim:triglocsB
	make/o/n=1 root:tim:dthisto
	make/o/n=1 root:tim:multiTS
	
	variable/G root:tim:doublepulse	=0 //0 - to process double pulse in single channel; 1- to process 2 pulses in 2 separate channels
	Variable/G root:tim:multievent	=0 // time A in one event, time B in the other 
	Variable/G root:tim:TS500	=1//500 MHz time stamps for P500
	Variable/G root:tim:EvIncr	=2 //P500e: same data in multiple events. E.g. set to 2 for 2-channel data
	variable/G root:tim:chanA	=0 // channel/ module to process (if only one, use A)
	variable/G root:tim:chanB =1
	variable/G root:tim:modA =0
	variable/G root:tim:modB =0
	variable/G  root:tim:defaultTriggerPos = 20		// ignore points before that
	variable/G  root:tim:LB = 12 					// length of baseline sum
	variable/G  root:tim:RTlow = 0.5				// CF threshold
	variable/G  root:tim:Nevents 
	variable/G  root:tim:ElowA
	variable/G  root:tim:EhighA
	variable/G  root:tim:ElowB
	variable/G  root:tim:EhighB
	variable/G  root:tim:RTlowA
	variable/G  root:tim:RThighA
	variable/G  root:tim:RTlowB
	variable/G  root:tim:RThighB
	variable/G  root:tim:cutE
	variable/G  root:tim:cutRT
	variable/G  root:tim:MaxEvents = 10000
	variable/G  root:tim:hist_binmin = -200
	variable/G  root:tim:hist_binsize = 1
	variable/G  root:tim:hist_binnumber =400
	variable/G  root:tim:TSscale = 2		// time stamp time scale in ns
	variable/G  root:tim:WFscale = 2	// waveform sample time scale in ns
	variable/G  root:tim:SlowCFD = 0
	
	Variable/G root:tim:multiTSindex
	
	wave eventvalues = root:tim:eventvalues
	eventvalues = 0
	
	make/o/n=(100) amplitudesA, amplitudesB, cfdA, cfdB, timediff, timediff_cut,energiesA, energiesB, tsA, tsB, rtA, rtB
	make/o/n=(100) tdiffdsp, tdiffdsp_cut,dspdtA, dspdtB
	make/o/n=(100)  timediff, timediff_cut, tdiffdsp, tdiffdsp_cut


	
End


Function Time_Cleanup()	

	make/o/n=(100) amplitudesA, amplitudesB, cfdA, cfdB, timediff, timediff_cut,energiesA, energiesB, tsA, tsB, rtA, rtB
	make/o/n=(100) tdiffdsp, tdiffdsp_cut,dspdtA, dspdtB
	make/o/n=(100)  timediff, timediff_cut, tdiffdsp, tdiffdsp_cut

End


Window Time_Panel() : Panel
	// Check if this panel has already been opened
	DoWindow/F TimingPanel
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	NewPanel/K=1 /W=(150,50,600,570)
	DoWindow/C TimingPanel
	ModifyPanel cbRGB=(51200,57088,45056)
	variable xinp = 10
	variable buttony = 435
	variable controly = 45
	variable boxheight = 400		
	variable xres = 185
	variable xres2 = 185+155
	variable yres = 60
	variable ctrlwidth = 150
	
	
	SetVariable TraceDataFile, value=root:pixie4:DataFile, pos={10,8},size={350,18},title="Data File   "
	SetVariable TraceDataFile, fsize=10,proc=Pixie_Ctrl_CommonSetVariable//,bodywidth=100	
	Button FindTraceDataFile, pos={370,6},size={50,18},proc=Pixie_Ctrl_CommonButton,title="Find",fsize=11


	Groupbox inp, pos={xinp-7,30}, size={165,boxheight}, title="Controls", frame=0
	//Checkbox inp0,pos={xinp,controly},size={90,16},title="Double pulses", variable= root:tim:doublepulse
	//Checkbox inp12,pos={xinp+87,controly},size={90,16},title="old data", variable= root:tim:olddata, fsize = 9
	//Checkbox inp20,pos={xinp,controly+16},size={90,16},title="multi event", variable= root:tim:multievent, fsize = 9
	//Checkbox inp20, help={"ch.A events relative to last ch.B event"}
	//Checkbox inp21,pos={xinp,controly+40},size={90,16},title="500 MHz TS", variable= root:tim:TS500, fsize = 9
	//Checkbox inp21, help={"P500 data with 500 MHz timestamps"}
	SetVariable inp22,pos={xinp,controly},size={ctrlwidth,16},title="Event increment",value= root:tim:EvIncr,limits={1,4,1}
	SetVariable inp22, help={"P500e: same data in multiple events. E.g. set to 2 for 2-channel data. For processing of file only"}
	SetVariable inp23,pos={xinp,controly+20},size={ctrlwidth,16},title="Max. events      ",value= root:tim:Maxevents,limits={1,1e9,1}
	SetVariable inp23, help={"Limit the number of events to process (for larger files)"}
	
	SetVariable inp24,pos={xinp,controly+40},size={ctrlwidth,16},title="Time stamp unit (ns) ",value= root:tim:TSscale
	SetVariable inp24, help={"Time stamp units are 2ns for Pixie-4e (any), 13.333ns for Pixie-4, 1ns for Pixie-Net"}
	SetVariable inp25,pos={xinp,controly+60},size={ctrlwidth,16},title="Sample interval (ns) ",value= root:tim:WFscale
	SetVariable inp25, help={"Waveform sampling intervals are 2ns for Pixie-4e (14/500), 8ns for Pixie-4e (16/125), 4ns for Pixie-Net, 13.333ns for Pixie-4"}
	Checkbox inp35,pos={xinp,controly+80},size={90,16},title=" Slow CFD (125 MHz only)", variable= root:tim:SlowCFD, fsize = 11



	controly = 150
	SetVariable inp3,pos={xinp,controly},size={85,16},title="Mod.  A",value= root:tim:modA,limits={0,3,1}
	SetVariable inp4,pos={xinp+95,controly},size={55,16},title="B",value= root:tim:modB,limits={0,3,1}
	SetVariable inp1,pos={xinp,controly+20},size={85,16},title="Chan. A",value= root:tim:chanA,limits={0,3,1}
	SetVariable inp2,pos={xinp+95,controly+20},size={55,16},title="B",value= root:tim:chanB,limits={0,3,1}
	
	SetVariable inp5,pos={xinp,controly+45},size={ctrlwidth,16},title="Baseline length     ",value= root:tim:LB,limits={1,8000,1}, help={"in cycles"}
	SetVariable inp6,pos={xinp,controly+65},size={ctrlwidth,16},title="CFD threshold      ",value= root:tim:RTlow, help={"e.g. 10 percent = 0.1"}
	SetVariable inp7,pos={xinp,controly+85},size={ctrlwidth,16},title="Skip initial samples",value= root:tim:defaultTriggerPos,limits={1,8000,1}, help={"in cycles"}

	controly = 260
	Checkbox inp17,pos={xinp+110,controly+0},size={90,16},title="cut", variable= root:tim:cutE, fsize = 9
	SetVariable inp8,pos={xinp,controly+0},size={100,16},title="Elow   A",value= root:tim:ElowA,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp9,pos={xinp,controly+20},size={100,16},title="           B",value= root:tim:ElowB,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp10,pos={xinp,controly+40},size={100,16},title="Ehigh  A",value= root:tim:EhighA,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp11,pos={xinp,controly+60},size={100,16},title="           B",value= root:tim:EhighB,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}

	controly = 345
	Checkbox inp18,pos={xinp+110,controly+0},size={90,16},title="cut", variable= root:tim:cutRT, fsize = 9
	SetVariable inp13,pos={xinp,controly+0},size={100,16},title="RTlow   A",value= root:tim:RTlowA,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp14,pos={xinp,controly+20},size={100,16},title="             B",value= root:tim:RTlowB,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp15,pos={xinp,controly+40},size={100,16},title="RThigh  A",value= root:tim:RThighA,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}
	SetVariable inp16,pos={xinp,controly+60},size={100,16},title="             B",value= root:tim:RThighB,limits={0,65535,1}, help={"energy limit for binning dt, in DSP units"}


	Button process, pos = {xinp,buttony}, size = {80,20}, title ="Process", proc = Time_buttons
	Button cuthisto, pos = {xinp,buttony+30}, size = {80,20}, title ="Cut E, histo", proc = Time_buttons
	Button fit, pos = {xinp,buttony+60}, size = {80,20}, title ="Fit histo", proc = Time_buttons
	
	SetVariable inp31,pos={xinp+90,buttony},size={145,16},title="Histo bin size (ns)",value= root:tim:hist_binsize
	SetVariable inp32,pos={xinp+90,buttony+20},size={145,16},title="No. of bins           ",value= root:tim:hist_binnumber
	Valdisplay inp30,pos={xinp+90,buttony+40},size={145,16},title="Start histo at (ns) ",value= (root:tim:hist_binnumber* root:tim:hist_binsize/-2)


	
	Groupbox res, pos={xres-7,30}, size={260,boxheight}, title="Pulse details", frame=0
			
	SetVariable CallReadEvents,pos={xres,55},size={130,18},proc=Time_process_event,title="Event No. "
	SetVariable CallReadEvents,format="%d",fsize=10,fstyle=1//,bodywidth=70
	SetVariable CallReadEvents,limits={0,Inf,1},value= root:pixie4:ChosenEvent
	Button plot, pos = {xres+185,53}, size = {50,18}, title ="Traces", proc = Time_buttons
	

	ctrlwidth= 150
	SetVariable setvar0,pos={xres,30+yres},size={ctrlwidth,16},title="base               A"
	SetVariable setvar0,limits={-inf,inf,0},value= root:tim:Eventvalues[0],noedit= 1, format="%.5g"
	SetVariable setvar0a,pos={xres2,30+yres},size={80,16},title="B"
	SetVariable setvar0a,limits={-inf,inf,0},value= root:tim:Eventvalues[1],noedit= 1, format="%.5g"
	
	SetVariable setvar1,pos={xres,50+yres},size={ctrlwidth,16},title="amplitude      A"
	SetVariable setvar1,limits={-inf,inf,0},value= root:tim:Eventvalues[2],noedit= 1, format="%.5g"
	SetVariable setvar1a,pos={xres2,50+yres},size={80,16},title="B"
	SetVariable setvar1a,limits={-inf,inf,0},value= root:tim:Eventvalues[3],noedit= 1, format="%.5g"
	
	SetVariable setvar3,pos={xres,70+yres},size={ctrlwidth,16},title="energy            A"
	SetVariable setvar3,limits={-inf,inf,0},value= root:tim:Eventvalues[4],noedit= 1, format="%.5g"
	SetVariable setvar3a,pos={xres2,70+yres},size={80,16},title="B"
	SetVariable setvar3a,limits={-inf,inf,0},value= root:tim:Eventvalues[5],noedit= 1, format="%.5g"
	
	SetVariable setvar6,pos={xres,90+yres},size={ctrlwidth,16},title="rise time         A"
	SetVariable setvar6,limits={-inf,inf,0},value= root:tim:Eventvalues[11],noedit= 1, format="%.5g"
	SetVariable setvar6a,pos={xres2,90+yres},size={80,16},title="B"
	SetVariable setvar6a,limits={-inf,inf,0},value= root:tim:Eventvalues[12],noedit= 1, format="%.5g"
	
	SetVariable setvar2,pos={xres,110+yres},size={ctrlwidth,16},title="CFD pos (ns)  A"
	SetVariable setvar2,limits={-inf,inf,0},value= root:tim:Eventvalues[6],noedit= 1, format="%.4f"
	SetVariable setvar2a,pos={xres2,110+yres},size={80,16},title="B"
	SetVariable setvar2a,limits={-inf,inf,0},value= root:tim:Eventvalues[7],noedit= 1, format="%.4f"
	
	SetVariable setvar8,pos={xres,130+yres},size={ctrlwidth,16},title="ev time high   A"
	SetVariable setvar8,limits={-inf,inf,0},value= root:tim:Eventvalues[15],noedit= 1, format="%d"
	SetVariable setvar8a,pos={xres2,130+yres},size={80,16},title="B"
	SetVariable setvar8a,limits={-inf,inf,0},value= root:tim:Eventvalues[16],noedit= 1, format="%d"
	
	SetVariable setvar9,pos={xres,150+yres},size={ctrlwidth,16},title="ev time low    A"
	SetVariable setvar9,limits={-inf,inf,0},value= root:tim:Eventvalues[17],noedit= 1, format="%d"
	SetVariable setvar9a,pos={xres2,150+yres},size={80,16},title="B"
	SetVariable setvar9a,limits={-inf,inf,0},value= root:tim:Eventvalues[18],noedit= 1, format="%d"
	
	SetVariable setvar5,pos={xres,170+yres},size={ctrlwidth,16},title="ch T stamp    A"
	SetVariable setvar5,limits={-inf,inf,0},value= root:tim:Eventvalues[8],noedit= 1, format="%u"
	SetVariable setvar5a,pos={xres2,170+yres},size={80,16},title="B"
	SetVariable setvar5a,limits={-inf,inf,0},value= root:tim:Eventvalues[9],noedit= 1, format="%u"
	
	SetVariable setvar7,pos={xres,190+yres},size={ctrlwidth,16},title="event #          A"
	SetVariable setvar7,limits={-inf,inf,0},value= root:tim:Eventvalues[13],noedit= 1, format="%d"
	SetVariable setvar7a,pos={xres2,190+yres},size={80,16},title="B"
	SetVariable setvar7a,limits={-inf,inf,0},value= root:tim:Eventvalues[14],noedit= 1, format="%d"


	SetVariable setvar4,pos={xres,220+yres},size={190,16},title="T difference B-A (ns) "
	SetVariable setvar4,limits={-inf,inf,0},value= root:tim:Eventvalues[10],noedit= 1, format="%.5g"
	
	SetVariable setvar20,pos={xres,250+yres},size={ctrlwidth,16},title="DSP off (ns)  A"
	SetVariable setvar20,limits={-inf,inf,0},value= root:tim:Eventvalues[20],noedit= 1, format="%.4f"
	SetVariable setvar21,pos={xres+150,250+yres},size={80,16},title="B"
	SetVariable setvar21,limits={-inf,inf,0},value= root:tim:Eventvalues[21],noedit= 1, format="%.4f"
	
	SetVariable setvar22,pos={xres,270+yres},size={ctrlwidth,16},title="DSP  dt (ns)  A"
	SetVariable setvar22,limits={-inf,inf,0},value= root:tim:Eventvalues[22],noedit= 1, format="%.4f"
	SetVariable setvar23,pos={xres2,270+yres},size={80,16},title="B"
	SetVariable setvar23,limits={-inf,inf,0},value= root:tim:Eventvalues[23],noedit= 1, format="%.4f"
	SetVariable setvar24,pos={xres,290+yres},size={190,16},title="DSP T difference  B-A (ns) "
	SetVariable setvar24,limits={-inf,inf,0},value= root:tim:Eventvalues[24],noedit= 1, format="%.5g"

	
	Button ShowHisto, pos = {xres+80,buttony}, size = {70,20}, title ="Show Histo", proc = Time_buttons
	Button ShowTdiff,  pos = {xres+80,buttony+30}, size = {70,20}, title ="Show T diff", proc = Time_buttons
	Button ShowTtable  pos = {xres+80,buttony+60}, size = {70,20}, title ="Show T table", proc = Time_buttons
	Button ShowTdiffvsE,  pos = {xres+160,buttony}, size = {70,20}, title ="T diff vs E", proc = Time_buttons
	Button ShowTdiffvsRT,  pos = {xres+160,buttony+30}, size = {70,20}, title ="T diff vs RT", proc = Time_buttons
	
	
EndMacro


Function Time_buttons(Ctrlname)
String Ctrlname
	
	if(cmpstr(ctrlName, "plot") == 0)
		Execute "Time_plot_traces()"
		return(0)
	endif
	
	if(cmpstr(ctrlName, "process") == 0)
		Time_process_file()
		return(0)
	endif
	
	if(cmpstr(ctrlName, "cuthisto") == 0)
		Time_process_cut()
		Time_process_histo()	
		return(0)
	endif
	
	if(cmpstr(ctrlName, "ShowHisto") == 0)
		Execute "Time_plot_dThisto()"
		return(0)
	endif
	
	if(cmpstr(ctrlName, "ShowTdiff") == 0)
		Execute "Time_plot_Tdiff()"
		return(0)
	endif
	
	if(cmpstr(ctrlName, "ShowTtable") == 0)
		Execute "Time_table()"
		return(0)
	endif
		
	if(cmpstr(ctrlName, "ShowTdiffvsE") == 0)
		Execute "Time_plot_TdiffVsEA()"
		Execute "Time_plot_TdiffVsEB()"
		return(0)
	endif
	
	if(cmpstr(ctrlName, "ShowTdiffvsRT") == 0)
		Execute "Time_plot_TdiffVsRTA()"
		Execute "Time_plot_TdiffVsRTB()"
		return(0)
	endif
	
	if(cmpstr(ctrlName, "fit") == 0)
		wave W_Coef
		Nvar defaultTriggerPos = root:tim:defaultTriggerPos
		Nvar LB =  root:tim:LB //= 12 // length of baseline sum
		Nvar RTlow = root:tim:RTlow //= 0.1
		Execute "Time_plot_dThisto()"
		CurveFit/q/NTHR=0/TBOX=0 gauss  dthisto_cut[pcsr(A),pcsr(B)] /D 
		print "BL, CFD, pos:", LB, RTlow, defaultTriggerPos
		print "[Igor] peak position, with cut (ns):",W_Coef[2], "FWHM (ns):",W_coef[3]*2*sqrt(ln(2))
		CurveFit/q/NTHR=0/TBOX=0 gauss  dthisto[pcsr(A),pcsr(B)] /D 
		print "[Igor] peak position, without cut (ns):",W_Coef[2], "FWHM, (ns):",W_coef[3]*2*sqrt(ln(2))
		CurveFit/q/NTHR=0/TBOX=0 gauss  dtdsphisto_cut[pcsr(A),pcsr(B)] /D 
		print "[DSP] peak position, with cut (ns):",W_Coef[2], "FWHM (ns):",W_coef[3]*2*sqrt(ln(2))
		CurveFit/q/NTHR=0/TBOX=0 gauss  dtdsphisto[pcsr(A),pcsr(B)] /D 
		print "[DSP] peak position, without cut (ns):",W_Coef[2], "FWHM (ns):",W_coef[3]*2*sqrt(ln(2))
	endif
	
	


End


Window Time_plot_TdiffvsEA() : Graph
	DoWindow/F TdiffvsEA
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1 /W=(50,200,400,400) timediff vs energiesA
	DoWindow/C TdiffvsEA
	AppendToGraph timediff_cut vs energiesA
	ModifyGraph mode=2
	ModifyGraph lSize=1.5
	ModifyGraph rgb(timediff)=(0,52224,0)
	ModifyGraph rgb(timediff_cut)=(0,0,0)
	ModifyGraph mirror=2
	Label left "Time difference (ns)"
	Label bottom "Energy A (DSP units)"
	Legend/C/N=text0/A=MC
EndMacro

Window Time_plot_TdiffvsRTA() : Graph
	DoWindow/F TdiffvsRTA
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1 /W=(50,200,400,400) timediff vs rtA
	DoWindow/C TdiffvsRTA
	AppendToGraph timediff_cut vs rtA
	ModifyGraph mode=2
	ModifyGraph lSize=1.5
	ModifyGraph rgb(timediff)=(0,52224,0)
	ModifyGraph rgb(timediff_cut)=(0,0,0)
	ModifyGraph mirror=2
	Label left "Time difference (ns)"
	Label bottom "Rise time A (DSP units)"
	Legend/C/N=text0/A=MC
EndMacro

Window Time_plot_TdiffvsRTB() : Graph
	DoWindow/F TdiffvsRTB
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1 /W=(50,200,400,400) timediff vs rtB
	DoWindow/C TdiffvsRTB
	AppendToGraph timediff_cut vs rtB
	ModifyGraph mode=2
	ModifyGraph lSize=1.5
	ModifyGraph rgb(timediff)=(16384,28160,65280)
	ModifyGraph rgb(timediff_cut)=(0,0,0)
	ModifyGraph mirror=2
	Label left "Time difference (ns)"
	Label bottom "Rise time B (DSP units)"
	Legend/C/N=text0/A=MC
EndMacro

Window Time_plot_TdiffvsEB() : Graph
	DoWindow/F TdiffvsEB
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1 /W=(450,200,800,400) timediff vs energiesB
	DoWindow/C TdiffvsEB
	AppendToGraph timediff_cut vs energiesB
	ModifyGraph mode=2
	ModifyGraph lSize=1.5
	ModifyGraph rgb(timediff)=(16384,28160,65280)
	ModifyGraph rgb(timediff_cut)=(0,0,0)
	ModifyGraph mirror=2
	Label left "Time difference (ns)"
	Label bottom "Energy B (DSP units)"
	Legend/C/N=text0/A=MC
EndMacro



Window Time_plot_traces() : Graph
	DoWindow/F Timingtraces
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	String fldrSav0= GetDataFolder(1)
	SetDataFolder root:tim:
	Display/K=1/W=(400,20,700,220) traceA,triglocsA
	DoWindow/C Timingtraces
	AppendToGraph/L=L1 traceB,triglocsB
	SetDataFolder fldrSav0
	ModifyGraph mirror=2
	ModifyGraph lblPos(left)=52
	ModifyGraph freePos(L1)=0
	ModifyGraph axisEnab(left)={0,0.48}
	ModifyGraph axisEnab(L1)={0.52,1}
	ModifyGraph mode(traceA)=6,rgb(traceA)=(0,52224,0)
	ModifyGraph mode(triglocsA)=3,marker(triglocsA)=26, rgb(triglocsA)=(0,26112,0)
	ModifyGraph mode(traceB)=6,rgb(traceB)=(16384,28160,65280)
	ModifyGraph mode(triglocsB)=3, marker(triglocsB)=26,rgb(triglocsB)=(0,9472,39168)
	SetAxis left 0,16000
	SetAxis L1 0,16000
	ShowInfo
	Legend/C/N=text0/J/A=MC "\\s(traceB) traceB\r\\s(traceA) traceA"
	Label left "ADC steps"
	Label bottom "Time"
EndMacro

Window Time_plot_dThisto() : Graph
	DoWindow/F Timinghisto
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1/W=(400,20,700,220) dthisto, dthisto_cut
	AppendToGraph dtdsphisto, dtdsphisto_cut
	DoWindow/C Timinghisto
	ModifyGraph mirror=2
	ModifyGraph mode=6
	ModifyGraph rgb(dtdsphisto)=(0,0,0)
	ModifyGraph rgb(dtdsphisto_cut)=(0,0,0)
	ModifyGraph lstyle(dthisto_cut)=11,lsize(dthisto_cut)=2,lstyle(dtdsphisto_cut)=11, lsize(dtdsphisto_cut)=2
	ShowInfo
	Legend/C/N=text0/A=MC
	Label left "N events"
	Label bottom "Time difference (ns)"
EndMacro

Window Time_plot_Tdiff() : Graph
	DoWindow/F Tdiff_Event
	if(V_Flag == 1)
		return 0
	endif	
	PauseUpdate; Silent 1		// building window...
	Display/K=1/W=(630,482.75,917.25,650) timediff, timediff_cut
	DoWindow/C Tdiff_Event
	ModifyGraph mode=2
	ModifyGraph mirror=2
	ModifyGraph rgb(timediff_cut)=(0,0,0)
	Label left "Time diff (ns)"
	Label bottom "event number"
	ShowInfo
	Legend/C/N=text0/A=MC
EndMacro

Macro Time_table()
	DoWindow/F CFDtable
	if(V_flag!=1)
		edit/K=1	amplitudesA, amplitudesB, energiesA, energiesB, tsA, tsB, rtA, rtB, cfdA, cfdB, timediff, timediff_cut, dspdtA, dspdtB, tdiffdsp, tdiffdsp_cut
		ModifyTable width=60
	endif

EndMacro


Function Time_process_file()
	
	Nvar  Nevents = root:tim:Nevents 				// number of events
	Wave eventvalues = root:tim:Eventvalues
	Variable ch
	Variable k,m, len,n, index, E, ret, Nbadevents
	String text

	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenEvent = root:pixie4:ChosenEvent
	wave listmodewave = root:pixie4:listmodewave
	Nvar chanA = root:tim:chanA
	Nvar chanB = root:tim:chanB
	Nvar modA = root:tim:modA
	Nvar modB = root:tim:modB
	Nvar doublepulse = root:tim:doublepulse
	Nvar MaxEvents = root:tim:MaxEvents
	
	Nevents=listmodewave[ChosenModule]			// get number of events in file
	Nevents = min(Nevents, MaxEvents)			// limit to user defined max
	
	make/o/n=(nevents) amplitudesA, amplitudesB, cfdA, cfdB, timediff, timediff_cut,energiesA, energiesB, tsA, tsB, rtA, rtB
	make/o/n=(nevents) tdiffdsp, tdiffdsp_cut, dspdtA, dspdtB
	wave amplitudesA
	wave amplitudesB
	wave energiesA
	wave energiesB
	wave cfdA
	wave cfdB
	wave tsA
	wave tsB
	wave timediff
	wave timediff_cut
	wave rtA
	wave rtB
	wave tdiffdsp
	wave tdiffdsp_cut
	wave dspdtA 
	wave dspdtB
	
	amplitudesA = nan
	amplitudesB = nan
	energiesA=nan
	energiesB=nan
	cfdA = nan	
	cfdB = nan	
	tsA=nan
	tsB=nan
	timediff=nan
	timediff_cut=nan	
	rtA=nan
	rtB=nan
	tdiffdsp=nan
	tdiffdsp_cut=nan
	dspdtA =nan
	dspdtB = nan
	
//	make/o/n=(Nevents*4) root:tim:multiTS
//	Wave multiTS = root:tim:multiTS
//	Nvar multiTSindex = root:tim:multiTSindex
	Nvar EvIncr= root:tim:EvIncr
//	multiTSindex =0
//	multiTS = nan
	
	Nbadevents =0
	for(m=0;m<nevents;m+=EvIncr)
		if (mod(m,500)==0)
			DoUpdate
		endif
		ChosenEvent = m
		ret = Time_process_event("",0,"","")
		if(ret==0)
			Nbadevents+=1
		endif
		amplitudesA[m] = Eventvalues[2]
		amplitudesB[m] = Eventvalues[3]
		energiesA[m] = Eventvalues[4]
		energiesB[m] = Eventvalues[5]
		cfdA[m] = Eventvalues[6]
		cfdB[m] = Eventvalues[7]	
		tsA[m]= Eventvalues[8]
		tsB[m]= Eventvalues[9]
		timediff[m]=Eventvalues[10]
		rtA[m] = Eventvalues[11]
		rtB[m] = Eventvalues[12]
		tdiffdsp[m] = Eventvalues[24]
		dspdtA[m] = Eventvalues[22]
		dspdtB[m] =  Eventvalues[23]

	endfor	
	Time_process_cut()
	Time_process_histo()
	
	print "N events without 2 traces", Nbadevents,"out of",nevents
End



Function Time_process_cut()
	
	wave energiesA
	wave energiesB
	wave rtA
	wave rtB
	wave timediff
	wave timediff_cut
	wave tdiffdsp
	wave tdiffdsp_cut
	
	Nvar  Nevents = root:tim:Nevents 
	Nvar ElowA =  root:tim:ElowA
	Nvar EhighA = root:tim:EhighA
	Nvar ElowB = root:tim:ElowB
	Nvar EhighB = root:tim:EhighB
	Nvar RTlowA =  root:tim:RTlowA
	Nvar RThighA = root:tim:RThighA
	Nvar RTlowB = root:tim:RTlowB
	Nvar RThighB = root:tim:RThighB
	Nvar cutRT = root:tim:cutRT
	Nvar cutE = root:tim:cutE
	Nvar EvIncr= root:tim:EvIncr
	
	Variable m, Eok, RTok, cutcount
	timediff_cut = nan
	tdiffdsp_cut = nan
	cutcount=0
	for(m=0;m<nevents;m+=EvIncr)
		Eok=1-cutE	//if cut enabled, default not ok but modified below; in not enabled, default ok
		RTok=1-cutRT
		if ( (energiesA[m]>ElowA) && (energiesA[m]<EhighA) && (energiesB[m]>ElowB) && (energiesB[m]<EhighB) )
			Eok=1
		endif
		
		if ( (rtA[m]>RTlowA) && (rtA[m]<RThighA) && (rtB[m]>RTlowB) && (rtB[m]<RThighB) )
			RTok=1
		endif
		
		if ( (Eok==1) && (RTok==1) )
			timediff_cut[m] =timediff[m]
			tdiffdsp_cut[m] =tdiffdsp[m]
			cutcount+=1
		endif
	endfor

	print "total:",nevents/EvIncr,"  after cut:",cutcount,"fraction:",cutcount/(nevents/EvIncr)

End



Function Time_process_histo()
	
	Nvar  hist_binsize = root:tim:hist_binsize 	
	Nvar  hist_binnumber = root:tim:hist_binnumber 	
	
	wave timediff
	wave timediff_cut
	wave tdiffdsp
	wave tdiffdsp_cut
	
	
	make/o/n=1 dthisto, dthisto_cut
	wave dthisto
	wave dthisto_cut
	dthisto=0
	dthisto_cut=0
	make/o/n=1 dtdsphisto, dtdsphisto_cut
	wave dtdsphisto
	wave dtdsphisto_cut
	dtdsphisto=0
	dtdsphisto_cut=0
	
	Variable binmin = hist_binsize*hist_binnumber/-2

	histogram/A/B={binmin,hist_binsize,hist_binnumber} timediff, dthisto
	histogram/A/B={binmin,hist_binsize,hist_binnumber} timediff_cut, dthisto_cut
	histogram/A/B={binmin,hist_binsize,hist_binnumber}  tdiffdsp, dtdsphisto
	histogram/A/B={binmin,hist_binsize,hist_binnumber}tdiffdsp_cut, dtdsphisto_cut

End



Function Time_process_event(ctrlName,varNum,varStr,varName) : SetVariableControl
	String ctrlName,varStr,varName
	Variable varNum
	
//	Nvar doublepulse = root:tim:doublepulse
//	Nvar olddata = root:tim:olddata
//	Nvar multievent = root:tim:multievent
	Nvar chanA = root:tim:chanA
	Nvar chanB = root:tim:chanB
	Nvar modA = root:tim:modA
	Nvar modB = root:tim:modB
	Nvar ChosenModule = root:pixie4:ChosenModule
	Nvar ChosenChannel = root:pixie4:ChosenChannel
	Nvar ChosenEvent = root:pixie4:ChosenEvent
	Wave ListModeChannelEnergy=root:pixie4:ListModeChannelEnergy
	Wave ListModeChannelTrigger=root:pixie4:ListModeChannelTrigger
	Wave eventvalues = root:tim:Eventvalues
	Nvar EventTimeHI = root:Pixie4:EventTimeHI
	Nvar EventTimeLO = root:Pixie4:EventTimeLO
	Nvar FilterClockMHz = root:pixie4:FilterClockMHz
	Nvar TSscale = root:tim:TSscale 					// time stamp time scale in ns
	Nvar WFscale = root:tim:WFscale 					// waveform sample time scale in ns
	Nvar SlowCFD =  root:tim:SlowCFD

	
//	Wave multiTS = root:tim:multiTS
//	Nvar TS500 = root:tim:TS500
//	Nvar multiTSindex = root:tim:multiTSindex
	
	Variable tscount, evtimensA, dval, evtimensB
	make/o/n=1 clearwave
	
	
	//  ******************* read from file  *******************
	ChosenModule = modA
	ChosenChannel = chanA
	// Set the current module and channel
	Pixie4_Set_Current_ModChan(ChosenChannel,ChosenModule)
	Pixie_IO_ReadEvent()
	//Nvar wftimescale = root:pixie4:wftimescale			// sample clock in MHz as read from the file
	Variable dspcfdA, dspcfdB
	
	// standard event information
	Wave sourceA = $("root:pixie4:trace"+num2str(chanA))
	duplicate/o sourceA, root:tim:traceA
	eventvalues[4] = ListModeChannelEnergy[chanA]
	eventvalues[8] = ListModeChannelTrigger[chanA]//	& 0xFFFFFFF0
	eventvalues[13] = ChosenEvent
	eventvalues[15] = EventTimeHI
	eventvalues[17] = EventTimeLO
	evtimensA = (EventTimeHI*65536+ EventTimeLO) 
	
	// DSP computed CFD values
	Wave eventwave =  root:pixie4:eventwave
	Variable Freltime, Fcfdtime
	
	// channel A DSP CFD
	dspcfdA =  eventwave[7+6+3+chanA*9+3]	//rise time or CFD time from DSP (UserRetVal) = time from cfd point to trigger time latch.
	//if((wftimescale*1e9)<=5)	// comparison wftimescale == 2e-9 fails!
	if(WFscale==2 && TSscale == 2)
		Fcfdtime = (dspcfdA & 0xFF)/256			// 2ns samples P4e
		Freltime = Eventvalues[8]-dspcfdA/256	
		if( (Eventvalues[8] & 0xFF) - floor(dspcfdA/256) < 0 )
			Freltime = Freltime + 256	
		endif
	endif
	if(WFscale==8 && TSscale == 2)
		if(!SlowCFD)													
			Fcfdtime = (dspcfdA & 0x3FF)/1024			// 8ns samples P4e
			Freltime = Eventvalues[8]-dspcfdA/256	
		else
			Fcfdtime = (dspcfdA&0x00FF)/256*WFscale/TSscale	// dsp value is 8bits of TS [8ns] + frac*255 [samples]. scale to match code below
			Freltime = (dspcfdA&0xFF00)/256*WFscale/TSscale	// 
		endif	
	endif	
	if(WFscale==4 && TSscale == 1)
		Fcfdtime = (dspcfdA & 0x3FF)/256/4		// 4ns samples PN
		Freltime =  floor((dspcfdA & 0xFC00) /256/4)		// timestamp of max in units of 4ns
		//Freltime = Eventvalues[8]-dspcfdA/256/4	
	endif

	eventvalues[22] = Fcfdtime*TSscale	//  lower 8 bits are fractional part, save to compare [in ns]
	eventvalues[20] = Freltime*TSscale	// difference to time stamp is true time of cfd crossing [in ns]

	if(modA!=modB)
		ChosenModule = modB
		ChosenChannel = chanB
		// Set the current module and channel
		Pixie4_Set_Current_ModChan(ChosenChannel,ChosenModule)
		Pixie_IO_ReadEvent()
	endif

	Wave sourceB = $("root:pixie4:trace"+num2str(chanB))
	duplicate/o sourceB, root:tim:traceB
	eventvalues[5] = ListModeChannelEnergy[chanB]
	eventvalues[9] = ListModeChannelTrigger[chanB]	//& 0xFFFFFFF0
	
	// channel B DSP CFD
//	if( (multievent) && (ListModeChannelEnergy[chanB]==0) )	// if no energy in channel B, go back to last event with this channel
//		ChosenEvent = eventvalues[14]
//		Pixie_IO_ReadEvent()
//		Wave sourceB = $("root:pixie4:trace"+num2str(chanB))
//		duplicate/o sourceB, root:tim:traceB
//		eventvalues[5] = ListModeChannelEnergy[chanB]
//		eventvalues[9] = (EventTimeHI*65536+ EventTimeLO) 
//		eventvalues[8] = evtimensA
//		ChosenEvent = eventvalues[13]	// restore event number
//		Pixie_IO_ReadEvent()
//	else
		eventvalues[14] = ChosenEvent
		eventvalues[16] = EventTimeHI
		eventvalues[18] = EventTimeLO
//	endif

	Wave eventwave =  root:pixie4:eventwave
	dspcfdB =  eventwave[7+6+3+chanB*9+3]	//rise time or CFD time from DSP (UserRetVal) = time from cfd point to trigger time latch.

	//if((wftimescale*1e9)<=5)	// comparison wftimescale == 2e-9 fails!
	if(WFscale==2 && TSscale == 2)
		Fcfdtime = (dspcfdB & 0xFF)/256			// 2ns samples P4e
		Freltime = Eventvalues[9]-dspcfdB/256	
		if( (Eventvalues[9] & 0xFF) - floor(dspcfdB/256) < 0 )
			Freltime = Freltime + 256	
		endif
	endif
	if(WFscale==8  && TSscale == 2)
		if(!SlowCFD)													
			Fcfdtime = (dspcfdB & 0x3FF)/1024			// 8ns samples P4e
			Freltime = Eventvalues[9]-dspcfdB/256	
		else
			Fcfdtime = (dspcfdB&0x00FF)/256*WFscale/TSscale	// dsp value is 8bits of TS [8ns] + frac*255 [samples]. scale to match code below
			Freltime = (dspcfdB&0xFF00)/256*WFscale/TSscale	// 
		endif	
	endif	
	if(WFscale==4  && TSscale == 1)
		Fcfdtime = (dspcfdB& 0x3FF)/256/4  		// 4ns samples PN
		Freltime =  floor((dspcfdB & 0xFC00) /256/4)		// timestamp of max in units of 4ns  
		//Freltime = Eventvalues[9]-dspcfdB/256/4		
	endif
	
	eventvalues[23] = Fcfdtime*TSscale	//  lower 8 bits are fractional part, save to compare [in ns]
	eventvalues[21] = Freltime*TSscale// difference to time stamp is true time of cfd crossing [in ns]
	
	// DSP time difference
	if(TSscale==2)	// P4
		if(!SlowCFD)									
			eventvalues[24]  = eventvalues[21] - eventvalues[20]
		else
			eventvalues[24]  = eventvalues[23] - eventvalues[22] + eventvalues[21] - eventvalues[20]
		endif
	else				// PN
		Eventvalues[24] = Eventvalues[9] - Eventvalues[8]		// T stamp difference	 (TS units = 1ns)
		Eventvalues[24] += WFscale*Eventvalues[22] - WFscale*Eventvalues[23]	//subtract fractions (sample units = 4ns)
		Eventvalues[24] += WFscale*Eventvalues[20] - WFscale*Eventvalues[21]	//subtract ticks from max to TS (sample units = 4ns)

	endif
	
	
	// ******************* process waveforms *******************
	Nvar defaultTriggerPos = root:tim:defaultTriggerPos
	Nvar LB =  root:tim:LB //= 12 // length of baseline sum
	Nvar RTlow = root:tim:RTlow //= 0.1
	
	Variable maxlocA, maxlocB, npntsA, npntsB, rms, goodevent
	Variable k,j, baseA, baseB, amplA, amplB, lev10,  lev1p, lev1x, lev2p, lev2x
	Variable max1p, max2p, cfdlevel, sep

	Wave traceA = root:tim:traceA
	Wave traceB = root:tim:traceB					
	goodevent = (WaveExists(traceA)==1) && (WaveExists(traceB)==1 )	// only analyze traces that are present

	wavestats/q/z clearwave		// preset V_npnts=1, if wave has zro or does not exist, it's unchanged
	wavestats/q/z	traceA
	npntsA = V_npnts
	wavestats/q/z clearwave		// preset V_npnts=1, if wave has zro or does not exist, it's unchanged
	wavestats/q/z	traceB
	npntsB = V_npnts

	goodevent = goodevent && (npntsA>1) && (npntsB>1)	// only analyze traces that have points

	if(goodevent)	
	
		duplicate/o traceA,  root:tim:triglocsA
		Wave triglocsA =  root:tim:triglocsA
		triglocsA = nan
		duplicate/o traceB, root:tim:triglocsB
		Wave triglocsB = root:tim:triglocsB
		triglocsB = nan
	
		
		// ***************  calculate base and amplitude  ***************
		baseA = 0
		for(j=defaultTriggerPos-LB;j<defaultTriggerPos;j+=1)
			baseA+=traceA[j]
		endfor
		baseA/=LB
		Eventvalues[0] =baseA
		
		baseB = 0
		for(j=defaultTriggerPos-LB;j<defaultTriggerPos;j+=1)
			baseB+=traceB[j]
		endfor
		baseB/=LB
		Eventvalues[1] =baseB
		
		//find max
		wavestats/q/z	traceA
		amplA = V_max-baseA
		Eventvalues[2] =amplA
		maxlocA = x2pnt (traceA, V_maxloc)
		
		wavestats/z/q	traceB
		amplB = V_max-baseB
		Eventvalues[3] =amplB
		maxlocB = x2pnt (traceB, V_maxloc)
		
		
		// ***************  find CFD crossing  ***************
				
		// find first 10% level before max
		cfdlevel = baseA+amplA*RTlow
		findlevel/q/R=[defaultTriggerPos,maxlocA] traceA, cfdlevel // coarse first
		lev1p = x2pnt (traceA, V_levelX)
		
//		if(doublepulse)
//			findlevel/EDGE=1/q/R=[lev1p+6,maxlocA+100] traceB, cfdlevel	//coarse second up to 100 cycles after abs. max
//			lev2p = x2pnt (traceB, V_levelX)
//		else
			cfdlevel = baseB+amplB*RTlow
			findlevel/q/R=[defaultTriggerPos,maxlocB] traceB, cfdlevel // coarse second
			lev2p = x2pnt (traceB, V_levelX)
//		endif
		
		// refine maxima
//		if(doublepulse)
//			wavestats/q/z/R=[lev1p,lev2p] traceA			// find max between first and second rising edge = first max
//			max1p = x2pnt (traceA, V_maxloc)	
//			sep = lev2p+(lev2p-lev1p-5)					// separation is roughly difference between lev1p and lev2p
//			wavestats/q/z/R=[lev2p,sep] traceB				// find max within <sep> cycles after second rising edge = second max
//			max2p = x2pnt (traceB, V_maxloc)
//		else
			wavestats/q/z/R=[lev1p,lev1p+50] traceA			// find maximum within 50 cycles after rising edge 
			max1p = x2pnt (traceA, V_maxloc)				// assumes pulse rise time is less than 50 cycles	
			wavestats/q/z/R=[lev2p,lev2p+50] traceB
			max2p = x2pnt (traceB, V_maxloc)
//		endif
		
		// in case of large bit errors skewing the maximum, exit
	//	if(  ((traceA[max1p]-traceA[max1p-1])>500) || ((traceB[max2p]-traceB[max2p-1])>500) )
	//		eventvalues[6] = nan
	//		eventvalues[7] = nan
	//		eventvalues[10] = nan
	//		return(-2)
	//	endif
		
		// find first crossing with refined maximum
		amplA = traceA[max1p] - baseA
		cfdlevel = baseA+amplA*RTlow
		findlevel/q/R=[defaultTriggerPos,max1p] traceA, cfdlevel // fine first
		lev1p = x2pnt (traceA, V_levelX)
		lev1x = V_levelX	//in x units
		Eventvalues[6] = lev1x  *1e9	//in ns
//		Eventvalues[6] = lev1x  / 8e-9 - floor(lev1x  / 8e-9)	//in samples
		triglocsA[lev1p] = traceA[lev1p]		// mark waveform
		//TriggerPos = lev1p
		
		
		// find second 10% level (after first pulse if double pulse)
		amplB = traceB[max2p] - baseB
		cfdlevel = baseB+amplB*RTlow
//		if(doublepulse)
//			findlevel/EDGE=1/q/R=[lev1p+6,max2p] traceB, cfdlevel	//fine second: the rising edge after the first edge but before the second max 
//		else
			findlevel/q/R=[defaultTriggerPos,max2p] traceB, cfdlevel // fine second
//		endif
		lev2p = x2pnt (traceB, V_levelX)
		lev2x = V_levelX
		Eventvalues[7] = lev2x *1e9	//in ns
//		Eventvalues[7] = lev2x  / 8e-9 - floor(lev2x  / 8e-9)	//in samples
		triglocsB[lev2p] = traceB[lev2p]		// mark waveform
		
		
//		// multipulse trace
//		Wave W_FindLevels
//		if((eventvalues[4]>50) && (eventvalues[5]>50))
//			findlevels/q/edge = 1 traceB, cfdlevel
//			if (V_LevelsFound<7)
//				for(tscount=0;tscount<V_LevelsFound;tscount+=1)
//					multiTS[multiTSindex] =  (eventvalues[9] - eventvalues[8])*(1000/FilterClockMHz) + ( W_FindLevels[tscount] - lev1x)  *1e9
//					multiTSindex+=1
//				endfor
//			endif
//		endif	
		
		// compute diff
//		if(multievent)
//			Eventvalues[10] = (eventvalues[8] - eventvalues[9])*(1000/75) + ( lev1x - lev2x)  *1e9
//			//based on event time stamps, B is generally smaller
//		else
//			tscale = 1000/FilterClockMHz
//			if (TS500)
//				tscale = tscale/4
//			endif
			//Eventvalues[10] = (eventvalues[9] - eventvalues[8])*TSscale + ( lev2x - lev1x)  *1e9	//in ns
			//Eventvalues[10] = (eventvalues[9]&0xFFFFFFFF - eventvalues[8]&0xFFFFFFFF)*TSscale //+ ( lev2x - lev1x)  *1e9	//in ns
			// based on channel time stamps. does not take into account rollovers from 64K to 0, but that is very rare
//		endif

		if(WFscale==2 && TSscale == 2)		// P4e 500
			Eventvalues[10] = (eventvalues[9] - eventvalues[8])*TSscale  + ( lev2x - lev1x)  *1e9	//in ns
		endif
		if(WFscale==8  && TSscale == 2)		// P4e 125
				Eventvalues[10] = ((eventvalues[9] & 0xFFFFFFE0) - (eventvalues[8] & 0xFFFFFFE0))*TSscale //+ ( lev2x - lev1x)  *1e9	//in ns
				Eventvalues[10] = eventvalues[10] + ( lev2x - lev1x)  *1e9	//in ns
		endif	
		if(WFscale==4  && TSscale == 1)		// PN 250
			Eventvalues[10] = (eventvalues[9] - eventvalues[8])*TSscale + ( lev2x - lev1x)  *1e9	//in ns
		endif
		
		//find first rise time
		cfdlevel = baseA+amplA*0.1
		findlevel/q/R=[defaultTriggerPos,max1p] traceA, cfdlevel // fine first
		lev1x = V_levelX	//in x units
		cfdlevel = baseA+amplA*0.9
		findlevel/q/R=[defaultTriggerPos,max1p] traceA, cfdlevel // fine first
		Eventvalues[11] = (V_levelX - lev1x) *1e9 // in ns
		
		//find second rise time
		cfdlevel = baseB+amplB*0.1
//		if(doublepulse)
//			findlevel/EDGE=1/q/R=[lev1p+6,max2p] traceB, cfdlevel	//fine second: the rising edge after the first edge but before the second max 
//		else
			findlevel/q/R=[defaultTriggerPos,max2p] traceB, cfdlevel // fine second
//		endif
		lev2x = V_levelX	//in x units
		cfdlevel = baseB+amplB*0.9
//		if(doublepulse)
//			findlevel/EDGE=1/q/R=[lev1p+6,max2p] traceB, cfdlevel	//fine second: the rising edge after the first edge but before the second max 
//		else
			findlevel/q/R=[defaultTriggerPos,max2p] traceB, cfdlevel // fine second
//		endif
		Eventvalues[12] = (V_levelX - lev2x) *1e9 // in ns
		
	else
	
		eventvalues[0] = nan
		eventvalues[1] = nan
		eventvalues[2] = nan
		eventvalues[3] = nan
		eventvalues[6] = nan
		eventvalues[7] = nan
		eventvalues[10] = nan
		eventvalues[11] = nan
		eventvalues[12] = nan
		
	endif
	

	
	return(goodevent)
	
End











//
//Function Dblpulse(npnts, base)		// not updated for tim yet
//Variable npnts, base
//
//	Nvar SL
//	Nvar SG
//	Nvar PWchannel = root:PW:PWchannel
//	Wave P500trace =  $("root:pixie4:trace"+num2str(PWchannel))
//	Nvar TriggerPos = root:TriggerPos		// first trigger in trace after default position
//	Nvar defaultTriggerPos
//	Nvar RTlow = root:PW:RTlow //= 0.1
//	Nvar LB =  root:PW:LB 
//	Nvar RemovePileup	 = root:RemovePileup
//	
//	variable k,  j , ampl, THlevel, smoothlength, off, numTriggers, maxloc
//	variable x0,x1, x2,x3, badtrigpos,m, firsttrig, maxdist, amplhigh
//	smoothlength = 125
//	
//	
//	Nvar FL
//	Nvar FG
//	Nvar Threshold 
//	
//	duplicate/o P500trace, ff
//	Wave ff
//	duplicate/o P500trace, triglocs
//	Wave triglocs
//	triglocs = nan
//	wavestats/q P500trace
//	maxloc = x2pnt(P500trace, V_maxloc)
//	
//	make/o/n=30 triggers, trigdt, amplitudes
//	Wave triggers
//	Wave trigdt
////	Wave RTlimits
//	Wave amplitudes
//	
//	trigdt=nan
//	ff=nan
//	amplitudes = nan
//	k=1
//	do
//		//ff[k]=sum(P500trace,x0,x1)-sum(P500trace,x2,x3)
//		ff[k]  = P500trace[k]-P500trace[k-1] +P500trace[k+1]-P500trace[k-2]
//		k+=1
//	while(k<npnts)
//
//	k=0
//	m=0
//	badtrigpos =0
//	triggers=nan
//	numTriggers = 0
//	firsttrig=-1
//	// find main trigger with 5x threshold
//	do
//		if( ff[k] > Threshold*5) 
//			if(  k >defaultTriggerPos  )
//				firsttrig =k
//				triggers[m]= k
//				numTriggers += 1
//				// find max for this trigger
//				j=k
//				amplhigh=0
//				do
//					amplhigh=P500trace[j]-base
//					j+=1
//				while ( ((P500trace[j] - P500trace[j-1]) >0) && (j< npnts) )
//				amplitudes[m] = amplhigh
//				// loop until ff[k] lower than TriggerThreshold again
//				do
//					k += 1
//				while( (ff[k] > (Threshold)) && (k < (npnts-(2*FL+FG))))
//			else 
//				k+=1
//			endif
//		else
//			k += 1
//		endif	
//				
//	while(k < (npnts-(2*FL+FG))  && (firsttrig==-1) )
//	
//	// find secondary triggers
//	//k=0
//	m+=1
//	do
//		if( ff[k] > Threshold) 
//			numTriggers += 1
//		//	triggers[m]= floor(k	-FL/2	)	
//			triggers[m]= k
//			
//			// find max for this trigger
//			j=k
//			amplhigh=0
//			do
//				amplhigh=P500trace[j]-base
//				j+=1
//			while ( ((P500trace[j] - P500trace[j-1]) >0) && (j< npnts) )
//			amplitudes[m] = amplhigh
//			m+=1
//			// loop until ff[k] lower than TriggerThreshold again
//			do
//				k += 1
//			while( (ff[k] > (Threshold)) && (k < (npnts-(2*FL+FG))))
//		else
//			k += 1
//		endif	
//				
//	while(k < (npnts-(2*FL+FG)))
//	
//	
//	// mark traces
//	for (m=0;m<numTriggers; m+=1)
//	//	if (abs(triggers[m]-TriggerPos) > FL)
//			triglocs[triggers[m]] = P500trace[triggers[m]]
//	//	endif
//	endfor
//	
//	
//	
//	k=0
//	maxdist=0
////	firsttrig = -1
//	if(numTriggers>1 && ( firsttrig>=0) )
//	//	do
//	//		if ( triggers[k] >defaultTriggerPos)
//	//			firsttrig = k
//	//		endif
//	//		k+=1
//		
//	//	while( (firsttrig==-1) && (k<numTriggers) )
//	
//	//	for(k=firsttrig+1; k<numTriggers;k+=1)
//		for(k=1; k<numTriggers;k+=1)
//			if(triggers[k] > firsttrig)
//			//	trigdt[k] = triggers[k]-triggers[firsttrig]
//				trigdt[k] = triggers[k]-firsttrig
//			endif
//							
//		endfor
//	
//	
//	endif
//	
//	return (numTriggers-1)
//
//End