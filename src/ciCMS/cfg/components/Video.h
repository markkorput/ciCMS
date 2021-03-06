#pragma once

#include "cinder/gl/gl.h"
//#if defined( CINDER_ANDROID )
//	#include "cinder/android/video/MovieGl.h"
//#else
	#include "cinder/qtime/QuickTimeGl.h"
//#endif

#include "ctree/signal.hpp"
#include "ciCMS/cfg/Cfg.h"

namespace cms { namespace cfg { namespace components {

  class Video {
    public:
      // ~Video();
      void cfg(cms::cfg::Cfg& cfg);

    private:
      bool loadMovie(const ci::fs::path& path);
			void onFrame();
			void update();

		private: // signals
			::ctree::Signal<void(ci::gl::TextureRef)>* frameTexSignal = NULL;

    private: //attrs
			bool verbose = false;
      bool bAutoStart = false;
      bool bLoop = false;

      // std::vector<ci::signals::Connection> connections;
      ci::qtime::MovieGlRef		mMovie = nullptr;
  };

}}}
