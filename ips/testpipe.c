#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[])
{
	extern int errno;
	// printf("%d",argc);
	if( argc<3 )
	{
		printf("Must specify 2 pipes as input to program!\n");
		return(-1);
	}
	const char* PIPE_IN = argv[1];
	const char* PIPE_OUT = argv[2];
	printf("Using %s as the input pipe\nUsing %s as the output pipe\n",PIPE_IN, PIPE_OUT);
	// Open pipe
	int id_in = open(PIPE_IN, O_WRONLY);
	if( id_in>0 )
	{
		printf("Input pipe opened with id:%d\n", id_in );	
	}
	else
	{
		printf("Error, pipe was opened with a problem (pid=%d)\n", id_in);
	}

	// Write a simple message to the pipe

	char* DATA = "pipes.txt";
	int img = open(DATA, O_RDONLY);
	printf("DATA file opened with id:%d\n", img );	
	size_t n_bytes = 5000;
	char buf[n_bytes];

	int r_out = read(img, &buf, n_bytes);
	if( r_out<0 )
		perror("DATA could not be read");
	printf("r_out = %d\t",r_out);

	int w_out = write(id_in, &buf, r_out);
	if (w_out<0)
	{
		printf("DATA could not be written to %d:%s\n",id_in,PIPE_IN);
		perror("Error Code");
	}
	printf("w_out = %d\n",w_out);

	int result;

	result = close(img);
	if(result<0) 
		perror("Error when closing image file");

	// result = close(pid);
	// if(result == 0)
	// {
	// 	fprintf(stdout, "Input Pipe closed successfuly!\n");
	// }
	// else
	// {
	// 	fprintf(stdout, "Error: Input Pipe could not be closed successfuly.\n");
	// }

	int id_ou = open(PIPE_OUT, O_RDONLY);
	if( id_ou < 0 )
		perror("Error when opening output pipe");
	else
		printf("Output pipe opened as %d\n", id_ou);
	// Read from pipe
	void *m = malloc(n_bytes);
	size_t sz = read(id_ou, m, n_bytes);
	if ((int)sz < 0)
		perror("Error when reading from output pipe");
	else
		printf("%ld bytes were read from output pipe:\n", sz);

	printf("%s\n",(char*) m);
	// Close pipe when done
	result = close(id_in);
	if(result<0) 
		perror("Error when closing input pipe");
	result = close(id_ou);
	if(result<0) 
		perror("Error when closing output pipe");
	

	return 1;
}

// Command to create test pipe:
// socat -d -d pty,raw,echo=0 pty,raw,echo=0