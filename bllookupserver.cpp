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

    recfifo.openread();
    sendfifo.openwrite();

    while (true) {
        // Receive a request from the client
        string request = recfifo.recv();
        cout << "Received request: " << request << endl;
        // TODO: Parse first 3 chars uding colon as delimiter
        // Get book number
        string strbook = GetNextToken(request, ":");
        int intbook = stoi(strbook);

        // Get the chapter number
        string strchap = GetNextToken(request, ":");
        int intchap = stoi(strchap);

        // Get the verse number
        string strverse = GetNextToken(request, " ");
        int intverse = stoi(strverse);

        // IF in format <Book>:<Chapter>:<Verse> Print verse, else print err
        if (intbook > 0 && intbook < 67 &&
            intchap > 0 && intchap < 151 &&
            intverse > 0 && intverse < 200) {
            // Parse request into a Ref object
            Ref ref(request);
            LookupResult status;
            Verse verse = webBible.lookup(ref, status);

            // Prepare response
            string response = request;
            if (status == SUCCESS) {
                // Send verse refernce, verse number, and verse text
                response = ref.getStrBookName() + " " + to_string(ref.getChap()) +
                    "\n" + to_string(ref.getVerse()) + " " + verse.getVerse();
            }
            else {
                response = webBible.error(ref, status);
            }

            // Send response back to the client
            sendfifo.send(response);
            cout << "Sent reply: " << response << endl;
        }
        else {
            sendfifo.send("Err: incorrect format");
        }
    }

    recfifo.fifoclose();
    sendfifo.fifoclose();
    return 0;
}
