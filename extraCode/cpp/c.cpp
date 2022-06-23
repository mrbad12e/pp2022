#include <bits/stdc++.h>
using namespace std;


int count=0;
int N;
vector<string> edges;
vector<string> res;

void intersection(string str){
    int it=0;
    string a;
    size_t pos;
    size_t len;
    for(int i=0;i<str.length();i++){
        if(str[i]==' '){
            pos=++i;
            len=0;
            while(str[i]!='_'&&str[i]!=' '){
                len++;
                i++;
            }
            if(str[i]==' '){
                i--;
                continue;
            }
            a=str.substr(pos,len);
            it++;
        }
    }
    if(it>=2) {
        if(find(res.begin(),res.end(),a)==res.end()) res.push_back(a);
    }
}


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
        edges.push_back(non_int+'$'+non_int+key+' '+num);
        num='0';
        edges.push_back(non_int+key+'$'+key+' '+num);
    } else {
        edges.push_back(non_int+'$'+key+' '+num);
    }
}
int main(){
    //Cau a
    fstream file;
    file.open("input.txt",ios::in);
    string str;
    cout<<"Prepare to read file...."<<endl;
    while(getline(file,str)){
        intersection(str);
    }
    cout<<res.size();
    file.close();
    //Assign trong so 
    //for(int i = 0; i < res.size(); i++){
    //    double num=3.0*(rand()%100)/100;
        //outfile<<res[i]<<' '<<num<<'\n';
        //cout<<res[i]<<' '<<num<<'\n';
    //}
    //Câu c
    fstream newfile;
    newfile.open("input.txt",ios::in);
    ofstream outfile("edges.txt");
    //string str;
    while(getline(newfile,str)){
        line_morphing(str);
    }
    for(string a:edges){
        outfile<<a<<'\n';
    }
    newfile.close();
    outfile.close();
    return 0;
}
