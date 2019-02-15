import os, sys


if len(sys.argv) < 3:
	print("Not enough inputs specified! Must specify 2 input arguments!")
	exit(-1)

pipe_in = sys.argv[1]
pipe_out = sys.argv[2]

id_in = os.open(pipe_in, os.O_WRONLY)
id_ou = os.open(pipe_out, os.O_RDONLY)

print(f"The input pipe has id {id_in}, and the output pipe has id {id_ou}")

buf = os.read(id_ou,5000)

print(f"The first {len(buf)} bytes have been read as:\n{buf.decode()}")

os.close(id_ou)

os.write(id_in, b"Message recieved!\n\0")

os.close(id_in)