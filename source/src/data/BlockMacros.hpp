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
#   error You must define K_MODULE_TYPE before including <BlockMacros.hpp> !
#endif

#ifndef K_BLOCK_SUPER_TYPE
#   error You must define K_BLOCK_SUPER_TYPE before include <BlockMacros.hpp> !
#endif

#ifndef K_BLOCK_TYPE
#   error You must define K_BLOCK_TYPE before including <BlockMacros.hpp> !
#endif

#include <QtCore/QAtomicInt>

#include "../KoreEngine.hpp"

// The complete declaration of the MetaBlock is required here to create a
// subclass
#include "MetaBlock.hpp"

#define K_BLOCK_BEGIN \
    namespace\
    {\
        QBasicAtomicInt blockInstancesCount = Q_BASIC_ATOMIC_INITIALIZER( 0 );\
        \
        class PrivateMetaBlock : public Kore::data::MetaBlock\
        {\
        public:\
            PrivateMetaBlock();\
            virtual bool canUnload() const;\
            virtual Kore::data::Block* createBlock() const;\
            virtual void destroyBlock( Kore::data::Block* b ) const;\
            virtual QVariant blockProperty( kint property ) const;\
            static Kore::plugin::Loadable* Instance();\
            static bool _Registered;\
        };\
    }\
    \
    Q_GLOBAL_STATIC( PrivateMetaBlock, globalMetaBlock );\
    \
    namespace {\
        PrivateMetaBlock::PrivateMetaBlock()\
            : MetaBlock( K_BLOCK_SUPER_TYPE::StaticMetaBlock(),\
                         & ( K_BLOCK_TYPE::staticMetaObject ) )\
        {\
            addFlag( Block::Static );\
        }\
        \
        bool PrivateMetaBlock::canUnload() const\
        {\
            return 0 == blockInstancesCount.load();\
        }\
        \
        void PrivateMetaBlock::destroyBlock( Kore::data::Block* b ) const\
        {\
            MetaBlock::destroyBlock( b );\
            blockInstancesCount.deref();\
        }\
        \
        Kore::plugin::Loadable* PrivateMetaBlock::Instance()\
        {\
            return globalMetaBlock;\
        }\
        \
        /* Force a static registration of the initialization routine */ \
        bool PrivateMetaBlock::_Registered =\
            K_MODULE_TYPE::RegisterLoadable(\
                & ( PrivateMetaBlock::Instance ) );\
    }\
    \
    const Kore::data::MetaBlock* K_BLOCK_TYPE::StaticMetaBlock()\
    {\
        return globalMetaBlock;\
    }\
    \
    const Kore::data::MetaBlock* K_BLOCK_TYPE::metaBlock() const\
    {\
        return StaticMetaBlock();\
    }

#define K_BLOCK_END

// Instantiation
#define K_BLOCK_VIRTUAL \
    namespace\
    {\
        Kore::data::Block* PrivateMetaBlock::createBlock() const\
        {\
            qFatal( "Can not instantiate virtual block "\
                    K_BLOCK_XSTR( K_BLOCK_TYPE ) );\
            return K_NULL;\
        }\
    }

#define K_BLOCK_ALLOCABLE \
    namespace\
    {\
        Kore::data::Block* PrivateMetaBlock::createBlock() const\
        {\
            Kore::data::Block* b = new K_BLOCK_TYPE;\
            if( NULL != b )\
            {\
                InitializeBlock( b );\
                SetBlockAllocated( b );\
                blockInstancesCount.ref();\
            }\
            return b;\
        }\
    }

// Properties
#define K_BLOCK_PROPERTY_METHOD( propertyMethod ) \
    namespace\
    {\
        QVariant PrivateMetaBlock::blockProperty( int id ) const\
        {\
            return propertyMethod( id );\
        }\
    }

#define K_BLOCK_PROPERTY_DEFAULT \
    namespace\
    {\
        QVariant PrivateMetaBlock::blockProperty( int id ) const\
        {\
            return Kore::data::Block::DefaultBlockProperty( id );\
        }\
    }
