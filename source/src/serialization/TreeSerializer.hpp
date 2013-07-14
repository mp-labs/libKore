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

#ifndef _Kore_serialization_TreeSerializer_hpp_
#define _Kore_serialization_TreeSerializer_hpp_

#include <QtCore/QIODevice>

#include <KoreMacros.hpp>

namespace Kore {

namespace data { class Block; }

namespace serialization {

class KoreExport TreeSerializerMonitor
{
public:
    virtual ~TreeSerializerMonitor() {}

    virtual bool shouldStopProcess() = K_VIRTUAL;

    virtual bool event( int errorCode, const QString& message ) = K_VIRTUAL;

    virtual void progress( qint64 min,
                           qint64 progress,
                           qint64 max ) = K_VIRTUAL;
};

class KoreExport TreeSerializer
{
public:
    enum SerializationError
    {
        NoError = 0,
        UnsupportedOperation,
        IOError,
        RequiresRandomAccess,
        InvalidData,
        SeekFailed,
        BlockInstantiationFailed,
        BlockSetPropertyFailed,
        UnknownRootBlockType,
        UnknownBlockType,
        UnknownModuleType,
        UnknownUserType,
        UnknownModuleUserType,
        UnregisteredModuleUserType,
        MAX_SERIALIZATION_ERROR
    };

public:
    virtual ~TreeSerializer() {}

    virtual int deflate( QIODevice* device,
                         const Kore::data::Block* block,
                         TreeSerializerMonitor* monitor ) const = K_VIRTUAL;

    virtual int inflate( QIODevice* device,
                         Kore::data::Block** block,
                         TreeSerializerMonitor* monitor ) const = K_VIRTUAL;
};

} /* serialization */ } /* Kore */

#endif // _Kore_serialization_TreeSerializer_hpp_
