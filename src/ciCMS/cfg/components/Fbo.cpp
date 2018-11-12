// "App.Fbo": {"size":"2048,1024", "drawOn":"update", "drawEmit":"drawVideoFbo", "boundsEmit":"videoFboBounds", "textureEmit":"vidFboFrameTexture"},
#include "cinder/Log.h"
#include "Fbo.h"

using namespace std;
using namespace cinder;
using namespace cms::cfg::components;

void Fbo::cfg(cms::cfg::Cfg& cfg) {

  std::string assetFile = "";
  ivec2 size = ci::ivec2(0);

  cfg
  .set_ivec2("size", size)
  .setBool("setMatricesWindow", this->bSetMatricesWindow)
  .setBool("setViewport", this->bSetViewport)
  .setColor("clearColor", this->clearColor)
  .connectAttr<void()>("drawOn", [this](){ this->draw(); })
  .withSignalByAttr<void()>("drawEmit",
    [this](::ctree::Signal<void()>& sig){
      this->drawSignal = &sig;
    })
  .withSignalByAttr<void(const ci::Area&)>("boundsEmit",
    [this](::ctree::Signal<void(const ci::Area&)>& sig){
      this->boundsSignal = &sig;
    })
  .withSignalByAttr<void(ci::gl::TextureRef)>("textureEmit",
    [this](::ctree::Signal<void(ci::gl::TextureRef)>& sig){
      this->textureSignal = &sig;
    });

  if (cfg.reader()->has("clearColor")) this->bClear = true;

  // allocate
  if (size.x > 0 && size.y > 0 &&
    (this->fboRef == nullptr || this->fboRef->getSize() != size)) {
    this->init(size);
  }
}

void Fbo::init(const ivec2& size) {
  this->fboRef = gl::Fbo::create(size.x, size.y);

  if (this->textureSignal) {
    auto texRef = this->fboRef->getColorTexture();
    this->textureSignal->emit(texRef);
  }

  if (this->boundsSignal) {
    auto bounds = this->fboRef->getBounds();
    this->boundsSignal->emit(bounds);
  }
}


void Fbo::draw() {
  if(verbose) CI_LOG_I("Fbo::draw");

  if (!this->fboRef) {
    if(verbose) CI_LOG_I("Fbo::draw - no fbo");
    return;
  }

  { // draw to fbo
    gl::ScopedFramebuffer bufScp(this->fboRef);
    if(bClear) gl::clear(this->clearColor);

    if (this->bSetViewport) {
      //gl::ScopedViewport(this->fboRef->getSize());
      gl::pushViewport();
      gl::viewport(this->fboRef->getSize());
    }

    if (this->bSetMatricesWindow) {
      gl::pushMatrices();
      // gl::setMatricesWindow(this->fboRef->getSize());
      gl::setMatricesWindowPersp(this->fboRef->getSize());
    }

    if (this->drawSignal) {
      if (boundsSignal) {
        auto bounds = this->fboRef->getBounds();
        this->boundsSignal->emit(bounds);
      }
      this->drawSignal->emit();
    }
    
    if (this->bSetMatricesWindow) {
      gl::popMatrices();
//      gl::popViewMatrix();
//      gl::popProjectionMatrix();
    }

    if (this->bSetViewport) {
      gl::popViewport();
    }
  }

  if (this->textureSignal) {
    auto texRef = this->fboRef->getColorTexture();
    this->textureSignal->emit(texRef);
  }
}
