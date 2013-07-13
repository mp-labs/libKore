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

#include <plugin/Loadable.hpp>

#include <QtCore/QMetaClassInfo>
#include <QtCore/QMetaObject>
#include <QtCore/QMetaProperty>

#include <QtCore/QMultiHash>
#include <QtCore/QVector>

namespace Kore { namespace data {

class BlockExtension;

class KoreExport MetaBlock : public Kore::plugin::Loadable
{
    Q_OBJECT

    friend class Block;
    friend class BlockExtension;

protected:
    MetaBlock( const MetaBlock* superMetaBlock, const QMetaObject* mo );

    virtual void library( Kore::data::Library* lib );

public:
    virtual bool canUnload() const = K_VIRTUAL;

    virtual Block* createBlock() const = K_VIRTUAL;
    template< typename T >
    T* createBlockT() const { return static_cast< T* >( createBlock() ); }

    const QMetaObject* blockMetaObject() const;

    QString blockClassName() const;

    virtual QMetaProperty property( kint property ) const;

    virtual QVariant blockProperty( kint property ) const = K_VIRTUAL;
    QVariant blockSetting( const QString& setting,
                           const QVariant& defaultValue ) const;

    BlockExtension* blockExtension( const QString& name ) const;
    QList< BlockExtension* > blockExtensions( const QString& name ) const;
    const QMultiHash< QString, BlockExtension* >& extensions() const;

    MetaBlock* superMetaBlock();
    const MetaBlock* superMetaBlock() const;

protected:
    inline static void SetBlockAllocated( Block* b )
    {
        b->addFlags( Block::Allocated );
    }

    inline static void InitializeBlock( Block* b )
    {
        b->initialize();
    }

    kbool registerBlockExtension( BlockExtension* extension );
    void unregisterBlockExtension( BlockExtension* extension );

    virtual void destroyBlock( Block* b ) const = K_NULL;

private:
    void createPropertiesCache() const;
    void clearExtensions();

private:
    const QMetaObject*  _blockMetaObject;
    const MetaBlock*    _superMetaBlock;

    QMultiHash< QString, BlockExtension* > _extensions;
};

}}

#define K_BLOCK_CREATE_INSTANCE( block )\
    ( block::StaticMetaBlock()->createBlockT< block >() )
