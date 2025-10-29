#include "stdafx.h"
#include "Player.h"
#include "Camera.h"
#include "Scene.h"
#include "Level.h"
#include "GameLevelManager.h"

GameLevelManager::GameLevelManager(ID3D12Device* pd3dDevice)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	for (int i = 0; i < 3; i++)
	{
		player[i] = CPlayer();
		//levelArray[i] = new CScene();
	}

	levelArray[0] = new MainLevel();
	levelArray[1] = new MainLevel();
	levelArray[2] = new MainLevel();

	currentLevel = 0;
	nextLevel = -1;
}

GameLevelManager::~GameLevelManager()
{
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	for (int i = 0; i < 3; i++)
	{
		player[i].Release();
		levelArray[i]->ReleaseObjects();
	}
}	

ID3D12RootSignature* GameLevelManager::CreateGraphicsRootSignature(ID3D12Device* pd3dDevice)
{
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;

	D3D12_DESCRIPTOR_RANGE descRange = {};
	descRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	descRange.NumDescriptors = 1;
	descRange.BaseShaderRegister = 2; // register(b2)
	descRange.RegisterSpace = 0;
	descRange.OffsetInDescriptorsFromTableStart = 0;

	D3D12_DESCRIPTOR_RANGE srvRange = {};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;
	srvRange.BaseShaderRegister = 0; // t0
	srvRange.RegisterSpace = 0;
	srvRange.OffsetInDescriptorsFromTableStart = 0;

	D3D12_ROOT_PARAMETER pd3dRootParameters[5];

	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[0].Descriptor.ShaderRegister = 1; //Camera
	pd3dRootParameters[0].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[1].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[1].DescriptorTable.pDescriptorRanges = &descRange;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 4; //Lights
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	pd3dRootParameters[3].DescriptorTable.NumDescriptorRanges = 1;
	pd3dRootParameters[3].DescriptorTable.pDescriptorRanges = &srvRange;
	pd3dRootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	pd3dRootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[4].Constants.Num32BitValues = 1;
	pd3dRootParameters[4].Constants.ShaderRegister = 3; // b3
	pd3dRootParameters[4].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;

	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;
	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3dSignatureBlob, &pd3dErrorBlob);
	pd3dDevice->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(), pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3dGraphicsRootSignature);
	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();

	return(pd3dGraphicsRootSignature);
}

void GameLevelManager::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	char debugMsg[128];
	sprintf_s(debugMsg, "BuildObjects() completed for Level %d\n", currentLevel);
	OutputDebugStringA(debugMsg);

	levelArray[currentLevel]->BuildObjects(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
}

void GameLevelManager::ReleaseScene()
{
	levelArray[currentLevel]->ReleaseUploadBuffers();
	levelArray[currentLevel]->ReleaseObjects();
}

void GameLevelManager::ChangeLevel(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList)
{
	ReleaseScene();

	// 변경 전후 출력
	char debugMsg[128];
	sprintf_s(debugMsg, "ChangeLevel() called: from Level %d to Level %d\n", currentLevel, nextLevel);
	OutputDebugStringA(debugMsg); // Visual Studio의 Output 창에 출력됨

	currentLevel = nextLevel;

	// 새 레벨 빌드
	BuildObjects(pd3dDevice, pd3dCommandList);
}
