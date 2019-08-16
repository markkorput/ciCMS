#include "TexSphere.h"

#include "cinder/Log.h"

using namespace ci;
using namespace ci::geom;
using namespace component;

class TexOffset : public ci::geom::Modifier {
  private:
    glm::vec2 mOffset;

  public:
    TexOffset(const glm::vec2& offset) : mOffset(offset){}

    Modifier* clone() const override { return new TexOffset(this->mOffset); }

    void process( SourceModsContext *ctx, const AttribSet &requestedAttribs ) const override {
      ctx->processUpstream( requestedAttribs );

      const size_t numVertices = ctx->getNumVertices();
      auto texCoordDims = ctx->getAttribDims( TEX_COORD_0 );

      if( texCoordDims == 2 ) {
        // CI_LOG_W("texCoordDims ARE 2");
        auto texCoords = reinterpret_cast<const float*>( ctx->getAttribData( Attrib::TEX_COORD_0 ) );

        vector<float> outTexCoord0;
        outTexCoord0.reserve( numVertices * texCoordDims );

        for( size_t v = 0; v < numVertices; ++v ) {
          outTexCoord0.push_back( texCoords[v * texCoordDims + 0]+mOffset.x );
          outTexCoord0.push_back( texCoords[v * texCoordDims + 1]+mOffset.y );
        }

        ctx->copyAttrib( Attrib::TEX_COORD_0, texCoordDims, 0, (const float*)outTexCoord0.data(), numVertices );
      }
    }
};

void TexSphere::cfg(cms::cfg::Cfg& cfg){
  // configurables
  std::string texAssetPath;
  glm::vec3 pos;
  glm::vec2 texOffset;
  float radius = 1.0f;

  cfg
  .connectAttr<void()>("drawOn", [this](){ this->draw(); })
  .set("tex", texAssetPath)
  .setVec3("pos", pos)
  .setFloat("radius", radius)
  .setVec2("texOffset", texOffset);

  this->setup(texAssetPath, pos, radius, texOffset);

}

void TexSphere::setup(const std::string& texAssetPath, const glm::vec3& pos, float radius, const glm::vec2& texOffset) {
  auto geom = ci::geom::Sphere()
    .subdivisions(64)
    .radius(radius)
    .center(pos) >> TexOffset(texOffset);

  // CI_LOG_W("radius" << radius << "pos: " << pos);

  this->batchRef = ci::gl::Batch::create(
    geom,
    //ci::gl::getStockShader(ci::gl::ShaderDef().color()));
    ci::gl::getStockShader(ci::gl::ShaderDef().texture()));

  ci::fs::path path = ci::app::getAssetPath(texAssetPath);

  if(!ci::fs::exists(path)){
    CI_LOG_W("Texture file does not exist: " << path.string());
  } else {
    auto imageSourceRef = ci::loadImage(path);

    if (!imageSourceRef) {
        CI_LOG_W("Couldn't load image" << path.string());
    } else {
      this->texRef = ci::gl::Texture::create(imageSourceRef);
    }
  }
}

void TexSphere::draw() {
  if (this->batchRef) {
      if(this->texRef){
        ci::gl::ScopedTextureBind tex0(this->texRef);
        this->batchRef->draw();
      } else {
        this->batchRef->draw();
      }

      // ci::gl::popModelMatrix();
  }
}
