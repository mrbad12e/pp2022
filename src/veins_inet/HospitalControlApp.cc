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
// along with this program. If not, see http://www.gnu.org/licenses/.
// 

#include "HospitalControlApp.h"
#include <stdio.h>
#include <iostream>
#include "Zone.h"
#include "Graph.h"
#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;

//#include "HashAPI.cpp"
//#include "UnitTest.cpp"
//#include "ReadFile.cpp"



using namespace veins;

Register_Class(HospitalControlApp);

void HospitalControlApp::initialize(int stage)
{
    TraCIDemoRSU11p::initialize(stage);
    if(graphGenerator == NULL){
        graphGenerator = new Parser();
        //graph = new Graph();
        //graphGenerator->readFile();
    }
    if (stage == 0) {
        //this->readCrossing();
        sendBeacon= new cMessage("send Beacon");
        graph = new Graph();
        djisktra = new Djisktra();
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void HospitalControlApp::finish()
{
    //Duoc goi khi RSU ket thuc cong viec
    TraCIDemoRSU11p::finish();
    if(traci == NULL){
        EV<<"NULL eventually"<<endl;
    }
    if(Constant::activation == NULL){
            EV<<"Constant is helpless eventually"<<endl;
    }
    //for (auto it = crossings.begin(); it != crossings.end(); it++){
        //EV<<it->id<<" "<<it->rec->xMin<<endl;
    //}

    this->djisktra->expSmoothing->printMaxWeights(this->djisktra->vertices);

    EV<<"#calling traci->getPersonIds(): "<<count<<endl;
    EV<<"1) As 10 AGVs => T: 3796, W: 1811, %: 48%"<<endl;
    EV<<"2) As 10 + 1(flow 11) AGVs => T: 4521, W: 2334, %: 52%"<<endl;
    EV<<"3) As 10 + 1(flow 10) + 1(flow11) AGVs => T: 4793, W: 2416, %: 50%"<<endl;
    EV<<"4) As 10 + 1(flow 11) + 1(flow 2) AGVs => T: 5175, W: 2782, %: 54%"<<endl;
    EV<<"5) As 10 + 1(flow10) + 1(flow 11) + 1(flow 2) AGVs => T: 5455, W: 2871, %: 53%"<<endl;
    EV<<"6) As 10 + 2(flow10) + 1(flow 11) + 1(flow 2) AGVs => T: 6846, W: 3616, %: 53%"<<endl;
    EV<<"7) As 10 + 2(flow10) + 2(flow 11) + 1(flow 2) AGVs => T: 5863, W: 2850, %: 49%"<<endl;
    EV<<"8) As 10 + 2(flow10) + 2(flow 11) + 1(flow 2) + 1(flow0) AGVs => T: 6734, W: 3517, %: 52%"<<endl;
    EV<<"9) As 10 + 2(flow10) + 2(flow 11) + 3(flow0-2) AGVs => T: 7649, W: 4202, %: 55%"<<endl;
    EV<<"10) As 10 + 2(flow10) + 2(flow 11) + 4(flow0-3) AGVs => T: 7261, W: 3621, %: 50%"<<endl;
    EV<<"11) As 10 + 2(flow10) + 2(flow 11) + 5(flow0-4) AGVs => T: 6488, W: 2644, %: 41%"<<endl;
    EV<<"12) As 10 + 2(flow10) + 2(flow 11) + 6(flow0-5) AGVs => T: 8027, W: 3973, %: 49.5%"<<endl;
    EV<<"13) As 10 + 2(flow10) + 2(flow 11) + 7(flow0-6) AGVs => T: 9578, W: 5289, %: 55%"<<endl;
    EV<<"14) As 10 + 2(flow10) + 2(flow 11) + 8(flow0-7) AGVs => T: 10471, W: 5970, %: 57%"<<endl;
    EV<<"15) As 10 + 2(flow10) + 2(flow 11) + 9(flow0-8) AGVs => T: 9538, W: 4830, %: 51%"<<endl;
    EV<<"16) As 10 + 2(flow10) + 2(flow 11) + 10(flow0-9) AGVs => T: 9939, W: 5083, %: 51%"<<endl;

    EV<<"As 10 + 1(flow 8) AGVs => T: 2709, W: 547, %: 20%"<<endl;
    EV<<"As 10 + 1(flow 2) AGVs => T: 3045.7, W: 875, %: 29%"<<endl;
    //EV<<"As 11 AGVs => T: 2720.8, W: 547, %: 20%"<<endl;
    EV<<"As 11 AGVs + 1(10) => T: 3239.9, W: 887.7, %: 27.4%"<<endl;

    EV<<"Total waiting time: "<<Constant::TOTAL_WAITING_TIME*0.1<<"(s)"<<endl;
    EV<<"Total travelling time: "<<Constant::TOTAL_TRAVELLING_TIME<<"(s)"<<endl;
    double percentage = Constant::TOTAL_WAITING_TIME*10/Constant::TOTAL_TRAVELLING_TIME;
    EV<<"% of waiting time: "<<percentage<<endl;
    // statistics recording goes here
}

void HospitalControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void HospitalControlApp::onWSM(BaseFrame1609_4 *wsm){
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
    cPacket *enc = wsm->getEncapsulatedPacket();
    if (TraCIDemo11pMessage *bc = dynamic_cast<TraCIDemo11pMessage*>(enc)) {
        if (sendBeacon != NULL) {
            if (sendBeacon->isScheduled()) {
                cancelEvent(sendBeacon);
            }

            if(traci == NULL){
                if(Constant::activation != NULL)
                    traci = Constant::activation->getCommandInterface();
            }

            /*if(simTime().dbl() - lastUpdate >= 0.2 && false){
                count++;
                std::list<std::string> allPeople = traci->getPersonIds();
                double x, y;
                //for(int i = 0; i < crossings.size(); i++){
                for (auto elem: allPeople) {
                    std::string personId = elem;
                    Coord peoplePosition = traci->getPersonPosition(personId);
                    std::pair<double,double> coordTraCI = traci->getTraCIXY(peoplePosition);
                    //veins::Coord newCoord;
                    x = coordTraCI.first;
                    y = coordTraCI.second;
                    //newCoord.z = 0;
                    for(int i = 0; i < crossings.size(); i++){
                        if (crossings[i].rec->checkInside(x, y)) {
                    //    crossings[i].peoples.push_back(std::make_tuple(personId, newCoord.x,
                            //newCoord.y, simTime().dbl()));
                            break;
                        }
                        else if (crossings[i].rec->checkAround(x, y)){
                                break;
                        }
                    }
                }

                lastUpdate = simTime().dbl();
            }*/

            TraCIDemo11pMessage *rsuBeacon = new TraCIDemo11pMessage();

            char *ret = mergeContent(bc->getSenderAddress());
            rsuBeacon->setDemoData(ret);
            rsuBeacon->setSenderAddress(myId);
            BaseFrame1609_4 *WSM = new BaseFrame1609_4();
            WSM->encapsulate(rsuBeacon);
            populateWSM(WSM);
            send(WSM, lowerLayerOut);
        }
        std::string newRoute = readMessage(bc);
        //newRoute = this->removeAntidromic(newRoute);
        if(newRoute.length() != 0){
            if((newRoute.find("-E230 -E233") != std::string::npos
                || newRoute.find("-E233 -E230") != std::string::npos)
                //&& simTime().dbl() > 160
                ){
                std::stringstream streamData(bc->getDemoData());
                std::string content ;
                double t = simTime().dbl();
                getline(streamData, content);
                EV<<"Control what?"<<endl;
            }
            sendToAGV(newRoute);
        }
    }

}

void HospitalControlApp::sendToAGV(std::string content){
    if(content.length() != 0){
        TraCIDemo11pMessage* rsuBeacon = new TraCIDemo11pMessage();
        if(sendBeacon != NULL){
            if (sendBeacon->isScheduled())
            {
                cancelEvent(sendBeacon);
            }
            const char *ret = content.c_str(); //mergeContent(bc->getSenderAddress());
            rsuBeacon->setDemoData(ret);
            rsuBeacon->setSenderAddress(myId);
            BaseFrame1609_4* WSM = new BaseFrame1609_4();
            WSM->encapsulate(rsuBeacon);
            populateWSM(WSM);
            send(WSM,lowerLayerOut);
        }
    }
}

void HospitalControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void HospitalControlApp::handleSelfMsg(cMessage* msg)
{
    TraCIDemoRSU11p::handleSelfMsg(msg);
}

void HospitalControlApp::handlePositionUpdate(cObject* obj)
{
    TraCIDemoRSU11p::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}



/*void HospitalControlApp::exponentialSmoothing(NodeVertex *nv, double stopTime) {
    if (nv->v->k == 0) {
        nv->v->predictW = stopTime;
        nv->v->d = nv->v->q = 0;
        nv->v->k++;
    } else {
        double error = stopTime - nv->v->predictW;
        nv->v->q = Constant::GAMMA * error - (1 - Constant::GAMMA) * nv->v->q;
        nv->v->d = Constant::GAMMA * abs(error)
                - (1 - Constant::GAMMA) * nv->v->d;
        double lambda = abs(nv->v->q / nv->v->d);
        nv->v->predictW = lambda * stopTime + (1 - lambda) * nv->v->predictW;
//        mes = mes + " " + std::to_string(nv->v->predictW);
    }
}*/

void HospitalControlApp::readLane(AGV *cur, std::string str) {
    double localWait = cur->itinerary->localWait * 0.1;
    str.erase(str.find("_"));
    cur->itinerary->laneId = str;
    if(cur->itinerary->prevLane.compare(cur->itinerary->laneId) != 0){
        if(cur->itinerary->prevLane.compare(cur->itinerary->station) == 0){
            cur->passedStation = cur->itinerary->prevLane.length() > 0;
        }
        int idVertex = this->djisktra->findVertex(str);
        if(cur->itinerary->prevVertex != idVertex){
            if(cur->itinerary->prevVertex != -1){
                this->djisktra->expSmoothing->exponentialSmooth(cur->itinerary->prevVertex,
                        this->djisktra->weightVertices[cur->itinerary->prevVertex]);
                this->djisktra->expSmoothing->addWait(cur->itinerary->prevVertex, -localWait);
            }
            cur->itinerary->localWait = 0;
            cur->itinerary->prevVertex = idVertex;
        }
        cur->itinerary->prevLane = str;
    }
}

std::string HospitalControlApp::readMessage(TraCIDemo11pMessage *bc) {
    double t = simTime().dbl();
    if(t > 187.55){
        EV<<"dfdffddf";
    }
    std::stringstream streamData(bc->getDemoData());
    std::string str;
    AGV *cur = NULL;
    for (auto a : vhs) {
        if (a->id.compare(std::to_string(bc->getSenderAddress())) == 0)
            cur = a;
    }
    if (cur == NULL) {
        cur = new AGV();//3 dong sau ghep thanh 1 phan ptkd cua AGV co tham so truyen vao
        cur->id = std::to_string(bc->getSenderAddress());
        cur->itinerary = new ItineraryRecord();
        cur->itinerary->localWait = 0;
        vhs.push_back(cur);
    }
    int i = 0;
    std::string newRoute = "";
    //bool debugHere = true;
    while (getline(streamData, str, ' ')) {
        if (i == 0) {
            //debugHere = (str.compare("E293_1") == 0);
            readLane(cur, str);
        } else if (i == 2) {
            if (std::stod(str) == 0) {
                cur->itinerary->localWait++;
                int currentIndex = cur->itinerary->prevVertex;
                this->djisktra->expSmoothing->addWait(currentIndex, 0.1);
                if(this->djisktra->expSmoothing->getWait(currentIndex)
                      > this->djisktra->weightVertices[currentIndex]
                ){
                    this->djisktra->expSmoothing->exponentialSmooth(currentIndex,
                                            this->djisktra->weightVertices[currentIndex]);
                }
                //debugHere = false;
            }
            else{

            }
        }
        if(i == 3){
            //if(debugHere && str.compare("route_3") == 0){
            //    EV<<"dsds"<<endl;
            //}
            newRoute = reRoute(cur, str);
        }
        i++;
    }
    return newRoute;
}

/*double HospitalControlApp::getAvailablePerdestrian(std::string crossId, double _time) {
    int count = 0;
    double start = 0;

    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    double tmp = (_time - start)/0.1;
    EV << "Start: "<<start << " End: "<< _time <<endl;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId](const Crossing& obj) {return obj.id.compare(crossId) == 0;});
    if (it != crossings.end())
    {
        double pivot = start;
        do {
            for(auto elem : it->peoples) {
                if (pivot <= std::get<3>(elem) && std::get<3>(elem) < pivot + 0.1) {
                    count++;
                    break;
                }
            }
            pivot = pivot + 0.1;
        } while(pivot < _time && count != tmp);
    }
    return count/tmp;

}*/

/*double HospitalControlApp::getVeloOfPerdestrian(std::string crossId, double _time) {
    double start = 0;
    if (_time - Constant::DELTA_T > 0) {
        start = _time - Constant::DELTA_T;
    }
    std::set < std::string > personIds;
    double sum = 0;
    int numPeople = 0;
    auto it = find_if(crossings.begin(), crossings.end(), [&crossId](const Crossing& obj) {return obj.id.compare(crossId) == 0;});
    if (it != crossings.end())
    {
        for(auto elem : it->peoples) {
            if (start <= std::get<3>(elem) && std::get<3>(elem) < _time) {
                personIds.insert(std::get<0>(elem));
            }
        }
        numPeople = personIds.size();
        for(auto person : personIds) {
            std::vector<std::tuple<std::string, double, double, double>> tmp;
            for(auto elem : it->peoples) {
                if(std::get<0>(elem).compare(person) == 0 && start <= std::get<3>(elem) && std::get<3>(elem) <= _time) {
                    tmp.push_back(elem);
                }
            }
            int n = tmp.size();
            if (n > 1){
                sum += sqrt(
                    (std::get<1>(tmp[n-1]) - std::get<1>(tmp[0]))*(std::get<1>(tmp[n-1]) - std::get<1>(tmp[0]))
                    + (std::get<2>(tmp[n-1]) - std::get<2>(tmp[0]))*(std::get<2>(tmp[n-1]) - std::get<2>(tmp[0]))
                    ) / (std::get<3>(tmp[n-1]) - std::get<3>(tmp[0]));
            }

        }
    }

    double averageSpeed = sum / numPeople;

    return averageSpeed;
}*/

std::string HospitalControlApp::reRoute(AGV *cur, std::string routeId){

    int idOfI_Vertex = this->djisktra->findI_Vertex(cur->itinerary->laneId, false);
    int src = -1, station = -1, exit = -1;
    int i = -1;
    if(idOfI_Vertex != cur->reRouteAt){
        for(i = 0; i < this->djisktra->itineraries.size(); i++){
            if(routeId.compare(std::get<0>(this->djisktra->itineraries[i])) == 0){
                src = std::get<1>(this->djisktra->itineraries[i]);
                station = std::get<2>(this->djisktra->itineraries[i]);
                if(cur->itinerary->station.length() == 0){
                    cur->itinerary->station = this->djisktra->vertices[station];
                }
                exit = std::get<3>(this->djisktra->itineraries[i]);
                break;
            }
        }
    }
    else{
        return "";
    }

    //if((idOfI_Vertex == station && i != -1){
    if(cur->passedStation){
        std::get<2>(this->djisktra->itineraries[i]) = -1;
        station = -1;
    }
    int nextDst = (station == -1) ? exit : station;
    if(nextDst > -1){
        if(idOfI_Vertex == nextDst){
            return "";
        }
        this->djisktra->DijkstrasAlgorithm(idOfI_Vertex, nextDst);
        double t = simTime().dbl();
        if(routeId.compare("route_11") == 0 && t >= 187.55 && (cur->id.compare("28") == 0)){
            EV<<"New route: "<<t<<endl;
        }
        std::string newRoute = this->djisktra->getRoute(this->djisktra->traces[nextDst], cur->itinerary->laneId, exit == nextDst);
        if(routeId.compare("route_11") == 0 && newRoute.find("-E230 -E232") != std::string::npos){
            EV<<"New route: "<<t<<endl;
        }
        if(nextDst != exit){
            this->djisktra->DijkstrasAlgorithm(nextDst, exit);

            std::string futureLane = ""; //this->djisktra->vertices[nextDst];
            trim_right(newRoute);
            //Cam xoa comment nay: O day can dam bao la newRoute khong co space o cuoi
            for(int i = newRoute.length() - 1; i >= 0; i--){
                if(newRoute[i] != ' '){
                    futureLane = newRoute[i] + futureLane;
                }
                else{
                    break;
                }
            }

            if(routeId.compare("route_11") == 0 && t >= 186.0){
                EV<<"New route: "<<t<<endl;
            }

            std::string lastPath = this->djisktra->getRoute(this->djisktra->traces[exit], futureLane, exit == exit);
            if(lastPath.find(futureLane + " ") != std::string::npos
                    && newRoute.find(" " + futureLane) != std::string::npos
            ){
                /*std::string omitedLastPath = "";
                for(int i = futureLane.length(); i < lastPath.length(); i++){
                    omitedLastPath = omitedLastPath + lastPath[i];
                }*/
                //newRoute = newRoute + " " + omitedLastPath;
                //if(futureLane.length() + 1>= lastPath.length() || simTime().dbl() > 2.35){
                //    EV<<"Out of range";
                //}
                lastPath = lastPath.substr(futureLane.length() + 1);
            }
            //else{
                newRoute = newRoute + " " + lastPath;
                if((newRoute.find("-E230 -E232") != std::string::npos
                    || newRoute.find("-E232 -E230") != std::string::npos)
                    //&& simTime().dbl() > 160
                    ){
                    double t = simTime().dbl();
                    EV<<"Control what?"<<endl;
                }

            //}
        }
        //std::string lastPath = this->djisktra->getFinalSegment(this->djisktra->traces[nextDst]);
        //if(lastPath.length() > 0){
        //    EV<<"BEUF hare"<<endl;
        //}
        double t1 = simTime().dbl();
        //if(t >= 26.9543){
        if(t1 > 7.3){
            //if(newRoute)
            EV<<"sfsddsfdss";
        }
        newRoute = removeAntidromic(newRoute);
        if((newRoute.find("-E230 -E233") != std::string::npos
            || newRoute.find("-E233 -E230") != std::string::npos)
            //&& simTime().dbl() > 160
            ){
            double t = simTime().dbl();
            EV<<"Control what?"<<endl;
        }
        if(newRoute.length() == 0)
            return "";
        newRoute = "$" + cur->id + "_" + newRoute;
        return newRoute;
    }
    return "";
}

std::string HospitalControlApp::removeAntidromic(std::string input){
    std::vector<std::string> list = split(input, " ");
    int i = 0;
    std::string temp1, temp2;
    bool foundAntidromic = false;
    bool foundDuplication = false;
    bool change = false;
    int size = list.size();
    while(i < size - 1){
        foundAntidromic = false;
        foundDuplication = false;
        for(i = 0; i < list.size() - 1; i++){
            temp1 = list[i];
            temp2 = list[i+1];
            if(temp1.compare(temp2) == 0){
                foundDuplication = true;
                break;
            }
            if(list[i][0] == '-' && list[i+1][0] != '-'){
                temp2 = '-' + temp2;
            }
            if(list[i][0] != '-' && list[i+1][0] == '-'){
                temp1 = '-' + temp1;
            }
            if(temp1.compare(temp2) == 0){
                foundAntidromic = true;
                break;
            }
        }
        if(foundDuplication){
            change = true;
            list.erase(list.begin() + i);
            size--;
            i = 0;
        }
        if(foundAntidromic){
            change = true;
            list.erase(list.begin() + i);
            list.erase(list.begin() + i);
            size -= 2;
            i = 0;
        }
    }
    if(!change)
        return input;
    if(list.size() >= 1){
        std::string result = list[0];
        for(int j = 1; j < list.size(); j++){
            result = result + " " + list[j];
        }
        return result;
    }
    else{
        return "";
    }
}

