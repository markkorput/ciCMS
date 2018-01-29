#ifdef CICMS_CTREE
#pragma once

#include "ctree/node.h"
#include "ctree/signal.hpp"

namespace cms { namespace cfg { namespace ctree {

  class Node : public ::ctree::Node {

    public:

      template<typename T>
      static Node* create(T* obj, const std::string& name){
        auto n = new Node(obj);

        n->destroySignal.connect([](Node& node){
          auto obj = node.getObject<T>();
          if(obj) delete obj;
          node.object = NULL;
        });

        return n;
      }

    protected: // private(!) constructor; use static create method to instantiate

      Node(void* obj, const std::string& name) : object(obj), name(name) {
      }

    public:
      ~Node(){
        this->destroySignal.emit(*this);
      }

      const std::string& getName(){ return name; }

    public: // methods
      template<typename T>
      T* getObject(){ return (T*)object; }

    public: // signals
      ::ctree::Signal<void(Node&)> destroySignal;

    private:
      void* object;
      // TODO; add some string type attribute for runtime type-checking?
      std::string name;
  };
}}}

#endif // CICMS_CTREE
