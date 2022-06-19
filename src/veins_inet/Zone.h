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

#ifndef VEINS_INET_ZONE_H_
#define VEINS_INET_ZONE_H_
#include <assert.h>
#include <ctype.h>
#include <regex>
#include "Constant.h"
//#include "Parser.h"
#include <string>
#include <bits/stdc++.h>
#include <tuple>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <filesystem>
#include <unistd.h>
#include <dirent.h>
using std::tuple;
class EmptyZone;
class UsingZone;
class Parser;
class Zone;

enum HeadOrTail{
    Head = 0,
    Normal = 1,
    Tail = 1000,
    Both = 2
};

enum TypeOfZone{
    ZoneType = 0,
    EmptyType = 1,
    UsingType = 2
};

class Zone{
public:
    Zone();
    virtual ~Zone();
    int carID;
    double _start;
    double _end;
    double _waiting;
    double _measuarement;
    double _forcast;
    HeadOrTail _hOrT;
    //virtual void accept(CarLog *log);
    virtual void copy(Zone* z);
    //virtual void copy(std::shared_ptr<Zone> z);
    virtual TypeOfZone getType();

};

class EmptyZone : public Zone{
public:
    EmptyZone();
    virtual ~EmptyZone();
    //void accept(CarLog *log) override;
    void copy(Zone* z) override;
    //void copy(std::shared_ptr<Zone> z) override;
    virtual TypeOfZone getType() override;
};

class UsingZone : public Zone{
public:
    UsingZone();
    virtual ~UsingZone();
    //void accept(CarLog *log) override;
    void copy(Zone* z) override;
    //void copy(std::shared_ptr<Zone> z) override;
    virtual TypeOfZone getType() override;

};

class GLane {
public:
    GLane(){
        length = 0;
        number = 0;//# of zones
        neighborsIds[0] = -1;
        neighborsIds[1] = -1;
        neighborsIds[2] = -1;
        numNeighbors = 0;
        from[0] = -1;
        from[1] = -1;
        from[2] = -1;
    }
    virtual ~GLane(){}
    double length;
    int number;//# of zones
    Zone** zones;
    std::string strId;
    long neighborsIds[3];
    int numNeighbors ;
    long from[3];
    double lastLength;
};
class Junction{
public:
    Junction(){ N = 0;}
    virtual ~Junction(){}
    GLane **allLanes;
    int N;
    int getIdOfLane(std::string tp){
        int i = 0;
        for(i = 0; i < tp.length(); i++){
            if(tp.at(i) == '_')
                break;
        }
        int NUM_LENGTH = 3;
        char num[NUM_LENGTH]; int index = 0;
        for(int j = i + 1; j < tp.length(); j++){
            if(isdigit(tp.at(j))){
                num[index] = tp.at(j);
                index++;
            }
            else{
                break;
            }
        }
        num[index] = '\0';

        int result = atoi(num);
        if((result + 1) > N) N = result + 1;
        if(N > 16)
            std::cout<<"at "<<tp<<" N > 16"<<std::endl;
        return result;
    }
};

class Parser {
public:
    Parser(){

    }
    virtual ~Parser(){

    }
    std::string getStrId(std::string str){
        std::string id = "";
        id = id + str.at(0) + str.at(1);

        for(int i = 2; i < str.length() - 1; i++){
            id = id + str.at(i);
            if(str.at(i + 1) == ' '){
                break;
            }
        }
        return id;
    }
    long getIndex(const char* str, int *typeJ){
        int offset = 0;
        int index = 0;
        *typeJ = str[0] == ':' ? 1 : 0;

        if(str[0] == '-' || str[0] == ':'){
            index++; //skip '-'
            offset = str[0] == '-' ? 1 : 0;
        }
        index++; //skip 'E' or 'J'
        char num[4];
        num[3] = '\0';
        int j = index;
        for(j = index; j < index + 3; j++){
            num[j - index] = str[j];
            if(str[j+1] == '_' && ((j - index + 1) <= 3))
            {
                num[j - index + 1] = '\0';
                break;
            }
        }
        long x = atoi(num);
        index = j+1;
        if(*typeJ == 1){
            char subNum[3];
            subNum[0] = str[j+2];

            if(str[j + 3] == '_'){
                subNum[1] = '\0';
            }
            else{
                subNum[1] = str[j+3];
                subNum[2] = '\0';
            }
            long sub = atoi(subNum);
            sub = sub << 16;
            x += this->N;
            x |= sub;
        }

        x = (offset == 1) ? -x : x;
        long y = *typeJ == 0 ? (2*abs(x) + offset) : abs(x);
        return y;
    }
    long adjust(long key){
        long newKey = key;
    /*
    374, 375, 376, 377,
    398, 399,
    832, 833,
    892, 893, 894, 895, 896, 897, 898, 899, 900, 901,
    934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947
    */
        #pragma region declare anchors
        int anchor0 = 143;
        int anchor1 = 152;
        //144, 145, 146, 147, 148, 149, 150, 151,
        int anchor2 = 173;
        int anchor3 = 180;
        //174, 175, 176, 177, 178, 179,
        int anchor4 = 373;
        int anchor5 = 378;
        //374, 375, 376, 377,
        int anchor6 = 397;
        int anchor7 = 400;
        //398, 399
        int anchor8 = 831;
        int anchor9 = 834;
        //832, 833,
        int anchor10 = 891;
        int anchor11 = 902;
        //892, 893, 894, 895, 896, 897, 898, 899, 900, 901,
        int anchor12 = 933;
        int anchor13 = 948;
        //934, 935, 936, 937, 938, 939, 940, 941, 942, 943, 944, 945, 946, 947
        #pragma endregion
        if(key <= anchor0){
            return key;
        }
        else if(key >= anchor1 && key <= anchor2){
            newKey = key - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor3 && key <= anchor4){
            newKey = key - (anchor3 - anchor2 - 1) - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor5 && key <= anchor6){
            newKey = key - (anchor5 - anchor4 - 1)
                         - (anchor3 - anchor2 - 1)
                         - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor7 && key <= anchor8){
            newKey = key //- (anchor7a - anchor6a - 1)
                         - (anchor7 - anchor6 - 1)
                         - (anchor5 - anchor4 - 1)
                         - (anchor3 - anchor2 - 1)
                         - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor9 && key <= anchor10){
            newKey = key - (anchor9 - anchor8 - 1)
                         - (anchor7 - anchor6 - 1)
                         - (anchor5 - anchor4 - 1)
                         - (anchor3 - anchor2 - 1)
                         - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor11 && key <= anchor12){
            newKey = key - (anchor11 - anchor10 - 1)
                         - (anchor9 - anchor8 - 1)
                         - (anchor7 - anchor6 - 1)
                         - (anchor5 - anchor4 - 1)
                         - (anchor3 - anchor2 - 1)
                         - (anchor1 - anchor0 - 1);
        }
        else if(key >= anchor13){
            newKey = key - (anchor13 - anchor12 - 1)
                         - (anchor11 - anchor10 - 1)
                         - (anchor9 - anchor8 - 1)
                         - (anchor7 - anchor6 - 1)
                         - (anchor5 - anchor4 - 1)
                         - (anchor3 - anchor2 - 1)
                         - (anchor1 - anchor0 - 1);
        }
        return newKey;
    }
    /*GLane *getAllLanes(){

    }*/
    void readFile(){
        std::fstream newfile;

        newfile.open("input.txt", std::ios::in);
        if (newfile.is_open()){   //checking whether the file is open
            std::string tp;
            getline(newfile, tp);
            N = atoi(tp.c_str());
            this->allLanes = initAllLanes(N);
            getline(newfile, tp);
            NJunc = atoi(tp.c_str());
            this->allJuncs = initAllJuncs(NJunc);
            //int sum = getSumFromZeroTo(N);
            int total = 0;
            while(getline(newfile, tp)){ //read data from file object and put it into string.
                update(/*allLanes,*/ tp, &total);
            }
            newfile.close(); //close the file object.
            this->setSourcesOfNormalLanes();
            this->setSourcesOfJuncs();
            //sort(v.begin(), v.end());

            assertAllLanes();

            assertAllJuncs();
        }
    }
    GLane **allLanes;
    Junction **allJuncs;
    int N;
    int NJunc;
    int extractLaneOfJunc(int id, int* offset = NULL){
        if(offset != NULL)
            *offset = id >> 16;
        int idOfJ = ((id << 16) >> 16) - this->N;
        return idOfJ;
    }

private:

    GLane **initAllLanes(int N){
        GLane** arr = (GLane **)malloc(N*sizeof(GLane*));
        for(int i = 0; i < N; i++){
            arr[i] = new GLane();//*t;
            arr[i]->length = 0;
            arr[i]->number = 0;
            arr[i]->neighborsIds[0] = -1;
            arr[i]->neighborsIds[1] = -1;
            arr[i]->neighborsIds[2] = -1;
            arr[i]->numNeighbors = 0;
        }
        return arr;
    }
    Junction **initAllJuncs(int N){
        Junction** arr = (Junction **)malloc(N*sizeof(Junction*));
        for(int i = 0; i < N; i++){
            arr[i] = NULL;
        }
        return arr;
    }
    void initJunc(long i){
        if(allJuncs[i] == NULL){
            allJuncs[i] = new Junction();
            allJuncs[i]->allLanes = (GLane **)malloc(16*sizeof(GLane*));
            for(int j = 0; j < 16; j++){
                allJuncs[i]->allLanes[j] = NULL;
            }
        }
    }
    long *getNeighbors(std::string content){
        std::string regex_str = " ";
        int count = 0;
        int NUM_NEIGHBOR = 3;

        long *arr = (long *)malloc(NUM_NEIGHBOR*sizeof(long));

        arr[0] = arr[1] = arr[2] = -1;
        auto tokens = split(content, regex_str);
        int typeJ = 0;
        for (auto& item : tokens)
        {
            if(count >= 2){
                long x = getIndex(item.c_str(), &typeJ);
                if(typeJ == 0)
                    x = adjust(x);
                arr[count - 2] = x;
            }
            count++;
        }
        return arr;
    }

    double getLength(const char *content){
        int found = -1;//find the position of length edge
        for(int i = 0; i < strlen(content); i++){
            if(content[i] == '_'){
                found = i;
            }
            else if(found != -1 && content[i] == ' '){
                found = i;
                break;
            }
        }
        int NUM_LENGTH = 5;
        char num[NUM_LENGTH]; int index = 0;
        if(found != -1 && isdigit(content[found + 1])){//found the start of lane's length
            for(int i = found; i < strlen(content); i++){
                if(content[i] == '.' || isdigit(content[i]) && index < NUM_LENGTH)
                {
                    num[index] = content[i];
                    index++;
                }
            }
        }
        else{
            return 0.0;
        }
        num[NUM_LENGTH - 1] = '\0';
        char *eptr;
        double result = strtod(num, &eptr);
        return result;
    }
    //int getSumFromZeroTo(int N);
    void update(std::string tp, int *total){
        int typeJ = 0;
        long i = getIndex(tp.c_str(), &typeJ);

        if(tp.at(0) != ':'){
            i = adjust(i);
        }
        *total += i;

        if(tp.at(0) == ':' && tp.at(1) == 'J'){
            int offset = 0;
            int idOfJunc = extractLaneOfJunc(i, &offset);
            initJunc(idOfJunc);
            update(allJuncs[idOfJunc], tp);
        }
        if(tp.at(0) == 'E' || tp.at(0) == '-')
        {
            update(allLanes[i], tp);
        }
    }

    void update(GLane* lane, std::string tp){
        double length = getLength(tp.c_str());
        assert(length != 0);
        long *indexesOfNeighbors = getNeighbors(tp);
        lane->length = length;
        lane->strId = getStrId(tp);
        lane->number = ceil(lane->length/Constant::ZONE_LENGTH);
        int size = lane->number;
        lane->lastLength = lane->length - (size-1)*Constant::ZONE_LENGTH;
        updateHeadOfTail(lane, size);
        lane->neighborsIds[0] = indexesOfNeighbors[0];
        lane->neighborsIds[1] = indexesOfNeighbors[1];
        lane->neighborsIds[2] = indexesOfNeighbors[2];
        if(indexesOfNeighbors[0] != -1) lane->numNeighbors++;
        if(indexesOfNeighbors[1] != -1) lane->numNeighbors++;
        if(indexesOfNeighbors[2] != -1) lane->numNeighbors++;
    }

    void update(Junction* junc, std::string tp){
        int idOfLane = junc->getIdOfLane(tp);
        junc->allLanes[idOfLane] = new GLane();
        update(junc->allLanes[idOfLane], tp);
    }
    void assertAllLanes(){}
    void assertAllJuncs(){}
    void setSourcesOfNormalLanes(){
        for(int i = 0; i < this->N; i++){
            for(int j = 0; j < 3; j++){
                if(allLanes[i]->neighborsIds[j] != -1){
                    int indexOfNeighbor = allLanes[i]->neighborsIds[j];
                    if(indexOfNeighbor < this->N){
                        if(allLanes[allLanes[i]->neighborsIds[j]]->from[0] == -1){
                            allLanes[indexOfNeighbor]->from[0] = i;
                        }
                        else if(allLanes[allLanes[i]->neighborsIds[j]]->from[1] == -1){
                            allLanes[allLanes[i]->neighborsIds[j]]->from[1] = i;
                        }
                        else if(allLanes[allLanes[i]->neighborsIds[j]]->from[2] == -1){
                            allLanes[allLanes[i]->neighborsIds[j]]->from[2] = i;
                        }
                    }
                    else{
                        int offset = 0;
                        int idOfJ = extractLaneOfJunc(indexOfNeighbor, &offset);
                        if(allJuncs[idOfJ]->allLanes[offset]->from[0] == -1){
                            allJuncs[idOfJ]->allLanes[offset]->from[0] = i;
                        }
                        else if(allJuncs[idOfJ]->allLanes[offset]->from[1] == -1){
                            allJuncs[idOfJ]->allLanes[offset]->from[1] = i;
                        }
                        else if(allJuncs[idOfJ]->allLanes[offset]->from[2] == -1){
                            allJuncs[idOfJ]->allLanes[offset]->from[2] = i;
                        }
                    }
                }
            }
        }
    }
    void setSourcesOfJuncs(){
        for(int i = 0; i < this->NJunc; i++){
            if(allJuncs[i] != NULL){
                for(int j = 0; j < 16; j++){
                    if(allJuncs[i]->allLanes[j] != NULL){
                        for(int k = 0; k < 3; k++){
                            if(allJuncs[i]->allLanes[j]->neighborsIds[k] != -1){
                                int indexOfNeighbor = allJuncs[i]->allLanes[j]->neighborsIds[k];
                                if(indexOfNeighbor < this->N){
                                    if(allLanes[indexOfNeighbor]->from[0] == -1){
                                        allLanes[indexOfNeighbor]->from[0] = ((i + this->N) | (j << 16));
                                    }
                                    else if(allLanes[indexOfNeighbor]->from[1] == -1){
                                        allLanes[indexOfNeighbor]->from[1] = ((i + this->N) | (j << 16));
                                    }
                                    else if(allLanes[indexOfNeighbor]->from[2] == -1){
                                        allLanes[indexOfNeighbor]->from[2] = ((i + this->N) | (j << 16));
                                    }
                                }
                                else{
                                    int offset = 0;
                                    int idOfJ = extractLaneOfJunc(indexOfNeighbor, &offset);
                                    if(allJuncs[idOfJ]->allLanes[offset]->from[0] == -1){
                                        allJuncs[idOfJ]->allLanes[offset]->from[0] = ((i + this->N) | (j << 16));
                                    }
                                    else if(allJuncs[idOfJ]->allLanes[offset]->from[1] == -1){
                                        allJuncs[idOfJ]->allLanes[offset]->from[1] = ((i + this->N) | (j << 16));
                                    }
                                    else if(allJuncs[idOfJ]->allLanes[offset]->from[2] == -1){
                                        allJuncs[idOfJ]->allLanes[offset]->from[2] = ((i + this->N) | (j << 16));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //std::vector<int> v;
    void updateHeadOfTail(GLane* lane, int size){
        lane->zones = new Zone*[size];
        for(int j = 0; j < size; j++){
            EmptyZone* z = new EmptyZone();
            z->carID = -1;
            lane->zones[j] = z;
            lane->zones[j]->carID = -1;
        }
        lane->zones[0]->_hOrT = HeadOrTail::Head;
        lane->zones[lane->number - 1]->_hOrT = HeadOrTail::Tail;
        if(size == 1) lane->zones[0]->_hOrT = HeadOrTail::Both;
    }
};




#endif /* VEINS_INET_ZONE_H_ */
