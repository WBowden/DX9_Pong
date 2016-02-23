//////////////////////////////////////////////////////////////////////////
// Name:	DirectXFramework.cpp
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
#include "DirectXFramework.h"

//Key Flags
#define W_UP 0x000000001
#define S_DOWN 0x000000002
#define ARROW_UP 0x000000004
#define ARROW_DOWN 0x00000008
#define ARROW_LEFT 0x00000010
#define ENTER_KEY 0x00000020
#define DISPLAY_SPRITE 0x00000040

//Sound Flags
#define SOUND1 0x00000080
#define SOUND2 0x00000100




ID3DXSprite*			m_pD3DSprite;
IDirect3DTexture9*		m_pTexture;
IDirect3DTexture9*		m_ballTexture;
IDirect3DTexture9*		m_wallTexture;
D3DXIMAGE_INFO			m_imageInfo;
D3DXIMAGE_INFO			m_ballImage, m_wallImage;
ID3DXFont*				m_pD3DFont;
IGraphBuilder*			m_pGraphBuilder;
IMediaControl*			m_pMediaControl;
IMediaEvent*			m_pMediaEvent;
IVideoWindow*			m_pVideoWindow;





CDirectXFramework::CDirectXFramework(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync		= false;
	m_pD3DObject	= 0;
	m_pD3DDevice	= 0;
	
}

CDirectXFramework::~CDirectXFramework(void)
{
	// If Shutdown is not explicitly called correctly, call it when 
	// this class is destroyed or falls out of scope as an error check.
	Shutdown();
}


void CDirectXFramework::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	m_hWnd = hWnd;
	CoInitialize(NULL);
	
	CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                  IID_IGraphBuilder, (void**)&m_pGraphBuilder);

	m_pGraphBuilder->QueryInterface(IID_IMediaControl,
                                (void**)&m_pMediaControl);

	m_pGraphBuilder->QueryInterface(IID_IMediaEvent,
                                (void**)&m_pMediaEvent);

	m_pGraphBuilder->RenderFile(L"intro.wmv", NULL);

	m_pMediaControl->QueryInterface(IID_IVideoWindow,
                                (void**)&m_pVideoWindow);
	// Setup the window
	m_pVideoWindow->put_Owner((OAHWND)m_hWnd);
	// Set the style
	m_pVideoWindow->put_WindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE);
	// Obtain the size of the window
	RECT WinRect;
	GetClientRect(m_hWnd, &WinRect);
	// Set the video size to the size of the window
	WinRect.top = 0;
	WinRect.bottom = 600;
	WinRect.left = 0;
	WinRect.right = 800;
	m_pVideoWindow->SetWindowPosition(WinRect.left, WinRect.top, 
										WinRect.right, WinRect.bottom);

	//////////////////////////////////////////////////////////////////////////
	// Direct3D Foundations - D3D Object, Present Parameters, and D3D Device
	//////////////////////////////////////////////////////////////////////////

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetWindowRect()
	RECT rect;

	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}
	
	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}
	
	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	//*************************************************************************

	//////////////////////////////////////////////////////////////////////////
	// Create a Font Object
	//////////////////////////////////////////////////////////////////////////
	
	// Load a font for private use for this process

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXCreateFont(m_pD3DDevice, 30, 0, FW_BOLD, 0, false, 
                  DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
                  DEFAULT_PITCH | FF_DONTCARE, TEXT("Times New Roman"), 
                  &m_pD3DFont);

	

	//////////////////////////////////////////////////////////////////////////
	// Create Sprite Object and Textures
	//////////////////////////////////////////////////////////////////////////
	// Create a sprite object, note you will only need one for all 2D sprites

	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);
	D3DXCreateSprite(m_pD3DDevice, &m_Ball);
	D3DXCreateSprite(m_pD3DDevice, &m_Wall);

	// Create a texture, each different 2D sprite to display to the screen
	// will need a new texture object.  If drawing the same sprite texture
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Paddle.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
                  &m_imageInfo, 0, &m_pTexture);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"Ball.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_ballImage, 0, &m_ballTexture);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"wall.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_wallImage, 0, &m_wallTexture);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"START.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_StartImage, 0, &m_StartText);

	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"CREDITS.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_CreditImage, 0, &m_CreditText);
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"CREDIT2.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_Credit2Image, 0, &m_Credit2Text);
	D3DXCreateTextureFromFileEx(m_pD3DDevice, L"EXIT.tga", 0, 0, 0, 0,
                  D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, 
                  D3DX_DEFAULT, D3DCOLOR_XRGB(255, 0, 255), 
				  &m_ExitImage, 0, &m_ExitText);

	// multiple times, just call that sprite's Draw() with different 
	// transformation values.


	//Paddle 1
	Paddle[0].xp = -12;
	Paddle[0].yp = 300;

	//Paddle 2
	Paddle[1].xp = 800;
	Paddle[1].yp = 300;

	//Ball
	Ball.xp = 400;
	Ball.yp = 300;

	//	Wall / Background
	Wall.xp = 400;
	Wall.yp = 300;

	//	Menu
	Menu.xp = 200;
	Menu.yp = 200;
	



	//Initial Direction
	Ball.DIR_UP_RIGHT = true;

	//MENU
	Menu.onGAME = false;
	Menu.onSTART = true;
	

	//*************************************************************************

	// create direct input object
	DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8,(void **)&m_pDIObject, NULL);

	// Create Keyboard
	m_pDIObject->CreateDevice(GUID_SysKeyboard, &m_pDIKeyboard, NULL);

	//Set Keyboard data format
	m_pDIKeyboard->SetDataFormat(&c_dfDIKeyboard); 

	//Set Keyboard coop level
	m_pDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); 

	// Create Mouse
	m_pDIObject->CreateDevice(GUID_SysMouse, &m_pDIMouse, NULL);

	// Set Mouse Data Format
	m_pDIMouse->SetDataFormat(&c_dfDIMouse2);

	// Set Mouse Coop Level
	m_pDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);


	//SOUND INITIALIZATION
	channel = 0;
	result = FMOD::System_Create(&system);
	
	result = system->init(100, FMOD_INIT_NORMAL, 0); // initialize fmod

	result = system->createSound("beep1.ogg", FMOD_DEFAULT, 0, &mySound1);
	result = system->createSound("beep2.ogg", FMOD_DEFAULT, 0, &mySound2);
	result = system->createSound("pongMusic.wav", FMOD_LOOP_NORMAL | FMOD_2D, 0, &myStream);


//BACKGROUND MUSIC---------------------------------------------
		result = system->playSound(myStream, 0, true, &channel);
		result = channel->setVolume(0.5f);
		result = channel->setPaused(false);
//-------------------------------------------------------------
}

void CDirectXFramework::Update()
{
	Getinput();
	Pollinput();

	// get current keyboard
	hr = m_pDIKeyboard->GetDeviceState(sizeof(keyboardBuffer), (LPVOID)&keyboardBuffer);

	// get current mouse
//	hr = m_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);


	
}

static int Player1Point = 0;
static int Player2Point = 0;

void CDirectXFramework::Render()
{
	timeGetTime();
	RECT rect;
	// If the device was not created successfully, return
	if(!m_pD3DDevice)
	{
		return;
	}

	//*************************************************************************

	//////////////////////////////////////////////////////////////////////////
	// All draw calls between swap chain's functions, and pre-render and post- 
	// render functions (Clear and Present, BeginScene and EndScene)
	//////////////////////////////////////////////////////////////////////////
	
	// Clear the back buffer, call BeginScene()
	m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	m_pD3DDevice->BeginScene();
	system->update();
			//////////////////////////////////////////////////////////////////////////
			// Draw 3D Objects (for future labs - not used in Week #1)
			//////////////////////////////////////////////////////////////////////////

			//////////////////////////////////////////////////////////////////////////
			// Draw 2D sprites
			//////////////////////////////////////////////////////////////////////////
			// Note: You should only be calling the sprite object's begin and end once, 
			// with all draw calls of sprites between them

			// Call Sprite's Begin to start rendering 2D sprite objects
				m_pD3DSprite->Begin(NULL);
				//////////////////////////////////////////////////////////////////////////
				// Matrix Transformations to control sprite position, scale, and rotate
				// Set these matrices for each object you want to render to the screen
				//////////////////////////////////////////////////////////////////////////
				// Scaling
				// Rotation on Z axis, value in radians, converting from degrees
				// Translation
				// Multiply scale and rotation, store in scale
				// Multiply scale and translation, store in world
				// Set Transform

				D3DXMATRIX transMat, rotMat, scaleMat, worldMat;
				D3DXMatrixIdentity(&transMat);
				D3DXMatrixIdentity(&scaleMat);
				D3DXMatrixIdentity(&rotMat);
				D3DXMatrixIdentity(&worldMat);
				D3DXMatrixRotationZ(&rotMat, D3DXToRadian(0));
				D3DXMatrixRotationZ(&rotMat, D3DXToRadian(0));
				D3DXMatrixTranslation(&transMat, Menu.xp, Menu.yp, 0.0f);
				D3DXMatrixScaling(&scaleMat, 0.5f, 0.5f, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				m_pD3DSprite->SetTransform(&worldMat);

				if(Menu.onSTART == true)
				{
			    	m_pD3DSprite->Draw(m_StartText, 0, &D3DXVECTOR3(m_StartImage.Width * 0.5f, 
					m_StartImage.Height * 0.5f, 0.0f), 0,
					D3DCOLOR_ARGB(255, 255, 255, 255));

					if(controlDown & ARROW_DOWN)
					{
						Menu.onSTART = false;
						Menu.onCREDITS = true;
						
					}
					if(controlDown & ENTER_KEY)
					{
						Menu.onSTART = false;
						Menu.onMovie = true;
						if(Menu.onMovie = true)
							{
							m_pMediaControl->Run();

							long evCode;
							m_pMediaEvent->WaitForCompletion(INFINITE, &evCode);

							m_pMediaControl->Stop();
							Menu.onMovie = false;
							Menu.onGAME = true;
							}
						
					}
				}
				else if(Menu.onCREDITS == true)
				{
					m_pD3DSprite->Draw(m_CreditText, 0, &D3DXVECTOR3(m_CreditImage.Width * 0.5f, 
					m_CreditImage.Height * 0.5f, 0.0f), 0,
					D3DCOLOR_ARGB(255, 255, 255, 255));

					if(controlDown & ARROW_DOWN)
					{
						Menu.onEXIT = true;
						Menu.onCREDITS = false;
						
					}
					if(controlDown & ENTER_KEY)
					{
						Menu.onCREDITS = false;
						Menu.onCREDITS2 = true;
						
					}
					if(controlDown & ARROW_UP)
					{
						Menu.onCREDITS = false;
						Menu.onSTART = true;
					}

				}
				else if(Menu.onEXIT == true)
				{
					m_pD3DSprite->Draw(m_ExitText, 0, &D3DXVECTOR3(m_ExitImage.Width * 0.5f, 
					m_ExitImage.Height * 0.5f, 0.0f), 0,
					D3DCOLOR_ARGB(255, 255, 255, 255));

					if(controlDown & ARROW_UP)
					{
						Menu.onEXIT = false;
						Menu.onCREDITS = true;
					}
					if(controlDown & ENTER_KEY)
					{
						Menu.onCREDITS = false;
						Menu.onCREDITS2 = true;
						exit(0);	
					}
				}
				if(Menu.onCREDITS2 == true)
				{
					
					m_pD3DSprite->Draw(m_Credit2Text, 0, &D3DXVECTOR3(m_Credit2Image.Width * 0.5f, 
					m_Credit2Image.Height * 0.5f, 0.0f), 0,
					D3DCOLOR_ARGB(255, 255, 255, 255));

					if(controlDown & ARROW_LEFT)
					{
						Menu.onCREDITS2 = false;
						Menu.onCREDITS = true;
					}
				}
if(Menu.onGAME == true)
{
	

//BACKGROUND IMAGE
				D3DXMatrixRotationZ(&rotMat, D3DXToRadian(0));
				D3DXMatrixTranslation(&transMat, Wall.xp, Wall.yp, 0.0f);
				D3DXMatrixScaling(&scaleMat, 1, 1, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				// Set Transform for the object m_pD3DSprite
				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_wallTexture, 0, &D3DXVECTOR3(m_wallImage.Width * 0.5f, 
									m_wallImage.Height * 0.5f, 0.0f), 0,
								D3DCOLOR_ARGB(255, 255, 255, 255));

				for(int i = 0; i < 2; ++i)
				{
					if(i == 0)
					{
						if(controlActive & S_DOWN) 
						{
							Paddle[0].yp = Paddle[0].yp + .1f;
						}

						if(controlActive & W_UP) 
						{
							Paddle[0].yp = Paddle[0].yp - .1f;
						}

						//Out of Bounds

						if(Paddle[i].yp-60 <= 0)
						{
							Paddle[0].yp = 60;
						}

						if(Paddle[i].yp+60 >= 600)
						{
							Paddle[0].yp = 540;
						}
						D3DXMatrixTranslation(&transMat, Paddle[i].xp, Paddle[i].yp, 0.0f);


					}
					if(i == 1)
					{
						if(controlActive & ARROW_DOWN) 
						{
							Paddle[1].yp = Paddle[1].yp + .1f;
						}

						if(controlActive & ARROW_UP) 
						{
							Paddle[1].yp = Paddle[1].yp - .1f;
						}

						//Out of Bounds
						if(Paddle[i].yp-60 <= 0)
						{
							Paddle[1].yp = 60;
						}
						if(Paddle[i].yp+60 >= 600)
						{
							Paddle[1].yp = 540;
						}
						D3DXMatrixTranslation(&transMat, Paddle[i].xp, Paddle[i].yp, 0.0f);
					}
			
				D3DXMatrixScaling(&scaleMat, 1, 1, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				// Set Transform for the object m_pD3DSprite
				m_pD3DSprite->SetTransform(&worldMat);

				// Draw the texture with the sprite object

				m_pD3DSprite->Draw(m_pTexture, 0, &D3DXVECTOR3(m_imageInfo.Width * 0.5f, 
									m_imageInfo.Height * 0.5f, 0.0f), 0,
									D3DCOLOR_ARGB(255, 255, 255, 255));
				

				
//WALL COLLISION
					if(Ball.yp-20 <= 0)
					{
						if(Ball.DIR_UP_RIGHT == true)
						{
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =true;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=false;
						}
						else if(Ball.DIR_UP_LEFT == true)
						{
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=true;
							Ball.DIR_UP_LEFT	=false;
						}
					}

					if(Ball.yp+20 >= 600)
					{
						if(Ball.DIR_DOWN_RIGHT == true)
						{
							Ball.DIR_UP_RIGHT	=true;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=false;
						}
						else if(Ball.DIR_DOWN_LEFT == true)
						{
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=true;
						}
					}

//BALL OUT OF BOUNDS
					if(Ball.xp >= 800)
					{
						result = system->playSound(mySound2, 0, false, 0);
						Player1Point++;

						Ball.xp = 400;
						Ball.yp = 300;

						Ball.DIR_UP_RIGHT	=true;
						Ball.DIR_DOWN_RIGHT =false;
						Ball.DIR_DOWN_LEFT	=false;
						Ball.DIR_UP_LEFT	=false;
					}
					if(Ball.xp <= 0)
					{
						result = system->playSound(mySound2, 0, false, 0);

						Player2Point++;

						Ball.xp = 400;
						Ball.yp = 300;

						Ball.DIR_UP_RIGHT	=false;
						Ball.DIR_DOWN_RIGHT =false;
						Ball.DIR_DOWN_LEFT	=false;
						Ball.DIR_UP_LEFT	=true;
					}

//PADDLE COLLISION
					if(Ball.xp >= Paddle[1].xp - 30
						&& Ball.yp >= Paddle[1].yp - 60
						&& Ball.yp <= Paddle[1].yp + 60)
					{
						if(Ball.DIR_DOWN_RIGHT == true)
						{
							result = system->playSound(mySound1, 0, false, 0);
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=true;
							Ball.DIR_UP_LEFT	=false;
						}
						else if(Ball.DIR_UP_RIGHT == true)
						{
							result = system->playSound(mySound1, 0, false, 0);
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=true;
						}
					}

					if(Ball.xp <= Paddle[0].xp + 30
						&& Ball.yp >= Paddle[0].yp - 60
						&& Ball.yp <= Paddle[0].yp + 60)
					{
						if(Ball.DIR_DOWN_LEFT == true)
						{
							result = system->playSound(mySound1, 0, false, 0);
							Ball.DIR_UP_RIGHT	=false;
							Ball.DIR_DOWN_RIGHT =true;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=false;
						}
						else if(Ball.DIR_UP_LEFT == true)
						{
							result = system->playSound(mySound1, 0, false, 0);
							Ball.DIR_UP_RIGHT	=true;
							Ball.DIR_DOWN_RIGHT =false;
							Ball.DIR_DOWN_LEFT	=false;
							Ball.DIR_UP_LEFT	=false;
						}
					}


//BALL DIRECTION
					if(Ball.DIR_UP_RIGHT == true)
					{
						Ball.xp = Ball.xp + 0.03f;
						Ball.yp = Ball.yp - 0.05f;
					}
					if(Ball.DIR_DOWN_RIGHT == true)
					{
						Ball.xp = Ball.xp + 0.03f;
						Ball.yp = Ball.yp + 0.05f;
					}
					if(Ball.DIR_DOWN_LEFT == true)
					{
						Ball.xp = Ball.xp - 0.03f;
						Ball.yp = Ball.yp + 0.05f;
					}
					if(Ball.DIR_UP_LEFT == true)
					{
						Ball.xp = Ball.xp - 0.03f;
						Ball.yp = Ball.yp - 0.05f;
					}
				
				}
				
				D3DXMatrixRotationZ(&rotMat, D3DXToRadian(0));
				D3DXMatrixTranslation(&transMat, Ball.xp, Ball.yp, 0.0f);
				D3DXMatrixScaling(&scaleMat, 1, 1, 0.0f);
				D3DXMatrixMultiply(&scaleMat, &scaleMat, &rotMat);
				D3DXMatrixMultiply(&worldMat, &scaleMat, &transMat);
				// Set Transform for the object m_pD3DSprite
				m_pD3DSprite->SetTransform(&worldMat);

				m_pD3DSprite->Draw(m_ballTexture, 0, &D3DXVECTOR3(m_ballImage.Width * 0.5f, 
								m_ballImage.Height * 0.5f, 0.0f), 0,
								D3DCOLOR_ARGB(255, 255, 255, 255));
				
				// End drawing 2D sprites
}
				m_pD3DSprite->End();



			//////////////////////////////////////////////////////////////////////////
			// Draw Text
			//////////////////////////////////////////////////////////////////////////
			
			// Calculate RECT structure for text drawing placement, using whole screen
if(Menu.onGAME == true)
{
			rect.left = 10;
			rect.right = 10;
			rect.bottom = 10;
			rect.top = 10;

			wchar_t Player1Text[256];
			swprintf(Player1Text, 256, L"Point(s): %i", Player1Point);

			m_pD3DFont->DrawText(0, Player1Text, -1, &rect, 
                 DT_TOP | DT_LEFT | DT_NOCLIP, 
                 D3DCOLOR_ARGB(255, 255, 255, 255));

			rect.left = 670;
			rect.right = 10;
			rect.bottom = 10;
			rect.top = 10;

			wchar_t Player2Text[256];
			swprintf(Player2Text, 256, L"Point(s): %i", Player2Point);

			m_pD3DFont->DrawText(0, Player2Text, -1, &rect, 
                 DT_TOP | DT_LEFT | DT_NOCLIP, 
                 D3DCOLOR_ARGB(255, 255, 255, 255));
			
			// to be drawn outside of the rect specified to draw text in.

			// EndScene, and Present the back buffer to the display buffer
}
			m_pD3DDevice->EndScene();
			m_pD3DDevice->Present(NULL, NULL, NULL, NULL);


	//*************************************************************************

	//Sound System Update
	if(controlDown & SOUND1)
	{
		

	}

	if(controlDown & SOUND2)
	{
		
	}
	
}

void CDirectXFramework::Shutdown()
{
	
	
	//*************************************************************************
	// Release COM objects in the opposite order they were created in

	// Texture
	m_pTexture->Release();
	// Sprite
	m_pD3DSprite->Release();
	// Font
	m_pD3DFont->Release();
	// 3DDevice	
		
	// 3DObject

	// Sound
	system->release();

	//*************************************************************************
	m_pVideoWindow->put_Visible(OAFALSE);
	m_pVideoWindow->put_Owner((OAHWND)m_hWnd);
	m_pMediaControl->Release();
	m_pMediaEvent->Release();
	m_pGraphBuilder->Release(); // Should be AFTER the other calls!
	CoUninitialize();
	
}

void CDirectXFramework::Getinput()
{
	controlCurrent = 0;
	//Movement
	if(keyboardBuffer[DIK_UP] & 0x80) controlCurrent |= ARROW_UP;
	if(keyboardBuffer[DIK_DOWN] & 0x80) controlCurrent |= ARROW_DOWN;
	if(keyboardBuffer[DIK_W] & 0x80) controlCurrent |= W_UP;
	if(keyboardBuffer[DIK_S] & 0x80) controlCurrent |= S_DOWN;

	if(keyboardBuffer[DIK_LEFT] & 0x80) controlCurrent |= ARROW_LEFT;
	if(keyboardBuffer[DIK_RETURN] & 0x80) controlCurrent |= ENTER_KEY;

	//Mouse Keys
//	if(mouseState.rgbButtons[0] & 0x80)controlCurrent |= SHRINK;
//	if(mouseState.rgbButtons[1] & 0x80)controlCurrent |= STRETCH;

	//Sound Keys
//	if(keyboardBuffer[DIK_1] & 0x80) controlCurrent |= SOUND1;
//	if(keyboardBuffer[DIK_2] & 0x80) controlCurrent |= SOUND2;


	controlActive = controlCurrent;
	controlDown = (controlCurrent ^ controlPrevious) & controlCurrent;
	controlUp = (controlCurrent ^ controlPrevious) & controlPrevious;

	controlPrevious = controlCurrent;


}

void CDirectXFramework::Pollinput()
{
		// Poll keyboard
		hr = m_pDIKeyboard->GetDeviceState(sizeof(keyboardBuffer), (void**)&keyboardBuffer);

		if( FAILED(hr) )
		{
			//Keyboard lost, zero out keyboard data stucture.
			ZeroMemory(keyboardBuffer, sizeof(keyboardBuffer));
			// Try to acquire for next time we poll
			hr = m_pDIKeyboard->Acquire();
		}

		// Poll Mouse
		hr = m_pDIMouse->GetDeviceState(sizeof(mouseState), (void**)&mouseState);

		if( FAILED(hr) )
		{
			//Keyboard lost, zero out keyboard data stucture.
			ZeroMemory(&mouseState, sizeof(mouseState));
			// Try to acquire for next time we poll
			hr = m_pDIMouse->Acquire();
		}
}
