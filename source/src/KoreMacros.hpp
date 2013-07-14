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

#include <QtCore/QtGlobal>
#include <assert.h>

/*!
 * @def K_NULL
 * @brief Null pointer.
 */
#define K_NULL      0

#define K_VIRTUAL   0

#define K_NULL_REF( type ) ( reinterpret_cast< type* >( K_NULL ) )

#ifndef K_DEBUG
#define _K_SAFE_BUILD
#endif /*_DEBUG*/

// Utilities
#define K_BLOCK_STR( x )    #x
#define K_BLOCK_XSTR( x )   K_BLOCK_STR( x )

/* Assert */
#ifdef K_DEBUG
#define K_ASSERT( expression )					assert( expression );
#else
#define K_ASSERT( expression )
#endif /*K_DEBUG*/
#define K_STRONG_ASSERT( expression )           assert( expression );

#define K_IS_ALIGNED( expression, alignment )\
    ( ! ( static_cast< size_t >( expression ) % alignment ) )

/* Inlining */
#define	_K_INLINE	inline

/* Unused */
#define _K_UNUSED( x )	x __attribute__( ( unused ) )

/* Memory alignment */
#if ( QT_POINTER_SIZE == 8 )
#   define _K_64
#endif

#ifdef _K_64
#define _K_DEFAULT_ALIGNMENT	16
#else
#define _K_DEFAULT_ALIGNMENT	8
#endif

#define _K_SSE_ALIGNMENT	16

#define _K_NEXT_ALIGNED_VALUE( value, alignment )\
    ( ( ( size_t ) value + ( alignment - 1 ) ) & ~( alignment - 1 ) )

#define _K_1TB	( 1024ll * 1024 * 1024 * 1024 )
#define _K_1GB	( 1024 * 1024 * 1024 )
#define _K_1MB	( 1024 * 1024 )
#define _K_1KB	( 1024 )

/* C++11 */
#define _K_DECL_OVERRIDE Q_DECL_OVERRIDE

#define K_FOURCC( a, b, c, d )\
    ( ( kuint ) ( ( ( a ) << 24 ) | ( ( b ) << 16 ) | ( ( c ) <<8 ) | ( d ) ) )

#define _K_PI	3.14159265358979323846

/* Min & Max */
#define K_MIN( a, b ) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )
#define K_MAX( a, b ) ( ( ( a ) > ( b ) ) ? ( a ) : ( b ) )

#ifdef _MSC_VER
#   define	_K_SSE_ALIGNED  __declspec( align( _K_SSE_ALIGNMENT ) )
#else
#   define	_K_SSE_ALIGNED  __attribute__( ( aligned( _K_SSE_ALIGNMENT ) ) )
#endif

#ifdef _K_BUILD_KORE
#   define KoreExport Q_DECL_EXPORT
#else
#   define KoreExport Q_DECL_IMPORT
#endif
