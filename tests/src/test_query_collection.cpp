#include "catch.hpp"
#include "ciCMS/ciCMS.h"

using namespace std;
using namespace cms;

TEST_CASE("cms::QueryCollection", ""){
    class TestItem {
        public:
            string name;
            float value;
            void set(const string& newName, float val){
                name = newName;
                value = val;
            }
    };

    class TestQuery {
        private:
            string name;
            bool bValueFilterEnabled, bNameFilterEnabled;
            float minValue;

        public:
            TestQuery() : bValueFilterEnabled(false), bNameFilterEnabled(false){}

            bool isValueFilterEnabled() const { return bValueFilterEnabled; }

            float getMinValue() const { return minValue; }

            void setMinValue(float min){
                minValue = min;
                bValueFilterEnabled = true;
            }

            void setNameFilter(const string& newName){
                name = newName;
                bNameFilterEnabled = true;
            }

            const string& getNameFilter() const { return name; }

            bool isNameFilterEnabled() const { return bNameFilterEnabled; }
    };

    class TestQueryCollection : public QueryCollection<TestItem, TestQuery> {
        public:
            // database is our in-memory dummy backend
            Collection<TestItem> database;

            TestQueryCollection(){
                // populate with some content
                database.create()->set("no.1", 10);
                database.create()->set("no.2", 20);
                database.create()->set("no.3", 30);
            }

            ExecutionRef nameQuery(string name){
                auto queryRef = make_shared<TestQuery>();
                queryRef->setNameFilter(name);
                return query(queryRef);
            }

        private:

            void execute(ExecutionRef execRef){
                auto queryRef = execRef->getQuery();

                // "query" our "database"
                database.each([this, queryRef, execRef](shared_ptr<TestItem> itemRef){
                    // name condition
                    if(queryRef->isNameFilterEnabled() && itemRef->name != queryRef->getNameFilter())
                        return;

                    // value condition
                    if(queryRef->isValueFilterEnabled() && itemRef->value < queryRef->getMinValue())
                        return;

                    // all conditions met, add this item to our collection
                    execRef->add(itemRef);
                });

                execRef->finalize(true);
            }
    };

    SECTION(".query()"){
        TestQueryCollection col;
        shared_ptr<TestQueryCollection::Execution> executionRef;

        vector<TestQuery*> doneQueries;

        col.queryDoneSignal.connect([&doneQueries](shared_ptr<TestQuery> queryRef){
            doneQueries.push_back(queryRef.get());
        });

        {   // start query with call to .query
            auto queryRef = make_shared<TestQuery>();
            queryRef->setNameFilter("no.3");

            executionRef = col.query(queryRef);
            REQUIRE(executionRef != nullptr);
            REQUIRE(executionRef->getQuery() == queryRef);
            REQUIRE(executionRef->isDone()); // this one happens to be extremely fast and, well, non-async
            REQUIRE(executionRef->isSuccess());
            REQUIRE(!executionRef->isFailure()); // this one happens to be extremely fast and, well, non-async
            // execution specific results
            REQUIRE(executionRef->result.size() == 1);
            REQUIRE(executionRef->result.at(0)->name == "no.3");
            REQUIRE(executionRef->result.at(0) == col.database.at(2));

        } // end of queryRef instance scope

        // check if the query was correctly performed;
        // it should've gotten us just the database item "no.3"
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0)->name == "no.3");
        REQUIRE(col.at(0) == col.database.at(2));
        REQUIRE(doneQueries.size() == 1);
        REQUIRE(doneQueries[0] == executionRef->getQuery().get());

        {   // do another query; with a value-filter this time
            auto queryRef = make_shared<TestQuery>();
            queryRef->setMinValue(20);
            executionRef = col.query(queryRef);

            REQUIRE(executionRef->isDone()); // another fast one
            REQUIRE(executionRef->isSuccess());
            // execution specific results
            REQUIRE(executionRef->result.size() == 2);
            REQUIRE(executionRef->result.at(0)->name == "no.2");
            REQUIRE(executionRef->result.at(0) == col.database.at(1));
            REQUIRE(executionRef->result.at(1)->name == "no.3");
            REQUIRE(executionRef->result.at(1) == col.database.at(2));
        }

        // the results of this query are added to the collection;
        // our collection by default performs model based duplicate filtering
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(0) == col.database.at(2));
        REQUIRE(col.at(1) == col.database.at(1));
        // REQUIRE(col.at(2) == col.database.at(2));
        REQUIRE(doneQueries.size() == 2);
        REQUIRE(doneQueries[1] == executionRef->getQuery().get());
    }

    SECTION(".query() with pass-by-reference argument"){
        // QueryType = int
        QueryCollection<TestItem, int> col;

        int result=0;

        // execRef->getQuery() returns a shared_ptr<int> == 5
        col.query(5)->whenDone([&result](QueryCollection<TestItem, int>::Execution& exec){
            result = *exec.getQuery();
        });

        REQUIRE(result == 5);
    }

    SECTION(".executeFn()"){
        TestQueryCollection col;

        // register custom execution logic ("overwrites" any existing execution method)
        col.executeFn([&col](TestQueryCollection::ExecutionRef execRef){
            // simply create a new item in the results model and give it the name of the query's name filter
            execRef->result.create()->name = execRef->getQuery()->getNameFilter();
        });

        REQUIRE(col.size() == 0);
        auto execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0) == execRef->result.at(0));
    }

    SECTION(".cacheCheckFn()"){
        TestQueryCollection col;

        // register custom execution logic ("overwrites" any existing execution method)
        col.executeFn([&col](TestQueryCollection::ExecutionRef execRef){
            // simply create a new item in the results model and give it the name of the query's name filter
            execRef->result.create()->name = execRef->getQuery()->getNameFilter();
        });

        REQUIRE(col.size() == 0);
        auto execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 1);
        REQUIRE(col.at(0) == execRef->result.at(0));
        // let's do that again
        execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->result.size() == 1);
        REQUIRE(execRef->result.at(0)->name == "Some Name");
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(1) == execRef->result.at(0));

        // now let's register a cache checkers which first checks which of the existing items
        // matches the query. If any, it aborts the query by finalizing it
        col.cacheCheckFn([](Collection<TestItem>& col, TestQueryCollection::ExecutionRef execRef){
            col.each([&execRef](shared_ptr<TestItem> itemRef){
                if(itemRef->name == execRef->getQuery()->getNameFilter())
                    execRef->result.add(itemRef);
            });

            // finalize the execRef so the actual "remote" query doesn't happen
            if(!execRef->result.isEmpty())
                execRef->abort();
        });

        // now let's do that a third time, this time no new item should appear
        execRef = col.nameQuery("Some Name");
        REQUIRE(execRef->isDone()); // this one happens to be extremely fast and, well, non-async
        REQUIRE(execRef->isAborted());
        REQUIRE(!execRef->isExecuted());
        REQUIRE(!execRef->isSuccess());
        REQUIRE(!execRef->isFailure());
        REQUIRE(execRef->result.size() == 2); // matched with two (both) of the cached items
        REQUIRE(col.size() == 2);
        REQUIRE(col.at(0) == execRef->result.at(0));
        REQUIRE(col.at(1) == execRef->result.at(1));
    }

    SECTION(".whenDone() .onSuccess .onFailure .whenAborted .whenExecuted"){
        TestQueryCollection col;

        string result="";

        auto execRef = col.nameQuery("no.1")
            ->whenDone([&result](TestQueryCollection::Execution& exec){
                result+="/done:"+exec.getQuery()->getNameFilter();
            })
            ->onSuccess([&result](TestQueryCollection::Execution& exec){
                result+="/success:"+exec.getQuery()->getNameFilter();
            })
            ->onFailure([&result](TestQueryCollection::Execution& exec){
                result+="/failure:"+exec.getQuery()->getNameFilter();
            })
            ->whenAborted([&result](TestQueryCollection::Execution& exec){
                result+="/aborted:"+exec.getQuery()->getNameFilter();
            })
            ->whenExecuted([&result](TestQueryCollection::Execution& exec){
                result+="/executed:"+exec.getQuery()->getNameFilter();
            });

        REQUIRE(result == "/done:no.1/success:no.1/executed:no.1");

        // register aborter
        col.cacheCheckFn([](Collection<TestItem>& col, TestQueryCollection::ExecutionRef execRef){
            execRef->abort();
        });

        // run again
        result = "";
        execRef = col.nameQuery("foobar")
            ->whenDone([&result](TestQueryCollection::Execution& exec){
                result+="/done:"+exec.getQuery()->getNameFilter();
            })
            ->onSuccess([&result](TestQueryCollection::Execution& exec){
                result+="/success:"+exec.getQuery()->getNameFilter();
            })
            ->onFailure([&result](TestQueryCollection::Execution& exec){
                result+="/failure:"+exec.getQuery()->getNameFilter();
            })
            ->whenAborted([&result](TestQueryCollection::Execution& exec){
                result+="/aborted:"+exec.getQuery()->getNameFilter();
            })
            ->whenExecuted([&result](TestQueryCollection::Execution& exec){
                result+="/executed:"+exec.getQuery()->getNameFilter();
            });

        REQUIRE(result == "/done:foobar/aborted:foobar");
    }
}
