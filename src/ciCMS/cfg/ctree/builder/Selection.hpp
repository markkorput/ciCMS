// this file is included in the _body_ of the builder class in ../Buider.h

class Selection {
  public:
    Selection(NodeT& node) : node(&node) {
    }

    NodeT* getNode(){
      return node;
    }

    template<typename ObjT>
    ObjT* get(const string& path){
      std::vector<string> strs;
      boost::split(strs,path,boost::is_any_of("."));

      auto parentNode = this->getNode();

      for(auto str : strs) {
        Selection sel(*parentNode);
        parentNode = sel.getChildNodeWithName(str);

        if(parentNode == NULL)
          return NULL;
      }

      return parentNode->template getObject<ObjT>();
    }

    template<typename ObjT>
    ObjT* getSibling(const string& path){
      if (!node->parent()) {
        return NULL;
      }

      Selection sel(*(NodeT*)node->parent());
      return sel.get<ObjT>(path);
    }

    template<typename ObjT>
    void attach(ObjT* obj){
      auto objNode = (NodeT*)NodeT::fromObj<ObjT>(obj);
      node->add(*objNode);
    }

    const std::string& getName() {
      return node->getName();
    }

  protected:

    NodeT* getChildNodeWithName(const string& name) {
      for(auto child : *node)
        if (((NodeT*)child)->getName() == name)
          return (NodeT*)child;

      return NULL;
    }

  private:
    NodeT* node;
};
