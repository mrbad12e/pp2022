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
        sendBeacon= new cMessage("send Beacon");
        graph = new Graph();
        djisktra = new Djisktra();
        this->readCrossing();
    }
    else if (stage == 1) {
        // Initializing members that require initialized other modules goes here
    }
}

void HospitalControlApp::readCrossing(){
    if(Constant::SHORTEST_PATH)
        return;
    //Always call after this->djisktra != NULL
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
    //std::string name = "";
    areas = (double *)malloc(this->djisktra->numIVertices*sizeof(double));
    for(int i = 0; i < this->djisktra->numIVertices; i++){
        areas[i] = 0;
    }
    this->aroundIntersections.resize(this->djisktra->numIVertices);
    for(int i = 0; i < crossings.size(); i++){
        for(int j = 0; j < this->djisktra->numIVertices; j++){
            //name = this->djisktra->vertices[j];
            //if(crossings[i].id.compare(name) == 0){
            if(crossings[i].id.compare(this->djisktra->vertices[j]) == 0){
                this->aroundIntersections[j].push_back(i);
                areas[j] += crossings[i].rec->getArea();
                break;
            }
        }

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

    //this->djisktra->expSmoothing->printMaxWeights(this->djisktra->vertices);

    EV<<"#calling traci->getPersonIds(): "<<count<<endl;
    EV<<"1) As 10 AGVs => T: 3022(3796), W: 241(1811), %: 8(48)%"<<endl;
    EV<<"2) As 10 + 1(flow 11) AGVs => T: 3177(4521), W: 463(2334), %: 15(52)%"<<endl;
    EV<<"3) As 10 + 1(flow 10) + 1(flow11) AGVs => T: 2982(4793), W: 76(2416), %: 3(50)%"<<endl;
    EV<<"4) As 10 + 1(flow 11) + 1(flow 2) AGVs => T: 3020(5175), W: 119(2782), %: 4(54)%"<<endl;
    EV<<"5) As 10 + 1(flow10) + 1(flow 11) + 1(flow 2) AGVs => T: 3220(5455), W: 76(2871), %: 2.3(53)%"<<endl;
    EV<<"6) As 10 + 2(flow10) + 1(flow 11) + 1(flow 2) AGVs => T: 3409(6846), W: 81(3616), %: 2.4(53)%"<<endl;
    EV<<"7) As 10 + 2(flow10) + 2(flow 11) + 1(flow 2) AGVs => T: 3988(5863), W: 133(2850), %: 3.3(49)%"<<endl;
    EV<<"8) As 10 + 2(flow10) + 2(flow 11) + 1(flow 2) + 1(flow0) AGVs => T: 4096(6734), W: 83.1(3517), %: 2(52)%"<<endl;
    EV<<"9) As 10 + 2(flow10) + 2(flow 11) + 3(flow0-2) AGVs => T: 4271(7649), W: 87(4202), %: 2(55)%"<<endl;
    EV<<"10) As 10 + 2(flow10) + 2(flow 11) + 4(flow0-3) AGVs => T: 5214(7261), W: 492(3621), %: 9.5(50)%"<<endl;
    EV<<"11) As 10 + 2(flow10) + 2(flow 11) + 5(flow0-4) AGVs => T: 4900(6488), W: 203(2644), %: 4(41)%"<<endl;
    EV<<"12) As 10 + 2(flow10) + 2(flow 11) + 6(flow0-5) AGVs => T: 5255(8027), W: 270(3973), %: 5(49.5)%"<<endl;
    EV<<"13) As 10 + 2(flow10) + 2(flow 11) + 7(flow0-6) AGVs => T: 5808(9578), W: 583(5289), %: 10(55)%"<<endl;
    EV<<"14) As 10 + 2(flow10) + 2(flow 11) + 8(flow0-7) AGVs => T: 6569(10471), W: 1049(5970), %: 16(57)%"<<endl;
    EV<<"15) As 10 + 2(flow10) + 2(flow 11) + 9(flow0-8) AGVs => T: 6439(9538), W: 694(4830), %: 11(51)%"<<endl;
    EV<<"16) As 10 + 2(flow10) + 2(flow 11) + 10(flow0-9) AGVs => T: 6709(9939), W: 895(5083), %: 13(51)%"<<endl;
    EV<<"17) As case 16 + 24(flow 12) AGVs => T: 7168(10482), W: 622(5369), %: 9(51)%"<<endl;
    EV<<"As 10 + 1(flow 8) AGVs => T: 2709, W: 547, %: 20%"<<endl;
    EV<<"As 10 + 1(flow 2) AGVs => T: 3045.7, W: 875, %: 29%"<<endl;
    //EV<<"As 11 AGVs => T: 2720.8, W: 547, %: 20%"<<endl;
    EV<<"As 11 AGVs + 1(10) => T: 3239.9, W: 887.7, %: 27.4%"<<endl;

    EV<<"Reproduce case "<<17<<") Total waiting time: "<<Constant::TOTAL_WAITING_TIME*0.1<<"(s)"<<endl;
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

            if(simTime().dbl() - lastUpdate >= 1 && !Constant::SHORTEST_PATH){
                count++;
                try{
                    std::list<std::string> allPeople = traci->getPersonIds();
                    for(int i = 0; i < crossings.size(); i++){
                        crossings[i].count = 0;
                    }

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
                                crossings[i].count++;
                                break;
                            }
                            else if (crossings[i].rec->checkAround(x, y)){
                                    break;
                            }
                        }
                    }
                }catch(std::exception& e){

                }

                predictDispearTime();
                lastUpdate = simTime().dbl();
            }

            TraCIDemo11pMessage *rsuBeacon = new TraCIDemo11pMessage();

            char *ret = mergeContent(bc->getSenderAddress());
            rsuBeacon->setDemoData(ret);
            rsuBeacon->setSenderAddress(myId);
            try{
                //EV<<"213";
                /*double t = simTime().dbl();
                if(t > 216.4){
                    EV<<"ehree"<<endl;
                }*/
                BaseFrame1609_4 *WSM = new BaseFrame1609_4();
                WSM->encapsulate(rsuBeacon);
                populateWSM(WSM);
                send(WSM, lowerLayerOut);
                //EV<<" 218"<<endl;
            }
            catch(std::exception& e){
                EV<<e.what()<<endl;
            }
        }
        if(!Constant::SHORTEST_PATH){
            std::string newRoute = readMessage(bc);
            //newRoute = this->removeAntidromic(newRoute);
            if(newRoute.length() != 0){
                /*if((newRoute.find("-E234 -E235") != std::string::npos
                    || newRoute.find("-E230 -E232") != std::string::npos
                        )
                    ){
                    std::stringstream streamData(bc->getDemoData());
                    std::string content ;
                    double t = simTime().dbl();
                    getline(streamData, content);
                    EV<<"Control what?"<<endl;
                }*/
                //EV<<"237";
                try{
                    /*double t = simTime().dbl();
                    if(t > 216.4){
                        EV<<"ehree"<<endl;
                    }*/
                    sendToAGV(newRoute);
                }
                catch(std::exception& e1){
                    EV<<e1.what()<<endl;
                }
                //EV<<"239"<<endl;
            }
        }
    }

}

void HospitalControlApp::predictDispearTime(){
    for(int i = 0; i < this->djisktra->numIVertices; i++){
        int sum = 0;
        for(int j = 0; j < this->aroundIntersections[i].size(); j++)
        {
            int index = this->aroundIntersections[i][j];
            sum += this->crossings[index].count;
        }
        double density = sum/areas[i];
        double velocity = getAverageVelocityByDensity(density);
        //if(i == 88){
        /*if(velocity < 0){
            EV<<"sdsfsdfsdf";
        }*/
        double predict = Constant::LENGTH_CROSSING*sum*0.5/velocity;
        if(this->djisktra->weightVertices[i] < predict
                || this->djisktra->expSmoothing->raisedTime[i] < 0
        ){
            if(predict == predict){
                this->djisktra->expSmoothing->fromPedestrians[i] = predict;
            }
            else{
                this->djisktra->expSmoothing->fromPedestrians[i] = 0;
            }
        }
    }
}

double HospitalControlApp::getAverageVelocityByDensity(double density) {
//    y = 0.2 * x^2 - 1.1 * x + 1.7
    return 0.2 * density * density - 1.1 * density + 1.7;
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
    //double t = simTime().dbl();
    //if(t > 216.4){
        //EV<<"dfdffddf";
    //}
    if(Constant::SHORTEST_PATH)
        return "";
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
            newRoute = reRoute(cur, str/*, t*/);
        }
        i++;
    }
    return newRoute;
}


std::string HospitalControlApp::reRoute(AGV *cur, std::string routeId/*, double t*/){

    if(Constant::SHORTEST_PATH)
        return "";
    if(this->djisktra->vertices[0][0] == cur->itinerary->laneId[0]){
        return "";//skip this case, too complex as AGV is on an intersection
    }

    int idOfI_Vertex = this->djisktra->findI_Vertex(cur->itinerary->laneId, false);
    /*if(this->djisktra->vertices[idOfI_Vertex].compare(cur->itinerary->laneId) == 0){
        return "";//skip this case, too complex
    }
    else{

    }*/
    int src = -1, station = -1, exit = -1;
    int i = -1;
    //if(t > 216.4){
        //EV<<"dfdffddf";
    //}
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
        if(cur->passedStation && Constant::STOP_AT_STATION){
            bool stop = cur->atStation > 0;
            if(cur->atStation == 0){
                cur->atStation = simTime().dbl();
                stop = true;
            }
            else{
                if(cur->atStation + Constant::PAUSING_TIME > simTime().dbl()){
                    stop = true;
                }
                else{
                    stop = false;
                }
            }
            if(stop){
                return "$" + cur->id + "_" + "0";
            }
            else{
                return "$" + cur->id + "_" + Constant::CARRY_ON;
            }
        }
        return "";
    }

    //if((idOfI_Vertex == station && i != -1){

    int nextDst = (cur->passedStation) ? exit : station;
    if(nextDst > -1){
        if(idOfI_Vertex == nextDst){
            return "";
        }
        this->djisktra->DijkstrasAlgorithm(idOfI_Vertex, nextDst, cur->itinerary->laneId);
        /*double t = simTime().dbl();
        if(routeId.compare("route_2") == 0 && t >= 87.15 //&& (cur->id.compare("28") == 0)
                ){
            EV<<"New route: "<<t<<endl;
        }
        if(routeId.compare("route_11") == 0 && t >= 185.754 && (cur->id.compare("28") == 0)
                ){
            EV<<"New route: "<<t<<endl;
        }*/
        std::string newRoute = this->djisktra->getRoute(this->djisktra->traces[nextDst], cur->itinerary->laneId, idOfI_Vertex, nextDst, exit);
        /*if(routeId.compare("route_2") == 0 && newRoute.find("-E234 -E235") != std::string::npos){
            EV<<"New route: "<<t<<endl;
        }
        if(routeId.compare("route_11") == 0 && newRoute.find("-E230 -E232") != std::string::npos){
            EV<<"New route: "<<t<<endl;
        }*/
        if(nextDst != exit){
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
            this->djisktra->DijkstrasAlgorithm(nextDst, exit, futureLane);

            //if(routeId.compare("route_0") == 0 && t >= 7.0){
            //    EV<<"New route: "<<t<<endl;
            //}

            std::string lastPath =
                    this->djisktra->getRoute(this->djisktra->traces[exit], futureLane, nextDst, exit, exit);
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
                /*if((newRoute.find("-E230 -E232") != std::string::npos
                    || newRoute.find("-E232 -E230") != std::string::npos)
                    //&& simTime().dbl() > 160
                    ){
                    double t = simTime().dbl();
                    EV<<"Control what?"<<endl;
                }*/

            //}
        }

        newRoute = removeAntidromic(newRoute);
        /*if((newRoute.find("-E81 E202") != std::string::npos
            || newRoute.find("-E233 -E230") != std::string::npos)
            //&& simTime().dbl() > 160
            ){
            double t = simTime().dbl();
            EV<<"Control what?"<<endl;
        }*/
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

