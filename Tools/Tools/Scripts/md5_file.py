"""
	This file will generate MD5 hash for a specific file.
"""

__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"

import os, hashlib

def MD5_FILE(FILE_NAME):
	MD5_HASH = hashlib.md5()

	try:
		with open(FILE_NAME, "rb") as f:
			for byte in iter(lambda: f.read(4096),b""):
				MD5_HASH.update(byte)

			print('%s, "%s"' % (FILE_NAME, MD5_HASH.hexdigest()))

	except Exception as e:
		print('Error: ' + str(e))

if __name__ == "__main__":
	FILE_NAME = raw_input("Enter the file name with extension : ")
	MD5_FILE(FILE_NAME)

	os.system("pause")
