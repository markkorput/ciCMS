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

#ifndef CINDER_MSW
namespace cms {
	template<>
	shared_ptr<Model> CollectionJsonLoader<Model>::findMatch(ci::JsonTree& jsonTree, CollectionBase<Model>& collection) {
    auto id = jsonTree.hasChild("id") ? jsonTree.getValueForKey("id") : jsonTree.getKey();
		// if (!jsonTree.hasChild("id"))
		// 	return nullptr;
    //
		// std::string id = jsonTree.getValueForKey("id");

		return collection.first([&id](shared_ptr<Model> modelRef) {
			return modelRef->get("id") == id;
		});
	}

	template<>
	bool CollectionJsonLoader<Model>::loadItem(ci::JsonTree& jsonTree, shared_ptr<Model> itemRef) {
		for (int idx = 0; idx < jsonTree.getNumChildren(); idx++) {
			ci::JsonTree subTree = jsonTree.getChild(idx);
			itemRef->set(subTree.getKey(), subTree.getValue());
		}

    if (!jsonTree.hasChild("id")) {
        itemRef->set("id", jsonTree.getKey());
    }

		return true;
	}

	template<>
	ci::JsonTree CollectionJsonWriter<Model>::getItemJsonTree(shared_ptr<Model> itemRef) {
		ci::JsonTree tree;
		itemRef->each([&](const string& attr, const string& value) {
			tree.addChild(ci::JsonTree(attr, value));
		});

		return tree;
	}

}
#endif
