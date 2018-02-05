#pragma once

// cinder
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
// blocks
#include "cinderSyphon.h"

class SyphonClientRenderer {
  public:
    SyphonClientRenderer() : mClient(NULL){}

    void draw() {
      if (mClient) {
        ci::gl::draw(mClient->getTexture());
      }
    }

    void setSyphonClient(syphonClient* client) {
      mClient = client;
    }

  private:
    syphonClient* mClient;
};
