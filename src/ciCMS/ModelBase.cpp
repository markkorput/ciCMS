#include "ModelBase.h"
#include "cinder/Log.h"
#include "boost/lexical_cast.hpp"

using namespace cms;

ModelBase* ModelBase::set(const string &attr, const string &value, bool notify){
    if(isLocked()){
        CI_LOG_V("model locked; queueing .set operation for attribute: " << attr);
        modQueueRefs.push_back(make_shared<Mod>(attr, value, notify));
        return this;
    }

    string old_value = _attributes[attr];

    _attributes[attr] = value;
    onSetAttribute(attr, value);

    if(notify && old_value != value){
        AttrChangeArgs args;
        args.model = this;
        args.attr = attr;
        args.value = value;
        onAttributeChanged(attr, value, old_value);

        if(notify){
            changeSignal.emit(*this);
            attributeChangeSignal.emit(args);
        }
    }

    // returning `this` allows the caller to link operations, like so:
    // model.set('name', 'Johnny')->set('surname', 'Blaze')->set('age', '44');
    return this;
}


ModelBase* ModelBase::set(map<string, string> &attrs, bool notify){
    for(map<string, string>::iterator it=attrs.begin(); it != attrs.end(); it++){
        this->set(it->first, it->second, notify);
    }

	return this;
}

string ModelBase::get(const string &attr, string _default) const {
    return has(attr) ? _attributes.at(attr) : _default;
}

bool ModelBase::has(const string& attr) const {
    return (_attributes.find(attr) == _attributes.end()) ? false : true;
}

void ModelBase::each(AttrIterateFunc func){
    lock([this, &func](){
        for(auto pair : this->_attributes){
            func(pair.first, pair.second);
        }
    });
}

void ModelBase::copy(shared_ptr<ModelBase> otherRef, bool also_ids){
    if(!otherRef){
        CI_LOG_W("ofxCMS::ModelBase.copy got nullptr");
        return;
    }

    copy(*otherRef.get(), also_ids);
}

void ModelBase::copy(ModelBase& other, bool also_ids){
    other.each([this, also_ids](const string& key, const string& value){
        if((key != "id" && key != "_id") || also_ids)
            this->set(key, value);
    });
}

void ModelBase::lock(LockFunctor func){
    lockCount++;
    func();
    lockCount--;

    // still (recursively) locked? skip processing of opereations queue
    if(isLocked())
        return;

    // after we're done iterating, we should process any items
    // accumulated in the vector modificaton queue
    for(auto modRef : modQueueRefs){
        set(modRef->attr, modRef->value, modRef->notify);
        // TODO; add support for removing attributes?
    }

    modQueueRefs.clear();
}

int ModelBase::getInt(const string& attr, int defaultValue){
    try {
        return std::stoi(get(attr));
    } catch(std::invalid_argument){
    }

    return defaultValue;
}

float ModelBase::getFloat(const string& attr, float defaultValue){
    try {
        return std::stof(get(attr));
    } catch(std::invalid_argument){
    }

    return defaultValue;
}

bool ModelBase::getBool(const string& attr, bool defaultValue){
    string s = get(attr);

    if(s == "true")
        return true;

    try {
        return boost::lexical_cast<bool>(s);
    } catch(boost::bad_lexical_cast){
    }

    return defaultValue;
}

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
ci::ColorAf ModelBase::getColor(const string& attr, const ci::ColorAf& defaultValue){
    std::vector<std::string> strings;
    string src = get(attr);
    boost::split(strings, src, boost::is_any_of(","));

    if(strings.size() == 1){
        if(strings[0] == "")
            return defaultValue;

        float val = std::stof(strings[0]);
        return ci::ColorAf(val/255.0f, val/255.0f, val/255.0f, 1.0f);
    }

    if(strings.size() == 3)
        return ci::ColorAf(std::stof(strings[0])/255.0f, std::stof(strings[1])/255.0f, std::stof(strings[2])/255.0f, 1.0f);

    if(strings.size() == 4)
        return ci::ColorAf(std::stof(strings[0])/255.0f, std::stof(strings[1])/255.0f, std::stof(strings[2])/255.0f, std::stof(strings[3])/255.0f);

    // unsupported/unrecognised format
    return defaultValue;
}
