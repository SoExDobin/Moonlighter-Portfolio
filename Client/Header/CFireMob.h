#pragma once
#include "CMonster.h"
namespace Engine {
    class CTexture;
}

class CPlayer;
class CHitRectBox;
class CDamageZone;

class CFireMob : public CMonster
{
public:
    enum FIREMOB_STATE { BORN, IDLE, EXPLODE, DEAD, FIREMOB_END };

private:
    explicit CFireMob(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit CFireMob(const CFireMob& rhs);
    virtual ~CFireMob() override;

public:
    HRESULT		Ready_GameObject() override;
    _int		Update_GameObject(const _float fTimeDelta) override;
    void		LateUpdate_GameObject(const _float fTimeDelta) override;
    void		Render_GameObject() override;

    void        hurting(const _float& fTiimeDelta) override;

private:
    HRESULT     Ready_Animation();
    HRESULT     Ready_Combat();

    virtual void    Ready_EntityComponent() override;
    virtual void    Configure_Component() override;

    //State Machine
private:
    void    Update_Born(const _float fTimeDelta);
    void    Update_Idle(const _float fTimeDelta);
    void    Update_Explode(const _float fTimeDelta);

private:
    CTexture*       m_pDynamicTexCom;
    CTransform*     m_pPlayerTransformCom;
    FIREMOB_STATE   m_eState;

    CDamageZone*    m_pDamageZone;
    DAMAGE_INFO     m_tDamage;

    _float          m_fElapsedTime;
    _bool           m_bIsExplode;

private:
    const _uint     n_iExplodeIdx = 11;
    const _float    n_fExplodeTime = 10.f;

public:
    static CFireMob* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    void				Free() override;

};
