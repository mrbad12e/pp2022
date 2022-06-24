#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
// Author: Aakash Prabhu
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

using namespace std;

/* Uses Dijkstra's Algorithm to find the Shortest Path from an arbitrary vertex to all other vertices
 * Since the following version of Dijkstra's is implemented using a min-priority queue,
 * the time complexity of the algorithm is O(V + ElogV).
 */
typedef pair<string, double> Pair; 
typedef tuple<double, string, int, string> Quad; 
vector<string> vertices;
vector<int> nextIndexOfBVertices;
vector<string> nextNameOfBVertices;
vector<Pair> edges;
vector<double> weightVertices;
vector<string> traces;

void getListVertices(string iVertices, string bVertices) {
  ifstream file1(iVertices);
  string line;
  
  while (getline(file1, line)) {
    vertices.push_back(line);
    weightVertices.push_back(0);
  }
  file1.close();
  int endOfI_Vertices = vertices.size();
  ifstream file2(bVertices);
  string dst = "";
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
    for(int u = 0; u < endOfI_Vertices; u++){
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
  file2.close();
  //return vertices;
}

void getListEdges(string weightEdges){
  ifstream file(weightEdges);
  string line;
  string name, weight;
  double w;
  while (getline(file, line)) {
    stringstream ss(line);
    getline(ss, name, ' ');
    getline(ss, weight, ' ');
    w =std::stod(weight);
    edges.push_back(make_pair(name, w));
  }
  file.close();
}

// First = Weight of Edge & Second = Weight of Vertex, Third = name of Vertex & Third = index of Vertex
const int numVertices = 448; // 323 vertices (107 i-vertices and 341 b-vertices)
const int numIVertices = 107;
vector <bool> visitedVertex(numVertices, false);
double ShortestPath[numVertices]; // Have an array to store the shortest path
string Trajectories[numVertices]; //an array to store the trajectory

void createAndAddEdge(vector <Quad> adjList[], int u, double weightEdge, double weightVertex, string v, int indexOfV){
  weightVertices[u] = weightVertex;
  adjList[u].push_back(make_tuple(weightEdge, v, indexOfV, v));
} // createAndAddEdge(vector <int> List[], int u, int v, int weight)

void generateAdj(vector <Quad> adjList[]){
  vector<int> locations;
  string src = "";
  string dst = "";
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
          //if(u != i)
          {
            if(dst.compare(vertices[u]) == 0){
              indexOfDst = u;
              break;
            }
          }
        }
        locations.push_back(j);
        createAndAddEdge(adjList, i, edges[j].second, 0, edges[j].first, indexOfDst);
        createAndAddEdge(adjList, indexOfDst, 0, 0, 
                 nextNameOfBVertices[indexOfDst - numIVertices], nextIndexOfBVertices[indexOfDst - numIVertices]);
      }
    }
    for(int k = locations.size() - 1; k >= 0; k--){
      edges.erase(edges.begin() + locations[k]);
    }
    locations.clear();
  }
}
void DijkstrasAlgorithm(vector <Quad> adjList[], int source, int target){
  priority_queue<Quad, vector<Quad>, greater<Quad> > PQ; // Set up priority queue
  Quad info;
  string trace;
  double weight;
  double tempW;
  int tempIndex;
  string tempTrace;
  ShortestPath[source] = 0; // Set source distance to zero

  for (int i = 0; i < numVertices; i++)
    if (i != source)
      ShortestPath[i] = 100000; // Initialize everything else to +infinity

  PQ.push(make_tuple(0, vertices[source], source, "")); // Source has weight 0;

  while (!PQ.empty()){
    info = PQ.top(); // Use to get minimum weight
    PQ.pop(); // Pop before checking for cycles
    source = get<2>(info); // get the vertex
    if(source == target)
      break;
    weight = get<0>(info); // current distance
    trace = get<3>(info);
    

    if (visitedVertex.at(source)) // Check for cycle
      continue; // Already accounted for it, move on

    visitedVertex.at(source) = true; // Else, mark the vertex so that we won't have to visit it again

    for (vector<Quad>::iterator it = adjList[source].begin(); it != adjList[source].end(); it++){
      tempW = get<0>(*it);
      tempIndex = get<2>(*it);
      
      
      if ((weight + tempW + weightVertices[tempIndex]) < ShortestPath[tempIndex]){ // Check if we can do better
         tempTrace = get<3>(*it);
         ShortestPath[tempIndex] = weight + tempW + weightVertices[tempIndex]; // Update new distance
         traces[tempIndex] = trace; //tempTrace;
         
          PQ.push(make_tuple(ShortestPath[tempIndex], vertices[tempIndex], tempIndex, trace + tempTrace)); // Push vertex and weight onto Priority Queue
         
      } // Update distance
    }
  } // While Priority Queue is not empty
} // DijkstrasAlgorithm

string getRoute(string trace){
  string route = "";
  string temp = "";
  for(int i = 1; i < trace.length(); i++){
    //if(temp.length() == 0){
      if(trace[i] != '$' && trace[i] != '_'){
        temp = temp + trace[i];
      }
      else{
        
        if(temp[0] != ':' && route.find(temp + " ") == string::npos){
          route = route + temp + " ";
        }
        temp = "";
      }
    //}
  }
  return route;
}

// Driver function
int main (void){

  int source = 87;
  int target = 103;
  vector<Quad> adjList[numVertices]; // Create an array of vectors that contain pairs of adjacent vertex and weight
  getListVertices("i-vertex.txt", "b-vertices.txt");
  getListEdges("weightEdges.txt");
  generateAdj(adjList);
  
  
  DijkstrasAlgorithm(adjList, source, target);

  cout << "Shortest path from source vertex " << vertices[source] << ": ";
  //for (int i = numIVertices; i < numVertices; i++){
  //for (int i = 0; i < numIVertices; i++)
  int i = target;
  {
    cout <<"to "<<vertices[i]<<" "<< ShortestPath[i] <<" by "<<getRoute(traces[i])<< "\n";
  }
  cout << endl;

} // main()