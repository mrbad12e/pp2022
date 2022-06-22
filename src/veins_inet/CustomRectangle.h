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

#ifndef VEINS_INET_CUSTOMRECTANGLE_H_
#define VEINS_INET_CUSTOMRECTANGLE_H_

#include <string>
//#include "inet/common/geometry/common/Coord.h"
#include "veins/base/utils/Coord.h"
//using namespace veins;
//using namespace std;

class CustomRectangle {
public:
    CustomRectangle();
    CustomRectangle(std::string points);
//    static inet::Coord string2Coord(std::string point);
    static veins::Coord string2Coord(std::string point);
//    static double triangleArea(inet::Coord A, inet::Coord B, inet::Coord C);
    static double triangleArea(veins::Coord A, veins::Coord B, veins::Coord C);
    //double rectangleArea();
//    bool checkInside(inet::Coord I);
    bool checkInside(double x, double y);
    bool checkAround(double x, double y);
    virtual ~CustomRectangle();

    //veins::Coord A;
    //veins::Coord B;
    //veins::Coord C;
    //veins::Coord D;
    //

private:
    static constexpr const double AROUND = 2;
    double xMin;
    double yMin;
    double xMax;
    double yMax;
};

#endif /* VEINS_INET_CUSTOMRECTANGLE_H_ */
