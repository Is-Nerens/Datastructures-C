// MIT License
// Copyright (c) 2026 Arran Stevens

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <stdio.h>
#include <stdint.h>

static LARGE_INTEGER _PERFORMANCE_freq;
static LARGE_INTEGER _PERFORMANCE_start_time;
static int _PERFORMANCE_initialized = 0;

static inline void _PERFORMANCE_init() {
    if (!_PERFORMANCE_initialized) {
        QueryPerformanceFrequency(&_PERFORMANCE_freq);
        _PERFORMANCE_initialized = 1;
    }
}

static inline void timer_start() {
    _PERFORMANCE_init();
    QueryPerformanceCounter(&_PERFORMANCE_start_time);
}

static inline void timer_stop() {
    LARGE_INTEGER end_time;
    QueryPerformanceCounter(&end_time);
    double elapsed_us = (double)(end_time.QuadPart - _PERFORMANCE_start_time.QuadPart) * 1e6 / (double)_PERFORMANCE_freq.QuadPart;
    printf("Elapsed time: %.3f us\n", elapsed_us);
}
