#pragma once

#include "CBase.h"
#include "Engine_Define.h"
#include "CFadeUI.h"
#include "CScene.h"

class CFadeManager : public CBase
{
    DECLARE_SINGLETON(CFadeManager)
private:
    explicit CFadeManager();
    virtual ~CFadeManager() override;

public:
    HRESULT Ready_FadeManager(LPDIRECT3DDEVICE9 pGrapicDev);
    HRESULT Init_FadeUI(CScene* pScene);

    // Fade_In, Out이 완료된이후 이벤트 등록해두면 CManageMent의 PendingEvent에서 처리
    void    Fade_In(function<void()> funcAfterEvent = NULL);
    void    Fade_Out(function<void()> funcAfterEvent = NULL);
    void    Check_FadeEnd_Event();

private:
    CFadeUI*                    m_pFadeUI;          // 오직 1개로 사용함 DontDestroy 객체
    list<function<void()>>      m_AfterEventlist;   // 페이딩 임시 이벤트 저장

private:
    void        Free() override;
};

