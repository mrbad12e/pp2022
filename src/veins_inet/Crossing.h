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

#ifndef VEINS_INET_CROSSING_H_
#define VEINS_INET_CROSSING_H_

#include <string>
#include "CustomRectangle.h"
//using namespace std;

class Crossing {
public:
    Crossing();
    virtual ~Crossing();
public:
    // id, name, from, to, length, rec
    std::string id;
    std::string name;
    //std::string from;
    //std::string to;
    //double length;
    CustomRectangle* rec;
    int count = 0;
    std::vector<std::tuple<std::string, double, double, double>> peoples;
};

#endif /* VEINS_INET_CROSSING_H_ */
