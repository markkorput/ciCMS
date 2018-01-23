#pragma once

#include "cinder/Log.h"
#include "cinder/Signals.h"
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionSync : public CollectionBase<ItemType> {
        public: // methods

            CollectionSync() : target(NULL), pSource(NULL), sourceRef(nullptr){}
            ~CollectionSync(){ destroy(); }

            void setup(CollectionBase<ItemType> *target, CollectionBase<ItemType>& source);
            void setup(CollectionBase<ItemType> *target, shared_ptr<CollectionBase<ItemType>> source);
            void destroy();

            const CollectionBase<ItemType>& getSource() const { return sourceRef ? *sourceRef.get() : *pSource; }

        private: // attributes

            CollectionBase<ItemType> *target;
            CollectionBase<ItemType>* pSource;
            shared_ptr<CollectionBase<ItemType>> sourceRef;
            std::vector<ci::signals::Connection> signalConnections;
    };
}

template<class ItemType>
void cms::CollectionSync<ItemType>::setup(CollectionBase<ItemType> *target, shared_ptr<CollectionBase<ItemType>> source){
    sourceRef = source;
    setup(target, *sourceRef.get());
}

template<class ItemType>
void cms::CollectionSync<ItemType>::setup(CollectionBase<ItemType> *target, CollectionBase<ItemType>& source){
    destroy();

    this->target = target;
    this->pSource = &source;

    if(!pSource){
        CI_LOG_W("got NULL source to sync from");
        return;
    }

    // add all instances currently in source
    pSource->each([this](shared_ptr<ItemType> instanceRef){
        if(!this->target->has(instanceRef)){
            this->target->add(instanceRef);
        }
    });

    // actively monitor for new instances added to pSource
    auto conn = pSource->addSignal.connect([&](ItemType& instance){
        if(!this->target->has(&instance)){
            this->target->add(this->pSource->find(&instance)); // need to convert ref var to shared_ptr
        }
    });

    signalConnections.push_back(conn);

    // actively montor for instances removed from pSource
    conn = pSource->removeSignal.connect([this](ItemType& instance){
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
    pSource = NULL;
    sourceRef = nullptr;
}
