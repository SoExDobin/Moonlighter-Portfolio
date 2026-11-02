#pragma once

#include "CRenderObject.h"

namespace Engine {
    class CTexture;
}

class CFadeUI : public CRenderObject
{
private:
    enum FADESTATE { FADE_IN, FADE_OUT, FADE_END };

private:
    explicit    CFadeUI(LPDIRECT3DDEVICE9 pGraphicDev);
    explicit    CFadeUI(const CFadeUI& rhs);
    virtual     ~CFadeUI() override;

public:
    void        StartFadeIn();
    void        StartFadeOut();

    void        Set_FadeSpeed(const _float fSpeed) { m_fFadeSpeed = fSpeed; }
    _float      Get_FadeSpeed() const { return m_fFadeSpeed; }

    _bool       Is_Fading_End() const { return m_bFadingEnd; }

public:
    HRESULT		Ready_GameObject() override;
    _int		Update_GameObject(const _float fTimeDelta) override;
    void		LateUpdate_GameObject(const _float fTimeDelta) override;
    void		Render_GameObject() override;

private:
    CTexture*   m_pTextureCom;
    FADESTATE   m_eState;
    _float      m_fFadeAlpha;
    _float      m_fFadeSpeed;
    _bool       m_bFadingEnd;

    const _float      n_fMinAlpha = 0.f;
    const _float      n_fMaxAlpha = 1.f;

public:
    static CFadeUI* Create(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    virtual void Free()override;
};


