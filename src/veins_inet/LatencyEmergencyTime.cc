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

#include "LatencyEmergencyTime.h"

LatencyEmergencyTime::LatencyEmergencyTime() {
    // TODO Auto-generated constructor stub

}

LatencyEmergencyTime::~LatencyEmergencyTime() {
    // TODO Auto-generated destructor stub
}

std::string LatencyEmergencyTime::getName(){
    return "Objective: Harmfulness No Emergence";
}


double LatencyEmergencyTime::getHarmfulnessArrival(AGV* cur, double time){
    if(cur->passedStation)
        return 0;
    double result = 0; //allSs[cur->itinerary->station]->getHarmfulness(time, cur->indexOfRoute, this->s, this->l);
    int count = cur->indexOfRoute;
    if(cur->bestTime + count*cur->period - cur->amplitude <= time &&
            cur->bestTime + count*cur->period + cur->amplitude >= time
    ){
        return 0;
    }
    if(cur->bestTime + count*cur->period - cur->amplitude > time){
        result = cur->bestTime + count*cur->period - cur->amplitude - time;
        result /= 2;
    }
    else{
        result = time - (cur->bestTime + count*cur->period + cur->amplitude);
    }
    return result;
}

void LatencyEmergencyTime::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
        int source, int target, std::string currLane, AGV* cur){
  Quad info; //(-1, "", -1, "");
  std::string trace;
  double weight, objective;
  double tempW;
  int tempIndex;
  double timeForEmergencyMode = 0;
  double now = cur->now;
  std::string tempTime = "";
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

  cur->PQ.push(std::make_tuple(cur->ShortestPath[source], 0, source, " 0")); // Source has weight cur->ShortestPath[source];

  while (!cur->PQ.empty()){
    x = cur->PQ.size();
    info = cur->PQ.top(); // Use to get minimum weight
    cur->PQ.pop(); // Pop before checking for cycles
    x = cur->PQ.size();
    cur->count = cur->count + 1;
    source = std::get<2>(info); // get the vertex
    if(source == target){
        while (!cur->PQ.empty())
            cur->PQ.pop();
        break;
    }
    objective = std::get<0>(info); // current distance
    weight = std::get<1>(info);
    trace = std::get<3>(info);
    tempTime = "";
    for(int i = trace.length() - 1; i > 0; i--){
        if((trace.at(i) >= '0' && trace.at(i) <= '9') || trace.at(i) == '.'){
            tempTime = trace.at(i) + tempTime;
        }
        else{
            break;
        }
    }



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

         //std::string content = vertices[tempIndex] + "_" + cur->id;
         std::string newTrace = trace + tempTrace;
         cur->PQ.push(make_tuple(newObjective, newWeight, /*content,*/ tempIndex, newTrace));
         x = cur->PQ.size();
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm
