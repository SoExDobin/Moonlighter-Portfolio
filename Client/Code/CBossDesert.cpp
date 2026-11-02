#include "pch.h"
#include "CBossDesert.h"

#include "CDataManager.h"
#include "CRenderer.h"
#include "CSoundManager.h"

#include "CBoss3IntroCover.h"
#include "CBoss3CircleBody.h"
#include "CBoss3DiamondBody.h"
#include "CBoss3SquareBody.h"
#include "CBoss3TriangleBody.h"

#include "CBoss3HeadMovement.h"
#include "CBossDesertHPBar.h"
#include "CDamageText.h"
#include "CWallChecker.h"
#include "CRandom.h"

#include "CLayerHelper.h"
#include "CTexture.h"
#include "CDamageZone.h"


CBossDesert::CBossDesert(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_pDynamicTexCom(nullptr), m_pMovement(nullptr)
    , m_pUpChecker(nullptr), m_pDownChecker(nullptr), m_pLeftChecker(nullptr), m_pRightChecker(nullptr)
    , m_pIntroCover(nullptr)
    , m_eState(SLEEP), m_fBodySpacing(0.f), m_fRecordStep(0.f), m_fFollowLerp(0.f)
    , m_fAttackDelta(0.f), m_fLightDelta(0.f), m_fDeadElapsedTime(0.f)
{
}

CBossDesert::~CBossDesert()
{
}

HRESULT CBossDesert::Ready_GameObject()
{
    m_iObjectID = OBJECT_ID::MONSTER;

    if (FAILED(CRenderObject::Ready_GameObject()))
        return E_FAIL;

    Ready_EntityComponent();

    Configure_Component();

    Ready_BodyBlock();
    Ready_WallChecker();

#ifdef _DEBUG
    Add_EditorFiled();
#endif // _DEBUG

    return S_OK;
}

_int CBossDesert::Update_GameObject(const _float fTimeDelta)
{
    if (m_bHurt)
        Hurting((fTimeDelta));
    
    m_pHPBar->Update_GameObject(fTimeDelta);
    m_pDamageText->Update_GameObject(fTimeDelta);
    if (m_vecNode.empty())
        m_pCombatComponent->Set_Invicible(false);
    CheckWall(fTimeDelta);

    _int iExit = CRenderObject::Update_GameObject(fTimeDelta);

    Record_HeadTrail();
    Apply_TrainBody(fTimeDelta);

    CRenderer::GetInstance()->Add_RenderGroup(RENDER_ALPHA, this);

    return iExit;
}

void CBossDesert::LateUpdate_GameObject(const _float fTimeDelta)
{
    if (m_eState == SLEEP && m_pIntroCover->Is_Destroy())
    {
        m_pMovement->Set_IsMove(true);
    }

    m_pHPBar->LateUpdate_GameObject(fTimeDelta);
    m_pDamageText->LateUpdate_GameObject(fTimeDelta);
    Change_Head_Texture();
    CRenderObject::LateUpdate_GameObject(fTimeDelta);

    if (m_eState == SLEEP) return;
    Attack(fTimeDelta);
    Check_LightAttack(fTimeDelta);
    Dead(fTimeDelta);

    if (m_pMovement->Get_IsMove())
        CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_head_idle.wav", CHANNEL_6, 0.2f);

    // 죽은 노드 제거
    for (auto pNode = m_vecNode.begin(); pNode != m_vecNode.end();)
    {
        if ((*pNode)->Get_IsDead())
        {
            pNode = m_vecNode.erase(pNode);
            continue;
        }
        ++pNode;
    }

#ifdef _DEBUG
    Display_Debug_Param();
#endif // _DEBUG
}

void CBossDesert::Render_GameObject()
{
    m_pHPBar->Render_GameObject();
    m_pDamageText->Render_GameObject();

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

    if (m_bHurt)
        End_Render_Hurting();
}

void CBossDesert::On_Collision(const Collision& tCollision)
{
    if (m_eState == DEAD) return;

    CMonster::On_Collision(tCollision);
}

HRESULT CBossDesert::Init_GroupHP()
{
    if (CBossDesertHPBar* pHpBar = static_cast<CBossDesertHPBar*>(m_pHPBar))
    {
        if (m_vecNode.empty()) return E_FAIL;

        pHpBar->Init_GroupHp();
    }
    else return E_FAIL;

    return S_OK;
}

void CBossDesert::Ready_EntityComponent()
{
    m_pColCom = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER);

    // Ready Combat Stats                
    m_pCombatStats = Add_Component<CCombatStats>(ID_DYNAMIC,
        n_wsCombatStat, CDataManager::Get_ClientPrototypeTag(COMBAT_STATS));
    m_pCombatComponent = Add_Component<CCombatComponent>(ID_DYNAMIC,
        n_wsCombat, CDataManager::Get_ClientPrototypeTag(COMBAT));

    if (m_pDynamicTexCom = Add_Component<CTexture>(ID_DYNAMIC, n_wsTex, TEXTURE))
    {
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Head_Left");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Head_Right");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Head_Up");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Head_Down");
        m_pDynamicTexCom->Ready_Texture(L"Boss3_Head_Dead");
        m_pDynamicTexCom->Set_Texture(UP);
    }

    m_pMovement = Add_Component<CBoss3HeadMovement>(ID_DYNAMIC, n_wsMovement
        , CDataManager::Get_ClientPrototypeTag(BOSS3_HEAD_MOVEMNET));

    m_pHPBar = CBossDesertHPBar::Create(m_pGraphicDevice, this);
    m_pDamageText = CDamageText::Create(m_pGraphicDevice, this);
    m_pDamageText->SetUp_DamageText({ 0.5f, 0.5f, 0.5f }, 2.f, 2.f);
}

void CBossDesert::Configure_Component()
{
    m_eState = SLEEP;
    m_fBodySpacing = 4.f;
    m_fRecordStep = 0.35f;
    m_fFollowLerp = 0.25f;

    m_fAttackDelta = n_fAttackCool;
    m_fLightDelta = n_fLightCool;

    // Set Basic Components
    m_pTransformCom->Add_Rotation(ROT_X, D3DXToRadian(65.f));

    // Scaling
    static_cast<CRectCollider*>(m_pColCom)->Set_Dimension({ 5.f, 5.f, 5.f });
    m_pTransformCom->Set_Scale({ 7.f, 7.f, 1.f });

    m_pCombatStats->Get_HealthStat()->m_OnValueMin = [&]() {
        Destroy(m_pUpChecker);
        Destroy(m_pDownChecker);
        Destroy(m_pLeftChecker);
        Destroy(m_pRightChecker);

        CSoundManager::GetInstance()->StopSound(CHANNEL_6);
        CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_death.wav", CHANNEL_6, 0.5f);

        m_pDynamicTexCom->Set_Texture(DEAD + 1);
        m_pDynamicTexCom->Set_Loop(false);
        m_pMovement->Set_IsMove(false);
        m_pTransformCom->Set_Angle(m_pTransformCom->Get_Angle().x, 0.f, 0.f);
        m_pTransformCom->Set_Scale(10.f, 10.f, 1.f);
        m_eState = DEAD;
        m_bDead = true;
        };

    // Set Combat Stats
    m_pCombatStats->Get_HealthStat()->SetUp(0.f, 200.f, 200.f);
    m_pCombatStats->Get_AttackStat()->SetUp(20.f, 50.f, 20.f);
    m_pCombatStats->Get_DefenseStat()->SetUp(7.f, 15.f, 11.f);
    m_pCombatComponent->SetUp(m_pCombatStats);
    m_pCombatComponent->Set_Invicible(true);

    // 피격 효과 유지 시간
    m_fTotalHurtTime = 0.2f;
    m_fTotalDeadTime = 1.f;

    m_pTransformCom->Set_Angle(D3DXToRadian(70.f), D3DXToRadian(-90.f), 0.f);
}

void CBossDesert::Change_Head_Texture()
{
    if (m_eState == DEAD) return;
    if (!m_pDynamicTexCom || !m_pTransformCom) return;

    _vec3 vLook;
    m_pTransformCom->Get_Info(INFO_LOOK, &vLook);

    vLook.y = 0.f;
    D3DXVec3LengthSq(&vLook);
    D3DXVec3Normalize(&vLook, &vLook);

    // 4방향 결정
    _int iDir = UP;
    if (fabsf(vLook.x) > fabsf(vLook.z))
        iDir = (vLook.x >= 0.f) ? RIGHT : LEFT;
    else
        iDir = (vLook.z >= 0.f) ? UP : DOWN;

    m_pDynamicTexCom->Set_Texture(iDir);
}

void CBossDesert::CheckWall(const _float fTimeDelta)
{
    if (m_eState == DEAD) return;
    if (m_eState == TURN && m_pMovement->Get_IsTurn() == false)
        m_eState = IDLE;

    if (m_eState == TURN || m_eState == DEAD || m_eState == LIGHT) return;

    if ((m_pUpChecker->Is_Wall() && m_pLeftChecker->Is_Wall()) ||   // 위로가며 우회전
        (m_pDownChecker->Is_Wall() && m_pRightChecker->Is_Wall()))  // 아래가며 좌회전
    {
        m_eState = TURN;
        m_pMovement->Turn(D3DXToRadian(90.f));
        return;
    }
    else if ((m_pUpChecker->Is_Wall() && m_pRightChecker->Is_Wall()) || // 위로가며  좌회전
        (m_pDownChecker->Is_Wall() && m_pLeftChecker->Is_Wall()))       // 아래가며 우회전
    {
        m_eState = TURN;
        m_pMovement->Turn(D3DXToRadian(-90.f));
        return;
    }

    _vec3 vPos = m_pMovement->Get_Dir();
    vPos = { roundf(vPos.x), 0.f, roundf(vPos.z) };
    _bool IsLeftWall = vPos.x >= 0.f && m_pRightChecker->Is_Wall();
    _bool IsRightWall = vPos.x <= 0.f && m_pLeftChecker->Is_Wall();
    _bool IsUpWall =  vPos.z >= 0.f && m_pUpChecker->Is_Wall();
    _bool IsDownWall =  vPos.z <= 0.f && m_pDownChecker->Is_Wall();

    _int i = Get_Int_Random<_int>(0, 1);
    if (IsLeftWall || IsRightWall || IsUpWall || IsDownWall)
    {
        if (i) m_pMovement->Turn(D3DXToRadian(90.f));
        else m_pMovement->Turn(D3DXToRadian(45.f));
        m_eState = TURN;
        return;
    }
}

void CBossDesert::Attack(const _float fTimeDelta)
{
    if (m_eState == LIGHT || m_eState == DEAD) return;
    
    m_fAttackDelta -= fTimeDelta;

    if (m_fAttackDelta >= 0.f) return;
    if (m_vecNode.empty())
    {
        m_fAttackDelta = n_fAttackCool;
        return;
    }
        

    for (auto& pNode : m_vecNode)
    {
        if (pNode->Get_IsReady() == false)
            return;
    }

#ifdef _DEBUG
    if (!m_bDemoEnd)
    {
        if (m_iDemoPatternIdx == 0)
        {
            //Body Light
            m_eState = LIGHT;
            for (auto pNode : m_vecNode)
                pNode->OnLight_Attack();
            m_pMovement->Set_IsMove(false);
            CSoundManager::GetInstance()->StopSound(CHANNEL_6);
        }
        else
        {
            // Body Attack
            m_vecNode[m_iDemoPatternIdx - 1]->OnAttack();
        }

        if (m_iDemoPatternIdx == 4)
            m_bDemoEnd = true;

        ++m_iDemoPatternIdx;
        return;
    }
#endif

    size_t i = m_vecNode.size() + n_fAttackCnt - 1;
    _int iPattern = Get_Int_Random<_int>(0, (_int)i);

    if (iPattern == 0)
    {
        //Body Light
        m_eState = LIGHT;
        for (auto pNode : m_vecNode)
            pNode->OnLight_Attack();
        m_pMovement->Set_IsMove(false);
        CSoundManager::GetInstance()->StopSound(CHANNEL_6);
    }
    else
    {
        // Body Attack
        m_vecNode[iPattern - 1]->OnAttack();
    }

    m_fAttackDelta = n_fAttackCool;
}

void CBossDesert::Check_LightAttack(const _float fTimeDelta)
{
    if (m_eState == DEAD) return;
    if (m_eState != LIGHT) return;

    m_fLightDelta -= fTimeDelta;

    if (m_fLightDelta >= 0.f) return;
    m_eState = IDLE;

    for (auto pNode : m_vecNode)
        pNode->OffLight_Attack();
    m_pMovement->Set_IsMove(true);

    m_fLightDelta = n_fLightCool;
    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_head_idle.wav", CHANNEL_6, 0.2f);
}

void CBossDesert::Dead(const _float fTimeDelta)
{
    if (m_eState != DEAD) return;
    if (m_pDynamicTexCom->Get_CurFrame() < 45) return;
    if (m_fDeadElapsedTime >= 4.f) return;

    m_fDeadElapsedTime += fTimeDelta;
    _vec3 vPos = m_pTransformCom->Get_Pos();
    vPos.y -= fTimeDelta * 0.4f;
    m_pTransformCom->Set_Pos(vPos);
}

void CBossDesert::Hurting(const _float fTimeDelta)
{
    if (m_fStateElapsedTime > m_fTotalHurtTime * 0.6f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255); // 흰
    else if (m_fStateElapsedTime > m_fTotalHurtTime * 0.4f)
        m_tColor = D3DCOLOR_ARGB(255, 255, 0, 0);     // 빨
    else
        m_tColor = D3DCOLOR_ARGB(255, 255, 255, 255); // 흰

    CMonster::hurting(fTimeDelta);
}

HRESULT CBossDesert::Ready_BodyBlock()
{

    CLayer* pLayer = CLayerHelper::GetInstance()->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(MONSTER_LAYER));

    CBoss3Body* pObj = CBoss3CircleBody::Create(m_pGraphicDevice, this);
    if (FAILED(pLayer->Add_GameObject(L"Cir_Body", pObj)))
        return E_FAIL;
    pObj->Get_Trans()->Set_Pos({ 34.f, 2.f, 16.f });
    m_vecNode.push_back(pObj);

    pObj = CBoss3DiamondBody::Create(m_pGraphicDevice, this);
    if (FAILED(pLayer->Add_GameObject(L"Dia_Body", pObj)))
        return E_FAIL;
    pObj->Get_Trans()->Set_Pos({ 32.5f, 2.f, 19.f });
    m_vecNode.push_back(pObj);

    pObj = CBoss3SquareBody::Create(m_pGraphicDevice, this);
    if (FAILED(pLayer->Add_GameObject(L"Sqr_Body", pObj)))
        return E_FAIL;
    pObj->Get_Trans()->Set_Pos({ 28.5f, 2.f, 19.f });
    m_vecNode.push_back(pObj);

    pObj = CBoss3TriangleBody::Create(m_pGraphicDevice, this);
    if (FAILED(pLayer->Add_GameObject(L"Sqr_Body", pObj)))
        return E_FAIL;
    pObj->Get_Trans()->Set_Pos({ 26.f, 2.2f, 16.f });
    m_vecNode.push_back(pObj);

    CRenderObject* pCover = CBoss3IntroCover::Create(m_pGraphicDevice);
    if (FAILED(pLayer->Add_GameObject(L"Intro_Cover", pCover)))
        return E_FAIL;
    pCover->Get_Trans()->Set_Pos({ 30.f, 6.f, 20.f });
    m_pIntroCover = pCover;

    Init_GroupHP(); // 모든 머리,몸체 그룹으로 체력 통합
    Init_TrainTrail();

    return S_OK;
}

HRESULT CBossDesert::Ready_WallChecker()
{
    CLayer* pLayer = CLayerHelper::GetInstance()->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(DEFAULT_LAYER0));

    if (m_pUpChecker = CWallChecker::Create(m_pGraphicDevice, this))
    {
        m_pUpChecker->Set_Offset({ 0.f, 0.f, 5.f });
        m_pUpChecker->Set_Dimension({ 5.2f, 1.f, 1.5f });
        pLayer->Add_GameObject(L"Front_Checker", m_pUpChecker);
    }
    else return E_FAIL;

    if (m_pDownChecker = CWallChecker::Create(m_pGraphicDevice, this))
    {
        m_pDownChecker->Set_Offset({ 0.f, 0.f, -5.f });
        m_pDownChecker->Set_Dimension({ 5.2f, 1.f, 1.5f });
        pLayer->Add_GameObject(L"Down_Checker", m_pDownChecker);
    }
    else return E_FAIL;

    if (m_pLeftChecker = CWallChecker::Create(m_pGraphicDevice, this))
    {
        m_pLeftChecker->Set_Offset({ -5.f, 0.f, 0.f });
        m_pLeftChecker->Set_Dimension({ 1.5f, 1.f, 5.2f });
        pLayer->Add_GameObject(L"Left_Checker", m_pLeftChecker);
    }
    else return E_FAIL;

    if (m_pRightChecker = CWallChecker::Create(m_pGraphicDevice, this))
    {
        m_pRightChecker->Set_Offset({ 5.f, 0.f, 0.f });
        m_pRightChecker->Set_Dimension({ 1.5f, 1.f, 5.2f });
        pLayer->Add_GameObject(L"Right_Checker", m_pRightChecker);
    }
    else return E_FAIL;

    return S_OK;
}

#pragma region Trail
int CBossDesert::PointsPerSegment() const
{
    _int pps = (_int)round(m_fBodySpacing / m_fRecordStep);
    return (pps < 1) ? 1 : pps;
}

void CBossDesert::Init_TrainTrail()
{
    m_deqHeadTrail.clear();
    m_deqHeadTrail.push_front(m_pTransformCom->Get_Pos());
}

void CBossDesert::Record_HeadTrail()
{
    if (m_eState == LIGHT || m_eState == DEAD) return;

    _vec3 vPos = m_pTransformCom->Get_Pos();

    if (m_deqHeadTrail.empty())
    {
        m_deqHeadTrail.push_front(vPos);
        return;
    }

    _vec3 vDest = vPos - m_deqHeadTrail.front();
    vDest.y = 0.f; 

    if (D3DXVec3LengthSq(&vDest) >= m_fRecordStep * m_fRecordStep)
        m_deqHeadTrail.push_front(vPos);

    const _int maxPoints = ((_int)m_vecNode.size() + 1) * PointsPerSegment() + 64;
    while ((_int)m_deqHeadTrail.size() > maxPoints)
        m_deqHeadTrail.pop_back();
}

void CBossDesert::Apply_TrainBody(const _float fTimeDelta)
{
    if (m_eState == LIGHT || m_eState == DEAD) return;
    if (m_vecNode.empty() || m_deqHeadTrail.empty())
        return;

    const int pps = PointsPerSegment();
    if ((int)m_deqHeadTrail.size() <= pps)
        return;

    // 바디 이동 속도
    const _float fSpeed = m_pMovement->Get_Speed();
    const _float baseSpeed = fSpeed * 1.2f;   // 기본 추종 속도
    const _float catchMul = 2.5f;              // 멀리 떨어지면 가속 배수
    const _float maxSpeed = fSpeed * 6.f;    // 최대 속도 캡(너무 빠른 스냅 방지)

    for (size_t i = 0; i < m_vecNode.size(); ++i)
    {
        CBoss3Body* pBody = m_vecNode[i];
        if (!pBody) continue;

        const int idx = (int)((i + 1) * pps);

        //루트가 아직 부족하면 넘어감
        if (idx >= (int)m_deqHeadTrail.size())
            continue;

        _vec3 vTarget = m_deqHeadTrail[idx];        

        _vec3 vCurBodyPos = pBody->Get_Trans()->Get_Pos();
        vTarget.y = vCurBodyPos.y;

        _vec3 vDir = vTarget - vCurBodyPos;
        vDir.y = 0.f;

        _float fDistSq = D3DXVec3LengthSq(&vDir);
        _float fDist = sqrtf(fDistSq);

        _float speed = baseSpeed;
        if (fDist > m_fBodySpacing * 2.f)
            speed *= catchMul;
        if (speed > maxSpeed) speed = maxSpeed;

        _float maxStep = speed * fTimeDelta;

        if (fDist > maxStep)
        {
            D3DXVec3Normalize(&vDir, &vDir);
            vCurBodyPos += vDir * maxStep;
            pBody->Get_Trans()->Set_Pos(vCurBodyPos);
        }
        else
        {
            pBody->Get_Trans()->Set_Pos(vTarget);
        }
    }
}
#pragma endregion


CBossDesert* CBossDesert::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CBossDesert* pInstance = new CBossDesert(pGraphicDev);

    if (FAILED(pInstance->Ready_GameObject()))
    {
        MSG_BOX("CBoss3 Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}

void CBossDesert::Free()
{
    Engine::CRenderObject::Free();

    m_pHPBar->Release();
    m_pDamageText->Release();
}

#ifdef _DEBUG
void CBossDesert::Add_EditorFiled()
{
    CGameObject::Add_EditorField("HP", DATATYPE::DATA_FLOAT, &debug_fHP);
    CGameObject::Add_EditorField("ATTACK", DATATYPE::DATA_FLOAT, &debug_fAttack);
    CGameObject::Add_EditorField("DEFENSE", DATATYPE::DATA_FLOAT, &debug_fDefense);
    CGameObject::Add_EditorField("Speed", DATATYPE::DATA_FLOAT, &debug_fSpeed);
}
void CBossDesert::Display_Debug_Param()
{
    debug_fHP = m_pCombatStats->Get_HealthStat()->Get_CurValue();
    debug_fAttack = m_pCombatStats->Get_AttackStat()->Get_CurValue();
    debug_fDefense = m_pCombatStats->Get_DefenseStat()->Get_CurValue();
    debug_fSpeed = 0.f;
}
#endif 
