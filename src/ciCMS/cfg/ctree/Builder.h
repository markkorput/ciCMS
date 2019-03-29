#pragma once

#ifdef CICMS_CTREE

#include "TreeBuilder.hpp"

// ‎#define ADD_TYPE(name) ADD_TYPE("name", name)
// ‎#define ADD_TYPE(name, type) this->add_default_instantiator<type>(name)
// ‎ADD_TYPE("UiButton", ui::Button);
// ‎ADD_TYPE(VideoSource);

namespace cms { namespace cfg { namespace ctree {

  /**
   * A builder class that builds instances of a Node class which can have an
   * instance of any desired class as flyweight, thus being able to build a
   * hierarchy out of a mix of any class without imposing requirements on
   * the used classes.
   */
  template<typename CfgT>
  class Builder : public TreeBuilder {

    public: // lifespan methods

      Builder() : bPrivateConfigurator(true) {
        this->configurator = new CfgT(this->getModelCollection());
        this->init();
      }

      Builder(CfgT& cfg) : bPrivateConfigurator(false), configurator(&cfg) {
        this->init();
      }

      ~Builder() {
        if(this->configurator && this->bPrivateConfigurator){
          delete this->configurator;
          this->configurator = NULL;
        }
      }

    private:

      void init(){
        auto objectFetcher = [this](const std::string& id){
          return this->registry->getById(id);
        };

        // give our configurator an object fetcher which looks for objects in our Registry
        configurator->setObjectFetcher(objectFetcher);
      }

    public: // configuration methods

      template<typename T>
      void addDefaultInstantiator(const string& name){
        this->addConfiguratorObjectInstantiator<T>(name);
      }

      template<typename T>
      void addConfiguratorObjectInstantiator(const string& name) {
        this->addInstantiator(name, [this, &name](CfgData& data){
          // create a node for in the hierarchy structure, with an
          // instance of the specified type attached to it
          auto node = NodeT::create<T>(this->getName(data));

          // get the attached object from the node
          auto object = node->template getObject<T>();

          // "configure" the object by passing it to our configurator
          this->configurator->apply(data, [this, object](ModelBase& mod){
            this->configurator->cfg(*object, mod.attributes());
          });

          // notify observer signal
          BuildArgs args(node, object, &data);
          buildSignal.emit(args);
          this->configurator->notifyNewObject(object, data);

          // return result
          return node;
        });
      }

      CfgT* getConfigurator() { return configurator; }

    protected:

      void notifyNewObject(void* obj, const CfgData& data) override {
        TreeBuilder::notifyNewObject(obj, data);
        this->configurator->notifyNewObject(obj, data);
      }

    private: // attributes
      bool bPrivateConfigurator;
      CfgT* configurator;
  };

}}}

#endif // CICMS_CTREE
