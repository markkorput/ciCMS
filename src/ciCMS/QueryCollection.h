#pragma once

#include <functional>
#include "cinder/Signals.h"
#include "Collection.h"

namespace cms {
    template<class ItemType, class QueryType>
    class QueryCollection : public Collection<ItemType> {

    public:
        typedef function<void(bool)> FinalizerFunc;

        class Execution;
        typedef shared_ptr<Execution> ExecutionRef;

        class Execution {
            friend QueryCollection;

            public:
                Execution(QueryCollection<ItemType, QueryType>& collection, shared_ptr<QueryType> query)
                    : collectionPointer(&collection), queryRef(query), bDone(false), bSuccess(false){}

                bool isDone() const { return bDone; }
                bool isSuccess() const { return bSuccess; }
                bool isFailure() const { return bDone && !bSuccess; }

                const QueryCollection* getCollection() const {
                    return collectionPointer;
                }

                const shared_ptr<QueryType> getQuery() const {
                    return queryRef;
                }

            public:
                ci::signals::Signal<void(ExecutionRef)> doneSignal;

            private:
                QueryCollection<ItemType, QueryType>* collectionPointer;
                shared_ptr<QueryType> queryRef;

                // these can only be modified by friend class QueryCollection
                bool bDone, bSuccess;
        };

    public:
        // start query execution and return shared pointer to execution object
        ExecutionRef query(const shared_ptr<QueryType> queryRef){
            auto execRef = make_shared<Execution>(*this, queryRef);

            // execute query using (virtual) executer
            execute(execRef->getQuery(), [this, execRef](bool success){
                this->finalize(execRef, success);
            });

            return execRef;
        };

    private:

        virtual void execute(shared_ptr<QueryType> queryRef, FinalizerFunc finalizer){
            // This is a semi-virtual method; does nothing but declare the
            // query execution to be a success. Should be overwritten.
            finalizer(true /* success */);
        }

    protected:

        void finalize(ExecutionRef execRef, bool success){
            execRef->bDone = true;
            execRef->bSuccess = success;
            execRef->doneSignal.emit(execRef);
        }

    public:
        ci::signals::Signal<void()> queryDoneEvent;
    };

    template<typename ItemType, typename QueryType>
    using QueryCollectionRef = shared_ptr<QueryCollection<ItemType, QueryType>>;
}
