#pragma once

#include "CBoss3Body.h"

class CBossDesert;

class CBoss3DiamondBody final : public CBoss3Body
{
private:
    enum DIAMOND_STATE { IDLE, ATTACK_PRE, ATTACK, ATTACK_END, LIGHT, LIGHT_END, DEAD, STATE_END };
private:
    explicit CBoss3DiamondBody(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss3DiamondBody() override;

public:
    HRESULT Ready_GameObject()                             override;
    _int    Update_GameObject(const _float fTimeDelta)     override;
    void    LateUpdate_GameObject(const _float fTimeDelta) override;
    void    Render_GameObject()                            override;

    void    OnLight_Attack() override;
    void    OffLight_Attack() override;
    void    OnAttack() override;

private:
    void    Ready_EntityComponent() override;
    void    Configure_Component() override;

    void    Light_Attack(const _float fTimeDelta);

    void    Attack_Prepare(const _float fTimeDelta);
    void    Attack(const _float fTimeDelta);
    void    Attack_End(const _float fTimeDelta);

private:
    void    Hurting(const _float fTimeDelta);

private:
    _vec3           m_vDir;
    DIAMOND_STATE   m_eState;

    PROJECTILE          m_tProj;
    DAMAGE_INFO         m_tDmgInfo;

    _vec3               m_vShootAngle;
    _float              m_fFireRate;
    _float              m_fFireRateAcc;
    _int                m_iShootCnt;
    _bool               m_bLightPrepareEnd;

private:
    const wstring n_wsCombatStat = L"Combat_Stats";
    const wstring n_wsCombat = L"Combat";

public:
    static CBoss3DiamondBody* Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pHead);
private:
    void Free() override;
};

