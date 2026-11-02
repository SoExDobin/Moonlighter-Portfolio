#pragma once

#include "CBase.h"
#include "CScene.h"

BEGIN(Engine)

class CGameObject;

class ENGINE_DLL CManagement : public CBase
{
	DECLARE_SINGLETON(CManagement)

private:
	explicit CManagement();
	virtual	~CManagement() override;

public:
    CGameObject*               Get_Object(const wstring& wsLayerTag, const wstring& wsObjTag);
    CGameObject*               Get_Object(const wstring& wsObjTag);
    const list<CGameObject*>*  Get_Object_List(const wstring& wsObjTag);
    CScene*                    Get_CurScene() const { return m_pCurScene; }

	CComponent*		Get_Component(COMPONENTID eID,
							const wstring& wsLayerTag,
							const wstring& wsObjTag,
							const wstring& wsComponentTag);

public:
    void            Add_CacheObject(CGameObject* pCacheObject);
    void            Add_PendingEvent(function<void()> funcEventArg);
    void            Load_CacheObject();
private:
    void            Release_PendingEvent();

public:
	HRESULT			Set_Scene(CScene* pScene);
    HRESULT         Init_Scene(CScene* pScene);


public:
	_int			Update_Scene(const _float fTimeDelta);
	void			LateUpdate_Scene(const _float fTimeDelta);
	void			Render_Scene(LPDIRECT3DDEVICE9 pGraphicDev);

private:
    _bool                                   m_bIsInit;
	CScene*				                    m_pCurScene;
	CScene*				                    m_pNextScene; // changeScene용
    unordered_set<CGameObject*>             m_usetCacheObject;
    vector<function<void()>>                m_vecPendingEvent;
	
public:
	virtual void			Free() override;
};

END
