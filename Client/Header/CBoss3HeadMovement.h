#pragma once

#include "CComponent.h"

class CBoss3HeadMovement final : public CComponent
{
private:
    explicit CBoss3HeadMovement(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBoss3HeadMovement(const CBoss3HeadMovement& rhs);
    virtual ~CBoss3HeadMovement() override;

public:
    _float Get_Speed()              const { return m_fSpeed; }
    void   Set_Speed(_float fSpeed)       { m_fSpeed = fSpeed; }
    _bool  Get_IsMove()             const { return m_bIsMove; }
    void   Set_IsMove(_bool bIsMove)      { m_bIsMove = bIsMove; }
    _float Get_Default_Speed()      const { return n_fDefaultSpeed; }

    _bool           Get_IsTurn() const { return m_bIsTurn; }
public:
    

public:
    void            Turn(_float fAngle);
    const _vec3&    Get_Dir() const { return m_vDir; }
    void            Set_Dir(const _vec3& vDir) { m_vDir = vDir; }

private:
    void            Update_Angle(const _float fTimeDelta);

public:
    HRESULT Ready_MovementComponent();
    _int    Update_Component(const _float fTimeDelta)       override;
    void    LateUpdate_Component()                          override;

private:
    _vec3           m_vDir;

    _float          m_fSpeed;
    _float          m_fTurnAmount;
    _float          m_fTurnAcc;

    _bool           m_bIsMove;
    _bool           m_bIsTurn;

private:
    const _float n_fDefaultSpeed = 10.f;

public:
    static CBoss3HeadMovement* Create(LPDIRECT3DDEVICE9 pGraphicDev);
    CComponent* Clone() override;
private:
    void Free() override;
};

