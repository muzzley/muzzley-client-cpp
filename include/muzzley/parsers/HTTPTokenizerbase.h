/*
Copyright (c) 2014, Muzzley

Permission to use, copy, modify, and/or distribute this software for 
any purpose with or without fee is hereby granted, provided that the 
above copyright notice and this permission notice appear in all 
copies.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE 
AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL 
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
PERFORMANCE OF THIS SOFTWARE.
*/

#pragma once

#include <muzzley/config.h>
#ifdef HAVE_HTTP_CLIENT

// Generated by Bisonc++ V4.05.00 on Fri, 27 Jun 2014 01:36:57 +0100

#ifndef muzzleyHTTPTokenizerBase_h_included
#define muzzleyHTTPTokenizerBase_h_included

#include <exception>
#include <vector>
#include <iostream>

// $insert preincludes
#include <muzzley/parsers/HTTPinc.h>

namespace // anonymous
{
    struct PI__;
}

// $insert namespace-open
namespace muzzley
{


class HTTPTokenizerBase
{
    public:
// $insert tokens

#ifdef VERSION
#undef VERSION
#endif

    // Symbolic tokens:
    enum Tokens__
    {
        METHOD = 257,
        VERSION,
        URL,
        STATUS,
        CR_LF,
        COLON,
        STRING,
        SPACE,
        BODY,
        QMARK,
        EQ,
        E,
    };

// $insert STYPE
typedef int STYPE__;

    private:
        int d_stackIdx__;
        std::vector<size_t>   d_stateStack__;
        std::vector<STYPE__>  d_valueStack__;

    protected:
        enum Return__
        {
            PARSE_ACCEPT__ = 0,   // values used as parse()'s return values
            PARSE_ABORT__  = 1
        };
        enum ErrorRecovery__
        {
            DEFAULT_RECOVERY_MODE__,
            UNEXPECTED_TOKEN__,
        };
        bool        d_debug__;
        size_t      d_nErrors__;
        size_t      d_requiredTokens__;
        size_t      d_acceptedTokens__;
        int         d_token__;
        int         d_nextToken__;
        size_t      d_state__;
        STYPE__    *d_vsp__;
        STYPE__     d_val__;
        STYPE__     d_nextVal__;

        HTTPTokenizerBase();

        void ABORT() const;
        void ACCEPT() const;
        void ERROR() const;
        void clearin();
        bool debug() const;
        void pop__(size_t count = 1);
        void push__(size_t nextState);
        void popToken__();
        void pushToken__(int token);
        void reduce__(PI__ const &productionInfo);
        void errorVerbose__();
        size_t top__() const;

    public:
        void setDebug(bool mode);
}; 

inline bool HTTPTokenizerBase::debug() const
{
    return d_debug__;
}

inline void HTTPTokenizerBase::setDebug(bool mode)
{
    d_debug__ = mode;
}

inline void HTTPTokenizerBase::ABORT() const
{
    throw PARSE_ABORT__;
}

inline void HTTPTokenizerBase::ACCEPT() const
{
    throw PARSE_ACCEPT__;
}

inline void HTTPTokenizerBase::ERROR() const
{
    throw UNEXPECTED_TOKEN__;
}


// As a convenience, when including ParserBase.h its symbols are available as
// symbols in the class Parser, too.
#define HTTPTokenizer HTTPTokenizerBase

// $insert namespace-close
}

#endif


#endif