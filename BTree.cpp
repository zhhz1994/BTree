#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

template <class T, int M=4>
class BTree{
    int m = ceil(M/2); 
public:
    int keyNum = 0;
    T keys[M];
    BTree *parent = NULL;
    BTree *children[M+1] = {NULL};
    BTree(){};
    BTree(T *start, int num){
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
        return keyNum >= M;
    }
    void setChild(BTree *child, int num){
        children[num] = child;
        if (child){
            child->parent = this;
        }
    }
    void passChildren(int begin, int end, BTree<T> *newParent, int newPosition){
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
    void transfer(int begin, int end, BTree<T> *newNode, int newPosition){
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
    BTree<T> *findRoot(){
        BTree<T> *p;
        for (p=this; p->parent; p=p->parent);
        return p;
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

    BTree<T> *findInsertChild(T data){
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
            (new BTree<T>)->setChild(this, 0);
            parent->keys[0] = keys[m-1];
            parent->keyNum = 1;
            
            keyNum--;
            parent->setChild(new BTree<T>, 1);
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
            parent->setChild(new BTree<T>, i+2);
            transfer(m, M, parent->children[i+2], 0);
            if (parent->isOverflow()){
                parent->split();
            }
        }
    }

    BTree *insert(T data){
        BTree<T> *p;
        for (p=this; !p->isLeaf(); p = p->findInsertChild(data));
        if (p->keyNum > M-1){
            cout<<"Leaf overflow, no place to insert.";
            return NULL;
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
        return findRoot();
    }
    
    BTree<T> *leftRotate(int num){
        cout<<"left rotating"<<endl;
        if (isLeaf()){
            cout<<"Can not rotate a leaf."<<endl;
            return this;
        }
        BTree<T> *leftChild = children[num];
        BTree<T> *rightChild = children[num+1];
        
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
    BTree<T> *rightRotate(int num){
//         cout<<"right rotating"<<endl;
        if (isLeaf()){
            cout<<"Can not rotate a leaf."<<endl;
            return this;
        }
        BTree<T> *leftChild = children[num];
        BTree<T> *rightChild = children[num+1];
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
    BTree<T> *merge(int num){
        cout<<"merging"<<endl;
        BTree<T> *leftChild = children[num];
        BTree<T> *rightChild = children[num+1];
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
    void deleteNode(int num){
        int i = num;
        BTree<T> *leftChild;
        BTree<T> *rightChild;
        BTree<T> *p = this;
        while(!p->isLeaf()){
            leftChild = p->children[i];
            rightChild = p->children[i+1];
            if (rightChild->keyNum-1 >= m-1 && leftChild->keyNum+1 <= M-1){
                p = p->leftRotate(i);
                i = p->keyNum-1;
            }
            else{
                p = p->rightRotate(i);
                i = 0;
                if (p->isOverflow()){
                    p->split();
                }
            }
        }
        for (int j=i; j < p->keyNum-1; j++){
            p->keys[j] = p->keys[j+1];
        }
        p->keyNum -= 1;
    }
    void rebalance(){
        BTree<T> *leftChild;
        BTree<T> *rightChild;
        BTree<T> *p = this;
        cout<<"balancing: ";
        p->printNode();
        cout<<endl;
        int found=0;
        if(!p->isLeaf()){
            int i;
            for (i=0; i < keyNum; i++){
                leftChild = children[i];
                rightChild = children[i+1];
                if (leftChild->keyNum < m-1){
                    found = 1;
                    cout<<"target found: ";
                    p->printBTree();
                    if (rightChild->keyNum > m-1){
                        leftRotate(i);
                        rightChild->rebalance();
                    }
                    else{
                        p = p->merge(i);
                        cout<<"new p: "; p->printNode();cout<<endl;
                        p->rebalance();
                    }
                    break;
                }
            }
            if (!found){
                for (int j=0; j < keyNum; j++){
                    printNode();
                    cout<<" j = "<<j<<": ";children[j]->printNode();cout<<endl;
                    children[j]->rebalance();
                }
            }
        }
    }
    ~BTree(){
        if (!this->isLeaf()){
            for (int i=0; i < M+1; i++)
                if (children[i])
                    delete(children[i]);
        }
    }
};

int main(){
    BTree<int> *rt = new BTree<int>;
    for (int i=0; i<20; i++){
        rt = rt->insert(i);
    }
//     rt->insert(8);
//     rt->insert(8);
//     rt->insert(9);
//     rt->insert(9);
//     rt->insert(9);
//     rt->insert(17);
    rt->printBTree();
    BTree<int> *p;
//     cout<<"After rotation:"<<endl;
//     p=rt->leftRotate(0);
//     rt->printBTree();
//     p=p->rightRotate(1);
//     rt->printBTree();
    
    cout<<"After deletion:"<<endl;
    rt->deleteNode(2);
    rt = rt->findRoot();
    rt->printBTree();
    rt->rebalance();
    rt->printBTree();

    delete rt;
    
//     rt = new BTree<int>;
//     for (int i=30; i>=0; i--){
//         rt = rt->insert(i);
//     }
//     rt->printBTree();
// 
// 
//     cout<<"After deletion:"<<endl;
//     rt->deleteNode(0);
//     rt->printBTree();
}
