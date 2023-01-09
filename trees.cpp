/* trees.cpp – A number of functions to do things with trees, built based on several LeetCode challenges.*/

#include <iostream>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>

/* First, some overarching things*/

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
bool deleteTree(TreeNode *root);
int maxDepth(TreeNode* root);
std::vector<TreeNode> nodesAtDepth(TreeNode *root, int depth);
std::vector<std::string> valsAtDepth(TreeNode *root, int depth);
bool printTree(TreeNode *root);

// A function to return a tree according to a list which reads node values from left to right, head to leaves.
// List values of INT_MIN stand in for gaps in the tree structure, i.e. [1, INT_MIN, 2] represents a tree with 
// head 1, and right leaf 2 but no left leaf; values of INT_MAX stand for empty space at the end of a row
TreeNode *listToRoot(int vals[], int len){ 
    if (len == 0) return NULL;
    if (len == 1) return new TreeNode(vals[0]);

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
                    // Work out how many nodes along this corresponds to
                    int k = 0;
                    for (int j=0; j<i*2; j++){
                        if (tree[depth+1][j] != INT_MIN && tree[depth+1][j] != INT_MAX)
                            k++;
                    }

                    leftptr = nodes[0][k];
                }
                if (tree[depth+1][i*2 + 1] != INT_MIN && tree[depth+1][i*2] != INT_MAX){
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

    if (nodes[0][0]) leftptr = nodes[0][0];
    if (nodes[0].size()>1 && nodes[0][1]) rightptr = nodes[0][1];
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

// Return a list of three-character node values, or " + " for non-existent nodes
// Used by printTree()
std::vector<std::string> valsAtDepth(TreeNode *root, int depth){
    std::vector<std::string> vals;
    std::vector<TreeNode> nodes = nodesAtDepth(root, depth);

    for (TreeNode node : nodes){
        std::string val;
        if (node.val == INT_MIN){
            val = " + ";
        } else{
            if (abs(node.val) < 10){
                val = " " + std::to_string(node.val) + " ";
            } else{
                val = std::to_string(node.val);
                while (val.size() < 3)
                    val += " ";
            }
        }
        
        vals.push_back(val);
    }

    return vals;
}

// Print an ascii visualisation of a tree, returning true if something was printed
/* The goals:
*            66
*      67          78
*   56    45    45    45
* 43 56 87 34 01 54 06 38
* 
*      77
*   55    44
* 76 45  90 12
*/
bool printTree(TreeNode *root){
    if (!root){
        std::cout << "Tree empty.\n";
        return false;
    }

    int depth = maxDepth(root);
    int maxWidth = (depth%2 == 0) ? (3*pow(2, depth-1)) : (3*pow(2, depth-1) + 1);

    // Print one row at a time
    for (int d=1; d<=depth; d++){
        std::vector<std::string> nodeVals = valsAtDepth(root, d-1);
        std::string line = "";

        for (int n=0; n<pow(2, d-1); n++){
            line += nodeVals[n];

            if (d < depth){
                // Supply the gaps between nodes
                if (n < pow(2, d-1)-1){
                    if (depth%2 == 0){
                        line += std::string(pow(3, depth-d), ' ');
                    } else{
                        line += std::string(pow(3, depth-d), ' ');
                    }
                }
            }
        }

        int padding = (maxWidth-line.size()) / 2;
        for (int i=0; i<padding; i++)
            std::cout << " ";
        std::cout << line << "\n\n";
    }

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

/* Now give the user an interface to use the above */
int main(int argc, char *argv[]){
    std::cout << "trees.cpp!\n\n";
    
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

    // Test the tree
    TreeNode *root = listToRoot(treeArray, sizeof(treeArray)/sizeof(treeArray[0]));
    std::cout << "\nTree formed:\n\n";
    printTree(root);
    std::cout << "Maximum depth: " << maxDepth(root) << "\n";
    deleteTree(root);

    return 0;
}