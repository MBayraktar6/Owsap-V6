import os
folders = [
	"onehand_sword",
	"twohand_sword",
	"dualhand_sword",
	"bow",
	"bell",
	"fan",
	"claw",
	"horse_onehand_sword",
	"horse_twohand_sword",
	"horse_dualhand_sword",
	"horse_bow",
	"horse_bell",
	"horse_fan",
	"horse_claw"
]
founder_folder = []
rootdir = os.getcwd()
for subdir, dirs, files in os.walk(rootdir):
	for folder in dirs:
		if folder in folders:
			founder_folder.append(folder)

difference = list(set(folders) - set(founder_folder))
print("missing: %s" % difference)

os.system("pause")
