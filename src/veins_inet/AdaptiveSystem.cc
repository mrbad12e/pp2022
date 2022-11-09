/*
 * AcoPath: Shortest path calculation using Ant Colony Optimization
 * Copyright (C) 2014-2021 by Constantine Kyriakopoulos
 * zfox@users.sourceforge.net
 * @version 0.9.1
 *
 * @section LICENSE
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "AdaptiveSystem.h"
#include "veins/veins.h"
//#include <bits/stdc++.h>
using namespace omnetpp;
/**
 * Constructor for edges.
 */
AdaptiveSystem::Edge::Edge()
{
    edgeStart = edgeEnd = weight = id = 0;
}

/**
 * Comparison of current instance with the rhs, based on ids.
 *
 * @param rhs The right-hand side object
 * @return bool The indication of current id being less than rhs'
 */
bool AdaptiveSystem::Edge::operator<(const Edge& rhs) const
{
    return id < rhs.id;
}

/**
 * Comparison of current instance with the rhs, based on ids.
 *
 * @param rhs The right-hand side object
 * @return bool The indication of current id being greater than rhs'
 */
bool AdaptiveSystem::Edge::operator>(const Edge& rhs) const
{
    return id > rhs.id;
}

/**
 * Comparison of current instance with the rhs for equality, based on ids.
 *
 * @param rhs The right-hand side object
 * @return bool The indication of equality
 */
bool AdaptiveSystem::Edge::operator==(const Edge& rhs) const
{
    return id == rhs.id;
}

/**
 * Empty constructor for the AdaptiveSystem.
 */
AdaptiveSystem::AdaptiveSystem() { }

/**
 * Empty destructor for the AdaptiveSystem.
 */
AdaptiveSystem::~AdaptiveSystem() { }

/**
 * Initialises the local topology representation.
 *
 * @param filename JSON-formatted file containing the topology representation
 */
void AdaptiveSystem::initTopo(const std::string& filename)
{
    ptree pt;
    boost::property_tree::read_json(filename, pt);
    ptree::const_iterator end = pt.end();
    for(ptree::const_iterator it = pt.begin(); it != end; ++it)
    {
        if(!std::strcmp(it->first.c_str(), "number_of_nodes"))
            continue;

        ptree::const_iterator end2 = it->second.end();
        for(ptree::const_iterator it2 = it->second.begin(); it2 != end2; ++it2)
        {
            ptree::const_iterator end3 = it2->second.end();
            int src = 0; int dest = 0; int length = 0;
            for(ptree::const_iterator it3 = it2->second.begin(); it3 != end3; ++it3)
            {
                if(!std::strcmp(it3->first.c_str(), "nodes"))
                {
                    int index = 0;
                    ptree::const_iterator end31 = it3->second.end();
                    for(ptree::const_iterator it31 = it3->second.begin(); it31 != end31; ++it31, ++index)
                    {
                        if(index == 0)
                            src = it31->second.get_value<int>();
                        if(index == 1)
                            dest = it31->second.get_value<int>();
                    }
                }
                if(!std::strcmp(it3->first.c_str(), "length"))
                    length = it3->second.get_value<int>();
            }

            insertEdge(src, dest, static_cast<double>(length));
        }
    }
}

/**
 * Inserts an edge.
 *
 * @param src Starting node
 * @param dest Ending node
 * @param weight Weight for the edge
 */
void AdaptiveSystem::insertEdge(int src, int dest, double weight) noexcept(false)
{
    AdaptiveSystem::Edge edge;
    edge.edgeStart = src;
    edge.edgeEnd = dest;
    edge.weight = weight;
    edge.id = ++edgeIdCnt;
    adaptiveEdges.push_back(edge);
}

bool AdaptiveSystem::isWorking(){
    bool isRunning = false;
    for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
        STATE_OF_REQUEST state = std::get<4>(*it);
        if(state == BEING_PROCESSED){
            isRunning = true;
            break;
        }
    }
    return isRunning;
}

bool AdaptiveSystem::removeExpiredRequests(std::vector<int>* expiredRequests){
    bool selfSearching = expiredRequests == NULL;
    if(expiredRequests != NULL){
        selfSearching = expiredRequests->size() == 0;
    }
    std::vector<int> *tooOldOnes ;
    if(selfSearching){
        tooOldOnes = new std::vector<int>();
        int i = 0;
        double t = simTime().dbl();
        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
            STATE_OF_REQUEST state = std::get<4>(*it);
            double createdTime = std::get<3>(*it);

            if(state != BEING_PROCESSED && (t - createdTime > Constant::DELAY)){
                tooOldOnes->push_back(i);
            }
            i++;
        }
    }
    else{
        tooOldOnes = expiredRequests;
    }
    int index = 0;
    int numberOfRemoved = 0;
    std::reverse(tooOldOnes->begin(), tooOldOnes->end());
    for(auto it = tooOldOnes->begin(); it != tooOldOnes->end(); it++){
        index = *it;
        if(index >= 0 && index < allRequests.size()){
            allRequests.erase(allRequests.begin() + index);
            numberOfRemoved++;
        }
    }
    return (numberOfRemoved != 0);
}

bool AdaptiveSystem::canExecuteReqs(){
    //int beingProcessed = 0;
    int waiting = 0;
    double t = simTime().dbl();
    for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
        STATE_OF_REQUEST state = std::get<4>(*it);
        double createdTime = std::get<3>(*it);
        if(state == BEING_PROCESSED){
            return false;
        }
        else if(state == WAITING_FOR_PROCESSING && (t - createdTime < Constant::DELAY)){
            waiting++;
        }
    }
    return (waiting >= 1);
}

bool AdaptiveSystem::isFullReqs(){
    int count = 0;
    for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
       STATE_OF_REQUEST state = std::get<4>(*it);
       if(state == WAITING_FOR_PROCESSING
       ){
           count++;
       }
    }
    return (count > Constant::MAX_REQUESTS);
}


bool AdaptiveSystem::insertRequest(int source, int dst, std::string id){
    if(!isWorking()){
        if(isFullReqs()){
            return false;
        }
        double t = simTime().dbl();
        this->removeExpiredRequests(NULL);

        for(std::vector<Request>::iterator it = allRequests.begin(); it != allRequests.end(); it++){
            int theSource = std::get<0>(*it);
            int theDestination = std::get<1>(*it);
            if(theSource == source && theDestination == dst){
                std::string ids = std::get<2>(*it);
                if(ids.find("$" + id + "$") == std::string::npos){
                    std::get<2>(*it) = ids + "$" + id + "$";
                    return true;
                }
            }
        }//end of for

        allRequests.push_back(
                std::make_tuple(source, dst, "$" + id + "$", t, WAITING_FOR_PROCESSING, ""));
        return true;

    }
    return false;
}

/**
 * Used for producing edge IDs.
 */
int AdaptiveSystem::edgeIdCnt = 0;
