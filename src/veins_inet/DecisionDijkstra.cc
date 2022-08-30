//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "DecisionDijkstra.h"

DecisionDijkstra::DecisionDijkstra() {
    // TODO Auto-generated constructor stub
    this->generateEmergencyEdges();
    this->generateEmergencyVertices();
}

DecisionDijkstra::~DecisionDijkstra() {
    // TODO Auto-generated destructor stub
}


void DecisionDijkstra::generateEmergencyEdges(){
    double tempW;
    int tempIndex;
    std::string tempTrace;

    this->emergencyAdjList.resize(this->adjList.size());
    for(int i = 0; i < this->adjList.size(); i++){
        for (std::vector<Quad>::iterator it = this->adjList[i].begin(); it != this->adjList[i].end(); it++){
              tempW = std::get<0>(*it);
              tempTrace = std::get<3>(*it);
              tempIndex = std::get<2>(*it);
              if(tempIndex >= this->numIVertices){
                  //tempIndex -= numIVertices;
                  //tempIndex /= 3;
                  EV<<"Exceed!"<<endl;
              }
              this->emergencyAdjList[i].push_back(std::make_tuple(tempW, tempW, tempIndex, tempTrace));
        }
    }
}

void DecisionDijkstra::generateEmergencyVertices(){
    for(int i = 0; i < this->numIVertices; i++){
        this->emergencyVertices.push_back("_" + vertices[i]);
    }

    this->timeW_E_Vertices.resize(numVertices);
    for(int i = 0; i < this->numVertices; i++){
        this->timeW_E_Vertices[i] = 0;
    }
}

void DecisionDijkstra::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
        int source, int target, std::string currLane, AGV* cur){
  Quad info; //(-1, "", -1, "");
  std::string trace;
  double weight, objective;
  double tempW;
  int tempIndex;
  double now = cur->now;
  if(currLane.compare(cur->itinerary->laneId) != 0){
      now = cur->ShortestPath[source];
  }
  double ratio = cur->ratio;
  std::string tempTrace;
  int index = findI_Vertex(currLane, false);
  double firstCost = firstValue(currLane, vertices[index]);

  cur->init(numVertices);
  cur->ShortestPath[source] = this->getHarmfulness(cur, ratio * firstCost + now);

  cur->PQ.push(std::make_tuple(cur->ShortestPath[source], 0, source, "")); // Source has weight cur->ShortestPath[source];

  while (!cur->PQ.empty()){
    info = cur->PQ.top(); // Use to get minimum weight
    cur->PQ.pop(); // Pop before checking for cycles
    cur->count = cur->count + 1;
    source = std::get<2>(info); // get the vertex
    if(source == target){
        while (!cur->PQ.empty())
            cur->PQ.pop();
        break;
    }
    /*objective = std::get<0>(info); // current distance
    weight = std::get<1>(info);
    trace = std::get<3>(info);*/


    if (cur->visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    cur->visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    this->checkActiveEdges(firstCost, source, cur, &info, currLane);
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm

void DecisionDijkstra::checkActiveEdges(double firstCost, int source, AGV* cur, Quad* info, std::string currLane){
    std::string trace;
    double weight = 0, objective = 0;
    double tempW;
    int tempIndex;
    double now = cur->now;
    double ratio = cur->ratio;
    std::string tempTrace;
    objective = std::get<0>(*info); // current distance
    weight = std::get<1>(*info);
    trace = std::get<3>(*info);
    for (std::vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
        tempW = /*(*it).weight; */std::get<0>(*it);
        tempTrace = /*(*it).trace; */std::get<3>(*it);
        tempIndex = /*(*it).source; */ std::get<2>(*it);
        if(!Constant::SHORTEST_PATH){
            timeWeightVertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, timeWeightVertices[tempIndex], vertices[tempIndex]);
        }

        double newWeight = 0, newObjective = 0; //weight + tempW + 40*weightVertices[tempIndex];

        if(!isValidTrace(currLane, tempTrace)){
            continue;
        }
        if(isAntidromic(trace, tempTrace)){
            continue;
        }
        newWeight = weight + tempW;
        if(!Constant::SHORTEST_PATH){
            double weightSmoothing = timeWeightVertices[tempIndex];
            if(weightSmoothing < 0.1 && tempIndex < this->numIVertices){
                newWeight += 100*(this->expSmoothing->useCycicalData(newWeight, vertices[tempIndex], weightSmoothing));
            }
            else{
                newWeight += 100*weightSmoothing;
            }
        }
        newWeight += firstCost;
        //newObjective = (ratio * (newWeight) + now);
        newObjective = this->getHarmfulness(cur, ratio * (newWeight) + now);
        newObjective += objective/*tempW;*/ /*now*/;
        if (newObjective < cur->ShortestPath[tempIndex]){ // Check if we can do better
            cur->ShortestPath[tempIndex] = newObjective; // Update new distance
            cur->traces[tempIndex] = trace; //tempTrace;
            if(tempIndex == 26){
                EV<<"DEEDDWF"<<endl;
            }
            if(tempTrace.length() == 0 && trace.length() == 0)
            {
                EV<<"ENULLLL"<<endl;
            }

            std::string content = vertices[tempIndex] + "_" + cur->id;
            std::string newTrace = trace + tempTrace;
            cur->PQ.push(make_tuple(newObjective, newWeight, /*content,*/ tempIndex, newTrace));
        } // Update distance
    }//End of for
}
