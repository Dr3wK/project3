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
        
        // Parse request into a Ref object
        Ref ref(request);
        LookupResult status;
        Verse verse = webBible.lookup(ref, status);

        // Prepare response
        string response = request;
        /*if (status == SUCCESS) {
            response = verse.getVerse();
        }
        else {
            response = webBible.error(ref, status);
        }
        */

        // Send response back to the client
        sendfifo.send(response);
        cout << "Sent reply: " << response << endl;
    }

    recfifo.fifoclose();
    sendfifo.fifoclose();
    return 0;
}
