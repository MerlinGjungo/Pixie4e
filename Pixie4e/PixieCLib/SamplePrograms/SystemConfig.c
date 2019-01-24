/**************************************************************************/
/*	SystemConfig.c							  */
/*		last updated: 4/05/11 DB				  */
/*									  */
/*	This is a common piece of code for all sample programs 		  */
/*	containing system configuration variables.		          */
/*									  */
/**************************************************************************/
 
	SystemParameterValues[Pixie_Get_Par_Idx("NUMBER_MODULES", "SYSTEM")] = NumberOfModules;
	SystemParameterValues[Pixie_Get_Par_Idx("OFFLINE_ANALYSIS",   "SYSTEM")] = 0;
	SystemParameterValues[Pixie_Get_Par_Idx("AUTO_PROCESSLMDATA", "SYSTEM")] = 0;
	SystemParameterValues[Pixie_Get_Par_Idx("MAX_NUMBER_MODULES", "SYSTEM")] = 7;//13;
	SystemParameterValues[Pixie_Get_Par_Idx("KEEP_CW",            "SYSTEM")] = 1;
	/* The PXI crate slot number where the Pixie module is installed */
	i = Pixie_Get_Par_Idx("SLOT_WAVE", "SYSTEM");
	for (j = 0; j < NumberOfModules; j++) SystemParameterValues[i++] = Slots[j];
	/* Download boot file names */
	Pixie_Hand_Down_Names(boot_file_names, "ALL_FILES");
	/* Call library function Pixie_User_Par_IO to initialize SystemParameterValues */
	Pixie_User_Par_IO(SystemParameterValues, "NUMBER_MODULES",     "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "OFFLINE_ANALYSIS",   "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "AUTO_PROCESSLMDATA", "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "MAX_NUMBER_MODULES", "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "SLOT_WAVE",          "SYSTEM", MOD_WRITE, 0, 0);
	Pixie_User_Par_IO(SystemParameterValues, "KEEP_CW",            "SYSTEM", MOD_WRITE, 0, 0);
	
