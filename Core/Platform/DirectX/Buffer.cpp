#include "Buffer.h"

vl::Resource::Buffer::Buffer(){
}

void vl::Resource::Buffer::InitDevice(ComPtr<ID3D11Device> deviceInstance){
	device = deviceInstance;
}
