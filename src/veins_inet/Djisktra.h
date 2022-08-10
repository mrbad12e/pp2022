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
#include <string.h>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "Constant.h"
#include "Graph.h"
#include "Crossing.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
// Author: Aakash Prabhu
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

typedef std::pair<std::string, double> Pair;
typedef std::tuple<double, std::string, int, std::string> Quad;

class ExponentialSmoothing{
public:

    ExponentialSmoothing(int num, int numIVertices){
        this->num = num;
        this->numIVertices = numIVertices;
        waitTime = (double *)malloc(num*sizeof(double));
        Qt = (double *)malloc(num*sizeof(double));
        Dt = (double *)malloc(num*sizeof(double));
        k = (int*)malloc(num*sizeof(int));
        raisedTime = (double *)malloc(num*sizeof(double));
        maxWeights = (double *)malloc(num*sizeof(double));
        timeOfPeaks = (double *)malloc(num*sizeof(double));
        fromPedestrians = (double *)malloc(num*sizeof(double));
        for(int i = 0; i < num; i++){
            raisedTime[i] = -1;
            maxWeights[i] = 0;
            timeOfPeaks[i] = -1;
            waitTime[i] = 0;
            fromPedestrians[i] = 0;
        }
        this->readCycicalData();
        //this->readCrossing();
    }

    double exponentialSmooth(int index, double oldPredict){
        double predictW = 0;
        double realData = waitTime[index];
        raisedTime[index] = simTime().dbl();
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

        if(predictW < 0)
            predictW = 0;
        if(maxWeights[index] < predictW){
            maxWeights[index] = predictW;
            timeOfPeaks[index] = raisedTime[index];
        }
        return predictW;
    }

    void printMaxWeights(std::vector<std::string> vertices){
        for(int i = 0; i < this->num; i++){
            if(maxWeights[i] > 0.001)
                EV<<vertices[i]<<" has max: "<<maxWeights[i]<<" at "<<timeOfPeaks[i]<<endl;
        }
    }

    double getDampingValue(int index, double predictW, std::string nameVertex){
        double timeOflastSmoothing = raisedTime[index];
        bool checkPedestrians = timeOflastSmoothing < 0 && index < numIVertices;
        if(!checkPedestrians){
            if(simTime().dbl() - timeOflastSmoothing > Constant::EXPIRED_TIME){
                if(waitTime[index] > 0){
                    return exponentialSmooth(index, predictW);
                }
                else{
                    waitTime[index] = 0;
                    checkPedestrians = index < numIVertices;
                }
            }
        }
        if(checkPedestrians){
            double x = fromPedestrians[index];
            if(x != x){
                fromPedestrians[index] = 0;
            }
            return fromPedestrians[index];
            //if(simTime().dbl() - lastGettingPedestrians > Constant::EXPIRED_TIME){
            //    lastGettingPedestrians = simTime().dbl();
            //    double area = 0; int numCrossings = 0;
                //int count = checkCrossing(nameVertex, &numCrossings, &area);
                //return this->getDispearTime(count, numCrossings, area);
            //}
        }
        return predictW;
    }

    void readCycicalData(){
        std::string line;
        std::ifstream MyReadFile("cyclicalData.txt");
        while(getline(MyReadFile, line)){
            allNoisyIntersections = allNoisyIntersections + "$" + line + "$";
        }
        MyReadFile.close();
    }

    double useCycicalData(double incommingTime, std::string name, double weightSmoothing){
        if(incommingTime > this->beginWalking && incommingTime < this->endWalking){
            if(allNoisyIntersections.find("$" + name + "$") != std::string::npos){
                return 0.5;
            }
        }
        return weightSmoothing;
    }


    double getDispearTime(int count, int numLocalCrossing, double area){
        if(count == 0 || numLocalCrossing == 0)
            return 0;
        double density = count / area;
        double velocity = abs(0.2*density*density - 1.1*density + 1.7);
        return Constant::LENGTH_CROSSING*count/(2*velocity);
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
    double* fromPedestrians;
    double* raisedTime;

private:
    int* k;
    double* waitTime;
    double* Qt;
    double* Dt;
    int beginWalking = 0;
    int endWalking = 400;
    std::string allNoisyIntersections = "";


    double* maxWeights;
    double* timeOfPeaks;
    int num;
    int numIVertices = 0;
    TraCICommandInterface* traci;

    std::vector<Crossing> crossings;
    double lastGettingPedestrians = 0;

};

class Djisktra {
public:
    Djisktra();
    virtual ~Djisktra();
    void initialize();
    void getListEdges(std::string weightEdges);
    void createAndAddEdge(//std::vector <Quad> adjList[],
            int u, double weightEdge, double weightVertex, std::string v, int indexOfV);
    //void generateAdj(std::vector<Quad> adjList[]);
    void DijkstrasAlgorithm(//std::vector<Quad> adjList[],
            int source, int target, std::string currLane);
    virtual void getItineraries(std::string itineraryFile);
    std::vector<double> weightVertices;
    std::vector<std::tuple<std::string, int, int, int>> itineraries;
    std::vector<std::string> traces;
    int findI_Vertex(std::string name, bool recursive);
    std::string getRoute(std::string trace, std::string currentLane, int currentVertex, int nextVertex, int exitVertex);
    ExponentialSmoothing* expSmoothing;
    std::vector<std::string> vertices;
    int findVertex(std::string name);
    std::string getFinalSegment(std::string trace);
    int numIVertices = 0;
    std::string getWeights(std::string route, AGV* cur);
    virtual std::string getJSONStation(std::string station);

private:
    void getListVertices(std::string iVertices, std::string bVertices);
    std::vector<std::pair<std::string, std::string>> supplyDisposal;

    std::vector<int> nextIndexOfBVertices;
    std::vector<std::string> nextNameOfBVertices;
    std::vector<Pair> edges;

    double timeForVeryNextVertex(std::string currLane, std::string veryNextVertex);
    void generateAdj();
    bool isAntidromic(std::string direction, std::string otherDirection);
    bool isValidTrace(std::string currLane, std::string trace);
    void getSupplyAndDisposalLocation(std::string fileName);
    std::vector<std::vector<Quad>> adjList;
    int numVertices = 0; // 323 vertices (107 i-vertices and 341 b-vertices)
    //std::string firstLanes = "$E0$E1$E226$E227$-E92$-E91$-E90$-E298$-E297$-E296$:J3$:J203$:J272$:J273$:J92$:J91$";

    //std::vector <bool> visitedVertex(numVertices, false);
    double *ShortestPath; //[numVertices]; // Have an array to store the shortest path

};

#endif /* VEINS_INET_DJISKTRA_H_ */
