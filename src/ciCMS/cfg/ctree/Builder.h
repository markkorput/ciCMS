#pragma once

#ifdef CICMS_CTREE

#include "ctree/signal.hpp"
#include "Node.h"
#include "Configurator.h"
#include "../Builder.h"

// ‎#define ADD_TYPE(name) ADD_TYPE("name", name)
// ‎#define ADD_TYPE(name, type) this->add_default_instantiator<type>(name)
// ‎ADD_TYPE("UiButton", ui::Button);
// ‎ADD_TYPE(VideoSource);

namespace cms { namespace cfg{ namespace ctree {

  template<typename CfgT>
  class Builder : public ::cms::cfg::Builder<Node> {

    public: // types

      // TODO; support custom Node extension types
      template<typename ObjT>
      class Wrapper : public ObjT, public Node {
      public:
        Wrapper() : Node((ObjT*)this) {
        }
      };

      typedef std::map<string, string> CfgDataRaw;
      typedef Model CfgData;

      struct BuildArgs {
        ::ctree::Node* node;
        void* object;
        CfgData* data;
        BuildArgs(::ctree::Node* n, void* o, CfgData* dat)
          : node(n), object(o), data(dat){}
      };

    public: // lifespan methods

      Builder() : bPrivateConfigurator(true) {
        this->configurator = new CfgT(this->getModelCollection());
      }

      Builder(CfgT& cfg) : bPrivateConfigurator(false), configurator(&cfg) {
      }

      ~Builder() {
        if(this->configurator && this->bPrivateConfigurator){
          delete this->configurator;
          this->configurator = NULL;
        }
      }

    public: // configuration methods

      template<typename T>
      void addDefaultInstantiator(const string& name){
        this->addInstantiator(name, [this](CfgData& data){
          auto wrapper = new Wrapper<T>();
          // create ouw object
          auto object = (T*)wrapper;

          // this->configurator->apply(data)->to(object);
          this->configurator->cfgWithModel(*object, data);
          // attach it to a ctree node
          auto node = (Node*)wrapper;
          this->configurator->cfgWithModel(*node, data);
          // emit signal
          BuildArgs args(node, object, &data);
          buildSignal.emit(args);
          // return result
          return node;
        });
      }

    public: // hierarchy operations

      template<typename ObjT>
      ObjT* build(const string& id){
        // our instantiators "wrap" each requested class in a wrapper class,
        // which is basically the requested class together with a Node.
        // Our parent builder class only knows about the Node part, so here
        // we convert from Node to requested class (via the WrapperClass)
        auto node = ::cms::cfg::Builder<Node>::build(id);
        auto wrapper = (Wrapper<ObjT>*)node;
        // TODO; perform some runtime type check?
        auto obj = (ObjT*)wrapper;
        return obj;
      }

      void destroy(cms::cfg::ctree::Node* n){
        // destroy all offspring
        for(auto it = n->begin(); it != n->end(); it++)
          this->destroy((cms::cfg::ctree::Node*)*it);

        // remove from parent, if it has one
        ::ctree::Node* parent = n->parent();
        if(parent){
          parent->erase((::ctree::Node*)n);
        }

        delete n;
      }

    public: // signals
      ::ctree::Signal<void(BuildArgs&)> buildSignal;

    protected:
      bool bPrivateConfigurator;
      CfgT* configurator;
  };
}}}
#endif // CICMS_CTREE
