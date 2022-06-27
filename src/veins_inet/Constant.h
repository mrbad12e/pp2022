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

#ifndef VEINS_INET_CONSTANT_H_
#define VEINS_INET_CONSTANT_H_

#include <stdlib.h>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include "veins/modules/mobility/traci/TraCIMobility.h"
using veins::TraCIMobility;
using namespace veins;

class Constant {
public:
    static constexpr const char* FIRST = "First vehicle";
    static constexpr const char* RSU_IDENTIFY = "RSU";
    static constexpr const long WANTED_ID = 52;
    static constexpr const double ZONE_LENGTH = 1.1;
    static constexpr const char* NON_CAR = "";
    static constexpr const double GAMMA = 0.3;
    static constexpr const double DELTA_T = 1;
    static constexpr const double EXPIRED_TIME = 2;
    static constexpr const double MAX_SPEED = 4;
    static constexpr const double LENGTH_CROSSING = 6.4;
    Constant();
    virtual ~Constant();
    static TraCIMobility* activation;
    static long TOTAL_WAITING_TIME;
    static double TOTAL_TRAVELLING_TIME;

};

static char* mergeContent(long Id){
    char *cstr = new char[strlen(Constant::RSU_IDENTIFY) + 1];
    strcpy(cstr, Constant::RSU_IDENTIFY);
    std::string str = std::to_string(Id);
    char *new_str = new char[str.length() + 1];
    strcpy(new_str, str.c_str());
    char *ret = new char[strlen(Constant::RSU_IDENTIFY) + strlen(new_str) + 1];
    strcpy(ret, cstr);
    strcat(ret, new_str);
    return ret;
}

static std::vector<std::string> split(const std::string& str, const std::string& delim)
{
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do
    {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos-prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    }
    while (pos < str.length() && prev < str.length());
    return tokens;
}

static std::string getLane(std::vector<std::string> list, bool wantLast){
    if(wantLast){
        for(int i = list.size() - 1; i >= 0; i--){
            if(list[i][0] != ':' &&
                    list[i].find("_") == std::string::npos
            ){
                return list[i];
            }
        }
    }
    else{
        for(int i = 1; i < list.size(); i++){
            if(list[i][0] != ':' &&
                    list[i].find("_") == std::string::npos
            ){
                return list[i];
            }
        }
    }
    return "";
}

static int locateLast(std::string route, std::string trace){
    std::vector<std::string> tokens = split(route, " ");
    int last = tokens.size() - 1;
    if(last < 0)
        return -1;
    std::string lastLane = tokens[last];

    if(trace.find("$" + lastLane + "$") == std::string::npos){
        return -1;//not found
    }
    else{
        int location = trace.find("$" + lastLane + "$") + lastLane.length();
        return location;
    }
}


#endif /* VEINS_INET_CONSTANT_H_ */
