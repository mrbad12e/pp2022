#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
struct Node {
  double time;
  double x;
  double y;
  string laneId;
  double lanePosition;
  double speed;
  double acceleration;
  double totalDistance;
  int carId;
};

// convert E2_1 to E2
string convertLaneId(string laneId) {
  string newLaneId = "";
  for (int i = 0; i < laneId.length(); i++) {
    if (laneId[i] == '_') {
      break;
    }
    newLaneId += laneId[i];
  }
  return newLaneId;
}

vector<string> getListIntersection(string fileName) {
  ifstream file(fileName);
  string line;
  vector<string> intersectionList;
  map<string, int> junctionMap;

  while (getline(file, line)) {
    // get first word of line
    stringstream ss(line);
    string word;
    getline(ss, word, ' ');
    string laneId = convertLaneId(word);
    if (junctionMap.find(laneId) == junctionMap.end()) {
      junctionMap[laneId] = 1;
    } else {
      junctionMap[laneId]++;
    }
  }

  for (auto it = junctionMap.begin(); it != junctionMap.end(); it++) {
    if (it->second > 2) {
      intersectionList.push_back(it->first);
    }
  }
  return intersectionList;
}

map<string, vector<string> > makeDirectEgde(string fileName) {
  vector<string> intersectionList = getListIntersection(fileName);
  map<string, vector<string> > directEdgeMap;

  ifstream file(fileName);
  string line;
  while (getline(file, line)) {
    // get first word of line
    stringstream ss(line);
    string word;
    getline(ss, word, ' ');
    string directEdge = convertLaneId(word);

    // get third word of line
    getline(ss, word, ' ');
    getline(ss, word, ' ');
    string laneId = convertLaneId(word);

    if (directEdge == laneId) {
      continue;
    }

    if (directEdgeMap.find(laneId) == directEdgeMap.end()) {
      vector<string> listDirectEdge;
      listDirectEdge.push_back(directEdge);
      directEdgeMap[laneId] = listDirectEdge;
    } else {
      directEdgeMap[laneId].push_back(directEdge);
    }
  }
  return directEdgeMap;
}

string normalizeLaneId(string laneId) {
  string newLaneId = "";
  // if laneId is -E2_1, then newLaneId is E2_1
  if (laneId[0] == '-') {
    newLaneId = laneId.substr(1);
  } else {
    newLaneId = laneId;
  }
  return newLaneId;
}

// check if laneId or normalizeLaneId is not in directEdge, return
string getDirectLane(vector<string> directEdge,
                     vector<string> listPosibleLane) {
  for (int i = 0; i < listPosibleLane.size(); i++) {
    int check = 0;
    for (int j = 0; j < directEdge.size(); j++) {
      string normalDirectEdge = normalizeLaneId(directEdge[j]);
      string normalLaneId = normalizeLaneId(listPosibleLane[i]);
      if (normalDirectEdge == normalLaneId) {
        check = 1;
        break;
      }
    }

    if (check == 0) {
      return listPosibleLane[i];
    }
  }
  return "";
}

int main(int argc, char const* argv[]) {
  map<string, vector<string> > directEdgeMap = makeDirectEgde("input.txt");
  vector<string> intersectionList = getListIntersection("input.txt");
  map<string, vector<vector<string> > > intersectionDirectLaneMap =
      map<string, vector<vector<string> > >();

  for (int i = 0; i < intersectionList.size(); i++) {
    string intersectionId = intersectionList[i];
    vector<string> listDirectEdge = directEdgeMap[intersectionId];
    vector<vector<string> > listPossibleLane = vector<vector<string> >();

    for (auto it = listDirectEdge.begin(); it != listDirectEdge.end(); it++) {
      vector<string> directEdge;
      string laneId = *it;

      while (find(intersectionList.begin(), intersectionList.end(), laneId) ==
             intersectionList.end()) {
        directEdge.push_back(laneId);

        if (directEdgeMap[laneId].empty()) {
          break;
        }
        laneId = getDirectLane(directEdge, directEdgeMap[laneId]);
      }
      listPossibleLane.push_back(directEdge);
    }
    intersectionDirectLaneMap[intersectionId] = listPossibleLane;
  }
  int totalIntersections = 0;
  
  vector<string> entryLanes;// = new vector<string>();

  for (auto it = intersectionDirectLaneMap.begin();
       it != intersectionDirectLaneMap.end(); it++) {
    //cout << "===========================" << endl;
    //cout << "intersection id: " << it->first << endl;
    vector<vector<string> > listDirectEdge = it->second;
    totalIntersections++;
    for (int i = 0; i < listDirectEdge.size(); i++) {
      vector<string> directEdge = listDirectEdge[i];
      //printf("%d. ", i + 1);
      int lastIndex = directEdge.size() - 1;
      /*for (int j = lastIndex; j >= 0; j--) {
        cout << "$"<<directEdge[j] << "";
      }
      cout<<"$"<<directEdge[0]<<"_"<<it->first<<endl;*/
      
      int index = 0;
      if(directEdge[0][0] == '-')
      	index = 1;
      string entry = "$" + it->first + "$" + (index == 0 ? "-" : "") + directEdge[0].substr(index);
      //cout<<entry<<endl;
      entryLanes.push_back(entry);
    }
  }
  cout<<"Total intersections: "<<totalIntersections<<endl;
  
  for (auto it = intersectionDirectLaneMap.begin();
       it != intersectionDirectLaneMap.end(); it++) {
    cout << "===========================" << endl;
    //cout << "intersection id: " << it->first << endl;
    vector<vector<string> > listDirectEdge = it->second;
    totalIntersections++;
    for (int i = 0; i < listDirectEdge.size(); i++) {
      vector<string> directEdge = listDirectEdge[i];
      //printf("%d. ", i + 1);
      int lastIndex = directEdge.size() - 1;
      bool found = false;
      int k = 0;
      for(k = 0; k < entryLanes.size(); k++)
      {
      	  if(entryLanes[k].find("$" + directEdge[lastIndex]) != std::string::npos)
      	  {
      	  	found = true;
      	  	cout<<entryLanes[k];
      	  	lastIndex--;
      	  	break;
      	  }
      }
      if(found)
      {
      	entryLanes.erase(entryLanes.begin() + (lastIndex + 1));
      }
      for (int j = lastIndex; j >= 0; j--) {
        cout << "$"<<directEdge[j] << "";
      }
      cout<<"$"<<directEdge[0]<<"_"<<it->first<<endl;
      
      
    }
  }

  return 0;
}

