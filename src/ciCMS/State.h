#pragma once

#include <memory>
#include <functional>
// #include "cinder/app/App.h"
#include "cinder/Signals.h"


using namespace std;

namespace cms {

    template<class StateType>
    class State {

    public: // types & constants

        typedef function<void(const StateType&)> PushFunc;

    private: // extensions

        typedef enum {
            UNKNOWN,
            PUSHER
        } ExtType;

        class StateExt {

            public:
                StateExt(State<StateType>& _state, void* _owner = NULL)
                    : state(&_state)
                    , owner(_owner)
                    , bEnabled(false)
                    , extType(UNKNOWN){
                }

                bool isEnabled(){ return bEnabled; }
                StateExt* enable(){ return this->setEnabled(true); }
                StateExt* disable(){ return this->setEnabled(false); }

                StateExt* setEnabled(bool enable){
                    if(bEnabled && !enable){
                        destroy();
                    }

                    if(enable && !bEnabled){
                        setup();
                    }

                    bEnabled = enable;
                    return this;
                }

            protected:

                virtual void setup(){};
                virtual void destroy(){};

                State<StateType>* state;

            private:
                bool bEnabled;

            public:
                void* owner;
                ExtType extType;
        };

        class StatePusher : public StateExt {

            public:

                StatePusher(State<StateType>& _state, PushFunc _func, void* _owner = NULL)
                    : StateExt(_state, _owner)
                    , func(_func){
                    this->extType = ExtType::PUSHER;
                }

            protected:

                virtual void setup(){
                    this->connection = this->state->newValueSignal.connect(this->func);
                    if(this->state->isInitialized())
                        this->func(this->state->getRef());
                }

                virtual void destroy(){
                    this->connection.disconnect();
                }

            private:

                PushFunc func;
                ci::signals::Connection connection;
        };

    public: // lifecycle methods

        State() : bInitialized(false){}

    public: // writer methods

        State<StateType>* operator=(const StateType &newValue){
            bool change = this->value != newValue;

            this->value = newValue;

            if(!bInitialized){
                bInitialized = true;
                this->initializeSignal.emit(*this);
            }

            if(change){
                this->newValueSignal.emit(this->value);
            }

            return this;
        }

    public: // reader methods

        const StateType& getRef(){ return value; }
        StateType val(){ return value; }
        bool isInitialized(){ return bInitialized; }

    public: // advanced operations methods

        void push(PushFunc func, void* owner = NULL){
            auto ext = make_shared<StatePusher>(*this, func, owner);
            ext->enable();
            extensions.push_back(ext);
        }

        /** Stops all extensions for the given owner */
        void stopPushes(void* owner){
            auto it = extensions.begin();
            while(it != extensions.end()){
                if((*it)->owner == owner && (*it)->extType == ExtType::PUSHER){
                    (*it)->disable();
                    it = extensions.erase(it);
                } else {
                    it++;
                }
            }
        }

    public: // events

        //! this event is triggered whenever the model changes (which means; when any attribute changes) and gives the caller a reference to this model
        ci::signals::Signal<void(const StateType&)> newValueSignal;
        ci::signals::Signal<void(State<StateType>&)> initializeSignal;

    private: // attributes

        bool bInitialized;
        StateType value;
        std::vector<shared_ptr<StateExt>> extensions;
    };
}
