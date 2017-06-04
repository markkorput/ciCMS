#pragma once

#include "cinder/app/App.h"
#include "cinder/Json.h"
#include "CollectionBase.h"

namespace cms {
    template<class ItemType>
    class CollectionJsonLoader {
        public:
            CollectionJsonLoader() : collection(NULL){}

            void setup(CollectionBase<ItemType>& collection, const ci::fs::path& filePath){
                this->collection = &collection;
                this->filePath = filePath;
            }

            bool load(){
                auto dataSourceRef = ci::loadFile(filePath);
                string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));

                if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
                    fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

                ci::JsonTree jsonTree(fileContentString);

                return load(jsonTree);
            }

            bool load(ci::JsonTree jsonTree){
                bool allSuccess = true;

                for(int idx=0; idx<jsonTree.getNumChildren(); idx++){
                    auto itemRef = collection->at(idx);
                    if(!itemRef)
                        itemRef = collection->create();
                    allSuccess &= loadItem(jsonTree.getChild(idx), itemRef);
                }

                return allSuccess;
            }

            virtual bool loadItem(ci::JsonTree& jsonTree, shared_ptr<ItemType> itemRef){ return false; }

        private:
            CollectionBase<ItemType>* collection;
            ci::fs::path filePath;
    };
}
