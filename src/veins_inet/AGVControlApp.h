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

#ifndef VEINS_INET_AGVCONTROLAPP_H_
#define VEINS_INET_AGVCONTROLAPP_H_

#include "VehicleControlApp.h"
#pragma once

#include "veins/veins.h"

#include "veins/modules/application/ieee80211p/DemoBaseApplLayer.h"
#include "veins/modules/application/traci/TraCIDemo11p.h"
#include <float.h>
#include <map>
#include <string>
#include "Graph.h"

using namespace omnetpp;

namespace veins {

typedef std::map<std::string, double> Dictionary;
//typedef std::map<std::string, std::string> RouteDictionary;

//extern std::map<std::string, std::string> routeDict;

class AGVControlApp: public veins::TraCIDemo11p {
public:
    void initialize(int stage) override;
    void finish() override;
    cMessage* sendBeacon;

protected:
    void onBSM(DemoSafetyMessage* bsm) override;
    void onWSM(BaseFrame1609_4* wsm) override;
    void onWSA(DemoServiceAdvertisment* wsa) override;

    void handleSelfMsg(cMessage* msg) override;
    void handlePositionUpdate(cObject* obj) override;
    void handleLowerMsg(cMessage* msg) override;
private:
    std::string checkForPausing();
    void addExpectedTime(std::string str);
    void exponentialSmooth(std::string key, double realTime);
    bool sentFirstMessage = false;
    bool receivedReRouting = false;
    double travellingTime = 0;
    long waitingIntervals = 0;
    //attributes for adaptive responsive rate
    double lambda = 0;
    int k = 0;
    double Qt = 0;
    double Dt = 0;
    double predictRatio = 1.1;

    std::string originalRoute = "";
    std::string prevRoute = "";
    std::string expectedRoute = "";
    std::string content = "";
    double velocityBeforeHalt = -1;
    double pausingTime = DBL_MAX;
    Dictionary dict;
    double APE = 0;
    int T = 0;
    double harmfulness = 0;
    Station* station;
    int indexInRoute = -1;
    int idOfMessage = 0;
    int idDebug;
    void runAfterStuck();
    void saveBeginningOfStuck(std::string junc);
    Dictionary stuckAtJunc;
    //static RouteDictionary routeDict;
    void getIndexInFlow(std::string idOfAGV, std::string routeId);
    static std::map<std::string, std::string> routeDict;
    std::vector<std::string> passedEdges;
    //std::string station = "";
};
}
#endif /* VEINS_INET_AGVCONTROLAPP_H_ */
