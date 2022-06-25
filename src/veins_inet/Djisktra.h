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
#include "Constant.h"
// Author: Aakash Prabhu
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

typedef std::pair<std::string, double> Pair;
typedef std::tuple<double, std::string, int, std::string> Quad;

class ExponentialSmoothing{
public:

    ExponentialSmoothing(int num){
        waitTime = (double *)malloc(num*sizeof(double));
        Qt = (double *)malloc(num*sizeof(double));
        Dt = (double *)malloc(num*sizeof(double));
        k = (int*)malloc(num*sizeof(int));
        raisedTime = (double *)malloc(num*sizeof(double));
        for(int i = 0; i < num; i++){
            raisedTime[i] = -1;
        }
    }

    double exponentialSmooth(int index, double oldPredict){
        double predictW = 0;
        double realData = waitTime[index];
        if(k[index] == 0){
            predictW = realData;
            Qt[index] = 0;
            Dt[index] = 0;
            k[index] = 1;
        }
        else{
            double error = realData - oldPredict;
            Qt[index] = Constant::GAMMA * error - (1 - Constant::GAMMA) * Qt[index];
            Dt[index] = Constant::GAMMA * abs(error)
                            - (1 - Constant::GAMMA) * Dt[index];
            double lambda = abs(Qt[index] / Dt[index]);
            predictW = lambda * realData + (1 - lambda) * oldPredict;
        }
        raisedTime[index] = simTime().dbl();
        return predictW;
    }

    double getDampingValue(int index, double predictW){
        bool checkPedestrians = raisedTime[index] < 0;
        if(!checkPedestrians){
            if(simTime.dbl() - raisedTime[index] > Constant::EXPIRED_TIME){
                if(waitTime[index] > 0){
                    return exponentialSmooth(index, predictW);
                }
                else{
                    waitTime[index] = 0;
                    checkPedestrians = true;
                }
            }
        }
        if(checkPedestrians){

        }
        return predictW;
    }
    double getWait(int index){
        //return getDampingValue(index, waitTime[index]);
        return waitTime[index];
    }
    void addWait(int index, double x){
        waitTime[index] += x;
        if(waitTime[index] < 0)
            waitTime[index] = 0;
    }
private:
    int* k;
    double* waitTime;
    double* Qt;
    double* Dt;
    double* raisedTime;
};

class Djisktra {
public:
    Djisktra();
    virtual ~Djisktra();
    void getListVertices(std::string iVertices, std::string bVertices);
    void getListEdges(std::string weightEdges);
    void createAndAddEdge(//std::vector <Quad> adjList[],
            int u, double weightEdge, double weightVertex, std::string v, int indexOfV);
    //void generateAdj(std::vector<Quad> adjList[]);
    void generateAdj();
    void DijkstrasAlgorithm(//std::vector<Quad> adjList[],
            int source, int target);
    void getItineraries(std::string itineraryFile);
    std::vector<double> weightVertices;
    std::vector<std::tuple<std::string, int, int, int>> itineraries;
    std::vector<std::string> traces;
    int findI_Vertex(std::string name, bool recursive);
    std::string getRoute(std::string trace);
    ExponentialSmoothing* expSmoothing;

private:
    std::vector<std::vector<Quad>> adjList;
    std::vector<std::string> vertices;

    std::vector<int> nextIndexOfBVertices;
    std::vector<std::string> nextNameOfBVertices;
    std::vector<Pair> edges;



    int numVertices = 0; // 323 vertices (107 i-vertices and 341 b-vertices)
    int numIVertices = 0;

    //std::vector <bool> visitedVertex(numVertices, false);
    double *ShortestPath; //[numVertices]; // Have an array to store the shortest path
};

#endif /* VEINS_INET_DJISKTRA_H_ */
