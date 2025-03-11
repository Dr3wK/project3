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
#include <map>
using namespace std;

Bible::Bible() { 
	// Default constructor
	infile = "/home/class/csc3004/Bibles/web-complete";

	// Build map index
	int indexResult = buildVerseIndex("/home/class/csc3004/Bibles/web-complete");
	if (indexResult == 0) {
		cerr << "Error building index." << endl;
	}
}

// Constructor – pass bible filename
Bible::Bible(const string s) { 
	infile = s; 

	// Build map index
	int indexResult = buildVerseIndex("/home/class/csc3004/Bibles/web-complete");
	if (indexResult == 0) {
		cerr << "Error building index." << endl;
	}
}

// REQUIRED: Return the next verse from the Bible file stream if the file is open.
// If the file is not open, open the file and return the first verse.
Verse Bible::nextVerse(LookupResult& status) { 
	
	status = OTHER; // Default status
	if (!instream.is_open()) {
		instream.open(infile, ios::in);
		if (!instream.is_open()) {
			cerr << "Error: Could not open file " << infile << endl;
			status = OTHER;
			return Verse();
		}
	}
	string line;
	if (getline(instream, line)) {
		status = SUCCESS;
		return Verse(line);
	}
	// Default output if end of file and file not open
	status = OTHER;
	return Verse();
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



// OPTIONAL access functions
// OPTIONAL: Return the reference after the given ref
Ref Bible::next(const Ref ref, LookupResult& status) { 
	  
	// Check if the reference exists in the index
	if (BibleRefs.find(ref) == BibleRefs.end()) {
		status = NO_VERSE; // Reference not found in the index
		return Ref();
	}

	// find the reference in the index
	// find what type of iterator
	auto iter = BibleRefs.find(ref);
	if (iter == BibleRefs.end()) {
		cerr << "Error: Reference not found in BibleRefs." << endl;
		status = NO_VERSE;
		return Ref();
	}

	// Move to the next reference in the index
	++iter;
	if (iter == BibleRefs.end()) {
		cerr << "Error: No next verse exists for the provided reference." << endl;
		status = NO_VERSE;
		return Ref();
	}

	// Return the next reference
	status = SUCCESS;
	return iter->first;
}

	// OPTIONAL: Return the reference before the given ref
Ref Bible::prev(const Ref ref, LookupResult &status) {

    // Check to see if first verse
    if (ref.getBook() == 1 && ref.getChap() == 1 && ref.getVerse() == 1) {
        status = NO_VERSE;  // No previous verse exists
        return Ref();
    }

    if (ref.getVerse() > 1) { // Previous verse in the same chapter
        
        return Ref(ref.getBook(), ref.getChap(), ref.getVerse() - 1);
    } 
	else if (ref.getChap() > 1) { // Last verse of the previous chapter
        
        Ref lastVerseInPrevChap(ref.getBook(), ref.getChap() - 1, MAX_VERSE);
        if (BibleRefs.find(lastVerseInPrevChap) != BibleRefs.end()) {
            return lastVerseInPrevChap;
        }

    } 
	else if (ref.getBook() > 1) { // Last chapter and verse of the previous book
        
        Ref lastVerseInPrevBook(ref.getBook() - 1, MAX_CHAP, MAX_VERSE);
        if (BibleRefs.find(lastVerseInPrevBook) != BibleRefs.end()) {
            return lastVerseInPrevBook;
        }
    }

    status = NO_VERSE;
    return Ref();
}

// REQUIRED: lookup finds a given verse in this Bible
Verse Bible::lookup(Ref ref, LookupResult& status) {
	status = OTHER;  // Placeholder status

	// Check if the reference is in the index
	if (BibleRefs.find(ref) == BibleRefs.end()) {
		status = NO_VERSE;  // Reference not found
		return Verse();
	}

	// If instream not yet open, open it
	if (!instream.is_open()) {
		instream.open(infile, ios::in);
		if (!instream.is_open()) {
			cerr << "Error: Cannot open input file: " << infile << endl;
			status = OTHER;
			return Verse();
		}
	}

	// Seek to the correct offset
	instream.clear();
	int offset = BibleRefs[ref];
	instream.seekg(offset);
	cout << "Pointer after seekg for Ref: " << instream.tellg() << endl;

	// Read the line
	string line;
	if (getline(instream, line)) {
		cout << "Pointer after getline: " << instream.tellg() << endl;
		status = SUCCESS;
		return Verse(line);
	}

	// Handle read failure
	cerr << "Error: Could not retrieve verse at offset " << offset << endl;
	status = OTHER;
	return Verse();
}

int Bible::buildVerseIndex(string fileName) {
	ifstream infile;
	int position;
	string line;

	// Open the file
	infile.open(fileName.c_str(), ios::in);
	if (!infile) {
		cerr << "Error: Could not open file " << fileName << endl;
		return 0;
	}

	while (true) {
		position = infile.tellg();  // Capture the position BEFORE reading the line
		if (!getline(infile, line)) break;  // Read the line check for end of file
		Ref ref(line);  // Create a reference from the line
		BibleRefs[ref] = position;  // Store the starting position of the verse
		lastOffset = position;  // Update the last offset
	}

	infile.close();
	return 1;
}


// Methods for diagnostic data
// Get the number of references in the index
int Bible::getIndexSize() const {
	return BibleRefs.size();
}

// Get the byte offset of the last verse added to the index
int Bible::getLastOffset() const {
	return lastOffset;
}

// Get the byte offset for a specific reference
int Bible::getOffset(const Ref& ref) {
	if (BibleRefs.find(ref) != BibleRefs.end()) {
		return BibleRefs[ref];
	}
	return -1; // Return -1 if the reference is not found
}

void Bible::display() {
	cout << "Bible file: " << infile << endl;
}

