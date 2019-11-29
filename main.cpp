#include <iostream>
#include <fstream>
#include <cstring>
#include <semaphore.h>

using namespace std; 

#define max_size 100 // total length of the message
#define EOL "<EOL>"

static sem_t s;

int uniq_count = 0; // total unique characters in the message
int freq_list[max_size]={0}; // list of frequency of the characters
char uniq_msg[max_size]={}; // list of unique characters
string code[max_size]; // list of code of the characters
char msg[max_size]; // msg to be decompressed

// function to find the index in array
int indexOf(char ch, const char *str) {
	int i = 0, index = -1;
	while(str[i] != '\0'){
		if (ch == str[i]){
			index = i;
			break;
		}
		i++;
	}
	return index;
}

// function to sort the characters in uniq array
void sort() {
	for (int i = 0; i < uniq_count; i++){
		for (int j = i+1; j < uniq_count; j++){
			if (freq_list[j] < freq_list[i]){
				int temp_freq = freq_list[i];
				char temp_ch = uniq_msg[i];
				string temp_code = code[i];

				freq_list[i] = freq_list[j];
				uniq_msg[i] =  uniq_msg[j];
				code[i] = code[j];
				
				freq_list[j] = temp_freq;
				uniq_msg[j] = temp_ch;
				code[j] = temp_code;
				
			} else if (freq_list[j] == freq_list[i]){
				if (uniq_msg[j] > uniq_msg[i]){
					int temp_freq = freq_list[i];
					char temp_ch = uniq_msg[i];
					string temp_code = code[i];

					freq_list[i] = freq_list[j];
					uniq_msg[i] =  uniq_msg[j];
					code[i] = code[j];

					freq_list[j] = temp_freq;
					uniq_msg[j] = temp_ch;
					code[j] = temp_code;
				}
			}
		}
	}
}

// child thread
void * child_thread(void* symbol) {

	int i = indexOf(*(char*)symbol, uniq_msg);

	if (uniq_msg[i] == '\n') {
		cout << EOL << " Binary code = " << code[i] << endl;
	} else {
		cout << uniq_msg[i] << " Binary code = " << code[i] << endl;
	}

	string temp = msg;
	int msgcnt = 0;
	int tempcnt = 0;

	for(int j = 0; j < code[i].length(); j++) {
		if (code[i][j] == '1') {
			msg[msgcnt] = uniq_msg[i];
			msgcnt++;
		} else if (code[i][j] == '0') {
			msg[msgcnt] = temp[tempcnt];
			msgcnt++;
			tempcnt++;
		}
	}

	sem_post(&s);
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {

	if (argc != 1) {
		std::cout << "Invalid parameters" << std::endl;
		exit(EXIT_FAILURE); // terminate with error
	}

	string line[max_size];

	int i = 0;
	int startindex = 0;

	while(i != max_size) {

		getline(cin, line[i]);
		int freq = 0;

		// retrieve unique characters from the code
		if (line[i][0] == '<' && line[i][1] == 'E' && line[i][2] == 'O' && line[i][3] == 'L' && line[i][4] == '>') {
			uniq_msg[i] = '\n';
			startindex = 6;
		} else {
			uniq_msg[i] = line[i][0];
			startindex = 2;
		}

		// retrieve frequency of the unique characters
		for (int j = startindex; line[i][j] != '\0'; j++) {
			if (line[i][j] == '1') {
				freq++;
			}

			if (line[i].length() - startindex <= 100) {
				code[i] = line[i].substr(startindex, line[i].length() - startindex);
			}
		}

		freq_list[i] = freq;

		// for terminating the loop
		if (line[i][0] == '\0')
			break;

		i++;
	}

	uniq_count = i;

	sort(); // sort the characters based on their freq (and ascii value for same frequency)

	pthread_t t[uniq_count]; //declare theread

	memset(msg, 0, sizeof(msg)); // reset the array

	int ok;

	ok = sem_init(&s, 0, 0);

	if (ok < 0) {
		perror("Semaphore Initialization Error");
		return(EXIT_FAILURE);
	}

	for (int i = 0; i < uniq_count; i++){

		ok = pthread_create(&t[i], NULL, &child_thread, (void*)&uniq_msg[i]);
		if (ok != 0) {
			perror("Thread Creation failed!");
			exit(EXIT_FAILURE);
		}
		sem_wait(&s);

		ok = pthread_join(t[i],NULL); // wait untill the child threads completes the execution

		if (ok != 0) {
			perror("Thread joined failed!");
			exit(EXIT_FAILURE);
		}
	}

	sem_destroy(&s);
	cout << "Decompressed file contents:" << endl << msg << endl;

	// writes the code to the file
	ofstream outfile;
	outfile.open("./Output.txt");

	if (!outfile) {
		perror("Unable to open file");
		exit(EXIT_FAILURE); // terminate with error
	}

	for (int i = 0; i < uniq_count; i ++){
		if (uniq_msg[i] == '\n') {
			outfile << EOL << " Binary code = " << code[i] << endl;
		} else {
			outfile << uniq_msg[i] << " Binary code = " << code[i] << endl;
		}
	}

	outfile << "Decompressed file contents:" << endl << msg;

	outfile.close();
	pthread_exit(NULL);
	exit(EXIT_SUCCESS);

	return 0;
}