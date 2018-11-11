#include "cinder/Log.h"
#include "Video.h"

using namespace std;
using namespace cinder;
using namespace cms::cfg::components;

void Video::cfg(cms::cfg::Cfg& cfg) {

  std::string assetFile = "";

  cfg
  .setBool("verbose", this->verbose)
  .setBool("autoStart", this->bAutoStart)
  .setBool("loop", this->bLoop)
  .set("assetFile", assetFile)

  .connectAttr<void(const fs::path&)>("loadFileOn", [this](const fs::path& filePath){
    if (verbose) CI_LOG_I("Video.loadFileOn");
    this->loadMovie(filePath);
  })

  .connectAttr<void()>("playOn", [this](){
    if (this->mMovie) { this->mMovie->play(); }
  })

  .connectAttr<void()>("updateOn", [this](){
    this->update();
  })

  .withSignalByAttr<void(ci::gl::TextureRef)>("frameTextureEmit",
    [this](::ctree::Signal<void(ci::gl::TextureRef)>& sig){
      frameTexSignal = &sig;
    });

  if (assetFile != "") {
    if (verbose) CI_LOG_I("loading asset file");
    this->loadMovie(app::getAssetPath(assetFile));
  }
}

void Video::update() {
  if (verbose) CI_LOG_I("Video.update");

  auto mov = this->mMovie;
  if (mov) {
    if (mov->checkNewFrame()) {
      this->onFrame();
    }
  }
}

bool Video::loadMovie(const ci::fs::path& moviePath) {
  if (verbose) CI_LOG_I("Video.loadMovie: " << moviePath.string());

  try {
    // load up the movie, set it to loop, and begin playing
    mMovie = qtime::MovieGl::create( moviePath );
  }
  catch( ci::Exception &exc ) {
    CI_LOG_E("Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what());
    mMovie.reset();
  }

  if (!mMovie) return false;
  
  mMovie->getReadySignal().connect([this](){
    if (verbose) CI_LOG_I("Video ready");
  });

  // THIS DOESN'"'T SEEM TO BE WORKING?! (make sure update is called every cycle?)
  mMovie->getNewFrameSignal().connect([this](){
    this->onFrame();
  });

  mMovie->setLoop(this->bLoop);

  if (this->bAutoStart) {
    if (verbose) CI_LOG_I("Video.autoStart");
    this->mMovie->play();
  }

  return true;
}

void Video::onFrame() {
  if (verbose) CI_LOG_I("Video.onFrame");
  auto texRef = mMovie->getTexture();
  if(this->frameTexSignal) this->frameTexSignal->emit(texRef);
}
