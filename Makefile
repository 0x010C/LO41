#my general makefile template
CXX = gcc
CFLAGS = -Wall -Werror -ansi -pedantic -fpic

#Application-related macros
P_TARGET = padre
P_EXESOURCE = padre ipc processTree workstation hmi
P_EXESOURCECFILE = $(P_EXESOURCE:=.c)

A_TARGET = atelier
A_EXESOURCE = atelier ipc
A_EXESOURCECFILE = $(A_EXESOURCE:=.c)

#To overload the content of the LD_LIBRARY_PATH
# export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:.

#Generating the executable
all: padre atelier
	@echo "\n--> Done :)"
	
padre:
	@echo "\n--> Generating the parent executable" $@
	$(CXX) $(P_EXESOURCECFILE) -o $(P_TARGET)

atelier:
	@echo "\n--> Generating the child executable" $@
	$(CXX) $(A_EXESOURCECFILE) -o $(A_TARGET)

exec: all
	./padre

#Cleaning the content of the current directory
clean:
	@echo "\n Cleaning temporary files"
	rm -rf *.o *~ *.save *.exe padre atelier
