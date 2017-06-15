#include "ModelCollection.h"
#include "CollectionJsonLoader.h"
using namespace cms;

shared_ptr<Model> ModelCollection::findByAttr(const std::string& attr, const std::string& value, bool createIfNotExist){
    auto model = this->first([&attr, &value](shared_ptr<Model> iterModel){
        return iterModel->get(attr) == value;
    });

    if(!model && createIfNotExist){
        model = make_shared<Model>();
        model->set(attr, value);
        this->add(model);
    }

    return model;
}

shared_ptr<Model> ModelCollection::findById(const std::string& value, bool createIfNotExist){
    return findByAttr(mIdAttributeName, value, createIfNotExist);
}
