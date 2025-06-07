__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"

import os, subprocess

DUMP_PROTO = "dump_proto.exe"

ITEM_PROTO = {
	"proto": "item_proto",
	"table": "item_proto.txt",
	"names": "item_names.txt"
}

MOB_PROTO = {
	"proto": "mob_proto",
	"table": "mob_proto.txt",
	"names": "mob_names.txt"
}

SHARE_FOLDER = "share"
LOCALE_FOLDER = "locale"
LOCALE_LIST = ["ae", "cz", "de", "en", "es", "fr", "gr", "hu", "it", "pl", "pt", "ro", "tr"]

CLIENT_LOCALE_FOLDER = "..\..\..\Client\pack\locale\locale"

def mkdir(path):
	p = subprocess.Popen(['mkdir', "{}".format(path)],\
		stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
	p.communicate()

def copy(src, dest):
	p = subprocess.Popen(['copy', "{}".format(src), "{}".format(dest)],\
		stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
	p.communicate()

def run(file, src):
	p = subprocess.Popen(DUMP_PROTO, cwd = r"{}".format(src))
	p.communicate()

class DumpProto():
	def __init__(self):
		self.CopyProtos()
		self.RunDumpProto()
		self.UpdateProtos()

	""" Step 1 """
	# Copy "item_proto.txt", "item_names.txt", "mob_proto.txt", "mob_names.txt" to all locale folders.
	def CopyProtos(self):
		for LOCALE in LOCALE_LIST:
			LOCALE_PATH = "{}\{}".format(LOCALE_FOLDER, LOCALE)
			mkdir(LOCALE_PATH)

			# Copy shared files to locale folder.
			copy("{}\{}".format(SHARE_FOLDER, ITEM_PROTO["table"]), LOCALE_PATH) # item_proto.txt
			copy("{}\{}\{}".format(SHARE_FOLDER, LOCALE, ITEM_PROTO["names"]), LOCALE_PATH) # item_names.txt

			copy("{}\{}".format(SHARE_FOLDER, MOB_PROTO["table"]), LOCALE_PATH) # mob_proto.txt
			copy("{}\{}\{}".format(SHARE_FOLDER, LOCALE, MOB_PROTO["names"]), LOCALE_PATH) # mob_names.txt

	""" Step 2 """
	# Run "dump_proto.exe" for all locale folders.
	def RunDumpProto(self):
		for LOCALE in LOCALE_LIST:
			LOCALE_PATH = "{}\{}".format(LOCALE_FOLDER, LOCALE)
			run(DUMP_PROTO, LOCALE_PATH)

	""" Step 3 """
	# Move all new protos to client locale folers.
	def UpdateProtos(self):
		for LOCALE in LOCALE_LIST:
			LOCALE_PATH = "{}\{}".format(LOCALE_FOLDER, LOCALE)

			CLIENT_LOCALE_PATH = "{}\{}".format(CLIENT_LOCALE_FOLDER, LOCALE)
			mkdir(CLIENT_LOCALE_PATH)

			copy("{}\{}".format(LOCALE_PATH, ITEM_PROTO["proto"]), CLIENT_LOCALE_PATH)
			copy("{}\{}".format(LOCALE_PATH, MOB_PROTO["proto"]), CLIENT_LOCALE_PATH)

if __name__ == "__main__":
	DumpProto()
