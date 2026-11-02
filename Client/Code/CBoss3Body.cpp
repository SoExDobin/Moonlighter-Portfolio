#include "pch.h"
#include "CBoss3Body.h"
#include "CWallChecker.h"
#include "CLayerHelper.h"
#include "CDataManager.h"
#include "CDamageZone.h"

CBoss3Body::CBoss3Body(LPDIRECT3DDEVICE9 pGraphicDev)
    : CMonster(pGraphicDev), m_pDynamicTexCom(nullptr), m_fSpeed(0.f)
{
}

CBoss3Body::~CBoss3Body()
{
}

void CBoss3Body::Ready_EntityComponent() {}

void CBoss3Body::Configure_Component() {}

HRESULT CBoss3Body::Ready_Body(CBossDesert* pHead)
{
    CRenderObject::Ready_GameObject();

    m_pHead = pHead;
    m_iObjectID = OBJECT_ID::MONSTER;

    CLayer* pLayer = CLayerHelper::GetInstance()->Get_Layer(CDataManager::GetInstance()->Get_LayerTag(DEFAULT_LAYER0));

    if (m_pWallChecker = CWallChecker::Create(m_pGraphicDevice, this))
    {
        m_pWallChecker->Set_Dimension({ 2.5f, 2.5f, 2.5f });
        pLayer->Add_GameObject(L"Wall_Checker", m_pWallChecker);
    }
    else return E_FAIL;

    m_tDamage.bCanParry = false;
    m_tDamage.bShouldKnockback = true;
    m_tDamage.vDirKnockback = { 0.f, 0.f, 0.f };
    m_tDamage.dwHitTargetFlag = OBJECT_ID::PLAYER;
    m_tDamage.eApplyTiming = COL_STATE::ENTER_COL;
    m_tDamage.fAmount = 20.f;
    m_tDamage.pAttacker = this;

    if (m_pDamageZone = CDamageZone::Create(m_pGraphicDevice, this, HIT_RECTBOX, m_tDamage))
    {
        m_pDamageZone->Activate_DamageZone(false);
        m_pDamageZone->Set_Position(m_pTransformCom->Get_Pos());
        static_cast<CHitRectBox*>(m_pDamageZone->Get_Hitbox())
            ->Set_Dimension({ 5.f, 5.f, 5.f, });
        pLayer->Add_GameObject(L"Damage_Zone", m_pDamageZone);
    }
    else return E_FAIL;

    return S_OK;
}
void CBoss3Body::Free()
{
    CMonster::Free();
}
