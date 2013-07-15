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

#include <KoreEngine.hpp>

#include <event/KoreEvent.hpp>
#include <event/ErrorEvent.hpp>

#include <parallel/Tasklet.hpp>

#include <plugin/Module.hpp>

#include <QtCore/QCoreApplication>
#include <QtCore/QtGlobal>
#include <QtCore/QThread>

#include <QtConcurrent/QtConcurrent>
#include <QtCore/QtDebug>

using namespace Kore;
using namespace Kore::data;
using namespace Kore::event;
using namespace Kore::parallel;
using namespace Kore::plugin;

/* TRANSLATOR Kore::KoreEngine */

Q_GLOBAL_STATIC( KoreEngine, koreEngineInstance )

KoreEngine::KoreEngine()
    : _modules( Block::System )
{
    addFlags( System );

    Q_ASSERT_X( ! koreEngineInstance.exists(),
                "KoreEngine",
                "Only one instance is allowed !" );

    blockName( tr( "Kore Engine" ) );
    K_ASSERT( _Instance == K_NULL )

    _modules.blockName( tr( "Modules" ) );
    addBlock( &_modules );
}

void KoreEngine::customEvent( QEvent* event )
{
    if( event->type() != KoreEvent::EventType() )
    {
        Library::customEvent( event );
        return; // This is not an event for us !
    }

    KoreEvent* kEvent = static_cast< KoreEvent* >(event);
    switch( kEvent->koreEventType() )
    {
    case KoreEvent::Error:
        {
            ErrorEvent* eEvent = static_cast< ErrorEvent* >( kEvent );
            emit error( eEvent->error(), eEvent->details() );
        }
        break;
    default:
        break;
    }
}

const QList< MetaBlock* > KoreEngine::MetaBlocks()
{
    return Instance()->_metaBlocksHash.values();
}

void KoreEngine::RegisterModule( Module* module )
{
    // Register the block
    Instance()->_modules.addBlock( module );
    Instance()->_modulesHash.insert( module->id(), module );

    // Register the module types
    QHash< int, Module* >& moduleTypes = Instance()->_moduleTypes;
    QList< int > types = module->_metaToModuleHash.keys();
    for( int i = 0; i < types.size(); ++i )
    {
        moduleTypes.insert( types.at( i ), module );
    }
}

void KoreEngine::UnregisterModule( Module* module )
{
    // Unregister the block
    Instance()->_modulesHash.remove( module->id() );

    // Unregister the module types
    QHash< int, Module* >& moduleTypes = Instance()->_moduleTypes;
    QList< int > types = module->_metaToModuleHash.keys();
    for( int i = 0; i < types.size(); ++i )
    {
        moduleTypes.remove( types.at( i ) );
    }
}

void KoreEngine::RegisterMetaBlock( MetaBlock* mb )
{
    QHash< QString, MetaBlock* >& mbsh = Instance()->_metaBlocksHash;
    K_ASSERT( ! mbsh.contains( mb->blockClassName() ) )
    mbsh.insert( mb->blockClassName(), mb );
}

void KoreEngine::UnregisterMetaBlock( MetaBlock* mb )
{
    Instance()->_metaBlocksHash.remove( mb->blockClassName() );
}

Block* KoreEngine::CreateBlock( const QString& name )
{
    const MetaBlock* mb = GetMetaBlock( name );
    return mb ? mb->createBlock() : K_NULL;
}

void KoreEngine::RunTasklet( Tasklet* tasklet, TaskletRunner::RunMode mode )
{
    // Find the runner.
    const TaskletRunner* runner = tasklet->metaTasklet()
            ? tasklet->metaTasklet()->bestRunner()
            : K_NULL;

     // Because the tasklet is its default runner as well !
    runner = runner ? runner : tasklet;

    switch( mode )
    {
    case TaskletRunner::Synchronous:
        // Run right here on the current thread.
        runner->run( tasklet );
        break;
    case TaskletRunner::Asynchronous:
        // TODO: Manage our own thread pool ? Would that be useful ?
        // Use a future, Qt ThreadPool.
        QtConcurrent::run( runner, &TaskletRunner::run, tasklet );
        break;
    default:
        qWarning( "Kore / Unknown running mode for tasklet %s",
                  qPrintable( tasklet->objectClassName() ) );
        break;
    }
}

const MetaBlock* KoreEngine::GetMetaBlock( const QString& name )
{
    return Instance()->_metaBlocksHash.value( name, K_NULL );
}

const Module* KoreEngine::GetModule( const QString& id )
{
    return Instance()->_modulesHash.value( id, K_NULL );
}

const Module* KoreEngine::GetModuleForUserType( int userType )
{
    return Instance()->_moduleTypes.value( userType, K_NULL );
}

void KoreEngine::Error( const QString& error, const QString& details )
{
    if( QThread::currentThread() == Instance()->thread() )
    {
        ErrorEvent event( error, details );
        QCoreApplication::sendEvent( Instance(), & event );
    }
    else
    {
        ErrorEvent* event = new ErrorEvent( error, details );
        QCoreApplication::postEvent( Instance(), event );
    }
}

KoreEngine* KoreEngine::Instance()
{
    return koreEngineInstance;
}
