#pragma once

#include "ciCMS/ModelCollection.h"

namespace cms { namespace cfg {
  class Builder {
    public:
      typedef std::function<void*(Model&)> InstantiatorFunc;
      typedef std::function<void(void*, void*)> ConnectorFunc;

      class Instantiator {
        public:
          InstantiatorFunc func;
          string className, rootClassName;
          Instantiator(const string& className, const string& rootClassName, InstantiatorFunc func)
            : func(func), className(className), rootClassName(rootClassName){}
      };

      typedef std::shared_ptr<Instantiator> InstantiatorRef;

      class Connector {
        public:
          ConnectorFunc func;
          string parentClass, childClass;
          Connector(const string&childClass, const string& parentClass, ConnectorFunc func)
            : func(func), parentClass(parentClass), childClass(childClass){}
      };

      typedef std::shared_ptr<Connector> ConnectorRef;

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

      void addInstantiator(const string& name, InstantiatorFunc func){ this->addInstantiator(name, name, func); }
      void addInstantiator(const string& name, const string& rootName, InstantiatorFunc func);

      void addConnector(const string& name, ConnectorFunc func){ this->addConnector(name, name, func); }
      void addConnector(const string& childName, const string& parentName, ConnectorFunc func);

      template<typename T>
      T* build(const string& id, bool recursive=true){
        auto pointer = this->doBuild(id, recursive);
        return (T*)(pointer);
      }

    private:
      void* doBuild(const string& id, bool recursive=true);
      void withEachChildId(const string& parentId, std::function<void(const string& childId)> func);
      InstantiatorRef findInstantiator(const string& id);
      ConnectorRef findConnector(const string& childClass, const string& parentClass);

    private:
      bool bPrivateModelCollection;
      ModelCollection* modelCollection;
      std::vector<InstantiatorRef> instantiators;
      std::vector<ConnectorRef> connectors;
  };
}}
