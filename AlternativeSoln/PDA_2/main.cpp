#include <iostream>
#include <cstring>
#include <fstream>
using namespace std; 
#define size 100 // length of the message to be decompressed

class message
{
public:
	char chr; // uniq character
	int count; // count of uniq characters
	string bcode; // binary code
};

message list[size]; // input compressed message list
int messageCount=0; // number of unique characters in the message
int childcount = 0; // no. of child thread counter
char dmessage[size]; // decompredded original message


// this function sorts the characters based on their freq (and ascii value for same frequency)
void sortlist() {
	for (int i=0; i<messageCount; i++){
		for (int j=i+1; j<messageCount; j++){
			if (list[j].count < list[i].count){
				message temp;
				temp = list[i];
				list[i] = list[j];
				list[j] = temp;
			} else if (list[j].count == list[i].count){
				if (list[j].chr > list[i].chr){
					message temp = list[i];
					list[i] = list[j];
					list[j] = temp;
				}
			}
		}
	}
}


// child thread
void * child_thrd_function(void* msg) {
	
	int count=0,count1=0;
	string prevmsg=dmessage; // to store previous result

	if (list[childcount].chr == '\n') {
		cout<<"<EOL> Binary Code = "<<list[childcount].bcode<<"\n";
	} else {
		cout<<list[childcount].chr<<" Binary Code = "<<list[childcount].bcode<<"\n";
	}

	for(int j=0; j<list[childcount].bcode.length(); j++) {
		switch(list[childcount].bcode[j]){
			case '1':
				dmessage[count]=list[childcount].chr;
				count++;
				break;
			case '0':
				dmessage[count]=prevmsg[count1];
				count++;
				count1++;
				break;
			default:
				break;

		}
	}
	childcount++;
}


int main(int argc, char *argv[]) {
	int idx=0, codeStart=0; // starting index of the Compressed Code
	string myLine[size]; // store array of lines from input files

	while(idx != size) {

		int repeatCount=0;
		getline(cin, myLine[idx]); // take input line by line and store to myLine
		
		// find the unique characters in the message
		if (myLine[idx].substr(0,5) == "<EOL>") {
			list[idx].chr='\n';
			codeStart=6;
		} else {
			list[idx].chr=myLine[idx][0];
			codeStart=2;
		}

		// find the repeatations of the characters in the message
		for (int chrcount=codeStart; myLine[idx][chrcount] != '\0'; chrcount++) {
			if (myLine[idx][chrcount] == '1') {
				repeatCount++;
			}
			if (myLine[idx].length() - codeStart <= 100) {
				list[idx].bcode=myLine[idx].substr(codeStart, myLine[idx].length() - codeStart);
			}
		}

		list[idx].count=repeatCount;
		if (myLine[idx][0] == '\0') break;// condition to break the while loop
		idx++; // increment the index of line
	}

	messageCount=idx; // total number of unique characters in the message
	sortlist(); // sort the message
	memset(dmessage, 0, sizeof(dmessage)); // clear the garbage value of the array element
	pthread_t mythread[messageCount]; // declaration of the thread
	
	for (int j=0; j<messageCount; j++){
		pthread_create(&mythread[j], NULL, &child_thrd_function, (void*)&list[j].chr);
		pthread_join(mythread[j],NULL); // wait the child threads
	}
	cout<<"Decompressed file contents:"<<"\n"<<dmessage<<"\n";
	// writes the output to the file
	ofstream outfile;
	outfile.open("./output.txt");

	if (outfile) {
		for (int i=0; i<messageCount; i ++){
			if (list[i].chr == '\n') {
				outfile<<"<EOL> Binary comCode = "<<list[i].bcode<<"\n";
			} else {
				outfile<<list[i].chr<<" Binary comCode = "<<list[i].bcode<<"\n";
			}
		}
		outfile<<"Decompressed file contents:"<<"\n"<<dmessage;
	}
	outfile.close();
	return 0;
}