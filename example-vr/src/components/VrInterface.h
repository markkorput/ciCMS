#pragma once

// EANBLE VR ON WINDOWS
#ifdef CINDER_MSW
  #define VR_ENABLED
#endif

#ifndef VR_ENABLED
  #define VR_PLACEHOLDER
#endif

#ifdef VR_ENABLED
  #include "cinder/vr/vr.h"
  // VR implementation based on;
  // https://gist.github.com/lithium-snepo/772eddede79b960666e521a8d1789d9c
  // and Cinder-VR block's Basic samples
#endif

#ifdef VR_PLACEHOLDER
  #include "cinder/CameraUi.h"
#endif

#include "cinder/app/App.h"
#include "ciCMS/cfg/Cfg.h"


namespace components {
  class VrInterface {

    const char DEFAULT_TOGGLE_UICAM_KEY = '/';
    const char DEFAULT_MIRRORMODE1_KEY = '1';
    const char DEFAULT_MIRRORMODE2_KEY = '2';
    const char DEFAULT_MIRRORMODE3_KEY = '3';
    const char DEFAULT_MIRRORMODE4_KEY = '4';
    const char DEFAULT_MIRRORMODE5_KEY = '5';
    const char DEFAULT_NO_MIRRORMODE_KEY = 'm';

  public:
    // VrInterface(){}
    void cfg(cms::cfg::Cfg& cfg);

  protected:
      void setup();
      void update();
      void draw();

      #ifdef VR_ENABLED
        void setMirroredMode(int mode);
      #endif

      void mouseDown(ci::app::MouseEvent& event);
      void mouseDrag(ci::app::MouseEvent& event);
      void keyDown(ci::app::KeyEvent& event);

      ci::Camera* getActiveCamera(){
        #ifdef VR_ENABLED
          if(this->mHmd)
            return &this->getEyeCamera(ci::vr::EYE_HMD);
        #endif

        #ifdef VR_PLACEHOLDER
          return &this->mCam;
        #endif

        return NULL;
      }

    protected: // signals

      ci::signals::Signal<void(void)>
        renderSceneSignal,
        renderVrSignal,
        renderNoVrSignal;

    private: // attributes

      bool bSetupDone = false;

      #ifdef VR_ENABLED
        vr::Context* mVrContext = nullptr;
        vr::Hmd* mHmd = nullptr;
        // const vr::Controller* pController = NULL;

        // Only applies to HTC Vive with standing tracking origin
        bool mRecalcOrigin = false;
        bool drawMirrored = true;
      #endif

      #ifdef VR_PLACEHOLDER
        // when VR is not available, we'll use CamerUI to
        // let the user rotate the camera using the mouse
        ci::CameraPersp mCam;
        ci::CameraUi mCamUi;
      #endif
  };
}
