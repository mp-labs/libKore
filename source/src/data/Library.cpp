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

#include <data/Library.hpp>

#include <KoreModule.hpp>

#define K_BLOCK_SUPER_TYPE      Kore::data::Block
#define K_BLOCK_TYPE            Kore::data::Library
#define K_BLOCK_PROPERTY_METHOD Kore::data::Library::LibraryProperty
#include <data/BlockMacros.hpp>
K_BLOCK_IMPLEMENTATION

using namespace Kore::data;

Library::Library( kuint64 extraFlags )
{
    // By default a library is browsable
    addFlags( Browsable );
    addFlags( extraFlags );
}

Library::~Library()
{
    addFlags( IsBeingDeleted );

    // Make a copy of the blocks...
    QList< Block* > blocks = _blocks;

    for( int i = 0; i < blocks.size(); ++i )
    {
        Block* b = blocks.at( i );
        if( b->checkFlag( Static ) )
        {
            // Remove the block from the tree to avoid deleting it, this
            // would cause an error as it was not newed.
            removeBlock( b );
        }
    }
}

void Library::clear()
{
    if( isEmpty() )
    {
        return; // Nothing to clear !
    }

    emit clearing();

    // Make a copy of the blocks...
    QList< Block* > blocks = _blocks;

    for( int i = 0; i < blocks.size(); ++i )
    {
        Block* b = blocks.at( i );
        // Remove the block from the tree
        removeBlock( b );
        if( ! b->checkFlag( Static ) )
        {
            // Delete the block
            delete b;
        }
    }

    // Clear the blocks list
    _blocks.clear();

    emit cleared();
}

kint Library::totalSize() const
{
    kint size = _blocks.size();
    for( kint i = 0; i < _blocks.size(); ++i )
    {
        if( _blocks.at(i)->isLibrary() )
        {
            size += static_cast< Library* >( _blocks.at( i ) )->totalSize();
        }
    }
    return size;
}

void Library::optimize( int )
{
    QList< Block* > list;
    list.reserve( _blocks.size() );
    list.append( _blocks );
    _blocks = list;
}

void Library::optimizeTree( int cause )
{
    // Optimize this library
    optimize( cause );
    // Optimize the tree
    for( kint i = 0; i < _blocks.size(); ++i )
    {
        Block* b = _blocks.at( i );
        b->isLibrary()
            ? static_cast< Library* >( b )->optimizeTree()
            : b->optimize();
    }
}

kbool Library::acceptsBlock( Block* b ) const
{
    Q_UNUSED( b );
    return false;
}

void Library::addBlock( Block* b )
{
    K_ASSERT( ! _blocks.contains( b ) )

    const kint index = _blocks.size();
    emit addingBlock( index );
    _blocks.append( b );
    // Set the new block index first
    b->index( index );
    // Set its library !
    b->library( this );
    emit blockAdded( index );
}

void Library::removeBlock( Block* b )
{
    K_ASSERT( _blocks.contains( b ) )

    const kint index = b->index();
    emit removingBlock( index );
    _blocks.removeAt( index );
    // Reindex the blocks from the right offset...
    indexBlocks( index );

    if( ! b->checkFlag( IsBeingRemoved ) )
    {
        // WE are removing the block, the block is not removing itself from us
        // Important to indicate to the block that WE are removing it
        b->addFlags( IsBeingRemoved );
        b->library( K_NULL );
    }

    // In every case, update the block's index
    b->index( -1 );

    emit blockRemoved( index );
}

void Library::insertBlock( Block* b, kint index )
{
    K_ASSERT( ! _blocks.contains( b ) )

    emit addingBlock( index );
    _blocks.insert( index, b );
    // Set the new block index first
    b->index( index );
    // Set its library !
    b->library( this );
    indexBlocks( index );
    emit blockAdded( index );
}

void Library::swapBlocks( Block* a, Block* b )
{
    K_ASSERT( _blocks.contains( a ) && _blocks.contains( b ) )

    emit swappingBlocks( a->index(), b->index() );
    // Swap in the list
    _blocks.swap( a->index(), b->index() );
    // Swap the respective indexes
    int index = a->index();
    a->index( b->index() );
    b->index( index );
    emit blocksSwapped( a->index(), b->index() );
}

void Library::moveBlock( Block* block, kint to )
{
    K_ASSERT( _blocks.contains( block ) )

    kint from = ( -1 == block->index() )
                    ? _blocks.indexOf( block )
                    : block->index();
    K_ASSERT( -1 != from )
    K_ASSERT( _blocks.at( from ) == block )

    if( from == to )
    {
        // No need to continue here...
        return;
    }

    emit movingBlock( from, to );
    _blocks.move( from, to );
    indexBlocks( K_MIN( from, to ) );
    emit blockMoved( from, to );
}

void Library::indexBlocks( kint startOffset )
{
    for( int i = startOffset; i < _blocks.size(); ++i )
    {
        _blocks.at(startOffset)->index(startOffset);
    }
}

kbool Library::isBrowsable() const
{
    return checkFlag( Browsable );
}

QVariant Library::LibraryProperty( kint property )
{
    switch( property )
    {
    case BlockTypeName:
        return tr( "Basic data library" );
    default:
        return Block::DefaultBlockProperty( property );
    }
}
