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
          this->nodesById[this->identifierFunc(args)] = (Node*)args.node;
        })
      );

      // register callback for when the builder destroys an object
      connections.push_back(
        b->destroySignal.connect([this](DestroyArgs& args){
          auto node = (Node*)args.node;
          for(auto it = this->nodesById.begin(); it != this->nodesById.end(); it++) {
            if (it->second == node) {
              this->nodesById.erase(it);
              return;
            }
          }
        })
      );
    }

    ~Registry() {
      for(auto conn : connections) conn.disconnect();
      connections.clear();
    }

    inline void* getById(const std::string& id) {
      auto node = this->nodesById[id];
      return node ? node->getObject<void>() : NULL;
    }

    template<typename ObjT>
    inline ObjT* get(const std::string& id) {
      auto node = this->nodesById[id];
      return node->getObject<ObjT>();
    }

    inline void* getByIndex(int idx) {
      auto it = nodesById.begin();
      for (int i=0; i<idx; i++) it++;
      return it->second->getObject<void>();
    }

    template<typename ObjT>
    inline ObjT* getByIndex(int idx) {
      auto it = nodesById.begin();
      for (int i=0; i<idx; i++) it++;
      return it->second->getObject<ObjT>();
    }

    inline Node* getNodeByIndex(int idx) {
      auto it = nodesById.begin();
      for (int i=0; i<idx; i++) it++;
      return it->second;
    }

    size_t size() const { return nodesById.size(); }

  private:
    IdentifierFunc identifierFunc;
    std::vector<ci::signals::Connection> connections;
    std::map<std::string, Node*> nodesById;
};
