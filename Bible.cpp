// Bible class function definitions
// Computer Science, MVNU
// Additions by Drew Koning

#include "Ref.h"
#include "Verse.h"
#include "Bible.h" 
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

Bible::Bible() { // Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";
}

// Constructor – pass bible filename
Bible::Bible(const string s) { infile = s; }

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status) {

	string line;

	// default verse
	Verse tempVerse;

	status = OTHER; // placeholder until retrieval is attempted
	instream.open(infile, ios::in);

	// Check to see if file is open
	if (!instream.is_open()) {
		cout << "Error, can't open input file: " << infile << endl;
		return tempVerse;
	}

	// Book out of bounds
	if (ref.getBook() < 1 || ref.getBook() > MAX_BIBLE) {
		status = NO_BOOK;
		cout << error(ref, status) << endl;
		return tempVerse;
	}

	// Initialize the max chapter and verse numbers for the given book
	short maxChap = 0, maxVerse = 0;
	while (getline(instream, line)) {
		// Find the first space separating reference and verse text
		int firstSpacePos = line.find_first_of(' ');
		// Create substring with remaining text after initial space
		string verseText = line.substr(0, firstSpacePos); //could possibly work now

		string strbook = GetNextToken(verseText, ":");
		short book = atoi(strbook.c_str());
		// Get the chapter number
		string strchap = GetNextToken(verseText, ":");
		short chap = atoi(strchap.c_str());
		// Get the verse number
		string strverse = GetNextToken(verseText, " ");
		short verseNum = atoi(strverse.c_str());

		// Set the max chapter and verse for each book for error handling
		if (ref.getBook() == book) {
			if (chap > maxChap) { maxChap = chap; }
			if (chap == ref.getChap() && verseNum > maxVerse) { maxVerse = verseNum; }
		}
		if (book > ref.getBook()) 
			break;  // break if we pass book
	}

	// Check for chapter out of bounds
	if (ref.getChap() < 1 || ref.getChap() > maxChap) {
		status = NO_CHAPTER;
		cout << error(ref, status) << endl;
		return tempVerse;
	}

	// Check for verse out of bounds
	if (ref.getVerse() < 1 || ref.getVerse() > maxVerse) {
		status = NO_VERSE;
		cout << error(ref, status) << endl;
		return tempVerse;
	}
	

	// Reset stream to beginning to find the actual verse
	instream.close();
	instream.open(infile, ios::in);

	if (instream.is_open()) {
		while (getline(instream, line)) {
			// Find the first space separating reference and verse text
			int firstSpacePos = line.find_first_of(' ');
			// Create substring with remaining text after initial space
			string verseText = line.substr(0, firstSpacePos); //could possibly work now

			string strbook = GetNextToken(verseText, ":");
			short book = atoi(strbook.c_str());
			// Get the chapter number
			string strchap = GetNextToken(verseText, ":");
			short chap = atoi(strchap.c_str());
			// Get the verse number
			string strverse = GetNextToken(verseText, " ");
			short verseNum = atoi(strverse.c_str());

			Ref compRef = Ref(book, chap, verseNum, 1);
			if (compRef == ref) {
				tempVerse = Verse(line);
				status = SUCCESS;
				break;
			}
		}
		
	}
	return tempVerse;
}

	// REQUIRED: Return the next verse from the Bible file stream if the file is open.
	// If the file is not open, open the file and return the first verse.
	Verse Bible::nextVerse(LookupResult& status) { 
		
		string line;
		Verse tempVerse;
		
		status = OTHER; // placeholder until retrieval is attempted

		if (!instream.is_open()) {
			cout << "Error, can't open input file: " << infile << endl;
			return tempVerse;
		}
		
		if (getline(instream, line)) {
	
			// Find the first space separating reference and verse text
			int firstSpacePos = line.find_first_of(' ');
			// Create substring with remaining text after initial space
			string verseText = line.substr(0, firstSpacePos); //could possibly work now
			tempVerse = Verse(line);
		}
		else {
			tempVerse = Verse();
		}
		return tempVerse;
	}

	// REQUIRED: Return an error message string to describe status
	string Bible::error(const Ref ref, LookupResult status) {
	
		Ref tempRef = ref;

		if (status == 1) {
			return "Error: No such book " + to_string(tempRef.getBook());
		}
		if (status == 2) {
			return "Error: No such chapter " + to_string(tempRef.getChap()) + " in " + tempRef.getStrBookName();
		}
		if (status == 3) {
			return "Error: No such verse: " + to_string(tempRef.getVerse()) + " in " + tempRef.getStrBookName() + " " + to_string(tempRef.getChap());
		}
		else
			return "Unknown Error has occurred";
	}

	void Bible::display() {
		cout << "Bible file: " << infile << endl;
	}

	// OPTIONAL access functions
	// OPTIONAL: Return the reference after the given ref
	Ref Bible::next(const Ref ref, LookupResult& status) { 
		Ref tempRef = ref;

		short verseNum = tempRef.getVerse() + 1;
		Ref nextRef = Ref(tempRef.getBook(), tempRef.getChap(), verseNum);

		return nextRef;
	}

	// OPTIONAL: Return the reference before the given ref
	Ref Bible::prev(const Ref ref, LookupResult & status) {
		Ref tempRef = ref;
		
		//previous verse num
		short verseNum = tempRef.getVerse() - 1;
		if (verseNum <= 0) 
		{
			// Error no verse exists
			status = NO_VERSE;
		}

		Ref prevRef = Ref(tempRef.getBook(), tempRef.getChap(), verseNum);

		return prevRef;
	}
