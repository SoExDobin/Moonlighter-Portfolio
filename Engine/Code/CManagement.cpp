#include "CManagement.h"
#include "CRenderer.h"
#include "CCollisionManager.h"
#include "CLayerHelper.h"
#include "CGameObject.h"

IMPLEMENT_SINGLETON(CManagement)

CManagement::CManagement()
    : m_pCurScene(nullptr), m_bIsInit(true)
{
}

CManagement::~CManagement()
{
    Free();
}

CGameObject* CManagement::Get_Object(const wstring& wsLayerTag, const wstring& wsObjTag)
{
    LAYERID eID = CLayerHelper::GetInstance()->GetLayerIDByName(wsLayerTag);
    if (eID == LAYER_NONE)
    {
        MSG_BOX("No Such Layer At CManageMent.cpp");
        return nullptr;
    }

    auto itBegin = m_pCurScene->Get_Layers().at(eID)->Get_Objects().begin();
    auto itEnd = m_pCurScene->Get_Layers().at(eID)->Get_Objects().end();

    auto iter = find_if(itBegin, itEnd,
        [&wsObjTag](const pair<wstring, const list<CGameObject*>>& pair) -> _bool {
            if (pair.first == wsObjTag)
                return true;

            return false;
        });

    if (iter == itEnd) {

        for (auto& cache : m_usetCacheObject)
        {
            if (cache->Get_Object_LayerMask().wsObjectKey == wsObjTag
                && cache->Get_Object_LayerMask().wsLayerTag == wsLayerTag)
                return cache;
        }

        return nullptr;
    }

    return iter->second.front();
}

CGameObject* CManagement::Get_Object(const wstring& wsObjTag)
{
    if (const list<CGameObject*>* pObj = Get_Object_List(wsObjTag))
        return pObj->front();

    return nullptr;
}

const list<CGameObject*>* CManagement::Get_Object_List(const wstring& wsObjTag)
{
    const list<CGameObject*>* pList = nullptr;
    for_each(m_pCurScene->Get_Layers().begin(), m_pCurScene->Get_Layers().end(),
        [&wsObjTag, &pList](const pair<_uint16, CLayer*>& layer) -> void {

            auto iter = find_if(layer.second->Get_Objects().begin(), layer.second->Get_Objects().end(),
                [&wsObjTag, &pList](const pair<const wstring, list<CGameObject*>>& pair) -> _bool {
                    if (pair.first == wsObjTag)
                        return true;

                    return false;
                });

            if (iter != layer.second->Get_Objects().end())
            {
                pList = &(iter->second);
                return;
            }
        });

    if (pList != nullptr) return pList;

    return nullptr;
}

CComponent* CManagement::Get_Component(COMPONENTID eID, 
                                    const wstring& wsLayerTag, 
                                    const wstring& wsObjTag, 
                                    const wstring& wsComponentTag)
{
    if (nullptr == m_pCurScene)
        return nullptr;

    return m_pCurScene->Get_Component(eID, wsLayerTag, wsObjTag, wsComponentTag);
}

void CManagement::Add_CacheObject(CGameObject* pCacheObject)
{
    if (m_usetCacheObject.find(pCacheObject) == m_usetCacheObject.end())
    {
        m_usetCacheObject.emplace(pCacheObject);
    }
}

void CManagement::Add_PendingEvent(function<void()> funcEventArg)
{
    m_vecPendingEvent.push_back(funcEventArg);
}

void CManagement::Load_CacheObject()
{
    if (m_usetCacheObject.empty()) return;
        
    if (m_pCurScene == nullptr) return;

    for (auto& pObj : m_usetCacheObject)
    {
        CLayer* pTargetLayer = m_pCurScene->Get_Layer(pObj->Get_Object_LayerMask().wsLayerTag);

        pTargetLayer->Add_GameObject(pObj->Get_Object_LayerMask().wsObjectKey, pObj);
        pObj->AddRef();

        vector<CComponent*> refRectColliders;
        pObj->Get_Components(ID_DYNAMIC, RECT_COLLIDER, refRectColliders);
        if (!refRectColliders.empty())
        {
            for (auto rectCol : refRectColliders)
                CCollisionManager::GetInstance()->Add_Collider(static_cast<CCollider*>(rectCol));
        }

        vector<CComponent*> refSphereColliders;
        pObj->Get_Components(ID_DYNAMIC, SPHERE_COLLIDER, refSphereColliders);
        if (!refSphereColliders.empty())
        {
            for (auto sphereCol : refSphereColliders)
                CCollisionManager::GetInstance()->Add_Collider(static_cast<CCollider*>(sphereCol));
        }
    }
}

void CManagement::Release_PendingEvent()
{
    m_vecPendingEvent.clear();
}

HRESULT CManagement::Set_Scene(CScene* pScene)
{
    if (pScene == nullptr) return E_FAIL;

    Safe_Release(m_pCurScene);

    m_pCurScene = pScene;

    Load_CacheObject();

    return S_OK;
}

HRESULT CManagement::Init_Scene(CScene* pScene)
{
    if (pScene == nullptr) return E_FAIL;

    Safe_Release(m_pCurScene);

    m_pCurScene = pScene;

    return S_OK;
}

_int CManagement::Update_Scene(const _float fTimeDelta)
{
    if (m_pCurScene == nullptr) return -1;

    return m_pCurScene->Update_Scene(fTimeDelta);
}

void CManagement::LateUpdate_Scene(const _float fTimeDelta)
{
    if (m_pCurScene == nullptr) return;

    m_pCurScene->LateUpdate_Scene(fTimeDelta);
}

void CManagement::Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev)
{
    if (m_pCurScene == nullptr) return;

    CRenderer::GetInstance()->Render_GameObject(pGraphicDev);
    m_pCurScene->Render_Scene();

    for_each(m_vecPendingEvent.begin(), m_vecPendingEvent.end(),
        [](function<void()>& e) -> void {
            if (e) e();
        });

    Release_PendingEvent();
}

void CManagement::Free()
{
    for_each(m_usetCacheObject.begin(), m_usetCacheObject.end(),
        [](CGameObject* pObj) -> void {
            Safe_Release(pObj);
        });

    Safe_Release(m_pCurScene);
}
