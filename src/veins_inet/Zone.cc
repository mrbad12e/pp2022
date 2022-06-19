#include "Zone.h"
Zone::Zone(){

}

Zone::~Zone(){

}

void Zone::copy(Zone* z) /*: base()*/
//void Zone::copy(std::shared_ptr<Zone> z)
{ }

//void Zone::accept(CarLog* log) /*: base()*/
//{

//}

TypeOfZone Zone::getType(){
    return TypeOfZone::ZoneType;
}

EmptyZone::EmptyZone() /*: base()*/
{
    this->_hOrT = HeadOrTail::Normal;
    //std::cout<<"EmptyZone"<<std::endl;
}

EmptyZone::~EmptyZone() /*: base()*/
{}

//void EmptyZone::accept(CarLog* log)  /*: base()*/
//{
//    log->updateWeight(this);
//}

TypeOfZone EmptyZone::getType(){
    return TypeOfZone::EmptyType;
}

void EmptyZone::copy(Zone* z) /*: base()*/
{
    if(z->getType() == TypeOfZone::UsingType){
        this->carID = -1;
        this->_start = z->_start;
        this->_end = z->_end;
        this->_waiting = z->_waiting;
        this->_measuarement = z->_measuarement;
        this->_forcast = z->_forcast;
    }
}

UsingZone::UsingZone() /*: base()*/
{
    //std::cout<<"sdfsdfsdf"<<std::endl;
}

UsingZone::~UsingZone() /*: base()*/
{}

//void UsingZone::accept(CarLog* log) /*: base()*/
//{
    //std::cout<<"sdfsdfsdf"<<std::endl;
//    log->updateWeight(this);
//}

TypeOfZone UsingZone::getType(){
    return TypeOfZone::UsingType;
}

void UsingZone::copy(Zone* z){
//void UsingZone::copy(std::shared_ptr<Zone> z){
    if(z->getType() == TypeOfZone::EmptyType){
        this->carID = z->carID;
        this->_start = z->_start;
        this->_end = z->_end;
        this->_waiting = z->_waiting;
        this->_measuarement = z->_measuarement;
        this->_forcast = z->_forcast;
    }
}
