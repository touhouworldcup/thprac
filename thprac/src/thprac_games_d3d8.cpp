#include "thprac_games_d3d8.h"

namespace THSnapshot {
    void* GetSnapshotData(IDirect3DDevice8* d3d8)
    {
        IDirect3DSurface8* surface = nullptr;
        d3d8->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &surface);
        D3DLOCKED_RECT rect = {};
        surface->LockRect(&rect, nullptr, 0);

        void* bmp = malloc(0xE2000);
        uint8_t* bmp_write = (uint8_t*)bmp;
        for (int32_t i = 0x1DF; i >= 0; --i) {
            uint8_t* bmp_bits = ((uint8_t*)rect.pBits) + i * rect.Pitch;
            for (size_t j = 0; j < 0x280; ++j) {
                memcpy(bmp_write, bmp_bits, 3); // This *should* get optimized to byte/word MOVs
                bmp_bits += 4;
                bmp_write += 3;
            }
        }

        surface->UnlockRect();
        surface->Release();

        return bmp;
    }
    void Snapshot(IDirect3DDevice8* d3d8)
    {
        wchar_t dir[] = L"snapshot/th000.bmp";
        HANDLE hFile;
        CreateDirectoryW(L"snapshot", nullptr);
        for (int i = 0; i < 1000; i++) {
            dir[13] = static_cast<wchar_t>(i % 10) + L'0';
            dir[12] = static_cast<wchar_t>((i % 100 - i % 10) / 10) + L'0';
            dir[11] = static_cast<wchar_t>((i - i % 100) / 100) + L'0';
            hFile = CreateFileW(dir, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (hFile != INVALID_HANDLE_VALUE)
                break;
        }
        if (hFile == INVALID_HANDLE_VALUE)
            return;

        auto header = "\x42\x4d\x36\x10\x0e\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00\x00\x00\x80\x02\x00\x00\xe0\x01\x00\x00\x01\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
        void* bmp = GetSnapshotData(d3d8);
        DWORD bytesRead;
        WriteFile(hFile, header, 0x36, &bytesRead, nullptr);
        WriteFile(hFile, bmp, 0xE2000, &bytesRead, nullptr);
        free(bmp);

        CloseHandle(hFile);
    }
};
