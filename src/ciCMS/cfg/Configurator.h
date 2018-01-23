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

    void cfg(Configurator& c, const std::map<string, string>& data){
      Model m;
      m.set(data);
      m.with("active", [&c](const bool& v){ c.setActive(v); });
    }

  private:
    bool bActive;
    ModelCollection* modelCollection;
  };
}}
