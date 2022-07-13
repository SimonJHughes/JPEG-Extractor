#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//Function prototypes
bool checkBytes(unsigned char[4]);
void extractImages(long int[], int, FILE*);

//Takes in filename, finds positions where a JPEG magic bit are
void getPositions(char fileName[])
{
	unsigned char bytes[4];
	long int goodPositions[100];
	int counter = 0;

	//Open a file in read binary mode
	FILE *fPtr;
	fPtr = fopen(fileName, "rb");

	//While there are still bytes to read in the file
	while(fread(bytes, sizeof(bytes), 1, fPtr))
	{
		//If bytes read match magic bits
		if(checkBytes(bytes))
		{
			//Add the position to goodPositions array
			printf("Good byte at %lx \n", ftell(fPtr) - 4);
			goodPositions[counter] = ftell(fPtr) - 4;
			counter++;
		}
	}

	//Store last position of file
	fseek(fPtr, 0, SEEK_END);
	goodPositions[counter] = ftell(fPtr);

	//Call to extract images
	extractImages(goodPositions, counter, fPtr);
}

//Takes in four bytes and returns true if bytes are JPEG magic bytes, false if otherwise
bool checkBytes(unsigned char inBytes[4])
{
	if(inBytes[0] == 0xff && inBytes[1] == 0xd8 && inBytes[2] == 0xff && (inBytes[3] == 0xe0 || inBytes[3] == 0xe1))
	{
		return true;
	}
	return false;
}

//Takes in array of good positions, size of the array, and a file pointer
void extractImages(long int input[], int size, FILE *ptr)
{
	unsigned int i;
	unsigned char buffer[512];
	char outputFileName[] = "image0.png";

	//For each position in good position array
	for(i = 0; i < size - 1; i++)
	{
		//Set up files to write to
		FILE *wPtr;
		outputFileName[5] = (i + 17) + '0';
		wPtr = fopen(outputFileName, "wb");

		//Goes to each position beginning with magic bytes
		fseek(ptr, input[i], SEEK_SET);
		
		//While the read position is not another magic byte, copy contents from read file to write file
		while(ftell(ptr) != input[i+1])
		{
			fread(buffer, sizeof(buffer), 1, ptr);
			fwrite(buffer, sizeof(buffer), 1, wPtr);
		}
	}
}