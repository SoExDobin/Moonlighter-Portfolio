#include "pch.h"
#include "CBossDesertHPBar.h"
#include "CRenderer.h"
#include "CTexture.h"
#include "EasingFunctions.h"
#include "CMath.h"
#include "CBossDesert.h"

CBossDesertHPBar::CBossDesertHPBar(LPDIRECT3DDEVICE9 pGraphicDev, CMonster* pOwner)
    : CMonsterHPBar(pGraphicDev, pOwner)
    , m_fMaxHp(0), m_fMinHp(0), m_fCurHp(0)
    , m_fBackgroundWidth(0), m_fCurrentScaleX(0)
    , m_fTargetScaleX(0), m_fOriginScaleX(0), m_fCurrentOffsetX(0), m_fTargetOffsetX(0)
    , m_fOriginOffsetX(0), m_fAnimElapsedTime(0), m_fAnimTotalTime(0), m_bAnimating(false)
{
    ZeroMemory(&m_tBackground, sizeof(m_tBackground));
    ZeroMemory(&m_tProgress, sizeof(m_tProgress));
}

CBossDesertHPBar::CBossDesertHPBar(const CBossDesertHPBar& rhs)
    : CMonsterHPBar(rhs)
    , m_fMaxHp(0), m_fMinHp(0), m_fCurHp(0)
    , m_fBackgroundWidth(0), m_fCurrentScaleX(0), m_fTargetScaleX(0), m_fOriginScaleX(0)
    , m_fCurrentOffsetX(0), m_fTargetOffsetX(0), m_fOriginOffsetX(0)
    , m_fAnimElapsedTime(0), m_fAnimTotalTime(0), m_bAnimating(false)
{
    memcpy(&m_tBackground, &rhs.m_tBackground, sizeof(m_tBackground));
    memcpy(&m_tProgress, &rhs.m_tProgress, sizeof(m_tProgress));
}

CBossDesertHPBar::~CBossDesertHPBar()
{
}

HRESULT CBossDesertHPBar::Ready_GameObject()
{
    // 기본 컴포넌트 추가
    if (FAILED(CGameObject::Ready_GameObject()))
        return E_FAIL;

    Set_RenderPriority(UI_HPBAR);
    // Progress
    m_tProgress.pTransform = Add_Component<CTransform>(ID_DYNAMIC, L"Fill_Transform", TRANSFORM);
    m_tProgress.pBuffer = Add_Component<CRectTex>(ID_STATIC, L"Fill_Buffer", RECTTEX);
    m_tProgress.pTexture = Add_Component<CTexture>(ID_DYNAMIC, L"Fill_Texture", TEXTURE);

    // BG
    m_tBackground.pTransform = Add_Component<CTransform>(ID_DYNAMIC, L"Background_Transform", TRANSFORM);
    m_tBackground.pBuffer = Add_Component<CRectTex>(ID_STATIC, L"Background_Buffer", RECTTEX);
    m_tBackground.pTexture = Add_Component<CTexture>(ID_DYNAMIC, L"Background_Texture", TEXTURE);

    Configure_Variable();

    return S_OK;
}

_int CBossDesertHPBar::Update_GameObject(const _float fTimeDelta)
{

    _int iExit = Engine::CGameObject::Update_GameObject(fTimeDelta);

    Engine::CRenderer::GetInstance()->Add_RenderGroup(RENDER_UI, this);

    Update_Bar(fTimeDelta);

    Tick_State(fTimeDelta);

    return iExit;
}

void CBossDesertHPBar::LateUpdate_GameObject(const _float fTimeDelta)
{
    Engine::CGameObject::LateUpdate_GameObject(fTimeDelta);
}

void CBossDesertHPBar::Render_GameObject()
{
    // 1) HP 채우기
    m_tProgress.pTexture->Set_Texture(0);
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_tProgress.pTransform->Get_World());
    m_tProgress.pBuffer->Render_Buffer();

    // 2) 배경
    m_tBackground.pTexture->Set_Texture(0);
    m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_tBackground.pTransform->Get_World());
    m_tBackground.pBuffer->Render_Buffer();
}

void CBossDesertHPBar::SetUp_HPBar(const _vec3& vScale, const _float& fOffsetY)
{
    m_tBackground.pTransform->Set_Scale(vScale);
    m_tProgress.pTransform->Set_Scale(vScale);
    m_fBackgroundWidth = vScale.x;
}

void CBossDesertHPBar::On_HPChanged()
{
    _float curRatio = m_pOwner->Get_HealthStat()->Get_CurValue();
    for (auto pBody : *m_pVecNode)
    {
        if (pBody)
            curRatio += pBody->Get_HealthStat()->Get_CurValue();
    }
    curRatio = curRatio / m_fMaxHp;

    // 타겟 상태
    m_fTargetScaleX = m_tBackground.pTransform->Get_Scale().x * curRatio;
    m_fTargetOffsetX = m_fBackgroundWidth * 0.5f * (1.f - curRatio);

    // 가로 스케일이 줄어들며 빈공간도 줄어든 채로 offset이 밀려, fill 이 frame을 튀어나오는 문제 방지 
    m_fTargetOffsetX -= m_fTargetOffsetX * 0.041f;

    m_fOriginScaleX = m_fCurrentScaleX;
    m_fOriginOffsetX = m_fCurrentOffsetX;

    // 애니메이션 시작
    m_fAnimElapsedTime = 0.f;
    m_bAnimating = true;

}

void CBossDesertHPBar::Init_GroupHp()
{
    m_fMaxHp = m_pOwner->Get_HealthStat()->Get_MaxValue();
    m_fMinHp = m_pOwner->Get_HealthStat()->Get_MinValue();
    m_fCurHp = m_pOwner->Get_HealthStat()->Get_CurValue();

    for (auto pBody : *m_pVecNode)
    {
        m_fMaxHp += pBody->Get_HealthStat()->Get_MaxValue();
        m_fMinHp += pBody->Get_HealthStat()->Get_MinValue();
        m_fCurHp += pBody->Get_HealthStat()->Get_CurValue();
    }
}

void CBossDesertHPBar::Update_Bar(const _float& fTimeDelta)
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

void CBossDesertHPBar::Apply_Transform()
{
    _vec3 vProgressPos, vScale;
    // Background
    m_tBackground.pTransform->Set_Pos(m_vStaticPos);

    // Fill
    vProgressPos = m_vStaticPos;
    vProgressPos.x -= m_fCurrentOffsetX;
    m_tProgress.pTransform->Set_Pos(vProgressPos);

    vScale = m_tProgress.pTransform->Get_Scale();
    vScale.x = m_fCurrentScaleX;
    m_tProgress.pTransform->Set_Scale(vScale);
}

void CBossDesertHPBar::Configure_Variable()
{
    // 스케일 조정
    m_tBackground.pTransform->Set_Scale({ 1000.f, 1000.f, 1.f });
    m_tProgress.pTransform->Set_Scale(m_tBackground.pTransform->Get_Scale());   // Full 

    m_fBackgroundWidth = m_tBackground.pTransform->Get_Scale().x;
    m_fCurrentScaleX = m_tProgress.pTransform->Get_Scale().x;

    // 텍스쳐 로드
    m_tBackground.pTexture->Ready_Texture(L"Boss_HP_Background");
    m_tBackground.pTexture->Set_Texture(0);

    m_tProgress.pTexture->Ready_Texture(L"Boss_HP_Fill");
    m_tProgress.pTexture->Set_Texture(0);

    // 위치 값 설정
    m_vStaticPos = { 570, 730, 0 };
    m_tBackground.pTransform->Set_Pos(m_vStaticPos);
    m_tProgress.pTransform->Set_Pos(m_vStaticPos);

    // HP 값 설정
    m_fMaxHp = m_pOwner->Get_HealthStat()->Get_MaxValue();
    m_fMinHp = m_pOwner->Get_HealthStat()->Get_MinValue();
    m_fCurHp = m_pOwner->Get_HealthStat()->Get_CurValue();

    // 애니메이션 적용 시간 
    m_fAnimTotalTime = 0.7f;
}

void CBossDesertHPBar::Tick_State(const _float& fTimeDelta)
{
}


CBossDesertHPBar* CBossDesertHPBar::Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pOwner)
{
    CBossDesertHPBar* pHpBar = new CBossDesertHPBar(pGraphicDev, pOwner);
    pHpBar->m_pVecNode = &(pOwner->Get_BodyGroup());
    if (FAILED(pHpBar->Ready_GameObject()))
    {
        Safe_Release(pHpBar);
        MSG_BOX("CBossDesertHPBar Create Falied");
        return nullptr;

    }
    return pHpBar;
}

void CBossDesertHPBar::Free()
{
    CGameObject::Free();
}
