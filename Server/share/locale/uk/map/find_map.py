import os

rectList = []

for path in open("index.txt"):
	if path[-1] == os.linesep:
		path = path[:-1]

	index = path.split()
	path = index[1]

	for line in open(path + "/" + "Setting.txt"):
		tokens = line.split()
		if not tokens:
			continue

		if tokens[0] == "BasePosition":
			x, y = [int(item) / 100 for item in tokens[1:3]]
		elif tokens[0] == "MapSize":
			w, h = [int(item) * 256 for item in tokens[1:3]]

	rectList.append((path, x, y, (x + w), (y + h)))

for rect in rectList:
	print("%s (%d, %d) (%d, %d)" % (rect[0], rect[1], rect[2], rect[3], rect[4]))

os.system("pause")
