#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <vector>

int totalMemorySize = 0;
int pageSizeOption = 0;
int pageFrameSize = 0;
std::string inputFileName = "";
std::ofstream outputFile; // Output file stream

using namespace std;

const int MAX_PROCESS_COUNT = 100;

// Struct for data
struct Process
{
  int id;
  int arrivalTime;
  int memoryLifetime;
  int memoryRequirement;
  int startTime; // Time when the process is allocated in memory
  int pagesRequired;
  bool isCompleted = false; // Track whether the process has finished
};

// Function prototypes
void grabUserInput();
void parseInputFile(const string &inputFileName, Process processArray[], int &processCount);
void manageProcesses(int currentTime, Process processArray[], int processCount, queue<Process> &processQueue);
void manageMemory(int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount, int currentTime, queue<Process> &processQueue);
void manageQueue(int currentTime, queue<Process> &processQueue, int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount);
void printResults(int currentTime, queue<Process> &processQueue, int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount);

// Function definition for grabUserInput
void grabUserInput()
{
  cout << "Enter the total memory size:  " << endl;
  cin >> totalMemorySize;

  if (totalMemorySize <= 0)
  {
    cout << "Error: Total memory size must be greater than 0." << endl;
    return;
  }

  cout << "Enter the page size (either 1, 2, or 3): " << endl;
  cin >> pageSizeOption;

  if (pageSizeOption == 1)
  {
    pageFrameSize = 100;
  }
  else if (pageSizeOption == 2)
  {
    pageFrameSize = 200;
  }
  else if (pageSizeOption == 3)
  {
    pageFrameSize = 400;
  }
  else
  {
    cout << "Error. Invalid input." << endl;
    return;
  }

  // Prompt for input file name
  cout << "Enter the input file name: " << endl;
  cin >> inputFileName;

  // Open the output file based on the page size
  if (pageFrameSize == 100)
  {
    outputFile.open("out1.txt");
  }
  else if (pageFrameSize == 200)
  {
    outputFile.open("out2.txt");
  }
  else if (pageFrameSize == 400)
  {
    outputFile.open("out3.txt");
  }

  if (!outputFile)
  {
    cerr << "Error opening output file!" << endl;
    return;
  }

  cout << "Output file set to: out" << pageFrameSize / 100 << ".txt" << endl;
}

// Parse input file function
void parseInputFile(const string &inputFileName, Process processArray[], int &processCount)
{
  ifstream inFile(inputFileName);

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
    p.id = stoi(line);

    // Arrival time and memory lifetime (second line)
    getline(inFile, line);
    stringstream ss(line);
    ss >> p.arrivalTime >> p.memoryLifetime;

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
    p.pagesRequired = (p.memoryRequirement + pageFrameSize - 1) / pageFrameSize;

    // Store the process data
    processArray[i] = p;

    // Skip the blank line after each process
    getline(inFile, line);
  }

  inFile.close();
}

// Process Management - handles process arrival, completion, queue management
void manageProcesses(int currentTime, Process processArray[], int processCount, queue<Process> &processQueue)
{
  // Process arrival
  for (int i = 0; i < processCount; ++i)
  {
    if (processArray[i].arrivalTime == currentTime && !processArray[i].isCompleted)
    {
      processQueue.push(processArray[i]); // Add process to queue
      outputFile << "t = " << currentTime << ": Process " << processArray[i].id << " arrives" << endl;
    }
  }
}

// Memory Manager - allocate memory to processes
void manageMemory(int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount, int currentTime, queue<Process> &processQueue)
{
  int totalFrames = totalMemorySize / pageFrameSize;

  // Allocate memory for processes that arrived at this time
  while (!processQueue.empty())
  {
    Process currentProcess = processQueue.front();
    processQueue.pop();

    // Check if process's memory requirement exceeds the total memory size
    if (currentProcess.memoryRequirement > totalMemorySize)
    {
      continue; // Skip this process, it can't fit in memory
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
      outputFile << "t = " << currentTime << ": Process " << currentProcess.id << " allocated to memory" << endl;

      for (int i = startFrame; i < startFrame + numPages; ++i)
      {
        memory[i] = currentProcess.id; // Mark memory as occupied
      }

      currentProcess.startTime = currentTime; // Mark start time of the process
      currentProcess.isCompleted = false;     // The process is now running
    }
    else
    {
      // If not enough memory, push it back to the queue
      processQueue.push(currentProcess);
      return;
    }
  }
}

// Queue Management - manage and log queue, memory, events, turnaround time
void manageQueue(int currentTime, queue<Process> &processQueue, int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount)
{
  // Process Completion
  for (int i = 0; i < processCount; ++i)
  {
    if (!processArray[i].isCompleted && processArray[i].startTime + processArray[i].memoryLifetime <= currentTime)
    {
      processArray[i].isCompleted = true; // Mark process as completed
      outputFile << "t = " << currentTime << ": Process " << processArray[i].id << " completes" << endl;

      // Free up memory for the completed process
      int numPages = processArray[i].pagesRequired;
      for (int j = 0; j < totalMemorySize / pageFrameSize; ++j)
      {
        if (memory[j] == processArray[i].id)
        {
          memory[j] = 0; // Free memory
        }
      }
    }
  }
}

// Print Results - responsible for printing the detailed events, including memory map and queue status
void printResults(int currentTime, queue<Process> &processQueue, int memory[], int totalMemorySize, int pageFrameSize, Process processArray[], int processCount)
{
  outputFile << "t = " << currentTime << ": Queue [ ";
  queue<Process> tempQueue = processQueue;
  while (!tempQueue.empty())
  {
    outputFile << tempQueue.front().id << " ";
    tempQueue.pop();
  }
  outputFile << "]" << endl;

  outputFile << "Memory map:" << endl;

  int totalFrames = totalMemorySize / pageFrameSize;
  for (int i = 0; i < totalFrames; i++)
  {
    if (memory[i] == 0) // Free frame
    {
      // If it's a free frame, print the range on a new line
      if (i == 0 || memory[i - 1] != 0) // Start of a free block
      {
        outputFile << i * pageFrameSize << "-" << (i + 1) * pageFrameSize - 1 << ": Free frame(s)" << endl;
      }
      else if (i == totalFrames - 1 || memory[i + 1] != 0) // End of a free block
      {
        outputFile << i * pageFrameSize << "-" << (i + 1) * pageFrameSize - 1 << ": Free frame(s)" << endl;
      }
    }
    else
    {
      // Process frame
      int processID = memory[i];
      int page = (i * pageFrameSize) / pageFrameSize + 1; // Determine which page it belongs to
      outputFile << i * pageFrameSize << "-" << (i + 1) * pageFrameSize - 1 << ": Process " << processID << ", Page " << page << endl;
    }
  }
  outputFile << endl;
}

int main()
{
  char runAgain = 'y';

  // Loop to run the program again if the user chooses to
  while (runAgain == 'y' || runAgain == 'Y')
  {
    Process processArray[MAX_PROCESS_COUNT];
    int processCount = 0;
    queue<Process> processQueue;
    int memory[MAX_PROCESS_COUNT] = {0}; // Memory tracker (by frame number)

    grabUserInput();
    parseInputFile(inputFileName, processArray, processCount);

    int currentTime = 0;
    while (true)
    {
      manageProcesses(currentTime, processArray, processCount, processQueue);
      manageMemory(memory, totalMemorySize, pageFrameSize, processArray, processCount, currentTime, processQueue);
      manageQueue(currentTime, processQueue, memory, totalMemorySize, pageFrameSize, processArray, processCount);
      printResults(currentTime, processQueue, memory, totalMemorySize, pageFrameSize, processArray, processCount);

      bool allProcessesCompleted = true;
      for (int i = 0; i < processCount; ++i)
      {
        if (!processArray[i].isCompleted)
        {
          allProcessesCompleted = false;
          break;
        }
      }

      if (allProcessesCompleted)
      {
        break;
      }

      currentTime++;
    }

    outputFile.close();

    // Ask the user if they would like to run the program again with different parameters
    cout << "Would you like to run the program again with different parameters (y/n)? ";
    cin >> runAgain;

    // If user chooses 'n', exit the loop and end the program
    if (runAgain == 'n' || runAgain == 'N')
    {
      cout << "You have successfully exited the program." << endl;
    }
  }

  return 0;
}
