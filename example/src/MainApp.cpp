// cinder
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/Rand.h"
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

class GridView {

};


class MainApp : public App {
public:
  MainApp();
  void setup() override;
  void update() override;
  void draw() override;
  void keyDown(KeyEvent event) override;

private:

  void drawGrid(vec2 size, vec2 step, float fps, uint32_t seed=1462);

  ci::Timer timer;
  std::vector<fs::path> paths;
  std::shared_ptr<VidFramesLoader> loader = nullptr;
  std::vector<gl::TextureRef> textures;
  std::vector<std::shared_ptr<std::vector<gl::TextureRef>>> sequences;
  int vidCursor = 0;
  vec2 step = vec2(64,17);
  vec2 size = vec2(64,17);
};


MainApp::MainApp() {

}

void MainApp::setup(){
  setWindowSize( 1280, 480 );
  timer.start();

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
  if(loader){
    loader->update();

    if(loader->isComplete()){

      auto sequence = std::make_shared<std::vector<gl::TextureRef>>();
      sequences.push_back(sequence);
      for(int i=0; i<loader->getLoadedFramesCount(); i++){
        auto tex = loader->getTexture(i);
        textures.push_back(tex);
        sequence->push_back(tex);
      }

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
    // float factor = (float)getMousePos().x / (float)getWindowSize().x;
    // int frame = (int)((float)textures.size()*factor);
    // auto tex = textures[frame];
    // gl::draw(tex, Rectf( 0, 0, getWindowSize().x, getWindowSize().y ));
    drawGrid(size, step, 25);
  }
}

void MainApp::keyDown(KeyEvent event){
  switch(event.getChar()){
    case 'l': {
      size = vec2(size.x*1.1f, size.y*1.1f);
      step = vec2(size.x, size.y*0.5f);
      return;
    }
    case 's': {
      size = vec2(size.x*0.9f, size.y*0.9f);
      step = vec2(size.x, size.y*0.5f);
      return;
    }
  }
}

void MainApp::drawGrid(vec2 size, vec2 step, float fps, uint32_t seed){
  vec2 cursor = vec2(0.0f, 0.0f);
  const auto winsize = getWindowSize();

  Rand rander;
  int counter = 0;

  // fill window veertically
  while(cursor.y < winsize.y){
    // fill window horizontally
    while(cursor.x < winsize.x){
      rander.seed(seed + counter);
      int startTex = (int)(rander.nextFloat() * (float)this->textures.size());
      // CI_LOG_I("draw at: "<<cursor<<" startIdx: " << startTex);

      int texIdx = (int)std::fmod(startTex + this->timer.getSeconds() * fps, this->textures.size());
      gl::draw(this->textures[texIdx], Rectf(cursor, cursor + size));
      cursor.x += step.x;
      counter += 1;
    }

    cursor.x = 0.0f;
    cursor.y += step.y;
  }
}

CINDER_APP( MainApp, RendererGl )
