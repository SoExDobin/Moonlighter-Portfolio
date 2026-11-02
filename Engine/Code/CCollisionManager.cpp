#include "CCollisionManager.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "CCollider.h"
#include "CSphereCollider.h"
#include "CRectCollider.h"
#include "CDInputManager.h"

IMPLEMENT_SINGLETON(CCollisionManager)

CCollisionManager::CCollisionManager() : m_bDebugMode(false)
{
}

CCollisionManager::~CCollisionManager()
{
    Free();
}

void CCollisionManager::Add_Collider(CCollider* pColComonent)
{
    m_vecCollider.push_back(pColComonent);
}

void CCollisionManager::Remove_Collider(CCollider* pColComonent)
{
    if (!pColComonent) return;

    Remove_CheckMember(pColComonent);

    auto it = std::remove(m_vecCollider.begin(), m_vecCollider.end(), pColComonent);
    m_vecCollider.erase(it, m_vecCollider.end());
}

void CCollisionManager::Remove_CheckMember(CCollider* pColComonent)
{
    for (auto* p : m_vecCollider)
    {
        if (p && p != pColComonent)
            p->Release_OverlapMember(pColComonent);
    }
}

void CCollisionManager::Update_Collision()
{
    size_t iColSize = m_vecCollider.size();

    for (_uint i = 0; i < iColSize; ++i)
    {
        CCollider* pSrc = m_vecCollider[i];
        if (pSrc == nullptr || pSrc->Get_Owner() == nullptr) continue;  // null, gameobject 체크
        if (pSrc->Is_Destroy()) continue;                               // 삭제 예정 이면 넘김
        if (pSrc->Is_Enable() == false) continue;                       // 활성 상태 확인

        for (_uint j = i + 1; j < iColSize; ++j)
        {
            CCollider* pDst = m_vecCollider[j];
            if (pDst == nullptr || pDst->Get_Owner() == nullptr)        // null, gameobject 체크
                continue;
            if (pDst->Is_Destroy()) continue;                           // 삭제 예정 이면 넘김
            if (pDst->Is_Enable() == false) continue;                   // 활성 상태 확인

            CGameObject* pSrcOwner = pSrc->Get_Owner();
            CGameObject* pDstOwner = pDst->Get_Owner();

            const LayerMask& tSrcMask = pSrcOwner->Get_Object_LayerMask();
            const LayerMask& tDstMask = pDstOwner->Get_Object_LayerMask();

            _bool bSrcCol = false;
            _bool bDstCol = false;

            // Dst가 충돌 대상이면 
            if ((tSrcMask.iMask & tDstMask.eLayerID) != 0)
            {

                bSrcCol = pSrc->Check_Collision(pDst);

                if (bSrcCol) // if Src 충돌 TRUE
                {
                    pSrc->Set_IsCol(true);

                    if (pSrc->Is_Overlapped(pDst)) // 이전 충돌 맴버
                    {
                        pSrc->Set_ColState(STAY_COL);
                        pSrc->Set_Collision({ pDstOwner, pDst, STAY_COL });
                    }
                    else // 첫 충돌 맴버
                    {
                        pSrc->Add_OverlapMember(pDst);
                        pSrc->Set_ColState(ENTER_COL);
                        pSrc->Set_Collision({ pDstOwner, pDst, ENTER_COL });
                    }
                }
                else // if Src 충돌 FALSE
                {
                    pSrc->Set_IsCol(false);

                    if (pSrc->Is_Overlapped(pDst))
                    {
                        pSrc->Set_ColState(EXIT_COL);
                        pSrc->Set_Collision({ pDstOwner, pDst , EXIT_COL });
                        pSrc->Release_OverlapMember(pDst);
                        bSrcCol = true;
                    }
                }
            }

            // Src가 충돌 대상이면
            if ((tDstMask.iMask & tSrcMask.eLayerID) != 0)
            {
                bDstCol = pDst->Check_Collision(pSrc);

                if (bDstCol) // Dst 충돌 TRUE
                {
                    pDst->Set_IsCol(true);

                    if (pDst->Is_Overlapped(pSrc))
                    {
                        pDst->Set_ColState(STAY_COL);
                        pDst->Set_Collision({ pSrcOwner, pSrc, STAY_COL });
                    }
                    else
                    {
                        pDst->Add_OverlapMember(pSrc);
                        pDst->Set_ColState(ENTER_COL);
                        pDst->Set_Collision({ pSrcOwner, pSrc, ENTER_COL });
                    }
                }
                else // Dst 충돌 FALSE
                {
                    pDst->Set_IsCol(false);

                    if (pDst->Is_Overlapped(pSrc))
                    {
                        pDst->Set_ColState(EXIT_COL);
                        pDst->Set_Collision({ pSrcOwner, pSrc, EXIT_COL });
                        pDst->Release_OverlapMember(pSrc);
                        bDstCol = true;
                    }
                }
            }

            if (bSrcCol) pSrcOwner->On_Collision(pSrc->Get_Collision());
            else
            {
                pSrc->Set_ColState(COL_STATE_END);
                pSrc->Set_Collision({ nullptr, nullptr, COL_STATE_END });
            }

            if (bDstCol) pDstOwner->On_Collision(pDst->Get_Collision());
            else
            {
                pDst->Set_ColState(COL_STATE_END);
                pDst->Set_Collision({ nullptr, nullptr, COL_STATE_END });
            }
        }
    }
}

void CCollisionManager::Clear_CollisionGroup()
{
    m_vecCollider.clear();
}

#ifdef _DEBUG
void CCollisionManager::Render_Collision()
{
    if (CDInputManager::GetInstance()->Get_DIKeyPressed(DIK_F9))
    {
        m_bDebugMode = !m_bDebugMode;
    }

    if (m_bDebugMode == false) return;
    for (auto& pCol : m_vecCollider)
    {
        if (pCol) pCol->Render_DebugCollider();
    }
}
#endif

_bool CCollisionManager::RectCollision(CRectCollider* pSrc, CRectCollider* pDst)
{
    _vec3 vSrcMin = pSrc->Get_Transform()->Get_Pos() + pSrc->Get_Offset() + (pSrc->Get_Dimension() * -0.5f * pSrc->Get_Scale());
    _vec3 vSrcMax = pSrc->Get_Transform()->Get_Pos() + pSrc->Get_Offset() + (pSrc->Get_Dimension() * 0.5f * pSrc->Get_Scale());

    _vec3 vDstMin = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset() + (pDst->Get_Dimension() * -0.5f * pDst->Get_Scale());
    _vec3 vDstMax = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset() + (pDst->Get_Dimension() * 0.5f * pDst->Get_Scale());

    if (vSrcMax.x < vDstMin.x) return false; // Src가 Dst 왼쪽
    if (vSrcMin.x > vDstMax.x) return false; // Src가 Dst 오른쪽

    if (vSrcMax.y < vDstMin.y) return false; // Src가 Dst 아래
    if (vSrcMin.y > vDstMax.y) return false; // Src가 Dst 위

    if (vSrcMax.z < vDstMin.z) return false; // Src가 Dst 뒤
    if (vSrcMin.z > vDstMax.z) return false; // Src가 Dst 앞

    return true;
}

_bool CCollisionManager::SphereCollision(CSphereCollider* pSrc, CSphereCollider* pDst)
{
    _vec3 vSrcPos = pSrc->Get_Transform()->Get_Pos() + pSrc->Get_Offset();
    _vec3 vDstPos = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset();
    _vec3 vDistance = vDstPos - vSrcPos;
    _float fDistance = D3DXVec3LengthSq(&vDistance);
    
    _float m_fRadiusSum = (pSrc->Get_Radius() * pSrc->Get_Scale() )
                        + (pDst->Get_Radius() * pDst->Get_Scale() );

    return (m_fRadiusSum * m_fRadiusSum) >= fDistance;
}

_bool CCollisionManager::SphereRectCollision(CSphereCollider* pSrc, CRectCollider* pDst)
{
    _vec3 vSpherePos = pSrc->Get_Transform()->Get_Pos() + pSrc->Get_Offset();

    _vec3 vRectMin = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset() + (pDst->Get_Dimension() * -0.5f * pDst->Get_Scale());
    _vec3 vRectMax = pDst->Get_Transform()->Get_Pos() + pDst->Get_Offset() + (pDst->Get_Dimension() * 0.5f * pDst->Get_Scale());

    _vec3 vClosestPoint;
    
    if (vSpherePos.x < vRectMin.x)          vClosestPoint.x = vRectMin.x;
    else if (vSpherePos.x > vRectMax.x)     vClosestPoint.x = vRectMax.x;
    else                                    vClosestPoint.x = vSpherePos.x;

    if (vSpherePos.y < vRectMin.y)          vClosestPoint.y = vRectMin.y;
    else if (vSpherePos.y > vRectMax.y)     vClosestPoint.y = vRectMax.y;
    else                                    vClosestPoint.y = vSpherePos.y;

    if (vSpherePos.z < vRectMin.z)          vClosestPoint.z = vRectMin.z;
    else if (vSpherePos.z > vRectMax.z)     vClosestPoint.z = vRectMax.z;
    else                                    vClosestPoint.z = vSpherePos.z;

    vClosestPoint = vClosestPoint - vSpherePos;
    _float fDistance = D3DXVec3LengthSq(&vClosestPoint);
    _float fRadius = pSrc->Get_Radius() * pSrc->Get_Scale();
    // 구의 반지름과 Rect구중심으로 부터 박스 최단거리의 비교
    return fDistance <= (fRadius * fRadius);
}

void CCollisionManager::Free()
{
    Clear_CollisionGroup();
}
