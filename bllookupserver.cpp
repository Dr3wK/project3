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
        getline(stream, verseStr, ':');
        getline(stream, numVersesStr);

        // Diagnostic Data to see what reference the program made in the console
        cout << "book: " << bookStr << " chapter: " << chapterStr << " verse: " << verseStr << " number of verses: " << numVersesStr << endl;

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

        recfifo.fifoclose();
        sendfifo.fifoclose();

        // Create a Ref object using the four-argument constructor
        Ref ref(bookNum, chapterNum, verseNum, numOfVerses);
        LookupResult status = SUCCESS;
        Verse verse = webBible.lookup(ref, status);
        
        recfifo.openread();
        sendfifo.openwrite();

        if (status == SUCCESS) {

            // Send chapter ref header
            Ref tempRef = verse.getRef();
            string headerRefStr = tempRef.getStrBookName() + " " + to_string(tempRef.getChap());
            sendfifo.send(headerRefStr);
            
            // Send first verse and get next one
            string response = to_string(ref.getVerse()) + " " + verse.getVerse(); // ref.getVerse() returns verse number
            sendfifo.send(response);
            ref = webBible.next(ref, status); // Move to the next verse

            // Retrieve next verse if success and number of verses is greater than 2
            for (int i = 1; i < numOfVerses; ++i) {
               
                verse = webBible.lookup(ref, status);

                // Send end msg if lookup cannot find verse
                if (status != SUCCESS) {
                    string endMessage = "$end";
                    sendfifo.send(endMessage);
                    break;
                }

                response = to_string(ref.getVerse()) + " " + verse.getVerse(); // ref.getVerse() returns verse number, 
                                                                               // verse.getVerse() gets only the verse text

                // Check to see if it is a new chapter, if it is print the new reference
                Ref tempRef = verse.getRef();
                if (tempRef.getVerse() == 1) {
                    response = "<br>" + tempRef.getStrBookName() + " " + to_string(tempRef.getChap()) + "<br>" + response;
                }

                sendfifo.send(response);
                ref = webBible.next(ref, status); // Move to the next verse
            }

        }
        else {
            // Send error through pipe
            sendfifo.send(webBible.error(ref, status));
        }
        string endMessage = "$end";
        sendfifo.send(endMessage);

        recfifo.fifoclose();
        sendfifo.fifoclose();
    }
    return 0;
}
