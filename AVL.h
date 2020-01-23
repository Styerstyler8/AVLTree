#ifndef AVL_H_INCLUDED
#define AVL_H_INCLUDED

#include <iostream> //For size_t and other things
#include <stdexcept> //For exceptions

namespace cop3530{

    //AVL tree, must be AVL balanced throughout
    //Only change is in insert and remove, must choose correct rotation for balance factor
    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    class AVL{

    private:

        struct node{
            key_type key;
            value_type value;
            node* left;
            node* right;
        };

        node* root;
        size_t get_size(node* curr); //Function to recursively get amount of key-value pairs
        node* do_delete(node*& curr, key_type k); //Removes node using recursion to fix tree
        void deletion(node* curr); //Helps the clear function. Deletes recursively
        int get_height(node* curr); //Helps height function, recursively calls for height on nodes
        node* rotate_left(node* curr); //Used during insertion at root, rotates subtree counterclockwise
        node* rotate_right(node* curr); //Used during insertion at root, rotates subtree clockwise
        node* insert_at_leaf(node*& curr, key_type k, value_type v); //Called if insertion is for a leaf
        int get_balance(node* curr); //Returns the balance factor for a subtree
        void recursive_copy(node*& curr);

    public:
        AVL();
        AVL(const AVL& b); //copy constructor
        AVL& operator=(const AVL& b); //Copy assignment operator
        AVL(AVL&& b); //Move constructor
        AVL& operator=(AVL&& b); //Move-assignment operator
        ~AVL();

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
    AVL<key_type, value_type, compare, equals> :: AVL(){
        root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    AVL<key_type, value_type, compare, equals> :: AVL(const AVL& b){ //Deep copy constructor
        root = nullptr;

        node* curr = b.root;
        recursive_copy(curr);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    AVL<key_type, value_type, compare, equals>& AVL<key_type, value_type, compare, equals> :: operator=(const AVL& b){ //Copy assignment operator
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
    AVL<key_type, value_type, compare, equals> :: AVL(AVL&& b){ //Move constructor
        //Creates a shallow copy and sets root of given BST to nullptr
        root = b.root;
        b.root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    AVL<key_type, value_type, compare, equals>& AVL<key_type, value_type, compare, equals> :: operator=(AVL&& b){ //Move assignment operator
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
    AVL<key_type, value_type, compare, equals> :: ~AVL(){
        //To destroy BST, clear it and get rid of root
        deletion(root);
    }

    //HELPER FUNCTIONS

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void AVL<key_type, value_type, compare, equals> :: recursive_copy(node*& curr){
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
    size_t AVL<key_type, value_type, compare, equals> :: get_size(node* curr){
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
    int AVL<key_type, value_type, compare, equals> :: get_balance(node* curr){
        //If curr exists, then get balance factor for given node
        if(curr){
            return get_height(curr->left) - get_height(curr->right);
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int AVL<key_type, value_type, compare, equals> :: get_height(node* curr){
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
    void AVL<key_type, value_type, compare, equals> :: deletion(node* curr){
        //Deletion goes to bottom of tree and works its way up, deleting each node on the way and setting children to nullptr
        if(curr){
            deletion(curr->left);
            deletion(curr->right);
            delete curr;
        }
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename AVL<key_type, value_type, compare, equals> :: node* AVL<key_type, value_type, compare, equals> :: do_delete(node*& curr, key_type k){
        //Removing, this means we have a match and curr is on node we have to remove

        if(!curr){
            return curr;
        }

        if(equals(k, curr->key)){
            //Case 1: no children, just delete curr and set to nullptr for parent
            if(!curr->left && !curr->right){
                node* temp = curr;
                curr = nullptr;
                delete temp;
            }
            //Case 2: One child. Simply delete and return right child so parent has that child
            else if(!curr->left){
                node* temp = curr->right;
                *curr = *temp;
                delete temp;
            }
            //Case 2 but for other child
            else if(!curr->right){
                node* temp = curr->left;
                *curr = *temp;
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

        //Case here: Only 1 item was in tree and just deleted that
        if(!curr){
            return curr;
        }

        int bf = get_balance(curr); //Gets the balance factor for this node

        //These if statements check if bf is not in range of -1<bf<1. Does corresponding rotation if not in range
        //This is similar to how insertion was done, just this time look at one child's bf and do corresponding rotation
        //Left left rotation (left heavy)
        if(bf > 1 && get_balance(curr->left) >= 0){
            return rotate_right(curr);
        }

        //Left right rotation
        if(bf > 1 && get_balance(curr->left) < 0){
            curr->left = rotate_left(curr->left);
            return rotate_right(curr);
        }

        //Right right rotation (right heavy)
        if(bf < -1 && get_balance(curr->right) <= 0){
            return rotate_left(curr);
        }

        //Right left rotation
        if(bf < -1 && get_balance(curr->left) > 0){
            curr->right = rotate_right(curr->right);
            return rotate_left(curr);
        }

        //Return node to keep track of children
        return curr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename AVL<key_type, value_type, compare, equals> :: node* AVL<key_type, value_type, compare, equals> :: rotate_left(node* curr){
        //Rotate's counter clockwise and returns the new root of the (sub)tree
        node* temp = curr;
        curr = curr->right;
        temp->right = curr->left;
        curr->left = temp;
        return curr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename AVL<key_type, value_type, compare, equals> :: node* AVL<key_type, value_type, compare, equals> :: rotate_right(node* curr){
        //Rotate's clockwise and returns the new root of the (sub)tree
        node* temp = curr;
        curr = curr->left;
        temp->left = curr->right;
        curr->right = temp;
        return curr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    typename AVL<key_type, value_type, compare, equals> :: node* AVL<key_type, value_type, compare, equals> :: insert_at_leaf(node*& curr, key_type k, value_type v){
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
        if(compare(k, curr->key)){
            curr->left = insert_at_leaf(curr->left, k, v);
        }
        //Go right
        else{
            curr->right = insert_at_leaf(curr->right, k, v);
        }

        int bf = get_balance(curr); //Gets the balance factor for this node

        //These if statements check if bf is not in range of -1<bf<1. Does corresponding rotation if not in range
        //Left left rotation (left heavy)
        if(bf > 1 && compare(k, curr->left->key)){
            return rotate_right(curr);
        }

        //Left right rotation
        if(bf > 1 && compare(curr->left->key, k)){
            curr->left = rotate_left(curr->left);
            return rotate_right(curr);
        }

        //Right right rotation (right heavy)
        if(bf < -1 && compare(curr->right->key, k)){
            return rotate_left(curr);
        }

        //Right left rotation
        if(bf < -1 && compare(k, curr->right->key)){
            curr->right = rotate_right(curr->right);
            return rotate_left(curr);
        }

        //Return child, with no changes made (keep going down)
        return curr;
    }

    //PUBLIC FUNCTIONS

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void AVL<key_type, value_type, compare, equals> :: insert(key_type k, value_type v){
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

        insert_at_leaf(root, k, v);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void AVL<key_type, value_type, compare, equals> :: remove(key_type k){
        //Check if empty to avoid errors, then call recursive function to remove
        if(is_empty()){
            throw std::runtime_error("Cannot remove from an empty map");
        }

        if(!contains(k)){
            throw std::runtime_error("Key is not in map");
        }

        do_delete(root, k);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    value_type& AVL<key_type, value_type, compare, equals> :: lookup(key_type k){
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
        throw std::runtime_error("Given key was not in the map to lookup");
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    bool AVL<key_type, value_type, compare, equals> :: contains(key_type k){
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
    bool AVL<key_type, value_type, compare, equals> :: is_empty(){
        //BST only empty if root doesnt exist
        if(!root){
            return true;
        }

        return false;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    bool AVL<key_type, value_type, compare, equals> :: is_full(){
        //BST never full
        return false;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    size_t AVL<key_type, value_type, compare, equals> :: size(){
        //Calls on the recursive function for size
        return get_size(root);
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    void AVL<key_type, value_type, compare, equals> :: clear(){
        //Calls recursive deletion, then sets root to nullptr to indicate empty
        deletion(root);
        root = nullptr;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int AVL<key_type, value_type, compare, equals> :: height(){
        //Calls recursive function height and subtracts 1 to get rid of counting root as 1
        return get_height(root) - 1;
    }

    template<typename key_type, typename value_type, bool (*compare)(key_type, key_type), bool (*equals)(key_type, key_type)>
    int AVL<key_type, value_type, compare, equals> :: balance(){
        //If empty or size == 1, then its 0, otherwise left - right
        if(is_empty() || size() == 1){
            return 0;
        }

        return get_height(root->left) - get_height(root->right);
    }
}


#endif
