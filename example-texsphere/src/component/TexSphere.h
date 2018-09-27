#pragma once

#include "ciCMS/cfg/Cfg.h"
#include "cinder/gl/gl.h"

namespace component {
  class TexSphere {

    public:
      void cfg(cms::cfg::Cfg& cfg);

    private:
      void setup(const std::string& texAssetPath, const glm::vec3& pos, float radius);
      void draw();

    private:
      ci::gl::BatchRef batchRef;
      ci::gl::TextureRef texRef;
  };
}
