#pragma once

#include "CRenderObject.h"

namespace Engine {
    class CTexture;
}

class CBoss3IntroCover : public CRenderObject
{
private:
    explicit CBoss3IntroCover(LPDIRECT3DDEVICE9 pGraphicDev);
    virtual ~CBoss3IntroCover() override;

public:
    HRESULT Ready_GameObject()                             override;
    _int    Update_GameObject(const _float fTimeDelta)     override;
    void    LateUpdate_GameObject(const _float fTimeDelta) override;
    void    Render_GameObject()                            override;
    
private:
    CTexture* m_pDynamicTexCom;

public:
    static CBoss3IntroCover* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
    void Free() override;
};

