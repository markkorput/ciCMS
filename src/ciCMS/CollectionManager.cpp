#include "CollectionManager.h"

#include "cinder/Json.h"

using namespace cms;

ModelCollectionRef CollectionManager::get(const std::string& name, bool createIfNotExist){
    ModelCollectionRef result = (*this)[name];

    if(result == nullptr && createIfNotExist){
        result = make_shared<ModelCollection>();
        (*this)[name] = result;
    }

    return result;
}

bool CollectionManager::loadJsonFromFile(const ci::fs::path& path){
    auto dataSourceRef = ci::loadFile(path);
    string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));

    if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
        fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

    ci::JsonTree jsonTree(fileContentString);
    return loadJson(jsonTree);
}

bool CollectionManager::loadJson(const ci::JsonTree& jsonTree){
    bool allGood = true;

    for(int idx=0; idx<jsonTree.getNumChildren(); idx++){
        auto subTree = jsonTree.getChild(idx);
        auto name = subTree.getKey();
        auto colRef = get(name, true /* create if doesn't already exist */);
        allGood &= colRef->loadJson(jsonTree.getChild(name));
    }

    return allGood;
}
