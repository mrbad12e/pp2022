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

#include "Constant.h"

TraCIMobility* Constant::activation = NULL;
long Constant::TOTAL_WAITING_TIME = 0;
double Constant::TOTAL_TRAVELLING_TIME = 0;
double Constant::TOTAL_APE = 0;
int Constant::TOTAL_AGV = 0;
double Constant::GLOBAL_HARMFULNESS = 0;
double Constant::GLOBAL_SONNER = 0;
double Constant::GLOBAL_LATER = 0;
double Constant::EMERGENCY_TIME = 0;
int Constant::edgeIdCnt = 0;
std::map<std::string, std::string> Constant::routeDict = {{"route_0", ""}};
//std::string Constant::SUPPLY_DISPOSAL = "$";
Constant::Constant() {
    // TODO Auto-generated constructor stub

}

Constant::~Constant() {
    // TODO Auto-generated destructor stub
}


