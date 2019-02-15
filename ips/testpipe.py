import os, sys


if len(sys.argv) < 3:
	print("Not enough inputs specified! Must specify 2 input arguments!")
	exit(-1)

pipe_in = sys.argv[1]
pipe_out = sys.argv[2]

id_in = os.open(pipe_in, os.O_RDONLY)
id_ou = os.open(pipe_out, os.O_WRONLY)

print(f"The input pipe has id {id_in}, and the output pipe has id {id_ou}")

buf = os.read(id_in,10)

print(f"The first 10 bytes have been read as {buf}")

os.close(id_in)

os.write(id_ou, "Message recieved!\n\0")

os.close(id_ou)