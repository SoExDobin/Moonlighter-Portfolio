# Moonlighter-Portfolio

《Moonlighter》를 DirectX 9 자체 엔진으로 옮긴 5인 팀 학습용 모작(2025.11.14 ~ 12.22)에서 제가 맡은 코드를 모은 공개본입니다. 원본은 팀 비공개 저장소 `Moonlighter-DX9`의 `develop`(`40e6c13`)이고, 파일 경로는 원본과 같습니다.

- 포트폴리오 페이지: https://soexdobin.xyz/projects/moonlighter/
- 시연 영상: https://www.youtube.com/watch?v=h-GAey4My9A

## 개요

| 항목 | 내용 |
| --- | --- |
| 개발 기간 | 2025.11 ~ 12 |
| 제작 | 5인 팀 · 학습 목적 모작 |
| 개발 환경 | C++ · DirectX 9 · D3DX9 |
| 라이브러리 | DirectInput 8 · FMOD · ImGui |
| 담당 | Engine 프레임워크(GameObject · Component · Layer · 충돌 · Scene 전환) · Client 사막 던전 보스 |

## 담긴 코드

- **Engine 프레임워크** (`Engine/Code`, `Engine/Header`, `Base`): GameObject · Component · Layer, LayerMask 충돌 필터와 Rect · Sphere 판정, Scene 관리와 DontDestroy 캐시, 렌더 그룹, Prototype · Resource · Sound Manager
- **사막 던전 보스** (`Client`): 머리와 몸체 네 개, 머리 이동 Component, WallChecker, 그룹 HP Bar, 투사체 · 용암 장판 · 불 몬스터
- **프레임워크를 쓰는 Client 코드**: 프레임 순서(`CMainApp`), Fade(`CFadeManager`, `CFadeUI`), `CLogoScene`, `CTrigger`, Client Component 작성 예제(`CTmpCustomComponent`, `CExampleObject`, `CExampleManager`)

## 파일 지도

| 내용 | 파일 |
| --- | --- |
| 프레임 실행 순서 | `Client/Code/CMainApp.cpp`, `Engine/Code/CScene.cpp`, `Engine/Code/CLayer.cpp`, `Engine/Code/CGameObject.cpp`, `Engine/Code/CRenderer.cpp` |
| GameObject · Component | `Engine/Header/CGameObject.h`(`Add_Component`), `Engine/Code/CPrototypeManager.cpp`, `Client/Code/CTmpCustomComponent.cpp` |
| Layer · LayerMask 충돌 필터 | `Engine/Header/CLayerHelper.h`, `Engine/Header/TagLayerMask.h`, `Engine/Code/TagLayerMask.cpp`, `Engine/Code/CCollisionManager.cpp`(`Update_Collision`) |
| Destroy · DontDestroySceneLoad | `Engine/Header/CGameObject.h`, `Engine/Code/CManagement.cpp`(`Set_Scene`, `Load_CacheObject`, `Render_Scene`의 PendingEvent) |
| Collider 참조 해제 | `Engine/Code/CCollider.cpp`, `Engine/Code/CCollisionManager.cpp`(`Remove_Collider`, `Remove_CheckMember`) |
| Sphere · Rect 판정 | `Engine/Code/CCollisionManager.cpp`(`SphereRectCollision`) |
| Fade 전환 | `Client/Code/CFadeManager.cpp`, `Client/Code/CFadeUI.cpp` |
| 사막 던전 보스 | `Client/Code/CBossDesert.cpp`(궤적 추종 · 벽 회전 · 공격 선택), `Client/Code/CBoss3CircleBody.cpp`(Body Light 반사), `Client/Code/CBossDesertHPBar.cpp` |

## 작성자 표시

팀 프로젝트라 일부 파일에는 팀원이 고친 줄이 섞여 있습니다. `source-manifest.json`에 파일마다 원본 커밋 기준 `git blame` 줄 수와 다른 작성자를 적었습니다. blame은 각 줄을 마지막으로 고친 사람을 보여 주므로 기여량 자체와는 다릅니다.

## 빠진 것

- 게임 리소스(`Client/Bin`)와 원본 Git 이력
- 팀원이 주로 작성한 Client 코드(Player, 몬스터, UI, 맵 등)와 Engine 파일 일부(지형 텍스처, 카메라 등)
- FMOD · ImGui 같은 외부 라이브러리

그래서 이 저장소만으로는 빌드되지 않습니다.
