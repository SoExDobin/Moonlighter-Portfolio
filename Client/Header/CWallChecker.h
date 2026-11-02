#pragma once

#include "CNonRenderObject.h"

namespace Engine {
    class CRectCollider;
}

class CWallChecker final : public CNonRenderObject
{
private:
    explicit CWallChecker(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CWallChecker() override;

public:
    HRESULT Ready_WallChecker(CGameObject* pOwner);
    _int    Update_GameObject(const _float fTimeDelta) override;
    void    LateUpdate_GameObject(const _float fTimeDelta) override;
    void    Render_GameObject() override {};
    void    On_Collision(const Collision& tCollision) override;

public:
    static const _vec3& WallAABBNormalXZ(const CRectCollider* pSrc, const CRectCollider* pDst);

public:
    void            Set_Scale(_float fScale) { m_pRectCollider->Set_Scale(fScale); }
    _float          Get_Scale() const { return m_pRectCollider->Get_Scale(); }
    void            Set_Offset(const _vec3& vOffset) { m_vOffset = vOffset; }
    const _vec3&    Get_Offset() const { return m_vOffset; }
    void            Set_Dimension(const _vec3& vOffset) { m_pRectCollider->Set_Dimension(vOffset); }
    const _vec3&    Get_Dimension() const { return m_pRectCollider->Get_Dimension(); }

    _bool           Is_Wall() const { return m_bIsWall; }
    const _vec3&    Get_ColNormal() const { return m_vCollisionNormal; }
    COL_STATE       Get_ColState() const { return m_eColState; }

private:
    CGameObject*    m_pOwner;
    CRectCollider*  m_pRectCollider;
    _vec3           m_vOffset;
    _vec3           m_vCollisionNormal;
    COL_STATE       m_eColState;
    _bool           m_bIsWall;

    
public:
    static CWallChecker* Create(LPDIRECT3DDEVICE9 pGraphicDev, CGameObject* pOwner);
private:
    void Free() override;
};

