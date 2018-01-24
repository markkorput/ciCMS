#include "Builder.h"

using namespace cms::cfg;

void* Builder::doBuild(const string& id, bool recursive){
  auto model = this->modelCollection->findById(id, true);
  auto instantiator = this->findInstantiator(model->get("type"));

  if(!instantiator){
    std::cerr << "Could not find instantiator for type: " << model->get("type");
    return NULL;
  }

  auto instance = instantiator->func(*model);

  if(recursive){
    this->withEachChildId(id, [this, &instantiator, instance](const string& childId){
      auto childmodel = this->modelCollection->findById(childId, true);
      auto childinstantiator = this->findInstantiator(childmodel->get("type"));

      if(childinstantiator){
        auto child = this->doBuild(childId, true);

        if(child){
          auto connector = this->findConnector(childinstantiator->rootClassName, instantiator->rootClassName);
          if(connector){
            connector->func(child, instance);
          }
        }
      }
    });
  }

  return instance;
}

void Builder::addInstantiator(const string& name, const string& rootName, InstantiatorFunc func){
  this->instantiators.push_back(
    std::make_shared<Instantiator>(name, rootName, func));
}

void Builder::addConnector(const string& childName, const string& parentName, ConnectorFunc func){
  this->connectors.push_back(
    std::make_shared<Connector>(childName, parentName, func));
}

Builder::InstantiatorRef Builder::findInstantiator(const string& className){
  for(auto ref : this->instantiators)
    if(ref->className == className)
      return ref;

  return nullptr;
}

Builder::ConnectorRef Builder::findConnector(const string& childClass, const string& parentClass){
  for(auto ref : this->connectors)
    if(ref->parentClass == parentClass && ref->childClass == childClass)
      return ref;

  return nullptr;
}

void Builder::withEachChildId(const string& parentId, std::function<void(const string& childId)> func){
  const std::string prefix = parentId+".";

  // loop over all item in our modelCollection
  for(unsigned int i=0; i<this->modelCollection->size(); i++){
    auto model = this->modelCollection->at(i);
    const string cId = model->getId();
    // call the func with this item's ID if the id starts with the parentId prefix
    if(model && cId.find(prefix) == 0){
      func(cId);
    }
  }
}
