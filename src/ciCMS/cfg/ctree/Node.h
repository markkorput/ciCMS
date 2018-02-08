#ifdef CICMS_CTREE
#pragma once

#include <iostream>
#include "ctree/node.h"
#include "ctree/signal.hpp"

namespace cms { namespace cfg { namespace ctree {

  class Node : public ::ctree::Node {

    public: // static methods

      template<typename T>
      static Node* fromObj(T* obj) {
        return (Node*)((long)obj - sizeof(Node));
      }

      template<typename T>
      static Node* create(const std::string& name){
        // allocate Node and Object in single block of memory
        // this way we can always calculate the Node's address from
        // the corresponding Object's address.
        void* mem = std::malloc(sizeof(Node) + sizeof(T));
        // call the constructors of the Node and Object, but
        // assign memory address manually
        T* obj = new ((void*)((long)mem + sizeof(Node)))T();
        Node* n = new (mem) Node(name);

        // std::cout << "CREATED NODE with name '" << name << "' " << obj << std::endl;

        // register a destroyFunc, to be executd when n->destroy is called
        // this way the caller does not need to know about the type of object
        // that the node points to, but it will still do the proper destruction
        n->destroyFunc = [mem, obj, n](){
          // std::cout << "DELETING NODE with name: '" <<n->getName() << "': " << obj << std::endl;
          obj->~T();
          n->~Node();
          std::free(mem);
        };

        return n;
      }

    protected: // private(!) constructor; use static create method to instantiate

      Node(const std::string& name) : name(name) {
      }

    public:

      void destroy() {
        if (this->destroyFunc) {
          this->destroyFunc();
        } else {
          std::cerr << "no destroyFunc set on Node with name: " << name;
        }
      }

      const std::string& getName(){ return name; }

    public: // methods

      template<typename T>
      T* getObject(){ return (T*)((long)this + sizeof(Node)); }

      void* getObjectPointer(){ return (void*)((long)this + sizeof(Node)); }

    private:
      std::function<void()> destroyFunc = nullptr;
      // TODO; add some string type attribute for runtime type-checking?
      std::string name;
  };
}}}

#endif // CICMS_CTREE
