#include "cinder/Log.h"
#include "ModelBase.h"
#include "deserialise.h"

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


ModelBase* ModelBase::set(const map<string, string> &attrs, bool notify){
    for(auto it=attrs.begin(); it != attrs.end(); it++){
        this->set(it->first, it->second, notify);
    }

	return this;
}

const string ModelBase::get(const string &attr, const string& _default) const {
    string tmp = has(attr) ? _attributes.at(attr) : _default;
    return tmp;
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


ModelBase* ModelBase::setBool(const string &attr, bool val, bool notify) {
    return this->set(attr, (string)(val ? "true" : "false"), notify);
}

ModelBase* ModelBase::set(const string &attr, const ci::vec2& val, bool notify) {
    return this->set(attr, std::to_string(val.x) +","+std::to_string(val.y), notify);
}

ModelBase* ModelBase::set(const string &attr, const ci::vec3& val, bool notify) {
    return this->set(attr, std::to_string(val.x) +","+std::to_string(val.y) +","+std::to_string(val.z), notify);
}


int ModelBase::getInt(const string& attr, int defaultValue){
  std::string tmp = this->get(attr);
  return cms::deserialiseInt(tmp, defaultValue);
}

float ModelBase::getFloat(const string& attr, float defaultValue){
  std::string tmp = this->get(attr);
  return cms::deserialiseFloat(tmp, defaultValue);
}

bool ModelBase::getBool(const string& attr, bool defaultValue){
  std::string tmp = this->get(attr);
  return cms::deserialiseBool(tmp, defaultValue);
}

glm::vec2 ModelBase::getVec2(const string& attr, const glm::vec2& defaultValue){
  const std::string tmp = this->get(attr);
  std::cout << "tmp: " << tmp << "(length: "<<tmp.length()<<")" << std::endl;
  auto v = cms::deserialiseVec2(tmp, defaultValue);
  std::cout << "tmp: " << tmp << "(length: "<<tmp.length()<<")" << std::endl;
  return v;
}

glm::vec3 ModelBase::getVec3(const string& attr, const glm::vec3& defaultValue){
  std::string tmp = this->get(attr);
  return cms::deserialiseVec3(tmp, defaultValue);
}

ci::ColorAf ModelBase::getColor(const string& attr, const ci::ColorAf& defaultValue){
  std::string tmp = this->get(attr);
  return cms::deserialiseColor(tmp, defaultValue);
}

bool ModelBase::with(const string& attr, function<void(const string&)> func){
    if(!this->has(attr))
        return false;
    func(this->get(attr));
    return true;
}

bool ModelBase::withInt(const string& attr, function<void(const int&)> func){
    if(!this->has(attr))
        return false;
    func(this->getInt(attr));
    return true;
}

bool ModelBase::withFloat(const string& attr, function<void(float)> func){
    if(!this->has(attr))
        return false;
    func(this->getFloat(attr));
    return true;
}


bool ModelBase::withBool(const string& attr, function<void(const bool&)> func){
    if(!this->has(attr))
        return false;

    func(this->getBool(attr));
    return true;
}

bool ModelBase::withVec2(const string& attr, function<void(const glm::vec2&)> func){
    if(!this->has(attr))
        return false;

    func(this->getVec2(attr));
    return true;
}

bool ModelBase::withVec3(const string& attr, function<void(const glm::vec3&)> func){
    if(!this->has(attr))
        return false;

    func(this->getVec3(attr));
    return true;
}
