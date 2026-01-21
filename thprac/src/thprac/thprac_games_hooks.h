#pragma once
#include <Windows.h>
#include <imgui.h>
#include <dinput.h>

namespace THPrac {
    void ChangeBGMSpeed(DWORD thiz = 0);
    void SetBGMSpeed(float speed);
    void HookBGMSpeed(bool is_hook);


    void HookCreateFont(bool useCorrectJaFont, bool useCustomFont,int fontidx);

    
    struct FastRetryOpt {
        // fast re
        static constexpr int fast_retry_cout_down_max = 15;
        bool enable_fast_retry = false;
        int fast_retry_count_down = 0;
};

    struct InputOpt {
    // raw input
    BYTE fake_di_State[256] = { 0 };
    bool is_ri_inited = false;

    // auto shoot
    bool enable_auto_shoot = false;
    int shoot_key_DIK = -1;
    bool last_is_auto_shoot_key_down = false;
    bool is_auto_shooting = false;
    bool is_th128 = false;

    // disable key
    bool disable_xkey = false;
    bool disable_Ckey_at_same_time = true;
    bool disable_shiftkey = false;
    bool disable_zkey = false;
    bool disable_f10_11_13 = false;
    bool disable_locale_change_hotkey = true;
    bool disable_win_key = false;

    bool g_disable_joy = false;

    enum class SOCD_Setting { SOCD_Default = 0,
        SOCD_2,
        SOCD_N };
    SOCD_Setting g_socd_setting;

    bool use_get_device_data = false;

    enum class KeyboardAPI { Default_API = 0,
        Force_win32KeyAPI,
        Force_dinput8KeyAPI,
        Force_RawInput };
    KeyboardAPI g_keyboardAPI;

    bool g_enable_keyhook;

    LPDIRECTINPUT8 dinput;
    HRESULT(STDMETHODCALLTYPE* realDIRelease)
    (LPDIRECTINPUT8 thiz);
    HRESULT(STDMETHODCALLTYPE* realEnumDevices)
    (LPDIRECTINPUT8 thiz, DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags);
    HRESULT(STDMETHODCALLTYPE* realCreateDevice)
    (LPDIRECTINPUT8 thiz, const GUID& guid, LPDIRECTINPUTDEVICE8W* lpddevice, LPUNKNOWN lpunk);

    LPDIRECTINPUTDEVICE8 ddevice;
    HRESULT(STDMETHODCALLTYPE* realAcquire)
    (LPDIRECTINPUTDEVICE8 thiz);
    HRESULT(STDMETHODCALLTYPE* realPoll)
    (LPDIRECTINPUTDEVICE8 thiz);
    HRESULT(STDMETHODCALLTYPE* realDDRelease)
    (LPDIRECTINPUTDEVICE8 thiz);
    HRESULT(STDMETHODCALLTYPE* realSetDataFormat)
    (LPDIRECTINPUTDEVICE8 thiz, LPCDIDATAFORMAT);
    HRESULT(STDMETHODCALLTYPE* realGetDeviceState)
    (LPDIRECTINPUTDEVICE8 thiz, DWORD, LPVOID);
    HRESULT(STDMETHODCALLTYPE* realSetCooperativeLevel)
    (LPDIRECTINPUTDEVICE8 thiz, HWND, DWORD);
    HRESULT(STDMETHODCALLTYPE* realSetProperty)
    (LPDIRECTINPUTDEVICE8 thiz, REFGUID guid, LPCDIPROPHEADER header);

    HRESULT(WINAPI* g_realDirectInput8Create)
    (HINSTANCE hinst, DWORD dwVersion, const IID& riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
    BOOL(WINAPI* g_realGetKeyboardState)
    (PBYTE lpKeyboardState);

    MMRESULT(WINAPI* g_realJoyGetDevCapsA)
    (UINT uJoyID, LPJOYCAPSA pjc, UINT cbjc);
    MMRESULT(WINAPI* g_realJoyGetPosEx)
    (UINT uJoyID, LPJOYINFOEX pji);
    DWORD(WINAPI* g_realXInputGetState3)
    (DWORD dwUserIndex, void* pState);
    DWORD(WINAPI* g_realXInputGetState4)
    (DWORD dwUserIndex, void* pState);
};

    struct InputLatencyTestOpt {
        bool test_input_latency = false;
        int clockid;
        double last_input_latency;
        void Init();
        void Input();
        void Present();
        double GetLatency();
    };

    void HookJoyInput();
    void HookKeyboardInput();
    void SetDinput8DeviceData();
    void InitInput();
    void EnableInputLatencyTest(bool is_enable);
    void ClearInputData(bool inactivate);
    bool GetLatency(double* latency);
    void GetRawInput(DWORD hraw);
    }