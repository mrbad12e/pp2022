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

#include "HarmfulnessDijkstra.h"

HarmfulnessDijkstra::HarmfulnessDijkstra() {
    // TODO Auto-generated constructor stub
    this->initialize();
    getItineraries("itinerary.txt");
}

/*HarmfulnessDijkstra::~HarmfulnessDijkstra() {
}*/
std::string HarmfulnessDijkstra::getJSONStation(std::string station){
    //std::string strStation = "";
    std::map<std::string, Station*>::iterator it;
    it = allSs.find(station);
    if(it == allSs.end())
        return Djisktra::getJSONStation(station);
    Station* s = allSs[station];
    return s->toJSON();
}

void HarmfulnessDijkstra::getItineraries(std::string itineraryFile){
    std::ifstream file(itineraryFile);
    std::string line;
    std::string nameRoute;
    std::string nameSrc, nameStation, nameDest;
    std::string period, bestTime, amplitude;
    int source, indexOfStation, dst;

    while (getline(file, line)) {
        if(line[0] != '#'){
            std::stringstream ss(line);
            getline(ss, nameRoute, ' ');
            getline(ss, nameSrc, ' ');
            getline(ss, nameStation, ' ');
            getline(ss, nameDest, ' ');
            getline(ss, period, ' ');
            getline(ss, bestTime, ' ');
            getline(ss, amplitude, ' ');
            source = findI_Vertex(nameSrc, false);
            indexOfStation = findI_Vertex(nameStation, false);
            Station* station = new Station(nameStation, bestTime, amplitude, period);

            allSs[nameStation] = station;
            dst = findI_Vertex(nameDest, true);
            itineraries.push_back(std::make_tuple(nameRoute, source, indexOfStation, dst));
        }
    }

}
