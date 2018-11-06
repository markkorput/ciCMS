#include "cinder/Log.h"
#include "cinder/gl/gl.h"

#include "VrInterface.h"

using namespace ci;
using namespace ci::app;
using namespace components;


void VrInterface::cfg(cms::cfg::Cfg& cfg) {
  cfg
  .connectAttr<void()>("setupOn", [this](){ this->setup();})
  .connectAttr<void()>("updateOn", [this](){ this->update();})
  .connectAttr<void()>("drawOn", [this](){ this->draw();})

  .withSignalByAttr<void()>("drawSceneEmit", [this](::ctree::Signal<void()>& sig){
    this->renderSceneSignal.connect([&sig](){ sig.emit(); });
  })
  .withSignalByAttr<void()>("drawNoVR", [this](::ctree::Signal<void()>& sig){
    this->renderNoVrSignal.connect([&sig](){ sig.emit(); });
  })
  .withSignalByAttr<void()>("drawVR", [this](::ctree::Signal<void()>& sig){
    this->renderVrSignal.connect([&sig](){ sig.emit(); });
  });

  if (cfg.reader()->getBool("autoSetup", true)) {
    this->setup();
  }

  if (cfg.reader()->getBool("windowMouseEvents", true)) {
    app::App::get()->getWindow()->getSignalMouseDown().connect([this](app::MouseEvent &event){
      this->mouseDown(event);
    });

    app::App::get()->getWindow()->getSignalMouseDrag().connect([this](app::MouseEvent &event){
      this->mouseDrag(event);
    });
  }

  if (cfg.reader()->getBool("windowKeyEvents", false)) {
    app::App::get()->getWindow()->getSignalKeyDown().connect([this](app::KeyEvent &event){
      this->keyDown(event);
    });
  }
}


void VrInterface::setup(){
  if (bSetupDone) return;

  #ifdef VR_ENABLED
    CI_LOG_I("Setting up VR...");

    try {
      ci::vr::initialize();
    } catch (const std::exception& e) {
      CI_LOG_E("VR failed: " << e.what());
    }

    try {
      mVrContext = ci::vr::beginSession(
        ci::vr::SessionOptions()
        .setOriginOffset(vec3(0, 0, 0)) // vec3(0, 0, -3)
        // .setControllerConnected([this](const ci::vr::Controller* controller) {
        //   CI_LOG_I("VR controller connected");
        //   this->pController = controller;
        // })
        // .setControllerDisconnected([this](const ci::vr::Controller* controller) {
        //   CI_LOG_I("VR controller DISconnected");
        //   this->pController = NULL;
        // })
      );
    }
    catch (const std::exception& e) {
      CI_LOG_E("VR Session failed: " << e.what());
    }

    if (mVrContext) {
      try {
        mHmd = mVrContext->getHmd();
        //mHmd->setMirrorMode(ci::vr::Hmd::MirrorMode::MIRROR_MODE_STEREO);
        mHmd->setMirrorMode(ci::vr::Hmd::MirrorMode::MIRROR_MODE_UNDISTORTED_STEREO);
      }
      catch (const std::exception& e) {
        CI_LOG_E("VR Hmd failed: " << e.what());
      }
    }

    setMirroredMode(0);

  #endif

  #ifdef VR_PLACEHOLDER
    mCam.setWorldUp(vec3(0, 1, 0));
    mCam.lookAt(vec3(0), vec3(0, 0, 1));
    mCam.setPerspective( 45.0f, getWindowAspectRatio(), 0.1, 10000 );

    CI_LOG_I("Setting up VR-replacement camera...");
    mCamUi = CameraUi( &mCam, app::App::get()->getWindow() );
  #endif

  this->bSetupDone = true;
}

void VrInterface::update(){
  #ifdef VR_ENABLED
    // Vive sometimes returns the wrong pose data initially so reinitialize the origin matrix after the first 60 frames.
    if (mHmd && (ci::vr::API_OPENVR == mVrContext->getApi()) && (!mRecalcOrigin) && (mHmd->getElapsedFrames() > 60)) {
      mHmd->calculateOriginMatrix();
      mRecalcOrigin = true;
    }
  #endif
}

void VrInterface::draw(){
  #ifdef VR_ENABLED
    // render for VR glasses?
    if(mHmd){

      // render for individual eyes
      mHmd->bind();
      {
        for (auto eye : mHmd->getEyes()) {
          mHmd->enableEye(eye);
          this->renderSceneSignal.emit();
          this->renderVrSignal.emit();
          mHmd->drawControllers(eye);
        }
      }
      mHmd->unbind();

      // also draw to monitor?
      if (drawMirrored) {
        mHmd->submitFrame();

        gl::viewport( getWindowWidth() / 2, 0.0f, getWindowWidth() / 2, getWindowHeight() );
        //        gl::viewport(getWindowSize());
        mHmd->enableEye(ci::vr::EYE_HMD);
        this->renderSceneSignal.emit();
        this->renderVrSignal.emit();
      } else {
        gl::viewport( getWindowWidth() / 2, 0.0f, getWindowWidth() / 2, getWindowHeight() );
//        gl::viewport(getWindowSize());
        gl::setMatricesWindow(getWindowSize());
        mHmd->drawMirrored(getWindowBounds(), true);
      }

      // done
      return;
    }
  #endif

  // draw "normal" (no VR)
  gl::viewport( getWindowWidth() / 2, 0.0f, getWindowWidth() / 2, getWindowHeight() );
  //gl::viewport(getWindowSize());
  gl::setMatrices(mCam);
  this->renderSceneSignal.emit();
  this->renderNoVrSignal.emit();
}

void VrInterface::mouseDown( MouseEvent& event ){
  #ifdef VR_PLACEHOLDER
    if (event.isMetaDown())
      mCamUi.mouseDown(event);
  #endif
}

void VrInterface::mouseDrag(MouseEvent& event){
  #ifdef VR_PLACEHOLDER
    if (event.isMetaDown())
      mCamUi.mouseDrag(event);
  #endif
}

void VrInterface::keyDown(KeyEvent& event){
  #ifdef VR_ENABLED
    if(event.isMetaDown()){
      switch (event.getChar()) {
        case DEFAULT_MIRRORMODE1_KEY: { setMirroredMode(0); return; }
        case DEFAULT_MIRRORMODE2_KEY: { setMirroredMode(1); return; }
        case DEFAULT_MIRRORMODE3_KEY: { setMirroredMode(2); return; }
        case DEFAULT_MIRRORMODE4_KEY: { setMirroredMode(3); return; }
        case DEFAULT_MIRRORMODE5_KEY: { setMirroredMode(4); return; }
        case DEFAULT_NO_MIRRORMODE_KEY: { drawMirrored = !drawMirrored; return; }
      }
    }
  #endif

  #ifdef VR_PLACEHOLDER
    if(event.getChar() == DEFAULT_TOGGLE_UICAM_KEY) {
      this->mCamUi.enable(!this->mCamUi.isEnabled());
    }
  #endif
}
