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
  .connectAttr<void(const Area&)>("fillOn", [this](const Area& area){
    if (verbose) CI_LOG_I("TexDraw.fillOn");
    this->rectRef = std::make_shared<Rectf>(area);
    this->fitCenteredAreaRef = nullptr;
    this->fillCenteredAreaRef = nullptr;
  })
  .connectAttr<void(const Area&)>("fitCenteredOn", [this](const Area& area){
    if (verbose) CI_LOG_I("TexDraw.fitCenteredOn");
    this->fitCenteredAreaRef = std::make_shared<Area>(area);
    this->fillCenteredAreaRef = nullptr;
    this->rectRef = nullptr;
  });

  if (cfg.reader()->has("bounds")) {
    vec4 bounds;
    cfg.setVec4("bounds", bounds);
    // Area area=this->fixedAreaRef = std::make_shared<Area>(bounds.x, bounds.y, bounds.z, bounds.w);
    this->rectRef = std::make_shared<Rectf>(bounds.x, bounds.y, bounds.z, bounds.w);
    this->fillCenteredAreaRef = nullptr;
    this->fitCenteredAreaRef = nullptr;
  }
}

void TexDraw::draw() {
  if (!this->mTex) return;

  // make configurable?
  gl::ScopedColor clrScp(1,1,1);

  if (this->fitCenteredAreaRef) {
    if (verbose) CI_LOG_I("TexDraw.draw.fitCenteredAreaRef texbounds:" << mTex->getBounds() <<", area: "<< *fitCenteredAreaRef);
    Rectf rect = Rectf( mTex->getBounds() ).getCenteredFit(*this->fitCenteredAreaRef, true );
    gl::draw(mTex, rect);
    return;
  }

  if (this->fillCenteredAreaRef) {
    if (verbose) CI_LOG_I("TexDraw.draw.fillCenteredAreaRef texbounds:" << mTex->getBounds() <<", area: "<< *fitCenteredAreaRef);
    Rectf rect = Rectf( mTex->getBounds() ).getCenteredFill(*this->fillCenteredAreaRef, true );
    gl::draw(mTex, rect);
    return;
  }

  if (this->rectRef) {
    gl::draw( mTex, *this->rectRef);
    return;
  }

  gl::draw(mTex);
}
