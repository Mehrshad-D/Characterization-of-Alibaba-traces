#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <queue>
#include <stack>
#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <fstream> 
#include <sstream>
using namespace std;

struct Node {
    long long key;
    long long memoryAccess;
    Node* left;
    Node* right;
    Node* parent;
    long long height;
    long long size;

    Node(long long k, long long m) : key(k), memoryAccess(m), left(nullptr), right(nullptr), parent(nullptr), height(1), size(1) {}
};

class AVLTree {
private:
    Node* root;
    unordered_map<long long, long long> memory_access_map;

    long long height(Node* n) {
        return n ? n->height : 0;
    }

    long long size(Node* n) {
        return n ? n->size : 0;
    }

    long long balanceFactor(Node* n) {
        return n ? height(n->left) - height(n->right) : 0;
    }

    void updateNode(Node* n) {
        if (n) {
            n->height = max(height(n->left), height(n->right)) + 1;
            n->size = size(n->left) + size(n->right) + 1;
        }
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        if (T2) T2->parent = y;
        x->parent = y->parent;
        y->parent = x;

        updateNode(y);
        updateNode(x);

        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        if (T2) T2->parent = x;
        y->parent = x->parent;
        x->parent = y;

        updateNode(x);
        updateNode(y);

        return y;
    }

    Node* balance(Node* n) {
        updateNode(n);

        if (balanceFactor(n) > 1) {
            if (balanceFactor(n->left) < 0)
                n->left = rotateLeft(n->left);
            return rotateRight(n);
        }
        if (balanceFactor(n) < -1) {
            if (balanceFactor(n->right) > 0)
                n->right = rotateRight(n->right);
            return rotateLeft(n);
        }

        return n;
    }

    Node* insert(Node* n, long long key, long long memoryAccess) {
        if (!n) return new Node(key, memoryAccess);

        if (key < n->key) {
            n->left = insert(n->left, key, memoryAccess);
            n->left->parent = n;
        } else if (key > n->key) {
            n->right = insert(n->right, key, memoryAccess);
            n->right->parent = n;
        } else {
            return n;
        }

        return balance(n);
    }

    Node* minValueNode(Node* n) {
        Node* current = n;
        while (current->left)
            current = current->left;
        return current;
    }

    Node* deleteNode(Node* root, long long key) {
        if (!root) return root;

        if (key < root->key) {
            root->left = deleteNode(root->left, key);
            if (root->left) root->left->parent = root;
        }
        else if (key > root->key) {
            root->right = deleteNode(root->right, key);
            if (root->right) root->right->parent = root;
        }
        else {
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;

                if (!temp) {
                    temp = root;
                    root = nullptr;
                } else {
                    *root = *temp;
                }

                delete temp;
                if (root) {
                    root->parent = nullptr; 
                }
            } else {
                Node* temp = minValueNode(root->right);
                root->key = temp->key;
                root->memoryAccess = temp->memoryAccess;
                root->right = deleteNode(root->right, temp->key);
                if (root->right) root->right->parent = root;
            }
        }

        if (!root) return root;

        return balance(root);
    }

    Node* search(Node* root, long long key) {
        if (!root || root->key == key)
            return root;

        if (root->key < key)
            return search(root->right, key);

        return search(root->left, key);
    }

    void printNode(Node* node, long long indent = 0) {
        if (node != nullptr) {
            if (node->right) {
                printNode(node->right, indent + 4);
            }
            if (indent) {
                cout << setw(indent) << ' ';
            }
            if (node->right) cout << " /\n" << setw(indent) << ' ';
            cout << node->key << "\n ";
            if (node->left) {
                cout << setw(indent) << ' ' << " \\\n";
                printNode(node->left, indent + 4);
            }
        }
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(long long key, long long memoryAccess) {
        root = insert(root, key, memoryAccess);
        memory_access_map[memoryAccess] = key;
    }

    void deleteNode(long long key) {
        Node* nodeToDelete = search(root, key);
        if (nodeToDelete) {
            memory_access_map.erase(nodeToDelete->memoryAccess);  
            root = deleteNode(root, key);
        }
    }

    long long searchByMemoryAccess(long long memoryAccess) {
        return memory_access_map.count(memoryAccess) ? memory_access_map[memoryAccess] : -1;
    }

    void inorderTraversal(Node* root) {
        if (!root) return;
        inorderTraversal(root->left);
        cout << root->key << " ";
        inorderTraversal(root->right);
    }

    void printInorder() {
        inorderTraversal(root);
        cout << endl;
    }

    long long reuseDistance(long long nodeKey) {
        Node* node = search(root, nodeKey);
        if (!node) return 0; 

        long long result = 0;
        if (node->left != nullptr) {
            result += (node->left)->size;
        }
        while (node->parent != nullptr) {
            if((node->parent)->right != nullptr){
                if ((node->parent)->right == node) {
                    result += (node->parent)->size - node->size;
                }
            }
            node = node->parent;
        }
        return result;
    }
    
    // void printMemoryAccessMap() {
    //     for (const auto& entry : memory_access_map) {
    //         cout << "Memory Access: " << entry.first << ", Key: " << entry.second << endl;
    //     }
    // }

    void drawTree() {
        printNode(root);
    }
};


vector<long long> readMemoryAccessesFromCSV(const string& filename) {
    vector<long long> memoryAccesses;
    ifstream file(filename);
    string line;

    if (!file.is_open()) {
        cerr << "Unable to open file " << filename << endl;
        return memoryAccesses;
    }

    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        long long column = 0;
        while (getline(ss, token, ',')) {
            if (column == 2) { 
                memoryAccesses.push_back(stoll(token));
                break;
            }
            ++column;
        }
    }

    file.close();
    return memoryAccesses;
}


int main() {
    AVLTree avl;

    vector<long long> memoryAccesses = readMemoryAccessesFromCSV("A42.csv");
    long long n = memoryAccesses.size();

    long long out = 0;
    long long counter = 0;
    long long maximum_distance = -1;
    long long minimum_distance = LLONG_MAX;
    long long current_distance;

    long long nodeKey;
    for (long long i = 0; i < n; i++) {
        nodeKey = avl.searchByMemoryAccess(memoryAccesses[i]);
        if (nodeKey == -1) {
            avl.insert(n - i, memoryAccesses[i]);
        } else {
            counter++;
            current_distance = avl.reuseDistance(nodeKey);
            out += current_distance;
            if(current_distance < minimum_distance)
                minimum_distance = current_distance;
            if(current_distance > maximum_distance)
                maximum_distance = current_distance;
            avl.deleteNode(nodeKey);
            avl.insert(n - i, memoryAccesses[i]);
        }
    }

    double average = (out * 1.0) / counter;

    cout << "count: " << counter << endl;
    cout << "Sum: " << out << endl;
    cout << "average: " << average << endl;
    cout << "maximum: " << maximum_distance << endl;
    cout << "minimum: " << minimum_distance << endl;
 

    return 0;
}
