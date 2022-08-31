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

#include "HarmfulnessDijkstra.h"

HarmfulnessDijkstra::HarmfulnessDijkstra() {
    // TODO Auto-generated constructor stub

}

HarmfulnessDijkstra::~HarmfulnessDijkstra() {
    // TODO Auto-generated destructor stub
}

double HarmfulnessDijkstra::getHarmfulnessArrival(AGV* cur, double time){
    double result = allSs[cur->itinerary->station]->getHarmfulness(time, cur->indexOfRoute);
    return result;
}

void HarmfulnessDijkstra::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
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
  cur->ShortestPath[source] = this->getHarmfulnessArrival(cur, ratio * firstCost + now);

  int x = cur->PQ.size();

  cur->PQ.push(std::make_tuple(cur->ShortestPath[source], 0, source, "")); // Source has weight cur->ShortestPath[source];

  while (!cur->PQ.empty()){
    x = cur->PQ.size();
    info = cur->PQ.top(); // Use to get minimum weight
    cur->PQ.pop(); // Pop before checking for cycles
    x = cur->PQ.size();
    cur->count = cur->count + 1;
    if(cur->id.compare("142") == 0 && cur->count == 20){
        EV<<"SDfsdfsERereer"<<endl;
    }
    source = std::get<2>(info); // get the vertex
    if(source == target){
        while (!cur->PQ.empty())
            cur->PQ.pop();
        break;
    }
    objective = std::get<0>(info); // current distance
    weight = std::get<1>(info);
    trace = std::get<3>(info);


    if (cur->visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    cur->visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    for (std::vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
      tempW = /*(*it).weight; */std::get<0>(*it);
      //tempW = std::get<1>(*it);
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
      newObjective = this->getHarmfulnessArrival(cur, ratio * (newWeight) + now);
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
         x = cur->PQ.size();
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm
