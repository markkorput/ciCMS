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

		private: // signals
			::ctree::Signal<void(ci::gl::TextureRef)> frameTexSignal;

    private: //attrs
      // std::vector<ci::signals::Connection> connections;
      ci::gl::TextureRef			mFrameTexture = nullptr;
      ci::qtime::MovieGlRef		mMovie = nullptr;
  };

}}}
