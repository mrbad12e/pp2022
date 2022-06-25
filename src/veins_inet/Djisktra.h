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
#include "Crossing.h"
#include "veins/modules/mobility/traci/TraCICommandInterface.h"
// Author: Aakash Prabhu
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

typedef std::pair<std::string, double> Pair;
typedef std::tuple<double, std::string, int, std::string> Quad;

class ExponentialSmoothing{
public:

    ExponentialSmoothing(int num){
        this->num = num;
        waitTime = (double *)malloc(num*sizeof(double));
        Qt = (double *)malloc(num*sizeof(double));
        Dt = (double *)malloc(num*sizeof(double));
        k = (int*)malloc(num*sizeof(int));
        raisedTime = (double *)malloc(num*sizeof(double));
        maxWeights = (double *)malloc(num*sizeof(double));
        timeOfPeaks = (double *)malloc(num*sizeof(double));
        for(int i = 0; i < num; i++){
            raisedTime[i] = -1;
            maxWeights[i] = 0;
            timeOfPeaks[i] = -1;
            waitTime[i] = 0;
        }
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
        bool checkPedestrians = raisedTime[index] < 0;
        if(!checkPedestrians){
            if(simTime().dbl() - raisedTime[index] > Constant::EXPIRED_TIME){
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

    void readCrossing(){
        std::string line;
        std::ifstream MyReadFile("crossing.txt");
        getline(MyReadFile, line);
        int numberOfCrossing =std::stoi(line);

        int k = 0;

        while (getline(MyReadFile, line)) {
            size_t pos;
            std::string token;
            Crossing tmp;

            for (int i = 0; i < 2; i++) {
                pos = line.find(" ");
                token = line.substr(0, pos);

                if (i == 0) tmp.id = token;
                if (i == 1) tmp.name = token; //std::atof(token.c_str());
                //if (i == 2) tmp.from = token;
                //if (i == 3) tmp.to = token;
                line.erase(0, pos + 1);

            }
            tmp.rec = new CustomRectangle(line);
            crossings.push_back(tmp);
            k++;
        }

        MyReadFile.close();
    }



    double checkCrossing(double predictW, std::string name){
        if(traci == NULL){
            if(Constant::activation != NULL)
                traci = Constant::activation->getCommandInterface();
        }
        if(traci != NULL){
            std::list<std::string> allPeople = traci->getPersonIds();
            double x, y;
            //for(int i = 0; i < crossings.size(); i++){
            for (auto elem: allPeople) {
                std::string personId = elem;
                Coord peoplePosition = traci->getPersonPosition(personId);
                std::pair<double,double> coordTraCI = traci->getTraCIXY(peoplePosition);
                x = coordTraCI.first;
                y = coordTraCI.second;
                /*for(int i = 0; i < crossings->size(); i++){
                    /*if (((Crossing)crossings[i]).rec->checkInside(x, y)) {
                        break;
                    }
                    else if (crossings[i].rec->checkAround(x, y)){
                            break;
                    }*/

                //}*/
                            }
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
    double* maxWeights;
    double* timeOfPeaks;
    int num;
    TraCICommandInterface* traci;

    std::vector<Crossing> crossings;

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
    std::string getRoute(std::string trace, std::string currentLane);
    ExponentialSmoothing* expSmoothing;
    std::vector<std::string> vertices;
    int findVertex(std::string name);
private:
    std::vector<std::vector<Quad>> adjList;


    std::vector<int> nextIndexOfBVertices;
    std::vector<std::string> nextNameOfBVertices;
    std::vector<Pair> edges;



    int numVertices = 0; // 323 vertices (107 i-vertices and 341 b-vertices)
    int numIVertices = 0;

    //std::vector <bool> visitedVertex(numVertices, false);
    double *ShortestPath; //[numVertices]; // Have an array to store the shortest path
};

#endif /* VEINS_INET_DJISKTRA_H_ */
