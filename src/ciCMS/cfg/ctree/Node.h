#ifdef CICMS_CTREE
#pragma once

#include <iostream>
#include "ctree/node.h"
#include "ctree/signal.hpp"

namespace cms { namespace cfg { namespace ctree {

  class Node : public ::ctree::Node {

    public:

      template<typename T>
      static Node* fromObj(T* obj) {
        return (Node*)((long)obj - sizeof(Node));
      }

      template<typename T>
      static Node* create(const std::string& name){
        void* mem = std::malloc(sizeof(Node) + sizeof(T));
        // long memI = (long)mem;
        void* objMem = (T*)((long)mem + sizeof(Node));
        T* obj = new (objMem)T();
        Node* n = new (mem) Node(obj, name);

        // T* obj = n + sizeof(Node);
        // n->object = obj;
        // n->name = name;
        // n->destroyFunc = [obj](){
        //   delete (T)obj;
        //   delete n;
        // });
        return n;
      }

      template<typename T>
      static Node* create(T* obj, const std::string& name){
        auto n = new Node(obj);
        // n->destroySignal.connect([](Node& node){
        //   auto obj = node.getObject<T>();
        //   if(obj) delete obj;
        //   node.object = NULL;
        // });

        return n;
      }

    protected: // private(!) constructor; use static create method to instantiate

      template<typename ObjT>
      Node(ObjT* obj, const std::string& name) : object(obj), name(name) {
        // std::cout << "CREACREA CREA: " << name << " = " << obj << std::endl;
        // this->destroySignal.connect([this](Node& node){
        //   auto obj = node.getObject<ObjT>();
        //   // std::cout << "objbojbjbobjbjobjo: " << obj << std::endl;
        //   std::cout << "DEL DEL DEL2 " <<this->name << " = " << obj << std::endl;
        //   if(obj) delete obj;
        //   node.object = NULL;
        // });
      }

    public:
      // ~Node(){
      //   this->destroySignal.emit(*this);
      // }

      const std::string& getName(){ return name; }

    public: // methods
      template<typename T>
      T* getObject(){ return (T*)object; }

    public: // signals
      std::function<void()> destroyFunc;

    private:
      void* object;
      // TODO; add some string type attribute for runtime type-checking?
      std::string name;
  };
}}}

#endif // CICMS_CTREE
