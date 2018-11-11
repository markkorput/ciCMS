#pragma once

#ifdef CICMS_CTREE

#include <iostream>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of

#include "ctree/signal.hpp"
#include "Node.h"
#include "../Configurator.h"
#include "../Builder.h"

// ‎#define ADD_TYPE(name) ADD_TYPE("name", name)
// ‎#define ADD_TYPE(name, type) this->add_default_instantiator<type>(name)
// ‎ADD_TYPE("UiButton", ui::Button);
// ‎ADD_TYPE(VideoSource);

namespace cms { namespace cfg { namespace ctree {

  class TreeBuilder : public ::cms::cfg::Builder<Node> {

    public: // types

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

      typedef Node NodeT;
      typedef Model CfgData;
      #include "builder/Selection.hpp"
      #include "builder/Registry.hpp"

    public:

      TreeBuilder() : bPrivateConfigurator(true) {
        this->configurator = new ::cms::cfg::Configurator(this->getModelCollection());

        this->setChilderFunc([](Node& parent, Node& child){
          parent.add(child);
        });

        // TODO; make this optional for performance optimization?
        this->registry = std::shared_ptr<Registry>(new Registry(this));

        auto objectFetcher = [this](const std::string& id){
          return this->registry->getById(id);
        };

        // give our configurator an object fetcher which looks for objects in our Registry
        configurator->setObjectFetcher(objectFetcher);
      }

      ~TreeBuilder() {
        if (bPrivateConfigurator && this->configurator != NULL) {
          delete this->configurator;
          this->configurator = NULL;
          bPrivateConfigurator = false;
        }
      }

      void reset() {
        while(registry->size() > 0) {
          auto node = registry->getNodeByIndex(0);
          this->destroyNode(node);
        }
      }

      /// Convenience which lets this builder configure itself
      /// (and its configurator) using one of the models in it model collection
      void cfg(const std::string& modelId) {
        this->configurator->cfg(
          *this->configurator,
          getModelCollection().findById(modelId, true)->attributes());
      }

      void cfg(cms::cfg::Cfg& cfg) {
        auto attrs = cfg.getAttributes();
        if (attrs) this->configurator->cfg(*this->configurator, *attrs);
      }

    public: // configuration methods

      template<typename T>
      void addCfgObjectInstantiator(const string& name) {
        this->addInstantiator(name, [this, &name](CfgData& data){
          // create a node for in the hierarchy structure, with an
          // instance of the specified type attached to it
          auto node = Node::create<T>(this->getName(data));

          // get the attached object from the node
          auto object = node->template getObject<T>();

          // "configure" the object by calling its cfg method
          this->configurator->apply(data, [this, object](ModelBase& mod){
            object->cfg(this->configurator->getCfg()->withData(mod.attributes()));
          });

          // notify observer signal
          BuildArgs args(node, object, &data);
          buildSignal.emit(args);
          this->notifyNewObject(object, data);

          // return result
          return node;
        });
      }

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
        return std::make_shared<Selection>(*Node::fromObj<SourceT>(origin));
      }

      const ::cms::cfg::Configurator* getConfigurator() const { return configurator; }

    protected: // helper methods

      std::string getName(CfgData& data){
        if (data.has("_name"))
          return data.get("_name");

        std::vector<string> strs;
        std::string id = data.getId();
        boost::split(strs,id,boost::is_any_of("."));
        return strs.back();
      }

      virtual void notifyNewObject(void* obj, const CfgData& data) {
        this->configurator->notifyNewObject(obj, data);
      }

    public: // signals
      ::ctree::Signal<void(BuildArgs&)> buildSignal;
      ::ctree::Signal<void(DestroyArgs&)> destroySignal;

    public: // for testing
      const std::shared_ptr<Registry> getRegistry() const { return registry; }

    protected:
      std::shared_ptr<Registry> registry;

    private:
      ::cms::cfg::Configurator* configurator = NULL;
      bool bPrivateConfigurator=false;

  };

}}}

#endif // CICMS_CTREE
