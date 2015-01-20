#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
	eyex = 0.0f;  //Alter to suit your current set up
	eyey = 20.0f;
	eyez = 15.0f;
	cam = 1; // for adaption when multiple cameras are added

}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

	// Initialize the world matrix
	//XMStoreFloat4x4(&_world[0], XMMatrixIdentity());
	//XMStoreFloat4x4(&_world[1], XMMatrixIdentity());
	//XMStoreFloat4x4(&_world[2], XMMatrixIdentity());
	//XMStoreFloat4x4(&_world[3], XMMatrixIdentity());
	//XMStoreFloat4x4(&_world[4], XMMatrixIdentity());
	//XMStoreFloat4x4(&_world[5], XMMatrixIdentity());


    // Initialize the view matrix
	//XMFLOAT4 Eye = XMFLOAT4(0.0f, 10.0f, 20.0f, 0.0f);
	//XMFLOAT4 At = XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	//XMFLOAT4 Up = XMFLOAT4(0.0f, 3.0f, 0.0f, 0.0f);

	//XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
	//XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

	// Initialise Depth Buffer
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D* _depthStencilBuffer;

	//Init Ground Buffer
	XMStoreFloat4x4(&_groundPlane, XMMatrixIdentity());
	XMStoreFloat4x4(&_groundPlane, XMMatrixScaling(200.0f, 100.0f, 100.0f)*XMMatrixTranslation(0.0f, 0.1f, 0.0f));
	
	//Crate
	modelCrate = new GameObject();
	objCrate = OBJLoader::Load("cube.obj", _pd3dDevice);
	modelCrate->Initialise(objCrate);
	XMStoreFloat4x4(&modelCrate->GetWorld(), XMMatrixIdentity());


	////Init Crate
	//for (int i = 0; i < 5; i++)
	//{
	//	
	//	modelCrate[i] = new GameObject();
	//	objCrate = OBJLoader::Load("cube.obj", _pd3dDevice);
	//	modelCrate[i]->Initialise(objCrate);
	//	XMStoreFloat4x4(&modelCrate[i]->GetWorld(), XMMatrixIdentity());
	//}
	//modelCrate[0]->SetTranslation(-25.0f, 0.0f, -25.0f);
	//modelCrate[1]->SetTranslation(30.0f, 0.0f, -25.0f);
	//modelCrate[2]->SetTranslation(25.0f, 0.0f, 25.0f);
	//modelCrate[3]->SetTranslation(-30.0f, 0.0f, 25.0f);
	//modelCrate[4]->SetTranslation(-5.0f, 0.0f, -5.0f);


	//Init Car
	playerCar = new GameObject();
	objMeshData = OBJLoader::Load("playerCar.obj", _pd3dDevice, true);
	playerCar->Initialise(objMeshData);
	XMStoreFloat4x4(&playerCar->GetWorld(), XMMatrixIdentity());
	playerCar->SetTranslation(-4.0f, 0.0f, -50.0f);
	playerCar->SetScale(0.1f, 0.1f, 0.1f);
	playerCar->SetRotation(0.0f, 90.0f, 0.0f);
	playerCar->UpdateWorld();

	npcCar = new GameObject();
	npcCar->Initialise(objMeshData);
	XMStoreFloat4x4(&npcCar->GetWorld(), XMMatrixIdentity());
	npcCar->SetTranslation(-20.0f, 0.0f, -70.0f);
	npcCar->SetScale(0.1f, 0.1f, 0.1f);
	npcCar->SetRotation(0.0f, 1.5f, 0.0f);
	npcCar->UpdateWorld();

	
	//objCrate = OBJLoader::Load("cube.obj", _pd3dDevice);
	//modelCrate.Initialise(objCrate);

	//Textures
	CreateDDSTextureFromFile(_pd3dDevice, L"car.dds", nullptr, &_pTextureCar);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCar);

	CreateDDSTextureFromFile(_pd3dDevice, L"crate_COLOR.dds", nullptr, &_pTextureCrate);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCrate);

	CreateDDSTextureFromFile(_pd3dDevice, L"track.dds", nullptr, &_pTextureTrack);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureTrack);

	//Camera
	XMFLOAT4 Eye(0.0f, 20.0f, 20.0f, 0.0f);
	XMFLOAT4 TopViewEye(0.0f, 50.0f, 20.0f, 0.0f); //For a set view from above
	XMFLOAT4 FarTop(0.0f, 75.0f, 20.0f, 0.0f);
	XMFLOAT4 At(0.0f, 0.0f, -50.0f, 0.0f);
	XMFLOAT4 Up(0.0f, 1.0f, 0.0f, 0.0f);
	XMFLOAT4 ChaseAt(-4.0f, 0.0f, -50.0f, 0.0f); //car starting position
	XMFLOAT4 ChaseEye(-15.0f, 10.0f, -50.0f, 0.0f);


	_pCam = new Camera(Eye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 1000.0f);
	_pCam->CalculateViewProjection();

	_pCamTopDown = new Camera(TopViewEye, At, Up, _WindowWidth, _WindowHeight, 0.01f, 1000.0f);
	_pCamTopDown->CalculateViewProjection();

	_pCamFarTopDown = new Camera(FarTop, At, Up, _WindowWidth, _WindowHeight, 0.01f, 1000.0f);
	_pCamFarTopDown->CalculateViewProjection();

	_pCamChase = new Camera(ChaseEye, ChaseAt, Up, _WindowWidth, _WindowHeight, 0.01f, 1000.0f);
	_pCamChase->CalculateViewProjection();

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"Lighting.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"Lighting.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};



	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f),		XMFLOAT3(-1.0f, 1.0f, -1.0f),	 XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f),		XMFLOAT3(1.0f, 1.0f, -1.0f),	 XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f),	XMFLOAT3(-1.0f, -1.0f, -1.0f),	 XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f),		XMFLOAT3(1.0f, -1.0f, -1.0f),	 XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f, 1.0f),  XMFLOAT3(-1.0f, 1.0f, 1.0f),  XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT3(1.0f, 1.0f, 1.0f),   XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f),  XMFLOAT3(1.0f, -1.0f, 1.0f),  XMFLOAT2(1.0f, 1.0f) },
    };


    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 8;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		0,1,2,    // side 1
		2,1,3,
		4,0,6,    // side 2
		6,0,2,
		7,5,6,    // side 3
		6,5,4,
		3,1,7,    // side 4
		7,1,5,
		4,5,0,    // side 5
		0,5,1,
		3,7,2,    // side 6
		2,7,6,

    };

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

HRESULT Application::InitGroundPlaneBuffer()
{
	HRESULT hr;

	////////////////////////////////////////

	//Create Ground Plane
	SimpleVertex groundPlane[] =
	{
		{ XMFLOAT3(-1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC gp;
	ZeroMemory(&gp, sizeof(gp));
	gp.Usage = D3D11_USAGE_DEFAULT;
	gp.ByteWidth = sizeof(SimpleVertex)* 4;
	gp.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	gp.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitDataGP;
	ZeroMemory(&InitDataGP, sizeof(InitDataGP));
	InitDataGP.pSysMem = groundPlane;

	////////////////////////////////////////


	hr = _pd3dDevice->CreateBuffer(&gp, &InitDataGP, &_pGroundPlaneBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitGroundPlaneIndexBuffer()
{
	HRESULT hr;

	//Create Ground Plane Index
	WORD groundPlaneIndices[] =
	{
		0, 1, 3,    // side 1
		0, 3, 2,
	};

	D3D11_BUFFER_DESC gp;
	ZeroMemory(&gp, sizeof(gp));

	gp.Usage = D3D11_USAGE_DEFAULT;
	gp.ByteWidth = sizeof(WORD)* 6;
	gp.BindFlags = D3D11_BIND_INDEX_BUFFER;
	gp.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = groundPlaneIndices;
	hr = _pd3dDevice->CreateBuffer(&gp, &InitData, &_pGroundPlaneIndexBuffer);

	if (FAILED(hr))
		return hr;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"GGS Assignment", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

	// Define depth buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	_pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	InitVertexBuffer();
	InitGroundPlaneBuffer();

    // Set vertex buffer
    
    _pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);

	InitIndexBuffer();
	InitGroundPlaneIndexBuffer();

    // Set index buffer
    _pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	



    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);

	//typedef struct D3D11_BLEND_DESC {
	//	BOOL AlphaToCoverageEnable;
	//	BOOL IndependentBlendEnable;
	//	D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[8];
	//} D3D11_BLEND_DESC;

	//typedef struct D3D11_RENDER_TARGET_BLEND_DESC {
	//	BOOL           BlendEnable;
	//	D3D11_BLEND    SrcBlend;
	//	D3D11_BLEND    DestBlend;
	//	D3D11_BLEND_OP BlendOp;
	//	D3D11_BLEND    SrcBlendAlpha;
	//	D3D11_BLEND    DestBlendAlpha;
	//	D3D11_BLEND_OP BlendOpAlpha;
	//	UINT8          RenderTargetWriteMask;
	//} D3D11_RENDER_TARGET_BLEND_DESC;

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable = true;
	rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_pd3dDevice->CreateBlendState(&blendDesc, &Transparency);


    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();

}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    //
    // Animate the cube
    //
	XMStoreFloat4x4(&_groundPlane, XMMatrixScaling(200.0f, 100.0f, 100.0f)*XMMatrixTranslation(0.0f,0.1f,0.0f));
	//XMStoreFloat4x4(&_world[0], XMMatrixRotationY(t)*XMMatrixTranslation(0,0,0));
	//XMStoreFloat4x4(&_world[1], XMMatrixRotationY(t)*XMMatrixTranslation(5,0,0)*XMMatrixRotationY(t));
	//XMStoreFloat4x4(&_world[2], XMMatrixTranslation(15, 0, 0)*XMMatrixRotationY(t)*XMMatrixTranslation(5, 0, 0)*XMMatrixRotationY(t));
	//XMStoreFloat4x4(&_world[3], XMMatrixRotationY(t)*XMMatrixTranslation(15, 0, 0)*XMMatrixRotationY(t));
	//XMStoreFloat4x4(&_world[4], XMMatrixRotationY(t)*XMMatrixTranslation(35, 0, 0)*XMMatrixRotationY(t));
	//XMStoreFloat4x4(&_world[5], XMMatrixRotationY(t));

	//Animate Car
	//playerCar.SetTranslation(2*t, 0.5f, 5.0f);
	//playerCar.SetScale(0.5f, 0.5f, 0.5f);
	//playerCar.UpdateWorld();

	//modelCrate.UpdateWorld();
	//modelCrate.Update(t);

	//USER INPUT
	float cameraSpeed = 0.01f;
	//Zoom Out
	if (GetAsyncKeyState(VK_LEFT)){
		eyex -= cameraSpeed;
	}

	//Zoom In
	if (GetAsyncKeyState(VK_RIGHT)){
		eyex += cameraSpeed; // or call camera setX function appropriately
	}

	//Pan Up
	if (GetAsyncKeyState(0x57)){
		eyey -= cameraSpeed;
	}

	//Pan Down
	if (GetAsyncKeyState(0x53)){
		eyey += cameraSpeed;
	}

	//Pan Left
	if (GetAsyncKeyState(0x41)){
		eyez -= cameraSpeed;
	}
	//Pan right
	if (GetAsyncKeyState(0x44)){
		eyez += cameraSpeed;
	}

	//CHANGE CAMERA
	if (GetAsyncKeyState(0x31)){
		cam = 1;
	}
	if (GetAsyncKeyState(0x32)){
		cam = 2;
	}
	if (GetAsyncKeyState(0x33)){
		cam = 3;
	}
	if (GetAsyncKeyState(0x34)){
		cam = 4;
	}
	if (GetAsyncKeyState(0x35)){
		cam = 5;
	}


	XMFLOAT4 Eye(eyex, eyey, eyez, 0.0f);
	XMFLOAT4 At(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 Up(0.0f, 1.0f, 0.0f, 0.0f);
	_pCam->SetEye(Eye);
	_pCam->CalculateViewProjection();

	
	_pCamTopDown->SetEye(Eye);
	//_pCamTopDown->CalculateViewProjection();
	_pCamFarTopDown->SetEye(Eye);
	_pCamChase->SetEye(Eye);
	//_pCamFarTopDown->CalculateViewProjection();


}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	XMMATRIX world;
	XMMATRIX view;
	XMFLOAT3 eyepos;
	XMMATRIX projection;
	if (cam == 1)
	{
		view = XMLoadFloat4x4(&_pCam->GetView());
		projection = XMLoadFloat4x4(&_pCam->GetProjection());
		eyepos = XMFLOAT3(_pCam->GetEye().x, _pCam->GetEye().y, _pCam->GetEye().z);
	}
	else if (cam == 2)
	{
		view = XMLoadFloat4x4(&_pCamTopDown->GetView());
		projection = XMLoadFloat4x4(&_pCamTopDown->GetProjection());
		eyepos = XMFLOAT3(_pCamTopDown->GetEye().x, _pCamTopDown->GetEye().y, _pCamTopDown->GetEye().z);
	}
	else if (cam == 3)
	{
		view = XMLoadFloat4x4(&_pCamFarTopDown->GetView());
		projection = XMLoadFloat4x4(&_pCamTopDown->GetProjection());
		eyepos = XMFLOAT3(_pCamFarTopDown->GetEye().x, _pCamTopDown->GetEye().y, _pCamTopDown->GetEye().z);
	}
	else if (cam == 4)
	{
		view = XMLoadFloat4x4(&_pCamChase->GetView());
		projection = XMLoadFloat4x4(&_pCamChase->GetProjection());
		eyepos = XMFLOAT3(_pCamChase->GetEye().x, _pCamChase->GetEye().y, _pCamChase->GetEye().z);
	}


	//XMVECTOR Eye = XMVectorSet(0.0f, 10.0f, 20.0f, 0.0f);
	//XMVECTOR At = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//XMVECTOR Up = XMVectorSet(0.0f, 3.0f, 0.0f, 0.0f);

	//XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

	//XMMATRIX world = XMLoadFloat4x4(&_world[0]);
	//XMMATRIX view = XMLoadFloat4x4(&_view);
	//XMMATRIX projection = XMLoadFloat4x4(&_projection);
    //
    // Update variables
    //
    ConstantBuffer cb;
	//cb.World = XMMatrixTranspose(world);

	//cb.diffuseMaterial = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//cb.diffuseLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//cb.lightVecW = XMFLOAT3(0.0f, 10.0f, 20.0f);

	//cb.gAmbientMtrl = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//cb.gAmbientLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	//cb.gSpecularMtrl = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//cb.gSpecularLight = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	//cb.gSpecularPower = 10.0f;
	//cb.gEyePosW = XMFLOAT3(0.0f, 10.0f, 20.0f);

	cb.diffuseMaterial = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.diffuseLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.lightVecW = XMFLOAT3(1.0f, 1.0f, 1.0f);

	cb.gAmbientMtrl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.gAmbientLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	cb.gSpecularMtrl = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.gSpecularLight = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	cb.gSpecularPower = 10.0f;
	cb.gEyePosW = XMFLOAT3(-100.0f, 100.0f, 100.0f);

	cb.View = XMMatrixTranspose(view);
	cb.Projection = XMMatrixTranspose(projection);
	
	
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	
	//
	//Shaders
	//
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);


	//Switch Textures
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCar);

	//PLAYER CAR
	world = XMLoadFloat4x4(&playerCar->GetWorld());
	cb.World = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	playerCar->Draw(_pd3dDevice,_pImmediateContext);

	//NPC CAR
	world = XMLoadFloat4x4(&npcCar->GetWorld());
	cb.World = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	npcCar->Draw(_pd3dDevice, _pImmediateContext);


		world = XMLoadFloat4x4(&modelCrate->GetWorld());
		modelCrate->SetTranslation(4.0f, 2.0f, 8.0f);
		modelCrate->SetScale(2.0f, 2.0f, 2.0f);
		modelCrate->UpdateWorld();
		_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCrate);
		cb.World = XMMatrixTranspose(world);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		modelCrate->Draw(_pd3dDevice, _pImmediateContext);

	//for (int i = 0; i < 5; i++)
	//{
	//	//CRATE
	//	world = XMLoadFloat4x4(&modelCrate[i]->GetWorld());
	//	modelCrate[i]->SetTranslation(4.0f, 2.0f, 8.0f);
	//	modelCrate[i]->SetScale(2.0f, 2.0f, 2.0f);
	//	modelCrate[i]->UpdateWorld();
	//	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureCrate);
	//	cb.World = XMMatrixTranspose(world);
	//	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//	modelCrate[i]->Draw(_pd3dDevice, _pImmediateContext);
	//}
	


    // Renders a triangle
    //
	

	//"fine-tune" the blending equation
	float blendFactor[] = { 0.75f, 0.75f, 0.75f, 1.0f };

	//Set the default blend state (no blending) for opaque objects
	//_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	//Render opaque objects//

	//Set the blend state for transparent objects
	//_pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);


	//HARD CODED CUBES

	//_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
	//_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

/*	world = XMLoadFloat4x4(&_world[0]);
	cb.World = XMMatrixTranspose(world);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);  */      

	//world = XMLoadFloat4x4(&_world[1]);
	//cb.World = XMMatrixTranspose(world);
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//_pImmediateContext->DrawIndexed(36, 0, 0);

	//world = XMLoadFloat4x4(&_world[2]);
	//cb.World = XMMatrixTranspose(world);
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//_pImmediateContext->DrawIndexed(36, 0, 0);

	//world = XMLoadFloat4x4(&_world[3]);
	//cb.World = XMMatrixTranspose(world);
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//_pImmediateContext->DrawIndexed(36, 0, 0);

	//world = XMLoadFloat4x4(&_world[4]);
	//cb.World = XMMatrixTranspose(world);
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//_pImmediateContext->DrawIndexed(36, 0, 0);


	//world = XMLoadFloat4x4(&_world[5]);
	//cb.World = XMMatrixTranspose(world);
	//_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	//_pImmediateContext->DrawIndexed(6, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pGroundPlaneBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pGroundPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);



	world = XMLoadFloat4x4(&_groundPlane);
	cb.World = XMMatrixTranspose(world);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureTrack);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(6, 0, 0);


    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}