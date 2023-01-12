/* lists.cpp – A number of functions to do things with linked lists, built based on several LeetCode challenges. */

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>

/* Overarching things */

// Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next){}
};

// Helper functions
int hasCycle(ListNode *head);

// Return the index of the node cycled back to – breaks if the list doesn't have a cycle
int cycleStart(ListNode *head){
    // Find the node in question
    ListNode *cycledTo = head;
    for (int i=0; i<=hasCycle(head); i++) cycledTo = cycledTo -> next;

    // Then work out its index
    int start = 0;
    ListNode *scanner = head;
    for (; scanner!=cycledTo; start++) scanner = scanner -> next;

    return start;
}

// Must be paired with deleteList()
ListNode *vecToList(std::vector<int> vals){
    ListNode *prev = nullptr;
    ListNode *curr;

    for (int i=vals.size()-1; i>=0; i--){
        curr = new ListNode(vals[i]);
        if (prev) curr -> next = prev;
        prev = curr;
    }

    return curr;
}

void printList(ListNode *head){
    if (!head){
        std::cout << "List empty.\n";
    } else{
        int p = hasCycle(head);

        if (p){
            /* Print the list with an extra arrow to indicate the loop back */
            // Print the linear part
            int q = cycleStart(head);
            ListNode *scanner = head;
            int cursorPos = 0;
            int pPos, qPos;

            std::cout << "\n";                                              // In case a previous function left us on a line
            for (int i=0; i<=p; i++){
                // Take note of where the arrow should start and end
                if (i == p) pPos = cursorPos;
                if (i == q) qPos = cursorPos;
                
                std::cout << scanner -> val;
                cursorPos += int(log10(scanner->val) + 1);
                if (i < p){
                    std::cout << " -> ";
                    cursorPos += 4;
                }

                scanner = scanner -> next;
            }
            std::cout << "\n";

            // Then print the arrow
            int i;
            for (i=0; i<pPos; i++){
                std::cout << ((i==qPos) ? "^" : " ");
            }
            std::cout << "|\n";

            for (i=0; i<pPos; i++){
                std::cout << ((i==qPos) ? "|" : " ");
            }
            std::cout << "|\n";

            for (i=0; i<=qPos; i++) std::cout << " ";
            for (; i<pPos; i++) std::cout << "-";
            std::cout << "\n";
        } else{
            // Else print the list without the frills
            ListNode *scanner = head;
            while (scanner){
                std::cout << scanner->val;
                std::cout << ((scanner->next) ? " -> " : "\n");
                scanner = scanner -> next;
                if (!scanner) break;
            }
        }
    }
}

void deleteList(ListNode *head){
    int p = hasCycle(head);
    if (p){
        // Deleting a linked list with cycles is a little more involved, but we can use the results of hasCycle()
        for (int i=0; i<p; i++){
            ListNode *temp = head -> next;
            delete head;
            head = temp;
        }

        delete head;
    } else{
        // The simpler version, if there are no cycles
        while (head){
            ListNode *temp = head -> next;
            delete head;
            if (!temp) break;
            head = temp;
        }
    }
}

/* Now the user functionality */

// Get an integer from the user, re-prompting until succesful
int getInt(std::string prompt){
    int num;
    do{
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << prompt;
        std::cin >> num;
    } while (std::cin.fail());

    return num;
}

// Use getInt() to get an integer from 1 to 10 from the user, returning it zero-indexed
int getListNum(std::string prompt = "Which # list? (1 - 10) "){
    int num;
    do{
        num = getInt(prompt);
    } while (num<1 || num>10);

    return num - 1;
}

// Get a singly linked list from the user (also to be paired with deleteList())
ListNode *getList(){
    // Prompt for values
    std::string input;
    std::cout << "Enter comma-separated values: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::getline(std::cin, input);

    // Parse string
    std::stringstream ss(input);
    std::string valStr;
    std::vector<int> vals;

    while (std::getline(ss, valStr, ',')){
        // Skip the whitespace each time, then let the loop take back control
        while (ss.get() == ' ');
        ss.seekg(-1, ss.cur);
        
        // To integer
        try{
            vals.push_back(stoi(valStr));
        } catch(...){
            std::cout << valStr << " isn't an integer. Starting again:\n\n";
            return getList();
        }
    }

    return vecToList(vals);
}

// Sort a list in ascending order
ListNode *sortList(ListNode *head){
    ListNode *result;
    ListNode *rScanner;

    ListNode *scanner;
    int min;
    int pos;                                                            // Zero-indexed

    // Look for the lowest value >= min and slip it out of the given list and into the returned list each time
    while (head){
        min = INT_MAX;

        // First find the node with the (shared) lowest value
        scanner = head;
        int p = 0;

        do{
            if (scanner->val <= min){
                min = scanner -> val;
                pos = p;
            }

            scanner = scanner -> next;
            p++;
        } while (scanner);

        // Then remove that node (the first node with its value) from the given list, by scanning to the previous node
        if (head->val == min){
            head = head ->next;
        } else{
            scanner = head;
            while (scanner->next->val != min) scanner = scanner -> next;
            ListNode *temp = (scanner->next) -> next;
            delete scanner -> next;
            scanner -> next = temp;
        }

        // And add an equal node to the new list
        if (!result){
            result = new ListNode(min);
            rScanner = result;
        } else{
            rScanner -> next = new ListNode(min);
            rScanner = rScanner -> next;
        }
    }

    return result;
}

// Concatenate two lists
ListNode *concatLists(ListNode *p, ListNode *q){
    ListNode *scanner = p;
    while (scanner -> next) scanner = scanner -> next;
    scanner -> next = q;

    return p;
}

// Check whether a list has a cycle in it, returning the zero-indexed position of the node which
// points to a previous node, or zero if no cycle
int hasCycle(ListNode *head) {
    std::vector<ListNode *> visited;
    int p = 0;
    
    while (head){
        if (std::find(visited.begin(), visited.end(), head) != visited.end()) return p - 1;
        visited.push_back(head);

        head = head -> next;
        p++;
    }

    return 0;
}

// Create a cycle by looping the last node of a list back to the first
void loopList(ListNode *head){
    ListNode *scanner = head;
    while (scanner->next) scanner = scanner -> next;
    scanner -> next = head;
}

// Remove a list's nth from last node
ListNode *removeNthFromEnd(ListNode *head, int n) {
    // Find length of list
    int length = 0;
    ListNode *node = head;

    while (node){
        node = node -> next;
        length++;
    }
    if (length <= n) return head -> next;

    // Hence go to n-1th node from end
    node = head; 
    for (int i=0; i<length-n-1; i++){
        node = node -> next;
    }

    // And slip
    ListNode *old = node -> next;
    node -> next = old -> next;
    delete old;

    return head;
}

// Swap every two adjacent pairs
ListNode *swapPairs(ListNode *head) {
    // Create a scanner node which always has p and q as its two next, which means starting
    // before head
    ListNode s = ListNode(0, head);
    ListNode *scanner = &s;
    bool first = true;

    // Go provided there are two more nodes to swap
    while (scanner->next && (scanner->next)->next){
        // Swap
        ListNode *tempp = scanner -> next;
        scanner -> next = tempp -> next;
        ListNode *tempq = (tempp->next) -> next;
        (scanner->next) -> next = tempp;
        tempp -> next = tempq;

        if (first){
            head = scanner -> next;
            first = false;
        }

        // Step
        scanner = (scanner->next) -> next;
    }

    return head;
}

// Create a replica of list p; doing this is easier when there is no cycle, but we'll
// merge the two cases into one abstraction
ListNode *copyList(ListNode *p){
    if (!p) return nullptr;

    int cycle = hasCycle(p);
    ListNode *q;

    if (cycle){
        // Copy the linear part of the list
        q = new ListNode(p -> val);
        ListNode *scanner = q;
        p = p -> next;

        for (int i=1; i<=cycle; i++){
            scanner -> next = new ListNode(p -> val);
            std::cout << p->val << "\n";
            scanner = scanner -> next;
            p = p -> next;
        }

        // And close the loop
        ListNode *loopedTo = q;
        int cStart = cycleStart(p);

        for (int i=0; i<=cStart; i++) loopedTo = loopedTo -> next;
        std::cout << loopedTo->val << "\n";
        scanner -> next = loopedTo;
    } else{
        q = new ListNode(p -> val);
        ListNode *scanner = q;
        p = p -> next;

        while (p){
            scanner -> next = new ListNode(p -> val);
            scanner = scanner -> next;
            p = p -> next;
        }
    }

    return q;
}

/* The interface. All of the commands the user might call are given their own function so we can map them */
ListNode *heads[10];

void setFunc(){
    // Report the current list in the specified slot
    int l = getListNum();
    if (!heads[l]){
        std::cout << "(List #" << l+1 << " is currently empty.)\n";
    } else{
        std::cout << "List #" << l+1 << " is currently: ";
        printList(heads[l]);
    }

    heads[l] = getList();
    std::cout << "List updated.\n";
}

void printFunc(){
    printList(heads[getListNum()]);
}

void sortFunc(){
    int l = getListNum();
    heads[l] = sortList(heads[l]);
    std::cout << "List sorted. Updated list: ";
    printList(heads[l]);
}

void cycleFunc(){
    int l = getListNum();
    int p = hasCycle(heads[l]);

    if (!heads[l]){
        std::cout << "No cycle (list empty).\n";
    } else if (p){
        std::cout << "List #" << l+1 << " has a cycle, at node " << p << ".\n";
    } else{
        std::cout << "List does not have a cycle.\n";
    }
}

void loopFunc(){
    int l = getListNum();
    loopList(heads[l]);
    std::cout << "List looped. New list: ";
    printList(heads[l]);
}

void catFunc(){
    int l = getListNum();
    int m = getListNum("Which other # list? (1 - 10) ");

    // Avert disaster
    if (!heads[l]){
        std::cout << "List #" << l+1 << " is empty.\n";
    } else if (!heads[m]){
        std::cout << "List #" << m+1 << " is empty.\n";
    } else if (hasCycle(heads[l])){
        std::cout << "Cannot concatenate to a list with a cycle.\n";
    } else{
        // Proceed
        heads[l] = concatLists(heads[l], heads[m]);
        std::cout << "New list #" << l+1 << ": ";
        printList(heads[l]);

        // So that we don't have multiple pointers to the same objects, confusing deleteList(), duplicate list m
        int p = hasCycle(heads[m]);

        if (p){
            //heads[m] = nullptr;
        } else{
            ListNode *newMHead = new ListNode(heads[m] -> val);
            ListNode *newMScanner = newMHead;
            while (heads[m] -> next){
                newMScanner -> next = new ListNode((heads[m]->next) -> val);
                heads[m] = heads[m] -> next;
                newMScanner = newMScanner -> next;
            }

            heads[m] = newMHead;
        }
    }
}

void nthFunc(){
    int l = getListNum();
    int n = getInt("How many places from end? ");
    removeNthFromEnd(heads[l], n);

    std::cout << "Removed. New list: ";
    printList(heads[l]);
}

void swapFunc(){
    int l = getListNum();
    heads[l] = swapPairs(heads[l]);
    std::cout << "Pairs swapped. New list: ";
    printList(heads[l]);
}

void copyFunc(){
    int l = getListNum("Copy which # list? (1 - 10) ");
    int m = getListNum("Copy to which other # list? (1 - 10) ");

    if (heads[m]){
        deleteList(heads[m]);
        std::cout << "Clearing list #" << m+1 << "...\n";
    }

    heads[m] = copyList(heads[l]);
    std::cout << "Copied. List #" << m+1 << " is now: ";
    printList(heads[m]);
}

int main(int argc, char *argv[]){
    std::cout << "Welcome to the LeetCode Linked List Manipulator. Enter a command like 'set', 'print', or 'sort'.\n";
    std::cout << "Try \"commands\" for a list of commands.\n\n";

    // Give the user ten linked list-slots to work with
    for (int l=0; l<10; l++)
        heads[l] = nullptr;

    // Create a map of commands to functions
    std::unordered_map<std::string, void (*)()> commands;
    commands.emplace("set", &setFunc);
    commands.emplace("print", &printFunc);
    commands.emplace("sort", &sortFunc);
    commands.emplace("cycle", &cycleFunc);
    commands.emplace("loop", &loopFunc);
    commands.emplace("cat", &catFunc);
    commands.emplace("nthfromend", &nthFunc);
    commands.emplace("swap", &swapFunc);
    commands.emplace("copy", &copyFunc);

    // Let the user do things
    std::string input;
    do{
        std::cout << "> ";
        std::cin >> input;

        if (commands.find(input) != commands.end()){
            commands.at(input)();
        } else if (input == "commands"){
            // List all commands in the map
            std::cout << "Available commands:\n\n";
            for (std::unordered_map<std::string, void (*)()>::iterator it = commands.begin();
            it != commands.end();
            ++it)
                std::cout << it->first << "\n";
        } else if (input != "exit"){
            std::cout << "Command not recognised.\n";
        }
    } while (input != "exit");

    for (int l=0; l<10; l++)
        if(heads[l]) deleteList(heads[l]);

    return 0;
}