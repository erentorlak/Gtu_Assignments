# Makefile for compiling and running tests

# Compiler
CXX = g++
CXXFLAGS = -std=c++11 

# Targets
TARGETS = makeFileSystem fileSystemOper

# Source files
MAKE_FILESYSTEM_SRC = part2.cpp
FILESYSTEM_OPER_SRC = part3.cpp

# Executable files
MAKE_FILESYSTEM_EXE = makeFileSystem
FILESYSTEM_OPER_EXE = fileSystemOper

# File system name
FS_NAME = mySystem.data

run : clean all test

all: $(TARGETS)

$(MAKE_FILESYSTEM_EXE): $(MAKE_FILESYSTEM_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(FILESYSTEM_OPER_EXE): $(FILESYSTEM_OPER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^
	
test: 
	./$(MAKE_FILESYSTEM_EXE) 1 $(FS_NAME)	

	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) mkdir /usr
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) mkdir /usr/ysa
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) mkdir /bin/ysa
	echo "asd123" > linuxFile.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) write /usr/ysa/file1 linuxFile.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) write /usr/file2 linuxFile.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) write /file3 linuxFile.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) dir /
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) del /usr/ysa/file1
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) dumpe2fs
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) read /usr/file2 linuxFile2.data 
	cmp linuxFile.data linuxFile2.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) chmod /usr/file2 -rw
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) read /usr/file2 linuxFile2.data 
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) chmod /usr/file2 +rw
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) addpw /usr/file2 test1234
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) read /usr/file2 linuxFile2.data
	./$(FILESYSTEM_OPER_EXE) $(FS_NAME) read /usr/file2 linuxFile2.data test1234
	
clean:
	rm -f $(TARGETS) $(FS_NAME) linuxFile.data linuxFile2.data

.PHONY: all test clean
