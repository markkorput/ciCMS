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
      unsigned int counter = 0;

      std::string name = strs[0];

      // loop over each child to find the one with this name
      for(auto child : *node){
        auto n = (NodeT*)child;

        if (n->getName() == name) {
          // no more sub-names? return this object for this child
          if(strs.size() == 1){
            return n->template getObject<ObjT>();
          }

          // remove first name (one we just found) and move to on level deeper
          strs.erase(strs.begin());
          return std::make_shared<Selection>(*n)->template get<ObjT>(boost::algorithm::join(strs, "."));
        }
      }

      return NULL;
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

  private:
    NodeT* node;
};
