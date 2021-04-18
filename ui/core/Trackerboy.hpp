
#pragma once

#include "core/audio/Renderer.hpp"
#include "core/model/ModuleDocument.hpp"
#include "core/model/InstrumentListModel.hpp"
#include "core/model/OrderModel.hpp"
#include "core/model/WaveListModel.hpp"
#include "core/Config.hpp"
#include "core/ColorTable.hpp"
#include "core/Miniaudio.hpp"
#include "core/Spinlock.hpp"

//
// Container struct for configuration and model classes used throughout the ui.
//
struct Trackerboy {

    Trackerboy();
    ~Trackerboy() = default;

    Spinlock spinlock;

    //
    // manages a miniaudio context and device list
    //
    Miniaudio miniaudio;

    //
    // Application settings
    //
    Config config;

    //
    // The document (module) being worked on
    //
    ModuleDocument document;

    //
    // Model classes provide a read/write interface for the document
    //
    InstrumentListModel instrumentModel;
    OrderModel orderModel;
    WaveListModel waveModel;

    //
    // audio renderering + playback
    //
    Renderer renderer;

};
