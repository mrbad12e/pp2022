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

#include "ArrivalDijkstra.h"

ArrivalDijkstra::ArrivalDijkstra() {
    // TODO Auto-generated constructor stub
    this->initialize();
    getItineraries("itinerary.txt");
}

/*HarmfulnessDijkstra::~HarmfulnessDijkstra() {
}*/
std::string ArrivalDijkstra::getJSONStation(std::string station){
    //std::string strStation = "";
    std::map<std::string, Station*>::iterator it;
    it = allSs.find(station);
    if(it == allSs.end())
        return Djisktra::getJSONStation(station);
    Station* s = allSs[station];
    return s->toJSON();
}

void ArrivalDijkstra::getItineraries(std::string itineraryFile){
    std::ifstream file(itineraryFile);
    std::string line;
    std::string nameRoute;
    std::string nameSrc, nameStation, nameDest, nameJunc;
    std::string period, bestTime, amplitude;
    int source, indexOfStation, dst;
    if(this->itineraries.size() > 0){
        this->itineraries.clear();
    }

    while (getline(file, line)) {
        if(line[0] != '#'){
            std::stringstream ss(line);
            getline(ss, nameRoute, ' ');
            getline(ss, nameSrc, ' ');
            getline(ss, nameStation, ' ');
            getline(ss, nameJunc, ' ');
            getline(ss, nameDest, ' ');
            getline(ss, period, ' ');
            getline(ss, bestTime, ' ');
            getline(ss, amplitude, ' ');
            source = findI_Vertex(nameSrc, false);
            indexOfStation = findI_Vertex(nameStation, false);
            Station* station = new Station(vertices[indexOfStation], bestTime, amplitude, period);

            allSs[vertices[indexOfStation]] = station;
            dst = findI_Vertex(nameDest, true);
            itineraries.push_back(std::make_tuple(nameRoute, source, indexOfStation, dst));
        }
    }
    file.close();

}

double ArrivalDijkstra::firstValue(std::string currLane, std::string veryNextVertex){
    int count = 0;
    double result = 0;
    for(int i = 0; i < this->edges.size(); i++ ){
        std::string path = this->edges[i].first;
        if(path.find("$" + currLane + "$") != std::string::npos
           && path.find(veryNextVertex + "$") != std::string::npos
            ){
            result += this->edges[i].second;
            count++;
        }
    }
    if(count > 1)
       result /= count;
    return result;
}

void ArrivalDijkstra::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
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
  cur->ShortestPath[source] = ratio * firstCost + now;
  //std::vector <bool> visitedVertex(numVertices, false);

  /*for (int i = 0; i < numVertices; i++)
    if (i != source)
      ShortestPath[i] = 100000; // Initialize everything else to +infinity
  */
  int x = cur->PQ.size();

  cur->PQ.push(std::make_tuple(cur->ShortestPath[source], 0, source, "")); // Source has weight cur->ShortestPath[source];
  if(source == 26
          && cur->ShortestPath[source] > 188.3
          && cur->ShortestPath[source] < 188.4){
      EV<<"ertterert"<<cur->id<<" "<<cur->now<<endl;
  }
  //cur->PQ.push(Quad(cur->ShortestPath[source], vertices[source] + "_" + cur->id, source, "")); // Source has weight cur->ShortestPath[source];

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
    //source = info.source;
    if(source == target){
      //continue;
        while (!cur->PQ.empty())
            cur->PQ.pop();
        break;
    }
    objective = //info.weight;//
            std::get<0>(info); // current distance
    weight = std::get<1>(info);
    trace = //info.trace;//
            std::get<3>(info);


    if (cur->visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    cur->visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    for (std::vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
      tempW = /*(*it).weight; */std::get<0>(*it);
      //tempW = std::get<1>(*it);
      tempTrace = /*(*it).trace; */std::get<3>(*it);
      tempIndex = /*(*it).source; */ std::get<2>(*it);
      if(!Constant::SHORTEST_PATH){
          weightVertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, weightVertices[tempIndex], vertices[tempIndex]);
      }

      double newWeight = 0, newObjective = 0; //weight + tempW + 40*weightVertices[tempIndex];
      /*if(trace.find("$E9$") != std::string::npos
            && tempTrace.find("$-E9$") != std::string::npos
              ){
          EV<<"fdfsf";
      }*/
      if(!isValidTrace(currLane, tempTrace)){
          continue;
      }
      if(isAntidromic(trace, tempTrace)){
          continue;
      }
      newWeight = weight + tempW;
      if(!Constant::SHORTEST_PATH){
          double weightSmoothing = weightVertices[tempIndex];
          if(weightSmoothing < 0.1 && tempIndex < this->numIVertices){
              newWeight += 100*(this->expSmoothing->useCycicalData(newWeight, vertices[tempIndex], weightSmoothing));
          }
          else{
              newWeight += 100*weightSmoothing;
          }
      }
      newWeight += firstCost;
      newObjective = ratio * (newWeight) + objective/*tempW;*/ /*now*/;
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

         /*if(newWeight > 188.3 && newWeight < 188.4
                 //&& cur->count < 20
                 && cur->id.compare("142") == 0){
             EV<<"sdfsdfsfsdfs"<<endl;
         }*/
         std::string content = vertices[tempIndex] + "_" + cur->id;
         std::string newTrace = trace + tempTrace;
         cur->PQ.push(make_tuple(newObjective, newWeight, /*content,*/ tempIndex, newTrace));
         /*cur->PQ.push(Quad(cur->ShortestPath[tempIndex],
                 vertices[tempIndex] + "_" + cur->id,
                 tempIndex, trace + tempTrace));*/
         // Push vertex and weight onto Priority Queue
         x = cur->PQ.size();
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm
