#pragma once

#ifdef CICMS_CTREE

#include <iostream>
#include <vector>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/classification.hpp> // for is_any_of

#include "ctree/signal.hpp"
#include "Node.h"
#include "Configurator.h"
#include "../Builder.h"

// ‎#define ADD_TYPE(name) ADD_TYPE("name", name)
// ‎#define ADD_TYPE(name, type) this->add_default_instantiator<type>(name)
// ‎ADD_TYPE("UiButton", ui::Button);
// ‎ADD_TYPE(VideoSource);

namespace cms { namespace cfg { namespace ctree {

  template<typename CfgT>
  class Builder : public ::cms::cfg::Builder<Node> {

    public: // types

      typedef Node NodeT;
      typedef std::map<string, string> CfgDataRaw;
      typedef Model CfgData;

      struct BuildArgs {
        ::ctree::Node* node;
        void* object;
        CfgData* data;
        BuildArgs(::ctree::Node* n, void* o, CfgData* dat)
          : node(n), object(o), data(dat){}
      };

      class Selection {
        public:
          Selection(NodeT& node) : node(&node) {
          }

          NodeT* getNode(){
            return node;
          }

          template<typename ObjT>
          ObjT* get(const string& path){
            std::vector<string> strs;
            boost::split(strs,path,boost::is_any_of("."));
            unsigned int counter = 0;

            std::string name = strs[0];

            // loop over each child to find the one with this name
            for(auto child : *node){
              auto n = (NodeT*)child;

              if (n->getName() == name) {
                // no more sub-names? return this object for this child
                if(strs.size() == 1){
                  return (ObjT*)n->template getObject<ObjT>(n);
                }

                // remove first name (one we just found) and move to on level deeper
                strs.erase(strs.begin());
                return std::make_shared<Selection>(*n)->template get<ObjT>(boost::algorithm::join(strs, "."));
              }
            }

            return NULL;
          }

          template<typename ObjT>
          ObjT* getSibling(const string& path){
            if (!node->parent()) {
              return NULL;
            }

            Selection sel(*(NodeT*)node->parent());
            return sel.get<ObjT>(path);
          }

          template<typename ObjT>
          void attach(ObjT* obj){
            auto objNode = (NodeT*)NodeT::fromObj<ObjT>(obj);
            node->add(*objNode);
          }

        private:
          NodeT* node;
      };

    public: // lifespan methods

      Builder() : bPrivateConfigurator(true) {
        this->configurator = new CfgT(this->getModelCollection());
        this->setup();
      }

      Builder(CfgT& cfg) : bPrivateConfigurator(false), configurator(&cfg) {
        this->setup();
      }

      ~Builder() {
        if(this->configurator && this->bPrivateConfigurator){
          delete this->configurator;
          this->configurator = NULL;
        }
      }

      void setup(){
        this->setChilderFunc([](NodeT& parent, NodeT& child){
          parent.add(child);
        });
      }

    public: // configuration methods

      template<typename T>
      void addDefaultInstantiator(const string& name){
        this->addInstantiator(name, [this, &name](CfgData& data){
          auto node = NodeT::create<T>(name);
          // create ouw object
          auto object = node->template getObject<T>();
          this->configurator->cfgWithModel(*object, data);
          // attach it to a ctree node
          this->configurator->cfgWithModel(*node, data);
          // emit signal
          BuildArgs args(node, object, &data);
          buildSignal.emit(args);
          // return result
          return node;
        });
      }

      CfgT* getConfigurator() { return configurator; }

    public: // hierarchy operations

      template<typename ObjT>
      ObjT* build(const string& id){
        auto node = ::cms::cfg::Builder<Node>::build(id);
        auto obj = node->template getObject<ObjT>();
        return obj;
      }

      void destroyNode(cms::cfg::ctree::Node* n){
        // std::cout << " - DESTROY: " << n->getName() << "(" << n->size() << " children)" << std::endl;

        // destroy all offspring
        // for(auto it = n->rbegin(); it != n->rend(); ++it) {
        while(n->size() > 0) {
          auto child = (cms::cfg::ctree::Node*)n->at(0);
          // std::cout << " child " << child->getName() << std::endl;
          this->destroyNode(child);
        }

        // remove from parent, if it has one
        ::ctree::Node* parent = n->parent();
        if(parent){
          parent->erase((::ctree::Node*)n);
        }

        n->destroy();
      }

      template<typename ObjT>
      void destroy(ObjT* obj){
        this->destroyNode(this->select(obj)->getNode());
      }

      template<typename SourceT>
      std::shared_ptr<Selection> select(SourceT* origin){
        // convert origin into a NodeT pointer
        return std::make_shared<Selection>(*NodeT::fromObj<SourceT>(origin));
      }

    public: // signals
      ::ctree::Signal<void(BuildArgs&)> buildSignal;

    protected: // helper methods

      std::string getName(CfgData& data){
        // if (data.has("name"))
        // return data.get("name");

        std::vector<string> strs;
        std::string id = data.getId();
        boost::split(strs,id,boost::is_any_of("."));
        return strs.back();
      }

    protected: // attributes
      CfgT* configurator;

    private: // attributes
      bool bPrivateConfigurator;
  };
}}}
#endif // CICMS_CTREE
