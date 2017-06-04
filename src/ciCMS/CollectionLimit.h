#pragma once

#include "cinder/Signals.h"
#include "CollectionBase.h"

#define CICMS_NO_LIMIT 0

namespace cms {

    template<class ItemType>
    class CollectionLimit {

        public:
            CollectionLimit() : collection(NULL), mLimit(CICMS_NO_LIMIT), bFifo(false){}
            ~CollectionLimit(){ destroy(); }

            void setup(CollectionBase<ItemType>* collection, unsigned int amount);
            void destroy();

            // bool limitReached(){ return mLimit != NO_LIMIT && collection->size() >= mLimit; }
            bool limitExceeded(){ return mLimit != CICMS_NO_LIMIT && collection->size() > mLimit; }
            void setFifo(bool fifo){ bFifo = fifo; }
            // bool getFifo(){ return bFifo; }

        private: // methods

            void enforce();

        private: // attributes

            CollectionBase<ItemType>* collection;
            unsigned int mLimit;
            bool bFifo; // first-in-first-out
            std::vector<ci::signals::Connection> signalConnections;
    };

}

template<class ItemType>
void cms::CollectionLimit<ItemType>::setup(CollectionBase<ItemType>* collection, unsigned int amount){
    destroy();
    this->collection = collection;
    this->mLimit = amount;

    auto conn = this->collection->addSignal.connect([this](ItemType& instance){
        this->enforce();
    });


    this->enforce();

    signalConnections.push_back(conn);
}

template<class ItemType>
void cms::CollectionLimit<ItemType>::destroy(){
    for(auto& conn : signalConnections){
        conn.disconnect();
    }

    signalConnections.clear();
    collection = NULL;
}

template<class ItemType>
void cms::CollectionLimit<ItemType>::enforce(){
    while(limitExceeded()){
        collection->removeByIndex(bFifo ? 0 : collection->size()-1);
    }
}
