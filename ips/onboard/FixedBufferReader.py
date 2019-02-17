
import io

# Simply make a class which always reads a buffer into a fixed size
class FixedBufferReader(io.BufferedReader):
	def __init__(self, raw, buffer_size=io.DEFAULT_BUFFER_SIZE, read_size=None):
		super().__init__(raw,buffer_size)
		self.read_size = read_size
	def read(self,read_size=None):
		if read_size is None:
			read_size = self.read_size
		return super().read(read_size)