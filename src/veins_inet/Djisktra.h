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
#include <string>
#include <tuple>
#include <stdio.h>
#include <stdlib.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "Constant.h"
#include "Graph.h"
#include "Crossing.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
// Author: Aakash Prabhu
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

using boost::property_tree::ptree;
using namespace omnetpp;

enum STATE_OF_REQUEST{
    WAITING_FOR_PROCESSING,
    BEING_PROCESSED,
    FINISHED
};

typedef std::tuple<int, int, std::string, double, STATE_OF_REQUEST, std::string> Request;

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
        }
        return predictW;
    }

    /*double getDampingAsEmergence(int index, double predictW, std::string nameVertex){
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
        }
        return predictW;
    }*/

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


class AdaptiveSystem
{
public:
    struct Edge
    {
        int edgeStart;
        int edgeEnd;
        double weightEdge;
        double weightSrc;
        double weightDst;
        long int id;
        std::string src;
        std::string dst;
        bool isBVertex;
        bool operator<(const Edge& rhs) const
        {
            return id < rhs.id;
        }

        bool operator>(const Edge& rhs) const
        {
            return id > rhs.id;
        }

        bool operator==(const Edge& rhs) const
        {
            return id == rhs.id;
        }

        Edge(){
            edgeStart = edgeEnd = weightEdge = id = 0;
        }
    };

    AdaptiveSystem(){

    }
    virtual ~AdaptiveSystem(){

    }
    virtual std::vector<int> path(int src, int dst){
        std::vector<int> v;
        return v;
    }
    virtual void insertEdge(int src, int dst, double weight) noexcept(false){
        AdaptiveSystem::Edge edge;
        edge.edgeStart = src;
        edge.edgeEnd = dst;
        edge.weightEdge = weight;
        edge.id = ++ Constant::edgeIdCnt;
        adaptiveEdges.push_back(edge);
    }

    //virtual void clear() = 0;
    bool isWorking()//OpenMP threads are running or not
    {
        bool isRunning = false;
        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
            STATE_OF_REQUEST state = std::get<4>(*it);
            if(state == BEING_PROCESSED){
                isRunning = true;
                break;
            }
        }
        return isRunning;
    }
    bool isFullReqs() //checking the number of reqs exceeds the maximum value or not
    {
        int count = 0;
        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
           STATE_OF_REQUEST state = std::get<4>(*it);
           if(state == WAITING_FOR_PROCESSING
           ){
               count++;
           }
        }
        return (count > Constant::MAX_REQUESTS);
    }
    virtual bool insertRequest(int source, int dst, std::string id){
        if(!isWorking()){
            if(isFullReqs()){
                return false;
            }
            double t = simTime().dbl();
            this->removeExpiredRequests(NULL);
            int i = 0;

            std::vector<int>* identicalReqs = new std::vector<int>();
            bool needRemoveIdentical = false;

            for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
                int theSource = std::get<0>(*it);
                int theDestination = std::get<1>(*it);
                std::string ids = std::get<2>(*it);
                if(theSource == source && theDestination == dst){
                    if(ids.find("$" + id + "$") == std::string::npos){
                        std::get<2>(*it) = ids + "$" + id + "$";
                        return true;
                    }
                    else{
                        return false;
                    }
                }
                else if(ids.find("$" + id + "$") != std::string::npos && theDestination == dst){
                    if(ids.compare("$" + id + "$") == 0){
                        identicalReqs->push_back(i);
                        needRemoveIdentical = true;
                    }
                    else{
                        std::string replacedStr = "$" + id + "$";
                        int start = ids.find(replacedStr);
                        std::string newIds = ids.replace(start, replacedStr.length(), "");
                        std::get<2>(*it) = newIds;
                    }
                }
                i++;
            }//end of for

            if(needRemoveIdentical)
                this->removeExpiredRequests(identicalReqs);
            allRequests.push_back(
                    std::make_tuple(source, dst, "$" + id + "$", t, WAITING_FOR_PROCESSING, ""));
            return true;

        }
        return false;
    }
    virtual bool removeExpiredRequests(std::vector<int>* expiredRequests){
        bool selfSearching = expiredRequests == NULL;
        if(expiredRequests != NULL){
            selfSearching = expiredRequests->size() == 0;
        }
        std::vector<int> *tooOldOnes ;
        bool inIncrementOrder = true;
        if(selfSearching){
            tooOldOnes = new std::vector<int>();
            int i = 0;
            double t = simTime().dbl();
            for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
                STATE_OF_REQUEST state = std::get<4>(*it);
                double createdTime = std::get<3>(*it);

                if(state != BEING_PROCESSED && (t - createdTime > Constant::DELAY)){
                    tooOldOnes->push_back(i);
                }
                i++;
            }
        }
        else{
            tooOldOnes = expiredRequests;
            int prev = -1;
            for(auto it = tooOldOnes->begin(); it != tooOldOnes->end(); it++){
                if(prev > (*it)){
                    inIncrementOrder = false;
                    break;
                }
                else{
                    prev = (*it);
                }
            }
        }
        int index = 0;
        int numberOfRemoved = 0;
        if(inIncrementOrder){
            std::reverse(tooOldOnes->begin(), tooOldOnes->end());
        }
        for(auto it = tooOldOnes->begin(); it != tooOldOnes->end(); it++){
            index = *it;
            if(index >= 0 && index < allRequests.size()){
                allRequests.erase(allRequests.begin() + index);
                numberOfRemoved++;
            }
        }
        return (numberOfRemoved != 0);
    }
    virtual bool canExecuteReqs(){
        int waiting = 0;
        double t = simTime().dbl();
        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
            STATE_OF_REQUEST state = std::get<4>(*it);
            double createdTime = std::get<3>(*it);
            if(state == BEING_PROCESSED){
                return false;
            }
            else if(state == WAITING_FOR_PROCESSING && (t - createdTime < Constant::DELAY)){
                waiting++;
            }
        }
        return (waiting >= 1);
    }
    virtual bool hasIdenticalReq(){
        int i = 0;
        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
            i++;
            int theSource = std::get<0>(*it);
            int theDestination = std::get<1>(*it);
            for(std::vector<Request>::iterator ptr = allRequests.begin() + i; ptr != allRequests.end(); ptr++){
                int src = std::get<0>(*ptr);
                int dst = std::get<1>(*ptr);
                if(src == theSource && dst == theDestination){
                    return true;
                }
            }
        }
        return false;
    }
    virtual void initAdaptiveEdges(std::vector<std::vector<Quad>> adjList, std::vector<std::string> vertices, std::vector<double> timeWeightVertices, int* countEdges)//prepare for parallelization
    {
        double tempW;
        int tempIndex;
        std::string tempTrace;
        std::string multiFiles = "";

        int numVertices = adjList.size();

        for(int i = 0; i < numVertices; i++){
          for (std::vector<Quad>::iterator it = adjList[i].begin(); it != adjList[i].end(); it++){
              AdaptiveSystem::Edge edge;
              tempW = std::get<0>(*it);
              if(tempW == 0){
                  tempW = Constant::LENGTH_OF_B_VERTEX;
                  edge.isBVertex = true;
              }
              else{
                  tempW -= Constant::LENGTH_OF_B_VERTEX;
                  edge.isBVertex = false;
              }
              tempTrace = std::get<3>(*it);
              tempIndex = std::get<2>(*it);
              edge.edgeStart = i;
              edge.weightSrc = std::round(timeWeightVertices[i]*100.0)/100.0;
              edge.edgeEnd = tempIndex;
              edge.weightDst = std::round(timeWeightVertices[tempIndex]*100.0)/100.0;
              edge.weightEdge = std::round(tempW*100.0)/100.0;
              edge.src = vertices[i];
              edge.dst = tempTrace;
              EV<<vertices[tempIndex]<<endl;
              edge.id = (*countEdges)++;
              multiFiles.append("input4ACO/" + std::to_string(i) + "_" +
                               std::to_string(tempIndex) + "_" +
                               twoDecimalString(edge.weightSrc) + "_" +
                               twoDecimalString(edge.weightDst) + "_" +
                               twoDecimalString(edge.weightEdge) + ".txt ");
              adaptiveEdges.push_back(edge);
          }
       }
       multiFiles = "touch " + multiFiles;
       system("rm input4ACO/*.txt");
       system(multiFiles.c_str());

    }


    virtual void updateWeights(std::vector<double> timeWeightVertices){
        int indexSrc, indexDst;
        //double weightSrc, weightDst;
        for(int i = 0; i < adaptiveEdges.size(); i++){
            indexSrc = adaptiveEdges[i].edgeStart;
            indexDst = adaptiveEdges[i].edgeEnd;
            adaptiveEdges[i].weightSrc = timeWeightVertices[indexSrc];
            adaptiveEdges[i].weightDst = timeWeightVertices[indexDst];
        }
    }
    std::vector<Request> allRequests;

protected:
    //virtual void initTopo(const std::string&);
    std::vector<Edge> adaptiveEdges;


};



class Djisktra : public AdaptiveSystem{
public:
    Djisktra();
    virtual ~Djisktra();
    void initialize();
    void getListEdges(std::string weightEdges);
    void createAndAddEdge(//std::vector <Quad> adjList[],
            int u, double weightEdge, double weightVertex, std::string v, int indexOfV);
    //void generateAdj(std::vector<Quad> adjList[]);
    virtual void planOut(//std::vector<Quad> adjList[],
            int source, int target, std::string currLane, AGV* cur);
    virtual void getItineraries(std::string itineraryFile);
    virtual std::string getName();

    std::vector<std::tuple<std::string, int, int, int>> itineraries;

    int findI_Vertex(std::string name, bool recursive);
    virtual std::string getRoute(std::string trace, std::string currentLane, int currentVertex, int nextVertex, int exitVertex);
    ExponentialSmoothing* expSmoothing;
    std::vector<std::string> vertices;
    std::vector<std::vector<Quad>> adjList;
    std::vector<double> timeWeightVertices;
    int findVertex(std::string name);
    std::string getFinalSegment(std::string trace);
    int numIVertices = 0;
    std::string getWeights(std::string route, AGV* cur);
    virtual std::string getJSONStation(std::string station);
    double timeForVeryNextVertex(std::string currLane, std::string veryNextVertex);
    int numVertices = 0; // 323 vertices (107 i-vertices and 341 b-vertices)
    bool isAntidromic(std::string direction, std::string otherDirection);
    virtual bool isValidTrace(std::string currLane, std::string trace);
    std::vector<Pair> edges;


private:
    //std::vector<std::string> traces;
    //double *ShortestPath; //[numVertices]; // Have an array to store the shortest path
    void getListVertices(std::string iVertices, std::string bVertices);
    std::vector<std::pair<std::string, std::string>> supplyDisposal;


    bool wasGeneratedAdj = false;

    void getSupplyAndDisposalLocation(std::string fileName);
    virtual void generateAdj();
    std::vector<int> nextIndexOfBVertices;
    std::vector<std::string> nextNameOfBVertices;

    //std::string firstLanes = "$E0$E1$E226$E227$-E92$-E91$-E90$-E298$-E297$-E296$:J3$:J203$:J272$:J273$:J92$:J91$";

    //std::vector <bool> visitedVertex(numVertices, false);


};

#endif /* VEINS_INET_DJISKTRA_H_ */
