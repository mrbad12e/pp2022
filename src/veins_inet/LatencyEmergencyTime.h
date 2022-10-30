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

#ifndef VEINS_INET_LATENCYEMERGENCYTIME_H_
#define VEINS_INET_LATENCYEMERGENCYTIME_H_
#include "DecisionDijkstra.h"

class LatencyEmergencyTime : public DecisionDijkstra{
public:
    LatencyEmergencyTime();
    virtual ~LatencyEmergencyTime();
    double getHarmfulnessArrival(AGV* cur, double time) override;
    std::string getName() override;
    void planOut(int source, int target, std::string currLane, AGV* cur) override;
    void checkActiveEdges(double firstCost, Quad* info, bool activeEdges) override;
private:
    int getBestChoise();
};

#endif /* VEINS_INET_LATENCYEMERGENCYTIME_H_ */
