#include <windows.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <iomanip> 

using namespace std;


const char* BIGGER_FONT_CODE = "\033[2m";
const char* BOLD_TEXT_CODE = "\033[1m";
const char* RED_COLOR_CODE = "\033[31m";
const char* GREEN_COLOR_CODE = "\033[32m";
const char* YELLOW_COLOR_CODE = "\033[33m";
const char* GREY_COLOR_CODE = "\033[90m";
const char* RESET_CODE = "\033[0m";

const int MAX_NODES = 400;
const int INF = 1e9;

struct node {
    int id;             //generated randomly
    int reward_score;       //score of a particular reward you obtained

    node* left;
    node* right;
    int height;
};

struct kruskalEdge {
    int u, v, weight;
    bool operator<(const kruskalEdge& other) const {
        return weight < other.weight;
    }
};


int height(node* root) {
    if (root == NULL)
        return 0;
    return root->height;
}
int balanceFactor(node* root) {
    if (root == NULL)
        return 0;
    return height(root->left) - height(root->right);
}
node* rotateLeft(node* root) {
    node* newRoot = root->right;
    root->right = newRoot->left;
    newRoot->left = root;
    root->height = max(height(root->left), height(root->right)) + 1;
    newRoot->height = max(height(newRoot->left), height(newRoot->right)) + 1;
    return newRoot;
}
node* rotateRight(node* root) {
    node* newRoot = root->left;
    root->left = newRoot->right;
    newRoot->right = root;
    root->height = max(height(root->left), height(root->right)) + 1;
    newRoot->height = max(height(newRoot->left), height(newRoot->right)) + 1;
    return newRoot;
}

class AVLTree {
public:
    node* root;
    int count;
    int totalScore;
    AVLTree() {
        root = NULL;
        count = 0;
        totalScore = 0;
    }

    node* createNode(int id, int scr) {
        node* newNode = new node();
        newNode->id = id;
        newNode->reward_score = scr;
        newNode->left = nullptr;
        newNode->right = nullptr;
        newNode->height = 1;
        count++;
        return newNode;
    }
    node* insert(node* &root, int key, int scr) {
        if (root == NULL)
            return createNode(key, scr);

        if (key < root->id)
            root->left = insert(root->left, key, scr);
        else if (key > root->id)
            root->right = insert(root->right, key, scr);
        else
            return root; // Key already exists in the tree

        root->height = max(height(root->left), height(root->right)) + 1;
        int bf = balanceFactor(root);

        if (bf > 1 && key < root->left->id)
            return rotateRight(root);

        if (bf > 1 && key > root->left->id) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }

        if (bf < -1 && key > root->right->id)
            return rotateLeft(root);

        if (bf < -1 && key < root->right->id) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }
        count++;
        return root;
    }
    node* findMin(node* root) {
        while (root->left != NULL)
            root = root->left;
        return root;
    }
    node* delete_node(node* root, int key) {
        if (root == NULL)
            return root;

        if (key < root->id)
            root->left = delete_node(root->left, key);
        else if (key > root->id)
            root->right = delete_node(root->right, key);
        else {
            count--;
            if (root->left == NULL && root->right == NULL) {
                delete root;
                return NULL;
            }
            else if (root->left == NULL) {
                node* temp = root->right;
                delete root;
                return temp;
            }
            else if (root->right == NULL) {
                node* temp = root->left;
                delete root;
                return temp;
            }
            else {
                node* minNode = findMin(root->right);
                root->id = minNode->id;
                root->right = delete_node(root->right, minNode->id);
            }
        }

        if (root == NULL)
            return root;

        root->height = max(height(root->left), height(root->right)) + 1;
        int bf = balanceFactor(root);

        if (bf > 1 && balanceFactor(root->left) >= 0)
            return rotateRight(root);

        if (bf > 1 && balanceFactor(root->left) < 0) {
            root->left = rotateLeft(root->left);
            return rotateRight(root);
        }

        if (bf < -1 && balanceFactor(root->right) <= 0)
            return rotateLeft(root);

        if (bf < -1 && balanceFactor(root->right) > 0) {
            root->right = rotateRight(root->right);
            return rotateLeft(root);
        }

        return root;
    }

    bool search(node* root, int key) {
        if (root == NULL)
            return false;
        if (root->id == key)
            return true;
        else if (root->id > key)
            return search(root->left, key);
        else
            return search(root->right, key);
    }
    int size(node* root) {
        if (root == NULL)
            return 0;
        return size(root->left) + size(root->right) + 1;
    }

    void traverse(node* root) {
        if (root == NULL)
            return;
        traverse(root->right);
        totalScore += root->reward_score;
        traverse(root->left);
    }
};

class Graph {
public:
    char** matrix;
    int** adjMatrix;

    const int ROW = 400;
    const int COL = 400;


    char** createGraph() {
        char** matrix = new char* [20];
        for (int i = 0; i < 20; i++)
            matrix[i] = new char[20];

        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 20; ++j) {
                matrix[i][j] = 'C';
            }
        }

        // Add the crystal to a random location in the matrix
        int crystal_x = rand() % 20, crystal_y = rand() % 20;
        matrix[crystal_x][crystal_y] = '*';

        // Add 25 rewards to random locations in the matrix
        char rewards[] = { 'J', 'P', 'W' };
        for (int i = 0; i < 25; ++i) {
            int reward_x = rand() % 20, reward_y = rand() % 20;
            while (matrix[reward_x][reward_y] != 'C') {
                reward_x = rand() % 20;
                reward_y = rand() % 20;
            }
            matrix[reward_x][reward_y] = rewards[rand() % 3];
        }

        // Add 15 death points to random locations in the matrix
        for (int i = 0; i < 15; ++i) {
            int death_x = rand() % 20, death_y = rand() % 20;
            while (matrix[death_x][death_y] != 'C') {
                death_x = rand() % 20;
                death_y = rand() % 20;
            }
            matrix[death_x][death_y] = '%';
        }

        // Add 50 obstacles to random locations in the matrix
        for (int i = 0; i < 50; ++i) {
            int obstacle_x = rand() % 20, obstacle_y = rand() % 20;
            while (matrix[obstacle_x][obstacle_y] != 'C') {
                obstacle_x = rand() % 20;
                obstacle_y = rand() % 20;
            }
            matrix[obstacle_x][obstacle_y] = '#';
        }

        // Add 20 monsters to random locations in the matrix
        char monsters[] = { '&', '$', '@' };
        for (int i = 0; i < 20; ++i) {
            int monster_x = rand() % 20, monster_y = rand() % 20;
            while (matrix[monster_x][monster_y] != 'C') {
                monster_x = rand() % 20;
                monster_y = rand() % 20;
            }
            matrix[monster_x][monster_y] = monsters[rand() % 3];
        }
        matrix[0][0] = 'C';

        cout << "\033[33m\n\t\t\t\t\t\tGame Map\n\n\033[0m";
        // Print the matrix
        for (int i = 0; i < 20; ++i) {
            cout << "\t\t\t\t  ";
            for (int j = 0; j < 20; ++j) {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
        cout << "\n\n";

        return matrix;
    }
    int** createAdjacencyMatrix(char** matrix, int& index_i, int& index_j) {
        int** adjMatrix = new int* [400];
        for (int i = 0; i < 400; i++)
            adjMatrix[i] = new int[400];

        for (int i = 0; i < 400; i++) {
            for (int j = 0; j < 400; j++) {
                adjMatrix[i][j] = 0;
            }
        }

        // Loop through each cell in the matrix
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 20; j++) {
                if (matrix[i][j] == '*') {
                    index_i = index_j = (i * 20) + j;
                }


                // Check if the cell is not a wall
                if (matrix[i][j] != '#') {
                    // Check the adjacent cells and add edges if they are not walls
                    if (i > 0 && matrix[i - 1][j] != '#') {
                        adjMatrix[i * 20 + j][(i - 1) * 20 + j] = 1;
                        adjMatrix[(i - 1) * 20 + j][i * 20 + j] = 1;
                    }
                    if (i < 19 && matrix[i + 1][j] != '#') {
                        adjMatrix[i * 20 + j][(i + 1) * 20 + j] = 1;
                        adjMatrix[(i + 1) * 20 + j][i * 20 + j] = 1;
                    }
                    if (j > 0 && matrix[i][j - 1] != '#') {
                        adjMatrix[i * 20 + j][i * 20 + (j - 1)] = 1;
                        adjMatrix[i * 20 + (j - 1)][i * 20 + j] = 1;
                    }
                    if (j < 19 && matrix[i][j + 1] != '#') {
                        adjMatrix[i * 20 + j][i * 20 + (j + 1)] = 1;
                        adjMatrix[i * 20 + (j + 1)][i * 20 + j] = 1;
                    }
                }
                //  Obstacles
                if (i > 0 && matrix[i - 1][j] == '#') {
                    adjMatrix[i * 20 + j][(i - 1) * 20 + j] = 100;
                    adjMatrix[(i - 1) * 20 + j][i * 20 + j] = 100;

                }
                if (i < 19 && matrix[i + 1][j] == '#') {
                    adjMatrix[i * 20 + j][(i + 1) * 20 + j] = 100;
                    adjMatrix[(i + 1) * 20 + j][i * 20 + j] = 100;

                }
                if (j > 0 && matrix[i][j - 1] == '#') {
                    adjMatrix[i * 20 + j][i * 20 + (j - 1)] = 100;
                    adjMatrix[i * 20 + (j - 1)][i * 20 + j] = 100;
                }
                if (j < 19 && matrix[i][j + 1] == '#') {
                    adjMatrix[i * 20 + j][i * 20 + (j + 1)] = 100;
                    adjMatrix[i * 20 + (j + 1)][i * 20 + j] = 100;
                }



            }
        }

        return adjMatrix;
    }
    
    //--------------------------------------------------------------------------
    bool findDeath(int* path, char** matrix) {
        int i = 0;
        while (path[i] < 400 && path[i] >= 0) {
            int _i = path[i] / 20;
            int _j = path[i] % 20;
            if (matrix[_i][_j] == '%') {

                return true;
            }

            i++;
        }
        return false;
    }
    int dijkstra(int** matrix, int n, int start, int end, int* path) {
        int* dist = new int[n];
        bool* visited = new bool[n];
        int* prev = new int[n];

        for (int i = 0; i < n; i++) {
            dist[i] = INT_MAX;
            visited[i] = false;
            prev[i] = -1;
        }

        dist[start] = 0;

        for (int count = 0; count < n - 1; count++) {
            int u = -1;
            for (int i = 0; i < n; i++) {
                if (!visited[i] && (u == -1 || dist[i] < dist[u])) {
                    u = i;
                }
            }

            if (u == end) {
                int p = end;
                int i = 0;
                while (p != -1) {
                    path[i++] = p;
                    p = prev[p];
                }
                reverse(path, path + i);
                return dist[end];
            }

            visited[u] = true;

            int v = 0;
            while(v<n){
                if (matrix[u][v] != 0 && !visited[v]) {
                    int alt = dist[u] + matrix[u][v];
                    if (alt < dist[v]) {
                        dist[v] = alt;
                        prev[v] = u;
                    }
                }
                v++;
            }
        }

        // Build an empty path
        path[0] = -1;

        return dist[end];
    }
    void display_dijkstra(AVLTree& Dijkstra_Score, char** matrix, int* path, int index_i, int index_j, int shortestCost) {

        if (findDeath(path, matrix)) {
            Beep(500, 500);
            cout << RED_COLOR_CODE << "\t\tDeath point" << RESET_CODE << endl;
            exit(EXIT_FAILURE);
        }

        cout << "\tThe shortest cost from index (0, 0)" << " to index (" << index_i << ", " << index_j << ") is: " << shortestCost << endl;
        cout << "\tThe optimal path is: \n\t\t";
        for (int i = 0; i < 400 && path[i] != -1 && path[i] < 400 && path[i] >= 0; i++) {
            int _i = path[i] / 20;
            int _j = path[i] % 20;
            int score = 0;

            if (matrix[_i][_j] == 'J' || matrix[_i][_j] == 'P' || matrix[_i][_j] == 'W' || matrix[_i][_j] == '&' || matrix[_i][_j] == '$' || matrix[_i][_j] == '@') {
                switch (matrix[_i][_j]) {
                case 'J':
                    score = 50;
                    break;
                case 'P':
                    score = 70;
                    break;
                case 'W':
                    score = 30;
                    break;
                case '&':
                    score = -50;
                    break;
                case '$':
                    score = -70;
                    break;
                case '@':
                    score = -30;
                    break;

                }
                if (score != 0) {
                    if (Dijkstra_Score.root == NULL) {
                        Dijkstra_Score.root = Dijkstra_Score.insert(Dijkstra_Score.root, 100, score);
                    }
                    else {
                        int randID = rand() % 200;
                        Dijkstra_Score.root = Dijkstra_Score.insert(Dijkstra_Score.root, randID, score);
                    }
                }
            }
            if (path[i] < 400 && path[i] >= 0)
                cout << path[i];
            if (i != 400 - 1 && path[i + 1] != -1 && path[i + 1] < 400 && path[i + 1] >= 0) {
                cout << GREY_COLOR_CODE << " -> " << RESET_CODE;
            }
        }
        cout << endl;
        Dijkstra_Score.traverse(Dijkstra_Score.root);
        if (Dijkstra_Score.totalScore >= 0)
            cout << "\n\tTotal Score = " << GREEN_COLOR_CODE << Dijkstra_Score.totalScore << RESET_CODE << endl;
        else if (Dijkstra_Score.totalScore < 0)
            cout << "\n\tTotal Score = " << RED_COLOR_CODE << Dijkstra_Score.totalScore << RESET_CODE << endl;

    }

    //--------------------------------------------------------------------------
    void floyd(int** adj_matrix, int n, int index_i, int index_j, int**& dist, int**& path) {

        // Initialize the distance and path matrices
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    dist[i][j] = 0; // Distance from a vertex to itself is 0
                    path[i][j] = -1; // No path exists from a vertex to itself
                }
                else if (adj_matrix[i][j] == 1) {
                    dist[i][j] = 1; // Distance between adjacent vertices is 1
                    path[i][j] = j; // Path from vertex i to j is direct
                }
                else {
                    dist[i][j] = INF; // Distance between non-adjacent vertices is infinite
                    path[i][j] = -1; // No path exists from a vertex i to j
                }
            }
        }

        // Update the distance and path matrices
        for (int k = 0; k < n; k++) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (dist[i][k] != INF && dist[k][j] != INF && dist[i][k] + dist[k][j] < dist[i][j]) {
                        dist[i][j] = dist[i][k] + dist[k][j];
                        path[i][j] = path[i][k];
                    }
                }
            }
        }

        AVLTree floyd_Score;

        int next = path[index_i][index_j];
        while (next != -1) {
            int _i = next / 20;
            int _j = next % 20;
            int score = 0;

            if (matrix[_i][_j] == 'J' || matrix[_i][_j] == 'P' || matrix[_i][_j] == 'W' || matrix[_i][_j] == '&' || matrix[_i][_j] == '$' || matrix[_i][_j] == '@' || matrix[_i][_j] == '%') {
                switch (matrix[_i][_j]) {
                case 'J':
                    score = 50;
                    break;
                case 'P':
                    score = 70;
                    break;
                case 'W':
                    score = 30;
                    break;
                case '&':
                    score = -50;
                    break;
                case '$':
                    score = -70;
                    break;
                case '@':
                    score = -30;
                    break;
                case '%':
                    Beep(500, 500);
                    cout << RED_COLOR_CODE << "\t\tDeath point" << RESET_CODE << endl;
                    exit(EXIT_FAILURE);
                    break;
                

                }
                if (score != 0) {
                    if (floyd_Score.root == NULL) {
                        floyd_Score.root = floyd_Score.insert(floyd_Score.root, 100, score);
                    }
                    else {
                        int randID = rand() % 200;
                        floyd_Score.root = floyd_Score.insert(floyd_Score.root, randID, score);
                    }
                }
            }
            
            
            if (next == index_j) {
                break;
            }
            next = path[next][index_j];
        }


        // Print the optimal cost and optimal path between every pair of vertices
        cout << "\tThe shortest cost from index (0, 0)" << " to index (" << index_j << ", " << index_j << ") is: " << dist[index_i][index_j] << endl;
   
        // Print the optimal path between vertices i and j
        cout << "\tThe optimal path is: \n\t\t" << index_i;
        next = path[index_i][index_j];
        while (next != -1) {
            cout << GREY_COLOR_CODE << " -> " << RESET_CODE << next;
            if (next == index_j) {
                break;
            }
            next = path[next][index_j];
        }
        cout << endl;
        floyd_Score.traverse(floyd_Score.root);
        if (floyd_Score.totalScore >= 0)
            cout << "\n\tTotal Score = " << GREEN_COLOR_CODE << floyd_Score.totalScore << RESET_CODE << endl;
        else if (floyd_Score.totalScore < 0)
            cout << "\n\tTotal Score = " << RED_COLOR_CODE << floyd_Score.totalScore << RESET_CODE << endl;
       

    }
        
    //--------------------------------------------------------------------------
    int parent[400]; 
    int key[400];
    bool mstSet[400];

    int kScore = 0;

    int minKey(int key[], bool mstSet[]) {
        int min = INT_MAX, min_index;

        for (int v = 0; v < ROW; v++)
            if (mstSet[v] == false && key[v] < min)
                min = key[v], min_index = v;

        return min_index;
    }
    void findPrimsDeath(int parent[], int dest, AVLTree& prims_Score) {
        if (parent[dest] == -1)
            return;

        findPrimsDeath(parent, parent[dest], prims_Score);

        int _i = (dest % COL) / 20;
        int _j = (dest % COL) % 20;
        int score = 0;

        if (matrix[_i][_j] == 'J' || matrix[_i][_j] == 'P' || matrix[_i][_j] == 'W' || matrix[_i][_j] == '&' || matrix[_i][_j] == '$' || matrix[_i][_j] == '@' || matrix[_i][_j] == '%') {
            switch (matrix[_i][_j]) {
            case 'J':
                score = 50;
                break;
            case 'P':
                score = 70;
                break;
            case 'W':
                score = 30;
                break;
            case '&':
                score = -50;
                break;
            case '$':
                score = -70;
                break;
            case '@':
                score = -30;
                break;
            case '%':
                Beep(500, 500);
                cout << RED_COLOR_CODE << "\t\tDeath point" << RESET_CODE << endl;
                exit(EXIT_FAILURE);
                break;


            }
            if (score != 0) {
                if (prims_Score.root == NULL) {
                    prims_Score.root = prims_Score.insert(prims_Score.root, 100, score);
                }
                else {
                    int randID = rand() % 200;
                    prims_Score.root = prims_Score.insert(prims_Score.root, randID, score);
                }
            }
        }

    }
    void printMST(int parent[], int dest) {
        if (parent[dest] == -1)
            return;

        printMST(parent, parent[dest]);
        cout << GREY_COLOR_CODE << " -> " << RESET_CODE << dest % COL;
    }

    void primMST(int src, int dest) {
        // initialize key values and mstSet
        for (int i = 0; i < ROW; i++) {
            key[i] = INT_MAX;
            mstSet[i] = false;
            parent[i] = -1;
        }

        key[src] = 0;

        for (int count = 0; count < ROW - 1; count++) {
            int u = minKey(key, mstSet);

            mstSet[u] = true;

            for (int v = 0; v < ROW; v++) {
                if (adjMatrix[u][v] && mstSet[v] == false && adjMatrix[u][v] < key[v]) {
                    parent[v] = u;
                    key[v] = adjMatrix[u][v];
                }
            }
        }
        AVLTree prims_Score;

        findPrimsDeath(parent, dest, prims_Score);

        // calculate the cost of the minimum spanning tree
        int cost = 0;
        for (int i = 0; i < ROW; i++) {
            if (parent[i] != -1) {
                cost += adjMatrix[i][parent[i]];
            }
        }

        // print the cost of the minimum spanning tree
        cout << "\tThe Optimal Cost is: " << cost << endl;


        // print the minimum spanning tree
        cout << "\tThe Optimal path is:\n\t\t0";
        printMST(parent, dest);
        cout << endl;



        prims_Score.traverse(prims_Score.root);
        if (prims_Score.totalScore >= 0)
            cout << "\n\tTotal Score = " << GREEN_COLOR_CODE << prims_Score.totalScore << RESET_CODE << endl;
        else if (prims_Score.totalScore < 0)
            cout << "\n\tTotal Score = " << RED_COLOR_CODE << prims_Score.totalScore << RESET_CODE << endl;

        kScore = prims_Score.totalScore;
    }

    //--------------------------------------------------------------------------

    int par[MAX_NODES], rank[MAX_NODES];
    

    int find(int u) {
        if (par[u] != u) {
            par[u] = find(par[u]);
        }
        return par[u];
    }
    void union_sets(int u, int v) {
        u = find(u);
        v = find(v);
        if (u != v) {
            if (rank[u] < rank[v]) {
                swap(u, v);
            }
            par[v] = u;
            if (rank[u] == rank[v]) {
                rank[u]++;
            }
        }
    }
    void kruskal(int **adj, int num_nodes, int end_node) {
        // Create an array of edges
        kruskalEdge *edges = new kruskalEdge[MAX_NODES * MAX_NODES];
        int num_edges = 0;
        for (int i = 0; i < num_nodes; i++) {
            for (int j = i + 1; j < num_nodes; j++) {
                if (adj[i][j] != 0 && adj[i][j] != INF) {
                    edges[num_edges] = { i, j, adj[i][j] };
                    num_edges++;
                }
            }
        }

        // Sort the edges by increasing weight
        for (int i = 0; i < num_edges; i++) {
            for (int j = i + 1; j < num_edges; j++) {
                if (edges[i].weight > edges[j].weight) {
                    swap(edges[i], edges[j]);
                }
            }
        }

        // Initialize disjoint-set data structure
        for (int i = 0; i < num_nodes; i++) {
            par[i] = i;
            rank[i] = 0;
        }

        // Iterate over edges and add them to the MST if they connect different components
        int mst_cost = 0;
        for (int i = 0; i < num_edges; i++) {
            int u = edges[i].u;
            int v = edges[i].v;
            if (find(u) != find(v)) {
                union_sets(u, v);
                mst_cost += edges[i].weight;
                if (find(end_node) == find(0)) {
                    break;
                }
            }
        }
        AVLTree kruskal_Score;

        bool flag = false;
        int curr_node = end_node;
        while (curr_node != 0) {
            for (int i = 0; i < num_nodes; i++) {
                if (adj[curr_node][i] != 0 && adj[curr_node][i] != INF && find(curr_node) == find(i)) {
                    int _i = curr_node / 20;
                    int _j = curr_node % 20;
                    int score = 0;

                    if (matrix[_i][_j] == 'J' || matrix[_i][_j] == 'P' || matrix[_i][_j] == 'W' || matrix[_i][_j] == '&' || matrix[_i][_j] == '$' || matrix[_i][_j] == '@' || matrix[_i][_j] == '%' ) {
                        switch (matrix[_i][_j]) {
                        case 'J':
                            score = 50;
                            break;
                        case 'P':
                            score = 70;
                            break;
                        case 'W':
                            score = 30;
                            break;
                        case '&':
                            score = -50;
                            break;
                        case '$':
                            score = -70;
                            break;
                        case '@':
                            score = -30;
                            break;
                        case '%':
                            Beep(500, 500);
                            cout << RED_COLOR_CODE << "\t\tDeath point" << RESET_CODE << endl;
                            exit(EXIT_FAILURE);
                            break;


                        }
                    }
                    

                    if (matrix[_i][_j] == '*') {
                        flag = true;
                        break;
                    }
                    curr_node = i;
                    break;
                }
            }
            if (flag)
                break;
        }
     

        // Output the cost of the minimum spanning tree
        cout << "\tThe Optimal Cost is: " << mst_cost << endl;
        cout << "\tThe Optimal path is:\n\t\t0";
        printMST(parent, end_node);
        cout << endl;

        kruskal_Score.traverse(kruskal_Score.root);
        if (kScore >= 0)
            cout << "\n\tTotal Score = " << GREEN_COLOR_CODE << kScore << RESET_CODE << endl;
        else if (kScore < 0)
            cout << "\n\tTotal Score = " << RED_COLOR_CODE << kScore << RESET_CODE << endl;


    }



};


int main() {
    srand(time(0)); // Initialize random seed

    Graph grp;
    grp.matrix = grp.createGraph();

    int SIZE = 20;
    int index_i = 0;
    int index_j = 0;

    grp.adjMatrix = grp.createAdjacencyMatrix(grp.matrix, index_i, index_j);

    int* path = new int[400];

    //--------------------------------------------------------------------------
    cout << YELLOW_COLOR_CODE << "\n\tDijkstra's algorithm" << RESET_CODE << endl;
    int shortestCost = grp.dijkstra(grp.adjMatrix, 400, 0, index_i, path);
    AVLTree Dijkstra_Score;

    if (path[0] == -1) {
        cout << "\tThere is no path from index (0, 0)" << " to index (" << index_i / 20 << ", " << index_j % 20 << ")" << endl;
    }
    else {
        grp.display_dijkstra(Dijkstra_Score, grp.matrix, path, index_i, index_j, shortestCost);
    }


    //--------------------------------------------------------------------------
    int** dist = new int* [400];
    int** path1 = new int* [400];
    for (int i = 0; i < 400; i++) {
        dist[i] = new int[400];
        path1[i] = new int[400];
    }

    cout << YELLOW_COLOR_CODE << "\n\tFloyd's algorithm" << RESET_CODE << endl;
    grp.floyd(grp.adjMatrix, 400, 0, index_j, dist, path1);


    //--------------------------------------------------------------------------
    cout << YELLOW_COLOR_CODE << "\n\tPrim's algorithm" << RESET_CODE << endl;
    grp.primMST(0, index_j);

    
    //--------------------------------------------------------------------------
    cout << YELLOW_COLOR_CODE << "\n\tKruskal's algorithm" << RESET_CODE << endl;
    grp.kruskal(grp.adjMatrix, 400, index_j);


    return 0;
}