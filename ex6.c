/******************
Name: Roei Wald
ID: 211589791
Assignment: ex4
*******************/
#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
#define POKEMONS_NUM 151
// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================
int main()
{
    mainMenu();
    freeAllOwners();
    return 0;
}
// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
            case 1:
                // Add new owner to the linking list
                openPokedexMenu();
            break;
            case 2:
                // enter to a pokedex for farther action
                enterExistingPokedexMenu();
            break;
            case 3:
                deletePokedex();
            break;
            case 4:
                mergePokedexMenu();
            break;
            case 5:
                sortOwners();
            break;
            case 6:
                printOwnersCircular();
            break;
            case 7:
                printf("Goodbye!\n");
            break;
            default:
                printf("Invalid.\n");
        }
    } while (choice != 7);
}

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove mj b leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char *temp = (char *)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    // if the node is empty
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
// open a menu for how to display the pokemons binary tree
void displayMenu(OwnerNode *owner)
{
    // if there is no pokemon in the pokedex
    if (owner->pokedexRoot == NULL)
    {
        printf("Pokedex is empty.\n");
        return;
    }
    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");
    // get the choice using readIntSafe func
    int choice = readIntSafe("Your choice: ");
    switch (choice)
    {
        case 1:
            displayBFS(owner->pokedexRoot);
        break;
        case 2:
            preOrderTraversal(owner->pokedexRoot);
        break;
        case 3:
            inOrderTraversal(owner->pokedexRoot);
        break;
        case 4:
            postOrderTraversal(owner->pokedexRoot);
        break;
        case 5:
            displayAlphabetical(owner->pokedexRoot);
        break;
        default:
            printf("Invalid choice.\n");
    }
}
// Generic funcs that runs on the BST by getting func pointer
// Generic pre-order func
void preOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    visit(root);
    preOrderGeneric(root->left, visit);
    preOrderGeneric(root->right, visit);
}
// Generic in-order func
void inOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    inOrderGeneric(root->left, visit);
    visit(root);
    inOrderGeneric(root->right, visit);
}
// Generic post-order func
void postOrderGeneric(PokemonNode *root, VisitNodeFunc visit) {
    if (root == NULL)
        return;
    postOrderGeneric(root->left, visit);
    postOrderGeneric(root->right, visit);
    visit(root);
}
// Func that print the pokedex post-order
void postOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Using the Generic func and give it the printPokemonNode func
    postOrderGeneric(root, printPokemonNode);
}
// Func that print the pokedex in-order
void inOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Using the Generic func and give it the printPokemonNode func
    inOrderGeneric(root, printPokemonNode);
}
// Func that print the pokedex pre-order
void preOrderTraversal(PokemonNode *root) {
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Using the Generic func and give it the printPokemonNode func
    preOrderGeneric(root, printPokemonNode);
}
// Recursive Func that collect the pokemon's pointers in the pokedex to the array
void collectAll(PokemonNode *root, NodeArray *na) {
    // Stop condition - root is get to the end
    if (root == NULL)
        return;
    // realloce new memory for every added pokemon pointer
    na->nodes = realloc(na->nodes, (na->size+1) * sizeof(PokemonNode*));
    // If memory allocation failed
    if (!na->nodes) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // add the root (the pokemon pointer) to the array in the current location (size)
    na->nodes[na->size] = root;
    // Increment the size
    na->size++;
    // reucursive call to add the rest of the BST
    collectAll(root->left, na);
    collectAll(root->right, na);
}
// The comper func to the qsort func (needed casting)
int compareByNameNode(const void *a, const void *b) {
    const PokemonNode *nodeA = *(const PokemonNode**)a;
    const PokemonNode *nodeB = *(const PokemonNode**)b;
    return strcmp(nodeA->data->name, nodeB->data->name);
}
// display Alphabetical
void displayAlphabetical(PokemonNode *root) {
    // if pokedex is empty
    if (root == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // initialize a node array struct
    NodeArray *na = (NodeArray *)malloc(sizeof(NodeArray));
    // If memory allocatioin failed
    if (!na) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    na->nodes = NULL;
    na->size = 0;
    // Collect all the pokemon pointers to the array
    collectAll(root, na);
    // Apply Qsort func to sort the pokedex alphabetical
    qsort(na->nodes, na->size, sizeof(PokemonNode*), compareByNameNode);
    // print the sorted array
    for (int i = 0; i < na->size; i++)
        printPokemonNode(na->nodes[i]);
    // free the memory that allocated
    free(na->nodes);
    free(na);
}
// Using queue linking list to print BFS
// Create the queue
Queue *createQueue() {
    // allocate memory
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    // If allocation failed
    if (!queue) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // Initialize the queue pointers to NULL
    queue->front = NULL;
    queue->rear = NULL;
    // Return the queue address
    return queue;
}
//n Create a node in the queue and attach a pokemon pointer to the node
Node *createNode(PokemonNode *pokemon) {
    // Allocate memory
    Node *nodeInQueue = (Node *)malloc(sizeof(Node));
    // If allocation failed
    if (!nodeInQueue) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // Attach the pokemon pointer to the node
    nodeInQueue->pokemon = pokemon;
    nodeInQueue->next = NULL;
    return nodeInQueue;
}
// Check if the queue is empty
int isEmptyQueue(Queue *queue) {
    // if the front location is pointing to NULL - its empty
    return (queue->front == NULL);
}
// Add node to the queue
void enQueue(Queue *queue, PokemonNode *pokemon) {
    // create a node
    Node *nodeInQueue = createNode(pokemon);
    // if the queue is empty initialize both front and rear to point to the new node
    if (queue->rear == NULL) {
        queue->front = nodeInQueue;
        queue->rear = nodeInQueue;
        return;
    }
    // if not empty - put the new node in the end
    queue->rear->next = nodeInQueue;
    queue->rear = nodeInQueue;
}
// get the first node in the queue out
PokemonNode* deQueue(Queue *queue) {
    // if the queue is empty
    if (isEmptyQueue(queue))
        return NULL;
    // get the first node
    Node *nodeInQueue = queue->front;
    // make the next node be the first one
    queue->front = queue->front->next;
    // If we get out the last nose
    if (queue->front == NULL)
        queue->rear = NULL;
    // save the data so we can free the memory
    PokemonNode *pokemon = nodeInQueue->pokemon;
    free(nodeInQueue);
    return pokemon;
}
// Print in BFS
void displayBFS(PokemonNode *root) {
    // If the pokedex is enpty
    if (root == NULL){
        printf("Pokedex is empty.\n");
        return;
    }
    // Create a queue
    Queue *queue = createQueue();
    // attach the root to the queue
    enQueue(queue, root);
    // as long as the queue us not empty - keep printing getting the nodes out
    while (!isEmptyQueue(queue)) {
        PokemonNode *current = deQueue(queue);
        printPokemonNode(current);
        // add the left children to the queue
        if (current->left != NULL)
            enQueue(queue, current->left);
        // add the right children to the queue
        if (current->right != NULL)
            enQueue(queue, current->right);
    }
    // free the queue memory allocation
    free(queue);
}


// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
// Print the exisiting owners
OwnerNode* printExistingPokadex (OwnerNode *head) {
    // Create itarator that will run on the linking list - starting from the head
    OwnerNode *iterator = head;
    // Create counter that will be the number of the owner in the list
    int counter = 1;
    // Print the owner name and the number, then nove the iterator to the next owner and increment the counter
    do {
        printf("%d. %s\n", counter, iterator->ownerName);
        counter++;
        iterator = iterator->next;
    }
    // Print until complete a circle
    while (iterator != head);
    // Now choose an owner by his number
    int ownerNum = readIntSafe("Choose a Pokedex by number: ");
    // mvoe the iterator to the choosen owner
    for (int i = 0; i < ownerNum-1; i++)
        iterator = iterator->next;
    printf("\nEntering %s's Pokedex...\n", iterator->ownerName);
    // return the chosen owner
    return iterator;
}
// Insert the pokemon to his location in the binary tree by ID
PokemonNode *insertPokemonNode(PokemonNode *root, PokemonNode *newNode) {
    // if the root id greater than the new pokemon - left
    if (root->data->id > newNode->data->id) {
        // if the left position is empty - insert there
        if (root->left == NULL) {
            root->left = newNode;

        }
        // if the left children is already occupied - move on to the left children and now check for him (recursivlly)
        else {
            // if this pokemon is exist in the next
            if (insertPokemonNode(root->left, newNode) == NULL) {
                return NULL;
            }
        }
    }
    // if the new pokemon is greater than the root - right (amd same implementation as the left)
    else if (root->data->id < newNode->data->id) {
        // If the right spot is avilable
        if (root->right == NULL) {
            root->right = newNode;
        }
        else{
            // if this pokemon is exist in the next
            if (insertPokemonNode(root->right, newNode) == NULL)
                return NULL;
        }
    }
    // if this pokemon is already exist
    else {
        return NULL;
    }
    return newNode;
}
// Add pokemon to the pokedex of the picked owner
void addPokemon(OwnerNode *owner) {
    // chose which pokemon to add by ID
    int choice = readIntSafe("Enter ID to add: ");
    // Adjust the ID of the pokemon to his location in the array
    if (choice > POKEMONS_NUM || choice < 1) {
        printf("Invalid ID.\n");
        return;
    }
    choice--;
    // Create pokemon node
    PokemonNode* newPokemon = createPokemonNode(pokedex + choice);
    // if the pokadex is empty - put the new pokemon as the root
    if (owner->pokedexRoot == NULL) {
        owner->pokedexRoot = newPokemon;
    }
    else {
        // insert the new pokemon to his location in the binary tree
        // if the pokemon is exist
        if (insertPokemonNode(owner->pokedexRoot, newPokemon) == NULL) {
            printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", choice+1);
            free(newPokemon);
            return;
        }
    }
    printf("Pokemon %s (ID %d) added. \n", newPokemon->data->name, newPokemon->data->id);
}
// func to find the father of a pokemon in a BST
PokemonNode *findFather(PokemonNode *root, PokemonNode *toRemove) {
    // initialize the iterators to root
    PokemonNode *father = root;
    PokemonNode *current = root;
    // get the id of the pokemon to renove
    int id = toRemove->data->id;
    // run as long as current pokemonNode is not null
    while (current){
        // if the current id match the id we looking for - return the father
        if (current->data->id == id)
            return father;
        // if the current id is greater than to remove id - move left
        if (current->data->id > id) {
            father = current;
            current = current->left;
        }
        // If the current id is less than the to remove id - move right
        else {
            father = current;
            current = current->right;
        }
    }
    return NULL;
}
void deletePokemon (PokemonNode **root, PokemonNode *toRemove) {
    PokemonNode * father = findFather(*root, toRemove);
    // If to remove is the root of the BST
    if (toRemove == *root) {
        // if no children
        if (toRemove->right == NULL && toRemove->left == NULL) {
            *root = NULL;
        }
        // one leve
    }
    // if no children
    if (toRemove->left == NULL && toRemove->right == NULL) {
        // if to remove is the root - clean the pokedex root to NULL
        if (toRemove == *root)
            *root = NULL;

        if (father->left == toRemove)
            father->left = NULL;
        else
            father->right = NULL;
        free(toRemove);
        return;
    }
    // If toRemove have left child
    if (toRemove->left != NULL && toRemove->right == NULL) {
        // if to remove is the root - make his left child to be the root
        if (toRemove == *root)
            *root = toRemove->left;
        // if to remove is left to his father - make his left child to be left to the father
        else if (father->left == toRemove)
            father->left = toRemove->left;
        // if to remove is right to his father - make his left child to be right to the father
        else
            father->right = toRemove->left;
        // free to remove
        free(toRemove);
        return;
    }
    // if toRemove have right child
    if (toRemove->left == NULL && toRemove->right != NULL) {
        // if to remove is the root - make his right child to be the root
        if (toRemove == *root)
            *root = toRemove->right;
        // if to remove is left to his father - make his right child to be left to the father
        else if (father->left == toRemove)
            father->left = toRemove->right;
        // if to remove is right to his father - make his right child to be right to the father
        else
            father->right = toRemove->right;
        // free to remove
        free(toRemove);
        return;
    }
    // If have two children
    // get the trace number (one right and all the way left
    PokemonNode *temp = toRemove->right;
    while (temp->left != NULL) {
        temp = temp->left;
    }
    // replace the data
    toRemove->data = temp->data;
    // delete the traced pokemon that we just replaced
    deletePokemon(&toRemove->right, temp);
}
// func to find pokemon based on ID
PokemonNode* findPokemon (PokemonNode* root, int id) {
    // If the pokedex is empty or we get all the way without find the id
    if (root == NULL)
        return NULL;
    // if we found matched id
    if (root->data->id == id)
        return root;
    // if the id we are looking for is less than the id we are at - recursive call to look in the left branch
    if (root->data->id > id)
        return findPokemon(root->left, id);
    // if the id we are looking for is greater the the id we are at - recursive call to look in the right branch
    if (root->data->id < id)
        return findPokemon(root->right, id);
    return NULL;
}
// func to free pokemon
void freePokemon(OwnerNode *owner) {
    // if pokedex is empty
    if (owner->pokedexRoot == NULL) {
        printf("No Pokemon to release.\n");
        return;
    }
    // get the choice of which pokemon to remove from the user
    int choice = readIntSafe("Enter Pokemon ID to release: ");
    // Find the pokemon in the pokedex
    PokemonNode *toRemove = findPokemon(owner->pokedexRoot, choice);
    // if the pokemon isnt in the pokedex
    if (toRemove == NULL) {
        printf("No Pokemon with ID %d found.\n", choice);
        return;
    }
    printf("Removing Pokemon %s (ID %d).\n", toRemove->data->name, toRemove->data->id);
    // delete the pokemon
    deletePokemon (&owner->pokedexRoot, toRemove);

}
// Pokemon fight
void pokemonFight(OwnerNode *owner) {
    if (owner->pokedexRoot == NULL) {
        printf("Pokedex is empty.\n");
        return;
    }
    // Get the picks for the fight
    int firstId = readIntSafe("Enter ID of the first Pokemon: ");
    int secondId = readIntSafe("Enter ID of the second Pokemon: ");
    // Get the pokemons addresses
    PokemonNode *firstPokemon = findPokemon(owner->pokedexRoot, firstId);
    PokemonNode *secondPokemon = findPokemon(owner->pokedexRoot, secondId);
    // one or both not exist
    if (firstPokemon == NULL || secondPokemon == NULL) {
        printf("One or both Pokemon IDs not found.\n");
        return;
    }
    // Get the power of each pokemon using casting
    float resultfirst = (float)firstPokemon->data->attack*1.5 + (float)firstPokemon->data->hp*1.2;
    printf("Pokemon 1: %s (Score = %.2f)\n", firstPokemon->data->name, resultfirst);
    float resultsecond = (float)secondPokemon->data->attack*1.5 + (float)secondPokemon->data->hp*1.2;
    printf("Pokemon 2: %s (Score = %.2f)\n", secondPokemon->data->name, resultsecond);
    // the first pokemon is the winner
    if (resultfirst > resultsecond)
        printf("%s wins!\n", firstPokemon->data->name);
    // the second pokemon is the winner
    else if (resultfirst < resultsecond)
        printf("%s wins!\n", secondPokemon->data->name);
    // Tie
    else
        printf("It's a tie!\n");

}
// Evolve pokemon
void evolvePokemon(OwnerNode *owner) {
    // If pokedex id empty
    if (owner->pokedexRoot == NULL) {
        printf("Cannot evolve. Pokedex empty.\n");
        return;
    }
    // get the choice
    int choice = readIntSafe("Enter ID of Pokemon to evolve:");
    PokemonNode *toEvolve = findPokemon(owner->pokedexRoot, choice);
    // If no such pokemon in the pokedex
    if (toEvolve == NULL) {
        printf("No Pokemon with ID %d found.\n", choice);
        return;
    }
    // can evolve
    if (toEvolve->data->CAN_EVOLVE == CAN_EVOLVE) {
        // alredy inside
        if (findPokemon(owner->pokedexRoot, toEvolve->data->id+1)) {
            printf("Evolution ID %d (%s) already in the Pokedex. Releasing %s (ID %d).\n", pokedex[choice].id,\
            pokedex[choice].name, toEvolve->data->name, toEvolve->data->id);
            // Delete the pokemon that just evolved
            deletePokemon(&owner->pokedexRoot, toEvolve);
            return;
        }
        // elvolved form isnt inside
        else {
            printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", toEvolve->data->name, toEvolve->data->id,\
                pokedex[choice].name, pokedex[choice].id);
            toEvolve->data = (PokemonData*)&pokedex[choice];
            return;
        }
    }
    // canot evolve
    else {
        printf("%s (ID %d) cannot evolve.\n", toEvolve->data->name, toEvolve->data->id);
        return;
    }
}
// Chose an owner and able further action
void enterExistingPokedexMenu()
{
    if (ownerHead == NULL) {
        printf("No existing Pokedexes.");
        return;
    }
    printf("\nExisting Pokedexes:\n");
    // print the owners and chose one
    OwnerNode* pickedOwner = printExistingPokadex(ownerHead);
    // further action for the picked owner
    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", pickedOwner->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");
        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            // Add pokemon to the pokedex
            addPokemon(pickedOwner);
            break;
        case 2:
            // display the pokedex in diffrent options
            displayMenu(pickedOwner);
            break;
        case 3:
            freePokemon(pickedOwner);
            break;
        case 4:
            pokemonFight(pickedOwner);
            break;
        case 5:
            evolvePokemon(pickedOwner);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}
// Find owner by his name
OwnerNode *findOwnerByName(const char *name) {
    // if there is no owners - return NULL
    if (ownerHead == NULL)
        return NULL;
    // create temp owner that will compare the name to the names of the owners in the linked list
    OwnerNode *temp = ownerHead;
    // check first the head and then move on until complete a circle
    do {
        if (!strcmp(name, temp->ownerName))
            return temp;
        temp = temp->next;
    }
    while (temp != ownerHead);
    return NULL;
}
// Link the linking list where the last owner entered is behind the head
void linkOwnerInCircularList(OwnerNode *newOwner) {
    // If there arnt any pokedex - the head will point the new owner and the owner will point to himself
    if (ownerHead == NULL) {
        ownerHead = newOwner;
        newOwner->next = newOwner;
        newOwner->prev = newOwner;
    }
    // if there are pokedexes
    else {
        // Create iterator that will run to the owner that behind the head
        OwnerNode *iterator = ownerHead;
        while (iterator->next != ownerHead) {
            iterator = iterator->next;
        }
        // Initialize the new owner to be behind the head
        newOwner->next = ownerHead;
        newOwner->prev = iterator;
        iterator->next = newOwner;
        ownerHead->prev = newOwner;
    }
}
// Create new owner
OwnerNode *createOwner(char *ownerName, PokemonNode *starter) {
    // allocate location in memory
    OwnerNode *newOwner = (OwnerNode *)malloc(sizeof(OwnerNode));
    // if allocation failed
    if (!newOwner) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // initialize the owner node struct
    newOwner->ownerName = ownerName;
    newOwner->next = NULL;
    newOwner->prev = NULL;
    newOwner->pokedexRoot = starter;
    return newOwner;
}
// Create new pokemon node
PokemonNode *createPokemonNode(const PokemonData *data) {
    // allocate memory
    PokemonNode *newNode = (PokemonNode *)malloc(sizeof(PokemonNode));
    // if allocation failed
    if (!newNode) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    // Initialize the pokemon node struct
    newNode->data = (PokemonData*)data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// Add new owner (pokedex)
void openPokedexMenu(void) {
    printf("Your name: ");
    // get the name using getDynamicInput func
    char* name = getDynamicInput();
    // if the name is already exist
    if (findOwnerByName(name)) {
        printf("Owner '%s' already exists. Not creating a new Pokedex.\n", name);
        return;
    }
    printf("Choose Starter:\n");
    printf("1. Bulbasaur\n");
    printf("2. Charmander\n");
    printf("3. Squirtle\n");
    int choice = readIntSafe ("Your choice: ");
    // cant choose diffrent then these options
    if (choice > 3 || choice < 1) {
        printf("Invalid choice.\n");
        return;
    }
    // adjust the choice to its real location in the pokemons array
    choice = (choice - 1) * 3;
    // create new pokemon node
    PokemonNode* newPokemon = createPokemonNode(pokedex + choice);
    // create new owner
    OwnerNode* newOwner = createOwner(name, newPokemon);
    // link the new owner to the end of the circular list
    linkOwnerInCircularList(newOwner);
    printf("New Pokedex created for %s with starter %s.\n", newOwner->ownerName,\
        newOwner->pokedexRoot->data->name);
}
OwnerNode* printExistingPokadexToDelete (OwnerNode *head) {
    // Create itarator that will run on the linking list - starting from the head
    OwnerNode *iterator = head;
    // Create counter that will be the number of the owner in the list
    int counter = 1;
    // Print the owner name and the number, then nove the iterator to the next owner and increment the counter
    do {
        printf("%d. %s\n", counter, iterator->ownerName);
        counter++;
        iterator = iterator->next;
    }
    // Print until complete a circle
    while (iterator != head);
    // Now choose an owner by his number
    int ownerNum = readIntSafe("Choose a Pokedex to delete by number: ");
    // mvoe the iterator to the choosen owner
    for (int i = 0; i < ownerNum-1; i++)
        iterator = iterator->next;
    printf("Deleting %s's entire Pokedex...\n", iterator->ownerName);
    // return the chosen owner
    return iterator;
}
// func to free the pokemons tree by post order methode
void freePokemonTree(PokemonNode *pokemon) {
    // stop comdition
    if (pokemon == NULL)
        return;
    freePokemonTree(pokemon->left);
    freePokemonTree(pokemon->right);
    free(pokemon);
}
// delete th epokadex (owner)
void deletePokedex(void) {
    if (ownerHead == NULL) {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    // print the pokedex and get the one to delete
    OwnerNode* toDelete = printExistingPokadexToDelete(ownerHead);
    // free the pokemon tree
    freePokemonTree(toDelete->pokedexRoot);
    // if there is more than one pokedex
    if (toDelete != toDelete->next){
        // adjust the liking to remove the pokedex
        toDelete->prev->next = toDelete->next;
        toDelete->next->prev = toDelete->prev;
        // if to delete is the head
        if (toDelete == ownerHead)
            ownerHead = toDelete->next;
    }
    // if there is only one owner
    else {
        ownerHead = NULL;
    }
    // free to delete
    free(toDelete->ownerName);
    free(toDelete);
    printf("Pokedex deleted.\n");
}
// merge to pokedex - insert the pokemon of the second to the first by BFS
void mergePokedexMenu(void) {
    // if there is less then 2 owners - canot merge
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("Not enough owners to merge.\n");
        return;
    }
    printf("\n=== Merge Pokedexes ===\n");
    // Get the names of the owner and find them in the list
    printf("Enter name of first owner: ");
    char* firstOwnerName = getDynamicInput();
    printf("Enter name of second owner: ");
    char* secondOwnerName = getDynamicInput();
    OwnerNode* firstOwner = findOwnerByName(firstOwnerName);
    OwnerNode* secondOwner = findOwnerByName(secondOwnerName);
    // if one or both arent exist
    if (firstOwner == NULL || secondOwner == NULL) {
        printf("One or both owners not found.\n");
        return;
    }
    // if both empty
    if (secondOwner->pokedexRoot == NULL && secondOwner->pokedexRoot == NULL ) {
        printf("Both Pokedexes empty. Nothing to merge.\n");
        return;
    }
    // make a queue to insert the second owner pokemons
    if (secondOwner->pokedexRoot != NULL) {
        // Create a queue
        Queue *queue = createQueue();
        // attach the root to the queue
        enQueue(queue, secondOwner->pokedexRoot);
        // as long as the queue us not empty - keep printing getting the nodes out
        while (!isEmptyQueue(queue)) {
            PokemonNode *current = deQueue(queue);
            // create a node for the pokemon node
            PokemonNode* newPokemon = createPokemonNode(pokedex + (current->data->id-1));
            // insert the new pokemon to his location in the binary tree of the first owner
            if (insertPokemonNode(firstOwner->pokedexRoot, newPokemon) == NULL)
                free(newPokemon);
            // add the left children to the queue
            if (current->left != NULL)
                enQueue(queue, current->left);
            // add the right children to the queue
            if (current->right != NULL)
                enQueue(queue, current->right);
        }
        // free the queue memory allocation
        free(queue);
    }
    printf("Merging %s and %s...\n", firstOwnerName, secondOwnerName);
    printf("Merge completed.\n");
    printf("Owner '%s' has been removed after merging.\n", secondOwnerName);
    // adjust the linkng
    secondOwner->prev->next = secondOwner->next;
    secondOwner->next->prev = secondOwner->prev;
    // if the second is the head, update the head
    if (secondOwner == ownerHead)
        ownerHead = secondOwner->next;
    // free the second owner
    freePokemonTree(secondOwner->pokedexRoot);
    free(secondOwner->ownerName);
    free(secondOwner);
    // free the names allocation
    free(firstOwnerName);
    free(secondOwnerName);
}
// Sort the owners by alphabetic
void sortOwners(void) {
    // if less than 2 owners - nothing to sort
    if (ownerHead == NULL || ownerHead->next == ownerHead) {
        printf("0 or 1 owners only => no need to sort.\n");
        return;
    }
    // create a counter to count the number of owners
    int ownersNum = 0;
    OwnerNode *iterator = ownerHead;
    do {
        iterator = iterator->next;
        ownersNum++;
    }
    while (iterator != ownerHead);
    // bouble sort
    for (int i = 0; i < ownersNum-1; i++) {
        iterator = ownerHead;
        for (int j = 0; j < ownersNum - i - 1; j++) {
            OwnerNode* iteratorNext = iterator->next;
            if (strcmp(iterator->ownerName, iteratorNext->ownerName) > 0) {
                swapOwnerData(iterator, iteratorNext);
            }
            iterator = iterator->next;
        }
    }
    printf("Owners sorted by name.\n");
}
// Func to swap the data of two owners
void swapOwnerData(OwnerNode *a, OwnerNode *b) {
    // save a info (name and pokedex root)
    PokemonNode* aRoot = a->pokedexRoot;
    char* aOwnerName = a->ownerName;
    // replace the data
    a->ownerName = b->ownerName;
    a->pokedexRoot = b->pokedexRoot;
    b->ownerName = aOwnerName;
    b->pokedexRoot = aRoot;
}
// Print the owners circular forwards or backwards
void printOwnersCircular(void) {
    // if no owners
    if (ownerHead == NULL) {
        printf("No owners.\n");
        return;
    }
    // Get the direction the user want to print the list (F,f,B,b avilabe)
    printf("Enter direction (F or B): ");
    char* direction = getDynamicInput();
    while (*direction != 'F' && *direction != 'B' && *direction != 'f' && *direction != 'b') {
        printf("Invalid direction, must be L or R.\n");
        printf("Enter direction (F or B): ");
        free (direction);
        direction = getDynamicInput();
    }
    // Get the number of prints the user wants
    int printNum = readIntSafe("How many prints? ");
    OwnerNode* iterator = ownerHead;
    // print the owners forwards
    if (*direction == 'F' || *direction == 'f') {
        for (int i = 0; i < printNum; i++) {
            printf("[%d] %s\n", i+1, iterator->ownerName);
            iterator = iterator->next;
        }
    }
    // print the owners backwards
    else {
        for (int i = 0; i < printNum; i++) {
            printf("[%d] %s\n", i+1, iterator->ownerName);
            iterator = iterator->prev;
        }
    }
    // free the direction allocation
    free(direction);
}
// free all the owners (used before exiting the program)
void freeAllOwners(void) {
    // if no owners - nothing to free
    if (ownerHead == NULL)
        return;
    // breake the circular (make the one that point back to the head point to NULL)
    ownerHead->prev->next = NULL;
    OwnerNode* iterator = ownerHead;
    // rum on two iterators and free the owners until finishing
    while (iterator != NULL) {
        OwnerNode* iteratorNext = iterator->next;
        freePokemonTree(iterator->pokedexRoot);
        free(iterator->ownerName);
        free(iterator);
        iterator = iteratorNext;
    }
}