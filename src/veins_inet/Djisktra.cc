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
    // TODO Auto-generated constructor stub
    getListVertices("i-vertex.txt", "b-vertices.txt");
    getListEdges("weightEdges.txt");
    adjList.resize(numVertices);
    generateAdj(/*adjList*/);
    getListEdges("weightEdges.txt");//re-create edges
    getItineraries("itinerary.txt");
    ShortestPath = (double *)malloc(numVertices*sizeof(double));
    this->expSmoothing = new ExponentialSmoothing(numVertices);
    //this->expSmoothing->waitTime = (double *)malloc(numVertices*sizeof(double));
    for(int i = 0; i < numVertices; i++)
        ShortestPath[i] = 0;
}

Djisktra::~Djisktra() {
    // TODO Auto-generated destructor stub
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
        name = "-" + name;
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
        traces.push_back("");
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
        traces.push_back("");
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
        int source, int target){
  std::priority_queue<Quad, std::vector<Quad>, std::greater<Quad> > PQ; // Set up priority queue
  Quad info;
  std::string trace;
  double weight;
  double tempW;
  int tempIndex;
  std::string tempTrace;

  ShortestPath[source] = 0; // Set source distance to zero
  std::vector <bool> visitedVertex(numVertices, false);

  for (int i = 0; i < numVertices; i++)
    if (i != source)
      ShortestPath[i] = 100000; // Initialize everything else to +infinity

  PQ.push(make_tuple(0, vertices[source], source, "")); // Source has weight 0;

  while (!PQ.empty()){
    info = PQ.top(); // Use to get minimum weight
    PQ.pop(); // Pop before checking for cycles
    source = std::get<2>(info); // get the vertex
    if(source == target)
      break;
    weight = std::get<0>(info); // current distance
    trace = std::get<3>(info);


    if (visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    for (std::vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
      tempW = std::get<0>(*it);
      tempIndex = std::get<2>(*it);
      weightVertices[tempIndex] = this->expSmoothing->getDampingValue(tempIndex, weightVertices[tempIndex], vertices[tempIndex]);
      double newWeight = weight + tempW + weightVertices[tempIndex];
      if (newWeight < ShortestPath[tempIndex]){ // Check if we can do better
         tempTrace = std::get<3>(*it);
         ShortestPath[tempIndex] = newWeight; // Update new distance
         traces[tempIndex] = trace; //tempTrace;
         PQ.push(make_tuple(ShortestPath[tempIndex], vertices[tempIndex], tempIndex, trace + tempTrace)); // Push vertex and weight onto Priority Queue
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm

std::string Djisktra::getRoute(std::string trace, std::string currLane){
  std::string route = (currLane[0] == ':') ? "" : (currLane + " ");
  std::string temp = "";
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
  for(int i = 1; i < trace.length(); i++){
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
  return route;
}

void Djisktra::getItineraries(std::string itineraryFile){
    std::ifstream file(itineraryFile);
    std::string line;
    std::string nameRoute;
    std::string nameSrc, nameStation, nameDest;
    int source, station, dst;

    while (getline(file, line)) {
        std::stringstream ss(line);
        getline(ss, nameRoute, ' ');
        getline(ss, nameSrc, ' ');
        getline(ss, nameStation, ' ');
        getline(ss, nameDest, ' ');
        source = findI_Vertex(nameSrc, false);
        station = findI_Vertex(nameStation, false);
        dst = findI_Vertex(nameDest, true);
        itineraries.push_back(std::make_tuple(nameRoute, source, station, dst));
    }

}
