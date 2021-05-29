#include "Turan_CBinding.h"

#include <SFML/Graphics.hpp>

void StartFrame_Graphics();
void EndFrame_Graphics();
typedef void(*Gameloopfunc)(unsigned char isSinglePlayer);
Gameloopfunc GAMELOOP = nullptr;
sf::RenderWindow* Window = nullptr;


void Start_GameEngine(void(*Setup_funcptr)(), void(*GameLoop)(unsigned char isTwoPlayer)) {
	Window = new sf::RenderWindow(sf::VideoMode(SCREENWIDTH, SCREENHEIGHT), "Game");
	Window->setFramerateLimit(30);
	Setup_funcptr();
	GAMELOOP = GameLoop;

	
	//Menü resimlerini yükle
	void* backGrImage = Turan_LoadImage("Textures/backGround.png");
	Sprite* backGr = CreateSprite(backGrImage);
	backGr->WIDTH = 500;
	backGr->HEIGHT = 600;

	void* StartImage = Turan_LoadImage("Textures/start_2.png");
	Sprite* StartButton = CreateSprite(StartImage);
	StartButton->WIDTH = 240;
	StartButton->HEIGHT = 80;
	StartButton->PosX = 130;
	StartButton->PosY = 150;

	void* StopImage = Turan_LoadImage("Textures/pause.png");
	Sprite* StopButton = CreateSprite(StopImage);
	StopButton->WIDTH = 60;
	StopButton->HEIGHT = 20;

	void* BackImage = Turan_LoadImage("Textures/back.png");
	Sprite* BackButton = CreateSprite(BackImage);
	BackButton->WIDTH = 240;
	BackButton->HEIGHT = 80;
	BackButton->PosX = 130;
	BackButton->PosY = 150;

	void* SinglePlayerImage = Turan_LoadImage("Textures/1P.png");
	Sprite* SinglePlayerButton = CreateSprite(SinglePlayerImage);
	SinglePlayerButton->WIDTH = 240;
	SinglePlayerButton->HEIGHT = 80;
	SinglePlayerButton->PosX = 130;
	SinglePlayerButton->PosY = 50;

	void* TwoPlayerImage = Turan_LoadImage("Textures/2P.png");
	Sprite* TwoPlayerButton = CreateSprite(TwoPlayerImage);
	TwoPlayerButton->WIDTH = 240;
	TwoPlayerButton->HEIGHT = 80;
	TwoPlayerButton->PosX = 130;
	TwoPlayerButton->PosY = 150;

	//Waiting for the start loop
	bool isStartPressed = false, isMouseLeftClickPressed = false;
	while (!isStartPressed) {
		StartFrame_Graphics();
		ShowImage(backGr); ShowImage(StartButton);
		if (DidMouseClickOnSprite(StartButton)) {
			isStartPressed = true;
			isMouseLeftClickPressed = true;
		}
		EndFrame_Graphics();
	}

	//Oyun Modu: 0 -> Daha secilmedi, 1 -> Tek kisilik, 2 -> Iki kisilik
	unsigned char GameMode = 0;
	while (GameMode == 0) {
		StartFrame_Graphics();
		ShowImage(backGr);
		ShowImage(SinglePlayerButton);
		ShowImage(TwoPlayerButton);
		//Bir kere de tiklasak, bilgisayar hizli oldugu icin cok kere týklamisiz gibi algilayabiliyor
		//O yuzden sol tikin birakilmasini beklememiz gerek
		if (isMouseLeftClickPressed && !IsMouse_Clicked(MOUSE_LEFT_CLICK)) {
			isMouseLeftClickPressed = false;
		}
		if (!isMouseLeftClickPressed){
			if (DidMouseClickOnSprite(SinglePlayerButton)) {
				GameMode = 1;
				isMouseLeftClickPressed = true;
			}
			else if (DidMouseClickOnSprite(TwoPlayerButton)) {
				GameMode = 2;
				isMouseLeftClickPressed = true;
			}
		}
		EndFrame_Graphics();
	}
	
	bool isGameRunning = true;
	while (ShouldCloseApplication() == 0) {
		StartFrame_Graphics();

		//Bir kere de tiklasak, bilgisayar hizli oldugu icin cok kere týklamisiz gibi algilayabiliyor
		//O yuzden sol tikin birakildigini kontrol etmemiz gerrek
		if (isMouseLeftClickPressed) {
			if (!IsMouse_Clicked(MOUSE_LEFT_CLICK)) {
				isMouseLeftClickPressed = false;
			}
		}


		if (isGameRunning) {
			GAMELOOP(GameMode - 1);
			ShowImage(StopButton);
			if (!isMouseLeftClickPressed && DidMouseClickOnSprite(StopButton)) {
				isGameRunning = false;
				isMouseLeftClickPressed = true;
			}
		}
		else {
			ShowImage(backGr); ShowImage(BackButton);
			if (!isMouseLeftClickPressed && DidMouseClickOnSprite(BackButton)) {
				isGameRunning = true;
				isMouseLeftClickPressed = true;
			}
		}

		EndFrame_Graphics();
	}
}


bool IsKeyPressed[8]{ false }, isMouseButtonPressed[3]{ false };
//SFML'in event'lerinin (Pencere kapama gibi) bir loop içerisinde halledilmesi gerek
void StartFrame_Graphics() {
	sf::Event sfevent;
	while (Window->pollEvent(sfevent)) {
		if (sfevent.type == sf::Event::Closed) {
			Window->close();
		}
	}
	for (unsigned char i = 0; i < 8; i++) {
		IsKeyPressed[i] = false;
	}
	for (unsigned char i = 0; i < 3; i++) {
		isMouseButtonPressed[i] = false;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		IsKeyPressed[0] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		IsKeyPressed[1] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		IsKeyPressed[2] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		IsKeyPressed[3] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::C)) {
		IsKeyPressed[4] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad2)) {
		IsKeyPressed[5] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad4)) {
		IsKeyPressed[6] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad6)) {
		IsKeyPressed[7] = true;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Numpad8)) {
		IsKeyPressed[8] = true;
	}

	//MOUSE PRESSED
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		isMouseButtonPressed[0] = true;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
		isMouseButtonPressed[1] = true;
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		isMouseButtonPressed[2] = true;
	}

	Window->clear(sf::Color::White);
}
void* Turan_LoadImage(const char* PATH) {
	sf::Texture* t = new sf::Texture;
	if (t->loadFromFile(PATH)) {
		printf("Loading of the image is successful!\n");
		return t;
	}
	return nullptr;
}
struct Sprite* CreateSprite(void* Texture) {
	sf::Sprite* sfsprite = new sf::Sprite(*(sf::Texture*)Texture);
	Sprite* tsprite = new Sprite;
	tsprite->sfSprite = sfsprite;
	tsprite->CropStartX = sfsprite->getTextureRect().left;
	tsprite->CropStartY = sfsprite->getTextureRect().top;
	tsprite->WIDTH = sfsprite->getTextureRect().width;
	tsprite->HEIGHT = sfsprite->getTextureRect().height;
	tsprite->PosX = 0; tsprite->PosY = 0;
	return tsprite;
}
void DestroySprite(struct Sprite* sprite) {
	sf::Sprite* sfsprite = (sf::Sprite*)sprite->sfSprite;
	delete sfsprite;
	delete sprite;
}

void ShowImage(struct Sprite* sprite) {
	sf::Sprite* sfSprite = (sf::Sprite*)sprite->sfSprite;

	if (sfSprite->getTexture() == nullptr) {
		printf("Sprite's texture isn't available!\n");
	}
	sfSprite->setPosition(sprite->PosX, sprite->PosY);
	sfSprite->setTextureRect(sf::IntRect(sprite->CropStartX, sprite->CropStartY, sprite->WIDTH, sprite->HEIGHT));
	Window->draw(*sfSprite);
}
void EndFrame_Graphics() {
	Window->display();
}
unsigned int ShouldCloseApplication() {
	return !Window->isOpen();
}
unsigned int DoesTexturesCollide(struct Sprite* Sprite1, struct Sprite* Sprite2) {
	sf::Sprite* sfSprite1 = (sf::Sprite*)Sprite1->sfSprite;
	sf::Sprite* sfSprite2 = (sf::Sprite*)Sprite2->sfSprite;

	sfSprite1->setPosition(Sprite1->PosX, Sprite1->PosY);
	sfSprite1->setTextureRect(sf::IntRect(Sprite1->CropStartX, Sprite1->CropStartY, Sprite1->WIDTH, Sprite1->HEIGHT));
	sfSprite2->setPosition(Sprite2->PosX, Sprite2->PosY);
	sfSprite2->setTextureRect(sf::IntRect(Sprite2->CropStartX, Sprite2->CropStartY, Sprite2->WIDTH, Sprite2->HEIGHT));

	return sfSprite1->getGlobalBounds().intersects(sfSprite2->getGlobalBounds());
}
unsigned char DidMouseClickOnSprite(struct Sprite* sprite) {
	sf::Sprite* sfSprite = (sf::Sprite*)sprite->sfSprite;
	sfSprite->setPosition(sprite->PosX, sprite->PosY);
	sfSprite->setTextureRect(sf::IntRect(sprite->CropStartX, sprite->CropStartY, sprite->WIDTH, sprite->HEIGHT));
	return bool(
		bool(sfSprite->getGlobalBounds().contains(float(sf::Mouse::getPosition(*Window).x), float(sf::Mouse::getPosition(*Window).y))) &&
		bool(IsMouse_Clicked(MOUSE_LEFT_CLICK))
		);
}

int IsKey_Pressed(enum KEYBOARD_KEYs_C key) {
	switch (key)
	{
	case KEYBOARD_W:
		return IsKeyPressed[0];
	case KEYBOARD_A:
		return IsKeyPressed[1];
	case KEYBOARD_S:
		return IsKeyPressed[2];
	case KEYBOARD_D:
		return IsKeyPressed[3];
	case KEYBOARD_C:
		return IsKeyPressed[4];
	case KEYBOARD_NP_2:
		return IsKeyPressed[5];
	case KEYBOARD_NP_4:
		return IsKeyPressed[6];
	case KEYBOARD_NP_6:
		return IsKeyPressed[7];
	case KEYBOARD_NP_8:
		return IsKeyPressed[8];
	default:
		printf("This key isn't defined!");
		return 0;
	}
}
int IsMouse_Clicked(enum MOUSE_BUTTONs_C button) {
	switch (button)
	{
	case MOUSE_LEFT_CLICK:
		return isMouseButtonPressed[0];
	case MOUSE_WHEEL_CLICK:
		return isMouseButtonPressed[1];
	case MOUSE_RIGHT_CLICK:
		return isMouseButtonPressed[2];
	default:
		printf("This mouse button isn't defined!");
		return 0;
	}
}
