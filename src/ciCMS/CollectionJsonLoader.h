#pragma once

// stdlib
#include <sys/stat.h>
#include <unistd.h>
// cinder
#include "cinder/app/App.h"
#include "cinder/Json.h"
// local
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionJsonLoader {
        public:
            CollectionJsonLoader() : collection(NULL){}

            void setup(CollectionBase<ItemType>& collection){
                this->collection = &collection;
            }

            void setup(CollectionBase<ItemType>& collection, const ci::fs::path& filePath){
                this->collection = &collection;
                this->filePath = filePath;
            }

            bool load(){
                // check if file exists
                struct stat buffer;
                if (stat (filePath.c_str(), &buffer) != 0) {
                    // log file-not-exist warning?
                    return false;
                }

                auto dataSourceRef = ci::loadFile(filePath);
                string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));

                if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
                    fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

                ci::JsonTree jsonTree(fileContentString);

                return load(jsonTree);
            }

            bool load(const ci::JsonTree& jsonTree){
                bool allSuccess = true;

                // for(int idx=0; idx<jsonTree.getNumChildren(); idx++){
                for(auto subTree : jsonTree.getChildren()) {
                    // auto subTree = jsonTree.getChild(idx);
                    auto existingItemRef = findMatch(subTree, *this->collection);
                    auto itemRef = existingItemRef ? existingItemRef : this->collection->create();
                    allSuccess &= loadItem(subTree, itemRef);
                    // auto itemRef = collection->at(idx);
                    // if(!itemRef)
                    //     itemRef = collection->create();
                    // allSuccess &= loadItem(jsonTree.getChild(idx), itemRef);
                }

                return allSuccess;
            }

            virtual shared_ptr<ItemType> findMatch(ci::JsonTree& jsonTree, CollectionBase<ItemType>& collection){ return nullptr; }
            virtual bool loadItem(ci::JsonTree& jsonTree, shared_ptr<ItemType> itemRef){ return false; }

        private:
            CollectionBase<ItemType>* collection;
            ci::fs::path filePath;
    };
}
