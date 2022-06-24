#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>

using namespace std;

vector<double> lengths;
vector<double> weights;
vector<string> segments;
vector<string> discreteEdges;
vector<int> countCollisions ;

int find(string name){
  int index = -1;
  bool found = false;
  for(int i = 0; i < discreteEdges.size(); i++){
    if(discreteEdges[i].compare(name) == 0){
      found = true;
      index = i;
      break;
    }
  }
  return index;
}

double getListDiscreteEdges(string fileName) {
  ifstream file(fileName);
  string line;
  double sum = 0;
  int index = 0;
  
  while (getline(file, line)) {
    // get first word of line
    stringstream ss(line);
    string word;
    getline(ss, word, ' ');
    
    string weight;
    getline(ss, weight, ' ');
    double x = std::stod(weight);
    index = find(word);
    if(index == -1){
      discreteEdges.push_back(word);
      lengths.push_back(x);
      countCollisions.push_back(0);
    }
    else{
      lengths[index] += x;
      countCollisions[index] += 1;
    }
    
    sum += x;
  }
  return sum;
}

void getListSegments(string fileName) {
  ifstream file(fileName);
  string line;

  while (getline(file, line)) {
    // get first word of line
    //stringstream ss(line);
    //string word;
    segments.push_back(line + "$");
  }

}

int main(int argc, char const* argv[]) {
  double x = getListDiscreteEdges("discreteEdges.txt");
  getListSegments("insideWeightEdges.txt");
  vector<int> locations;
  string temp = "";
  string key = "";
  double y = 0;
  double sum = 0;
  
  for (int i = 0; i < segments.size(); i++) {
    //cout<<"Edge: "<<segments[i]<</*" weight: "<<lengths[i]<<*/endl;
    sum = 0;
    locations.clear();
    temp ="";
    for(int j = 0; j < discreteEdges.size(); j++){
      key = "$" + discreteEdges[j] + "$";
      if((segments[i].find(key) != std::string::npos)
          && (temp.find(key) == std::string::npos)
          ){
        sum += lengths[j] / (countCollisions[j] + 1);
        locations.push_back(j);
        temp = temp + "$" + discreteEdges[j] + "$";
      }
    }
    //sum = sum / ()
    y += sum;
    weights.push_back(sum);
    for(int k = locations.size() - 1; k >= 0; k--){
      discreteEdges.erase(discreteEdges.begin() + locations[k]);
      lengths.erase(lengths.begin() + locations[k]);
    }
  }
  assert(y < x);
  //cout<<argc<<" "<<argv<<endl;
  //cout<<"y = "<<y <<" < x = "<<x<<endl;
  //cout<<"% (x-y)/x = "<<(x-y)*100/x<<endl;
  //cout<<weights.size()<<endl;
  for (int i = 0; i < weights.size(); i++) {
    if(i == 71){
      //cout<<segments[i]<<" ";
    }
    //if(weights[i] == 0)
      //cout<<"Edge ["<<i<<"] name has words: "<<segments[i].length()<<" weight: "<<weights[i]<<endl;
    cout<<segments[i]<<"  "<<weights[i]<<endl;
  }
}