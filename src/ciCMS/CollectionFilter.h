#pragma once

#include <functional>
#include "cinder/Signals.h"
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionFilter : public CollectionBase<ItemType> {

        public: // types & constants
            typedef function<bool(ItemType&)> Functor;

        public:
            CollectionFilter() : collection(NULL){};
            ~CollectionFilter(){ destroy(); }
            void setup(CollectionBase<ItemType>* collection, Functor func, bool accept=true);
            void destroy();

        private:
            CollectionBase<ItemType>* collection;
            Functor func;
            std::vector<ci::signals::Connection> signalConnections;
    };
}

template<class ItemType>
void cms::CollectionFilter<ItemType>::setup(CollectionBase<ItemType>* collection, Functor func, bool accept){
    destroy();
    this->collection = collection;
    this->func = func;

    // apply check to all currently added models
    collection->each([this, accept](shared_ptr<ItemType> model){
        if(this->func(*model.get()) != accept){
            this->collection->remove(model);
        }
    });

    // apply to newly added models
    if(accept){
        auto conn = collection->beforeAddSignal.connect(this->func);
        signalConnections.push_back(conn);
        return;
    }

    auto conn = collection->beforeAddSignal.connect([this](ItemType& instance) -> bool {
        return !this->func(instance);
    });
    signalConnections.push_back(conn);
}

template<class ItemType>
void cms::CollectionFilter<ItemType>::destroy(){
    for(auto& conn : signalConnections){
        conn.disconnect();
    }

    signalConnections.clear();
    collection = NULL;
}
