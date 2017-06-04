#pragma once

#include "CollectionBase.h"
#include "CollectionLimit.h"
#include "CollectionSync.h"

namespace cms {
    template<class ItemType>
    class Collection : public CollectionBase<ItemType> {
        public:
            void limit(unsigned int amount){
                collectionLimit.setup(this, amount);
            }

            void setFifo(bool newFifo){
                collectionLimit.setFifo(newFifo);
            }

            void sync(shared_ptr<Collection<ItemType>> other, bool active=true);
            void stopSync(shared_ptr<Collection<ItemType>> other);

        private:
            CollectionLimit<ItemType> collectionLimit;
            std::vector<shared_ptr<CollectionSync<ItemType>>> collectionSyncs;
    };
}

template<class ItemType>
void cms::Collection<ItemType>::sync(shared_ptr<Collection<ItemType>> other, bool active){
    auto sync = make_shared<CollectionSync<ItemType>>();
    sync->setup(this, other);

    // if active safe pointer so it doesn't auto-destruct
    if(active)
        collectionSyncs.push_back(sync);
}

template<class ItemType>
void cms::Collection<ItemType>::stopSync(shared_ptr<Collection<ItemType>> other){
    for(auto it = collectionSyncs.begin(); it != collectionSyncs.end(); it++){
        if(it->getSource() == other){
            collectionSyncs.erase(it);
            return;
        }
    }

    CI_LOG_W("Could not source to stop syncing from");
}
