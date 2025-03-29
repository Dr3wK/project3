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

#define logging // enable log file
#define LOG_FILENAME "/home/class/csc3004/tmp/drekoning-client.log"
#include "logfile.h"


// Pipes for communication 
// TODO determine correct pipe names                                                    
string receive_pipe = "BLreply";
string send_pipe = "BLrequest";


int main() {
    // prepare the response output,
    // send required header before any other output
    cout << "Content-Type: text/plain\n\n" << endl;

    Fifo recfifo(receive_pipe);
    Fifo sendfifo(send_pipe);

    #ifdef logging
        logFile.open(logFilename.c_str(), ios::out);
    #endif

    sendfifo.openwrite();
    recfifo.openread();

    Cgicc cgi;
    // GET THE INPUT DATA
    // browser sends us a string of field name/value pairs from HTML form
    // retrieve the value for each appropriate field name
    form_iterator st = cgi.getElement("search_type");
    form_iterator book = cgi.getElement("book");
    form_iterator chapter = cgi.getElement("chapter");
    form_iterator verse = cgi.getElement("verse");
    form_iterator nv = cgi.getElement("num_verse");

    log("Got elements: book, chapter, verse, nv");
    // Convert to integers
    int bookNum = book->getIntegerValue();
    int chapterNum = chapter->getIntegerValue();
    int verseNum = verse->getIntegerValue();
    int numOfVerses = nv->getIntegerValue();

    log("book number after conversion: " + to_string(bookNum));
    log("chapter number after conversion: " + to_string(chapterNum));
    log("verse number after conversion: " + to_string(verseNum));
    log("number of verses to lookup after conversion: " + to_string(numOfVerses));

    
    // Convert and check input data
    bool validInput = false;
    if (chapter != cgi.getElements().end()) {

        if (chapterNum > 150) {
            cout << "<p>The chapter number (" << chapterNum << ") is too high.</p>";
            return 1;
        }
        else if (chapterNum <= 0) {
            cout << "<p>The chapter must be a positive number.</p>";
            return 1;
        }
        else if (verseNum <= 0) {
            cout << "<p>The verse must be a positive number.</p>";
            return 1;
        }
        // largest chapter has 176 verses
        else if (verseNum > 176) {
            cout << "<p>The Verse number (" << verseNum << ") is too high.</p>";
            return 1;
        }
        else if (numOfVerses <= 0) {
            cout << "<p>The number of verses must be a positive number.</p>";
            return 1;
        }
        else if (bookNum >= 66 && chapterNum >= 22 && verseNum > 21){
            cout << "<p>No Verse exists after Revelation 22:21 </p>";
            return 1;
        }
        else
            validInput = true;
    }

    if (bookNum < 1 || chapterNum < 1 || verseNum < 1 || numOfVerses < 1) {
        cout << "<p>Error: Invalid input values (must be positive).</p>";
        return 1;
    }
    // Build verse reference as string
    string searchString =  to_string(bookNum) + ":" + to_string(chapterNum) + ":" + to_string(verseNum) + ":" + to_string(numOfVerses);

    // Send request to server
    sendfifo.send(searchString);
     log("Sent request: " + searchString);

    // Start paragraph
    cout << "<p>";
    
    string response = recfifo.recv();
    while (response != "$end") {
        
        cout << response << "<br>";
        log("server response: " + response);
        response = recfifo.recv();
    }
    cout << "</p>";

    recfifo.fifoclose();
    sendfifo.fifoclose();
    
    return 0;
}

