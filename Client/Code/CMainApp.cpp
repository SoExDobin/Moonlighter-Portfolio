#include "pch.h"
#include "CMainApp.h"

#include "CManagement.h"
#include "CDInputManager.h"
#include "CRenderer.h"
#include "CPrototypeManager.h"
#include "CDataManager.h"
#include "CEditor.h"
#include "CLightManager.h"
#include "CCollisionManager.h"
#include "CFontManager.h"
#include "CLayerHelper.h"
#include "CLogoScene.h"

#include "CLoadingScene.h"

#include "CMainScene.h"
#include "CPlayerTestScene.h"
#include "CUITestScene.h"
#include "CEngineMediator.h"
#include "CMonsterTestScene.h"
#include "CComposeScene.h"
#include "CEditScene.h"
#include "CCameraManager.h"
#include "CDungeonManager.h"
#include "CInvenManager.h"
#include "CPageManager.h"
#include "CItemManager.h"
#include "CMapManager.h"
#include "CWorldItemManager.h"
#include "CFadeManager.h"
#include "CNavigation.h"
#include "CNPCManager.h"

#include <ctime>
#include <cstdlib>

CMainApp::CMainApp()
	: m_pDeviceClass(nullptr), m_pGraphicDevice(nullptr)
	, m_pManageClass(nullptr)
{
	m_pManageClass = CManagement::GetInstance();
}

CMainApp::~CMainApp()
{

}

HRESULT CMainApp::Ready_MainApp()
{
    srand(static_cast<unsigned>(time(nullptr)));

	if (FAILED(Ready_DefaultSetting(&m_pGraphicDevice)))
		return E_FAIL;

	if (FAILED(Ready_Scene(m_pGraphicDevice)))
		return E_FAIL;

    CWorldItemManager::GetInstance()->Set_Device(m_pGraphicDevice);

	return S_OK;
}

_int CMainApp::Update_MainApp(const _float fDeltaTime)
{
	Engine::CDInputManager::GetInstance()->Update_InputDev();

	m_pManageClass->Update_Scene(fDeltaTime);

    Engine::CCollisionManager::GetInstance()->Update_Collision();

	return 0;
}

void CMainApp::LateUpdate_MainApp(const _float fDeltaTime)
{
	m_pManageClass->LateUpdate_Scene(fDeltaTime);
    CFadeManager::GetInstance()->Check_FadeEnd_Event();
}

void CMainApp::Render_MainApp()
{
	m_pDeviceClass->Render_Begin(D3DXCOLOR(0.7f, 0.9f, 1.0f, 1.f));

    //ImGui::Begin() must be called between CEditor::Render_Begin() and CEditor::Render_End().
    //CEditor::GetInstance()->Render_Begin();
	//CEditor::GetInstance()->Render_Editor();

	m_pManageClass->Render_Scene(m_pGraphicDevice);
#ifdef _DEBUG
     //Engine::CCollisionManager::GetInstance()->Render_Collision();
#endif 

    CEngineMediator::GetInstance()->Render_Mediator();
	//CEditor::GetInstance()->Render_End();

	m_pDeviceClass->Render_End();
}

HRESULT CMainApp::Ready_DefaultSetting(LPDIRECT3DDEVICE9* ppGraphicDevice)
{
	if (FAILED(CGraphicDevice::GetInstance()->Ready_GraphicDevice(
		g_hWnd, MODE_WIN, WINCX, WINCY, &m_pDeviceClass
	)))
	{
		return E_FAIL;
	}
	m_pDeviceClass->AddRef();

	(*ppGraphicDevice) = m_pDeviceClass->Get_GraphicDevice();
	(*ppGraphicDevice)->AddRef();

	(*ppGraphicDevice)->SetRenderState(D3DRS_ZENABLE, TRUE);
	(*ppGraphicDevice)->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	//(*ppGraphicDevice)->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	//(*ppGraphicDevice)->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

    // 폰트
    // 불러오기
    CFontManager::GetInstance()->Register_FontFile(
        L"../Bin/Resource/Font/Galmuri11Condensed.ttf"
    );

    // 등록된 폰트 이름으로 폰트 생성
    CFontManager::GetInstance()->Ready_Font(
        m_pGraphicDevice,
        L"Moon_Font",   // wsFontTag (네가 붙이는 별명)
        L"Galmuri11 Condensed",    // -> 띄어쓰기 건드리면 안돼요! wsFontType (폰트 이름) 
        0,                 // Width (0이면 자동)
        24,                // Height
        FW_NORMAL          // Weight
    );

    CFontManager::GetInstance()->Ready_Font(
        m_pGraphicDevice,
        L"Moon_Font_Loading",   // wsFontTag (네가 붙이는 별명)
        L"Galmuri11 Condensed",    // -> 띄어쓰기 건드리면 안돼요! wsFontType (폰트 이름) 
        20,                 // Width (0이면 자동)
        48,                // Height
        FW_NORMAL          // Weight
    );

    if (FAILED(CDataManager::GetInstance()->Ready_Prototype(m_pGraphicDevice)))
        return E_FAIL;
    if (FAILED(CFadeManager::GetInstance()->Ready_FadeManager(m_pGraphicDevice)))
        return E_FAIL;
    if (FAILED(CItemManager::GetInstance()->Ready_ItemManager()))
        return E_FAIL;
	if (FAILED(CDInputManager::GetInstance()->Ready_InputDev(g_hInst, g_hWnd)))
		return E_FAIL;
	if (FAILED(CDataManager::GetInstance()->Ready_Data((*ppGraphicDevice))))
		return E_FAIL;
	if (FAILED(CEditor::GetInstance()->Ready_Editor(g_hWnd, m_pGraphicDevice)))
		return E_FAIL;
    if (FAILED(CEngineMediator::GetInstance()->Ready_Mediator(m_pGraphicDevice)))
        return E_FAIL;
    if (FAILED(CNPCManager::GetInstance()->Ready_NPCManager(m_pGraphicDevice)))
        return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Scene(LPDIRECT3DDEVICE9 pGraphicDevice)
{
    CScene* pLoadingScene = CLoadingScene::Create(pGraphicDevice);
    //CFadeManager::GetInstance()->Init_FadeUI(pLoadingScene);
    if (FAILED(Engine::CManagement::GetInstance()->Init_Scene(pLoadingScene)))
    	return E_FAIL;

	return S_OK;
}

CMainApp* CMainApp::Create()
{
	CMainApp* pApp = new CMainApp();
	if (FAILED(pApp->Ready_MainApp()))
	{
		MSG_BOX("CMainApp Create Failed");
		Safe_Release(pApp);
		return nullptr;
	}

	return pApp;
}

void CMainApp::Free()
{
	Safe_Release(m_pDeviceClass);
	Safe_Release(m_pGraphicDevice);

    CWorldItemManager::DestroyInstance();
    CMapManager::DestroyInstance();
    CItemManager::DestroyInstance();
    CInvenManager::DestroyInstance();
	CDataManager::DestroyInstance();
    CEngineMediator::DestroyInstance();
    CCameraManager::DestroyInstance();
    CFadeManager::DestroyInstance();
    CNavigation::DestroyInstance();
    CNPCManager::DestroyInstance();

    if (CDungeonManager::s_bInitialized)
        CDungeonManager::DestroyInstance();

    Engine::CLayerHelper::DestroyInstance();
    Engine::CFontManager::DestroyInstance();
	Engine::CEditor::DestroyInstance();
	Engine::CLightManager::DestroyInstance();
	Engine::CDInputManager::DestroyInstance();
	Engine::CRenderer::DestroyInstance();
	Engine::CGraphicDevice::DestroyInstance();
	Engine::CManagement::DestroyInstance();
	Engine::CTimeManager::DestroyInstance();
	Engine::CFrameManager::DestroyInstance();
	Engine::CPrototypeManager::DestroyInstance();
    Engine::CCollisionManager::DestroyInstance();
    CPageManager::DestroyInstance();
}
