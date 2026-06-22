#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <limits>

using namespace std;

// ============================================================================
// MODULE 1 & 2: MACHINE & PREDICTIVE HEALTH MONITORING
// ============================================================================
/**
 * MACHINE CLASS
 * Encapsulates the details of an industrial equipment asset.
 */
class Machine {
public:
    int id;
    string name;
    string location;
    int healthScore; 
    int faultCount;  
    Machine() : id(0), name(""), location(""), healthScore(100), faultCount(0) {}

    Machine(int id, string name, string location, int faults)
        : id(id), name(name), location(location), faultCount(faults) {
        calculateHealth();
    }

    void calculateHealth() {
        healthScore = 100 - (faultCount * 10);
        if (healthScore < 0) 
        {
            healthScore = 0;
        }
    }

    string getStatus() const {
        if (healthScore > 70) return "Healthy";
        if (healthScore >= 41) return "Maintenance Soon";
        return "Critical";
    }

    void display() const {
        cout << "ID: " << setw(5) << id
             << " | Name: " << setw(25) << left << name
             << " | Location: " << setw(20) << left << location
             << " | Health: " << setw(3) << right << healthScore << "% ["
             << setw(16) << left << getStatus() + "]"
             << " | Faults: " << faultCount << endl;
    }
};

// ============================================================================
// MODULE 3: MAINTENANCE REQUEST QUEUE (FIFO)
// ============================================================================

class Request {
public:
    int requestId;
    int machineId;
    string description;
    int priority; 

    Request() : requestId(0), machineId(0), description(""), priority(0) {}
    Request(int rId, int mId, string desc, int prio)
        : requestId(rId), machineId(mId), description(desc), priority(prio) {}

    void display() const {
        cout << "Req ID: " << setw(5) << requestId
             << " | Machine ID: " << setw(5) << machineId
             << " | Priority Score: " << setw(3) << priority
             << " | Issue: " << description << endl;
    }
};

/**
 * CUSTOM QUEUE CLASS
 */
class RequestQueue {
private:
    struct Node {
        Request data;
        Node* next;
        Node(const Request& r) : data(r), next(nullptr) {}
    };
    Node* frontNode;
    Node* rearNode;
    int count;

public:
    RequestQueue() : frontNode(nullptr), rearNode(nullptr), count(0) {}

    ~RequestQueue() {
        clear();
    }

    void enqueue(const Request& r) {
        Node* newNode = new Node(r);
        if (rearNode == nullptr) {
            frontNode = rearNode = newNode;
        } else {
            rearNode->next = newNode;
            rearNode = newNode;
        }
        count++;
    }

    bool dequeue() {
        if (isEmpty()) return false;
        Node* temp = frontNode;
        frontNode = frontNode->next;
        if (frontNode == nullptr) {
            rearNode = nullptr;
        }
        delete temp;
        count--;
        return true;
    }

    Request front() const {
        if (isEmpty()) return Request();
        return frontNode->data;
    }

    bool isEmpty() const {
        return frontNode == nullptr;
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void display() const {
        if (isEmpty()) {
            cout << "No pending maintenance requests.\n";
            return;
        }
        Node* curr = frontNode;
        while (curr != nullptr) {
            curr->data.display();
            curr = curr->next;
        }
    }
};

// ============================================================================
// MODULE 4: MAINTENANCE HISTORY STACK (LIFO)
// ============================================================================

struct HistoryRecord {
    string operationType; // "ADD", "UPDATE", "DELETE"
    Machine machineState;
};

/**
 * CUSTOM STACK CLASS
 * 
 * VIVA EXPLANATION:
 * - Why this data structure: Undo operations follow a Last-In-First-Out (LIFO) order.
 *   The last modification made is the first one reverted.
 * - Time Complexity: Push: O(1), Pop: O(1)
 * - Real-world purpose: Keeps a step-by-step history log to allow safety rollbacks of database edits.
 */
class HistoryStack {
private:
    struct Node {
        HistoryRecord data;
        Node* next;
        Node(const HistoryRecord& record) : data(record), next(nullptr) {}
    };
    Node* topNode;
    int count;

public:
    HistoryStack() : topNode(nullptr), count(0) {}

    ~HistoryStack() {
        clear();
    }

    void push(const HistoryRecord& record) {
        Node* newNode = new Node(record);
        newNode->next = topNode;
        topNode = newNode;
        count++;
    }

    bool pop() {
        if (isEmpty()) return false;
        Node* temp = topNode;
        topNode = topNode->next;
        delete temp;
        count--;
        return true;
    }

    HistoryRecord top() const {
        if (isEmpty()) return HistoryRecord();
        return topNode->data;
    }

    bool isEmpty() const {
        return topNode == nullptr;
    }

    int size() const {
        return count;
    }

    void clear() {
        while (!isEmpty()) {
            pop();
        }
    }

    void display() const {
        if (isEmpty()) {
            cout << "No modification history available.\n";
            return;
        }
        Node* curr = topNode;
        int i = 1;
        while (curr != nullptr) {
            cout << "  [" << i++ << "] Action: " << setw(8) << curr->data.operationType << " | ";
            curr->data.machineState.display();
            curr = curr->next;
        }
    }
};

// ============================================================================
// MODULE 1: BINARY SEARCH TREE (BST)
// ============================================================================

/**
 * CUSTOM BINARY SEARCH TREE (BST)
 * 
 * VIVA EXPLANATION:
 * - Why this data structure: Organizes machine records dynamically using ID as the key.
 * - Time Complexity: Insertion: O(log N) average, Deletion: O(log N) average, Search: O(log N) average.
 * - Real-world purpose: Allows sorted outputs (inorder traversal) and faster logarithmic operations.
 */
class MachineBST {
private:
    struct Node {
        Machine data;
        Node* left;
        Node* right;
        Node(const Machine& m) : data(m), left(nullptr), right(nullptr) {}
    };
    Node* root;

    Node* insertHelper(Node* node, const Machine& m) {
        if (node == nullptr) return new Node(m);
        if (m.id < node->data.id) {
            node->left = insertHelper(node->left, m);
        } else if (m.id > node->data.id) {
            node->right = insertHelper(node->right, m);
        }
        return node;
    }

    Node* findMin(Node* node) {
        while (node->left != nullptr) {
            node = node->left;
        }
        return node;
    }

    Node* deleteHelper(Node* node, int id, bool& success) {
        if (node == nullptr) {
            success = false;
            return nullptr;
        }
        if (id < node->data.id) {
            node->left = deleteHelper(node->left, id, success);
        } else if (id > node->data.id) {
            node->right = deleteHelper(node->right, id, success);
        } else {
            success = true;
            if (node->left == nullptr) {
                Node* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                Node* temp = node->left;
                delete node;
                return temp;
            }
            Node* temp = findMin(node->right);
            node->data = temp->data;
            node->right = deleteHelper(node->right, temp->data.id, success);
        }
        return node;
    }

    Node* searchHelper(Node* node, int id) const {
        if (node == nullptr || node->data.id == id) return node;
        if (id < node->data.id) return searchHelper(node->left, id);
        return searchHelper(node->right, id);
    }

    void inorderHelper(Node* node, vector<Machine>& list) const {
        if (node == nullptr) return;
        inorderHelper(node->left, list);
        list.push_back(node->data);
        inorderHelper(node->right, list);
    }

    void clearHelper(Node* node) {
        if (node == nullptr) return;
        clearHelper(node->left);
        clearHelper(node->right);
        delete node;
    }

public:
    MachineBST() : root(nullptr) {}

    ~MachineBST() {
        clear();
    }

    void insert(const Machine& m) {
        root = insertHelper(root, m);
    }

    bool remove(int id) {
        bool success = false;
        root = deleteHelper(root, id, success);
        return success;
    }

    Machine* search(int id) const {
        Node* node = searchHelper(root, id);
        if (node == nullptr) return nullptr;
        return &(node->data);
    }

    void collectAll(vector<Machine>& list) const {
        inorderHelper(root, list);
    }

    void clear() {
        clearHelper(root);
        root = nullptr;
    }

    bool isEmpty() const {
        return root == nullptr;
    }
};

// ============================================================================
// MODULE 7 - 10: MACHINE CONNECTIVITY GRAPH & TRAVERSALS
// ============================================================================

/**
 * CONNECTIVITY GRAPH CLASS (Adjacency List)
 * 
 * VIVA EXPLANATION:
 * - Why this data structure: Represents connections (power grid, network cables) between floor equipment.
 * - Time Complexity: Add Link: O(1), Search Link: O(V)
 * - Real-world purpose: Factory layouts require route mapping for service operations and failure propagations.
 */
class ConnectivityGraph {
private:
    struct Edge {
        int destId;
        double weight; // Distance/travel time in meters
    };
    unordered_map<int, vector<Edge>> adjList;

public:
    void addNode(int id) {
        if (adjList.find(id) == adjList.end()) {
            adjList[id] = vector<Edge>();
        }
    }

    void addConnection(int id1, int id2, double weight) {
        addNode(id1);
        addNode(id2);

        // Undirected graph connection (bidirectional links)
        adjList[id1].push_back({id2, weight});
        adjList[id2].push_back({id1, weight});
    }

    void removeConnection(int id1, int id2) {
        if (adjList.find(id1) != adjList.end()) {
            auto& edges = adjList[id1];
            for (auto it = edges.begin(); it != edges.end(); ++it) {
                if (it->destId == id2) {
                    edges.erase(it);
                    break;
                }
            }
        }
        if (adjList.find(id2) != adjList.end()) {
            auto& edges = adjList[id2];
            for (auto it = edges.begin(); it != edges.end(); ++it) {
                if (it->destId == id1) {
                    edges.erase(it);
                    break;
                }
            }
        }
    }

    void removeNode(int id) {
        if (adjList.find(id) == adjList.end()) return;

        // Remove node connections from neighbor lists
        vector<int> neighbors;
        for (const auto& edge : adjList[id]) {
            neighbors.push_back(edge.destId);
        }
        for (int neighborId : neighbors) {
            removeConnection(id, neighborId);
        }
        adjList.erase(id);
    }

    void display(const unordered_map<int, Machine>& machines) const {
        cout << "\n-------------------------------------------------------------\n";
        cout << "                 FACTORY FLOOR NETWORK MAP\n";
        cout << "-------------------------------------------------------------\n";
        if (adjList.empty()) {
            cout << "No nodes or connections configured in the network.\n";
            return;
        }

        for (const auto& pair : adjList) {
            int node = pair.first;
            string name = "Technician Room";
            if (node != 0) {
                auto it = machines.find(node);
                name = (it != machines.end()) ? it->second.name : "Unknown Machine";
            }
            cout << "Node ID: " << setw(3) << node << " [" << name << "]\n";
            if (pair.second.empty()) {
                cout << "  -> (Isolated / No Connections)\n";
            }
            for (const auto& edge : pair.second) {
                string destName = "Technician Room";
                if (edge.destId != 0) {
                    auto it = machines.find(edge.destId);
                    destName = (it != machines.end()) ? it->second.name : "Unknown Machine";
                }
                cout << "    -> Connected to: ID " << setw(3) << edge.destId 
                     << " [" << setw(25) << left << destName << "] | Distance: " 
                     << fixed << setprecision(1) << edge.weight << "m\n";
            }
            cout << endl;
        }
    }

    /**
     * BREADTH-FIRST SEARCH (BFS)
     * 
     * VIVA EXPLANATION:
     * - Purpose: Finds immediately adjacent connected machines. Used to trace physical outage spreads.
     * - Time Complexity: O(V + E) where V is vertices and E is edges.
     */
    void runBFS(int startId, const unordered_map<int, Machine>& machines) const {
        if (adjList.find(startId) == adjList.end()) {
            cout << "Error: Start Node with ID " << startId << " does not exist in the graph.\n";
            return;
        }

        unordered_map<int, bool> visited;
        queue<int> q;

        q.push(startId);
        visited[startId] = true;

        cout << "\nBFS Outage Spread propagation (Starting at ID " << startId << "):\n";
        int step = 1;
        while (!q.empty()) {
            int curr = q.front();
            q.pop();

            string name = "Technician Room";
            if (curr != 0) {
                auto it = machines.find(curr);
                name = (it != machines.end()) ? it->second.name : "Unknown Machine";
            }
            cout << "  Step " << step++ << ": Node ID " << curr << " (" << name << ")\n";

            for (const auto& edge : adjList.at(curr)) {
                if (!visited[edge.destId]) {
                    visited[edge.destId] = true;
                    q.push(edge.destId);
                }
            }
        }
    }

    /**
     * DEPTH-FIRST SEARCH (DFS)
     * 
     * VIVA EXPLANATION:
     * - Purpose: Traces deep dependency chains to search if an equipment failure breaks the sequence.
     * - Time Complexity: O(V + E)
     */
    void dfsHelper(int curr, unordered_map<int, bool>& visited, int& step, const unordered_map<int, Machine>& machines) const {
        visited[curr] = true;

        string name = "Technician Room";
        if (curr != 0) {
            auto it = machines.find(curr);
            name = (it != machines.end()) ? it->second.name : "Unknown Machine";
        }
        cout << "  Step " << step++ << ": Node ID " << curr << " (" << name << ")\n";

        for (const auto& edge : adjList.at(curr)) {
            if (!visited[edge.destId]) {
                dfsHelper(edge.destId, visited, step, machines);
            }
        }
    }

    void runDFS(int startId, const unordered_map<int, Machine>& machines) const {
        if (adjList.find(startId) == adjList.end()) {
            cout << "Error: Start Node with ID " << startId << " does not exist in the graph.\n";
            return;
        }

        unordered_map<int, bool> visited;
        cout << "\nDFS Dependency Path inspection (Starting at ID " << startId << "):\n";
        int step = 1;
        dfsHelper(startId, visited, step, machines);
    }

    /**
     * DIJKSTRA'S SHORTEST PATH ALGORITHM
     * 
     * VIVA EXPLANATION:
     * - Purpose: Finds the shortest physical route for a technician from Technician Room (0) to a target machine.
     * - Time Complexity: O(V^2) - clean arrays/lookup loops suitable for Viva presentation.
     */
    void runDijkstra(int startId, int targetId, const unordered_map<int, Machine>& machines) const {
        if (adjList.find(startId) == adjList.end() || adjList.find(targetId) == adjList.end()) {
            cout << "Error: Start or Target node does not exist in graph.\n";
            return;
        }

        const double INF = 1e9;
        vector<int> nodes;
        for (const auto& pair : adjList) {
            nodes.push_back(pair.first);
        }

        unordered_map<int, double> dist;
        unordered_map<int, int> parent;
        unordered_map<int, bool> visited;

        for (int node : nodes) {
            dist[node] = INF;
            parent[node] = -1;
            visited[node] = false;
        }

        dist[startId] = 0;

        for (size_t i = 0; i < nodes.size(); i++) {
            int u = -1;
            double minDist = INF;

            // Find unvisited node with minimum distance
            for (int node : nodes) {
                if (!visited[node] && dist[node] < minDist) {
                    minDist = dist[node];
                    u = node;
                }
            }

            if (u == -1) break;
            visited[u] = true;

            if (u == targetId) break;

            // Relax adjacent edges
            for (const auto& edge : adjList.at(u)) {
                int v = edge.destId;
                double weight = edge.weight;
                if (!visited[v] && dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    parent[v] = u;
                }
            }
        }

        if (dist[targetId] > INF / 2) {
            cout << "No route exists from ID " << startId << " to ID " << targetId << ".\n";
            return;
        }

        // Reconstruct path
        vector<int> path;
        int curr = targetId;
        while (curr != -1) {
            path.push_back(curr);
            curr = parent[curr];
        }
        reverse(path.begin(), path.end());

        // Display shortest path results
        cout << "\n=============================================================\n";
        cout << "                  DIJKSTRA ROUTE PATH PLANNER\n";
        cout << "=============================================================\n";
        cout << "Shortest Path Distance: " << dist[targetId] << " meters\n\n";
        cout << "Step-by-Step Path:\n  ";

        for (size_t i = 0; i < path.size(); i++) {
            int node = path[i];
            string name = "Technician Room";
            if (node != 0) {
                auto it = machines.find(node);
                name = (it != machines.end()) ? it->second.name : "Unknown Machine";
            }
            cout << name << " (ID: " << node << ")";
            if (i < path.size() - 1) {
                cout << " -> ";
            }
        }
        cout << "\n=============================================================\n";
    }
};

// ============================================================================
// MODULE 5: SORTING - MERGE SORT BY PRIORITY
// ============================================================================

struct MachinePriority {
    Machine machine;
    double priorityValue; // Computed priority
};

// Merge Sort Helpers
void merge(vector<MachinePriority>& arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector<MachinePriority> L(n1);
    vector<MachinePriority> R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        // Descending order sort (highest priority first)
        if (L[i].priorityValue >= R[j].priorityValue) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

/**
 * MERGE SORT
 * 
 * VIVA EXPLANATION:
 * - Purpose: Sorts equipment in descending order of calculated urgency score.
 * - Time Complexity: O(N log N) in all cases (worst, average, and best).
 * - Space Complexity: O(N) auxiliary storage space.
 */
void mergeSort(vector<MachinePriority>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSort(arr, l, m);
    mergeSort(arr, m + 1, r);
    merge(arr, l, m, r);
}

// ============================================================================
// MODULE 6: SEARCHING ALGORITHMS
// ============================================================================

/**
 * LINEAR SEARCH
 * 
 * VIVA EXPLANATION:
 * - Time Complexity: O(N) - sequential search space scanning.
 */
int linearSearchByID(const vector<Machine>& arr, int targetId) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i].id == targetId) return i;
    }
    return -1;
}

int linearSearchByName(const vector<Machine>& arr, const string& targetName) {
    for (size_t i = 0; i < arr.size(); ++i) {
        if (arr[i].name == targetName) return i;
    }
    return -1;
}

// Sorting helpers by ID/Name for Binary Search
void mergeByID(vector<Machine>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<Machine> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].id <= R[j].id) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortByID(vector<Machine>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortByID(arr, l, m);
    mergeSortByID(arr, m + 1, r);
    mergeByID(arr, l, m, r);
}

void mergeByName(vector<Machine>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<Machine> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i].name <= R[j].name) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSortByName(vector<Machine>& arr, int l, int r) {
    if (l >= r) return;
    int m = l + (r - l) / 2;
    mergeSortByName(arr, l, m);
    mergeSortByName(arr, m + 1, r);
    mergeByName(arr, l, m, r);
}

/**
 * BINARY SEARCH
 * 
 * VIVA EXPLANATION:
 * - Prerequisite: Input array must be pre-sorted.
 * - Time Complexity: O(log N) - search space halved each step.
 */
int binarySearchByID(const vector<Machine>& arr, int targetId) {
    int l = 0, r = arr.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (arr[m].id == targetId) return m;
        if (arr[m].id < targetId) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

int binarySearchByName(const vector<Machine>& arr, const string& targetName) {
    int l = 0, r = arr.size() - 1;
    while (l <= r) {
        int m = l + (r - l) / 2;
        if (arr[m].name == targetName) return m;
        if (arr[m].name < targetName) l = m + 1;
        else r = m - 1;
    }
    return -1;
}

// ============================================================================
// SYSTEM COORDINATOR (MAIN PROGRAM LOGIC)
// ============================================================================

class SystemCoordinator {
private:
    MachineBST bst;
    unordered_map<int, Machine> machineMap;
    RequestQueue requestQueue;
    HistoryStack undoStack;
    ConnectivityGraph graph;
    int nextRequestId;

public:
    SystemCoordinator() : nextRequestId(501) {}

    // Load initial sample data
    void loadSampleData() {
        Machine m1(101, "CNC Milling Machine", "Aisle A", 2); // Health: 100 - 20 = 80 (Healthy)
        Machine m2(102, "Robotic Welding Arm", "Assembly Line 1", 0); // Health: 100 (Healthy)
        Machine m3(103, "Hydraulic Press", "Aisle B", 6); // Health: 100 - 60 = 40 (Critical)
        Machine m4(104, "Injection Molding Machine", "Plastics Division", 4); // Health: 100 - 40 = 60 (Maintenance Soon)
        Machine m5(105, "Industrial Air Compressor", "Utility Room", 1); // Health: 100 - 10 = 90 (Healthy)

        insertMachineInternal(m1, false);
        insertMachineInternal(m2, false);
        insertMachineInternal(m3, false);
        insertMachineInternal(m4, false);
        insertMachineInternal(m5, false);

        // Set up factory floor layout connectivity graph
        graph.addNode(0); // Technician Room
        graph.addConnection(0, 101, 15.0);
        graph.addConnection(0, 102, 25.0);
        graph.addConnection(101, 102, 10.0);
        graph.addConnection(101, 103, 30.0);
        graph.addConnection(102, 104, 20.0);
        graph.addConnection(103, 105, 12.0);
        graph.addConnection(104, 105, 18.0);

        requestQueue.enqueue(Request(nextRequestId++, 103, "Hydraulic fluid leak detected", 80));
        requestQueue.enqueue(Request(nextRequestId++, 104, "Calibration offset error", 40));
    }

    void insertMachineInternal(const Machine& m, bool trackUndo = true) {
        bst.insert(m);
        machineMap[m.id] = m;
        graph.addNode(m.id);

        if (trackUndo) {
            undoStack.push({"ADD", m});
        }
    }

    void addMachine() {
        cout << "\n--- Add New Machine ---\n";
        int id;
        cout << "Enter Machine ID: ";
        if (!(cin >> id) || id <= 0) {
            cout << "Invalid ID. Operation canceled.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        // Duplicate Validation
        if (machineMap.find(id) != machineMap.end()) {
            cout << "Error: Machine with ID " << id << " already exists!\n";
            return;
        }

        string name, location;
        cout << "Enter Machine Name: ";
        getline(cin >> ws, name);
        cout << "Enter Location: ";
        getline(cin >> ws, location);

        int faults;
        cout << "Enter Fault Count: ";
        if (!(cin >> faults) || faults < 0) {
            cout << "Invalid Fault Count. Setting to 0.\n";
            faults = 0;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        Machine newMach(id, name, location, faults);
        insertMachineInternal(newMach, true);

        cout << "Machine successfully added.\n";
        newMach.display();
    }

    void searchMachine() {
        cout << "\n--- Search Machine ---\n";
        cout << "1. O(1) Hash Map Search (Standard)\n";
        cout << "2. Linear Search by Name (O(N))\n";
        cout << "3. Binary Search by ID (O(log N))\n";
        int choice;
        cout << "Enter Choice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return;
        }

        if (choice == 1) {
            int id;
            cout << "Enter Machine ID: ";
            cin >> id;
            auto it = machineMap.find(id);
            if (it != machineMap.end()) {
                cout << "\n[O(1) Hash Lookup Result] Found:\n  ";
                it->second.display();
            } else {
                cout << "Machine ID " << id << " not found.\n";
            }
        } 
        else if (choice == 2) {
            vector<Machine> list;
            bst.collectAll(list);
            string name;
            cout << "Enter Machine Name: ";
            getline(cin >> ws, name);

            int res = linearSearchByName(list, name);
            if (res != -1) {
                cout << "\n[O(N) Linear Search Result] Found:\n  ";
                list[res].display();
            } else {
                cout << "Machine with name '" << name << "' not found.\n";
            }
        } 
        else if (choice == 3) {
            vector<Machine> list;
            bst.collectAll(list);
            int id;
            cout << "Enter Machine ID to Search: ";
            cin >> id;

            // Sort by ID before Binary Search
            mergeSortByID(list, 0, list.size() - 1);
            int res = binarySearchByID(list, id);
            if (res != -1) {
                cout << "\n[O(log N) Binary Search Result] Found:\n  ";
                list[res].display();
            } else {
                cout << "Machine ID " << id << " not found.\n";
            }
        }
    }

    void updateMachine() {
        cout << "\n--- Update Machine Details ---\n";
        int id;
        cout << "Enter Machine ID to Update: ";
        cin >> id;

        auto it = machineMap.find(id);
        if (it == machineMap.end()) {
            cout << "Error: Machine not found.\n";
            return;
        }

        Machine oldState = it->second;
        undoStack.push({"UPDATE", oldState});

        string name, location;
        cout << "Enter New Name: ";
        getline(cin >> ws, name);
        cout << "Enter New Location: ";
        getline(cin >> ws, location);

        int faults;
        cout << "Enter New Fault Count: ";
        cin >> faults;

        bst.remove(id);
        Machine updatedMach(id, name, location, faults);
        bst.insert(updatedMach);
        machineMap[id] = updatedMach;

        cout << "Machine details updated successfully.\n";
        updatedMach.display();
    }

    void deleteMachine() {
        cout << "\n--- Delete Machine ---\n";
        int id;
        cout << "Enter Machine ID to Delete: ";
        cin >> id;

        auto it = machineMap.find(id);
        if (it == machineMap.end()) {
            cout << "Error: Machine not found.\n";
            return;
        }

        Machine oldState = it->second;
        undoStack.push({"DELETE", oldState});

        bst.remove(id);
        machineMap.erase(id);
        graph.removeNode(id);

        cout << "Machine ID " << id << " deleted from the system.\n";
    }

    void displayAllMachines() {
        cout << "\n=========================================================================================\n";
        cout << "                               ALL REGISTERED MACHINES (BST In-Order)\n";
        cout << "=========================================================================================\n";
        vector<Machine> list;
        bst.collectAll(list);

        if (list.empty()) {
            cout << "No machines registered in system.\n";
            return;
        }

        for (const auto& m : list) {
            m.display();
        }
        cout << "=========================================================================================\n";
    }

    void addMaintenanceRequest() {
        cout << "\n--- Add Maintenance Request ---\n";
        int machId;
        cout << "Enter Machine ID: ";
        cin >> machId;

        auto it = machineMap.find(machId);
        if (it == machineMap.end()) {
            cout << "Error: Machine does not exist.\n";
            return;
        }

        string desc;
        cout << "Enter Issue Description: ";
        getline(cin >> ws, desc);

        // Priority Score Formula: (100 - Health Score) + Fault Count * 5
        int priorityVal = (100 - it->second.healthScore) + (it->second.faultCount * 5);

        Request req(nextRequestId++, machId, desc, priorityVal);
        requestQueue.enqueue(req);

        cout << "Maintenance request successfully registered (FIFO Queue).\n";
        req.display();
    }

    void processRequest() {
        cout << "\n--- Process Maintenance Request ---\n";
        if (requestQueue.isEmpty()) {
            cout << "No pending requests in queue.\n";
            return;
        }

        Request req = requestQueue.front();
        requestQueue.dequeue();

        cout << "Processing Request ID " << req.requestId << " for Machine ID " << req.machineId << "...\n";
        cout << "Description: " << req.description << "\n";

        // Restore machine state to Healthy (Fault Count = 0)
        auto it = machineMap.find(req.machineId);
        if (it != machineMap.end()) {
            undoStack.push({"UPDATE", it->second});

            it->second.faultCount = 0;
            it->second.calculateHealth();

            // Sync BST
            bst.remove(req.machineId);
            bst.insert(it->second);

            cout << "Repair complete. Machine health restored.\n";
            it->second.display();
        } else {
            cout << "Machine was removed prior to processing.\n";
        }
    }

    void undoLastUpdate() {
        if (undoStack.isEmpty()) {
            cout << "No modification history to undo.\n";
            return;
        }

        HistoryRecord record = undoStack.top();
        undoStack.pop();

        cout << "\n[UNDO] Rolling back last operation (" << record.operationType << ")...\n";

        if (record.operationType == "UPDATE") {
            bst.remove(record.machineState.id);
            bst.insert(record.machineState);
            machineMap[record.machineState.id] = record.machineState;
        } 
        else if (record.operationType == "ADD") {
            bst.remove(record.machineState.id);
            machineMap.erase(record.machineState.id);
            graph.removeNode(record.machineState.id);
        } 
        else if (record.operationType == "DELETE") {
            bst.insert(record.machineState);
            machineMap[record.machineState.id] = record.machineState;
            graph.addNode(record.machineState.id);
        }
        cout << "Undo successful. Restored Machine State:\n  ";
        record.machineState.display();
    }

    void sortMachinesByPriority() {
        cout << "\n=========================================================================================\n";
        cout << "                          MAINTENANCE SCHEDULE (MERGE SORT PRIORITY)\n";
        cout << "=========================================================================================\n";
        vector<Machine> list;
        bst.collectAll(list);

        if (list.empty()) {
            cout << "No machines in system.\n";
            return;
        }

        vector<MachinePriority> priorityList;
        for (const auto& m : list) {
            double priorityVal = (100 - m.healthScore) + m.faultCount * 5;
            priorityList.push_back({m, priorityVal});
        }

        // Run custom Merge Sort
        mergeSort(priorityList, 0, priorityList.size() - 1);

        cout << left << setw(10) << "Rank"
             << setw(12) << "Priority"
             << setw(8) << "ID"
             << setw(25) << "Machine Name"
             << setw(15) << "Status"
             << "Health\n";
        cout << "-----------------------------------------------------------------------------------------\n";

        int rank = 1;
        for (const auto& entry : priorityList) {
            cout << left << setw(10) << rank++
                 << setw(12) << entry.priorityValue
                 << setw(8) << entry.machine.id
                 << setw(25) << entry.machine.name
                 << setw(15) << entry.machine.getStatus()
                 << entry.machine.healthScore << "%\n";
        }
        cout << "=========================================================================================\n";
    }

    void connectMachines() {
        cout << "\n--- Connect Machines (Graph Link) ---\n";
        int id1, id2;
        cout << "Enter First Node ID (0 for Tech Room): ";
        cin >> id1;
        cout << "Enter Second Node ID (0 for Tech Room): ";
        cin >> id2;

        if (id1 == id2) {
            cout << "Nodes must be different.\n";
            return;
        }

        if (id1 != 0 && machineMap.find(id1) == machineMap.end()) {
            cout << "Invalid Node ID " << id1 << ".\n";
            return;
        }
        if (id2 != 0 && machineMap.find(id2) == machineMap.end()) {
            cout << "Invalid Node ID " << id2 << ".\n";
            return;
        }

        double distance;
        cout << "Enter Physical Distance (meters): ";
        cin >> distance;

        if (distance <= 0) {
            cout << "Distance must be positive.\n";
            return;
        }

        graph.addConnection(id1, id2, distance);
        cout << "Link added successfully.\n";
    }

    void runBFS() {
        cout << "\n--- Run BFS Outage Spread propagation ---\n";
        int startId;
        cout << "Enter Start Node ID (0 for Tech Room): ";
        cin >> startId;
        graph.runBFS(startId, machineMap);
    }

    void runDFS() {
        cout << "\n--- Run DFS Dependency Traversal ---\n";
        int startId;
        cout << "Enter Start Node ID (0 for Tech Room): ";
        cin >> startId;
        graph.runDFS(startId, machineMap);
    }

    void runDijkstra() {
        cout << "\n--- Run Dijkstra Route Path Planner ---\n";
        int targetId;
        cout << "Enter Target Machine ID to route: ";
        cin >> targetId;
        // Start route from Technician Room (0)
        graph.runDijkstra(0, targetId, machineMap);
    }

    void displayNetwork() {
        graph.display(machineMap);
    }

    void displayRequests() {
        cout << "\n=========================================================================================\n";
        cout << "                               PENDING FIFO MAINTENANCE QUEUE\n";
        cout << "=========================================================================================\n";
        requestQueue.display();
        cout << "=========================================================================================\n";
    }
};

// ============================================================================
// MAIN FUNCTION & CLI MENU
// ============================================================================

int main() {
    SystemCoordinator coordinator;
    coordinator.loadSampleData();

    cout << "=========================================================================================\n";
    cout << "        WELCOME TO SMART INDUSTRIAL EQUIPMENT MAINTENANCE & PREDICTIVE MONITORING\n";
    cout << "=========================================================================================\n";
    cout << "Sample data loaded successfully. Ready for DSA Viva demonstration.\n";

    while (true) {
        cout << "\n=============================================\n";
        cout << "                  MAIN MENU\n";
        cout << "=============================================\n";
        cout << " 1. Add Machine\n";
        cout << " 2. Search Machine\n";
        cout << " 3. Update Machine\n";
        cout << " 4. Delete Machine\n";
        cout << " 5. Display Machines\n";
        cout << " 6. Add Maintenance Request\n";
        cout << " 7. Process Maintenance Request\n";
        cout << " 8. Undo Last Update\n";
        cout << " 9. Sort Machines By Priority\n";
        cout << "10. Connect Machines\n";
        cout << "11. Run BFS\n";
        cout << "12. Run DFS\n";
        cout << "13. Run Dijkstra\n";
        cout << "14. Exit\n";
        cout << "=============================================\n";

        int choice;
        cout << "Select Option (1-14): ";
        if (!(cin >> choice)) {
            cout << "Invalid input. Please choose a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: coordinator.addMachine(); break;
            case 2: coordinator.searchMachine(); break;
            case 3: coordinator.updateMachine(); break;
            case 4: coordinator.deleteMachine(); break;
            case 5: coordinator.displayAllMachines(); break;
            case 6: coordinator.addMaintenanceRequest(); break;
            case 7: coordinator.processRequest(); break;
            case 8: coordinator.undoLastUpdate(); break;
            case 9: coordinator.sortMachinesByPriority(); break;
            case 10: coordinator.connectMachines(); break;
            case 11: coordinator.runBFS(); break;
            case 12: coordinator.runDFS(); break;
            case 13: coordinator.runDijkstra(); break;
            case 14:
                cout << "\nExiting... Good luck with your DSA Viva!\n";
                return 0;
            default:
                cout << "Invalid choice. Please pick between 1 and 14.\n";
        }
    }
    return 0;
}
