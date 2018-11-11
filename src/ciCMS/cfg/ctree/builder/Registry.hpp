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
        })
      );
    }

    ~Registry() {
      for(auto conn : connections) {
        conn.disconnect();
      }

      connections.clear();
    }

    void* getById(const std::string& id) {
      return this->objectsById[id];
    }

    template<typename ObjT>
    ObjT* get(const std::string& id) {
      return (ObjT*)this->objectsById[id];
    }

    size_t size() const { return objectsById.size(); }
    
  private:
    IdentifierFunc identifierFunc;
    std::vector<ci::signals::Connection> connections;
    std::map<std::string, void*> objectsById;
};
