#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "GameObject.h"
#include "Structures.h"
#include "OBJLoader.h"
#include "DDSTextureLoader.h"
#include "Camera.h"

using namespace DirectX;

struct ConstantBuffer
{
	XMMATRIX World;
	XMMATRIX View;
	XMMATRIX Projection;

	XMFLOAT4 diffuseMaterial;	//Diffuse
	XMFLOAT4 diffuseLight;
	XMFLOAT3 lightVecW;

	XMFLOAT4 gAmbientMtrl;		//Ambient Material
	XMFLOAT4 gAmbientLight;		//Ambient Colour

	XMFLOAT4 gSpecularMtrl;		//Specular
	XMFLOAT4 gSpecularLight;
	FLOAT	 gSpecularPower;
	XMFLOAT3 gEyePosW;
};


class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4              _world[5];
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;


	//Camera
	Camera*					_pCam;
	Camera*					_pCamTopDown;
	Camera*					_pCamFarTopDown;
	Camera*					_pCamChase;
	float					eyex, eyey, eyez;
	int						cam;

	//GroundPlane
	XMFLOAT4X4				_groundPlane;
	ID3D11Buffer*           _pGroundPlaneBuffer;
	ID3D11Buffer*           _pGroundPlaneIndexBuffer;

	//Cars
	MeshData				objMeshData;
	GameObject*				playerCar;
	GameObject*				npcCar;

	//Crate
	MeshData				objCrate;
	GameObject*				modelCrate;
	XMMATRIX				worldCrate;

	//Textures
	ID3D11SamplerState*     _pSamplerLinear = nullptr;
	ID3D11ShaderResourceView* _pTextureCar;
	ID3D11ShaderResourceView* _pTextureCrate;
	ID3D11ShaderResourceView* _pTextureTrack;

	//Blending
	ID3D11BlendState* Transparency;



private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();
	HRESULT InitGroundPlaneBuffer();
	HRESULT InitGroundPlaneIndexBuffer();

	// Depth/Stencil buffers
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;


	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
};

