/* 
 * An example of 0x7030 mode.
*/

#include "reader.h"

int main(int argc, char *argv[])
{
	U32 *UserData = NULL;
	U32 i;
	U64 NumEvents;
	char *filename;

    AutoProcessLMData = 3; // only 3 for this mode
	// To force debug messages printout
	//PrintDebugMsg_other = 1;
	//PrintDebugMsg_QCdetail = 1;
	//PrintDebugMsg_QCerror = 1;

    if (argc != 2) {
        printf("Provide file name as argument\n");
        return (1);
    }

    filename = malloc(256*sizeof(char));

	/* Allocate memory for the user array.
	 * Depending on the task the user array must have an appropriate size.
	 * In case of 0x7030 the size is 17 (10 input parameters, 7 output values--used internally).
     */

	UserData = calloc(17, sizeof(U32));
	if (UserData == NULL) {
		printf("Cannot allocate memory\n");
	}

    /* PSA control inputs:
     * UserData[1]   LoQ0     sum0 length
     * UserData[2]   LoQ1     sum1 length
     * UserData[3]   SoQ0     sum0 start
     * UserData[4]   SoQ1     sum1 start
     * UserData[5]   RTlow    rise-time start, % (10)
     * UserData[6]   RThigh   rise-time stop, % (90)
     * UserData[7]   PSAoption    0: Q1/Q0, 1: (Q1-Q0)/Q0
     * UserData[8]   PSAdiv8      1: divide PSA by 8
     * UserData[9]   PSAletrig    1: use leading edge trigger
     * UserData[10]  PSAth        trigger threshold, ADC steps (LE)    
     */
    UserData[1] = 12;
    UserData[2] = 64;
    UserData[3] = 0;
    UserData[4] = 32;
    UserData[5] = 10;
    UserData[6] = 90;
    UserData[7] = 0;
    UserData[8] = 0;
    UserData[9] = 0;
    UserData[10] = 10;

    filename = argv[1];
	Pixie_Acquire_Data(0x7030, UserData, filename, 0); // get traces
	NumEvents = UserData[0];
	printf("0x7030 done, processed %lu events\n", NumEvents);

	if (UserData != NULL) free(UserData);
	/* Done */
	return(0);
}
