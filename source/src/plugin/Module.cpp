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

#include <plugin/Module.hpp>
#include <KoreModule.hpp>

#define K_BLOCK_SUPER_TYPE  Kore::data::Library
#define K_BLOCK_TYPE        Kore::plugin::Module
#include <data/BlockMacros.hpp>
K_BLOCK_IMPLEMENTATION

using namespace Kore;
using namespace Kore::data;
using namespace Kore::plugin;

Module::Module()
{
    addFlags( Static );
}

Module::~Module()
{
}

int Module::userTypeIdForModuleTypeId( quint16 moduleType ) const
{
    return _moduleToMetaHash.value( moduleType, QMetaType::UnknownType );
}

quint16 Module::moduleTypeIdForUserTypeId( int metaTypeId ) const
{
    return _metaToModuleHash.value( metaTypeId, 0xffff );
}

void Module::registerLoadable( Loadable::Instantiator instantiator )
{
    _instantiators.append( instantiator );
}

void Module::registerModuleType( int runtimeId, quint16 moduleId )
{
    _metaToModuleHash.insert( runtimeId, moduleId );
    _moduleToMetaHash.insert( moduleId, runtimeId );
}

bool Module::load()
{
    if( hasParent() )
    {
        // The module was already loaded
        qWarning( "Module %s (%s) is already lodaded",
                  qPrintable( name() ),
                  qPrintable( version() ) );
        return false;
    }

    qDebug( "Loading module %s (%s)",
            qPrintable( name() ),
            qPrintable( version() ) );

    // Set the block name
    blockName( name() );

    // Load the loadables
    for( int i = 0; i < _instantiators.size(); ++i )
    {
        Loadable* loadable = ( _instantiators.at( i ) )();
        if( K_NULL != loadable )
        {
            addBlock( loadable );
        }
        else
        {
            qWarning( "Failed to create Loadable instance @ %p",
                      _instantiators.at( i ) );
            return false;
        }
    }

    // Register the types
    registerModuleTypes();

    // Register the module
    Kore::KoreEngine::RegisterModule( this );

    return true;
}

bool Module::unload()
{
    // Check that all loadables can be unloaded...
    for( kint i = 0; i < size(); ++i )
    {
        if( ! at( i )->canUnload() )
        {
            return false;
        }
    }

    // Unregister the module
    Kore::KoreEngine::UnregisterModule( this );

    // If it's okay, unload the module
    clear();

    return true;
}
