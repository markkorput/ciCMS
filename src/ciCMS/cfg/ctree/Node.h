#ifdef CICMS_CTREE
#pragma once

#include "ctree/node.h"
#include "ctree/signal.hpp"

namespace cms { namespace cfg { namespace ctree {

  class Node : public ::ctree::Node {

    public:

      template<typename T>
      static Node* create(T* obj){
        auto n = new Node(obj);

        n->destroySignal.connect([](Node& node){
          auto obj = node.getObject<T>();
          if(obj) delete obj;
          node.object = NULL;
        });

        return n;
      }

    private: // private(!) constructor; use static create method to instantiate

      Node(void* obj) : object(obj) {
      }

    public:
      ~Node(){
        this->destroySignal.emit(*this);
      }

    public: // methods
      template<typename T>
      T* getObject(){ return (T*)object; }

    public: // signals
      ::ctree::Signal<void(Node&)> destroySignal;

    private:
      void* object;
  };
}}}

#endif // CICMS_CTREE
