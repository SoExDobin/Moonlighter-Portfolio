#include "pch.h"
#include "CBossDesertLavaField.h"
#include "CSoundManager.h"

CBossDesertLavaField::CBossDesertLavaField(LPDIRECT3DDEVICE9 pGraphicDev)
    : CProjectile(pGraphicDev)
{
}

CBossDesertLavaField::CBossDesertLavaField(const CProjectile& rhs)
    : CProjectile(rhs)
{
}

CBossDesertLavaField::~CBossDesertLavaField()
{
}

HRESULT CBossDesertLavaField::Ready_GameObject()
{
    CProjectile::Ready_GameObject();

    m_iObjectID = OBJECT_ID::MONSTER_ATK;

    m_pTransformCom->Set_Scale({ 4.f, 4.f, 4.f });

    m_fMaxLifeTime = 10.f;

    m_pDynamicTexCom->Ready_Texture(L"Boss3_Projectile2_Field");
    m_pDynamicTexCom->Set_Texture(0);
    m_pDynamicTexCom->Set_Loop(false);

    CSoundManager::GetInstance()->PlaySoundOnce(L"desert_dungeon_boss_lava_spawn.wav", CHANNEL_10, 0.2f);

    return S_OK;
}

_int CBossDesertLavaField::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CProjectile::Update_GameObject(fTimeDelta);

    return iExit;
}

void CBossDesertLavaField::LateUpdate_GameObject(const _float fTimeDelta)
{
    CProjectile::LateUpdate_GameObject(fTimeDelta);
}

void CBossDesertLavaField::Render_GameObject()
{
    CProjectile::Render_GameObject();
}

void CBossDesertLavaField::On_Collision(const Collision& tCollision)
{

}

void CBossDesertLavaField::Ready_Shoot(const _float& fTimeDelta)
{
    m_bReadyShoot = true;
    m_bShooting = m_bHasImpacted = false;
}


void CBossDesertLavaField::Shooting(const _float& fTimeDelta)
{
}

void CBossDesertLavaField::Post_Impact(const _float& fTimeDelta)
{
}

CBossDesertLavaField* CBossDesertLavaField::Create(LPDIRECT3DDEVICE9 pGraphicDev, PROJECTILE tInfo)
{
    CBossDesertLavaField* pProjectile = new CBossDesertLavaField(pGraphicDev);

    pProjectile->Set_ProjectileInfo(tInfo);
    if (FAILED(pProjectile->Ready_GameObject()))
    {
        Safe_Release(pProjectile);
        MSG_BOX("Create Projectile Failed");
        return nullptr;
    }

    return pProjectile;
}


void CBossDesertLavaField::Free()
{
    CRenderObject::Free();
}
