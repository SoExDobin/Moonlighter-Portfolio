#include "pch.h"
#include "CBossDesertProjectile2.h"
#include "CBossDesertLavaField.h"
#include "CLayerHelper.h"
#include "CDataManager.h"
#include "CSoundManager.h"

CBossDesertProjectile2::CBossDesertProjectile2(LPDIRECT3DDEVICE9 pGraphicDev)
    : CProjectile(pGraphicDev), m_fAngleY(0.f), m_fArcHeight(0.f)
{
    ZeroMemory(&m_vTargetPos, sizeof(_vec3));
    ZeroMemory(&m_vStartPos, sizeof(_vec3));
}

CBossDesertProjectile2::CBossDesertProjectile2(const CProjectile& rhs)
    : CProjectile(rhs), m_fAngleY(0.f), m_fArcHeight(0.f)
{
    ZeroMemory(&m_vTargetPos, sizeof(_vec3));
    ZeroMemory(&m_vStartPos, sizeof(_vec3));
}

CBossDesertProjectile2::~CBossDesertProjectile2()
{
}

HRESULT CBossDesertProjectile2::Ready_GameObject()
{
    CProjectile::Ready_GameObject();

    m_iObjectID = OBJECT_ID::MONSTER_ATK;

    m_pTransformCom->Set_Scale({ 3.f, 3.f, 3.f });

    m_fMaxLifeTime = 2.5f;

    m_pDynamicTexCom->Ready_Texture(L"Boss3_Projectile2");

    m_fElapsedLifeTime = 0.f;

    {
        // 투사체 정보 설정 
        m_tProj.fTextureSpeed = 5.f;
        m_tProj.fSpeed = 7.5f;
        m_tProj.fScale = 1.f;
        m_tProj.eColType = COL_TYPE::SPHERE_COL;
        m_tProj.dwCurFlag = OBJECT_ID::MONSTER_ATK;
        m_tProj.dwHitTargetFlag = OBJECT_ID::PLAYER;
        m_tProj.dwDestroyOnHitFlag
            = OBJECT_ID::PLAYER_ATK | OBJECT_ID::INTERACTABLE | OBJECT_ID::ENVIRONMENT;

        // 데미지 정보 설정 
        m_tDmgInfo.pAttacker = this;
        m_tDmgInfo.dwHitTargetFlag = m_tProj.dwHitTargetFlag;
        m_tDmgInfo.fAmount = 8.f;
        m_tDmgInfo.bCanParry = false;
        m_tDmgInfo.bShouldKnockback = true;
        m_tDmgInfo.eApplyTiming = Engine::STAY_COL;
        m_tDmgInfo.vDirKnockback = { 0.f, 0.f, 0.f };

        m_tProj.tDamageInfo = m_tDmgInfo;
    }

    return S_OK;
}

_int CBossDesertProjectile2::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    return iExit;
}

void CBossDesertProjectile2::LateUpdate_GameObject(const _float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);
    if (m_fElapsedLifeTime >= m_fMaxLifeTime)
    {
        CBossDesertLavaField* pProj = CBossDesertLavaField::Create(m_pGraphicDevice, m_tProj);
        _vec3 vPos = m_pTransformCom->Get_Pos();
        vPos.y = 0.75f;
        pProj->Set_SpawnPos(vPos);
        pProj->Set_Shooting();

        if (FAILED(CLayerHelper::GetInstance()
            ->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(MONSTER_ATK_LAYER))
            ->Add_GameObject(L"Lava_Field_Projectile", pProj)))
            MSG_BOX("Create Lava_Field_Projectile Projectile Failed");
    }
}

void CBossDesertProjectile2::Render_GameObject()
{
    CProjectile::Render_GameObject();
}

void CBossDesertProjectile2::On_Collision(const Collision& tCollision)
{
    if (tCollision.pColTarget->Get_ObjectID() == PLAYER)
    {
        int a = 0;
    }
}

void CBossDesertProjectile2::Ready_Shoot(const _float& fTimeDelta)
{
    m_bReadyShoot = true;
    m_bShooting = m_bHasImpacted = false;
}


void CBossDesertProjectile2::Shooting(const _float& fTimeDelta)
{
    _float fElapsedDelta = m_fElapsedLifeTime / m_fMaxLifeTime;
    if (fElapsedDelta > 1.f) fElapsedDelta = 1.f;

    // 선형 보간
    _vec3 vPos = m_vStartPos + (m_vTargetPos - m_vStartPos) * fElapsedDelta;
    vPos.y += sinf(D3DX_PI * fElapsedDelta) * m_fArcHeight;

    m_pTransformCom->Set_Pos(vPos);
}

void CBossDesertProjectile2::Post_Impact(const _float& fTimeDelta)
{
}

CBossDesertProjectile2* CBossDesertProjectile2::Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo)
{
    CBossDesertProjectile2* pProjectile = new CBossDesertProjectile2(pGraphicDev);

    pProjectile->Set_ProjectileInfo(tInfo);
    if (FAILED(pProjectile->Ready_GameObject()))
    {
        Safe_Release(pProjectile);
        MSG_BOX("Create Projectile Failed");
        return nullptr;
    }

    return pProjectile;
}


void CBossDesertProjectile2::Free()
{
    CRenderObject::Free();
}
