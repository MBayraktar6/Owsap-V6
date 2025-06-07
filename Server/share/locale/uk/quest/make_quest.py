import os
import subprocess
import pre_qc
import sys

__WIN__ = sys.platform.startswith('win')

QC_FILE_NAME = 'quest_list'
QC_ROOT_PATH = 'source'
PRE_QC_PATH = 'pre_qc/'
QC_EXTENSION_LIST = ('.quest')
if __WIN__:
	QC_COMMAND_STRING = ' call qc_d.exe {}'
else:
	QC_COMMAND_STRING = './qc {}'
QC_COMMENT_CHARACTER = '#'

os.chdir(os.path.dirname(os.path.realpath(__file__)))

if __WIN__:
	os.system('del {}'.format(QC_FILE_NAME))
	os.system('del object')
else:
	os.system('rm -f {}'.format(QC_FILE_NAME))
	os.system('rm -rf object')

os.system('mkdir object')
if not os.path.exists('pre_qc'):
	os.system('mkdir pre_qc')

if not __WIN__:
	os.system('chmod -R 770 object')
	# os.system('chgrp quest object')

QC_LIST = open(QC_FILE_NAME, "w")
QC_LOG_DICT = {'ok': 0, 'error': 0}
QC_FAILED = []

for root, dirs, files in os.walk(QC_ROOT_PATH):
	for fileName in files:
		if fileName.endswith(QC_EXTENSION_LIST):
			filePath = "{}/{}".format(root, fileName)

			preQC = pre_qc.run(filePath, fileName)
			if preQC == True:
				quest = PRE_QC_PATH + fileName
			else:
				quest = filePath

			hasCompiled = subprocess.call(QC_COMMAND_STRING.format(quest), shell=True) == 0
			if not hasCompiled:
				QC_FAILED.append(quest)
				print("Failed: {}".format(quest))

			QC_LOG_DICT['ok' if hasCompiled else 'error'] += 1
			QC_LIST.write(quest + "\n")

QC_LIST.close()
print('succeeded: {}, failed: {}.'.format(*QC_LOG_DICT.values()))
if QC_FAILED:
	for quest in QC_FAILED:
		print(' failed: {}'.format(quest))

if __WIN__:
	os.system("pause")
