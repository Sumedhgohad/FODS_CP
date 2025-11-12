#include "airline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>


Flight flights[MAX_FLIGHTS];
int flightCount = 0;
Passenger passengers[MAX_PASSENGERS];
int passengerCount = 0;
CheckInNode* checkInFront = NULL;
CheckInNode* checkInRear = NULL;

#define MAX_AIRPORTS 40
#define INF 1e12

typedef struct {
    int src, dest;
    double weight;
} Edge;

typedef struct {
    int vertexCount;
    double **adjMatrix;
} Graph;


static const char* AIRPORTS[] = {"DEL", "BOM", "BLR", "HYD", "CCU", "PNQ"};
static const struct {const char *src, *dest; double weight;} ROUTES[] = {
    {"DEL", "BOM", 720}, {"DEL", "BLR", 1740}, {"DEL", "HYD", 1265}, 
    {"DEL", "CCU", 1300}, {"DEL", "PNQ", 1440}, {"BOM", "BLR", 984}, 
    {"BOM", "HYD", 710}, {"BOM", "CCU", 1650}, {"BOM", "PNQ", 148},
    {"BLR", "HYD", 550}, {"BLR", "CCU", 1515}, {"BLR", "PNQ", 840},
    {"HYD", "CCU", 1180}, {"HYD", "PNQ", 620}, {"CCU", "PNQ", 1655}
};


void addFlight() {
    if (flightCount >= MAX_FLIGHTS) {
        printf("Flight list full!\n");
        return;
    }
    Flight* f = &flights[flightCount];
    printf("\n=== ADD NEW FLIGHT ===\n");
    printf("Flight Number: "); scanf("%s", f->flightNumber);
    printf("Origin: "); scanf("%s", f->origin);
    printf("Destination: "); scanf("%s", f->destination);
    printf("Departure (HH:MM): "); scanf("%s", f->departureTime);
    printf("Arrival (HH:MM): "); scanf("%s", f->arrivalTime);
    printf("Aircraft: "); scanf("%s", f->aircraft);
    printf("Capacity: "); scanf("%d", &f->capacity);
    printf("Price: $"); scanf("%f", &f->price);
    printf("Priority (1-10): "); scanf("%d", &f->priority);
    f->bookedSeats = 0;
    strcpy(f->status, "scheduled");
    flightCount++;
    printf("Flight added successfully!\n");
}

void displayFlights() {
    printf("\n=== ALL FLIGHTS ===\n");
    printf("%s %s %s %s %s\n", "Flight", "Route", "Departure", "Price", "Available");
    for (int i = 0; i < flightCount; i++) {
        Flight* f = &flights[i];
        char route[32];
        sprintf(route, "%s-%s", f->origin, f->destination);
        printf("%s %s %s ₹%f %d\n", 
               f->flightNumber, route, f->departureTime, f->price, 
               f->capacity - f->bookedSeats);
    }
}

void bookTicket() {
    if (passengerCount >= MAX_PASSENGERS) {
        printf("Passenger list full!\n");
        return;
    }
    Passenger* p = &passengers[passengerCount];
    printf("\n=== BOOK TICKET ===\n");
    displayFlights();
    printf("\nPassenger ID: "); scanf("%s", p->id);
    printf("First Name: "); scanf("%s", p->firstName);
    printf("Last Name: "); scanf("%s", p->lastName);
    printf("Email: "); scanf("%s", p->email);
    printf("Phone: "); scanf("%s", p->phone);
    printf("Flight Number: "); scanf("%s", p->flightId);
    
    for (int i = 0; i < flightCount; i++) {
        if (strcmp(flights[i].flightNumber, p->flightId) == 0) {
            if (flights[i].bookedSeats < flights[i].capacity) {
                flights[i].bookedSeats++;
                strcpy(p->checkInStatus, "pending");
                passengerCount++;
                printf("Ticket booked successfully!\n");
                return;
            } else {
                printf("Flight is full!\n");
                return;
            }
        }
    }
    printf("Flight not found!\n");
}

void displayPassengers() {
    printf("\n=== ALL PASSENGERS ===\n");
    printf("%-8s %-20s %-25s %-12s %-12s\n", "ID", "Name", "Email", "Flight", "Status");
    for (int i = 0; i < passengerCount; i++) {
        Passenger* p = &passengers[i];
        char name[64];
        sprintf(name, "%s %s", p->firstName, p->lastName);
        printf("%-8s %-20s %-25s %-12s %-12s\n", 
               p->id, name, p->email, p->flightId, p->checkInStatus);
    }
}


void checkInPassenger() {
    char id[NAME_LEN];
    int priority;
    printf("\n=== CHECK-IN PASSENGER ===\n");
    printf("Passenger ID: "); scanf("%s", id);
    printf("Priority (1=VIP, 2=Regular): "); scanf("%d", &priority);
    
    for (int i = 0; i < passengerCount; i++) {
        if (strcmp(passengers[i].id, id) == 0) {
            CheckInNode* newNode = (CheckInNode*)malloc(sizeof(CheckInNode));
            newNode->passenger = passengers[i];
            newNode->priority = priority;
            time_t now = time(0);
            strcpy(newNode->checkInTime, ctime(&now));
            newNode->next = NULL;
            
            if (!checkInFront) {
                checkInFront = checkInRear = newNode;
            } else if (priority == 1) {  // VIP goes to front
                newNode->next = checkInFront;
                checkInFront = newNode;
            } else {  // Regular goes to rear
                checkInRear->next = newNode;
                checkInRear = newNode;
            }
            printf("Added to check-in queue!\n");
            return;
        }
    }
    printf("Passenger not found!\n");
}

void displayCheckInQueue() {
    printf("\n=== CHECK-IN QUEUE ===\n");
    printf("%-20s %-12s %-10s\n", "Passenger", "Flight", "Priority");
    CheckInNode* curr = checkInFront;
    while (curr) {
        char name[64];
        sprintf(name, "%s %s", curr->passenger.firstName, curr->passenger.lastName);
        printf("%-20s %-12s %-10s\n", name, curr->passenger.flightId, 
               curr->priority == 1 ? "VIP" : "Regular");
        curr = curr->next;
    }
}

void processCheckInQueue() {
    printf("\n=== PROCESSING CHECK-IN ===\n");
    if (!checkInFront) {
        printf("No passengers in queue.\n");
        return;
    }
    CheckInNode* temp = checkInFront;
    Passenger p = temp->passenger;
    printf("Processing: %s %s (Flight: %s)\n", p.firstName, p.lastName, p.flightId);
    
    for (int i = 0; i < passengerCount; i++) {
        if (strcmp(passengers[i].id, p.id) == 0) {
            strcpy(passengers[i].checkInStatus, "checked-in");
            break;
        }
    }
    
    checkInFront = checkInFront->next;
    if (!checkInFront) checkInRear = NULL;
    free(temp);
    printf("Check-in completed!\n");
}


static int findAirport(char names[][NAME_LEN], int count, const char* name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(names[i], name) == 0) return i;
    }
    return -1;
}

static Graph createGraph(int vertices) {
    Graph g;
    g.vertexCount = vertices;
    g.adjMatrix = (double**)malloc(sizeof(double*) * vertices);
    for (int i = 0; i < vertices; i++) {
        g.adjMatrix[i] = (double*)malloc(sizeof(double) * vertices);
        for (int j = 0; j < vertices; j++) {
            g.adjMatrix[i][j] = (i == j) ? 0 : INF;
        }
    }
    return g;
}

static void freeGraph(Graph* g) {
    if (g->adjMatrix) {
        for (int i = 0; i < g->vertexCount; i++) {
            free(g->adjMatrix[i]);
        }
        free(g->adjMatrix);
    }
}

static void printGraph(Graph* g, char names[][NAME_LEN]) {
    printf("\nRoute Network Graph:\n");
    printf("%-10s", "");
    for (int j = 0; j < g->vertexCount; j++) printf("%-10s", names[j]);
    printf("\n");
    
    for (int i = 0; i < g->vertexCount; i++) {
        printf("%-10s", names[i]);
        for (int j = 0; j < g->vertexCount; j++) {
            if (g->adjMatrix[i][j] >= INF/2) printf("%-10s", "INF");
            else printf("%-10.0f", g->adjMatrix[i][j]);
        }
        printf("\n");
    }
}

static double primMST(Graph* g, char names[][NAME_LEN]) {
    int V = g->vertexCount;
    double key[V];
    int parent[V];
    bool inMST[V];
    
    for (int i = 0; i < V; i++) {
        key[i] = INF;
        inMST[i] = false;
        parent[i] = -1;
    }
    key[0] = 0;
    
    for (int count = 0; count < V - 1; count++) {
        double min = INF;
        int u = -1;
        for (int v = 0; v < V; v++) {
            if (!inMST[v] && key[v] < min) {
                min = key[v];
                u = v;
            }
        }
        inMST[u] = true;
        for (int v = 0; v < V; v++) {
            if (!inMST[v] && g->adjMatrix[u][v] < key[v]) {
                key[v] = g->adjMatrix[u][v];
                parent[v] = u;
            }
        }
    }
    
    double total = 0;
    printf("\nMinimum Spanning Tree (Prim's):\n");
    for (int v = 1; v < V; v++) {
        printf("  %s -- %s : %.0f\n", names[parent[v]], names[v], g->adjMatrix[parent[v]][v]);
        total += g->adjMatrix[parent[v]][v];
    }
    return total;
}

static double kruskalMST(Graph* g, char names[][NAME_LEN]) {
    int V = g->vertexCount;
    
    Edge edges[V * V];
    int edgeCount = 0;
    for (int i = 0; i < V; i++) {
        for (int j = i + 1; j < V; j++) {
            if (g->adjMatrix[i][j] < INF/2) {
                edges[edgeCount].src = i;
                edges[edgeCount].dest = j;
                edges[edgeCount].weight = g->adjMatrix[i][j];
                edgeCount++;
            }
        }
    }
    
    for (int i = 0; i < edgeCount - 1; i++) {
        for (int j = 0; j < edgeCount - i - 1; j++) {
            if (edges[j].weight > edges[j + 1].weight) {
                Edge temp = edges[j];
                edges[j] = edges[j + 1];
                edges[j + 1] = temp;
            }
        }
    }
    
    int parent[V];
    for (int i = 0; i < V; i++) parent[i] = i;
    
    double total = 0;
    int mstEdges = 0;
    printf("\nMinimum Spanning Tree (Kruskal's):\n");
    
    for (int i = 0; i < edgeCount && mstEdges < V - 1; i++) {
        int u = edges[i].src;
        int v = edges[i].dest;
        
        int ru = u;
        while (parent[ru] != ru) ru = parent[ru];
        
        int rv = v;
        while (parent[rv] != rv) rv = parent[rv];
        
        if (ru != rv) {
            printf("  %s -- %s : %.0f\n", names[u], names[v], edges[i].weight);
            total += edges[i].weight;
            mstEdges++;
            parent[ru] = rv;  
        }
    }
    return total;
}

static void floydWarshallAllPairs(Graph* graph, char names[][NAME_LEN]) {
    int V = graph->vertexCount;
    double dist[V][V];
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            dist[i][j] = graph->adjMatrix[i][j];
        }
    }
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                }
            }
        }
    }
    printf("\nAll-Pairs Shortest Paths (Floyd-Warshall):\n");
    printf("\n%-10s", "From/To");
    for (int j = 0; j < V; j++) printf("%-10s", names[j]);
    printf("\n");
    for (int i = 0; i < V; i++) {
        printf("%-10s", names[i]);
        for (int j = 0; j < V; j++) {
            if (dist[i][j] >= INF/2) printf("%-10s", "INF");
            else printf("%-10.0f", dist[i][j]);
        }
        printf("\n");
    }
}


void analyzeRouteNetwork() {
    printf("\nROUTE NETWORK ANALYSIS\n");

    int airportCount = sizeof(AIRPORTS) / sizeof(AIRPORTS[0]);
    int routeCount = sizeof(ROUTES) / sizeof(ROUTES[0]);

    char names[MAX_AIRPORTS][NAME_LEN];
    for (int i = 0; i < airportCount; i++) strcpy(names[i], AIRPORTS[i]);

    Graph g = createGraph(airportCount);
    for (int i = 0; i < routeCount; i++) {
        int s = findAirport(names, airportCount, ROUTES[i].src);
        int d = findAirport(names, airportCount, ROUTES[i].dest);
        if (s != -1 && d != -1) {
            g.adjMatrix[s][d] = ROUTES[i].weight;
            g.adjMatrix[d][s] = ROUTES[i].weight;
        }
    }
    printf("Airports: %d  Routes: %d\n", airportCount, routeCount);
    printGraph(&g, names);
    
    printf("\nMinimum Spanning Tree (Prim's)\n");
    clock_t primStart = clock();
    double primWeight = primMST(&g, names);
    double primTime = ((double)(clock() - primStart) * 1000) / CLOCKS_PER_SEC;
    printf("Total weight: %.0f (Time: %.3f ms)\n", primWeight, primTime);
    
    printf("\nMinimum Spanning Tree (Kruskal's)\n");
    clock_t mstStart = clock();
    double mstWeight = kruskalMST(&g, names);
    double mstTime = ((double)(clock() - mstStart) * 1000) / CLOCKS_PER_SEC;
    printf("Total weight: %.0f (Time: %.3f ms)\n", mstWeight, mstTime);
    
    printf("\n All-Pairs Shortest Paths (Floyd-Warshall) \n");
    clock_t fwStart = clock();
    floydWarshallAllPairs(&g, names);
    double fwTime = ((double)(clock() - fwStart) * 1000) / CLOCKS_PER_SEC;
    printf("\nTime: %.3f ms\n", fwTime);
    
    freeGraph(&g);
    printf("\nAnalysis complete.\n");
}


void mainMenu() {
    int choice;
    while (1) {
        printf("AIRLINE MANAGEMENT SYSTEM \n");
        printf("| 1. Add Flight \n");
        printf("| 2. Book Ticket \n");
        printf("| 3. Check-in Passenger \n");
        printf("| 4. Process Check-in Queue\n");
        printf("| 5. Display All Flights\n");
        printf("| 6. Display All Passengers \n");
        printf("| 7. Display Check-in Queue\n");
        printf("| 8. Analyze Route Network (MST)\n");
        printf("| 0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        
        switch (choice) {
            case 1: addFlight(); break;
            case 2: bookTicket(); break;
            case 3: checkInPassenger(); break;
            case 4: processCheckInQueue(); break;
            case 5: displayFlights(); break;
            case 6: displayPassengers(); break;
            case 7: displayCheckInQueue(); break;
            case 8: analyzeRouteNetwork(); break;
            case 0:
                printf("Thank you for using Airline Management System!\n");
                exit(0);
            default:
                printf("Invalid choice!\n");
        }
    }
}