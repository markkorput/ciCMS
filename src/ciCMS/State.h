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

    public: // lifecycle methods

        State() : value(NULL){}

    public: // writer methods

        State<StateType>* operator=(const StateType &newValue){
            if(this->value == NULL){
                this->value = new StateType();
            }

            (*this->value) = newValue;

            return this;
        }


    public: // reader methods

        bool isInitialized(){ return value != NULL; }

    public: // advanced operations methods

        void push(PushFunc func){}
        void push(PushFunc func, void* owner){}
        void stopPushes(void* owner){}

    private: // operations

        void initialize(StateType* firstValue){
            //TODO? if this->value != null log warning?
            value = firstValue;
            this->initializeSignal.emit(*this);
        }

    public: // events

        //! this event is triggered whenever the model changes (which means; when any attribute changes) and gives the caller a reference to this model
        ci::signals::Signal<void(const StateType&)> newValueSignal;
        ci::signals::Signal<void(State<StateType>&)> initializeSignal;

    private: // attributes

        StateType* value;
    };
}
