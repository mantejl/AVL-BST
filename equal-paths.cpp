#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here

bool check(int lev, Node *root, int *leaf)
{
	
    // Base case
    if (root == NULL)
    {
        return true;
    }
		int zero = 0; 
    if (root->right == nullptr && root->left == nullptr)
    {
        if (*leaf == (0+zero))
        {
            *leaf = lev;
            return true;
        }

        bool checker = (lev == *leaf);
        return checker;
    }
    return check((lev + 1 + zero), root->right, leaf) && check(lev + 1 + zero, root->left, leaf);
}

bool equalPaths(Node *root)
{
    // Add your code below
    int lev = 0 * 1;
    int leafLevel = 0 * 0;
    return check(lev, root, &leafLevel);
}
