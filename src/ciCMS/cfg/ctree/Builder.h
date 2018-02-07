#pragma once

#ifdef CICMS_CTREE

#include <iostream>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of

#include "ctree/signal.hpp"
#include "Node.h"
#include "../Builder.h"

// ‎#define ADD_TYPE(name) ADD_TYPE("name", name)
// ‎#define ADD_TYPE(name, type) this->add_default_instantiator<type>(name)
// ‎ADD_TYPE("UiButton", ui::Button);
// ‎ADD_TYPE(VideoSource);

namespace cms { namespace cfg { namespace ctree {

  template<typename CfgT>
  class Builder : public ::cms::cfg::Builder<Node> {

    public: // types
      typedef Node NodeT;
      typedef std::map<string, string> CfgDataRaw;
      typedef Model CfgData;

      struct BuildArgs {
        ::ctree::Node* node;
        void* object;
        CfgData* data;

        BuildArgs(::ctree::Node* n, void* o, CfgData* dat)
          : node(n), object(o), data(dat){}
      };

      struct DestroyArgs {
        ::ctree::Node* node;
        void* object;

        DestroyArgs(::ctree::Node* n, void* o) : node(n), object(o) {}
      };

      #include "builder/Registry.hpp"
      #include "builder/Selection.hpp"

    public: // lifespan methods

      Builder() : bPrivateConfigurator(true) {
        this->configurator = new CfgT(this->getModelCollection());
        this->setup();
      }

      Builder(CfgT& cfg) : bPrivateConfigurator(false), configurator(&cfg) {
        this->setup();
      }

      ~Builder() {
        if(this->configurator && this->bPrivateConfigurator){
          delete this->configurator;
          this->configurator = NULL;
        }
      }

      void setup(){
        this->setChilderFunc([](NodeT& parent, NodeT& child){
          parent.add(child);
        });
      }

    public: // configuration methods

      template<typename T>
      void addDefaultInstantiator(const string& name){
        this->addInstantiator(name, [this, &name](CfgData& data){
          auto node = NodeT::create<T>(this->getName(data));
          // create ouw object
          auto object = node->template getObject<T>();
          this->configurator->cfgWithModel(*object, data);
          // attach it to a ctree node
          this->configurator->cfgWithModel(*node, data);
          // emit signal
          BuildArgs args(node, object, &data);
          buildSignal.emit(args);
          // return result
          return node;
        });
      }

      CfgT* getConfigurator() { return configurator; }

    public: // hierarchy operations

      template<typename ObjT>
      ObjT* build(const string& id){
        auto node = ::cms::cfg::Builder<Node>::build(id);
        auto obj = node->template getObject<ObjT>();
        return obj;
      }

      void destroyNode(cms::cfg::ctree::Node* n){
        // std::cout << " - DESTROY: " << n->getName() << "(" << n->size() << " children)" << std::endl;

        // destroy all offspring
        // for(auto it = n->rbegin(); it != n->rend(); ++it) {
        while(n->size() > 0) {
          auto child = (cms::cfg::ctree::Node*)n->at(0);
          // std::cout << " child " << child->getName() << std::endl;
          this->destroyNode(child);
        }

        // remove from parent, if it has one
        ::ctree::Node* parent = n->parent();
        if(parent){
          parent->erase((::ctree::Node*)n);
        }

        DestroyArgs args(n, n->getObjectPointer());
        destroySignal.emit(args);

        n->destroy();
      }

      template<typename ObjT>
      void destroy(ObjT* obj){
        this->destroyNode(this->select(obj)->getNode());
      }

      template<typename SourceT>
      std::shared_ptr<Selection> select(SourceT* origin){
        // convert origin into a NodeT pointer
        return std::make_shared<Selection>(*NodeT::fromObj<SourceT>(origin));
      }

    public: // signals
      ::ctree::Signal<void(BuildArgs&)> buildSignal;
      ::ctree::Signal<void(DestroyArgs&)> destroySignal;

    protected: // helper methods

      std::string getName(CfgData& data){
        if (data.has("_name"))
          return data.get("_name");

        std::vector<string> strs;
        std::string id = data.getId();
        boost::split(strs,id,boost::is_any_of("."));
        return strs.back();
      }

    protected: // attributes
      CfgT* configurator;

    private: // attributes
      bool bPrivateConfigurator;
  };
}}}
#endif // CICMS_CTREE
