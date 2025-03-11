/***************************************************************************
 * sslookupserver.cpp  -  Program to serve Shakespeare search results
 *
 * copyright : (C) 2009 by Jim Skon
 ***************************************************************************/
 
#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include "fifo.h"
#include "textindex.h"

// TODO return verse line as string

// Include stemming stuff                                              
#include "english_stem.h"

using namespace std;

// TODO determine correct pipe names
string receive_pipe = "BLrequest";
string send_pipe = "BLreply";

string convertInt(int number)
{
  stringstream ss;//create a stringstream
  ss << number;//add number to the stream
  return ss.str();//return a string with the contents of the stream
}

/* Server main line,create name MAP, wait for and serve requests */
int main() {
  list<int> result;
  list<int>::iterator it;
  int position;
  string fileName = TEXT_FILENAME;
  ifstream textFile;
  string line;  // A line from the text file
  string message;
  stemming::english_stem<char, std::char_traits<char> > StemEnglish;

  /* Build an index of the text */
  int status = buildTextIndex (fileName);

  /* Create the communication fifos */
  Fifo recfifo(receive_pipe);
  Fifo sendfifo(send_pipe);


  // Open the text file                 
  textFile.open(fileName.c_str(), ios::in);
  if (textFile.bad()) {
    cout << "File: " << fileName << " cannot be opened" << endl;
    exit (1);
  }

  recfifo.openread(); // only open once
  
  while (1) { // "infinite loop" for server: repeat each time a request is received

    recfifo.openread(); // NOT NEEDED!

    /* Get a message from a client */
    string searchWord = recfifo.recv();
    
    /* get the stem of the search word */
    string stemmedWord=searchWord;
    StemEnglish(stemmedWord);

    cout << "Message: |" << stemmedWord << "|" << stemmedWord.length() << endl;
    sendfifo.openwrite();

    /* Send back the search term message */
    message =  "Search term: <i>" + searchWord + "</i>, Stemmed to: <i>" + stemmedWord + "</i>"; 
    message = "<h3>"+message+"</h3>";
    sendfifo.send(message);

    if (stemmedWord.length() > 1) {

      // search for all lines with this word
      result = indexSearch(stemmedWord);
      int results = result.size();
      
      if (result.size() < 1) {
	cout << "No Matches" << endl;
	message = "No Matches";
	sendfifo.send(message);
      } else {
	
	// Give the match count                                                    
	message =  "Matches: " + convertInt(results);
	cout << message << endl;
	message = "<h3>"+message+"</h3>";
	sendfifo.send(message);
	/* Traverse the list of references */
	for (it = result.begin() ; it != result.end(); it++)
	  {
	    // Grab the postion of the reference out of the iterator               
	    position = (*it);
	    // Seek that line in the file, and display it                  
	    textFile.seekg (position, ios::beg);
	    getline(textFile,line);
	    //cout << line << endl;
	    sendfifo.send(line);
	  }
	
      } // results


    } else { // word length not > 1
      message = "Stemmed word must be at least 2 characters long: " +stemmedWord;
      sendfifo.send(message);
    }
    message = "$end";
    sendfifo.send(message);
    sendfifo.fifoclose(); 
    // recfifo.fifoclose();   
  }
}
