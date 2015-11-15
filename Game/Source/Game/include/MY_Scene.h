#pragma once

#include <Scene.h>
#include <UILayer.h>
#include <Slider.h>
#include <AutoMusic.h>

class PerspectiveCamera;
class MousePerspectiveCamera;
class MeshEntity;

class ShaderComponentHsv;

class Shader;
class RenderSurface;
class StandardFrameBuffer;
class Material;
class Sprite;

class PointLight;

class ComponentShaderText;
class TextLabel;

class MY_Scene : public Scene{
public:
	Shader * screenSurfaceShader;
	RenderSurface * screenSurface;
	StandardFrameBuffer * screenFBO;
	
	ComponentShaderText * textShader;
	
	Sprite * mouseIndicator;

	virtual void update(Step * _step) override;
	virtual void render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions) override;
	
	virtual void load() override;
	virtual void unload() override;

	UILayer uiLayer;

	AutoDrums * drums;
	SliderControlled * loopSlider;
	SliderController * bpmSlider;
	float bpm;
	unsigned long int seed;
	TextLabel * seedLabel;
	void generate();

	MY_Scene(Game * _game);
	~MY_Scene();
};