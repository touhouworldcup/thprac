## [简体中文](/README_CN.md) [日本語](/README_JP.md) [한국어] (/README_KO.md)

# **thprac**
> thprac은 동방 프로젝트 슈팅 게임의 프랙티스(연습) 툴입니다.

[![ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/V7V7O03J4)

## [다운로드](https://github.com/touhouworldcup/thprac/releases/latest) - [베타판 다운로드](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip)
> 베타 버전은 다음 베타 버전이 아닌, 다음 안정 릴리즈 버전으로만 업데이트된다는 점에 유의하세요.

## **목차**
* [다운로드](#다운로드)
* [사용법](#사용법)
* [호환성](#호환성)
* [주요 기능](#주요-기능)
* [빠른 메뉴](#빠른-메뉴)
* [고급 옵션](#고급-옵션)
* [게임별 특화 기능](#게임별-특화-기능)
* [크레딧](#크레딧)
* [소스 코드 빌드 방법](#소스-코드-빌드-방법)
* [자주 묻는 질문(FAQ)](#자주-묻는-질문(FAQ))

## <a name="다운로드"></a> **다운로드**
이 툴을 다운로드하려면 이곳에서 [최신 릴리즈](https://github.com/touhouworldcup/thprac/releases/latest) 를, 이곳에서 [최신 베타](https://nightly.link/touhouworldcup/thprac/workflows/main/master/thprac.zip) 를 다운로드할 수 있습니다. 최신 베타 버전은 다음 베타 버전이 아닌 다음 안정(stable) 릴리즈 버전으로 업데이트된다는 점을 다시 한번 유의해 주세요.

## <a name="사용법"></a> **사용법**
이 툴은 다양한 방식으로 사용할 수 있습니다. 가장 주된 방법은 **게임과 동일한 폴더에 실행 파일을 넣는 것**, **게임을 실행한 후 이 실행 파일을 켜는 것**, 또는 **thprac의 런처를 사용하는 것**입니다.

### **게임 폴더에 thprac 넣기**
**이 방법은 Steam 게임에서는 작동하지 않습니다.** `thprac.exe` 파일을 게임과 동일한 폴더에 넣어서 실행할 수 있습니다. 이 방법은 vpatch도 자동으로 감지합니다. 이 기능을 사용하려면 게임이 최신 버전으로 업데이트되어 있어야 하며, 아래의 파일명 규칙 중 하나를 따라야 합니다. (vpatch 환경에서도 작동합니다)
- thXX.exe (대부분의 게임)
- 東方紅魔郷.exe (동방홍마향)
- alcostg.exe (황혼주점)

### **게임을 먼저 실행한 후 thprac 켜기**
**이 방법은 대부분의 환경에서 작동합니다.** 먼저 원하는 방식(thcrap, vpatch, Steam 등)으로 게임을 실행한 다음 `thprac.exe`를 실행합니다. 그러면 thprac이 현재 실행 중인 게임을 감지하고 툴을 적용하려고 시도할 것입니다. thprac이 적용된 것을 확인하려면 게임의 메인 메뉴 화면으로 돌아가야 할 수도 있습니다.

### **런처를 사용하여 thprac 켜기**
아래 영상은 런처 사용 방법을 설명하고 있습니다. (이미지를 클릭하세요)
[![thprac 한국어 소개 영상 (26/06/02)](https://github.com/user-attachments/assets/e5a5e2d1-ceb9-4936-b2b3-a9623afd516e)](https://www.youtube.com/watch?v=oAfq3Q8uho8)

### 명령줄
다음과 같은 명령줄 인자(옵션)를 지원합니다:
- `<게임 exe 경로>` The command `thprac.exe <게임 exe 경로>` 명령어를 사용하면 확인 팝업 없이 게임을 실행하고 thprac을 즉시 적용합니다. 즉, 동방 게임 실행 파일을 thprac.exe 위로 드래그 앤 드롭하기만 해도 thprac이 적용된 상태로 실행할 수 있습니다. 참고로 `<게임 exe 경로>` 뒤에 인자를 더 추가하더라도 이는 게임 실행 파일로 그대로 전달될 뿐입니다. ZUN은 명령줄 매개변수를 사용하지 않기 때문에 이는 완전히 무의미합니다.
- `--attach <pid>` 확인 팝업 없이 즉시 프로세스 ID가 `<pid>`인 프로세스에 thprac을 주입합니다.
- `--attach` (다른 플래그 없이 사용): `thprac.exe --attach` 명령어는 확인 팝업을 띄우지 않고 가장 먼저 발견된 동방 게임 프로세스에 thprac을 주입합니다.
- `--without-vpatch` vpatch를 자동으로 적용하는 것을 방지합니다.
- `--without-oilp` OpenInputLagPatch를 자동으로 적용하는 것을 방지합니다.

마지막 두 플래그에 대한 짧은 참고: 두 플래그 모두 지정되지 않은 상태에서 vpatch와 OpenInputLagPatch가 모두 존재할 경우, OpenInputLagPatch가 우선적으로 적용됩니다.

명령어 예시:
```
thprac.exe --attach 1234
thprac.exe --attach
thprac.exe C:\Users\Name\Desktop\Games\Touhou\th17\th17.exe
thprac.exe --without-vpatch C:\Users\Name\Desktop\Games\Touhou\th12\th12.exe
```

## <a name="호환성"></a> **호환성**
thprac은 **Windows Vista** 이상의 모든 Windows 버전을 공식적으로 지원합니다. Windows XP의 경우 [One-Core-API](https://github.com/Skulltrail192/One-Core-API-Binaries) 를 사용하여 구동될 수 있으나, 적극적으로 테스트하고 있지는 **않습니다**.

또한 thprac은 **Wine** 및 Steam Deck과도 호환됩니다. 퀵 메뉴는 Steam Deck에서 잘 작동하도록 맞춤 제작되었지만, 아직 완전히 테스트되지는 않았습니다.

## <a name="주요-기능"></a> **주요 기능**

thprac은 모든 정규 시리즈는 물론 요정대전쟁, 황혼주점까지 지원하는 강화된 프랙티스 모드를 제공합니다.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174433923-0a6069e7-d10d-4107-8f0d-f4a8a9d56976.png)

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174433975-8f23b0b0-e48e-4be1-8cb7-d8e3e7ab6b8e.png)

thprac은 위 사진과 같이 원래의 프랙티스 메뉴를 새로운 UI로 완전히 교체합니다. 특정 스펠 카드에서는 페이즈를 선택하거나 세부 매개변수를 직접 조절할 수도 있습니다. 원래 프랙티스 모드가 존재하지 않는 게임들(황혼주점, 요정대전쟁 등)의 경우, "Start Game" 메뉴 안에 프랙티스 메뉴가 추가됩니다.

![Uwabami Breakers](https://user-images.githubusercontent.com/23106652/174434103-5fee7a13-0254-4602-a468-42330b985bb2.png)
![Great Fairy Wars](https://user-images.githubusercontent.com/23106652/174434121-063142f2-ef3d-4721-ab96-a252343cdb0e.png)

이 메뉴는 키보드나 컨트롤러를 사용해 조작할 수 있습니다. **위**, **아래** 키를 사용해 옵션을 선택하고, **샷** 키를 눌러 실행합니다. 마우스를 사용해 메뉴와 상호작용하는 것도 가능합니다.

또한, thprac은 사용자가 선택한 프랙티스 옵션을 리플레이 저장 시 자동으로 포함시킵니다. 해당 리플레이를 재생할 때 thprac이 알아서 그 옵션들을 그대로 적용해 줍니다. **단, "Custom(커스텀)" 모드로 thprac을 사용 중에 저장된 리플레이는 thprac이 없는 순정 상태의 게임에서는 정상적으로 작동하지 않습니다.**

## <a name="빠른-메뉴"></a> **빠른 메뉴**
지원되는 모든 게임에서 **(동방화영총(TH09) 및 동방수왕원(TH19) 제외)**, **`백스페이스(Backspace)`** 키를 눌러 퀵 메뉴로 진입할 수 있습니다. 이 메뉴에서는 기능키(F1~F12)를 눌러 활성화할 수 있는 다양한 편의 옵션을 보여줍니다.

이 옵션들은 마우스로 클릭할 수도 있습니다. 즉, 데스크톱에서는 마우스를 쓰거나, Steam Deck 환경에서는 왼쪽 엄지로 화면을 탭하여 사용할 수 있습니다. Steam Deck에서 이 기능을 사용하려면 Steam Input을 통해 **`백스페이스(Backspace)`** 키를 매핑해 두세요.

만약 다른 키로 열고 싶다면, 런처의 설정(Settings) 탭에서 이 메뉴를 여는 단축키를 변경할 수 있습니다.

![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174434813-73748a66-0f6d-4c6e-9f3a-895a49b93434.png)
![Wily Beast and Weakest Creature](https://user-images.githubusercontent.com/23106652/174434834-6bd93104-1ed2-48ae-a440-9d9cb871ea03.png)

## <a name="고급-옵션"></a> **고급 옵션**
지원되는 모든 게임에서 **(동방화영총(TH09) 및 동방수왕원(TH19) 제외)**, **`F12`** 를 눌러 고급 옵션에 접근할 수 있습니다. 이 옵션들은 게임의 버그 픽스, 패치, 또는 삶을 편하게 해주는 유용한 기능들을 제공합니다.

퀵 메뉴와 마찬가지로, 이 기능을 여는 단축키 역시 런처의 설정 탭에서 변경할 수 있습니다.

![Imperishable Night](https://user-images.githubusercontent.com/23106652/174434977-683da583-324b-4bd5-8408-13373dfd5a93.png)
![Unconnected Marketeers](https://user-images.githubusercontent.com/23106652/174435006-e906d30d-0ef5-4930-ae57-1f0919beb5af.png)

## <a name="게임별-특화-기능"></a> **게임별 특화 기능**
### 불릿필리아들의 암시장 - 강제 웨이브 선택
![100th Black Market](https://github.com/touhouworldcup/thprac/assets/23106652/02c55e5e-5c89-462f-beea-9ab07cbb1051)

"커스텀" 모드로 thprac을 활성화한 경우, 게임이 웨이브를 선택하기 직전에 위와 같은 팝업 창이 나타납니다.

### 동방화영총 - 툴
![Phantasmagoria of Flower View](https://user-images.githubusercontent.com/23106652/174434249-2bf1d70a-101c-4538-a4e6-8eeaf273dd88.png) 

매치 모드에서 게임을 시작할 때, 모드 선택 창에서 "커스텀"을 선택할 수 있습니다. 그렇게 하면 게임 화면 안에 위 사진과 같은 창이 나타납니다. 이 창은 자유롭게 이동시키고 조절할 수 있습니다. 이 기능은 고급 옵션을 대체하며 **`F12`** 키로 열고 닫을 수 있습니다. 

### 동방수왕원 - 툴
![TH19 Tools](https://github.com/touhouworldcup/thprac/assets/23106652/a2cdb385-b61d-4111-af6b-b195e85bf18a)

화영총의 툴과 완전히 동일하며, 동방수왕원을 위해 제공됩니다.

### 동방홍마향 - 일시정지 메뉴
![TH06](https://user-images.githubusercontent.com/23106652/174436027-734d642a-300c-45ab-9591-b6219aca087b.png)

"Exit(타이틀로)"를 선택할 때 리플레이를 저장할 것인지 묻는 기능이 추가되었습니다. 

**경고**: 이런 식으로 중간에 저장된 리플레이는 게임을 저장했던 지점 이후로도 계속 이어집니다. 다만, 리플레이 속 플레이어 캐릭터는 움직이지 않으며 탄도 쏘지 않고 가만히 서 있게 됩니다.

### 홍마향, 비봉 나이트메어 다이어리 및 이후 작품들의 ESC + R (빠른 재시작) 픽스

## <a name="크레딧"></a> **크레딧 (2022년 5월 27일 이후)**
- 개발: [32th System](https://www.youtube.com/channel/UChyVpooBi31k3xPbWYsoq3w), [muter3000](https://github.com/muter3000), [zero318](https://github.com/zero318), [Lmocinemod](https://github.com/Lmocinemod), [Cao Minh](https://github.com/hoangcaominh), [raviddog](https://github.com/raviddog) 
- 중국어 번역: [CrestedPeak9](https://twitter.com/CrestedPeak9), maksim71_doll, DeepL
- 일본어 번역: [Yu-miya](https://www.twitch.tv/toho_yumiya), [SOC](https://github.com/soc-3), [wefma](https://github.com/wefma), CyrusVorazan, DeepL
- 한국어 번역: [Tea Barley](https://www.youtube.com/@teabarley), [Srty7462](https://www.youtube.com/@srty7462), [Sepheille](https://www.youtube.com/@cinitalp)
- [이전 버그 트래커] (https://github.com/ack7139/thprac/issues) 의 모든 리포트를 [현재 위치](https://github.com/touhouworldcup/thprac/issues) 로 이전: [toimine](https://www.youtube.com/channel/UCtxu8Rg0Vh8mX6iENenetuA)
- 영어 README.md: [Galagyy](https://github.com/Galagyy)
- 중국어 README.md 번역: [TNT569](https://github.com/TNT569), [H-J-Granger](https://github.com/H-J-Granger)
- 일본어 README.md 번역: [wefma](https://github.com/wefma)
- 한국어 README.md 번역: [Tea Barley](https://www.youtube.com/@teabarley)

## <a name="소스-코드-빌드-방법"></a> **소스 코드 빌드 방법**
### 최초 설정 (처음 한 번만 수행)
`thprac`이라는 이름의 폴더 안에서 `loc_json.cpp`를 `loc_json.exe` 로 컴파일해야 합니다. 어떤 방식이든 편한 방법으로 컴파일하면 됩니다. 아래의 명령어를 추천합니다.
```
cl /Isrc\3rdparties\yyjson /nologo /EHsc /O2 /std:c++20 loc_json.cpp .\src\3rdParties\yyjson\yyjson.c /Fe:loc_json.exe
```

### 명령줄에서 빌드하기
Visual Studio 개발자 명령 프롬프트에서 다음 명령어를 실행합니다.
```
msbuild thprac.sln -t:restore,build -p:RestorePackagesConfig=true,Configuration=Release
```

### Visual Studio GUI에서 빌드하기
`thprac.sln` 파일을 열고 상단의 "빌드"를 클릭한 후, "솔루션 빌드"를 클릭합니다.

## <a name="자주-묻는-질문(FAQ)"></a> **자주 묻는 질문(FAQ)**

### 일반

#### thprac 프로젝트가 왜 이곳으로 옮겨졌나요?
원래 개발자인 Ack님이 thprac/Marketeer의 향후 개발을 무기한 중단했기 때문입니다. 아래는 원작자의 코멘트입니다:
> 저는 thprac/Marketeer의 모든 향후 개발을 무기한 중단할 것입니다. 라이선스 조항만 준수하신다면 누구든 계속해서 이 프로젝트를 이어나가셔도 좋습니다.  
> 제 실력이 부족한 탓에 코드에는 이해하기 어려운 작성 방식과 끔찍한 로직들이 넘쳐나며, 전체적으로 엉망진창이 되었습니다. 이로 인해 불편을 드렸다면 죄송합니다.  

현재 Ack님과는 연락이 닿지 않으며, 다른 개발자들이 이어받아 개발을 진행하고 있습니다.

#### 제 백신이 thprac을 악성코드로 인식합니다. 안전한가요?
thprac에는 악성 코드가 포함되어 있지 않지만, 게임 메모리에 접근하는 프로그램의 특성상 백신이 이를 오진할 수 있습니다. 경고가 뜰 경우, thprac이 작동할 수 있도록 백신에 예외 설정이나 신뢰 규칙을 추가해 주세요. 그래도 문제가 해결되지 않는다면 [이 버전](https://github.com/touhouworldcup/thprac/issues/112) 을 시도해 보시고, 계속 문제가 발생하면 리포트해 주세요.

---

### 호환성

#### thprac은 영어 패치와도 함께 사용할 수 있나요?
thprac은 **thcrap**과 호환되며, [Universal THCRAP Launcher](https://github.com/thpatch/Universal-THCRAP-Launcher/) 와 마찬가지로 thcrap 런처 역할을 수행할 수 있습니다. 단, gensokyo.org에서 만든 것과 같은 정적 영어 패치는 지원하지 않습니다.

---

#### thprac이 동방홍마향을 찾지 못합니다. 어떻게 해야 하나요?

게임 실행 파일의 이름이 `東方紅魔郷.exe` 또는 `th06.exe`로 되어 있는지 확인하세요. 그래도 thprac이 게임을 감지하지 못한다면, 런처를 통해 파일 이름에 상관없이 게임을 수동으로 실행시킬 수 있으며, 이 경우 thprac이 강제로 적용됩니다.

---

### 주요 기능

#### TH06-10에서 게임 플레이 도중에 리플레이를 저장하려면 어떻게 해야 하나요?
기술적인 한계로 인해 구작들에서의 인게임 도중 리플레이 저장은 지원하지 않습니다. thprac 2.0.8.3 매뉴얼에 따르면:
> 이 게임들이 프로그래밍된 방식 때문에, 이 기능을 추가하는 것은 꽤나 번거로운 작업입니다. 따라서 현재로서는 이에 대한 직접적인 해결책이 없습니다.  

다만, 동방홍마향의 경우 현재 이 기능이 추가되어 지원됩니다.

참고: 게임 중간에 저장된 리플레이는 재생 시 저장된 지점 이후에 타이틀 화면으로 돌아가지 않습니다.

---

#### "BGM 지속 재생"이 무슨 뜻인가요?
게임을 재시작할 때 배경 음악(BGM)이 초기화되어 처음부터 다시 재생되는 것을 막아주는 옵션입니다.

---

#### "강제 취재"가 무슨 뜻인가요? (동방문화첩/더블 스포일러)
이 기능은 카메라가 항상 보스를 추적(락온)하도록 고정하며, 카메라의 재충전 시간을 완전히 없애줍니다.

---

#### 언어는 어떻게 전환하나요?
- 런처는 Windows 시스템 설정에 따라 자동으로 언어를 선택합니다.  
- 게임 내에서 언어를 변경하려면 다음 단축키를 사용하세요:  
  - **`ALT + 1`**: 일본어 
  - **`ALT + 2`**: 중국어 
  - **`ALT + 3`**: 영어
  - **`ALT + 4`**: 한국어 

이 언어 변경 단축키는 런처의 설정 탭에서 변경할 수 있습니다.

참고로 이 단축키들은 런처 자체에서는 작동하지 않습니다. 자세한 지침은 **"사용법"** 섹션의 동영상을 참조해 주세요.

---

### 버그 리포트

#### 버그는 어디에 제보하나요?
[깃허브의 Issues 탭](https://github.com/touhouworldcup/thprac/issues) 을 방문하여 버그를 제보하거나 개선 사항을 제안해 주세요.

---

### 기술적 문제

#### 고급 옵션에서 지원하지 않는 VsyncPatch 버전이라고 뜹니다.
호환되는 VsyncPatch 버전을 사용하고 있는지 확인하세요. [이곳](https://maribelhearn.com/tools#vpatch) 에서 다운로드할 수 있습니다. 가급적 **rev7** 버전의 DLL 파일들을 사용하시기 바랍니다.

---

#### vpatch를 사용할 때 프레임(FPS) 조절이 고장난 것 같습니다.

"DX8 to DX9 Converter"와 같은 일부 도구는 VsyncPatch와 충돌을 일으킵니다. 리플레이 속도 조절(느리게/빠르게) 기능은 현재 **동방신령묘(TH13)** 에서만 지원됩니다.

---

### 게임별 특화 정보

#### "휘침성 - 마리사 레이저 관련" 기능이 무엇인가요?
**동방휘침성 (TH14)**에서 발생하는 악명 높은 마리사 레이저의 프레임 동기화 오류  버그를 고쳐줍니다. 원 개발자인 Ack 님의 [빌리빌리 시연 영상](https://www.bilibili.com/video/av285566068) (중국어, [Youtube 백업 영상 링크](https://www.youtube.com/watch?v=Hkh_AEGHLto)) 을 참고해 주세요.
