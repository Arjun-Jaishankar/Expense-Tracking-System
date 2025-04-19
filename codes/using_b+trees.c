// Expense tracking system implementation using B+ Trees

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define MAX_USERS 1000
#define MAX_FAMILIES 100
#define MAX_EXPENSES 1000
#define MAX_FAMILY_MEMBERS 4
#define ORDER 5 // B+ tree order

// Enums
typedef enum {
    RENT,
    UTILITY, 
    GROCERY,
    STATIONARY,
    LEISURE
} ExpenseCategory;

typedef enum {FAILURE,SUCCESS} status_code;

typedef enum {False,True} Boolean;

// Structs
typedef struct Date {
    int day;
    int month; 
    int year;
} Date;

typedef struct User {
    int userID;
    char userName[50];
    float income;
} User;

typedef struct Family {
    int familyID;
    char familyName[50];
    int numMembers;
    int memberIDs[MAX_FAMILY_MEMBERS];
    float totalIncome;
    float totalMonthlyExpense;
} Family;

typedef struct Expense {
    int expenseID;
    int userID;
    ExpenseCategory category;
    float amount;
    Date date;
} Expense;

// B+ Tree Node Structures
typedef struct BPlusTreeNode {
    int *keys;
    void **pointers;
    struct BPlusTreeNode *parent;
    Boolean isLeaf;
    int numKeys;
    struct BPlusTreeNode *next;  // Next leaf node
    struct BPlusTreeNode *prev;  // Previous leaf node (new addition)
} BPlusTreeNode;

typedef struct BPlusTree {
    BPlusTreeNode *root;
    int order;
} BPlusTree;

// Global B+ Trees
BPlusTree *userTree = NULL;
BPlusTree *familyTree = NULL; 
BPlusTree *expenseTree = NULL;

// Function Declarations
BPlusTree* createBPlusTree(int order);
status_code addUser(User *user);
status_code addExpense(Expense *expense);
status_code createFamily(Family *family); 
status_code updateOrDeleteIndividualFamilyDetails(int id, Boolean isDelete);
status_code deleteUser(int userID); 
status_code updateDeleteExpense(int expenseID, Boolean isDelete);
float getTotalExpense(int familyID);
status_code getCategoricalExpense(int familyID, ExpenseCategory category);
Date getHighestExpenseDay(int familyID);
float getIndividualExpense(int userID);
status_code getExpenseInPeriod(Date start, Date end);
status_code getExpenseInRange(int expenseID1, int expenseID2, int individualID);

// Helper Functions
BPlusTreeNode* findLeaf(BPlusTree *tree, int key);
void insertIntoLeaf(BPlusTreeNode *leaf, int key, void *value);
void splitLeaf(BPlusTree *tree, BPlusTreeNode *leaf);
void deleteFromLeaf(BPlusTreeNode *leaf, int key);
void mergeLeaves(BPlusTree *tree, BPlusTreeNode *leaf1, BPlusTreeNode *leaf2);
User* findUser(int userID);
Family* findFamily(int familyID);
Family* findUsersFamily(int userID);

BPlusTreeNode* findLeaf(BPlusTree *tree, int key) {
    if (!tree || !tree->root) {
        printf("Error: Tree or root is null\n");
        return NULL;
    }

    BPlusTreeNode *node = tree->root;
    while (node) {
        if (node->isLeaf == True) {
            return node;
        }

        int i;
        for (i = 0; i < node->numKeys; i++) {
            if (key < node->keys[i]) {
                break;
            }
        }
        node = node->pointers[i];
    }

    return NULL;
}

void insertIntoLeaf(BPlusTreeNode *leaf, int key, void *value) {
    if (!leaf || !value) {
        printf("Error: NULL leaf or value in insertIntoLeaf\n");
        return;
    }
    
    /* Ensure we have space
    if (leaf->numKeys >= ORDER-1) {
        printf("Error: Leaf node is full\n");
        return;
    }
    */

    int insertPosition = 0;
    while (insertPosition < leaf->numKeys && leaf->keys[insertPosition] < key) {
        insertPosition++;
    }
    
    // Shift elements
    for (int i = leaf->numKeys; i > insertPosition; i--) {
        leaf->keys[i] = leaf->keys[i-1];
        leaf->pointers[i] = leaf->pointers[i-1];
    }
    
    // Insert new element
    leaf->keys[insertPosition] = key;
    leaf->pointers[insertPosition] = value;
    leaf->numKeys++;
}

void insertIntoParent(BPlusTree *tree, BPlusTreeNode *left, int key, BPlusTreeNode *right) {
    BPlusTreeNode *parent = left->parent;
    
    if (parent == NULL) {
        // Create new root
        BPlusTreeNode *newRoot = malloc(sizeof(BPlusTreeNode));
        newRoot->isLeaf = False;
        newRoot->numKeys = 1;
        newRoot->keys = malloc(sizeof(int) * tree->order);
        newRoot->pointers = malloc(sizeof(void*) * (tree->order + 1));
        newRoot->keys[0] = key;
        newRoot->pointers[0] = left;
        newRoot->pointers[1] = right;
        newRoot->parent = NULL;
        
        left->parent = newRoot;
        right->parent = newRoot;
        
        tree->root = newRoot;
        return;
    }
    
    // Find position to insert
    int pos = 0;
    while (pos < parent->numKeys && parent->keys[pos] < key) {
        pos++;
    }
    
    // Shift keys and pointers to make room
    for (int i = parent->numKeys; i > pos; i--) {
        parent->keys[i] = parent->keys[i-1];
    }
    for (int i = parent->numKeys + 1; i > pos + 1; i--) {
        parent->pointers[i] = parent->pointers[i-1];
    }
    
    // Insert new key and pointer
    parent->keys[pos] = key;
    parent->pointers[pos+1] = right;
    parent->numKeys++;
    
    // Check if parent needs to be split
    if (parent->numKeys == tree->order) {
        // Split the parent node
        BPlusTreeNode *newParent = malloc(sizeof(BPlusTreeNode));
        newParent->isLeaf = False;
        newParent->numKeys = 0;
        newParent->keys = malloc(sizeof(int) * tree->order);
        newParent->pointers = malloc(sizeof(void*) * (tree->order + 1));
        newParent->parent = parent->parent;
        
        // Move half of keys and pointers to new parent
        int splitPoint = parent->numKeys / 2;
        int promotedKey = parent->keys[splitPoint];
        
        for (int i = splitPoint + 1; i < parent->numKeys; i++) {
            newParent->keys[newParent->numKeys] = parent->keys[i];
            newParent->pointers[newParent->numKeys] = parent->pointers[i];
            ((BPlusTreeNode*)newParent->pointers[newParent->numKeys])->parent = newParent;
            newParent->numKeys++;
        }
        newParent->pointers[newParent->numKeys] = parent->pointers[parent->numKeys];
        ((BPlusTreeNode*)newParent->pointers[newParent->numKeys])->parent = newParent;
        
        // Update original parent
        parent->numKeys = splitPoint;
        
        // Recursively insert into grandparent
        insertIntoParent(tree, parent, promotedKey, newParent);
    }
}

void deleteFromParent(BPlusTree *tree, BPlusTreeNode *leftChild, BPlusTreeNode *rightChild) {
    BPlusTreeNode *parent = leftChild->parent;
    int deletePosition = 0;

    // Find position of rightChild pointer
    while (deletePosition <= parent->numKeys && 
           parent->pointers[deletePosition] != rightChild) {
        deletePosition++;
    }
    deletePosition--;  // Move to the key position

    // Shift keys and pointers left
    for (int i = deletePosition; i < parent->numKeys - 1; i++) {
        parent->keys[i] = parent->keys[i+1];
        parent->pointers[i+1] = parent->pointers[i+2];
    }
    parent->numKeys--;

    // If root is empty after deletion
    if (parent == tree->root && parent->numKeys == 0) {
        tree->root = leftChild;
        free(parent->keys);
        free(parent->pointers);
        free(parent);
        return;
    }

    // If node has too few entries
    int minKeys = (tree->order - 1) / 2;
    if (parent->numKeys < minKeys) {
        // Try to borrow from left sibling
        BPlusTreeNode *leftSibling = NULL;
        BPlusTreeNode *grandParent = parent->parent;
        int parentPosition = 0;
        
        while (parentPosition <= grandParent->numKeys && 
               grandParent->pointers[parentPosition] != parent) {
            parentPosition++;
        }
        
        if (parentPosition > 0) {
            leftSibling = grandParent->pointers[parentPosition - 1];
            if (leftSibling->numKeys > minKeys) {
                // Shift parent entries right
                for (int i = parent->numKeys; i > 0; i--) {
                    parent->keys[i] = parent->keys[i-1];
                    parent->pointers[i+1] = parent->pointers[i];
                }
                parent->pointers[1] = parent->pointers[0];
                
                // Move last key-pointer pair from left sibling
                parent->keys[0] = grandParent->keys[parentPosition - 1];
                parent->pointers[0] = leftSibling->pointers[leftSibling->numKeys];
                grandParent->keys[parentPosition - 1] = leftSibling->keys[leftSibling->numKeys - 1];
                
                leftSibling->numKeys--;
                parent->numKeys++;
                return;
            }
        }

        // Try to borrow from right sibling
        BPlusTreeNode *rightSibling = NULL;
        if (parentPosition < grandParent->numKeys) {
            rightSibling = grandParent->pointers[parentPosition + 1];
            if (rightSibling->numKeys > minKeys) {
                // Move first key-pointer pair from right sibling
                parent->keys[parent->numKeys] = grandParent->keys[parentPosition];
                parent->pointers[parent->numKeys + 1] = rightSibling->pointers[0];
                grandParent->keys[parentPosition] = rightSibling->keys[0];
                
                // Shift right sibling entries left
                for (int i = 0; i < rightSibling->numKeys - 1; i++) {
                    rightSibling->keys[i] = rightSibling->keys[i+1];
                    rightSibling->pointers[i] = rightSibling->pointers[i+1];
                }
                rightSibling->pointers[rightSibling->numKeys - 1] = 
                    rightSibling->pointers[rightSibling->numKeys];
                
                rightSibling->numKeys--;
                parent->numKeys++;
                return;
            }
        }

        // Merge with a sibling
        if (leftSibling != NULL) {
            // Merge with left sibling
            for (int i = 0; i < parent->numKeys; i++) {
                leftSibling->keys[leftSibling->numKeys + i] = parent->keys[i];
                leftSibling->pointers[leftSibling->numKeys + i + 1] = parent->pointers[i + 1];
            }
            leftSibling->numKeys += parent->numKeys;
            deleteFromParent(tree, leftSibling, parent);
            free(parent->keys);
            free(parent->pointers);
            free(parent);
        }
        else if (rightSibling != NULL) {
            // Merge with right sibling
            for (int i = 0; i < rightSibling->numKeys; i++) {
                parent->keys[parent->numKeys + i] = rightSibling->keys[i];
                parent->pointers[parent->numKeys + i + 1] = rightSibling->pointers[i + 1];
            }
            parent->numKeys += rightSibling->numKeys;
            deleteFromParent(tree, parent, rightSibling);
            free(rightSibling->keys);
            free(rightSibling->pointers);
            free(rightSibling);
        }
    }
}

void splitLeaf(BPlusTree *tree, BPlusTreeNode *leaf) {
    // Create new leaf
    BPlusTreeNode *newLeaf = malloc(sizeof(BPlusTreeNode));
    if (!newLeaf) {
        printf("Error: Failed to allocate new leaf node\n");
        return;
    }
    
    // Initialize new leaf
    newLeaf->keys = malloc(sizeof(int) * tree->order);
    newLeaf->pointers = malloc(sizeof(void*) * tree->order);
    newLeaf->isLeaf = True;
    newLeaf->numKeys = 0;
    newLeaf->parent = leaf->parent;
    newLeaf->next = leaf->next;
    newLeaf->prev = leaf;
    
    // Update original leaf's next pointer and next node's prev pointer
    if (leaf->next != NULL) {
        leaf->next->prev = newLeaf;
    }
    leaf->next = newLeaf;
    
    // Calculate split point (move half the keys to new leaf)
    int splitPoint = (leaf->numKeys + 1) / 2;  // More balanced split
    
    // Move keys and pointers to new leaf
    for (int i = splitPoint; i < leaf->numKeys; i++) {
        newLeaf->keys[newLeaf->numKeys] = leaf->keys[i];
        newLeaf->pointers[newLeaf->numKeys] = leaf->pointers[i];
        newLeaf->numKeys++;
    }
    
    // Update original leaf's count
    leaf->numKeys = splitPoint;
    
    // Insert the new node into the parent
    if (leaf->parent == NULL) {
        // Create new root
        BPlusTreeNode *newRoot = malloc(sizeof(BPlusTreeNode));
        if (!newRoot) {
            printf("Error: Failed to allocate new root node\n");
            return;
        }
        
        newRoot->keys = malloc(sizeof(int) * tree->order);
        newRoot->pointers = malloc(sizeof(void*) * (tree->order + 1));
        newRoot->isLeaf = False;
        newRoot->numKeys = 1;
        newRoot->parent = NULL;
        
        newRoot->keys[0] = newLeaf->keys[0];  // First key of new leaf
        newRoot->pointers[0] = leaf;
        newRoot->pointers[1] = newLeaf;
        
        leaf->parent = newRoot;
        newLeaf->parent = newRoot;
        
        tree->root = newRoot;
    } else {
        // Insert into existing parent
        insertIntoParent(tree, leaf, newLeaf->keys[0], newLeaf);
    }
}

void deleteFromLeaf(BPlusTreeNode *leaf, int key) {
    int deletePosition = 0;
    
    // Find key to delete
    while (deletePosition < leaf->numKeys && leaf->keys[deletePosition] != key) {
        deletePosition++;
    }
    
    // Shift keys and pointers left
    for (int i = deletePosition; i < leaf->numKeys - 1; i++) {
        leaf->keys[i] = leaf->keys[i+1];
        leaf->pointers[i] = leaf->pointers[i+1];
    }
    
    leaf->numKeys--;
}

void mergeLeaves(BPlusTree *tree, BPlusTreeNode *leaf1, BPlusTreeNode *leaf2) {
    // Copy keys and pointers from leaf2 to leaf1
    for (int i = 0; i < leaf2->numKeys; i++) {
        leaf1->keys[leaf1->numKeys + i] = leaf2->keys[i];
        leaf1->pointers[leaf1->numKeys + i] = leaf2->pointers[i];
    }
    
    leaf1->numKeys += leaf2->numKeys;
    
    // Update next pointer and prev pointer of next node
    leaf1->next = leaf2->next;
    if (leaf2->next != NULL) {
        leaf2->next->prev = leaf1;
    }
    
    // Delete leaf2 from parent
    deleteFromParent(tree, leaf1, leaf2);
    
    // Free leaf2
    free(leaf2->keys);
    free(leaf2->pointers);
    free(leaf2);
}

BPlusTree* createBPlusTree(int order) {
    BPlusTree *tree = malloc(sizeof(BPlusTree));
    if (!tree) return NULL;

    tree->order = order;
    tree->root = malloc(sizeof(BPlusTreeNode));
    if (!tree->root) {
        free(tree);
        return NULL;
    }

    // Initialize root node properly
    tree->root->keys = malloc(order * sizeof(int));
    tree->root->pointers = malloc((order + 1) * sizeof(void*));
    if (!tree->root->keys || !tree->root->pointers) {
        free(tree->root->keys);
        free(tree->root->pointers);
        free(tree->root);
        free(tree);
        return NULL;
    }

    tree->root->isLeaf = True;
    tree->root->numKeys = 0;
    tree->root->parent = NULL;
    tree->root->next = NULL;
    tree->root->prev = NULL;
    // Initialize all pointers to NULL
    for (int i = 0; i < order + 1; i++) {
        tree->root->pointers[i] = NULL;
    }

    return tree;
}

User* findUser(int userID) {
    BPlusTreeNode *leaf = findLeaf(userTree, userID);
    if (!leaf) return NULL;  // Add this check
    
    for (int i = 0; i < leaf->numKeys; i++) {
        if (leaf->keys[i] == userID) {
            return (User*)leaf->pointers[i];
        }
    }
    return NULL;
}

Family* findFamily(int familyID) {
    BPlusTreeNode *leaf = findLeaf(familyTree, familyID);
    if (!leaf) return NULL;  // Add this check
    
    for (int i = 0; i < leaf->numKeys; i++) {
        if (leaf->keys[i] == familyID) {
            return (Family*)leaf->pointers[i];
        }
    }
    return NULL;
}

Family* findUsersFamily(int userID) {
    if (!familyTree || !familyTree->root) return NULL;  // Add this check
    
    BPlusTreeNode *leaf = familyTree->root;
    // Go to leftmost leaf
    while (!leaf->isLeaf) {
        leaf = leaf->pointers[0];
    }
    
    // Traverse all leaf nodes
    while (leaf != NULL) {
        for (int i = 0; i < leaf->numKeys; i++) {
            Family *family = (Family*)leaf->pointers[i];
            for (int j = 0; j < family->numMembers; j++) {
                if (family->memberIDs[j] == userID) {
                    return family;
                }
            }
        }
        leaf = leaf->next;
    }
    return NULL;
}

status_code addUser(User *user) {
    if (!userTree) {
        printf("Error: User tree not initialized\n");
        return FAILURE;
    }

    if (!user) {
        printf("Error: Null user pointer\n");
        return FAILURE;
    }

    // First check if user already exists
    BPlusTreeNode *leaf = findLeaf(userTree, user->userID);
    if (!leaf) {
        printf("Error: Failed to find leaf node\n");
        return FAILURE;
    }

    for (int i = 0; i < leaf->numKeys; i++) {
        if (leaf->keys[i] == user->userID) {
            printf("User ID %d already exists\n", user->userID);
            return FAILURE;
        }
    }

    // Now proceed with getting other details
    printf("Enter User Name: ");
    scanf("%49s", user->userName);
    printf("Enter Income: ");
    scanf("%f", &user->income);

    // Ask about family assignment
    printf("Assign to family:\n");
    printf("1. Existing family\n");
    printf("2. Create new family\n");
    int familyChoice;
    scanf("%d", &familyChoice);
    
    if (familyChoice == 1) {
        // Add to existing family
        printf("Enter Family ID: ");
        int familyID;
        scanf("%d", &familyID);
        
        Family *family = findFamily(familyID);
        if (family == NULL) {
            printf("Family not found\n");
            return FAILURE;
        }
        
        if (family->numMembers >= MAX_FAMILY_MEMBERS) {
            printf("Family already has maximum members\n");
            return FAILURE;
        }
        
        // Check if user already belongs to another family
        Family *existingFamily = findUsersFamily(user->userID);
        if (existingFamily != NULL && existingFamily->familyID != familyID) {
            printf("User with ID %d already belongs to family %d (%s)\n", 
                   user->userID, existingFamily->familyID, existingFamily->familyName);
            return FAILURE;
        }
        
        // Add user to family
        family->memberIDs[family->numMembers++] = user->userID;
        family->totalIncome += user->income;
        
        // Update family in tree
        updateOrDeleteIndividualFamilyDetails(familyID, False);
    }
    else if (familyChoice == 2) {
        // Create new family with this user
        Family newFamily;
        printf("Enter new Family ID: ");
        scanf("%d", &newFamily.familyID);
        printf("Enter Family Name: ");
        scanf("%49s", newFamily.familyName);
        
        newFamily.numMembers = 1;
        newFamily.memberIDs[0] = user->userID;
        newFamily.totalIncome = user->income;
        newFamily.totalMonthlyExpense = 0.0;
        
        createFamily(&newFamily);
    }                

    User *newUser = malloc(sizeof(User));
    if (!newUser) {
        printf("Error: Memory allocation failed\n");
        return FAILURE;
    }
    *newUser = *user;

    if (leaf->numKeys < userTree->order - 1) {
        insertIntoLeaf(leaf, user->userID, newUser);
    } else {
        insertIntoLeaf(leaf, user->userID, newUser);
        // Check if we need to split
        if (leaf->numKeys == userTree->order) {
            splitLeaf(userTree, leaf);
        }
    }

    printf("Added user %d successfully\n", user->userID);
    return SUCCESS;
}

status_code addExpense(Expense *expense) {
    status_code sc = SUCCESS;
    // Validate date (10-day month)
    if (expense->date.day < 1 || expense->date.day > 10) {
        printf("Error: Day must be between 1 and 10\n");
        return FAILURE;
    }
    // Find leaf node where expense should be inserted
    BPlusTreeNode *leaf = findLeaf(expenseTree, expense->expenseID);
    if (!leaf) {
        printf("Error: Could not find leaf node\n");
        return FAILURE;
    }
    
    // Check if expense already exists
    for (int i = 0; i < leaf->numKeys; i++) {
        if (leaf->keys[i] == expense->expenseID) {
            printf("Expense with ID %d already exists\n", expense->expenseID);
            return FAILURE;
        }
    }
    
    // Create copy of expense data
    Expense *newExpense = malloc(sizeof(Expense));
    *newExpense = *expense;
    
    // If leaf has space, insert directly
    if (leaf->numKeys < expenseTree->order - 1) {
        insertIntoLeaf(leaf, expense->expenseID, newExpense);
    }
    // Otherwise split leaf
    else {
        insertIntoLeaf(leaf, expense->expenseID, newExpense);
    	// Check if we need to split
    	if (leaf->numKeys == userTree->order) {
            splitLeaf(userTree, leaf);
    	}
    }
    
    // Update family's total expense if user exists
    Family *family = findUsersFamily(expense->userID);
    if (family != NULL) {
        family->totalMonthlyExpense += expense->amount;
    }
    
    return sc;
}

status_code createFamily(Family *family) {
    status_code sc = SUCCESS;
    
    // Validate family member count
    if (family->numMembers < 1 || family->numMembers > MAX_FAMILY_MEMBERS) {
        printf("Invalid number of family members (%d). Must be between 1 and %d\n", 
               family->numMembers, MAX_FAMILY_MEMBERS);
        return FAILURE;
    }
    
    // Verify all member users exist and don't belong to other families
    float totalIncome = 0.0;
    for (int i = 0; i < family->numMembers; i++) {
        User *user = findUser(family->memberIDs[i]);
        if (user == NULL) {
            printf("User with ID %d does not exist\n", family->memberIDs[i]);
            return FAILURE;
        }
        
        // Check if user already belongs to another family
        Family *existingFamily = findUsersFamily(family->memberIDs[i]);
        if (existingFamily != NULL && existingFamily->familyID != family->familyID) {
            printf("User with ID %d already belongs to family %d (%s)\n", 
                   family->memberIDs[i], existingFamily->familyID, existingFamily->familyName);
            return FAILURE;
        }
        
        totalIncome += user->income;
    }
    
    // Check if family ID already exists (for updates)
    Family *existingFamilyWithSameID = findFamily(family->familyID);
    if (existingFamilyWithSameID != NULL) {
        // This is an update to existing family
        // Remove old family members from their families (if they're being removed)
        for (int i = 0; i < existingFamilyWithSameID->numMembers; i++) {
            Boolean memberStillPresent = False;
            for (int j = 0; j < family->numMembers; j++) {
                if (existingFamilyWithSameID->memberIDs[i] == family->memberIDs[j]) {
                    memberStillPresent = True;
                    break;
                }
            }
            if (!memberStillPresent) {
                // Member is being removed - no action needed as we're replacing the family
            }
        }
    }
    
    // Create copy of family data
    Family *newFamily = malloc(sizeof(Family));
    if (!newFamily) {
        printf("Memory allocation failed for new family\n");
        return FAILURE;
    }
    *newFamily = *family;
    newFamily->totalIncome = totalIncome;
    newFamily->totalMonthlyExpense = 0.0;  // Initialize expense
    
    // Find leaf node where family should be inserted
    BPlusTreeNode *leaf = findLeaf(familyTree, family->familyID);
    if (!leaf) {
        printf("Error: Could not find leaf node\n");
        free(newFamily);
        return FAILURE;
    }
    
    // If family exists, delete the old version first
    if (existingFamilyWithSameID != NULL) {
        deleteFromLeaf(leaf, family->familyID);
        free(existingFamilyWithSameID);
    }
    
    // Insert into B+ tree
    if (leaf->numKeys < familyTree->order - 1) {
        insertIntoLeaf(leaf, family->familyID, newFamily);
    } else {
        insertIntoLeaf(leaf, family->familyID, newFamily);
        splitLeaf(familyTree, leaf);
    }
    
    printf("Family %d created/updated successfully with %d members\n", 
           family->familyID, family->numMembers);
    return sc;
}

status_code updateOrDeleteIndividualFamilyDetails(int id, Boolean isDelete) {
    status_code sc = SUCCESS;
    
    // Find leaf node containing the family
    BPlusTreeNode *leaf = findLeaf(familyTree, id);
    if (leaf == NULL) {
        printf("Family with ID %d not found\n", id);
        return FAILURE;
    }

    // Find family in leaf node
    int pos = 0;
    while (pos < leaf->numKeys && leaf->keys[pos] != id) {
        pos++;
    }

    if (pos == leaf->numKeys) {
        printf("Family with ID %d not found\n", id);
        return FAILURE;
    }

    Family *family = (Family*)leaf->pointers[pos];

    if (isDelete) {
        printf("Deleting family %d and all its members\n", id);
        
        // First delete all members of this family
        for (int i = 0; i < family->numMembers; i++) {
            int memberID = family->memberIDs[i];
            
            // Delete the user and all their expenses
            status_code delStatus = deleteUser(memberID);
            if (delStatus != SUCCESS) {
                printf("Failed to delete user %d from family %d\n", memberID, id);
                sc = FAILURE;
            }
        }
        
        // Then delete the family itself
        deleteFromLeaf(leaf, id);
        free(family);
        
        // Handle B+ tree underflow if needed
        int minKeys = (familyTree->order - 1) / 2;
        if (leaf->numKeys < minKeys && leaf->parent != NULL) {
                BPlusTreeNode *parent = leaf->parent;
                int leafPos = 0;
                
                // Find our position in parent's pointers
                while (leafPos <= parent->numKeys && parent->pointers[leafPos] != leaf) {
                    leafPos++;
                }

                // ===== Borrow from Left Sibling =====
                if (leafPos > 0) {
                    BPlusTreeNode *leftSibling = parent->pointers[leafPos - 1];
                    if (leftSibling->numKeys > minKeys) {
                        // Make space at beginning of leaf
                        for (int i = leaf->numKeys; i > 0; i--) {
                            leaf->keys[i] = leaf->keys[i-1];
                            leaf->pointers[i] = leaf->pointers[i-1];
                        }
                        
                        // Move last element from left sibling
                        leaf->keys[0] = leftSibling->keys[leftSibling->numKeys - 1];
                        leaf->pointers[0] = leftSibling->pointers[leftSibling->numKeys - 1];
                        leaf->numKeys++;
                        
                        // Update parent's key
                        parent->keys[leafPos - 1] = leaf->keys[0];
                        
                        // Remove from left sibling
                        leftSibling->numKeys--;
                        return SUCCESS;
                    }
                }

                // ===== Borrow from Right Sibling =====
                if (leafPos < parent->numKeys) {
                    BPlusTreeNode *rightSibling = parent->pointers[leafPos + 1];
                    if (rightSibling->numKeys > minKeys) {
                        // Take first element from right sibling
                        leaf->keys[leaf->numKeys] = rightSibling->keys[0];
                        leaf->pointers[leaf->numKeys] = rightSibling->pointers[0];
                        leaf->numKeys++;
                        
                        // Shift elements in right sibling
                        for (int i = 0; i < rightSibling->numKeys - 1; i++) {
                            rightSibling->keys[i] = rightSibling->keys[i+1];
                            rightSibling->pointers[i] = rightSibling->pointers[i+1];
                        }
                        rightSibling->numKeys--;
                        
                        // Update parent's key
                        parent->keys[leafPos] = rightSibling->keys[0];
                        return SUCCESS;
                    }
                }

                // ===== Merge with Sibling =====
                if (leafPos > 0) {  // Merge with left sibling
                    BPlusTreeNode *leftSibling = parent->pointers[leafPos - 1];
                    
                    // Copy all elements to left sibling
                    for (int i = 0; i < leaf->numKeys; i++) {
                        leftSibling->keys[leftSibling->numKeys + i] = leaf->keys[i];
                        leftSibling->pointers[leftSibling->numKeys + i] = leaf->pointers[i];
                    }
                    leftSibling->numKeys += leaf->numKeys;
                    leftSibling->next = leaf->next;
                    
                    if (leaf->next) {
                        leaf->next->prev = leftSibling;
                    }
                    
                    // Delete from parent
                    deleteFromParent(familyTree, leftSibling, leaf);
                    
                    // Free the empty leaf
                    free(leaf->keys);
                    free(leaf->pointers);
                    free(leaf);
                } 
                else if (leafPos < parent->numKeys) {  // Merge with right sibling
                    BPlusTreeNode *rightSibling = parent->pointers[leafPos + 1];
                    
                    // Copy all elements from right sibling
                    for (int i = 0; i < rightSibling->numKeys; i++) {
                        leaf->keys[leaf->numKeys + i] = rightSibling->keys[i];
                        leaf->pointers[leaf->numKeys + i] = rightSibling->pointers[i];
                    }
                    leaf->numKeys += rightSibling->numKeys;
                    leaf->next = rightSibling->next;
                    
                    if (rightSibling->next) {
                        rightSibling->next->prev = leaf;
                    }
                    
                    // Delete from parent
                    deleteFromParent(familyTree, leaf, rightSibling);
                    
                    // Free the empty right sibling
                    free(rightSibling->keys);
                    free(rightSibling->pointers);
                    free(rightSibling);
                }
          }
        
    } 
    else {
        // Update family details
        printf("Enter new family name: ");
        scanf("%49s", family->familyName);

        printf("Update members? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        
        if (choice == 'y' || choice == 'Y') {
            printf("Enter number of members (1-4): ");
            int newNumMembers;
            if (scanf("%d", &newNumMembers) != 1 || 
                newNumMembers < 1 || newNumMembers > MAX_FAMILY_MEMBERS) {
                printf("Invalid number of members\n");
                return FAILURE;
            }
            
            float newTotalIncome = 0.0;
            printf("Enter member IDs:\n");
            for (int i = 0; i < newNumMembers; i++) {
                printf("Member %d ID: ", i+1);
                if (scanf("%d", &family->memberIDs[i]) != 1) {
                    printf("Invalid member ID\n");
                    return FAILURE;
                }
                
                User *user = findUser(family->memberIDs[i]);
                if (user == NULL) {
                    printf("User with ID %d not found\n", family->memberIDs[i]);
                    return FAILURE;
                }
                newTotalIncome += user->income;
            }
            family->numMembers = newNumMembers;
            family->totalIncome = newTotalIncome;
        }
    }
    
    return sc;
}

status_code updateUser(int userID) {
    status_code sc = SUCCESS;
    
    // Find the user
    User *user = findUser(userID);
    if (user == NULL) {
        printf("User with ID %d not found\n", userID);
        return FAILURE;
    }

    // Get new details
    printf("Enter new user name: ");
    char newName[50];
    scanf("%49s", newName);
    
    printf("Enter new income: ");
    float newIncome;
    if (scanf("%f", &newIncome) != 1) {
        printf("Invalid income entered\n");
        return FAILURE;
    }

    // Find all families containing this user
    BPlusTreeNode *familyLeaf = familyTree->root;
    while (!familyLeaf->isLeaf) {
        familyLeaf = familyLeaf->pointers[0];
    }

    float incomeDifference = newIncome - user->income;
    
    // Update user details
    strcpy(user->userName, newName);
    user->income = newIncome;

    // Update all families containing this user
    while (familyLeaf != NULL) {
        for (int i = 0; i < familyLeaf->numKeys; i++) {
            Family *family = (Family*)familyLeaf->pointers[i];
            for (int j = 0; j < family->numMembers; j++) {
                if (family->memberIDs[j] == userID) {
                    family->totalIncome += incomeDifference;
                    break;
                }
            }
        }
        familyLeaf = familyLeaf->next;
    }

    printf("User %d updated successfully\n", userID);
    return sc;
}

status_code deleteUser(int userID) {
    status_code sc = SUCCESS;
    
    // Find the user's leaf node
    BPlusTreeNode *leaf = findLeaf(userTree, userID);
    if (leaf == NULL) {
        printf("User with ID %d not found\n", userID);
        return FAILURE;
    }

    // Verify user exists in leaf
    int pos = 0;
    while (pos < leaf->numKeys && leaf->keys[pos] != userID) {
        pos++;
    }

    if (pos == leaf->numKeys) {
        printf("User with ID %d not found\n", userID);
        return FAILURE;
    }

    User *user = (User*)leaf->pointers[pos];
    
    // Delete all expenses by this user first
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; ) {
            Expense *exp = (Expense*)expenseLeaf->pointers[i];
            if (exp->userID == userID) {
                // Delete expense and update family totals if needed
                Family *family = findUsersFamily(userID);
                if (family != NULL) {
                    family->totalMonthlyExpense -= exp->amount;
                }
                
                deleteFromLeaf(expenseLeaf, exp->expenseID);
                free(exp);
                // After deletion, keys shift left so we don't increment i
            } else {
                i++;
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    // Remove user from any families they belong to
    Family *family = findUsersFamily(userID);
    if (family != NULL) {
        // Remove user from family
        int newMemberCount = 0;
        for (int j = 0; j < family->numMembers; j++) {
            if (family->memberIDs[j] != userID) {
                family->memberIDs[newMemberCount++] = family->memberIDs[j];
            }
        }
        family->numMembers = newMemberCount;
        family->totalIncome -= user->income;
        
        // If this was the last member, delete the family
        if (family->numMembers == 0) {
            updateOrDeleteIndividualFamilyDetails(family->familyID, True);
        }
    }

    // Finally delete the user
    deleteFromLeaf(leaf, userID);
    free(user);

    printf("User %d deleted successfully\n", userID);
    return SUCCESS;
}

status_code updateDeleteExpense(int expenseID, Boolean isDelete) {
    status_code sc = SUCCESS;
    // Find leaf node containing the expense
    BPlusTreeNode *leaf = findLeaf(expenseTree, expenseID);

    if (leaf == NULL) {
        printf("Expense with ID %d not found\n", expenseID);
        return FAILURE;
    }

    // Find expense in leaf node
    int pos = 0;
    while (pos < leaf->numKeys && leaf->keys[pos] != expenseID) {
        pos++;
    }

    if (pos == leaf->numKeys) {
        printf("Expense with ID %d not found\n", expenseID);
        return FAILURE;
    }

    Expense *expense = (Expense*)leaf->pointers[pos];

    if (isDelete) {
        // First update the family's total expenses
        Family *family = findUsersFamily(expense->userID);
        if (family != NULL) {
            family->totalMonthlyExpense -= expense->amount;
            printf("Updated family %d total expense by -%.2f\n", 
                   family->familyID, expense->amount);
        }

        // Then delete the expense
        deleteFromLeaf(leaf, expenseID);
        free(expense);
    	// After deleting from leaf, check if we need to update prev pointers
    	if (leaf->next != NULL && leaf->next->prev == leaf) {
            leaf->next->prev = leaf->prev;
    	}
    	if (leaf->prev != NULL && leaf->prev->next == leaf) {
            leaf->prev->next = leaf->next;
    	}
        // Handle B+ tree underflow if needed
        int minKeys = (expenseTree->order - 1) / 2;
        if (leaf->numKeys < minKeys) {
            BPlusTreeNode *parent = leaf->parent;
            if (parent != NULL) {
                int leafPos = 0;
                while (leafPos <= parent->numKeys && parent->pointers[leafPos] != leaf) {
                    leafPos++;
                }

                // Try borrowing from left sibling
                if (leafPos > 0) {
                    BPlusTreeNode *leftSibling = parent->pointers[leafPos - 1];
                    if (leftSibling->numKeys > minKeys) {
                        // Shift entries right
                        for (int i = leaf->numKeys; i > 0; i--) {
                            leaf->keys[i] = leaf->keys[i-1];
                            leaf->pointers[i] = leaf->pointers[i-1];
                        }

                        // Copy last entry from left sibling
                        leaf->keys[0] = leftSibling->keys[leftSibling->numKeys - 1];
                        leaf->pointers[0] = leftSibling->pointers[leftSibling->numKeys - 1];
                        leftSibling->numKeys--;
                        leaf->numKeys++;

                        // Update parent key
                        parent->keys[leafPos - 1] = leaf->keys[0];
                        return SUCCESS;
                    }
                }

                // Try borrowing from right sibling
                if (leafPos < parent->numKeys) {
                    BPlusTreeNode *rightSibling = parent->pointers[leafPos + 1];
                    if (rightSibling->numKeys > minKeys) {
                        // Copy first entry from right sibling
                        leaf->keys[leaf->numKeys] = rightSibling->keys[0];
                        leaf->pointers[leaf->numKeys] = rightSibling->pointers[0];
                        leaf->numKeys++;

                        // Shift right sibling entries left
                        for (int i = 0; i < rightSibling->numKeys - 1; i++) {
                            rightSibling->keys[i] = rightSibling->keys[i+1];
                            rightSibling->pointers[i] = rightSibling->pointers[i+1];
                        }
                        rightSibling->numKeys--;

                        // Update parent key
                        parent->keys[leafPos] = rightSibling->keys[0];
                        return SUCCESS;
                    }
                }

                // Merge with a sibling if can't borrow
                if (leafPos > 0) {
                    BPlusTreeNode *leftSibling = parent->pointers[leafPos - 1];
                    mergeLeaves(expenseTree, leftSibling, leaf);
                } else {
                    BPlusTreeNode *rightSibling = parent->pointers[leafPos + 1];
                    mergeLeaves(expenseTree, leaf, rightSibling);
                }
            }
        }
    } else {
        // Update expense details
        float oldAmount = expense->amount;
        int oldUserID = expense->userID;
        
        printf("Enter new expense amount: ");
        if (scanf("%f", &expense->amount) != 1 || expense->amount < 0) {
            printf("Invalid amount entered\n");
            return FAILURE;
        }

        printf("Enter expense category (RENT/UTILITY/GROCERY/STATIONARY/LEISURE): ");
        char categoryStr[20];
        scanf("%19s", categoryStr);
        
        // Convert category string to enum (case-insensitive)
        if (strcasecmp(categoryStr, "RENT") == 0) {
            expense->category = RENT;
        } else if (strcasecmp(categoryStr, "UTILITY") == 0) {
            expense->category = UTILITY;
        } else if (strcasecmp(categoryStr, "GROCERY") == 0) {
            expense->category = GROCERY;
        } else if (strcasecmp(categoryStr, "STATIONARY") == 0) {
            expense->category = STATIONARY;
        } else if (strcasecmp(categoryStr, "LEISURE") == 0) {
            expense->category = LEISURE;
        } else {
            printf("Invalid category entered\n");
            return FAILURE;
        }

        printf("Enter expense date (DD MM YYYY): ");
        if (scanf("%d %d %d", &expense->date.day, &expense->date.month, &expense->date.year) != 3 ||
            expense->date.month < 1 || expense->date.month > 12 ||
            expense->date.day < 1 || expense->date.day > 10) {
            printf("Invalid date format, day must be 1 to 10\n");
            return FAILURE;
        }

        // Update family's total expenses if amount or user changed
        if (oldAmount != expense->amount || oldUserID != expense->userID) {
            // Remove from old family if user changed
            if (oldUserID != expense->userID) {
                Family *oldFamily = findUsersFamily(oldUserID);
                if (oldFamily != NULL) {
                    oldFamily->totalMonthlyExpense -= oldAmount;
                }
            }
            
            // Add to new family
            Family *family = findUsersFamily(expense->userID);
            if (family != NULL) {
                family->totalMonthlyExpense += (expense->amount - (oldUserID == expense->userID ? oldAmount : 0));
            }
        }
    }
    
    return sc;
}

float getTotalExpense(int familyID) {
    float total = 0.0;
    
    // Find the family
    Family *family = findFamily(familyID);
    if (family == NULL) {
        printf("Family with ID %d not found\n", familyID);
        return total;
    }

    // Traverse expense tree to find expenses for this family
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    
    // Go to leftmost leaf
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Traverse all leaf nodes
    while (expenseLeaf != NULL) {
        // Check each expense in current leaf
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            // Check if expense belongs to a family member
            for (int j = 0; j < family->numMembers; j++) {
                if (expense->userID == family->memberIDs[j]) {
                    total += expense->amount;
                    break;
                }
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    return total;
}

status_code getCategoricalExpense(int familyID, ExpenseCategory category) {
    status_code sc = SUCCESS;
    float total = 0.0;
    int numMembersWithExpenses = 0;
    
    // Array to store individual contributions (userID, amount)
    typedef struct {
        int userID;
        float amount;
    } UserContribution;
    
    UserContribution contributions[MAX_FAMILY_MEMBERS] = {0};
    
    // Find the family
    Family *family = findFamily(familyID);
    if (family == NULL) {
        printf("Family with ID %d not found\n", familyID);
        return FAILURE;
    }

    // Initialize contributions array with family members
    for (int i = 0; i < family->numMembers; i++) {
        contributions[i].userID = family->memberIDs[i];
        contributions[i].amount = 0.0;
    }

    // Traverse expense tree
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Collect all matching expenses
    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            
            // Check if expense belongs to this family and category
            for (int j = 0; j < family->numMembers; j++) {
                if (expense->userID == family->memberIDs[j] && 
                    expense->category == category) {
                    
                    total += expense->amount;
                    contributions[j].amount += expense->amount;
                    break;
                }
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    // Sort contributions in descending order
    for (int i = 0; i < family->numMembers - 1; i++) {
        for (int j = i + 1; j < family->numMembers; j++) {
            if (contributions[i].amount < contributions[j].amount) {
                UserContribution temp = contributions[i];
                contributions[i] = contributions[j];
                contributions[j] = temp;
            }
        }
    }

    // Print results
    printf("\nCategory: ");
    switch(category) {
        case RENT: printf("Rent"); break;
        case UTILITY: printf("Utility"); break;
        case GROCERY: printf("Grocery"); break;
        case STATIONARY: printf("Stationary"); break;
        case LEISURE: printf("Leisure"); break;
        default: printf("Unknown"); break;
    }
    printf("\nTotal Family Expense: %.2f\n", total);
    
    printf("\nIndividual Contributions (sorted):\n");
    for (int i = 0; i < family->numMembers; i++) {
        User *user = findUser(contributions[i].userID);
        if (user && contributions[i].amount > 0) {
            printf("- %s (ID: %d): %.2f\n", 
                   user->userName, 
                   user->userID, 
                   contributions[i].amount);
            numMembersWithExpenses++;
        }
    }

    if (numMembersWithExpenses == 0) {
        printf("No expenses found in this category\n");
        sc = FAILURE;
    }

    return sc;
}

Date getHighestExpenseDay(int familyID) {
    Date maxDate = {0, 0, 0}; // Initialize empty date
    float maxAmount = 0.0;
    
    // Find the family
    Family *family = findFamily(familyID);
    if (family == NULL) {
        printf("Family with ID %d not found\n", familyID);
        return maxDate;
    }

    // Traverse expense tree
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Track highest expense
    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            
            // Check if expense belongs to a family member
            for (int j = 0; j < family->numMembers; j++) {
                if (expense->userID == family->memberIDs[j]) {
                    if (expense->amount > maxAmount) {
                        maxAmount = expense->amount;
                        maxDate = expense->date;
                    }
                    break;
                }
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    return maxDate;
}

float getIndividualExpense(int userID) {
    float total = 0.0;
    
    // Find the user
    User *user = findUser(userID);
    if (user == NULL) {
        printf("User with ID %d not found\n", userID);
        return total;
    }

    // Traverse expense tree
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Sum expenses for this user
    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            if (expense->userID == userID) {
                total += expense->amount;
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    return total;
}

// Helper function to compare dates
int compareDates(Date date1, Date date2) {
    if (date1.year != date2.year) {
        return date1.year < date2.year ? -1 : 1;
    }
    if (date1.month != date2.month) {
        return date1.month < date2.month ? -1 : 1;
    }
    if (date1.day != date2.day) {
        return date1.day < date2.day ? -1 : 1;
    }
    return 0;
}

const char* getCategoryName(ExpenseCategory category) {
    switch(category) {
        case RENT: return "Rent";
        case UTILITY: return "Utility";
        case GROCERY: return "Grocery";
        case STATIONARY: return "Stationary";
        case LEISURE: return "Leisure";
        default: return "Unknown";
    }
}

status_code getExpenseInPeriod(Date start, Date end) {
    status_code sc = SUCCESS;
    float total = 0.0;
    int expenseCount = 0;
    // Validate input dates
    if (start.day < 1 || start.day > 10 || end.day < 1 || end.day > 10) {
        printf("Error: Day must be between 1 and 10\n");
        return FAILURE;
    }
    // Validate date range
    if (compareDates(start, end) > 0) {
        printf("Error: Start date must be before end date\n");
        return FAILURE;
    }

    // Traverse expense tree
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    
    // Go to leftmost leaf
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Print header
    printf("\nExpenses between %02d/%02d/%04d and %02d/%02d/%04d:\n",
           start.day, start.month, start.year,
           end.day, end.month, end.year);
    printf("------------------------------------------------\n");

    // Traverse all leaf nodes
    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            
            // Check if date is within range
            if (compareDates(expense->date, start) >= 0 && 
                compareDates(expense->date, end) <= 0) {
                
                // Print expense details
                printf("[ID: %d] %02d/%02d/%04d - %s: %.2f (User: %d)\n",
                       expense->expenseID,
                       expense->date.day, expense->date.month, expense->date.year,
                       getCategoryName(expense->category),
                       expense->amount,
                       expense->userID);
                
                total += expense->amount;
                expenseCount++;
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    // Print summary
    printf("\nSummary:\n");
    printf("Total expenses: %d\n", expenseCount);
    printf("Total amount: %.2f\n", total);
    
    if (expenseCount == 0) {
        printf("No expenses found in this period\n");
        sc = FAILURE;
    }

    return sc;
}

status_code getExpenseInRange(int expenseID1, int expenseID2, int individualID) {
    status_code sc = FAILURE; // Default to failure (no expenses found)
    float total = 0.0;
    int expenseCount = 0;
    
    // Validate input range
    if (expenseID1 > expenseID2) {
        printf("Error: Starting expense ID (%d) must be less than or equal to ending expense ID (%d)\n", 
               expenseID1, expenseID2);
        return FAILURE;
    }

    // Check if individual exists (if ID is provided)
    if (individualID != 0 && findUser(individualID) == NULL) {
        printf("Error: Individual with ID %d not found\n", individualID);
        return FAILURE;
    }

    // Traverse expense tree
    BPlusTreeNode *expenseLeaf = expenseTree->root;
    
    // Go to leftmost leaf
    while (!expenseLeaf->isLeaf) {
        expenseLeaf = expenseLeaf->pointers[0];
    }

    // Print header
    printf("\nExpenses in range %d to %d", expenseID1, expenseID2);
    if (individualID != 0) {
        printf(" for individual %d", individualID);
    }
    printf(":\n");
    printf("--------------------------------------------\n");

    // Traverse all leaf nodes
    while (expenseLeaf != NULL) {
        for (int i = 0; i < expenseLeaf->numKeys; i++) {
            Expense *expense = (Expense*)expenseLeaf->pointers[i];
            
            // Check if expense is within range and matches individual (if specified)
            if (expense->expenseID >= expenseID1 && 
                expense->expenseID <= expenseID2 &&
                (individualID == 0 || expense->userID == individualID)) {
                
                // Get user details for display
                User *user = findUser(expense->userID);
                const char *userName = user ? user->userName : "Unknown";
                
                // Print detailed expense information
                printf("[ID: %d] %02d/%02d/%04d - %s: %.2f (User: %d - %s)\n",
                       expense->expenseID,
                       expense->date.day, expense->date.month, expense->date.year,
                       getCategoryName(expense->category),
                       expense->amount,
                       expense->userID,
                       userName);
                
                total += expense->amount;
                expenseCount++;
                sc = SUCCESS; // At least one expense found
            }
        }
        expenseLeaf = expenseLeaf->next;
    }

    // Print summary
    printf("\nSummary:\n");
    printf("Total expenses found: %d\n", expenseCount);
    printf("Total amount: %.2f\n", total);
    
    if (expenseCount == 0) {
        printf("No expenses found in the specified range");
        if (individualID != 0) {
            printf(" for individual %d", individualID);
        }
        printf("\n");
    }

    return sc;
}

// File handling functions
void saveIndividualsToFile(BPlusTree *tree, FILE *fp) {
    if (tree == NULL || tree->root == NULL || fp == NULL) {
        return;
    }

    // Start from the leftmost leaf node
    BPlusTreeNode *current = tree->root;
    while (!current->isLeaf) {
        current = current->pointers[0];
    }

    // Traverse through all leaf nodes
    while (current != NULL) {
        // Save all keys and records in this leaf node
        for (int i = 0; i < current->numKeys; i++) {
            User *user = (User*)current->pointers[i];
            fprintf(fp, "%d %s %.2f\n", 
                   user->userID, 
                   user->userName, 
                   user->income);
        }
        current = current->next;
    }
}

void saveFamiliesToFile(BPlusTree *tree, FILE *fp) {
    if (tree == NULL || tree->root == NULL || fp == NULL) {
        return;
    }

    // Start from the leftmost leaf node
    BPlusTreeNode *current = tree->root;
    while (!current->isLeaf) {
        current = current->pointers[0];
    }

    // Traverse through all leaf nodes
    while (current != NULL) {
        // Save all family records in this leaf node
        for (int i = 0; i < current->numKeys; i++) {
            Family *family = (Family*)current->pointers[i];
            
            // Write family header info
            fprintf(fp, "%d %s %d %.2f %.2f", 
                   family->familyID, 
                   family->familyName,
                   family->numMembers, 
                   family->totalIncome,
                   family->totalMonthlyExpense);
            
            // Write member IDs
            for (int j = 0; j < family->numMembers; j++) {
                fprintf(fp, " %d", family->memberIDs[j]);
            }
            fprintf(fp, "\n");
        }
        current = current->next;
    }
}

void saveExpensesToFile(BPlusTree *tree, FILE *fp) {
    if (tree == NULL || tree->root == NULL || fp == NULL) {
        return;
    }

    // Start from the leftmost leaf node
    BPlusTreeNode *current = tree->root;
    while (!current->isLeaf) {
        current = current->pointers[0];
    }

    // Traverse through all leaf nodes
    while (current != NULL) {
        // Save all expense records in this leaf node
        for (int i = 0; i < current->numKeys; i++) {
            Expense *expense = (Expense*)current->pointers[i];
            
            // Convert category enum to string
            const char *categoryStr;
            switch(expense->category) {
                case RENT: categoryStr = "RENT"; break;
                case UTILITY: categoryStr = "UTILITY"; break;
                case GROCERY: categoryStr = "GROCERY"; break;
                case STATIONARY: categoryStr = "STATIONARY"; break;
                case LEISURE: categoryStr = "LEISURE"; break;
                default: categoryStr = "UNKNOWN"; break;
            }

            // Write expense record
            fprintf(fp, "%d %d %d %d %d %s %.2f\n", 
                   expense->expenseID,
                   expense->userID,
                   expense->date.day,
                   expense->date.month,
                   expense->date.year,
                   categoryStr,
                   expense->amount);
        }
        current = current->next;
    }
}

BPlusTree* loadIndividualsFromFile(FILE *fp) {
    if (!fp) {
        printf("Error: File pointer is NULL\n");
        return NULL;
    }

    BPlusTree *tree = createBPlusTree(ORDER);
    if (!tree) {
        printf("Error: Failed to create B+ tree\n");
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Skip empty lines or lines that are too short
        if (strlen(line) < 5) continue;

        User *user = malloc(sizeof(User));
        if (!user) {
            printf("Warning: Failed to allocate user\n");
            continue;
        }

        // Parse space-separated line
        if (sscanf(line, "%d %49s %f", 
                  &user->userID, user->userName, &user->income) != 3) {
            printf("Warning: Failed to parse line: %s", line);
            free(user);
            continue;
        }

        BPlusTreeNode *leaf = findLeaf(tree, user->userID);
        if (!leaf) {
            printf("Error: Failed to find leaf for user %d\n", user->userID);
            free(user);
            continue;
        }
        insertIntoLeaf(leaf, user->userID, user);
        if (leaf->numKeys == ORDER-1) {
            splitLeaf(tree, leaf);
        }
    }

    return tree;
}

BPlusTree* loadFamiliesFromFile(FILE *fp) {
    if (!fp) {
        printf("Error: File pointer is NULL\n");
        return NULL;
    }

    BPlusTree *tree = createBPlusTree(ORDER);
    if (!tree) {
        printf("Error: Failed to create B+ tree\n");
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Skip empty lines
        if (strlen(line) < 5) continue;

        Family *newFamily = malloc(sizeof(Family));
        if (!newFamily) {
            printf("Warning: Failed to allocate family\n");
            continue;
        }

        // Parse space-separated line
        char *ptr = line;
        int items_parsed = sscanf(ptr, "%d %49s %d %f %f",
                                 &newFamily->familyID,
                                 newFamily->familyName,
                                 &newFamily->numMembers,
                                 &newFamily->totalIncome,
                                 &newFamily->totalMonthlyExpense);
        
        if (items_parsed != 5) {
            printf("Warning: Failed to parse family header: %s", line);
            free(newFamily);
            continue;
        }

        // Parse member IDs
        ptr = strchr(ptr, ' '); // Skip familyID
        for (int i = 0; i < 4; i++) ptr = strchr(ptr + 1, ' '); // Skip next 4 fields
        
        for (int i = 0; i < newFamily->numMembers; i++) {
            if (ptr) {
                newFamily->memberIDs[i] = atoi(ptr);
                ptr = strchr(ptr + 1, ' ');
            } else {
                newFamily->memberIDs[i] = -1; // Invalid ID
            }
        }

        // Insert into B+ tree
        BPlusTreeNode *leaf = findLeaf(tree, newFamily->familyID);
        if (!leaf) {
            printf("Error: Failed to find leaf for family %d\n", newFamily->familyID);
            free(newFamily);
            continue;
        }

        // Check for duplicate familyID
        Boolean duplicate = False;
        for (int i = 0; i < leaf->numKeys; i++) {
            if (leaf->keys[i] == newFamily->familyID) {
                duplicate = True;
                free(newFamily);
                printf("Duplicate family ID %d found, skipping\n", newFamily->familyID);
                break;
            }
        }

        if (!duplicate) {
            insertIntoLeaf(leaf, newFamily->familyID, newFamily);
            if (leaf->numKeys == tree->order - 1) {
                splitLeaf(tree, leaf);
            }
        }
    }

    return tree;
}

BPlusTree* loadExpensesFromFile(FILE *fp) {
    if (!fp) {
        printf("Error: File pointer is NULL\n");
        return NULL;
    }

    BPlusTree *tree = createBPlusTree(ORDER);
    if (!tree) {
        printf("Error: Failed to create B+ tree\n");
        return NULL;
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        // Skip empty lines
        if (strlen(line) < 5) continue;

        Expense *newExpense = malloc(sizeof(Expense));
        if (!newExpense) {
            printf("Warning: Failed to allocate expense\n");
            continue;
        }

        char categoryStr[20];
        // Parse space-separated line
        if (sscanf(line, "%d %d %d %d %d %19s %f",
                  &newExpense->expenseID,
                  &newExpense->userID,
                  &newExpense->date.day,
                  &newExpense->date.month,
                  &newExpense->date.year,
                  categoryStr,
                  &newExpense->amount) != 7) {
            printf("Warning: Failed to parse expense: %s", line);
            free(newExpense);
            continue;
        }

        // Validate date (10-day month)
        if (newExpense->date.day < 1 || newExpense->date.day > 10) {
            printf("Warning: Invalid day %d in expense %d (must be 1-10), skipping\n",
                   newExpense->date.day, newExpense->expenseID);
            free(newExpense);
            continue;
        }

        // Convert category string to enum
        if (strcmp(categoryStr, "RENT") == 0) newExpense->category = RENT;
        else if (strcmp(categoryStr, "UTILITY") == 0) newExpense->category = UTILITY;
        else if (strcmp(categoryStr, "GROCERY") == 0) newExpense->category = GROCERY;
        else if (strcmp(categoryStr, "STATIONARY") == 0) newExpense->category = STATIONARY;
        else if (strcmp(categoryStr, "LEISURE") == 0) newExpense->category = LEISURE;
        else newExpense->category = RENT; // Default category

        // Insert into B+ tree
        BPlusTreeNode *leaf = findLeaf(tree, newExpense->expenseID);
        if (!leaf) {
            printf("Error: Failed to find leaf for expense %d\n", newExpense->expenseID);
            free(newExpense);
            continue;
        }

        // Check for duplicate expenseID
        Boolean duplicate = False;
        for (int i = 0; i < leaf->numKeys; i++) {
            if (leaf->keys[i] == newExpense->expenseID) {
                duplicate = True;
                free(newExpense);
                printf("Duplicate expense ID %d found, skipping\n", newExpense->expenseID);
                break;
            }
        }

        if (!duplicate) {
            insertIntoLeaf(leaf, newExpense->expenseID, newExpense);
            if (leaf->numKeys == tree->order - 1) {
                splitLeaf(tree, leaf);
            }
        }
    }

    return tree;
}

// Traversal function for display
void traverseAndPrint(BPlusTree *tree, void (*printFunc)(void*)) {
    if (!tree || !tree->root) return;
    
    // Go to leftmost leaf
    BPlusTreeNode *current = tree->root;
    while (!current->isLeaf) {
        current = current->pointers[0];
    }
    
    // Forward traversal
    printf("Forward traversal:\n");
    while (current) {
        for (int i = 0; i < current->numKeys; i++) {
            printFunc(current->pointers[i]);
        }
        current = current->next;
    }
    
    // Go to rightmost leaf
    current = tree->root;
    while (!current->isLeaf) {
        current = current->pointers[current->numKeys];
    }
    while (current->next) {
        current = current->next;
    }
    
    // Backward traversal
    printf("\nBackward traversal:\n");
    while (current) {
        for (int i = current->numKeys - 1; i >= 0; i--) {
            printFunc(current->pointers[i]);
        }
        current = current->prev;
    }
}

// Print functions for each type
void printUser(void *data) {
    User *user = (User*)data;
    printf("ID: %d, Name: %s, Income: %.2f\n", 
           user->userID, user->userName, user->income);
}

void printFamily(void *data) {
    Family *fam = (Family*)data;
    printf("ID: %d, Name: %s, Members: %d, Income: %.2f, Expenses: %.2f\n",
           fam->familyID, fam->familyName, fam->numMembers, fam->totalIncome, fam->totalMonthlyExpense);
    printf("Member IDs: ");
    for (int i = 0; i < fam->numMembers; i++) {
        printf("%d ", fam->memberIDs[i]);
    }
    printf("\n");
}

void printExpense(void *data) {
    Expense *exp = (Expense*)data;
    printf("ID: %d, User: %d, Date: %02d/%02d/%04d, Category: %s, Amount: %.2f\n",
           exp->expenseID, exp->userID, exp->date.day, exp->date.month, 
           exp->date.year, getCategoryName(exp->category), exp->amount);
}

// Helper function to recursively free B+ tree nodes
void freeBPlusTreeNode(BPlusTreeNode *node, void (*freeData)(void*)) {
    if (node == NULL) return;
    
    // If not a leaf, recursively free children
    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; i++) {
            freeBPlusTreeNode(node->pointers[i], freeData);
        }
    }
    // If leaf, free data items
    else {
        for (int i = 0; i < node->numKeys; i++) {
            if (freeData != NULL) {
                freeData(node->pointers[i]);
            }
        }
    }
    
    // Free node's arrays and the node itself
    free(node->keys);
    free(node->pointers);
    free(node);
}

// Function to free user data
void freeUserData(void *data) {
    if (data != NULL) {
        free((User*)data);
    }
}

// Function to free family data
void freeFamilyData(void *data) {
    if (data != NULL) {
        free((Family*)data);
    }
}

// Function to free expense data
void freeExpenseData(void *data) {
    if (data != NULL) {
        free((Expense*)data);
    }
}

// Main function to destroy entire B+ tree
void destroyBPlusTree(BPlusTree *tree, void (*freeData)(void*)) {
    if (tree == NULL) return;
    
    // Free all nodes recursively
    if (tree->root != NULL) {
        freeBPlusTreeNode(tree->root, freeData);
    }
    
    // Free the tree structure itself
    free(tree);
}

int main() {
    // Initialize trees
    userTree = createBPlusTree(ORDER);
    familyTree = createBPlusTree(ORDER);
    expenseTree = createBPlusTree(ORDER);
    
    if (!userTree || !familyTree || !expenseTree) {
        printf("Error: Failed to initialize B+ trees\n");
        return 1;
    }

    // Load data
    FILE *indFile = fopen("individuals.txt", "r");
    if (indFile) {
        printf("Debug: Loading individuals...\n");
        BPlusTree *newUserTree = loadIndividualsFromFile(indFile);
        if (newUserTree) {
            destroyBPlusTree(userTree, freeUserData);
            userTree = newUserTree;
            printf("Debug: Loaded %d users\n", userTree->root->numKeys);
        } else {
            printf("Warning: Failed to load individuals\n");
        }
        fclose(indFile);
    } else {
        printf("Warning: Could not open individuals.txt\n");
    }

    FILE *famFile = fopen("families.txt", "r");
    if (famFile != NULL) {
        BPlusTree *newFamilyTree = loadFamiliesFromFile(famFile);
        if (newFamilyTree) {
            destroyBPlusTree(familyTree, freeFamilyData);
            familyTree = newFamilyTree;
        }
        fclose(famFile);
    }

    FILE *expFile = fopen("expenses.txt", "r");
    if (expFile != NULL) {
        BPlusTree *newExpenseTree = loadExpensesFromFile(expFile);
        if (newExpenseTree) {
            destroyBPlusTree(expenseTree, freeExpenseData);
            expenseTree = newExpenseTree;
        }
        fclose(expFile);
    }

    int choice;
    int running = 1;

    while (running) {
        printf("\nExpense Tracking System Menu:\n");
        printf("1. Add a User\n");
        printf("2. Add an Expense\n");
        printf("3. Update Individual Details\n");
        printf("4. Delete Individual\n");
        printf("5. Update Family Details\n");
        printf("6. Delete Family\n");
        printf("7. Update Expense\n");
        printf("8. Delete Expense\n");
        printf("9. Get Highest Expense Day for Family\n");
        printf("10. Get Total Family Expense\n");
        printf("11. Get Categorical Expense\n");
        printf("12. Get Individual Expense\n");
        printf("13. Get Expenses in Period\n");
        printf("14. Get Expenses in Range\n");
        printf("15. Display User Database\n");
        printf("16. Display Family Database\n");
        printf("17. Display Expense Database\n");
        printf("18. Exit\n");
        printf("Enter your choice (1-18): ");
        scanf("%d", &choice);

        switch(choice) {
            case 1: {
                // Add individual implementation
                User newUser;
                printf("Enter User ID: ");
                scanf("%d", &newUser.userID);
    		// First check if user exists
    		BPlusTreeNode *leaf = findLeaf(userTree, newUser.userID);
    		if (!leaf) {
        	    printf("Error: Failed to access user database\n");
        	    break;
    		}
    
    		// Check for duplicate first
    		if (findUser(newUser.userID) != NULL) {
        	    printf("Error: User ID %d already exists\n", newUser.userID);
        	    break; // Exit immediately
    		}

                printf("Enter User Name: ");
                scanf("%49s", newUser.userName);
                printf("Enter Income: ");
                scanf("%f", &newUser.income);
        	// Ask about family assignment
        	printf("Assign to family:\n");
        	printf("1. Existing family\n");
        	printf("2. Create new family\n");
        	int familyChoice;
        	scanf("%d", &familyChoice);
        
        	if (familyChoice == 1) {
            	    // Add to existing family
            	    printf("Enter Family ID: ");
            	    int familyID;
            	    scanf("%d", &familyID);
            
            	    // Find family and add user
            	    Family *family = findFamily(familyID);
            	    if (family == NULL) {
                	printf("Family not found\n");
                	break;
            	    }
            
            	    if (family->numMembers >= MAX_FAMILY_MEMBERS) {
                	printf("Family already has maximum members\n");
                	break;
            	    }
            
            	    // Add user to family
            	    family->memberIDs[family->numMembers++] = newUser.userID;
            	    family->totalIncome += newUser.income;
            
            	    // Update family in tree
            	    updateOrDeleteIndividualFamilyDetails(familyID, False);
        	}
        	else if (familyChoice == 2) {
            	    // Create new family with this user
            	    Family newFamily;
            	    printf("Enter new Family ID: ");
            	    scanf("%d", &newFamily.familyID);
            	    printf("Enter Family Name: ");
            	    scanf("%49s", newFamily.familyName);
            
            	    newFamily.numMembers = 1;
            	    newFamily.memberIDs[0] = newUser.userID;
            	    newFamily.totalIncome = newUser.income;
            	    newFamily.totalMonthlyExpense = 0.0;
            
            	    createFamily(&newFamily);
        	}                

                status_code result = addUser(&newUser);
                if (result == SUCCESS) {
                    printf("User added successfully!\n");
                } else {
                    printf("Failed to add user.\n");
                }
                break;
            }
            case 2: {
                // Add expense implementation
                Expense newExpense;
                char categoryStr[20];
                
                printf("Enter Expense ID: ");
                scanf("%d", &newExpense.expenseID);
                printf("Enter User ID: ");
                scanf("%d", &newExpense.userID);
                printf("Enter Date (day month year): ");
                scanf("%d %d %d", &newExpense.date.day, &newExpense.date.month, &newExpense.date.year);
                printf("Enter Category (RENT/UTILITY/GROCERY/STATIONARY/LEISURE): ");
                scanf("%19s", categoryStr);
                printf("Enter Amount: ");
                scanf("%f", &newExpense.amount);
                
                // Convert category string to enum
                if (strcmp(categoryStr, "RENT") == 0) newExpense.category = RENT;
                else if (strcmp(categoryStr, "UTILITY") == 0) newExpense.category = UTILITY;
                else if (strcmp(categoryStr, "GROCERY") == 0) newExpense.category = GROCERY;
                else if (strcmp(categoryStr, "STATIONARY") == 0) newExpense.category = STATIONARY;
                else if (strcmp(categoryStr, "LEISURE") == 0) newExpense.category = LEISURE;
                else newExpense.category = RENT; // Default
                
                status_code result = addExpense(&newExpense);
                if (result == SUCCESS) {
                    printf("Expense added successfully!\n");
                } else {
                    printf("Failed to add expense.\n");
                }
                break;
            }
            case 3: {
    		int userID;
    		printf("Enter User ID to update: ");
    		if (scanf("%d", &userID) != 1) {
        	    printf("Invalid user ID\n");
        	    break;
    		}
    		status_code result = updateUser(userID);
    		if (result != SUCCESS) {
        	    printf("Failed to update user\n");
    		}
    		break;
	    }
	    case 4: {
    		int userID;
    		printf("Enter User ID to delete: ");
    		if (scanf("%d", &userID) != 1) {
        	    printf("Invalid user ID\n");
        	    break;
    		}
    		status_code result = deleteUser(userID);
    		if (result != SUCCESS) {
        	    printf("Failed to delete user\n");
    		}
    		break;
	    }
            case 5: {
                int familyID;
                printf("Enter Family ID to update: ");
                scanf("%d", &familyID);
                status_code result = updateOrDeleteIndividualFamilyDetails(familyID, False);
                if (result == SUCCESS) {
                    printf("Family updated successfully!\n");
                } else {
                    printf("Failed to update family.\n");
                }
                break;
            }
            case 6: {
                int familyID;
                printf("Enter Family ID to delete: ");
                scanf("%d", &familyID);
                status_code result = updateOrDeleteIndividualFamilyDetails(familyID, True);
                if (result == SUCCESS) {
                    printf("Family deleted successfully!\n");
                } else {
                    printf("Failed to delete family.\n");
                }
                break;
            }
            case 7: {
                int expenseID;
                printf("Enter Expense ID to update: ");
                scanf("%d", &expenseID);
                status_code result = updateDeleteExpense(expenseID, False);
                if (result == SUCCESS) {
                    printf("Expense updated successfully!\n");
                } else {
                    printf("Failed to update expense.\n");
                }
                break;
            }
            case 8: {
                int expenseID;
                printf("Enter Expense ID to delete: ");
                scanf("%d", &expenseID);
                status_code result = updateDeleteExpense(expenseID, True);
                if (result == SUCCESS) {
                    printf("Expense deleted successfully!\n");
                } else {
                    printf("Failed to delete expense.\n");
                }
                break;
            }
            case 9: {
                int familyID;
                printf("Enter Family ID: ");
                scanf("%d", &familyID);
                Date highDay = getHighestExpenseDay(familyID);
                if (highDay.day == 0 && highDay.month == 0 && highDay.year == 0) {
                    printf("No expenses found for this family.\n");
                } else {
                    printf("Highest expense day: %02d/%02d/%04d\n", 
                           highDay.day, highDay.month, highDay.year);
                }
                break;
            }
            case 10: {
                int familyID;
                printf("Enter Family ID: ");
                scanf("%d", &familyID);
                float total = getTotalExpense(familyID);
                Family *family = findFamily(familyID);
                if (family == NULL) {
                    printf("Family not found.\n");
                } else {
                    printf("Total family expense: %.2f\n", total);
                    printf("Family income: %.2f\n", family->totalIncome);
                    if (total > family->totalIncome) {
                        printf("Warning: Expenses (%.2f) exceed income (%.2f) by %.2f\n",
                               total, family->totalIncome, total - family->totalIncome);
                    } else {
                        printf("Remaining budget: %.2f\n", family->totalIncome - total);
                    }
                }
                break;
            }
            case 11: {
                int familyID;
                char categoryStr[20];
                printf("Enter Family ID: ");
                scanf("%d", &familyID);
                printf("Enter Category (RENT/UTILITY/GROCERY/STATIONARY/LEISURE): ");
                scanf("%19s", categoryStr);
                
                ExpenseCategory category;
                if (strcmp(categoryStr, "RENT") == 0) category = RENT;
                else if (strcmp(categoryStr, "UTILITY") == 0) category = UTILITY;
                else if (strcmp(categoryStr, "GROCERY") == 0) category = GROCERY;
                else if (strcmp(categoryStr, "STATIONARY") == 0) category = STATIONARY;
                else if (strcmp(categoryStr, "LEISURE") == 0) category = LEISURE;
                else {
                    printf("Invalid category.\n");
                    break;
                }
                
                status_code result = getCategoricalExpense(familyID, category);
                if (result != SUCCESS) {
                    printf("Failed to get categorical expenses.\n");
                }
                break;
            }
            case 12: {
                int userID;
                printf("Enter User ID: ");
                scanf("%d", &userID);
                float total = getIndividualExpense(userID);
                User *user = findUser(userID);
                if (user == NULL) {
                    printf("User not found.\n");
                } else {
                    printf("Total individual expense for %s (ID: %d): %.2f\n",
                           user->userName, user->userID, total);
                }
                break;
            }
            case 13: {
                Date start, end;
                printf("Enter start date (day month year): ");
                scanf("%d %d %d", &start.day, &start.month, &start.year);
                printf("Enter end date (day month year): ");
                scanf("%d %d %d", &end.day, &end.month, &end.year);
                status_code result = getExpenseInPeriod(start, end);
                if (result != SUCCESS) {
                    printf("No expenses found in this period.\n");
                }
                break;
            }
            case 14: {
                int expID1, expID2, userID;
                printf("Enter start Expense ID: ");
                scanf("%d", &expID1);
                printf("Enter end Expense ID: ");
                scanf("%d", &expID2);
                printf("Enter User ID (0 for all users): ");
                scanf("%d", &userID);
                status_code result = getExpenseInRange(expID1, expID2, userID);
                if (result != SUCCESS) {
                    printf("No expenses found in this range.\n");
                }
                break;
            }
            case 15: {
                printf("\nUser Database:\n");
                printf("--------------\n");
                traverseAndPrint(userTree, printUser);
                break;
            }
            case 16: {
                printf("\nFamily Database:\n");
                printf("----------------\n");
                traverseAndPrint(familyTree, printFamily);
                break;
            }
            case 17: {
                printf("\nExpense Database:\n");
                printf("-----------------\n");
                traverseAndPrint(expenseTree, printExpense);
                break;
            }
            case 18: {
                // Save data to files
                FILE *outInd = fopen("individuals.txt", "w");
                saveIndividualsToFile(userTree, outInd);
                fclose(outInd);

                FILE *outFam = fopen("families.txt", "w");
                saveFamiliesToFile(familyTree, outFam);
                fclose(outFam);

                FILE *outExp = fopen("expenses.txt", "w");
                saveExpensesToFile(expenseTree, outExp);
                fclose(outExp);

                running = 0;
                printf("Data saved. Exiting...\n");
                break;
            }
            default:
                printf("Invalid choice! Please enter a number between 1 and 18.\n");
        }
    }
    
    // Clean up
    destroyBPlusTree(userTree, freeUserData);
    destroyBPlusTree(familyTree, freeFamilyData);
    destroyBPlusTree(expenseTree, freeExpenseData);
    
    return 0;
}
