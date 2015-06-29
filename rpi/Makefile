#==============================================================================
# Project folders
#==============================================================================

#Main folders
SRCFOLDER := src/
INCFOLDER := include/
BINFOLDER := bin/
LIBFOLDER := lib/
OBJFOLDER := obj/
TESTFOLDER := test/
OTHERFOLDER := other/
DOCFOLDER := doc/

#==============================================================================
# Project flags
#==============================================================================
CC = g++ $(SIMPLEWARNINGS)
SIMPLEWARNINGS := -Wall
ALLWARNINGS := -Wall -Wextra -pedantic -Wshadow -Wredundant-decls -Woverloaded-virtual -Wsynth
IFLAG = -I./$(INCFOLDER)
GOOGLETESTFLAG = -lgtest -lgtest_main

#==============================================================================
# Project Strings
#==============================================================================
TESTSEPARATOR = "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
CLEANSEPARATOR = "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "
#==============================================================================
# Project files
#==============================================================================
CPPFILES = $(wildcard src/*.cpp)
TESTCPPFILES = $(wildcard test/*.cpp)

all: prepare rpi

rpi: srcObjects
	$(CC) obj/*.o -o bin/app/$@ $(IFLAG)

#==============================================================================
# Preparing object files
#==============================================================================
# Files
srcObjects: $(CPPFILES:src/%.cpp=obj/%.o)

#Necessary use include for PostgreSQL library here too, if it is used in other file.
obj/%.o: src/%.cpp
	$(CC) -c $< -o $@ $(IFLAG)

#==============================================================================
# Testing
# About: The command test compile all tests and put it in the folder bin/test.
# 	You should have a file in the src folder mapped into test. To make it 
#		organized, you should keep the same folder structure in test folder.
# Example:
#		make test  -> Compile all the tests
#		make testModel -> Compile all the tests in model folder.
#==============================================================================
test: tests

# Test Model
tests: $(TESTCPPFILES:test/%_test.cpp=bin/test/%.bin)

bin/test/%.bin: test/%_test.cpp src/%.cpp
	$(CC) $^ -o $@ $(IFLAG) $(GOOGLETESTFLAG)

#=====================================================================================
# Project Actions
#=====================================================================================
PHONY: clean prepare doxy runTest

doxy:
	doxygen Doxyfile 

run:
	$(APPBINFOLDER)rpi

runTest:
	$(TESTBINFOLDER)*.bin

prepare:
	mkdir -p $(BINFOLDER)test
	mkdir -p $(BINFOLDER)app

clean:
	@echo $(CLEANSEPARATOR)
	rm -fr bin/*
	rm -fr obj/*

