#pragma once

#include <functional>
#include "cinder/Signals.h"
#include "CollectionBase.h"

namespace cms {
    class CollectionTransformerBase {
    public:
        // ~CollectionTransformerBase(){ this->destroy(); }
        // virtual void destroy(){}
        virtual void* getSource() = 0;
    };

    template<class SourceType, class TargetType>
    class CollectionTransformer : public CollectionTransformerBase {
        public:
            typedef function<shared_ptr<TargetType>(SourceType&)> TransformFunctor;

        public: // methods

            CollectionTransformer() : sourceCollection(NULL), targetCollection(NULL){}
            ~CollectionTransformer(){ destroy(); }

            void setup(
                CollectionBase<SourceType> &source,
                CollectionBase<TargetType> &target,
                TransformFunctor func);

            virtual void destroy();

            void* getSource() override { return (void*)sourceCollection; }

        private: // attributes

            CollectionBase<SourceType> *sourceCollection;
            CollectionBase<TargetType> *targetCollection;
            TransformFunctor func;
            std::map<SourceType*, TargetType*> transformLinks;
            std::vector<ci::signals::Connection> signalConnections;
    };
}

template<class SourceType, class TargetType>
void cms::CollectionTransformer<SourceType, TargetType>::setup(
    CollectionBase<SourceType> &source,
    CollectionBase<TargetType> &target,
    TransformFunctor func){

    this->sourceCollection = &source;
    this->targetCollection = &target;
    this->func = func;

    // transform all instances currently in source
    this->sourceCollection->each([this](shared_ptr<SourceType> sourceRef){
        // generate tranformed instance
        auto targetRef = this->func(*sourceRef.get());
        this->targetCollection->add(targetRef);
        // record a "link-record" between the source and target instances
        this->transformLinks[sourceRef.get()] = targetRef.get();
    });

    // actively transform new instances appearing in source
    auto conn = this->sourceCollection->addSignal.connect([&](SourceType& sourceInstance){
        // generate tranformed instance
        auto targetRef = this->func(sourceInstance);
        this->targetCollection->add(targetRef);
        // record a "link-record" between the source and target instances
        this->transformLinks[&sourceInstance] = targetRef.get();
    });

    signalConnections.push_back(conn);

    // actively remove generated instances when instances are removed from source
    conn = this->sourceCollection->removeSignal.connect([this](SourceType& sourceInstance){
        // find "link-record" for removed source instance
        auto it = this->transformLinks.find(&sourceInstance);

        if(it == this->transformLinks.end()){
            // ofLogWarning() << "Could not find transformed match for removed source instance";
            return;
        }

        // remove the generated target instance
        this->targetCollection->remove(it->second);

        // remove the link
        this->transformLinks.erase(it);
    });

    signalConnections.push_back(conn);
}

template<class SourceType, class TargetType>
void cms::CollectionTransformer<SourceType, TargetType>::destroy(){
    for(auto& conn : signalConnections){
        conn.disconnect();
    }

    signalConnections.clear();
    this->sourceCollection = NULL;
    this->targetCollection = NULL;
}
