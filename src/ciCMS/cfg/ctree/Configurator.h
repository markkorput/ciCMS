#pragma once

#ifdef CICMS_CTREE

#include "Node.h"
#include "../Configurator.h"

namespace cms { namespace cfg { namespace ctree {

  // class Configurator : public ::cms::cfg::Configurator {
  // public:
  //
  //   Configurator() : ::cms::cfg::Configurator() {
  //   }
  //
  //   Configurator(ModelCollection& mc) : ::cms::cfg::Configurator(mc) {
  //   }
  //
  //   template<typename T>
  //   void cfgWithModel(T& c, Model& model){
  //     this->apply(model, [this, &c](ModelBase& mod){
  //       this->cfg(c, mod.attributes());
  //     });
  //   }
  // };
}}}

#endif
