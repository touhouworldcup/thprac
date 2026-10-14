#pragma once
#include <d3d8.h>

namespace THSnapshot {
	void* GetSnapshotData(IDirect3DDevice8* d3d8);
	void Snapshot(IDirect3DDevice8* d3d8);
}