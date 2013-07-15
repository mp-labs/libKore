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

#include <QtCore/QList>
#include <QtCore/QMetaType>

#include <KoreTypes.hpp>
#include <data/LibraryT.hpp>

#include "Loadable.hpp"

namespace Kore {

class KoreEngine;

namespace plugin {

class KoreExport Module : public Kore::data::LibraryT< Loadable >
{
    Q_OBJECT
    K_BLOCK

    Q_PROPERTY( QString name READ name DESIGNABLE true CONSTANT FINAL )
    Q_PROPERTY( QString author READ author DESIGNABLE true CONSTANT FINAL )
    Q_PROPERTY( QString url READ url DESIGNABLE true CONSTANT FINAL )
    Q_PROPERTY( QString version READ version DESIGNABLE true CONSTANT FINAL )

    friend class Kore::KoreEngine;

protected:
    Module();
    virtual ~Module();

public:
    virtual kbool load();
    virtual kbool unload();

public:
    virtual QString id() const = K_VIRTUAL;
    virtual QString name() const = K_VIRTUAL;
    virtual QString author() const = K_VIRTUAL;
    virtual QString url() const = K_VIRTUAL;
    virtual QString version() const = K_VIRTUAL;

    int userTypeIdForModuleTypeId( quint16 moduleType ) const;
    quint16 moduleTypeIdForUserTypeId( int userType ) const;

    void registerLoadable( Loadable::Instantiator instantiator );

protected:
    virtual void registerModuleTypes() = K_VIRTUAL;
    void registerModuleType( int qtMetaId, quint16 moduleId );

private:
    QList< Loadable::Instantiator > _instantiators;
    QHash< int, quint16 >           _metaToModuleHash;
    QHash< quint16, int >           _moduleToMetaHash;
};

}}

#define K_MODULE \
    friend class Kore::data::MetaBlock;\
    protected:\
        virtual void registerModuleTypes();\
    public:\
        static const Kore::plugin::Module* Instance();\
        static bool RegisterLoadable( Kore::plugin::Loadable::Instantiator );\
    private:\
        static Kore::plugin::Module* PrivateInstance();

#define K_MODULE_REGISTER_META_TYPE( type ) \
    registerModuleType( qMetaTypeId< type >(), moduleTypeIdx++ );\
    qRegisterMetaTypeStreamOperators< type >();
