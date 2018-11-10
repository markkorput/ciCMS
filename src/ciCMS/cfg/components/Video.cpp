#include "cinder/Log.h"
#include "Video.h"

using namespace std;
using namespace cinder;
using namespace cms::cfg::components;

void Video::cfg(cms::cfg::Cfg& cfg) {

  bool justLoaded = false;

  cfg

  .connectAttr<void(const fs::path&)>("loadFileOn", [this, &justLoaded](const fs::path& filePath){
    if (this->loadMovie(filePath)) {
      justLoaded = true;
    }
  })

  .connectAttr<void()>("playOn", [this](){
    if (this->mMovie) { this->mMovie->play(); }
  })

  .withSignalByAttr<void(ci::gl::TextureRef)>("frameTextureEmit",
    [this](::ctree::Signal<void(ci::gl::TextureRef)>& sig){

      frameTexSignal.connect([&sig](ci::gl::TextureRef texref){
        sig.emit(texref);
      });
    });

  if (justLoaded && cfg.getBool("autoStart", false) && this->mMovie) {
    this->mMovie->play();
  }
}


bool Video::loadMovie(const ci::fs::path& moviePath) {
  bool success = true;
  try {
    // load up the movie, set it to loop, and begin playing
    mMovie = qtime::MovieGl::create( moviePath );

    mMovie->getNewFrameSignal().connect([this](){
      this->onFrame();
    });

    //mMovie->setLoop();
    // mMovie->play();

    // {
    //   // load audio
    //   mVoice = loadFile( loadAsset("bbb.wav") );
    //
    //   if(mVoice){
    //     auto ctx = audio::Context::master();
    //     auto fmt = new audio::MonitorNode(audio::MonitorSpectralNode::Format().fftSize( 2048 ).windowSize( 1024 ));
    //     mMonitorNode = ctx->makeNode(fmt);
    //     mVoice->getOutputNode()->disconnectAllOutputs(); // don't want to hear it, just for analyzing
    //     mVoice->getOutputNode() >> mMonitorNode;
    //     mVoice->start();
    //     ctx->enable();
    //
    //     mMovie->setVolume(0.0f);
    //
    //     mMovie->getNumFrames();
    //     volumeFrames = new float[mMovie->getNumFrames()];
    //     frameSampleCounter = 0;
    //     frameVolumesSum = 0.0f;
    //     volumeFrameCounter = 0;
    //     CI_LOG_I("Audio loaded, initialized volumeFrames array with length: " << mMovie->getNumFrames());
    //
    //     // register audio volume sample counter that executes every update
    //     getSignalUpdate().connect([this](){
    //       if(mMonitorNode) {
    //         frameVolumesSum += mMonitorNode->getVolume();
    //         frameSampleCounter++;
    //       }
    //     });
    //
    //     // register movie-frame callback that calculates and saves
    //     // the average audio volume for the last frame
    //     mMovie->getNewFrameSignal().connect([this](){
    //       float avg = this->frameSampleCounter == 0.0f ? 0.0f : this->frameVolumesSum / this->frameSampleCounter;
    //       this->volumeFrames[volumeFrameCounter] = avg;
    //
    //       // CI_LOG_I("Frame volume #" << this->volumeFrameCounter << " based on " << this->frameSampleCounter << " samples: " << avg);
    //       this->getWindow()->setTitle( "Frame volume #" + std::to_string(this->volumeFrameCounter)+"/"+std::to_string(this->mMovie->getNumFrames()) );
    //       this->volumeFrameCounter++;
    //       this->frameSampleCounter = 0;
    //       this->frameVolumesSum = 0.0f;
    //     });
    //
    //     mMovie->getEndedSignal().connect([this](){
    //       CI_LOG_I("Video done, storing data to : output.bin");
    //       // saveFrameVolumes("output.bin", this->volumeFrames, this->volumeFrameCounter);
    //       writeFileStream("output.bin", true)->writeData(this->volumeFrames, this->volumeFrameCounter * sizeof(float));
    //     });
    //   }
    // }

    // console() << "Playing: " << mMovie->isPlaying() << std::endl;
  }
  catch( ci::Exception &exc ) {
    app::console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
    mMovie.reset();
    success = false;
  }

  mFrameTexture.reset();
  return success;
}

void Video::onFrame() {
  auto texRef = mMovie->getTexture();
  this->frameTexSignal.emit(texRef);
}
