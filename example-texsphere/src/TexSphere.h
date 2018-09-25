#pragma once

#include "ctree/signal.hpp"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

class TexSphere {

  public:

    void cfg(cms::cfg::Cfg& cfg){
      cfg
      .connectAttr<void()>("drawOn", [this](){ this->draw(); })
      .set("tex", this->texAssetPath)
      .setVec3("pos", this->pos)
      .setFloat("radius", this->radius);
      // TEX

      this->setup();
    }



  private:

    void setup() {
      auto geom = ci::geom::Sphere()
        .subdivisions(64)
        .radius(radius)
        .center(pos);

      CI_LOG_W("radius" << radius << "pos: " << pos);

      this->batchRef = ci::gl::Batch::create(
        geom,
        //ci::gl::getStockShader(ci::gl::ShaderDef().color()));
        ci::gl::getStockShader(ci::gl::ShaderDef().texture()));

      ci::fs::path path = ci::app::getAssetPath(texAssetPath);

      if(!ci::fs::exists(path)){
        CI_LOG_W("Texture file does not exist: " << path.string());
      } else {
        auto imageSourceRef = ci::loadImage(path);

        if (!imageSourceRef) {
            CI_LOG_W("Couldn't load image" << path.string());
        } else {
          this->texRef = ci::gl::Texture::create(imageSourceRef);
        }
      }
    }

    void draw() {
      if (this->batchRef) {
          if(this->texRef){
            ci::gl::ScopedTextureBind tex0(this->texRef);
            this->batchRef->draw();
          } else {
            this->batchRef->draw();
          }

          // ci::gl::popModelMatrix();
      }
    }

  private:
    // configurables
    std::string texAssetPath;
    glm::vec3 pos;
    float radius = 1.0f;

    // attributes
    ci::gl::BatchRef batchRef;
    ci::gl::TextureRef texRef;
};
