// this file is included in the _body_ of the builder class in ../Buider.h

class Registry {
  public:
    typedef std::function<std::string(BuildArgs&)> IdentifierFunc;

  public:
    Registry(TreeBuilder* b) : Registry(b, [](BuildArgs& args) { return args.data->getId(); }) {}

    Registry(TreeBuilder* b, IdentifierFunc idFunc) : identifierFunc(idFunc) {
      // register callback for when the builder builds an object
      connections.push_back(
        b->buildSignal.connect([this](BuildArgs& args){
          // CI_LOG_I("Registry detected: " << this->identifierFunc(args));
          this->objectsById[this->identifierFunc(args)] = args.object;
          // this->nodesById[this->identifierFunc(args)] = (Node*)args.node;
        })
      );

      // register callback for when the builder destroys an object
      connections.push_back(
        b->destroySignal.connect([this](DestroyArgs& args){
          for(auto it = this->objectsById.begin(); it != this->objectsById.end(); it++) {
            if (it->second == args.object) {
              this->objectsById.erase(it);
              return;
            }
          }

          // auto node = (Node*)args.node;
          // for(auto it = this->nodesById.begin(); it != this->nodesById.end(); it++) {
          //   if (it->second == node) {
          //     this->nodesById.erase(it);
          //     return;
          //   }
          // }
        })
      );
    }

    ~Registry() {
      for(auto conn : connections) conn.disconnect();
      connections.clear();
    }

    void* getById(const std::string& id) {
      return this->objectsById[id];
      // auto node = this->nodesById[id];
      // return node->getObject<void>();
    }

    void* getByIndex(int idx) {
      // return (objectsById.begin() + idx).second;
      return NULL;
    }

    template<typename ObjT>
    ObjT* get(const std::string& id) {
      return (ObjT*)this->objectsById[id];
      // auto node = this->nodesById[id];
      // return node->getObject<ObjT>();
    }

    size_t size() const { return objectsById.size(); }
    // size_t size() const { return nodesById.size(); }

  private:
    IdentifierFunc identifierFunc;

    std::map<std::string, void*> objectsById;
    std::map<std::string, Node*> nodesById;
    std::vector<ci::signals::Connection> connections;
};
