#pragma once

#include <map>
#include <iostream>
#include "ciCMS/deserialise.h"
#include "ciCMS/ModelBase.h"

namespace cms { namespace cfg {

  template<typename T>
  class ConfigurableFacade {
    public:

      class AttrBase {
        public:
          AttrBase(const std::string& name) : name(name){}
          const std::string& getName() const { return name; }

          virtual void deserialise(T& obj, const std::string& str){
            // do nothing
          }

        private:
          std::string name;
      };

      template<typename AttrType>
      class Attribute : public AttrBase {
        public:
          typedef std::function<void(T& obj, const AttrType&)> SetterFunc;
          Attribute(const std::string& name, SetterFunc setter) : AttrBase(name), setterFunc(setter) {}

          AttrType convert(const std::string& str){}

          virtual void deserialise(T& obj, const std::string& str) override {
            // AttrType val;
            // cmsconv(str, val);
            // this->setterFunc(obj, val);
            this->setterFunc(obj, cms::deserialise<AttrType>(str));
          }

        private:
          SetterFunc setterFunc;
        };

      typedef std::shared_ptr<AttrBase> AttrBaseRef;

    public:

      virtual void cfg(T& obj, const std::map<std::string, std::string>& data){
        // bool any=false;

        for(auto it = data.begin(); it != data.end(); it++){
          for(auto attr : attributes){
            if(attr->getName() == it->first){
              attr->deserialise(obj, it->second);
              // any = true;
            }
          }
        }

        // return any;
      }

      template<typename AttrType>
      void add(const std::string &name, typename Attribute<AttrType>::SetterFunc func){
        auto attr = std::make_shared<Attribute<AttrType>>(name, func);
        this->attributes.push_back(attr);
      }

    protected:

      // TODO refactor reader method from ModelBase into separate class
      std::shared_ptr<ModelBase> reader(const std::map<std::string, std::string>& data){
        auto reader = std::make_shared<ModelBase>();
        reader->set(data);
        return reader;
      }

    private:
      std::vector<AttrBaseRef> attributes;
  };
}}
