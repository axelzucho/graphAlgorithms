#include "stdafx.h"
#include "disjointset.h"

#include <iostream>
#include <vector>
#include <queue>
#include <chrono>
#include <map>
#include <stack>

using namespace std;
using namespace std::chrono;
typedef TPt<TNEANet> PNGraph;
typedef pair<int, int> customPair;

struct oper{
    bool operator()(const std::tuple<int,int,int>& one, const std::tuple<int,int,int>& two)
    {
        return std::get<0>(one) > std::get<0>(two);
    }
};


void insertVertex(int value, PNGraph  &g) {
    g.AddNode(value);
}

void insertEdge(int a, int b, int weight, PNGraph &g) {
    g.AddEdge(a,b);
    TNEANet::TEdgeI EI = g->GetEI(a,b);
    g.AddIntAttrDatE(EI, weight, "weight");
}

void deleteVertex(int value, PNGraph &g) {
    g->DelNode(value);
}

void deleteEdge(int a, int b, PNGraph &g) {
    g->DelEdge(a,b);
}



void dfs(PNGraph * g, int startingNode) {
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

void bfs(PNGraph * g, int startingNode){
    std::queue<int> visiting;
    map<int,bool> travelled;
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

vector<pair<int, int>> prim(PNGraph * g, int vertexStart){
    vector<pair<int, int>> result;
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    map<int,bool> travelled;
    std::tuple<int,int,int> now;
    int weight;
    pair<v_it,v_it> iterators = vertices(g);
    int checking, target, weight;
    for(int i = 0; i < g->GetNodes; ++i){
        result.push_back(make_pair(-1, INT32_MAX));
    }
    result[checking].second = 0;
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
            next.push(make_tuple(weight,target,checking));
        }

    }
    return result;
}

vector<pair<int, int>> dijkstra(PNGraph * g, int vertexStart){
    vector<pair<int, int>> result;
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    map<int,bool> travelled;
    std::tuple<int,int,int> now;
    int weight;
    pair<v_it,v_it> iterators = vertices(g);
    int checking, target, weight;
    for(int i = 0; i < g->GetNodes; ++i){
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



vector<vector<int>> floydWarshall(PNGraph * g){
    vector<vector<int>> distances;
    unsigned long vNum = g->GetNodes();
    distances.resize(vNum);
    pair<v_it,v_it> iterators = vertices(g);
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

vector<std::tuple<int, int, int>> kruskal (PNGraph * g) {
    priority_queue<std::tuple<int,int,int>,vector<std::tuple<int,int,int>>, oper> next;
    int vertex = g->GetNodes();
    vector<std::tuple<int, int, int>> result;
    int origin, target, set_o, set_t;
    std::tuple<int,int,int> now;
    DisjointSets ds(vertex);
    for (TNEANet::TEdgeI EI = g->BegEI(); EI < g->EndEI(); EI++) {
        origin = EI.GetSrcNId();
        target = EI.GetDstNId();
        next.push(g->GetIntAttrDatE(EI, "weight"),origin,target);
        int set_u = ds.find(origin);
        int set_v = ds.find(target);
        if (set_u != set_v){
            cout << u << " - " << v << endl;
            ds.merge(set_u, set_v, );
        }
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

    PNGraph g = TNEANet::New();
    //Here we are inserting the edges of the graph provided
    insertEdge(1, 3, 8, &g);
    insertEdge(1, 4, 8, &g);
    insertEdge(3, 2, 7, &g);
    insertEdge(3, 10, 4,&g);
    insertEdge(3, 5, 8, &g);
    insertEdge(2, 5, 7, &g);
    insertEdge(4, 8, 2, &g);
    insertEdge(4, 7, 3, &g);
    insertEdge(4, 5, 1, &g);
    insertEdge(5, 6, 9, &g);
    insertEdge(6, 13, 4, &g);
    insertEdge(7, 4, 6, &g);
    insertEdge(8, 7, 3, &g);
    insertEdge(8, 9, 3, &g);
    insertEdge(9, 10, 2, &g);
    insertEdge(9, 12, 4, &g);
    insertEdge(10, 3, 10, &g);
    insertEdge(10, 6, 6, &g);
    insertEdge(11, 12, 6, &g);
    insertEdge(12, 9, 2, &g);
    insertEdge(12, 11, 8, &g);
    insertEdge(12, 14, 9, &g);
    insertEdge(13, 14, 6, &g);
    insertEdge(14, 13, 12, &g);
    /*
    insertVertex(g); //Here we insert a vertex
    insertEdge(1,5,3,g); //Here we insert an edge from vertex 1 to 5 with weight 3
    deleteVertex(15,g); //Here we delete the vertex 15
    deleteEdge(1,5,g); //Here we delete the edge going from Vertex 1 to 5.
    */
    dfs(&g, 1); //Here we do a depth-first search, that will print the path

    bfs(&g, 1); //Here we do a breath-first search, that will print the path

    vector<pair<int, int>> Prim = prim(&g, 1); //Here we get a vector of pairs, telling us the minimum spanning tree according to prim

    vector<std::tuple<int, int, int>> Kruskal = kruskal(&g); //Here we get a tuple of vertex and int, telling us the minimum spanning tree according to kruskal

    vector<pair<int, int>> Dijkstra = dijkstra(&g,1); //Here we get the minimum distances from certain node to other ones using Dijkstra's algorithm

    vector<vector<int>> FloydWarhsall = floydWarshall(&g); //Here we get the minimum distances from all nodes to other ones using Floyd-Warshall's algorithm

    return 0;
}