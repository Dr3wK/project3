// Verse class function definitions
// Computer Science, MVNU
// Additions by Drew Koning

#include "Verse.h"
#include <iostream>
using namespace std;

Verse::Verse() {  	// Default constructor
    verseText = "Uninitialized Verse!";
    verseRef = Ref();
} 

// REQUIRED: Parse constructor - pass verse string from file
Verse::Verse(const string s) {
	// use Ref constructor to create verseRef
	// store the remainder of the string in verseText
	verseRef = Ref(s);
	
	// Find the first space seperating reference and verse text
	int firstSpacePos = s.find_first_of(' ');
	// Create substring with remaining text after intial space
	verseText = s.substr(firstSpacePos + 1);
	
}  	

// REQUIRED: Accessors
string Verse::getVerse() {
	return verseText;
}

Ref Verse::getRef() {
	return verseRef;
}

// display reference and verse
void Verse::display() {
    cout << verseRef.getVerse() << " " << verseText << endl;
 }

