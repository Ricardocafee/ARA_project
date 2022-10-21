#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>    //To get the interval of time later
#include <unistd.h>
#include <time.h>
 
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

struct timeval t_initial; 

double uniformDistribution()   //Uniform Distribution for delay
{
    return rand()/(RAND_MAX + 1.0)*2000;
}
 
// Data structure to store a graph object
struct Graph
{
    // An array of pointers to Node to represent an adjacency list
    struct Node *head[N];
};

struct Calendar
{
    struct width_length* head;   //First event
    struct width_length* tail;  //Last event
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
    int time;
    int source;
    int dest;
    struct width_length* next;
    struct width_length* prev;
};
 
// Data structure to store a graph edge
struct Edge {
    int src, dest, width, length;
};


struct Graph *graph;     //Backwards graph
struct Graph *graph_out;  //Original graph
struct Calendar *calendar; //Calendar

struct width_length *wl;  //Will store the width and length of the best path (According to the order) in the node

 
// Function to create an adjacency list from specified edges
struct Graph* createGraph(struct Edge edges[], int n, bool check)
{
    // allocate storage for the graph data structure
    
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
  
 
    // initialize head pointer for all vertices
    if(check == false)
    {
        for (int i = 0; i < nodes_count; i++) {
            graph->head[i] = NULL;
            wl[i].visited=false;
            wl[i].length=0;
            wl[i].width=0;
            wl[i].time = 0;
        }
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

struct Calendar* initializeCalendar()  //Initialize Calendar
{
    struct Calendar* calendar_ = (struct Calendar*)malloc(sizeof(struct Calendar));

    calendar_->head=NULL;
    calendar_->tail=NULL;
    return calendar_;
}

void insertNode(struct width_length* new_node)  //Insert node at the end of the calendar
{
    calendar->tail->next=new_node;
    new_node->prev =calendar->tail;
    calendar->tail=new_node;
}

//Insert node in the middle of the calendar
void insertmiddleNode(struct width_length* new_node, struct width_length* tail, struct width_length* prev_tail) 
{
    prev_tail = tail->prev;
    new_node->next = tail;
    new_node->prev = prev_tail;
    prev_tail->next = new_node;
    tail->prev=new_node;
}

//Add element to the calendar and put it its correspondent order

void addToCalendar(int time, int dest, int index, int width, int length)
{
    struct width_length* new_node = (struct width_length*)malloc(sizeof(struct width_length));
    new_node->width=width;
    new_node->length=length;
    new_node->dest = dest;
    new_node->source =index;
    new_node->time=time;

    struct width_length* tail = calendar->tail;
    struct width_length* prev_tail;
    struct width_length* head_ = calendar->head;


    if(calendar->head == NULL)
    {
        calendar->head=new_node;
        calendar->tail=new_node;
        return;
    }
    else if(calendar->head == calendar->tail)
    {
        if(new_node->time > calendar->head->time)
            {
                calendar->head->next=new_node;
                insertNode(new_node);
                return;
            }
        else if((new_node->dest == calendar->head->dest) && (new_node->source == calendar->head->source))   
        {
            new_node->time=tail->time+50;
            calendar->head->next=new_node;
            insertNode(new_node);
            return;
        }
        else
        {
            new_node->next=calendar->head;
            calendar->head = new_node;
            calendar->tail->prev = new_node;
            return;
        }
    }
    else
    {
        while(tail != NULL)
        {   
            if(tail->next==NULL)
            {
                if(new_node->time>tail->time)
                {
                    insertNode(new_node);
                    return;
                }
                else if(new_node->dest == tail->dest && new_node->source == tail->source)
                {
                    printf("\nTwo messages in same link\n");
                    new_node->time=tail->time+50;
                    insertNode(new_node);
                    return;
                }
                else if(tail->prev!=NULL)
                {
                    if((new_node->time > tail->prev->time) && (new_node->time < tail->time))
                    {
                        insertmiddleNode(new_node, tail, tail->prev);
                        return;
                    }
                    tail = tail->prev;
                    continue;
                }
                else
                {
                    tail = tail->prev;
                    continue;
                }
            }
            else if(tail == calendar->head )
            {
                if(new_node->dest==calendar->head->dest && new_node->source==calendar->head->source)
                {
                    new_node->prev = calendar->head;
                    new_node->next = calendar->head->next;
                    calendar->head->next->prev = new_node;
                    calendar->head->next=new_node;
                    return;
                }

                if(new_node->time < calendar->head->time)
                {
                    new_node->next = calendar->head;
                    calendar->head->prev = new_node;
                    calendar->head = new_node;
                    return;
                }
            }
            else if((new_node->time > tail->prev->time) && (new_node->time < tail->time))
            {
                insertmiddleNode(new_node, tail, tail->prev);
                return;
            }
            else if((new_node->source == tail->prev->source) && (new_node->dest == tail->prev->dest))
            {

                printf("\nTwo messages in same link\n");
                new_node->time = tail->prev->time+50;
                if(new_node->time >= tail->prev->time)
                {
                    new_node->time = tail->prev->time - 1;
                }
                insertmiddleNode(new_node, tail, tail->prev);
                return;
            }
            else
            {
                tail = tail->prev;
            }
        }
    }
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

        //nodes_count++;  //Not sure?? (Merge)

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
    int nodes=0;



    fp = fopen(filename, "r");

    //Check if file exists
    if(fp == NULL) {
        printf("Could not open file %s", filename);
        return 0;
    }

    if(fscanf(fp, " %d", &nodes_count) != 1){
        printf("Empty file!");
        return 0;
    }

    //Store values according to their field

    while(fscanf(fp, " %d", &num) == 1) {

        count = count + 1;
        

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
        if(ptr->width > wide_width)  //Maximum width is prioritized
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


int generateTime_inchannel()
{
    struct timeval t_final;
    double time_random = uniformDistribution();
    int time_delay = (int) time_random;

    //gettimeofday(&t_final, NULL);
    //double _time = (t_final.tv_sec-t_initial.tv_sec)*1000 + (t_final.tv_usec - t_initial.tv_usec)/1000;
    //int time_ = (int) _time;
    int time_ =  time_delay + 1000;
    return time_;
}


//Main function responsible for the sending and receiving of routing message and state's update of nodes

void shortest_widest(int index, int dest, int s, int d, int time_){

    struct Node* ptr = graph->head[index];
    struct Node* aux;
    struct width_length* node_calendar;
    struct timeval t_final;
    int time;
    

    int width_node = wl[index].width;  //Width of the node computed based on the links to the out-neighbors
    int length_node = wl[index].length; //Length of the node computed based on the links to the out-neighbors
    int compute_width, compute_length=0;  //Variables that will compute the width and length that will be send in the routing messages


    while (ptr != NULL)
        {   
            if(ptr->dest != dest)
            {
                ptr=ptr->next;
                continue;
            }
            
            bool check = false;  //Will verify if status of node has changed
            compute_length = wl[index].length+ptr->length; //Length's sum between out-neighbor and link

            //Will compute the width of the path (between out-neighbor and link)
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
                wl[dest].time = time_;

                aux = graph->head[dest];   //Node that will send the messages
                while(aux != NULL)
                {
                    time = generateTime_inchannel();  //Generate time spent in the channel

                    time= time + time_;

                    addToCalendar(time, aux->dest, dest, wl[dest].width, wl[dest].length);  //Add event to the calendar
                    aux = aux->next; //Add next event
    
                }
                return;  //Analyze link with the next in-neighbor of the current node
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

                if(check)
                {
                    wl[dest].time = time_;
                    aux = graph->head[dest];   //Node that will send the messages
                    while(aux!=NULL)  
                    {
                        time = generateTime_inchannel();   //Generate time
                        time = time + time_;
                        addToCalendar(time, aux->dest, dest,wl[dest].width, wl[dest].length);  //Add event to the calendar
                        aux=aux->next;   //next neighbor
                    }
                }
                return;
            }
        }
    

}


//Function responsible for sending routing messages for all the in-neighbors
void sendMessages(int source, int dest_final, int n){

    int time, d;
    int index;
    struct timeval interval_time;

    struct Node* ptr = graph->head[source];
    struct width_length* node_extracted;

    while (ptr!=NULL)  //Initial case: Root node send routing messages to the in-neighbors
    {
        d = ptr->dest;  //In-neighbor
        time = generateTime_inchannel();  //Time spent in the channel (delay + unit of time)
        addToCalendar(time, d, source, 999999, 0);  //Add event to the calendar
        ptr=ptr->next;  //Next in-neighbor
    }

    struct width_length* varying_head = calendar->head; //Ptr that will tranverse the calendar while event are being removed
    
    bool cont = true;
    int time_;

    while(cont) //While nodes are not in a stable state
    {   
        if(varying_head==NULL) break;
        //waitTime(varying_head->time);  //Wait until the next event
        index = varying_head->source;  //Node that will send the message
        d = varying_head->dest;  //Node that will receive the message
        time_ = varying_head->time;
        shortest_widest(index, d,source, dest_final, time_);

        varying_head=varying_head->next;  //Next neighbor
    }

}

//Print calendar
void printCalendar()
{
    struct width_length* ptr = calendar->head;

    printf("\n===========\n");
    printf("Calendar\n");
    printf("===========\n");


    while(ptr!=NULL)
    {
        printf("\nSource: %d, Destination: %d, Width: %d, Length: %d, Time: %d\n",ptr->source, ptr->dest, ptr->width, ptr->length, ptr->time);
        ptr=ptr->next;
    }

   

}

void printStatistics(int src)
{

    printf("\n===========\n");
    printf("Statistics\n");
    printf("===========\n\n");


    for(int i=0; i < nodes_count ; i++){
        if(wl[i].time!=0 || i==src){
            printf("Node %d: , Widht: %d, Lenght: %d, Time: %d ms\n", i, wl[i].width, wl[i].length, wl[i].time);
        }
    }

    printf("\n\n");

}

int minWidth(int a, int b)
{
    if(b < a){
        return b;
    }else{
        return a;
    }
}

int lessLength()
{
    int minlength=9999, minnode=0, maxwidth=0;

    for(int i=0; i<nodes_count; i++){
        if(wl[i].visited==false && wl[i].length < minlength){
            minlength = wl[i].length;
            maxwidth = wl[i].width;
            minnode = i;
        }else if((wl[i].visited==false) && (wl[i].length == minlength)){
            if(wl[i].width > maxwidth){
                minnode = i;
                maxwidth = wl[i].width;
            }
        }
    }

    return minnode;
}


void printWS(int src, int dest)
{
    struct Node* aux;

    //wl reset
    for(int i = 0 ; i < nodes_count ; i++){
        wl[i].visited = false;
        wl[i].length = 99999;
        wl[i].width = 0;
        wl[i].prev = NULL;
    }

    wl[src].length=0;
    wl[src].width=99999;

    int nodes_left = nodes_count, current = 0;    

    while(nodes_left > 0){
        current = lessLength();
        if(current==dest){
            break;
        }
        wl[current].visited=true;
        nodes_left--;
        aux = graph->head[current];

        while (aux!=NULL)
        {
            if (wl[aux->dest].length > (wl[current].length + aux->length))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
                wl[aux->dest].width = minWidth(wl[current].width, aux->width);
            }
            else if((wl[aux->dest].length == (wl[current].length + aux->length)) && (wl[aux->dest].width < minWidth(wl[current].width, aux->length)))
            {
                wl[aux->dest].width = minWidth(wl[current].width, aux->width);
            }

            aux=aux->next;
            
        }
        
    }

    printf("=========================\n");
    printf("Widest Shortest Algorithm\n");
    printf("     Dest %d: (%d,%d)    \n",dest, wl[dest].width, wl[dest].length);
    printf("==========================\n");

}

void printSW(int src, int dest)
{

    



}


 
// Directed graph implementation in C
int main(void)
{

    struct Edge edges[MAX_EDGES];
    srand(time(NULL));
    
    //Read file and extract number of edges
    int n = readFile(edges);

    wl = (struct width_length*)malloc(nodes_count * sizeof(struct width_length));
    
    // construct a graph from the given edges
    graph = createGraph(edges, n, false);  //Backtrack graph
    graph_out = createGraph(edges, n, true);   //Original graph
    calendar = initializeCalendar();  //Initialize graph

 
    // Function to print adjacency list representation of a graph, Backtrack graph since routing messages have opposite direction to the links
    printGraph(n);

    int s = 3;  //Source   (Switched with destination according to the routing messages direction)
    int d = 0;  //Destination
    short_wide=true;  //Define the order; Shortest_widest -> True, Widest-shortest -> False



    sendMessages(s,d,n);  //Function responsible for sending the routing messages

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

    printCalendar();

    printStatistics(s);

    printWS(s, d);

    
    return 0;
}
