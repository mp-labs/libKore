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
#include <QtCore/QSettings>

#include <data/Block.hpp>
#include <data/BlockSettings.hpp>
#include <data/MetaBlock.hpp>
#include <data/BlockExtension.hpp>

#include <KoreApplication.hpp>
#include <KoreEngine.hpp>

using namespace Kore::data;
using namespace Kore;

MetaBlock::MetaBlock( const MetaBlock* superMetaBlock, const QMetaObject* mo )
    : _blockMetaObject( mo )
    , _superMetaBlock( superMetaBlock )
{
    blockName( tr( "MetaBlock for %1" ).arg( mo->className() ) );
}

void MetaBlock::library( Library* lib )
{
    Block::library( lib );

    if( hasParent() )
	{
        Kore::KoreEngine::RegisterMetaBlock( this );
	}
	else
	{
		clearExtensions();
        Kore::KoreEngine::UnregisterMetaBlock( this );
	}
}

const QMetaObject* MetaBlock::blockMetaObject() const
{
    return _blockMetaObject;
}

QString MetaBlock::blockClassName() const
{
    return QLatin1String( _blockMetaObject->className() );
}


void MetaBlock::clearExtensions()
{
    QList< BlockExtension* > extensions = _extensions.values();
	for(int i = 0; i < extensions.size(); i++)
	{
        extensions.at( i )->unregisterWithMetaBlock( this );
	}
}

QVariant MetaBlock::blockSetting( const QString& setting,
                                  const QVariant& defaultValue ) const
{
	QSettings settings;
    settings.beginGroup( K_BLOCK_SETTINGS_GROUP );
    QString key = QString( "%1.%2" ).arg( blockClassName() ).arg( setting );
    return settings.value( key, defaultValue );
}

BlockExtension* MetaBlock::blockExtension( const QString& name ) const
{
    return _extensions.value( name, K_NULL );
}

QList< BlockExtension* > MetaBlock::blockExtensions( const QString& name ) const
{
    return _extensions.values( name );
}

const QMultiHash< QString, BlockExtension* >& MetaBlock::extensions() const
{
	return _extensions;
}

kbool MetaBlock::registerBlockExtension( BlockExtension* extension )
{
    qDebug( "Registering EXTENSION %s (%s) from block %s",
            qPrintable( extension->name() ),
            qPrintable( extension->extensionName() ),
            qPrintable( blockName() ) );
    _extensions.insertMulti( extension->extensionName(), extension );
	return true;
}

void MetaBlock::unregisterBlockExtension( BlockExtension* extension )
{
    qDebug( "Unregistering EXTENSION %s (%s) from block %s",
            qPrintable( extension->name() ),
            qPrintable( extension->extensionName() ),
            qPrintable( blockName() ) );
    _extensions.remove( extension->extensionName(), extension );
}

const MetaBlock* MetaBlock::superMetaBlock() const
{
    return _superMetaBlock;
}

void MetaBlock::destroyBlock( Block* b ) const
{
    QCoreApplication::removePostedEvents( b );
	delete b;
}
