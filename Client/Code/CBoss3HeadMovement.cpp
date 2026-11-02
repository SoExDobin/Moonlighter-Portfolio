#include "pch.h"
#include "CBoss3HeadMovement.h"
#include "CMapManager.h"
#include "CWallChecker.h"
#include "CLayerHelper.h"
#include "CDataManager.h"
#include <cmath>

CBoss3HeadMovement::CBoss3HeadMovement(LPDIRECT3DDEVICE9 pGraphicDev)
    : CComponent(pGraphicDev), m_fSpeed(0.f), m_bIsMove(false), m_bIsTurn(false)
{
    ZeroMemory(&m_vDir, sizeof(_vec3));
}

CBoss3HeadMovement::CBoss3HeadMovement(const CBoss3HeadMovement& rhs)
    : CComponent(rhs), m_fSpeed(rhs.m_fSpeed), m_bIsMove(rhs.m_bIsMove), m_bIsTurn(m_bIsTurn)
    , m_vDir(rhs.m_vDir)
{

}

CBoss3HeadMovement::~CBoss3HeadMovement()
{
}

void CBoss3HeadMovement::Turn(_float fYAngle)
{
    if (m_pTrans == nullptr) return;

    m_bIsTurn = true;
    m_fTurnAmount = fYAngle;
}

void CBoss3HeadMovement::Update_Angle(const _float fTimeDelta)
{
    if (!m_bIsTurn) return;

    if (m_fTurnAmount < 0.f)
    {
        _vec3 vAngle = Get_Transform()->Get_Angle();
        vAngle.y -= fTimeDelta * 10.f;
        Get_Transform()->Set_Angle(vAngle.x, vAngle.y, vAngle.z);
        m_fTurnAcc += fTimeDelta * 10.f;
    }
    else
    {
        _vec3 vAngle = Get_Transform()->Get_Angle();
        vAngle.y += fTimeDelta * 2.f;
        Get_Transform()->Set_Angle(vAngle.x, vAngle.y, vAngle.z);
        m_fTurnAcc += fTimeDelta * 2.f;
    }

    if (fabsf(m_fTurnAcc) >= fabsf(m_fTurnAmount))
    {
        m_fTurnAcc = 0.f;
        m_fTurnAmount = 0.f;
        m_bIsTurn = false;
    }
}

HRESULT CBoss3HeadMovement::Ready_MovementComponent()
{
    m_fSpeed = 7.5f;

    // TODO 이거 나중에 제거
    //m_bIsMove = true;

    return S_OK;
}

_int CBoss3HeadMovement::Update_Component(const _float fTimeDelta)
{
    if (m_bIsMove == false) return m_bIsDetroy;

    Update_Angle(fTimeDelta);

    const _float fYAngle = m_pTrans->Get_Angle().y;
    m_vDir = { sinf(fYAngle), 0.f, cosf(fYAngle) };

    _vec3 vPos = Get_Transform()->Get_Pos() + (m_vDir * m_fSpeed * fTimeDelta);
    Get_Transform()->Set_Pos(vPos);

    return m_bIsDetroy;
}

void CBoss3HeadMovement::LateUpdate_Component()
{
}

CBoss3HeadMovement* CBoss3HeadMovement::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBoss3HeadMovement* pInstance = new CBoss3HeadMovement(pGraphicDev);
    
    if (FAILED(pInstance->Ready_MovementComponent()))
    {
        MSG_BOX("CBoss3HeadMovement Component Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

CComponent* CBoss3HeadMovement::Clone()
{
    return new CBoss3HeadMovement(*this);
}

void CBoss3HeadMovement::Free()
{
}
