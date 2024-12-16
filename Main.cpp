// -------------------------------
// Assignment: Memory Management Simulator
// Due date: 12/15/24
// Purpose: This program involves creating a simulation to analyze the impact of limited memory and memory management policies by simulating process execution, generating event traces, and calculating average turnaround time.
// -------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int memorySize = 0;
int pageOption = 0;
int pageSize = 0;
std::string inputFile = "";
std::string outputFile = "";

using namespace std;

const int MAX_PROCESSES = 100; // Maximum number of processes

// Structure to hold process data
struct Process
{
	int processID;
	int arrivalTime;
	int lifetimeInMemory;
	int memoryRequirement;
};

// Function prototypes
void grabUserInput();
void parseInputFile(const string &inputFile, Process processes[], int &processCount);

// Function definition for grabUserInput
void grabUserInput()
{
	cout << "Enter the memory size:  " << endl;
	cin >> memorySize;

	if (memorySize <= 0)
	{
		cout << "Error: Memory size must be greater than 0." << endl;
		return;
	}

	cout << "Enter the page size (either 1, 2, or 3): " << endl;
	cin >> pageOption;

	// Depending on the value of pageOption, determine the memory size of each page
	if (pageOption == 1)
	{
		pageSize = 100;
	}
	else if (pageOption == 2)
	{
		pageSize = 200;
	}
	else if (pageOption == 3)
	{
		pageSize = 400;
	}
	else
	{
		cout << "Error. Invalid input." << endl;
		return;
	}

	// Prompt for input file name
	cout << "Enter the input file name: " << endl;
	cin >> inputFile;

	// Generate the output file name based on the page size
	if (pageSize == 100)
	{
		outputFile = "Out1";
	}
	else if (pageSize == 200)
	{
		outputFile = "Out2";
	}
	else if (pageSize == 400)
	{
		outputFile = "Out3";
	}

	cout << "Output file set to: " << outputFile << endl;
}

// Function definition for parseInputFile
void parseInputFile(const string &inputFile, Process processes[], int &processCount)
{
	ifstream inFile(inputFile);

	if (!inFile)
	{
		cerr << "Error opening input file!" << endl;
		return;
	}

	string line;

	// Read the number of processes
	getline(inFile, line);
	processCount = stoi(line);

	// Skip the blank line after the number of processes
	getline(inFile, line);

	// Loop through each process
	for (int i = 0; i < processCount; ++i)
	{
		Process p;

		// Process ID (first line of process details)
		getline(inFile, line);
		p.processID = stoi(line);

		// Arrival time and lifetime in memory (second line)
		getline(inFile, line);
		stringstream ss(line);
		ss >> p.arrivalTime >> p.lifetimeInMemory;

		// Memory requirement (address space line)
		getline(inFile, line);
		stringstream memoryStream(line);
		int memoryPiece, totalMemory = 0;

		while (memoryStream >> memoryPiece)
		{
			totalMemory += memoryPiece; // Sum the memory pieces
		}
		p.memoryRequirement = totalMemory;

		// Store the process data
		processes[i] = p;

		// Skip the blank line after each process
		getline(inFile, line);
	}

	inFile.close();
}

// Function to print out the parsed processes for verification
void printProcesses(const Process processes[], int processCount)
{
	for (int i = 0; i < processCount; ++i)
	{
		cout << "Process ID: " << processes[i].processID
				 << ", Arrival Time: " << processes[i].arrivalTime
				 << ", Lifetime in Memory: " << processes[i].lifetimeInMemory
				 << ", Memory Requirement: " << processes[i].memoryRequirement << endl;
	}
}


int main()
{
	Process processes[MAX_PROCESSES]; // Array to hold processes
	int processCount = 0;							// Counter to track number of processes

	// Call the function to grab user input
	grabUserInput();

	// Parse the input file
	parseInputFile(inputFile, processes, processCount);

	// Print the processes for verification
	printProcesses(processes, processCount);

	system("pause");
	return 0;
}
