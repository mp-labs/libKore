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

#ifndef K_MODULE_TYPE
#   error You must define K_MODULE_TYPE before including <ModuleMacros.hpp> !
#endif

#ifndef K_MODULE_TYPES_LIST
#   define K_MODULE_TYPES_LIST
#endif

#include "../KoreEngine.hpp"

#include <QtCore/QGlobalStatic>

#define K_MODULE_IMPL \
    Q_GLOBAL_STATIC( K_MODULE_TYPE, moduleInstance )\
    void K_MODULE_TYPE::registerModuleTypes()\
    {\
        quint16 moduleTypeIdx = 0;\
        Q_UNUSED( moduleTypeIdx );\
        K_MODULE_TYPES_LIST\
    }\
    \
    const Kore::plugin::Module* K_MODULE_TYPE::Instance()\
    {\
        return moduleInstance;\
    }\
    \
    Kore::plugin::Module* K_MODULE_TYPE::PrivateInstance()\
    {\
        return moduleInstance;\
    }\
    \
    bool K_MODULE_TYPE::RegisterLoadable(\
            Kore::plugin::Loadable::Instantiator instantiator )\
    {\
        moduleInstance->registerLoadable( instantiator );\
        return true;\
    }

#define K_MODULE_PLUGIN_IMPL \
    void K_MODULE_TYPE::registerModuleTypes()\
    {\
        quint16 moduleTypeIdx = 0;\
        Q_UNUSED( moduleTypeIdx );\
        K_MODULE_TYPES_LIST\
    }\
    \
    const Kore::plugin::Module* K_MODULE_TYPE::Instance()\
    {\
        return moduleInstance;\
    }\
    \
    Kore::plugin::Module* K_MODULE_TYPE::PrivateInstance()\
    {\
        return moduleInstance;\
    }\
    \
    bool K_MODULE_TYPE::RegisterLoadable(\
            Kore::plugin::Loadable::Instantiator instantiator )\
    {\
        moduleInstance->registerLoadable( instantiator );\
        return true;\
    }
