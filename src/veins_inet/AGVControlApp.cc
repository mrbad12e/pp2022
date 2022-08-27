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

#include "AGVControlApp.h"
//#include "HospitalControlApp.h"
#include "Constant.h"
#include "jute.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"


using namespace veins;

std::map<std::string, std::string> AGVControlApp::routeDict = {{"route_0", ""}};
Register_Class(AGVControlApp);

void AGVControlApp::initialize(int stage)
{
    TraCIDemo11p::initialize(stage);
    this->station = new Station();
    sentFirstMessage = false;
    if (stage == 0) {
        idDebug = getId();
        sendBeacon= new cMessage("send Beacon");
        {
            mobility = TraCIMobilityAccess().get(getParentModule());
            traciVehicle = mobility->getVehicleCommandInterface();
            originalRoute = traciVehicle->getRouteId();
            //if(myId == 154 || idDebug == 153){
            //    EV<<"readasc"<<endl;
            //}
        }
    }
    else if (stage == 1) {

        if (sendBeacon->isScheduled())
        {
            cancelEvent(sendBeacon);
        }

        scheduleAt(simTime() + 0.1, sendBeacon);
        this->travellingTime = simTime().dbl();
        //EV<<"Initialize AGV at "<<simTime().dbl()<<" ";
        curPosition = mobility->getPositionAt(simTime());

        if(Constant::activation == NULL){
            Constant::activation = mobility;
        }
        //if(myId == 154 || idDebug == 153){
        //    EV<<"readasc"<<endl;
        //}
        //RouteDictionary *r = &Constant::routeDict;
        getIndexInFlow(std::to_string(myId), originalRoute);
        this->station->getStation(originalRoute);
    }
}

void AGVControlApp::getIndexInFlow(std::string idOfAGV, std::string routeId){
    if(AGVControlApp::routeDict[routeId].find("$" + idOfAGV + "$") == std::string::npos){
        std::string s = AGVControlApp::routeDict[routeId];
        int n = std::count(s.begin(), s.end(), '$');
        AGVControlApp::routeDict[routeId] = AGVControlApp::routeDict[routeId] + "$" + idOfAGV + "$";
        this->indexInRoute = (n/2);
    }
    //return -1;
}

void AGVControlApp::finish()
{
    EV<<myId<<endl;
    this->travellingTime = //traciVehicle->getWaitingTime();
    //                    traciVehicle->getAccumulatedWaitingTime();
            simTime().dbl() - this->travellingTime;
    //EV<<"This AGV spends "<<this->travellingTime<<" for travelling"<<endl;
    Constant::TOTAL_TRAVELLING_TIME += this->travellingTime;
    Constant::TOTAL_WAITING_TIME += this->waitingIntervals;
    Constant::TOTAL_AGV++;
    Constant::GLOBAL_HARMFULNESS += this->harmfulness;
    if(T != 0)
        Constant::TOTAL_APE += (APE/T);
    TraCIDemo11p::finish();
    if(Constant::activation == NULL){
            EV<<"Constant is helpless eventually"<<endl;
    }
    // statistics recording goes here
}

void AGVControlApp::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void AGVControlApp::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void AGVControlApp::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void AGVControlApp::handleSelfMsg(cMessage* msg)
{
    TraCIDemo11p::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
    {
        TraCIDemo11pMessage* carBeacon = new TraCIDemo11pMessage("test", 0);
        {
            std::string str = traciVehicle->getLaneId();
            str.erase(str.find("_"));
            if(str[0] != ':'){
                bool add = true;
                if(passedEdges.size() > 0){
                    if(passedEdges[passedEdges.size() - 1].compare(str) == 0){
                        add = false;
                    }
                }
                if(add){
                    passedEdges.push_back(str);
                }
            }
            if(goAround(passedEdges)){
                EV<<"Go around"<<myId<<" "<<originalRoute<<endl;
            }
           content = "{\"laneId\" : \"" +
                   traciVehicle->getLaneId() + "\", ";

           content = content + " \"idMess\" : \"" + std::to_string(idOfMessage) + "\", ";
           idOfMessage++;

           content = content + checkForPausing();

           double speed = traciVehicle->getSpeed();
           if(speed == 0.0){
               this->waitingIntervals++;
               std::string laneID = traciVehicle->getLaneId();
               this->saveBeginningOfStuck(laneID);
               if(this->stuckAtJunc[laneID] +
                   Constant::PAUSING_TIME < simTime().dbl()){
                   this->runAfterStuck();
               }
           }

           content = content + "\"lanePos\" : " + /*"L.P"*/ "\""
                   + std::to_string(traciVehicle->getLanePosition())
                   + "\", ";
           this->exponentialSmooth(traciVehicle->getLaneId(), simTime().dbl());
           content = content + "\"speed\" : " + "\""
                   /*"velo:"*/ + std::to_string(speed)
                               + "\", "
                                   ;
           content = content + "\"ratio\" : \"" + std::to_string(predictRatio) + "\", ";

           content = content + "\"now\" : \"" + std::to_string(simTime().dbl()) + "\", ";

           content = content + "\"indexInRoute\" : \"" + std::to_string(this->indexInRoute) + "\", ";

           content = content + "\"originalRouteId\" : " + "\"" + originalRoute + "\"}";
           carBeacon->setDemoData(content.c_str());
           carBeacon->setSenderAddress(myId);
           BaseFrame1609_4* WSM = new BaseFrame1609_4();
           WSM->encapsulate(carBeacon);
           populateWSM(WSM);
           send(WSM,lowerLayerOut);

           if(expectedRoute.length() > 0 && !Constant::SHORTEST_PATH){
               double t = simTime().dbl();

               std::string current = traciVehicle->getLaneId();
               int x = current.find("_");
               if(x > 0)
                   current = current.substr(0, x);
               if(expectedRoute.find(current) != std::string::npos){
                   try{
                       std::vector<std::string> v = split(expectedRoute, " ");

                       int i = 0; bool found = false;
                       if(goAround(v)){
                           EV<<"Go around"<<endl;
                       }
                       for(i = 0; i < v.size(); i++){
                           if(v[i].compare(current) == 0){
                               found = true;
                               break;
                           }
                       }
                       if(!found) i = 0;
                       std::list<std::string> l(v.begin() + i, v.end());
                       bool change = traciVehicle->changeVehicleRoute(l);
                       if(change){
                           expectedRoute = "";
                           v.clear();
                           if(myId == 190 && t > 295){
                               EV<<"SDFSFSDFSDf"<<endl;
                           }
                       }
                   }catch(std::exception &e){
                       //const char* x= e.what();
                       //EV<<x<<endl;
                   }
               }
           }
           return;
        }
    }
}

std::string AGVControlApp::checkForPausing(){
    std::string content = traciVehicle->getLaneId();

    if(content.find(this->station->getName() + "_") != std::string::npos
           && this->station->getName().length() > 0
           && //pausingTime == DBL_MAX
           pausingTime + Constant::PAUSING_TIME > simTime().dbl()
    ){
        if(velocityBeforeHalt == -1 && pausingTime == DBL_MAX){
            velocityBeforeHalt = traciVehicle->getSpeed();
            if(velocityBeforeHalt == 0)
                velocityBeforeHalt = 2;
            pausingTime = simTime().dbl();
            traciVehicle->setSpeed(0);
        }
        if(pausingTime != DBL_MAX){
            return " \"atStation\" : \"" + std::to_string(pausingTime) + "\", ";
        }
    }
    return "";
}

void AGVControlApp::handlePositionUpdate(cObject* obj)
{
    TraCIDemo11p::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void AGVControlApp::addExpectedTime(std::string str){
    std::vector<std::string> list = split(str, "_");
    if(list[1].compare("0") != 0 && list[0].length() > 0){
        dict[list[0]] = std::stod(list[1]);
    }
}

void AGVControlApp::exponentialSmooth(std::string key, double realTime){

    key.erase(key.find("_"));
    std::map<std::string, double>::iterator it;
    it = dict.find(key);
    if(it == dict.end())
        return;
    double weight = dict[key];
    if(weight == 0)
        return;

    dict.erase(key);//no longer check the key

    double realRatio = realTime / weight;

    double error = realRatio - predictRatio;
    if(key.compare(this->station->getName()) == 0){
        APE += abs(realTime - weight)/realTime;
        T++;
        this->harmfulness = this->station->getHarmfulness(realTime, this->indexInRoute);
    }
    Qt = Constant::GAMMA * error - (1 - Constant::GAMMA) * Qt;
    Dt = Constant::GAMMA * abs(error)
                    - (1 - Constant::GAMMA) * Dt;
    Dt = (Dt == 0) ? 1 : Dt;
    double lambda = abs(Qt / Dt);
    predictRatio = lambda * realRatio + (1 - lambda) * predictRatio;

    if(predictRatio <= 0)
        predictRatio = 1;
}

void AGVControlApp::handleLowerMsg(cMessage* msg)
{
    //if(msg == NULL){
    //    return;
    //}
    //try{
    BaseFrame1609_4* WSM = check_and_cast<BaseFrame1609_4*>(msg);
    cPacket* enc = WSM->getEncapsulatedPacket();
    //if(enc == NULL){
    //    return;
    //}
    if(TraCIDemo11pMessage* bc = dynamic_cast<TraCIDemo11pMessage*>(enc)){
        //int length = strlen(bc->getDemoData());
        std::stringstream streamData(bc->getDemoData());
        std::string tmp;
        std::string str = "";
        while(getline(streamData, tmp)) str += tmp;
        jute::jValue v = jute::parser::parse(str);
        std::string id = v["id"].as_string();
        if(//str.find("$" + std::to_string(myId) + "_") != std::string::npos
             std::to_string(myId).compare(id) == 0
        ){
            //if(myId == 154){
            //    EV<<"DFFDFD"<<endl;
            //}
            double t = simTime().dbl();
            std::string newRoute = //str.substr(std::to_string(myId).length() + 2);
                    v["newRoute"].as_string();
            int size = v["weights"].size();

            for(int i = 0; i < size; i++){
                addExpectedTime(v["weights"][i].as_string());
            }
            if(prevRoute.compare(newRoute) != 0){
                if(newRoute.compare("0") == 0){
                   //force AGV to stop
                    if(velocityBeforeHalt == -1){
                        velocityBeforeHalt = traciVehicle->getSpeed();
                        if(velocityBeforeHalt == 0)
                            velocityBeforeHalt = 2;
                    }
                    pausingTime = simTime().dbl();
                    traciVehicle->setSpeed(0);

                }
                else{

                    if(newRoute.compare(Constant::CARRY_ON) == 0){
                        if(prevRoute.length() == 0){
                            //This case happends as: (1) AGV follows the original Dijsktra
                            //(2) messages from RSU were lost
                            double sp = traciVehicle->getSpeed();
                            //std::string laneID = traciVehicle->getLaneId();
                            //EV_TRACE<<myId<<" "<<this->waitingIntervals<<" "<<sp<<" "<<laneID<<endl;
                            if(sp == 0 && pausingTime + Constant::PAUSING_TIME < simTime().dbl()){
                                //if waiting to long
                                this->runAfterStuck();
                                return;
                            }
                            else{
                                return;
                            }
                        }
                        newRoute = prevRoute;

                    }

                    prevRoute = newRoute;

                    this->runAfterStuck();

                    if(!Constant::SHORTEST_PATH){
                        std::vector<std::string> v = split(newRoute, " ");

                        std::list<std::string> l(v.begin(), v.end());
                        if(l.size() == 0 || goAround(v)){
                            EV_TRACE<<"ERRR";
                        }
                        bool change = traciVehicle->changeVehicleRoute(l);
                        if(!change){
                            expectedRoute = newRoute;
                        }
                        else{
                            if(myId == 190 && t > 295){
                                EV<<"Ddsffssf"<<endl;
                            }
                           expectedRoute = "";
                           v.clear();
                           v.shrink_to_fit();
                           l.clear();
                        }
                    }
                }
            }
            else{
                double sp = traciVehicle->getSpeed();
                std::string laneID = traciVehicle->getLaneId();
                //EV<<sp<<" "<<laneID<<endl;
                if(sp == 0 && laneID[0] == ':'){
                    saveBeginningOfStuck(laneID);
                    if(this->stuckAtJunc[laneID] +
                            Constant::PAUSING_TIME < simTime().dbl()){
                        this->runAfterStuck();
                    }
                }
            }
        }

    }
    else{

    }
    //}catch(std::exception& e1){
    //    EV_TRACE<<"ffffff"<<endl;
    //}
}

void AGVControlApp::saveBeginningOfStuck(std::string junc){
    std::map<std::string, double>::iterator it;
    it = this->stuckAtJunc.find(junc);
    if(it == this->stuckAtJunc.end()){
        this->stuckAtJunc[junc] = simTime().dbl();
    }
}

void AGVControlApp::runAfterStuck(){
    if(velocityBeforeHalt != -1){
        traciVehicle->setSpeed(velocityBeforeHalt);
        velocityBeforeHalt = -1;
    }
    else{
        if(pausingTime + Constant::PAUSING_TIME < simTime().dbl()){
            return;
        }
        if(traciVehicle->getSpeed() == 0){
            traciVehicle->setSpeed(2);
        }
    }
}
