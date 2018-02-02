#pragma once

// cinder
#include "cinder/gl/gl.h"
// blocks
#include "cinderSyphon.h"

class SyphonClientRenderer {
  public:
    void draw() {
      if (mClient)
        ci::gl::draw(mClient->getTexture());
    }

    void setSyphonClient(syphonClient* client) { mClient = client; }

  private:
    syphonClient* mClient;
};
