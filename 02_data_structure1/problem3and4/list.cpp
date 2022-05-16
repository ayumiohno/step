#include "list.hpp"
#include "node.hpp"
#include <iostream>

/******************************************************
ほしい機能(改良版)
* listの最後に追加
* 探索
* Node自身を削除
* listの末尾に移動 
*******************************************************/


void List::addLast(Node* node)
{
    std::cout << "call add last" << std::endl;
    node->setLeft(this->last, tag);
    std::cout << "set left" << std::endl;
    this->last->setRight(node, tag);
    this->last = node;
}

void List::deleteFirst()
{
    //this->first = nullptr;  //TODO hidoi
    if (this->first) {
        this->first->deleteNode();
    }
}

Node* List::find(Data* object)
{
    std::cout << "call find" << std::endl;
    this->first = nullptr;  //TODO hidoi
    if (!this->first) {
        //std::cout << "nullptr: " << this->first << " " << std::endl;
        return nullptr;
    }
    std::cout << "go to loop" << std::endl;
    this->first->findLoop(object, tag);
}
