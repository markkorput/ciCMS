#pragma once

#include "cinder/Json.h"
#include "Model.h"
#include "Collection.h"

namespace cms {
    class ModelCollection : public Collection<Model> {

    };
}

// Specialization implementations can be found in ModelCollection.cpp (obviously)
template<>
shared_ptr<cms::Model> cms::CollectionJsonLoader<cms::Model>::findMatch(ci::JsonTree& jsonTree, CollectionBase<Model>& collection);

template<>
bool cms::CollectionJsonLoader<cms::Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef);

template<>
ci::JsonTree cms::CollectionJsonWriter<cms::Model>::getItemJsonTree(shared_ptr<cms::Model> itemRef);
