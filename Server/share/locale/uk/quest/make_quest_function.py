__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"

import os, re

FUNCTION_PATH = "functions"
LUA_FILES = ["questlib.lua", "lualib.lua"]
LUB_FUNCTION_PATTERN = re.compile(r"\b([\w.]+)\s*=\s*function\s*|\bfunction\s+([\w.]+)\s*\(")
QUEST_FUNCTION_FILE = "quest_functions"

def BuildQuestFunctions():
	lines = []
	for file_name in os.listdir(FUNCTION_PATH):
		if file_name.endswith(".func"):
			with open(os.path.join(FUNCTION_PATH, file_name), "r") as file:
				file_lines = file.readlines()

			for line in file_lines:
				if line.strip() and not line.strip().startswith("//") and not line.strip().startswith("#"):
					lines.append(line)

	for lua_file in LUA_FILES:
		with open(lua_file, "r") as file:
			lua_code = file.read()

		function_names_list = [match.group(1) or match.group(2) for match in LUB_FUNCTION_PATTERN.finditer(lua_code)]
		for line in function_names_list:
			lines.append(line + "\n")

	with open(QUEST_FUNCTION_FILE, "w") as file:
		file.writelines(lines)

if __name__ == "__main__":
	BuildQuestFunctions()
