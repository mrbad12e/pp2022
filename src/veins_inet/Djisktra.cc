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

#include "Djisktra.h"

Djisktra::Djisktra() {
    this->initialize();
    getItineraries("itinerary.txt");
}

Djisktra::~Djisktra() {
    // TODO Auto-generated destructor stub
}

void Djisktra::initialize(){
    getListVertices("i-vertex.txt", "b-vertices.txt");
    this->getSupplyAndDisposalLocation("SupplyDisposal.txt");
    getListEdges("weightEdges.txt");
    adjList.resize(numVertices);
    if(!this->wasGeneratedAdj){
        generateAdj(/*adjList*/);
        this->wasGeneratedAdj = true;
    }
    getListEdges("weightEdges.txt");//re-create edges

    this->expSmoothing = new ExponentialSmoothing(numVertices, numIVertices);
    //this->expSmoothing->waitTime = (double *)malloc(numVertices*sizeof(double));

}

int Djisktra::findVertex(std::string name){
    for(int i = 0; i < this->numIVertices; i++){
        if(vertices[i].compare(name) == 0){
            return i;
        }
    }
    std::string nameOfB_Vertex = "";
    for(int i = 0; i < edges.size(); i++){
        if(edges[i].first.find("$" + name + "$") != std::string::npos){
            for(int j = edges[i].first.length() - 2; j >= 0; j--){
                if(edges[i].first[j] != '$'){
                    nameOfB_Vertex = edges[i].first[j] + nameOfB_Vertex;
                }
                else{
                    break;
                }
            }
            break;
        }
    }
    for(int i = this->numIVertices; i < this->numVertices ; i++){
        if(vertices[i].compare(nameOfB_Vertex) == 0){
            return i;
        }
    }
    return -1;
}

int Djisktra::findI_Vertex(std::string name, bool recursive){
    std::string nameOfI_Vertex = "";
    for(int i = 0; i < edges.size(); i++){
        if(edges[i].first.find("$" + name + "$") != std::string::npos){
            for(int j = edges[i].first.length() - 2; j >= 0; j--){
                if(edges[i].first[j] != '_'){
                    nameOfI_Vertex = edges[i].first[j] + nameOfI_Vertex;
                }
                else{
                    break;
                }
            }
            break;
        }
    }
    for(int i = 0; i < numIVertices; i++){
        if(vertices[i].compare(nameOfI_Vertex) == 0){
            return i;
        }
    }
    if(recursive){
        if(name[0] != '-')
            name = "-" + name;
        else{
            name = name.substr(1);
        }
        return findI_Vertex(name, false);
    }
    return -1;
}
void Djisktra::getListVertices(std::string iVertices, std::string bVertices) {
    std::ifstream file1(iVertices);
    std::string line;
    numIVertices = 0;
    while (getline(file1, line)) {
        vertices.push_back(line);
        weightVertices.push_back(0);
        //traces.push_back("");
    }
    file1.close();

    numIVertices = vertices.size();
    std::ifstream file2(bVertices);
    std::string dst = "";
    while (getline(file2, line)) {
        dst = "";
        for(int k = line.length() - 1; k > 0; k--){
            if(line[k] != '_'){
                dst = line[k] + dst;
            }
            else{
                break;
            }
        }
        for(int u = 0; u < numIVertices; u++){
            if(dst.compare(vertices[u]) == 0){
                nextIndexOfBVertices.push_back(u);
                nextNameOfBVertices.push_back(dst);
                break;
            }
        }
        vertices.push_back(line);
        weightVertices.push_back(0);
        //traces.push_back("");
    }
    numVertices = vertices.size();
    file2.close();
}

void Djisktra::getListEdges(std::string weightEdges){
    std::ifstream file(weightEdges);
    std::string line;
    std::string name, weight;
    double w;
    while (getline(file, line)) {
        std::stringstream ss(line);
        getline(ss, name, ' ');
        getline(ss, weight, ' ');
        w =std::stod(weight) / Constant::MAX_SPEED;
        edges.push_back(std::make_pair(name, w));
    }
    file.close();
}

void Djisktra::createAndAddEdge(//std::vector<Quad> adjList[],
            int u, double weightEdge, double weightVertex, std::string v, int indexOfV){
    weightVertices[u] = weightVertex;
    adjList[u].push_back(make_tuple(weightEdge, v, indexOfV, v));
    //adjList[u].push_back(Quad(weightEdge, v, indexOfV, v));
} // createAndAddEdge

//void Djisktra::generateAdj(std::vector<Quad> adjList[]){
void Djisktra::generateAdj(){
    std::vector<int> locations;
    std::string src = "";
    std::string dst = "";
    int indexOfDst = -1;
    for(int i = 0; i < numIVertices; i++){
        src = "$" + vertices[i] + "$";
        for(int j = 0; j < edges.size(); j++){
            if(src.compare(edges[j].first.substr(0, src.length())) == 0){
                dst = "";
                //assign dst: name of the next vertex
                for(int k = edges[j].first.length() - 2; k > 0; k--){
                    if(edges[j].first[k] != '$'){
                        dst = edges[j].first[k] + dst;
                    }
                    else{
                        break;
                    }
                }
                indexOfDst = -1;
                for(int u = numIVertices; u < numVertices; u++){
                    if(dst.compare(vertices[u]) == 0){
                        indexOfDst = u;
                        break;
                    }
                }
                locations.push_back(j);
                createAndAddEdge(//adjList,
                        i, edges[j].second, 0, edges[j].first, indexOfDst);
                createAndAddEdge(//adjList,
                        indexOfDst, 0, 0,
                        nextNameOfBVertices[indexOfDst - numIVertices], nextIndexOfBVertices[indexOfDst - numIVertices]);
            }
        }
        for(int k = locations.size() - 1; k >= 0; k--){
            edges.erase(edges.begin() + locations[k]);
        }
        locations.clear();
    }
}

void Djisktra::DijkstrasAlgorithm(//std::vector <Quad> adjList[],
        int source, int target, std::string currLane, AGV* cur){
  std::priority_queue<Quad, std::vector<Quad>, std::greater<Quad> > PQ; // Set up priority queue
  Quad info; //(-1, "", -1, "");
  std::string trace;
  double weight;
  double tempW;
  int tempIndex;
  std::string tempTrace;

  //ShortestPath[source] = 0; // Set source distance to zero
  cur->init(numVertices);
  cur->ShortestPath[source] = 0;
  //for (int i = 0; i < numVertices; i++)
  //  if (i != source)
  //    ShortestPath[i] = 100000; // Initialize everything else to +infinity

  PQ.push(make_tuple(0, vertices[source], source, "")); // Source has weight 0;
  //PQ.push(Quad(0, vertices[source], source, ""));

  while (!PQ.empty()){
    info = PQ.top(); // Use to get minimum weight
    PQ.pop(); // Pop before checking for cycles
    source = /*info.source; */std::get<2>(info); // get the vertex
    if(source == target)
      //continue;
        break;
    weight = /*info.weight; */std::get<0>(info); // current distance
    trace = /*info.trace; */std::get<3>(info);


    if (cur->visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    cur->visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    for (std::vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
      tempW = /*(*it).weight; */std::get<0>(*it);
      tempTrace = /*(*it).trace;*/std::get<3>(*it);
      /*if(tempTrace.compare(":J8") == 0){
          EV<<"qqqqq"<<endl;
      }*/
      tempIndex = /*(*it).source; */std::get<2>(*it);
      if(!Constant::SHORTEST_PATH){
          weightVertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, weightVertices[tempIndex], vertices[tempIndex]);
      }

      double newWeight = 0; //weight + tempW + 40*weightVertices[tempIndex];
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

      if (newWeight < cur->ShortestPath[tempIndex]){ // Check if we can do better
         cur->ShortestPath[tempIndex] = newWeight; // Update new distance
         cur->traces[tempIndex] = trace; //tempTrace;
         PQ.push(make_tuple(cur->ShortestPath[tempIndex], vertices[tempIndex], tempIndex, trace + tempTrace)); // Push vertex and weight onto Priority Queue
         //PQ.push(Quad(cur->ShortestPath[tempIndex], vertices[tempIndex], tempIndex, trace + tempTrace)); // Push vertex and weight onto Priority Queue
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm

std::string Djisktra::getRoute(std::string trace, std::string currLane, int currentVertex, int nextVertex, int exitVertex){

    if(currLane.compare("E301") == 0
          && currentVertex == 45
      && nextVertex == 70
      && exitVertex == 44
          ){
        EV<<"DDDDDD"<<endl;
  }
  std::string route = (currLane[0] == ':') ? "" : (currLane + " ");
  std::string temp = "";
  std::string strCurrVertex = this->vertices[currentVertex];
  std::string strNextVertex = this->vertices[nextVertex];
  for(int i = 0; i < edges.size(); i++){
      if(edges[i].first.find("$" + currLane + "$") != std::string::npos){
          std::vector<std::string> list = split(edges[i].first, "$" + currLane + "$");
          std::string remaining = list[list.size() - 1];
          for(int j = 0; j < remaining.length(); j++){
              if(remaining[j] != '$' && remaining[j] != '_'){
                  temp = temp + remaining[j];
              }
              else{

                  if(temp[0] != ':' && route.find(temp + " ") == std::string::npos){
                    route = route + temp + " ";
                  }
                  temp = "";
              }
          }
          break;
      }
  }
  temp = "";
  int start = locateLast(route, trace);
  start = (start < 0) ? 1 : start;
  for(int i = start; i < trace.length(); i++){
    if(trace[i] != '$' && trace[i] != '_'){
        temp = temp + trace[i];
    }
    else{

        if(temp[0] != ':' && route.find(temp + " ") == std::string::npos){
          route = route + temp + " ";
        }
        temp = "";
    }
  }
  if(nextVertex == exitVertex)
      route = route + this->getFinalSegment(trace);

  return route;
}

void Djisktra::getItineraries(std::string itineraryFile){
    std::ifstream file(itineraryFile);
    std::string line;
    std::string nameRoute;
    std::string nameSrc, nameStation, nameDest, rubbish;
    int source, station, dst;

    while (getline(file, line)) {
        if(line[0] != '#'){
            std::stringstream ss(line);
            getline(ss, nameRoute, ' ');
            getline(ss, nameSrc, ' ');
            getline(ss, nameStation, ' ');
            getline(ss, rubbish, ' ');
            getline(ss, nameDest, ' ');
            source = findI_Vertex(nameSrc, false);
            station = findI_Vertex(nameStation, false);
            dst = findI_Vertex(nameDest, true);
            itineraries.push_back(std::make_tuple(nameRoute, source, station, dst));
        }
    }

}

void Djisktra::getSupplyAndDisposalLocation(std::string fileName){
    std::ifstream file(fileName);
    std::string line;
    std::string name, path;

    while (getline(file, line)) {
        std::stringstream ss(line);
        getline(ss, name, '$');
        getline(ss, path, '$');
        supplyDisposal.push_back(std::make_pair(name, path));
    }
    file.close();
}

bool Djisktra::isAntidromic(std::string direction, std::string otherDirection){
    //bool antidromic = false;
    std::vector<std::string> firstDirection = split(direction, "$");
    std::vector<std::string> secondDirection = split(otherDirection, "$");
    if(firstDirection.size() >= 2 && secondDirection.size() >= 2){
        if(firstDirection[firstDirection.size() - 1].compare(secondDirection[0]) == 0
        || secondDirection[secondDirection.size() - 1].compare(firstDirection[0]) == 0
        ){
            std::string nextOfDirection = //firstDirection[firstDirection.size() - 2];
                            getLane(firstDirection, true);
            std::string nextOfOtherDirection = //secondDirection[1];
                            getLane(secondDirection, false);
            if(nextOfDirection.length() > 0 && nextOfOtherDirection.length() > 0){
                if(nextOfDirection[0] == '-' && nextOfOtherDirection[0] != '-'){
                    nextOfOtherDirection = '-' + nextOfOtherDirection;
                }
                if(nextOfDirection[0] != '-' && nextOfOtherDirection[0] == '-'){
                    nextOfDirection = '-' + nextOfDirection;
                }
                if(nextOfDirection.compare(nextOfOtherDirection) == 0){
                    return true;
                }
            }
            nextOfDirection = getLane(firstDirection, false);
            nextOfOtherDirection = getLane(secondDirection, true);
            if(nextOfDirection.length() > 0 && nextOfOtherDirection.length() > 0){
                if(nextOfDirection[0] == '-' && nextOfOtherDirection[0] != '-'){
                    nextOfOtherDirection = '-' + nextOfOtherDirection;
                }
                if(nextOfDirection[0] != '-' && nextOfOtherDirection[0] == '-'){
                    nextOfDirection = '-' + nextOfDirection;
                }
                if(nextOfDirection.compare(nextOfOtherDirection) == 0){
                    return true;
                }
            }
        }
    }
    return false;
}

bool Djisktra::isValidTrace(std::string currLane, std::string trace){
    std::string otherLane = "";
    if(currLane[0] == '-'){
        otherLane = currLane.substr(1);
    }
    else{
        otherLane = "-" + currLane;
    }
    if(otherLane.length() > 0){
        return (trace.find("$" + otherLane + "$") == std::string::npos);
    }
    return true;
}

double Djisktra::timeForVeryNextVertex(std::string currLane, std::string veryNextVertex){
    double result = 0;
    int count = 0;
    for(int i = 0; i < this->edges.size(); i++ ){
        std::string path = this->edges[i].first;
        if(path.find("$" + currLane + "$") == 0
            && path.find(veryNextVertex + "$") != std::string::npos
            ){
            result += this->edges[i].second;
            count++;
        }
    }
    if(count > 1)
        result /= count;
    //result /= Constant::MAX_SPEED;
    return result;
}

std::string Djisktra::getJSONStation(std::string station){
    return "\"station\" : \"" + station + "\"";
}

std::string Djisktra::getWeights(std::string route, AGV* cur
        //double ratio, double now, std::string currLane
        ){
    //std::vector<std::string> cost;
    std::string cost = "";
    std::vector<std::string> v = split(route, " ");
    if(v.size() == 0)
        return "";
    double ratio = cur->ratio;
    double now = cur->now;
    std::string currLane = cur->itinerary->laneId;
    int index = -1;
    int prevIndex = -1;
    int count = 0;
    index = findI_Vertex(v[0], false);
    double firstCost = 0;
    //double now = simTime().dbl();
    if(index != -1){
        firstCost = timeForVeryNextVertex(currLane, vertices[index]);
        prevIndex = index;
        if(firstCost > 0){
            cost = cost + "\"" + vertices[index] + "_"
                + std::to_string(ratio * firstCost + now) + "\",";
        }
    }

    int min = v.size() > 6 ? 6 : v.size();
    for(int i = 1; count < min - 1 && i < v.size() - 1; i++){
        index = findI_Vertex(v[i], false);
        if(index != prevIndex && index != -1){
            cost = cost + "\"" + vertices[index] + "_" +
                    (std::to_string(now + ratio * (cur->ShortestPath[index] + firstCost))) + "\",";
            prevIndex = index;
            count++;

        }
    }
    if(v.size() > 2){
        index = findI_Vertex(v[v.size() - 1], true);
        if(index != prevIndex){
            //cost.push_back(std::to_string(rate * ShortestPath[index]));
            cost = cost + "\"" + vertices[index] + "_" +
                    (std::to_string(now + ratio * (cur->ShortestPath[index] + firstCost))) + "\",";
        }
    }
    if(cost.length() > 1)
        cost = cost.substr(0, cost.length() - 1);
    cost = "[" + cost + "]";
    return cost;
}


std::string Djisktra::getFinalSegment(std::string trace){
    int index = trace.length() > 10 ? (trace.length() - 10) : 0;
    std::string lastTrace = trace.substr(index);
    for(int i = 0; i < this->supplyDisposal.size(); i++){
        if(lastTrace.find("_" + this->supplyDisposal[i].first + "$") != std::string::npos){
            return (" " + this->supplyDisposal[i].second);
        }
    }
    /*double t = simTime().dbl();
    if(t > 2.35){
          EV<<"sdfsdfs";
      }*/
    return "";
}
