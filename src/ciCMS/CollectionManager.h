#pragma once

#include <memory>
#include <functional>

#include "cinder/app/App.h"
#include "cinder/Log.h"

#include "ModelCollection.h"

namespace cms {
    class CollectionManager;
    typedef shared_ptr<CollectionManager> CollectionManagerRef;

    class CollectionManager : public std::map<std::string, ModelCollectionRef> {
    private:
        static CollectionManagerRef singletonRef;

    public:
        static CollectionManagerRef singleton();
        static void deleteSingleton();

    public:
        ModelCollectionRef get(const std::string& name, bool createIfNotExist=false);

    };
}
