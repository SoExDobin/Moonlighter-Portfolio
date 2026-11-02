#include "pch.h"
#include "CBoss3DiamondBody.h"
#include "CRenderer.h"
#include "CDataManager.h"
#include "CBossDesert.h"
#include "CBossDesertBodyHPBar.h"
#include "CDamageText.h"
#include "CWallChecker.h"
#include "CDamageZone.h"
#include "CLayerHelper.h"
#include "CBossDesertProjectile.h"
#include "CSoundManager.h"

CBoss3DiamondBody::CBoss3DiamondBody(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBoss3Body(pGraphicDev), m_eState(IDLE)
    , m_fFireRate(0.f), m_fFireRateAcc(0.f), m_iShootCnt(0), m_bLightPrepareEnd(false)
{
    ZeroMemory(&m_vDir, sizeof(_vec3));
    ZeroMemory(&m_vShootAngle, sizeof(_vec3));
}

CBoss3DiamondBody::~CBoss3DiamondBody()
{
}

HRESULT CBoss3DiamondBody::Ready_GameObject()
{
    m_iObjectID = OBJECT_ID::MONSTER;

    Ready_EntityComponent();

    Configure_Component();

    return S_OK;
}

_int CBoss3DiamondBody::Update_GameObject(const _float fTimeDelta)
{
    if (m_bDead)
    {
        Dying(fTimeDelta);
        Destroy(m_pWallChecker);
        Destroy(m_pDamageZone);
        m_eState = DEAD;
    }
    if (m_bHurt)
        Hurting(fTimeDelta);

    m_pHPBar->Update_GameObject(fTimeDelta);
    m_pDamageText->Update_GameObject(fTimeDelta);
    if (m_eState != DEAD)
    {
        m_pDamageZone->Get_Trans()->Set_Pos(
            m_pTransformCom->Get_Pos());
    }

    _int iExit = CRenderObject::Update_GameObject(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CBoss3DiamondBody::LateUpdate_GameObject(const _float fTimeDelta)
{
    m_pHPBar->LateUpdate_GameObject(fTimeDelta);
    m_pDamageText->LateUpdate_GameObject(fTimeDelta);

    Light_Attack(fTimeDelta);
    Attack_Prepare(fTimeDelta);
    Attack(fTimeDelta);
    Attack_End(fTimeDelta);

    CRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CBoss3DiamondBody::Render_GameObject()
{
    m_pHPBar->Render_GameObject();
    m_pDamageText->Render_GameObject();

    if (m_bDead)   // 죽어갈 때 효과
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

void CBoss3DiamondBody::OnLight_Attack()
{
    _vec3 vAngle = m_pHead->Get_Trans()->Get_Angle();
    m_vDir = { -cosf(vAngle.z) , 0.f, sinf(vAngle.x) };
    D3DXVec3Normalize(&m_vDir, &m_vDir);
    m_pDynamicTexCom->Set_Texture(LIGHT);
    m_pDynamicTexCom->Set_Loop(false);
    m_eState = LIGHT;
    m_bLightPrepareEnd = false;
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_head_charge_start.wav", CHANNEL_6, 0.5f);
}

void CBoss3DiamondBody::OffLight_Attack()
{
    ZeroMemory(&m_vDir, sizeof(_vec3));

    m_pDamageZone->Activate_DamageZone(false);
    m_pDynamicTexCom->Set_Texture(LIGHT_END);
    m_pDynamicTexCom->Set_Loop(false);
    m_eState = IDLE;
}

void CBoss3DiamondBody::OnAttack()
{
    m_vShootAngle = { -10.f, 0.f, -10.f };

    m_bIsReady = false;
    m_eState = ATTACK_PRE;
    m_pDynamicTexCom->Set_Texture(ATTACK_PRE);
    m_pDynamicTexCom->Set_Loop(false);
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_open_fire_ball.wav", CHANNEL_9, 0.2f);
}

void CBoss3DiamondBody::Light_Attack(const _float fTimeDelta)
{
    if (m_eState != LIGHT) return;
    if (!m_pDynamicTexCom->Get_OneLoop()) return;
    if (m_bLightPrepareEnd == false)
    {
        m_bLightPrepareEnd = true;
        CSoundManager::GetInstance()->PlaySoundFX(L"desert_dungeon_boss_head_charge_stop.wav", CHANNEL_7, 0.2f);
    }
    m_pDamageZone->Activate_DamageZone(true);

    if (m_pWallChecker->Is_Wall() &&
        m_pWallChecker->Get_ColState() == ENTER_COL)
    {
        _vec3 vNormal = m_pWallChecker->Get_ColNormal();
        _float fScalar = D3DXVec3Dot(&m_vDir, &vNormal);
        m_vDir = m_vDir - (2.f * fScalar) * vNormal;
        D3DXVec3Normalize(&m_vDir, &m_vDir);
    }

    _vec3 vPos = m_pTransformCom->Get_Pos() + (m_vDir * fTimeDelta * m_fSpeed);
    m_pTransformCom->Set_Pos(vPos);
}

void CBoss3DiamondBody::Attack_Prepare(const _float fTimeDelta)
{
    if (m_eState != ATTACK_PRE) return;
    if (!m_pDynamicTexCom->Get_OneLoop()) return;

    m_fFireRateAcc = 0.f;
    m_iShootCnt = 0;
    m_eState = ATTACK;
    m_pDynamicTexCom->Set_Texture(ATTACK);
    m_pDynamicTexCom->Set_Loop(true);
}

void CBoss3DiamondBody::Attack(const _float fTimeDelta)
{
    if (m_eState != ATTACK) return;

    m_fFireRateAcc += fTimeDelta;

    if (m_fFireRateAcc >= m_fFireRate)
    {
        const _float fScale = m_pTransformCom->Get_Scale().x * 0.5f;
        for (size_t i = 0; i < 36; ++i)
        {
            m_vShootAngle.x = m_vShootAngle.x + 10;
            m_vShootAngle.z = m_vShootAngle.z + 10;

            _vec3 vDir = { sinf(D3DXToRadian(m_vShootAngle.x)),
                           0.f,
                           cosf(D3DXToRadian(m_vShootAngle.z)) };
            D3DXVec3Normalize(&vDir, &vDir);

            CBossDesertProjectile* pProj =
                CBossDesertProjectile::Create(m_pGraphicDevice, m_tProj);
            pProj->Set_SpawnPos(m_pTransformCom->Get_Pos() + (vDir * fScale));
            pProj->Set_ShootDir(vDir);
            pProj->Set_Shooting();

            // FIXME : 레이어 설명 들은 뒤 수정 
            if (FAILED(CLayerHelper::GetInstance()
                ->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(MONSTER_ATK_LAYER))
                ->Add_GameObject(L"Circle_Projectile", pProj)))
                MSG_BOX("Create Boss3 Diamond Projectile");
        }

        m_fFireRateAcc -= m_fFireRate;
        ++m_iShootCnt;
        CSoundManager::GetInstance()->PlaySoundFX(L"desert_dungeon_projectile.wav", CHANNEL_8, 0.25f);
    }

    if (m_iShootCnt == 3)
    {
        m_eState = ATTACK_END;
        m_pDynamicTexCom->Set_Texture(ATTACK_END);
        m_pDynamicTexCom->Set_Loop(false);
    }
}

void CBoss3DiamondBody::Attack_End(const _float fTimeDelta)
{
    if (m_eState != ATTACK_END) return;
    if (!m_pDynamicTexCom->Get_OneLoop()) return;

    m_bIsReady = true;
    m_eState = IDLE;
    m_pDynamicTexCom->Set_Texture(IDLE);
    m_pDynamicTexCom->Set_Loop(false);
}

void CBoss3DiamondBody::Ready_EntityComponent()
{
    m_pColCom = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER);

    // Ready Combat Stats                
    m_pCombatStats = Add_Component<CCombatStats>(ID_DYNAMIC,
        n_wsCombatStat, CDataManager::Get_ClientPrototypeTag(COMBAT_STATS));
    m_pCombatComponent = Add_Component<CCombatComponent>(ID_DYNAMIC,
        n_wsCombat, CDataManager::Get_ClientPrototypeTag(COMBAT));

    if (m_pDynamicTexCom = Add_Component<CTexture>(ID_DYNAMIC, n_wsTex, TEXTURE))
    {
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock_Attack_Prepare");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock_Attack");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock_Attack_End");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock_Light_Start");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_DiaBlock_Light_End");
        m_pDynamicTexCom->Set_Texture(IDLE);
    }
}

void CBoss3DiamondBody::Configure_Component()
{
    m_fSpeed = 20.f;
    m_fFireRate = 0.75f;
    m_iShootCnt = 0;

    // Set Basic Components
    m_pTransformCom->Add_Rotation(ROT_X, D3DXToRadian(65.f));

    // Scaling
    static_cast<CRectCollider*>(m_pColCom)->Set_Dimension({ 4.f, 4.f, 4.f });
    m_pTransformCom->Set_Scale({ 7.f, 7.f, 1.f });

    // Set Combat Stats
    m_pCombatStats->Get_HealthStat()->SetUp(0.f, 200.f, 200.f);
    m_pCombatStats->Get_AttackStat()->SetUp(20.f, 50.f, 20.f);
    m_pCombatStats->Get_DefenseStat()->SetUp(7.f, 15.f, 11.f);
    m_pCombatComponent->SetUp(m_pCombatStats);

    // 피격 효과 유지 시간
    m_fTotalHurtTime = 0.2f;
    m_fTotalDeadTime = 1.f;

    m_pCombatStats->Get_HealthStat()->m_OnValueMin = [&]() {
        On_Dead();
        m_pDynamicTexCom->Stop_Anim();
        };

    m_pHPBar = CBossDesertBodyHPBar::Create(m_pGraphicDevice, this);
    static_cast<CBossDesertBodyHPBar*>(m_pHPBar)->SetUp_HPBar({ 3.f, 3.f, 1.f }, 0.f);
    static_cast<CBossDesertBodyHPBar*>(m_pHPBar)->Set_HeadHPBar(m_pHead->Get_HpBar());

    m_pDamageText = CDamageText::Create(m_pGraphicDevice, this);
    m_pDamageText->SetUp_DamageText({ 0.5f, 0.5f, 0.5f }, 2.f, 2.f);

    {
        // 투사체 정보 설정 
        m_tProj.fTextureSpeed = 10.f;
        m_tProj.fSpeed = 20.f;
        m_tProj.fScale = 0.5f;
        m_tProj.eColType = COL_TYPE::SPHERE_COL;
        m_tProj.dwCurFlag = OBJECT_ID::MONSTER_ATK;
        m_tProj.dwHitTargetFlag = OBJECT_ID::PLAYER;
        m_tProj.dwDestroyOnHitFlag
            = OBJECT_ID::PLAYER_ATK | OBJECT_ID::INTERACTABLE | OBJECT_ID::ENVIRONMENT;

        // 데미지 정보 설정 
        m_tDmgInfo.pAttacker = this;
        m_tDmgInfo.dwHitTargetFlag = m_tProj.dwHitTargetFlag;
        m_tDmgInfo.fAmount = 10.f;
        m_tDmgInfo.bCanParry = false;
        m_tDmgInfo.bShouldKnockback = true;
        m_tDmgInfo.eApplyTiming = Engine::ENTER_COL;
        m_tDmgInfo.vDirKnockback = { 0.f, 0.f, 0.f };

        m_tProj.tDamageInfo = m_tDmgInfo;
    }
}

void CBoss3DiamondBody::Hurting(const _float fTimeDelta)
{
    if (m_fStateElapsedTime > m_fTotalHurtTime * 0.6f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255); 
    else if (m_fStateElapsedTime > m_fTotalHurtTime * 0.4f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 0, 0);     
    else
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255); 

    CMonster::hurting(fTimeDelta);
}

CBoss3DiamondBody* CBoss3DiamondBody::Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pHead)
{
    CBoss3DiamondBody* pInstance = new CBoss3DiamondBody(pGraphicDev);

    if (FAILED(pInstance->Ready_Body(pHead)) ||
        FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CBoss3DiamondBody Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CBoss3DiamondBody::Free()
{
    Safe_Release(m_pHPBar);
    Safe_Release(m_pDamageText);
    CBoss3Body::Free();
}
