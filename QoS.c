#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
// Define the maximum number of vertices in the graph
#define N 40
#define MAX_EDGES 100
#define SIZE 10000 //Size of the FIFO

void enqueue(int item);
bool dequeue ();

int nodes_count = 0;
int Rear = -1;
int Front = -1;
int inp_arr[SIZE];
 
// Data structure to store a graph object
struct Graph
{
    // An array of pointers to Node to represent an adjacency list
    struct Node* head[N];
};
 
// Data structure to store adjacency list nodes of the graph
struct Node
{
    int dest, weight, length;
    struct Node* next;
};
 
// Data structure to store a graph edge
struct Edge {
    int src, dest, weight, length;
};
 
// Function to create an adjacency list from specified edges
struct Graph* createGraph(struct Edge edges[], int n)
{
    // allocate storage for the graph data structure
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
 
    // initialize head pointer for all vertices
    for (int i = 0; i < N; i++) {
        graph->head[i] = NULL;
    }
 
    // add edges to the directed graph one by one
    for (int i = 0; i < n; i++)
    {
        // get the source and destination vertex
        int src = edges[i].src;
        int dest = edges[i].dest;
        int weight = edges[i].weight;
        int length = edges[i].length;
 
        // allocate a new node of adjacency list from src to dest
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = dest;
        newNode->weight = weight;
        newNode->length = length;
 
        // point new node to the current head
        newNode->next = graph->head[src];
 
        // point head pointer to the new node
        graph->head[src] = newNode;
    }
 
    return graph;
}
 
// Function to print adjacency list representation of a graph
void printGraph(struct Graph* graph)
{

    for (int i = 0; i < N; i++)
    {
        // print current vertex and all its neighbors
        struct Node* ptr = graph->head[i];
        nodes_count++;


        if(ptr==NULL && i > 0) break;


        while (ptr != NULL)
        {
            printf("%d —> %d (%d,%d)\t", i, ptr->dest, ptr->weight, ptr->length);
            ptr = ptr->next;
        }
 
        printf("\n");
    }
}

int readFile(struct Edge edges[])
{
    FILE *fp;
    int count= 0; //column counter
    int line = 0; //Line counter
    char filename[] = "file_input";
    char c; //To store a character read from a file
    int num;


    fp = fopen(filename, "r");

    //Check if file exists
    if(fp == NULL) {
        printf("Could not open file %s", filename);
        return 0;
    }

    //Store values according to their field

    while(fscanf(fp, " %d", &num) == 1) {
        count = count + 1;

        if(!num) break;

        switch (count)
        {
            //Store values in their corresponding fields
        case 1:
            edges[line].src = num;
            break;
        case 2:
            edges[line].dest = num;
            break;       
        case 3:
            edges[line].weight = num;
            break;
        case 4:
            edges[line].length = num;
            break;
        
        default:
            break;
        }

    //If 4 parameters was read, then increment number of edges read
        if(count == 4)
            {
                line++;
                count = 0;
            }
        }

    //Close the file
    fclose(fp);
    return line;
    
}

void enqueue(int item)
{
    if(Rear == SIZE - 1){
        printf("Overflow \n");
    } 
    else
    {
        if (Front == -1)
            {
                Front = 0;
            }

        Rear = Rear + 1;
        inp_arr[Rear] = item;
    }

}

bool dequeue(){
    if (Front == -1 || Front > Rear){
       // printf("Underflow\n");
        return true;
    }
    else
    {
        Front = Front + 1;
        return false;
    }
}


bool isReachable(int s, int d, struct Graph* graph){

    //Source is the same as destination
    if(s == d){
        return true;
    }

    bool visited[nodes_count];

    for(int i = 0; i < nodes_count; i++){
        visited[i] = false;
    }

    visited[s] = true;

    struct Node* ptr;
    int current_node;

    enqueue(s);

    while(1)
    {
        current_node = inp_arr[Front];
        //printf("Current node: %d\n", current_node);

        if (ptr = graph->head[current_node])
        {
            //printf("Visited node: %d\n", ptr->dest);
        }
        else
        {
            if(dequeue())
            {
                return false;
            }
            continue;
        }
        
    
        while (ptr != NULL)
            {
                //printf("Visited (yes or no) - %d\n", visited[ptr->dest]);
                if(visited[ptr->dest] == false)
                {
                    
                    if (ptr->dest == d)
                    {
                        return true;
                    }
                    
                    enqueue(ptr->dest);               
                }

                ptr = ptr->next;
            }

        if(dequeue())
        {
            return false;
        }
    }

}
 
// Directed graph implementation in C
int main(void)
{
    struct Edge edges[MAX_EDGES];

    //Read file and extract number of edges
    int n = readFile(edges);
 
    // construct a graph from the given edges
    struct Graph *graph = createGraph(edges, n);
 
    // Function to print adjacency list representation of a graph
    printGraph(graph);

    bool btt = isReachable(1,2,graph);

    if(btt)
    {
        printf("\nIt is reachable\n");
    }
    else
    {
        printf("\nIt is not reachable\n");
    }
 
    return 0;
}
