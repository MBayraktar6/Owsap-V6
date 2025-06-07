# -*- coding: 949 -*-

__author__ = "Owsap"
__copyright__ = "Copyright 2024, Owsap Development"

import os
class build:
	group_path_dict = {
		"group" : "group.txt",
		"group_group" : "group_group.txt",
		"mob_drop_item" : "mob_drop_item.txt",
		"special_item_group" : "special_item_group.txt"
	}

	def __init__(self, group_path):
		if not group_path in self.group_path_dict:
			print("cannot find group path {}".format(group_path))
			return

		self.share_path = "../"
		self.group_path = group_path
		self.group_file = self.group_path_dict[group_path]

		self.truncate_group()
		self.dump_groups()

	def truncate_group(self):
		with open(os.path.join(self.share_path, self.group_file), "w") as file:
			file.truncate()

	def dump_groups(self):
		with open(os.path.join(self.share_path, self.group_file), "a") as output_file:
			for file in os.listdir(self.group_path):
				if file.endswith(".txt"):
					try:
						#with open(os.path.join(self.group_path, file), "r", encoding="cp1252") as input_file:
						with open(os.path.join(self.group_path, file), "r") as input_file:
							output_file.write(input_file.read())
					except Exception as err:
						print("failed at {} : {}".format(file, err))
						break

		print("build complete {}".format(self.group_file))

if __name__ == "__main__":
	import sys
	if len(sys.argv) == 2:
		build(sys.argv[1])
	else:
		print("usage: build.py <group_path>")
		print("<group_path>")
		for key in build.group_path_dict.keys():
			print("  {}".format(key))
