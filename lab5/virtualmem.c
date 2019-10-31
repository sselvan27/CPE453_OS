#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define TABLE_SIZE 16
#define P_SIZE 256
#define F_SIZE 256
#define PM_SIZE P_SIZE*F_SIZE

// Translation lookaside buffer struct
struct TLBtable {
	unsigned int pgNum;
	unsigned int fNum;
};


int main (int argc, char *argv[])
{
	// Local variables
	int logAddr = 0;
	int offsetNum = 0;
	int pgNumber = 0;
	int psyAddr = 0;
	int frame = 0;
	int value = 0;
	int hit = 0;
	int TLB_index = 0;
	int TLB_size = 0;

	// Bit masking variables
	unsigned pgNumberMask = 65280; //1111111100000000
	unsigned offsetMask = 255; //11111111

	// Counter variables
	int TLB_hitcount = 0;
	int addrCount = 0;
	int pg_faultcount = 0;

	// Check args for test file
	if (argc != 2)
	{
		fprintf(stderr, "Usage ./virtualmem <Filename.txt> \n");
		exit(1);
	}

	// Open input files
	// Create output file descriptors
	FILE *addresses = fopen(argv[1],"r");
	FILE *BACKINGSTORE = fopen("BACKING_STORE.bin", "rb");
	FILE *output = fopen("output.txt", "w");

	// Static memory allocation
	int pMemory[PM_SIZE];
	char buf[F_SIZE];
	int index;

	// Initialize array to default -1 and create physical table
	int pTable[P_SIZE];
	memset(pTable, -1, P_SIZE*sizeof(int));

	// Initiliaze array to default -1 and create TLB struct
	struct TLBtable tlb[TABLE_SIZE];
	memset (pTable, -1, TABLE_SIZE*sizeof(char));

	// Loop through addresses in addresses.txt file
	while(fscanf(addresses, "%d", &logAddr) == 1)
	{
		addrCount++;

		// Modify the page number and offset for each logical address
		pgNumber = logAddr & pgNumberMask;
		pgNumber = pgNumber >> 8;
		offsetNum = logAddr & offsetMask;
		hit = -1;

		// Check for reoccuring page number
		// Duplicate page number is a hit
		for(index = 0; index < TLB_size; index++)
		{
			if(tlb[index].pgNum == pgNumber)
			{
				hit = tlb[index].fNum;
				psyAddr = hit*F_SIZE + offsetNum;
			}
		}

		if(!(hit == -1))
		{
			TLB_hitcount++;
		}

		// If it is a miss, get physical page number
		else if(pTable[pgNumber] == -1)
		{
			fseek(BACKINGSTORE, pgNumber*256, SEEK_SET);
			fread(buf, sizeof(char), 256, BACKINGSTORE);
			pTable[pgNumber] = frame;

			for(index = 0; index < 256; index++)
			{
				pMemory[frame*F_SIZE + index] = buf[index];
			}

			pg_faultcount++;
			frame++;

			//FIFO for TLB
			if(TLB_size == 16)
				TLB_size--;

			for(TLB_index = TLB_size; TLB_index > 0; TLB_index--)
			{
				tlb[TLB_index].pgNum = tlb[TLB_index-1].pgNum;
				tlb[TLB_index].fNum = tlb[TLB_index-1].fNum;
			}

			if (TLB_size <= 15)
				TLB_size++;

			tlb[0].pgNum = pgNumber;
			tlb[0].fNum = pTable[pgNumber];
			psyAddr = pTable[pgNumber]*P_SIZE + offsetNum;
		}
		else
		{
			psyAddr = pTable[pgNumber]*256 + offsetNum;
		}

		//Gets the value from the bin file provided
		value = pMemory[psyAddr];
		//print the addresses and value to output.txt
		fprintf(output, "Virtual address: %d Physical address: %d Value: %d\n", logAddr, psyAddr, value);
	}

//Close input files
fclose(addresses);
fclose(BACKINGSTORE);

//Close output file
fclose(output);

return 0;

}