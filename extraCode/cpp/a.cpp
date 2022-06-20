#include <bits/stdc++.h>
using namespace std;
int count=0;
int N;
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
int main(){
    //Câu a
    fstream newfile;
    newfile.open("input.txt",ios::in);
    string str;
    cout<<"Prepare to read file...."<<endl;
    while(getline(newfile,str)){
        intersection(str);
    }
    cout<<res.size();
    newfile.close();
    //Câu b
    ofstream outfile("vertices.txt");
    cin>>N;
    for(int i=0;i<N;i++){
        double num=3.0*(rand()%100)/100;
        outfile<<res[i]<<' '<<num<<'\n';
        cout<<res[i]<<' '<<num<<'\n';
    }
    outfile.close();
    return 0;
}
