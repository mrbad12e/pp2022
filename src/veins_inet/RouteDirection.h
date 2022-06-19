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

#ifndef VEINS_INET_ROUTEDIRECTION_H_
#define VEINS_INET_ROUTEDIRECTION_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#pragma once

#include "veins/veins.h"
using namespace omnetpp;
namespace veins {


class RouteDirection {
public:
    RouteDirection();
    virtual ~RouteDirection();
    void readFile();
private:
    std::vector<std::string> allRoutes;
    void update(std::string tp);
};
}
#endif /* VEINS_INET_ROUTEDIRECTION_H_ */
