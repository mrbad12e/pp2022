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
//    Djisktra::base();
    getListVertices("i-vertex.txt", "b-vertices.txt");
    this->getSupplyAndDisposalLocation("SupplyDisposal.txt");
    getListEdges("weightEdges.txt");
    adjList.resize(numVertices);
    generateAdj(/*adjList*/);
    getListEdges("weightEdges.txt");//re-create edges
    getItineraries("itinerary.txt");
    ShortestPath = (double *)malloc(numVertices*sizeof(double));
    this->expSmoothing = new ExponentialSmoothing(numVertices, numIVertices);
    //this->expSmoothing->waitTime = (double *)malloc(numVertices*sizeof(double));
    for(int i = 0; i < numVertices; i++)
        ShortestPath[i] = 0;
}

/*HarmfulnessDijkstra::~HarmfulnessDijkstra() {
}*/

void HarmfulnessDijkstra::getItineraries(std::string itineraryFile){
    std::ifstream file(itineraryFile);
    std::string line;
    std::string nameRoute;
    std::string nameSrc, nameStation, nameDest;
    std::string period, bestTime, amplitude;
    int source, station, dst;

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
            station = findI_Vertex(nameStation, false);
            //Station* station = new Station();

            //allSs[station];
            dst = findI_Vertex(nameDest, true);
            itineraries.push_back(std::make_tuple(nameRoute, source, station, dst));
        }
    }

}
