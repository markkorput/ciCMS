#include "TexDraw.h"

using namespace cms::cfg::components;
using namespace cinder;

void TexDraw::cfg(cms::cfg::Cfg& cfg) {
  cfg

  .connectAttr<void(ci::gl::TextureRef)>("textureOn", [this](ci::gl::TextureRef texref){
    this->mTex = texref;
  })

  .connectAttr<void()>("drawOn", [this](){ this->draw(); })

  .connectAttr<void(const Area&)>("fitCenteredOn", [this](const Area& area){
    this->fitCenteredAreaRef = std::make_shared<Area>(area);
  });
}

void TexDraw::draw() {
  if (!this->mTex) return;

  if (this->fitCenteredAreaRef) {
    Rectf centeredRect = Rectf( mTex->getBounds() ).getCenteredFit(*this->fitCenteredAreaRef, true );
    gl::draw(mTex, centeredRect);
    return;
  }

  if (this->rectRef) {
    gl::draw( mTex, *this->rectRef);
    return;
  }

  gl::draw(mTex);
}
