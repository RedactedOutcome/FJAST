#Architecture can be [x64]
Platform = Windows
Configuration = Release
TargetName = FJASTP
Architecture = x64

Config=$(Configuration)/$(Architecture)-$(Platform)/
OutputDir = bin/$(TargetName)/$(Config)
IntDir = bin-int/$(TargetName)/$(Config)
ExampleOutputDir=bin/example/$(Config)
ExampleIntDir=bin-int/example/$(Config)
Include = include/

MF =
RF = 
CC = 
LK = 
Files =
Defines =
CFlags = 
LFlags = 
Files = 
SRC_DIR = src/

BoostDir = ${BOOST_ROOT}
HBUFFER_LIB_SRC = libs/HBuffer/

ifeq ($(Platform), Windows)
include WindowsMake.mk
else
$(error Invalid Platform)
endif

default: build

clean:
	$(RF) bin-int
make_folders:
	mkdir -p $(IntDir)
	mkdir -p $(OutputDir)
build_pch:
	$(CC) $(CFlags) $(LibCFlags) $(Defines) $(IncludeDirs) /Ycpch.h /Fp$(IntDir)pch.pch src/pch.cpp
build: make_folders $(IntDir)pch.pch
	$(CC) $(Files) $(CFlags) $(LibCFlags) $(Defines) $(IncludeDirs) /Yupch.h /Fp$(IntDir)pch.pch 
	$(LibCreator) $(LFlags) $(LibFlags) $(LibDirs) $(Libs)
build_example:
	$(MF) $(ExampleIntDir)
	$(MF) $(ExampleOutputDir)
	$(CC) $(ExampleFiles) $(CFlags) $(ExampleCFlags) $(Defines) $(IncludeDirs) /Ycpch.h /Fp$(IntDir)pch.pch
	$(LK) $(LFlags) $(ExampleLFlags) $(LibDirs) $(Libs) $(ExampleLibs) $(ExampleLibDirs)
run:
	$(ExampleOutputDir)Example.exe
buildnrun: build run
rebuild: make_folders build_pch build
rebuildnrun:make_folders build_pch build run
build_allnrun: build build_example run
build_examplenrun: build_example run

$(IntDir)pch.pch: build_pch