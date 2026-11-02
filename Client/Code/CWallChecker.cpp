#include "pch.h"
#include "CWallChecker.h"
#include "CDataManager.h"
#include "CRenderObject.h"

CWallChecker::CWallChecker(LPDIRECT3DDEVICE9 pGraphicDev)
    : CNonRenderObject(pGraphicDev), m_bIsWall(false)
    , m_pRectCollider(nullptr), m_pOwner(nullptr)
{
    ZeroMemory(&m_vOffset, sizeof(_vec3));
    ZeroMemory(&m_vCollisionNormal, sizeof(_vec3));
}

CWallChecker::~CWallChecker()
{
}

HRESULT CWallChecker::Ready_WallChecker(CGameObject* pOwner)
{
    CNonRenderObject::Ready_GameObject();
    Get_LayerMask().Ease_Mask();
    Get_LayerMask().Add_Mask(CDataManager::GetInstance()->Get_LayerTag(ENVIRONMENT_LAYER));

    if (m_pRectCollider = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER))
    {

    }
    else return E_FAIL;

    m_pOwner = pOwner;

    return S_OK;
}

_int CWallChecker::Update_GameObject(const _float fTimeDelta)
{
    if (m_pOwner->Get_Type() == RENDER_OBJECT)
    {
        _vec3 vPos = static_cast<CRenderObject*>(m_pOwner)->Get_Trans()->Get_Pos() + m_vOffset;
        m_pTransformCom->Set_Pos(vPos);
    }

    _int iExit = CNonRenderObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CWallChecker::LateUpdate_GameObject(const _float fTimeDelta)
{
    CNonRenderObject::LateUpdate_GameObject(fTimeDelta);
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());
}

void CWallChecker::On_Collision(const Collision& tCollision)
{
    if (tCollision.pColTarget->Get_ObjectID() != ENVIRONMENT)
    {
        m_bIsWall = false;
        return;
    }

    if (tCollision.pColSource->Get_ColType() != RECT_COL) return;

    if (tCollision.eColState == EXIT_COL)
        m_bIsWall = false;
    else
    {
        m_bIsWall = true;
        m_eColState = tCollision.eColState;
        m_vCollisionNormal =
            WallAABBNormalXZ(m_pRectCollider, static_cast<CRectCollider*>(tCollision.pColSource));
    }
        
}

const _vec3& CWallChecker::WallAABBNormalXZ(const CRectCollider* pSrc, const CRectCollider* pDst)
{
    _vec3 vSrc = pSrc->Get_Transform()->Get_Pos() + pSrc->Get_Offset();
    _vec3 vDst = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset();

    _vec3 vDist = vSrc - vDst;
    vDist.y = 0.f;

    const _vec3 vSrcDimension = pSrc->Get_Dimension();
    const _vec3 vDstDimension = pDst->Get_Dimension();

    const _float fSrcX = (vSrcDimension.x * 0.5f) * pSrc->Get_Scale();
    const _float fSrcZ = (vSrcDimension.z * 0.5f) * pSrc->Get_Scale();
    const _float fDstX = (vDstDimension.x * 0.5f) * pDst->Get_Scale();
    const _float fDstZ = (vDstDimension.z * 0.5f) * pDst->Get_Scale();

    const _float fOverlapX = (fSrcX + fDstX) - fabsf(vDist.x); 
    const _float fOverlapZ = (fSrcZ + fDstZ) - fabsf(vDist.z); 

    // 겹침이 더 "작은" 축이 충돌 면(노말 축)
    _vec3 vNormal { 0.f,0.f,0.f };
    if (fOverlapX < fOverlapZ)
        vNormal.x = (vDist.x >= 0.f) ? 1.f : -1.f;
    else
        vNormal.z = (vDist.z >= 0.f) ? 1.f : -1.f;

    return vNormal;
}

CWallChecker* CWallChecker::Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner)
{
    CWallChecker* pInstance = new CWallChecker(pGraphicDev);

    if (FAILED(pInstance->Ready_WallChecker(pOwner)))
    {
        MSG_BOX("CWallChecker Create Fail");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CWallChecker::Free()
{
    CNonRenderObject::Free();
}
