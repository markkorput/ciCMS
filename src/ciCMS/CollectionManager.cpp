#include "CollectionManager.h"

#include "cinder/Log.h"
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

    if(dataSourceRef == nullptr || dataSourceRef->getBuffer() == nullptr){
        CI_LOG_W("could not load json file: " << path);
        return false;
    }

    string fileContentString( static_cast<const char*>( dataSourceRef->getBuffer()->getData() ));

    if(fileContentString.length() > dataSourceRef->getBuffer()->getSize())
        fileContentString.resize( dataSourceRef->getBuffer()->getSize() );

    try{
      ci::JsonTree jsonTree(fileContentString);
      return loadJson(jsonTree);
    } catch(const cinder::JsonTree::ExcJsonParserError &err){
        CI_LOG_E("Malformed json: " << err.what());
    }

    return false;
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
