"""
	This file will generate MD5 hash for each pack files.
	Add this script in the pack folder of your client.
"""

__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"

import os
import hashlib

DATA_EXT = ".data"
INDEX_EXT = ".idx"

PACK_NAMES = [
	"bgm",
	"effect",
	"environment",
	"guild",
	"icon",
	"item",
	"locale",
	"map",
	"monster",
	"monster2",
	"npc",
	"npc_mount",
	"npc_pet",
	"npc2",
	"patch_osf",
	"pc",
	"pc2",
	"pc3",
	"property",
	"root",
	"season1",
	"season2",
	"sound",
	"terrain",
	"textureset",
	"tree",
	"ui",
	"zone"
]

def MD5_DATA(PACK_NAME):
	MD5_HASH = hashlib.md5()
	DATA_FILE = os.getcwd() + os.sep + PACK_NAME + DATA_EXT

	try:
		with open(DATA_FILE, "rb") as f:
			for byte in iter(lambda: f.read(4096),b""):
				MD5_HASH.update(byte)

			print('\t{ "pack/%s.data", "%s" },' % (PACK_NAME, MD5_HASH.hexdigest()))
	except:
		pass

def MD5_INDEX(PACK_NAME):
	MD5_HASH = hashlib.md5()
	INDEX_FILE = os.getcwd() + os.sep + PACK_NAME + INDEX_EXT

	try:
		with open(INDEX_FILE, "rb") as f:
			for byte in iter(lambda: f.read(4096),b""):
				MD5_HASH.update(byte)

			print('\t{ "pack/%s.idx", "%s" },' % (PACK_NAME, MD5_HASH.hexdigest()))
	except:
		pass

if __name__ == "__main__":
	for PACK_NAME in PACK_NAMES:
		MD5_DATA(PACK_NAME)
		MD5_INDEX(PACK_NAME)

	os.system("pause")
