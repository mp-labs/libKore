/*
 * Copyright (c) 2013, Moving Pixel Labs (http://www.mp-labs.net)
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the Moving Pixel Labs nor the names of its
 *      contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL MOVING PIXEL LABS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <data/Block.hpp>
#include <data/Library.hpp>

#include <memory/MemoryManager.hpp>

namespace Kore {

class KoreExport KoreApplication {

public:
    KoreApplication( kint argc, kchar** argv );
    ~KoreApplication();

    const Kore::data::Library* rootLibrary() const;
    const Kore::memory::MemoryManager* memoryManager() const;

    Kore::data::Library* dataLibrary() const;

    kint argc() const;
    kchar** argv() const;

    kbool isClosing() const;

    void exit( kint exitCode = 0 );
    void quit();

    static KoreApplication* Instance();
    static QString Version();

protected:
    Kore::data::Library*            _rootLibrary;
    Kore::data::Library*            _dataLibrary;
    Kore::memory::MemoryManager*    _memoryManager;

    kint    _argc;
    kchar** _argv;

    kbool   _closing;

    static KoreApplication* _Instance;
};

}

#define kApp ( Kore::KoreApplication::Instance() )
