#pragma once

#include <memory>
#include <functional>
#include "cinder/app/App.h"

#define CICMS_INVALID_INDEX (-1)
#define CICMS_INVALID_CID (NULL)

namespace cms {
    template<class ItemType>
    class CollectionBase {

        public: // types & constants

            typedef void* CidType;
            typedef function<void(void)> LockFunctor;
            typedef function<void(shared_ptr<ItemType>)> IterateRefFunc;
            typedef function<bool(shared_ptr<ItemType>)> FindFunc;

            class Modification {
                public:
                    shared_ptr<ItemType> addRef;
                    ItemType* removeCid;
                    bool notify;
                    Modification() : addRef(nullptr), removeCid(CICMS_INVALID_CID), notify(true){}
                    Modification(shared_ptr<ItemType> ref, bool _notify=true) : addRef(ref), removeCid(CICMS_INVALID_CID), notify(_notify){}
                    Modification(ItemType* cid, bool _notify=true) : addRef(nullptr), removeCid(cid), notify(_notify){}
            };

        public: // methods

            CollectionBase() : vectorLockCount(0){}
            ~CollectionBase(){ destroy(); }

            void destroy();

            // CRUD - Create
            shared_ptr<ItemType> create();
            void add(shared_ptr<ItemType> instanceRef, bool notify=true);

            // CRUD - Read
            // const vector<shared_ptr<ItemType>> &instances(){ return instanceRefs; }
            shared_ptr<ItemType> at(unsigned int idx);
            shared_ptr<ItemType> find(ItemType* cid);
            //! Convenience method that convert the void* CidType to a ItemType* value
            shared_ptr<ItemType> find(CidType cid){ return this->find((ItemType*)cid); }
            shared_ptr<ItemType> first(FindFunc func){
                for(int idx=0; idx<size(); idx++){
                    auto itemRef = this->at(idx);
                    if(func(itemRef))
                        return itemRef;
                }

                return nullptr;
            }

            unsigned int size(){ return instanceRefs.size(); }
            bool has(shared_ptr<ItemType> instanceRef){ return indexOf(instanceRef.get()) != CICMS_INVALID_INDEX; }
            bool has(ItemType* cid){ return indexOf(cid) != CICMS_INVALID_INDEX; }
            //! Convenience method that convert the void* CidType to a ItemType* value
            bool has(CidType cid){ return has((ItemType*)cid); }

            int randomIndex(){ return size() == 0 ? CICMS_INVALID_INDEX : rand()%size(); }
            shared_ptr<ItemType> random(){ return at(randomIndex()); }
            shared_ptr<ItemType> previous(shared_ptr<ItemType> instanceRef, bool wrap=false);
            shared_ptr<ItemType> next(shared_ptr<ItemType> instanceRef, bool wrap=false);

            // CRUD - "update"
            void each(IterateRefFunc func);

            // CRUD - Delete
            shared_ptr<ItemType> remove(shared_ptr<ItemType> instanceRef, bool notify=true);
            shared_ptr<ItemType> remove(ItemType* cid, bool notify=true);
            //! Convenience method that convert the void* CidType to a ItemType* value
            shared_ptr<ItemType> remove(CidType cid, bool notify=true){ return this->remove((ItemType*)cid); }
            shared_ptr<ItemType> removeByIndex(unsigned int index, bool notify=true);

        protected: // methods

            int indexOf(ItemType* cid);
            bool isLocked() const { return vectorLockCount > 0; }
            void lock(LockFunctor func);

        public: // events

            ci::signals::Signal<bool(ItemType&), ci::signals::CollectorUntil0<bool>> beforeAddSignal;
            ci::signals::Signal<void(ItemType&)> addSignal,
                                                removeSignal;

        private: // attributes

            std::vector<shared_ptr<ItemType>> instanceRefs;
            unsigned int vectorLockCount;
            std::vector<shared_ptr<Modification>> operationsQueue;
    };
}

template <class ItemType>
void cms::CollectionBase<ItemType>::destroy(){
    if(isLocked()){
        CI_LOG_W("collection being destroyed while locked");
    }

    for(int i=instanceRefs.size()-1; i>=0; i--){
        removeByIndex(i);
    }
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::create(){
    // create instance with auto-incremented ID
    auto ref = make_shared<ItemType>();
    // add to our collection and return
    add(ref);
    return ref;
}

template <class ItemType>
void cms::CollectionBase<ItemType>::add(shared_ptr<ItemType> instanceRef, bool notify){
    // vector being iterated over? schedule removal operation for when iteration is done
    if(isLocked()){
        operationsQueue.push_back(make_shared<Modification>(instanceRef, notify));
        return;
    }

    if(instanceRef == nullptr){
        // What the hell are we supposed to do with this??
        CI_LOG_W("got nullptr instance to add to collection");
        return;
    }

    if(beforeAddSignal.getNumSlots() > 0 && !beforeAddSignal.emit(*instanceRef.get()))
        return;

    // add to our collection
    instanceRefs.push_back(instanceRef);

    // let's tell the world
    if(notify)
        addSignal.emit(*(instanceRef.get()));

    // success!
    return;
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::previous(shared_ptr<ItemType> instanceRef, bool wrap){
    int idx = indexOf(instanceRef.get());

    if(idx == CICMS_INVALID_INDEX)
        return nullptr;

    if(idx > 0)
        return at(idx-1);

    if(wrap)
        return at(size()-1);

    return nullptr;
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::next(shared_ptr<ItemType> instanceRef, bool wrap){
    int idx = indexOf(instanceRef.get());

    if(idx == CICMS_INVALID_INDEX)
        return nullptr;

    if(idx < size()-1)
        return at(idx+1);

    if(wrap)
        return at(0);

    return nullptr;
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::at(unsigned int idx){
    if(idx < 0 || idx >= size()){
        CI_LOG_W("invalid index");
        return nullptr;
    }

    return instanceRefs.at(idx);
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::find(ItemType* cid){
    int idx = indexOf(cid);
    if(idx == CICMS_INVALID_INDEX)
        return nullptr;
    return at(idx);
}

template <class ItemType>
int cms::CollectionBase<ItemType>::indexOf(ItemType* cid){
    int idx=0;

    for(auto instanceRef : instanceRefs){
        if(instanceRef.get() == cid)
            return idx;
        idx++;
    }

    return CICMS_INVALID_INDEX;
}

template <class ItemType>
void cms::CollectionBase<ItemType>::each(IterateRefFunc func){
    // when locked all add/remove operations to our internal instanceRefs vector
    // are intercepted and queue for execution until after the lock is lifter,
    // so we can safely iterate over the vector and callbacks are free to call
    // our add/remove methods without causing errors
    lock([&](){
        for(auto instanceRef : this->instanceRefs){
            func(instanceRef);
        }
    });
}

template <class ItemType>
shared_ptr<ItemType>  cms::CollectionBase<ItemType>::remove(shared_ptr<ItemType> instanceRef, bool notify){
    if(instanceRef == nullptr){
        CI_LOG_W("got NULL parameter");
        return nullptr;
    }

    // find index and remove by index
    return remove(instanceRef.get());
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::remove(ItemType* cid, bool notify){
    // vector being iterated over? schedule removal operation for when iteration is done
    if(isLocked()){
        operationsQueue.push_back(make_shared<Modification>(cid, notify));
        return nullptr;
    }

    int idx = indexOf(cid);

    if(idx == CICMS_INVALID_INDEX){
        CI_LOG_W("could not find instance to remove");
        return nullptr;
    }

    // find
    auto instanceRef = at(idx);

    // notify (BEFORE removing from vector, because the shared_ptr might need to be looked up)
    if(notify)
        removeSignal.emit(*instanceRef.get());

    // remove
    instanceRefs.erase(instanceRefs.begin() + idx);


    // return removed instance
    return instanceRef;
}

template <class ItemType>
shared_ptr<ItemType> cms::CollectionBase<ItemType>::removeByIndex(unsigned int index, bool notify){
    auto instanceRef = at(index);

    // check
    if(!instanceRef){
        CI_LOG_W("couldn't find instance with index: " << index);
        return nullptr;
    }

    // invoke main remove routine
    return remove(instanceRef.get(), notify);
}

template <class ItemType>
void cms::CollectionBase<ItemType>::lock(LockFunctor func){
    vectorLockCount++;
    func();
    vectorLockCount--;

    // still (recursively) iterating over our vector? skip processing opereations queue
    if(isLocked())
        return;

    // after we're done iterating, we should process any items
    // accumulated in the vector modificaton queue
    for(auto modification : operationsQueue){
        if(modification->addRef){
            add(modification->addRef, modification->notify);
        } else {
            remove(modification->removeCid, modification->notify);
        }
    }

    operationsQueue.clear();
}
