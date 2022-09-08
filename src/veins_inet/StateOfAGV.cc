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

#include "StateOfAGV.h"

StateOfAGV::StateOfAGV() {
    // TODO Auto-generated constructor stub
    start = -1; //simTime().dbl();
    total = 0;
}

StateOfAGV::StateOfAGV(double t) {
    // TODO Auto-generated constructor stub
    start = t;
}

StateOfAGV::~StateOfAGV() {
    // TODO Auto-generated destructor stub
}

double StateOfAGV::totalEmergencyTime(){
    return total;
}

void StateOfAGV::startEmergencyMode(){
    if(start == -1){
        start = simTime().dbl();
    }
    else{
        double temp = simTime().dbl();
        total += temp - start;
        start = temp;
    }
}

void StateOfAGV::stopEmergencyMode(){
    if(start != -1){
        double temp = simTime().dbl();
        total += temp - start;
        start = -1;
    }
}

