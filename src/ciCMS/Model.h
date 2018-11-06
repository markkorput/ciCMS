#pragma once
#include "ModelBase.h"

namespace cms{

    class Model : public ModelBase {
        public: // types
            typedef function<void(const string&)> AttributeTransformFunctor;
            typedef function<void(ModelBase& model)> ModelTransformFunctor;


            //! registers an attribute transformer
            ci::signals::Connection& transformAttribute(const string& attr, AttributeTransformFunctor func, bool active=true){
                return transformAttribute(attr, func, NULL, active);
            }

            ci::signals::Connection& transformAttribute(const string& attr, AttributeTransformFunctor func, void* owner, bool active=true){
                std::vector<string> attrs;
                attrs.push_back(attr);
                return transformAttributes(attrs, func, owner, active);
            }

            //! register the same transformer on various attributes
            ci::signals::Connection& transformAttributes(const std::vector<string> &attrs, AttributeTransformFunctor func, bool active=true){
                return transformAttributes(attrs, func, NULL, active);
            }

            ci::signals::Connection& transformAttributes(const std::vector<string> &attrs, AttributeTransformFunctor func, void* owner, bool active=true){
                auto connection = this->attributeChangeSignal.connect([func, attrs](const Model::AttrChangeArgs& args){
                    // see if we "have" the changed attribute
                    bool have_attribute = false;
                    for(auto& attr : attrs){
                        if(attr == args.attr){
                            have_attribute = true;
                            break;
                        }
                    }

                    if(have_attribute){
                        // perform "transformation"
                        func(args.value);
                    }
                });

                if(!active){
                    connection.disable();
                }

                for(auto& attr : attrs)
                    if(this->has(attr))
                        func(this->get(attr));

                attributeTransformerSignalConnections[owner].push_back(connection);
                return attributeTransformerSignalConnections[owner].back();
            }

            std::vector<ci::signals::Connection>& stopAttributeTransform(void* owner){
                auto& signalConnections = attributeTransformerSignalConnections[owner];

                for(auto& signalConnection : signalConnections)
                    signalConnection.disable();

                attributeTransformerSignalConnections[owner].clear();
                return signalConnections;
            }


            ci::signals::Connection& transform(ModelTransformFunctor func, void* owner = NULL, bool active=true){
                // register change listener to invoke the functor
                auto connection = this->changeSignal.connect(func);

                // invoke the functor now
                func(*this);

                if(active)
                    modelTransformerSignalConnections[owner].push_back(connection);
                else
                    connection.disable();

				return connection;
            }

            std::vector<ci::signals::Connection>& stopTransform(void* owner){
                auto& signalConnections = modelTransformerSignalConnections[owner];

                for(auto& signalConnection : signalConnections)
                    signalConnection.disable();

                modelTransformerSignalConnections[owner].clear();
                return signalConnections;
            }

        private:
            //! internal list of active value transformers, grouped by owner see .transform methods
            std::map<void*, std::vector<ci::signals::Connection>> attributeTransformerSignalConnections;
            std::map<void*, std::vector<ci::signals::Connection>> modelTransformerSignalConnections;
    };
}
