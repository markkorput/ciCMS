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
  if (!this->fboRef|| !this->drawSignal) return;
  if(verbose) CI_LOG_I("Fbo::draw drawing");
  {
    gl::ScopedFramebuffer bufScp(this->fboRef);
    this->drawSignal->emit();
  }

  if (this->textureSignal) {
    auto texRef = this->fboRef->getColorTexture();
    this->textureSignal->emit(texRef);
  }
}
