#pragma once

#include "cinder/app/App.h"
#include "cinder/Json.h"
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionJsonWriter {
        public:
            CollectionJsonWriter() : collection(NULL){}

            void setup(CollectionBase<ItemType>& collection){
                this->collection = &collection;
            }

            ci::JsonTree getJsonTree(){
                ci::JsonTree collectionTree;
                this->collection->each([this, &collectionTree](shared_ptr<ItemType> itemRef){
                    collectionTree.addChild(this->getItemJsonTree(itemRef));
                });

                return collectionTree;
            }

            std::string toJsonString(){
                return getJsonTree().serialize();
            }

            virtual ci::JsonTree getItemJsonTree(shared_ptr<ItemType> itemRef){
                return ci::JsonTree();
            }

            void writeJson(const ci::fs::path& path){
                auto targetPathRef = ci::writeFile(path);
                targetPathRef->getStream()->write(this->toJsonString());
            }

        private:
            CollectionBase<ItemType>* collection;
    };
}
