#include "TexDraw.h"
#include "cinder/app/App.h"
using namespace cms::cfg::components;
using namespace cinder;

void TexDraw::cfg(cms::cfg::Cfg& cfg) {
  cfg

  .setBool("verbose", this->verbose)

  .connectAttr<void(ci::gl::TextureRef)>("textureOn", [this](ci::gl::TextureRef texref){
    if (verbose) CI_LOG_I("TexDraw.textureOn");
    this->mTex = texref;
  })

  .connectAttr<void()>("drawOn", [this](){ this->draw(); })

  .connectAttr<void(const Area&)>("fitCenteredOn", [this](const Area& area){
    if (verbose) CI_LOG_I("TexDraw.fitCenteredOn");
    this->fitCenteredAreaRef = std::make_shared<Area>(area);
  });
}

void TexDraw::draw() {
  if (!this->mTex) return;

  // make configurable?
  gl::ScopedColor clrScp(1,1,1);

  if (this->fitCenteredAreaRef) {
    if (verbose) CI_LOG_I("TexDraw.draw.fitCenteredAreaRef texbounds:" << mTex->getBounds() <<", area: "<< *fitCenteredAreaRef);
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
