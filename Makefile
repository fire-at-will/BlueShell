###########################################################
#
# Simple Makefile for Operating Systems Project 1
# tcush
#
###########################################################
.SUFFIXES: .h .c .cpp .l .o

CC = gcc
CPP = g++
CCOPTS =
CPPOPTS =
LEX = flex
LIBS = -ll		#lfl on linux
RM = /bin/rm
RMOPTS = -f

tcush: tcush.yy.o tcush.o
	$(CPP) $(CPPOPTS) tcush.o tcush.yy.o -o tcush -ll

tcush.o: tcush.cpp
	 $(CPP) $(CPPOPTS) -c tcush.cpp

tcush.yy.o: tcush.yy.c
	$(CPP) $(CPPOPTS) -c tcush.yy.c

tcush.yy.c: tcush.l
	$(LEX) -o tcush.yy.c tcush.l

clean:
	$(RM) $(RMOPTS) *.o *~ core tcush tcush.yy.c
