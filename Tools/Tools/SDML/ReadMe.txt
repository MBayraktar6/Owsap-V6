SDML (Server Data Motion List)
This script filters out files from the client to the server's data monster folder,
checking only for motion-related files in models and ignoring any unnecessary file types.

.
├─── 7za
│	7-Zip Standalone
│
├─── DML (Default Motion List)
│	This folder includes default motion files for models that don't have any motion files on the client.
│
└─── Ymir Work
	All in one folder for all models. The package folders below must be dumped in this directory.
	- d:/ymir work/monster/*
	- d:/ymir work/monster2/*
	- d:/ymir work/npc/*
	- d:/ymir work/npc_mount/*
	- d:/ymir work/npc_pet/*
	- d:/ymir work/npc2/*

After placing all the model folders in the "Ymir Work" folder, copy all the model folders from "DML"
to the "Ymir Work" folder. If there are any existing files, do not overwrite the copied files.
