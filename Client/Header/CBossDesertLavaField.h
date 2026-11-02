#pragma once
#include "CProjectile.h"

class CBossDesertLavaField : public CProjectile
{
private:
    explicit CBossDesertLavaField(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CBossDesertLavaField(const CProjectile& rhs);
    virtual ~CBossDesertLavaField();

public:
    HRESULT     Ready_GameObject() override;
    _int        Update_GameObject(const _float fTimeDelta) override;
    void        LateUpdate_GameObject(const _float fTimeDelta) override;
    void        Render_GameObject() override;
    void        On_Collision(const Collision& tCollision) override;

private:
    virtual void        Ready_Shoot(const _float& fTimeDelta) override;
    virtual void        Shooting(const _float& fTimeDelta) override;
    virtual void        Post_Impact(const _float& fTimeDelta) override;

public:
    static CBossDesertLavaField* Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo);

protected:
    void                Free() override;

};


