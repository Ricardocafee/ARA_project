#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
 
// Define the maximum number of vertices in the graph
#define N 5000
#define MAX_EDGES 100
#define SIZE 5000 //Size of the FIFO

void enqueue(int item);  //Enqueue elements on the stack
bool dequeue ();  //Dequeue

int nodes_count = 0; //Number of nodes
int Rear = -1; //index of top of the FIFO
int Front = -1; //index of bottom of the FIFO
int inp_arr[SIZE]; //FIFO
int no_solutions=0;
bool short_wide = false;  //False - Widest-shortest; True - Shortest-widest
 
// Data structure to store a graph object
struct Graph
{
    // An array of pointers to Node to represent an adjacency list
    struct Node* head[N];
};
 
// Data structure to store adjacency list nodes of the graph
struct Node
{
    int dest, width, length;
    struct Node* next;
};


//Will store the status of the node. The most preferred path (width, length) 
//from the dest (source in routing messages) until it.

//If not visited yet, it will administrate the length and width by the first routing message it received
///If visisted, a comparison among the links to the out-neighbors will be needed to define the best path

struct width_length  
{
    int width, length;   //Width and length attributes
    bool visited;  
};
 
// Data structure to store a graph edge
struct Edge {
    int src, dest, width, length;
};


struct Graph *graph;     //Backwards graph
struct Graph *graph_out;  //Original graph

struct width_length wl[N];  //Will store the width and length of the best path (According to the order) in the node

 
// Function to create an adjacency list from specified edges
struct Graph* createGraph(struct Edge edges[], int n, bool check)
{
    // allocate storage for the graph data structure
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
  
 
    // initialize head pointer for all vertices
    for (int i = 0; i < N; i++) {
        graph->head[i] = NULL;
        wl[i].visited=false;
        wl[i].length=0;
        wl[i].width=0;
    }


 
    // add edges to the directed graph one by one
    for (int i = 0; i < n; i++)
    {
        int src, dest;
        // get the source and destination vertex
        //Check is a boolean variable which assigns the creation of the backtrack graph or the original graph

        if(check == false) //If false -> backwards graph
        {
            src = edges[i].dest; //Change of direction since routing messages have opposite direction
            dest = edges[i].src;
        }
        else  //If true, original graph
        {
            src = edges[i].src; //Maintain the direction of the links
            dest = edges[i].dest;
        }
        
        int width = edges[i].width;
        int length = edges[i].length;

 
        // allocate a new node of adjacency list from src to dest
        struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
        newNode->dest = dest;
        newNode->width = width;
        newNode->length = length;
 
        // point new node to the current head
        newNode->next = graph->head[src];
 
        // point head pointer to the new node
        graph->head[src] = newNode;
    }
 
    return graph;
}
 
// Function to print adjacency list representation of a graph
void printGraph(int n)
{
    //Firstly, the backtrack graph
    printf("\nBacktrack Graph\n");

    for (int i = 0; i < n; i++)
    {

        // print current vertex and all its neighbors
        struct Node* ptr = graph->head[i];


        if(ptr==NULL && i > 0) continue;

        nodes_count++;

        while (ptr != NULL)
        {
            printf("%d —> %d (%d,%d)\t", i, ptr->dest, ptr->width, ptr->length);
            ptr = ptr->next;
        }
 
        printf("\n");
    }

    //Secondly, the original graph
    printf("\nOriginal Graph\n");

    for (int i = 0; i < n; i++)
    {

        // print current vertex and all its neighbors
        struct Node* ptr = graph_out->head[i];


        if(ptr==NULL && i > 0) continue;

        while (ptr != NULL)
        {
            printf("%d —> %d (%d,%d)\t", i, ptr->dest, ptr->width, ptr->length);
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
            edges[line].width = num;
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

//Function responsible for enqueuing the elements on the FIFO
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


//Dequeing the FIFO stack
bool dequeue(){
    if (Front == -1 || Front > Rear){
       // printf("Underflow\n");
        return false;
    }
    else
    {
        Front = Front + 1;
        return true;
    }
}


//This function will assign the computed length and width of the considerated link in the original graph.
//It will administrate the concatenation of the information received by the routing message to the head node
//with the length and width of the link considerated

//Is useful for the current node to decide which is the best path by comparing the links to the out-neighbors

void assign_linkOut(int source, int dest, int length, int width)
{
    struct Node* ptr = graph_out->head[source];
    int d;

    while(ptr != NULL)  //For loop to assign the width and length computed to the right link
    {
        d = ptr->dest;
        if(dest == d)  //Link was found
        {
            ptr->length=length;
            ptr->width=width;
            break;
        }
        ptr=ptr->next;
    }
}

//Function responsible for comparing the links to the out-neighbors based on the routing messages received
//and find the best path according to the order choosen (Shortest-widest)

bool compareLinks_sw(int source){  

struct Node* ptr = graph_out->head[source];

    int i = 0;
    int min_length;
    int wide_width;
    int d;

    while(ptr != NULL)
    {
        d = ptr->dest;
        if(wl[d].visited == false) //If that node [d] didn't received any routing message, discard
        {
            ptr = ptr->next;
            continue;
        }

        if(i == 0) //If it is the first link analyzed, assign its length and width to the node
        {
            min_length = ptr->length;
            wide_width = ptr->width;
            i = i+1;
            ptr = ptr->next;
            continue;
        }

        //Comparison between links. 
        if(ptr->width > wide_width)  //Maximum with is prioritized
        {
            min_length=ptr->length;
            wide_width=ptr->width;
        }
        else if(ptr->width == wide_width) //In case of equality, choose the shortest one
        {
            if(ptr->length < min_length)
            {
                min_length = ptr->length;
            }
        }
        ptr = ptr->next;
    }


    //In case any of the attributes was changed, update its state and inform the in-neighbors

    if(wl[source].length != min_length || wl[source].width != wide_width)
    {
        wl[source].length = min_length;
        wl[source].width = wide_width;
        return true;
    }
    else return false; //Otherwise, state will remain the same
}

//Function responsible for comparing the links to the out-neighbors based on the routing messages received
//and find the best path according to the order choosen (Widest-shortest)

bool compareLinks_ws(int source){

struct Node* ptr = graph_out->head[source];

    int i = 0;
    int min_length;
    int wide_width;
    int d;

    while(ptr != NULL)
    {
        d = ptr->dest;
        if(wl[d].visited == false) //If that node [d] didn't received any routing message, discard
        {
            ptr = ptr->next;
            continue;
        }

        if(i == 0)  //If it is the first link analyzed, assign its length and width to the node
        {
            min_length = ptr->length;
            wide_width = ptr->width;
            i = i+1;
            ptr = ptr->next;
            continue;
        }

        //Comparison between links. 
        if(ptr->length < min_length)  //Shortest path is prioritized
        {
            min_length=ptr->length;
            wide_width=ptr->width;
        }
        else if(ptr->length == min_length)  //In case of equality, choose the widest one
        {
            if(ptr->width > wide_width)
            {
                wide_width = ptr->width;
            }
        }
        ptr = ptr->next;
    }


    //Function responsible for comparing the links to the out-neighbors based on the routing messages received
    //and find the best path according to the order choosen (Widest-shortest)

    if(wl[source].length != min_length || wl[source].width != wide_width)
    {
        wl[source].length = min_length;
        wl[source].width = wide_width;
        return true;
    }
    else return false;
}

//Main function responsible for the sending and receiving of routing message and state's update of nodes

void shortest_widest(int index, int s, int d){

    struct Node* ptr = graph->head[index];
    

    int width_node = wl[index].width;  //Width of the node computed based on the links to the out-neighbors
    int length_node = wl[index].length; //Length of the node computed based on the links to the out-neighbors
    int compute_width, compute_length=0;  //Variables that will compute the width and length that will be send in the routing messages
 
    wl[s].visited=0;  
    

    while (ptr != NULL)
        {
            int dest = ptr->dest;  //Node which will receive the routing message
            bool check = false;  //Will verify if status of node has changed
            compute_length = wl[index].length+ptr->length; //Length's sum between out-neighbor and link

            //Will compute the with of the path (between out-neighbor and link)
            if(index == s)
            {
                compute_width = ptr->width;  //If it is the link from the destination to the previous node
            }
            else
            {
               
                if(wl[index].width <= ptr->width)
                {
                    
                    compute_width = wl[index].width;
                    
                }
                else
                {
                    compute_width = ptr->width;
                }
            }
            //Assign these parameters to the links of the original graph
            //Later on, they will be compared to find the best path

            assign_linkOut(dest, index, compute_length, compute_width); 


            //If node was not visited yet, administrate the computed length and width of the first routing message
            if(wl[dest].visited == false) 
            {

                wl[dest].visited = true;
                wl[dest].length = compute_length;
                wl[dest].width = compute_width;
                

                printf("\n%d -> %d width -> (%d, %d) and (%d, %d)\n",index, dest, compute_width,compute_length, wl[dest].width, wl[dest].length);

                enqueue(dest);      //Added to the queue to be analyzed later
                ptr = ptr->next;  //Analyze link with the next in-neighbor of the current node

                continue;

            }
            else
            {
                
                if(short_wide)  //For Shortest_widest order
                {
                    check = compareLinks_sw(dest);
                }
                else    //For Widest-shortest order
                {
                    check = compareLinks_ws(dest);
                }

                if(check) enqueue(dest);  //If status has changed, it needs to informs its in-neighbors

                printf("\n%d -> %d width -> (%d, %d) and (%d, %d)\n",index, dest, compute_width, compute_length, wl[dest].width, wl[dest].length);

                ptr=ptr->next;

            }
        }
    

}


//Function responsible for dequeueing the stack  and send routing messages for all the in-neighbors
void sendMessages(int source, int dest, int n){

    enqueue(source);
    bool cont = true;


    while(cont)
    {
        int index = inp_arr[Front]; //Node extracted from Queue
        shortest_widest(index, source, dest);
        cont = dequeue();       
    }


}
 
// Directed graph implementation in C
int main(void)
{
    struct Edge edges[MAX_EDGES];

    //Read file and extract number of edges
    int n = readFile(edges);
 
    // construct a graph from the given edges
    graph = createGraph(edges, n, false);  //Backtrack graph
    graph_out = createGraph(edges,n,true);   //Original graph

 
    // Function to print adjacency list representation of a graph, Backtrack graph since routing messages have opposite direction to the links
    printGraph(n);

    int s = 4;  //Source   (Switched with destination according to the routing messages direction)
    int d = 1;  //Destination

    short_wide=true;  //Define the order; Shortest_widest -> True, Widest-shortest -> False

    sendMessages(s,d,n);

    if(short_wide) 
    {
        printf("\n=========================\n");
        printf("Shortest-widest order\n");
        printf("=========================\n");
    }
    else
    {
        printf("\n=========================");
        printf("\nWidest-shortest order\n");
        printf("=========================\n");
    }
    printf("\nDest %d: (%d,%d)\n",d,wl[d].width, wl[d].length);

 
    return 0;
}
