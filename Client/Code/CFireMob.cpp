#include "pch.h"
#include "CFireMob.h"
#include "CRenderer.h"
#include "CPrototypeManager.h"
#include "CPlayer.h"
#include "CHitRectBox.h"
#include "CDamageZone.h"
#include "CMobHPBar.h"
#include "CDamageText.h"
#include "CDungeonManager.h"
#include "CSoundManager.h"
#include "CPlanet.h"

CFireMob::CFireMob(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_pDynamicTexCom(nullptr), m_eState(FIREMOB_END), m_fElapsedTime(0.f)
    , m_bIsExplode(false)
{
}

CFireMob::CFireMob(const CFireMob& rhs)
    : CMonster(rhs), m_pDynamicTexCom(nullptr), m_eState(FIREMOB_END), m_fElapsedTime(0.f)
    , m_bIsExplode(false)
{
}

CFireMob::~CFireMob()
{
}

HRESULT CFireMob::Ready_GameObject()
{
    m_iObjectID = OBJECT_ID::MONSTER;

    if (FAILED(Engine::CRenderObject::Ready_GameObject()))
        return E_FAIL;

    // 텍스처 컴포넌트 생성 
    m_pDynamicTexCom = Add_Component<CTexture>(ID_DYNAMIC, L"Texture_Com", TEXTURE);

    Ready_EntityComponent();
    Ready_Animation();
    Ready_Combat();

    Configure_Component();

    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_elemental_born.wav", CHANNEL_10, 0.2f);

    return S_OK;
}

_int CFireMob::Update_GameObject(const _float fTimeDelta)
{
    if (m_bDead)
        Dying(fTimeDelta);
    if (m_bHurt)
        hurting((fTimeDelta));

    m_pHPBar->Update_GameObject(fTimeDelta);
    m_pDamageText->Update_GameObject(fTimeDelta);

    _int iExit = Engine::CRenderObject::Update_GameObject(fTimeDelta);
    Engine::CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CFireMob::LateUpdate_GameObject(const _float fTimeDelta)
{
    m_pHPBar->LateUpdate_GameObject(fTimeDelta);
    m_pDamageText->LateUpdate_GameObject(fTimeDelta);

    Update_Born(fTimeDelta);
    Update_Idle(fTimeDelta);
    Update_Explode(fTimeDelta);

    Engine::CRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CFireMob::Render_GameObject()
{
    m_pHPBar->Render_GameObject();
    m_pDamageText->Render_GameObject();

    if (m_bDead)    // 죽어갈 때 효과
        CMonster::Begin_Render_Dying();
    if (m_bHurt)   // 맞았을 때 효과 
        CMonster::Begin_Render_Hurting();

    // 기본 렌더 로직
    {
        m_pGraphicDevice->SetTransform(D3DTS_WORLD, m_pTransformCom->Get_World());

        m_pGraphicDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
        m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

        m_pDynamicTexCom->SetUp_Texture();
        m_pBufferCom->Render_Buffer();

        m_pGraphicDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        m_pGraphicDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }

    // 적용한 효과 복구 로직
    if (m_bDead)
        End_Render_Dying();
    if (m_bHurt)
        End_Render_Hurting();
}

void CFireMob::hurting(const _float& fTiimeDelta)
{
    if (m_bKnockback)
    {
        _vec3 vKnockback = m_pCombatComponent->Get_KnockbackDir();
        m_pTransformCom->Move_Pos(&vKnockback, fTiimeDelta, 2.f);
    }

    CMonster::hurting(fTiimeDelta);
}

HRESULT CFireMob::Ready_Animation()
{
    m_pDynamicTexCom->Ready_Texture(L"FireMob_Born");  
    m_pDynamicTexCom->Ready_Texture(L"FireMob_Idle");
    m_pDynamicTexCom->Ready_Texture(L"FireMob_Explode");

    return S_OK;
}

HRESULT CFireMob::Ready_Combat()
{
    m_tDamage.dwHitTargetFlag = OBJECT_ID::PLAYER;
    m_tDamage.eApplyTiming = COL_STATE::ENTER_COL;
    m_tDamage.fAmount = 5.f;
    m_tDamage.pAttacker = this;

    m_pDamageZone = CDamageZone::Create(m_pGraphicDevice, this, HIT_RECTBOX, m_tDamage);
    m_pDamageZone->Activate_DamageZone(false);
    m_pDamageZone->Set_Position(Get_Trans()->Get_Pos());
    m_pDamageZone->Get_Hitbox()->Set_Scale(2.5f);

    CLayerHelper::GetInstance()->Get_Layer(
        CDataManager::GetInstance()->Get_LayerTag(MONSTER_ATK_LAYER))->
        Add_GameObject(L"Explode_Zone", m_pDamageZone);

    CGameObject* pPlayer = CManagement::GetInstance()->Get_Object(CDataManager::Get_LayerTag(PLAYER_LAYER), L"Player");
    m_pPlayerTransformCom = static_cast<CTransform*>(pPlayer->Get_Component(ID_DYNAMIC, TRANSFORM));

    return S_OK;
}

void CFireMob::Ready_EntityComponent()
{
    // Ready Collider
    m_pColCom = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER);

    // Ready Combat Stats                
    m_pCombatStats = Add_Component<CCombatStats>(ID_DYNAMIC,
        L"CombatStats_Com",
        CDataManager::GetInstance()->Get_ClientPrototypeTag(COMBAT_STATS));
    m_pCombatComponent = Add_Component<CCombatComponent>(ID_DYNAMIC,
        L"Combat_Com",
        CDataManager::GetInstance()->Get_ClientPrototypeTag(COMBAT));
}

void CFireMob::Configure_Component()
{
    // Set Basic Components
    m_pTransformCom->Set_Scale(5.f, 7.5f, 1.f);
    m_pTransformCom->Add_Rotation(ROT_X, D3DXToRadian(55.f));

    static_cast<CRectCollider*>(m_pColCom)->Set_Dimension({ 1.f, 1.f, 1.5f });

    // Set Combat Stats
    m_pCombatStats->Get_HealthStat()->SetUp(0.f, 10.f, 10.f);
    m_pCombatStats->Get_AttackStat()->SetUp(5.f, 10.f, 7.5f);
    m_pCombatStats->Get_DefenseStat()->SetUp(0.f, 1.f, 0.5f);

    m_pCombatComponent->SetUp(m_pCombatStats);

    m_pCombatStats->Get_HealthStat()->m_OnValueMin = [&]() {
        On_Dead();
        m_eState = DEAD;
        Destroy(m_pDamageZone);
        m_pDynamicTexCom->Stop_Anim();
        CDungeonManager::GetInstance()->On_MonsterKilled(this);
        };

    // 피격 효과 유지 시간
    m_fTotalHurtTime = 0.2f;
    m_fTotalDeadTime = 1.f;

    // 몬스터 UI
    m_pHPBar = CMobHPBar::Create(m_pGraphicDevice, this);
    static_cast<CMobHPBar*>(m_pHPBar)->SetUp_HPBar({ 2.f, 2.f, 1.f }, 0.f);
    m_pDamageText = CDamageText::Create(m_pGraphicDevice, this);

    // 상태 머신
    m_eState = BORN;
    m_pDynamicTexCom->Set_Texture(BORN);
    m_pDynamicTexCom->Set_Loop(false);
}

void CFireMob::Update_Born(const _float fTimeDelta)
{
    if (m_eState == DEAD) return;
    if (m_eState != BORN) return;
    if (m_pDynamicTexCom->Get_OneLoop() == false) return;

    m_eState = IDLE;
    m_pDynamicTexCom->Set_Texture(IDLE);
    m_pDynamicTexCom->Set_Loop(true);
    m_pTransformCom->Set_Scale({ 2.5f, 3.75f, 1.f });

    _vec3 vPos = m_pTransformCom->Get_Pos();
    vPos.z -= 1.25f;
    
    m_pTransformCom->Set_Pos(vPos);
}

void CFireMob::Update_Idle(const _float fTimeDelta)
{
    if (m_eState == DEAD) return;
    if (m_eState != IDLE) return;

    _vec3 vDst = m_pPlayerTransformCom->Get_Pos() - m_pTransformCom->Get_Pos();
    _float fSqr = D3DXVec3Length(&vDst);

    if (fSqr <= 3.f || m_fElapsedTime >= n_fExplodeTime)
    {
        m_eState = EXPLODE;
        m_pDynamicTexCom->Set_Texture(EXPLODE);
        m_pDynamicTexCom->Set_Loop(false);
        m_pDamageZone->Set_Position(Get_Trans()->Get_Pos());
    }

    m_fElapsedTime += fTimeDelta;
}

void CFireMob::Update_Explode(const _float fTimeDelta)
{
    if (m_eState == DEAD) return;
    if (m_eState != EXPLODE) return;

    if (m_pDynamicTexCom->Get_CurFrame() >= n_iExplodeIdx && m_bIsExplode == false)
    {
        m_bIsExplode = true;
        m_pDamageZone->Activate_DamageZone(true);
        CSoundManager::GetInstance()->PlaySoundFX(L"desert_dungeon_elemental_attack.wav", CHANNEL_10, 0.25f);
    }
    if (m_pDynamicTexCom->Get_OneLoop())
    {
        m_pDamageZone->Activate_DamageZone(false);
        m_eState = DEAD;
        Destroy(this);
        Destroy(m_pDamageZone);
    }
}

CFireMob* CFireMob::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CFireMob* pInstance = new CFireMob(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CFireMob Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CFireMob::Free()
{
    Engine::CGameObject::Free();

    m_pHPBar->Release();
    m_pDamageText->Release();
}
