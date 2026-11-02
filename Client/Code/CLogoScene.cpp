#include "pch.h"
#include "CLogoScene.h"
#include "CDataManager.h"
#include "CCameraManager.h"
#include "CDInputManager.h"
#include "CManagement.h"
#include "CPlayer.h"
#include "CStartButton.h"
#include "CEndButton.h"
#include "CPlayerButton.h"
#include "CMonsterButton.h"
#include "CUIButton.h"
#include "CMapButton.h"
#include "CButtonN.h"
#include "CFadeManager.h"

#include "CLogoUI.h"
#include "Client_Define.h"
#include "CComposeScene.h"
#include "CVillageScene.h"
#include "CShopScene.h"
#include "CUITestScene.h"
#include "CMapManager.h"
#include "CSoundManager.h"

CLogoScene::CLogoScene(LPDIRECT3DDEVICE9 pGraphicDev)
    : CScene(pGraphicDev)
{
}

CLogoScene::~CLogoScene()
{
}

HRESULT CLogoScene::Ready_LogoScene(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CScene::Ready_Scene();

    CLayer* pUILayer = Get_Layer(CDataManager::GetInstance()->Get_LayerTag(UI_LAYER));

    CGameObject* pLogo = CLogoUI::Create(pGraphicDev);
    if (pLogo == nullptr)
        return E_FAIL;
    if (FAILED(pUILayer->Add_GameObject(L"LogoUI", pLogo)))
        return E_FAIL;

    // 버튼
    CGameObject* pButton = nullptr;
    pButton = CStartButton::Create(pGraphicDev);
    if (pButton == nullptr)
        return E_FAIL;
    if (FAILED(pUILayer->Add_GameObject(L"StartButton", pButton)))
        return E_FAIL;

    pButton = nullptr;
    pButton = CEndButton::Create(pGraphicDev);
    if (pButton == nullptr)
        return E_FAIL;
    if (FAILED(pUILayer->Add_GameObject(L"EndButton", pButton)))
        return E_FAIL;
    /*
#ifdef _DEBUG
    {

        pButton = nullptr;
        pButton = CPlayerButton::Create(pGraphicDev);
        if (pButton == nullptr)
            return E_FAIL;
        if (FAILED(pUILayer->Add_GameObject(L"PlayerButton", pButton)))
            return E_FAIL;

        pButton = nullptr;
        pButton = CMonsterButton::Create(pGraphicDev);
        if (pButton == nullptr)
            return E_FAIL;
        if (FAILED(pUILayer->Add_GameObject(L"MonsterButton", pButton)))
            return E_FAIL;

        pButton = nullptr;
        pButton = CUIButton::Create(pGraphicDev);
        if (pButton == nullptr)
            return E_FAIL;
        if (FAILED(pUILayer->Add_GameObject(L"UIButton", pButton)))
            return E_FAIL;

        pButton = nullptr;
        pButton = CMapButton::Create(pGraphicDev);
        if (pButton == nullptr)
            return E_FAIL;
        if (FAILED(pUILayer->Add_GameObject(L"MapButton", pButton)))
            return E_FAIL;
    }
#endif
*/
    if (CGameObject* pPlayer = CPlayer::Create(pGraphicDev))
    {
        pPlayer->Init_Layer(CDataManager::GetInstance()->Get_LayerTag(PLAYER_LAYER), L"Player");
        Engine::CGameObject::DontDestroySceneLoad(pPlayer);
    }

    CFadeManager::GetInstance()->Fade_Out();
    CMapManager::GetInstance()->SetLogoPreTr();
    CMapManager::GetInstance()->SetTerrainEnum(TR_LOGO);
    CSoundManager::GetInstance()->PlaySoundLoopSection(
        L"main_menu_screen.wav"
        , CHANNEL_0, 0.5f, 8000, 32000, true);
    return S_OK;
}


_int CLogoScene::Update_Scene(const _float fTimeDelta)
{
    _int iExit = Engine::CScene::Update_Scene(fTimeDelta);

    return iExit;
}

void CLogoScene::LateUpdate_Scene(const _float fTimeDelta)
{
    Engine::CScene::LateUpdate_Scene(fTimeDelta);
}

CLogoScene* CLogoScene::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CLogoScene* pInstance = new CLogoScene(pGraphicDev);

    if (FAILED(pInstance->Ready_LogoScene(pGraphicDev)))
    {
        MSG_BOX("CLogoScene Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CLogoScene::Free()
{
    CScene::Free();
}
