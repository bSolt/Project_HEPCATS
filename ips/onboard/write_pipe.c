#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if( argc < 3 )
	{
		printf("[C] Must pass propper input image and pipe to program!\n");
		printf("[C] Usage: ./write_img [IMG_PATH] [PIPE_PATH]\n");
		return -1;
	}
	// printf("%d",argc);
	const char* PIPE = argv[2];
	printf("[C] Using %s as the pipe",PIPE);
	
	// Clear out output pipe first


	// Open input pipe
	int pid = open(PIPE, O_RDWR);
	if( pid>0 )
	{
		printf("[C] Pipe opened for writing, with id:%d\n", pid );	
	}
	else
	{
		printf("[C] Error, pipe was opened with a problem (pid=%d)\n", pid);
		perror("Error Code");
		return 1;
	}

	// We now want to read from a RAW image file and then write the bytes to the message pipe

	char* DATA = argv[1];
	FILE* dti = fopen(DATA, "rb");
	if(!dti)
		perror("[C] DATA file could not be opened");
	char* buffer;
	long filelen;
	fseek(dti,0,SEEK_END);
	filelen = ftell(dti);
	rewind(dti);
	// Add an EOF to the end of the buffer.
	// buffer[filelen+1] = 0x00;
	buffer = (char *) malloc((filelen+1)*sizeof(char)); // this will allocate just enough memory for the file
	fread(buffer, filelen,1,dti);
	int result = fclose(dti);
	printf("[C] DATA from %s was read successfully! length = %ld bytes\n",DATA, filelen);
	if( result < 0 )
		perror("[C] DATA file could not close");


	// int r_out = read(dti, &buf, n_bytes);
	// if( r_out<0 )
	// 	perror("DATA could not be read");
	// printf("r_out = %d\t",r_out);

	int w_out = write(pid, buffer, (filelen)*sizeof(char));
	if (w_out<0)
	{
		printf("[C] DATA could not be written to %d:%s\n",pid,PIPE);
		perror("[C] Error Code");
		return 1;
	}
	printf("[C] w_out = %d\n",w_out);

// Ben says this is unnecesary (and he's right)
	// result = close(pid);
	// if(result<0) 
	// 	perror("[C] Error when closing write pipe");
	// pid = open(PIPE, O_RDONLY);
	// if( pid>0 )
	// {
	// 	printf("[C] Pipe opened for reading with id:%d\n", pid );	
	// }
	// else
	// {
	// 	printf("[C] Error, pipe was opened with a problem (pid=%d)\n", pid);
	// 	perror("Error Code");
	// 	return 1;
	// }

	// wait a second
	sleep(1);

	printf("[C] Attmepting to read %s\n",PIPE);
	// Read from pipe
	uint32_t n_bytes;
	// First read in the size of the buffer needed
	read(pid, &n_bytes, 4);
	if(n_bytes>0)
	{
		// allocate the buffer
		char* m = malloc(n_bytes);
		// read into the buffer
		size_t sz = read(pid, m, n_bytes);
		if ((int)sz < 0)
			perror("[C] Error when reading from output pipe");
		else
			printf("[C] %ld bytes were read from output pipe:\n", sz);

		printf("[C] %s\n",(char*) m);

		free(m);
	}
	else
		printf("[C] EOF read, No Aurora Detected.\n");
	
	// Close pipe when done
	result = close(pid);
	if(result<0) 
		perror("[C] Error when closing read pipe");

	// Remember not to cause memory leaks.
	free(buffer);
	

	return 0;
}

// Command to create test pipe:
// mkfifo pipe.fifo

// FILE *fileptr;
// char *buffer;
// long filelen;

// fileptr = fopen("myfile.txt", "rb");  // Open the file in binary mode
// fseek(fileptr, 0, SEEK_END);          // Jump to the end of the file
// filelen = ftell(fileptr);             // Get the current byte offset in the file
// rewind(fileptr);                      // Jump back to the beginning of the file

// buffer = (char *)malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
// fread(buffer, filelen, 1, fileptr); // Read in the entire file
// fclose(fileptr); // Close the file