#include <iostream>

#include <cstring>

#include <fstream>

using namespace std;

#define size 100

struct elements
{
	char msg;
	int freq;
	string code;
};

int no_char = 0; // no of unique character in the message
char origmsg[size]; // original message to be decompressed
elements records[size]; // reach records with unique character, its freq and compressed code

// function to swap the records
void swap(elements * xp, elements * yp)
{
	elements temp = * xp;
	* xp = * yp;
	* yp = temp;
}

// function to sort the records
void selectionSort(elements arr[], int n)
{
	int i, j, min_idx;

	// One by one move boundary of unsorted subarray  
	for (i = 0; i < n - 1; i++)
	{
		// Find the minimum element in unsorted array  
		min_idx = i;
		for (j = i + 1; j < n; j++)
			if (arr[j].freq < arr[min_idx].freq)
			{
				min_idx = j;
			}
		else if (arr[j].freq == arr[min_idx].freq)
		{
			if (arr[j].msg > arr[min_idx].msg)
			{
				min_idx = j;
			}
		}
		// Swap the found minimum element with the first element  

		swap( & arr[min_idx], & arr[i]);

	}
}

// function to return the index of array
int whichElement(char x, elements r[], int n)
{
	for (int i = 0; i < n; i++) {
		if (r[i].msg == x) {
			return i;
		}
	}
	return -1;
}

// child thread
void * childFunc(void* ch) {
	int i = whichElement(*(char*)ch, records, no_char);
	
	if (records[i].msg == '\n')
	{
		cout << "<EOL> Binary Code = " << records[i].code << endl;
	} else
	{
		cout << records[i].msg << " Binary Code = " << records[i].code << endl;
	}

	string temp = origmsg;
	int msgcnt = 0;
	int tempcnt = 0;

	for(int j = 0; j < records[i].code.length(); j++)
	{
		if (records[i].code[j] == '1')
		{
			origmsg[msgcnt] = records[i].msg;
			msgcnt++;
		} else if (records[i].code[j] == '0')
		{
			origmsg[msgcnt] = temp[tempcnt];
			msgcnt++;
			tempcnt++;
		}
	}
}

int main(int argc, char const *argv[])
{
	string codeline[size];

	int i = 0;
	while (getline(cin, codeline[i]))
	{
		i++;
	}
	no_char = i;

	for (int j = 0; j < i; j++)
	{
		records[j].msg = codeline[j][0]; // store the message
		int startidx = 2; // char + space
		if (codeline[j].substr(0,5) == "<EOL>") {
			records[j].msg = '\n'; // store the message
			startidx = 6; // "<EOL>" + space
		}
		for (int k = startidx; k < codeline[j].length(); k++)
		{
			if (codeline[j][k] == '1') records[j].freq++; // store the freq
		}

		records[j].code = codeline[j].substr(startidx,codeline[j].length()-startidx); // store the code
	}

	selectionSort(records, no_char);

	pthread_t td[no_char];
	
	for (int i = 0; i < no_char; i++)
	{

		pthread_create(&td[i], NULL, &childFunc, (void*)&records[i].msg);
		
		pthread_join(td[i],NULL);
		
	}

	cout<<"Decompressed file contents:"<<endl<<origmsg<<endl;
	ofstream outputFile;
	outputFile.open("./output.txt");
	
	if(outputFile)
	{
		for (int i = 0; i < no_char; i++)
		{
			outputFile << records[i].code <<'\n';
		}
	}

	outputFile.close();
	
	return 0;
}