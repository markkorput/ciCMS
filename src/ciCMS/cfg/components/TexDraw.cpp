#include "TexDraw.h"
#include "cinder/app/App.h"
using namespace cms::cfg::components;
using namespace cinder;

void TexDraw::cfg(cms::cfg::Cfg& cfg) {

  cfg
  .setBool("verbose", this->verbose)
  .setBool("fillViewport", this->bFillViewport)
  .connectAttr<void(ci::gl::TextureRef)>("textureOn", [this](ci::gl::TextureRef texref){
    if (verbose) CI_LOG_I("TexDraw.textureOn");
    this->mTex = texref;
  })

  .connectAttr<void()>("drawOn", [this](){ this->draw(); })
  .connectAttr<void(const Area&)>("fillBoundsOn", [this](const Area& area){
    if (verbose) CI_LOG_I("TexDraw.fillBoundsOn");
    this->rectRef = std::make_shared<Rectf>(area);
    this->fitCenteredAreaRef = nullptr;
    this->fillCenteredAreaRef = nullptr;
  })
  .connectAttr<void(const Area&)>("fitCenteredOn", [this](const Area& area){
    if (verbose) CI_LOG_I("TexDraw.fitCenteredOn");
    this->fitCenteredAreaRef = std::make_shared<Area>(area);
    this->fillCenteredAreaRef = nullptr;
    this->rectRef = nullptr;
  })

  .reader()
    ->withVec4("bounds", [this](const ci::vec4& bounds){
      this->rectRef = std::make_shared<Rectf>(bounds.x, bounds.y, bounds.z, bounds.w);
      this->fillCenteredAreaRef = nullptr;
      this->fitCenteredAreaRef = nullptr;
    })

    .withVec4("winPercentageBounds", [this](const ci::vec4& bounds){
      ci::vec2 winsize = (ci::vec2)ci::app::getWindowSize();

      this->rectRef = std::make_shared<Rectf>(
        bounds.x * winsize.x, 
        bounds.y * winsize.y,
        bounds.z * winsize.x,
        bounds.w * winsize.y);
      this->fillCenteredAreaRef = nullptr;
      this->fitCenteredAreaRef = nullptr;
    });
}

void TexDraw::draw() {
  if (!this->mTex) return;

  // make configurable?
  gl::ScopedColor clrScp(1,1,1);

  if (this->bFillViewport) {
    auto viewport = gl::getViewport();
    Rectf rect(viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y);
    gl::draw(mTex, rect);
    return;
  }

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
