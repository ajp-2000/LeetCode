/* trees.cpp – A number of functions to do things with binary trees, built based on several LeetCode challenges. */

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <vector>

/* First, some overarching things*/

// The character that represents non-existent nodes on a tree
const std::string NULL_CHAR = " ";

// The struct for a tree node: trees will be passed by their heads
struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;

    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
};

TreeNode *listToRoot(int vals[], int len);
bool deleteTree(TreeNode* root);
int maxDepth(TreeNode* root);
int maxAbsVal(TreeNode* root);
std::vector<TreeNode> nodesAtDepth(TreeNode *root, int depth);
std::vector<std::string> valsAtDepth(TreeNode *root, int depth);
bool printTree(TreeNode *root);

// A function to return a tree according to a list which reads node values from left to right, head to leaves.
// List values of INT_MIN stand in for gaps in the tree structure, i.e. [1, INT_MIN, 2] represents a tree with 
// head 1, and right leaf 2 but no left leaf; values of INT_MAX stand for empty space at the end of a row
TreeNode *listToRoot(int vals[], int len){ 
    if (len == 0) return nullptr;
    if (len == 1) return new TreeNode(vals[0], nullptr, nullptr);

    // First convert the list to a 2D vector so we know where we stand
    int depth = 0;
    int i = 0;
    std::vector<std::vector<int> > tree;

    while (i < len){
        std::vector<int> currLine;
        for (int j=0; j<pow(2, depth); j++){
            // i is the position in vals, j the position within the current depth
            if (i == len){
                // Then the list passed stops short of every node, so fill in nulls
                for (; j<pow(2, depth); j++)
                    currLine.push_back(INT_MIN);

                break;
            }

            currLine.push_back(vals[i]);
            i++;
        }
        tree.push_back(currLine);
        depth++;
    }

    // And build a tree using this vector, starting with the most remote leaf nodes, then working up the tree
    // Remembering to take INT_MIN as a placeholder for a non-existent node
    depth--;
    int maxDepth = depth;
    std::vector<std::vector<TreeNode *> > nodes;

    for (; depth>0; depth--){
        std::vector<TreeNode *> curr;

        for (i=0; i<pow(2, depth); i++){
            // Create the node with its value, attaching any child nodes
            // If there are children, their i-positions will be  i*2 and i*2 + 1, for depth++
            TreeNode *leftptr = nullptr;
            TreeNode *rightptr = nullptr;

            if (tree[depth][i] == INT_MAX  || tree[depth][i] == INT_MIN) continue;

            if (depth < maxDepth){
                if (tree[depth+1][i*2] != INT_MIN && tree[depth+1][i*2] != INT_MAX){
                    //std::cout << tree[depth+1][i*2] << "\n";
                    // Work out how many nodes along this corresponds to
                    int k = 0;
                    for (int j=0; j<i*2; j++){
                        if (tree[depth+1][j] != INT_MIN && tree[depth+1][j] != INT_MAX)
                            k++;
                    }

                    leftptr = nodes[0][k];
                }
                if (tree[depth+1][i*2 + 1] != INT_MIN && tree[depth+1][i*2 + 1] != INT_MAX){
                    //std::cout << tree[depth+1][i*2 + 1] << "\n";
                    int k = 0;
                    for (int j=0; j<i*2 + 1; j++){
                        if (tree[depth+1][j] != INT_MIN && tree[depth+1][j] != INT_MAX)
                            k++;
                    }
                    rightptr = nodes[0][k];
                }
            }

            TreeNode *n = new TreeNode(tree[depth][i], leftptr, rightptr);
            curr.push_back(n);
        }

        nodes.insert(nodes.begin(), curr);
    }

    // Finally reaching the root
    TreeNode *leftptr = nullptr;
    TreeNode *rightptr = nullptr;

    if (tree[1][0] != INT_MIN && tree[1][0] != INT_MAX) leftptr = nodes[0][0];
    if (tree[1][1] != INT_MIN && tree[1][1] != INT_MAX){
        rightptr = nodes[0][(tree[1][0] != INT_MIN && tree[1][0] != INT_MAX) ? 1 : 0];
    }
    TreeNode *root = new TreeNode(vals[0], leftptr, rightptr);
    
    return root;
}

// Delete a tree recursively, given its head: needs to be used on any tree created by listToRoot()
// Returns true iff any deleting was done (i.e. if the root passed wasn't a null pointer)
bool deleteTree(TreeNode *root){
    if (root == nullptr) return false;

    deleteTree(root->left);
    deleteTree(root->right);
    
    delete root;
    return true;
}

// Return the values of all a tree's nodes at a given depth, like reading a horizontal line off the tree
// Used by valsAtDepth()
std::vector<TreeNode> nodesAtDepth(TreeNode *root, int depth){
    std::vector<TreeNode> nodes;

    if (depth > 0){
        std::vector<TreeNode> newNodes;
        if (root->left != nullptr){
            newNodes = nodesAtDepth(root->left, depth-1);
            nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());
        } else{
            for (int i=0; i<pow(2, depth-1); i++)
                nodes.push_back(TreeNode(INT_MIN));
        }

        if (root->right != nullptr){
            newNodes = nodesAtDepth(root->right, depth-1);
            nodes.insert(nodes.end(), newNodes.begin(), newNodes.end());
        } else{
            for (int i=0; i<pow(2, depth-1); i++)
                nodes.push_back(TreeNode(INT_MIN));
        }
    } else if (depth == 0){
        nodes.push_back(*root);
    }

    return nodes;
}

// Return a list of n-character node values, or " + " for non-existent nodes
// Used by printTree()
std::vector<std::string> valsAtDepth(TreeNode *root, int depth, int n){
    std::vector<std::string> vals;
    std::vector<TreeNode> nodes = nodesAtDepth(root, depth);

    for (TreeNode node : nodes){
        std::string val = "";
        int digits = (node.val==0 || node.val==INT_MIN) ? 1 : int(log10(node.val) + 1);
        int preSize = (n - digits) / 2;

        for (int i=0; i<preSize; i++) val += " ";
        val += (node.val > INT_MIN) ? std::to_string(node.val) : NULL_CHAR;
        for (int i=0; i<n-digits-preSize; i++) val += " ";
        vals.push_back(val);
    }

    return vals;
}

// Print an ascii visualisation of a tree, returning true if something was printed
// - Every node is given: 3 characters by default, or more if the tree contains longer vals
// - Nodes are separated by: 1 character if places is odd, or two if even;
// or more if required by the spacing of nodes lower down the tree
bool printTree(TreeNode *root){
    if (!root){
        std::cout << "Tree empty.\n";
        return false;
    }

    // Establish the spacing
    int places = std::max(3, int(log10(maxAbsVal(root)))+1);
    int spaces = 2 - (places % 2);
    int depth = maxDepth(root);

    // Set out the rows from the bottom up
    std::vector<std::vector<int> > rows;
    for (int d=depth-1; d>=0; d--){
        std::vector<int> row;

        if (d == depth-1){
            int pos = 0;
            for (int i=0; i<pow(2, d); i++){
                row.push_back(pos);
                pos += places + spaces;
            }
        } else{
            for (int i=0; i<pow(2, d); i++){
                row.push_back(int((rows[0][i*2] + rows[0][i*2+1]) / 2));
            }
        }

        rows.insert(rows.begin(), row);
    }

    // Then print
    for (int d=0; d<depth; d++){
        int cursor = 0;
        std::vector<std::string> nodeVals = valsAtDepth(root, d, places);

        for (int i=0; i<nodeVals.size(); i++){
            for (int j=cursor; j<rows[d][i]; j++){
                std::cout << " ";
                cursor++;
            }
            std::cout << nodeVals[i];
            cursor += places;
        }
        std::cout << "\n\n";
    }
    std::cout << std::endl;

    return true;
}

/* Then come the functions the user might directly use, including maxDepth() which is used by the above*/

// Return the maximum depth of a tree, depth = 1 being a lone root
int maxDepth(TreeNode* root) {
    if (!root)
        return 0;

    int a = maxDepth(root->left);
    int b = maxDepth(root->right);

    return 1 + std::max(a, b);
}

// Return the greatest absolute node-value in the tree
int maxAbsVal(TreeNode* root){
    if (!root) return INT_MIN;

    int lVal = INT_MIN;
    int rVal = INT_MIN;
    if (root -> left) lVal = maxAbsVal(root->left);
    if (root -> right) rVal = maxAbsVal(root->right);

    return std::max(abs(root->val), std::max(lVal, rVal));
}

// Return the minimum depth of a tree
int minDepth(TreeNode* root) {
    if (!root) return 0;
    return 1 + std::min(minDepth(root->left), minDepth(root->right));
}

// Check whether two trees are the same
bool isSameTree(TreeNode* p, TreeNode* q) {
    // Deal with the possibility of nulls
    if ((p==nullptr) || (q==nullptr)){
        if ((p==nullptr) && (q==nullptr))
            return true;
        return false;
    }

    // Compare values
    if (p->val != q->val)
        return false;
        
    // Compare subtrees
    if (!isSameTree(p->left, q->left) || !isSameTree(p->right, q->right))
        return false;
        
    return true;
    }

// Recursively check if two trees are mirror images of each other
bool areMirrors(TreeNode* p, TreeNode* q){
    // Deal with potential nulls
    if ((p==nullptr) || (q==nullptr)){
        if ((p==nullptr) && (q==nullptr))
            return true;
        return false;
    }

    // Check the present case
    if (p->val != q->val)
        return false;
        
    // Recurse
    if (!areMirrors(p->left, q->right) || !areMirrors(p->right, q->left))
        return false;
        
    return true;
}

// Check whether a tree is symmetrical
bool isSymmetric(TreeNode* root) {
    // Deal with possible nulls
    if (root == nullptr)
        return true;
        
    return areMirrors(root->left, root->right);
}

// Check whether a tree is balanced
bool isBalanced(TreeNode* root) {
    if (!root)
        return true;
        
    if ((!isBalanced(root->left)) || (!isBalanced(root->right)))
        return false;
        
    if (abs(maxDepth(root->left) - maxDepth(root->right)) <= 1)
        return true;
        
    return false;
}

// Check whether a tree has a path summing to a given value
bool hasPathSum(TreeNode* root, int targetSum) {
    // Try each possible route to a leaf
    if (!root) return false;
    if (root->val == targetSum) return true;

    return (hasPathSum(root->left, targetSum-root->val)) || (hasPathSum(root->right, targetSum-root->val));
}

// Return all root-to-leaf paths with the target sum
std::vector<std::vector<int> > pathSum(TreeNode* root, int targetSum){
    std::vector<std::vector<int> > paths;

    if (root){
        std::vector<int> path;
        path.push_back(root -> val);

        if (root->val==targetSum && (!root->left && !root->right))
                paths.push_back(path);

        std::vector<TreeNode*> children;
        if (root -> left) children.push_back(root -> left);
        if (root -> right) children.push_back(root -> right);

        for (TreeNode* child : children){
            std::vector<std::vector<int> > stubs = pathSum(child, targetSum - (root->val));
            for (int s=0; s<stubs.size(); s++){
                stubs[s].insert(stubs[s].begin(), path.begin(), path.end());
                paths.push_back(stubs[s]);
            }
        }
    }

    return paths;
}

// Flatten the tree to the right by pre-order traversal, effectively creating a singly-linked list
void flatten(TreeNode* root){
    if (root){
        if (root -> left) flatten(root->left);
        if (root -> right) flatten(root->right);

        if (root->left){
            TreeNode *temp = root -> right;
            root -> right = root -> left;
            root -> left = nullptr;

            TreeNode *rScanner = root -> right;
            while (rScanner -> right) rScanner = rScanner -> right;
            rScanner -> right = temp;
        }
    }
}

std::vector<int> preorderTraversal(TreeNode* root){
    std::vector<int> result;
    if (!root) return result;

    result.push_back(root->val);
    std::vector<int> left = preorderTraversal(root->left);
    result.insert(result.end(), left.begin(), left.end());
    std::vector<int> right = preorderTraversal(root->right);
    result.insert(result.end(), right.begin(), right.end());

    return result;
}

std::vector<int> postorderTraversal(TreeNode* root){
    std::vector<int> result;
    if (!root) return result;

    std::vector<int> left = postorderTraversal(root->left);
    result.insert(result.end(), left.begin(), left.end());
    std::vector<int> right = postorderTraversal(root->right);
    result.insert(result.end(), right.begin(), right.end());
    result.push_back(root->val);

    return result;
}

std::vector<std::vector<int> > levelOrder(TreeNode* root){
    std::vector<std::vector<int> > result;

    std::vector<TreeNode*> lastNodes;
    if (root){
        TreeNode *dummy = new TreeNode(0, root, nullptr);
        lastNodes.push_back(dummy);
    
        for (int depth=0; depth<maxDepth(root); depth++){
            std::vector<int> row;
            std::vector<TreeNode*> currNodes;

            for (TreeNode *curr : lastNodes){
                if (curr -> left){
                    row.push_back(curr -> left -> val);
                    currNodes.push_back(curr -> left);
                }
                if (curr -> right){
                    row.push_back(curr -> right -> val);
                    currNodes.push_back(curr -> right);
                }
            }

            lastNodes.clear();
            lastNodes = currNodes;
            result.push_back(row);
        }

        delete dummy;
    }

    return result;
}

// maxVal() and minVal() are for use by isValidBST()
int maxVal(TreeNode* root){
    if (!root) return INT_MIN;

    int lVal = INT_MIN;
    int rVal = INT_MIN;
    if (root -> left) lVal = maxVal(root->left);
    if (root -> right) rVal = maxVal(root->right);

    return std::max(root->val, std::max(lVal, rVal));
}

int minVal(TreeNode* root){
    if (!root) return INT_MAX;

    int lVal = INT_MAX;
    int rVal = INT_MAX;
    if (root -> left) lVal = minVal(root->left);
    if (root -> right) rVal = minVal(root->right);

    return std::min(root->val, std::min(lVal, rVal));
}

// Determine whether / not a tree is a valid binary search tree
bool isValidBST(TreeNode* root){
    if (!root) return true;

    if (root -> left){
        if (maxVal(root->left) >= root->val) return false;
        if (!isValidBST(root->left)) return false;
    }
    if (root -> right){
        if (minVal(root->right) <= root->val) return false;
        if (!isValidBST(root->right)) return false;
    }

    return true;
}

// Return all structurally unique BSTs with exactly n nodes of unique values from 1 to n
std::vector<TreeNode*> generateTrees(int n){
    std::vector<TreeNode*> trees;

    // All the options for the root
    for (int i=0; i<n; i++){
        TreeNode *root = new TreeNode(i);

        // All the options for which node we are going to explore the options for, i.e. next node, one after that, etc.
        for (int j=0; j<n; j++){
            // All the options for what value the jth node along will take
            for (int k=0; k<n; k++){

            }
        }
    }

    return trees;
}

// Get a tree from the user
TreeNode *getTree(){
    // Take input for a tree then print it
    int depth = 0;
    int i = 0;                                                      // An overall count for position in vals
    std::string line;

    std::cout << "How deep is the tree? ";
    do {
        getline(std::cin, line);
        try{
            depth = stoi(line);
            if (depth < 1) std::cout << "Please enter a positive integer: ";
        } catch(...){
            std::cout << "Please enter a number: ";
        }
    } while (depth < 1);

    int treeArray[int(std::pow(2, depth) - 1)];

    // Take line-by-line input of the tree values
    for (int d=0; d<depth; d++){
        std::cout << "Enter line " << (d+1) << ", with values separated by spaces: ";
        getline(std::cin, line);

        // Parse the line
        std::vector<int> vals;
        std::stringstream s(line);
        std::string word;
        while (s >> word){
            try{
                vals.push_back(stoi(word));
            } catch(...){
                // Interpret NULL as a not-node, with the stand-in value INT_MIN
                if (word == "NULL"){
                    vals.push_back(INT_MIN);
                    continue;
                }

                // Otherwise nnounce the error and prompt for the same again
                std::cout << "That wasn't a number." << std::endl;
                std::cin.clear();
                d--;
                continue;
            }
        }

        // Add the values to the tree list
        if (vals.size() > pow(2, d)){
            // Too many values have been entered for this line of the tree
            std::cout << "Too many values. Try again.\n";
            d--;
            continue;
        }

        int j = 0;
        for (; j<vals.size(); j++){
            treeArray[i+j] = vals[j];
        }
        for (; j < pow(2, d); j++){
            treeArray[i+j] = INT_MAX;
        }
        
        i += j;
    }

    // Wrap up
    TreeNode *root = new TreeNode();
    root = listToRoot(treeArray, sizeof(treeArray)/sizeof(treeArray[0]));
    std::cout << "\nTree formed:\n\n";
    printTree(root);

    return root;
}

/* Now handle the above */

// Get an integer at all
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
int getTreeNum(std::string prompt = "Which # tree? (1 - 10) "){
    int num;
    do{
        num = getInt(prompt);
    } while (num<1 || num>9);

    return num - 1;
}

/* The interface */
TreeNode *roots[10];

void setFunc(){
    int t = getTreeNum();

    // Report the current tree
    if (!roots[t]){
        std::cout << "Tree #" << t+1 << " is currently empty.\n";
    } else{
        std::cout << "Tree #" << t+1 << " is currently:\n\n";
        printTree(roots[t]);
    }

    // Enter a new tree
    std::cout << "Setting tree #" << t+1 << ":\n\n";
    roots[t] = getTree();
}

void printFunc(){
    printTree(roots[getTreeNum()]);
}

void maxDepthFunc(){
    int t = getTreeNum();
    std::cout << "Maximum depth: " << maxDepth(roots[t]) << "\n";
}

void minDepthFunc(){
    int t = getTreeNum();
    std::cout << "Minimum depth: " << minDepth(roots[t]) << "\n";
}

void sameFunc(){
    int t1 = getTreeNum();
    int t2 = getTreeNum("Which other # tree? (1 - 10) ");
    std::cout << (isSameTree(roots[t1], roots[t2]) ? "Same tree.\n" : "Different trees.\n");
}

void symFunc(){
    std::cout << (isSymmetric(roots[getTreeNum()]) ? "Symmetrical.\n" : "Asymmetrical.\n");
}

void balancedFunc(){
    std::cout << (isBalanced(roots[getTreeNum()]) ? "Balanced.\n" : "Unbalanced.\n");
}

void hasPathFunc(){
    int t = getTreeNum();
    int targetSum = getInt("What is the target sum? ");
    std::cout << (hasPathSum(roots[t], targetSum) ? "Path found.\n" : "Path not found.\n");
}

void pathsFunc(){
    int t = getTreeNum();
    int targetSum = getInt("What is the target sum? ");
    std::vector<std::vector<int> > paths = pathSum(roots[t], targetSum);
    if (paths.size() == 0){
        std::cout << "No paths found.\n";
    } else{
        std::cout << "Paths found:\n";
        for (std::vector<int> path : paths){
            for (int i=0; i<path.size(); i++){
                std::cout << path[i];
                if (i < path.size()-1) std::cout << " -> ";
            }
            std::cout << "\n";
        }
    }
}

void flattenFunc(){
    int t = getTreeNum();
    flatten(roots[t]);
    std::cout << "Flattened:\n";
    printTree(roots[t]);
}

void preorderFunc(){
    int t = getTreeNum();
    if (!roots[t]){
        std::cout << "Tree empty.\n";
    } else{
        std::vector<int> traversal = preorderTraversal(roots[t]);
        std::cout << "Pre-order traversal: ";
        for (int t : traversal)
            std::cout << t << " ";
        std::cout << "\n";
    }
}

void postorderFunc(){
    int t = getTreeNum();
    if (!roots[t]){
        std::cout << "Tree empty.\n";
    } else{
        std::vector<int> traversal = postorderTraversal(roots[t]);
        std::cout << "Pre-order traversal: ";
        for (int t : traversal)
            std::cout << t << " ";
        std::cout << "\n";
    }
}

void levelorderFunc(){
    int t = getTreeNum();
    if (!roots[t]){
        std::cout << "Tree empty.\n";
    } else{
        std::vector<std::vector<int> > traversal = levelOrder(roots[t]);
        std::cout << "Pre-order traversal: ";
        for (std::vector<int> trav : traversal){
            std::cout << "[";
            for (int t : trav)
                std::cout << t << " ";
            std::cout << "] ";
        }
        std::cout << "\n";
    }
}

void bstFunc(){
    int t = getTreeNum();
    std::cout << "Tree #" << t+1 << (isValidBST(roots[t]) ? " is " : " is not ") << "a valid binary search tree.\n";
}

int main(int argc, char *argv[]){
    std::cout << "trees.cpp!\n";
    std::cout << "Type \"commands\" for a full list of commands.\n\n";
    
    // Let the user do things with ten trees, NULL by default
    for (int t=0; t<10; t++) roots[t] = nullptr;

    // Create a map of commands to functions
    std::unordered_map<std::string, void (*)()> commands;
    commands.emplace("set", &setFunc);
    commands.emplace("print", &printFunc);
    commands.emplace("maxdepth", &maxDepthFunc);
    commands.emplace("mindepth", &minDepthFunc);
    commands.emplace("same", &sameFunc);
    commands.emplace("symmetrical", &symFunc);
    commands.emplace("balances", &balancedFunc);
    commands.emplace("path", &hasPathFunc);
    commands.emplace("paths", &pathsFunc);
    commands.emplace("flatten", &flattenFunc);
    commands.emplace("preorder", &preorderFunc);
    commands.emplace("postorder", &postorderFunc);
    commands.emplace("levelorder", &levelorderFunc);
    commands.emplace("bst", &bstFunc);

    std::string input;
    do {
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

    // Wrap up
    std::cout << "\nExiting.\n";
    for (int t=0; t<10; t++) if (roots[t]) deleteTree(roots[t]);

    return 0;
}