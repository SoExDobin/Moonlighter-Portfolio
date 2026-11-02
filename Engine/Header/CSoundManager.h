#pragma once

#include "CBase.h"
#include "Engine_Define.h"

BEGIN(Engine)

class ENGINE_DLL CSoundManager : public CBase
{
    DECLARE_SINGLETON(CSoundManager)
private:
    explicit CSoundManager();
    virtual ~CSoundManager() override;

public:
    HRESULT     Ready_SoundManager();

    void        PlaySoundOnce(const wstring& wsSoundKey, CHANNELID eID, float fVolume);
    void        PlaySoundFX(const wstring& wsSoundKey, CHANNELID eID, float fVolume);
    void        PlayBGM(const wstring& wsSoundKey, float fVolume);
    void        StopSound(CHANNELID eID);
    void        StopAll();
    void        SetChannelVolume(CHANNELID eID, float fVolume);
    void        LoadSoundFile(const wstring& wsPath);
    void        PlaySoundLoopSection(const wstring& wsSoundKey,
                                     CHANNELID eID,
                                     float fVolume,
                                     unsigned int loopStartMs,
                                     unsigned int loopEndMs,
                                     bool bPlayIntro);
public:
    void       LoadMp3SoundFile(const wstring& wsPath);

private:
    unordered_map<wstring, FMOD_SOUND*> m_umapSound;
    FMOD_CHANNEL* m_pChannelArr[Engine::MAXCHANNEL];
    FMOD_SYSTEM* m_pSystem;

private:
    void        Free() override;
};

END
