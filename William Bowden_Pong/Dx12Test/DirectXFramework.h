//////////////////////////////////////////////////////////////////////////
// Name:	DirectXFramework.h
// Date:	April 2nd, 2010
// Author:	Kyle Lauing [klauing@devry.edu] or [kylelauing@gmail.com]
// Purpose: This file is used to create a very simple framework for using
//			DirectX 9 for the GSP 381 course for DeVry University.
// Disclaimer:	
//			Copyright © 2010 by DeVry Educational Development Corporation.
//			All rights reserved.  No part of this work may be reproduced 
//			or used in any form or by any means – graphic, electronic, or 
//			mechanical, including photocopying, recording, Web distribution 
//			or information storage and retrieval systems – without the 
//			prior consent of DeVry Educational Development Corporation.
//////////////////////////////////////////////////////////////////////////
#pragma once
#pragma comment(lib, "winmm.lib")
//////////////////////////////////////////////////////////////////////////
// Direct3D 9 headers and libraries required
//////////////////////////////////////////////////////////////////////////
#include <d3d9.h>
#include <d3dx9.h>
#define DIRECTINPUT_VERSION 0x0800

#include <InitGuid.h>
#include <dinput.h>
#include <math.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <dshow.h>

#include <fmod.hpp>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "Strmiids.lib")


// Macro to release COM objects fast and safely
#define SAFE_RELEASE(x) if(x){x->Release(); x = 0;}

struct mySprite
{
	float				xp, yp;
	int					rot, size;
};

struct myBall
{
	float				xp, yp;

	bool				DIR_UP_RIGHT;
	bool				DIR_DOWN_RIGHT;
	bool				DIR_DOWN_LEFT;
	bool				DIR_UP_LEFT;
	
};

struct myStartMenu
{
	float				xp, yp;

	bool				onSTART;
	bool				onCREDITS;
	bool				onCREDITS2;
	bool				onEXIT;
	bool				onGAME;
	bool				onMovie;
};
class CDirectXFramework
{
	//////////////////////////////////////////////////////////////////////////
	// Application Variables
	//////////////////////////////////////////////////////////////////////////
	HWND				m_hWnd;			// Handle to the window
	bool				m_bVsync;		// Boolean for vertical syncing

	//////////////////////////////////////////////////////////////////////////
	// Direct3D Variables
	//////////////////////////////////////////////////////////////////////////
	IDirect3D9*			m_pD3DObject;	// Direct3D 9 Object
	IDirect3DDevice9*	m_pD3DDevice;	// Direct3D 9 Device
	D3DCAPS9			m_D3DCaps;		// Device Capabilities
	IGraphBuilder*		m_pGraphBuilder; //Direct Show

	//////////////////////////////////////////////////////////////////////////
	// Font Variables
	//////////////////////////////////////////////////////////////////////////
	ID3DXFont*			m_pD3DFont;		// Font Object

	//////////////////////////////////////////////////////////////////////////
	// Sprite Variables
	//////////////////////////////////////////////////////////////////////////
	mySprite			Paddle[2];
	myBall				Ball;
	myStartMenu			Wall, Menu;
	ID3DXSprite*		m_pD3DSprite;	// Sprite Object
	ID3DXSprite*		m_Ball;
	ID3DXSprite*		m_Wall;

	ID3DXSprite*		m_START; ID3DXSprite* m_CREDITS; ID3DXSprite* m_CREDITS2; ID3DXSprite* m_EXIT;

	IDirect3DTexture9*	m_pTexture;		// Texture Object for a sprite
	IDirect3DTexture9*  m_ballTexture;
	IDirect3DTexture9*  m_wallTexture;

	IDirect3DTexture9*	m_StartText; IDirect3DTexture9* m_CreditText; IDirect3DTexture9* m_Credit2Text; 
	IDirect3DTexture9*	m_ExitText;

	D3DXIMAGE_INFO		m_imageInfo;	// File details of a texture
	D3DXIMAGE_INFO		m_ballImage, m_wallImage;

	D3DXIMAGE_INFO		m_StartImage, m_CreditImage, m_Credit2Image, m_ExitImage;

	//////////////////////////////////////////////////////////////////////////
	// Sprite Variables
	//////////////////////////////////////////////////////////////////////////
	LPDIRECTINPUT8 m_pDIObject;
	LPDIRECTINPUTDEVICE8 m_pDIKeyboard;
	LPDIRECTINPUTDEVICE8 m_pDIMouse;
	HRESULT hr;
	char keyboardBuffer[256]; // keyboard data info
	DIMOUSESTATE2 mouseState; // mouse data info

	int controlActive;
	int controlDown;
	int controlUp;
	int controlCurrent;
	int controlPrevious;

	FMOD::Sound *mySound1;
	FMOD::Sound *mySound2;
	FMOD::Sound *mySound3;
	FMOD_RESULT result;
	FMOD::System *system;
	FMOD::Channel *channel;
	FMOD::Channel *channel1;
	FMOD::Sound* myStream;


public:
	//////////////////////////////////////////////////////////////////////////
	// Init and Shutdown are preferred to constructors and destructor,
	// due to having more control when to explicitly call them when global.
	//////////////////////////////////////////////////////////////////////////
	CDirectXFramework(void);
	~CDirectXFramework(void);

	//////////////////////////////////////////////////////////////////////////
	// Name:		Init
	// Parameters:	HWND hWnd - Handle to the window for the application
	//				HINSTANCE hInst - Handle to the application instance
	//				bool bWindowed - Boolean to control windowed or full-screen
	// Return:		void
	// Description:	Ran once at the start.  Initialize DirectX components and 
	//				variables to control the application.  
	//////////////////////////////////////////////////////////////////////////
	void Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed);

	//////////////////////////////////////////////////////////////////////////
	// Name:		Update
	// Parameters:	float elapsedTime - Time that has elapsed since the last
	//					update call.
	// Return:		void
	// Description: Runs every frame, use dt to limit functionality called to
	//				a certain amount of elapsed time that has passed.  Used 
	//				for updating variables and processing input commands prior
	//				to calling render.
	//////////////////////////////////////////////////////////////////////////
	void Update();

	//////////////////////////////////////////////////////////////////////////
	// Name:		Render
	// Parameters:	float elapsedTime - Time that has elapsed since the last
	//					render call.
	// Return:		void
	// Description: Runs every frame, use dt to limit functionality called to
	//				a certain amount of elapsed time that has passed.  Render
	//				calls all draw call to render objects to the screen.
	//////////////////////////////////////////////////////////////////////////
	void Render();

	//////////////////////////////////////////////////////////////////////////
	// Name:		Shutdown
	// Parameters:	void
	// Return:		void
	// Description:	Runs once at the end of an application.  Destroy COM 
	//				objects and deallocate dynamic memory.
	//////////////////////////////////////////////////////////////////////////
	void Shutdown();

	void Getinput();

	void Pollinput();

	

};
