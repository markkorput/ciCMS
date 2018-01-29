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

  class Builder : ::cms::cfg::Builder<Node> {

    public: // types

      typedef std::map<string, string> CfgDataRaw;
      typedef Model CfgData;

      struct BuildArgs {
        ::ctree::Node* node;
        void* object;
        CfgData* data;
        BuildArgs(::ctree::Node* n, void* o, CfgData* dat)
          : node(n), object(o), data(dat){}
      };

    public:

      Builder() {
        this->configurator = new Configurator();
      }

      ~Builder() {
        if(this->configurator){
          delete this->configurator;
          this->configurator = NULL;
        }
      }

    protected:

      template<typename T>
      void addDefaultInstantiator(const string& name){
        this->addInstantiator(name, [this](const CfgData& data){
          // create ouw object
          auto object = new T();
          this->configurator->cfgWithModel(*object, data);
          // attach it to a ctree node
          auto node = Node::create(object);
          this->configurator->cfgWithModel(*node, data);
          // emit signal
          BuildArgs args(node, object, data);
          buildSignal.emit(args);
          // return result
          return node;
        });
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

    public:
      ::ctree::Signal<void(BuildArgs&)> buildSignal;

    protected:
      Configurator* configurator;
  };
}}}
#endif CICMS_CTREE
