#pragma once
#include <Arduino.h>
#include <memory>

inline String str_sprintf( const char *fmt, ... ) __attribute__( ( format( printf, 1, 2 ) ) );

inline String str_sprintf( const char *fmt, ... )
{
    va_list args;
    va_start( args, fmt );

    va_list args_copy;
    va_copy( args_copy, args );

    // BUGBUG: Investigate a vasprintf here and String::copy() to get move semantics
    // on the return.
    // Could Save one complete format, a copy, and an alloc and we're called a
    // few times a second.
    int requiredLen = vsnprintf( nullptr, 0, fmt, args ) + 1;
    va_end( args );

    if( requiredLen <= 0 )
    {
        va_end( args_copy );
        return {};
    }

    std::unique_ptr<char[]> str = std::make_unique<char[]>( requiredLen );
    vsnprintf( str.get(), requiredLen, fmt, args_copy );
    va_end( args_copy );

    String retval;
    retval.reserve( requiredLen ); // At least saves one scan of the buffer.

    retval = str.get();
    return retval;
}