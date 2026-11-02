#include "CSoundManager.h"

IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
	: m_pSystem(nullptr)
{
}

CSoundManager::~CSoundManager()
{
	Free();
}

HRESULT CSoundManager::Ready_SoundManager()
{
	FMOD_System_Create(&m_pSystem, FMOD_VERSION);
	FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	return S_OK;
}

void CSoundManager::PlaySoundOnce(const wstring& wsSoundKey, CHANNELID eID, float fVolume)
{
	unordered_map<wstring, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_umapSound.begin(), m_umapSound.end(),
		[&](auto& iter)->bool
		{
			return wsSoundKey == iter.first;
		});

	if (iter == m_umapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundManager::PlaySoundFX(const wstring& wsSoundKey, CHANNELID eID, float fVolume)
{
	unordered_map<wstring, FMOD_SOUND*>::iterator iter;

	iter = find_if(m_umapSound.begin(), m_umapSound.end(),
		[&](auto& iter)->bool
		{
			return wsSoundKey == iter.first;
		});

	if (iter == m_umapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[eID]);

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundManager::PlayBGM(const wstring& wsSoundKey, float fVolume)
{
	unordered_map<wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_umapSound.begin(), m_umapSound.end(), [&](auto& iter)->bool
		{
			return wsSoundKey == iter.first;
		});

	if (iter == m_umapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[CHANNEL_0]);
	FMOD_Channel_SetMode(m_pChannelArr[CHANNEL_0], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[CHANNEL_0], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundManager::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundManager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundManager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundManager::LoadSoundFile(const wstring& wsPath)
{
    std::wstring wsBasePath = wsPath;
    if (!wsBasePath.empty())
    {
        wchar_t back = wsBasePath.back();
        if (back != L'/' && back != L'\\')
            wsBasePath += L'/';
    }

    // 검색 패턴 (*.mp3 or *.wav)
    std::wstring wsSearchPath = wsBasePath + L"*.wav";

    _wfinddata_t fd;
    intptr_t hFind = _wfindfirst(wsSearchPath.c_str(), &fd);
    if (hFind == -1)
    {
        MSG_BOX("There is no Sound File");
        return;
    }
        
    do
    {
        // 전체 경로(wchar_t)
        std::wstring wsFullPath = wsBasePath + fd.name;

        // FMOD 는 char* 경로를 받으므로 멀티바이트로 변환
        char szFullPath[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0,
                            wsFullPath.c_str(), -1,
                            szFullPath, MAX_PATH,
                            nullptr, nullptr);

        FMOD_SOUND* pSound = nullptr;
        FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem,
                                                   szFullPath,
                                                   FMOD_DEFAULT,
                                                   0,
                                                   &pSound);

        if (eRes == FMOD_OK)
        {
            // player_attack.wav < 파일 지정자 포함
            std::wstring wsKey = fd.name;
            m_umapSound.emplace(wsKey, pSound);
        }

    } while (_wfindnext(hFind, &fd) != -1);   // ★ _wfindnext

    _findclose(hFind);
    FMOD_System_Update(m_pSystem);
}

void CSoundManager::PlaySoundLoopSection(
    const wstring& wsSoundKey,
    CHANNELID eID,
    float fVolume,
    unsigned int loopStartMs,
    unsigned int loopEndMs,
    bool bPlayIntro)
{
    auto iter = m_umapSound.find(wsSoundKey);
    if (iter == m_umapSound.end())
        return;

    FMOD_SOUND* pSound = iter->second;

    // 길이 클램프 (ms)
    unsigned int lenMs = 0;
    FMOD_Sound_GetLength(pSound, &lenMs, FMOD_TIMEUNIT_MS);

    if (lenMs == 0) return;
    if (loopStartMs > lenMs) loopStartMs = lenMs;
    if (loopEndMs > lenMs) loopEndMs = lenMs;
    if (loopEndMs <= loopStartMs + 1) return;

    // 기존 재생 중이면 정지
    if (m_pChannelArr[eID])
        FMOD_Channel_Stop(m_pChannelArr[eID]);

    // 일단 PAUSED로 틀어놓고 세팅 후 풀기 (첫 프레임 튐 방지)
    FMOD_System_PlaySound(m_pSystem, pSound, nullptr, TRUE, &m_pChannelArr[eID]);

    FMOD_CHANNEL* ch = m_pChannelArr[eID];
    if (!ch) return;

    // “구간 루프”의 핵심
    FMOD_Channel_SetMode(ch, FMOD_LOOP_NORMAL);
    FMOD_Channel_SetLoopPoints(ch,
        loopStartMs, FMOD_TIMEUNIT_MS,
        loopEndMs, FMOD_TIMEUNIT_MS);

    // 인트로 없이 구간만 돌리고 싶으면 시작 위치를 loopStart로 점프
    if (!bPlayIntro)
        FMOD_Channel_SetPosition(ch, loopStartMs, FMOD_TIMEUNIT_MS);

    FMOD_Channel_SetVolume(ch, fVolume);
    FMOD_Channel_SetPaused(ch, FALSE);

    FMOD_System_Update(m_pSystem);
}

void CSoundManager::LoadMp3SoundFile(const wstring& wsPath)
{
    std::wstring wsBasePath = wsPath;
    if (!wsBasePath.empty())
    {
        wchar_t back = wsBasePath.back();
        if (back != L'/' && back != L'\\')
            wsBasePath += L'/';
    }

    std::wstring wsSearchPath = wsBasePath + L"*.mp3";

    _wfinddata_t fd;
    intptr_t hFind = _wfindfirst(wsSearchPath.c_str(), &fd);
    if (hFind == -1)
    {
        MSG_BOX("There is no Sound File");
        return;
    }

    do
    {
        // 전체 경로(wchar_t)
        std::wstring wsFullPath = wsBasePath + fd.name;

        // FMOD 는 char* 경로를 받으므로 멀티바이트로 변환
        char szFullPath[MAX_PATH] = {};
        WideCharToMultiByte(CP_ACP, 0,
            wsFullPath.c_str(), -1,
            szFullPath, MAX_PATH,
            nullptr, nullptr);

        FMOD_SOUND* pSound = nullptr;
        FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem,
            szFullPath,
            FMOD_DEFAULT,
            0,
            &pSound);

        if (eRes == FMOD_OK)
        {
            // player_attack.wav < 파일 지정자 포함
            std::wstring wsKey = fd.name;
            m_umapSound.emplace(wsKey, pSound);
        }

    } while (_wfindnext(hFind, &fd) != -1);   // ★ _wfindnext

    _findclose(hFind);
    FMOD_System_Update(m_pSystem);
}

void CSoundManager::Free()
{
	for (auto& Mypair : m_umapSound)
	{
		FMOD_Sound_Release(Mypair.second);
	}
	m_umapSound.clear();

	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
