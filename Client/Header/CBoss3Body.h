#pragma once

#include "CMonster.h"

namespace Engine {
    class CTexture;
}

class CBossDesert;
class CWallChecker;
class CDamageZone;

class CBoss3Body : public CMonster
{
protected:
    explicit CBoss3Body(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss3Body();

private:
    virtual void Ready_EntityComponent() override;
    virtual void Configure_Component() override;

public:
    HRESULT   Ready_Body(CBossDesert* pHead);

    virtual void      OnLight_Attack() PURE;
    virtual void      OffLight_Attack() PURE;
    virtual void      OnAttack() PURE;

public:
    void            Set_Head(CBossDesert* pHead) { m_pHead = pHead; }
    CBossDesert*    Get_Head() const { return m_pHead; }
    _bool           Get_IsReady() const { return m_bIsReady; }

protected:
    CBossDesert* m_pHead;

    CWallChecker* m_pWallChecker;
    CDamageZone* m_pDamageZone;

    DAMAGE_INFO      m_tDamage;

    _float m_fSpeed;
    _bool  m_bIsReady;

protected:
    CTexture* m_pDynamicTexCom;
    const wstring n_wsTex = L"Texture_Com";

protected:
    virtual void Free();
};

