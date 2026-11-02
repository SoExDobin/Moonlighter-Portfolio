#include "pch.h"
#include "CBossDesertBodyHPBar.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "EasingFunctions.h"
#include "CMath.h"

CBossDesertBodyHPBar::CBossDesertBodyHPBar(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner)
    : CMonsterHPBar(pGraphicDev, pOwner)
    , m_fMaxHp(0), m_fMinHp(0), m_fCurHp(0)
    , m_fBackgroundWidth(0), m_fOffsetY(0), m_bVisible(false), m_fVisibleElapsedTime(false), m_fVisibleTotalTime(false), m_fCurrentScaleX(0)
    , m_fTargetScaleX(0), m_fOriginScaleX(0), m_fCurrentOffsetX(0), m_fTargetOffsetX(0)
    , m_fOriginOffsetX(0), m_fAnimElapsedTime(0), m_fAnimTotalTime(0), m_bAnimating(false), m_fAlpha(0),
    m_bAppear(false), m_bDisappear(false), m_fFadeElapsedTime(0), m_fFadeTotalTime(0)
{
    m_pOwnerTransform = static_cast<CTransform*>(m_pOwner->Get_Component(ID_DYNAMIC, TRANSFORM));
    ZeroMemory(&m_tBackground, sizeof(m_tBackground));
    ZeroMemory(&m_tProgress, sizeof(m_tProgress));
}

CBossDesertBodyHPBar::CBossDesertBodyHPBar(const CBossDesertBodyHPBar& rhs)
    : CMonsterHPBar(rhs)
    , m_pOwnerTransform(rhs.m_pOwnerTransform)
    , m_fMaxHp(0), m_fMinHp(0), m_fCurHp(0)
    , m_fBackgroundWidth(0), m_fOffsetY(0), m_fCurrentScaleX(0), m_fTargetScaleX(0), m_fOriginScaleX(0)
    , m_fCurrentOffsetX(0), m_fTargetOffsetX(0), m_fOriginOffsetX(0)
    , m_fAnimElapsedTime(0), m_fAnimTotalTime(0), m_bAnimating(false),
    m_bVisible(false)
{
    memcpy(&m_tBackground, &rhs.m_tBackground, sizeof(m_tBackground));
    memcpy(&m_tProgress, &rhs.m_tProgress, sizeof(m_tProgress));
}

CBossDesertBodyHPBar::~CBossDesertBodyHPBar()
{
}

HRESULT CBossDesertBodyHPBar::Ready_GameObject()
{
    // 기본 컴포넌트 추가
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    // BG
    m_tBackground.pTransform = Add_Component<CTransform>(ID_DYNAMIC, L"Background_Transform", TRANSFORM);
    m_tBackground.pBuffer = Add_Component<CRectTex>(ID_STATIC, L"Background_Buffer", RECTTEX);
    m_tBackground.pTexture = Add_Component<CTexture>(ID_DYNAMIC, L"Background_Texture", TEXTURE);

    // Progress
    m_tProgress.pTransform = Add_Component<CTransform>(ID_DYNAMIC, L"Fill_Transform", TRANSFORM);
    m_tProgress.pBuffer = Add_Component<CRectTex>(ID_STATIC, L"Fill_Buffer", RECTTEX);
    m_tProgress.pTexture = Add_Component<CTexture>(ID_DYNAMIC, L"Fill_Texture", TEXTURE);

    Configure_Variable();

    return S_OK;
}

_int CBossDesertBodyHPBar::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);
    Engine::CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    Update_Bar(fTimeDelta);

    Tick_State(fTimeDelta);

    return iExit;
}

void CBossDesertBodyHPBar::LateUpdate_GameObject(const _float fTimeDelta)
{
    Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBossDesertBodyHPBar::Render_GameObject()
{
    if (m_bAppear || m_bDisappear)
    {
        m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
        m_pGraphicDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
        m_pGraphicDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
        m_pGraphicDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

        // 버텍스 알파 사용
        m_pGraphicDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);

        D3DMATERIAL9 mat;
        ZeroMemory(&mat, sizeof(mat));
        mat.Diffuse = D3DXCOLOR(1, 1, 1, m_fAlpha);
        m_pGraphicDevice->SetMaterial(&mat);

        // TextureStage는 텍스처 그대로 사용
        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        m_pGraphicDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    }

    if (m_bVisible || m_bDisappear)
    {
        // 1) 배경
        m_tBackground.pTexture->Set_Texture(0);
        m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_tBackground.pTransform->Get_World());
        m_tBackground.pBuffer->Render_Buffer();

        // 2) HP 채우기
        m_tProgress.pTexture->Set_Texture(0);
        m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_tProgress.pTransform->Get_World());
        m_tProgress.pBuffer->Render_Buffer();
    }

    if (m_bAppear || m_bDisappear)
    {
        m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        m_pGraphicDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    }
}

void CBossDesertBodyHPBar::SetUp_HPBar(const _vec3& vScale, const _float& fOffsetY)
{
    m_tBackground.pTransform->Set_Scale(vScale);
    m_tProgress.pTransform->Set_Scale(vScale);
    m_fBackgroundWidth = vScale.x;

    m_fOffsetY = fOffsetY;
}

void CBossDesertBodyHPBar::On_HPChanged()
{
    float curRatio = m_pOwner->Get_HealthStat()->Get_CurValue() / m_fMaxHp;

    // 타겟 상태
    m_fTargetScaleX = m_tBackground.pTransform->Get_Scale().x * curRatio;
    m_fTargetOffsetX = m_fBackgroundWidth * 0.5f * (1.f - curRatio);

    m_fOriginScaleX = m_fCurrentScaleX;
    m_fOriginOffsetX = m_fCurrentOffsetX;

    // 애니메이션 시작
    m_fAnimElapsedTime = 0.f;
    m_bAnimating = true;

    // 나타내기
    m_bAppear = true;
    m_bDisappear = false;
    m_bVisible = true;

    m_fFadeElapsedTime = m_fVisibleElapsedTime = 0.f;

    if (m_pHeadHpBar != nullptr)
        m_pHeadHpBar->On_HPChanged();
}

void CBossDesertBodyHPBar::Update_Bar(const _float& fTimeDelta)
{
    if (m_bAnimating)
    {
        m_fAnimElapsedTime += fTimeDelta;

        float t = m_fAnimElapsedTime / m_fAnimTotalTime;
        if (t > 1.f) t = 1.f;

        float e = Ease::OutCubic(t);

        m_fCurrentScaleX = Lerp(m_fOriginScaleX, m_fTargetScaleX, e);
        m_fCurrentOffsetX = Lerp(m_fOriginOffsetX, m_fTargetOffsetX, e);

        if (t >= 1.f)
        {
            m_fAnimElapsedTime = 0.f;
            m_bAnimating = false;
            m_fCurrentScaleX = m_fTargetScaleX;
            m_fCurrentOffsetX = m_fTargetOffsetX;
        }
    }

    // 실제 적용
    Apply_Transform();
}

void CBossDesertBodyHPBar::Apply_Transform()
{
    _vec3 vPos, vProgressPos, vScale;

    // Background
    vPos = m_pOwnerTransform->Get_Pos();
    vPos.y += m_fOffsetY;
    m_tBackground.pTransform->Set_Pos(vPos);

    // Fill
    vProgressPos = vPos;
    vProgressPos.x -= m_fCurrentOffsetX;
    m_tProgress.pTransform->Set_Pos(vProgressPos);

    vScale = m_tProgress.pTransform->Get_Scale();
    vScale.x = m_fCurrentScaleX;
    m_tProgress.pTransform->Set_Scale(vScale);
}

void CBossDesertBodyHPBar::Configure_Variable()
{
    // 스케일 조정 
    m_tBackground.pTransform->Set_Scale({ 4.f, 3.f, 1.f });
    m_tProgress.pTransform->Set_Scale(m_tBackground.pTransform->Get_Scale());   // Full 

    m_fBackgroundWidth = m_tBackground.pTransform->Get_Scale().x;
    m_fCurrentScaleX = m_tProgress.pTransform->Get_Scale().x;

    // 텍스쳐 로드
    m_tBackground.pTexture->Ready_Texture(L"HP_Background");
    m_tBackground.pTexture->Set_Texture(0);

    m_tProgress.pTexture->Ready_Texture(L"HP_Fill");
    m_tProgress.pTexture->Set_Texture(0);

    // 위치 값 설정
    _vec3 vOwnerPos = m_pOwnerTransform->Get_Pos();
    vOwnerPos.y += m_fOffsetY;
    m_tBackground.pTransform->Set_Pos(vOwnerPos);
    m_tProgress.pTransform->Set_Pos(vOwnerPos);

    // HP 값 설정
    m_fMaxHp = m_pOwner->Get_HealthStat()->Get_MaxValue();
    m_fMinHp = m_pOwner->Get_HealthStat()->Get_MinValue();
    m_fCurHp = m_pOwner->Get_HealthStat()->Get_CurValue();

    // 애니메이션 적용 시간 
    m_fAnimTotalTime = 0.7f;
    m_fFadeTotalTime = 0.2f;
    m_fVisibleTotalTime = 2.5f;

    m_fAlpha = 0.f;
}

void CBossDesertBodyHPBar::Tick_State(const _float& fTimeDelta)
{
    if (m_bAppear)
    {
        m_fFadeElapsedTime += fTimeDelta;
        float t = m_fFadeElapsedTime / m_fFadeTotalTime;

        if (t > 1.f) t = 1.f;

        m_fAlpha = 255.f * t;

        if (t >= 1.f)
        {
            m_fAlpha = 255.f;
            m_bAppear = false;
            m_fFadeElapsedTime = 0.f;
        }
        return;
    }

    if (m_bVisible)
    {
        m_fVisibleElapsedTime += fTimeDelta;

        if (m_fVisibleElapsedTime >= m_fVisibleTotalTime)
        {
            m_bVisible = false;
            m_bDisappear = true;
            m_fFadeElapsedTime = 0.f;
        }
        return;
    }

    if (m_bDisappear)
    {
        m_fFadeElapsedTime += fTimeDelta;
        float t = m_fFadeElapsedTime / m_fFadeTotalTime;

        if (t > 1.f) t = 1.f;

        m_fAlpha = 255.f * (1.f - t);

        if (t >= 1.f)
        {
            m_fAlpha = 0.f;
            m_bDisappear = false;
            m_fFadeElapsedTime = 0.f;
        }
    }
}


CBossDesertBodyHPBar* CBossDesertBodyHPBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner)
{
    CBossDesertBodyHPBar* pHpBar = new CBossDesertBodyHPBar(pGraphicDev, pOwner);

    if (FAILED(pHpBar->Ready_GameObject()))
    {
        Safe_Release(pHpBar);
        MSG_BOX("Monster HPBar Create Falied");
        return nullptr;

    }
    return pHpBar;
}

void CBossDesertBodyHPBar::Free()
{
    CGameObject::Free();
}
