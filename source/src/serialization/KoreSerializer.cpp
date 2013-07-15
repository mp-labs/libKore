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

#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QMetaObject>
#include <QtCore/QStack>
#include <QtCore/QStringList>
#include <QtCore/QVector>

#include "KoreSerializer.hpp"

#include <KoreEngine.hpp>

#include <data/Block.hpp>
#include <data/MetaBlock.hpp>

#include <plugin/Module.hpp>

using namespace Kore;
using namespace Kore::data;
using namespace Kore::plugin;
using namespace Kore::serialization;

#define CREATE_STREAM( name, device ) \
    QDataStream name( device );\
    name.setVersion( QDataStream::Qt_5_1 )

#define LIBRARY_HAS_CHILDREN_FLAG   0x80000000
#define LIBRARY_HAS_CHILREN_MASK    0x7FFFFFFF

#define END_OF_STREAM               ( K_FOURCC( 'K', 'E', 'N', 'D' ) )

namespace {

struct Context
{
    Context( QByteArray* buf, QIODevice* dev, TreeSerializerMonitor* mon )
        : buffer( buf )
        , device( dev )
        , monitor( mon )
        , blocksCount( 0 )
    { /* NOTHING */ }

    QByteArray* buffer;

    QIODevice* device;
    TreeSerializerMonitor* monitor;

    quint32 blocksCount;

    QStringList metaBlocksNames;
    QList< const MetaBlock* > metaBlocksList;
    QMap< const MetaBlock*, quint32 > metaBlocks;

    QStringList modulesIds;
    QList< const Module* > modulesList;
    QMap< const Module*, quint16 > modules;

    quint32 getMetaBlockIndex( const MetaBlock* mb )
    {
        quint32 index = metaBlocks.value( mb, 0xffffffff );
        if( 0xffffffff == index )
        {
            index = metaBlocksList.size();
            metaBlocksList.append( mb );
            metaBlocks.insert( mb, index );
        }
        return index;
    }

    const MetaBlock* getMetaBlock( quint32 index )
    {
        return metaBlocksList.value( index, K_NULL );
    }

    quint16 getModuleIndex( const Module* module )
    {
        quint16 index = modules.value( module, 0xffff );
        if( 0xffff == index )
        {
            index = modulesList.size();
            modulesList.append( module );
            modules.insert( module, index );
        }
        return index;
    }

    const Module* getModule( quint16 index )
    {
        return modulesList.value( index, K_NULL );
    }
};

int WriteMetaData( Context& ctx )
{
    // Store the position
    const qint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    // Write the modules metadata
    stream << static_cast< quint16 >( ctx.modulesList.size() );
    for( int i = 0; i < ctx.modulesList.size(); ++i )
    {
        stream << ctx.modulesList.at( i )->id().toLatin1();
    }

    // Write the metablocks metadata
    stream << static_cast< quint32 >( ctx.metaBlocksList.size() );
    for( int i = 0; i < ctx.metaBlocksList.size(); ++i )
    {
        stream << ctx.metaBlocksList.at( i )->blockClassName().toLatin1();
    }

    // Write the number of serialized blocks
    stream << ctx.blocksCount;

    // Write the size of the metadata
    stream << static_cast< quint32 >( ctx.device->pos() - startPos );

    // Write the end of stream tag
    stream << static_cast< quint32 >( END_OF_STREAM );

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int ReadMetaData( Context& ctx )
{
    // Store the initial position in the device
    const quint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    // Go read the metadata size
    if( ! ctx.device->seek( ctx.device->size() - sizeof( quint32 ) ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Read the end of stream tag
    quint32 endOfStreamTag;
    stream >> endOfStreamTag;
    if( END_OF_STREAM != endOfStreamTag )
    {
        return TreeSerializer::InvalidData;
    }

    if( ! ctx.device->seek( ctx.device->size() - ( 2 * sizeof( quint32 ) ) ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Read the metadata size
    quint32 metaDataSize;
    stream >> metaDataSize;

    // Go at the beginning of the metadata
    if( ! ctx.device->seek(
            ctx.device->size() - ( 2 * sizeof( quint32 ) ) - metaDataSize ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Modules
    quint16 modulesCount;
    stream >> modulesCount;

    // Reserve the space for the modules
    ctx.modulesIds.reserve( modulesCount );
    ctx.modulesList.reserve( modulesCount );

    for( quint16 i = 0; i < modulesCount; ++i )
    {
        QByteArray moduleId;
        stream >> moduleId;

        QString moduleIdStr = QString::fromLatin1( moduleId, moduleId.size() );

        ctx.modulesIds.append( moduleIdStr );

        const Module* module = KoreEngine::GetModule( moduleIdStr );

        if( ( K_NULL == module ) &&
            ( K_NULL != ctx.monitor ) &&
            ( ! ctx.monitor->event( TreeSerializer::UnknownModuleType,
                                    moduleIdStr ) ) )
        {
            return TreeSerializer::UnknownModuleType;
        }

        // Store in the list (even if NULL)
        ctx.modulesList.append( module );
    }

    // Metablocks
    quint32 metaBlocksCount;
    stream >> metaBlocksCount;

    // Reserve the space for the blocks (OK since indices are integers)
    ctx.metaBlocksNames.reserve( metaBlocksCount );
    ctx.metaBlocksList.reserve( metaBlocksCount );

    for( qint32 i = 0; i < metaBlocksCount; ++i )
    {
        QByteArray blockClass;
        stream >> blockClass;

        QString blockClassStr = QString::fromLatin1( blockClass,
                                                     blockClass.size() );

        ctx.metaBlocksNames.append( blockClassStr );

        const MetaBlock* mb = KoreEngine::GetMetaBlock( blockClassStr );

        if( ( K_NULL == mb ) &&
            ( K_NULL != ctx.monitor ) &&
            ( ! ctx.monitor->event( TreeSerializer::UnknownBlockType,
                                    blockClassStr ) ) )
        {
            return TreeSerializer::UnknownBlockType;
        }

        // Store in the list (even if NULL)
        ctx.metaBlocksList.append( mb );
    }

    // Read the number of serialized blocks
    stream >> ctx.blocksCount;

    // Restore to the initial position
    if( ! ctx.device->seek( startPos ) )
    {
        return TreeSerializer::SeekFailed;
    }

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int WriteBlockProperties( Context& ctx, const Block* block )
{
    const qint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    // Write a blank properties size
    stream << quint16( 0 );

    quint16 propertiesCount = 0;

    for	( kint i = Block::staticMetaObject.propertyOffset();
          i < block->metaObject()->propertyCount();
          ++i )
    {
        QMetaProperty prop = block->metaBlock()->property( i );
        if( ! prop.isStored( block ) )
        {
            continue;
        }

        QVariant variant = prop.read( block );

        // Do not serialize NULL variants
        if( variant.isNull() )
        {
            continue;
        }

        const int propType = static_cast< int >( prop.type() );

        if( ( propType > QMetaType::UnknownType ) &&
            ( propType < QMetaType::User ) )
        {
            // Write the property index
            stream << static_cast< quint16 >( prop.propertyIndex() );

            switch( propType )
            {
            case QMetaType::QString:
                // For strings, encode to UTF-8  byte array to save space.
                stream << variant.toString().toUtf8();
                break;
            default:
                // Native type, simply write the data
                stream << variant;
                break;
            }
        }
        else if( prop.userType() >= QMetaType::User )
        {
            // Custom type, query + store it's module and in module meta type id
            const Module* module;
            module = KoreEngine::GetModuleForUserType( prop.userType() );
            if( K_NULL == module )
            {
                if( ( K_NULL != ctx.monitor ) &&
                    ! ctx.monitor->event(
                            TreeSerializer::UnregisteredModuleUserType,
                            QLatin1String( prop.typeName() ) ) )
                {
                    return TreeSerializer::UnregisteredModuleUserType;
                }
                else
                {
                    qWarning( "Skipping property: %s with type %s of Block "
                              "type %s because it was not registered",
                              prop.name(),
                              prop.typeName(),
                              block->metaObject()->className() );
                    continue;
                }
            }

            quint16 type = module->moduleTypeIdForUserTypeId( prop.userType() );
            if( 0xffff == type )
            {
                if( ( K_NULL != ctx.monitor ) &&
                    ! ctx.monitor->event( TreeSerializer::UnknownModuleUserType,
                                          QLatin1String( prop.typeName() ) ) )
                {
                    return TreeSerializer::UnknownModuleUserType;
                }
                else
                {
                    qWarning( "Skipping property: %s with type %s of Block "
                              "type %s because it was mis-registered",
                              prop.name(),
                              prop.typeName(),
                              block->metaObject()->className() );
                    continue;
                }
            }

            // Write the property index
            stream << static_cast< quint16 >( prop.propertyIndex() );

            // Write two quint16, one for the module index in the metadata,
            // and the other for the type ID within the module
            stream << ctx.getModuleIndex( module );
            stream << type;

            // Write the data
            if( ! QMetaType::save( stream,
                                   prop.userType(),
                                   variant.constData() ) )
            {
                ctx.monitor->event( TreeSerializer::UnknownModuleUserType,
                                    QLatin1String( prop.typeName() ) );
                return TreeSerializer::UnknownModuleUserType;
            }
        }
        else
        {
            if( ( K_NULL != ctx.monitor ) &&
                ! ctx.monitor->event( TreeSerializer::UnknownUserType,
                                      QLatin1String( prop.typeName() ) ) )
            {
                return TreeSerializer::UnknownUserType;
            }
            else
            {
                qWarning( "Skipping property: %s with unknown type %s of Block "
                          "type %s",
                          prop.name(),
                          prop.typeName(),
                          block->metaObject()->className() );
                continue;
            }
        }

        // Increment the count of serialized properties
        ++propertiesCount;
    }

    // Save the end position
    const qint64 endPos = ctx.device->pos();

    // Go back to the beginning
    if( ! ctx.device->seek( startPos ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Write the actual properties size
    stream << propertiesCount;

    // Move back to the end
    if( ! ctx.device->seek( endPos ) )
    {
        return TreeSerializer::SeekFailed;
    }

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int ReadBlockProperties( Context& ctx, Block* block )
{
    CREATE_STREAM( stream, ctx.device );

    // Retrieve the number of properties
    quint16 propertiesCount;
    stream >> propertiesCount;

    for	( quint16 i = 0; i < propertiesCount; ++i )
    {
        // Retrieve the index of the property
        quint16 propertyIdx;
        stream >> propertyIdx;

        QMetaProperty prop = block->metaBlock()->property( propertyIdx );

        int propType = static_cast< int >( prop.type() );

        QVariant variant;

        if( ( propType > QMetaType::UnknownType ) &&
            ( propType < QMetaType::User ) )
        {
            switch( propType )
            {
            case QMetaType::QString:
                // For strings parse an UTF-8 buffer
                {
                    QByteArray utf8;
                    stream >> utf8;
                    variant = QString::fromUtf8( utf8, utf8.size() );
                }
                break;
            default:
                // Qt native type, simply read the data
                stream >> variant;
                break;
            }
        }
        else if( prop.userType() >= QMetaType::User )
        {
            // Custom type, retrieve it's module and in module meta type id
            quint16 moduleId;
            quint16 moduleType;

            stream >> moduleId;
            stream >> moduleType;

            const Module* module = ctx.getModule( moduleId );
            if( K_NULL == module )
            {
                // Notify the client of the error
                if( K_NULL != ctx.monitor )
                {
                    ctx.monitor->event(
                                TreeSerializer::UnregisteredModuleUserType,
                                QLatin1String( prop.typeName() ) );
                }

                // Return error
                return TreeSerializer::UnregisteredModuleUserType;
            }

            propType = module->userTypeIdForModuleTypeId( moduleType );
            if( QMetaType::UnknownType == propType )
            {
                // Notify the client of the error
                if( K_NULL != ctx.monitor )
                {
                    ctx.monitor->event( TreeSerializer::UnknownModuleUserType,
                                        QLatin1String( prop.typeName() ) );
                }

                // Return error
                return TreeSerializer::UnknownModuleUserType;
            }

            // Create a variant with the proper type
            variant = QVariant( propType, K_NULL );

            // Read the data
            if( ! QMetaType::load( stream, propType, variant.data() ) )
            {
                ctx.monitor->event( TreeSerializer::CustomPropertyReadFailed,
                                    QLatin1String( prop.typeName() ) );
                return TreeSerializer::CustomPropertyReadFailed;
            }
        }
        else
        {
            // Notify the user
            if( K_NULL != ctx.monitor )
            {
                ctx.monitor->event( TreeSerializer::UnknownUserType,
                                    QLatin1String( prop.typeName() ) );
            }

            // Return error
            return TreeSerializer::UnknownUserType;
        }

        // Finally, set the property on the block
        if( ! prop.write( block, variant ) )
        {
            if( ( K_NULL != ctx.monitor ) &&
                ! ctx.monitor->event( TreeSerializer::BlockSetPropertyFailed,
                                      QLatin1String( prop.typeName() ) ) )
            {
                return TreeSerializer::BlockSetPropertyFailed;
            }
            else
            {
                qWarning( "Failed to set property %s of type %s on Block %s",
                          prop.name(),
                          prop.typeName(),
                          block->metaObject()->className() );
            }
        }
    }

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int DeflateBlockRandomAccess( Context& ctx, const Block* block, int childrenNb )
{
    // Store the position at the beginning of this block
    const qint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    // Write a blank block header
    // Type
    stream << quint32( 0 );
    // Length
    stream << quint32( 0 );
    // Library size
    if( block->isLibrary() )
    {
        stream << quint32( 0 );
    }

    // Write the block properties
    int err = WriteBlockProperties( ctx, block );
    if( TreeSerializer::NoError != err )
    {
        return err;
    }

    // Store the position at the end of this block
    const qint64 endPos = ctx.device->pos();

    // Go back to the header position
    if( ! ctx.device->seek( startPos ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Write the final complete header
    if( 0 != childrenNb )
    {
        // Type
        quint32 type = ctx.getMetaBlockIndex( block->metaBlock() );
        // Add the library flag !
        stream << static_cast< quint32 >( type | LIBRARY_HAS_CHILDREN_FLAG );
        // Length including header
        stream << static_cast< quint32 >( endPos - startPos );
        // Library size
        stream << childrenNb;
    }
    else
    {
        // Type
        stream << ctx.getMetaBlockIndex( block->metaBlock() );
        // Length including header
        stream << static_cast< quint32 >( endPos - startPos );
    }

    // Go back to the end position
    if( ! ctx.device->seek( endPos ) )
    {
        return TreeSerializer::SeekFailed;
    }

    // Increment the blocks count
    ++( ctx.blocksCount );

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int DeflateBlock( Context& ctx, const Block* block, int childrenNb )
{
    if( ctx.device->isSequential() )
    {
        // Save the actual device
        QIODevice* device = ctx.device;

        // Create a temporary random access memory device
        QBuffer memDevice( ctx.buffer );

        // Set it as the context device
        ctx.device = & memDevice;

        // Do the block serialization in random access mode
        int err = DeflateBlockRandomAccess( ctx, block, childrenNb );

        // Restore the device
        ctx.device = device;

        // Handle error...
        if( TreeSerializer::NoError != err )
        {
            return err;
        }

        // Write the raw data
        ctx.device->write( memDevice.data(), memDevice.size() );
    }
    else
    {
        int err = DeflateBlockRandomAccess( ctx, block, childrenNb );
        // Handle error...
        if( TreeSerializer::NoError != err )
        {
            return err;
        }
    }

    return TreeSerializer::NoError;
}

int InflateBlock( Context& ctx, Block** block, int* childrenNb )
{
    const qint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    quint32 type;
    quint32 length;

    stream >> type;
    stream >> length;

    if( LIBRARY_HAS_CHILDREN_FLAG & type )
    {
        // Retrieve the number of child nodes
        quint32 childrenCount;
        stream >> childrenCount;

        *childrenNb = childrenCount;

        // Cleanup the type
        type = LIBRARY_HAS_CHILREN_MASK & type;
    }
    else
    {
        *childrenNb = 0;
    }

    // Retrieve the block meta type
    const MetaBlock* mb = ctx.getMetaBlock( type );

    if( ( K_NULL == mb ) )
    {
        // We did not find the corresponding block
        if( ( K_NULL != ctx.monitor ) &&
            ! ctx.monitor->event( TreeSerializer::UnknownBlockType,
                                  ctx.metaBlocksNames.at( type ) ) )
        {
            // Abort
            return TreeSerializer::UnknownBlockType;
        }
        else
        {
            // The user wants to continue anyway...

            // Store a NULL pointer for the caller
            *block = K_NULL;

            // Skip the block's data
            if( ! ctx.device->seek( startPos + length ) )
            {
                return TreeSerializer::SeekFailed;
            }

            return TreeSerializer::NoError;
        }
    }

    // Try to create an instance of that block...
    *block = mb->createBlock();

    if( K_NULL == ( *block ) )
    {
        if( ( K_NULL != ctx.monitor ) &&
            ! ctx.monitor->event( TreeSerializer::BlockInstantiationFailed,
                                  mb->blockClassName() ) )
        {
            // Abort
            return TreeSerializer::BlockInstantiationFailed;
        }
        else
        {
            // The user wants to continue anyway...

            // Store a NULL pointer for the caller
            *block = K_NULL;

            // Skip the block's data
            if( ! ctx.device->seek( startPos + length ) )
            {
                return TreeSerializer::SeekFailed;
            }

            return TreeSerializer::NoError;
        }
    }

    int err = ReadBlockProperties( ctx, *block );
    if( TreeSerializer::NoError != err )
    {
        ( *block )->destroy();
        *block = K_NULL;
        return err;
    }

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

int SkipBlock( Context& ctx, int* childrenNb )
{
    const qint64 startPos = ctx.device->pos();

    CREATE_STREAM( stream, ctx.device );

    quint32 type;
    quint32 length;

    stream >> type;
    stream >> length;

    if( LIBRARY_HAS_CHILDREN_FLAG & type )
    {
        // Retrieve the number of child nodes
        quint32 childrenCount;
        stream >> childrenCount;

        *childrenNb = childrenCount;
    }

    // Skip the block data
    if( ! ctx.device->seek( startPos + length ) )
    {
        return TreeSerializer::SeekFailed;
    }

    if( QDataStream::Ok != stream.status() )
    {
        return TreeSerializer::IOError;
    }

    return TreeSerializer::NoError;
}

} // namespace

int KoreSerializer::deflate( QIODevice* device,
                             const Block* block,
                             TreeSerializerMonitor* monitor ) const
{
    int err;

    // Pre allocate 2KB of memory for the buffer
    QByteArray buffer( 2048, 0x00 );

    // Create a context
    Context ctx( &buffer, device, monitor );

    // Do a pre-order visit of the tree without recursion as recursion
    // on big "deep" datasets could lead to a stack overflow.

    // We need that stack to avoid recursion
    QStack< const Block* > blocks;

    const Block* b = block;
    while( ( ! blocks.empty() ) || ( K_NULL != b ) )
    {
        if( K_NULL != b )
        {
            // Count the number of children to be serialized
            int childrenNb = 0;

            if( b->isLibrary() )
            {
                const Library* lib = static_cast< const Library* >( b );
                for( int i = 0; i < lib->size(); ++i )
                {
                    // Stack 'em in reverse order to serialize them in proper
                    // order
                    const Block* child = lib->at( lib->size() - i - 1 );
                    if( child->checkFlag( Block::Serializable ) )
                    {
                        // Increment the count of serialized children
                        ++ childrenNb;
                        blocks.push( child );
                    }
                }
            }

            err = DeflateBlock( ctx, b, childrenNb );
            if( NoError != err )
            {
                return err;
            }

            b = blocks.empty() ? K_NULL : blocks.pop();
        }
        else
        {
            b = blocks.pop();
        }
    }

    // Write the meta data in the file, at the end.
    err = WriteMetaData( ctx );
    if( NoError != err )
    {
        return err;
    }

    return NoError;
}

namespace
{
    struct LibContext
    {
        Library* lib;
        int childrenNb;
    };
}

int KoreSerializer::inflate( QIODevice* device,
                             Block** block,
                             TreeSerializerMonitor* monitor ) const
{
    int err;
    Block* root = K_NULL;
    QStack< LibContext > libs;
    Context ctx( K_NULL, device, monitor );

    if( device->isSequential() )
    {
        err = RequiresRandomAccess;
        goto cleanup;
    }

    err = ReadMetaData( ctx );
    if( NoError != err )
    {
        goto cleanup;
    }

    // First, inflate the "ROOT" element
    int childrenNb;
    err = InflateBlock( ctx, & root, & childrenNb );
    if( NoError != err )
    {
        goto cleanup;
    }

    if( K_NULL == root )
    {
        qWarning( "Could not inflate root Block" );
        return UnknownRootBlockType;
    }

    if( 0 != childrenNb )
    {
        LibContext libCtx = { static_cast< Library* >( root ), childrenNb };
        libs.push( libCtx );
    }

    while( ! libs.empty() )
    {
        // We could not load an unknown Block but the user did not request
        // cancellation of the inflate operation, we must get rid of the full
        // subtree.
        if( K_NULL == libs.top().lib )
        {
            // We need to skip all the child blocks and libraries
            err = SkipBlock( ctx, & childrenNb );
            if( TreeSerializer::NoError != err )
            {
                return err;
            }

            // Add the child blocks of this block to the current NULL lib count
            // as we do not want to load them anyway. This will cause the
            // unloaded parent library as well as its full subtree not to be
            // loaded.
            // -1 is for the block we have juste skipped.
            libs.top().childrenNb += childrenNb - 1;

            // Check if it still has more children, and pop it if it does not
            if( 0 == libs.top().childrenNb )
            {
                libs.pop();
            }
        }
        else
        {
            Block* b;
            err = InflateBlock( ctx, & b, & childrenNb );
            if( NoError != err )
            {
                goto cleanup;
            }

            // Add that block to the library which is at the top of the stack
            // it can be NULL if the client did not ask for termination on
            // a UnknownBlockType error.
            if( K_NULL != b )
            {
                libs.top().lib->addBlock( b );
            }

            // Decrement the children count at the top of the stack as we just
            // parsed one
            --libs.top().childrenNb;

            // Check if it still has more children, and pop it if it does not
            if( 0 == libs.top().childrenNb )
            {
                libs.pop();
            }

            // Put this block at the top of the stack if it is a library
            if( 0 != childrenNb )
            {
                LibContext libCtx = { static_cast< Library* >( b ),
                                      childrenNb };
                libs.push_back( libCtx );
            }
        }
    }

    // Store the result tree in the client's variable
    *block = root;

    // Set the error to none
    err = NoError;

cleanup:
    if( ( NoError != err ) && ( K_NULL != root ) )
    {
        // Do not leak the root if an error occured
        root->destroy();
    }

    return err;
}
