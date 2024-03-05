#include "Playlist.h"

Playlist::Playlist() {
    songs = std::vector<Song>();
}

void Playlist::clear() {
    songs.clear();
}

void Playlist::addSong(const char *file) {
    Song song;
    song.file = std::string(file);
    songs.push_back(song);
}
