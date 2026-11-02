#pragma once

#include "CMonsterHPBar.h"
#include "Client_Define.h"

namespace Engine { class CTexture; }

class CBossDesertBodyHPBar : public CMonsterHPBar
{
private:
    explicit CBossDesertBodyHPBar(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner);
    explicit CBossDesertBodyHPBar(const CBossDesertBodyHPBar& rhs);
    virtual ~CBossDesertBodyHPBar();

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
    void            Set_HeadHPBar(CMonsterHPBar* pHpBar) { m_pHeadHpBar = pHpBar; }
    CMonsterHPBar*  Get_HeadHPBar() const                { return m_pHeadHpBar; }

private:
    void    Configure_Variable();
    void    Tick_State(const _float& fTimeDelta); // 여러 이벤트 관리

private:
    UI_OBJECT     m_tBackground;          // BG
    UI_OBJECT     m_tProgress;              // 현재 값 표시

private:
    CMonsterHPBar* m_pHeadHpBar;
    CTransform* m_pOwnerTransform;

    _bool       m_bVisible;     // 가시 여부
    _float       m_fVisibleElapsedTime, m_fVisibleTotalTime;

    _float      m_fMaxHp, m_fMinHp, m_fCurHp;
    _float      m_fBackgroundWidth;
    _float      m_fOffsetY;     // 몬스터 머리 위 높이 

    // 애니메이션 효과에 사용
    _float      m_fCurrentScaleX, m_fTargetScaleX, m_fOriginScaleX;
    _float      m_fCurrentOffsetX, m_fTargetOffsetX, m_fOriginOffsetX;
    _float      m_fAnimElapsedTime, m_fAnimTotalTime;
    _bool       m_bAnimating;

    // Fade 효과에 사용 
    _float      m_fAlpha;
    _bool      m_bAppear, m_bDisappear;
    _float     m_fFadeElapsedTime, m_fFadeTotalTime;


public:
    static CBossDesertBodyHPBar* Create(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner);

private:
    virtual void Free() override;


};

