#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

const int N = 5e5;
const int M = 8e8;
int start_node;
int end_node;
using namespace std;

struct edge
{
    int st;
    int end;
};
// vector<int> graph[N];

bool check(edge e)
{
    if (e.st > end_node || e.st < start_node)
        return false;
    if (e.end > end_node || e.end < start_node)
        return false;
    return true;
}
void make_graph(edge *graph_edges, int n, map<int, vector<int>> &graph)
{
    for (int i = 0; i < n; i++)
    {
        if (check(graph_edges[i]))
        {
            graph[graph_edges[i].st].push_back(graph_edges[i].end);
            graph[graph_edges[i].end].push_back(graph_edges[i].st);
        }
    }
}

void dijikstra(int src, map<int, vector<int>> graph)
{
    int n = end_node - start_node + 1;
    map<int, int> parent;
    map<int, int> distance;
    map<int, bool> vis;
    distance[src] = 0;
    for (int i = start_node; i <= end_node; i++)
    {
        if (i == src)
            continue;
        distance[i] = INT_MAX;
        vis[i] = false;
    }
    set<pair<int, int>> pq;
    pq.insert({0, src});
    auto it1 = pq.begin();
    // cout << it1->first << " " << it1->second << endl ;
    while (!pq.empty())
    {
        auto it = pq.begin();
        pq.erase(it);
        int v = it->second;
        int d = it->first;
        // cout << v << " " << d << endl ;
        if (vis[v])
            continue;
        vis[v] = true;
        for (auto child : graph[v])
        {
            if (child == v)
                continue;
            if (distance[child] > (d + 1))
            {
                // cout << " aese hi " << it->first << " " <<  distance[child] << endl ;
                distance[child] = d + 1;
                parent[child] = v;
                pq.insert({d + 1, child});
            }
        }
    }
    for (int i = start_node; i <= end_node; i++)
    {
        if (src >= i)
            continue;
        if (distance[i] == INT_MAX)
        {
            // cout << "There is no path between " << src << " and " << i << endl << endl  ;
            continue;
        }
        cout << " Distance from " << src << " to " << i << " is : " << distance[i] << endl;
        cout << " Path : ";
        int k = i;
        while (k != src)
        {
            cout << k << " -> ";
            k = parent[k];
        }
        cout << src << endl
             << endl;
        // cout << i << "--> " << distance[i] <<  endl ;
    }
}
int main(int argc, char **argv)
{
    int shmid1 = shmget(atoi(argv[1]), M, IPC_CREAT | 0666);
    edge *graph_edges = (edge *)shmat(shmid1, NULL, 0);

    int shmid3 = shmget(atoi(argv[2]), N, IPC_CREAT | 0666);
    int *degrees = (int *)shmat(shmid3, NULL, 0);

    int shmid4 = shmget(atoi(argv[3]), 1, IPC_CREAT | 0666);
    int *total_nodes = (int *)shmat(shmid4, NULL, 0);

    int shmid5 = shmget(atoi(argv[4]), 1, IPC_CREAT | 0666);
    int *total_edges = (int *)shmat(shmid5, NULL, 0);
    // cout << " Total nodes  : " << *total_nodes << endl ;
    while (1)
    {
        start_node = (((atoi(argv[5]) - 1) * (*total_nodes + 1))) / 10;
        end_node = ((atoi(argv[5]) * (*total_nodes + 1))) / 10 - 1;

        // cout << start_node << " " << end_node << endl ;
        map<int, vector<int>> graph;
        make_graph(graph_edges, *total_edges, graph);
        for (int i = start_node; i < end_node; i++)
        {
            dijikstra(i, graph);
        }
        for( int i = 0 ; i < 50 ; i++ )cout << endl ;
        sleep(30) ;
    }

    // dijikstra( start_node + 1 , graph) ;
    shmdt(graph_edges);
    shmdt(total_nodes);
    shmdt(total_edges);
    shmdt(degrees);
    return 0;
}