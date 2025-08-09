//
// Created by Rayann CHOUCRALLAH on 27/07/2025.
//

#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <unordered_map>
#include "SDL3_mixer/SDL_mixer.h"

enum class SoundID {
    COLLECT_FRUIT,
    JUMP,
    HURT
};

class SoundManager {
    static SoundManager *s_instance;
public:
    ~SoundManager();
    void loadSoundFromFile(SoundID id, const std::string& path);
    void playSound(const SoundID id);

    static SoundManager& getInstance();

private:
    SoundManager();

    std::unordered_map<SoundID, MIX_Track*> m_sounds;
    MIX_Mixer* m_mixer;
};

#endif //SOUNDMANAGER_H
