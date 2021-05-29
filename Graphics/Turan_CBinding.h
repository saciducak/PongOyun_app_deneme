#pragma once
#define SCREENWIDTH 500.0
#define SCREENHEIGHT 600.0

#ifdef __cplusplus
extern "C" {
#endif 
	void Start_GameEngine(void(*Setup_funcptr)(), void(*GameLoop)(unsigned char isTwoPlayer));
	struct Sprite {
		double PosX, PosY, CropStartX, CropStartY, WIDTH, HEIGHT;
		void* sfSprite;
	};
	void* Turan_LoadImage(const char* PATH);
	struct Sprite* CreateSprite(void* Texture);
	void DestroySprite(struct Sprite* sprite);
	void ShowImage(struct Sprite* SpriteToShow);
	unsigned int ShouldCloseApplication();
	unsigned int DoesTexturesCollide(struct Sprite* Sprite1, struct Sprite* Sprite2);
	unsigned char DidMouseClickOnSprite(struct Sprite* sprite);

	enum KEYBOARD_KEYs_C {
		KEYBOARD_W = 0,
		KEYBOARD_A,
		KEYBOARD_S,
		KEYBOARD_D,
		KEYBOARD_C,
		KEYBOARD_NP_2,
		KEYBOARD_NP_4,
		KEYBOARD_NP_6,
		KEYBOARD_NP_8
	};
	enum MOUSE_BUTTONs_C {
		MOUSE_LEFT_CLICK = 0,
		MOUSE_WHEEL_CLICK = 1,
		MOUSE_RIGHT_CLICK = 2
	};


	int IsKey_Pressed(enum KEYBOARD_KEYs_C key);
	int IsMouse_Clicked(enum MOUSE_BUTTONs_C button);

#ifdef __cplusplus
}
#endif  