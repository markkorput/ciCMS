#pragma once

#include <functional>
#include "CollectionBase.h"
#include "CollectionLimit.h"
#include "CollectionSync.h"
#include "CollectionFilter.h"
#include "CollectionTransformer.h"

namespace cms {
    template<class ItemType>
    class Collection : public CollectionBase<ItemType> {
        public: // types & constants
            typedef function<bool(ItemType&)> FilterFunctor;

        public:
            void limit(unsigned int amount){
                collectionLimit.setup(this, amount);
            }

            void setFifo(bool newFifo){
                collectionLimit.setFifo(newFifo);
            }

            void sync(shared_ptr<Collection<ItemType>> other, bool active=true);
            void stopSync(shared_ptr<Collection<ItemType>> other);

            void filter(FilterFunctor func, bool active=true);
            void reject(FilterFunctor func, bool active=true);

            template<class SourceType>
            void transform(CollectionBase<SourceType> &sourceCollection, function<shared_ptr<ItemType>(SourceType&)> func/*, bool active=true*/){
                // create
                auto transformerRef = make_shared<CollectionTransformer<SourceType, ItemType>>();
                // configure
                transformerRef->setup(sourceCollection, *this, func);
                // store
                collectionTransformers.push_back(transformerRef);
            }

            template<class SourceType>
            void stopTransform(CollectionBase<SourceType> &sourceCollection){
                // search
                for(auto it = collectionTransformers.begin(); it != collectionTransformers.end(); it++){
                    // find
                    if((*it)->getSource() == (void*)&sourceCollection){
                        // remove/destroy
                        collectionTransformers.erase(it);
                        return;
                    }
                }

                CI_LOG_W("Could not find source collection to stop transforming from");
            }

        private:
            CollectionLimit<ItemType> collectionLimit;
            std::vector<shared_ptr<CollectionSync<ItemType>>> collectionSyncs;
            std::vector<shared_ptr<CollectionFilter<ItemType>>> collectionFilters;
            std::vector<shared_ptr<CollectionTransformerBase>> collectionTransformers;
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

template<class ItemType>
void cms::Collection<ItemType>::filter(FilterFunctor func, bool active){
    auto filter = make_shared<CollectionFilter<ItemType>>();
    filter->setup(this, func);

    // if active; save filter so it doesn't auto-destruct (since it's a shared_ptr)
    if(active)
        collectionFilters.push_back(filter);
}

template<class ItemType>
void cms::Collection<ItemType>::reject(FilterFunctor func, bool active){
    auto filter = make_shared<CollectionFilter<ItemType>>();
    filter->setup(this, func, false /* reject instead of accept */);

    // if active; save filter so it doesn't auto-destruct (since it's a shared_ptr)
    if(active)
        collectionFilters.push_back(filter);
}
