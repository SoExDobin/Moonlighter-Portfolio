#pragma once

#include "CBoss3Body.h"

class CBossDesert;

class CBoss3SquareBody final : public CBoss3Body
{
private:
    enum SQUARE_STATE { IDLE, ATTACK, LIGHT, LIGHT_END, DEAD, STATE_END };
private:
    explicit CBoss3SquareBody(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss3SquareBody() override;

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
    void    Attack(const _float fTimeDelta);

private:
    void    Hurting(const _float fTimeDelta);

private:
    PROJECTILE          m_tProj;
    DAMAGE_INFO         m_tDmgInfo;

    _vec3           m_vDir;
    _vec3           m_vTargetPos;
    SQUARE_STATE    m_eState;
    CGameObject*    m_pPlayer;
    _bool           m_bLightPrepareEnd;

private:
    const _uint   n_iAttackTex = 32;
    const wstring n_wsCombatStat = L"Combat_Stats";
    const wstring n_wsCombat = L"Combat";

public:
    static CBoss3SquareBody* Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pHead);
private:
    void Free() override;
};

