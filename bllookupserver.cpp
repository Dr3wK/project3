/***************************************************************************
 * bllookupserver.cpp  -  Program to serve Bible search results
 *
 * copyright : (C) 2009 by Jim Skon
 * modified by : Drew Koning 2025
 ***************************************************************************/
 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>


#include <map>
#include "fifo.h"
#include "Bible.h"


// TODO return verse line as string

using namespace std;

// TODO determine correct pipe names
string receive_pipe = "BLrequest";
string send_pipe = "BLreply";



/* Server main line,create name MAP, wait for and serve requests */
int main() {
    /* Create the communication fifos */
    Bible webBible("/home/class/csc3004/Bibles/web-complete");
    Fifo recfifo(receive_pipe);
    Fifo sendfifo(send_pipe);

    

    while (true) {

        recfifo.openread();
        sendfifo.openwrite();
        // Receive a request from the client
        string request = recfifo.recv();
        cout << "Received request: " << request << endl;
        
        string bookStr, chapterStr, verseStr, numVersesStr;
        istringstream stream(request);
        getline(stream, bookStr, ':');
        getline(stream, chapterStr, ':');
        getline(stream, verseStr, ' ');
        getline(stream, numVersesStr); 

        // Default number of verses to 1 if not specified
        int numOfVerses;
        if (numVersesStr.empty()) {
            numOfVerses = 1;
        }
        else {
            numOfVerses = stoi(numVersesStr);
        }

        // Convert the parsed strings to integers
        int bookNum = stoi(bookStr);
        int chapterNum = stoi(chapterStr);
        int verseNum = stoi(verseStr);

        // Catch empty reference
        if (bookStr.empty() || chapterStr.empty() || verseStr.empty() || !isdigit(bookStr[0])) {
            sendfifo.send("Error: Invalid input format. Expected format is <book>:<chapter>:<verse> <number of verses>");
            continue;
        }

        // Create a Ref object using the four-argument constructor
        Ref ref(bookNum, chapterNum, verseNum, numOfVerses);
        LookupResult status;
        Verse verse = webBible.lookup(ref, status);

        if (status == SUCCESS) {
            string response = ref.getStrBookName() + " " + to_string(ref.getChap()) + "\n";

            // Retrieve and concatenate multiple verses in repsonce
            for (int i = 0; i < numOfVerses; ++i) {
                cout << to_string(ref.getAmountVerses()) << endl;
                verse = webBible.lookup(ref, status);
                cout << status << endl;
                if (status != SUCCESS) { break; }

                response += to_string(ref.getVerse()) + " " + verse.getVerse() + "\n"; // ref.getVerse() returns verse number
                                                                                       // verse.getVerse() returns the verse text
                ref = webBible.next(ref, status); // Move to the next verse
            }
            sendfifo.send(response);
        }
        else {
            // Send error through pipe
            sendfifo.send(webBible.error(ref, status));
        }

            recfifo.fifoclose();
            sendfifo.fifoclose();
    }
    
    return 0;
}
