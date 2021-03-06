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

#include <data/LibraryT.hpp>
#include <parallel/TaskletRunner.hpp>

#include <QtCore/QHash>
#include <QtCore/QVector>

namespace Kore {

namespace plugin { class Module; }

class KoreApplication;

class KoreExport KoreEngine : public Kore::data::Library
{
    friend class KoreApplication;

    Q_OBJECT

public:
    KoreEngine();

protected:
    virtual void customEvent( QEvent* event );

signals:
    void error( const QString& error, const QString& details );

public:
    static const QList< Kore::data::MetaBlock* > MetaBlocks();

    static void RegisterModule( Kore::plugin::Module* module );
    static void UnregisterModule( Kore::plugin::Module* module );

    static void RegisterMetaBlock( Kore::data::MetaBlock* mb );
    static void UnregisterMetaBlock( Kore::data::MetaBlock* mb );
    static Kore::data::Block* CreateBlock( const QString& name );

    template< typename T >
    static T* CreateBlockT( const QString& name )
        { return static_cast< T* >( CreateBlock( name ) ); }

    template< typename T >
    static void RegisterTaskletRunner( Kore::parallel::TaskletRunner* runner )
        { T::StaticMetaTasklet()->registerTaskletRunner( runner ); }
    static void RunTasklet( Kore::parallel::Tasklet* tasklet,
                            Kore::parallel::TaskletRunner::RunMode mode );

    static const Kore::data::MetaBlock* GetMetaBlock( const QString& name );

    static const Kore::plugin::Module* GetModule( const QString& id );
    static const Kore::plugin::Module* GetModuleForUserType( int userType );

    static void Error( const QString& error,
                       const QString& details = QString() );

    static KoreEngine* Instance();

private:
    Kore::data::LibraryT< Kore::plugin::Module >    _modules;
    QHash< QString, Kore::data::MetaBlock* >        _metaBlocksHash;
    QHash< int, Kore::plugin::Module* >             _moduleTypes;
    QHash< QString, Kore::plugin::Module* >         _modulesHash;
};

}
