#include "pch.h"
#include "CBoss3SquareBody.h"
#include "CRenderer.h"
#include "CDataManager.h"

#include "CBossDesertBodyHPBar.h"
#include "CDamageText.h"
#include "CBossDesert.h"
#include "CBoss3HeadMovement.h"
#include "CWallChecker.h"
#include "CDamageZone.h"
#include "CLayerHelper.h"
#include "CRandom.h"
#include "CManagement.h"
#include "CBossDesertProjectile3.h"
#include "CSoundManager.h"

CBoss3SquareBody::CBoss3SquareBody(LPDIRECT3DDEVICE9 pGraphicDev)
    : CBoss3Body(pGraphicDev), m_eState(IDLE), m_pPlayer(nullptr), m_bLightPrepareEnd(false)
{
    ZeroMemory(&m_vDir, sizeof(_vec3));
    ZeroMemory(&m_vTargetPos, sizeof(_vec3));
}

CBoss3SquareBody::~CBoss3SquareBody()
{

}

HRESULT CBoss3SquareBody::Ready_GameObject()
{
    m_iObjectID = OBJECT_ID::MONSTER;

    Ready_EntityComponent();

    Configure_Component();

    if (m_pPlayer = Engine::CManagement::GetInstance()
        ->Get_Object(CDataManager::GetInstance()->Get_LayerTag(PLAYER_LAYER), L"Player"))
    {
        ;
    }
    else return E_FAIL;

    return S_OK;
}

_int CBoss3SquareBody::Update_GameObject(const _float fTimeDelta)
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

void CBoss3SquareBody::LateUpdate_GameObject(const _float fTimeDelta)
{
    m_pHPBar->LateUpdate_GameObject(fTimeDelta);
    m_pDamageText->LateUpdate_GameObject(fTimeDelta);

    Light_Attack(fTimeDelta);
    Attack(fTimeDelta);

    CRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CBoss3SquareBody::Render_GameObject()
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

void CBoss3SquareBody::OnLight_Attack()
{
    _vec3 vAngle = m_pHead->Get_Trans()->Get_Angle();
    m_vDir = { cosf(vAngle.z), 0.f, sinf(vAngle.x) };
    D3DXVec3Normalize(&m_vDir, &m_vDir);
    m_pDynamicTexCom->Set_Texture(LIGHT);
    m_pDynamicTexCom->Set_Loop(false);
    m_eState = LIGHT;
    m_bLightPrepareEnd = false;
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_head_charge_start.wav", CHANNEL_6, 0.5f);
}

void CBoss3SquareBody::OffLight_Attack()
{
    ZeroMemory(&m_vDir, sizeof(_vec3));

    m_pDamageZone->Activate_DamageZone(false);
    m_pDynamicTexCom->Set_Texture(LIGHT_END);
    m_pDynamicTexCom->Set_Loop(false);
    m_eState = IDLE;
}

void CBoss3SquareBody::OnAttack()
{
    if (m_pPlayer->Get_Type() != RENDER_OBJECT) return;
    m_vTargetPos = static_cast<CRenderObject*>(m_pPlayer)->Get_Trans()->Get_Pos();

    m_bIsReady = false;
    m_eState = ATTACK;
    m_pDynamicTexCom->Set_Texture(ATTACK);
    m_pDynamicTexCom->Set_Loop(false);
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_open_beam.wav", CHANNEL_9, 0.2f);
}

void CBoss3SquareBody::Light_Attack(const _float fTimeDelta)
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

void CBoss3SquareBody::Attack(const _float fTimeDelta)
{
    if (m_eState != ATTACK) return;

    if (m_pDynamicTexCom->Get_CurFrame() == n_iAttackTex)
    {
        _vec3 vStart = m_pTransformCom->Get_Pos();
        _vec3 vTarget = m_vTargetPos;
        vTarget.x += Get_Int_Random<_int>(-3, 3);
        vTarget.z += Get_Int_Random<_int>(-3, 3);
        _float fSpeed = Get_Float_Random(1.f, 20.f);

        CBossDesertProjectile3* pProj = CBossDesertProjectile3::Create(m_pGraphicDevice, m_tProj);

        _vec3 vDir = vTarget - vStart;
        vDir.y = 0.f;
        D3DXVec3Normalize(&vDir, &vDir);
        vStart.y = 2.f;
        pProj->Set_SpawnPos(vStart);
        pProj->Set_ShootDir(vDir);
        pProj->Set_Speed(fSpeed);
        pProj->Set_Shooting();

        if (FAILED(CLayerHelper::GetInstance()
            ->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(MONSTER_ATK_LAYER))
            ->Add_GameObject(L"Square_Projectile", pProj)))
            MSG_BOX("Create Boss3 Square Projectile");
        CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_fire_ball.wav", CHANNEL_8, 0.5f);
    }

    if (m_pDynamicTexCom->Get_OneLoop())
    {
        m_bIsReady = true;
        m_eState = IDLE;
        m_pDynamicTexCom->Set_Texture(IDLE);
        m_pDynamicTexCom->Set_Loop(false);
    }
}

void CBoss3SquareBody::Ready_EntityComponent()
{
    m_pColCom = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER);

    // Ready Combat Stats                
    m_pCombatStats = Add_Component<CCombatStats>(ID_DYNAMIC,
        n_wsCombatStat, CDataManager::Get_ClientPrototypeTag(COMBAT_STATS));
    m_pCombatComponent = Add_Component<CCombatComponent>(ID_DYNAMIC,
        n_wsCombat, CDataManager::Get_ClientPrototypeTag(COMBAT));

    if (m_pDynamicTexCom = Add_Component<CTexture>(ID_DYNAMIC, n_wsTex, TEXTURE))
    {
        m_pDynamicTexCom->Ready_Texture(L"Boss3_SqrBlock");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_SqrBlock_Attack");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_SqrBlock_Light_Start");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_SqrBlock_Light_End");
        m_pDynamicTexCom->Set_Texture(IDLE);
    }
}

void CBoss3SquareBody::Configure_Component()
{
    m_fSpeed = 20.f;

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
        m_tProj.fScale = 1.f;
        m_tProj.eColType = COL_TYPE::SPHERE_COL;
        m_tProj.dwCurFlag = OBJECT_ID::ID_END;
        m_tProj.dwHitTargetFlag = OBJECT_ID::ID_END;
        m_tProj.dwDestroyOnHitFlag
            = OBJECT_ID::PLAYER_ATK | OBJECT_ID::INTERACTABLE | OBJECT_ID::ENVIRONMENT;

        // 데미지 정보 설정 
        m_tDmgInfo.pAttacker = this;
        m_tDmgInfo.dwHitTargetFlag = m_tProj.dwHitTargetFlag;
        m_tDmgInfo.fAmount = 0.f;
        m_tDmgInfo.bCanParry = false;
        m_tDmgInfo.bShouldKnockback = true;
        m_tDmgInfo.eApplyTiming = Engine::ENTER_COL;
        m_tDmgInfo.vDirKnockback = { 0.f, 0.f, 0.f };

        m_tProj.tDamageInfo = m_tDmgInfo;
    }
}

void CBoss3SquareBody::Hurting(const _float fTimeDelta)
{
    if (m_fStateElapsedTime > m_fTotalHurtTime * 0.6f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255);
    else if (m_fStateElapsedTime > m_fTotalHurtTime * 0.4f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 0, 0);
    else
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255);

    CMonster::hurting(fTimeDelta);
}

CBoss3SquareBody* CBoss3SquareBody::Create(LPDIRECT3DDEVICE9 pGraphicDev, CBossDesert* pHead)
{
    CBoss3SquareBody* pInstance = new CBoss3SquareBody(pGraphicDev);
    
    if (FAILED(pInstance->Ready_Body(pHead)) ||
        FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CBoss3SquareBody Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CBoss3SquareBody::Free()
{
    Safe_Release(m_pHPBar);
    Safe_Release(m_pDamageText);
    CBoss3Body::Free();
}
