#!python2

"""
	This file will generate a hex sequence table.
	The generated table can be replaced @
		EterLib/NetStream.cpp @ s_bSequenceTable
		game/src/sequence.cpp @ gc_abSequence
"""

__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"

import random

class SequenceTable():

	SEQUENCE_TABLE_SIZE = 32768
	TABLE_LENGTH = SEQUENCE_TABLE_SIZE / 16 # base 16

	def __init__(self, filename):
		self.filename = filename

	def RandomHex(self):
		return " ".join(["0x%x," % int(hex(random.randint(0, 255)), 16) for i in range(16)])

	def Generate(self):
		try:
			f = open(self.filename, 'w')
			f.write("{\n")
			for i in range(self.TABLE_LENGTH):
				f.write("\t%s\n" % self.RandomHex())
			f.write("};\n")
		finally:
			f.close()

if __name__ == "__main__":
	Table = SequenceTable('sequence_table.cpp')
	Table.Generate()
