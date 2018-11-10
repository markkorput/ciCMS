#include "cinder/Log.h"
#include "Video.h"

using namespace std;
using namespace cinder;
using namespace cms::cfg::components;

void Video::cfg(cms::cfg::Cfg& cfg) {
  cfg
  .setBool("verbose", this->verbose)
  .setBool("autoStart", this->bAutoStart)

  .connectAttr<void(const fs::path&)>("loadFileOn", [this](const fs::path& filePath){
    if (verbose) CI_LOG_I("Video.loadFileOn");

    // load movie
    if (this->loadMovie(filePath)) {
      if (this->bAutoStart) {
        if (verbose) CI_LOG_I("Video.autoStart");
        this->mMovie->play();
      }
    }
  })

  .connectAttr<void()>("playOn", [this](){
    if (this->mMovie) { this->mMovie->play(); }
  })

  .connectAttr<void()>("updateOn", [this](){
    this->update();
  })

  .withSignalByAttr<void(ci::gl::TextureRef)>("frameTextureEmit",
    [this](::ctree::Signal<void(ci::gl::TextureRef)>& sig){

      frameTexSignal.connect([&sig](ci::gl::TextureRef texref){
        sig.emit(texref);
      });
    });
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

  bool success = true;

  try {
    // load up the movie, set it to loop, and begin playing
    mMovie = qtime::MovieGl::create( moviePath );

    mMovie->getReadySignal().connect([this](){
      if (verbose) CI_LOG_I("Video ready");
    });

    // THIS DOESN"T SEEM TO BE WORKING?!
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
    if (verbose) app::console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
    mMovie.reset();
    success = false;
  }

  mFrameTexture.reset();
  return success;
}

void Video::onFrame() {
  if (verbose) CI_LOG_I("Video.onFrame");
  auto texRef = mMovie->getTexture();
  this->frameTexSignal.emit(texRef);
}
