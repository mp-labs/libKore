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

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>

#include <data/Block.hpp>
#include <data/Library.hpp>
#include <data/MetaBlock.hpp>

namespace
{

class BlockPrivateMetaBlock : public Kore::data::MetaBlock
{
public:
    BlockPrivateMetaBlock()
        : MetaBlock( K_NULL, & Kore::data::Block::staticMetaObject )
    {}

    virtual bool canUnload() const { return false; }
    virtual Block* createBlock() const { return K_NULL; }

    virtual QVariant blockProperty( kint property ) const
    {
        return Kore::data::Block::DefaultBlockProperty( property );
    }
};

}

Q_GLOBAL_STATIC( BlockPrivateMetaBlock, BlockMetaBlock );

const Kore::data::MetaBlock* Kore::data::Block::metaBlock() const
{
    return BlockMetaBlock;
}

const Kore::data::MetaBlock* Kore::data::Block::StaticMetaBlock()
{
    return BlockMetaBlock;
}

using namespace Kore::data;

Block::Block()
    : _library( K_NULL )
    , _flags( 0 )
    , _index( -1 )
{
}

void Block::initialize()
{
    // TODO: Do some cache/init in here!!
}

Block::~Block()
{
    if( checkFlag( Static ) )
    {
        destroy();
    }

    K_STRONG_ASSERT( checkFlag( IsBeingDeleted ) )
    K_STRONG_ASSERT( _library == K_NULL )
}

bool Block::destroy()
{
    // This block is being deleted.
    addFlag( IsBeingDeleted );

    // Notify our watchers.
    emit blockDeleted();

    // Cleanup the connection to the library
    if( hasParent() )
    {
        // Immediate removal from the library tree.
        library()->removeBlock( this );
    }

    // Actually destroy this thing.
    if( checkFlag( Static ) )
    {
        // This was statically allocated (Q_GLOBAL_STATIC): do nothing
    }
    else if( checkFlag( Allocated ) )
    {
        // If the block was allocated
        metaBlock()->destroyBlock( this );
    }
    else if( checkFlag( System ) )
    {
        // The block does not have a factory but should be deleted anyway as it
        // is a SYSTEM block.
        delete this; // Delete this straight up...
    }
    else
    {
        // This block can not be completely removed...
        return false;
    }

    return true;
}

void Block::index( kint idx )
{
    if( _index != idx )
    {
        // Very important to emit the signal before, so that watchers can keep
        // accurate record of the indices...
        emit indexChanged( _index, idx );
        _index = idx;
    }
}

void Block::library( Library* lib )
{
    if( hasParent() )
    {
        // We test whether the Block already belongs to a Library
        // if that Library is currently removing this Block, there is no need
        // to perform further removal. (tricky one :P)
        if( -1 != _index )
        {
            // The index is valid: the library is not removing us, we are
            // removing ourselves from the library!
            _index = -1; // Make sure the library knows that WE are going!
            // Here the current library is not aware that the block is about to
            // change of owner.
            library()->removeBlock( this );
        }
        else if( lib == K_NULL )
        {
            // We set a NULL library.
            emit blockRemoved();
        }
    }
    else if( lib )
    {
        // The block had no library and is getting one: it is being inserted !
        emit blockInserted();
    }

    setParent( lib ); // Qt threading, tree deletion and so on...
    _library = lib;
}

void Block::optimize( int cause )
{
    Q_UNUSED( cause );
    // Default does... well, nothing :D
    // This is definitely an optimal Block.
}

QString Block::objectClassName() const
{
    return QLatin1String( metaObject()->className() );
}

void Block::blockName( const QString& name )
{
    if( objectName() != name )
    {
        setObjectName( name );
        emit blockNameChanged( name );
    }
}

QString Block::blockName() const
{
    return objectName();
}

kbool Block::isLibrary() const
{
    return false;
}

void Block::addFlag( kuint64 flag )
{
    _flags |= flag;
}

void Block::removeFlag( kuint flag )
{
    ( _flags & flag ) ? ( _flags ^= flag ) : _flags;
}

kbool Block::fastInherits( const MetaBlock* mb ) const
{
    // Speedup proposal: far more efficient based on pointer arithmetic.
    // Avoid using qMetaCast based on strcmp stuff...
    for( const QMetaObject* mo = this->metaObject();
         NULL != mo;
         mo = mo->superClass() )
    {
        if( mb->blockMetaObject() == mo )
        {
            return true;
        }
    }
    return false;
}

QVariant Block::DefaultBlockProperty( kint property )
{
    switch( property )
    {
    case Block::BlockTypeName:
        return tr( "Basic building block" );
    case Block::BlockPropertiesName:
        {
            QHash< QString, QVariant > result;
            result.insert( "blockName", tr( "Name" ) );
            return result;
        }
    default:
        return QVariant();
    }
}
