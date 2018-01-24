#pragma once

#include "ciCMS/ModelCollection.h"

namespace cms { namespace cfg {
  template<class T>
  class Builder {

    public:

      typedef std::function<T*(Model&)> InstantiatorFunc;
      typedef std::function<void(T&, Model&)> ExtenderFunc;
      typedef std::function<void(T&, T&)> ChilderFunc;

      class Instantiator {
        public:
          InstantiatorFunc func;
          string name;
          Instantiator(const string& name, InstantiatorFunc func)
            : func(func), name(name){}
      };

      typedef std::shared_ptr<Instantiator> InstantiatorRef;

      class Extender {
        public:
          ExtenderFunc func;
          string name;
          Extender(const string& name, ExtenderFunc func)
            : func(func), name(name){}
      };

      typedef std::shared_ptr<Extender> ExtenderRef;

    public:
      Builder() : bPrivateModelCollection(true) {
        modelCollection = new ModelCollection();
      }

      ~Builder(){
        if(bPrivateModelCollection && modelCollection){
          delete modelCollection;
          modelCollection = NULL;
        }
      }

      ModelCollection& getModelCollection(){ return *this->modelCollection; }

      void addInstantiator(const string& name, InstantiatorFunc func);
      void addExtender(const string& name, ExtenderFunc func);
      void setChilderFunc(ChilderFunc func){ this->childerFunc = func; }

      T* build(const string& id, bool recursive=true);

    private:
      void withEachChildId(const string& parentId, std::function<void(const string& childId)> func);
      InstantiatorRef findInstantiator(const string& id);
      ExtenderRef findExtender(const string& name);

    private:
      bool bPrivateModelCollection;
      ModelCollection* modelCollection;
      std::vector<InstantiatorRef> instantiators;
      std::vector<ExtenderRef> extenders;
      ChilderFunc childerFunc = nullptr;
  };

  template<class T>
  T* Builder<T>::build(const string& id, bool recursive){
    auto model = this->modelCollection->findById(id, true);
    auto instantiator = this->findInstantiator(model->get("type"));

    if(!instantiator){
      std::cerr << "Could not find instantiator for type: " << model->get("type");
      return NULL;
    }

    auto instance = instantiator->func(*model);


    if(recursive){
      this->withEachChildId(id, [this, instance](const string& childId){
        auto childmodel = this->modelCollection->findById(childId, true);
        auto extender = this->findExtender(childmodel->get("type"));

        if(extender) {
          extender->func(*instance, *childmodel);
        } else {
          if(this->childerFunc){
            auto childInstance = this->build(childId, true);
            if(childInstance){
              this->childerFunc(*instance, *childInstance);
            }
          }
        }
      });
    }

    return instance;
  }

  template<class T>
  void Builder<T>::addInstantiator(const string& name, InstantiatorFunc func){
    this->instantiators.push_back(std::make_shared<Instantiator>(name, func));
  }

  template<class T>
  void Builder<T>::addExtender(const string& name, ExtenderFunc func){
    this->extenders.push_back(std::make_shared<Extender>(name, func));
  }

  template<class T>
  typename Builder<T>::InstantiatorRef Builder<T>::findInstantiator(const string& name){
    for(auto ref : this->instantiators)
      if(ref->name == name)
        return ref;

    return nullptr;
  }

  template<class T>
  typename Builder<T>::ExtenderRef Builder<T>::findExtender(const string& name){
    for(auto ref : this->extenders)
      if(ref->name == name)
        return ref;

    return nullptr;
  }

  template<class T>
  void Builder<T>::withEachChildId(const string& parentId, std::function<void(const string& childId)> func){
    const std::string divider = ".";
    const std::string prefix = parentId+divider;

    // loop over all item in our modelCollection
    for(unsigned int i=0; i<this->modelCollection->size(); i++){
      if(auto model = this->modelCollection->at(i)){
        const string cId = model->getId();
        // call the func with this item's ID if the id starts with the parentId prefix
        if(cId.find(prefix) == 0 && cId.find(divider, prefix.length()) == std::string::npos){
          func(cId);
        }
      }
    }
  }
}}
