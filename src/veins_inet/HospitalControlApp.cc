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
//#include "HashAPI.cpp"
//#include "UnitTest.cpp"
//#include "ReadFile.cpp"

#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"

using namespace veins;

Register_Class(HospitalControlApp);

void HospitalControlApp::initialize(int stage)
{
    TraCIDemoRSU11p::initialize(stage);
    if(graphGenerator == NULL){
        graphGenerator = new Parser();
        //graphGenerator->readFile();
    }
    if (stage == 0) {
        this->readCrossing();

        sendBeacon= new cMessage("send Beacon");
        graph = new Graph();
        count = new Count();
        count->k = 0;
        count->i = 0;
        count->laneId = "";
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
    for (auto it = crossings.begin(); it != crossings.end(); it++){
        //EV<<it->id<<" "<<it->rec->xMin<<endl;
    }
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

            TraCIDemo11pMessage *rsuBeacon = new TraCIDemo11pMessage();

            char *ret = mergeContent(bc->getSenderAddress());
            rsuBeacon->setDemoData(ret);
            rsuBeacon->setSenderAddress(myId);
            BaseFrame1609_4 *WSM = new BaseFrame1609_4();
            WSM->encapsulate(rsuBeacon);
            populateWSM(WSM);
            send(WSM, lowerLayerOut);
        }
        std::stringstream streamData(bc->getDemoData());
        std::string str, before_name;
        before_name = count->laneId;
        int i = 0;
        while (getline(streamData, str, ' ')) {
            if (i == 0) {
                int x = str.find("_");
                str.erase(x);
                if (count->laneId.length() == 0) {
                    count->laneId = str;
                } else if (count->laneId.length() > 0 && str.compare(count->laneId) != 0) {
                    std::string mes;
                    if (before_name.front() == ':') {//kiem tra xem co phai laneID la mot intersection
                        mes = count->laneId + " "
                                + std::to_string(count->k * 0.1);
                        NodeVertex *nv = graph->searchVertex(before_name);
                        double w =
                                (nv->v->getW() == 0) ?
                                        count->k :
                                        (nv->v->getW() + count->k) / 2;//why???
                        nv->v->setW(w);
                    } else {
                        std::string full_name = before_name + "-" + str;
                        mes = full_name + " " + std::to_string(count->k * 0.1);
                        NodeVertex *nv = graph->searchVertex(full_name);
                        if (nv != NULL) {
                            double w =
                                    (nv->v->getW() == 0) ?
                                            count->k :
                                            (nv->v->getW() + count->k) / 2;//why?
                            nv->v->setW(w);
                        } else {
                            Vertex *e = new Vertex();
                            e->setId(full_name);
                            e->setW(count->k);
                            graph->addVertex(e);
                        }
                    }
                    message.push_back(mes);
                    count->i = 0;
                    count->k = 0;
                    count->laneId.erase();
                }
            } else if (i == 2) {
                if (std::stod(str) == 0) {
                    count->i = 0;
                    count->k++;
                } else {
                    if (count->i == 0 && count->k > 0)
                        count->k--;
                    count->i++;
                }
            }
            i++;
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

void HospitalControlApp::readCrossing(){
    std::string line;
    std::ifstream MyReadFile("crossing.txt");
    getline(MyReadFile, line);
    int numberOfCrossing =std::stoi(line);

    int k = 0;

    while (getline(MyReadFile, line)) {
        size_t pos;
        std::string token;
        Crossing tmp;

        for (int i = 0; i < 4; i++) {
            pos = line.find(" ");
            token = line.substr(0, pos);

            if (i == 0) tmp.id = token;
            if (i == 1) tmp.length = std::atof(token.c_str());
            if (i == 2) tmp.from = token;
            if (i == 3) tmp.to = token;
            line.erase(0, pos + 1);

        }
        tmp.rec = new CustomRectangle(line);
        crossings.push_back(tmp);
        k++;
    }

    MyReadFile.close();
}

