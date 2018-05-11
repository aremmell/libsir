/**
 * @file sir.h
 * @brief Public interface.
 * 
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 * 
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @version 2.0.0
 * @copyright
 * 
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Ryan M. Lederman
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _SIR_HH_INCLUDED
#define _SIR_HH_INCLUDED

#include <string>
#include <sstream>
#include <memory>
#include <sir.h>

namespace sir
{   
    typedef std::basic_string<sirchar_t> string_t;
    typedef bool (*sirlogfn_t)(const sirchar_t*, ...);

    template<typename _TChar>
    class bufferT : public std::basic_stringbuf<_TChar>
    {
    public:
        explicit bufferT(sirlogfn_t fn) : _fn(fn) { }
    
    protected:
        virtual int sync() override {
            if (_fn) {
                bool call = _fn(this->str().c_str());
                this->str("");
            }
            return 0;
        }

        sirlogfn_t _fn;
    };

    template<typename _TChar>
    class stream : public std::basic_ostream<_TChar>
    {
    public:
        explicit stream(sirlogfn_t fn)
            : _buf(fn), std::basic_ostream<_TChar>(&_buf) { }

    protected:
        bufferT<_TChar> _buf;
    };

    template<typename _TStream>
    class loggerT
    {
    public:
        loggerT() : debug(sir_debug), info(sir_info), notice(sir_notice),
            warn(sir_warn), error(sir_error), crit(sir_crit),
            alert(sir_alert), emerg(sir_emerg) { }

        _TStream debug;
        _TStream info;
        _TStream notice;
        _TStream warn;
        _TStream error;
        _TStream crit;
        _TStream alert;
        _TStream emerg;
    };

    class logger : public loggerT<stream<sirchar_t>>
    {
    };

} /* !namespace sir */

#endif /* !_SIR_HH_INCLUDED */
