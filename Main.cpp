// -------------------------------
// Assignment: Memory Management Simulator
// Due date: 12/15/24
// Purpose: This program involves creating a simulation to analyze the impact of limited memory and memory management policies by simulating process execution, generating event traces, and calculating average turnaround time.
// -------------------------------

// -------------------------------
// Assignment: Memory Management Simulator
// Due date: 12/15/24
// Purpose: This program simulates memory management for process execution and calculates average turnaround time.
// -------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>

int memorySize = 0;
int pageOption = 0;
int pageSize = 0;
std::string inputFile = "";
std::ofstream outFile; // Output file stream

using namespace std;

const int MAX_PROCESSES = 100;

// Struct for data
struct Process
{
	int processID;
	int arrivalTime;
	int lifetimeInMemory;
	int memoryRequirement;
	int startTime; // Time when the process is allocated in memory
	int pagesRequired;
	bool completed = false; // Track whether the process has finished
};

// Function prototypes
void grabUserInput();
void parseInputFile(const string &inputFile, Process processes[], int &processCount);
void processManagement(int currentTime, Process processes[], int processCount, queue<Process> &inputQueue);
void memoryManager(int memory[], int memorySize, int pageSize, Process processes[], int processCount, int currentTime, queue<Process> &inputQueue);
void queueManagement(int currentTime, queue<Process> &inputQueue, int memory[], int memorySize, int pageSize, Process processes[], int processCount);
void printResults(int currentTime, queue<Process> &inputQueue, int memory[], int memorySize, int pageSize, Process processes[], int processCount);

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

	// Open the output file based on the page size
	if (pageSize == 100)
	{
		outFile.open("out1.txt");
	}
	else if (pageSize == 200)
	{
		outFile.open("out2.txt");
	}
	else if (pageSize == 400)
	{
		outFile.open("out3.txt");
	}

	if (!outFile)
	{
		cerr << "Error opening output file!" << endl;
		return;
	}

	cout << "Output file set to: out" << pageSize / 100 << ".txt" << endl;
}

// Parse input file function
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

		// Calculate pages required for this process
		p.pagesRequired = (p.memoryRequirement + pageSize - 1) / pageSize;

		// Store the process data
		processes[i] = p;

		// Skip the blank line after each process
		getline(inFile, line);
	}

	inFile.close();
}

// Process Management - handles process arrival, completion, queue management
void processManagement(int currentTime, Process processes[], int processCount, queue<Process> &inputQueue)
{
	// Process arrival
	for (int i = 0; i < processCount; ++i)
	{
		if (processes[i].arrivalTime == currentTime)
		{
			inputQueue.push(processes[i]); // Add process to queue
			outFile << "t = " << currentTime << ": Process " << processes[i].processID << " arrives" << endl;
		}
	}
}

// Memory Manager - allocate memory to processes
void memoryManager(int memory[], int memorySize, int pageSize, Process processes[], int processCount, int currentTime, queue<Process> &inputQueue)
{
	int totalFrames = memorySize / pageSize;

	// Allocate memory for processes that arrived at this time
	while (!inputQueue.empty())
	{
		Process currentProcess = inputQueue.front();
		inputQueue.pop();

		// Check if process's memory requirement exceeds the total memory size
		if (currentProcess.memoryRequirement > memorySize)
		{
			outFile << "Process " << currentProcess.processID << " is too large to fit in memory and will be ignored." << endl;
			continue; // Skip this process
		}

		// Proceed with memory allocation if the process fits
		int numPages = currentProcess.pagesRequired;

		int freeFrames = 0;
		int startFrame = -1;

		for (int i = 0; i < totalFrames; ++i)
		{
			if (memory[i] == 0) // Found free frame
			{
				if (startFrame == -1)
					startFrame = i;
				freeFrames++;
			}
			else
			{
				startFrame = -1;
				freeFrames = 0;
			}

			if (freeFrames >= numPages)
			{
				break;
			}
		}

		if (freeFrames >= numPages)
		{
			// Allocate memory
			outFile << "MM moves Process " << currentProcess.processID << " to memory" << endl;

			for (int i = startFrame; i < startFrame + numPages; ++i)
			{
				memory[i] = currentProcess.processID; // Mark memory as occupied
			}

			currentProcess.startTime = currentTime; // Mark start time of the process
			outFile << "Process " << currentProcess.processID << " allocated at time " << currentTime << endl;
		}
		else
		{
			outFile << "Not enough memory to allocate Process " << currentProcess.processID << endl;
		}
	}
}

// Queue Management - manage and log queue, memory, events, turnaround time
void queueManagement(int currentTime, queue<Process> &inputQueue, int memory[], int memorySize, int pageSize, Process processes[], int processCount)
{
	printResults(currentTime, inputQueue, memory, memorySize, pageSize, processes, processCount);
}

// Print Results - responsible for printing the detailed events
void printResults(int currentTime, queue<Process> &inputQueue, int memory[], int memorySize, int pageSize, Process processes[], int processCount)
{
	int totalFrames = memorySize / pageSize;

	// Log the time and input queue status
	outFile << "t = " << currentTime << ": ";

	// If a process arrives, log that event
	bool processArrived = false;
	queue<Process> tempQueue = inputQueue; // Copy the queue to display
	while (!tempQueue.empty() && processes[tempQueue.front().processID - 1].arrivalTime == currentTime)
	{
		Process p = tempQueue.front();
		tempQueue.pop();
		outFile << "Process " << p.processID << " arrives" << endl;
		processArrived = true;
	}

	// If any process arrives, print input queue
	if (processArrived)
	{
		outFile << "Input Queue: [";
		while (!inputQueue.empty())
		{
			Process p = inputQueue.front();
			inputQueue.pop();
			outFile << p.processID;
			if (!inputQueue.empty())
				outFile << " ";
		}
		outFile << "]" << endl;
	}

	// Log memory movements (i.e., when a process is moved to memory)
	for (int i = 0; i < processCount; ++i)
	{
		if (processes[i].startTime == currentTime) // If the process is moved to memory
		{
			outFile << "MM moves Process " << processes[i].processID << " to memory" << endl;

			// Allocate memory for the process
			int numPages = processes[i].pagesRequired;
			outFile << "Memory Map:" << endl;

			// Print memory map
			for (int j = 0; j < totalFrames; ++j)
			{
				if (memory[j] == 0) // Free frame
				{
					outFile << j * pageSize << "-" << (j + 1) * pageSize - 1 << ": Free frame(s)" << endl;
				}
				else
				{
					outFile << j * pageSize << "-" << (j + 1) * pageSize - 1 << ": Process " << processes[i].processID
									<< ", Page " << (j + 1) << endl;
				}
			}
		}
	}

	// Print the final memory status at this time
	outFile << "Memory Map: " << endl;
	for (int i = 0; i < totalFrames; ++i)
	{
		if (memory[i] == 0)
		{
			outFile << i * pageSize << "-" << (i + 1) * pageSize - 1 << ": Free frame(s)" << endl;
		}
		else
		{
			outFile << i * pageSize << "-" << (i + 1) * pageSize - 1 << ": Process " << memory[i] << endl;
		}
	}
}

int main()
{
	grabUserInput();
	Process processes[MAX_PROCESSES];
	int processCount = 0;
	parseInputFile(inputFile, processes, processCount);

	queue<Process> inputQueue;
	int memory[memorySize / pageSize] = {0}; // Memory map, initialized to zero (all free)

	// Run until all processes have completed
	while (true)
	{
		bool allProcessesCompleted = true;

		// Check if any process is still not completed
		for (int i = 0; i < processCount; ++i)
		{
			if (!processes[i].completed)
			{
				allProcessesCompleted = false;
				break;
			}
		}

		if (allProcessesCompleted)
			break;

		for (int currentTime = 0; currentTime < 1000; ++currentTime)
		{
			processManagement(currentTime, processes, processCount, inputQueue);
			memoryManager(memory, memorySize, pageSize, processes, processCount, currentTime, inputQueue);
			queueManagement(currentTime, inputQueue, memory, memorySize, pageSize, processes, processCount);
		}
	}
	outFile.close();
	return 0;
}
