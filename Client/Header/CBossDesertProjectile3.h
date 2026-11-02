#pragma once

#include "CProjectile.h"

class CBossDesertProjectile3 : public CProjectile
{
private:
    explicit CBossDesertProjectile3(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBossDesertProjectile3(const CProjectile& rhs);
    virtual ~CBossDesertProjectile3();

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float fTimeDelta) override;
    void        LateUpdate_GameObject(const _float fTimeDelta) override;
    void        Render_GameObject() override;
    void        On_Collision(const Collision& tCollision) override;

public:
    const _float    Get_Speed() const { return m_fSpeed; }
    void            Set_Speed(const _float fSpeed) { m_fSpeed = fSpeed; }

private:
    virtual void        Ready_Shoot(const _float& fTimeDelta) override;
    virtual void        Shooting(const _float& fTimeDelta) override;
    virtual void        Post_Impact(const _float& fTimeDelta) override;

private:
    _float m_fSpeed;
    _bool  m_bOutofField;

public:
    static CBossDesertProjectile3* Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo);

protected:
    void                Free() override;

};


