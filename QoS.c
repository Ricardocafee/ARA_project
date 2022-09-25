#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
// Define the maximum number of vertices in the graph
#define N 40
#define MAX_EDGES 100
#define SIZE 10000 //Size of the FIFO

void enqueue(int item);
bool dequeue ();

int nodes_count = 0; //Number of nodes
int Rear = -1; //index of top of the FIFO
int Front = -1; //index of bottom of the FIFO
int inp_arr[SIZE]; //FIFO
int no_solutions=0;
 
// Data structure to store a graph object
struct Graph
{
    // An array of pointers to Node to represent an adjacency list
    struct Node* head[N];
};
 
// Data structure to store adjacency list nodes of the graph
struct Node
{
    int dest, weight, length, depth;
    struct Node* next;
};
 
// Data structure to store a graph edge
struct Edge {
    int src, dest, weight, length;
};

struct Graph *graph;
 
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
        int depth = 0;
 
        // allocate a new node of adjacency list from src to dest
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = dest;
        newNode->weight = weight;
        newNode->length = length;
        newNode->depth = depth;
 
        // point new node to the current head
        newNode->next = graph->head[src];
 
        // point head pointer to the new node
        graph->head[src] = newNode;
    }
 
    return graph;
}
 
// Function to print adjacency list representation of a graph
void printGraph()
{

    for (int i = 0; i < N; i++)
    {
        // print current vertex and all its neighbors
        struct Node* ptr = graph->head[i];


        if(ptr==NULL && i > 0) break;

        nodes_count++;

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

int RecursiveFunction(int s, int d, bool visited[]){

    if(s == d)
    {
        no_solutions++;                         
        return 1;
    }
    

    struct Node* current_node = graph->head[s];
    visited[s] = true;
    int check=0;
    bool path_exists = false;


    while (current_node != NULL)
            {
                if(visited[current_node->dest] == false){
                    check = RecursiveFunction(current_node->dest, d, visited);
                }

                if(check>0)
                    {
                        path_exists = true;
                        
                        if(check > current_node->depth)
                            {
                                graph->head[s]->depth = check;
                            }
                        printf("Source - %d, current_node depth: %d, out-neighbor - %d\n",s,current_node->depth,current_node->dest);
                        
                    }
                current_node = current_node->next;
            }
    if(path_exists) return check+1;
    return 0;

}


int isReachable(int s, int d){


    //Vector with length equal to the number of nodes
    bool visited[nodes_count];

    //Initialization of vector
    for(int i = 0; i < nodes_count; i++){
        visited[i] = false;
    }


    int path_check;

    path_check = RecursiveFunction(s,d,visited);

    struct Node* ptr;
    int current_node;
    
    if(path_check > 0) return true;

    return false;
}

void merge (int arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r -m;


    //Create temp arrays
    int L[n1];
    int R[n2];

    int low = arr[l];
    int high = arr[r];

    //Copy data to temp array
    for(i = 0; i < n1; i++)
    {
        L[i] = arr[l+i];       
    }
    for(j = 0; j < n2; j++)
    {
        R[j] = arr[m+1+j];
    }

    //Merge the temp arrays
    i = 0;
    j = 0;
    k = l;

    while(i < n1 && j < n2)
    {
        if (graph->head[L[i]]->depth <= graph->head[R[j]]->depth)
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;           
        }
        k++;
    }

    //Copy the remaining element of L[]
    while(i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    //Copy the remaining elements of R[]
    while(j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void MergeSort(int arr[], int l, int r)
{
    int low = arr[l];
    int high = arr[r];


    if(l < r)
    {
        
        //Finding mid element
        int m = l + (r-l)/2;

        //Recursively sorting both the halves
        MergeSort(arr, l, m);
        MergeSort(arr,m+1,r);

        //Merge the array
        merge(arr, l, m, r);
    }
}
 
// Directed graph implementation in C
int main(void)
{
    struct Edge edges[MAX_EDGES];

    //Read file and extract number of edges
    int n = readFile(edges);
 
    // construct a graph from the given edges
    graph = createGraph(edges, n);
 
    // Function to print adjacency list representation of a graph
    printGraph();

    int s = 1;
    int d = 4;

    //isReachable function will check if there exists a path from the source to the destination 
    bool path_exists = isReachable(s,d);

    if(path_exists)
    {
        printf("\nIt is reachable.\n");
    }
    else
    {
        printf("\nIt is not reachable\n");
        return 0;
    }
    
    struct Node* ptr;
    int ordered_nodes[nodes_count];

    for(int j = 0; j < nodes_count; j++)
    {
        ordered_nodes[j]=0;
    }
    printf("\nNodes count: %d", nodes_count);


    for(int i = nodes_count-1; i > 0; i--){
        ptr = graph->head[i];
        printf("\nNode: %d, Degree of node - %d\n", i,ptr->depth);

        if(ptr->depth>0) enqueue(i);

    }
   /* printf("Longest path: %d ", d);
    for(int i = nodes_count-2; i >= 0; i--){
        if(ordered_nodes[i]>0){
            enqueue(i);
            printf("<- %d ", ordered_nodes[i]);
        }
    }
    */

    printf("Rear - %d", Rear);

    MergeSort(inp_arr, 0, Rear);

    for (int i = 0; i < 3; i++)
    {
        printf("\n%d",inp_arr[i]);
    }

    
    
 
    return 0;
}
