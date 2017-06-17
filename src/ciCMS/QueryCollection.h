#pragma once

#include "cinder/Signals.h"
#include "Collection.h"

namespace cms {
    template<class ItemType, class QueryType>
    class QueryCollection : Collection<ItemType> {

    public:
        class Execution {
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

                void setResult(bool success){
                    bSuccess = success;
                    bDone = true;
                }

            public:
                ci::signals::Signal<void(shared_ptr<QueryType>)> doneSignal;

            private:
                QueryCollection<ItemType, QueryType>* collectionPointer;
                shared_ptr<QueryType> queryRef;
                bool bDone, bSuccess;
        };

        typedef shared_ptr<Execution> ExecutionRef;

    public:
        // start query execution and return shared pointer to execution object
        ExecutionRef query(const shared_ptr<QueryType> queryRef){
            auto execRef = make_shared<Execution>(*this, queryRef);
            execute(execRef);
            return execRef;
        };

    private:

        void execute(ExecutionRef execRef){
            // virtual method; should be implemented by inheriting class
            execRef->setResult(true);
        }

    public:
        ci::signals::Signal<void()> queryDoneEvent;
    };

    template<typename ItemType, typename QueryType>
    using QueryCollectionRef = shared_ptr<QueryCollection<ItemType, QueryType>>;
}
