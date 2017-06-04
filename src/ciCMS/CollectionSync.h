#pragma once

#include "cinder/Log.h"
#include "cinder/Signals.h"
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionSync : public CollectionBase<ItemType> {
        public: // methods

            CollectionSync() : target(NULL), source(nullptr){}
            ~CollectionSync(){ destroy(); }

            void setup(CollectionBase<ItemType> *target, shared_ptr<CollectionBase<ItemType>> source);
            void destroy();

            shared_ptr<CollectionBase<ItemType>> getSource() const { return source; }

        private: // attributes

            CollectionBase<ItemType> *target;
            shared_ptr<CollectionBase<ItemType>> source;
            std::vector<ci::signals::Connection> signalConnections;
    };
}

template<class ItemType>
void cms::CollectionSync<ItemType>::setup(CollectionBase<ItemType> *target, shared_ptr<CollectionBase<ItemType>> source){
    destroy();

    this->target = target;
    this->source = source;

    if(!source){
        CI_LOG_W("got nullptr source to sync from");
        return;
    }

    // add all instances currently in source
    source->each([this](shared_ptr<ItemType> instanceRef){
        if(!this->target->has(instanceRef)){
            this->target->add(instanceRef);
        }
    });

    // actively monitor for new instances added to source
    auto conn = source->addSignal.connect([&](ItemType& instance){
        if(!this->target->has(&instance)){
            this->target->add(this->source->find(&instance)); // need to convert ref var to shared_ptr
        }
    });

    signalConnections.push_back(conn);

    // actively montor for instances removed from source
    conn = source->removeSignal.connect([this](ItemType& instance){
        if(this->target->has(&instance))
            this->target->remove(&instance);
    });

    signalConnections.push_back(conn);
}

template<class ItemType>
void cms::CollectionSync<ItemType>::destroy(){
    for(auto& conn : signalConnections){
        conn.disconnect();
    }

    signalConnections.clear();
    source = nullptr;
}
