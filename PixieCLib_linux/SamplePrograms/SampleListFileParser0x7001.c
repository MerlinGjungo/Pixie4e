/* 
 * An example of basic binary file processing: mode 0x7001.
*/

#include "reader.h"

int main(int argc, char *argv[])
{
	U32 *UserData = NULL;
	U32 i;
	U64 NumEvents;
	char *filename;
	// To force debug messages printout
	PrintDebugMsg_other = 1;
	PrintDebugMsg_QCdetail = 1;
	PrintDebugMsg_QCerror = 1;

    if (argc != 3) {
        printf("Provide file name and AutoProcessLMData as arguments\n");
        return (1);
    }

    filename = malloc(256*sizeof(char));

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
    filename = argv[1];
	AutoProcessLMData = atoi(argv[2]); 
	Pixie_List_Mode_Parser(filename, UserData, 0x7001);
	printf("0x7001 done, events found %d\n", UserData[0]);
	NumEvents = UserData[0];

    /*
     * For parsing Pixie4 .bin files use
     * Pixie_Parser_List_Mode_Events(filename, UserData)
     */

	if (UserData != NULL) free(UserData);
    free(filename);
	/* Done */
	return(0);
}
