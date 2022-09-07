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

#ifndef VEINS_INET_DECISIONDIJKSTRA_H_
#define VEINS_INET_DECISIONDIJKSTRA_H_

#include "HarmfulnessDijkstra.h"

class DecisionDijkstra: public HarmfulnessDijkstra {
public:
    DecisionDijkstra();
    virtual ~DecisionDijkstra();
    void DijkstrasAlgorithm(int source, int target, std::string currLane, AGV* cur) override;
    bool isValidTrace(std::string currLane, std::string trace) override;
    std::string getName() override;
    virtual void checkActiveEdges(double firstCost, Quad* info, bool activeEdges);
    //void generateAdj() override;
    AGV* cur;
    std::string currLane;
    std::vector<std::vector<Quad>> emergencyAdjList;
    std::vector<double> timeW_E_Vertices;
private:
    void generateEmergencyEdges();
    void generateEmergencyVertices();
    std::vector<std::string> emergencyVertices;
    double getHarmfulnessEmergency(double time);
    double getHarmfulnessAvailable(double time, std::string tempTrace);
    void getBeneficialAndNeutral();
    std::string beneficialLanes;
    std::string neutralLanes;


};

#endif /* VEINS_INET_DECISIONDIJKSTRA_H_ */
