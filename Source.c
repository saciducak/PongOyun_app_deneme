#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include "Graphics/Turan_CBinding.h"
#include <math.h>
#include <time.h>
#pragma comment(lib, "winmm.lib")

struct paddle {
	double posx, posy;
};

struct ball {
	double posx, posy;
	double vx,vy;
	unsigned char isMoving;
};


struct paddle pad;
struct paddle pad2;
struct ball top;

typedef enum BlockType {
	NormalBlockType,
	PointPlusBlockType,
	PointSourBlockType,
	SpeedPlusBlockType,
	SpeedSourBlockType,
	SizePlusBlockType,
	SizeSourBlockType
} BlockType;
struct block {
	unsigned char isShown;
	struct Sprite* sprite;
	enum BlockType type;
};
#define MAXIMUMACTIVEBLOCKCOUNT 12
unsigned int CountOfActiveBlocks = 0, BlockWidth = 40, BlockHeight = 23;
struct block blocks[MAXIMUMACTIVEBLOCKCOUNT];

void* blocktexture, * pointPlustexture, * pointSourtexture, * speedPlustexture, * speedSourtexture, * sizePlustexture, * sizeSourtexture;
void RandomBlockGenerator() {
	for (unsigned int CreatedBlock = 0; CreatedBlock < MAXIMUMACTIVEBLOCKCOUNT - CountOfActiveBlocks; CreatedBlock++) {
		unsigned int i = 0;
		for (i = 0; i < MAXIMUMACTIVEBLOCKCOUNT; i++) {
			if (blocks[i].isShown == 0) {
				break;
			}
		}

		blocks[i].isShown = 1;
		unsigned char rand_type = rand() % 16;
		if (rand_type < 10) { blocks[i].type = NormalBlockType; }
		else { blocks[i].type = rand_type - 10; };
		switch (blocks[i].type) {
		case NormalBlockType:
			blocks[i].sprite = CreateSprite(blocktexture);
			break;
		case PointPlusBlockType:
			blocks[i].sprite = CreateSprite(pointPlustexture);
			break;
		case PointSourBlockType:
			blocks[i].sprite = CreateSprite(pointSourtexture);
			break;
		case SpeedPlusBlockType:
			blocks[i].sprite = CreateSprite(speedPlustexture);
			break;
		case SpeedSourBlockType:
			blocks[i].sprite = CreateSprite(speedSourtexture);
			break;
		case SizePlusBlockType:
			blocks[i].sprite = CreateSprite(sizePlustexture);
			break;
		case SizeSourBlockType:
			blocks[i].sprite = CreateSprite(sizeSourtexture);
			break;
		default:
			printf("Block type isn't supported!");
		}
		blocks[i].sprite->WIDTH = BlockWidth;
		blocks[i].sprite->HEIGHT = BlockHeight;


		//Konumlandirma
		float GridSizeX = (float)((float)(SCREENWIDTH - 10.0) / ((float)(BlockWidth + 3)));
		float GridSizeY = (float)((float)(SCREENHEIGHT - 150.0) / ((float)(BlockHeight + 3)));
		unsigned char isFoundAnEmptyPlace = 0;
		while (isFoundAnEmptyPlace == 0) {
			float PosX = ((rand() % (unsigned int)(GridSizeX)) * (BlockWidth + 3)) + 10.0f;
			float PosY = ((rand() % (unsigned int)(GridSizeY)) * (BlockHeight + 3)) + 75.0f;

			unsigned char isColliding = 0;
			for (unsigned int SearchIndex = 0; SearchIndex < MAXIMUMACTIVEBLOCKCOUNT && isColliding == 0; SearchIndex++) {
				if (blocks[SearchIndex].isShown == 0 || SearchIndex == i) {
					continue;
				}

				if (blocks[SearchIndex].sprite->PosX == PosX && blocks[SearchIndex].sprite->PosY == PosY) {
					isColliding = 1;
				}
			}
			if (isColliding == 0) {
				isFoundAnEmptyPlace = 1;
				blocks[i].sprite->PosX = PosX;
			 	blocks[i].sprite->PosY = PosY;
			}
		}
	}

	CountOfActiveBlocks = MAXIMUMACTIVEBLOCKCOUNT;
}

void TopunYonunuDegistir(struct ball* top, struct paddle* pad, int* carpan_puan, int* asil_puan, int* carpan_puan2, int* asil_puan2, float ballsSpeed) {

	if (top->posx >= SCREENWIDTH - 17.0f) {
		mciSendString("play duvar from 0", NULL, 0, 0);
		if (top->vx > 0)
			top->vx = -top->vx;
	}
	else if (top->posx <= 9.0f) {
		mciSendString("play duvar from 0", NULL, 0, 0);
		if(top->vx<0)
			top->vx = -top->vx;
	}
	if (top->posy <= -30.0f) {
		mciSendString("play gameO from 0", NULL, 0, 0);
		*carpan_puan = 0;
		*asil_puan += 1;
		top->vy = -top->vy;
		top->posx = pad2.posx + 15;
		top->posy = pad2.posy + 50;
		RandomBlockGenerator();
	}
	else if (top->posy >= SCREENHEIGHT + 30.0f) {
		mciSendString("play gameO from 0", NULL, 0, 0);
		*carpan_puan2 = 0;
		*asil_puan2 += 1;
		top->vy = -top->vy;
		top->posx = pad->posx + 15;
		top->posy = pad->posy - 50;

		RandomBlockGenerator();
	}
}


unsigned char CollideTopwithTexture(struct Sprite* topsprite, struct ball* top, struct Sprite* Texture) {
	if (!DoesTexturesCollide(topsprite, Texture)) {
		return 0;
	}
	unsigned int CollidingTopPosX = top->posx;
	unsigned int CollidingTopPosY = top->posy;

	topsprite->PosX -= top->vx;
	topsprite->PosY -= top->vy;
	
	for (unsigned int IterativeSolverIndex = 0; IterativeSolverIndex < 256; IterativeSolverIndex++) {
		topsprite->PosX += top->vx / 256.0;
		topsprite->PosY += top->vy / 256.0;
		if (DoesTexturesCollide(topsprite, Texture)) {
			topsprite->PosX -= top->vx;
			topsprite->PosY -= top->vy;
			return 1;
		}
	}
}


int i, j, p = 0;
//bu degisken sesin bozuk cikmamasini sagliyor
int optimize = 0;

//hizlar
float ballsSpeed = 7.0f, paddelsSpeed = 17.0f, ballsFriction = 1.01f, paddelsBouncePower = 1.65f;

//bloklarin boyutlari
int blockX = 40, blockY = 23, arkaplanX = SCREENWIDTH, arkaplanY = SCREENHEIGHT, paddleX = 90, paddleY = 9, topXY = 13;

//puan sistemi degiskenleri
int p1_asil_puan = 0;
int p1_carpan_puan = 0;
int p1_topbizde = 1;
int p2_asil_puan = 0;
int p2_carpan_puan = 0, p2_topbizde = 0, limit_deger = 50;



struct Sprite* arkaplansprite, *p1_paddlesprite, *p2_paddlesprite, *topsprite, *carpan1sprite, *puan1sprite, *carpan2sprite, *puan2sprite;

float saniye = 0, saniye1 = 0, saniye2 = 0, saniye3 = 0;
int sayabilirsin = 0, sayabilirsin1 = 0, sayabilirsin2 = 0, sayabilirsin3 = 0, p1_buyudu = 0, p1_kuculdu = 0;

void* p1_paddletexture, *p2_paddletexture, *p1_paddleLongtexture, *p2_paddleLongtexture, *p1_paddleShorttexture, *p2_paddleShorttexture, *Rakamlar[10];


void Setup() {
	srand(time(NULL));
	PlaySound("Sounds/arkaPlanMuzigi.wav", NULL, SND_ASYNC | SND_LOOP | SND_FILENAME);

	//ayný seyin iki kez acilma sebebi art arda geldince sorun olmasidir
	mciSendString("open Sounds/klasikBlokKirildi.wav alias block", NULL, 0, 0);
	mciSendString("open Sounds/klasikBlokKirildi.wav alias block1", NULL, 0, 0);
	mciSendString("open Sounds/olumluBlokKirildi.wav alias olumluBlock", NULL, 0, 0);
	mciSendString("open Sounds/olumsuzBlokKirildi.wav alias olumsuzBlock", NULL, 0, 0);
	mciSendString("open Sounds/paddenSekti.wav alias pad", NULL, 0, 0);
	mciSendString("open Sounds/duvardanSekme.wav alias duvar", NULL, 0, 0);
	mciSendString("open Sounds/gameOver.wav alias gameO", NULL, 0, 0);


	void* toptexture = Turan_LoadImage("Textures/ball.png");
	blocktexture = Turan_LoadImage("Textures/block.png");
	void* arkaplantexture = Turan_LoadImage("Textures/backGround.png");
	pointPlustexture = Turan_LoadImage("Textures/pointPlus.png");
	pointSourtexture = Turan_LoadImage("Textures/pointSour.png");
	speedPlustexture = Turan_LoadImage("Textures/speedPlus.png");
	speedSourtexture = Turan_LoadImage("Textures/speedSour.png");
	sizePlustexture = Turan_LoadImage("Textures/sizePlus.png");
	sizeSourtexture = Turan_LoadImage("Textures/sizeSour.png");

	p1_paddletexture = Turan_LoadImage("Textures/paddle.png");
	p2_paddletexture = Turan_LoadImage("Textures/paddle2.png");
	p1_paddleLongtexture = Turan_LoadImage("Textures/paddleLong.png");
	p2_paddleLongtexture = Turan_LoadImage("Textures/paddleLong2.png");
	p1_paddleShorttexture = Turan_LoadImage("Textures/paddleShort.png");
	p2_paddleShorttexture = Turan_LoadImage("Textures/paddleShort2.png");

	void* puantexture = Turan_LoadImage("Textures/puan.png");
	puan1sprite = CreateSprite(puantexture);
	puan1sprite->WIDTH = 83;
	puan1sprite->HEIGHT = 33;
	puan1sprite->PosX = 15;
	puan1sprite->PosY = 10;
	puan2sprite = CreateSprite(puantexture);
	puan2sprite->WIDTH = 83;
	puan2sprite->HEIGHT = 33;
	puan2sprite->PosX = 15;
	puan2sprite->PosY = SCREENHEIGHT - 40;


	void* carpantexture = Turan_LoadImage("Textures/carpan.png");
	carpan1sprite = CreateSprite(carpantexture);
	carpan1sprite->WIDTH = 112;
	carpan1sprite->HEIGHT = 33;
	carpan1sprite->PosX = SCREENWIDTH / 2;
	carpan1sprite->PosY = 10;
	carpan2sprite = CreateSprite(carpantexture);
	carpan2sprite->WIDTH = 112;
	carpan2sprite->HEIGHT = 33;
	carpan2sprite->PosX = SCREENWIDTH / 2;
	carpan2sprite->PosY = SCREENHEIGHT - 40;



	top.vx = ballsSpeed;
	top.vy = ballsSpeed;
	top.isMoving = 0;


	//pad ve topun olusacagi konum
	//burada paddlespeed kullanilma sebebi pad arkaplanin disina tasmasin diye
	pad.posy = 550;
	pad.posx = paddelsSpeed * 5 + 7;
	pad2.posy = 50;
	pad2.posx = paddelsSpeed * 5 + 7;
	unsigned int padposx_int = pad.posx;
	top.posx = pad.posx - (padposx_int % 13);
	top.posy = pad.posy - 30;

	p = 0;
	for (unsigned int i = 0; i < MAXIMUMACTIVEBLOCKCOUNT; i++) {
		blocks[i].isShown = 0;
		blocks[i].sprite = NULL;
		blocks[i].type = NormalBlockType;
	}
	RandomBlockGenerator();


	arkaplansprite = CreateSprite(arkaplantexture);
	arkaplansprite->PosX = 0;
	arkaplansprite->PosY = 0;
	arkaplansprite->WIDTH = arkaplanX;
	arkaplansprite->HEIGHT = arkaplanY;

	p1_paddlesprite = CreateSprite(p1_paddletexture);
	p1_paddlesprite->PosX = pad.posx;
	p1_paddlesprite->PosY = pad.posy;
	p1_paddlesprite->WIDTH = paddleX;
	p1_paddlesprite->HEIGHT = paddleY;

	p2_paddlesprite = CreateSprite(p2_paddletexture);
	p2_paddlesprite->PosX = pad2.posx;
	p2_paddlesprite->PosY = pad2.posy;
	p2_paddlesprite->WIDTH = paddleX;
	p2_paddlesprite->HEIGHT = paddleY;

	topsprite = CreateSprite(toptexture);
	topsprite->WIDTH = topXY;
	topsprite->HEIGHT = topXY;
	topsprite->PosX = top.posx;
	topsprite->PosY = top.posy;


	//Skor Tablosu icin rakamlari yukle
	Rakamlar[0] = Turan_LoadImage("Textures/0.png");
	Rakamlar[1] = Turan_LoadImage("Textures/1.png");
	Rakamlar[2] = Turan_LoadImage("Textures/2.png");
	Rakamlar[3] = Turan_LoadImage("Textures/3.png");
	Rakamlar[4] = Turan_LoadImage("Textures/4.png");
	Rakamlar[5] = Turan_LoadImage("Textures/5.png");
	Rakamlar[6] = Turan_LoadImage("Textures/6.png");
	Rakamlar[7] = Turan_LoadImage("Textures/7.png");
	Rakamlar[8] = Turan_LoadImage("Textures/8.png");
	Rakamlar[9] = Turan_LoadImage("Textures/9.png");
}

unsigned char AISpeedMultiplierFrameCounter = 0; float AIPaddleSpeedMultiplier = 1.0f;
void GameLoop(unsigned char isTwoPlayer) {
	ShowImage(arkaplansprite);
	//size buff
	if (sayabilirsin)
		saniye += 2;
	if (saniye / 60 >= 10) {
		if (p1_buyudu)
			p1_paddlesprite = CreateSprite(p1_paddletexture);
		else
			p2_paddlesprite = CreateSprite(p2_paddletexture);

		sayabilirsin = 0;
		saniye = 0;

	}
	//size nerf
	if (sayabilirsin1)
		saniye1 += 2;
	if (saniye1 / 60 >= 10) {

		if (p1_kuculdu)
			p1_paddlesprite = CreateSprite(p1_paddletexture);
		else
			p2_paddlesprite = CreateSprite(p2_paddletexture);

		sayabilirsin1 = 0;
		saniye1 = 0;

	}
	//speed buff
	if (sayabilirsin2)
		saniye2 += 2;
	if (saniye2 / 60 >= 3) {
		sayabilirsin2 = 0;
		saniye2 = 0;
		top.vx = top.vx / 1.25f;
		top.vy = top.vy / 1.25f;
	}
	//speed nerf
	if (sayabilirsin3)
		saniye3 += 2;
	if (saniye3 / 60 >= 3) {
		sayabilirsin3 = 0;
		saniye3 = 0;
		top.vx = top.vx * 1.25f;
		top.vy = top.vy * 1.25f;
	}

	for (unsigned int BlockIndex = 0; BlockIndex < MAXIMUMACTIVEBLOCKCOUNT; BlockIndex++) {
		//Eger blok gosterilmiyorsa, es geç
		if (blocks[BlockIndex].isShown == 0) {
			continue;
		}

		ShowImage(blocks[BlockIndex].sprite);
		//Eger carpmadýysa, es geç
		if (CollideTopwithTexture(topsprite, &top, blocks[BlockIndex].sprite) == 0) {
			continue;
		}

		top.vx = top.vx;
		top.vy = -top.vy;
		blocks[BlockIndex].isShown = 0;
		//DestroySprite(blocks[i].sprite);
		blocks[BlockIndex].sprite = NULL;
		CountOfActiveBlocks--;

		switch (blocks[BlockIndex].type) {
		case NormalBlockType:
			if (optimize % 2 != 0) {
				mciSendString("play block from 0", NULL, 0, 0);
			}
			else {
				mciSendString("play block1 from 0", NULL, 0, 0);
			}
			optimize++;
			if (p1_topbizde)
				p1_carpan_puan += 10;
			else
				p2_carpan_puan += 10;
			break;
		case PointPlusBlockType:
			mciSendString("play olumluBlock from 0", NULL, 0, 0);
			if (p1_topbizde) {
				p1_asil_puan += 1;
			}
			else {
				p2_asil_puan += 1;
			}
			break;
		case PointSourBlockType:
			mciSendString("play olumsuzBlock from 0", NULL, 0, 0);
			if (p1_topbizde) {
				p2_asil_puan += 1;
			}
			else {
				p1_asil_puan += 1;
			}
			break;
		case SpeedPlusBlockType:
			mciSendString("play olumluBlock from 0", NULL, 0, 0);
			top.vx *= 1.25f;
			top.vy *= 1.25f;
			sayabilirsin2 = 1;
			if (p1_topbizde)
				p1_carpan_puan += 10;
			else
				p2_carpan_puan += 10;
			break;
		case SpeedSourBlockType:
			mciSendString("play olumsuzBlock from 0", NULL, 0, 0);
			top.vx = top.vx / 1.25f;
			top.vy = top.vy / 1.25f;
			sayabilirsin3 = 1;
			if (p1_topbizde)
				p1_carpan_puan += 10;
			else
				p2_carpan_puan += 10;
			break;
		case SizePlusBlockType:
			mciSendString("play olumluBlock from 0", NULL, 0, 0);
			if (p1_topbizde) {
				p1_paddlesprite = CreateSprite(p1_paddleLongtexture);
				p1_buyudu = 1;
			}

			else {
				p2_paddlesprite = CreateSprite(p2_paddleLongtexture);
				p1_buyudu = 0;
			}

			sayabilirsin = 1;
			if (p1_topbizde)
				p1_carpan_puan += 10;
			else
				p2_carpan_puan += 10;
			break;
		case SizeSourBlockType:
			mciSendString("play olumsuzBlock from 0", NULL, 0, 0);
			if (p1_topbizde) {
				p1_paddlesprite = CreateSprite(p1_paddleShorttexture);
				p1_kuculdu = 1;
			}

			else {
				p2_paddlesprite = CreateSprite(p2_paddleShorttexture);
				p1_kuculdu = 0;
			}

			sayabilirsin1 = 1;
			if (p1_topbizde)
				p1_carpan_puan += 10;
			else
				p2_carpan_puan += 10;
			break;
		default:
			printf("BlockType isn't supported!");
		}
	}



	//pad1'den sekme
	//buradaki if statementlari top x ekseninde surekli cizgisel gitmesin diye yazildi
	if (CollideTopwithTexture(topsprite, &top, p1_paddlesprite)) {
		mciSendString("play pad from 0", NULL, 0, 0);
		p2_topbizde = 0;
		p1_topbizde = 1;
		if (IsKey_Pressed(KEYBOARD_A)) {
			if (top.vx > 0) {
				top.vx = top.vx / paddelsBouncePower;
				top.vy = -top.vy;
				RandomBlockGenerator();
			}
			else {
				top.vx = top.vx * paddelsBouncePower;
				top.vy = -top.vy;
				RandomBlockGenerator();
			}
		}
		else if (IsKey_Pressed(KEYBOARD_D)) {
			if (top.vx > 0) {
				top.vx = top.vx * paddelsBouncePower;
				top.vy = -top.vy;
				RandomBlockGenerator();
			}
			else {
				top.vx = top.vx / paddelsBouncePower;
				top.vy = -top.vy;
				RandomBlockGenerator();
			}
			
		}
		else {
			top.vx = top.vx;
			top.vy = -top.vy;
			RandomBlockGenerator();
		}
	}
	//topa hizlandiginda surtunme kuvveti uygulaniyor
	
	if (top.vx > 0) {

		if (top.vx > ballsSpeed) {
			top.vx = top.vx / ballsFriction;
		}
		else if (top.vx < ballsSpeed) {
			top.vx = top.vx * ballsFriction;
		}
	}
	else if (top.vx < 0) {
		if ((-1)*top.vx > ballsSpeed) {
			top.vx = top.vx / ballsFriction;
		}
		else if ((-1)*top.vx < ballsSpeed) {
			top.vx = top.vx * ballsFriction;
		}
	}


	//pad2'dan dekme
	if (CollideTopwithTexture(topsprite, &top, p2_paddlesprite)) {
		mciSendString("play pad from 0", NULL, 0, 0);
		p2_topbizde = 1;
		p1_topbizde = 0;
		top.vx = top.vx;
		top.vy = -top.vy;
		RandomBlockGenerator();
	}

	//Player 1'in konumunu degistirir
	if (IsKey_Pressed(KEYBOARD_D)) {

		pad.posx += paddelsSpeed;
		if (pad.posx >= arkaplanX - paddleX - 7) {

			pad.posx -= paddelsSpeed;

		}
	}
	else if (IsKey_Pressed(KEYBOARD_A) && pad.posx > 7) {

		pad.posx -= paddelsSpeed;
	}

	p1_paddlesprite->PosX = pad.posx;
	p1_paddlesprite->PosY = pad.posy;
	ShowImage(p1_paddlesprite);


	//Player 2'in konumunu degistirir
	//Eger tek kisilik modsa, yapay zeka hareket edecek
	if (isTwoPlayer == 0) {
		//Tek kisilik mod
		if ((AISpeedMultiplierFrameCounter % 25) == 24) {
			AIPaddleSpeedMultiplier = ((float)(rand() % 10) / 9.0f) + 0.1f;
			AISpeedMultiplierFrameCounter = 0;
		}
		AISpeedMultiplierFrameCounter++;
		pad2.posx += (top.posx - pad2.posx - 45) * AIPaddleSpeedMultiplier * 17.0 / 100.0;
		if (pad2.posx + p2_paddlesprite->WIDTH >= SCREENWIDTH) {
			pad2.posx = SCREENWIDTH - p2_paddlesprite->WIDTH;
		}
		else if (pad2.posx < 0) {
			pad2.posx = 0;
		}
	}
	else {
		//Iki kisilik mod
		if (IsKey_Pressed(KEYBOARD_NP_6)) {
			pad2.posx += paddelsSpeed;
			if (pad2.posx >= arkaplanX - paddleX - 7) {
				pad2.posx -= paddelsSpeed;
			}
		}
		else if (IsKey_Pressed(KEYBOARD_NP_4) && pad2.posx > 7) {
			pad2.posx -= paddelsSpeed;
		}
	}




	p2_paddlesprite->PosX = pad2.posx;
	p2_paddlesprite->PosY = pad2.posy;
	ShowImage(p2_paddlesprite);

	//top ilk hareket icin kullaniciyi bekliyor
	if (top.isMoving == 0 && (IsKey_Pressed(KEYBOARD_A) || IsKey_Pressed(KEYBOARD_D))) {

		top.isMoving = 1;
	}
	if (top.isMoving) {

		top.posx += top.vx;
		top.posy += top.vy;
	}

	//Topun yonu degismesi gerekirse degistirir
	TopunYonunuDegistir(&top, &pad, &p1_carpan_puan, &p1_asil_puan, &p2_carpan_puan, &p2_asil_puan, ballsSpeed);

	//sprite gösteriliyor ve konum guncelleniyor
	topsprite->PosX = top.posx;
	topsprite->PosY = top.posy;
	ShowImage(topsprite);

	//asil puan artiyor
	if (p1_carpan_puan >= limit_deger)
	{
		p1_asil_puan += 1;
		p1_carpan_puan = 0;
	}
	if (p2_carpan_puan >= limit_deger)
	{
		p2_asil_puan += 1;
		p2_carpan_puan = 0;
	}

	if (p1_asil_puan <= 0) {
		p1_asil_puan = 0;
	}
	if (p2_asil_puan <= 0) {
		p2_asil_puan = 0;
	}
	//Player 1 Skor Tablosunu Goster
	{
		ShowImage(puan1sprite);
		ShowImage(carpan1sprite);
		unsigned int AsilPuan = p1_asil_puan;
		unsigned char AsilPuanlog10 = log10(p1_asil_puan);
		AsilPuanlog10++;
		struct Sprite** AsilPuanSprite = malloc(sizeof(struct Sprite*) * AsilPuanlog10);
		unsigned int LastPosFromRight = 30;
		for (unsigned char i = 0; i < AsilPuanlog10; i++) {
			unsigned char GosterilecekRakam = AsilPuan % 10;
			AsilPuanSprite[AsilPuanlog10 - i - 1] = CreateSprite(Rakamlar[GosterilecekRakam]);
			AsilPuanSprite[AsilPuanlog10 - i - 1]->WIDTH = 25;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->HEIGHT = 33;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->PosX = (SCREENWIDTH / 2) - LastPosFromRight - 25;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->PosY = 10;
			ShowImage(AsilPuanSprite[AsilPuanlog10 - i - 1]);
			LastPosFromRight += 25;
			AsilPuan = AsilPuan / 10;
		}

		unsigned int CarpanPuan = p1_carpan_puan;
		unsigned char CarpanPuanlog10 = log10(p1_carpan_puan);
		CarpanPuanlog10++;
		struct Sprite** CarpanPuanSprite = malloc(sizeof(struct Sprite*) * CarpanPuanlog10);
		LastPosFromRight = 0;
		for (unsigned char i = 0; i < CarpanPuanlog10; i++) {
			unsigned char GosterilecekRakam = CarpanPuan % 10;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1] = CreateSprite(Rakamlar[GosterilecekRakam]);
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->WIDTH = 25;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->HEIGHT = 33;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->PosX = SCREENWIDTH - LastPosFromRight - 25;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->PosY = 10;
			ShowImage(CarpanPuanSprite[CarpanPuanlog10 - i - 1]);
			LastPosFromRight += 25;
			CarpanPuan = CarpanPuan / 10;
		}
	}
	//Player 2 Skor Tablosunu Goster
	{
		ShowImage(puan2sprite);
		ShowImage(carpan2sprite);
		unsigned int AsilPuan = p2_asil_puan;
		unsigned char AsilPuanlog10 = log10(p2_asil_puan);
		AsilPuanlog10++;
		struct Sprite** AsilPuanSprite = malloc(sizeof(struct Sprite*) * AsilPuanlog10);
		unsigned int LastPosFromRight = 30;
		for (unsigned char i = 0; i < AsilPuanlog10; i++) {
			unsigned char GosterilecekRakam = AsilPuan % 10;
			AsilPuanSprite[AsilPuanlog10 - i - 1] = CreateSprite(Rakamlar[GosterilecekRakam]);
			AsilPuanSprite[AsilPuanlog10 - i - 1]->WIDTH = 25;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->HEIGHT = 33;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->PosX = (SCREENWIDTH / 2)- LastPosFromRight - 25;
			AsilPuanSprite[AsilPuanlog10 - i - 1]->PosY = SCREENHEIGHT - 40;
			ShowImage(AsilPuanSprite[AsilPuanlog10 - i - 1]);
			LastPosFromRight += 25;
			AsilPuan = AsilPuan / 10;
		}

		unsigned int CarpanPuan = p2_carpan_puan;
		unsigned char CarpanPuanlog10 = log10(p2_carpan_puan);
		CarpanPuanlog10++;
		struct Sprite** CarpanPuanSprite = malloc(sizeof(struct Sprite*) * CarpanPuanlog10);
		LastPosFromRight = 0;
		for (unsigned char i = 0; i < CarpanPuanlog10; i++) {
			unsigned char GosterilecekRakam = CarpanPuan % 10;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1] = CreateSprite(Rakamlar[GosterilecekRakam]);
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->WIDTH = 25;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->HEIGHT = 33;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->PosX = SCREENWIDTH - LastPosFromRight - 25;
			CarpanPuanSprite[CarpanPuanlog10 - i - 1]->PosY = SCREENHEIGHT - 40;
			ShowImage(CarpanPuanSprite[CarpanPuanlog10 - i - 1]);
			LastPosFromRight += 25;
			CarpanPuan = CarpanPuan / 10;
		}
	}
}



int main() {
	Start_GameEngine(Setup, GameLoop); 


	printf("Game engine is closed, so application is closed too!");
	return 1;
}
