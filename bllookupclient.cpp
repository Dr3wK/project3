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
#include "Bible.h"
using namespace std;



//#define logging // enable log file
//#include "logfile.h"

// Pipes for communication 
// TODO determine correct pipe names                                                    
string receive_pipe = "BLreply";
string send_pipe = "BLrequest";

string buildSearchString(int bk, int chpr, int vrs, int numVrs) {
    return to_string(bk) + ":" + to_string(chpr) + ":" +
        to_string(vrs) + ":" + to_string(numVrs); // Updated format
}


int main() {
  // prepare the response output,
  // send required header before any other output
    cout << "Content-Type: text/plain\n\n" << endl;

    Cgicc cgi;
    // GET THE INPUT DATA
    // browser sends us a string of field name/value pairs from HTML form
    // retrieve the value for each appropriate field name
    form_iterator st = cgi.getElement("search_type");
    //form_iterator bible = cgi.getElement("bible");
    form_iterator book = cgi.getElement("book");
    form_iterator chapter = cgi.getElement("chapter");
    form_iterator verse = cgi.getElement("verse");
    form_iterator nv = cgi.getElement("num_verse");

    // Convert to integers
    short bookNum = book->getIntegerValue();
    short chapterNum = chapter->getIntegerValue();
    short numOfVerses = nv->getIntegerValue();
    short verseNum = verse->getIntegerValue();
    //short bibleNum = bible->getIntegerValue();

    // Convert and check input data
    bool validInput = false;
    if (chapter != cgi.getElements().end()) {

        if (chapterNum > 150) {
            cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>" << "<br>";
        }
        else if (chapterNum <= 0) {
            cout << "<p>The chapter must be a positive number.</p>" << "<br>";
        }
        else if (verseNum <= 0) {
            cout << "<p>The verse must be a positive number.</p>" << "<br>";
        }
        // largest chapter has 176 verses
        else if (verseNum > 176) {
            cout << "<p>The Verse number (" << verseNum << ") is too high.</p>" << "<br>";
        }
        else if (numOfVerses <= 0) {
            cout << "<p>The number of verses must be a positive number.</p>" << "<br>";
        }
        else
            validInput = true;
    }


    Fifo recfifo(receive_pipe);
    Fifo sendfifo(send_pipe);

    sendfifo.openwrite();
    recfifo.openread();

    LookupResult result = OTHER;
    string searchString;

    if (validInput) {
        searchString = buildSearchString(bookNum, chapterNum, verseNum, numOfVerses); // Updated format
    }
    else {
        cout << "<p>Invalid Input: </p>" << endl;
    }



    /*
    string bibleVersionArray[3] = {
        "/home/class/csc3004/Bibles/web-complete",
        "/home/class/csc3004/Bibles/kjv-complete",
        "/home/class/csc3004/Bibles/ylt-complete",
    };
    */

    // Send request to server
    sendfifo.send(searchString);
    cout << "Sent request: " << searchString << endl;

    // Receive and print the server response
    string response = recfifo.recv();
    while (response != "$end") {
        cout << response << "<br>" << endl;
        response = recfifo.recv();
    }

    recfifo.fifoclose();
    sendfifo.fifoclose();
    
    return 0;
}

