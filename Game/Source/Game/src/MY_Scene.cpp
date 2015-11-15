#pragma once

#include <MY_Scene.h>
#include <MY_ResourceManager.h>

#include <Game.h>
#include <MeshEntity.h>
#include <MeshInterface.h>
#include <MeshFactory.h>
#include <Resource.h>

#include <DirectionalLight.h>
#include <PointLight.h>
#include <SpotLight.h>

#include <shader\ComponentShaderBase.h>
#include <shader\ComponentShaderText.h>
#include <shader\ShaderComponentText.h>
#include <shader\ShaderComponentTexture.h>
#include <shader\ShaderComponentDiffuse.h>
#include <shader\ShaderComponentHsv.h>
#include <shader\ShaderComponentMVP.h>

#include <shader\ShaderComponentIndexedTexture.h>
#include <TextureColourTable.h>

#include <Box2DWorld.h>
#include <Box2DMeshEntity.h>
#include <Box2DDebugDrawer.h>

#include <MousePerspectiveCamera.h>
#include <FollowCamera.h>

#include <System.h>
#include <Mouse.h>
#include <Keyboard.h>
#include <GLFW\glfw3.h>

#include <RenderSurface.h>
#include <StandardFrameBuffer.h>
#include <NumberUtils.h>

#include <NodeBulletBody.h>
#include <BulletMeshEntity.h>
#include <TextArea.h>
#include <Box2DWorld.h>
#include <Box2DDebugDrawer.h>

#include <RenderOptions.h>
#include <CharacterUtils.h>

MY_Scene::MY_Scene(Game * _game) :
	Scene(_game),
	screenSurfaceShader(new Shader("assets/engine basics/DefaultRenderSurface", false, true)),
	screenSurface(new RenderSurface(screenSurfaceShader)),
	screenFBO(new StandardFrameBuffer(true)),
	textShader(new ComponentShaderText(true)),
	uiLayer(0,0,0,0),
	seed(0),
	bpm(120)
{
	textShader->textComponent->setColor(glm::vec3(56.f/255.f, 56.f/255.f, 56.f/255.f));
	drums = new AutoDrums(
		MY_ResourceManager::scenario->getAudio("SNARE")->sound,
		MY_ResourceManager::scenario->getAudio("KICK")->sound,
		MY_ResourceManager::scenario->getAudio("RIDE")->sound
	);

	srand(250);

	childTransform->addChild(drums, false);

	
	loopSlider = new SliderControlled(uiLayer.world, &drums->curTime, 0, drums->lengthInSeconds);
	loopSlider->setHeight(12);
	loopSlider->setWidth(290);
	loopSlider->setMarginRight(5);
	

	bpmSlider = new SliderController(uiLayer.world, &bpm, bpm, 30, 240);
	bpmSlider->setWidth(290);
	bpmSlider->setMarginRight(5);
	bpmSlider->setHeight(12);


	uiLayer.addMouseIndicator();
	Font * font = MY_ResourceManager::scenario->getFont("COURIER")->font;
	VerticalLinearLayout * vl = new VerticalLinearLayout(uiLayer.world);
	vl->verticalAlignment = kMIDDLE;
	vl->horizontalAlignment = kLEFT;
	vl->setRationalHeight(1.f);
	vl->setRationalWidth(1.f);
	{
		TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
		l->setText(L"Controls: Type numbers to manually enter a seed");
		vl->addChild(l);
	}
	{
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer.world);
		TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
		l->setText(L"Time: ");
		l->setWidth(30);
		loopSlider->setBackgroundColour(244.f/255.f, 180.f/255.f, 213.f/255.f, 1);
		loopSlider->fill->setBackgroundColour(244.f/255.f, 180.f/255.f, 213.f/255.f, 1);
		loopSlider->thumb->setBackgroundColour(255.f/255.f, 200.f/255.f, 233.f/255.f, 1);

		hl->addChild(l);
		hl->addChild(loopSlider);
		vl->addChild(hl);
	}
	{
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer.world);
		TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
		l->setText(L"BPM: ");
		l->setWidth(30);

		hl->addChild(l);
		hl->addChild(bpmSlider);
		vl->addChild(hl);
	}
	{
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer.world);
		hl->setMarginTop(12);
		NodeUI * b = new NodeUI(uiLayer.world);
		b->setMouseEnabled(true);
		b->setWidth(-1);
		b->setHeight(-1);
		b->setMarginRight(6);
		b->setPadding(6);
		b->onClickFunction = [this](){
			std::wstringstream ss;
			ss << rand();
			seedLabel->setText(ss.str());
			generate();
		};
		{
			TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
			l->setText(L"Random Seed");
			HorizontalLinearLayout * bl = new HorizontalLinearLayout(uiLayer.world);
			bl->horizontalAlignment = kCENTER;
			bl->verticalAlignment = kMIDDLE;
			bl->setWidth(-1.f);
			bl->setHeight(-1.f);
			b->addChild(bl);
			bl->addChild(l);
		}

		NodeUI * b2 = new NodeUI(uiLayer.world);
		b2->setMouseEnabled(true);
		b2->setWidth(-1);
		b2->setHeight(-1);
		b2->setMarginRight(6);
		b2->setPadding(6);
		b2->onClickFunction = [this](){
			generate();
		};
		{
			TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
			l->setText(L"Load Seed");
			HorizontalLinearLayout * bl = new HorizontalLinearLayout(uiLayer.world);
			bl->horizontalAlignment = kCENTER;
			bl->verticalAlignment = kMIDDLE;
			bl->setWidth(-1.f);
			bl->setHeight(-1.f);
			b2->addChild(bl);
			bl->addChild(l);
		}
		

		TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
		l->setText(L"Seed: ");
		l->setPadding(0, 6);
		
		seedLabel = new TextLabel(uiLayer.world, font, textShader);
		seedLabel->setText(L"0");
		seedLabel->setPadding(0, 6);
		
		hl->addChild(b);
		hl->addChild(b2);
		hl->addChild(l);
		hl->addChild(seedLabel);
		vl->addChild(hl);
	}
	
	{
		HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer.world);
		hl->horizontalAlignment = kCENTER;
		hl->verticalAlignment = kMIDDLE;
		hl->setWidth(-1.f);
		hl->setHeight(-1.f);
	}

	HorizontalLinearLayout * hl = new HorizontalLinearLayout(uiLayer.world);
	hl->setRationalHeight(1.f);
	hl->setRationalWidth(1.f);
	hl->verticalAlignment = kMIDDLE;
	{
		VerticalLinearLayout * v = new VerticalLinearLayout(uiLayer.world);
		v->setWidth(100);
		v->setHeight(100);
		v->horizontalAlignment = kCENTER;
		TextLabel * l = new TextLabel(uiLayer.world, font, textShader);
		l->setText(L"SweetBeats");
		l->setWidth(100);
		l->horizontalAlignment = kCENTER;
		NodeUI * logo = new NodeUI(uiLayer.world);
		logo->setWidth(90);
		logo->setHeight(90);
		logo->background->mesh->pushTexture2D(MY_ResourceManager::scenario->getTexture("LOGO")->texture);
		v->addChild(l);
		v->addChild(logo);
		hl->addChild(v);
	}
	hl->addChild(vl);

	uiLayer.addChild(hl);

	generate();
}

MY_Scene::~MY_Scene(){
	deleteChildTransform();
	textShader->safeDelete();

	screenSurface->safeDelete();
	//screenSurfaceShader->safeDelete();
	screenFBO->safeDelete();
}


void MY_Scene::update(Step * _step){
	if(keyboard->keyJustDown(GLFW_KEY_SPACE)){
		generate();
	}

	if(keyboard->keyJustDown(GLFW_KEY_ESCAPE)){
		game->exit();
	}


	// handle inputs
	if(keyboard->keyJustDown(GLFW_KEY_BACKSPACE)){
		if(seedLabel->textDisplayed.size() > 0){
			seedLabel->setText(seedLabel->textDisplayed.substr(0, seedLabel->textDisplayed.size() - 1));
		}
		if(seedLabel->textDisplayed.size() <= 0){
			seedLabel->setText(L"0");
		}
	}
	/*if(keyboard->keyJustUp(GLFW_KEY_ENTER)) {
		std::wstring s;
		s += '\n';
		//label->appendText(s);
	}*/
	if(keyboard->justReleasedKeys.size() > 0){
		std::wstringstream acc;
		for(auto k : keyboard->justReleasedKeys){
			if(k.second < 256 && CharacterUtils::isDigit(k.second)){
				if(!keyboard->shift){
					acc << (wchar_t)tolower(k.second);
				}else {
					acc << (wchar_t)k.second;
				}
			}
		}
		if(acc.tellp() > 0 && seedLabel->textDisplayed.size() < 10){
			seedLabel->setText(seedLabel->textDisplayed + acc.str());
		}
	}



	//std::cout << drums->curTime << std::endl;

	glm::uvec2 sd = sweet::getScreenDimensions();
	uiLayer.resize(0, sd.x, 0, sd.y);
	Scene::update(_step);
	uiLayer.update(_step);
}

void MY_Scene::generate(){
	sweet::NumberUtils::seed(stoul(seedLabel->textDisplayed));
	drums->bpm = bpm;
	drums->generate();
	
	//melody->bpm = drums->bpm;
	//melody->timeSignatureBottom = drums->timeSignatureBottom;
	//melody->timeSignatureTop = drums->timeSignatureTop;
	//melody->generate();

	//std::cout << "Melody:\t" << melody->toString();
	std::cout << drums->lengthInBeats << std::endl;
	std::cout << drums->lengthInSeconds << std::endl;
	loopSlider->valueMax = drums->lengthInSeconds;
}

void MY_Scene::render(sweet::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	screenFBO->resize(game->viewPortWidth, game->viewPortHeight);
	//Bind frameBuffer
	screenFBO->bindFrameBuffer();
	//render the scene to the buffer
	
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	_renderOptions->clearColour[0] = 254.f/255.f;
	_renderOptions->clearColour[1] = 190.f/255.f;
	_renderOptions->clearColour[2] = 223.f/255.f;
	_renderOptions->clear();
	Scene::render(_matrixStack, _renderOptions);

	//Render the buffer to the render surface
	screenSurface->render(screenFBO->getTextureId());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	uiLayer.render(_matrixStack, _renderOptions);
}

void MY_Scene::load(){
	Scene::load();	

	screenSurface->load();
	screenFBO->load();
	uiLayer.load();
}

void MY_Scene::unload(){
	uiLayer.unload();
	screenFBO->unload();
	screenSurface->unload();

	Scene::unload();	
}