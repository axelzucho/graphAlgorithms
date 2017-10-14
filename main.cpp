#include <boost/graph/adjacency_list.hpp>
#include <iostream>
#include <stack>
#include <map>
#include <queue>
#include <boost/graph/detail/adjacency_list.hpp>
#include <tuple>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <chrono>

typedef boost::property<boost::edge_weight_t, int> EdgeWeightProperty;
typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, boost::no_property, EdgeWeightProperty > DirectedGraph;
typedef boost::graph_traits<DirectedGraph>::out_edge_iterator oe_it;
typedef boost::graph_traits<DirectedGraph>::edge_iterator e_it;
typedef boost::graph_traits<DirectedGraph>::vertex_iterator v_it;
typedef boost::graph_traits<DirectedGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<DirectedGraph>::edge_descriptor Edge;

DirectedGraph g, f;

using namespace std;
using namespace boost;
using namespace chrono;
struct oper{
    bool operator()(const std::tuple<int,Vertex, Vertex>& one, const std::tuple<int,Vertex, Vertex>& two)
    {
        return std::get<0>(one) > std::get<0>(two);
    }
};

struct oper2{
    bool operator()(const Edge& one, const Edge& two){
        return boost::get(boost::edge_weight_t(), g, one) > boost::get(boost::edge_weight_t(), g, two);
    }
};

void insertVertex(DirectedGraph a){
    add_vertex(a);
}

void insertEdgeUndirected(unsigned long s, unsigned long d, int w, DirectedGraph &a){
    add_edge(s,d,w,a);
    add_edge(d,s,w,a);
}

void insertEdge(unsigned long s, unsigned long d, int w, DirectedGraph &a){
    add_edge(s,d,w,a);
}

void deleteVertex(unsigned long v, DirectedGraph &a){
    remove_vertex(v,a);
}

void deleteEdge(unsigned long s, unsigned long d, DirectedGraph &a){
    remove_edge(s,d,a);
}

void dfs(DirectedGraph &a, unsigned long startingNode){
    stack<Vertex> visiting;
    map<Vertex,bool> travelled;
    Vertex checking = a.vertex_set()[startingNode];
    visiting.push(checking);
    while(!visiting.empty()){
        checking = visiting.top();
        visiting.pop();
        pair<oe_it,oe_it> iterators = out_edges(checking,a);
        for(oe_it it = iterators.first; it != iterators.second; ++it){
            Vertex target = boost::target(*it,a);
            if(travelled[target]) continue;
            travelled[target] = true;
            visiting.push(target);
        }
        cout << checking << "\n";

    }

}

void bfs(DirectedGraph &a, unsigned long startingNode){
    std::queue<Vertex> visiting;
    map<Vertex,bool> travelled;
    Vertex checking = a.vertex_set()[startingNode];
    visiting.push(checking);
    while(!visiting.empty()){
        checking = visiting.front();
        visiting.pop();
        pair<oe_it,oe_it> iterators = out_edges(checking,a);
        for(oe_it it = iterators.first; it != iterators.second; ++it){
            Vertex target = boost::target(*it,a);
            if(travelled[target]) continue;
            travelled[target] = true;
            visiting.push(target);
        }
        cout << checking << "\n";

    }

}

vector<pair<Vertex, int>> prim(DirectedGraph g, int vertexStart){
    vector<pair<Vertex, int>> result;
    priority_queue<std::tuple<int,Vertex,Vertex>,vector<std::tuple<int,Vertex, Vertex>>, oper> next;
    map<Vertex,bool> travelled;
    std::tuple<int,Vertex,Vertex> now;
    int weight;
    pair<v_it,v_it> iterators = vertices(g);
    Vertex checking = g.vertex_set()[vertexStart];
    for(v_it it = iterators.first; it != iterators.second; ++it){
        result.push_back(make_pair(-1, INT32_MIN));
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
        pair<oe_it,oe_it> oeIt = out_edges(checking,g);
        for(oe_it it = oeIt.first; it != oeIt.second; ++it){
            Vertex target = boost::target(*it,g);
            if(travelled[target]) continue;
            //std::pair<Edge,bool> a = boost::edge(checking,std::get<2>(now), g);
            weight = boost::get(boost::edge_weight_t(),g, *it);
            next.push(make_tuple(weight,target,checking));
        }

    }
    return result;

}

vector<std::tuple<Vertex, Vertex, int>> kruskal(DirectedGraph g){
    vector<vector<int>*> travelled;
    vector<std::tuple<Vertex, Vertex, int>> result;
    priority_queue<Edge, vector<Edge>, oper2> next;
    pair<v_it,v_it> iterators = vertices(g);
    int counter = 0;
    Edge checking;
    Vertex source, target;
    for(v_it it = iterators.first; it != iterators.second; ++it){
        vector<int>* a = new vector<int>;
        a->push_back(counter);
        travelled.push_back(a);
        //result.push_back(make_tuple(source, source, INT32_MIN));
        ++counter;
    }
    pair<e_it,e_it> edIt = edges(g);
    for(e_it it = edIt.first; it != edIt.second; ++it){
        std::pair<Edge,bool> a = boost::edge(boost::source(*it,g),boost::target(*it,g), g);
        next.push(a.first);
    }
    while(!next.empty()){
        checking = next.top();
        source = boost::source(checking,g);
        target = boost::target(checking,g);
        next.pop();
        if(*travelled[source] == *travelled[target]) continue;
        vector<int> add;
        for(vector<int>::iterator it = travelled[target]->begin(); it != travelled[target]->end(); ++it){
            travelled[source]->push_back(*it);
            add.push_back(*it);
        }
        for(vector<int>::iterator it = add.begin(); it != add.end(); ++it){
            travelled[*it] = travelled[source];
        }
        /*
        for(int i = 0; i < travelled.size(); ++i){
            for(vector<int>::iterator it = travelled[i]->begin(); it != travelled[i]->end(); ++it){
                cout << *it << " ";
            }
            cout << "\n";
        }*/
        result.push_back(make_tuple(source,target, boost::get(boost::edge_weight_t(), g, checking)));
    }
    return result;

}

vector<pair<int, Vertex>> dijkstra(DirectedGraph g, unsigned long initialNode){
    vector<pair<int, Vertex>> distances;
    map<Vertex, bool> visited;
    priority_queue<std::tuple<int,Vertex,Vertex>, vector<std::tuple<int,Vertex,Vertex>>,oper> next;
    std::tuple<int,Vertex,Vertex> checking;
    Vertex source, target;
    int weight, checkingWeight;
    pair<v_it,v_it> iterators = vertices(g);
    for(v_it it = iterators.first; it != iterators.second; ++it){
        distances.push_back(make_pair(INT_MAX, -1));
    }
    distances[initialNode].first = 0;
    next.push(std::make_tuple(0,initialNode,initialNode));
    while(!next.empty()){
        checking = next.top();
        next.pop();
        target = std::get<2>(checking);
        source = std::get<1>(checking);
        if(visited[target]) continue;
        visited[target] = true;
        checkingWeight = std::get<0>(checking);
        if(distances[target].first < checkingWeight) continue;
        distances[target] = make_pair(checkingWeight,source);
        pair<oe_it,oe_it> oeIt = out_edges(target,g);
        for(oe_it it = oeIt.first; it != oeIt.second; ++it){
            Vertex newTarget = boost::target(*it,g);
            if(visited[newTarget]) continue;
            weight = boost::get(boost::edge_weight_t(),g, *it);
            next.push(make_tuple(distances[target].first + weight,target,newTarget));
        }
    }

    return distances;

}

vector<vector<int>> floydWarshall(DirectedGraph g){
    vector<vector<int>> distances;
    unsigned long vNum = num_vertices(g);
    distances.resize(vNum);
    pair<v_it,v_it> iterators = vertices(g);
    for(v_it it = iterators.first; it != iterators.second; ++it){
        for(v_it it2 = iterators.first; it2 != iterators.second; ++it2){
            if(it == it2){
                distances[*it].push_back(0);
                continue;
            }
            else distances[*it].push_back(100000);
        }
    }
    pair<e_it,e_it> edIt = edges(g);
    for(e_it it = edIt.first; it != edIt.second; ++it){
        distances[boost::source(*it,g)][boost::target(*it,g)] = boost::get(boost::edge_weight_t(),g, *it);
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

int main()
{
    insertEdge(1, 3, 8, g);
    insertEdge(1, 4, 8, g);
    insertEdge(3, 2, 7, g);
    insertEdge(3, 10, 4, g);
    insertEdge(3, 5, 8, g);
    insertEdge(2, 5, 7, g);
    insertEdge(4, 8, 2, g);
    insertEdge(4, 7, 3, g);
    insertEdge(4, 5, 1, g);
    insertEdge(5, 6, 9, g);
    insertEdge(6, 13, 4, g);
    insertEdge(7, 4, 6, g);
    insertEdge(8, 7, 3, g);
    insertEdge(8, 9, 3, g);
    insertEdge(9, 10, 2, g);
    insertEdge(9, 12, 4, g);
    insertEdge(10, 3, 10, g);
    insertEdge(10, 6, 6, g);
    insertEdge(11, 12, 6, g);
    insertEdge(12, 9, 2, g);
    insertEdge(12, 11, 8, g);
    insertEdge(12, 14, 9, g);
    insertEdge(13, 14, 6, g);
    insertEdge(14, 13, 12, g);

    cout << "Inserting vertex:\n";
    auto t1_a = high_resolution_clock::now();
    insertVertex(g);
    auto t2_a = high_resolution_clock::now();
    auto duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Inserting edge:\n";
    t1_a = high_resolution_clock::now();
    insertEdge(1,5,3,g);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Deleting vertex:\n";
    t1_a = high_resolution_clock::now();
    deleteVertex(15,g);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Deleting edge:\n";
    t1_a = high_resolution_clock::now();
    deleteEdge(1,5,g);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";
/*
    cout << "DFS:\n";
    auto t1_a = high_resolution_clock::now();
    dfs(g, 1);
    auto t2_a = high_resolution_clock::now();
    auto duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "BFS:\n";
    t1_a = high_resolution_clock::now();
    bfs(g, 1);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Prim:\n";
    t1_a = high_resolution_clock::now();
    prim(g, 1);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Kruskal:\n";
    t1_a = high_resolution_clock::now();
    kruskal(g);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Dijkstra:\n";
    t1_a = high_resolution_clock::now();
    dijkstra(g,1);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

    cout << "Floyd-Warshall:\n";
    t1_a = high_resolution_clock::now();
    floydWarshall(g);
    t2_a = high_resolution_clock::now();
    duration_a = duration_cast<microseconds>( t2_a - t1_a ).count();
    cout << "Duration = " << duration_a << " micros\n";

*/
    return 0;
}