// -------------------------------
// Assignment: Memory Management
// Due date: 12/15/24
// Purpose: This project involves creating a simulation to analyze the impact of limited memory and memory management policies by simulating process execution, generating event traces, and calculating average turnaround time.
// -------------------------------

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <queue>

using namespace std;

// Global Variables
int totalMemorySize = 0;
int pageSizeOption = 0;
int pageFrameSize = 0;
string inputFileName = "";
ofstream outputFile;

const int MAX_PROCESS_COUNT = 100;
int processCount = 0;
int currentTime = 0;
int memory[MAX_PROCESS_COUNT] = {0};
queue<int> processQueue;

// Process information as seen in input file
int processID[MAX_PROCESS_COUNT];
int processArrivalTime[MAX_PROCESS_COUNT];
int processLifetime[MAX_PROCESS_COUNT];
int processMemoryRequirement[MAX_PROCESS_COUNT];
int processPagesRequired[MAX_PROCESS_COUNT];
int processStartTime[MAX_PROCESS_COUNT];
bool processCompleted[MAX_PROCESS_COUNT] = {false};

// Function declarations
void grabUserInput();
void parseInputFile(const string &inputFileName);
void manageProcesses(int currentTime);
void manageMemory();
void manageQueue();
void printResults();

// Function to get necessary memory information from user
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

  cout << "Enter the input file name: " << endl;
  cin >> inputFileName;

  // Determine which output file to write to depending on pageFrameSize
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

// Function to parse our provided input file
void parseInputFile(const string &inputFileName)
{
  ifstream inFile(inputFileName);

  if (!inFile)
  {
    cerr << "Error opening input file!" << endl;
    return;
  }

  string line;
  getline(inFile, line);

  // READ: # of Processes
  processCount = stoi(line);

  for (int i = 0; i < processCount; ++i)
  {
    // READ: Process ID (Line 1)
    getline(inFile, line);
    processID[i] = stoi(line);

    // READ: Arrival time and lifetime in memory (Line 2)
    getline(inFile, line);
    stringstream ss(line);
    ss >> processArrivalTime[i] >> processLifetime[i];

    // READ: Address Space (Line 3)
    getline(inFile, line);
    stringstream memoryStream(line);
    int memoryPiece, totalMemory = 0;

    // Parse memory chunks from input (memoryStream) and calcualte memory requirement for each process.
    while (memoryStream >> memoryPiece)
    {
      totalMemory += memoryPiece;
    }
    processMemoryRequirement[i] = totalMemory;

    // In order to calculate page requirements, we would need to divided our memoryRequirement / Page Size.
    processPagesRequired[i] = (processMemoryRequirement[i] + pageFrameSize - 1) / pageFrameSize;

    getline(inFile, line);
  }

  inFile.close();
}

// Function for managing the processes themselves, such as when it arrives and completion time
void manageProcesses(int currentTime)
{
  int i = 0;
  // Iterate through all processes to confirm if any processes arrive. If a process has arrived and has not been completed, then add a process to the queue.
  while (i < processCount)
  {
    if (processArrivalTime[i] == currentTime && !processCompleted[i])
    {
      processQueue.push(i);
      outputFile << "t = " << currentTime << ": Process " << processID[i] << " arrives" << endl;
    }
    i++;
  }
}

// Function to manage memory, such as processing memory allocation for each process.
void manageMemory()
{
  // Calculate the total # of frames available in memory
  int totalFrames = totalMemorySize / pageFrameSize;
  // Assume memory has not been allocated
  bool memoryAllocated = false;

  // Main loop for processing each process within processQueue. This will only run when the processQueue is not empty or when there is memory allocated for a process
  while (!processQueue.empty() && !memoryAllocated)
  {
    int processIndex = processQueue.front();
    processQueue.pop();

    // Included logic to skip over a process only if a process requires more memory than available
    if (processMemoryRequirement[processIndex] > totalMemorySize)
    {
      continue;
    }

    // Retrieve the # of pages the process requires
    int numPages = processPagesRequired[processIndex];
    int freeFrames = 0;
    int startFrame = -1;

    // A for loop that iterates over all memory frames. Will check if each memory frame is free.
    for (int i = 0; i < totalFrames; ++i)
    {
      // Frame is free
      if (memory[i] == 0)
      {
        if (startFrame == -1)
        {
          startFrame = i;
        }
        freeFrames++;
      }
      // If frame is not free
      else
      {
        startFrame = -1;
        freeFrames = 0;
      }

      // Condition checks if freeFrames is greater than numPages. If this is true, then free memory has been found for the process, and there is sufficient memory to allocate.
      if (freeFrames >= numPages)
      {
        memoryAllocated = true;
        break;
      }
    }

    // Condition checks if memory has been allocated for a process. If true, then we can begin to allocate this memory
    if (memoryAllocated)
    {
      // Logs data to output file. Displays the time, Process ID, and allocation message
      outputFile << "t = " << currentTime << ": Process " << processID[processIndex] << " allocated to memory" << endl;

      // Iterates through memory frames. Assign a memory frame to each processID.
      for (int i = startFrame; i < startFrame + numPages; ++i)
      {
        memory[i] = processID[processIndex];
      }

      // Assign a procesStartTime based on when the process was allocated memory.
      processStartTime[processIndex] = currentTime;
    }
    // Condition checks if there was not enough free memory frames to allocate. Will continue to attempt to allocate memory.
    else
    {
      processQueue.push(processIndex);
      return;
    }
  }
}

// Function to manage our queue, more specifically to manage the completion of processes.
void manageQueue()
{
  int totalFrames = totalMemorySize / pageFrameSize;
  int processIndex = 0;
  bool allProcessesCompleted = true;

  // Loop through all processes
  while (processIndex < processCount)
  {
    // Check if current process has not been completed and if the process is old (lifetime exceeded)
    if (!processCompleted[processIndex] && processStartTime[processIndex] + processLifetime[processIndex] <= currentTime)
    {
      processCompleted[processIndex] = true;
      // Log process completion
      outputFile << "t = " << currentTime << ": Process " << processID[processIndex] << " completes" << endl;

      // Deallocate memory for the completed process
      int numPages = processPagesRequired[processIndex];
      int freedFrames = 0;
      int frameIndex = 0;

      // Iterate over memory frames to see if each frame belogs to a completed process
      while (frameIndex < totalFrames && freedFrames < numPages)
      {
        if (memory[frameIndex] == processID[processIndex])
        {
          memory[frameIndex] = 0;
          freedFrames++;
        }
        frameIndex++;
      }
    }

    // If a process has not yet been completed, then set the allProcessesCompleted value to false as not all processes have been completed yet
    if (!processCompleted[processIndex])
    {
      allProcessesCompleted = false;
    }

    processIndex++;

    // If all processes are marked as complete and if there are no more processes left, then exit the loop.
    if (allProcessesCompleted && processIndex >= processCount)
    {
      break;
    }
  }
}

void printResults()
{
  outputFile << "t = " << currentTime << ": Queue [ ";

  // Temporary queue creation so that we can still use the queue while we iterate
  queue<int> tempQueue = processQueue;

  // Loops over the temporary queue and outputs processID to the file.
  while (!tempQueue.empty())
  {
    outputFile << processID[tempQueue.front()] << " ";
    tempQueue.pop();
  }
  outputFile << "]" << endl;

  // Print the memory map.
  outputFile << "Memory map:" << endl;

  // Calculate the total # of memory frames
  int totalFrames = totalMemorySize / pageFrameSize;

  for (int i = 0; i < totalFrames; ++i)
  {
    // Condition determines if a frame is free. If it is free, then print the memory frames that are free.
    if (memory[i] == 0)
    {
      if (i == 0 || memory[i - 1] != 0)
      {
        outputFile << i * pageFrameSize << "-" << (i + 1) * pageFrameSize - 1 << ": Free frame(s)" << endl;
      }
    }

    // Condition determines if a frame is occupied. If it is occupied, then print a range of memory frames and determine the process occupying such frame.
    else
    {
      outputFile << i * pageFrameSize << "-" << (i + 1) * pageFrameSize - 1 << ": Process " << memory[i] << endl;
    }
  }
  outputFile << endl;
}

int main()
{
  char runAgain = 'y';
  // Main loop, does user want to continue running the program while changing paramaters?
  while (runAgain == 'y' || runAgain == 'Y')
  {
    grabUserInput();
    parseInputFile(inputFileName);

    // Virtual clock
    currentTime = 0;
    while (true)
    {
      manageProcesses(currentTime);
      manageMemory();
      manageQueue();
      printResults();

      // Check if all processes have been completed or incompleted.
      bool allProcessesCompleted = true;
      for (int i = 0; i < processCount; ++i)
      {
        if (!processCompleted[i])
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

    cout << "Would you like to run the program again with different parameters (y/n)? ";
    cin >> runAgain;

    if (runAgain == 'n' || runAgain == 'N')
    {
      cout << "You have successfully exited the program." << endl;
    }
  }
  return 0;
}
