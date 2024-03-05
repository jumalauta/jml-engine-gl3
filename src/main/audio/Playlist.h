#ifndef ENGINE_AUDIO_PLAYLIST_H_
#define ENGINE_AUDIO_PLAYLIST_H_

#include <string>
#include <vector>

class Song {
public:
    std::string file;
};

class Playlist {
public:
    Playlist();
    void clear();
    void addSong(const char *file);
private:
    std::vector<Song> songs;
};

#endif /*ENGINE_AUDIO_PLAYLIST_H_*/
