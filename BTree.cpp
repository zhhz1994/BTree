#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

template <class T, int M=4>
class BTreeNode{
    int m = ceil(M/2); 
public:
    int keyNum = 0;
    T keys[M];
    BTreeNode *parent = NULL;
    BTreeNode *children[M+1] = {NULL};
    BTreeNode(){};
    BTreeNode(T *start, int num){
        if (num <= M){
            copy(start, start + num, keys);
            keyNum = num;
        }
        else 
            cout<<"Invalid number of elements."<<endl;
    }
    int isLeaf(){
        for (int i=0; i<M; i++){
            if (children[i])
                return 0;
        }
        return 1;
    }
    bool isOverflow(){
        return keyNum > M-1;
    }
    bool isRich(){
        return keyNum > m-1 && keyNum <= M-1;
    }
    bool isUnderflow(){
        if (parent)
            return keyNum < m-1;
        else
            return keyNum < 1;
    }
    void setChild(BTreeNode *child, int num){
        children[num] = child;
        if (child){
            child->parent = this;
        }
    }
    void passChildren(int begin, int end, BTreeNode<T> *newParent, int newPosition){
        for (int i=begin; i<end && children[i] != NULL; i++){
            if (newPosition > M){
                cout<<"newPosition > M"<<endl;
                break;
            }
            newParent->setChild(children[i], newPosition);
            children[i] = NULL;
            newPosition++;
        }
    }
    void transfer(int begin, int end, BTreeNode<T> *newNode, int newPosition){
        int i;
        for (i=begin; i<end; i++){
            if (i > M || newPosition > M){
                cout<<"index > M during transfer"<<endl;
                break;
            }
            newNode->keys[newPosition] = this->keys[i];
            newNode->setChild(children[i], newPosition);
            children[i] = NULL;
            newPosition ++;
        }
        newNode->setChild(children[i], newPosition);
        children[i] = NULL;
        this->keyNum -= (end - begin);
        newNode->keyNum += (end - begin);
    }
    BTreeNode<T> *findRoot(){
        BTreeNode<T> *p;
        for (p=this; p->parent; p=p->parent);
        return p;
    }
    int findIndex(){
        int i=0;
        if (!parent){
            cout<<"Root has no child index."<<endl;
            return 0;
        }
        for (i=0; i < parent->keyNum+1; i++){
            if (parent->children[i] == this)
                return i;
        }
        cout<<"index not found"<<endl;
        return 0;
    }
    void printNode(){
        cout<<"(";
        int i;
        for (i=0; i<keyNum; i++)
            cout<<keys[i]<<", ";
        if (i) cout<<"\b\b";
        cout<<") ";
    }
    void printBTree(){
        printNode();
        if (isLeaf()){
            cout<<": leaf"<<endl;
            return;
        }
        else{
            cout<<"--> ";
        }
        for (int i=0; i < M+1; i++)
            if (children[i]!=NULL){
                children[i]->printNode();
            }
        cout<<endl;
        for (int i=0; i < M+1; i++){
            if (children[i]!=NULL){
                if (children[i]->parent != this){
                    cout<<endl;children[i]->printNode();cout<<" has wrong parent "<<children[i]->parent<<" instead of "<<this<<endl;
                }
                if (!children[i]->isLeaf())
                children[i]->printBTree();
            }
        }
    } 

    BTreeNode<T> *findBranch(T data){
        int i;
        for (i=0; i < keyNum && keys[i] < data; i++);
        return children[i];
    }
    void split(){
        if (keyNum < M){
            cout<<"No need to split."<<endl;
            return;
        }
        if (parent == NULL){    //this node is the root
            cout<<"spliting the root."<<endl;
            (new BTreeNode<T>)->setChild(this, 0);
            parent->keys[0] = keys[m-1];
            parent->keyNum = 1;
            
            keyNum--;
            parent->setChild(new BTreeNode<T>, 1);
            transfer(m, M, parent->children[1], 0);
        }
        else if (parent->keyNum <= M-1){
            int i;
            for (i = parent->keyNum-1; i>=0 && parent->keys[i] > keys[m-1]; i--){
                parent->keys[i+1] = parent->keys[i];
                parent->setChild(parent->children[i+1], i+2);
                
            }
            parent->keys[i+1] = keys[m-1];
            parent->keyNum += 1;
            
            keyNum--;
            parent->setChild(new BTreeNode<T>, i+2);
            transfer(m, M, parent->children[i+2], 0);
            if (parent->isOverflow()){
                parent->split();
            }
        }
    }
    
    BTreeNode<T> *leftRotate(int num){
//         cout<<"left rotating"<<endl;
        if (isLeaf()){
            cout<<"Can not rotate a leaf."<<endl;
            return this;
        }
        BTreeNode<T> *leftChild = children[num];
        BTreeNode<T> *rightChild = children[num+1];
        
        int n = leftChild->keyNum;
        leftChild->keys[n] = keys[num];
        leftChild->keyNum += 1;
        leftChild->setChild(rightChild->children[0], n+1);
        
        keys[num] = rightChild->keys[0];
        rightChild->keyNum -= 1;
        int i;
        for (i=0; i < rightChild->keyNum; i++){
            rightChild->keys[i] = rightChild->keys[i+1];
            rightChild->setChild(rightChild->children[i+1], i);
        }
        rightChild->setChild(rightChild->children[i+1], i);
        rightChild->children[i+1] = NULL;
        return leftChild;
    }
    BTreeNode<T> *rightRotate(int num){
//         cout<<"right rotating"<<endl;
        if (isLeaf()){
            cout<<"Can not rotate a leaf."<<endl;
            return this;
        }
        BTreeNode<T> *leftChild = children[num];
        BTreeNode<T> *rightChild = children[num+1];
        int n = rightChild->keyNum;
        rightChild->setChild(rightChild->children[n], n+1);
        for (int i = n-1; i>=0; i--){
            rightChild->keys[i+1] = rightChild->keys[i];
            rightChild->setChild(rightChild->children[i], i+1);
        }
        rightChild->keys[0] = keys[num];
        leftChild->passChildren(leftChild->keyNum, leftChild->keyNum+1, rightChild, 0);
        rightChild->keyNum += 1;
        
        keys[num] = leftChild->keys[leftChild->keyNum-1];
        leftChild->keyNum -= 1;        
        return rightChild;
    }
    BTreeNode<T> *merge(int num){
//         cout<<"merging "<<endl;
        BTreeNode<T> *leftChild = children[num];
        BTreeNode<T> *rightChild = children[num+1];
        int n = leftChild->keyNum;
        leftChild->keys[n] = keys[num];
        leftChild->keyNum += 1;
        rightChild->transfer(0, rightChild->keyNum, leftChild, n+1);
        this->transfer(num+1, keyNum, this, num);
        keyNum -= 1;
        children[num] = leftChild;
        delete rightChild;
        return leftChild;
    }
    BTreeNode<T> *rebalance(){
        int i;
        BTreeNode<T> *p = this;
        BTreeNode<T> *leftSibling = NULL, *rightSibling = NULL;
        while (p->isUnderflow()){
            leftSibling =NULL;
            rightSibling = NULL;
            if (!p->parent){
                p = p->children[0];
                p->parent->children[0] = NULL;
                delete p->parent;
                p->parent = NULL;
                
                return p->findRoot();
            }
            i = p->findIndex();
            if (i-1 >= 0){
                leftSibling = p->parent->children[i-1];
            }
            if (i+1 <= p->parent->keyNum){
                rightSibling = p->parent->children[i+1];
            }
            if (leftSibling && leftSibling->isRich()){
                p->parent->rightRotate(i-1);
                break;
            }
            else if (rightSibling && rightSibling->isRich()){
                p->parent->leftRotate(i);
                break;
            }
            else if (leftSibling){
//                 cout<<"merging with left.";
                p =p->parent->merge(i-1);
                p = p->parent;
            }
            else if (rightSibling){
//                 cout<<"merging with right.";
                p = p->parent->merge(i);
                p = p->parent;
            }
            else{
                p->printNode();
                cout<<"have no sibling."<<endl;
                break;
            }
        }
        return p->findRoot();
    }

    ~BTreeNode(){
        if (!this->isLeaf()){
            for (int i=0; i < M+1; i++)
                if (children[i])
                    delete(children[i]);
        }
    }
};

template <class T, int M=4>
class BTree{
public:
    BTreeNode<T, M> *root;
    BTree(){
        root = new BTreeNode<T, M>;
    }
    void printBTree(){
        root->printBTree();
    }
    void insert(T data){
        BTreeNode<T, M> *p;
        for (p=root; !p->isLeaf(); p = p->findBranch(data));
        if (p->keyNum > M-1){
            cout<<"Leaf overflow, no place to insert.";
            return;
        }
        int i;
        for (i=p->keyNum-1; i>=0 && p->keys[i] > data; i--){
            p->keys[i+1] = p->keys[i];
        }
        p->keys[i+1] = data;
        p->keyNum += 1;
        if (p->isOverflow()){
            p->split();
        }
        root = root->findRoot();
    }
    BTreeNode<T, M> *searchKey(T data){
        BTreeNode<T, M> *p = root;
        int i;
        while (!p->isLeaf()){
            for (i = p->keyNum-1; i>=0 && p->keys[i] > data; i--);
            if (p->keys[i] == data)
                return p;
            else
                p = p->children[i+1];
        }
        return NULL;
    }
    void deleteKey(T data){
        BTreeNode<T, M> *p = root;
        int i;
        while (p){
            for (i = p->keyNum-1; i>=0 && p->keys[i] > data; i--){
            }
            if (i>=0 && p->keys[i] == data){
//                 cout<<"key found "<<data<<" i="<<i<<endl;
                break;
            }
            else{
                p = p->children[i+1];
            }
        }
        if (!p){
            cout<<"Key not found."<<endl;
            return;
        }
        BTreeNode<T, M> *q = NULL;
        if (p->isLeaf()){
            q = p;
            for (int j=i; j < p->keyNum; j++){
                p->keys[j] = p->keys[j+1];
            }
            p->keyNum -= 1;
            if (!p->parent){
                return;
            }
        }
        else{
            for (q = p->children[i]; !q->isLeaf(); q = q->children[q->keyNum]);
            p->keys[i] = q->keys[q->keyNum-1];
            q->keyNum -= 1;
        }
        root = q->rebalance();
    }
    
    ~BTree(){
        delete root;
    }
};

int main(){
    BTree<int> rt;
    for (int i=0; i<20; i++){
        rt.insert(i);
    }
    rt.printBTree();
    for (int i=0; i<10; i++){
        cout<<"deleting "<<2*i<<endl;
        rt.deleteKey(2*i);
        rt.printBTree();
    }
    for (int i=19; i>=0; i-=2){
        cout<<"deleting "<<i<<endl;
        rt.deleteKey(i);
        rt.printBTree();
    }
}
