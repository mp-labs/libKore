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

inline kint Kore::data::Block::index() const
{
    return _index;
}

inline Kore::data::Library* Kore::data::Block::library()
{
    return _library;
}

inline const Kore::data::Library* Kore::data::Block::library() const
{
    return _library;
}

template< typename T >
inline T* Kore::data::Block::parent()
{
    return static_cast< T* >( _library );
}

template< typename T >
inline const T* Kore::data::Block::parent() const
{
    return static_cast< const T* >( _library );
}

inline kbool Kore::data::Block::hasParent() const
{
     return K_NULL != _library;
}

inline kbool Kore::data::Block::checkFlag( kuint64 flag ) const
{
    return 0 != ( _flags & flag );
}

inline kbool Kore::data::Block::isBeingDeleted() const
{
    return checkFlag( IsBeingDeleted );
}

template<typename T>
inline kbool Kore::data::Block::fastInherits() const
{
    const QMetaObject* classMO = & T::staticMetaObject;

    // Speedup proposal... Far more efficient based on pointer arithmetic.
    // What about cache locality ?
    // Avoid using qMetaCast based on strcmp stuff...
    for( const QMetaObject* mo = this->metaObject(); mo; mo = mo->superClass() )
    {
        if( mo == classMO )
        {
            return true;
        }
    }

    return false;
}
