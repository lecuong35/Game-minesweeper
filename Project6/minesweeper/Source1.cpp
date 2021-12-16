#include<iostream>
#include<vector>
#include<algorithm>
#include<fstream>

using namespace std;

int main()
{
	vector<short> arr;
	ifstream read;
	read.open("file9x9.txt");
	if (read.is_open())
	{
		short i = 0;
		while (read >> i) {
			if (i == 0) break;
			arr.push_back(i);
		}
	}
	else
	{
		cout << "khong mo duoc de doc";
	}
	arr.push_back(54);
	read.close();
	short size = arr.size();
	short j = 0;
	ofstream write;
	write.open("file9x9.txt", ios::trunc);
	if(write.is_open())
	{
		while (j < size)
		{
			write << arr[j];
			++j;
		}
	}
	else
	{
		cout << "khong mo duoc de ghi";
	}
	write.close();
}