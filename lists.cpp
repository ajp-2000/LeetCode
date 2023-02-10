/* lists.cpp – A number of functions to do things with linked lists, built based on several LeetCode challenges. */

#include <iostream>
#include <cmath>
#include <iterator>
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
int cycleStart(ListNode *head);
int listLen(ListNode *head);

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
        std::cout << "List empty." << std::endl;
    } else{
        int p = hasCycle(head);

        if (p){
            /* Print the list with an extra arrow to indicate the loop back */
            if (p == -1){
                // This is a single node linking to itself, which needs to be treated differently
                std::cout << "\n" << head->val << "--\n";

                std::cout << "^";
                for (int i=0; i<=((head->val==0) ? 1 : int(log10(head->val) + 1)); i++) std::cout << " ";
                std::cout << "|\n";

                for (int i=0; i<=((head->val==0) ? 1 : int(log10(head->val) + 1)); i++) std::cout << "-";
                std::cout << "\n";
            } else{
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
                    cursorPos += ((scanner->val==0) ? 1 : int(log10(scanner->val) + 1));
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
                std::cout << std::endl;
            }
        } else{
            // Else print the list without the frills
            ListNode *scanner = head;
            while (scanner){
                std::cout << scanner->val;
                if (scanner -> next) std::cout << " -> ";
                scanner = scanner -> next;
                if (!scanner) break;
            }

            std::cout << std::endl;
        }
    }
}

void deleteList(ListNode *head){
    int p = hasCycle(head);

    // Account for either cycles or not
    int i = 0;
    while (head && (i<=p || p==0)){
        ListNode *temp = head -> next;
        delete head;
        if (!temp) break;
        head = temp;
        i++;
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

// Find the length of a list
int listLen(ListNode *head){
    if (hasCycle(head)) return -1;

    int len = 0;
    while (head){
        len++;
        head = head -> next;
    }

    return len;
}

// Sort a list in ascending order
ListNode *sortList(ListNode *head){
    ListNode *result = nullptr;
    ListNode *rScanner = nullptr;

    ListNode *scanner = nullptr;
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
            head = head -> next;
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

// Check if a list is sorted in an ascending order
bool isSorted(ListNode *head){
    int prev = INT_MIN;

    while (head){
        if (head->val < prev) return false;
        prev = head -> val;
        head = head -> next;
    }

    return true;
}

// Check whether a list has a cycle in it, returning the zero-indexed position of the node which
// points to a previous node, or zero if no cycle
int hasCycle(ListNode *head) {
    std::vector<ListNode *> visited;
    int p = 0;
    
    while (head){
        if (std::find(visited.begin(), visited.end(), head) != visited.end()){
            // Use -1 as code for a loop starting at 0 so that return value 0 can still represent false
            if (p > 1) return p - 1;

            return -1;
        }
        visited.push_back(head);

        head = head -> next;
        p++;
    }

    return 0;
}

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

    q = new ListNode(p -> val);
    ListNode *scanner = q;
    ListNode *pScanner = p;
    pScanner = pScanner -> next;

    int i = 1;
    while ((cycle && i<=cycle) || (!cycle && pScanner)){
        scanner -> next = new ListNode(pScanner -> val);
        scanner = scanner -> next;
        pScanner = pScanner -> next;
        i++;
    }

    // Close the loop (if required)
    if (cycle){
        ListNode *loopedTo = q;
        int cStart = cycleStart(p);

        for (i=0; i<cStart; i++) loopedTo = loopedTo -> next;
        scanner -> next = loopedTo;
    }

    return q;
}

// Concatenate two lists, returning the head of p with q appended; create a duplicate to copy
// so we don't entangle p and q
ListNode *concatLists(ListNode *p, ListNode *q){
    ListNode *qCopy = copyList(q);

    ListNode *scanner = p;
    while (scanner -> next) scanner = scanner -> next;
    scanner -> next = qCopy;

    return p;
}

// Merge two sorted lists into a long sorted list. Always to be called on copies of the original lists, because this function
// uses the same nodes, just reordering them
ListNode *mergeLists(ListNode *p, ListNode *q){
    if (!p) return q;
    if (!q) return p;

    // Start the merged list
    ListNode *head = nullptr;
    if (p->val < q->val){
        head = p;
        p = p -> next;
    } else{
        head =  q;
        q = q -> next;
    }

    // Loop through it
    ListNode *scanner = head;
    while (p && q){
        if (p->val < q->val){
            scanner -> next = p;
            scanner = scanner -> next;
            p = p -> next;
        } else{
            scanner -> next = q;
            scanner = scanner -> next;
            q = q -> next;
        }
    }

    while (p){
        scanner -> next = p;
        scanner = scanner -> next;
        p = p -> next;
    }

    while (q){
        scanner -> next = q;
        scanner = scanner -> next;
        q = q -> next;
    }

    return head;
}

// Merge k sorted lists; the same as for mergeLists() applies
ListNode *mergeKLists(std::vector<ListNode *> &lists){
    // Initial hassle
    std::vector<ListNode *>::iterator i = lists.begin();
    while (i != lists.end()){
        if (!*i){
            i = lists.erase(i);
        } else{
            ++i;
        }
    }

    int k = lists.size();
    if (k == 0) return nullptr;
    if (k == 1) return lists[0];

    // Because the lists are given as a vector, we can set our loop to continue provided there are any lists at all,
    // and on each iteration it will run through only those lists we haven't yet used up and removed from the vector
    ListNode *result;
    ListNode *rScanner;
    bool first = true;

    while (lists.size()){
        // Find the next smallest node and append it to our results list
        int min = INT_MAX;
        for (ListNode *head : lists) min = std::min(min, head->val);
        if (first){
            result = new ListNode(min);
            rScanner = result;
            first = false;
        } else{
            rScanner -> next = new ListNode(min);
            rScanner = rScanner -> next;
        }

        // Remove that node from our source material
        for (int l=0; l<lists.size(); l++){
            if (lists[l]->val == min){
                ListNode *temp = lists[l];

                // Either shift the list forwards, or remove it entirely if there's nothing left
                if (temp -> next){
                    lists[l] = temp -> next;
                } else{
                    lists.erase(lists.begin() + l);
                }
                delete temp;

                break;
            }
        }
    }

    return result;
}

// Rotate a list right by k places
ListNode *rotateRight(ListNode *head, int k){
    if (k==0 || !head || !head->next) return head;              // A single node doesn't need rotating
    k %= listLen(head);

    ListNode *result = head;
    while (k > 0){
        /* Remove the last node and place it at the head of the list */
        // Scan forward to the penultimate node
        ListNode *scanner = result;
        while (scanner->next && (scanner->next)->next) scanner = scanner -> next;

        // Isolate the last node
        ListNode *temp = scanner -> next;
        scanner -> next = nullptr;

        // Prepend it to the head
        temp -> next = result;
        result = temp;

        k--;
    }

    return result;
}

// Remove all nodes which have duplicates value-wise from a sorted list, i.e. [1, 2, 3, 3, 4] becomes [1, 2, 4]
ListNode *deleteDuplicates(ListNode *head){
    if (!head) return nullptr;

    ListNode *scanner = head;
    std::vector<int> vals;
    std::vector<int> duplicates;

    // Do a first pass to work out which values are duplicate
    for (; scanner; scanner=scanner->next){
        if (find(vals.begin(), vals.end(), scanner->val) != vals.end()){
            duplicates.push_back(scanner -> val);
        } else{
            vals.push_back(scanner -> val);
        }
    }

    // Then remove nodes with that value
    for (scanner=head; scanner; scanner=scanner->next){
        if (find(duplicates.begin(), duplicates.end(), scanner->val) == duplicates.end()){
            // The result starts here, so delete prior nodes and set head to scanner
            while (head != scanner){
                ListNode *temp = head;
                head = temp -> next;
                delete temp;
            }

            break;
        }

        // If all nodes are duplicates
        if (!scanner -> next) return nullptr;
    }

    scanner = head;
    while (scanner -> next){
        if (find(duplicates.begin(), duplicates.end(), scanner->next->val) != duplicates.end()){
            ListNode *temp = scanner -> next;
            if (temp->next){
                scanner -> next = temp -> next;
            } else{
                scanner -> next = nullptr;
            }

            delete temp;
        } else{
            scanner = scanner -> next;
        }
    }

    return head;
}

// Reverse a list in groups of k nodes
ListNode *reverseKGroup(ListNode *head, int k){
    ListNode *dummy = new ListNode(0, head);
    ListNode *scanner = dummy;

    while (scanner -> next){
        // Check we still have k nodes to work with, at the same time as making a vector of the next k nodes
        ListNode *checker = scanner;
        std::vector<ListNode *> group;

        for (int kk=0; kk<k; kk++){
            checker = checker -> next;
            if (!checker) break;
            group.push_back(checker);
        }
        if (!checker) break;
        checker = checker -> next;

        // Reverse the k nodes starting at scanner -> next
        for (auto it=group.rbegin(); it!=group.rend(); ++it){
            std::cout << (*it)->val << "\n";
            scanner -> next = *it;
            scanner = scanner -> next;
        }

        scanner -> next = checker;
    }

    head = dummy -> next;
    delete dummy;

    return head;
}

// Rearrange a list such that all nodes with val < x come before all with val >= x
// Preserve order within each partition
ListNode *partitionList(ListNode *head, int x){
    ListNode *dummy = new ListNode(0, head);
    ListNode *scanner = dummy;
    ListNode *part1 = nullptr;
    ListNode *dummy1 = new ListNode(0, part1);
    ListNode *scanner1 = dummy1;

    // Go through the list siphoning off nodes < x to the tail
    while (scanner && scanner->next){
        if (scanner->next->val < x){
            // Remove the node from the main list
            ListNode *temp = scanner -> next;
            scanner -> next = temp -> next;

            // And add it to the first partition
            scanner1 -> next = temp;
            scanner1 = scanner1 -> next;
            scanner1 -> next = nullptr;
        } else{
            // To avoid the moving making us skip a node
            scanner = scanner -> next;
        }
    }

    // Append the two partitions
    ListNode *part2 = dummy -> next;
    if (dummy1 -> next){
        dummy -> next = dummy1 -> next;
        scanner1 -> next = part2;
    }

    head = dummy -> next;
    delete dummy;
    delete dummy1;

    return head;
}

// Reverse nodes between n = left and right
// We can assume left and right are within range
ListNode *reverseBetween(ListNode *head, int left, int right){
    // Seek to the node before the first to reverse
    ListNode *dummy = new ListNode(0, head);
    ListNode *scanner = dummy;
    for (int i=0; i<left; i++)
        scanner = scanner -> next;

    // Jump ahead to find the tail
    ListNode *tail = scanner;
    for (int i=left; i<=right; i++) tail = tail -> next;
    
    // Join one node at a time to the tail, working backwards until we hit the starting point
    ListNode *tScanner = tail;
    while (scanner != tail){
        ListNode *temp = scanner -> next;
        scanner -> next = tScanner;
        tScanner = scanner;
        scanner = temp;
    }

    // Join the beginning of the list to the node that is now at the starting point
    scanner = dummy;
    for (int i=0; i<left-1; i++)
        scanner = scanner -> next;
    scanner -> next = tScanner;

    head = dummy -> next;
    delete dummy;

    return head;
}

// Reorder list (L0 -> L1 -> ... -> Ln-1 -> Ln) to (L0 -> Ln -> L1 -> Ln-1 -> L2 -> ...)
ListNode *reorderList(ListNode *head){
    // Use two pointers at each end of the list, converging, to find the nodes we need; reorder them with a new head
    ListNode *fwdScanner = head;
    ListNode *bckwdScanner = head;
    while (bckwdScanner -> next) bckwdScanner = bckwdScanner -> next;

    ListNode *dummyHead = fwdScanner;
    ListNode *dummy = new ListNode(0);
    ListNode *dummyDummy = dummy;

    while (fwdScanner && bckwdScanner){
        // Assign the nodes
        dummy -> next = fwdScanner;
        dummy = dummy -> next;
        if (fwdScanner == bckwdScanner) break;
        ListNode *fwdTemp = fwdScanner;

        // Decrement the backward pointer
        ListNode *s = fwdScanner;
        while (s->next != bckwdScanner)
            s = s -> next;
        ListNode *newBckwdScanner = s;

        fwdScanner = fwdScanner -> next;
        dummy -> next = bckwdScanner;
        dummy = dummy -> next;
        if (fwdScanner == bckwdScanner) break;
        bckwdScanner = newBckwdScanner;
    }

    dummy -> next = nullptr;
    delete dummyDummy;
    head = dummyHead;

    return head;
}

// Rearrange a list with all the odd-indiced nodes (first, third, etc.) followed by all the even
ListNode *oddEven(ListNode *head){
    if (!head || !head->next) return head;
        
    ListNode *odds = new ListNode(0);
    ListNode *evens = new ListNode(0);
    ListNode *oddScanner = odds;
    ListNode *evenScanner = evens;

    ListNode *parts[] = {oddScanner, evenScanner};
    int parity = 0;
    while (head){
        parts[parity] -> next = head;
        parts[parity] = parts[parity] -> next;
        head = head -> next;
        parity = (parity == 0);
    }
    parts[parity] -> next = nullptr;

    ListNode *newHead = odds -> next;
    ListNode *newScanner = newHead;
    while (newScanner -> next) newScanner = newScanner -> next;
    newScanner -> next = evens -> next;
    delete odds;
    delete evens;

    return newHead;
}

// Check whether a linked list is palindromic
bool isPalindrome(ListNode *head){
    // Find the middle of the list
    ListNode *fast = head;
    ListNode *slow = head;
    while (fast && fast->next){
        fast = fast -> next -> next;
        slow = slow -> next;
    }

    // Now reverse the second half of the list
    ListNode *prev = slow;
    slow = slow -> next;
    prev -> next = nullptr;

    while (slow){
        ListNode *temp = slow -> next;
        slow -> next = prev;
        prev = slow;
        slow = temp;
    }

    // Check the two halves for identity
    fast = head;
    slow = prev;

    while (slow){
        if (slow->val != fast->val) return false;
        fast = fast -> next;
        slow = slow -> next;
    }

    return true;
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

void printFunc(int l){
    printList(heads[l]);
}

void lenFunc(int l){
    int len = listLen(heads[l]);

    if (len == -1){
        std::cout << "List #" << l+1 << " has a cycle.\n";
    } else{
        std::cout << "List #" << l+1 << " has " << len << " nodes.\n";
    }
}

void sortFunc(int l){
    heads[l] = sortList(heads[l]);
    std::cout << "List sorted. Updated list: ";
    printList(heads[l]);
}

void cycleFunc(int l){
    int p = hasCycle(heads[l]);

    if (p){
        std::cout << "List #" << l+1 << " has a cycle, from node " << std::max(0, p) << " to node " << cycleStart(heads[l]) << ".\n";
    } else{
        std::cout << "List does not have a cycle.\n";
    }
}

void loopFunc(int l){
    loopList(heads[l]);
    std::cout << "List looped. New list: ";
    printList(heads[l]);
}

void catFunc(int l){
    int m = getListNum("Which other # list? (1 - 10) ");

    // Avert disaster
    if (!heads[m]){
        std::cout << "List empty.\n";
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

void nthFunc(int l){
    int n = getInt("How many places from end? ");
    removeNthFromEnd(heads[l], n);

    std::cout << "Removed. New list: ";
    printList(heads[l]);
}

void swapFunc(int l){
    if (hasCycle(heads[l])){
        std::cout << "Can't swap pairs in a list with a cycle.\n";
    } else{
        heads[l] = swapPairs(heads[l]);
        std::cout << "Pairs swapped. New list: ";
        printList(heads[l]);
    }
}

void copyFunc(int l){
    int m = getListNum("Copy to which  # list? (1 - 10) ");
    if (heads[m]){
        deleteList(heads[m]);
        std::cout << "Clearing list #" << m+1 << "...\n";
    }

    heads[m] = copyList(heads[l]);
    std::cout << "Copied. List #" << m+1 << " is now: ";
    printList(heads[m]);
}

void sortedFunc(int l){

    if (isSorted(heads[l])){
        std::cout << "List sorted.\n";
    } else{
        std::cout << "List unsorted.\n";
    }
}

void mergeFunc(int l){
    int m = getListNum("Which other # list? (1 - 10) ");
    ListNode *p = heads[l];
    ListNode *q = copyList(heads[m]);

    if (!isSorted(p)) p = sortList(p);
    if (!isSorted(q)) q = sortList(q);

    if (!(heads[l] = mergeLists(p, q))){
        std::cout << "Both lists are empty.\n";
    } else{
        std::cout << "Lists merged. List #" << l+1 << " is now: ";
        printList(heads[l]);
    }
}

void mergeKFunc(){
    int k = getInt("How many lists? ");
    if (k < 2){
        std::cout << "Can only merge two or more lists.\n";
        return;
    }
    std::cout << "The result of the merge will be stored at the first list specified.\n\n";

    int listNums[k];
    for (int l=0; l<k; l++) listNums[l] = getListNum("Which # for list number " + std::to_string(l+1) + "? ");

    // Gather the lists to pass to mergeKLists() as a vector; all but the first are duplicates of heads[k] because the merge will use the 
    // same nodes we pass it (and store these in a list starting at the first head)
    std::vector<ListNode *> listsVec;
    for (int l=0; l<k; l++){
        ListNode *curr = ((l==0) ?  heads[listNums[0]] : copyList(heads[listNums[l]]));
        listsVec.push_back(curr);
    }

    // Pass the gathered lists
    heads[listNums[0]] = mergeKLists(listsVec);
    std::cout << "Lists merged. List #" << listNums[0]+1 << " is now: ";
    printList(heads[listNums[0]]);
}

void rotateFunc(int l){
    int k = getInt("Rotate by how many places? ");

    if (k < 0){
        std::cout << "Rotation must be by a positive number of places.\n";
    } else if (hasCycle(heads[l])){
        std::cout << "Can't rotate a list with a cycle.\n";
    } else{
        heads[l] = rotateRight(heads[l], k);
        std::cout << "List rotated. New list: ";
        printList(heads[l]);
    }
}

void deleteDuplicatesFunc(int l){
    if (!isSorted(heads[l])) heads[l] = sortList(heads[l]);

    heads[l] = deleteDuplicates(heads[l]);
    std::cout << "Duplicates removed. List #" << l+1 << " is now: ";
    printList(heads[l]);
}

void reverseKFunc(int l){
    int k = getInt("Enter k: ");
    if (k < 1){
        std::cout << "k must be at least 1.\n";
        return;
    } else if (k > listLen(heads[l])){
        std::cout << "Reversal redundant: the list has fewer than " << k << " nodes.\n";
        return;
    }

    heads[l] = reverseKGroup(heads[l], k);
    std::cout << "Reversal complete. List is now: ";
    printList(heads[l]);
}

void partitionFunc(int l){
    int x = getInt("Enter the partition boundary: ");

    heads[l] = partitionList(heads[l], x);
    std::cout << "Partition complete. List is now: ";
    printList(heads[l]);
}

void reverseBetweenFunc(int l){
    int a = getInt("Reverse nodes starting at position (not zero-indexed): ");
    if (a < 1){
        std::cout << "Cannot start at a negative index.\n";
        return;
    } else if (a > listLen(heads[l])){
        std::cout << "Node out of range.\n";
        return;
    }

    int b = getInt("And up to: ");
    if (b < a){
        std::cout << "The stopping node cannot be before the starting node.\n";
        return;
    } else if (b > listLen(heads[l])){
        std::cout << "Node out of range.\n";
        return;
    }

    heads[l] = reverseBetween(heads[l], a, b);
    std::cout << "Reversed. List #" << l+1 << " is now: ";
    printList(heads[l]);
}

void reorderFunc(int l){
    heads[l] = reorderList(heads[l]);
    std::cout << "Reordered list #" << l+1 << ": ";
    printList(heads[l]);
}

void oddevenFunc(int l){
    heads[l] = oddEven(heads[l]);
    std::cout << "Rearranged list #" << l+1 << ": ";
    printList(heads[l]);
}

void palindromeFunc(int l){
    std::cout << "List #" << l+1 << (isPalindrome(heads[l]) ? " is " : " is not ") << "a palindrome.\n";
}

int main(int argc, char *argv[]){
    std::cout << "Welcome to the LeetCode Linked List Manipulator. Enter a command like 'set', 'print', or 'sort'.\n";
    std::cout << "Try \"commands\" for a list of commands.\n\n";

    // Give the user ten linked list-slots to work with
    for (int l=0; l<10; l++)
        heads[l] = nullptr;

    // Create a map of commands to functions
    std::unordered_map<std::string, void (*)(int)> commands;
    //commands.emplace("set", &setFunc);
    commands.emplace("print", &printFunc);
    commands.emplace("len", &lenFunc);
    commands.emplace("sort", &sortFunc);
    commands.emplace("cycle", &cycleFunc);
    commands.emplace("loop", &loopFunc);
    commands.emplace("cat", &catFunc);
    commands.emplace("nthfromend", &nthFunc);
    commands.emplace("swap", &swapFunc);
    commands.emplace("copy", &copyFunc);
    commands.emplace("sorted", &sortedFunc);
    commands.emplace("merge", &mergeFunc);
    //commands.emplace("mergek", &mergeKFunc);
    commands.emplace("rotate", &rotateFunc);
    commands.emplace("duplicates", &deleteDuplicatesFunc);
    commands.emplace("reversek", &reverseKFunc);
    commands.emplace("partition", &partitionFunc);
    commands.emplace("reversebet", &reverseBetweenFunc);
    commands.emplace("reorder", &reorderFunc);
    commands.emplace("oddeven", &oddevenFunc);
    commands.emplace("palindrome", &palindromeFunc);

    // Let the user do things
    std::string input;
    do{
        std::cout << "> ";
        std::cin >> input;

        // Asking for the first list and checking for emptiness can be abstracted out to this point
        // Although set and mergek have to ask for the list # differently
        if (commands.find(input) != commands.end()){
            int l = getListNum();
            if (!heads[l]){
                std::cout << "List empty.\n";
            } else{
                (commands.at(input))(l);
            }
        } else if (input == "set"){
            setFunc();
        } else if (input == "mergek"){
            mergeKFunc();
        } else if (input == "commands"){
            // List all commands in the map
            std::cout << "Available commands:\n\n";
            for (std::unordered_map<std::string, void (*)(int)>::iterator it = commands.begin();
            it != commands.end();
            ++it)
                std::cout << it->first << "\n";
        }  else if (input != "exit"){
            std::cout << "Command not recognised.\n";
        }
    } while (input != "exit");

    for (int l=0; l<10; l++)
        if(heads[l]) deleteList(heads[l]);

    return 0;
}