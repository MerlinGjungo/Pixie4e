

#include "reader.h"

int main(int argc, char *argv[])
{
	U32 *UserData = NULL;
	U32 i;
	U64 NumEvents;
//	char filename[128] = "C:\\XIA\\P4eSW\\PulseShape\\killme0003.b00";
	char filename[128] = "ListModeData.b00";
	// To force debug messages printout
	PrintDebugMsg_other = 1;
	PrintDebugMsg_QCdetail = 1;
	PrintDebugMsg_QCerror = 1;

	/* Allocate memory for the user array.
	 * Depending on the task the user array must have an appropriate size.
	 * In case of 0x7001 the size is 17 for the maximum number of mudules in the system */
	UserData = calloc(PRESET_MAX_MODULES + 1, sizeof(U32));
	if (UserData == NULL) {
		printf("Cannot allocate memory\n");
	}
	/* Task 0x7001 is used primarily to count events in the binary list mode file. 
	 * This number is needed by other tasks. In this case AutoProcessLMData = 0. 
	 * To produce text output AutoProcessLMData must be 1 or greater according to the documentation */
	AutoProcessLMData = 2; 
	//Pixie_List_Mode_Parser(filename, UserData, 0x7001); // parse headers
	//printf("0x7001 done, events found %d\n", UserData[0]);
	//NumEvents = UserData[0];


	// See reader.c for details of this new task 0x7011 (works only for run type 0x400)
	// Data is written out in .out file: two lines per event.
	// Line one: event number, channel number, time-stamp low word, time-stamp mid word, time-stamp high word, number of waveform samples to follow
	// Line two: waveform samples (16-bit words)
	//UserData[<module number>] will contain number of processed events

	Pixie_List_Mode_Parser(filename, UserData, 0x7011); // get traces
	NumEvents = UserData[0];
	printf("0x7011 done, processed %lu events\n", NumEvents);

	if (UserData != NULL) free(UserData);
	/* Done */
	return(0);
}
