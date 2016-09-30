// Include DirectX
#include "../thirdparty/Win32_DirectXAppUtil.h"

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

#include "render.h"
#include <Windows.h>

//#define ENABLE_STREAM

struct SingleEyeScene
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

		TriangleSet screen;
		float w = 40.0f;
		float h = 30.0f;
		float d = 60.0f;
		float b = 0.1f;
		float z1 = -10;
		float z2 = z1;
		float x1 = -10;
		float x2 = 10;
		float y1 = -10;
		float y2 = 10;

		int size = 1024;
		uint32_t c = (mode == 1 ? 0xffffffff : 0xffffffff);
		screen.AddQuad(
			Vertex(XMFLOAT3(x2, y1, z2), c, 1, 1),
			Vertex(XMFLOAT3(x1, y1, z2), c, 0, 1),
			Vertex(XMFLOAT3(x2, y2, z2), c, 1, 0),
			Vertex(XMFLOAT3(x1, y2, z2), c, 0, 0));
		Add(
			new Model(&screen, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1),
				new Material(
					//new Texture(false, 1296, 972, Texture::AUTO_WHITE)
					new Texture(1296, 972, false)
				)
			)
		);
		//Models[0]->Fill->Tex->MipLevels = 1;
	}

	SingleEyeScene() : numModels(0) {}
	SingleEyeScene(int mode) :
		numModels(0)
	{
		Init(mode);
	}
	void Release()
	{
		while (numModels-- > 0)
			delete Models[numModels];
	}
	~SingleEyeScene()
	{
		Release();
	}
};

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

	void Commit()
	{
		ovr_CommitTextureSwapChain(Session, TextureChain);
	}
};

static Render* render = nullptr;

// return true to retry later (e.g. after display lost)
static bool MainLoop(bool retryCreate)
{
	// Initialize these to nullptr here to handle device lost failures cleanly
	ovrMirrorTexture mirrorTexture = nullptr;
	OculusTexture  * pEyeRenderTexture[2] = { nullptr, nullptr };
	DepthBuffer    * pEyeDepthBuffer[2] = { nullptr, nullptr };
	SingleEyeScene          * roomScene = nullptr;
	SingleEyeScene          * roomScene2 = nullptr;
	Camera         * mainCam = nullptr;
	ovrMirrorTextureDesc mirrorDesc = {};
	long long frameIndex = 0;

	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result))
		return retryCreate;

	int size = 512;
	int width, height;
	uint32_t * texDataL = new uint32_t[size * size];
	uint32_t * texDataR = new uint32_t[size * size];

#ifdef ENABLE_STREAM
	cv::VideoCapture cap("udpsrc port=6000 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink");
	if (!cap.isOpened()) {
		FATALERROR("Failed to open video stream.");
	}
	cv::VideoCapture capR("udpsrc port=6001 ! application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink");
	if (!capR.isOpened()) {
		FATALERROR("Failed to open video stream.");
	}
#endif

	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

	if (!DIRECTX.InitDevice(hmdDesc.Resolution.w, hmdDesc.Resolution.h, reinterpret_cast<LUID*>(&luid)))
		goto Done;

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

	roomScene = new SingleEyeScene(0);
	roomScene2 = new SingleEyeScene(1);

	// Create camera
	mainCam = new Camera(XMVectorSet(0.0f, 0.0f, 5.0f, 0), XMQuaternionIdentity());

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	// Main loop
	while (DIRECTX.HandleMessages())
	{
		/*
		cv::Mat frameL;
		cv::Mat frameR;
		*(render->_leftCameraStream) >> frameL;
		*(render->_rightCameraStream) >> frameR;*/
		//std::stringstream ss;
		//ss << "w=" << frame.cols << " h=" << frame.rows << "\n";
		//OutputDebugString(ss.str().c_str());

		/*
		if (frame.rows != 0 && frame.rows != 0)
		{
			if (roomScene->Models[0]->Fill->Tex->SizeH != frame.rows ||
				roomScene->Models[0]->Fill->Tex->SizeW != frame.cols)
			{
				roomScene->Models[0]->Fill->Tex = new Texture(false, frame.cols, frame.rows, Texture::AUTO_WHITE);
				roomScene2->Models[0]->Fill->Tex = new Texture(false, frame.cols, frame.rows, Texture::AUTO_WHITE);
			}
		}	*/	
/*
		if (frame.rows >= size && frame.cols >= size) {
			cv::Rect myROI(0, 0, size, size);
			cv::Mat image = frame(myROI);
			image = image.clone();
		}*/
#ifdef ENABLE_STREAM
		cv::Mat frame;
		capR >> frame;
		cv::Rect myROI(0, 0, size, size);
		cv::Mat image = frame(myROI);
		image = image.clone();

		cv::Mat frameR;
		cap >> frameR;
		cv::Mat imageR = frameR(myROI);
		imageR = imageR.clone();
#endif

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
					roomScene->Models[0]->Pos = CombinedPosF;
					roomScene->Models[0]->Rot = CombinedRotF;
				}
				else {
					roomScene2->Models[0]->Pos = CombinedPosF;
					roomScene2->Models[0]->Rot = CombinedRotF;
				}

#ifdef ENABLE_STREAM
				uint8_t * ptr1, *ptr2;

				ptr1 = (uint8_t*)texDataL;
				ptr2 = image.data;
				for (int i = 0; i < size * size; i++)
				{
					*(uint32_t*)ptr1 = *(uint32_t*)ptr2;
					ptr1[3] = 0xFF;
					ptr1 += 4;
					ptr2 += 3;
				}

				ptr1 = (uint8_t*)texDataR;
				ptr2 = imageR.data;
				for (int i = 0; i < size * size; i++)
				{
					*(uint32_t*)ptr1 = *(uint32_t*)ptr2;
					ptr1[3] = 0xFF;
					ptr1 += 4;
					ptr2 += 3;
				}

				roomScene2->Models[0]->Fill->Tex->FillTexture(texDataL);
				roomScene->Models[0]->Fill->Tex->FillTexture(texDataR);
#endif
				if (render->_leftCameraStream->isNewFrame())
				{
					cv::Mat frameL;
					*(render->_leftCameraStream) >> frameL;
					if (frameL.rows >= size && frameL.cols >= size)
					{
						uint32_t * texDataPtr = (uint32_t *)frameL.data;
						roomScene->Models[0]->Fill->Tex->FillTexture(texDataPtr);
					}
				}
				if (render->_rightCameraStream->isNewFrame())
				{
					cv::Mat frameR;
					*(render->_rightCameraStream) >> frameR;

					if (frameR.rows >= size && frameR.cols >= size)
					{
						uint32_t * texDataPtr = (uint32_t *)frameR.data;
						roomScene2->Models[0]->Fill->Tex->FillTexture(texDataPtr);
					}
				}

				if (eye == 0)
					roomScene->Render(&prod, 1, 1, 1, 1, true);
				else
					roomScene2->Render(&prod, 1, 1, 1, 1, true);

				pEyeRenderTexture[eye]->Commit();
			}

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
			if (!OVR_SUCCESS(result))
				goto Done;

			frameIndex++;
		}

		ID3D11Texture2D* tex = nullptr;
		ovr_GetMirrorTextureBufferDX(session, mirrorTexture, IID_PPV_ARGS(&tex));
		DIRECTX.Context->CopyResource(DIRECTX.BackBuffer, tex);
		tex->Release();
		DIRECTX.SwapChain->Present(0, 0);

	}

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

	return retryCreate || (result == ovrError_DisplayLost);
}

void Render::start(HINSTANCE hinst) {
	render = this;
	ovrResult result = ovr_Initialize(nullptr);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(DIRECTX.InitWindow(hinst, L"Oculus Room Tiny (DX11)"), "Failed to open window.");

	DIRECTX.Run(MainLoop);

	ovr_Shutdown();
}

void Render::setLeftCameraStream(CameraStream* leftCameraStream)
{
	_leftCameraStream = leftCameraStream;
}

void Render::setRightCameraStream(CameraStream* rightCameraStream)
{
	_rightCameraStream = rightCameraStream;
}
