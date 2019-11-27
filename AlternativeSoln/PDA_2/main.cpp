#include <iostream>
#include <cstring>
#include <fstream>
using namespace std; 
#define size 100 // length of the message to be decompressed

string comCode[size]; // list of compressed comCode of the characters
char messageList[size]={}; // list of unique characters in the message
int messageCount=0; // number of unique characters in the message
int countList[size]={0}; // list of frequency of the unique characters in the message
char message[size]; // message to be decompressed


// this function sorts the characters based on their freq (and ascii value for same frequency)
void sortMessage() {
	for (int i=0; i<messageCount; i++){
		for (int j=i+1; j<messageCount; j++){
			if (countList[j]<countList[i]){
				int temp_freq=countList[i];
				string temp_comCode=comCode[i];
				char temp_ch=messageList[i];
				countList[i]=countList[j];
				messageList[i]= messageList[j];
				comCode[i]=comCode[j];
				countList[j]=temp_freq;
				comCode[j]=temp_comCode;
				messageList[j]=temp_ch;
			} else if (countList[j] == countList[i]){
				if (messageList[j] > messageList[i]){
					char temp_ch=messageList[i];
					int temp_freq=countList[i];
					string temp_comCode=comCode[i];
					countList[i]=countList[j];
					comCode[i]=comCode[j];
					messageList[i]= messageList[j];
					countList[j]=temp_freq;
					messageList[j]=temp_ch;
					comCode[j]=temp_comCode;
				}
			}
		}
	}
}


// this function finds the index in string array (arg1: string array, arg2: element)
int findIndex(char *strArr, char c) {
	int i=0, idx=-1;
	while(strArr[i] != '\0'){
		if (c == strArr[i]){
			idx=i;
			break;
		}
		i++;
	}
	return idx;
}


// child thread
void * child_thrd_function(void* msg) {
	int idx=findIndex(messageList, *(char*)msg);

	if (messageList[idx] == '\n') {
		cout<<"<EOL> Binary comCode = "<<comCode[idx]<<endl;
	} else {
		cout<<messageList[idx]<<" Binary comCode = "<<comCode[idx]<<endl;
	}

	int tempcount=0, count=0;
	string tempmsg=message;
	
	for(int j=0; j<comCode[idx].length(); j++) {
		if (comCode[idx][j] == '1') {
			message[count]=messageList[idx];
			count++;
		} else if (comCode[idx][j] == '0') {
			message[count]=tempmsg[tempcount];
			count++;
			tempcount++;
		}
	}
}


int main(int argc, char *argv[]) {
	int idx=0, codeStart=0; // starting index of the Compressed Code
	string myLine[size]; // store array of lines from input files

	while(idx != size) {

		int repeatCount=0;
		getline(cin, myLine[idx]); // take input line by line and store to myLine
		
		// find the unique characters in the message
		if (myLine[idx][0] == '<' && myLine[idx][1] == 'E' && myLine[idx][2] == 'O' && myLine[idx][3] == 'L' && myLine[idx][4] == '>') {
			messageList[idx]='\n';
			codeStart=6;
		} else {
			messageList[idx]=myLine[idx][0];
			codeStart=2;
		}

		// find the repeatations of the characters in the message
		for (int chrcount=codeStart; myLine[idx][chrcount] != '\0'; chrcount++) {
			if (myLine[idx][chrcount] == '1') {
				repeatCount++;
			}
			if (myLine[idx].length() - codeStart <= 100) {
				comCode[idx]=myLine[idx].substr(codeStart, myLine[idx].length() - codeStart);
			}
		}

		countList[idx]=repeatCount;
		if (myLine[idx][0] == '\0') break;// condition to break the while loop
		idx++; // increment the index of line
	}

	messageCount=idx; // total number of unique characters in the message
	sortMessage(); // sort the message
	memset(message, 0, sizeof(message)); // clear the garbage value of the array element
	pthread_t mythread[messageCount]; // declaration of the thread
	
	for (int j=0; j<messageCount; j++){
		pthread_create(&mythread[j], NULL, &child_thrd_function, (void*)&messageList[j]);
		pthread_join(mythread[j],NULL); // wait the child threads
	}
	cout<<"Decompressed file contents:"<<endl<<message<<endl;
	// writes the output to the file
	ofstream outfile;
	outfile.open("./output.txt");

	if (outfile) {
		for (int i=0; i<messageCount; i ++){
			if (messageList[i] == '\n') {
				outfile<<"<EOL> Binary comCode = "<<comCode[i]<<endl;
			} else {
				outfile<<messageList[i]<<" Binary comCode = "<<comCode[i]<<endl;
			}
		}
		outfile<<"Decompressed file contents:"<<endl<<message;
	}
	outfile.close();
	return 0;
}