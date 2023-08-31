#include <stdio.h>
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <glob.h>

const int N = 5e5;
const int M = 8e8;

using namespace std;

struct edge
{
    int st;
    int end;
};
vector<int> graph[N];
void make_graph(edge *graph_edges, int n)
{
    for (int i = 0; i < n; i++)
    {
        graph[graph_edges[i].st].push_back(graph_edges[i].end);
        graph[graph_edges[i].end].push_back(graph_edges[i].st);
    }
    for (int i = 0; i < n; i++)
    {
        cout << i << " --- > ";
        for (auto it : graph[i])
        {
            cout << it << " ";
        }
        cout << endl;
    }
}
int main()
{
    key_t k1 = 235, k2 = 236, k3 = 237, k4 = 238 ;

    int shmid1 = shmget(k1, M, IPC_CREAT | 0666);
    edge *graph_edges = (edge *)shmat(shmid1, NULL, 0);

    int shmid3 = shmget(k2, N, IPC_CREAT | 0666);
    int *degrees = (int *)shmat(shmid3, NULL, 0);

    int shmid4 = shmget(k3, 1, IPC_CREAT | 0666);
    int *total_nodes = (int *)shmat(shmid4, NULL, 0);

    int shmid5 = shmget(k4, 1, IPC_CREAT | 0666);
    int *total_edges = (int *)shmat(shmid5, NULL, 0);

    *total_edges = 0;
    *total_nodes = 0;

    memset(graph_edges, 0, M);
    memset(degrees, 0, N);
    FILE *fp = fopen("graph.txt", "r");

    char read_line[100];
    int read_size;

    while ((fgets(read_line, 100, fp)) != NULL)
    {

        edge ed;
        sscanf(read_line, "%d %d", &ed.st, &ed.end);
        graph_edges[*total_edges] = ed;
        *total_edges = *total_edges + 1;
        *total_nodes = max(*total_nodes, ed.st);
        *total_nodes = max(*total_nodes, ed.end);
        degrees[ed.st]++;
        degrees[ed.end]++;
    }

    fclose(fp);


    int saved_stdout = dup(STDOUT_FILENO);
    if (fork() == 0)
    {
        string file = "producer_out.txt";
        // cout << file << endl ;
        int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
        dup2(fd, STDOUT_FILENO);
        char *args[] = {(char *)"./producer", (char *)to_string(k1).c_str() , (char *)to_string(k2).c_str() , (char *)to_string(k3).c_str() , (char *)to_string(k4).c_str() , (char *)NULL};
        execvp(*args, args);
        exit(EXIT_SUCCESS);
    }
    else
    {
        // wait(NULL);
        dup2(saved_stdout, STDOUT_FILENO);
        sleep(3);
        int consumer_count = 10;
        for (int i = 0; i < consumer_count; i++)
        {
            int pid = fork();
            if (pid == 0)
            {
                string file = "./consumer_out/consumer";
                file.push_back('0' + i);
                file += ".txt";
                // cout << file << endl ;
                int fd = open(file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, STDOUT_FILENO);
                // int fd = open(argv[outp].c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
                char *args[] = {(char *)"./consumer", (char *)to_string(k1).c_str() , (char *)to_string(k2).c_str() , (char *)to_string(k3).c_str() , (char *)to_string(k4).c_str() ,(char *)to_string(i + 1).c_str(), (char *)NULL};
                execvp(*args, args);
                exit(EXIT_SUCCESS);
            }
            else
            {
                // wait(NULL);
                dup2(saved_stdout, STDOUT_FILENO);
            }
        }
    }
    for (int i = 0; i < 1; i++)
        wait(NULL);
    shmdt(graph_edges);
    shmdt(total_nodes);
    shmdt(total_edges);
    shmdt(degrees);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    shmctl(shmid4, IPC_RMID, NULL);
    shmctl(shmid5, IPC_RMID, NULL);
    return 0;
}