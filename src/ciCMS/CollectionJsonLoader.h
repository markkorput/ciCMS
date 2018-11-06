#pragma once

// cinder
#include "cinder/app/App.h"
#include "cinder/Json.h"
// stdlib
#ifdef CINDER_MSW
#include <io.h>
#define access _access_s
#else
#include <unistd.h> // access/stat for file exist check
#endif
#include <sys/stat.h>
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

			bool load() {
        // check if file exists
        #ifdef CINDER_MSW
          if (!std::experimental::filesystem::exists(filePath.c_str())) {
          // if (access(filePath.c_str(), 0) != 0) {
            // log file-not-exist warning?
            return false;
          }
        #else
			struct stat buffer;
			if (stat (filePath.c_str(), &buffer) != 0) {
				// log file-not-exist warning?
			   return false;
			}
        #endif

                auto dataSourceRef = ci::loadFile(filePath);
                string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));

                if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
                    fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

                return this->load(fileContentString);
            }

            bool load(const std::string& jsonString) {
                ci::JsonTree jsonTree(jsonString);
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
