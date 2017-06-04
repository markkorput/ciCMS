#pragma once

#include "cinder/Json.h"
#include "Model.h"
#include "Collection.h"

namespace cms {
    class ModelCollection : public Collection<Model> {

    };
}

template<>
bool cms::CollectionJsonLoader<cms::Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef){
    for(int idx=0; idx<jsonTree.getNumChildren(); idx++){
        ci::JsonTree subTree = jsonTree.getChild(idx);
        itemRef->set(subTree.getKey(), subTree.getValue());
    }
    return true;
}
