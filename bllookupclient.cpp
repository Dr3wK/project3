/* bllookupclient.cpp
 * Bile verse search client:
 * receives an Ajax request from web client
 * extracts search string, and sends request to bllookupserver
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


//#define logging // enable log file
//#include "logfile.h"

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

    Fifo recfifo(receive_pipe);
    Fifo sendfifo(send_pipe);

    sendfifo.openwrite();
    recfifo.openread();

    // Send request to server
    sendfifo.send(searchString);
    cout << "Sent request: " << searchString << endl;

    // Receive and print the server response
    string response = recfifo.recv();
    while (response != "$end") {
        cout << response << "<br>" << endl;
        response = recfifo.recv();
    }

    
    return 0;
}
