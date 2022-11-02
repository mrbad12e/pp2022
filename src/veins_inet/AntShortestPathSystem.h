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

#include <unordered_map>
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include <random>
#include <cmath>
#include <limits>
#include <iostream>
#include <string>
#include <map>
#include "AdaptiveSystem.h"
#include "Djisktra.h"
using namespace std;

#ifndef VEINS_INET_ANTSYSTEM_H_
#define VEINS_INET_ANTSYSTEM_H_

struct edgeHash
{
    size_t operator()(const AdaptiveSystem::Edge& edge) const
    {
        return std::hash<long int>()(edge.id);
    }
};

class AntShortestPathSystem : public AdaptiveSystem, Djisktra
{
public:
    static const int ANTS = 250;
    static const int ITERATIONS = 150;
    static const int PHERO_QUANTITY = 100;
    static constexpr double A_PAR = 1;
    static constexpr double B_PAR = 5;
    static constexpr double EVAPO_RATE = 0.5;
    AntShortestPathSystem(const std::string&, int = 0, int = 0);
    AntShortestPathSystem(int ants, int iterations);
    AntShortestPathSystem();
    virtual ~AntShortestPathSystem();
    virtual std::vector<int> path(int, int);
    virtual void clear();
    virtual void insertEdge(int, int, double) noexcept(false);

private:
    void init(int, int);
    double prob(int, int, std::vector<int>&);
    double heuInfo(int, int);
    double pheromone(int, int);
    virtual double diffPheromone(double);
    std::vector<int> availNeighbours(int);
    virtual void updateTrails(std::map<int, std::vector<int>>&,
            std::map<int, double>&);
    virtual void goAnt(int, int, std::vector<int>&);
    virtual double calcTourLength(std::vector<int>&);
    bool isCyclic(int, const std::vector<int>&);
    std::unordered_map<AdaptiveSystem::Edge, double, edgeHash> edge2phero;
    int ants;
    int iterations;
    std::mt19937_64 gen;
};

#endif /* VEINS_INET_ANTSYSTEM_H_ */
