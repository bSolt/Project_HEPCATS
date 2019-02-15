#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
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
		printf("Error, pipe was opened with a probelm (pid=%d)\n", id_in);
	}

	// Write a simple message to the pipe

	char* IMAGE = "mro.jpg";
	int img = open(IMAGE, O_RDONLY);
	printf("Image file opened with id:%d\n", img );	
	void *buf;

	size_t r_out = read(img, buf, 0);
	printf("r_out = %ld\t",r_out);

	size_t w_out = write(id_in, buf, r_out);
	printf("w_out = %ld\n",w_out);

	int result;

	result = close(img);
	if(result<0) 
		printf("Error when closing image file\n");

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
	printf("Output pipe opened as %d\n", id_ou);
	// Read from pipe
	void *m = calloc(100,sizeof(char));
	size_t sz = read(id_ou, m, 100);
	printf("%ld bytes were read from pipe.\n", sz);

	// Close pipe when done
	result = close(id_in);
	if(result<0) 
		printf("Error when closing input pipe\n");
	result = close(id_ou);
	if(result<0) 
		printf("Error when closing output pipe\n");
	

	return 1;
}

// Command to create test pipe:
// socat -d -d pty,raw,echo=0 pty,raw,echo=0