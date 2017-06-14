#pragma once

#include <memory>
#include <functional>
#include "cinder/app/App.h"
// #include "cinder/Signals.h"

using namespace std;

namespace cms {
    class ModelBase {

    public:
        typedef void* CidType;

        typedef function<void(const string&, const string&)> AttrIterateFunc;
        typedef function<void(void)> LockFunctor;

        // used in attributeChangeEvent notifications
        class AttrChangeArgs {
        public:
            ModelBase *model;
            string attr;
            string value;
        };

        //! Sub-type used a record-format when queueing modifications while to model is locked
        class Mod {
            public:
                string attr, value;
                bool notify;
                Mod(const string& _attr, const string& _value, bool _notify=true) : attr(_attr), value(_value), notify(_notify){}
        };

    public:

        ModelBase() : lockCount(0){}
        ModelBase* set(const string &attr, const string &value, bool notify = true);
        ModelBase* set(map<string, string> &attrs, bool notify=true);
        string get(const string &attr, string _default = "") const;

        string getId() const { return get("id", get("_id")); }
        CidType cid() const { return (CidType)this; }

        const map<string, string> &attributes() const { return _attributes; }

        bool has(const string& attr) const;
        bool equals(shared_ptr<ModelBase> other){ return other->cid() == cid(); }
        size_t size() const { return _attributes.size(); }

        void each(AttrIterateFunc func);
        void copy(shared_ptr<ModelBase> otherRef, bool also_ids=false);
        void copy(ModelBase& other, bool also_ids=false);

    protected: // methods

        bool isLocked() const { return lockCount > 0; }
        void lock(LockFunctor func);

    protected: // callbacks

        //! this virtual method is called whenever an attribute is written (using this->set()) and can be overwritten be inheriting classes
        virtual void onSetAttribute(const string &attr, const string &value){}
        //! this virtual method is called whenever an attribute is changed (using this->set()) and can be overwritten be inheriting classes
        virtual void onAttributeChanged(const string &attr, const string &value, const string &old_value){}

    public: // conversion getters

        int getInt(const string& attr, int defaultValue = 0);
        float getFloat(const string& attr, float defaultValue = 0.0f);
        bool getBool(const string& attr, bool defaultValue = false);
        ci::ColorAf getColor(const string& attr, const ci::ColorAf& defaultValue = ci::ColorAf(1.0f, 1.0f, 1.0f, 1.0f));

    public: // events

        //! this event is triggered whenever the model changes (which means; when any attribute changes) and gives the caller a reference to this model
        ci::signals::Signal<void(ModelBase&)> changeSignal;
        //! this event is triggered whenever the model changes (which means; when any attribute changes) and gives the caller an object with a pointer to the model and information about which attribute changed
        ci::signals::Signal<void(const AttrChangeArgs&)> attributeChangeSignal;


    private: // attributes

        //! the internal storage map for this model's attributes
        map<string, string> _attributes;
        // a counter to track the number of active (recursive) locks
        int lockCount;
        std::vector<shared_ptr<Mod>> modQueueRefs;
    };
}
