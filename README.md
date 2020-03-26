# ciCMS

_A sort of 'content management system' for Cinder applications_

## cms

Contains various classes to manage data and facilitate reactive programming
 * cms::Model -- Container for key-values pairs that lets you register callbacks when its content changes
 * cms::ModelCollection -- Container for list (collections) of models that lets you register callbacks for mutations (additions/removals) from the collection

## cms::cfg

The cms::cfg namespace contains various classes that aim to make "configurable" application logic.

Originally this method was developed specifically for UI-specific purposes with the goal to separate configuration-logic from application-logic. UI involves a lot of data; every scene item/component has to be configured, positioned, styled, etc. and all this configuration obfuscates the logical structure of the application.

What [started](https://github.com/fusefactory/JavaLibUiBuilder) as an effort to "configure" classes based on json data (inspired by how CSS stylesheets style an HTML document) evolved into a system that lets you instantiate entire (pieces of) applications from a configuration file. This approach is very much inspired by VPLs like [MaxMSP](https://cycling74.com/products/max) and [Touch Designer](https://derivative.ca/) that have highly modular and independent building blocks that can be wired together to perform complex tasks.

### Example

What do you need?
 * A configuration file
 * Some logic that defines how classes and their properties can be confiugured

Below is an example configuration file (from the [texsphere](example-texsphere/assets/config.json) example in this repository) that defines 4 components;
 * A Runner that emits a ```draw``` events and connects its drawState to the global ```drawEnabled```
 * A Camera that listens for the ```draw``` event, then sets its matrices and emits the ```cam1``` event
 * A TexSphere that draws itself when the ```cam1``` event is emitted
 * A Keyboard component for user input that toggle the ```drawEnabled``` state when the ```d``` key is pressed

 ```json
{
  "Cfgr": {"active": true},
  "Runner": {"drawEmit": "draw", "drawState":"drawEnabled"},

  "Runner.Camera": {"on": "draw", "emit":"cam1", "pos": "0,0,0", "lookAt": "0,0,0.5", "ui":true},
  "Runner.TexSphere": {"radius": 10, "pos": "0,0,0", "tex":"equi.jpg", "drawOn":"cam1", "texOffset":"0.8,0"},

  "Runner.Keyboard": {"key:d": "toggle:drawEnabled"}
}
```

Below is the [example's setup function](example-texsphere/src/MainApp.cpp) that shows how different components are
registered and then the entire application structure is loaded dynamically at runtime from the config file.

```c++
void MainApp::setup(){
  // Register supported components
  builder.addCfgObjectInstantiator<component::Runner>("Runner");
  builder.addCfgObjectInstantiator<component::Keyboard>("Keyboard");
  builder.addCfgObjectInstantiator<component::TexSphere>("TexSphere");
  builder.addCfgObjectInstantiator<component::Camera>("Camera");
  
  // Load configuration file
  builder.getModelCollection().loadJsonFromFile(ci::app::getAssetPath("config.json"));

  // build (part of) our application hierarchy by instantiating the 'Runner' component
  // and all of its child componnents
  pRunner = builder.build<component::Runner>("Runner");
}
```

Finally here is the configuration logic of [TexSphere](example-texsphere/src/component/TexSphere.cpp) (one of the component classes);

```c++
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
```
