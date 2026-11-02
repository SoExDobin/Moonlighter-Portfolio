#pragma once

#include "CMonster.h"
#include "CBoss3Body.h"

namespace Engine {
    class CTexture;
}

class CWallChecker;
class CBoss3HeadMovement;

class CBossDesert final : public CMonster
{
private:
    enum HEAD_DIR { LEFT, RIGHT, UP, DOWN, DIR_END };
    enum HEAD_STATE { IDLE, TURN, LIGHT, DEAD, SLEEP, STATE_END };
private:
    explicit CBossDesert(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBossDesert() override;

public:
    HRESULT Ready_GameObject()                             override;
    _int    Update_GameObject(const _float fTimeDelta)     override;
    void    LateUpdate_GameObject(const _float fTimeDelta) override;
    void    Render_GameObject()                            override;
    void    On_Collision(const Collision& tCollision)      override;

public:
    HRESULT Init_GroupHP();
    CBoss3HeadMovement* Get_Movment() { return m_pMovement; }
    vector<CBoss3Body*>& Get_BodyGroup() { return m_vecNode; }

private:
    void    Ready_EntityComponent() override;
    void    Configure_Component() override;

    void    Change_Head_Texture();
    void    CheckWall(const _float fTimeDelta);

    void    Attack(const _float fTimeDelta);
    void    Check_LightAttack(const _float fTimeDelta);
    void    Dead(const _float fTimeDelta);

private:
    void  Init_TrainTrail();
    void  Record_HeadTrail();
    void  Apply_TrainBody(const _float fTimeDelta);
    _int  PointsPerSegment() const;

private:
    void    Hurting(const _float fTimeDelta);
    HRESULT Ready_BodyBlock();
    HRESULT Ready_WallChecker();

private:
    CTexture*               m_pDynamicTexCom;
    CBoss3HeadMovement*     m_pMovement;

    vector<CBoss3Body*>     m_vecNode;
    deque<_vec3>            m_deqHeadTrail;
    CGameObject*            m_pIntroCover;
    CWallChecker*           m_pUpChecker;
    CWallChecker*           m_pDownChecker;
    CWallChecker*           m_pLeftChecker;
    CWallChecker*           m_pRightChecker;

    HEAD_STATE m_eState;

    _float m_fBodySpacing;   // 몸통 간격(월드 단위)
    _float m_fRecordStep;    // 머리 경로 기록 간격
    _float m_fFollowLerp;    // 몸통이 타겟으로 붙는 정도(0~1)

    _float m_fAttackDelta;
    _float m_fLightDelta;
    _float m_fDeadElapsedTime;

#ifdef _DEBUG
    _uint  m_iDemoPatternIdx = 0;
    _bool  m_bDemoEnd = false;
#endif

private:
    const wstring n_wsTex = L"Texture_Com";
    const wstring n_wsMovement = L"HeadMovment_Com";
    const wstring n_wsCombatStat = L"Combat_Stats";
    const wstring n_wsCombat = L"Combat";

    const _float n_fAttackCool = 8.f;
    const _float n_fLightCool = 10.f;
    const _int n_fAttackCnt = 1;

public:
    static CBossDesert* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
    void Free() override;

#ifdef _DEBUG
    void    Add_EditorFiled();
    void    Display_Debug_Param();
    _float  debug_fHP = 0.f;
    _float  debug_fAttack = 0.f;
    _float  debug_fDefense = 0.f;
    _float  debug_fSpeed = 0.f;
#endif
};

