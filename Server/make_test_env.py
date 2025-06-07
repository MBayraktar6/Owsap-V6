""" make_test_env.py script """

__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"
__project__ = "OSF Project"
__website__ = "https://osf.owsap.dev/"
__version__ = "1.0.1"

import os, sys, subprocess
import json
import re

root = os.getcwd()
win32 = sys.platform == "win32"

class config:
	@staticmethod
	def load(file_path):
		with open(file_path, "r") as config_file:
			return json.loads(re.sub(r'/\*.*?\*/', '', config_file.read()))

class utils:
	@staticmethod
	def mklink(dest, src, file = False):
		p = subprocess.Popen(['mklink', '/j', "{}".format(dest), "{}".format(src)],
			stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
		p.communicate()

	@staticmethod
	def mkdir(path):
		p = subprocess.Popen(['mkdir', "{}".format(path)],
			stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
		p.communicate()

	@staticmethod
	def copy(src, dest):
		p = subprocess.Popen(['copy', "{}".format(src), "{}".format(dest)],
			stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True)
		p.communicate()

	@staticmethod
	def rename(old, new):
		if os.path.isfile(root + os.sep + new):
			os.remove(root + os.sep + new)

		try:
			os.rename(root + os.sep + old, root + os.sep + new)
		except WindowsError as e:
			if e.winerror == 183:
				print("{} already exists.".format(new))

class make:
	@staticmethod
	def process(data):
		## Dump Proto
		print("Copying Dump Proto")
		utils.copy(r"..\Srcs\Tools\Binary\Dump Proto\share\item_proto.txt", "db")
		utils.copy(r"..\Srcs\Tools\Binary\Dump Proto\share\en\item_names.txt", "db")
		utils.copy(r"..\Srcs\Tools\Binary\Dump Proto\share\mob_proto.txt", "db")
		utils.copy(r"..\Srcs\Tools\Binary\Dump Proto\share\en\mob_names.txt", "db")

		## Database Cache Server
		print(f"Copying {data['db_file']['win32']} to db path.")
		utils.copy(r"..\Srcs\Server\db\db_d.exe", f"db\\{data['db_file']['win32']}")

		## Auth Core
		print(f"Copying {data['game_file']['win32']} to auth path.")
		utils.mklink(r"auth\package", r"share\package")
		utils.mklink(r"auth\panama", r"share\panama")

		utils.copy(r"..\Srcs\Server\game\game_d.exe", f"auth\\{data['game_file']['win32']}")
		utils.copy(r"share\lib\*.dll", "auth")

		## Channel Game Cores
		print("Copying game_d.exe to game path.")
		for ch, cores in data["channels"].items():
			for core in range(1, cores + 1):
				utils.mkdir(f"game\\channel{ch}\\core{core}")

				utils.mklink(f"game\\channel{ch}\\core{core}\\data", "share\\data")
				utils.mklink(f"game\\channel{ch}\\core{core}\\locale", "share\\locale")
				utils.mklink(f"game\\channel{ch}\\core{core}\\package", "share\\package")
				utils.mklink(f"game\\channel{ch}\\core{core}\\panama", "share\\panama")

				utils.copy(r"..\Srcs\Server\game\game_d.exe", f"game\\channel{ch}\\core{core}\\{data['game_file']['win32']}")
				utils.copy(r"share\lib\*.dll", f"game\\channel{ch}\\core{core}")

		## Misc.
		print(f"Copying {data['qc_file']['win32']} to share path.")
		utils.copy(r"..\Srcs\Server\game\src\quest\qc_d.exe", f"share\\locale\\{data['locale_service']}\\quest\\{data['qc_file']['win32']}")

if __name__ == "__main__":
	script_name = os.path.basename(sys.argv[0])

	try:
		json_file = f"server.json"
		if win32:
			make.process(config.load(json_file))
		else:
			print(f"{script_name} is only for Windows.")

	except FileNotFoundError:
		print(f"Cannot find the file {json_file}", xout.LIGHT_RED)

	except json.JSONDecodeError:
		print(f"Error decoding JSON in {json_file}", xout.LIGHT_RED)

	#os.system("pause")
