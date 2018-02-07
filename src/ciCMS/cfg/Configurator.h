#pragma once

#include <iostream>
#include "ciCMS/ModelCollection.h"

namespace cms { namespace cfg {

  class Configurator {
    public: // types

      typedef Model CfgData;
      typedef std::map<string, string> CfgDataRaw;
      typedef std::function<void*(const std::string&)> ObjectFetcherFunc;

      // class Applier {
      //   public:
      //     Applier(Configurator* cfg, CfgData& data) : cfg(cfg), data(data){
      //     };
      //
      //     template<typename ObjT>
      //     void to(ObjT* obj){
      //       cfg->cfgWithModel(*obj, data);
      //     }
      //
      //   private:
      //     Configurator* cfg;
      //     CfgData& data;
      // };

    public: // lifecycle methods
      Configurator() : bActive(false), bPrivateModelCollection(true){
        modelCollection = new ModelCollection();
      }

      Configurator(ModelCollection& mc) : bActive(false), bPrivateModelCollection(false), modelCollection(&mc) {
      }

      ~Configurator(){
        if(bPrivateModelCollection && modelCollection){
          delete modelCollection;
          modelCollection = NULL;
        }
      }

    public: // getters and setters

      bool isActive() const { return this->bActive; }
      void setActive(bool active) {
        this->bActive = active;
        if (active)
          std::cout << "Configurator::setActive; configurator set to active = TRUE, use for development only!" << std::endl;
      }

      ModelCollection& getModelCollection() { return *this->modelCollection; }

      void apply(Model& model, Model::ModelTransformFunctor func, void* activeCallbackOwner = NULL){
        model.transform(func, activeCallbackOwner, this->bActive);
      }

      // shared_ptr<Applier> apply(CfgData& data) {
      //   return std::make_shared<Applier>(this, data);
      // }

      void setObjectFetcher(ObjectFetcherFunc func){
        this->objectFetcherFunc = func;
      }

      inline void* getObjectPointer(const std::string& id) {
        return this->objectFetcherFunc ? this->objectFetcherFunc(id) : NULL;
      }

      template<typename ObjT>
      ObjT* getObject(const std::string& id) {
        return (ObjT*)this->getObjectPointer(id);
      }

    public: // cfgs

      template<typename T>
      void cfgWithModel(T& c, Model& model){
        this->apply(model, [this, &c](ModelBase& mod){
          this->cfg(c, mod.attributes());
        });
      }

      void cfg(Configurator& c, const std::map<string, string>& data){
        Model m;
        m.set(data);
        m.withBool("active", [&c](const bool& v){ c.setActive(v); });
      }

    private: // attributes
      bool bActive, bPrivateModelCollection;
      ModelCollection* modelCollection;
      ObjectFetcherFunc objectFetcherFunc;
  };
}}
