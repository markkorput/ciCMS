#pragma once

#include "cinder/Json.h"
#include "Model.h"
#include "Collection.h"

namespace cms {
    class ModelCollection;
    typedef shared_ptr<ModelCollection> ModelCollectionRef;

    class ModelCollection : public Collection<Model> {
    public:
        ModelCollection() : mIdAttributeName("id"){}

        shared_ptr<Model> findByAttr(const std::string& attr, const std::string& value, bool createIfNotExist = false);
        shared_ptr<Model> findById(const std::string& value, bool createIfNotExist = false);

        void filter(const string& attr, const string& value){
            Collection<Model>::filter([attr, value](Model& model) -> bool {
                return model.get(attr) == value;
            });
        }
    private:
        std::string mIdAttributeName;
    };
}
namespace cms {
#ifndef CINDER_MSW

    // non-windows; implementations in ModelCollection.cpp

    template<>
    shared_ptr<Model> CollectionJsonLoader<Model>::findMatch(ci::JsonTree& jsonTree, CollectionBase<Model>& collection);

    template<>
    bool CollectionJsonLoader<Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef);

    template<>
    ci::JsonTree CollectionJsonWriter<Model>::getItemJsonTree(shared_ptr<Model> itemRef);

#else
    template<>
    shared_ptr<Model> CollectionJsonLoader<Model>::findMatch(ci::JsonTree& jsonTree, CollectionBase<Model>& collection){
    std::string id;

    if(jsonTree.hasChild("id")) {
      id = jsonTree.getValueForKey("id");
    } else if (jsonTree.hasParent() && jsonTree.getParent().getNodeType() == ci::JsonTree::NODE_OBJECT) {
      id = jsonTree.getKey();
    } else {
      return nullptr;
    }

    return collection.first([&id](shared_ptr<Model> modelRef) {
      return modelRef->get("id") == id;
    });
    }

    template<>
    bool CollectionJsonLoader<Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef){
      for (int idx = 0; idx < jsonTree.getNumChildren(); idx++) {
        ci::JsonTree subTree = jsonTree.getChild(idx);
        itemRef->set(subTree.getKey(), subTree.getValue());
      }

      if (!jsonTree.hasChild("id")) {
          itemRef->set("id", jsonTree.getKey());
      }

      return true;
    }

    template<>
    ci::JsonTree CollectionJsonWriter<Model>::getItemJsonTree(shared_ptr<Model> itemRef){
        ci::JsonTree tree;
        itemRef->each([&](const string& attr, const string& value) {
            tree.addChild(ci::JsonTree(attr, value));
        });

        return tree;
    }
#endif
}
