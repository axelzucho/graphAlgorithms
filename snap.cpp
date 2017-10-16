#include "stdafx.h"
#include "disjointset.h"

#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <map>
#include <stack>
#include <tuple>

using namespace std;
using namespace chrono;
PNEANet g = TNEANet::New();

struct oper{
    bool operator()(const std::tuple<int,int,int>& one, const std::tuple<int,int,int>& two)
    {
        return std::get<0>(one) > std::get<0>(two);
    }
};


void insertVertex(int value) {
    g->AddNode(value);
}

void insertEdge(int a, int b, int weight) {
    g->AddEdge(a,b);
    TNEANet::TEdgeI EI = g->GetEI(a,b);
    g->AddIntAttrDatE(EI, weight, "weight");
}

void deleteVertex(int value) {
    g->DelNode(value);
}

void deleteEdge(int a, int b) {
    g->DelEdge(a,b);
}



void dfs(int startingNode) {
    stack<int> visiting;
    map<int, bool> travelled;
    visiting.push(startingNode);
    travelled[startingNode] = true;
    int checking, target;
    while (!visiting.empty()) {
        checking = visiting.top();
        visiting.pop();
        TNEANet::TNodeI NI = g->GetNI(checking);
        for (int i = 0; i < NI.GetOutDeg(); i++) {
            target = NI.GetOutNId(i);
            if (travelled[target]) continue;
            travelled[target] = true;
            visiting.push(target);
        }
        cout << checking << "\n";
    }
}

void bfs(int startingNode){
    std::queue<int> visiting;
    map<int,bool> travelled;
    visiting.push(startingNode);
    travelled[startingNode] = true;
    int checking, target;
    while (!visiting.empty()) {
        checking = visiting.front();
        visiting.pop();
        TNEANet::TNodeI NI = g->GetNI(checking);
        for (int i = 0; i < NI.GetOutDeg(); i++) {
            target = NI.GetOutNId(i);
            if (travelled[target]) continue;
            travelled[target] = true;
            visiting.push(target);
        }
        cout << checking << "\n";
    }

}

vector<pair<int, int>> prim(int vertexStart){
    vector<pair<int, int>> result;
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    map<int,bool> travelled;
    std::tuple<int,int,int> now;
    int checking, target, weight;
    for(int i = 0; i < g->GetNodes(); ++i){
        result.push_back(make_pair(-1, INT32_MAX));
    }
    result[checking].second = 0;
    next.push(make_tuple(0,checking,checking));
    cout << "passed" << "\n";
    while(!next.empty()){
        now = next.top();
        next.pop();
        checking = std::get<1>(now);
        if(travelled[checking]) continue;
        travelled[checking] = true;
        result[checking] = make_pair(std::get<2>(now), std::get<0>(now));
        TNEANet::TNodeI NI = g->GetNI(checking);
        for(int i = 0; i < NI.GetOutDeg(); ++i){
            target = NI.GetOutNId(i);
            if(travelled[target]) continue;
            TNEANet::TEdgeI EI = g->GetEI(checking,target);
            weight = g->GetIntAttrDatE(EI, "weight");
            next.push(make_tuple(weight,target,checking));
        }

    }
    return result;
}

vector<pair<int, int>> dijkstra(int vertexStart){
    vector<pair<int, int>> result;
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    map<int,bool> travelled;
    std::tuple<int,int,int> now;
    int checking, target, weight;
    for(int i = 0; i < g->GetNodes(); ++i){
        result.push_back(make_pair(INT32_MAX, -1));
    }
    result[checking].first = 0;
    next.push(make_tuple(0,checking,checking));
    while(!next.empty()){
        now = next.top();
        next.pop();
        checking = std::get<1>(now);
        if(travelled[checking]) continue;
        travelled[checking] = true;
        result[checking] = make_pair(std::get<2>(now), std::get<0>(now));
        TNEANet::TNodeI NI = g->GetNI(checking);
        for(int i = 0; i < NI.GetOutDeg(); ++i){
            target = NI.GetOutNId(i);
            if(travelled[target]) continue;
            TNEANet::TEdgeI EI = g->GetEI(checking,target);
            weight = g->GetIntAttrDatE(EI, "weight");
            next.push(make_tuple(result[target].first +weight,target,checking));
        }

    }
    return result;
}



vector<vector<int>> floydWarshall(){
    vector<vector<int>> distances;
    unsigned long vNum = g->GetNodes();
    distances.resize(vNum);
    for(int it = 0; it  < g->GetNodes(); ++it){
        for(int it2 = 0; it2 < g->GetNodes(); ++it2){
            if(it == it2){
                distances[it].push_back(0);
                continue;
            }
            else distances[it].push_back(100000);
        }
    }
    for (TNEANet::TEdgeI EI = g->BegEI(); EI < g->EndEI(); EI++) {
        distances[EI.GetSrcNId()][EI.GetDstNId()] = g->GetIntAttrDatE(EI, "weight");
    }
    for(int k = 0; k < vNum; ++k){
        for(int i = 0; i < vNum; ++i){
            for(int j = 0; j < vNum; ++j){
                if(distances[i][j] > distances[i][k] + distances[k][j])
                    distances[i][j] = distances[i][k] + distances[k][j];
            }
        }
    }
    return distances;

}

vector<std::tuple<int, int, int>> kruskal () {
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    int vertex = g->GetNodes();
    vector<std::tuple<int, int, int>> result;
    int origin, target, set_o, set_t;
    std::tuple<int,int,int> now;
    DisjointSets ds(vertex);
    for (TNEANet::TEdgeI EI = g->BegEI(); EI < g->EndEI(); EI++) {
        origin = EI.GetSrcNId();
        target = EI.GetDstNId();
        next.push(std::make_tuple((int)g->GetIntAttrDatE(EI, "weight"),(int)origin,(int)target));
    }
    while(!next.empty()){
        now = next.top();
        next.pop();
        origin = std::get<1>(now);
        target = std::get<2>(now);
        set_o = ds.find(origin);
        set_t = ds.find(target);
        if (set_o != set_t){
            result.push_back(make_tuple(std::get<0>(now), origin,target));
            ds.merge(set_o, set_t);
        }
    }
    return result;
}

int main(){ //Example Program
    for(int i = 1; i < 15; ++i) insertVertex(i);


    //Here we are inserting the edges of the graph provided
    insertEdge(1, 3, 8);
    insertEdge(1, 4, 8);
    insertEdge(3, 2, 7);
    insertEdge(3, 10, 4);
    insertEdge(3, 5, 8);
    insertEdge(2, 5, 7);
    insertEdge(4, 8, 2);
    insertEdge(4, 7, 3);
    insertEdge(4, 5, 1);
    insertEdge(5, 6, 9);
    insertEdge(6, 13, 4);
    insertEdge(7, 4, 6);
    insertEdge(8, 7, 3);
    insertEdge(8, 9, 3);
    insertEdge(9, 10, 2);
    insertEdge(9, 12, 4);
    insertEdge(10, 3, 10);
    insertEdge(10, 6, 6);
    insertEdge(11, 12, 6);
    insertEdge(12, 9, 2);
    insertEdge(12, 11, 8);
    insertEdge(12, 14, 9);
    insertEdge(13, 14, 6);
    insertEdge(14, 13, 12);
    /*
    insertVertex(g); //Here we insert a vertex
    insertEdge(1,5,3,g); //Here we insert an edge from vertex 1 to 5 with weight 3
    deleteVertex(15,g); //Here we delete the vertex 15
    deleteEdge(1,5,g); //Here we delete the edge going from Vertex 1 to 5.
    */
    //dfs(1); //Here we do a depth-first search, that will print the path

    //bfs(1); //Here we do a breath-first search, that will print the path

    vector<pair<int, int>> Prim = prim(1); //Here we get a vector of pairs, telling us the minimum spanning tree according to prim
    cout << "1" << "\n";
    vector<std::tuple<int, int, int>> Kruskal = kruskal(); //Here we get a tuple of vertex and int, telling us the minimum spanning tree according to kruskal

    vector<pair<int, int>> Dijkstra = dijkstra(1); //Here we get the minimum distances from certain node to other ones using Dijkstra's algorithm

    vector<vector<int>> FloydWarhsall = floydWarshall(); //Here we get the minimum distances from all nodes to other ones using Floyd-Warshall's algorithm

    return 0;
}
