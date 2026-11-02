#include "CNonRenderObject.h"


const wstring& CNonRenderObject::n_wsTransformKey = L"Transform_Com";

const wstring& CNonRenderObject::Get_TransformKey() { return n_wsTransformKey; }

CNonRenderObject::CNonRenderObject(LPDIRECT3DDEVICE9 pGraphicDev)
    : CGameObject(pGraphicDev), m_pTransformCom(nullptr)
{
}

CNonRenderObject::CNonRenderObject(const CNonRenderObject& rhs)
    : CGameObject(rhs), m_pTransformCom(nullptr)
{
}

CNonRenderObject::~CNonRenderObject()
{
}

HRESULT CNonRenderObject::Ready_GameObject()
{
    if (FAILED(Engine::CGameObject::Ready_GameObject()))
        return E_FAIL;

    if (m_pTransformCom = Add_Component<CTransform>(ID_DYNAMIC, n_wsTransformKey, TRANSFORM))
        ;
    else
        return E_FAIL;

    return S_OK;
}

_int CNonRenderObject::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CNonRenderObject::LateUpdate_GameObject(const _float fTimeDelta)
{
    Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CNonRenderObject::Render_GameObject()
{
}

void CNonRenderObject::Free()
{
    Engine::CGameObject::Free();
}
