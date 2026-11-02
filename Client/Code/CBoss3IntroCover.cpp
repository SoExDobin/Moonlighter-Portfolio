#include "pch.h"
#include "CBoss3IntroCover.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "CSoundManager.h"

CBoss3IntroCover::CBoss3IntroCover(LPDIRECT3DDEVICE9 pGraphicDev)
    : CRenderObject(pGraphicDev), m_pDynamicTexCom(nullptr)
{
}

CBoss3IntroCover::~CBoss3IntroCover()
{
}

HRESULT CBoss3IntroCover::Ready_GameObject()
{ 
    CRenderObject::Ready_GameObject();

    if (m_pDynamicTexCom = Add_Component<CTexture>(ID_DYNAMIC, L"Texture_Com", TEXTURE))
    {
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Intro");
        m_pDynamicTexCom->Set_Texture(0);
        m_pDynamicTexCom->Set_Loop(false);
    }
    else return E_FAIL;

    m_pTransformCom->Set_Scale({ 29.f, 29.f, 1.f });
    m_pTransformCom->Set_Angle({ D3DXToRadian(75.f), 0.f, 0.f});
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_born.wav", CHANNEL_6, 0.2f);
    return S_OK;
}

_int CBoss3IntroCover::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CRenderObject::Update_GameObject(fTimeDelta);

    Engine::CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    if (m_pDynamicTexCom->Get_OneLoop())
    {
        Destroy(this);
    }

    return iExit;
}

void CBoss3IntroCover::LateUpdate_GameObject(const _float fTimeDelta)
{
    CRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CBoss3IntroCover::Render_GameObject()
{
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

    m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

    m_pDynamicTexCom->SetUp_Texture();
    m_pBufferCom->Render_Buffer();

    m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

CBoss3IntroCover* CBoss3IntroCover::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBoss3IntroCover* pInstance = new CBoss3IntroCover(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CBoss3 Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CBoss3IntroCover::Free()
{
    CRenderObject::Free();
}
