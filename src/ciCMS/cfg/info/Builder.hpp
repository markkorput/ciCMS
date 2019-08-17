#pragma once

#include "Port.hpp"
#include "PortBuilder.hpp"

namespace cms { namespace cfg { namespace info {

  template<typename T>
  class Builder {

    public:

      template<typename V>
      PortBuilder<T,V>& attr(const std::string& id) { return addPort<V>(id, Port::FLAG_INOUT); }

      template<typename V>
      PortBuilder<T,V>& output(const std::string& id) { return addPort<V>(id, Port::FLAG_OUT); }

      template<typename V>
      PortBuilder<T,V>& addPort(const std::string& id, int flags) {
        auto output = new PortBuilder<T,V>(id, flags);
        outputs.push_back((PortBuilderBase*)output);
        return *output;
      }

    public:
      std::vector<PortBuilderBase*> outputs;
  };
}}}