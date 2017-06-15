#include "CollectionManager.h"

using namespace cms;

ModelCollectionRef CollectionManager::get(const std::string& name, bool createIfNotExist){
    ModelCollectionRef result = (*this)[name];

    if(result == nullptr && createIfNotExist){
        result = make_shared<ModelCollection>();
        (*this)[name] = result;
    }

    return result;
}
