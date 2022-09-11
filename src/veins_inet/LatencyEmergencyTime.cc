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
    return "Objective: Threshold - " + std::to_string(Constant::THRESHOLD)
            + "Trade-off Latency & EmergencyTime  =>" + std::to_string(Constant::EMERGENCY_TIME/60);
}


double LatencyEmergencyTime::getHarmfulnessArrival(AGV* cur, double time){
    if(cur->passedStation)
        return 0;
    double result = 0; //allSs[cur->itinerary->station]->getHarmfulness(time, cur->indexOfRoute, this->s, this->l);
    int count = cur->indexOfRoute;
    double bestTime = this->allSs[cur->itinerary->station]->bestTime;
    double period = this->allSs[cur->itinerary->station]->period;
    double amplitude = this->allSs[cur->itinerary->station]->amplitude;
    if(bestTime + count*period - amplitude <= time && bestTime + count*period + amplitude >= time
    ){
        return 0;
    }
    if(bestTime + count*period - amplitude > time){
        result = bestTime + count*period - amplitude - time;
        result /= 60;
        //result /= 2;
    }
    else{
        result = time - (bestTime + count*period + amplitude);
        result /= 60;
    }
    return result;
}

void LatencyEmergencyTime::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
        int source, int target, std::string currLane, AGV* cur){
  Quad info; //(-1, "", -1, "");
  std::string trace;
  double now = cur->now;
  this->cur = cur;
  this->currLane = currLane;
  if(this->currLane.compare(this->cur->itinerary->laneId) != 0){
      now = this->cur->expectedTimeAtStation; //this->cur->ShortestPath[source];
  }
  double ratio = this->cur->ratio;
  std::string tempTrace;
  double totalEmergency = 0;
  double totalCost = 0;
  int index = findI_Vertex(this->currLane, false);
  double firstCost = firstValue(this->currLane, vertices[index]);

  this->cur->init(numVertices);
  this->cur->ShortestPath[source] = this->getHarmfulnessArrival(this->cur, ratio * firstCost + now);

  this->cur->PQ.push(std::make_tuple(this->cur->ShortestPath[source], 0, source, " 0")); // Source has weight cur->ShortestPath[source];
  bool found = false;

  while (!this->cur->PQ.empty()){
    info = this->cur->PQ.top(); // Use to get minimum weight
    this->cur->PQ.pop(); // Pop before checking for cycles
    this->cur->count = cur->count + 1;
    source = std::get<2>(info); // get the vertex
    if(source == target){
        std::string last = std::get<3>(info);
        std::string lastOfLast = last.substr(last.length() - 27);
        extractTraceAndTime(&last, &totalEmergency);
        totalCost = std::get<0>(info);
        bool accept = false;
        /*if(cur->MIN_LATENCY > 100000){
            cur->MIN_LATENCY = Constant::THRESHOLD;
            accept = true;
            cur->MIN_EMERGENCY = totalEmergency;
        }
        else*/
        if(target != this->cur->itinerary->exit || this->cur->itinerary->exit == this->cur->itinerary->indexStation){
            /*if(cur->MIN_EMERGENCY > totalEmergency ){
                if(totalCost < Constant::THRESHOLD || cur->MIN_EMERGENCY == DBL_MAX){
                    //the cur->MIN_EMERGENCY == DBL_MAX to ensure that at least one path will be added
                    accept = true;
                    cur->MIN_EMERGENCY = totalEmergency;
                    found = true;
                }
            }*/
            if(totalCost < Constant::THRESHOLD || cur->MIN_EMERGENCY == DBL_MAX){
                //the cur->MIN_EMERGENCY == DBL_MAX to ensure that at least one path will be added
                accept = true;
                found = true;
            }
            if(accept && cur->traces[target].compare(last) != 0){
                //cur->expectedTimeAtStation = ratio * std::get<1>(info) + cur->now;
                this->cur->allMinOptions.push_back(std::make_tuple(totalEmergency, std::get<1>(info), target, last)); // Source has weight cur->ShortestPath[source];
                lastOfLast = last.substr(last.length() - 17);
                cur->traces[target] = last;
            }
        }
        else{
            if(cur->traces[target].compare(last) != 0){
                cur->traces[target] = last;
            }
            while (!this->cur->PQ.empty())
                this->cur->PQ.pop();
            break;
        }

    }
    else //if(!found)
    {
        bool activeEdges = true;
        this->checkActiveEdges(firstCost, &info, activeEdges);
        this->checkActiveEdges(firstCost, &info, false);
    }
  } // While Priority Queue is not empty

  if(found){
      while (!this->cur->PQ.empty())
          this->cur->PQ.pop();
      int index = this->getBestChoise();
      cur->traces[target] = std::get<3>(this->cur->allMinOptions[index]);
      cur->expectedTimeAtStation = ratio * std::get<1>(this->cur->allMinOptions[index]) + cur->now;
      this->cur->allMinOptions.clear();
      //cur->traces[target] = "";
  }
} // DijkstrasAlgorithm

int LatencyEmergencyTime::getBestChoise(){
    double t = 0;
    int index = 0;
    double expectedValue = std::get<0>(this->cur->allMinOptions[0]);
    int size = this->cur->allMinOptions.size();
    for(int i = 1; i < size; i++){
        t = std::get<0>(this->cur->allMinOptions[i]);
        if(Constant::STRATEGY == ModeOfLatencyEmergenyTime::MIN){
            if(expectedValue > t){
                expectedValue = t;
                index = i;
            }
        }
        else if(Constant::STRATEGY == ModeOfLatencyEmergenyTime::MAX){
            if(expectedValue <= t){
                expectedValue = t;
                index = i;
            }
        }
    }
    return index;
}

void LatencyEmergencyTime::checkActiveEdges(double firstCost, Quad* info, bool activeEdges){
    std::string trace;
    double weight = 0, objective = 0;
    double tempW;
    int tempIndex;
    double timeForEmergencyMode = 0;

    double now = cur->now;
    double ratio = cur->ratio;
    std::string tempTrace;
    objective = std::get<0>(*info); // current distance
    weight = std::get<1>(*info);
    int source = std::get<2>(*info);
    if(!activeEdges){
        if(source >= this->numIVertices)
            source = std::get<2>(this->adjList[source][0]);
    }
    if(activeEdges){
        if (this->cur->visitedVertex.at(source)) // Check for cycle
            return; // Already accounted for it, move on
    }
    else{
        if (this->cur->visitedEmergencyVertex.at(source)) // Check for cycle
             return; // Already accounted for it, move on
    }

    if(activeEdges){
        this->cur->visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again
    }
    else{
        this->cur->visitedEmergencyVertex.at(source) = true;
    }

    trace = std::get<3>(*info);
    extractTraceAndTime(&trace, &timeForEmergencyMode);
    std::vector<std::vector<Quad>> *list = activeEdges ? &adjList : &emergencyAdjList;
    for (std::vector<Quad>::iterator it = (*list)[source].begin(); it != (*list)[source].end(); it++){
        tempW = /*(*it).weight; */std::get<0>(*it);
        tempTrace = /*(*it).trace; */std::get<3>(*it);
        double tempEmergencyTime = 0;
        tempIndex = /*(*it).source; */ std::get<2>(*it);
        if(!Constant::SHORTEST_PATH){
            if(activeEdges){
                timeWeightVertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, timeWeightVertices[tempIndex], vertices[tempIndex]);
            }
            else{
                timeW_E_Vertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, timeW_E_Vertices[tempIndex], vertices[tempIndex]);
            }
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
            double weightSmoothing = activeEdges ? timeWeightVertices[tempIndex] : timeW_E_Vertices[tempIndex];
            if(weightSmoothing < 0.1 && tempIndex < this->numIVertices && activeEdges){
                newWeight += 100*(this->expSmoothing->useCycicalData(newWeight, vertices[tempIndex], weightSmoothing));
            }
            else{
                newWeight += 100*weightSmoothing;
            }
        }

        if(!activeEdges){
            //newObjective += this->getHarmfulnessEmergency(ratio * tempW);//tempW la trong so cua canh khan cap
        }

        //newObjective += this->getHarmfulnessAvailable(ratio * tempW, tempTrace);

        newWeight += firstCost;

        newObjective = this->getHarmfulnessArrival(cur, ratio * (newWeight) + now);

        //newObjective += objective/*tempW;*/ /*now*/;

        if(activeEdges){
            tempW = timeForEmergencyMode;
        }
        else{
            tempW = ratio*tempW + timeForEmergencyMode;
            //newObjective += tempW*0.25;
        }


        if (newObjective < cur->ShortestPath[tempIndex]
             || newObjective < Constant::THRESHOLD
        ){ // Check if we can do better
            cur->ShortestPath[tempIndex] = newObjective; // Update new distance
            cur->traces[tempIndex] = trace; //tempTrace;
            //std::string content = vertices[tempIndex] + "_" + cur->id;
            std::string newTrace = trace + tempTrace + " " + std::to_string(tempW);
            if(tempIndex == 106){
                EV<<"Target here"<<endl;
            }
            cur->PQ.push(make_tuple(newObjective, newWeight, /*content,*/ tempIndex, newTrace));
        } // Update distance
    }//End of for
}

