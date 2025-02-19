/* Demo server program for Bible lookup using AJAX/CGI interface
 * By James Skon, Febrary 10, 2011
 * updated by Bob Kasper, January 2020
 * Mount Vernon Nazarene University
 * Additions by Drew Koning
 * 
 * This sample program works using the cgicc AJAX library to
 * allow live communication between a web page and a program running on the
 * same server that hosts the web server.
 *
 * This program is run by a request from the associated html web document.
 * A Javascript client function invokes an AJAX request,
 * passing the input form data as the standard input string.
 *
 * The cgi.getElement function parses the input string, searching for the matching
 * field name, and returing a "form_iterator" oject, which contains the actual
 * string the user entered into the corresponding field. The actual values can be
 * accessed by dereferencing the form iterator twice, e.g. **verse
 *     refers to the actual string entered in the form's "verse" field.
 */


#include <iostream>
#include <stdio.h>
#include <string.h>
using namespace std;

/* Required libraries for AJAX to function */
#include "/home/class/csc3004/cgicc/Cgicc.h"
#include "/home/class/csc3004/cgicc/HTTPHTMLHeader.h"
#include "/home/class/csc3004/cgicc/HTMLClasses.h"
using namespace cgicc;

#include "Ref.h"
#include "Verse.h"
#include "Bible.h"

int main() {
	/* A CGI program must send a response header with content type
	 * back to the web client before any other output.
	 * For an AJAX request, our response is not a complete HTML document,
	 * so the response type is just plain text to insert into the web page.
	 */
	cout << "Content-Type: text/plain\n\n";

	Cgicc cgi;  // create object used to access CGI request data

	// GET THE INPUT DATA
	// browser sends us a string of field name/value pairs from HTML form
	// retrieve the value for each appropriate field name
	form_iterator st = cgi.getElement("search_type");
	form_iterator bible = cgi.getElement("bible");
	form_iterator book = cgi.getElement("book");
	form_iterator chapter = cgi.getElement("chapter");
	form_iterator verse = cgi.getElement("verse");
	form_iterator nv = cgi.getElement("num_verse");

	// Convert to integers
	short bookNum = book->getIntegerValue();
	short verseNum = verse->getIntegerValue();
	short numVerse = nv->getIntegerValue();
	short chapterNum = chapter->getIntegerValue();
	short bibleNum = bible->getIntegerValue();
	
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
		else if (numVerse <= 0) {
			cout << "<p>The number of verses must be a positive number.</p>" << "<br>";
		}
		else
			validInput = true;
	}

	/* TO DO: OTHER INPUT VALUE CHECKS ARE NEEDED ... but that's up to you! */

	/* TO DO: PUT CODE HERE TO CALL YOUR BIBLE CLASS FUNCTIONS
	 *        TO LOOK UP THE REQUESTED VERSES
	 */
	
	LookupResult result = OTHER;

	string bibleVersionArray[3] = {
		"/home/class/csc3004/Bibles/web-complete",
     	"/home/class/csc3004/Bibles/kjv-complete",
     	"/home/class/csc3004/Bibles/ylt-complete",
	};
		
	// get Bible stored at index
	Bible webBible(bibleVersionArray[bibleNum - 1]);

	/* SEND BACK THE RESULTS
	 * Finally we send the result back to the client on the standard output stream
	 * in HTML text format.
	 * This string will be inserted as is inside a container on the web page,
	 * so we must include HTML formatting commands to make things look presentable!
	 */

	if (validInput) {
		//cout << "Search Type: <b>" << **st << "</b>";

		Ref ref(bookNum, chapterNum, verseNum, numVerse);

		Verse bibleVerse = webBible.lookup(ref, result);

		cout << "<p>"
			<< ref.getStrBookName() << " " << **chapter << ":" << **verse << "<br>";
		if (result == 0) {
			// display the verse if the verse was successfully found.
			cout << to_string(ref.getVerse()) << " " << bibleVerse.getVerse();
		}
		if (numVerse > 1 && result == 0) {
			for (int i = 0; i < numVerse - 1; i++) {

				// set verse object to the next verse
				bibleVerse = webBible.nextVerse(result);

				// Check to see if it is a new chapter, if it is print the new reference
				Ref tempRef = bibleVerse.getRef();

				if (tempRef.getVerse() == 1) {

					cout << "<br><br>" << tempRef.getStrBookName() << " " << to_string(tempRef.getChap());
				}

				// Check to see if empty ref
				// Default value of empty is 0, 0, 0
				// Prints if book is not default value
				if (tempRef.getBook() != 0) {
					cout << "<br>" << to_string(tempRef.getVerse()) << " " << bibleVerse.getVerse();
				}
			}
		}
		cout << "<br>";
	}
	else {
		cout << "<p>Invalid Input: <em>report the more specific problem.</em></p>" << endl;
	}
	return 0;
}
