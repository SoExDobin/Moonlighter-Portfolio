#include "pch.h"
#include "CTrigger.h"
#include "CPrototypeManager.h"

CTrigger::CTrigger(LPDIRECT3DDEVICE9 pGraphicDev)
    : CNonRenderObject(pGraphicDev)
{
}

CTrigger::CTrigger(const CTrigger& rhs)
    : CNonRenderObject(rhs)
{
}

CTrigger::~CTrigger()
{
}

void CTrigger::Add_Event(function<void()> funcEvent)
{
    m_vecEvent.push_back(funcEvent);
}

void CTrigger::Remove_Event(function<void()> funcEvent)
{
    auto iter = find_if(m_vecEvent.begin(), m_vecEvent.end(),
        [&funcEvent](function<void()>& e) -> _bool {
            return e.target_type() == funcEvent.target_type();
        });

    if (iter == m_vecEvent.end()) return;

    m_vecEvent.erase(iter);
}

HRESULT CTrigger::Ready_Trigger()
{
    CNonRenderObject::Ready_GameObject();

    if (m_pColCom = Add_Component<CRectCollider>(ID_DYNAMIC, L"Collider_Com", RECT_COLLIDER))
    {
        m_pColCom->Set_Dimension(_vec3{ 2.f, 1.f, 2.f });
        m_pColCom->Set_Offset(_vec3{ 0.f, 3.f, 0.f });
    }
    else return E_FAIL;

    return S_OK;
}

_int CTrigger::Update_GameObject(const _float fTimeDelta)
{
    _int iExit = CNonRenderObject::Update_GameObject(fTimeDelta);

    return iExit;
}

void CTrigger::LateUpdate_GameObject(const _float fTimeDelta)
{
    CNonRenderObject::LateUpdate_GameObject(fTimeDelta);
}

void CTrigger::On_Collision(const Collision& tCollision)
{

    if (tCollision.eColState == ENTER_COL && tCollision.pColTarget->Get_ObjectID() & m_eTargetID)
    {
        printf("충돌 : %d\n", tCollision.pColTarget->Get_ObjectID());
        for_each(m_vecEvent.begin(), m_vecEvent.end(),
            [](function<void()> e) -> void {
                e();
            });
    }



    // 오브젝트 배치 // 던전 Obstacle, 샵 테이블, 아이템 테이블
    
    // 벽 충돌 콜라이더 
    // 샵

    // 리소스 마충돌 X

    // 샵


    // 플레이어 만?
    // NPC 만?
}

CTrigger* CTrigger::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
    CTrigger* pInstance = new CTrigger(pGraphicDev);

    if (FAILED(pInstance->Ready_Trigger()))
    {
        MSG_BOX("CTrigger Create Failed");
        Safe_Release(pInstance);
        return nullptr;
    }

    return pInstance;
}


void CTrigger::Free()
{
    CNonRenderObject::Free();
}
