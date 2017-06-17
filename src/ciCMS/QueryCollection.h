#pragma once

#include <functional>
#include "cinder/Signals.h"
#include "Collection.h"

namespace cms {
    template<class ItemType, class QueryType>
    class QueryCollection : public Collection<ItemType> {

    public:
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

    public:

        // start query execution and return shared pointer to execution object to caller
        ExecutionRef query(const shared_ptr<QueryType> queryRef){
            // create execution instance
            auto execRef = make_shared<Execution>(queryRef);

            // copy all items added to the execution's result collection to our collection
            this->sync(execRef->result);

            // when execution emits doneSignal, we also emit our queryDoneSignal and stop syncing
            execRef->doneSignal.connect([this](Execution& exec){
                this->stopSync(exec.result);
                this->queryDoneSignal.emit(exec.getQuery());
            });

            // execute query using (virtual) executer
            execute(execRef);

            // give execution instance to caller
            return execRef;
        };

    private:

        virtual void execute(ExecutionRef execRef){
            // This is a semi-virtual method; does nothing but declare the
            // query execution to be a success. Should be overwritten.
            execRef->finalize(true /* success */);
        }

    public:
        ci::signals::Signal<void(shared_ptr<QueryType>)> queryDoneSignal;
    };

    template<typename ItemType, typename QueryType>
    using QueryCollectionRef = shared_ptr<QueryCollection<ItemType, QueryType>>;
}
