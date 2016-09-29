/************************************************************************************
Filename    :   Win32_RoomTiny_Main.cpp
Content     :   First-person view test application for Oculus Rift
Created     :   11th May 2015
Authors     :   Tom Heath
Copyright   :   Copyright 2015 Oculus, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*************************************************************************************/
/// This is an entry-level sample, showing a minimal VR sample, 
/// in a simple environment.  Use WASD keys to move around, and cursor keys.
/// Dismiss the health and safety warning by tapping the headset, 
/// or pressing any key. 
/// It runs with DirectX11.

// Include DirectX
#include "Win32_DirectXAppUtil.h"

// Include the Oculus SDK
#include "OVR_CAPI_D3D.h"

// OpenCV
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/core/directx.hpp>
#include <opencv2/core/ocl.hpp>

struct Prostor
{
	static const int MAX_MODELS = 100;
	Model *Models[MAX_MODELS];
	int numModels;

	void Add(Model * n)
	{
		if (numModels < MAX_MODELS)
			Models[numModels++] = n;
	}

	void Render(XMMATRIX * projView, float R, float G, float B, float A, bool standardUniforms)
	{
		for (int i = 0; i < numModels; ++i)
			Models[i]->Render(projView, R, G, B, A, standardUniforms);
	}

	void RenderInstanced(XMMATRIX * projViews, float R, float G, float B, float A, bool standardUniforms)
	{
		for (int i = 0; i < numModels; ++i)
			Models[i]->RenderInstanced(projViews, R, G, B, A, standardUniforms);
	}

	void Init(int mode)
	{
		TriangleSet cube;
		cube.AddSolidColorBox(0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0xff00ff00);
		Add(
			new Model(&cube, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1),
				new Material(
					new Texture(false, 256, 256, Texture::AUTO_CEILING)
				)
			)
		);

		//D3D11_TEXTURE2D_DESC desc;
		//desc.Width = 256;
		//desc.Height = 256;
		//desc.MipLevels = desc.ArraySize = 1;
		//desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//desc.SampleDesc.Count = 1;
		//desc.Usage = D3D11_USAGE_DYNAMIC;
		//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		//desc.MiscFlags = 0;

		//ID3D11Device *pd3dDevice = DIRECTX.Device; // Don't forget to initialize this
		//ID3D11Texture2D *pTexture = NULL;
		//pd3dDevice->CreateTexture2D(&desc, NULL, &pTexture);

		TriangleSet screen;
		float w = 40.0f;
		float h = 30.0f;
		float d = 60.0f;
		float b = 0.1f;
		//screen.AddSolidColorBox(-w/2, -h/2, -d, w/2, h/2, -d-b, mode==1 ? 0xffff0000 : 0xff0000ff);
		float z1 = -30;
		float z2 = z1;
		float x1 = -10;
		float x2 = 10;
		float y1 = -10;
		float y2 = 10;

		int size = 512;
		uint32_t c = (mode == 1 ? 0xffffffff : 0xffffffff);
		screen.AddQuad(
			Vertex(XMFLOAT3(x2, y1, z2), c, 1, 1),
			Vertex(XMFLOAT3(x1, y1, z2), c, 0, 1),
			Vertex(XMFLOAT3(x2, y2, z2), c, 1, 0),
			Vertex(XMFLOAT3(x1, y2, z2), c, 0, 0));
		Add(
			new Model(&screen, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1),
				new Material(
					new Texture(false, size, size, Texture::AUTO_WHITE)
				)
			)
		);

		/*uint32_t * texData = new uint32_t[size * size];

		if (mode)
		{

		for (int i = 0; i < size * size; i++) texData[i] = 0xFF000000 | (i % 256);
		Models[1]->Fill->Tex->FillTexture(texData);
		}
		delete[] texData;*/
	}

	Prostor() : numModels(0) {}
	Prostor(int mode) :
		numModels(0)
	{
		Init(mode);
	}
	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];
	}
	~Prostor()
	{
		Release();
	}
};

//------------------------------------------------------------
// ovrSwapTextureSet wrapper class that also maintains the render target views
// needed for D3D11 rendering.
struct OculusTexture
{
	ovrSession               Session;
	ovrTextureSwapChain      TextureChain;
	std::vector<ID3D11RenderTargetView*> TexRtv;

	OculusTexture() :
		Session(nullptr),
		TextureChain(nullptr)
	{
	}

	bool Init(ovrSession session, int sizeW, int sizeH)
	{
		Session = session;

		ovrTextureSwapChainDesc desc = {};
		desc.Type = ovrTexture_2D;
		desc.ArraySize = 1;
		desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
		desc.Width = sizeW;
		desc.Height = sizeH;
		desc.MipLevels = 1;
		desc.SampleCount = 1;
		desc.MiscFlags = ovrTextureMisc_DX_Typeless;
		desc.BindFlags = ovrTextureBind_DX_RenderTarget;
		desc.StaticImage = ovrFalse;

		ovrResult result = ovr_CreateTextureSwapChainDX(session, DIRECTX.Device, &desc, &TextureChain);
		if (!OVR_SUCCESS(result))
			return false;

		int textureCount = 0;
		ovr_GetTextureSwapChainLength(Session, TextureChain, &textureCount);
		for (int i = 0; i < textureCount; ++i)
		{
			ID3D11Texture2D* tex = nullptr;
			ovr_GetTextureSwapChainBufferDX(Session, TextureChain, i, IID_PPV_ARGS(&tex));
			D3D11_RENDER_TARGET_VIEW_DESC rtvd = {};
			rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			ID3D11RenderTargetView* rtv;
			DIRECTX.Device->CreateRenderTargetView(tex, &rtvd, &rtv);
			//			cv::Mat slika;
			//			cv::directx::convertFromD3D11Texture2D(tex, slika);
			TexRtv.push_back(rtv);
			tex->Release();
		}

		return true;
	}

	~OculusTexture()
	{
		for (int i = 0; i < (int)TexRtv.size(); ++i)
		{
			Release(TexRtv[i]);
		}
		if (TextureChain)
		{
			ovr_DestroyTextureSwapChain(Session, TextureChain);
		}
	}

	ID3D11RenderTargetView* GetRTV()
	{
		int index = 0;
		ovr_GetTextureSwapChainCurrentIndex(Session, TextureChain, &index);
		return TexRtv[index];
	}

	// Commit changes
	void Commit()
	{
		ovr_CommitTextureSwapChain(Session, TextureChain);
	}
};

// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
	// Initialize these to nullptr here to handle device lost failures cleanly
	ovrMirrorTexture mirrorTexture = nullptr;
	OculusTexture  * pEyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer    * pEyeDepthBuffer[2] = { nullptr, nullptr };
	Prostor          * roomScene = nullptr;
	Prostor          * roomScene2 = nullptr;
	Camera         * mainCam = nullptr;
	ovrMirrorTextureDesc mirrorDesc = {};
	long long frameIndex = 0;

	int size = 512;
	int width, height;
	uint32_t * texDataL = new uint32_t[size * size];
	uint32_t * texDataR = new uint32_t[size * size];

	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result))
		return retryCreate;



	cv::VideoCapture cap("udpsrc port=6000 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink");
	if (!cap.isOpened()) {
		FATALERROR("Failed to open video stream.");
	}
	cv::VideoCapture capR("udpsrc port=6001 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink");
	if (!capR.isOpened()) {
		FATALERROR("Failed to open video stream.");
	}

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

	// Setup Device and Graphics
	// Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution. Changed ...
	if (!DIRECTX.InitDevice(hmdDesc.Resolution.w, hmdDesc.Resolution.h, reinterpret_cast<LUID*>(&luid)))
		goto Done;

	//cv::directx::ocl::initializeContextFromD3D11Device(DIRECTX.Device);

	// Make the eye render buffers (caution if actual size < requested due to HW limits). 
	ovrRecti         eyeRenderViewport[2];

	for (int eye = 0; eye < 2; ++eye)
	{
		ovrSizei idealSize = ovr_GetFovTextureSize(session, (ovrEyeType)eye, hmdDesc.DefaultEyeFov[eye], 1.0f);
		pEyeRenderTexture[eye] = new OculusTexture();
		if (!pEyeRenderTexture[eye]->Init(session, idealSize.w, idealSize.h))
		{
			if (retryCreate) goto Done;
			FATALERROR("Failed to create eye texture.");
		}
		pEyeDepthBuffer[eye] = new DepthBuffer(DIRECTX.Device, idealSize.w, idealSize.h);
		eyeRenderViewport[eye].Pos.x = 0;
		eyeRenderViewport[eye].Pos.y = 0;
		eyeRenderViewport[eye].Size = idealSize;
		if (!pEyeRenderTexture[eye]->TextureChain)
		{
			if (retryCreate) goto Done;
			FATALERROR("Failed to create texture.");
		}
	}

	// Create a mirror to see on the monitor.
	mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	mirrorDesc.Width = DIRECTX.WinSizeW;
	mirrorDesc.Height = DIRECTX.WinSizeH;
	result = ovr_CreateMirrorTextureDX(session, DIRECTX.Device, &mirrorDesc, &mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		if (retryCreate) goto Done;
		FATALERROR("Failed to create mirror texture.");
	}

	// Create the room model
	//roomScene = new Scene(false);
	roomScene = new Prostor(0);
	roomScene2 = new Prostor(1);

	// Create camera
	mainCam = new Camera(XMVectorSet(0.0f, 0.0f, 5.0f, 0), XMQuaternionIdentity());

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	// Main loop
	while (DIRECTX.HandleMessages())
	{
		cv::Mat frame;
		capR >> frame;
		cv::Rect myROI(0, 0, size, size);
		cv::Mat image = frame(myROI);
		image = image.clone();

		cv::Mat frameR;
		cap >> frameR;
		cv::Mat imageR = frameR(myROI);
		imageR = imageR.clone();

		ovrSessionStatus sessionStatus;
		ovr_GetSessionStatus(session, &sessionStatus);
		if (sessionStatus.ShouldQuit)
		{
			// Because the application is requested to quit, should not request retry
			retryCreate = false;
			break;
		}
		if (sessionStatus.ShouldRecenter)
			ovr_RecenterTrackingOrigin(session);

		if (sessionStatus.IsVisible)
		{
			XMVECTOR forward = XMVector3Rotate(XMVectorSet(0, 0, -0.05f, 0), mainCam->Rot);
			XMVECTOR right = XMVector3Rotate(XMVectorSet(0.05f, 0, 0, 0), mainCam->Rot);
			if (DIRECTX.Key['W'] || DIRECTX.Key[VK_UP])      mainCam->Pos = XMVectorAdd(mainCam->Pos, forward);
			if (DIRECTX.Key['S'] || DIRECTX.Key[VK_DOWN])    mainCam->Pos = XMVectorSubtract(mainCam->Pos, forward);
			if (DIRECTX.Key['D'])                            mainCam->Pos = XMVectorAdd(mainCam->Pos, right);
			if (DIRECTX.Key['A'])                            mainCam->Pos = XMVectorSubtract(mainCam->Pos, right);
			static float Yaw = 0;
			if (DIRECTX.Key[VK_LEFT])  mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw += 0.02f, 0);
			if (DIRECTX.Key[VK_RIGHT]) mainCam->Rot = XMQuaternionRotationRollPitchYaw(0, Yaw -= 0.02f, 0);

			// Animate the cube
			static float cubeClock = 0;
			roomScene->Models[0]->Pos = XMFLOAT3(9 * sin(cubeClock), 3, 9 * cos(cubeClock));
			roomScene2->Models[0]->Pos = XMFLOAT3(9 * sin(cubeClock), 3, 9 * cos(cubeClock));
			cubeClock += 0.015f;

			// Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyeOffset) may change at runtime.
			ovrEyeRenderDesc eyeRenderDesc[2];
			eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
			eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

			// Get both eye poses simultaneously, with IPD offset already included. 
			ovrPosef         EyeRenderPose[2];
			ovrVector3f      HmdToEyeOffset[2] = { eyeRenderDesc[0].HmdToEyeOffset,
				eyeRenderDesc[1].HmdToEyeOffset };

			double sensorSampleTime;    // sensorSampleTime is fed into the layer later
			ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyeOffset, EyeRenderPose, &sensorSampleTime);

			// Render Scene to Eye Buffers
			for (int eye = 0; eye < 2; ++eye)
			{
				// Clear and set up rendertarget
				DIRECTX.SetAndClearRenderTarget(pEyeRenderTexture[eye]->GetRTV(), pEyeDepthBuffer[eye]);
				DIRECTX.SetViewport((float)eyeRenderViewport[eye].Pos.x, (float)eyeRenderViewport[eye].Pos.y,
					(float)eyeRenderViewport[eye].Size.w, (float)eyeRenderViewport[eye].Size.h);

				//Get the pose information in XM format
				XMVECTOR eyeQuat = XMVectorSet(EyeRenderPose[eye].Orientation.x, EyeRenderPose[eye].Orientation.y,
					EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w);
				XMVECTOR eyePos = XMVectorSet(EyeRenderPose[eye].Position.x, EyeRenderPose[eye].Position.y, EyeRenderPose[eye].Position.z, 0);

				//				std::cout << EyeRenderPose[eye].Position.x << " " << EyeRenderPose[eye].Position.y << " " << EyeRenderPose[eye].Position.z << " (" << EyeRenderPose[eye].Orientation.x << " " << EyeRenderPose[eye].Orientation.y << " "  << EyeRenderPose[eye].Orientation.z << " " << EyeRenderPose[eye].Orientation.w << std::endl;

				// Get view and projection matrices for the Rift camera
				XMVECTOR CombinedPos = XMVectorAdd(mainCam->Pos, XMVector3Rotate(eyePos, mainCam->Rot));
				Camera finalCam(CombinedPos, XMQuaternionMultiply(eyeQuat, mainCam->Rot));
				XMMATRIX view = finalCam.GetViewMatrix();
				ovrMatrix4f p = ovrMatrix4f_Projection(eyeRenderDesc[eye].Fov, 0.2f, 1000.0f, ovrProjection_None);
				XMMATRIX proj = XMMatrixSet(p.M[0][0], p.M[1][0], p.M[2][0], p.M[3][0],
					p.M[0][1], p.M[1][1], p.M[2][1], p.M[3][1],
					p.M[0][2], p.M[1][2], p.M[2][2], p.M[3][2],
					p.M[0][3], p.M[1][3], p.M[2][3], p.M[3][3]);
				XMMATRIX prod = XMMatrixMultiply(view, proj);


				// Move the screen
				XMFLOAT3 CombinedPosF;
				XMStoreFloat3(&CombinedPosF, CombinedPos);
				XMVECTOR CombinedRot = XMQuaternionMultiply(eyeQuat, mainCam->Rot);
				XMFLOAT4 CombinedRotF;
				XMStoreFloat4(&CombinedRotF, CombinedRot);
				if (eye == 0) {
					roomScene->Models[1]->Pos = CombinedPosF;
					roomScene->Models[1]->Rot = CombinedRotF;
				}
				else {
					roomScene2->Models[1]->Pos = CombinedPosF;
					roomScene2->Models[1]->Rot = CombinedRotF;
				}


				//cv::Mat img;
				//cv::cvtColor(image, img, cv::COLOR_BGR2RGBA, 4);
				uint8_t * ptr1, *ptr2;

				ptr1 = (uint8_t*)texDataL;
				ptr2 = image.data;
				for (int i = 0; i < size * size; i++)
				{
					//memcpy(ptr1, ptr2, 3);
					*(uint32_t*)ptr1 = *(uint32_t*)ptr2;
					ptr1[3] = 0xFF;
					ptr1 += 4;
					ptr2 += 3;
				}

				ptr1 = (uint8_t*)texDataR;
				ptr2 = imageR.data;
				for (int i = 0; i < size * size; i++)
				{
					//memcpy(ptr1, ptr2, 3);
					*(uint32_t*)ptr1 = *(uint32_t*)ptr2;
					ptr1[3] = 0xFF;
					ptr1 += 4;
					ptr2 += 3;
				}

				//for (int i = 0; i < size * size; i++) texDataR[i] = 0xFF000000 | (rand()*20 % 256);
				//for (int i = 0; i < size * size; i++) texDataL[i] = 0xFF000000 | (i % 256) | (i & 0xFF00);
				/*if (image.isContinuous()) {
				roomScene2->Models[1]->Fill->Tex->FillTexture((uint32_t*)image.data);
				}
				roomScene->Models[1]->Fill->Tex->FillTexture(texDataL);
				*/

				roomScene2->Models[1]->Fill->Tex->FillTexture(texDataL);
				roomScene->Models[1]->Fill->Tex->FillTexture(texDataR);

				if (eye == 0)
					roomScene->Render(&prod, 1, 1, 1, 1, true);
				else
					roomScene2->Render(&prod, 1, 1, 1, 1, true);

				// Commit rendering to the swap chain
				pEyeRenderTexture[eye]->Commit();
			}

			// Initialize our single full screen Fov layer.
			ovrLayerEyeFov ld = {};
			ld.Header.Type = ovrLayerType_EyeFov;
			ld.Header.Flags = 0;

			for (int eye = 0; eye < 2; ++eye)
			{
				ld.ColorTexture[eye] = pEyeRenderTexture[eye]->TextureChain;
				ld.Viewport[eye] = eyeRenderViewport[eye];
				ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
				ld.RenderPose[eye] = EyeRenderPose[eye];
				ld.SensorSampleTime = sensorSampleTime;
			}

			ovrLayerHeader* layers = &ld.Header;
			result = ovr_SubmitFrame(session, frameIndex, nullptr, &layers, 1);
			// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
			if (!OVR_SUCCESS(result))
				goto Done;

			frameIndex++;
		}

		// Render mirror
		ID3D11Texture2D* tex = nullptr;
		ovr_GetMirrorTextureBufferDX(session, mirrorTexture, IID_PPV_ARGS(&tex));
		DIRECTX.Context->CopyResource(DIRECTX.BackBuffer, tex);
		//cv::directx::convertToD3D11Texture2D(frame, tex);
		tex->Release();
		//		cv::Mat slika;
		//		cv::directx::convertFromD3D11Texture2D(tex, slika);
		DIRECTX.SwapChain->Present(0, 0);

	}

	// Release resources
Done:
	delete mainCam;
	delete roomScene;
	if (mirrorTexture)
		ovr_DestroyMirrorTexture(session, mirrorTexture);
	for (int eye = 0; eye < 2; ++eye)
	{
		delete pEyeRenderTexture[eye];
		delete pEyeDepthBuffer[eye];
	}
	DIRECTX.ReleaseDevice();
	ovr_Destroy(session);

	delete[] texDataL;
	delete[] texDataR;

	// Retry on ovrError_DisplayLost
	return retryCreate || (result == ovrError_DisplayLost);
}

//-------------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE, LPSTR, int)
{
	// Initializes LibOVR, and the Rift
	ovrResult result = ovr_Initialize(nullptr);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(DIRECTX.InitWindow(hinst, L"Oculus Room Tiny (DX11)"), "Failed to open window.");

	DIRECTX.Run(MainLoop);

	ovr_Shutdown();
	return(0);
}

////////////////////
//
//char udp_buffer[512] = { 0 };
//
//int nogavica = connect(25564);
//uint32_t add = 0;
//uint8_t *tmp = (uint8_t*)&add;
//tmp[0] = 127;
//tmp[1] = 0;
//tmp[2] = 0;
//tmp[3] = 1;
//sendData(add, nogavica, "neki", 4, 25565);
//
//while (1)
//{
//	int newData = receive(nogavica, udp_buffer, 512);
//
//	if (newData)
//	{
//		CString tmp(udp_buffer);
//
//		if (tmp == "Exit")
//		{
//			exit(0);
//		}
//		/*
//		// Data received here...
//		if (strcmp(udp_buffer, "Exit") == 0)
//		{
//		exit(0);
//		}
//		*/
//		// Clear the buffer
//		memset(udp_buffer, 0, sizeof(udp_buffer));
//	}
//
//
//
//
//
//
//}
//disconnect(nogavica);
//TerminateWinsock();
//
/////////////////////