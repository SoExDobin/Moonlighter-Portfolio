#include "pch.h"
#include "CBossDesertProjectile.h"

CBossDesertProjectile::CBossDesertProjectile(LPDIRECT3DDEVICE9 pGraphicDev)
    : CProjectile(pGraphicDev)
{
}

CBossDesertProjectile::CBossDesertProjectile(const CProjectile& rhs)
    : CProjectile(rhs)
{
}

CBossDesertProjectile::~CBossDesertProjectile()
{
}

HRESULT CBossDesertProjectile::Ready_GameObject()
{
    CProjectile::Ready_GameObject();

    m_iObjectID = OBJECT_ID::MONSTER_ATK;

    m_pTransformCom->Set_Scale({ 2.f, 2.f, 2.f });


    m_fMaxLifeTime = 3.f;

    m_pDynamicTexCom->Ready_Texture(L"Boss3_Projectile1");

    return S_OK;
}

_int CBossDesertProjectile::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    return iExit;
}

void CBossDesertProjectile::LateUpdate_GameObject(const _float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);
}

void CBossDesertProjectile::Render_GameObject()
{
    CProjectile::Render_GameObject();
}

void CBossDesertProjectile::On_Collision(const Collision& tCollision)
{
    if (tCollision.pColTarget->Get_ObjectID() == PLAYER)
    {
        int a = 0;
    }
}

void CBossDesertProjectile::Ready_Shoot(const _float& fTimeDelta)
{
    m_bReadyShoot = true;
    m_bShooting = m_bHasImpacted = false;
}


void CBossDesertProjectile::Shooting(const _float& fTimeDelta)
{
    m_pTransformCom->Move_Pos(&m_vShootDir, fTimeDelta, m_tInfo.fSpeed);

}

void CBossDesertProjectile::Post_Impact(const _float& fTimeDelta)
{
}

CBossDesertProjectile* CBossDesertProjectile::Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo)
{
    CBossDesertProjectile* pProjectile = new CBossDesertProjectile(pGraphicDev);

    pProjectile->Set_ProjectileInfo(tInfo);
    if (FAILED(pProjectile->Ready_GameObject()))
    {
        Safe_Release(pProjectile);
        MSG_BOX("Create Projectile Failed");
        return nullptr;
    }

    return pProjectile;
}


void CBossDesertProjectile::Free()
{
    CRenderObject::Free();
}
