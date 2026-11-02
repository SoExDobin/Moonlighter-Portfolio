#include "CFont.h"

CFont::CFont(LPDIRECT3DDEVICE9 pGraphicDev)
	: m_pGraphicDev(pGraphicDev), m_pSprite(nullptr), m_pFont(nullptr)
{
	m_pGraphicDev->AddRef();
}

CFont::~CFont()
{
}

HRESULT CFont::Ready_Font(const wstring& pFontType, _uint iWidth, _uint iHeight, _uint iWeight)
{
	D3DXFONT_DESC			tFont_Desc;
	ZeroMemory(&tFont_Desc, sizeof(D3DXFONT_DESC));

	tFont_Desc.CharSet = HANGUL_CHARSET;
	tFont_Desc.Width = iWidth;
	tFont_Desc.Height = iHeight;
	tFont_Desc.Weight = iWeight;
	lstrcpy(tFont_Desc.FaceName, pFontType.c_str());

	if (FAILED(D3DXCreateFontIndirect(m_pGraphicDev, &tFont_Desc, &m_pFont)))
	{
		MSG_BOX("Font Create Failed");
		return E_FAIL;
	}

	if (FAILED(D3DXCreateSprite(m_pGraphicDev, &m_pSprite)))
	{
		MSG_BOX("Sprite Create Failed");
		return E_FAIL;
	}
    printf("[Font Ready] Font=%p, Sprite=%p\n", m_pFont, m_pSprite);


	return S_OK;
}

void CFont::Render_Font(const wstring& wsString, const _vec2* pPos, D3DXCOLOR Color , DWORD dwFormat)
{
    if (!m_pSprite || !m_pFont || !pPos)
        return;

    RECT rc{};
    const LONG maxWidth = 100; 
    const LONG maxHeight = 50;

    // 정렬 타입에 따른 기준좌표 설정
    // vpos로 값 설정해서 그렇다. rect로 받아오면 이렇게 안해도 됨
    if (dwFormat & DT_RIGHT)
    {
        rc.right = (LONG)pPos->x;
        rc.left = rc.right - maxWidth;
    }
    else if (dwFormat & DT_CENTER)
    {
        rc.left = (LONG)(pPos->x - maxWidth / 2);
        rc.right = (LONG)(pPos->x + maxWidth / 2);
    }
    else
    {
        rc.left = (LONG)pPos->x;
        rc.right = rc.left + maxWidth;
    }

    rc.bottom = (LONG)pPos->y;
    rc.top = rc.bottom - maxHeight;


	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_pFont->DrawTextW(m_pSprite, wsString.c_str(), -1, &rc, dwFormat, Color); //lstrlen(wsString.c_str()) -> -1


	m_pSprite->End();
}


CFont* CFont::Create(LPDIRECT3DDEVICE9 pGraphicDev, const wstring& pFontType, _uint iWidth, _uint iHeight, _uint iWeight)
{
	CFont* pFont = new CFont(pGraphicDev);

	if (FAILED(pFont->Ready_Font(pFontType, iWidth, iHeight, iWeight)))
	{
		Safe_Release(pFont);
		MSG_BOX("Font Create Failed");
		return nullptr;
	}

	return pFont;
}

void CFont::Free()
{
}
