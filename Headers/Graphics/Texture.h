#pragma once

#include <d3d12.h>
#include <d3dx12.h>
#include <wrl.h>
using namespace Microsoft::WRL;

class Texture
{
public:
	Texture(unsigned char* data, int width, int height);
	~Texture();

	int GetSRVIndex();
	CD3DX12_GPU_DESCRIPTOR_HANDLE GetSRV();
	D3D12_GPU_VIRTUAL_ADDRESS GetGPULocation();
	ComPtr<ID3D12Resource> GetResource();

private:
	void UploadData(unsigned char* data, int width, int height);

private:
	ComPtr<ID3D12Resource> textureResource;
	int srvIndex = 0;
};