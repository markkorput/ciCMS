#pragma once

#include <functional>
#include "cinder/Signals.h"
#include "cinder/Log.h"
#include "Collection.h"

namespace cms {
    template<class ItemType, class QueryType>
    class QueryCollection : public Collection<ItemType> {

        public: // sub-classes/-types

            class Execution;
            typedef shared_ptr<Execution> ExecutionRef;

            class Execution {
                friend QueryCollection;

                public:
                    Execution(shared_ptr<QueryType> query)
                        : queryRef(query), bDone(false), bSuccess(false){}

                    bool isDone() const { return bDone; }
                    bool isSuccess() const { return bSuccess; }
                    bool isFailure() const { return bDone && !bSuccess; }

                    const shared_ptr<QueryType> getQuery() const {
                        return queryRef;
                    }

                    void add(shared_ptr<ItemType> itemRef){
                        result.add(itemRef);
                    }

                    void finalize(bool success=true){
                        bDone = true;
                        bSuccess = success;
                        doneSignal.emit(*this);
                    }

                public:

                    ci::signals::Signal<void(Execution&)> doneSignal;
                    Collection<ItemType> result;

                private:
                    shared_ptr<QueryType> queryRef;
                    // these can only be modified by friend class QueryCollection
                    bool bDone, bSuccess;
            };

            typedef function<void(ExecutionRef)> ExecuteFunctor;
            typedef function<void(Collection<ItemType>&, ExecutionRef)> CacheCheckFunctor;

        public: // methods

            QueryCollection() : executeFunc(nullptr), cacheCheckFunc(nullptr){}

            // start query execution and return shared pointer to execution object to caller
            ExecutionRef query(const shared_ptr<QueryType> queryRef){
                // create execution instance
                auto execRef = make_shared<Execution>(queryRef);

                // when execution emits doneSignal, we also emit our queryDoneSignal and stop syncing
                execRef->doneSignal.connect([this](Execution& exec){
                    this->queryDoneSignal.emit(exec.getQuery());
                });

                // first our cacheCheckFunctor if any is registered,
                // before calling our executor
                if(cacheCheckFunc){
                    cacheCheckFunc(*this, execRef);

                    if(execRef->isDone()){
                        // "abort"
                        return execRef;
                    }
                }

                {   // copy all items added to the execution's result collection to our collection
                    // this->sync(execRef->result);
                    Collection<ItemType>* pResult = &execRef->result;
                    execRef->result.addSignal.connect([this, pResult](ItemType& item){
                        if(!this->has(&item))
                            this->add(pResult->find(&item));
                    });
                }

                // execute query using (virtual) executer
                if(this->executeFunc)
                    this->executeFunc(execRef);
                else
                    this->execute(execRef);

                // give execution instance to caller
                return execRef;
            };

            void executeFn(ExecuteFunctor func){
                executeFunc = func;
            }

            void cacheCheckFn(CacheCheckFunctor func){
                cacheCheckFunc = func;
            }

        private: // methods

            virtual void execute(ExecutionRef execRef){
                // This is a semi-virtual method; does nothing but declare the
                // query execution to be a success. Should be overwritten.
                execRef->finalize(true /* success */);
            }

        public: // signals

            ci::signals::Signal<void(shared_ptr<QueryType>)> queryDoneSignal;

        private: // attributes

            ExecuteFunctor executeFunc;
            CacheCheckFunctor cacheCheckFunc;
    };

    template<typename ItemType, typename QueryType>
    using QueryCollectionRef = shared_ptr<QueryCollection<ItemType, QueryType>>;
}
