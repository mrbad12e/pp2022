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

#ifndef VEINS_GRAPH_H_
#define VEINS_GRAPH_H_

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <float.h>
#include <map>
#include <string>
#include <string.h>
#include <vector>
#include <algorithm>
#include <queue> // To set up priority queue
#include <functional> // To use std::greater<T> -> This will prove to be useful in picking the minimum weight

//typedef std::tuple<double, std::string, int, std::string> Quad;
typedef std::tuple<double, double, int, std::string> Quad;

class ItineraryRecord {// Ban ghi hanh trinh cua xe
public:
    std::string laneId, prevLane = "";
    std::string prevEdge = "";
    int prevVertex = -1;
    std::string station = "";
    //int stopTime;
    int localWait;
};

class Station{
public:
    Station(std::string name, std::string bestTime, std::string amplitude, std::string period){
        this->isNotChanged = false;
        this->setAttributes(name, bestTime, amplitude, period);
    }

    Station(){
        this->name = "";
        this->isNotChanged = false;
    }

    std::string getName(){
        return this->name;
    }

    void setDest(std::string dest){
        this->dest = dest;
    }

    std::string getDest(){
        return this->dest;
    }

    void getStation(std::string routeId){
        std::ifstream file("itinerary.txt");
        std::string line;
        std::string nameRoute;
        std::string nameSrc, nameStation, nameJunc, nameDest;
        std::string period, bestTime, amplitude;
        int source, indexOfStation, dst;

        while (getline(file, line)) {
            if(line[0] != '#'){
                std::stringstream ss(line);
                getline(ss, nameRoute, ' ');
                if(nameRoute.compare(routeId) == 0){
                    getline(ss, nameSrc, ' ');
                    getline(ss, nameStation, ' ');
                    getline(ss, nameJunc, ' ');
                    getline(ss, nameDest, ' ');
                    getline(ss, period, ' ');
                    getline(ss, bestTime, ' ');
                    getline(ss, amplitude, ' ');
                    this->setAttributes(nameJunc, bestTime, amplitude, period);
                    this->setDest(nameDest);
                    break;
                }
            }
        }
        file.close();
    }

    void setAttributes(std::string name, std::string bestTime, std::string amplitude, std::string period){
        //if(this->isNotChanged)
        //    return;

        if(!name.empty()){
                this->name = name;
        }
        if(!bestTime.empty()){
                this->bestTime = std::stod(bestTime);
        }
        if(!amplitude.empty()){
                this->amplitude = std::stod(amplitude);
        }
        if(!period.empty()){
                this->period = std::stod(period);
        }
    }

    std::string toJSON(){
        return "\"station\" : { \"name\" : \"" + this->name + "\", " +
                                "\"period\": \"" + std::to_string(this->period) + "\", " +
                                "\"bestTime\" : \"" + std::to_string(this->bestTime) + "\", " +
                                "\"amplitude\" : \"" + std::to_string(this->amplitude) + "\"}";
    }

    double getHarmfulness(double reachedTime, int count, double *sooner, double *later){
        *sooner = 0;
        *later = 0;
        if(count < 0)
            return DBL_MAX;
        if(bestTime + count*period - amplitude <= reachedTime &&
                bestTime + count*period + amplitude >= reachedTime
        ){
            return 0;
        }
        if(bestTime + count*period - amplitude > reachedTime){
            *sooner = bestTime + count*period - amplitude - reachedTime;
            *sooner /= 60;
            return ((*sooner)*0.1 + 4);
        }
        else{
            *later = reachedTime - (bestTime + count*period + amplitude);
            *later /= 60;
            double delta = *later;
            return (delta*delta + 2*delta + 1);
        }
    }
    void setProtected(bool isNotChanged){
        this->isNotChanged = isNotChanged;
    }
private:
    std::string name;
    double bestTime = -1;
    double amplitude = -1;
    double period = -1;
    std::string dest = "";
    bool isNotChanged = false;
    //std::string allRequests = "";
};
///usr/include/c++/9/bits/stl_function.h:386:20: error: no match for ‘operator<’ (operand types are ‘const Quad’ and ‘const Quad’)

class AGV {
public:
    std::string id;
    ItineraryRecord *itinerary;
    int reRouteAt = -1;
    bool passedStation = false;
    double atStation = 0;
    double ratio = 1.1;
    double now = 0;
    int indexOfRoute = -1;
    double *ShortestPath;
    std::vector<std::string> traces;
    std::vector <bool> visitedVertex;
    std::vector <bool> visitedEmergencyVertex;
    std::priority_queue<Quad, std::vector<Quad>, std::greater<Quad> > PQ; // Set up priority queue
    //std::priority_queue<Quad> PQ;
    void init(int numVertices, double initSource = DBL_MAX){
        if(!initialized){
            ShortestPath = (double *)malloc(numVertices*sizeof(double));
            for(int i = 0; i < numVertices; i++){
                ShortestPath[i] = initSource;
                traces.push_back("");
                visitedVertex.push_back(false);
                visitedEmergencyVertex.push_back(false);
            }
            initialized = true;
            return;
        }

        for(int i = 0; i < numVertices; i++){
            ShortestPath[i] = initSource;
            traces[i].clear();
            visitedVertex[i] = false;
            visitedEmergencyVertex[i] = false;
        }
    }
    bool isInitialized(){
        return initialized;
    }
    int count = 0;
private:
    bool initialized = false;

};

class Edge {
private:
    std::string id, from, to;
    double w;

public:
    Edge() {
    }
    void setId(std::string id) {
        this->id = id;
    }
    void setW(double w) {
        this->w = w;
    }
    void setFrom(std::string from) {
        this->from = from;
    }
    void setTo(std::string to) {
        this->to = to;
    }
    std::string getId() {
        return id;
    }
    double getW() {
        return this->w;
    }
    std::string getFrom() {
        return this->from;
    }
    std::string getTo() {
        return this->to;
    }
};

class Vertex {
private:
    std::string id;
    double w;
    std::vector<Edge*> from, to;

public:
    int k;
    double predictW, q, d;
    Vertex() {
        k = 0;
        predictW = 0;
    }
    void setId(std::string id) {
        this->id = id;
    }
    void setFrom(Edge *from) {
        for (auto f : this->from) {
            if (f->getId().compare(from->getId()) == 0)
                return;
        }
        this->from.push_back(from);
    }
    void setFrom(std::vector<Edge*> from) {
        this->from = from;
    }
    void setTo(Edge *to) {
        for (auto t : this->to) {
            if (t->getId().compare(to->getId()) == 0)
                return;
        }
        this->to.push_back(to);
    }
    void setTo(std::vector<Edge*> to) {
        this->to = to;
    }
    void setW(double w) {
        this->w = w;
    }
    std::string getId() {
        return id;
    }
    std::vector<Edge*> getFrom() {
        return this->from;
    }
    std::vector<Edge*> getTo() {
        return this->to;
    }
    double getW() {
        return this->w;
    }
};

class Internal {
private:
    std::string from, to;
    double w;
    std::vector<std::string> j_parts;

public:
    Internal() {
    }
    void setFrom(std::string from) {
        this->from = from;
    }
    void setTo(std::string to) {
        this->to = to;
    }
    void setW(double w) {
        this->w = w;
    }
    void setJpart(std::string j_part) {
        this->j_parts.push_back(j_part);
    }
    std::string getFrom() {
        return this->from;
    }
    std::string getTo() {
        return this->to;
    }
    double getW() {
        return this->w;
    }
    std::vector<std::string> getJparts() {
        return this->j_parts;
    }
};

class Intersection: public Vertex {
private:
    std::vector<Internal*> internals;

public:
    void setInternal(Internal *internal) {
        this->internals.push_back(internal);
    }
    std::vector<Internal*> getInternals() {
        return this->internals;
    }
};

//class ExitBuffer: public Vertex {
////public:
////    void setW(double w) {
////    }
////
////    double getW() {
////        return 0;
////    }
//};

class J_of_vertex {
public:
    std::string name, from, to;
    double w;
    J_of_vertex(std::string name) {
        this->name = name;
    }
    J_of_vertex(std::string name, double w) {
        this->name = name;
        this->w = w;
    }
};

class NodeVertex {
public:
    Intersection *v;
    std::vector<J_of_vertex*> j_of_vertex;
    NodeVertex *left, *right;
    J_of_vertex* search_j(std::string name) {
        for (auto j : j_of_vertex) {
            if (j->name.compare(name) == 0)
                return j;
        }
        return NULL;
    }
};

class NodeEdge {
public:
    Edge *e;
    std::vector<std::string> e_of_edge;
    NodeEdge *left, *right;
    int check_edge(std::string name) {
        for (auto e : e_of_edge) {
            if (e.compare(name) == 0)
                return 1;
        }
        return 0;
    }
};

class Graph {
private:
    NodeVertex *vertices;
    NodeEdge *edges;
    void insertVertex(NodeVertex **root, Vertex *v, J_of_vertex *j_of_vertex) {
        if (*root == NULL) {
            *root = new NodeVertex();
            root[0]->v = (Intersection*) v;
            if (j_of_vertex != NULL)
                root[0]->j_of_vertex.push_back(j_of_vertex);
            return;
        }
        if (v->getId() < root[0]->v->getId()) {
            insertVertex(&root[0]->left, v, j_of_vertex);
        } else if (v->getId() > root[0]->v->getId()) {
            insertVertex(&root[0]->right, v, j_of_vertex);
        } else
            return;
    }
    void insertEdge(NodeEdge **root, Edge *e) {
        if (*root == NULL) {
            *root = new NodeEdge();
            root[0]->e = e;
            return;
        }
        if (e->getId() < root[0]->e->getId()) {
            insertEdge(&root[0]->left, e);
        } else if (e->getId() > root[0]->e->getId()) {
            insertEdge(&root[0]->right, e);
        } else
            return;
    }
    void handle_1(std::string j_root, std::string str, int i) {
        std::string j_id = j_root;
        j_id.erase(j_id.find('_'));
        if (i == 1) {
            NodeVertex *nv = searchVertex(j_id);
            if (nv == NULL) {
                Vertex *v = new Vertex();
                v->setId(j_id);
                insertVertex(&vertices, v,
                        new J_of_vertex(j_root, std::stod(str)));
            } else {
                J_of_vertex *j = nv->search_j(j_root);
                if (j == NULL) {
                    nv->j_of_vertex.push_back(
                            new J_of_vertex(j_root, std::stod(str)));
                } else {
                    j->w = std::stod(str);
                }
            }
        }
        if (i > 1) {
            std::string str_id = str;
            str_id.erase(str.find('_'));
            NodeVertex *nv = searchVertex(j_id);
            J_of_vertex *j = nv->search_j(j_root);
            if (str.front() != ':') {
                j->to = str_id;
                NodeEdge *ne = searchEdge(str_id);
                if (ne == NULL) {
                    Edge *e = new Edge();
                    e->setId(str_id);
                    e->setFrom(j_id);
                    insertEdge(&edges, e);
                    nv->v->setTo(e);
                } else {
                    ne->e->setFrom(j_id);
                    nv->v->setTo(ne->e);
                }
            } else {
                j->to = str;
                j = nv->search_j(str);
                if (j == NULL) {
                    nv->j_of_vertex.push_back(new J_of_vertex(str));
                }
                j = nv->search_j(str);
                j->from = j_root;
            }
        }
    }
    void handle_2(std::string e_root, std::string str, int i) {
        std::string e_id = e_root;
        e_id.erase(e_id.find('_'));
        if (i == 1) {
            NodeEdge *ne = searchEdge(e_id);
            if (ne == NULL) {
                Edge *e = new Edge();
                e->setId(e_id);
                e->setW(std::stod(str));
                insertEdge(&edges, e);
                ne = searchEdge(e_id);
                ne->e_of_edge.push_back(e_root);
            } else {
                if (ne->check_edge(e_root) == 0) {
                    ne->e_of_edge.push_back(e_root);
                    ne->e->setW(
                            (std::stod(str)
                                    + ((ne->e->getW() == 0) ?
                                            std::stod(str) : ne->e->getW()))
                                    / 2);
                }
            }
        }
        if (i > 1) {
            std::string str_id = str;
            str_id.erase(str.find('_'));
            NodeEdge *ne = searchEdge(e_id);
            if (str.front() == ':') {
                NodeVertex *nv = searchVertex(str_id);
                if (nv == NULL) {
                    Vertex *v = new Vertex();
                    v->setId(str_id);
                    v->setFrom(ne->e);
                    insertVertex(&vertices, v, new J_of_vertex(str));
                    std::string newExitBuffer = ne->e->getId() + "-"
                            + v->getId();
                    Vertex *eb = new Vertex();
                    eb->setId(newExitBuffer);
                    eb->setW(0);
                    insertVertex(&vertices, eb, NULL);
                    nv = searchVertex(str_id);
                    J_of_vertex *j = nv->search_j(str);
                    j->from = e_id;
                    ne->e->setTo(str_id);
                } else {
                    int c = 0;
                    for (auto f : nv->v->getFrom()) {
                        if (f->getId().compare(ne->e->getId()) == 0)
                            c = 1;
                    }
                    if (c == 0) {
                        std::string newExitBuffer = ne->e->getId() + "-"
                                + nv->v->getId();
                        Vertex *eb = new Vertex();
                        eb->setId(newExitBuffer);
                        eb->setW(0);
                        insertVertex(&vertices, eb, NULL);
                    }
                    nv->v->setFrom(ne->e);
                    J_of_vertex *j = nv->search_j(str);
                    if (j == NULL) {
                        nv->j_of_vertex.push_back(new J_of_vertex(str));
                    }
                    j = nv->search_j(str);
                    j->from = e_id;
                    ne->e->setTo(str_id);
                }
            }
        }
    }
    void readLine(std::string str_line) {
        std::stringstream streamData(str_line);
        std::string str, str_root;
        int i = 0, c = 0;
        while (getline(streamData, str, ' ')) {
            if (i == 0)
                str_root = str;
            if (str.front() == ':' && i == 0) {
                c = 1;
            } else if (i == 0) {
                c = 2;
            }
            if (c == 1 && i > 0)
                handle_1(str_root, str, i);
            else if (c == 2 && i > 0)
                handle_2(str_root, str, i);
            i++;
        }
    }
    void inorder(NodeVertex **root) {
        if (*root != NULL) {
            inorder(&root[0]->left);
            if (root[0]->j_of_vertex.size() <= 2
                    && root[0]->j_of_vertex.size() > 0) {
                double w = 0;
                for (auto j : root[0]->j_of_vertex) {
                    w = w + j->w;
                }
                w = w / root[0]->j_of_vertex.size();
                root[0]->v->setW(w);
            } else if (root[0]->j_of_vertex.size() > 2) {
                Intersection *i = new Intersection();
                i->setId(root[0]->v->getId());
                i->setFrom(root[0]->v->getFrom());
                i->setTo(root[0]->v->getTo());
                for (auto j : root[0]->j_of_vertex) {
                    if (j->from.front() != ':' && j->to.front() != ':') {
                        Internal *in = new Internal();
                        in->setFrom(j->from);
                        in->setTo(j->to);
                        in->setW(j->w);
                        in->setJpart(j->name);
                        i->setInternal(in);
                    } else {
                        Internal *in = new Internal();
                        if (j->from.front() != ':') {
                            in->setFrom(j->from);
                            in->setW(j->w);
                            in->setJpart(j->name);
                            J_of_vertex *jv = root[0]->search_j(j->to);
                            do {
                                in->setJpart(jv->name);
                                in->setW(in->getW() + jv->w);
                                if (jv->to.front() == ':')
                                    jv = root[0]->search_j(jv->to);
                                else
                                    break;
                            } while (jv != NULL);
                            in->setTo(jv->to);
                        }
                        i->setInternal(in);
                    }
                }
                root[0]->v = i;
            }
            inorder(&root[0]->right);
        }
    }
    void readFile() {
        std::string str_line;
        std::ifstream MyReadFile("input.txt");
        while (getline(MyReadFile, str_line)) {
            readLine(str_line);
        }
        MyReadFile.close();
        inorder(&vertices);
    }
public:
    Graph() {
        readFile();
    }

    void addVertex(Vertex *v) {
        insertVertex(&vertices, v, NULL);
    }

    void addEdge(Edge *e) {
    }

    NodeVertex* getVertex() {
        return this->vertices;
    }

    NodeVertex* searchVertex(std::string id) {
        NodeVertex *cur = vertices;
        while (cur != NULL) {
            if (id < cur->v->getId())
                cur = cur->left;
            else if (id > cur->v->getId())
                cur = cur->right;
            else
                break;
        }
        return cur;
    }

    NodeEdge* searchEdge(std::string id) {
        NodeEdge *cur = edges;
        while (cur != NULL) {
            if (id < cur->e->getId())
                cur = cur->left;
            else if (id > cur->e->getId())
                cur = cur->right;
            else
                break;
        }
        return cur;
    }
//    virtual ~Graph();
};

#endif /* VEINS_GRAPH_H_ */
