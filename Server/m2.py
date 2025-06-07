""" m2.py server script """

__author__ = "Owsap"
__copyright__ = "Copyright 2023, Owsap Development"
__project__ = "OSF Project"
__website__ = "https://osf.owsap.dev/"
__version__ = "1.0.3"

import os
import sys
import platform
import subprocess
import shutil
import psutil
import signal

import json, re
import time

from datetime import datetime
from datetime import timedelta
from _weakref import proxy

HEADER = f"""
   ██████\033[35m╗ \033[36m███████\033[35m╗\033[36m███████\033[35m╗\033[36m
  ██\033[35m╔═══\033[36m██\033[35m╗\033[36m██\033[35m╔════╝\033[36m██\033[35m╔════╝\033[36m
  ██\033[35m║   \033[36m██\033[35m║\033[36m███████\033[35m╗\033[36m█████\033[35m╗\033[36m
  ██\033[35m║   \033[36m██\033[35m║╚════\033[36m██\033[35m║\033[36m██\033[35m╔══╝\033[36m
\033[35m  ╚\033[36m██████\033[35m╔╝\033[36m███████\033[35m║\033[36m██\033[35m║\033[36m
\033[35m   ╚═════╝ ╚══════╝╚═╝\033[36m
   {__author__} Server Files

  Web: {__website__}
  {__copyright__}
 
  M2 Server Script
  Version: {__version__}
"""

## Options
OPTION_QUIT = 0
OPTION_START = 1
OPTION_STOP = 2
OPTION_RESTART = 3
OPTION_STATUS = 4
OPTION_CLEAR = 5
OPTION_INSTALL_SYMLINKS = 6
OPTION_INSTALL_DEPENDENCIES = 7
OPTION_INSTALL_DATABASE = 8
OPTION_COMPILE_QUESTS = 9
OPTION_BACKUP = 10
OPTION_CREATE_ALIAS = 11

OPTION_ARG_DICT = {
	"quit" : {
		"key" : OPTION_QUIT,
		"label" : "Quit",
		"description" : "Quit the script."
	},
	"start" : {
		"key" : OPTION_START,
		"label" : "Start Server",
		"description" : "Initiate the server if it is not currently running."
	},
	"stop" : {
		"key" : OPTION_STOP,
		"label" : "Stop Server",
		"description" : "Gracefully shut down the server."
	},
	"restart" : {
		"key" : OPTION_RESTART,
		"label" : "Restart Server",
		"description" : "Gracefully shut down the server and then restart it."
	},
	"status" : {
		"key" : OPTION_STATUS,
		"label" : "Server Status",
		"description" : "Retrieve information about the current server state and resource usage."
	},
	"clear" : {
		"key" : OPTION_CLEAR,
		"label" : "Clear Files",
		"description" : "Remove all temporary files used by the server."
	},
	"install" : {
		"symlinks" : {
			"key" : OPTION_INSTALL_SYMLINKS,
			"label" : "Create Symbolic Links",
			"description" : "Create symbolic links for files and directories within each server path."
		},
		"dependencies" : {
			"key" : OPTION_INSTALL_DEPENDENCIES,
			"label" : "Install Dependencies",
			"description" : "Install all essential dependencies required to operate the server."
		},
		"database" : {
			"key" : OPTION_INSTALL_DATABASE,
			"label" : "Install Server Database",
			"description" : "Install the necessary databases and tables for proper server functionality."
		},
	},
	"qc" : {
		"key" : OPTION_COMPILE_QUESTS,
		"label" : "Compile Quests",
		"description" : "Rebuild all quests within the server."
	},
	"backup" : {
		"key" : OPTION_BACKUP,
		"label" : "Backup Server",
		"description" : "Perform manual or automatic backups of the server's data."
	},
	"alias" : {
		"key" : OPTION_CREATE_ALIAS,
		"label" : "Create Alias",
		"description" : "Create an alias for the script, allowing it to be executed as a command."
	},
}

## Operating Systems
WINDOWS = sys.platform == "win32"
UNIX = sys.platform.startswith("linux") or sys.platform == "darwin" or "bsd" in sys.platform.lower()

UNIX_POSTFIX = "unix"
WIN32_POSTFIX = "win32"

if UNIX:
	OS_POSTFIX = UNIX_POSTFIX
elif WINDOWS:
	OS_POSTFIX = WIN32_POSTFIX
elif WINDOWS:
	OS_POSTFIX = "none"

## Resource Monitor
RESOURCE_MONITOR = "bpytop"

## Server Data File
SERVER_DATA_FILE = "server.json"

## Servers
DB = 0
AUTH = 1
GAME = 2

## Input Return Confirm
if not WINDOWS:
	import tty
	import termios

	FX_RETURN_KEY_CONFIRM = False
else:
	FX_RETURN_KEY_CONFIRM = True

KEY_INTERRUPT = '\x03'
KEY_ESC = '\x1b'

class Config:
	@staticmethod
	def load_config(file_path):
		with open(file_path, "r") as config_file:
			return json.loads(re.sub(r'/\*.*?\*/', '', config_file.read()))

class Utils:
	if not FX_RETURN_KEY_CONFIRM:
		@staticmethod
		def get_key_press():
			fd = sys.stdin.fileno()
			old_settings = termios.tcgetattr(fd)
			try:
				tty.setraw(sys.stdin.fileno())
				key = sys.stdin.read(1)
				if key == KEY_INTERRUPT or key == KEY_ESC:
					raise KeyboardInterrupt
				return key
			finally:
				termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)

	@staticmethod
	def get_space_string(space_value):
		if isinstance(space_value, int):
			spacing_string = "\t" * space_value
		elif isinstance(space_value, float):
			tab = int(space_value)
			decimal_part = int((space_value - tab) * 10)
			spacing_string = "\t" * tab + " " * decimal_part
		else:
			spacing_string = ""

		return spacing_string

class FXOutput:
	RESET = "\033[0m"
	BRIGHT = "\033[1m"
	UNDERLINED = "\033[4m"
	FLASHING = "\033[5m"

	RED = "\033[31m"
	GREEN = "\033[32m"
	YELLOW = "\033[33m"
	BLUE = "\033[34m"
	PURPLE = "\033[35m"
	CYAN = "\033[36m"
	LIGHT_GRAY = "\033[37m"

	BLACK_BACKGROUND = "\033[40m"
	RED_BACKGROUND = "\033[41m"
	GREEN_BACKGROUND = "\033[42m"
	YELLOW_BACKGROUND = "\033[43m"
	BLUE_BACKGROUND = "\033[44m"
	PURPLE_BACKGROUND = "\033[45m"
	CYAN_BACKGROUND = "\033[46m"
	WHITE_BACKGROUND = "\033[47m"

	GRAY = "\033[90m"
	LIGHT_RED = "\033[91m"
	LIGHT_GREEN = "\033[92m"
	LIGHT_YELLOW = "\033[93m"
	LIGHT_BLUE = "\033[94m"
	WHITE = "\033[97m"

	SPACING = 0.2

	@staticmethod
	def print(text, color_code = RESET, space_value = SPACING):
		return print(f"{Utils.get_space_string(space_value)}{color_code}{text}{FXOutput.RESET}")

	@staticmethod
	def line():
		return print()

	@staticmethod
	def input(prompt, color_code = RESET, space_value = SPACING):
		return input(f"{Utils.get_space_string(space_value)}{color_code}{prompt}{FXOutput.RESET}")

	@staticmethod
	def create_tooltip(lines, width, space_value):
		width += 2
		spacing = Utils.get_space_string(space_value)
		tooltip = f"{spacing}╭" + "─" * width + "╮\n"
		for line in lines:
			tooltip += f"{spacing}│ {line.ljust(width - 2)} │\n"
		tooltip += f"{spacing}╰" + "─" * width + "╯"
		return tooltip

	@staticmethod
	def tooltip(lines, color_code = RESET, space_value = SPACING):
		tooltip = FXOutput.create_tooltip(lines, width = max(len(line) for line in lines), space_value = space_value)
		FXOutput.print(tooltip, color_code, 0)

	@staticmethod
	def confirm(prompt, color_code, space_value = SPACING):
		if FX_RETURN_KEY_CONFIRM:
			return FXOutput.input(prompt, color_code, space_value)
		else:
			xout.print(prompt, color_code, space_value)
			key = Utils.get_key_press()
			return key

class M2:
	class Server:
		def __init__(self, parent, data):
			self.parent = proxy(parent)
			self.data = data

		def start(self, type, ch = 0, core = 0):
			process_info = self.get_process_info()
			if type == DB:
				try:
					path = os.path.join(os.getcwd(), self.data['paths']['db'])
					command = f"{path}/{self.data['db_file'][OS_POSTFIX]}"

					if not os.path.exists(command):
						xout.print(f"{command} does not exist.", xout.LIGHT_RED)
						return

					if not WINDOWS:
						try:
							pid_list = subprocess.check_output(f"pgrep -f {command}", shell = True).decode().strip().split('\n')
							if pid_list:
								xout.print(f"Database Server already running.", xout.LIGHT_RED)
								return
						except subprocess.CalledProcessError:
							pass

					try:
						if not WINDOWS:
							with open(os.devnull, "w") as null_device:
								result = subprocess.Popen(command, shell = True, stdout = null_device, stderr=subprocess.STDOUT, cwd = path)
								if result.returncode is None:
									xout.print("Started Database Server.", xout.LIGHT_GREEN)
								else:
									xout.print("Failed to start Database Server.", xout.LIGHT_RED)
						else:
							subprocess.Popen(["start", command], stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True, cwd = path)

					except Exception as e:
						xout.print(f"An error occurred: {e}", xout.LIGHT_RED)

				except subprocess.CalledProcessError as error:
					xout.print(f"Error Starting Database Server: {error}", xout.LIGHT_RED)

			elif type == AUTH:
				try:
					path = os.path.join(os.getcwd(), self.data['paths']['auth'])
					command = f"{path}/{self.data['game_file'][OS_POSTFIX]}"

					if not os.path.exists(command):
						xout.print(f"{command} does not exist.", xout.LIGHT_RED)
						return

					if not WINDOWS:
						try:
							pid_list = subprocess.check_output(f"pgrep -f {command}", shell = True).decode().strip().split('\n')
							if pid_list:
								xout.print(f"Authentication Server already running.", xout.LIGHT_RED)
								return
						except subprocess.CalledProcessError:
							pass

					try:
						if not WINDOWS:
							with open(os.devnull, "w") as null_device:
								result = subprocess.Popen(command, shell = True, stdout = null_device, stderr=subprocess.STDOUT, cwd = path)
								if result.returncode is None:
									xout.print("Started Authentication Server.", xout.LIGHT_GREEN)
								else:
									xout.print("Failed to start Authentication Server.", xout.LIGHT_RED)
						else:
							subprocess.Popen(["start", command], stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True, cwd = path)

					except Exception as e:
						xout.print(f"An error occurred: {e}", xout.LIGHT_RED)

				except subprocess.CalledProcessError as error:
					xout.print(f"Error Starting Authentication Server: {error}", xout.LIGHT_RED)

			elif type == GAME:
				try:
					path = os.path.join(os.getcwd(), self.data['paths']['game'], f"channel{ch}", f"core{core}")
					command = f"{path}/{self.data['game_file'][OS_POSTFIX]}"

					if not os.path.exists(command):
						xout.print(f"{command} does not exist.", xout.LIGHT_RED)
						return

					if not WINDOWS:
						try:
							pid_list = subprocess.check_output(f"pgrep -f {command}", shell = True).decode().strip().split('\n')
							if pid_list:
								xout.print(f"Game Server Channel {ch} Core {core} already running.", xout.LIGHT_RED)
								return
						except subprocess.CalledProcessError:
							pass

					try:
						if not WINDOWS:
							with open(os.devnull, "w") as null_device:
								result = subprocess.Popen(command, shell = True, stdout = null_device, stderr=subprocess.STDOUT, cwd = path)
								if result.returncode is None:
									xout.print(f"Started Game Server Channel {ch} Core {core}.", xout.LIGHT_GREEN)
								else:
									xout.print(f"Failed to start Game Server Channel {ch} Core {core}.", xout.LIGHT_RED)
						else:
							subprocess.Popen(["start", command], stdout = subprocess.PIPE, stderr = subprocess.STDOUT, shell = True, cwd = path)

					except Exception as e:
						xout.print(f"Error starting Game Server Channel {ch} Core {core}: {e}", xout.LIGHT_RED)

				except subprocess.CalledProcessError as error:
					xout.print(f"Error starting Game Server Channel {ch} Core {core}: {error}", xout.LIGHT_RED)

		def stop(self, auto = False):
			if WINDOWS:
				return

			process_info = self.get_process_info()
			for name, path in process_info.items():
				if not os.path.exists(path):
					xout.print(f"{path} does not exist.", xout.LIGHT_RED)
					continue

				try:
					command = f"pgrep -f {path}"

					pid_list = subprocess.check_output(command, shell = True).decode().strip().split('\n')
					for pid in pid_list:
						kill = False

						if auto:
							kill = True
						else:
							while True:
								confirm_kill = xout.confirm(f"Stop {name}? (y/n) ", xout.RESET, 1)
								if confirm_kill.lower() in ["y", "yes"]:
									kill = True
									break
								elif confirm_kill.lower() in ["n", "no"]:
									break
								else:
									xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED, 1)
									continue

						if kill:
							try:
								os.kill(int(pid), signal.SIGTERM)
								xout.print(f"Sent SIGTERM to PID {pid} for graceful shutdown.", xout.WHITE, FXOutput.SPACING if auto else 1)
							except OSError:
								xout.print(f"Failed to send SIGTERM to PID {pid}.", xout.LIGHT_RED, FXOutput.SPACING if auto else 1)

				except subprocess.CalledProcessError:
					xout.print(f"{name} is not running.", xout.LIGHT_YELLOW, FXOutput.SPACING if auto else 1)

		def restart(self):
			if WINDOWS:
				return

			self.stop(True)
			self.parent.run(OPTION_START)

		def delete_files_in_directory(self, directory):
			try:
				for filename in os.listdir(directory):
					file_path = os.path.join(directory, filename)
					#file_name_without_extension = os.path.splitext(filename)[0]
					#if os.path.isfile(file_path) and file_name_without_extension in self.data["temp_files"]:
					if os.path.isfile(file_path) and filename in self.data["temp_files"]:
						xout.print(f"{file_path}", xout.GRAY)
						os.remove(file_path)

				for filename in os.listdir(directory):
					file_path = os.path.join(directory, filename)
					if os.path.isdir(file_path) and filename in self.data["temp_folders"]:
						xout.print(f"{file_path}", xout.GRAY)
						shutil.rmtree(file_path)

			except Exception as error:
				xout.print(f"Error deleting files in {directory}.", xout.LIGHT_RED)
				xout.print(f"Exception: {error}", xout.LIGHT_RED)

		def clear(self):
			auth_path = os.path.join(os.getcwd(), self.data['paths']['auth'])
			self.delete_files_in_directory(auth_path)

			db_path = os.path.join(os.getcwd(), self.data['paths']['db'])
			self.delete_files_in_directory(db_path)

			for ch, cores in self.data['channels'].items():
				for core in range(1, cores + 1):
					core_path = os.path.join(os.getcwd(), self.data['paths']['game'], f'channel{ch}', f'core{core}')
					self.delete_files_in_directory(core_path)

			xout.print('Temporary files cleared.', xout.LIGHT_GREEN)

		def check_cron_job(self):
			cron_file_path = os.path.join(self.data["backup"]["path"]["cron"], "m2backup")
			return os.path.exists(cron_file_path)

		def backup(self, type, value = 0):
			if WINDOWS:
				return

			if type not in self.data["backup"]["types"]:
				xout.print(f"Invalid backup type named '{type}'", xout.LIGHT_RED)
				return

			if type == "mysql":
				try:
					script_dir = os.path.dirname(os.path.abspath(__file__))
					backup_folder = os.path.join(script_dir, self.data["backup"]["path"]["mysql"], datetime.now().strftime("%d.%m.%Y %H:%M"))
					os.makedirs(backup_folder, exist_ok = True)

					for db in self.data["mysql"]["database"]:
						xout.print(f"Dumping {db} database")
						cmd = [
							"mysqldump",
							f"--login-path={os.path.join(script_dir, self.data['mysql']['login_path'])}",
							"--set-gtid-purged=OFF",
							db,
							"-r", f"{backup_folder}/{db}.sql"
						]
						subprocess.run(cmd)

					xout.print("Database backed up successfully.", xout.LIGHT_GREEN)
				except Exception as e:
					xout.print(f"Error backing up the database: {e}", xout.LIGHT_RED)

			elif type == "server":
				try:
					script_dir = os.path.dirname(os.path.abspath(__file__))

					backup_folder = os.path.join(script_dir, self.data["backup"]["path"]["server"], datetime.now().strftime("%d.%m.%Y %H:%M"))
					os.makedirs(backup_folder, exist_ok = True)

					script_backup_folder = os.path.join(backup_folder, "server")
					archive_filename = os.path.join(backup_folder, "server.tar.gz")

					cmd = ["tar", "-czvf", archive_filename, "-C", os.path.dirname(script_dir), os.path.basename(script_dir)]
					subprocess.run(cmd)

					xout.print("Server directory backed up successfully.", xout.LIGHT_GREEN)
				except KeyboardInterrupt as c:
					xout.print(f"Operation canceled", xout.LIGHT_YELLOW)
				except Exception as e:
					xout.print(f"Error backing up the server directory: {e}", xout.LIGHT_RED)

			elif type == "auto":
				try:
					if int(value) == 1:
						script_name = os.path.basename(sys.argv[0])
						cron_job = (
							"SHELL=/bin/sh\n"
							"PATH=/etc:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin\n\n"
							"# Order of crontab fields\n"
							"# minute hour mday month wday user command\n"
							"{} root cd {} && /usr/local/bin/python {} --backup all >> {} 2>&1"
						).format(self.data["backup"]["cron_time"], os.getcwd(), script_name, os.path.join(os.getcwd(), "cron.log"))

						cron_file_path = os.path.join(self.data["backup"]["path"]["cron"], self.data["backup"]["cron_file"])
						with open(cron_file_path, "w") as cron_file:
							cron_file.write(cron_job)

						xout.print(f"Automatic backups are now enabled.", xout.LIGHT_GREEN)
					else:
						cron_file_path = os.path.join(self.data["backup"]["path"]["cron"], self.data["backup"]["cron_file"])
						if os.path.exists(cron_file_path):
							os.remove(cron_file_path)

						xout.print(f"Automatic backups are now disabled.", xout.GRAY)

				except Exception as e:
					xout.print(f"Error while creating cron job.", xout.LIGHT_RED)
					xout.print(f"Exception: {e}", xout.LIGHT_RED)

			elif type == "all":
				self.backup("mysql")
				self.backup("server")

		def install_symlinks(self):
			share_path = os.path.join(os.getcwd(), self.data["paths"]["share"])

			try:
				auth_path = os.path.join(os.getcwd(), self.data["paths"]['auth'])

				if os.path.exists(auth_path):
					xout.print(f"Installing symbolic links for {auth_path}", xout.GRAY)

					for symlink in self.data["shared_paths"]["auth"]:
						if symlink == "db":
							symlink = self.data["db_file"][OS_POSTFIX]
						elif symlink == "game":
							symlink = self.data["game_file"][OS_POSTFIX]

						symlink_path = os.path.join(auth_path, symlink)
						target_path = os.path.join(share_path, symlink)

						if not os.path.exists(target_path):
							xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
							continue

						if not os.path.exists(symlink_path):
							if not WINDOWS:
								os.symlink(target_path, symlink_path)
							else:
								if os.path.isdir(target_path):
									command = ['mklink', '/j', symlink_path, target_path]
								else:
									command = ['mklink', '/H', symlink_path, target_path]

								p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
								p.communicate()

					for symlink in self.data["shared_libs"]["auth"][OS_POSTFIX]:
						symlink_path = os.path.join(auth_path, symlink)
						target_path = os.path.join(share_path, "lib", OS_POSTFIX, symlink)

						if not os.path.exists(target_path):
							xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
							continue

						if not os.path.exists(symlink_path):
							if not WINDOWS:
								os.symlink(target_path, symlink_path)
							else:
								if os.path.isdir(target_path):
									command = ['mklink', '/j', symlink_path, target_path]
								else:
									command = ['mklink', '/H', symlink_path, target_path]

								p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
								p.communicate()
				else:
					xout.print(f"{auth_path} not found.", xout.LIGHT_RED)

			except Exception as error:
				xout.print(f"Error while installing symbolic links for {auth_path}.", xout.LIGHT_RED)
				xout.print(f"Exception: {error}", xout.LIGHT_RED)

			try:
				db_path = os.path.join(os.getcwd(), self.data["paths"]['db'])

				if os.path.exists(db_path):
					xout.print(f"Installing symbolic links for {db_path}", xout.GRAY)

					for symlink in self.data["shared_paths"]["db"]:
						if symlink == "db":
							symlink = self.data["db_file"][OS_POSTFIX]
						elif symlink == "game":
							symlink = self.data["game_file"][OS_POSTFIX]

						symlink_path = os.path.join(db_path, symlink)
						target_path = os.path.join(share_path, symlink)

						if not os.path.exists(target_path):
							xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
							continue

						if not os.path.exists(symlink_path):
							if not WINDOWS:
								os.symlink(target_path, symlink_path)
							else:
								if os.path.isdir(target_path):
									command = ['mklink', '/j', symlink_path, target_path]
								else:
									command = ['mklink', '/H', symlink_path, target_path]

								p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
								p.communicate()

					for symlink in self.data["shared_libs"]["db"][OS_POSTFIX]:
						symlink_path = os.path.join(db_path, symlink)
						target_path = os.path.join(share_path, "lib", OS_POSTFIX, symlink)

						if not os.path.exists(target_path):
							xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
							continue

						if not os.path.exists(symlink_path):
							if not WINDOWS:
								os.symlink(target_path, symlink_path)
							else:
								if os.path.isdir(target_path):
									command = ['mklink', '/j', symlink_path, target_path]
								else:
									command = ['mklink', '/H', symlink_path, target_path]

								p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
								p.communicate()
				else:
					xout.print(f"{db_path} not found.", xout.LIGHT_RED)

			except Exception as error:
				xout.print(f"Error while installing symbolic links for {db_path}.", xout.LIGHT_RED)
				xout.print(f"Exception: {error}", xout.LIGHT_RED)

			try:
				for ch, cores in self.data["channels"].items():
					for core in range(1, cores + 1):
						core_path = os.path.join(os.getcwd(), self.data["paths"]['game'], f"channel{ch}", f"core{core}")

						if os.path.exists(core_path):
							xout.print(f"Installing symbolic links for {core_path}", xout.GRAY)

							for symlink in self.data["shared_paths"]["game"]:
								if symlink == "db":
									symlink = self.data["db_file"][OS_POSTFIX]
								elif symlink == "game":
									symlink = self.data["game_file"][OS_POSTFIX]

								symlink_path = os.path.join(core_path, symlink)
								target_path = os.path.join(share_path, symlink)

								if not os.path.exists(target_path):
									xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
									continue

								if not os.path.exists(symlink_path):
									if not WINDOWS:
										os.symlink(target_path, symlink_path)
									else:
										if os.path.isdir(target_path):
											command = ['mklink', '/j', symlink_path, target_path]
										else:
											command = ['mklink', '/H', symlink_path, target_path]

										p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
										p.communicate()

							for symlink in self.data["shared_libs"]["game"][OS_POSTFIX]:
								symlink_path = os.path.join(core_path, symlink)
								target_path = os.path.join(share_path, "lib", OS_POSTFIX, symlink)

								if not os.path.exists(target_path):
									xout.print(f"{symlink} not found in {share_path}", xout.LIGHT_RED, 1)
									continue

								if not os.path.exists(symlink_path):
									if not WINDOWS:
										os.symlink(target_path, symlink_path)
									else:
										if os.path.isdir(target_path):
											command = ['mklink', '/j', symlink_path, target_path]
										else:
											command = ['mklink', '/H', symlink_path, target_path]

										p = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, shell=True)
										p.communicate()
						else:
							xout.print(f"{core_path} not found.", xout.LIGHT_RED)

			except Exception as error:
				xout.print(f"Error while installing symbolic links for {core_path}.", xout.LIGHT_RED)
				xout.print(f"Exception: {error}", xout.LIGHT_RED)

		def detect_shell(self):
			shell = os.getenv('SHELL')
			if shell is None:
				raise EnvironmentError("Could not detect the shell. SHELL environment variable is not set.")
			return shell

		def create_alias(self):
			try:
				script_name = os.path.basename(sys.argv[0])
				script_path = os.path.abspath(script_name)
				current_directory = os.getcwd()

				shell = self.detect_shell()

				if 'csh' in shell:
					alias_cmd = f'alias {self.data["alias_name"]} \'cd {current_directory} && python {script_path}\''
					config_file = os.path.expanduser("~/.cshrc")
					source_cmd = "source"
				elif 'bash' in shell:
					alias_cmd = f'alias {self.data["alias_name"]}="cd {current_directory} && python {script_path}"'
					config_file = os.path.expanduser("~/.bashrc")
					source_cmd = "source"
				elif 'zsh' in shell:
					alias_cmd = f'alias {self.data["alias_name"]}="cd {current_directory} && python {script_path}"'
					config_file = os.path.expanduser("~/.zshrc")
					source_cmd = "source"
				elif 'sh' in shell:
					alias_cmd = f'alias {self.data["alias_name"]}="cd {current_directory} && python {script_path}"'
					config_file = os.path.expanduser("~/.profile")
					source_cmd = "."
				else:
					raise EnvironmentError(f"Unsupported shell: {shell}")

				with open(config_file, "a") as config_file_handle:
					config_file_handle.write(f"\n{alias_cmd}\n")

				xout.print(f"Alias '{self.data['alias_name']}' created for {script_name} in {shell}", xout.LIGHT_GREEN)
				xout.print(f"Please run `{source_cmd} {config_file}` to reload your aliases.", xout.LIGHT_YELLOW)

			except Exception as error:
				xout.print(f"Error while creating alias for {script_name}.", xout.LIGHT_RED)
				xout.print(f"Exception: {error}", xout.LIGHT_RED)

		def install_dependencies(self):
			if WINDOWS:
				xout.print("MySQL Installer: https://dev.mysql.com/downloads/installer/", xout.GRAY)
				return

			for package in self.data["packages"]:
				xout.print(f"Installing package {package}", xout.GRAY)
				os.system(f"pkg install {package}")

		def install_database(self):
			if WINDOWS:
				xout.print("Make sure you have MySQL installed and added to your system variables path.", xout.LIGHT_YELLOW)

			for db in self.data["mysql"]["database"]:
				xout.print(f"Creating {db} database")

				sql_file = os.path.join(os.getcwd(), self.data['paths']['db'], 'data', f'{db}.sql')
				if not WINDOWS:
					cmd = [
						"mysql",
						"--login-path==" + self.data["mysql"]["login_path"],
						"-e", f"source {sql_file}"
					]
				else:
					cmd = [
						"mysql",
						"-u" + self.data["mysql"]["user"],
						"-p" + self.data["mysql"]["password"],
						"-e", f"source {sql_file}"
					]

				subprocess.run(cmd)

		def compile_quests(self):
			try:
				quest_path = os.path.join(os.getcwd(), self.data["paths"]["share"], "locale", self.data["locale_service"], "quest", self.data["quest_script"]["make"])
				result = subprocess.run(["python", quest_path], shell=False, check=True)
			except subprocess.CalledProcessError as e:
				xout.print(f"An error occurred while executing the script: {e}", xout.LIGHT_RED)

		def get_process_info(self):
			process_info = {
				f"Database Server" : os.path.join(os.getcwd(), self.data['paths']['db'], self.data['db_file'][OS_POSTFIX]),
				f"Authentication Server" : os.path.join(os.getcwd(), self.data['paths']['auth'], self.data['game_file'][OS_POSTFIX])
			}

			for ch, cores in self.data["channels"].items():
				for core in range(1, cores + 1):
					process_info[f"Game Server Channel {ch} Core{core}"] = os.path.join(os.getcwd(), self.data["paths"]["game"], f"channel{ch}", f"core{core}", self.data['game_file'][OS_POSTFIX])

			return process_info

		def status(self):
			if WINDOWS:
				return

			process_info = self.get_process_info()
			for name, path in process_info.items():
				if not os.path.exists(path):
					xout.print(f"{path} does not exist.", xout.LIGHT_RED)
					continue

				try:
					pid_list = subprocess.check_output(f"pgrep -f {path}", shell = True).decode().strip().split('\n')
					for pid in pid_list:
						xout.print(f"{name} (Found)", xout.WHITE)

						process = psutil.Process(int(pid))

						xout.print(f"PID: {pid}", xout.GRAY, 1)

						## Uptime
						current_time = time.time()
						start_time = process.create_time()

						uptime_seconds = current_time - start_time
						uptime_readable = str(timedelta(seconds=uptime_seconds))
						xout.print(f"Uptime: {uptime_readable}", xout.GRAY, 1)

						## CPU Usage
						cpu_usage = process.cpu_percent(interval = 1)
						xout.print(f"CPU Usage: {cpu_usage:.2f}%", xout.GRAY, 1)

						## RAM Usage
						memory_info = process.memory_info()
						ram_usage = memory_info.rss / (1024 * 1024)
						xout.print(f"RAM Usage: {ram_usage:.2f} MB", xout.GRAY, 1)

						## Status
						xout.print(f"Status: {process.status()}", xout.GRAY, 1)

				except subprocess.CalledProcessError:
					xout.print(f"{name} is not running.", xout.LIGHT_YELLOW)

			while True:
				confirm_kill = xout.confirm(f"View live resource monitor? (y/n) ", xout.RESET)
				if confirm_kill.lower() in ["y", "yes"]:
					os.system(f"{RESOURCE_MONITOR}")
					break
				elif confirm_kill.lower() in ["n", "no"]:
					break
				else:
					xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
					continue

	def __init__(self, data):
		self.server = self.Server(self, data)
		self.option_dict = {
			OPTION_QUIT: (
				lambda: exit(),
				OPTION_ARG_DICT["quit"]["label"],
				OPTION_ARG_DICT["quit"]["description"]
			),
			OPTION_START: (
				lambda type, ch = -1, core = -1: self.server.start(type, ch, core),
				OPTION_ARG_DICT["start"]["label"],
				OPTION_ARG_DICT["start"]["description"]
			),
			OPTION_STOP: (
				lambda: self.server.stop(),
				OPTION_ARG_DICT["stop"]["label"],
				OPTION_ARG_DICT["stop"]["description"]
			),
			OPTION_RESTART: (
				lambda: self.server.restart(),
				OPTION_ARG_DICT["restart"]["label"],
				OPTION_ARG_DICT["restart"]["description"]
			),
			OPTION_STATUS: (
				lambda: self.server.status(),
				OPTION_ARG_DICT["status"]["label"],
				OPTION_ARG_DICT["status"]["description"]
			),
			OPTION_CLEAR: (
				lambda: self.server.clear(),
				OPTION_ARG_DICT["clear"]["label"],
				OPTION_ARG_DICT["clear"]["description"]
			),
			OPTION_INSTALL_SYMLINKS: (
				lambda: self.server.install_symlinks(),
				OPTION_ARG_DICT["install"]["symlinks"]["label"],
				OPTION_ARG_DICT["install"]["symlinks"]["description"]
			),
			OPTION_INSTALL_DEPENDENCIES: (
				lambda: self.server.install_dependencies(),
				OPTION_ARG_DICT["install"]["dependencies"]["label"],
				OPTION_ARG_DICT["install"]["dependencies"]["description"]
			),
			OPTION_INSTALL_DATABASE: (
				lambda: self.server.install_database(),
				OPTION_ARG_DICT["install"]["database"]["label"],
				OPTION_ARG_DICT["install"]["database"]["description"]
			),
			OPTION_COMPILE_QUESTS: (
				lambda: self.server.compile_quests(),
				OPTION_ARG_DICT["qc"]["label"],
				OPTION_ARG_DICT["qc"]["description"]
			),
			OPTION_BACKUP: (
				lambda type, value = 0: self.server.backup(type, value),
				OPTION_ARG_DICT["backup"]["label"],
				OPTION_ARG_DICT["backup"]["description"]
			),
			OPTION_CREATE_ALIAS: (
				lambda: self.server.create_alias(),
				OPTION_ARG_DICT["alias"]["label"],
				OPTION_ARG_DICT["alias"]["description"]
			),
		}
		if WINDOWS:
			del self.option_dict[OPTION_STOP]
			del self.option_dict[OPTION_RESTART]
			del self.option_dict[OPTION_STATUS]
			del self.option_dict[OPTION_BACKUP]
			del self.option_dict[OPTION_CREATE_ALIAS]

	def __del__(self):
		self.option_dict.clear()
		if self.server:
			del self.server

	def check_paths(self):
		check = True
		for path, folder in self.server.data["paths"].items():
			if not os.path.exists(path):
				xout.print(f"{path} path not found.", xout.LIGHT_RED)
				check = False

		return check

	def run(self, option = -1):
		if not self.check_paths():
			return

		if option != -1:
			if option not in self.option_dict:
				xout.print("Invalid option", xout.LIGHT_RED)
				return

			self.handle_option(option)
		else:
			self.display_message()
			self.display_options()

	def display_message(self):
		os_name = platform.system()
		os_version = platform.release()
		architecture = platform.machine()

		xout.print(HEADER, xout.CYAN, 1)
		if WINDOWS:
			xout.print(f"OS: {os_name} {os_version} {architecture} {xout.YELLOW} (DEV)", xout.GRAY)
			xout.print("Using this script on Windows is meant for development only, which might limit some of its features.", xout.YELLOW)
		else:
			xout.print(f"OS: {os_name} {os_version} {architecture} {xout.YELLOW} (PRODUCTION)", xout.GRAY)

		xout.print(f"Directory: {os.getcwd()}", xout.GRAY)
		if not WINDOWS:
			if self.server.check_cron_job():
				xout.print(f"Automatic Backups: {xout.GREEN}Enabled", xout.GRAY)
			else:
				xout.print(f"Automatic Backups: {xout.RED}Disabled", xout.GRAY)

		xout.line()

	def display_options(self):
		for option, (func, label, description) in self.option_dict.items():
			option_text = f"{option}."
			formatted_option = f"{option_text:<3}"
			xout.print(f"{formatted_option} {label:<25} {description}", xout.RESET)

		xout.line()

		while True:
			try:
				option_str = xout.input("Select an option: ", xout.GRAY)
				if option_str.isdigit():
					option = int(option_str)
					if option not in self.option_dict:
						xout.print("Invalid option", xout.LIGHT_RED)
						continue

					self.handle_option(option)
					break
				else:
					xout.print("Invalid input. Please enter a valid integer.", xout.LIGHT_RED)

			except KeyboardInterrupt:
				xout.print("Input canceled.", xout.YELLOW)
				break

	def handle_option(self, option):
		func, label, description = self.option_dict[option]
		xout.print(f"> {label}", xout.RESET)

		if option == OPTION_START:

			while True:
				try:
					confirm_db = xout.confirm("Initiate Database Server? (y/n) ", xout.GRAY)
					if confirm_db.lower() in ["y", "yes"]:
						func(DB)
						break
					elif confirm_db.lower() in ["n", "no"]:
						xout.print("Skipped.", xout.LIGHT_YELLOW)
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue
				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

			xout.line()

			while True:
				try:
					confirm_auth = xout.confirm("Initiate Authentication Server? (y/n) ", xout.GRAY)
					if confirm_auth.lower() in ["y", "yes"]:
						func(AUTH)
						break
					elif confirm_auth.lower() in ["n", "no"]:
						xout.print("Skipped.", xout.LIGHT_YELLOW)
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue
				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

			xout.line()

			xout.print(f"Game Server", xout.GRAY)
			xout.tooltip([
				"Input Options",
				"",
				"a (all) : Initiate all channel cores.",
				"c (choose) : Choose individual channel cores.",
				"s (skip) : Skip channel."
			], xout.LIGHT_GRAY)

			queue_dict = {}
			for ch, cores in self.server.data["channels"].items():
				while True:
					try:
						confirm_ch = xout.confirm(f"Channel {ch} (Cores {cores}) ", xout.GRAY)
						if confirm_ch.lower() in ["c", "choose"]:
							cores_list = []
							for core in range(1, cores + 1):
								while True:
									confirm_core = xout.confirm(f"Initiate Core {core}? (y/n) ", xout.LIGHT_GRAY, 1)
									if confirm_core.lower() in ["y", "yes"]:
										xout.print("Yes.", xout.WHITE, 1)
										cores_list.append(core)
										break
									elif confirm_core.lower() in ["n", "no"]:
										xout.print("No.", xout.GRAY, 1)
										break
									else:
										xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED, 1)
										continue

							if cores_list:
								queue_dict[ch] = cores_list
							break

						elif confirm_ch.lower() in ["a", "all"]:
							xout.print("All.", xout.WHITE)

							cores_list = list(range(1, cores + 1))
							if cores_list:
								queue_dict[ch] = cores_list
							break

						elif confirm_ch.lower() in ["s", "skip"]:
							xout.print("Skipped.", xout.LIGHT_YELLOW)
							break

						else:
							xout.print("Invalid option. Please choose one of the valid input options.", xout.LIGHT_RED)
							continue

					except KeyboardInterrupt:
						xout.print("Input canceled.", xout.YELLOW)
						break

			for ch, cores in queue_dict.items():
				for core in cores:
					func(GAME, ch, core)

		elif option == OPTION_STATUS:
			if WINDOWS:
				xout.print("This option is not available on Windows, use the Windows Task Manager for more resource information.", xout.LIGHT_YELLOW)
				return

			func()

		elif option == OPTION_STOP:
			if WINDOWS:
				xout.print("This option is not available on Windows, close all command-line windows to close the server.", xout.LIGHT_YELLOW)
				return

			xout.line()
			while True:
				try:
					confirm_auth = xout.confirm("Stop Server? (y/n) ", xout.GRAY)
					if confirm_auth.lower() in ["y", "yes"]:
						func()
						break
					elif confirm_auth.lower() in ["n", "no"]:
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

		elif option == OPTION_CLEAR:

			xout.line()
			while True:
				try:
					xout.print("This will permanently delete temporary files.", xout.YELLOW)
					confirm = xout.confirm("Clear all temporary files? (y/n) ", xout.GRAY)
					if confirm.lower() in ["y", "yes"]:
						func()
						break
					elif confirm.lower() in ["n", "no"]:
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

		elif option == OPTION_RESTART:
			if WINDOWS:
				xout.print("This option is not available on Windows, close all command-line windows to close the server and start again.", xout.LIGHT_YELLOW)
				return

			xout.line()
			while True:
				try:
					confirm = xout.confirm("Are you sure you want to restart the server? (y/n) ", xout.GRAY)
					if confirm.lower() in ["y", "yes"]:
						func()
						break
					elif confirm.lower() in ["n", "no"]:
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

		elif option == OPTION_INSTALL_DATABASE:

			xout.line()
			while True:
				try:
					xout.print("This will drop all data from your database!", xout.YELLOW)
					confirm = xout.confirm("Do you want to continue? (y/n) ", xout.GRAY)
					if confirm.lower() in ["y", "yes"]:
						func()
						break
					elif confirm.lower() in ["n", "no"]:
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

		elif option == OPTION_CREATE_ALIAS:
			if WINDOWS:
				xout.print("This option is not available on Windows.", xout.LIGHT_YELLOW)
				return

			xout.line()
			while True:
				try:
					confirm = xout.confirm(f"Create {self.server.data['alias_name']} as an alias? (y/n) ", xout.GRAY)
					if confirm.lower() in ["y", "yes"]:
						func()
						break
					elif confirm.lower() in ["n", "no"]:
						break
					else:
						xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
						continue

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break

		elif option == OPTION_BACKUP:
			if WINDOWS:
				xout.print("This option is not available on Windows.", xout.LIGHT_YELLOW)
				return

			xout.line()
			if self.server.check_cron_job():
				xout.tooltip(["Automatic backups are enabled."], xout.GREEN)
			else:
				xout.tooltip(["Automatic backups are disabled."], xout.YELLOW)

			xout.tooltip([
				"Input Options",
				"1. Backup MySQL (Database)",
				"2. Backup Server (Directory)",
				"3. Backup All (MySQL and Server)",
				"4. Enable/Disable automatic backup.",
			], xout.LIGHT_GRAY)

			while True:
				try:
					option_str = xout.input("Select an option: ", xout.GRAY)
					if option_str.isdigit():
						option = int(option_str)
						if option == 1:
							func("mysql")
							break

						elif option == 2:
							func("server")
							break

						elif option == 3:
							func("all")
							break

						elif option == 4:
							try:
								confirm = xout.confirm("Enable automatic backups? (y/n) ", xout.GRAY)
								if confirm.lower() in ["y", "yes"]:
									func("auto", 1)
									break
								elif confirm.lower() in ["n", "no"]:
									func("auto", 0)
									break
								else:
									xout.print("Invalid input. Please enter 'y' or 'n'.", xout.LIGHT_RED)
									continue
							except KeyboardInterrupt:
								xout.print("Input canceled.", xout.YELLOW)
								break
						else:
							xout.print("Invalid option", xout.LIGHT_RED)
							continue
					else:
						xout.print("Invalid input. Please enter a valid integer.", xout.LIGHT_RED)

				except KeyboardInterrupt:
					xout.print("Input canceled.", xout.YELLOW)
					break
		else:
			xout.line()
			func()

if __name__ == "__main__":
	xout = FXOutput()

	config = Config()
	try:
		json_file = SERVER_DATA_FILE
		data = config.load_config(json_file)
		m2 = M2(data)

		if len(sys.argv) > 1:
			cmd = sys.argv[1]
			if cmd in OPTION_ARG_DICT.keys():
				if cmd == "install":
					if len(sys.argv) > 2:
						sub_cmd = sys.argv[2]
						if sub_cmd in OPTION_ARG_DICT[cmd].keys():
							m2.run(OPTION_ARG_DICT[cmd][sub_cmd]["key"])
						else:
							xout.print(f"Unknown command: {cmd} {sub_cmd}", xout.LIGHT_RED)
					else:
						xout.print("Usage: install <option name>", xout.LIGHT_YELLOW)
						for option_name, option_dict in OPTION_ARG_DICT[cmd].items():
							xout.print(f"> {option_name:<15} {option_dict['label']}", xout.GRAY)
				else:
					m2.run(OPTION_ARG_DICT[cmd]["key"])
			elif cmd == "backup":
				try:
					if len(sys.argv) > 3:
						m2.server.backup(sys.argv[2], sys.argv[3])
					else:
						m2.server.backup(sys.argv[2])
				except Exception as error:
					xout.print(f"Error: {error}", xout.LIGHT_RED)
					xout.print("Usage: --backup <type (mysql, server, all)>", xout.LIGHT_YELLOW)
			else:
				xout.print(f"Unknown option: {cmd}", xout.LIGHT_RED)
		else:
			m2.run()

	except FileNotFoundError:
		xout.print(f"Cannot find the file {json_file}", xout.LIGHT_RED)

	except json.JSONDecodeError:
		xout.print(f"Error decoding JSON in {json_file}", xout.LIGHT_RED)
