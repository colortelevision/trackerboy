/*
** Trackerboy - Gameboy / Gameboy Color music tracker
** Copyright (C) 2019-2020 stoneface86
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
**
*/

#pragma once

#include <cstdint>

namespace trackerboy {

//
// The speed type determines the tempo during pattern playback. Its unit is
// frames per row in Q4.4 format. Speeds with a fractional component will
// have some rows taking an extra frame.
//
using Speed = uint8_t;

// minimum possible speed, 1.0 frames per row
static constexpr Speed SPEED_MIN = 0x10;

// maximum possible speed, 15.0 frames per row
static constexpr Speed SPEED_MAX = 0xF0;


}
