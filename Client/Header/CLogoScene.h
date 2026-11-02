#pragma once

#include "CScene.h"

class CLogoScene : public CScene
{
private:
    explicit CLogoScene(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CLogoScene() override;

public:
    HRESULT			Ready_LogoScene(LPDIRECT3DDEVICE9 pGraphicDev);
    _int			Update_Scene(const _float fTimeDelta) override;
    void			LateUpdate_Scene(const _float fTimeDelta) override;

private:
    HRESULT			Ready_Camera_Layer();

    HRESULT			Ready_Light();

public:
    static CLogoScene* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
    void			Free() override;
};

