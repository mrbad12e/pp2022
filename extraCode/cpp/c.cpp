#include <bits/stdc++.h>
using namespace std;
int count=0;
int N;
vector<string> res;
void line_morphing(string str){
    int it=0;
    string num;
    string non_int;
    string key;
    size_t pos;
    size_t len;
    for(int i=0;i<str.length();i++){ 
        if(str[i]=='_') {
            non_int=str.substr(0,i);
            break;
        }
    }
    for(int i=0;i<str.length();i++){
        if(str[i]==' '){
            pos=++i;
            len=0;
            while(str[i]!='_'&&str[i]!=' '){
                len++;
                i++;
            }
            if(str[i]==' '){
                num=str.substr(pos,len);
                i--;
                continue;
            }
            key=str.substr(pos,len);
            it++;
        }
    }
    if(it>=2){
        res.push_back(non_int+'$'+non_int+key+' '+num);
        num='0';
        res.push_back(non_int+key+'$'+key+' '+num);
    } else {
        res.push_back(non_int+'$'+key+' '+num);
    }
}
int main(){
    //Câu c
    fstream newfile;
    newfile.open("input.txt",ios::in);
    ofstream outfile("edges.txt");
    string str;
    while(getline(newfile,str)){
        line_morphing(str);
    }
    for(string a:res){
        outfile<<a<<'\n';
    }
    newfile.close();
    outfile.close();
    return 0;
}