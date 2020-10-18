#include "TexDraw.h"
#include "cinder/app/App.h"
using namespace cms::cfg::components;
using namespace cinder;

void TexDraw::cfg(cms::cfg::Cfg& cfg) {
  Base::cfg(cfg);

  verbose("TexDraw::cfg");

  cfg
  .setBool("fillViewport", this->bFillViewport)
  .connectAttr<void(ci::gl::TextureRef)>("textureOn", [this](ci::gl::TextureRef texref){
    verbose("TexDraw.textureOn");
    this->mTex = texref;
  })

  .connectAttr<void()>("drawOn", [this](){
      if (this->isEnabled())
        this->draw(); })

  .connectAttr<void(const Area&)>("fillBoundsOn", [this](const Area& area){
    verbose("TexDraw.fillBoundsOn");
    this->rectRef = std::make_shared<Rectf>(area);
    this->fitCenteredAreaRef = nullptr;
    this->fillCenteredAreaRef = nullptr;
  })
  .connectAttr<void(const Area&)>("fitCenteredOn", [this](const Area& area){
    verbose("TexDraw.fitCenteredOn");
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
      
      if (!this->winSizeListenerRef) {
        winSizeListenerRef = std::make_shared<utils::WinSizeListener>();
        winSizeListenerRef->setWinSizeCallback([this](const ci::ivec2& size){ this->updateWindowSize(size); });
      }

      this->winPercentagebounds = std::make_shared<ci::vec4>(bounds.x, bounds.y, bounds.z, bounds.w);
      auto winSize = winSizeListenerRef->getWindowSize();
      this->updateWindowSize(winSize);
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
    // verbose("TexDraw.draw.fitCenteredAreaRef texbounds:" << mTex->getBounds() <<", area: "<< *fitCenteredAreaRef);
    Rectf rect = Rectf( mTex->getBounds() ).getCenteredFit(*this->fitCenteredAreaRef, true );
    gl::draw(mTex, rect);
    return;
  }

  if (this->fillCenteredAreaRef) {
    // verbose("TexDraw.draw.fillCenteredAreaRef texbounds:" + mTex->getBounds() <<", area: "<< *fitCenteredAreaRef);
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

void TexDraw::updateWindowSize(const ci::ivec2& size) {
  if (winPercentagebounds) {
    this->rectRef = std::make_shared<Rectf>(
      winPercentagebounds->x * size.x, 
      winPercentagebounds->y * size.y,
      winPercentagebounds->z * size.x,
      winPercentagebounds->w * size.y);
  }
}