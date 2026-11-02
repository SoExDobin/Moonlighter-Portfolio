#include "pch.h"
#include "CFadeManager.h"
#include "CLayerHelper.h"
#include "CDataManager.h"
#include "CManagement.h"

IMPLEMENT_SINGLETON(CFadeManager)

CFadeManager::CFadeManager()
    : m_pFadeUI(nullptr)
{
}

CFadeManager::~CFadeManager()
{
    Free();
}

HRESULT CFadeManager::Ready_FadeManager(LPDIRECT3DDEVICE9 pGrapicDev)
{
    m_pFadeUI = CFadeUI::Create(pGrapicDev);
    if (m_pFadeUI == nullptr) return E_FAIL;
    m_pFadeUI->AddRef();

    return S_OK;
}

HRESULT CFadeManager::Init_FadeUI(CScene* pScene)
{
    CLayer* pLayer = pScene->Get_Layer(CDataManager::Get_LayerTag(UI_LAYER));
    if (pLayer == nullptr) return E_FAIL;

    pLayer->Add_GameObject(L"FadeUI", m_pFadeUI);
    CGameObject::DontDestroySceneLoad(m_pFadeUI);

    return S_OK;
}

void CFadeManager::Fade_In(function<void()> funcAfterEvent)
{
    if (m_pFadeUI == nullptr) return;

    m_pFadeUI->StartFadeIn();

    if (funcAfterEvent != NULL)
        m_AfterEventlist.push_back(funcAfterEvent);
}

void CFadeManager::Fade_Out(function<void()> funcAfterEvent)
{
    if (m_pFadeUI == nullptr) return;

    m_pFadeUI->StartFadeOut();

    if (funcAfterEvent != NULL)
        m_AfterEventlist.push_back(funcAfterEvent);
}

void CFadeManager::Check_FadeEnd_Event()
{
    if (m_pFadeUI->Is_Fading_End())
    {
        if (m_AfterEventlist.empty()) return;

        CManagement::GetInstance()->Add_PendingEvent(m_AfterEventlist.front());
        m_AfterEventlist.pop_front();
    }
}

void CFadeManager::Free()
{
    Safe_Release(m_pFadeUI);
}
