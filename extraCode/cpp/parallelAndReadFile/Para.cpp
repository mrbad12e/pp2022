#include <iostream>
#include <omp.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

enum STATE_OF_REQUEST{
    WAITING_FOR_PROCESSING,
    BEING_PROCESSED,
    FINISHED,
    NONE
};

int signal(){
	DIR *folder;
	folder = opendir(".");
	struct dirent *entry;
	int result = -1;
	while(entry = readdir(folder)){
		if(strcmp(entry->d_name, "run") == 0){
			result = 0;
		}
		if(strcmp(entry->d_name, "stop") == 0){
			result = 1;
		}
	}
	closedir(folder);
	return result;
}


using namespace std;
int main(){
	cout<<"Prepare for infinite loop"<<endl;
	STATE_OF_REQUEST state = WAITING_FOR_PROCESSING;
	
	int action = 0;

	while(1){
		usleep(100);
		switch(state){
			case BEING_PROCESSED:
				break;
			case WAITING_FOR_PROCESSING:
				action = signal(/*folder*/);
				switch(action){
					case 0:
						state = BEING_PROCESSED;
						#pragma omp parallel
						{
							cout<<"Hello "<<omp_get_thread_num()<<endl;
						}
						state = FINISHED;
						break;
				}
				break;
			case FINISHED:
				action = signal(/*folder*/);
				switch(action){
					case 1:
						state = WAITING_FOR_PROCESSING;
						cout<<"Waiting for new order"<<endl;
						break;
				}
				break;
		}
	}
	cout<<"Het roi"<<endl;
	//closedir(folder);
	return 0;
}
