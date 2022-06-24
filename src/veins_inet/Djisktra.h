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

#ifndef VEINS_INET_DJISKTRA_H_
#define VEINS_INET_DJISKTRA_H_
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

typedef pair<string, double> Pair;
typedef tuple<double, string, int, string> Quad;

class Djisktra {
public:
    Djisktra();
    virtual ~Djisktra();
    void getListVertices(std::string iVertices, std::string bVertices);
    void getListEdges(std::string weightEdges);
    void createAndAddEdge(std::vector <Quad> adjList[], int u, double weightEdge, double weightVertex, string v, int indexOfV);
    void generateAdj(std::vector <Quad> adjList[]);
    void DijkstrasAlgorithm(std::vector <Quad> adjList[], int source, int target);


private:
    std::vector<Quad> adjList;
    std::vector<std::string> vertices;
    std::vector<int> nextIndexOfBVertices;
    std::vector<std::string> nextNameOfBVertices;
    std::vector<Pair> edges;
    std::vector<double> weightVertices;
    std::vector<std::string> traces;
    std::string getRoute(std::string trace);
    int numVertices = 448; // 323 vertices (107 i-vertices and 341 b-vertices)
    int numIVertices = 107;
    //std::vector <bool> visitedVertex(numVertices, false);
    double ShortestPath[numVertices]; // Have an array to store the shortest path
};

#endif /* VEINS_INET_DJISKTRA_H_ */
