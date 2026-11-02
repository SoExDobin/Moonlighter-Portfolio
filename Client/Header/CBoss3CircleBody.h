#pragma once

#include "CBoss3Body.h"

class CBossDesert;

class CBoss3CircleBody final : public CBoss3Body
{
private:
    enum CIRCLE_STATE { IDLE, ATTACK_PRE, ATTACK, ATTACK_END, LIGHT, LIGHT_END, DEAD, STATE_END };
private:
    explicit CBoss3CircleBody(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss3CircleBody() override;

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
    CIRCLE_STATE        m_eState;
    _vec3               m_vDir;

    PROJECTILE          m_tProj;
    DAMAGE_INFO         m_tDmgInfo;

    vector<_vec3>       m_vecShootDir;
    _float              m_fFireRate;
    _float              m_fFireRateAcc;
    _float              m_fShootTime;
    _bool               m_bLightPrepareEnd;

private:
    const wstring n_wsCombatStat = L"Combat_Stats";
    const wstring n_wsCombat = L"Combat";

    _float              n_fShootTime = 8.f;

public:
    static CBoss3CircleBody* Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pHead);
private:
    void Free() override;
};

