#pragma once

#include <memory>
#include <functional>
// #include "cinder/app/App.h"
#include "cinder/Signals.h"
#include "cinder/Log.h"


using namespace std;

namespace cms {

    template<class StateType>
    class State {

    public: // types & constants

        typedef function<void(const StateType&)> PushFunc;

        struct ChangeArgs {
            StateType previous;
            StateType current;
			bool previouslyInitialized;
			
            ChangeArgs(StateType p, StateType c, bool previnit) : previous(p), current(c), previouslyInitialized(previnit) {}
        };

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

        class StateValueRunner : public StateExt {
          public:
            StateType value;
            std::function<void(void)> func;
            int count = 0;
            int maxTimes = -1;
            bool bNegative = false;
            std::vector<ci::signals::Connection> connections;

            StateValueRunner(State<StateType>& state, StateType value, std::function<void(void)> func, bool negative=false)
              : StateExt(state, NULL), value(value), func(func), bNegative(negative){}

            StateValueRunner* setMaxTimes(int times){ this->maxTimes = times; return this; }
            StateValueRunner* setOnce(){ return this->setMaxTimes(1); }

            StateValueRunner setIsNegative(bool negative){ this->bNegative = negative; return this; }
            bool isNegative(){ return this->bNegative; }

          protected:
            void setup(){
              this->connections.push_back(
                this->state->newValueSignal.connect(
                  [this](StateType val){ this->check(val); }));

              this->check(this->state->val());
            }

            void destroy(){
              for(auto conn : this->connections)
                conn.disconnect();
              this->connections.clear();
            }

            void check(StateType val){
              bool equal = (val == this->value);

              if(equal ^ this->bNegative)
                this->run();
            }

            void run(){
              this->func();
              count += 1;

              if(this->maxTimes != -1 && count >= this->maxTimes){
                this->destroy();
              }
            }
        };

    public: // lifecycle methods

        State() : bInitialized(false){}

    public: // writer methods

        State<StateType>* set(const StateType val) {
          return this->operator=(val);
        }

        State<StateType>* operator=(const StateType &newValue){
            bool change = this->value != newValue;
            StateType prevValue = this->value;
            this->value = newValue;

			bool wasInit = bInitialized;

            if(!bInitialized){
                bInitialized = true;
                this->initializeSignal.emit(*this);
            }

            if(change){
                // TODO; perform NULL check?
                this->newValueSignal.emit(this->value);
                ChangeArgs args(prevValue, this->value, wasInit);
                this->changeSignal.emit(args);
            }

            return this;
        }

    public: // reader methods

        const StateType& getRef(){ return value; }
        StateType val() const { return value; }
        bool isInitialized() const { return bInitialized; }

    public: // advanced operations methods

        void push(State<StateType>& otherState) {
          this->push([&otherState](const StateType& v){ otherState = v; }, &otherState);
        }

        void push(PushFunc func, void* owner = NULL){
            auto ext = std::make_shared<StatePusher>(*this, func, owner);
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

        State<StateType>* when(StateType value, std::function<void()> func){
          auto ext = std::make_shared<StateValueRunner>(*this, value, func);
          ext->enable();
          extensions.push_back(ext);
          return this;
        }

        State<StateType>* whenNot(StateType value, std::function<void()> func){
          auto ext = std::make_shared<StateValueRunner>(*this, value, func, true);
          ext->enable();
          extensions.push_back(ext);
          return this;
        }

        State<StateType>* whenNot(StateType value, std::function<void(const StateType&)> func){
          auto funcWrapper = [this, &func](){ func(this->getRef()); };
          return this->whenNot(value, funcWrapper);
        }

        State<StateType>* whenOnce(StateType value, std::function<void()> func){
          auto ext = std::make_shared<StateValueRunner>(*this, value, func);
          ext->setOnce();
          ext->enable();
          extensions.push_back(ext);
          return this;
        }

    public: // signals

        ci::signals::Signal<void(const StateType&)> newValueSignal;
        ci::signals::Signal<void(State<StateType>&)> initializeSignal;
        ci::signals::Signal<void(ChangeArgs&)> changeSignal;

    private: // attributes

        bool bInitialized;
        StateType value;
        std::vector<shared_ptr<StateExt>> extensions;
    };
}
