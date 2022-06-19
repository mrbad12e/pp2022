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

#include "IPerson.h"
#include "Constant.h"
#include "veins/modules/application/traci/TraCIDemo11pMessage_m.h"


using namespace veins;

Register_Class(IPerson);

void IPerson::initialize(int stage)
{
    TraCIDemo11p::initialize(stage);
    EV<<"SDfsdfsdfsdfsdfs"<<endl;

}

void IPerson::finish()
{
    TraCIDemo11p::finish();
    //EV<<"AGV["<<myId<<"] reaches destination over here"<<endl;
    // statistics recording goes here
}

void IPerson::onBSM(DemoSafetyMessage* bsm)
{
    //for my own simulation circle
}

void IPerson::onWSM(BaseFrame1609_4* wsm)
{
    // Your application has received a data message from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void IPerson::onWSA(DemoServiceAdvertisment* wsa)
{
    // Your application has received a service advertisement from another car or RSU
    // code for handling the message goes here, see TraciDemo11p.cc for examples
}

void IPerson::handleSelfMsg(cMessage* msg)
{
    TraCIDemo11p::handleSelfMsg(msg);
    // this method is for self messages (mostly timers)
    // it is important to call the DemoBaseApplLayer function for BSM and WSM transmission
}

void IPerson::handlePositionUpdate(cObject* obj)
{
    TraCIDemo11p::handlePositionUpdate(obj);
    // the vehicle has moved. Code that reacts to new positions goes here.
    // member variables such as currentPosition and currentSpeed are updated in the parent class

}

void IPerson::handleLowerMsg(cMessage* msg)
{

}
