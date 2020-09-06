
#pragma once

#include "trackerboy/data/Table.hpp"
#include "trackerboy/data/Song.hpp"

#include <fstream>
#include <string>
#include <vector>


namespace trackerboy {


class Module {

public:
    Module();

    void clear();

    std::string artist();

    std::string title();

    std::string copyright();

    //SongTable& songTable();
    std::vector<Song>& songs();

    WaveTable& waveTable();

    InstrumentTable& instrumentTable();

    void setArtist(std::string artist);

    void setTitle(std::string title);

    void setCopyright(std::string copyright);


private:

    //SongTable mSongTable;
    std::vector<Song> mSongs;
    WaveTable mWaveTable;
    InstrumentTable mInstrumentTable;


    // information about the module (same format as *.gbs)
    std::string mTitle;
    std::string mArtist;
    std::string mCopyright;

};


}
