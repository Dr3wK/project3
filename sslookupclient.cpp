/* sslookupclient.cpp
 * Shakespeare full-text search client:
 * receives an Ajax request from web client
 * extracts search string, and sends request to sslookupserver
 */
#include <iostream>
#include <fstream>
#include <string>

// Stuff for Ajax
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

//Stuff for pipes                                                               
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include "fifo.h"
using namespace std;

#include "textindex.h"

#define logging // enable log file
#include "logfile.h"

// Pipes for communication 
// TODO determine correct pipe names                                                    
string receive_pipe = "BLreply";
string send_pipe = "BLrequest";

int main() {
  // prepare the response output,
  // send required header before any other output
  cout << "Content-Type: text/plain\n\n" << endl;
  
  Cgicc cgi;
  form_iterator sstring = cgi.getElement("sstring");
  string searchString = **sstring;
  int length = searchString.length();
  
  Fifo recfifo(receive_pipe);
  Fifo sendfifo(send_pipe);
  
#ifdef logging
  logFile.open(logFilename.c_str(),ios::out);
#endif
  
  sendfifo.openwrite();
  log("Open request pipe");
  
  // Call server to get results                                                 
  // TODO: search String needs to be Ref
  sendfifo.send(searchString);
  log("Request: "+searchString);

  recfifo.openread();
  log("Open reply fifo");

  // output the response to the web page
  string results = "";
  int times = 0; // Counter for header lines
  while (results != "$end") {
    results = recfifo.recv();
    log("Reply: "+results);
    if (results != "$end") {
      cout << results << endl;
      if (times++ > 2) {
	cout << "<br>";
      }
    }
  }
  cout << endl; // flush output when done
  recfifo.fifoclose();
  log("close reply fifo");
  sendfifo.fifoclose();
  log("close request fifo");
  return 0;
}
