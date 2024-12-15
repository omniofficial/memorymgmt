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

	cout << "Enter the page size: " << endl;
	cin >> inputPageSize;


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