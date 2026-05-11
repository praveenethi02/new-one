#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <sstream>
using namespace std;

//  PATIENT STRUCTURE

struct Patient {
    int    id;
    string name;
    int    age;
    string disease;
    int    priority;   // 1 = Emergency, 2 = Urgent, 3 = Normal
    string timeAdded;

    Patient() : id(0), age(0), priority(3) {}
    Patient(int i, string n, int a, string d, int p, string t)
        : id(i), name(n), age(a), disease(d), priority(p), timeAdded(t) {}

    string priorityLabel() const {
        if (priority == 1) return "EMERGENCY";
        if (priority == 2) return "URGENT";
        return "NORMAL";
    }
};

// Helper to get current time as string
string currentTime() {
    time_t now = time(0);
    char buf[20];
    strftime(buf, sizeof(buf), "%H:%M:%S", localtime(&now));
    return string(buf);
}

//  1. LINKED LIST — Patient Registry
//     Stores all registered patients

struct LLNode {
    Patient data;
    LLNode* next;
    LLNode(Patient p) : data(p), next(nullptr) {}
};

class PatientLinkedList {
    LLNode* head;
    int count;
public:
    PatientLinkedList() : head(nullptr), count(0) {}

    void insert(Patient p) {
        LLNode* node = new LLNode(p);
        if (!head) { head = node; }
        else {
            LLNode* cur = head;
            while (cur->next) cur = cur->next;
            cur->next = node;
        }
        count++;
    }

    bool remove(int id) {
        if (!head) return false;
        if (head->data.id == id) {
            LLNode* tmp = head;
            head = head->next;
            delete tmp;
            count--;
            return true;
        }
        LLNode* cur = head;
        while (cur->next && cur->next->data.id != id)
            cur = cur->next;
        if (!cur->next) return false;
        LLNode* tmp = cur->next;
        cur->next = tmp->next;
        delete tmp;
        count--;
        return true;
    }

    Patient* search(int id) {
        LLNode* cur = head;
        while (cur) {
            if (cur->data.id == id) return &cur->data;
            cur = cur->next;
        }
        return nullptr;
    }

    void display() const {
        if (!head) {
            cout << "  [No patients registered]\n";
            return;
        }
        cout << "  " << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(5)  << "Age"
             << setw(18) << "Disease"
             << setw(12) << "Priority"
             << setw(10) << "Time"
             << "\n";
        cout << "  " << string(71, '-') << "\n";
        LLNode* cur = head;
        while (cur) {
            cout << "  "
                 << setw(6)  << cur->data.id
                 << setw(20) << cur->data.name
                 << setw(5)  << cur->data.age
                 << setw(18) << cur->data.disease
                 << setw(12) << cur->data.priorityLabel()
                 << setw(10) << cur->data.timeAdded
                 << "\n";
            cur = cur->next;
        }
    }

    int size() const { return count; }

    ~PatientLinkedList() {
        while (head) {
            LLNode* tmp = head;
            head = head->next;
            delete tmp;
        }
    }
};

// ─────────────────────────────────────────────
//  2. PRIORITY QUEUE — Patient Waiting Line
//     Uses a simple linked-list-backed priority queue
//     Lower priority number = served first
// ─────────────────────────────────────────────
struct QNode {
    Patient data;
    QNode* next;
    QNode(Patient p) : data(p), next(nullptr) {}
};

class PriorityQueue {
    QNode* front;
    int count;
public:
    PriorityQueue() : front(nullptr), count(0) {}

    void enqueue(Patient p) {
        QNode* node = new QNode(p);
        // Insert by priority (1 first), then FIFO within same priority
        if (!front || p.priority < front->data.priority) {
            node->next = front;
            front = node;
        } else {
            QNode* cur = front;
            while (cur->next && cur->next->data.priority <= p.priority)
                cur = cur->next;
            node->next = cur->next;
            cur->next = node;
        }
        count++;
    }

    Patient dequeue() {
        if (!front) throw runtime_error("Queue is empty!");
        QNode* tmp = front;
        Patient p = tmp->data;
        front = front->next;
        delete tmp;
        count--;
        return p;
    }

    Patient peek() const {
        if (!front) throw runtime_error("Queue is empty!");
        return front->data;
    }

    bool isEmpty() const { return front == nullptr; }
    int  size()    const { return count; }

    void display() const {
        if (!front) {
            cout << "  [Queue is empty]\n";
            return;
        }
        cout << "  Position | " << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(12) << "Priority"
             << setw(10) << "Time"
             << "\n";
        cout << "  " << string(58, '-') << "\n";
        QNode* cur = front;
        int pos = 1;
        while (cur) {
            cout << "    " << setw(6) << pos++
                 << "   " << setw(6)  << cur->data.id
                          << setw(20) << cur->data.name
                          << setw(12) << cur->data.priorityLabel()
                          << setw(10) << cur->data.timeAdded
                 << "\n";
            cur = cur->next;
        }
    }

    ~PriorityQueue() {
        while (front) {
            QNode* tmp = front;
            front = front->next;
            delete tmp;
        }
    }
};

// ─────────────────────────────────────────────
//  3. STACK — Treatment History / Undo
//     Tracks recently served patients (LIFO)
// ─────────────────────────────────────────────
struct SNode {
    Patient data;
    SNode* next;
    SNode(Patient p) : data(p), next(nullptr) {}
};

class TreatmentStack {
    SNode* top;
    int count;
public:
    TreatmentStack() : top(nullptr), count(0) {}

    void push(Patient p) {
        SNode* node = new SNode(p);
        node->next = top;
        top = node;
        count++;
    }

    Patient pop() {
        if (!top) throw runtime_error("Stack is empty!");
        SNode* tmp = top;
        Patient p = tmp->data;
        top = top->next;
        delete tmp;
        count--;
        return p;
    }

    Patient peek() const {
        if (!top) throw runtime_error("Stack is empty!");
        return top->data;
    }

    bool isEmpty() const { return top == nullptr; }
    int  size()    const { return count; }

    void display() const {
        if (!top) {
            cout << "  [No treatment history]\n";
            return;
        }
        cout << "  (Most recent first)\n";
        cout << "  " << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(18) << "Disease"
             << setw(12) << "Priority"
             << "\n";
        cout << "  " << string(56, '-') << "\n";
        SNode* cur = top;
        while (cur) {
            cout << "  "
                 << setw(6)  << cur->data.id
                 << setw(20) << cur->data.name
                 << setw(18) << cur->data.disease
                 << setw(12) << cur->data.priorityLabel()
                 << "\n";
            cur = cur->next;
        }
    }

    ~TreatmentStack() {
        while (top) {
            SNode* tmp = top;
            top = top->next;
            delete tmp;
        }
    }
};

// ─────────────────────────────────────────────
//  4. BST — Patient Search Tree by ID
//     Fast lookup, insertion, in-order display
// ─────────────────────────────────────────────
struct BSTNode {
    Patient data;
    BSTNode* left;
    BSTNode* right;
    BSTNode(Patient p) : data(p), left(nullptr), right(nullptr) {}
};

class PatientBST {
    BSTNode* root;

    BSTNode* insert(BSTNode* node, Patient p) {
        if (!node) return new BSTNode(p);
        if (p.id < node->data.id)
            node->left  = insert(node->left, p);
        else if (p.id > node->data.id)
            node->right = insert(node->right, p);
        return node;
    }

    BSTNode* search(BSTNode* node, int id) {
        if (!node || node->data.id == id) return node;
        if (id < node->data.id) return search(node->left, id);
        return search(node->right, id);
    }

    BSTNode* findMin(BSTNode* node) {
        while (node->left) node = node->left;
        return node;
    }

    BSTNode* remove(BSTNode* node, int id) {
        if (!node) return nullptr;
        if (id < node->data.id)
            node->left  = remove(node->left, id);
        else if (id > node->data.id)
            node->right = remove(node->right, id);
        else {
            if (!node->left) {
                BSTNode* tmp = node->right;
                delete node;
                return tmp;
            } else if (!node->right) {
                BSTNode* tmp = node->left;
                delete node;
                return tmp;
            }
            BSTNode* mn = findMin(node->right);
            node->data = mn->data;
            node->right = remove(node->right, mn->data.id);
        }
        return node;
    }

    void inorder(BSTNode* node) const {
        if (!node) return;
        inorder(node->left);
        cout << "  "
             << setw(6)  << node->data.id
             << setw(20) << node->data.name
             << setw(5)  << node->data.age
             << setw(18) << node->data.disease
             << setw(12) << node->data.priorityLabel()
             << "\n";
        inorder(node->right);
    }

    void destroy(BSTNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    PatientBST() : root(nullptr) {}

    void insert(Patient p)  { root = insert(root, p); }
    void remove(int id)     { root = remove(root, id); }

    Patient* search(int id) {
        BSTNode* node = search(root, id);
        return node ? &node->data : nullptr;
    }

    void display() const {
        if (!root) {
            cout << "  [BST is empty]\n";
            return;
        }
        cout << "  (Sorted by Patient ID — In-Order Traversal)\n";
        cout << "  " << left
             << setw(6)  << "ID"
             << setw(20) << "Name"
             << setw(5)  << "Age"
             << setw(18) << "Disease"
             << setw(12) << "Priority"
             << "\n";
        cout << "  " << string(61, '-') << "\n";
        inorder(root);
    }

    ~PatientBST() { destroy(root); }
};

// ─────────────────────────────────────────────
//  DISPLAY HELPERS
// ─────────────────────────────────────────────
void printHeader(const string& title) {
    cout << "\n  \n";
    cout << "   " << left << setw(42) << title << "\n";
    cout << " \n";
}

void printSeparator() {
    cout << "\n  " << string(60, '=') << "\n";
}

void printMenu() {
    cout << "\n";
    cout << "  \n";
    cout << "         HOSPITAL PATIENT MANAGEMENT SYSTEM         \n";
    cout << "  \n";
    cout << "    [1]  Register New Patient                       \n";
    cout << "    [2]  Add Patient to Waiting Queue               \n";
    cout << "    [3]  Serve Next Patient (Dequeue)               \n";
    cout << "    [4]  View Patient Queue                         \n";
    cout << "    [5]  View All Registered Patients (Linked List) \n";
    cout << "    [6]  Search Patient by ID (BST)                 \n";
    cout << "    [7]  View BST (Sorted by ID)                    \n";
    cout << "    [8]  View Treatment History (Stack)             \n";
    cout << "    [9]  Undo Last Treatment (Stack Pop)            \n";
    cout << "    [10] Remove Patient from Registry               \n";
    cout << "    [0]  Exit                                       \n";
    cout << "  \n";
    cout << "  Enter choice: ";
}

// ─────────────────────────────────────────────
//  MAIN PROGRAM
// ─────────────────────────────────────────────
int main() {
    PatientLinkedList registry;
    PriorityQueue     waitingQueue;
    TreatmentStack    treatmentHistory;
    PatientBST        bst;

    int nextId = 1001;

    // ── Pre-load sample data ──────────────────
    auto addSample = [&](string name, int age, string disease, int pri) {
        Patient p(nextId++, name, age, disease, pri, currentTime());
        registry.insert(p);
        bst.insert(p);
    };

    addSample("Amal Perera",    45, "Chest Pain",    1);
    addSample("Nimal Silva",    30, "Flu",            3);
    addSample("Kamala Bandara", 62, "Diabetes",       2);
    addSample("Sunil Fernando", 25, "Fracture",       2);
    addSample("Priya Mendis",   19, "Headache",       3);

    // ── Main loop ────────────────────────────
    int choice;
    do {
        printMenu();
        cin >> choice;
        cin.ignore();

        if (choice == 1) {
            // ── Register New Patient ─────────
            printHeader("REGISTER NEW PATIENT");
            string name, disease;
            int age, priority;

            cout << "\n  Name    : "; getline(cin, name);
            cout << "  Age     : "; cin >> age; cin.ignore();
            cout << "  Disease : "; getline(cin, disease);
            cout << "  Priority (1=Emergency, 2=Urgent, 3=Normal): ";
            cin >> priority; cin.ignore();

            if (priority < 1 || priority > 3) {
                cout << "\n  [!] Invalid priority. Defaulting to Normal.\n";
                priority = 3;
            }

            Patient p(nextId++, name, age, disease, priority, currentTime());
            registry.insert(p);
            bst.insert(p);

            cout << "\n  ✔ Patient registered successfully!\n";
            cout << "  Assigned ID: " << p.id << " | Priority: " << p.priorityLabel() << "\n";

        } else if (choice == 2) {
            // ── Enqueue Patient ──────────────
            printHeader("ADD PATIENT TO QUEUE");
            int id;
            cout << "\n  Enter Patient ID: ";
            cin >> id; cin.ignore();

            Patient* p = registry.search(id);
            if (!p) {
                cout << "\n  [!] Patient ID " << id << " not found in registry.\n";
            } else {
                waitingQueue.enqueue(*p);
                cout << "\n  ✔ " << p->name << " added to waiting queue.\n";
                cout << "  Queue position depends on priority (" << p->priorityLabel() << ").\n";
            }

        } else if (choice == 3) {
            // ── Serve Next Patient ───────────
            printHeader("SERVE NEXT PATIENT");
            if (waitingQueue.isEmpty()) {
                cout << "\n  [!] No patients in the queue.\n";
            } else {
                Patient served = waitingQueue.dequeue();
                treatmentHistory.push(served);
                cout << "\n  ✔ Now serving:\n";
                cout << "    ID      : " << served.id       << "\n";
                cout << "    Name    : " << served.name     << "\n";
                cout << "    Age     : " << served.age      << "\n";
                cout << "    Disease : " << served.disease  << "\n";
                cout << "    Priority: " << served.priorityLabel() << "\n";
                cout << "\n  Patient moved to Treatment History.\n";
            }

        } else if (choice == 4) {
            // ── View Queue ───────────────────
            printHeader("PATIENT WAITING QUEUE");
            cout << "\n  Total in queue: " << waitingQueue.size() << "\n\n";
            waitingQueue.display();

        } else if (choice == 5) {
            // ── View Registry ────────────────
            printHeader("ALL REGISTERED PATIENTS (Linked List)");
            cout << "\n  Total registered: " << registry.size() << "\n\n";
            registry.display();

        } else if (choice == 6) {
            // ── BST Search ───────────────────
            printHeader("SEARCH PATIENT BY ID (BST)");
            int id;
            cout << "\n  Enter Patient ID: ";
            cin >> id; cin.ignore();

            Patient* p = bst.search(id);
            if (!p) {
                cout << "\n  [!] Patient with ID " << id << " not found.\n";
            } else {
                cout << "\n  ✔ Patient Found:\n";
                cout << "    ID      : " << p->id       << "\n";
                cout << "    Name    : " << p->name     << "\n";
                cout << "    Age     : " << p->age      << "\n";
                cout << "    Disease : " << p->disease  << "\n";
                cout << "    Priority: " << p->priorityLabel() << "\n";
                cout << "    Added   : " << p->timeAdded << "\n";
            }

        } else if (choice == 7) {
            // ── BST Display ──────────────────
            printHeader("BST — PATIENTS SORTED BY ID");
            cout << "\n";
            bst.display();

        } else if (choice == 8) {
            // ── Treatment History ────────────
            printHeader("TREATMENT HISTORY (Stack)");
            cout << "\n  Total treated: " << treatmentHistory.size() << "\n\n";
            treatmentHistory.display();

        } else if (choice == 9) {
            // ── Undo Treatment (Stack Pop) ───
            printHeader("UNDO LAST TREATMENT");
            if (treatmentHistory.isEmpty()) {
                cout << "\n  [!] No treatment history to undo.\n";
            } else {
                Patient undone = treatmentHistory.pop();
                waitingQueue.enqueue(undone);
                cout << "\n  ✔ Treatment of " << undone.name << " (ID: " << undone.id << ") undone.\n";
                cout << "  Patient has been re-added to the waiting queue.\n";
            }

        } else if (choice == 10) {
            // ── Remove Patient ───────────────
            printHeader("REMOVE PATIENT FROM REGISTRY");
            int id;
            cout << "\n  Enter Patient ID to remove: ";
            cin >> id; cin.ignore();

            bool removed = registry.remove(id);
            if (removed) {
                bst.remove(id);
                cout << "\n  ✔ Patient ID " << id << " removed from registry and BST.\n";
            } else {
                cout << "\n  [!] Patient ID " << id << " not found.\n";
            }

        } else if (choice != 0) {
            cout << "\n  [!] Invalid option. Please try again.\n";
        }

        if (choice != 0) {
            cout << "\n  Press Enter to continue...";
            cin.get();
        }

    } while (choice != 0);

    cout << "\n   Thank you for using Hospital Management System. Goodbye!\n\n";
    return 0;
}