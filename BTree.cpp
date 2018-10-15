#include <iostream>
#include <vector>
#include <cmath>
using namespace std;

template <class T, int M=4>
class BTree{
    int m = ceil(M/2); 
public:
    int keyNum = 0;
    T keys[M-1+1];
    BTree *parent = NULL;
    BTree *children[M-1+2] = {NULL};
    BTree(){};
    BTree(T *start, int num){
        if (num <= M-1+1){
            copy(start, start + num, keys);
            keyNum = num;
        }
        else 
            cout<<"Invalid number of elements."<<endl;
    }
    int isLeaf(){
        for (int i=0; i<M-1+1; i++){
            if (children[i])
                return 0;
        }
        return 1;
    }
    bool isOverflow(){
        return keyNum >= M-1+1;
    }
    void setChild(BTree *child, int num){
        children[num] = child;
        child->parent = this;
    }
    void printNode(){
        cout<<"(";
        for (int i=0; i<keyNum; i++)
            cout<<keys[i]<<", ";
        cout<<"\b\b) ";
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
//                 if (children[i]->parent != this)
//                     cout<<" has wrong parent "<<children[i]->parent<<" instead of "<<this;
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
        for (i=0; i < keyNum; i++)
            if (keys[i] > data)
                break;
        return children[i];
    }
    void split(){
        if (keyNum != M){
            cout<<"No need to split."<<endl;
            return;
        }
        if (parent == NULL){    //this node is the root
            cout<<"spliting the root."<<endl;
            (new BTree<T>)->setChild(this, 0);
            parent->keys[0] = keys[m-1];
            parent->keyNum = 1;
            
            parent->setChild(new BTree<T>(keys+m, M-m), 1);
            for (int i=m; i < M+1 && children[i]!=NULL; i++){
                parent->children[1]->setChild(children[i], i-m);
                children[i] = NULL;
            }
            keyNum = m-1;
        }
        else if (parent->keyNum <= M-1){
            int i;
            for (i = parent->keyNum-1; parent->keys[i] > keys[m-1]; i--){
                parent->keys[i+1] = parent->keys[i];
                parent->setChild(parent->children[i+1], i+2);
            }
            parent->keys[i+1] = keys[m-1];
            parent->keyNum += 1;
            
            parent->setChild(new BTree<T>(keys+m, M-m), i+2);
            for (int j=m; j < M+1 && children[j]!=NULL; j++){
                parent->children[i+2]->setChild(children[j], j-m);
                children[j] = NULL;
            }
            keyNum = m-1;
            if (parent->isOverflow()){
                parent->split();
            }
        }
    }
    BTree *insert(T data){
        if (!this->parent && this->keyNum == 0){
            this->keys[0] = data;
            this->keyNum += 1;
            return this;
        }
        BTree<T> *p;
        for (p=this; !p->isLeaf(); p = p->findInsertChild(data));
        if (p->keyNum > M-1){
            cout<<"Leaf overflow, no place to insert.";
            return NULL;
        }
        int i;
        for (i=p->keyNum-1; i>=0 && p->keys[i] > data; i--)
            p->keys[i+1] = p->keys[i];
        p->keys[i+1] = data;
        p->keyNum += 1;
        if (p->isOverflow())
            p->split();
        
        for (p=this; p->parent != NULL; p = p->parent);
        return p;
    }
    ~BTree(){
        if (!this->isLeaf()){
            for (int i=0; i < M-1+2; i++)
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
    rt->printBTree();
    delete rt;
    rt = new BTree<int>;
    
    for (int i=20; i>=0; i--){
        rt = rt->insert(i);
    }
    rt->printBTree();
}
