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

#include <KoreTypes.hpp>
#include <KoreMacros.hpp>

#include <QtCore/QObject>
#include <QtCore/QVariant>

namespace Kore { namespace data {

class Library;
class MetaBlock;

/*!
 * @brief A Block is the root object of the the Kore library class hierarchy.
 *
 * Blocks are organized in memory as a tree. This allows operations such as
 * serialization to be performed simply and quickly. Moreover, this also
 * simplifies memory management aspects as the deletion of a node of the tree
 * triggers the proper cleanup of all its child nodes recursively.
 *
 * \sa	Kore::data::Library
 */
class KoreExport Block : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString blockName
                READ blockName WRITE blockName NOTIFY blockNameChanged
                STORED false DESIGNABLE false USER true )

    friend class Library;
    friend class MetaBlock;

public:
    /*!
     * @enum	Flags.
     *
     * The different flags to mark a Block with.
     */
    enum Flags
    {
        /// The block was allocated by its allocator
        Allocated =         0x1 << 0,
        /// The block was allocated using new
        Newed =             0x1 << 1,
        /// The block is currently being deleted
        IsBeingDeleted =    0x1 << 2,
        /// The block is Serializable: should be considered for serialization.
        Serializable =      0x1 << 3,
        /// The block is browsable, understand GUI navigable.
        Browsable =         0x1 << 4,
        /// The block is editable, users can modify it.
        Editable =          0x1 << 5,
        /// The block is statically allocated, disables memory management
        Static =            0x1 << 6,
        /// The block is a system library statically allocated by the system
        System =            Static,
        /// The block is owned by the system (can not be deleted by the user).
        SystemOwned =       0x1 << 7,
        /// MAX FLAG for subclasses flags
        MAX_FLAG =          0x1 << 8,
        /// MAX MAX FLAG to set the enumeration size (64 bits)
        MAX_MAX_FLAG =      0x1 << 63
    };

    enum Properties
    {
        BlockTypeName = 1,      //! Pretty name for the type
        BlockPropertiesName,    //! Pretty name for the properties
        BlockSettings,          //! Settings for the block (default values...)
        MAX_PROPERTY
    };

    enum OptimizationCauses
    {
        None,
        DeserializationComplete,
        MAX_OPTIMIZATION_CAUSE
    };

protected:
    /*!
     * @brief Default constructor.
     *
     * This default constructor is protected as a Block should not be
     * instantiated as-is but rather sub-classed and specialized.
     */
    Block();

public:
    /*!
     * @brief Initialization routine.
     *
     * Because some generic initialization is not possible in the base
     * constructor (the object is not fully built), this virtual method call
     * allows to do some generic post instantiation initialization.
     * All overrides must call the parent implementation.
     */
    virtual void initialize( kuint64 flags = 0 );

    virtual ~Block();

public:
    /*!
     * @property Block::index
     * @brief The index is the position of this Block in its parent Library.
     */
    inline kint index() const;
protected:
    virtual void index( kint idx );

    /*!
     * @property Block::library
     * @brief The library is the parent node of this Block.
     * Classes that inherit Block can customize the behavior upon
     * insertion/removal in a tree hierarchy by customizing this method.
     */
    virtual void library( Library* lib );
public:
    inline Library* library();
    inline const Library* library() const;

    template< typename T >
    inline T* parent();
    template< typename T >
    inline const T* parent() const;

    inline kbool hasParent() const;

    /*!
     * @brief Optimization.
     *
     * @param cause The cause of the optimization.
     *
     * Block trees sometimes need to be optimized for performance or memory
     * footprint. This virtual method is where such optimizations should be
     * implemented.
     */
    virtual void optimize( int cause = None );

    QString objectClassName() const;

    virtual void blockName( const QString& name );
    virtual QString blockName() const;

    /*!
     * @brief Check the Block for a flag.
     *
     * Convenience method.
     *
     * @param[ in ] flag Flag to be checked for.
     * @return      true on success, false otherwise
     */
    inline kbool checkFlag( kuint64 flag ) const;

    /*!
     * \brief isLibrary
     * \return true if this block is indeed a library, false otherwise
     */
    virtual kbool isLibrary() const;

    /*!
     * \brief isBeingDeleted
     * \return true if this block is in the process of being deleted
     */
    inline kbool isBeingDeleted() const;

    /*!
     * @brief	The object's meta block.
     *
     * This method returns a const pointer to the MetaBlock of the object.
     *
     * @return MetaBlock* the object's meta block.
     */
    virtual const MetaBlock* metaBlock() const;
    static const MetaBlock* StaticMetaBlock();

    /*!
     * Inheritance checks
     */
    template< typename T >
    kbool fastInherits() const;
    kbool fastInherits( const MetaBlock* mb ) const;

protected:
    /*!
     * @brief	Adds a flag to the Block.
     *
     * Convenience method.
     *
     * @param[in] flag	Flag to be added.
     */
    void addFlags( kuint64 flag );
    /*!
     * @brief	Removes a flag from the Block.
     *
     * Convenience method.
     *
     * @param[in] flag	Flag to be removed.
     */
    void removeFlag( kuint64 flag );

signals:
    void blockNameChanged( const QString& name );
    void blockInserted();
    void blockRemoved();
    void blockDeleted();
    void indexChanged( kint oldIndex, kint newIndex );

public slots:
    /*!
     * @brief	Destroy the block.
     */
    virtual bool destroy();

public:
    static QVariant DefaultBlockProperty( kint property );

private:
    // Better alignment: pointers first (32/64...)
    Library*    _library;	//! The parent library
    // Members afterwards
    kuint64     _flags;		//!	The block flags
    kint        _index;		//! The block Index of this Block in its Library.
};

} /* namespace data */ } /* namespace Kore */

#include "Block.inl"

#define K_BLOCK \
    friend class Kore::data::Block;\
    \
    public:\
        static const Kore::data::MetaBlock* StaticMetaBlock();\
        virtual const Kore::data::MetaBlock* metaBlock() const;\
        \
    private:
