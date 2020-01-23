#ifndef BSTRAND_H_INCLUDED
#define BSTRAND_H_INCLUDED

#include <iostream> //For size_t and other things
#include <stdexcept> //For exceptions
#include <time.h> //Used to initialize srand
#include <stdlib.h> //For rand and srand

namespace cop3530{

    //BST for inserting randomly, either root or leaf
    //Only change is in insert, randomly pick either to insert at leaf or root
    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    class BSTRAND{

    private:

        struct node{
            key_type key;
            value_type value;
            node* left;
            node* right;
        };

        node* root;
        size_t get_size(node* curr); //Function to recursively get amount of key-value pairs
        void insert_at_root(node*& curr, key_type k, value_type v); //Recursively inserts k-v pair
        node* do_delete(node* curr, key_type k); //Removes node using recursion to fix tree
        void deletion(node* curr); //Helps the clear function. Deletes recursively
        int get_height(node* curr); //Helps height function, recursively calls for height on nodes
        void rotate_left(node*& curr); //Used during insertion at root, rotates subtree counterclockwise
        void rotate_right(node*& curr); //Used during insertion at root, rotates subtree clockwise
        node* insert_at_leaf(node* curr, key_type k, value_type v); //Called if insertion is for a leaf
        void recursive_copy(node*& curr);

    public:
        BSTRAND();
        BSTRAND(const BSTRAND& b); //copy constructor
        BSTRAND& operator=(const BSTRAND& b); //Copy assignment operator
        BSTRAND(BSTRAND&& b); //Move constructor
        BSTRAND& operator=(BSTRAND&& b); //Move-assignment operator
        ~BSTRAND();

        void insert(key_type k, value_type v); //Adds k-v pair to map
        void remove(key_type k); //Removes k-v pair from map
        value_type& lookup(key_type k); //Returns a reference to the value associated with given key

        bool contains(key_type k); //Returns true if tree contains value associated with key
        bool is_empty(); //Returns true if tree is empty
        bool is_full(); //Returns true if no more pairs can be added to map
        size_t size(); //Returns all key value pairs in map
        void clear(); //Removes all elements from map
        int height(); //Returns tree's height
        int balance(); //Returns tree's balance factor
    };

    //CONSTRUCTORS AND DESTRUCTORS

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals> :: BSTRAND(){
        root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals> :: BSTRAND(const BSTRAND& b){ //Deep copy constructor
        root = nullptr;

        node* curr = b.root;
        recursive_copy(curr);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals>& BSTRAND<key_type, value_type, compare, equals> :: operator=(const BSTRAND& b){ //Copy assignment operator
        //Check that this and given BST arent the same
        if(this == &b){
            return *this;
        }

        //Else, clear the tree and add a deep copy of given BST
        clear();

        node* curr = b.root;
        recursive_copy(curr);
        return *this;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals> :: BSTRAND(BSTRAND&& b){ //Move constructor
        //Creates a shallow copy and sets root of given BST to nullptr
        root = b.root;
        b.root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals>& BSTRAND<key_type, value_type, compare, equals> :: operator=(BSTRAND&& b){ //Move assignment operator
        //Check that this and given BST arent the same
        if(this == &b){
            return *this;
        }

        //Free existing BST, copy from given, then set to default given BST
        clear();
        root = b.root;
        b.root = nullptr;
        return *this;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    BSTRAND<key_type, value_type, compare, equals> :: ~BSTRAND(){
        //To destroy BST, delete root, which will delete children, with destructor for nodes
        deletion(root);
    }

    //HELPER FUNCTIONS

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: recursive_copy(node*& curr){
        if(curr){
            insert(curr->key, curr->value);
        }

        if(curr->left){
            recursive_copy(curr->left);
        }
        if(curr->right){
            recursive_copy(curr->right);
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    size_t BSTRAND<key_type, value_type, compare, equals> :: get_size(node* curr){
        //Recursively goes through counting number of nodes
        //Case if node we are on doesn't exist
        if(!curr){
            return 0;
        }
        //Case where we reached bottom of tree, count 1
        if(!curr->left && !curr->right){
            return 1;
        }
        //Return left side + right + 1 to account for itself
        return get_size(curr->left) + get_size(curr->right) + 1;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int BSTRAND<key_type, value_type, compare, equals> :: get_height(node* curr){
        //Gets max height of left and right size, returns max
        int left_max = 1;
        int right_max = 1;

        //Case we are on node that does not exist
        if(!curr){
            return 0;
        }
        //Bottom of tree, add 1 and work our way up
        if(!curr->left && !curr->right){
            return 1;
        }

        //Keep adding to specific side and work recursively
        left_max += get_height(curr->left);
        right_max += get_height(curr->right);

        //Return the side with most items
        if(left_max > right_max){
            return left_max;
        }

        return right_max;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: deletion(node* curr){
        //Deletion goes to bottom of tree and works its way up, deleting each node on the way and setting children to nullptr
        if(curr){
            deletion(curr->left);
            deletion(curr->right);
            delete curr;
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename BSTRAND<key_type, value_type, compare, equals> :: node* BSTRAND<key_type, value_type, compare, equals> :: do_delete(node* curr, key_type k){
        //Removing, this means we have a match and curr is on node we have to remove
        if(equals(k, curr->key)){
            //Case 1: no children, just delete curr and set to nullptr for parent
            if(!curr->left && !curr->right){
                delete curr;
                curr = nullptr;
            }
            //Case 2: One child. Simply delete and return right child so parent has that child
            else if(!curr->left){
                node* temp = curr;
                curr = curr->right;
                delete temp;
            }
            //Case 2 but for other child
            else if(!curr->right){
                node* temp = curr;
                curr = curr->left;
                delete temp;
            }
            //Case 3: 2 children from deletion node
            else{
                //Get the in-order successor
                node* temp = curr->right;
                while(temp->left){
                    temp = temp->left;
                }

                //Swap in order successor's values with deletion node's
                curr->key = temp->key;
                curr->value = temp->value;

                //Keep working in order to fix tree
                curr->right = do_delete(curr->right, temp->key);
            }
        }
        //Key comes before node we are currently on, so go left
        else if(compare(k, curr->key)){
            curr->left = do_delete(curr->left, k);
        }
        //Key greater than, go right
        else{
            curr->right = do_delete(curr->right, k);
        }

        //Return node to keep track of children
        return curr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: rotate_left(node*& curr){
        //Rotate's counter clockwise using process shown in class
        node* temp = curr;
        curr = curr->right;
        temp->right = curr->left;
        curr->left = temp;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: rotate_right(node*& curr){
        //Rotate's clockwise using process shown in class
        node* temp = curr;
        curr = curr->left;
        temp->left = curr->right;
        curr->right = temp;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: insert_at_root(node*& curr, key_type k, value_type v){
        //Since we are inserting at root, we have base case here. Here we insert at a leaf, but rotate up from recursive calls
        if(!curr){
            curr = new node;
            curr->key = k;
            curr->value = v;
            curr->left = nullptr;
            curr->right = nullptr;
        }
        //Key is less than current node, go left and rotate right
        else if(compare(k, curr->key)){
            insert_at_root(curr->left, k, v);
            rotate_right(curr);
        }
        //Go right and rotate left
        else{
            insert_at_root(curr->right, k, v);
            rotate_left(curr);
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename BSTRAND<key_type, value_type, compare, equals> :: node* BSTRAND<key_type, value_type, compare, equals> :: insert_at_leaf(node* curr, key_type k, value_type v){
        //Since we are inserting at leaf, only insert when curr is nullptr
        if(!curr){
            curr = new node;
            curr->key = k;
            curr->value = v;
            curr->left = nullptr;
            curr->right = nullptr;
            return curr;
        }
        //Key is less than current node, go left
        else if(compare(k, curr->key)){
            curr->left = insert_at_leaf(curr->left, k, v);
        }
        //Go right
        else{
            curr->right = insert_at_leaf(curr->right, k, v);
        }

        //Return child, with no changes made (keep going down)
        return curr;
    }

    //PUBLIC FUNCTIONS

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: insert(key_type k, value_type v){
        //Inserting randomly, we will generate a random number between 0 and tree size, inclusive
        //If this random number is 0, insert at root, otherwise insert at leaf

        //If tree empty, just insert at root, otherwise check if key already used and recursively insert k-v pair
        if(is_empty()){
            root = new node;
            root->key = k;
            root->value = v;
            root->left = nullptr;
            root->right = nullptr;
            return;
        }

        if(contains(k)){
            throw std::runtime_error("Already contain that key");
        }

        srand(time(nullptr));
        int rand_number = rand() % size(); //Random number between 0 and tree size inclusive

        //If random number is 0, then inserting at root
        if(rand_number == 0){
            insert_at_root(root, k, v);
        }
        else{ //Leaf insert
            insert_at_leaf(root, k, v);
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: remove(key_type k){
        //Check if empty to avoid errors, then call recursive function to remove
        if(is_empty()){
            throw std::runtime_error("Cannot remove from an empty map");
        }

        do_delete(root, k);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    value_type& BSTRAND<key_type, value_type, compare, equals> :: lookup(key_type k){
        //Again check if empty and then go through tree looking for key
        if(is_empty()){
            throw std::runtime_error("Cannot lookup in an empty map");
        }

        node* curr = root;

        while(curr){
            //Key matched node we are on
            if(equals(k, curr->key)){
                return curr->value;
            }
            //Key comes before node we are on, go left
            else if(compare(k, curr->key)){
                curr = curr->left;
            }
            //Go right
            else{
                curr = curr->right;
            }
        }

        //No key was found, return error
        throw std::runtime_error("Given key was not in the map to remove");
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    bool BSTRAND<key_type, value_type, compare, equals> :: contains(key_type k){
        //Same as lookup, just returning bool instead of value
        if(is_empty()){
            return false;
        }

        node* curr = root;
        while(curr){
            if(equals(k, curr->key)){
                return true;
            }
            else if(compare(k, curr->key)){
                curr = curr->left;
            }
            else{
                curr = curr->right;
            }
        }

        return false;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    bool BSTRAND<key_type, value_type, compare, equals> :: is_empty(){
        //BST only empty if root doesnt exist
        if(!root){
            return true;
        }

        return false;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    bool BSTRAND<key_type, value_type, compare, equals> :: is_full(){
        //BST never full
        return false;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    size_t BSTRAND<key_type, value_type, compare, equals> :: size(){
        //Calls on the recursive function for size
        return get_size(root);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void BSTRAND<key_type, value_type, compare, equals> :: clear(){
        //Calls recursive deletion, then sets root to nullptr to indicate empty
        deletion(root);
        root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int BSTRAND<key_type, value_type, compare, equals> :: height(){
        //Calls recursive function height and subtracts 1 to get rid of counting root as 1
        return get_height(root) - 1;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int BSTRAND<key_type, value_type, compare, equals> :: balance(){
        //If empty or size == 1, then its 0, otherwise left - right
        if(is_empty() || size() == 1){
            return 0;
        }

        return get_height(root->left) - get_height(root->right);
    }
}

#endif
