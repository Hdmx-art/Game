//
// Created by Rayann CHOUCRALLAH on 27/07/2025.
//

#include "SoundManager.h"
#include <string>
#include "Config.h"

SoundManager* SoundManager::s_instance = nullptr;

SoundManager::~SoundManager() {
    MIX_Quit();
}

void SoundManager::loadSoundFromFile(SoundID id, const std::string &path) {
    WARNING_IF (m_sounds.find(id) != m_sounds.end(), "Le son a déjà été chargé");
    MIX_Audio* audio = MIX_LoadAudio(m_mixer, path.c_str(), true);
    PTR_CHECK(audio);

    m_sounds[id] = MIX_CreateTrack(m_mixer);
    PTR_CHECK(m_sounds[id]);
    BOOL_CHECK(MIX_SetTrackAudio(m_sounds[id], audio));
}

void SoundManager::playSound(const SoundID id) {
    if (ENABLE_SOUND)
        BOOL_CHECK(MIX_PlayTrack(m_sounds[id], 0));
}

SoundManager &SoundManager::getInstance() {
    if (s_instance == nullptr)
        s_instance = new SoundManager();
    return *s_instance;
}

SoundManager::SoundManager() {
    BOOL_CHECK(MIX_Init());

    SDL_AudioSpec audioSpec;
    audioSpec.freq = 44100;
    audioSpec.channels = 2;
    audioSpec.format = SDL_AUDIO_F32;
    m_mixer =  MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audioSpec);
}
