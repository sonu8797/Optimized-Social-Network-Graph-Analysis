#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>

const int N = 5e5;
const int M = 8e8;

using namespace std;

struct edge
{
    int st;
    int end;
};

vector<int> graph[N];
void make_graph(edge *graph_edges, int n )
{
    int max_node = -1;
    for (int i = 0; i < n; i++)
    {
        graph[graph_edges[i].st].push_back(graph_edges[i].end);
        graph[graph_edges[i].end].push_back(graph_edges[i].st);
    }
    
}
void print_graph( int m )
{
    for (int i = 0; i < m; i++)
    {
        cout << i << " --- > ";
        for (auto it : graph[i])
        {
            cout << it << " ";
        }
        cout << endl;
    }
}
int main( int argc , char **argv )
{
    srand(time(NULL));
    int shmid1 = shmget(atoi(argv[1]), M, IPC_CREAT | 0666);
    edge *graph_edges = (edge *)shmat(shmid1, NULL, 0);

    int shmid3 = shmget(atoi(argv[2]), N, IPC_CREAT | 0666);
    int *degrees = (int *)shmat(shmid3, NULL, 0);

    int shmid4 = shmget(atoi(argv[3]), 1, IPC_CREAT | 0666);
    int *total_nodes = (int *)shmat(shmid4, NULL, 0);

    int shmid5 = shmget(atoi(argv[4]), 1, IPC_CREAT | 0666);
    int *total_edges = (int *)shmat(shmid5, NULL, 0);
    make_graph( graph_edges , *total_edges  ) ;
    print_graph(*total_nodes) ;
    for( int i = 0 ; i < 50 ; i++ )cout << endl ;
    while (1)
    {
        int m = rand() % 21 + 10;
        for (int i = 0; i < m; i++)
        {
            int k = rand() % 20 + 1;
            int curr_ver = *total_nodes + 1;
            vector<double> probabilities;
            for (int i = 0; i < *total_nodes; i++)
            {
                double prob = degrees[i] * (0.5) / (*total_edges);
                probabilities.push_back(prob);
            }
            std::random_device rd;
            std::mt19937 gen(rd());
            std::discrete_distribution<> d(probabilities.begin(), probabilities.end());
            unordered_set<int> numbers;
            while (numbers.size() < k)
            {
                int number = d(gen);
                if (numbers.find(number) == numbers.end())
                {
                    numbers.insert(number);
                }
            }
            for (auto it : numbers)
            {
                // cout << it << " ";
                edge ed;
                ed.st = it;
                ed.end = curr_ver;
                graph_edges[(*total_edges)++] = ed;
                graph[ed.st].push_back(ed.end) ;
                graph[ed.end].push_back(ed.st) ;
                degrees[ed.st]++;
                degrees[ed.end]++;
            }
           *total_nodes = *total_nodes + 1 ;
        }
        print_graph(*total_nodes) ;
        for( int i = 0 ; i < 50 ; i++ )cout <<  endl ;
        sleep(50) ;
    }
    shmdt(graph_edges);
    shmdt(total_nodes);
    shmdt(degrees);

    return 0;
}