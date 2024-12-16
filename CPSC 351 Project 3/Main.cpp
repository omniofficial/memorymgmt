#include <iostream>
#include <string>

int inputMemSize = 0;
int inputPageSize = 0;
int pageSize = 0;
std::string inputFile = "";
std::string outputFile = "";


using namespace std;

int main()
{
	cout << "Enter the memory size:  " << endl;
	cin >> inputMemSize;

	cout << "Enter the page size (either 1, 2, or 3): " << endl;
	cin >> inputPageSize;

	// Depending on the value of inputPageSize, we will determine the memory size of each page.
	if (inputPageSize == 1)
	{
		pageSize = 100;
	}
	else if (inputPageSize == 2)
	{
		pageSize = 200;
	}
	else if (inputPageSize == 3)
	{
		pageSize = 300;
	}
	else
	{
		cout << "Error. Invalid input." << endl;
	}


	system("pause");
	return 0;
}