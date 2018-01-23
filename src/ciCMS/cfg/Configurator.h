#pragma once

#include "ciCMS/ModelCollection.h"

namespace cms { namespace cfg {
  class Configurator {
  public:
    Configurator() : bActive(false){
      modelCollection = new ModelCollection();
    }

    Configurator(ModelCollection& mc) : bActive(false), modelCollection(&mc) {
    }

    bool isActive() const { return this->bActive; }
    void setActive(bool active) { this->bActive = active; }

    ModelCollection& getModelCollection() { return *this->modelCollection; }

    void apply(Model& model, Model::ModelTransformFunctor func, void* activeCallbackOwner = NULL){
      model.transform(func, activeCallbackOwner, this->bActive);
    }

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

  private:
    bool bActive;
    ModelCollection* modelCollection;
  };
}}
