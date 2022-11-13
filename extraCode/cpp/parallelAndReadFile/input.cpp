#include <iostream>
#include <stdio.h>
using namespace std;
int main(){
	char c;
	while(1){
		cout<<"Nhap r: run; s: stop"<<endl;
		cout<<"Nhap vao ky tu dieu khien:";
		cin>>c;
		int result;
		switch(c){
			case 's':
				result = rename("run", "stop");
				break;	
			case 'r':
				result = rename("stop", "run");
				break;
			case 'e':
				break;
		}
	}
	return 0;
}
