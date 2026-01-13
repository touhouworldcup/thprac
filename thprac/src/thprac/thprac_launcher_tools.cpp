#include "thprac_launcher_tools.h"
#include "thprac_launcher_utils.h"
#include "thprac_launcher_games.h"
#include "thprac_launcher_main.h"
#include "thprac_main.h"
#include "thprac_gui_locale.h"
#include "thprac_utils.h"
#include "utils/utils.h"
#include <functional>
#include <string>
#include <vector>
#include "..\3rdParties\rapidcsv\rapidcsv.h"
#include <numbers>
#include <format>
#include "thprac_launcher_tools_waifus.h"

#include <DirectXMath.h>
#include <d3d9.h>

#include <dinput.h>
#include <deque>

namespace THPrac {

extern LPDIRECT3DDEVICE9 g_pd3dDevice;
void LauncherToolsGuiSwitch(const char* gameStr);



class THGuiInputTest {
    int clockid = -1;

public:
    
    THGuiInputTest()
    {
        clockid = SetUpClock();
    }
    std::string GetKeyName(int dik)
    {
        for (auto& kb : keyBindDefine) {
            if (kb.dik == dik) {
                return std::format("{}(DIK_{})", kb.keyname, dik);
            }
        }
        return std::string("DIK_") + std::to_string(dik);
    }
    bool GuiUpdate()
    {
        static IDirectInput8* dinput8 = []() -> auto { 
            static IDirectInput8* dinput8;
            DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput8, NULL);
            return dinput8;
        }();
        double delta_time = ResetClock(clockid);

        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_INPUT_TEST));
        ImGui::Separator();

        if (!dinput8) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_DINPUT8_FAILED));
            return result;
        }
        static IDirectInputDevice8* pMouse = nullptr;
        static IDirectInputDevice8* pKeyboard = nullptr;
        static IDirectInputDevice8* pGamepad = nullptr;

        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_INPUT_TEST_MOUSE))) {
            if (!pMouse) {
                if (SUCCEEDED(dinput8->CreateDevice(GUID_SysMouse, &pMouse, NULL))) {
                    pMouse->SetDataFormat(&c_dfDIMouse2);
                    pMouse->SetCooperativeLevel(GetActiveWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
                    pMouse->Acquire();
                }
            }
            if (pMouse) {
                DIMOUSESTATE2 mouseState;
                HRESULT hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);

                static double mouse_time[8] = { 0.0f };
                if (FAILED(hr)) {
                    pMouse->Acquire();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_ACQUIRING));
                } else {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_OK));

                    ImGui::Columns(2, S(THPRAC_TOOLS_INPUT_TEST_MOUSE_DELTA));
                    ImGui::Text("Delta X : %ld", mouseState.lX);
                    ImGui::Text("Delta Y : %ld", mouseState.lY);
                    ImGui::Text("Delta Z : %ld", mouseState.lZ);
                    
                    ImGui::NextColumn();
                    ImGui::Text(S(THPRAC_TOOLS_INPUT_TEST_BUTTONS));
                    for (int i = 0; i < 8; i++) {
                        if (mouseState.rgbButtons[i] & 0x80) {
                            ImGui::TextColored(ImVec4(1, 0.5f, 0, 1), "[%d]: frame %d", i, (int)(mouse_time[i]*60.0f));
                            mouse_time[i] += delta_time;
                        } else {
                            mouse_time[i] = 0;
                        }
                    }
                    ImGui::Columns(1);

                    const char* mbtn_strs[] = { "[L]", "[R]", "[M]"};
                    for (int i = 0; i < 3; i++) {
                        if (mouseState.rgbButtons[i] & 0x80) {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.3, 0.3, 1));
                            ImGui::Button(mbtn_strs[i]);
                            ImGui::PopStyleColor(1);
                        } else {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.5, 0.3, 1));
                            ImGui::Button(mbtn_strs[i]);
                            ImGui::PopStyleColor(1);
                        }
                        if(i!=2)
                            ImGui::SameLine();
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), S(THPRAC_TOOLS_INPUT_MOUSE_FAILED));
            }
        }
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_INPUT_KEYBOARD))) {
            if (!pKeyboard) {
                if (SUCCEEDED(dinput8->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL))) {
                    pKeyboard->SetDataFormat(&c_dfDIKeyboard);
                    pKeyboard->SetCooperativeLevel(GetActiveWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
                    pKeyboard->Acquire();
                }
            }
            if (pKeyboard) {
                static char keyBuffer_last[256] = { 0 };
                static char keyBuffer[256] = { 0 };
                static double key_time[256] = { 0 };
                struct KeyEvent
                {
                    int dik;
                    int type;
                    double time;
                };
                static std::deque<KeyEvent> keys_event;

                HRESULT hr = pKeyboard->GetDeviceState(sizeof(keyBuffer), (LPVOID)&keyBuffer);
                if (FAILED(hr)) {
                    pKeyboard->Acquire();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_ACQUIRING));
                } else {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_OK));
                    ImGui::Text(S(THPRAC_TOOLS_INPUT_TEST_BUTTONS));
                    ImGui::Columns(2);
                    bool anyPressed = false;
                    for (int i = 0; i < 256; i++) {
                        if (keyBuffer[i] & 0x80) {
                            anyPressed = true;
                            ImGui::Button(std::format("{}: {} frame  ({:.3f} ms)", GetKeyName(i), (int)(key_time[i]*60.0f), key_time[i] * 1000.0f).c_str());
                            key_time[i] += delta_time;
                            if (!keyBuffer_last[i]){
                                keys_event.push_back({ i, 1, 0 });
                            }
                        } else {
                            if (keyBuffer_last[i]) {
                                keys_event.push_back({ i, 0, key_time[i] });
                            }
                            key_time[i] = 0.0f;
                        }
                    }
                    while (keys_event.size() >= 10)
                        keys_event.pop_front();
                    memcpy(keyBuffer_last, keyBuffer, 256);

                    if (!anyPressed)
                        ImGui::TextDisabled("(None)");
                    ImGui::NextColumn();
                    for (auto& i : keys_event)
                    {
                        if (i.type == 1)
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5,0.3,0.3,1));
                            ImGui::Button(std::format("[D]  {}", GetKeyName(i.dik)).c_str());
                            ImGui::PopStyleColor(1);
                        }
                        else
                        {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.5, 0.3, 1));
                            ImGui::Button(std::format("[U]  {}:  {:.3f} ms", GetKeyName(i.dik), i.time * 1000.0f).c_str());
                            ImGui::PopStyleColor(1);
                        }
                    }
                    ImGui::Columns(1);
                }
            }
        }

        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_INPUT_GAMEPAD))) {

            static bool s_gamepadFound = false;
            if (ImGui::Button(S(THPRAC_TOOLS_INPUT_GAMEPAD_CONNECT))) {
                if (pGamepad) {
                    pGamepad->Release();
                    pGamepad = NULL;
                    s_gamepadFound = false;
                }
                static GUID targetGuid = GUID_NULL;
                targetGuid = GUID_NULL;

                dinput8->EnumDevices(
                    DI8DEVCLASS_GAMECTRL,
                    [](const DIDEVICEINSTANCE* pdidInstance, VOID* pRefGuid) -> BOOL {
                        *(GUID*)pRefGuid = pdidInstance->guidInstance;
                        return DIENUM_STOP;
                    },
                    &targetGuid, DIEDFL_ATTACHEDONLY);

                if (targetGuid != GUID_NULL) {
                    if (SUCCEEDED(dinput8->CreateDevice(targetGuid, &pGamepad, NULL))) {
                        pGamepad->SetDataFormat(&c_dfDIJoystick2);
                        pGamepad->SetCooperativeLevel(GetActiveWindow(), DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
                        DIPROPRANGE diprg;
                        diprg.diph.dwSize = sizeof(DIPROPRANGE);
                        diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
                        diprg.diph.dwHow = DIPH_DEVICE;
                        diprg.diph.dwObj = 0;
                        diprg.lMin = -10000.0f;
                        diprg.lMax = +10000.0f;
                        pGamepad->SetProperty(DIPROP_RANGE, &diprg.diph);

                        pGamepad->Acquire();
                        s_gamepadFound = true;
                    }
                }
            }
            if (pGamepad && s_gamepadFound) {
                DIJOYSTATE2 joyState;
                HRESULT hr = pGamepad->GetDeviceState(sizeof(DIJOYSTATE2), &joyState);
                if (FAILED(hr)) {
                    pGamepad->Acquire();
                    ImGui::TextColored(ImVec4(1, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_ACQUIRING_GAMEPAD));
                } else {
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), S(THPRAC_TOOLS_INPUT_TEST_OK));
                    // axes
                    ImGui::Text(S(THPRAC_TOOLS_INPUT_GAMEPAD_AXES));

                    float alx = joyState.lX;
                    float aly = joyState.lY;

                    float arx = joyState.lRx;
                    float ary = joyState.lRy;
                    float az = joyState.lZ;

                    float s1 = joyState.rglSlider[0];
                    float s2 = joyState.rglSlider[1];

                    float circle_radius = 100.0f;
                    float space = 0.25f * circle_radius;
                    ImVec2 axes_area = { circle_radius * 6.0f + space * 5.0f, circle_radius * 2.0f + space * 2.0f };
                    
                    auto p0 = ImGui::GetCursorScreenPos();
                    ImGui::InvisibleButton("x-y", axes_area);
                    ImVec2 p1 = { p0.x + axes_area.x, p0.y + axes_area.y };
                    {
                        auto p = ImGui::GetWindowDrawList();
                        p->AddRectFilled(p0, p1, IM_COL32(0,0,0,255));
                        ImVec2 circle_1_cen = { p0.x + circle_radius + space, p0.y+circle_radius + space };
                        ImVec2 circle_2_cen = { p0.x + circle_radius*3.0f + space*2.0f, p0.y+circle_radius + space };
                        ImVec2 circle_3_cen = { p0.x + circle_radius*5.0f + space*4.0f, p0.y+circle_radius + space };

                        p->AddCircleFilled(circle_1_cen, circle_radius, IM_COL32(60, 60, 60, 255));
                        p->AddCircleFilled(circle_2_cen, circle_radius, IM_COL32(60, 60, 60, 255));
                        p->AddCircleFilled(circle_3_cen, circle_radius, IM_COL32(60, 60, 60, 255));

                        p->AddRect({ circle_1_cen.x - circle_radius, circle_1_cen.y - circle_radius }, { circle_1_cen.x + circle_radius, circle_1_cen.y + circle_radius }, IM_COL32(255, 255, 255, 255), 6.0f);
                        p->AddRect({ circle_2_cen.x - circle_radius, circle_2_cen.y - circle_radius }, { circle_2_cen.x + circle_radius, circle_2_cen.y + circle_radius }, IM_COL32(255, 255, 255, 255), 6.0f);
                        p->AddRect({ circle_3_cen.x - circle_radius, circle_3_cen.y - circle_radius }, { circle_3_cen.x + circle_radius, circle_3_cen.y + circle_radius }, IM_COL32(255, 255, 255, 255), 6.0f);

                        ImVec2 circle_1 = { circle_1_cen.x + circle_radius * alx / 10000.0f, circle_1_cen.y + circle_radius * aly / 10000.0f };
                        p->AddCircleFilled(circle_1, circle_radius * 0.1f, IM_COL32(250, 150, 150, 255));

                        ImVec2 circle_2 = { circle_2_cen.x + circle_radius * arx / 10000.0f, circle_2_cen.y + circle_radius * ary / 10000.0f };
                        p->AddCircleFilled(circle_2, circle_radius*0.1f, IM_COL32(150, 250, 150, 255));
                        ImVec2 circle_3 = { 0, 0 };
                        if (joyState.rgdwPOV[0] == 0xFFFFFFFF)
                            circle_3 = { circle_3_cen.x , circle_3_cen.y};
                        else
                        {
                            float deg = joyState.rgdwPOV[0] / 100.0f;
                            float angle = deg * DirectX::XM_PI / 180.0f - DirectX::XM_PIDIV2;
                            circle_3 = { circle_3_cen.x + circle_radius * cosf(angle), circle_3_cen.y + circle_radius * sinf(angle) };
                        }
                        p->AddCircleFilled(circle_3, circle_radius * 0.1f, IM_COL32(150, 150, 250, 255));

                        p->AddText({ circle_1_cen.x - circle_radius, circle_1_cen.y + circle_radius }, 0xFFFFFFFF, S(THPRAC_TOOLS_INPUT_GAMEPAD_L_STICK));
                        p->AddText({ circle_2_cen.x - circle_radius, circle_2_cen.y + circle_radius }, 0xFFFFFFFF, S(THPRAC_TOOLS_INPUT_GAMEPAD_R_STICK));
                        p->AddText({ circle_3_cen.x - circle_radius, circle_3_cen.y + circle_radius }, 0xFFFFFFFF, S(THPRAC_TOOLS_INPUT_GAMEPAD_D_PAD));
                    }
                    ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_TRIGGER), &az, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                    if (ImGui::CollapsingHeader("Data")) {
                        float width_slider = ImGui::GetTextLineHeight();
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_L_STICK_X), &alx, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_L_STICK_Y), &aly, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_R_STICK_X), &arx, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_R_STICK_Y), &ary, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_TRIGGER), &az, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_S1), &s1, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::SliderFloat(S(THPRAC_TOOLS_INPUT_GAMEPAD_S2), &s2, -10000.0f, 10000.0f, "%.0f", ImGuiSliderFlags_::ImGuiSliderFlags_NoInput);
                        ImGui::Separator();
                        ImGui::Text(S(THPRAC_TOOLS_INPUT_GAMEPAD_D_PAD));
                        ImGui::SameLine();
                        if (joyState.rgdwPOV[0] == 0xFFFFFFFF)
                            ImGui::Text(S(THPRAC_TOOLS_INPUT_GAMEPAD_D_PAD_CENTER));
                        else
                            ImGui::Text("%s: %lu deg", S(THPRAC_TOOLS_INPUT_GAMEPAD_D_PAD_DEG),joyState.rgdwPOV[0] / 100);
                    }
                    
                    // Buttons
                    ImGui::Separator();
                    ImGui::Text(S(THPRAC_TOOLS_INPUT_TEST_BUTTONS));
                    ImGui::Columns(2);
                    for (int i = 0; i < 128; i++) {
                        if (joyState.rgbButtons[i] & 0x80) {
                            ImGui::Button((std::string("B") + std::to_string(i)).c_str());
                        }
                    }
                    ImGui::NextColumn();
                    const char* btn_strs[] = { "[A]", "[B]", "[X]", "[Y]", "[LB]", "[RB]" };
                    for (int i = 0; i < 6; i++)
                    {
                        if (joyState.rgbButtons[i] & 0x80) {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5, 0.3, 0.3, 1));
                            ImGui::Button(btn_strs[i]);
                            ImGui::PopStyleColor(1);
                        } else {
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.5, 0.3, 1));
                            ImGui::Button(btn_strs[i]);
                            ImGui::PopStyleColor(1);
                        }
                        if (i % 2 == 0)
                            ImGui::SameLine();
                    }
                    ImGui::Columns(1);
                }
            } else {
                ImGui::TextDisabled(S(THPRAC_TOOLS_INPUT_GAMEPAD_NO_GAMEPAD));
            }
        }
        return result;
    }

private:
};

std::wstring NormalizePath(std::wstring path)
{
    std::replace(path.begin(), path.end(), L'/', L'\\');
    std::transform(path.begin(), path.end(), path.begin(), ::towupper);
    return path;
}

std::vector<std::wstring> THClearGame()
{
    std::vector<std::wstring> gameKilled;

    auto paths = GetAllGamePaths();
    std::vector<std::wstring> normalizedList;
    for (const auto& path : paths) {
        normalizedList.push_back(NormalizePath(path));
    }
    if (paths.empty())
        return gameKilled;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return gameKilled;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(PROCESSENTRY32W);
    if (Process32FirstW(hSnapshot, &pe)) {
        do {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
            if (hProcess) {
                wchar_t processPath[MAX_PATH];
                DWORD dwSize = MAX_PATH;
                if (QueryFullProcessImageNameW(hProcess, 0, processPath, &dwSize)) {
                    std::wstring currentPath = NormalizePath(processPath);
                    auto it = std::find(normalizedList.begin(), normalizedList.end(), currentPath);
                    if (it != normalizedList.end()) {
                        if (TerminateProcess(hProcess, 9)) {
                            gameKilled.push_back(currentPath);
                        }
                    }
                }
                CloseHandle(hProcess);
            }
        } while (Process32NextW(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
    return gameKilled;
}

class DiceBase {
public:
    DirectX::XMFLOAT3 pos;
    DirectX::XMMATRIX rot;
    
    DirectX::XMFLOAT4 q_last;
    DirectX::XMFLOAT4 q_final;
    DirectX::XMFLOAT4 q_start_tumble_axis;
    DirectX::XMFLOAT4 q_final_tumble_axis;

    DirectX::XMFLOAT3 eye_f;
    DirectX::XMFLOAT3 at_f;
    DirectX::XMFLOAT3 up_f;

    float total_spin;

protected:
    float size;
    struct FaceDef {
        int id;
        std::vector<int> v_indices;
        DirectX::XMFLOAT3 norm;
    };
    std::vector<DirectX::XMFLOAT3> m_vertices;
    std::vector<FaceDef> m_faces;

public:
    int GetType()
    {
        return m_faces.size();
    }

    DiceBase(float scale)
        : size(scale)
        , total_spin(0.0f)
    {
        using namespace DirectX;
        static auto distYaw = GetRndGenerator(XM_2PI / 4.0f - 0.2f, XM_2PI / 4.0f + 0.2f);

        pos = XMFLOAT3(0, 0, 0);
        rot = XMMatrixRotationY(distYaw());
        XMStoreFloat4(&q_last, XMQuaternionRotationMatrix(rot));
        XMStoreFloat4(&q_final, XMQuaternionRotationMatrix(rot));
        XMStoreFloat4(&q_start_tumble_axis, XMQuaternionIdentity());
        XMStoreFloat4(&q_final_tumble_axis, XMQuaternionIdentity());

        
        eye_f = XMFLOAT3(18, 35, 0);
        at_f = XMFLOAT3(0, 0, 0);
        up_f = XMFLOAT3(0, 1, 0);
    }
    virtual ~DiceBase() = default;

    int GetResult()
    {
        using namespace DirectX;
        XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        int max_id = -1;
        float max_dot = -2.0f;
        for (const auto& face : m_faces) {
            XMVECTOR localNorm = XMLoadFloat3(&face.norm);
            XMVECTOR worldNorm = XMVector3TransformNormal(localNorm, rot);
            float dotVal = XMVectorGetX(XMVector3Dot(worldNorm, vUp));
            if (dotVal > max_dot) {
                max_dot = dotVal;
                max_id = face.id;
            }
        }
        return max_id;
    }
    void DragDrop(float dx, float dy)
    {
        using namespace DirectX;
        CXMMATRIX viewInverse = XMMatrixInverse(nullptr,GetView());
        float dragLen = std::sqrt(dx * dx + dy * dy);
        if (dragLen < 20.0f) {
            Drop();
            return;
        }
        XMVECTOR qCurrent = XMQuaternionRotationMatrix(rot);
        XMStoreFloat4(&q_last, qCurrent);

        XMVECTOR axisView = XMVectorSet(dy, dx, 0.0f, 0.0f);
        XMVECTOR axisWorld = XMVector3TransformNormal(axisView, viewInverse);
        axisWorld = XMVector3Normalize(axisWorld);
        
        static auto distJitter = GetRndGenerator(-0.2f, 0.2f);
        float baseTurns = XM_2PI * 0.2f;
        float speedFactor = 0.015f;
        float finalSpinAngle = (baseTurns * XM_2PI) + (dragLen * speedFactor) + distJitter();
        
        XMVECTOR qSpinDelta = XMQuaternionRotationAxis(axisWorld, finalSpinAngle);
        XMVECTOR qPredicted = XMQuaternionMultiply(qCurrent, qSpinDelta);
        XMMATRIX mPredicted = XMMatrixRotationQuaternion(qPredicted);

        XMVECTOR vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        int bestFaceIdx = -1;
        float max_dot = -2.0f;
        for (int i = 0; i < m_faces.size(); i++) {
            XMVECTOR localNorm = XMLoadFloat3(&m_faces[i].norm);
            XMVECTOR worldNorm = XMVector3TransformNormal(localNorm, mPredicted);
            float dotVal = XMVectorGetX(XMVector3Dot(worldNorm, vUp));
            if (dotVal > max_dot) {
                max_dot = dotVal;
                bestFaceIdx = i;
            }
        }

        if (bestFaceIdx == -1)
            bestFaceIdx = 0; 

        XMVECTOR vFaceNorm = XMLoadFloat3(&m_faces[bestFaceIdx].norm);
        XMVECTOR vNormRough = XMVector3TransformNormal(vFaceNorm, mPredicted);
        XMVECTOR qCorrection = GetQuatFromTo(vNormRough, vUp);

        XMVECTOR qFinalVec = XMQuaternionMultiply(qPredicted, qCorrection);

        XMStoreFloat4(&q_final, qFinalVec);

        XMVECTOR qSpinInv = XMQuaternionInverse(qSpinDelta);
        XMVECTOR qRewoundStart = XMQuaternionMultiply(qCurrent, qSpinInv);
        XMStoreFloat4(&q_last, qRewoundStart);
        
        XMVECTOR vRefY = XMVectorSet(0, 1, 0, 0);
        XMVECTOR qAxisStart = GetQuatFromTo(vRefY, axisWorld);

        static auto distPerturb = GetRndGenerator(-0.1f, 0.1f);
        XMVECTOR axisEnd = XMVector3Normalize(axisWorld + XMVectorSet(distPerturb(), distPerturb(), distPerturb(), 0));
        XMVECTOR qAxisEnd = GetQuatFromTo(vRefY, axisEnd);
        XMStoreFloat4(&q_start_tumble_axis, qAxisStart);
        XMStoreFloat4(&q_final_tumble_axis, qAxisEnd);
        
        total_spin = finalSpinAngle;
    }

    void Drop()
    {
        using namespace DirectX;
        if (m_faces.empty())
            return;
        XMVECTOR qCurrent = XMQuaternionRotationMatrix(rot);
        XMStoreFloat4(&q_last, qCurrent);
        static auto distFaceIdx = GetRndGenerator(0.0f, 1.0f);
        static auto distYaw = GetRndGenerator(0.0f, XM_2PI);
        static auto distAxis = GetRndGeneratorNormal(-1.0f, 1.0f);
        static auto distSpin = GetRndGenerator(XM_2PI * 5.0f, XM_2PI * 8.0f);
        
        int rndIdx = floorf(distFaceIdx() * (int)m_faces.size());
        if (rndIdx == m_faces.size())
            rndIdx = m_faces.size()-1;

        XMVECTOR vFaceNorm = XMLoadFloat3(&m_faces[rndIdx].norm);
        XMVECTOR vUp = XMVectorSet(0, 1, 0, 0);
        XMVECTOR qAlign = GetQuatFromTo(vFaceNorm, vUp);
        
        XMVECTOR qYaw = XMQuaternionRotationAxis(vUp, distYaw());
        XMVECTOR qFinalVec = XMQuaternionMultiply(qAlign, qYaw);
        XMStoreFloat4(&q_final, qFinalVec);
       
        XMVECTOR axisStart = XMVector3Normalize(XMVectorSet(distAxis(), distAxis(), distAxis(), 0.0f));
        XMVECTOR axisEnd = XMVector3Normalize(XMVectorSet(distAxis(), distAxis(), distAxis(), 0.0f));

        XMStoreFloat4(&q_start_tumble_axis, XMQuaternionRotationAxis(axisStart, distSpin()));
        XMStoreFloat4(&q_final_tumble_axis, XMQuaternionRotationAxis(axisEnd, distSpin()));
        total_spin = distSpin();
    }

    virtual std::vector<ImVec2> GetUV(int faceID) = 0;

    void Update(float t)
    {
        using namespace DirectX;
        t = std::max(0.0f, std::min(t, 1.0f));

        XMVECTOR qStart = XMLoadFloat4(&q_last);
        XMVECTOR qEnd = XMLoadFloat4(&q_final);
        float t_rot = t * t * (3.0f - 2.0f * t);
        XMVECTOR qBase = XMQuaternionSlerp(qStart, qEnd, t_rot);
       
        qStart = XMLoadFloat4(&q_start_tumble_axis);
        qEnd = XMLoadFloat4(&q_final_tumble_axis);
        XMVECTOR qBase_axis = XMQuaternionSlerp(qStart, qEnd, t_rot);
       
        float spinAngle = total_spin * std::pow(1.0f - t, 2.0f);

        XMVECTOR vAxis = XMVectorSet(0, 1, 0, 0);
        vAxis = XMVector3Rotate(vAxis, qBase_axis);
        
        XMVECTOR qSpin = XMQuaternionRotationAxis(vAxis, spinAngle);
        XMVECTOR qResult = XMQuaternionMultiply(qBase, qSpin);

        rot = XMMatrixRotationQuaternion(qResult);
    }

    DirectX::XMMATRIX GetView()
    {
        using namespace DirectX;
        XMVECTOR vEye = XMLoadFloat3(&eye_f);
        XMVECTOR vAt = XMLoadFloat3(&at_f);
        XMVECTOR vUp = XMLoadFloat3(&up_f);
        XMMATRIX mView = XMMatrixLookAtLH(vEye, vAt, vUp);
        return mView;
    }

    void Render(std::vector<ImVec2>& points, std::vector<ImVec2>& uvs, std::vector<int>& dice_num)
    {
        using namespace DirectX;
        points.clear();
        uvs.clear();
        dice_num.clear();
        XMMATRIX mView = GetView();
        XMMATRIX mProj = XMMatrixOrthographicLH(80.0f, 80.0f, 0.1f, 100.0f);
        XMMATRIX mVP = XMMatrixMultiply(mView, mProj);
        XMMATRIX mTrans = XMMatrixTranslation(pos.x, pos.y, pos.z);
        XMMATRIX mWorld = XMMatrixMultiply(rot, mTrans);
        auto viewDir_f = XMFLOAT3(-eye_f.x, -eye_f.y, -eye_f.z);
        XMVECTOR viewDir = XMLoadFloat3(&viewDir_f);

        for (const auto& face : m_faces) {
            auto norm = XMLoadFloat3(&face.norm);
            auto wNorm = XMVector3TransformNormal(norm, rot);
            XMFLOAT3 dotval;
            XMStoreFloat3(&dotval, XMVector3Dot(wNorm, viewDir));
            if (dotval.x <= 0) {
                dice_num.push_back(face.id);

                std::vector<ImVec2> faceUVs = GetUV(face.id);
                for (size_t i = 0; i < face.v_indices.size(); i++) {
                    int v_idx = face.v_indices[i];

                    XMVECTOR vPos = XMLoadFloat3(&m_vertices[v_idx]);
                    vPos = XMVectorScale(vPos, size);
                    auto vw = XMVector3TransformCoord(vPos, mWorld);
                    auto vp = XMVector3TransformCoord(vw, mVP);
                    XMFLOAT3 vp_f;
                    XMStoreFloat3(&vp_f, vp);
                    points.push_back({ (vp_f.x + 1.0f) * 0.5f, (1.0f - vp_f.y) * 0.5f });
                    if (i < faceUVs.size()) {
                        uvs.push_back(faceUVs[i]);
                    } else {
                        uvs.push_back({ 0, 0 });
                    }
                }
            }
        }
    }

protected:
    DirectX::XMVECTOR GetQuatFromTo(DirectX::XMVECTOR u, DirectX::XMVECTOR v)
    {
        using namespace DirectX;
        u = XMVector3Normalize(u);
        v = XMVector3Normalize(v);
        float d = XMVectorGetX(XMVector3Dot(u, v));
        if (d >= 1.0f - 1e-6f)
            return XMQuaternionIdentity();
        if (d <= -1.0f + 1e-6f) {
            XMVECTOR axis = XMVector3Cross(XMVectorSet(1, 0, 0, 0), u);
            if (XMVectorGetX(XMVector3LengthSq(axis)) < 1e-6f)
                axis = XMVector3Cross(XMVectorSet(0, 1, 0, 0), u);
            return XMQuaternionRotationAxis(XMVector3Normalize(axis), XM_PI);
        }
        XMVECTOR axis = XMVector3Cross(u, v);
        float s = sqrtf((1.0f + d) * 2.0f);
        float invs = 1.0f / s;
        return XMQuaternionNormalize(XMVectorSet(
            XMVectorGetX(axis) * invs,
            XMVectorGetY(axis) * invs,
            XMVectorGetZ(axis) * invs,
            s * 0.5f));
    }
    void ComputeFaceNormal(FaceDef& face)
    {
        using namespace DirectX;
        if (face.v_indices.size() < 3)
            return;
        XMVECTOR v0 = XMLoadFloat3(&m_vertices[face.v_indices[0]]);
        XMVECTOR v1 = XMLoadFloat3(&m_vertices[face.v_indices[1]]);
        XMVECTOR v2 = XMLoadFloat3(&m_vertices[face.v_indices[2]]);
        XMVECTOR n = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
        XMStoreFloat3(&face.norm, n);
    }
};
class Dice6 : public DiceBase {
public:
    Dice6()
        : DiceBase(12.0f)
    {
        InitGeometry();
    }

private:
    void InitGeometry()
    {
        m_vertices = {
            { -1, 1, 1 }, { 1, 1, 1 }, { 1, 1, -1 }, { -1, 1, -1 }, 
            { -1, -1, 1 }, { 1, -1, 1 }, { 1, -1, -1 }, { -1, -1, -1 } 
        };
        // Y+
        FaceDef f1;
        f1.id = 1;
        f1.v_indices = { 0, 1, 2, 3 };
        ComputeFaceNormal(f1);
        m_faces.push_back(f1);
        // Y-
        FaceDef f6;
        f6.id = 6;
        f6.v_indices = { 7, 6, 5, 4 };
        ComputeFaceNormal(f6);
        m_faces.push_back(f6);
        // X+
        FaceDef f2;
        f2.id = 2;
        f2.v_indices = { 2, 1, 5, 6 };
        ComputeFaceNormal(f2);
        m_faces.push_back(f2);
        // X-
        FaceDef f5;
        f5.id = 5;
        f5.v_indices = { 0, 3, 7, 4 };
        ComputeFaceNormal(f5);
        m_faces.push_back(f5);
        // Z+
        FaceDef f3;
        f3.id = 3;
        f3.v_indices = { 1, 0, 4, 5 };
        ComputeFaceNormal(f3);
        m_faces.push_back(f3);
        // Z-
        FaceDef f4;
        f4.id = 4;
        f4.v_indices = { 3, 2, 6, 7 };
        ComputeFaceNormal(f4);
        m_faces.push_back(f4);
    }

    std::vector<ImVec2> GetUV(int faceID) override
    {
        std::vector<ImVec2> uvs;

        float step = 1.0f / 6.0f;
        float u_min = (faceID - 1) * step;
        float u_max = faceID * step;
        float v_min = 0.0f;
        float v_max = 1.0f;

        uvs.push_back({ u_min, v_min }); // TL
        uvs.push_back({ u_max, v_min }); // TR
        uvs.push_back({ u_max, v_max }); // BR
        uvs.push_back({ u_min, v_max }); // BL

        return uvs;
    }
};
class Dice12 : public DiceBase {
public:
    Dice12()
        : DiceBase(12.0f)
    {
        InitGeometry();
    }

private:
    void InitGeometry()
    {
        using namespace DirectX;
        m_vertices.clear();
        m_faces.clear();
        float phi = 1.61803398875f;
        float inv_phi = 1.0f / phi;
        std::vector<XMFLOAT3> temp_verts = {
            { -1, -1, -1 }, { -1, -1, 1 }, { -1, 1, -1 }, { -1, 1, 1 },
            { 1, -1, -1 }, { 1, -1, 1 }, { 1, 1, -1 }, { 1, 1, 1 },
            { 0, -phi, -inv_phi }, { 0, -phi, inv_phi }, { 0, phi, -inv_phi }, { 0, phi, inv_phi },
            { -inv_phi, 0, -phi }, { -inv_phi, 0, phi }, { inv_phi, 0, -phi }, { inv_phi, 0, phi },
            { -phi, -inv_phi, 0 }, { -phi, inv_phi, 0 }, { phi, -inv_phi, 0 }, { phi, inv_phi, 0 }
        };
        std::vector<std::vector<int>> face_inds = {
            { 13,15,  7,   11,  3,     }, // Face 1: Top-Front
            { 19, 6,  10,  11,  7,     }, // Face 2
            {  2, 17,  3,   11, 10,    }, // Face 3
            { 12, 0,  16,  17,  2,     }, // Face 4
            { 8 ,9,   1,   16,  0,     }, // Face 5
            { 9 , 5,  15,  13,  1,     }, // Face 6
            {  5,9,   8,    4,  18,    }, // Face 7
            { 14, 6,  19,  18,  4,     }, // Face 8
            { 12, 2,  10,   6,  14,    }, // Face 9
            { 14,4,   8,    0,  12,    }, // Face 10: Bottom-Back
            {  7, 15,  5,   18, 19,    }, // Face 11
            {  1, 13,  3,   17, 16,    } // Face 12
        };
        XMVECTOR v0 = XMLoadFloat3(&temp_verts[face_inds[0][0]]);
        XMVECTOR v1 = XMLoadFloat3(&temp_verts[face_inds[0][1]]);
        XMVECTOR v2 = XMLoadFloat3(&temp_verts[face_inds[0][2]]);
        XMVECTOR face1Norm = XMVector3Normalize(XMVector3Cross(v1 - v0, v2 - v0));
        XMVECTOR vUp = XMVectorSet(0, 1, 0, 0);
        XMVECTOR qAlign = GetQuatFromTo(face1Norm, vUp);
        XMMATRIX mAlign = XMMatrixRotationQuaternion(qAlign);
        for (auto& v : temp_verts) {
            XMVECTOR vPos = XMLoadFloat3(&v);
            vPos = XMVector3TransformCoord(vPos, mAlign);
            XMStoreFloat3(&v, vPos);
        }
        m_vertices = temp_verts;
        for (int i = 0; i < 12; i++) {
            FaceDef fd;
            fd.id = i + 1;
            fd.v_indices = face_inds[i];
            ComputeFaceNormal(fd);
            m_faces.push_back(fd);
        }
    }

    std::vector<ImVec2> GetUV(int faceID) override
    {
        std::vector<ImVec2> uvs;
        using namespace DirectX;
        int idx = faceID - 1;
        int col = idx % 4;
        int row = idx / 4;
        float cell_w = 1.0f / 4.0f;
        float cell_h = 1.0f / 3.0f;

        float u_start = col * cell_w;
        float v_start = row * cell_h;

        ImVec2 pts[5];
        for (int i = 0; i < 5; i++)
        {
            float angle = -XM_PIDIV2 + i * (XM_2PI / 5.0f);
            pts[i].x = cosf(angle);
            pts[i].y = sinf(angle);
        }
        float k = 0.5f / pts[1].x;
        float dx = 0.5f;
        float dy = 1.0f - k * pts[2].y;
        
        for (int i = 0; i < 5; i++) {
            float final_u = u_start + (pts[i].x * k + dx) * cell_w;
            float final_v = v_start + (pts[i].y * k + dy) * cell_h;
            uvs.push_back({ final_u, final_v });
        }
        return uvs;
    }
};

class THGuiDice {
public:
    struct DiceT
    {
        std::unique_ptr<DiceBase> dice;
        bool is_playing;
        float t;
    };
    std::vector<DiceT> dices;
    int max_dice = 16;

    ImTextureID tex6;
    ImTextureID tex12;

    THGuiDice()
        : dices()
    {
        dices.push_back({ std::make_unique<Dice6>(), false, 0.0f});
        InitImage();
    }

    void InitImage()
    {
       const uint8_t data6[575] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
    0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x20, 0x08, 0x03, 0x00, 0x00, 0x00, 0x73, 0x46, 0x87, 
    0xB7, 0x00, 0x00, 0x00, 0x1B, 0x50, 0x4C, 0x54, 0x45, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x00, 0x00, 0xAA, 0x00, 0x00, 0xFF, 0x55, 0x55, 0xFF, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 
    0x55, 0xFF, 0xFF, 0xFF, 0xE0, 0x29, 0xF2, 0x03, 0x00, 0x00, 0x00, 0x09, 0x74, 0x52, 0x4E, 0x53, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x53, 0x4F, 0x78, 0x12, 0x00, 0x00, 0x00, 
    0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x17, 0x12, 0x00, 0x00, 0x17, 0x12, 0x01, 0x67, 0x9F, 
    0xD2, 0x52, 0x00, 0x00, 0x01, 0xB5, 0x49, 0x44, 0x41, 0x54, 0x58, 0x85, 0xD5, 0x57, 0xD1, 0x92, 
    0x83, 0x20, 0x0C, 0xDC, 0xD4, 0x3B, 0xFC, 0xFF, 0xCF, 0xC5, 0x4E, 0x2F, 0xF7, 0xA0, 0xD6, 0x08, 
    0x24, 0x44, 0xAC, 0x63, 0xD9, 0x99, 0xBB, 0x4A, 0x42, 0x20, 0x81, 0x90, 0x05, 0x22, 0xF4, 0x8D, 
    0xC7, 0xDD, 0x0E, 0x9C, 0x45, 0xF7, 0x01, 0xFC, 0xDC, 0xED, 0x80, 0x0F, 0x0C, 0x50, 0x59, 0xD0, 
    0xB8, 0x03, 0x81, 0x99, 0xC3, 0x59, 0xAF, 0xFC, 0xE0, 0xE5, 0xAF, 0x20, 0x68, 0x0B, 0x80, 0x23, 
    0x80, 0x68, 0x45, 0xC0, 0xCC, 0x46, 0xF3, 0xB0, 0x1E, 0x00, 0x42, 0xFA, 0x19, 0x80, 0x73, 0x67, 
    0x20, 0xEA, 0xAA, 0x64, 0xC9, 0x8C, 0x15, 0x74, 0xE9, 0x0D, 0x34, 0x05, 0x10, 0x92, 0x5F, 0xA5, 
    0x43, 0x28, 0x09, 0x75, 0xC3, 0xF2, 0x0A, 0x6F, 0x98, 0xD2, 0xCF, 0x09, 0xE8, 0xA5, 0x0A, 0x11, 
    0x92, 0x43, 0xBC, 0x09, 0xA8, 0x85, 0xC8, 0xC2, 0x92, 0x3C, 0xE3, 0xA4, 0xF5, 0xE0, 0x75, 0x96, 
    0x62, 0xF3, 0xB0, 0xDE, 0x40, 0x12, 0x00, 0x11, 0xF2, 0xD3, 0xA3, 0xF8, 0x67, 0xCE, 0xC0, 0x99, 
    0x43, 0x4A, 0x19, 0x7C, 0x37, 0xF7, 0xAB, 0x91, 0x19, 0x68, 0xD8, 0xA5, 0xD0, 0xF0, 0x20, 0x80, 
    0x1E, 0x75, 0x4B, 0x12, 0xFF, 0x95, 0x1E, 0x81, 0x8C, 0x66, 0x49, 0x3F, 0x59, 0x7A, 0x63, 0x22, 
    0xD1, 0x6F, 0x78, 0xAF, 0xFD, 0x9F, 0xCF, 0xF8, 0x1B, 0x20, 0x77, 0x60, 0xCB, 0x9D, 0xA1, 0x75, 
    0xB8, 0x70, 0x92, 0xDF, 0x34, 0x7B, 0x9D, 0x27, 0x44, 0x00, 0xE2, 0x56, 0xE1, 0x2B, 0xC1, 0x85, 
    0xF9, 0x63, 0x85, 0xDF, 0x1A, 0xED, 0x0D, 0x9E, 0x10, 0x29, 0x24, 0x37, 0xA3, 0x31, 0x87, 0x96, 
    0x31, 0xDB, 0x8C, 0x0D, 0xFB, 0x54, 0x3E, 0x97, 0xC1, 0x71, 0x42, 0x2F, 0x3C, 0x60, 0x40, 0x04, 
    0xD0, 0x9A, 0x37, 0x02, 0xE3, 0x95, 0xF6, 0x42, 0x59, 0x66, 0x62, 0x11, 0x40, 0x6B, 0x12, 0x4C, 
    0xE3, 0x09, 0x63, 0xC3, 0x9E, 0x90, 0xF0, 0x44, 0x99, 0x89, 0xB7, 0xEF, 0xFC, 0x08, 0xFC, 0x3E, 
    0xDB, 0xDD, 0xFA, 0x04, 0xB2, 0xF9, 0x57, 0xC1, 0x8E, 0x89, 0xD7, 0x46, 0xE6, 0x7F, 0x88, 0x38, 
    0xB5, 0xB2, 0x17, 0x62, 0x77, 0x88, 0x99, 0x18, 0x00, 0x95, 0xFD, 0x37, 0xCF, 0x48, 0xA8, 0x5C, 
    0x40, 0xB2, 0x3A, 0x1E, 0xF6, 0xF5, 0xBE, 0xA6, 0xD7, 0xE1, 0xBA, 0xCC, 0x2D, 0x83, 0xBB, 0xEF, 
    0x6E, 0x75, 0xFD, 0xE7, 0x2E, 0x73, 0x9E, 0x32, 0xBA, 0xAE, 0x85, 0xF1, 0x80, 0x91, 0xDD, 0x1C, 
    0xFA, 0xC3, 0xEF, 0x01, 0x7D, 0x87, 0xFA, 0xE0, 0x81, 0x9C, 0x89, 0x23, 0xE2, 0x2C, 0x18, 0x1C, 
    0xFB, 0xF4, 0x5A, 0x7E, 0xC7, 0x97, 0xD9, 0xAD, 0xC6, 0xDE, 0x42, 0x3F, 0x0F, 0x44, 0x15, 0x7D, 
    0x32, 0x60, 0x36, 0x3D, 0x01, 0xCE, 0x1D, 0x58, 0x28, 0x44, 0x3D, 0x02, 0xF9, 0x8B, 0xA9, 0xA6, 
    0x4F, 0x04, 0x99, 0xBE, 0xC2, 0x27, 0x22, 0xC5, 0x7C, 0x2F, 0xB2, 0x0B, 0xCA, 0x68, 0x8D, 0x58, 
    0x76, 0x7A, 0xE3, 0x90, 0x7B, 0x9F, 0x94, 0x37, 0x13, 0x99, 0xFE, 0xA2, 0x6B, 0x7A, 0x13, 0x7F, 
    0x03, 0x8A, 0x4C, 0xDC, 0x23, 0xFA, 0x28, 0xA3, 0x06, 0xFE, 0x01, 0xD1, 0x67, 0x97, 0xE3, 0xD0, 
    0xE2, 0xF1, 0x39, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 
};
       const uint8_t data12[1227] = {
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
    0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x60, 0x08, 0x03, 0x00, 0x00, 0x00, 0xE2, 0xA4, 0x8A, 
    0x7B, 0x00, 0x00, 0x00, 0x27, 0x50, 0x4C, 0x54, 0x45, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 
    0x55, 0xAA, 0x55, 0x55, 0xFF, 0xAA, 0xAA, 0xFF, 0x00, 0x55, 0xFF, 0x55, 0xAA, 0xFF, 0x00, 0xAA, 
    0xFF, 0x00, 0xAA, 0xAA, 0x55, 0xFF, 0xFF, 0x55, 0xAA, 0xAA, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x36, 0xC8, 0xC8, 0x08, 0x00, 0x00, 0x00, 0x0D, 0x74, 0x52, 0x4E, 0x53, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3D, 0xE8, 0x22, 0x86, 0x00, 0x00, 0x00, 
    0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x17, 0x12, 0x00, 0x00, 0x17, 0x12, 0x01, 0x67, 0x9F, 
    0xD2, 0x52, 0x00, 0x00, 0x04, 0x31, 0x49, 0x44, 0x41, 0x54, 0x68, 0x81, 0xED, 0x9A, 0xEB, 0x8E, 
    0xE3, 0x36, 0x0C, 0x85, 0x3F, 0x59, 0x17, 0x4F, 0xF4, 0xFE, 0xCF, 0xAA, 0xAC, 0x4D, 0x5D, 0xFA, 
    0xC3, 0x71, 0x62, 0x27, 0xA2, 0x66, 0x8B, 0x62, 0x31, 0x6D, 0x61, 0x62, 0x81, 0x35, 0x4C, 0x51, 
    0x3A, 0x22, 0x29, 0x0D, 0x0F, 0x1D, 0x33, 0xF1, 0xB3, 0xF2, 0xD3, 0xEB, 0x5F, 0x00, 0x2E, 0x00, 
    0x17, 0x80, 0x0B, 0xC0, 0x7F, 0x0E, 0x80, 0xF9, 0x87, 0xFA, 0x4F, 0xB1, 0x27, 0x93, 0x10, 0xC4, 
    0x57, 0x75, 0xAC, 0x2B, 0x34, 0x22, 0xDA, 0x80, 0x60, 0x0A, 0x0D, 0x06, 0x13, 0xE0, 0xE6, 0x0F, 
    0x6B, 0x73, 0x72, 0x81, 0x13, 0xAC, 0x6A, 0xEF, 0x84, 0x48, 0x02, 0x9F, 0xFB, 0xEB, 0xDB, 0x34, 
    0xD4, 0x03, 0x61, 0xF9, 0xD4, 0xBE, 0xAF, 0xAF, 0xC7, 0x24, 0x08, 0xB1, 0xDC, 0x03, 0xF8, 0xBE, 
    0xBE, 0x26, 0xFC, 0xFD, 0x1E, 0x40, 0xB4, 0x19, 0xB0, 0x10, 0x7D, 0x6B, 0x27, 0xA7, 0x9F, 0x42, 
    0x50, 0x40, 0xF7, 0x80, 0xA9, 0x20, 0x54, 0x10, 0x25, 0xD0, 0x0D, 0xA0, 0xA2, 0x27, 0x82, 0x5B, 
    0x80, 0xE5, 0x4D, 0x7F, 0xDC, 0xB0, 0xB1, 0x2A, 0xF6, 0x4D, 0x2A, 0x04, 0x55, 0xB9, 0xCD, 0xE4, 
    0x74, 0xEB, 0x20, 0xE0, 0xCD, 0x8E, 0xB4, 0x07, 0xA0, 0xB9, 0x08, 0x6A, 0x00, 0x9F, 0x18, 0x14, 
    0x29, 0x3E, 0x58, 0x77, 0x13, 0x35, 0x44, 0x9B, 0x73, 0x3E, 0xEC, 0xCF, 0x49, 0xD8, 0x46, 0x07, 
    0xC9, 0xB4, 0x7E, 0x1A, 0xBD, 0xE4, 0x96, 0x40, 0x4F, 0xD2, 0x05, 0x7C, 0x76, 0x32, 0x08, 0xC1, 
    0x37, 0xD2, 0x20, 0x58, 0xDD, 0xC4, 0xB9, 0x70, 0x37, 0x1E, 0x3D, 0x49, 0x77, 0xD3, 0x78, 0x7A, 
    0xFD, 0xB7, 0x2E, 0xA2, 0x60, 0x13, 0xF8, 0xB5, 0xAF, 0x14, 0xB1, 0x90, 0x3D, 0xA4, 0xBE, 0xAD, 
    0x40, 0x58, 0x77, 0x24, 0x0A, 0x80, 0x41, 0x06, 0x01, 0x90, 0x87, 0xA7, 0x9C, 0xC4, 0x96, 0x60, 
    0xDD, 0x28, 0x5A, 0x88, 0xCD, 0xB8, 0x0F, 0x17, 0x9D, 0x8E, 0xE1, 0x54, 0x89, 0x83, 0x24, 0x9C, 
    0xCA, 0x70, 0x7D, 0x98, 0x8B, 0x7A, 0x8A, 0xCD, 0x0A, 0x52, 0x5B, 0x15, 0xC8, 0x73, 0x3D, 0x9C, 
    0x83, 0xF7, 0x4D, 0xEB, 0xB7, 0x08, 0x53, 0x01, 0xDB, 0x54, 0xB5, 0x17, 0x6A, 0x68, 0x4E, 0x14, 
    0x8C, 0xDB, 0xB6, 0xED, 0x2F, 0xF8, 0x92, 0xE3, 0x24, 0x27, 0x00, 0x83, 0x5B, 0x7C, 0xBB, 0x26, 
    0x29, 0x40, 0xFC, 0xD5, 0x45, 0x91, 0x6D, 0x11, 0x58, 0x95, 0xF5, 0xDB, 0xF6, 0xD6, 0x00, 0xF5, 
    0x34, 0xC0, 0xBC, 0xDD, 0x44, 0x45, 0xDD, 0xA2, 0x7B, 0xC2, 0xD3, 0x70, 0xBA, 0x3A, 0x23, 0x93, 
    0x92, 0xA3, 0xFB, 0x18, 0xEA, 0xD9, 0xDC, 0xFC, 0x74, 0x41, 0xF0, 0xD3, 0xEB, 0x5F, 0x00, 0x2E, 
    0x00, 0x17, 0x80, 0x0B, 0xC0, 0x05, 0xE0, 0x02, 0x70, 0x01, 0xF8, 0x3F, 0x02, 0x18, 0xB5, 0x08, 
    0xCC, 0xA7, 0xF2, 0x50, 0x15, 0xDF, 0x24, 0x5A, 0x33, 0x4D, 0xD3, 0xA4, 0x97, 0x86, 0xA1, 0x55, 
    0xAD, 0x2C, 0x7D, 0x5A, 0x35, 0xAD, 0x43, 0x10, 0x82, 0xB4, 0x06, 0xDE, 0x95, 0xE3, 0xDB, 0x57, 
    0x51, 0xEA, 0xD2, 0x83, 0x52, 0xE8, 0x95, 0x79, 0xB0, 0x0B, 0x45, 0x29, 0xBC, 0xA7, 0x79, 0xFB, 
    0xBF, 0x4F, 0x4B, 0xD8, 0xA8, 0xD9, 0x57, 0xB1, 0xBF, 0xE4, 0xCC, 0x8C, 0x4E, 0x55, 0x71, 0x14, 
    0x43, 0xE6, 0xAE, 0x4D, 0xE1, 0x16, 0x74, 0x62, 0x90, 0xF3, 0x06, 0x11, 0x2D, 0xAC, 0x19, 0xFC, 
    0x82, 0xF1, 0x1E, 0xA3, 0x96, 0xE5, 0xD0, 0xB6, 0xEA, 0xBB, 0x2B, 0x61, 0xB4, 0xFE, 0x2E, 0x3A, 
    0x77, 0x2B, 0x80, 0x35, 0xA5, 0x88, 0x3F, 0x05, 0xF1, 0x05, 0xC0, 0xCB, 0xE6, 0x3E, 0xEB, 0xFA, 
    0x13, 0x84, 0x05, 0xC0, 0x33, 0xAC, 0xBB, 0x2B, 0x2A, 0xC1, 0xB7, 0xA5, 0xCB, 0x7A, 0x5E, 0xEE, 
    0x4A, 0x80, 0xF7, 0x91, 0xA2, 0xB4, 0x29, 0xB6, 0xD0, 0x27, 0x59, 0xBE, 0xEB, 0x41, 0x28, 0xBA, 
    0xDD, 0x2C, 0x9E, 0x72, 0xF0, 0x14, 0xAF, 0x38, 0xE7, 0x5C, 0x20, 0x75, 0x57, 0x08, 0x02, 0x76, 
    0x36, 0x1E, 0x44, 0xEF, 0x92, 0x3C, 0x29, 0xF8, 0x48, 0xCE, 0x03, 0x5E, 0xC7, 0xF0, 0x2B, 0x88, 
    0x40, 0x89, 0x4A, 0xA3, 0xCC, 0x54, 0xA2, 0x59, 0xA9, 0xBE, 0xA2, 0x9E, 0xD3, 0xB0, 0x82, 0xD7, 
    0x72, 0x28, 0x43, 0xF4, 0x02, 0x62, 0xFA, 0x2D, 0x1A, 0x37, 0xB1, 0xFB, 0x46, 0x9B, 0xBF, 0xAE, 
    0xDB, 0x3C, 0x9A, 0x93, 0x87, 0x0E, 0x70, 0x40, 0xBE, 0xCF, 0x1E, 0xB4, 0x10, 0xA4, 0x64, 0x3F, 
    0xDE, 0x9D, 0xA5, 0x00, 0x04, 0x58, 0x14, 0x7D, 0x10, 0xBD, 0x7D, 0x01, 0x10, 0x85, 0x75, 0x8A, 
    0x83, 0x16, 0x8D, 0x05, 0x42, 0xD2, 0x01, 0x78, 0x80, 0xAA, 0xEB, 0x87, 0x19, 0xF0, 0x50, 0xAC, 
    0xBC, 0x75, 0xC9, 0x5E, 0xE4, 0xF4, 0x96, 0xF0, 0xF9, 0xEB, 0xAE, 0x1E, 0xF5, 0xA9, 0xE0, 0x27, 
    0xEA, 0xA0, 0x97, 0xDA, 0x06, 0x5D, 0x46, 0xD5, 0xFE, 0x05, 0x20, 0xD8, 0xED, 0x16, 0x55, 0xAF, 
    0x9A, 0xA9, 0x8C, 0xF5, 0xDC, 0x12, 0xF3, 0x20, 0x02, 0x8F, 0x2B, 0xCE, 0xEA, 0xF4, 0xDC, 0x55, 
    0x98, 0x06, 0xF7, 0x4C, 0xA8, 0x75, 0xA8, 0xC7, 0x8D, 0xBB, 0x8C, 0x5D, 0xFB, 0xAB, 0x3F, 0x70, 
    0x01, 0xB8, 0x00, 0x5C, 0x00, 0x2E, 0x00, 0x17, 0x80, 0x0B, 0xC0, 0x05, 0xE0, 0xDF, 0x06, 0xA0, 
    0x43, 0xE0, 0xFF, 0x80, 0x1C, 0x57, 0x39, 0xF4, 0x07, 0x82, 0x29, 0xB4, 0x86, 0xF5, 0xA5, 0x63, 
    0xF3, 0xAD, 0x84, 0xE0, 0x77, 0x42, 0x11, 0x82, 0x74, 0x1C, 0xEB, 0xCA, 0xA3, 0xB5, 0xE0, 0x4A, 
    0x3B, 0xF4, 0x10, 0x5E, 0x00, 0x82, 0x5D, 0x88, 0x36, 0x48, 0x9B, 0xA7, 0x2E, 0x82, 0xE9, 0x6B, 
    0x5B, 0x20, 0xB4, 0x4A, 0xF4, 0x1F, 0x63, 0xCC, 0x22, 0x7B, 0xB9, 0xD9, 0xD6, 0x4E, 0x9B, 0xC4, 
    0x09, 0x53, 0x7B, 0x3C, 0x78, 0x5B, 0xEB, 0x5C, 0xDE, 0x01, 0xB4, 0x15, 0xCF, 0x9A, 0x7D, 0x95, 
    0x2E, 0xF5, 0x72, 0x79, 0x5B, 0xC0, 0xAD, 0x2D, 0x62, 0xEE, 0x65, 0x3E, 0x23, 0x70, 0xF2, 0xAC, 
    0x77, 0x5D, 0xB7, 0x32, 0x2D, 0xFB, 0x67, 0xAF, 0x82, 0xCF, 0xAE, 0x3C, 0xF9, 0xDD, 0x8B, 0x9A, 
    0x15, 0x76, 0xE6, 0xDD, 0xA3, 0x5E, 0x7B, 0xDB, 0x20, 0x08, 0xFE, 0x9E, 0x17, 0xDB, 0xA3, 0x47, 
    0xA3, 0x92, 0xF8, 0xC3, 0x25, 0xFB, 0x26, 0x4F, 0xC1, 0x5A, 0x01, 0x13, 0xE3, 0x80, 0x5E, 0x92, 
    0x37, 0x93, 0xE9, 0xBD, 0xB5, 0x91, 0x63, 0xEF, 0xF1, 0x25, 0xED, 0xB9, 0xAB, 0x18, 0x4F, 0xF4, 
    0xEA, 0x3D, 0x5B, 0x82, 0x43, 0x7A, 0x3B, 0xD9, 0x67, 0x2D, 0xC4, 0x07, 0x45, 0x1D, 0x7E, 0xE5, 
    0x1C, 0xC8, 0xFD, 0x9E, 0xBD, 0x74, 0x00, 0x4C, 0x80, 0x23, 0xD4, 0x94, 0x54, 0xF2, 0xFB, 0x8D, 
    0xC4, 0xEE, 0xE3, 0x41, 0xF6, 0x8D, 0x85, 0xF5, 0x40, 0x62, 0x9F, 0x00, 0x56, 0x0F, 0x72, 0xB3, 
    0x32, 0xFC, 0x7C, 0xFC, 0x94, 0xF0, 0xE1, 0xBB, 0xD4, 0x7D, 0xEC, 0xDA, 0x9E, 0x7E, 0x04, 0xF1, 
    0x9A, 0x26, 0xFB, 0x48, 0x4A, 0x68, 0x97, 0x63, 0xDA, 0xB7, 0x95, 0xA0, 0xCF, 0x91, 0x0F, 0xC0, 
    0x3B, 0x19, 0xFA, 0xF4, 0x6B, 0x58, 0xE0, 0xD0, 0xA7, 0x3A, 0x4C, 0x93, 0x1F, 0x3F, 0x80, 0x18, 
    0x7E, 0xFC, 0xF5, 0xDB, 0x3F, 0x19, 0x0F, 0xEB, 0xEC, 0xE1, 0xE9, 0x16, 0x0B, 0xF8, 0x36, 0x39, 
    0x75, 0xA8, 0xFA, 0xD7, 0x41, 0x00, 0xA6, 0xC8, 0xEA, 0xDC, 0x8A, 0x7D, 0xCF, 0xD4, 0x18, 0x7B, 
    0x8F, 0x2F, 0xB1, 0x0F, 0x1F, 0x18, 0x80, 0x54, 0xE6, 0xFD, 0xFD, 0x89, 0x9C, 0x8E, 0x7E, 0xA2, 
    0x72, 0x4B, 0x9B, 0x26, 0xE4, 0xC2, 0xF7, 0xED, 0xC2, 0xDF, 0x97, 0x13, 0x00, 0x27, 0x0C, 0x19, 
    0xFE, 0x9F, 0x90, 0x33, 0x3D, 0x0F, 0xF5, 0xBB, 0xDF, 0x11, 0xFD, 0x61, 0x00, 0x3F, 0x20, 0x3F, 
    0xBD, 0xFE, 0x05, 0xE0, 0x02, 0xC0, 0x5F, 0x1A, 0xAC, 0x8C, 0x9E, 0x9E, 0xEB, 0x65, 0xA2, 0x00, 
    0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 
};
       tex6 = ReadImage(9, (DWORD)g_pd3dDevice, "d6.png", (LPCSTR)data6, sizeof(data6));
       tex12 = ReadImage(9, (DWORD)g_pd3dDevice, "d12.png", (LPCSTR)data12, sizeof(data12));
    }
  
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_DICE));
        ImGui::Separator();

        if (ImGui::Button(S(THPRAC_TOOLS_DICE_DROP), ImVec2(160.0f,0.0f))){
            for (auto& d : dices)
            {
                d.dice->Drop();
                d.is_playing = true;
                d.t = 0.0f;
            }
        }

        bool is_all_playing = false;
        for (auto& d : dices) {
            is_all_playing |= d.is_playing;
        }
        if (!is_all_playing)
        {
            static int n_dice = 1;
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            if (ImGui::SliderInt(S(THPRAC_TOOLS_DICE_NUM), &n_dice, 1, max_dice)) {
                if (n_dice > max_dice)
                    n_dice = max_dice;
                else if (n_dice <= 0)
                    n_dice = 1;
                if (n_dice > dices.size()) {
                    int sz = dices.size();
                    for (int i = 0; i < n_dice - sz; i++)
                    {
                        if (dices.size()==0 || dices[dices.size() - 1].dice->GetType() == 6)
                            dices.push_back({ std::make_unique<Dice6>(), false, 0.0f });
                        else
                            dices.push_back({ std::make_unique<Dice12>(), false, 0.0f });
                    }
                } else if (n_dice < dices.size()) {
                    dices.resize(n_dice);
                }
            }
            ImGui::SameLine();
            ImGui::SetNextItemWidth(240.0f);
            if (ImGui::Button("D12", ImVec2(160.0f, 0.0f)))
            {
                if (dices.size() < max_dice)
                {
                    dices.push_back({ std::make_unique<Dice12>(), false, 0.0f });
                    n_dice = dices.size();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("D6", ImVec2(160.0f, 0.0f))) {
                if (dices.size() < max_dice)
                {
                    dices.push_back({ std::make_unique<Dice6>(), false, 0.0f });
                    n_dice = dices.size();
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Del", ImVec2(160.0f, 0.0f)) && dices.size()>=1) {
                dices.resize(dices.size()-1);
                n_dice = dices.size();
            }
        }
        for (auto& d : dices)
        {
            if (d.is_playing) {
                d.t += 0.008f;
                if (d.t >= 1.0f) {
                    d.t = 1.0f;
                    d.is_playing = false;
                }
                d.dice->Update(d.t);
            }
        }
        ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImVec2 csz = ImGui::GetContentRegionAvail();
        if (csz.y < 0)
            csz.y = 0;
        ImVec2 p1 = { p0.x + csz.x, p0.y + csz.y };

        ImDrawList* p = ImGui::GetWindowDrawList();
        p1 = { p0.x + csz.x, p0.y + csz.y };

        auto CvtPts = [](ImVec2 p, ImVec2 p1, ImVec2 p2) -> ImVec2 {
            return { p.x * (p2.x - p1.x) + p1.x, p.y * (p2.y - p1.y) + p1.y };
        };

        static ImVec4 colors_f[6] = { ImVec4(0, 0, 0, 0) };
        static UINT32 colors[6];
        static int last_hover_idx = -1;

        if (last_hover_idx >= 0 && last_hover_idx < dices.size()) {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                dices[last_hover_idx].dice = dices[last_hover_idx].dice->GetType() == 6 ? (std::unique_ptr<DiceBase>)std::make_unique<Dice12>() : (std::unique_ptr<DiceBase>)std::make_unique<Dice6>();
                dices[last_hover_idx].is_playing = false;
                dices[last_hover_idx].t = 0.0f;
            }
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                auto delt = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
                dices[last_hover_idx].dice->DragDrop(delt.x, delt.y);
                dices[last_hover_idx].t = 0;
                dices[last_hover_idx].is_playing = true;
            }
        }
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            last_hover_idx = -1;
        }
        if (colors_f[0].x == 0) {
            float s = 0.5f;
            float v = 0.9f;
            ImGui::ColorConvertHSVtoRGB(1, 0.0f, v, colors_f[0].x, colors_f[0].y, colors_f[0].z);
            ImGui::ColorConvertHSVtoRGB(240.0f / 360.0f, s, v, colors_f[1].x, colors_f[1].y, colors_f[1].z);
            ImGui::ColorConvertHSVtoRGB(0.0f / 360.0f, s, v, colors_f[2].x, colors_f[2].y, colors_f[2].z);
            ImGui::ColorConvertHSVtoRGB(30.0f / 360.0f, s, v, colors_f[3].x, colors_f[3].y, colors_f[3].z);
            ImGui::ColorConvertHSVtoRGB(120.0f / 360.0f, s, v, colors_f[4].x, colors_f[4].y, colors_f[4].z);
            ImGui::ColorConvertHSVtoRGB(60.0f / 360.0f, s, v, colors_f[5].x, colors_f[5].y, colors_f[5].z);
            for (int i = 0; i < 6; i++) {
                colors_f[i].w = 1.0f;
                colors[i] = ImGui::ColorConvertFloat4ToU32(colors_f[i]);
            }
        }
        int cutX[] = { 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4 };
        int cutY[] = { 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4 };
        if (csz.y > 0.0f) {
            p->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 100));
            p->AddRect(p0, p1, IM_COL32(255, 255, 255, 100));
            p->PushClipRect(p0, p1);

            p1.y = p1.y - ImGui::GetTextLineHeight() * 2.0f;
            int cur_cutx = cutX[dices.size()];
            int cur_cuty = cutY[dices.size()];
            float width2 = (p1.x - p0.x) / cur_cutx;
            float height = (p1.y - p0.y) / cur_cuty;
            float width = std::min(width2, height);
            height = width;

            for (int idxd = 0; idxd < dices.size(); idxd++) {
                auto& d = dices[idxd];

                int idx_x = idxd % cur_cutx;
                int idx_y = idxd / cur_cutx;
                ImVec2 cur_p0 = { p0.x + width2 * idx_x, p0.y + height * idx_y };
                ImVec2 cur_p1 = { cur_p0.x + width, cur_p0.y + height};

                float hover_sz = 0.7f;
                ImVec2 hover_p0 = { cur_p1.x * (1.0f - hover_sz) + cur_p0.x * hover_sz, cur_p1.y * (1.0f - hover_sz) + cur_p0.y * hover_sz };
                ImVec2 hover_p1 = { cur_p1.x * hover_sz + cur_p0.x * (1.0f - hover_sz), cur_p1.y * hover_sz + cur_p0.y * (1.0f - hover_sz) };

                if (ImGui::IsMouseHoveringRect(hover_p0, hover_p1) && !ImGui::IsMouseDown(ImGuiMouseButton_Left)){
                    last_hover_idx = idxd;
                }
                
                std::vector<ImVec2> points;
                std::vector<ImVec2> uvs;
                std::vector<int> dice_num;
                d.dice->Render(points,uvs, dice_num);
                for (auto& p : points)
                    p = CvtPts(p, cur_p0, cur_p1);
                if (d.dice->GetType() == 6)
                {
                    for (int i = 0; i < points.size(); i += 4) {
                        auto diceidx = dice_num[i / 4] - 1;
                        if (tex6) {
                            p->AddImageQuad(tex6, points[i + 0], points[i + 1], points[i + 2], points[i + 3], uvs[i + 0], uvs[i + 1], uvs[i + 2], uvs[i + 3]);
                        } else {
                            p->AddQuadFilled(points[i + 0], points[i + 1], points[i + 2], points[i + 3], colors[diceidx]);
                        }
                    }
                    for (int i = 0; i < points.size(); i += 4) {
                        if (last_hover_idx == idxd)
                            p->AddPolyline(points.data() + i, 4, IM_COL32(255, 128, 0, 255), ImDrawFlags_Closed, 4.0f);
                        else
                            p->AddPolyline(points.data() + i, 4, IM_COL32(0, 0, 0, 255), ImDrawFlags_Closed, 3.0f);
                    }
                }
                else
                {
                    //12
                    for (int i = 0; i < points.size(); i += 5) {
                        auto diceidx = dice_num[i / 5] - 1;
                        if (tex12) {
                            p->AddImageQuad(tex12, points[i + 0], points[i + 1], points[i + 2], points[i + 3], uvs[i + 0], uvs[i + 1], uvs[i + 2], uvs[i + 3]);
                            p->AddImageQuad(tex12, points[i + 0], points[i + 3], points[i + 4], points[i + 0], uvs[i + 0], uvs[i + 3], uvs[i + 4], uvs[i + 0]);
                        } else {
                            p->AddQuadFilled(points[i + 0], points[i + 1], points[i + 2], points[i + 3], colors[diceidx]);
                        }
                    }
                    for (int i = 0; i < points.size(); i += 5) {
                        if (last_hover_idx == idxd)
                            p->AddPolyline(points.data() + i, 5, IM_COL32(255, 128, 0, 255), ImDrawFlags_Closed, 4.0f);
                        else
                            p->AddPolyline(points.data() + i, 5, IM_COL32(0, 0, 0, 255),ImDrawFlags_Closed,3.0f);
                    }
                }
                p->AddText(ImGui::GetFont(),64.0, { cur_p0.x * 0.5f + cur_p1.x * 0.5f, cur_p1.y - ImGui::GetTextLineHeight() * 1.6f },
                    0xFFFFFFFF, std::format("{}", d.dice->GetResult()).c_str());
            }
            p->PopClipRect();
        }
        return result;
    }
};

class THRoll
{
public:
    struct THRollSelection {
        std::string name;
        float weight;
        ImVec4 color;
    };
    float GetRandomFloat(float a=0.0f,float b=1.0f)
    {
        static auto rnd_gen = GetRndGenerator(0.0f,1.0f);
        return rnd_gen()*(b-a)+a;
    }
    float MInterpolation(float t, float a, float b)
    {
        if (t < 0.0f) {
            return a;
        } else if (t < 1.0f) {
            t = 1.0f - powf(1.0f - t, 3.0f);
            return t * (b - a) + a;
        }
        return b;
        // if (t < 0.0f) {
        //     return a;
        // } else if (t < 0.5) {
        //     float k = (b - a) * 2.0f;
        //     return k * t * t + a;
        // } else if (t < 1.0f) {
        //     float k = (b - a) * 4.0f;
        //     t = t - 1.0f;
        //     return k * t * t * t + b;
        // }
        //return b;
    };

private:
    std::vector<THRollSelection> selections;
    float weight_sum;
    std::string mRollName;

    bool DrawPie(ImDrawList* p, ImVec2 mid, float radius, float angle1, float angle2, uint32_t col_fill)
    {
        uint32_t col_line = 0xFFFFFFFF;
        bool res = false;
        std::vector<ImVec2> points;
        points.push_back({ cosf(angle1), sinf(angle1) });
        float dangle = std::numbers::pi * 0.005f;
        for (float angle = angle1 + dangle; angle < angle2; angle += dangle)
            points.push_back({ cosf(angle), sinf(angle) });
        points.push_back({ cosf(angle2), sinf(angle2) });

        auto mp = ImGui::GetMousePos();
        if (hypotf(mp.y - mid.y, mp.x - mid.x) < radius) {
            float mangle = atan2(mp.y - mid.y, mp.x - mid.x);
            mangle += (2.0f * std::numbers::pi) * ceilf((angle1 - mangle) / (2.0f * std::numbers::pi));
            res = mangle < angle2;
        }
        uint32_t col_fill2 = 0;
        if (res) {
            radius *= 1.1f;
            ImVec4 color = ImGui::ColorConvertU32ToFloat4(col_fill);
            col_fill2 = ImGui::ColorConvertFloat4ToU32(ImVec4 { color.x * 0.75f,
                                                                color.y * 0.75f,
                                                                color.z * 0.75f,
                                                                color.w });
        }
        
        for (int i = 0; i < points.size() - 1; i++) {
            ImVec2 p2 = { mid.x + points[i].x * radius, mid.y + points[i].y * radius };
            ImVec2 p3 = { mid.x + points[i + 1].x * radius, mid.y + points[i + 1].y * radius };
            p->AddTriangleFilled(mid, p2, p3, res ? col_fill2 : col_fill);
            p->AddTriangle(mid, p2, p3, res ? col_fill2 : col_fill, 1.33f);
            if (res) {
                p->AddLine(p2, p3, col_line, 2.0f);
            }
            
        }
        p->AddLine(mid, { mid.x + points[0].x * radius, mid.y + points[0].y * radius }, col_line, 2.0f);
        if (res) {
            p->AddLine(mid, { mid.x + points[points.size() - 1].x * radius, mid.y + points[points.size() - 1].y * radius }, col_line, 2.0f);
        }
        return res;
    }

    void InitWeight()
    {
        weight_sum = 0.0f;
        for (auto& i : selections)
        {
            weight_sum += i.weight;
        }
    }
    void InitColors(bool rand_color = true)
    {
        if (rand_color) {
            for (int i = 0; i < selections.size(); i++) {
                float r, g, b, h, s, v;
                h = GetRandomFloat();
                s = GetRandomFloat() * 0.4f + 0.6f;
                v = GetRandomFloat() * 0.4f + 0.6f;
                ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                selections[i].color = { r, g, b, 1.0f };
            }
        } else {
            int hi = 0, si = 255, vi = 255;
            for (int i = 0; i < selections.size(); i++) {
                float r, g, b, h, s, v;
                if (selections.size() > 100)
                    hi += 7;
                else if (selections.size() > 50)
                    hi += 19;
                else
                    hi += 41;
                if (hi >= 256) {
                    hi -= 256;
                    si -= 83;
                    if (si <= 64) {
                        si += 191;
                        vi -= 101;
                        if (vi <= 128)
                            vi += 127;
                    }
                }
                hi %= 256;
                h = (float)hi / 255.0f;
                vi %= 256;
                s = (float)vi / 255.0f;
                si %= 256;
                v = (float)si / 255.0f;
                ImGui::ColorConvertHSVtoRGB(h, s, v, r, g, b);
                selections[i].color = { r, g, b, 1.0f };
            }
        }
    }

public:
    THRollSelection GetSelection(int idx)
    {
        return selections[idx];
    }

    int NumSelection()
    {
        return selections.size();
    }

    void RemoveSelection(int idx)
    {
        if (idx < selections.size()) {
            selections.erase(selections.begin() + idx);
        }
        InitWeight();
    }

    void InitRoll(std::string rollName, std::vector<THRollSelection> sels, bool reset_color = true, bool rand_color = true)
    {
        selections = sels;
        mRollName = rollName;
        if (reset_color) {
            InitColors(rand_color);
        }
        InitWeight();
    }

    void InitRoll(std::wstring csv_filename, bool rand_color = true)
    {
        std::vector<std::string> mNames = {};
        std::vector<float> mWeights = {};

        std::string rollName = utf16_to_utf8(GetNameFromFullPath(csv_filename).c_str());

        rapidcsv::Document doc(utf16_to_mb(csv_filename.c_str(), CP_ACP), rapidcsv::LabelParams(0, -1));
        mNames = doc.GetColumn<std::string>(0);
        mWeights = doc.GetColumn<float>(1);
        int n = std::min(mNames.size(), mWeights.size());
        std::vector<THRollSelection> selections_csv;
        for (int i = 0; i < n; i++) {
            THRollSelection sel;
            sel.name = mNames[i];
            sel.weight = mWeights[i];
            sel.color = { 1.0f, 1.0f, 1.0f, 1.0f };
            selections_csv.push_back(sel);
        }
        InitRoll(rollName, selections_csv, true, rand_color);
    }

    void InitRoll(std::string rollName,std::vector<std::string> names, bool rand_color = true)
    {
        std::vector <THRollSelection> sels = {};
        for (auto& n : names)
        {
            THRollSelection sel;
            sel.name = n;
            sel.weight = 1.0f;
            sel.color = { 1.0f, 1.0f, 1.0f, 1.0f };
            sels.push_back(sel);
        }
        InitRoll(rollName, sels, true, rand_color);
    }

    THRoll(std::string rollName, std::vector<THRollSelection> sels, bool reset_color = true, bool rand_color = true)
    {
        InitRoll(rollName, sels, reset_color, rand_color);
    }
    THRoll()
    {
        weight_sum = 0.0f;
    }
    THRoll(std::wstring csv_filename, bool rand_color = true)
    {
        InitRoll(csv_filename, rand_color);
    }

    int CalcIdx(float angle)
    {
        angle = -angle;
        angle = angle - floorf(angle / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
        int idx = -1;
        float res = angle / (2.0f * std::numbers::pi);
        float weight_cur = 0;
        for (int i = 0; i < selections.size(); i++) {
            float weight_next = weight_cur + selections[i].weight;
            if (weight_cur / weight_sum <= res && weight_next / weight_sum > res) {
                idx = i;
                break;
            }
            weight_cur = weight_next;
        }
        return idx;
    }

    ImVec2 GuiDraw(float& angle, int selection_idx, float height_rem = 0.0f,bool* p_is_hovered = nullptr)
    {
        ImVec2 mousePosRel = { 0.0f, 0.0f };
        angle = angle - floorf(angle / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
        {
            ImVec2 p0 = ImGui::GetCursorScreenPos();
            ImVec2 csz = ImGui::GetContentRegionAvail();
            csz.y -= height_rem;
            if (csz.y < 0)
                csz.y = 0;
            ImVec2 cmid = { p0.x + csz.x * 0.5f, p0.y + csz.y * 0.5f };
            ImVec2 p1 = { cmid.x + csz.x * 0.5f, cmid.y + csz.y * 0.5f };
            float hheight = std::min(csz.x, csz.y) * 0.95f * 0.5f;

            ImDrawList* p = ImGui::GetWindowDrawList();
            p->AddRectFilled(p0, p1, IM_COL32(50, 50, 50, 100));
            p->AddRect(p0, p1, IM_COL32(255, 255, 255, 100));
            p->PushClipRect(p0, p1);

            ImVec2 cir_cen = { hheight * 1.1f, cmid.y };
            mousePosRel = ImGui::GetMousePos();
            mousePosRel.x = (mousePosRel.x - cir_cen.x) / hheight;
            mousePosRel.y = (mousePosRel.y - cir_cen.y) / hheight;
            if (selections.size() > 0 && csz.y > 0.0f) {
                {
                    p->AddCircle(cir_cen, hheight, 0xFFFFFFFF);
                    float pie_angle_start = angle;
                    for (int i = 0; i < selections.size(); i++) {
                        float pie_angle_delta = selections[i].weight / weight_sum * 2.0f * std::numbers::pi;

                        auto col1 = ImGui::ColorConvertFloat4ToU32(selections[i].color);
                        bool hovered = false;
                        if (selection_idx==i)
                            hovered = DrawPie(p, cir_cen, hheight * 1.05f, pie_angle_start, pie_angle_start + pie_angle_delta, col1);
                        else
                            hovered  = DrawPie(p, cir_cen, hheight, pie_angle_start, pie_angle_start + pie_angle_delta, col1);
                        if (hovered)
                        {
                            ImGui::SetTooltip("%s", selections[i].name.c_str());
                            if (p_is_hovered)
                                *p_is_hovered = hovered;
                        }
                        pie_angle_start += pie_angle_delta;
                    }
                }
                ImVec2 tri_pos1 = { cir_cen.x + hheight * 0.95f, cir_cen.y };
                ImVec2 tri_pos2 = { cir_cen.x + hheight * 1.45f, cir_cen.y - hheight * 0.05f };
                ImVec2 tri_pos3 = { cir_cen.x + hheight * 1.45f, cir_cen.y + hheight * 0.05f };
                p->AddTriangleFilled(tri_pos1, tri_pos2, tri_pos3, 0xFFFFCCCC);
                p->AddTriangle(tri_pos1, tri_pos2, tri_pos3, 0xFFFFFFFF, 1.5f);
                if (selection_idx >= 0 && selection_idx < selections.size())
                    p->AddText({ cir_cen.x + hheight * 1.5f, cir_cen.y - ImGui::GetTextLineHeight() * 0.5f }, 0xFFFFFFFF, std::format("{}", selections[selection_idx].name).c_str());
                p->PopClipRect();
            }
        }
        return mousePosRel;
    }
};


class THGuiRollAll {
private:
    THRoll mRoll;
    bool mRandColor;

    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;

public:
    THGuiRollAll()
    {
        is_rolling = false;
        angle_fin = 0.0f;
        angle_last = 0.0f;
        mRandColor = false;
        time_tot = 240.0f;
        time = 0.0f;
    }
    
    void LoadRoll()
    {
        std::wstring csv_filename = LauncherWndFileSelect(nullptr, L"csv(*.csv)\0*.csv\0*.*\0\0");
        mRoll.InitRoll(csv_filename, true);
    }
    void LoadRollWaifu()
    {
        mRoll.InitRoll("waifu", waifus, mRandColor);
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_ROLLF_FROM_FILE));
        ImGui::Separator();
        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_CSV)))
            LoadRoll();
        ImGui::SameLine();
        GuiHelpMarker(S(THPRAC_TOOLS_ROLLF_CSV_DESC));
        ImGui::SameLine();
        if (ImGui::Button("waifus"))
            LoadRollWaifu();
        ImGui::SameLine();
        ImGui::Checkbox(S(THPRAC_TOOLS_ROLLF_RANDOM_COLOR), &mRandColor);
        ImGui::SameLine();
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_ROLLF_CLPS))){
            if (ImGui::BeginTable("__rolls table",2)) {
                for (int i = 0; i < mRoll.NumSelection(); i++)
                {
                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text("%s", mRoll.GetSelection(i).name.c_str());
                    ImGui::TableNextColumn();
                    ImGui::Text("%.2f", mRoll.GetSelection(i).weight);
                }
                ImGui::EndTable();
            }
        }
        ImGui::Separator();

        if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_START)) && mRoll.NumSelection() > 0)
        {
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }
        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);
        if (!is_rolling) {
            ImGui::SameLine();
            if (ImGui::Button(S(THPRAC_TOOLS_ROLLF_REMOVE_CUR))){
                mRoll.RemoveSelection(roll_result);
            }
            
        }
        if (!is_rolling) {
            ImGui::SameLine();
            ImGui::Text(S(THPRAC_TOOLS_ROLLF_TIME));
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150.0f);
            if (ImGui::DragFloat("##roll time", &time_tot, 1.0f, 1, 1000))
                time_tot = std::clamp(time_tot, 1.0f, 1000.0f);
        }
        bool is_cur_hovered = false;
        auto posRel = mRoll.GuiDraw(angle_cur, roll_result, 0.0f, &is_cur_hovered);
        static bool is_hovered = false;
        static ImVec2 pos_mouseDown;
        if (is_cur_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            is_hovered |= is_cur_hovered;
            pos_mouseDown = posRel;
        }
        if (is_hovered && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            is_hovered = false;

            ImVec2 deltDrag = { posRel.x - pos_mouseDown.x, posRel.y - pos_mouseDown.y };
            float drag_force = 12.0f;
            float drag_dist = posRel.x * deltDrag.y - posRel.y * deltDrag.x;
            auto real_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::ResetMouseDragDelta();
            if (fabsf(drag_dist) >= 0.1f && hypotf(real_drag.x, real_drag.y) >= 5.0f) {
                is_rolling = true;
                time = 0;
                angle_last = angle_cur - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
                angle_fin = angle_last + drag_force * drag_dist;
            }
        }
        return result;
    }

private:
};

THGuiTestReactionTest::THGuiTestReactionTest()
{
    QueryPerformanceFrequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = GetRndGenerator(1200u, 2800u);
    // mRndSeedGen = GetRndGenerator(0u, 1u);
    mShowProgressBar = false;
    mCurTest = 0;
}
bool THGuiTestReactionTest::GuiUpdate(bool ingame)
{
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    Gui::KeyboardInputUpdate(VK_DOWN);
    Gui::KeyboardInputUpdate(VK_UP);
    Gui::KeyboardInputUpdate(VK_RIGHT);
    Gui::KeyboardInputUpdate(VK_LEFT);
    Gui::KeyboardInputUpdate('Z');
    Gui::KeyboardInputUpdate(VK_SHIFT);

    ImVec2 colorBtnSz = ImVec2(ImGui::GetWindowWidth()*0.5f, ImGui::GetTextLineHeight() * 10.0f);
    ImVec2 colorBtnPos = ImVec2(ImGui::GetWindowWidth()*0.25f, ImGui::GetTextLineHeight() * 2.0f);
    DWORD colorBtnFlag = ImGuiColorEditFlags_::ImGuiColorEditFlags_NoTooltip;
    ImVec4 color_BeforeTimeReact = ImVec4(1, 0.2, 0.2, 1);
    ImVec4 color_AfterTimeReact = ImVec4(0, 1, 0.2, 1);
    ImVec4 color_WaitPress = ImVec4(1, 1, 0.2, 1);

    bool isKeyPressed = ImGui::IsKeyDown(37) || ImGui::IsKeyDown(38) || ImGui::IsKeyDown(39) || ImGui::IsKeyDown(40)
        || (GetAsyncKeyState(VK_DOWN) & 0x8000) || (GetAsyncKeyState(VK_UP) & 0x8000) || (GetAsyncKeyState(VK_RIGHT) & 0x8000) || (GetAsyncKeyState(VK_LEFT) & 0x8000);
    if (!ingame){
        if (ImGui::Button(S(THPRAC_BACK))) {
            Reset();
            return false;
        }
    }
    switch (mTestState)
    {
    case NOT_BEGIN:
    {
        if (!ingame) {
            ImGui::SameLine();
            GuiCenteredText(S(THPRAC_TOOLS_REACTION_TEST));
            ImGui::Separator();
        }
    
        ImGui::RadioButton(S(THPRAC_TOOLS_REACTION_TEST_DOWN), (int*)&mTestType, PRESS);
        ImGui::SameLine();
        ImGui::RadioButton(S(THPRAC_TOOLS_REACTION_TEST_UP), (int*)&mTestType, RELEASE);
    
        ImGui::Checkbox(S(THPRAC_TOOLS_REACTION_SHOW_PROC_BAR), &mShowProgressBar);
    
        ImGui::DragInt(S(THPRAC_TOOLS_REACTION_TEST_TIME), &mTestTime, 1.0f, 1, 20);
        ImGui::NewLine();
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_BEGIN)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) {
            // in win 7, due to the initialize sequence, QueryPerformanceFrequency might get 0 value at the beginning, 
            // so query it every time before test begin to avoid this problem
            QueryPerformanceFrequency(&mTimeFreq);
            mTestState = WAIT_TIME;
            mCurTest = 1;
            mFrameCount = 0;
            if (mTestType == PRESS) {
                mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
            mResults = {};
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    
    case TOO_EARLY: {
        ImGui::Text(S(THPRAC_TOOLS_REACTION_TEST_TOO_EARLY));
        if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) // press z(90)/shift(16)
        {
            mFrameCount = 0;
            mTestState = WAIT_TIME;
            mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
            mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
        }
        ImGui::InvisibleButton("inv", colorBtnSz);
    } break;
    case SHOW_RES: {
        if (mResults.size() != 0 && mFrameCounts.size() != 0)
        {
            ImGui::Text("%s(%d): %.1f ms (%.1f frame)(framecount: %d)", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest, mResults[mResults.size() - 1], mResults[mResults.size() - 1] / 16.66667f,(int)mFrameCounts[mFrameCounts.size()-1]);
            if (mCurTest < mTestTime)
                ImGui::InvisibleButton("inv", colorBtnSz);
        }else
            ImGui::Text("%s(%d): ?????", S(THPRAC_TOOLS_REACTION_TEST_RESULT), mCurTest);
        if (mCurTest == mTestTime)
        {
            float avg = 0.0f;
            float maxv = 60.0f;
            float minv = 0.0f;
            float avg_frameCnt = 0.0f;
            float maxfcnt = 10.0f;
            float minfcnt = 0.0f;
            for (auto x : mResults) {
                avg += fabsf(x);
                maxv = std::max(x, maxv);
                minv = std::min(x, minv);
            }
            for (auto x : mFrameCounts) {
                avg_frameCnt += fabsf(x);
                maxfcnt = std::max(x, maxfcnt);
                minfcnt = std::min(x, minfcnt);
            }
            avg /= (float)mTestTime;
            avg_frameCnt /= (float)mTestTime;
            ImGui::Separator();
            ImGui::Text("%s: %s, %s", S(THPRAC_TOOLS_REACTION_MODE), mTestType == PRESS ? S(THPRAC_TOOLS_REACTION_MODE_DOWN) : S(THPRAC_TOOLS_REACTION_MODE_UP), 
                mShowProgressBar ? S(THPRAC_TOOLS_REACTION_MODE_PROGRESSBAR) : S(THPRAC_TOOLS_REACTION_MODE_NORMAL)
            );
            ImGui::Text("%s: %.1f ms (%.1f frame)(framecount: %.1f)", S(THPRAC_TOOLS_REACTION_TEST_RESULT_AVG), avg, avg / 16.66667f, avg_frameCnt);
            
            ImGui::PlotHistogram(S(THPRAC_TOOLS_REACTION_TEST_RESULT), &mResults[0], mTestTime, 0, S(THPRAC_TOOLS_REACTION_TEST_RESULT), minv-10.0f, maxv+20.0f, ImVec2(0, 200.0));
            
            ImGui::PlotHistogram(std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str(), &mFrameCounts[0], mTestTime, 0, std::format("{}(framecount)", S(THPRAC_TOOLS_REACTION_TEST_RESULT)).c_str(), minfcnt-1.0f, maxfcnt + 10.0f, ImVec2(0, 200.0));
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) {
                mTestState = NOT_BEGIN;
            }
        } else {
            if (ImGui::Button(S(THPRAC_TOOLS_REACTION_TEST_NEXT_TEST)) || ImGui::IsKeyPressed('Z') || ImGui::IsKeyPressed(VK_SHIFT)) // press z
            {
                mCurTest++;
                mTestState = WAIT_TIME;
                mFrameCount = 0;
                mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
            }
        }
    } break;
    
    case REACT_TIME:
    case WAIT_TIME:
    case WAIT_TIME_PRESSED:
    {
        if (mTestType == PRESS)
        {
            if (mTestState == WAIT_TIME && curTime.QuadPart >= mPressTime.QuadPart){
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            ImGui::Text("test %d", mCurTest);
            mFrameCount++;
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(std::fabsf(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart)), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    if (!mShowProgressBar){
                        mTestState = TOO_EARLY;
                    } else {
                        // allow negative reaction time
                        auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceil(fabsf(curTestReactionTimeMs))/16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            } else {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (isKeyPressed){
                    auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(mFrameCount);
                    mTestState = SHOW_RES;
                }
            }
        }else{
            if (mTestState == WAIT_TIME_PRESSED && curTime.QuadPart >= mPressTime.QuadPart) {
                mTestState = REACT_TIME;
                mFrameCount = 0;
            }
            mFrameCount++;
            ImGui::Text("test %d", mCurTest);
            if (mTestState == WAIT_TIME) {
                if (mShowProgressBar) {
                    ImGui::ProgressBar(1.0f, ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_WaitPress, colorBtnFlag, colorBtnSz);
                if (isKeyPressed) {
                    mWaitTime.QuadPart = mRndSeedGen() / 1000.0 * (double)mTimeFreq.QuadPart;
                    mPressTime.QuadPart = mWaitTime.QuadPart + curTime.QuadPart;
                    mTestState = WAIT_TIME_PRESSED;
                }
            } else if(mTestState == REACT_TIME){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(0.0f, ImVec2(0, 0), "press...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_AfterTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                    mResults.push_back(curTestReactionTimeMs);
                    mFrameCounts.push_back(mFrameCount);
                    mTestState = SHOW_RES;
                }
            }
            else if (mTestState == WAIT_TIME_PRESSED){
                if (mShowProgressBar) {
                    ImGui::ProgressBar(std::clamp(std::fabsf(((double)(mPressTime.QuadPart - curTime.QuadPart)) / ((double)mWaitTime.QuadPart)), 0.0f, 1.0f), ImVec2(0, 0), "waiting...");
                }
                ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + colorBtnPos.x, ImGui::GetCursorPosY() + colorBtnPos.y));
                ImGui::ColorButton("color", color_BeforeTimeReact, colorBtnFlag, colorBtnSz);
                if (!isKeyPressed) {
                    if (!mShowProgressBar) {
                        mTestState = TOO_EARLY;
                    } else {
                        auto curTestReactionTimeMs = ((double)(curTime.QuadPart - mPressTime.QuadPart)) / ((double)mTimeFreq.QuadPart) * 1000.0;
                        mResults.push_back(curTestReactionTimeMs);
                        mFrameCounts.push_back(ceil(fabsf(curTestReactionTimeMs)) / 16.6667f);
                        mTestState = SHOW_RES;
                    }
                }
            }
        }
    } 
    break;
    }
    return true;
}

void THGuiTestReactionTest::Reset()
{
    QueryPerformanceFrequency(&mTimeFreq);
    mTestState = NOT_BEGIN;
    mTestType = PRESS;
    mTestTime = 5;
    mRndSeedGen = GetRndGenerator(1200u, 3500u);
    mShowProgressBar = false;
    mCurTest = 0;
    mResults = {};
    mFrameCounts = {};
}


class THGuiRollPlayer {
public:
    THRoll mRoll;

    bool first_roll;
    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;


    THGuiRollPlayer()
    {
        is_rolling = false;
        first_roll = true;
        angle_fin = -0.1f;
        angle_last = -0.1f;
        time_tot = 240.0f;
        time = 0.0f;

        mRndTextGen = GetRndGenerator(1u, 20u);

        for (auto& game : gGameRoll) {
            if (game.playerSelect) {
                mGameOption.push_back(game);
            }
        }
        if (mRndTextGen() == 1) {
            mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
        } else {
            mRollText = S(THPRAC_GAMEROLL_ROLL);
        }
        SetPlayerOpt();
        UpdateRoll();
    }

    void UpdateRoll()
    {
        std::vector<std::string> names;
        for (auto& player : mPlayerOption) {
            if (player.second) {
                names.push_back(player.first);
            }
        }
        mRoll.InitRoll("rndgame", names, false);
    }

    void SwitchGame(const char* gameStr)
    {
        int i = 0;
        for (auto& game : mGameOption) {
            if (!strcmp(game.name, gameStr)) {
                mGameSelected = i;
                SetPlayerOpt();
            }
            ++i;
        }
        UpdateRoll();
    }
    void SetPlayerOpt()
    {
        mPlayerOption.clear();
        mRollText = S(THPRAC_GAMEROLL_ROLL);
        auto playerStr = S(mGameOption[mGameSelected].shottypes);
        while (playerStr[0] != '\0') {
            mPlayerOption.push_back(std::pair<std::string, bool>(playerStr, true));
            for (; playerStr[0] != '\0'; ++playerStr)
                ;
            ++playerStr;
        }
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_PLAYER));
        ImGui::Separator();

        bool changed = false;
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_RND_PLAYER))) {

            if (ImGui::BeginCombo(S(THPRAC_TOOLS_RND_PLAYER_GAME), S(mGameOption[mGameSelected].title), 0)) { // The second parameter is the label previewed before opening the combo.
                changed = true;
                for (size_t n = 0; n < mGameOption.size(); n++) {
                    bool is_selected = (mGameSelected == n);
                    if (ImGui::Selectable(S(mGameOption[n].title), is_selected)) {
                        mGameSelected = n;
                        SetPlayerOpt();
                    }
                    if (is_selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::NewLine();

            if (mPlayerOption.size()) {
                auto& selected = mGameOption[mGameSelected];
                ImGui::Columns(selected.playerColumns, 0, false);
                for (auto& player : mPlayerOption) {
                    changed |= ImGui::Checkbox(player.first.c_str(), &player.second);
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
            }
        }

        std::vector<std::string> candidate;
        for (auto& player : mPlayerOption) {
            if (player.second) {
                candidate.push_back(player.first);
            }
        }

        if (changed) {
            UpdateRoll();
        }

        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);

        auto wndSize = ImGui::GetWindowSize();
        bool is_cur_hovered = false;
        auto posRel = mRoll.GuiDraw(angle_cur, roll_result, wndSize.y * 0.15f, &is_cur_hovered);
        static bool is_hovered = false;
        static ImVec2 pos_mouseDown;
        if (is_cur_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            is_hovered |= is_cur_hovered;
            pos_mouseDown = posRel;
        }
        if (is_hovered && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            is_hovered = false;
            ImVec2 deltDrag = { posRel.x - pos_mouseDown.x, posRel.y - pos_mouseDown.y };
            float drag_force = 12.0f;
            float drag_dist = posRel.x * deltDrag.y - posRel.y * deltDrag.x;
            auto real_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::ResetMouseDragDelta();
            if (fabsf(drag_dist) >= 0.1f && hypotf(real_drag.x,real_drag.y)>=5.0f) {
                first_roll = false;
                is_rolling = true;
                time = 0;
                angle_last = angle_cur - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
                angle_fin = angle_last + drag_force * drag_dist;
            }
        }

        if (!first_roll && !is_rolling) {
            auto result = roll_result;
            if (result != -1 && result < candidate.size()) {
                char outputStr[256];
                sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), candidate[result].c_str());
                mRollText = outputStr;
                mRollGame = S(mGameOption[mGameSelected].title);
            }
        }

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            first_roll = false;
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }

        if (mRollText != S(THPRAC_GAMEROLL_ROLL) && ImGui::BeginPopupContextItem("##roll_player_popup")) {
            if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(S(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollGame.c_str());
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
            } else {
                mRollText = S(THPRAC_GAMEROLL_ROLL);
            }
        }
        return result;
    }

private:
    std::function<unsigned int(void)> mRndTextGen;
    std::vector<GameRoll> mGameOption;
    size_t mGameSelected = 0;
    std::vector<std::pair<std::string, bool>> mPlayerOption;
    std::string mRollText;
    std::string mRollGame;
};

class THGuiRollGame {
public:
    THRoll mRoll;

    bool first_roll;
    bool is_rolling;
    float angle_fin;
    float angle_last;
    float time;
    float time_tot;

    THGuiRollGame()
    {
        is_rolling = false;
        first_roll = true;
        angle_fin = -0.1f;
        angle_last = -0.1f;
        time_tot = 240.0f;
        time = 0.0f;

        mRndTextGen = GetRndGenerator(1u, 20u);
        for (auto& game : gGameRoll) {
            if (game.type == ROLL_MAIN) {
                game.selected = true;
            }
            mGameOption[game.type].push_back(game);
        }
        UpdateRoll();
        if (mRndTextGen() == 1) {
            mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
        } else {
            mRollText = S(THPRAC_GAMEROLL_ROLL);
        }
    }
    
    bool GuiGameTypeChkBox(const char* text, int idx)
    {
        if (ImGui::Checkbox(text, &(mGameTypeOpt[idx]))) {
            for (auto& game : mGameOption[idx]) {
                game.selected = mGameTypeOpt[idx];
            }
            return true;
        }
        return false;
    }
    void UpdateRoll()
    {
        std::vector<std::string> names;
        for (auto& gameType : mGameOption) {
            for (auto& game : gameType) {
                if (game.selected)
                    names.push_back(S(game.title));
            }
        }
        mRoll.InitRoll("rndgame", names, false);
    }
    bool GuiUpdate()
    {
        bool result = true;
        if (ImGui::Button(S(THPRAC_BACK))) {
            result = false;
        }
        ImGui::SameLine();
        GuiCenteredText(S(THPRAC_TOOLS_RND_GAME));
        ImGui::Separator();

        bool changed = false;
        if (ImGui::CollapsingHeader(S(THPRAC_TOOLS_RND_GAME)))
        {
            int i = 0;
            for (auto& gameType : mGameOption) {
                bool allSelected = true;
                ImGui::Columns(6, 0, false);
                for (auto& game : gameType) {
                    changed |= ImGui::Checkbox(S(game.title), &game.selected);
                    if (!game.selected) {
                        allSelected = false;
                    }
                    mGameTypeOpt[i] = allSelected;
                    ImGui::NextColumn();
                }
                ImGui::Columns(1);
                ImGui::NewLine();
                ++i;
            }
            ImGui::NewLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_TOOLS_RND_GAME_PC98), 0);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_MAIN_SERIES), 1);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_STG), 2);
            ImGui::SameLine();
            changed |= GuiGameTypeChkBox(S(THPRAC_GAMES_SPINOFF_OTHERS), 3);
        }

        std::vector<GameRoll> candidate;
        for (auto& gameType : mGameOption) {
            for (auto& game : gameType) {
                if (game.selected) {
                    candidate.push_back(game);
                }
            }
        }

        if (changed) {
            UpdateRoll();
        }

        float angle_cur;
        if (is_rolling) {
            angle_cur = mRoll.MInterpolation(time / (float)time_tot, angle_last, angle_fin);
            if (time < time_tot) {
                time++;
            } else {
                is_rolling = false;
            }
        } else {
            angle_cur = angle_fin;
        }
        int roll_result = mRoll.CalcIdx(angle_cur);

        auto wndSize = ImGui::GetWindowSize();

        bool is_cur_hovered = false;
        auto posRel = mRoll.GuiDraw(angle_cur, roll_result, wndSize.y * 0.15f, &is_cur_hovered);
        static bool is_hovered = false;
        static ImVec2 pos_mouseDown;
        if (is_cur_hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            is_hovered |= is_cur_hovered;
            pos_mouseDown = posRel;
        }
        if (is_hovered && !ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            is_hovered = false;
            ImVec2 deltDrag = { posRel.x - pos_mouseDown.x, posRel.y - pos_mouseDown.y };
            float drag_force = 12.0f;
            float drag_dist = posRel.x * deltDrag.y - posRel.y * deltDrag.x;
            auto real_drag = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
            ImGui::ResetMouseDragDelta();
            if (fabsf(drag_dist) >= 0.1f && hypotf(real_drag.x, real_drag.y) >= 5.0f) {
                first_roll = false;
                is_rolling = true;
                time = 0;
                angle_last = angle_cur - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
                angle_fin = angle_last + drag_force * drag_dist;
            }
        }

        if (!first_roll && !is_rolling) {
            auto result = roll_result;
            if (result != -1 && result < candidate.size()){
                char outputStr[256];
                sprintf_s(outputStr, S(THPRAC_TOOLS_ROLL_RESULT), S(candidate[result].title));
                mRollText = outputStr;
                mRollResult = candidate[result];
            }
        }

        if (GuiButtonRelCentered(mRollText.c_str(), 0.9f, ImVec2(1.0f, 0.08f))) {
            first_roll = false;
            is_rolling = true;
            time = 0;
            angle_last = angle_fin - floorf(angle_fin / (2.0f * std::numbers::pi)) * 2.0f * std::numbers::pi;
            angle_fin = (mRoll.GetRandomFloat() * 2.0f * std::numbers::pi) + 30.0f * std::numbers::pi;
        }
        if (mRollText != S(THPRAC_GAMEROLL_ROLL) && ImGui::BeginPopupContextItem("##roll_game_popup")) {
            if (mRollResult.playerSelect) {
                if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_PLAYER))) {
                    mRollText = S(THPRAC_GAMEROLL_ROLL);
                    LauncherToolsGuiSwitch(mRollResult.name);
                }
            }
            if (ImGui::Selectable(S(THPRAC_TOOLS_RND_TURNTO_GAME))) {
                GuiLauncherMainSwitchTab(S(THPRAC_GAMES));
                LauncherGamesGuiSwitch(mRollResult.name);
            }
            ImGui::EndPopup();
        }

        if (!result) {
            if (mRndTextGen() == 1) {
                mRollText = S(THPRAC_GAMEROLL_EENY_MEENY);
            } else {
                mRollText = S(THPRAC_GAMEROLL_ROLL);
            }
        }
        return result;
    }

private:
    std::function<unsigned int(void)> mRndTextGen;
    std::vector<GameRoll> mGameOption[4];
    std::string mRollText;
    GameRoll mRollResult;
    bool mGameTypeOpt[4];
};


class THToolsGui {
private:
    THToolsGui()
    {
        mGuiUpdFunc = [&]() { return GuiContent(); };
    }
    SINGLETON(THToolsGui);

public:
    void GuiUpdate()
    {
        GuiMain();
    }
    void Switch(const char* gameStr)
    {
        mGuiRollPlayer.SwitchGame(gameStr);
        mGuiUpdFunc = [&]() { return mGuiRollPlayer.GuiUpdate(); };
    }

private:
    float GetWidthRel(const char* text, float rsv = 0.0f)
    {
        auto offset = ImGui::GetFontSize() * rsv;
        return ImGui::GetStyle().FramePadding.x * 2 + ImGui::CalcTextSize(text).x + offset;
    }
    bool CenteredButton(const char* text, float posYRel, float width, int xIdx = 0,int xTotalColumn = 1,const char* helper = nullptr)
    {
        auto columnWidth = ImGui::GetColumnWidth();
        auto columnOffset = ImGui::GetColumnOffset();

        float xOfs = columnWidth / (xTotalColumn + 1.0f) * (xIdx+1.0f);
        float cursorX = xOfs - width / 2.0f + columnOffset;
        ImGui::SetCursorPosX(cursorX);
        GuiSetPosYRel(posYRel);
        bool btn = ImGui::Button(text, ImVec2(width, 0.0f));
        if (helper){
            ImGui::SameLine();
            HelpMarker(helper);
        }
        return btn;
    }

    void ClearGame()
    {
        auto game_killed = THClearGame();
        static char chs[256];
        sprintf_s(chs, S(THPRAC_KILL_ALL_GAME_DONE), game_killed.size());
        std::wstringstream ss;
        ss << utf8_to_utf16(chs) << L"\n";
        int n = 0;
        for (const auto& path : game_killed) {
            auto path_a = path;
            if (path_a.length() > 60) {
                path_a = path_a.substr(0, 57) + L"...";
            }
            ss << L" - " << path_a << L"\n";
            n++;
            if (n > 32) {
                //???
                ss << "...";
                break;
            }
        }
        MessageBoxW(NULL, ss.str().c_str(), L"done", MB_OK);
    }
    bool GuiContent()
    {
        
        float y_inc = 0.2f,y_init=0.16f;
        auto width = GetWidthRel(S(THPRAC_TOOLS_APPLY_THPRAC), 2.0f);

        float y = y_init;
        if (CenteredButton(S(THPRAC_TOOLS_APPLY_THPRAC), y, width, 0, 2, S(THPRAC_TOOLS_APPLY_THPRAC_DESC))) {
            FindOngoingGame(true);
        }
        y += y_inc;
        if (CenteredButton(S(THPRAC_KILL_ALL_GAME), y, width, 0, 2, S(THPRAC_KILL_ALL_GAME_ALERT))) {
            ClearGame();
        }
        y += y_inc;
        if (CenteredButton(S(THPRAC_TOOLS_INPUT_TEST), y, width, 0, 2)) {
            mGuiUpdFunc = [&]() { return mInputTest.GuiUpdate(); };
        }
        y += y_inc;
        if (CenteredButton(S(THPRAC_TOOLS_REACTION_TEST), y, width, 0, 2)) {
            mGuiUpdFunc = [&]() { return mGuiReactionTest.GuiUpdate(); };
        }
        y = y_init;
        if (CenteredButton(S(THPRAC_TOOLS_RND_GAME), y, width,1,2)) {
            mGuiUpdFunc = [&]() { return mGuiRollGame.GuiUpdate(); };
        }
        y += y_inc;
        if (CenteredButton(S(THPRAC_TOOLS_RND_PLAYER), y, width, 1, 2)) {
            mGuiUpdFunc = [&]() { return mGuiRollPlayer.GuiUpdate(); };
        }
        
        y += y_inc;
        if (CenteredButton(S(THPRAC_TOOLS_ROLLF_FROM_FILE), y, width, 1, 2)) {
            mGuiUpdFunc = [&]() { return mGuiRollAll.GuiUpdate(); };
        }
        y += y_inc;
        if (CenteredButton(S(THPRAC_TOOLS_DICE), y, width, 1, 2)) {
            mGuiUpdFunc = [&]() { return mGuiDice.GuiUpdate(); };
        }
        return true;
    }
    void GuiMain()
    {
        if (!mGuiUpdFunc()) {
            mGuiUpdFunc = [&]() { return GuiContent(); };
        }
    }

    std::function<bool(void)> mGuiUpdFunc = []() { return true; };
    THGuiRollGame mGuiRollGame;
    THGuiRollPlayer mGuiRollPlayer;
    THGuiTestReactionTest mGuiReactionTest;
    THGuiInputTest mInputTest;
    THGuiRollAll mGuiRollAll;
    THGuiDice mGuiDice;
};

bool LauncherToolsGuiUpd()
{
    THToolsGui::singleton().GuiUpdate();
    return true;
}

void LauncherToolsGuiSwitch(const char* gameStr)
{
    THToolsGui::singleton().Switch(gameStr);
}
}
