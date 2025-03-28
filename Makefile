# Bibleajax program
# Bob Kasper, MVNU Computer Science
# updated January 2020 to use
# c++11 compiler option, current paths on cs.mvnu.edu
# Additions by Drew Koning

# This is a Makefile for the Bible web app demo program.
# Copy this directory to a location within your home directory. 
# Change the USER name value below to your own user name.
# Then use "make" to build the server program,
# and deploy it to the live web server directory.
# To test the program, go to http://cs.mvnu.edu/class/csc3004/USER/
# and open the bibleajax.html link.

# TO DO: Must replace "username" by your username
USER= drekoning

# Use GNU C++ compiler with C++11 standard
CC= g++
CFLAGS= -g -std=c++11

all: bllookupserver bllookupclient testReader PutCGI PutHTML
# PutCGI PutHTML

# TO DO: For bibleajax.cgi, add dependencies to include
# compiled classes from Project 1 to be linked into the executable program

	
# Lookup Server target
bllookupserver: bllookupserver.o fifo.o Bible.o Ref.o Verse.o
	$(CC) $(CFLAGS) -o bllookupserver bllookupserver.o fifo.o Bible.o Ref.o Verse.o

bllookupserver.o: bllookupserver.cpp Bible.h Ref.h Verse.h fifo.h
	$(CC) $(CFLAGS) -c bllookupserver.cpp

# Lookup Client target
bllookupclient: bllookupclient.o fifo.o
	$(CC) $(CFLAGS) -o bllookupclient bllookupclient.o fifo.o -lcgicc

bllookupclient.o: bllookupclient.cpp fifo.h
	$(CC) $(CFLAGS) -c bllookupclient.cpp

# TO DO: copy targets to build classes from Project 1:
# Bible.o, Ref.o, Verse.o
Ref.o : Ref.h Ref.cpp
	$(CC) $(CFLAGS) -c Ref.cpp

Verse.o : Verse.h Ref.h Verse.cpp
	$(CC) $(CFLAGS) -c Verse.cpp

Bible.o : Bible.h Ref.h Verse.h Bible.cpp
	$(CC) $(CFLAGS) -c Bible.cpp 


# Build fifo.o
fifo.o : fifo.h fifo.cpp
	$(CC) $(CFLAGS) -c fifo.cpp

#Build testReader 
testReader: testReader.o Ref.o Verse.o Bible.o fifo.o
	$(CC) $(CFLAGS) -o testReader testReader.o Ref.o Verse.o Bible.o fifo.o
	
#Compile testReader
testReader.o: Ref.h Verse.h Bible.h testReader.cpp
	$(CC) $(CFLAGS) -c testReader.cpp

PutCGI: bllookupclient
		chmod 757 bllookupclient
		cp bllookupclient /var/www/html/class/csc3004/$(USER)/cgi-bin
		echo "Current contents of your cgi-bin directory: "
		ls -l /var/www/html/class/csc3004/$(USER)/cgi-bin/

PutHTML:
	cp bibleindex.html /var/www/html/class/csc3004/$(USER)

	echo "Current contents of your HTML directory: "
	ls -l /var/www/html/class/csc3004/$(USER)


clean:
	rm *.o bllookupserver bllookupclient testReader
	

