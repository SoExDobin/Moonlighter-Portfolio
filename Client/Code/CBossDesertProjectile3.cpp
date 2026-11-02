#include "pch.h"
#include "CBossDesertProjectile3.h"
#include "CBossDesertLavaField.h"
#include "CLayerHelper.h"
#include "CDataManager.h"
#include "CFireMob.h"

CBossDesertProjectile3::CBossDesertProjectile3(LPDIRECT3DDEVICE9 pGraphicDev)
    : CProjectile(pGraphicDev), m_fSpeed(0.f), m_bOutofField(false)
{
}

CBossDesertProjectile3::CBossDesertProjectile3(const CProjectile& rhs)
    : CProjectile(rhs), m_fSpeed(0.f), m_bOutofField(false)
{
}

CBossDesertProjectile3::~CBossDesertProjectile3()
{
}

HRESULT CBossDesertProjectile3::Ready_GameObject()
{
    CProjectile::Ready_GameObject();

    m_iObjectID = OBJECT_ID::ID_END;

    m_pTransformCom->Set_Scale({ 2.f, 2.f, 2.f });

    m_fMaxLifeTime = 2.f;

    m_pDynamicTexCom->Ready_Texture(L"Boss3_Projectile3");

    m_fElapsedLifeTime = 0.f;

    return S_OK;
}

_int CBossDesertProjectile3::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    return iExit;
}

void CBossDesertProjectile3::LateUpdate_GameObject(const _float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);

    if (m_bOutofField) return;
    if (m_fElapsedLifeTime >= m_fMaxLifeTime)
    {
        CRenderObject* pObj = CFireMob::Create(m_pGraphicDevice);

        _vec3 vInstantPos = m_pTransformCom->Get_Pos();
        pObj->Get_Trans()->Set_Pos(vInstantPos);

        CLayer* pLayer = Engine::CLayerHelper::GetInstance()->Get_Layer(
            CDataManager::GetInstance()->Get_LayerTag(MONSTER_LAYER));
        pLayer->Add_GameObject(L"Fire_Mob", pObj);
    }
}

void CBossDesertProjectile3::Render_GameObject()
{
    CProjectile::Render_GameObject();
}

void CBossDesertProjectile3::On_Collision(const Collision& tCollision)
{
    if (tCollision.pColTarget->Get_ObjectID() == ENVIRONMENT)
    {
        Destroy(this);
        m_bOutofField = true;
    }
}

void CBossDesertProjectile3::Ready_Shoot(const _float& fTimeDelta)
{
    m_bReadyShoot = true;
    m_bShooting = m_bHasImpacted = false;
}


void CBossDesertProjectile3::Shooting(const _float& fTimeDelta)
{
    _vec3 vPos = m_pTransformCom->Get_Pos();
    vPos += m_vShootDir * fTimeDelta * m_fSpeed;
    m_pTransformCom->Set_Pos(vPos);
}

void CBossDesertProjectile3::Post_Impact(const _float& fTimeDelta)
{
}

CBossDesertProjectile3* CBossDesertProjectile3::Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo)
{
    CBossDesertProjectile3* pProjectile = new CBossDesertProjectile3(pGraphicDev);

    pProjectile->Set_ProjectileInfo(tInfo);
    if (FAILED(pProjectile->Ready_GameObject()))
    {
        Safe_Release(pProjectile);
        MSG_BOX("Create Projectile Failed");
        return nullptr;
    }

    return pProjectile;
}


void CBossDesertProjectile3::Free()
{
    CRenderObject::Free();
}
