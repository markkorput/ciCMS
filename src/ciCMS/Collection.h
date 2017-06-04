#pragma once

#include "CollectionBase.h"
#include "CollectionLimit.h"

namespace cms {
    template<class ItemType>
    class Collection : public CollectionBase<ItemType> {
        public:
            void limit(unsigned int amount){
                collectionLimit.setup(this, amount);
            }

            void setFifo(bool newFifo){
                collectionLimit.setFifo(newFifo);
            }

        private:
            CollectionLimit<ItemType> collectionLimit;
    };
}
