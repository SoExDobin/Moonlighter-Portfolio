#pragma once

#include "CMonsterHPBar.h"
#include "Client_Struct.h"
#include "CBoss3Body.h"

namespace Engine { class CTexture; }

class CBossDesert;

class CBossDesertHPBar : public CMonsterHPBar
{
private:
    explicit CBossDesertHPBar(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner);
    explicit CBossDesertHPBar(const CBossDesertHPBar& rhs);
    virtual ~CBossDesertHPBar();

public:
    HRESULT		Ready_GameObject()override;
    _int		Update_GameObject(const _float fTimeDelta)override;
    void		LateUpdate_GameObject(const _float fTimeDelta)override;
    void		Render_GameObject()override;

public:
    void    SetUp_HPBar(const _vec3& vScale, const _float& fOffsetY);   // 스케일 값, 위치 값 조정하기
    void    Update_Bar(const _float& fTimeDelta);                       // 기본 Update 문
    void    Apply_Transform();                                          // 위치 및 스케일 값 조정

    void    On_HPChanged() override;                                    // 몬스터 피격당한 경우 호출
    void    Init_GroupHp();                                             // 머리, 몸체 HP 재설정

private:
    void    Configure_Variable();
    void    Tick_State(const _float& fTimeDelta); // 여러 이벤트 관리

private:
    vector<CBoss3Body*>* m_pVecNode;

    UI_OBJECT     m_tBackground;          // BG
    UI_OBJECT     m_tProgress;            // 현재 값 표시

    _vec3       m_vStaticPos;

    _float      m_fMaxHp, m_fMinHp, m_fCurHp;
    _float      m_fBackgroundWidth;

    // 애니메이션 효과에 사용
    _float      m_fCurrentScaleX, m_fTargetScaleX, m_fOriginScaleX;
    _float      m_fCurrentOffsetX, m_fTargetOffsetX, m_fOriginOffsetX;
    _float      m_fAnimElapsedTime, m_fAnimTotalTime;
    _bool       m_bAnimating;

public:
    static CBossDesertHPBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pOwner);

private:
    virtual void Free() override;
};

