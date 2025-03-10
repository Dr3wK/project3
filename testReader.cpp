#include "Ref.h"
#include "Verse.h"
#include "Bible.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char* argv[]) {
    // Create Bible object to process the raw text file
    Bible webBible("/home/class/csc3004/Bibles/web-complete");

    // Build the index
    int indexResult = webBible.buildVerseIndex("/home/class/csc3004/Bibles/web-complete");
    if (indexResult == 0) {
        cerr << "Error building index." << endl;
        return 1;
    }

    // Diagnostic output for program demonstration
    cout << "Index built successfully." << endl;
    cout << "Number of references in index: " << webBible.getIndexSize() << endl;
    cout << "Byte offset of the last verse added to the index: " << webBible.getLastOffset() << endl;

    // Print the byte offset of specific references
    Ref ref1(1, 1, 1);  // Genesis 1:1
    Ref ref2(1, 1, 2);  // Genesis 1:2
    Ref ref3(1, 1, 3);  // Genesis 1:3

    cout << "Byte offset for Genesis 1:1: " << webBible.getOffset(ref1) << endl;
    cout << "Byte offset for Genesis 1:2: " << webBible.getOffset(ref2) << endl;
    cout << "Byte offset for Genesis 1:3: " << webBible.getOffset(ref3) << endl;

    // Check command line arguments
    if (argc > 5) {
        cerr << "Error: too many arguments" << endl;
        exit(0);
    }
    if (argc < 4) {
        cerr << "Error: verse number missing" << endl;
        exit(0);
    }

    int bookNum = atoi(argv[1]);
    int chapterNum = atoi(argv[2]);
    int startingVerseNum = atoi(argv[3]);
    int amountVerses = 1;
    if (argc > 4) {
        amountVerses = atoi(argv[4]);
    }

    // Create a reference from the numbers
    Ref ref(bookNum, chapterNum, startingVerseNum, amountVerses);

    // Use the Bible object to retrieve the verse by reference
    LookupResult result;
    Verse verse = webBible.lookup(ref, result);

    if (result == 0) {
        cout << ref.getStrBookName() << " " << ref.getChap() << endl;

        // Display the verse if the verse was successfully found.
        verse.display();
    }

    if (amountVerses > 1 && result == 0) {
        for (int i = 0; i < amountVerses - 1; i++) {
            // Set verse object to the next verse
            verse = webBible.nextVerse(result);

            // Check to see if it is a new chapter, if it is print the new reference
            Ref tempRef = verse.getRef();
            if (tempRef.getVerse() == 1) {
                cout << tempRef.getStrBookName() << " " << tempRef.getChap() << endl;
            }
            verse.display();
        }
    }
    cout << endl;

    return 0;
}