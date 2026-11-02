#pragma once

#include "CNonRenderObject.h"

namespace Engine {
    class CRectCollider;
}

class CTrigger : public CNonRenderObject
{
protected:
    explicit CTrigger(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CTrigger(const CTrigger& rhs);
    virtual ~CTrigger() override;

public:
    const _vec3&    Get_Size() const { return m_pColCom->Get_Dimension(); };
    void            Set_Size(_vec3 vSize) { m_pColCom->Set_Dimension(vSize); };

    const _vec3&    Get_Offset() const { return m_pColCom->Get_Offset(); };
    void            Set_Offset(_vec3 vSize) { m_pColCom->Set_Offset(vSize); };

    const _vec3&    Get_Transform() const { return m_pTransformCom->Get_Pos(); };
    void            Set_Pos(_vec3 vPos) { m_pTransformCom->Set_Pos(vPos); };

    void           Set_Target(OBJECT_ID eID) {  m_eTargetID = static_cast<OBJECT_ID>( static_cast<int>(m_eTargetID) | static_cast<int>(eID) );  }

public:
    void    Add_Event(function<void()> funcEvent);
    void    Remove_Event(function<void()> funcEvent);

public:
    virtual HRESULT Ready_Trigger();
    virtual _int    Update_GameObject(const _float fTimeDelta) override;
    virtual void    LateUpdate_GameObject(const _float fTimeDelta) override;
    virtual void    On_Collision(const Collision& tCollision) override;

protected:
    CRectCollider*                  m_pColCom;
    vector<function<void()>>       m_vecEvent;

    OBJECT_ID m_eTargetID = OBJECT_ID::NONE;

public:
    static CTrigger* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
    virtual void        Free() override;
};

