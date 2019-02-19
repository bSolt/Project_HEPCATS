#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	if( argc < 4 )
	{
		printf("[C] Must pass propper input image and pipe to program!\n");
		printf("[C] Usage: ./write_img [IMG_PATH] [PIPE_IN_PATH] [PIPE_OUT_PATH]\n");
		return -1;
	}
	// printf("%d",argc);
	const char* PIPE_IN = argv[2];
	const char* PIPE_OUT = argv[3];
	printf("[C] Using %s as the input pipe\nUsing %s as the ouput pipe\n",PIPE_IN,PIPE_OUT);
	
	// Clear out output pipe first


	// Open input pipe
	int id_in = open(PIPE_IN, O_WRONLY);
	if( id_in>0 )
	{
		printf("[C] Input pipe opened with id:%d\n", id_in );	
	}
	else
	{
		printf("[C] Error, pipe was opened with a problem (pid=%d)\n", id_in);
		perror("Error Code");
		return -1;
	}

	// We now want to read from a RAW image file and then write the bytes to the message pipe

	char* DATA = argv[1];
	FILE* dti = fopen(DATA, "rb");
	if(dti)
		printf("[C] DATA file, %s opened at loc:0x%x\n", DATA, dti );	
	else
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
	if( result < 0 )
		perror("[C] DATA file could not close");


	// int r_out = read(dti, &buf, n_bytes);
	// if( r_out<0 )
	// 	perror("DATA could not be read");
	// printf("r_out = %d\t",r_out);

	int w_out = write(id_in, buffer, (filelen)*sizeof(char));
	if (w_out<0)
	{
		printf("[C] DATA could not be written to %d:%s\n",id_in,PIPE_IN);
		perror("[C] Error Code");
		return -1;
	}
	printf("[C] w_out = %d\n",w_out);

	result = close(id_in);
	if(result<0) 
		perror("[C] Error when closing input pipe");

	sleep(1);
	printf("[C] Attmepting to read %s\n",PIPE_OUT);
	//read from output pipe
	int id_ou = open(PIPE_OUT, O_RDONLY);
	if( id_ou < 0 )
		perror("[C] Error when opening output pipe");
	else
		printf("[C] Output pipe opened as %d\n", id_ou);
	// Read from pipe
	size_t n_bytes = 1000;

	void *m = calloc(n_bytes,sizeof(char));

	size_t sz = read(id_ou, m, n_bytes);
	if ((int)sz < 0)
		perror("[C] Error when reading from output pipe");
	else
		printf("[C] %ld bytes were read from output pipe:\n", sz);

	printf("[C] %s\n",(char*) m);
	// Close pipe when done
	result = close(id_ou);
	if(result<0) 
		perror("[C] Error when closing output pipe");
	

	return 1;
}

// Command to create test pipe:
// socat -d -d pty,raw,echo=0 pty,raw,echo=0

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