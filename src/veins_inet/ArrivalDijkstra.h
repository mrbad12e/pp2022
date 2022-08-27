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

#ifndef VEINS_INET_ARRIVALDIJKSTRA_H_
#define VEINS_INET_ARRIVALDIJKSTRA_H_

#include "Djisktra.h"

typedef std::map<std::string, Station*> AllStations;

class ArrivalDijkstra: public Djisktra {
public:
    ArrivalDijkstra();
    void getItineraries(std::string itineraryFile) override;
    std::string getJSONStation(std::string station) override;
    double firstValue(std::string currLane, std::string veryNextVertex);
    void DijkstrasAlgorithm(int source, int target, std::string currLane, AGV* cur) override;
    AllStations allSs;
private:
};

#endif /* VEINS_INET_ARRIVALDIJKSTRA_H_ */
