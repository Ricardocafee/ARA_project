#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>    //To get the interval of time later
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

// Define the maximum number of vertices in the graph
#define N 5000
#define MAX_EDGES 100000
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
    struct Node **head;
};

struct Calendar
{
    struct width_length* head;   //First event
    struct width_length* tail;  //Last event
};

struct Box_plot
{
    struct box_parameters* head;   //List for box-plot
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

struct box_parameters {
    int tempo_estab, width_box, length_box;
    int stored_value;
    struct box_parameters* next;
};


struct Graph *graph;     //Backwards graph
struct Graph *graph_out;  //Original graph
struct Calendar *calendar; //Calendar
struct Box_plot *list_box; 

struct width_length *wl;  //Will store the width and length of the best path (According to the order) in the node


 
// Function to create an adjacency list from specified edges
struct Graph* createGraph(struct Edge edges[], int n, bool check)
{
    // allocate storage for the graph data structure
    
    struct Graph* new_graph = (struct Graph*)malloc(sizeof(struct Graph));
    new_graph->head = malloc(nodes_count * sizeof(struct Node*));

    for (int i = 0; i < nodes_count; i++) {
            new_graph->head[i]=NULL;
    }
  
 
    // initialize head pointer for all vertices
    if(check == false)
    {
        for (int i = 0; i < nodes_count; i++) {
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
        newNode->next = new_graph->head[src];
 
        // point head pointer to the new node
        new_graph->head[src] = newNode;
    }
 
    return new_graph;
}


struct width_length* initializeNodeStates()
{
    for (int i = 0; i < nodes_count; i++) {
    wl[i].visited=false;
    wl[i].length=0;
    wl[i].width=0;
    wl[i].time = 0;
    }

    return wl;
}

struct Calendar* initializeCalendar()  //Initialize Calendar
{
    struct Calendar* calendar_ = (struct Calendar*)malloc(sizeof(struct Calendar));

    calendar_->head=NULL;
    calendar_->tail=NULL;
    return calendar_;
}

struct Box_plot* initializeBoxPlot()
{
    struct Box_plot* plot_box = (struct Box_plot*)malloc(sizeof(struct Box_plot));

    plot_box->head=NULL;
    return plot_box;
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

void calendarFree()
{
    struct width_length* aux1 = calendar->head;
    struct width_length* aux2 = calendar->head;

    while(aux1->next!=NULL){
        aux2=aux1->next;
        free(aux1);
        aux1=aux2;
    }
    free(aux1);
    free(calendar);

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
    new_node->visited=false;

    struct width_length* tail = calendar->tail;
    struct width_length* prev_tail;
    struct width_length* head_ = calendar->head;


    if(calendar->head == NULL)
    {
        calendar->head=new_node;
        calendar->tail=new_node;
        new_node->prev =calendar->tail;
        new_node->next = NULL;
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
                    //printf("\nTwo messages in same link\n");
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

                if((new_node->dest==calendar->head->dest) && (new_node->source==calendar->head->source))
                {
                    new_node->prev = calendar->head;
                    new_node->next = calendar->head->next;
                    calendar->head->next->prev = new_node;
                    calendar->head->next=new_node;
                    return;
                }
                else if(new_node->time < calendar->head->time)
                {
                    new_node->next = calendar->head;
                    calendar->head->prev = new_node;
                    calendar->head = new_node;
                    return;
                }
                else
                {
                    new_node->prev = calendar->head;
                    new_node->next = calendar->head->next;
                    calendar->head->next->prev = new_node;
                    calendar->head->next=new_node;
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

               // printf("\nTwo messages in same link asdas\n");
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

void addListBox(int temp_estab, int width_box, int length_box)
{
    struct box_parameters* new_node = (struct box_parameters*)malloc(sizeof(struct box_parameters));
    new_node->tempo_estab=temp_estab;
    new_node->width_box=width_box;
    new_node->length_box=length_box;
    new_node->stored_value = 0;
    new_node->next=NULL;


    if(list_box->head == NULL)
    {
        list_box->head = new_node;
        return;
    }
    else
    {
        new_node->next = list_box->head;
        list_box->head = new_node;
    }

}

void FrontBackSplit(struct box_parameters* source, struct box_parameters** frontRef, struct box_parameters** backRef)
{
    struct box_parameters* fast;
    struct box_parameters *slow;

    slow=source;
    fast=source->next;

    while(fast!=NULL)
    {
        fast = fast->next;
        if (fast != NULL)
        {
            slow = slow->next;
            fast = fast->next;
        }
    }


    *frontRef = source;
    *backRef = slow->next;
    slow->next = NULL;
}

struct box_parameters* SortedMerge(struct box_parameters* a, struct box_parameters* b, int i)
{
    struct box_parameters* result = NULL;

    

    if(a == NULL)
    {
        return b;
    }
    else if(b == NULL)
    {
    return a;
    }

    if(i == 0)
    {
        a->stored_value=a->tempo_estab;
        b->stored_value=b->tempo_estab;
    }
    else if(i == 1)
    {
        a->stored_value=a->width_box;
        b->stored_value=b->width_box;
    }
    else
    {
        a->stored_value=a->length_box;
        b->stored_value=b->length_box;
    }

    if(a->stored_value <= b->stored_value)
    {
        result = a;
        result->next = SortedMerge(a->next, b , i);
    }
    else
    {
        result = b;
        result->next = SortedMerge(a, b->next, i);
    }


    return result;
}

void MergeSort(struct box_parameters** headRef, int i)
{
    struct box_parameters* head = *headRef;
    struct box_parameters* a;
    struct box_parameters* b;

    if((head==NULL) || (head->next == NULL))
    {
        return;
    }

    FrontBackSplit(head, &a, &b);

    MergeSort(&a,i);
    MergeSort(&b,i);

    *headRef = SortedMerge(a,b, i);
    
}
 
// Function to print adjacency list representation of a graph
void printGraph(int n)
{
    struct Node* ptr = NULL;
    int i=0;

    //Firstly, the backtrack graph
    printf("\nBacktrack Graph\n");

    for (i = 0; i < nodes_count; i++)
    {

        // print current vertex and all its neighbors
        ptr = graph->head[i];


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

    for (i = 0; i < nodes_count; i++)
    {

        // print current vertex and all its neighbors
        ptr = graph_out->head[i];


        if(ptr==NULL && i > 0) continue;

        while (ptr != NULL)
        {
            printf("%d —> %d (%d,%d)\t", i, ptr->dest, ptr->width, ptr->length);
            ptr = ptr->next;
        }
 
        printf("\n");
    }
}

int readFile(struct Edge edges[], char *filename)
{
    FILE *fp;
    int count= 0; //column counter
    int line = 0; //Line counter
    char c; //To store a character read from a file
    char num;
    int nodes=0;


    filename = "Abilene.csv";


    fp = fopen(filename, "r");

    char buffer[1024];
    char *token;
    int row = 0;
    int column = 0;

    //Check if file exists
    if(fp == NULL) {
        printf("Could not open file %s, put the name of the file as an argument", filename);
        return 0;
    }

    if(fscanf(fp, " %d", &nodes_count) != 1){
        printf("Empty file!");
        return 0;
    }

    //Store values according to their field

    while(fgets(buffer, 1024, fp))
    {
        if(row == 0)
        {
            row++;
            continue;
        }

        token = strtok(buffer,",");
        column = 0;
    
        while(token != NULL)
        {

            if(column == 0)
            {
                edges[row-1].src = atoi(token);
            }
            else if(column == 1)
            {
                edges[row-1].dest = atoi(token);
            }
            else if(column == 2)
            {
                edges[row-1].width = atoi(token);
            }
            else
            {
                edges[row-1].length = atoi(token);
            }

            column++;
            
            token = strtok(NULL, ",");
        }
        row++;
    
    }


    //Close the file
    fclose(fp);
    return row-1;
    
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

bool compareLinks_sw(int source, int dest){  

    struct Node* ptr = graph_out->head[source];

    int i = 0;
    int min_length;
    int wide_width;
    int d;

    while(ptr != NULL)
    {
        d = ptr->dest;
        if(wl[d].visited == false && (d != dest)) //If that node [d] didn't received any routing message, discard
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

bool compareLinks_ws(int source, int dest){

    struct Node* ptr = graph_out->head[source];

    int i = 0;
    int min_length;
    int wide_width;
    int d;
    int aux = dest;

    while(ptr != NULL)
    {
        d = ptr->dest;
        if(wl[d].visited == false && (d != dest)) //If that node [d] didn't received any routing message, discard
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

    int time_ =  time_delay + 1000;
    return time_;
}


//Main function responsible for the sending and receiving of routing message and state's update of nodes

void shortest_widest(int index, int dest, int s, int d, int time_){

    struct Node* ptr = graph->head[index];
    struct Node* aux;
    struct timeval t_final;
    int time;

    wl[s].visited=true;

    int width_node = wl[index].width;  //Width of the node computed based on the links to the out-neighbors
    int length_node = wl[index].length; //Length of the node computed based on the links to the out-neighbors
    int compute_width, compute_length=0;  //Variables that will compute the width and length that will be send in the routing messages

    if(index == d)
        return;
    

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

                    if(aux->dest != index)
                    {
                        addToCalendar(time, aux->dest, dest, wl[dest].width, wl[dest].length);  //Add event to the calendar
                    }
                    aux = aux->next; //Add next event
    
                }
                return;  //Analyze link with the next in-neighbor of the current node
            }
            else
            {
                
                if(short_wide)  //For Shortest_widest order
                {
                    check = compareLinks_sw(dest, s);
                }
                else    //For Widest-shortest order
                {
                    check = compareLinks_ws(dest, s);
                }

                if(check)
                {
                    wl[dest].time = time_;
                    aux = graph->head[dest];   //Node that will send the messages
                    while(aux!=NULL)  
                    {
                        
                        time = generateTime_inchannel();   //Generate time
                        time = time + time_;
                        if(aux->dest != index)
                            {
                                addToCalendar(time, aux->dest, dest,wl[dest].width, wl[dest].length);  //Add event to the calendar
                            }
                        
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

    while (ptr!=NULL)  //Initial case: Root node send routing messages to the in-neighbors
    {
        d = ptr->dest;  //In-neighbor
        time = generateTime_inchannel();  //Time spent in the channel (delay + unit of time)
        addToCalendar(time, d, source, 99999, 0);  //Add event to the calendar
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

void printListBox()
{
    struct box_parameters* ptr = list_box->head;

    printf("\n===========\n");
    printf(" List-box \n");
    printf("===========\n");


    while(ptr!=NULL)
    {
        printf("\n Temp esteb: %d, Width: %d, Length: %d, Stored value: %d",ptr->tempo_estab, ptr->width_box, ptr->length_box, ptr->stored_value);
        ptr=ptr->next;
    }

   

}

float ComputeMedian(int boxtimes[nodes_count-1], int num_nodes)
{
    float median;

    if((num_nodes)%2)   //Source node does not count
    {
        //Odd number of nodes explored       
        int median_index = (int) floor((num_nodes)/2);
        median = boxtimes[median_index];
    }
    else
    {
        //Even number of nodes explored
        int up_median_ind = (int) floor((num_nodes-1)/2) + 1;
        int low_median_ind = (int) floor((num_nodes-1)/2);
        median = (boxtimes[up_median_ind]+boxtimes[low_median_ind])/2;
    }

    return median;
}

float ComputeQ1andQ3(int boxtimes[nodes_count-1], bool check, int num_nodes)
{
    //Check informs if it is Q1 or Q3: Q1 -> Check = True; Q3 -> Check = False
    float Q;

    if((num_nodes)%4)   //Source node does not count
    {
        //Odd number of nodes explored
        int q_index;

        if(check){       
            q_index = (int) floor((num_nodes)/4);
        }
        else
        {
            q_index = (int) floor(3*(num_nodes)/4);
        }
        Q = boxtimes[q_index];
    }
    else
    {
        //Even number of nodes explored
        int up_q_ind;
        int low_q_ind;

        if(check){       
            up_q_ind = (int) floor((num_nodes)/4)+1;
            low_q_ind = (int) floor((num_nodes)/4);
        }
        else
        {
            up_q_ind = (int) floor(3*(num_nodes)/4)+1;
            low_q_ind = (int) floor(3*(num_nodes)/4);
        }

        Q = (boxtimes[up_q_ind]+boxtimes[low_q_ind])/2;
    }

    return Q;
}



void BoxPlot(int n_pairs, int i)
{
    struct box_parameters* ptr = list_box->head;

    int count = 1;

    int median, Q1, Q3; //Boxplot parameters
    int min, max;

    printf("\n===================================\n");
    printf("  Box Plot: ");
    if(i == 0)
    {
        printf("Stabilization Times\n");
    }
    else if(i == 1)
    {
        printf("Width\n");
    }
    else{
        printf("Length\n");
    }
    printf("===================================\n");


    min = list_box->head->stored_value;

    if((n_pairs+1)%4) 
    {
            int up_q1_ind = (int) floor((n_pairs)/4)+1;
            int low_q1_ind = (int) floor((n_pairs)/4);

            int up_q3_ind = (int) floor(3*(n_pairs)/4)+1;
            int low_q3_ind = (int) floor(3*(n_pairs)/4);

            int up_median_ind = (int) floor((n_pairs)/2)+1;
            int low_median_ind = (int) floor((n_pairs)/2);

            int low_q1, up_q1, low_q3, up_q3, up_median, low_median;

            while(ptr!=NULL)
            {
                if(count == low_q1_ind)
                {
                low_q1 = ptr->stored_value;
                }
                if(count == up_q1_ind)
                {
                up_q1 = ptr->stored_value;
                }
                if(count == low_q3_ind)
                {
                low_q3 = ptr->stored_value;
                }
                if(count == up_q3_ind)
                {
                up_q3 = ptr->stored_value;
                }
                if(count == up_median_ind)
                {
                up_median = ptr->stored_value;
                }
                if(count == low_median_ind)
                {
                low_median = ptr->stored_value;
                }
                if(ptr->next == NULL)
                    max = ptr->stored_value;

                ptr = ptr->next;
                count++;
            }

            Q1 = (int) (low_q1 + up_q1)/2;
            Q3 = (int) (low_q3 + up_q3)/2;
            median = (int) (low_median + up_median)/2;
    }
    else
    {
        int q1_index = (int) ((n_pairs+1)/4);
        int q3_index = (int) (3*(n_pairs+1)/4);
        int median_index = (int) ((n_pairs+1)/2);

        while(ptr!=NULL)
            {
                if(count == q1_index)
                {
                Q1 = ptr->stored_value;
                }
                if(count == q3_index)
                {
                Q3 = ptr->stored_value;
                }
                if(count == median_index)
                {
                median = ptr->stored_value;
                }

                if(ptr->next == NULL)
                    max = ptr->stored_value;
                    
                ptr = ptr->next;
                count++;
            }
    }

    printf("\nMax: %d", max);
    printf("\nQ3: %d", Q3);
    printf("\nMedian: %d", median);
    printf("\nQ1: %d", Q1);
    printf("\nMin: %d\n", min);

}


void printStatistics(int src)
{

    printf("\n===========\n");
    printf("Statistics\n");
    printf("===========\n\n");


    for(int i=0; i < nodes_count ; i++){
        if(wl[i].time!=0 || i==src){
            printf("Node %d: , Width: %d, Length: %d, Time: %d ms\n", i, wl[i].width, wl[i].length, wl[i].time);
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
    int minlength=99999, minnode=0, maxwidth=0;

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


void printWS_stablestate(int src, int dest)
{
    struct Node* aux;

    //wl reset
    for(int i = 0 ; i < nodes_count ; i++){
        wl[i].visited = false;
        wl[i].length = 99999;
        wl[i].width = 0;
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
    printf("Widest Shortest Stable State\n");
    printf("  From %d to %d: (%d,%d)    \n",dest, src, wl[dest].width, wl[dest].length);
    printf("==========================\n");

}

int greatestWidth()
{
    int maxWidth=0, maxnode=0;

    for(int i=0; i<nodes_count; i++){
        if(wl[i].visited==false && wl[i].width > maxWidth){
            maxWidth = wl[i].width;
            maxnode = i;
        }
    }

    return maxnode;

}

//dijkstra algorithm to find the widest path between source and destination
int findMaxWidth(int src, int dest)
{
    struct Node* aux;

    //wl reset
    for(int i = 0 ; i < nodes_count ; i++){
        wl[i].visited = false;
        wl[i].length = 99999;
        wl[i].width = 0;
    }

    wl[src].length=0;
    wl[src].width=99999;

    int nodes_left = nodes_count, current = 0;    

    while(nodes_left > 0){
        current = greatestWidth();  //pick the node with the greatest width among the rest
        if(current==dest){
            return wl[current].width;  //if the node picked is the destination, then its width is the greatest along the path
        }
        wl[current].visited=true;
        nodes_left--;
        aux = graph->head[current];

        while (aux!=NULL)
        {
            if (wl[aux->dest].width < (minWidth(wl[current].width, aux->width)))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
                wl[aux->dest].width = minWidth(wl[current].width, aux->width);
            }
            else if(wl[aux->dest].width == (minWidth(wl[current].width, aux->width)) && (wl[aux->dest].length > (wl[current].length + aux->length)))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
            }

            aux=aux->next;
            
        }

    }
}


void printSW(int src, int dest)
{
    struct Node* aux;
    int maxWidth = 0;

    //get the maximum width possible between source and destination
    maxWidth = findMaxWidth(src, dest);

    //wl reset for dijkstra algorithm, but capped widthwise
    for(int i = 0 ; i < nodes_count ; i++){
        wl[i].visited = false;
        wl[i].length = 99999;
        if(wl[i].width > maxWidth){
            wl[i].width = maxWidth;
        }
    }

    wl[src].length=0;
    wl[src].width=99999;

    int nodes_left = nodes_count, current = 0;    

    //general dijkstra where only the edges within the range of the max width are considered
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
            if ((wl[aux->dest].length > (wl[current].length + aux->length)) && (aux->width == maxWidth))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
            }

            aux=aux->next;
            
        }
    }

    printf("=========================\n");
    printf("Shortest Widest Algorithm\n");
    printf("  From %d to %d: (%d,%d)    \n",dest, src, wl[dest].width, wl[dest].length);
    printf("==========================\n");
}

//Shortest Widest stable state
void printSW_stablestate(int src, int dest)
{
    struct Node* aux;

    //wl reset
    for(int i = 0 ; i < nodes_count ; i++){
        wl[i].visited = false;
        wl[i].length = 99999;
        wl[i].width = 0;
    }

    wl[src].length=0;
    wl[src].width=99999;

    int nodes_left = nodes_count, current = 0;    

    while(nodes_left > 0){
        current = greatestWidth();
        if(current==dest){
            break;
        }
        wl[current].visited=true;
        nodes_left--;
        aux = graph->head[current];

        while (aux!=NULL)
        {
            if (wl[aux->dest].width < minWidth(wl[current].width, aux->width))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
                wl[aux->dest].width = minWidth(wl[current].width, aux->width);
            }
            else if((wl[aux->dest].width == minWidth(wl[current].width, aux->width)) && (wl[aux->dest].length > (wl[current].length + aux->length)))
            {
                wl[aux->dest].length = wl[current].length + aux->length;
            }

            aux=aux->next;
            
        }
        
    }

    printf("=========================\n");
    printf("Shortest Widest Stable State\n");
    printf("  From %d to %d: (%d,%d)    \n",dest, src, wl[dest].width, wl[dest].length);
    printf("==========================\n");
}

//Stable State Algorithm
void printStableState(int src, int dest)
{
    if(short_wide){
        printSW_stablestate(src, dest);
    }else{
        printWS_stablestate(src,dest);
    }
}


void InterativeModeQoS(int n)
{
    int source=0, dest=0;

    printf("\n==================================\n");
    printf(" Interative Mode - QoS Protocol ");
    printf("\n==================================\n");

    printf("Enter the source: ");
    scanf("%d", &dest);
    printf("Enter the destination: ");
    scanf("%d", &source);             //Directions swaped due to routing messages direction

    short_wide=true;

    for(int count = 0; count < 2; count++)  //Test the WS order and the SW order
    {
    
        calendar = initializeCalendar();  //Initialize graph
        wl = initializeNodeStates();  //Initialize node states

        sendMessages(source,dest,n);  //Function responsible for sending the routing messages

        calendarFree();

        if((wl[dest].length==0 || wl[dest].length==99999) && (wl[dest].width==0))
        {
            printf("\nThere is no path from %d to %d\n", dest, source);
            
            return;
        }

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


        printf("\nFrom %d to %d: (%d,%d)\n",dest,source,wl[dest].width, wl[dest].length);

        short_wide=false;
    }

}


void InterativeModeWS()
{
    int source, dest;

    printf("\n=============================================\n");
    printf(" Interative Mode - Widest-Shortest Algorithm ");
    printf("\n=============================================\n");

    printf("Enter the source: ");
    scanf("%d", &dest);
    printf("Enter the destination: ");
    scanf("%d", &source);             //Directions swaped due to routing messages direction

    short_wide=false;


    printWS_stablestate(source,dest);

    if((wl[dest].length==0 || wl[dest].length==99999) && (wl[dest].width==0))
    {
        printf("\nThere is no path from %d to %d\n", dest, source);
        return;
    }

}

void InterativeModeSW()
{
    int source, dest;

    printf("\n=============================================\n");
    printf(" Interative Mode - Shortest-Widest Algorithm ");
    printf("\n=============================================\n");

    printf("Enter the source: ");
    scanf("%d", &dest);
    printf("Enter the destination: ");
    scanf("%d", &source);             //Directions swaped due to routing messages direction


    printSW_stablestate(source,dest);

    if((wl[dest].length==0 || wl[dest].length==99999) && (wl[dest].width==0))
    {
        printf("\nThere is no path from %d to %d\n", dest, source);
        return;
    }

}

void freeGraphs()
{
    struct Node* aux1 = NULL;
    struct Node* aux2 = NULL;


    for(int i=0;i<nodes_count;i++){
        aux1 = graph->head[i];
        while(aux1!=NULL){
            aux2=aux1->next;
            free(aux1);
            aux1=aux2;
        }
    }
    free(graph->head);
    free(graph);

    for(int i=0;i<nodes_count;i++){
        aux1 = graph_out->head[i];
        while(aux1!=NULL){
            aux2=aux1->next;
            free(aux1);
            aux1=aux2;
        }
    }
    free(graph_out->head);
    free(graph_out);
}
 
 
// Directed graph implementation in C
int main(int argc, char *argv[])
{
    struct Edge edges[MAX_EDGES];
    srand(time(NULL));
    
    int n = readFile(edges, argv[1]);
    wl = (struct width_length*)malloc(nodes_count * sizeof(struct width_length));

    graph = createGraph(edges, n, false);  //Backtrack graph
    graph_out = createGraph(edges, n, true);   //Original graph
    printGraph(n);  // Function to print adjacency list representation of a graph, Backtrack graph since routing messages have opposite direction to the links

    bool no_path = false;
    short_wide=false;  //Define the order; Shortest_widest -> True, Widest-shortest -> False
    int counter_pairs = 0;

    list_box = initializeBoxPlot();

    for(int i = 0; i<nodes_count; i++)
    {
        for(int j = 0; j<nodes_count; j++)
        {
            if(i == j) continue;
            no_path = false;   

            int s = i;  //Source   (Switched with destination according to the routing messages direction)
            int d = j;  //Destination

            int temp_estab = 0;
            int width_box = 0;
            int length_box = 0;

            for(int count = 0; count < 10; count++)   //10 iterations to obtain some tendency on data (Box plot)
            {
                calendar = initializeCalendar();  //Initialize graph
                wl = initializeNodeStates();  //Initialize node states

                sendMessages(s,d,n);  //Function responsible for sending the routing messages

                if((wl[d].length==0 || wl[d].length==99999) && (wl[d].width==0))
                {
                    printf("\nThere is no path from %d to %d\n", d, s);
                    no_path = true;
                    break;
                }

                temp_estab = wl[d].time + temp_estab;
                width_box = wl[d].width + width_box;
                length_box = wl[d].length + length_box;
                
                calendarFree();

            }

            if(no_path) continue;

            counter_pairs++;

            temp_estab = temp_estab/10;
            width_box = width_box/10;
            length_box = length_box/10;
            addListBox(temp_estab, width_box, length_box);

            /*if(short_wide) 
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


            printf("\nFrom %d to %d: (%d,%d)\n",d,s,wl[d].width, wl[d].length);
            printStatistics(s);*/
            //printWS(s, d);
            printStableState(s, d);

        }
    }
    for (int i = 0; i < 3; i++)
    {
        MergeSort(&list_box->head, i);  //Variable i defines which is the variable taken into consideration in the sorting
        //printListBox();
        BoxPlot(counter_pairs, i);
    }

    InterativeModeQoS(n);
    InterativeModeWS();
    InterativeModeSW();

    free(wl);
    freeGraphs();
    
    return 0;
}
