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
            Collection<Model>::filter([&attr, &value](Model& model) -> bool {
                return model.get(attr) == value;
            });
        }
    private:
        std::string mIdAttributeName;
    };
}

// Specialization implementations can be found in ModelCollection.cpp (obviously)
template<>
shared_ptr<cms::Model> cms::CollectionJsonLoader<cms::Model>::findMatch(ci::JsonTree& jsonTree, CollectionBase<Model>& collection);

template<>
bool cms::CollectionJsonLoader<cms::Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef);

template<>
ci::JsonTree cms::CollectionJsonWriter<cms::Model>::getItemJsonTree(shared_ptr<cms::Model> itemRef);
