#pragma once

#include <iostream>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of
#include "cinder/app/App.h"
#include "ciCMS/ModelCollection.h"

namespace cms { namespace cfg {
  class BuilderBase {
    public:
      virtual void* doBuild(const string& id, bool recursive=true){ return NULL; }
  };

  template<class T>
  class Builder : public BuilderBase {

    public: // types

      typedef Model CfgData;
      typedef std::function<T*(CfgData&)> InstantiatorFunc;
      typedef std::function<void(T&, CfgData&)> ExtenderFunc;
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

    public: // methods

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

      T* build(const string& id, bool recursive=true){
        return (T*)this->doBuild(id, recursive);
      }

      void add(BuilderBase& builder){
        subBuilders.push_back(&builder);
      }

    protected:
      virtual void* doBuild(const string& id, bool recursive=true) override;
      std::string getType(const CfgData& data);

    private:
      void withEachChildId(const string& parentId, std::function<void(const string& childId)> func);
      InstantiatorRef findInstantiator(const string& id);
      ExtenderRef findExtender(const string& name);

    public:
      ci::signals::Signal<void(T& item, Model& model)> instantiateSignal;
    private:
      bool bPrivateModelCollection;
      ModelCollection* modelCollection;
      std::vector<InstantiatorRef> instantiators;
      std::vector<ExtenderRef> extenders;
      ChilderFunc childerFunc = nullptr;
      std::vector<BuilderBase*> subBuilders;
  };

  template<class T>
  void* Builder<T>::doBuild(const string& id, bool recursive){
    auto model = this->modelCollection->findById(id, true);
    std::string typ = this->getType(*model);
    auto instantiator = this->findInstantiator(typ);

    if(!instantiator){
      for(auto subBuilder : this->subBuilders) {
        auto result = subBuilder->doBuild(id, recursive);
        if(result)
          return result;
      }
      std::cerr << "Could not find instantiator for type: " << typ;
      return NULL;
    }

    std::cout << " - building: " << id << std::endl;

    auto instance = instantiator->func(*model);
    this->instantiateSignal.emit(*instance, *model);

    if(recursive){
      this->withEachChildId(id, [this, instance](const string& childId){
        // std::cout << "withEachChildId: " << childId;
        auto childmodel = this->modelCollection->findById(childId, true);
        auto extender = this->findExtender(this->getType(*childmodel));

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
  std::string Builder<T>::getType(const CfgData& data){
    if (data.has("type"))
      return data.get("type");

    std::vector<string> strs;
    std::string id = data.getId();
    boost::split(strs,id,boost::is_any_of("."));
    return strs.back();
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
