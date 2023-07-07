/*
* Program used to find duplicate lines in a large text file. See README.md for
* more details.
* @author Christos Cunning
*/

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

/*
* Merge sorted temporary files together into sortedFile.
*
* @param numChunks Number of chunks that input file has been split into.
* @param totalLinesRead Total number of lines read from input file.
* @param tempFilePathBase The base name of the temporary files (ex: "temp_file_")
* @param sortedFile File stream handle for the file that the sorted data will be written to.
*/
void externalMergeSort(const int numChunks, const int totalLinesRead, const string tempFilePathBase, fstream & sortedFile) {
	// store next smallest string from each chunk
	string* next = new string[numChunks];

	// open every temporary file
	ifstream* tempFiles = new ifstream[numChunks];

	for (int i = 0; i < numChunks; i++) {
		string tempFilePath = tempFilePathBase + to_string(i);
		tempFiles[i].open(tempFilePath, ifstream::in);

		if (!tempFiles[i].is_open()) {
			cout << "Error: failed to open temp file: '" << tempFilePath << "'\n";
			//return;
		}

		// get first string from each chunk
		if (!getline(tempFiles[i], next[i])) {
			// no more lines to read from a given temp file
			next[i] = "";
		}
	}

	// find smallest line from temp files and add it to sorted file
	string smallest;
	int smallestChunkIndex;

	for (int i = 0; i < totalLinesRead; i++) {
		smallest = "";
        // Loop through every chunk to find the smallest line
		for (int j = 0; j < numChunks; j++) {
			if (next[j] != "" && (smallest == "" || next[j] < smallest)) {
				smallest = next[j];
				smallestChunkIndex = j;
			}
		}
		// write smallest to original sorted file
		sortedFile << smallest << "\n";

		// get next string from chunk we took smallest string from
		if (!getline(tempFiles[smallestChunkIndex], next[smallestChunkIndex])) {
			next[smallestChunkIndex] = "";
		}
	}

	// close temp files
	for (int i = 0; i < numChunks; i++) {
		tempFiles[i].close();
        // delete temp files
        string tempFilePath = tempFilePathBase + to_string(i);
        if (remove(tempFilePath.c_str()) != 0) {
            cout << "Error: failed to delete file '" << tempFilePath << "'\n";
        }
	}
    
	// free allocated memory
	delete[] next;
	delete[] tempFiles;
}

/*
* Performs algorithm to find duplicates lines in a random text file.
*
*
* @param inputFilePath The file that will be used for input.
* @param sortedFilePath The sorted version of the input file will be written to this file.
* @param outputFilePath The duplicate lines found in the input file will be written to this file.
* @param MAX_CHUNK_SIZE The maximum amount of memory that each chunk will take up. Used to limit overall memory consumption of the algorithm.
*/
void findDuplicates(const string inputFilePath, const string sortedFilePath, const string outputFilePath, const int MAX_CHUNK_SIZE) {
	const int MAX_CHUNK_LENGTH = MAX_CHUNK_SIZE / sizeof(string); // even if all strings are empty, this would be the maximum size of array needed

	const string tempFilePathBase = "temp_chunk_";

	// open input file for reading
	fstream inputFile(inputFilePath);
	if (!inputFile.is_open()) {
		cout << "Error: Failed to open input file: '" << inputFilePath << "'\n";
		return;
	}
	

	// split large input file into smaller chunks that fit into MAX_CHUNK_SIZE
	vector<string> chunk;

	int numChunks = 0; // Number of chunks that input file is split into
	int totalLinesRead = 0; // Total number of lines read from input file
	

	// sort each chunk of the total file and write the sorted chunks to temporary files
	while (inputFile.peek() != EOF) {
		int chunkLinesRead = 0; // Number of lines read for current chunk
        int currentChunkSize = 0; // Size of current chunk

		// ensure that we stop reading lines if we reach end of file (no more lines to read)
		for (int i = 0; i < MAX_CHUNK_LENGTH && currentChunkSize < MAX_CHUNK_SIZE && inputFile.peek() != EOF; i++) {

			// Read a line from input file
			string line;

			if (!getline(inputFile, line)) {
				cout << "Error: Failed to read line\n";
			}

			// keep track of how many bytes read for each line
			int lineSize = sizeof(string) + line.size();
			//cout << sizeof(vector<char>) << ", " << sizeof(string) << ", " << line.size() << endl;
			currentChunkSize += lineSize;

			chunk.push_back(line);

			chunkLinesRead++;
		}

		// sort chunk
		sort(chunk.begin(), chunk.end());

		// open temporary file in write mode
		string tempFilePath = tempFilePathBase + to_string(numChunks);
		//cout << tempFilePath << "\n";
		ofstream tempFile(tempFilePath);
		if (!tempFile.is_open()) {
			cout << "Error: Failed to open temp file '" << tempFilePath << "'\n";
			return;
		}

		// write lines to file
		for (int i = 0; i < chunkLinesRead; i++) {
			tempFile << chunk[i] << "\n";
		}

		// close temp file
		tempFile.close();

		// clear vector
		chunk.clear();

		// update number of chunks
		numChunks++;

		// update total
		totalLinesRead += chunkLinesRead;
	}

	// open sorted file
	fstream sortedFile(sortedFilePath, fstream::in | fstream::out | fstream::trunc);
	if (!sortedFile.is_open()) {
		cout << "Error: failed to open sorted file: '" << sortedFilePath << "'\n";
		return;
	}

	// External merge sort
	externalMergeSort(numChunks, totalLinesRead, tempFilePathBase, sortedFile);

	// now that inputFile is sorted in sortedFile, can iterate through it to find any duplicate lines (that will now appear sequentially)
	int numDupes = 0;
	
	// write duplicate lines to outputFile
	ofstream outputFile(outputFilePath);
	if (!outputFile.is_open()) {
		cout << "Error: failed to open output file\n";
		return;
	}

	// seek sorted file back to the beginning
	sortedFile.seekg(0, sortedFile.beg);

	string current;
	string last = "";
	for (int i = 0; i < totalLinesRead; i++) {
		if (!getline(sortedFile, current)) {
			cout << "Error: Failed to read line from sorted file.\n";
			return;
		}
        // Check if current line matches last line (ie: duplicate)
		if (current == last) {
			numDupes++;
			outputFile << current << "\n";
		}

		last = current;
	}

	cout << "Number of duplicate lines: " << numDupes << "\n";

	// close sorted file
	sortedFile.close();

	// close output file
	outputFile.close();

	// close input file
	inputFile.close();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        cout << "Run program like: './FindDupes NUM_LINES MAX_CHUNK_SIZE'" << endl;
        return 0;
    }
    
    char* endp;
    int NUM_LINES = strtol(argv[1], &endp, 0);
    if (endp == argv[1] || *endp) {
        cout << "Invalid value for 'NUM_LINES' argument. Must be an integer." << endl;
    }
    
    int MAX_CHUNK_SIZE = strtol(argv[2], &endp, 0);
    if (endp == argv[2] || *endp) {
        cout << "Invalid value for 'MAX_CHUNK_SIZE' argument. Must be an integer." << endl;
    }

    string filePathBase = "./testing/";

    // Paths to input, sorted, and output files
    string inputFilePath = filePathBase + "INPUT_test_data_" + to_string(NUM_LINES) + ".txt";
    string sortedFilePath = filePathBase + "SORTED_test_data_" + to_string(NUM_LINES) + ".txt";
    string outputFilePath = filePathBase + "OUTPUT_test_data_" + to_string(NUM_LINES) + ".txt";

    // Run algorithm
    findDuplicates(inputFilePath, sortedFilePath, outputFilePath, MAX_CHUNK_SIZE);

    
    cout << "\n";
    return 0;
}