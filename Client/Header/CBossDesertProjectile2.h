#pragma once

#include "CProjectile.h"

class CBossDesertProjectile2 : public CProjectile
{
private:
    explicit CBossDesertProjectile2(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBossDesertProjectile2(const CProjectile& rhs);
    virtual ~CBossDesertProjectile2();

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float fTimeDelta) override;
    void        LateUpdate_GameObject(const _float fTimeDelta) override;
    void        Render_GameObject() override;
    void        On_Collision(const Collision& tCollision) override;

public:
    _float          Get_AngleY() const { return m_fAngleY; }
    const _vec3&    Get_TargetPos() const { return m_vTargetPos; }
    void            Set_TargetPos(const _vec3& vPos) { m_vTargetPos = vPos; }
    const _vec3&    Get_StartPos() const { return m_vStartPos; }
    void            Set_StartPos(const _vec3& vPos) { m_vStartPos = vPos; }
    void            Set_ArcHeight(_float h) { m_fArcHeight = h; }

private:
    virtual void        Ready_Shoot(const _float& fTimeDelta) override;
    virtual void        Shooting(const _float& fTimeDelta) override;
    virtual void        Post_Impact(const _float& fTimeDelta) override;

private:
    PROJECTILE          m_tProj;
    DAMAGE_INFO         m_tDmgInfo;

    _float      m_fAngleY;
    _float      m_fArcHeight;
    _vec3       m_vTargetPos;
    _vec3       m_vStartPos;

public:
    static CBossDesertProjectile2* Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo);

protected:
    void                Free() override;

};


