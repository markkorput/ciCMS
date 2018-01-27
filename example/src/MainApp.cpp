// cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/qtime/QuickTimeGl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VidFramesLoader {
public:
  VidFramesLoader(const fs::path& path) : path(path) {
    try {
      // load up the movie, set it to loop, and begin playing
      mMovie = qtime::MovieGl::create(path);

      auto connection = mMovie->getNewFrameSignal().connect([this](){
        auto tex = mMovie->getTexture();
        mMovie->stop();

        auto fbo = gl::Fbo::create(tex->getWidth(), tex->getHeight());
        this->fbos.push_back(fbo);
        gl::ScopedFramebuffer fbScp( fbo );
        gl::draw(tex);

        mMovie->play();
        mMovie->seekToFrame(this->fbos.size());

        // this->textures.push_back(newtex);
        this->bWaitingForFrame = false;
        CI_LOG_I("loaded tex #"<<this->fbos.size());
      });


      CI_LOG_I("movie loaded ("<<path<<"), number of frames: " <<mMovie->getNumFrames());


      // //mMovie->setLoop();
      mMovie->play();
    } catch( ci::Exception &exc ) {
      console() << "Exception caught trying to load the movie from path: " << path << ", what: " << exc.what() << std::endl;
      mMovie = nullptr;
    }
  }

  void update(){
    if(!mMovie || isComplete()) return;
    auto tex = mMovie->getTexture();
    // int frameIndex = getLoadedFramesCount();
    // mMovie->stop();
    // mMovie->seekToFrame(frameIndex);
    // mMovie->play();
    // CI_LOG_I("waiting for frame #" << frameIndex);
    // bWaitingForFrame = true;
  }

  unsigned int getLoadedFramesCount(){
    return fbos.size();
  }

  qtime::MovieGlRef getMovie(){ return this->mMovie; }

  bool isLoading(){
    return !isComplete();
  }

  bool isComplete(){
    return mMovie && getLoadedFramesCount() >= mMovie->getNumFrames();
  }

  gl::Texture2dRef getTexture(int idx){
    return idx < 0 || idx >= this->fbos.size() ? nullptr : fbos[idx]->getColorTexture();
  }

  gl::FboRef getFbo(int idx){
    return idx < 0 || idx >= this->fbos.size() ? nullptr : fbos[idx];
  }

private:
  fs::path path;
  // std::vector<gl::TextureRef> textures;
  std::vector<gl::FboRef> fbos;
  qtime::MovieGlRef mMovie = nullptr;

  bool bWaitingForFrame = false;
};


class MainApp : public App {
public:
  MainApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(KeyEvent event) override;
  void next();

private:
  ci::Timer timer;
  std::vector<fs::path> paths;
  std::shared_ptr<VidFramesLoader> loader = nullptr;
  std::vector<gl::TextureRef> textures;
  int vidCursor = 0;
};


MainApp::MainApp() {

}

void MainApp::setup(){
  setWindowSize( 1280, 480 );

  auto args = getCommandLineArgs();
  // CI_LOG_I("args size: " << args.size());
  // for(auto arg : args){
  //   CI_LOG_I("arg: " << arg);
  // }

  // fs::path moviePath = getAssetPath(args.size() > 1 ? args[1] : "_bar0.mov");
  paths.push_back(getAssetPath("_bar0.mov"));
  paths.push_back(getAssetPath("_bar1.mov"));
  paths.push_back(getAssetPath("_bar2.mov"));
  paths.push_back(getAssetPath("_bar3.mov"));
  paths.push_back(getAssetPath("_bar4.mov"));
  paths.push_back(getAssetPath("_bar5.mov"));

}

void MainApp::update(){
  // auto dt = this->timer.getSeconds();
  // this->timer.start();

  if(loader){
    loader->update();

    if(loader->isComplete()){
      for(int i=0; i<loader->getLoadedFramesCount(); i++)
        textures.push_back(loader->getTexture(i));

      loader = nullptr;
    }
  }

  if(!loader && vidCursor < paths.size()){
    loader = std::make_shared<VidFramesLoader>(paths[vidCursor]);
    vidCursor += 1;
  }
}

void MainApp::draw(){
  gl::clear(Color(0,0,0));

  if(loader && loader->isLoading()){
    auto tex = loader->getTexture(loader->getLoadedFramesCount()-1);

    if(tex){
      gl::draw(tex);
    } else {
      tex = loader->getMovie()->getTexture();
      if(tex){
        gl::draw(tex);
      }
    }
  } else {
    float factor = (float)getMousePos().x / (float)getWindowSize().x;
    int frame = (int)((float)textures.size()*factor);
    auto tex = textures[frame];
    gl::draw(tex, Rectf( 0, 0, getWindowSize().x, getWindowSize().y ));
  }
}

void MainApp::keyDown(KeyEvent event){
  // switch(event.getChar()){
  //   case 'a':
  // }
}

CINDER_APP( MainApp, RendererGl )
