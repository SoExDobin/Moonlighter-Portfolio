#include "pch.h"
#include "CFadeUI.h"
#include "CTexture.h"
#include "CRenderer.h"
#include "CResourceManager.h"
#include "CDInputManager.h"
#include "CDataManager.h"

CFadeUI::CFadeUI(LPDIRECT3DDEVICE9 pGraphicDev)
    : CRenderObject(pGraphicDev), m_pTextureCom(nullptr)
    , m_fFadeAlpha(0.f), m_fFadeSpeed(0.f), m_eState(FADE_END)
{
}

CFadeUI::CFadeUI(const CFadeUI& rhs)
    : CRenderObject(rhs), m_pTextureCom(nullptr)
    , m_fFadeAlpha(0.f), m_fFadeSpeed(0.f), m_eState(FADE_END)
{
}

CFadeUI::~CFadeUI()
{
}

void CFadeUI::StartFadeIn()
{
    m_eState = FADE_IN;
}

void CFadeUI::StartFadeOut()
{
    m_eState = FADE_OUT;
}

HRESULT CFadeUI::Ready_GameObject()
{
    CRenderObject::Ready_GameObject();

    m_pTransformCom->Set_Scale({ WINCX, -WINCY, 0.f });
    m_pTransformCom->Set_Pos({ WINCX * 0.5f, WINCY * 0.5f, 0.f });

    if (m_pTextureCom = Add_Component<CTexture>(ID_DYNAMIC, L"Fade", TEXTURE))
    {
        if (FAILED(CResourceManager::GetInstance()->Add_Sprite(m_pGraphicDevice, L"Fade",
            L"../Bin/Resource/Sprite/FadeDot/Fade.png", 1)))
            return E_FAIL;

        m_pTextureCom->Set_Speed(5.f);
        m_pTextureCom->Ready_Texture(L"Fade");
        m_pTextureCom->Set_Texture(0);
    }

    m_fFadeSpeed = 1.f;
    m_fFadeAlpha = 1.f;

    Set_RenderPriority(100000);

    return S_OK;
}

_int CFadeUI::Update_GameObject(const _float fTimeDelta)
{
    m_bFadingEnd = false;
    _int iExit = Engine::CRenderObject::Update_GameObject(fTimeDelta);
    Engine::CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    //if (CDInputManager::GetInstance()->Get_DIKeyPressed(DIK_N))
    //{
    //    StartFadeIn();
    //}
    //if (CDInputManager::GetInstance()->Get_DIKeyPressed(DIK_B))
    //{
    //    StartFadeOut();
    //}

    switch (m_eState)
    {
    case FADE_IN:
    {
        m_fFadeAlpha += m_fFadeSpeed * fTimeDelta;
        if (m_fFadeAlpha >= n_fMaxAlpha)
        {
            m_fFadeAlpha = n_fMaxAlpha;
            m_eState = FADE_END;
            m_bFadingEnd = true;
        }
    } break;
    case FADE_OUT:
    {
        m_fFadeAlpha -= m_fFadeSpeed * fTimeDelta;
        if (m_fFadeAlpha <= n_fMinAlpha)
        {
            m_fFadeAlpha = n_fMinAlpha;
            m_eState = FADE_END;
            m_bFadingEnd = true;
        }
    }
    default:
        break;
    }
        

    return iExit;
}

void CFadeUI::LateUpdate_GameObject(const _float fTimeDelta)
{
    Engine::CRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CFadeUI::Render_GameObject()
{
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
    D3DCOLOR tfColor = D3DCOLOR_COLORVALUE(1.f, 1.f, 1.f, m_fFadeAlpha); // 0~1

    m_pGraphicDevice->SetRenderState(D3DRS_TEXTUREFACTOR, tfColor);

    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);

    // 색은 텍스처 그대로 쓰게 (혹시 다른 데서 건드렸으면 확실히 지정)
    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);

    m_pTextureCom->SetUp_Texture();
    m_pBufferCom->Render_Buffer();

    // 원복해주면 더 안전 (다른 UI가 영향 안 받게)
    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
}

CFadeUI* CFadeUI::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFadeUI* pInstance = new CFadeUI(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("Fadeui Create Failed");
        Safe_Release(pInstance);
        pInstance = nullptr;
    }

    return pInstance;
}

void CFadeUI::Free()
{
    Safe_Release(m_pTextureCom);
    CRenderObject::Free();
}
