// Include DirectX
#include "../thirdparty/Win32_DirectXAppUtil.h"

// Include the Oculus SDK
#include "OVR_CAPI_D3D.h"

#include <OVR_Math.h>

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

const double pi = std::acos(-1);

struct SingleEyeScene
{
	static const int DEFAULT_WIDTH;
	static const int DEFAULT_HEIGHT;
	static const float DEFAULT_Z;
	static const float DEFAULT_FOV;
	static const uint32_t DEFAULT_COLOUR;

	static const int MAX_MODELS = 100;
	Model *cameraScreen;
	Model *Models[MAX_MODELS];
	int numModels;

	void Add(Model * n)
	{
		if (numModels < MAX_MODELS)
			Models[numModels++] = n;
	}

	void Render(XMMATRIX * projView, float R, float G, float B, float A, bool standardUniforms)
	{
		cameraScreen->Render(projView, R, G, B, A, standardUniforms);
		for (int i = 0; i < numModels; ++i)
			Models[i]->Render(projView, R, G, B, A, standardUniforms);
	}

	void RenderInstanced(XMMATRIX * projViews, float R, float G, float B, float A, bool standardUniforms)
	{
		cameraScreen->RenderInstanced(projViews, R, G, B, A, standardUniforms);
		for (int i = 0; i < numModels; ++i)
			Models[i]->RenderInstanced(projViews, R, G, B, A, standardUniforms);
	}

	void Init()
	{
		prepareCameraScreen(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_Z, DEFAULT_FOV, 0xffffffff);

		TriangleSet cube;
		cube.AddSolidColorBox(0.5f, -0.5f, 0.5f, -0.5f, 0.5f, -0.5f, 0xff00ff00);
		Add(
			new Model(&cube, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1),
				new Material(
					new Texture(false, 256, 256, Texture::AUTO_CEILING)
				)
			)
		);
	}

	void prepareCameraScreen(int width, int height, float z, float fov, uint32_t colour)
	{
		TriangleSet screen;
		float ratio = (float)height / width;
		float length = z*2.0*std::tan(fov / 2.0);
		float x1 = -length / 2;
		float x2 = length / 2;
		float y1 = x1 * ratio;
		float y2 = x2 * ratio;
		screen.AddQuad(
			Vertex(XMFLOAT3(x2, y1, -z), colour, 1, 1),
			Vertex(XMFLOAT3(x1, y1, -z), colour, 0, 1),
			Vertex(XMFLOAT3(x2, y2, -z), colour, 1, 0),
			Vertex(XMFLOAT3(x1, y2, -z), colour, 0, 0));
		delete cameraScreen;
		cameraScreen = new Model(&screen, XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 0, 1),
			new Material(
				new Texture(width, height, false, 1, 1, true)
			)
		);
	}

	SingleEyeScene() :
		numModels(0),
		cameraScreen(nullptr)
	{}
	SingleEyeScene(bool init) :
		numModels(0),
		cameraScreen(nullptr)
	{
		if ( init )
			Init();
	}
	void Release()
	{
		delete cameraScreen;
		while (numModels-- > 0)
			delete Models[numModels];
	}
	~SingleEyeScene()
	{
		Release();
	}
};

const int SingleEyeScene::DEFAULT_WIDTH = 1296;
const int SingleEyeScene::DEFAULT_HEIGHT = 972;
const float SingleEyeScene::DEFAULT_Z = 50.0f;
const float SingleEyeScene::DEFAULT_FOV = 54.0f*pi / 180.0f;
const uint32_t SingleEyeScene::DEFAULT_COLOUR = 0xffffffff;

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
	SingleEyeScene          * leftEyeScene = nullptr;
	SingleEyeScene          * rightEyeScene = nullptr;
	Camera         * mainCam = nullptr;
	ovrMirrorTextureDesc mirrorDesc = {};
	long long frameIndex = 0;

	ovrSession session;
	ovrGraphicsLuid luid;
	ovrResult result = ovr_Create(&session, &luid);
	if (!OVR_SUCCESS(result))
		return retryCreate;
	
	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

	const int scale = 2;
	if (!DIRECTX.InitDevice(hmdDesc.Resolution.w / scale, hmdDesc.Resolution.h / scale, reinterpret_cast<LUID*>(&luid)))
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

	leftEyeScene  = new SingleEyeScene(true);
	rightEyeScene = new SingleEyeScene(true);

	// Create camera
	mainCam = new Camera(XMVectorSet(0.0f, 0.0f, 5.0f, 0), XMQuaternionIdentity());

	// FloorLevel will give tracking poses where the floor height is 0
	ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

	// Main loop
	while (DIRECTX.HandleMessages())
	{
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
				CameraStream* cameraStream;
				SingleEyeScene* scene;
				if (eye == 0) {
					cameraStream = render->leftCameraStream;
					scene = leftEyeScene;
				}
				else {
					cameraStream = render->rightCameraStream;
					scene = rightEyeScene;
				}

				// Animate the cube
				static float cubeClock = 0;
				scene->Models[0]->Pos = XMFLOAT3(9 * sin(cubeClock), 3, 9 * cos(cubeClock));
				cubeClock += 0.005f;

				// Clear and set up rendertarget
				DIRECTX.SetAndClearRenderTarget(pEyeRenderTexture[eye]->GetRTV(), pEyeDepthBuffer[eye]);
				DIRECTX.SetViewport((float)eyeRenderViewport[eye].Pos.x, (float)eyeRenderViewport[eye].Pos.y,
					(float)eyeRenderViewport[eye].Size.w, (float)eyeRenderViewport[eye].Size.h);

				//Get the pose information in XM format
				XMVECTOR eyeQuat = XMVectorSet(EyeRenderPose[eye].Orientation.x, EyeRenderPose[eye].Orientation.y,
					EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w);
				XMVECTOR eyePos = XMVectorSet(EyeRenderPose[eye].Position.x, EyeRenderPose[eye].Position.y, EyeRenderPose[eye].Position.z, 0);

				// Query the HMD for ts current tracking state.
				ovrTrackingState ts = ovr_GetTrackingState(session, ovr_GetTimeInSeconds(), ovrTrue);
				if ( ts.StatusFlags & ovrStatus_OrientationTracked ) {
					ovrQuatf quat = ts.HeadPose.ThePose.Orientation;
					OVR::Quatf q(quat);
					float yaw, pitch, roll;
					q.GetYawPitchRoll(&yaw, &pitch, &roll);
					render->sensors->setOrientation(roll, pitch, yaw);
				}

				// std::cout << EyeRenderPose[eye].Position.x << " " << EyeRenderPose[eye].Position.y << " " << EyeRenderPose[eye].Position.z << " (" << EyeRenderPose[eye].Orientation.x << " " << EyeRenderPose[eye].Orientation.y << " "  << EyeRenderPose[eye].Orientation.z << " " << EyeRenderPose[eye].Orientation.w << std::endl;

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
				scene->cameraScreen->Pos = CombinedPosF;
				scene->cameraScreen->Rot = CombinedRotF;

				// Update camera screen
				if (cameraStream->isNewFrame()) {
					cv::Mat frame;
					*cameraStream >> frame;
					if (frame.rows > 0 && frame.cols > 0) {
						if (scene->cameraScreen->Fill->Tex->SizeH != frame.rows || scene->cameraScreen->Fill->Tex->SizeW != frame.cols) {
							scene->prepareCameraScreen(frame.cols, frame.rows,
								SingleEyeScene::DEFAULT_Z,
								SingleEyeScene::DEFAULT_FOV,
								SingleEyeScene::DEFAULT_COLOUR);
						}
						if ( !frame.isContinuous() ) {
							frame = frame.clone();
						}
						uint32_t * texData = (uint32_t *)frame.data;
						scene->cameraScreen->Fill->Tex->FillTexture(texData);
					}
				}

				// Render
				scene->Render(&prod, 1, 1, 1, 1, true);

				// Commit
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
	delete leftEyeScene;
	if (mirrorTexture)
		ovr_DestroyMirrorTexture(session, mirrorTexture);
	for (int eye = 0; eye < 2; ++eye)
	{
		delete pEyeRenderTexture[eye];
		delete pEyeDepthBuffer[eye];
	}
	DIRECTX.ReleaseDevice();
	ovr_Destroy(session);

	return retryCreate || (result == ovrError_DisplayLost);
}

Render::Render() :
	leftCameraStream(nullptr),
	rightCameraStream(nullptr),
	sensors(nullptr)
{
}

void Render::start(HINSTANCE hinst) {
	render = this;
	ovrResult result = ovr_Initialize(nullptr);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	VALIDATE(DIRECTX.InitWindow(hinst, L"Bruc2016"), "Failed to open window.");

	DIRECTX.Run(MainLoop);

	ovr_Shutdown();
}
