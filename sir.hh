/**
 * @file sir.hh
 * @brief libsir C++ wrapper.
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

#include <sir.h>
#include <sirhelpers.h>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>

/**
 * @defgroup cpp C++ wrapper
 * 
 * A simple C++ wrapper for libsir.
 * 
 * @addtogroup cpp @{
 */

namespace sir {
    
    /**
     * Technically, we could just use std::string, but
     * template over sirchar_t in case it may be defined
     * as a type other than char in the future (e.g., wchar_t).
     */
    typedef std::basic_string<sirchar_t> string_t;
    
    /**
     * Defines the public method to_string. Derive your types
     * from this in order to easily write the human-readable form
     * of objects to libsir.
     * 
     * @example
     *   sir::logger mylogger;
     *   mytype mytype1; // Derives from sir::printable.
     *   mylogger.info << "The value of mytype1 is: " << mytype1  << std::endl;
     */
    class printable {
    public:
        virtual const string_t to_string() const = 0;
    };

    ///////////////////////////////////////////////
    class liberror : public printable
    {
    public:
        liberror() { _code = sir_geterror(_message); }

        uint16_t code() const  { return _code; }
        const string_t message() const { return _message; }

        const string_t to_string() const final {
            std::stringstream strm("libsir error code ");
            strm << _code << ": " << _message;
            return strm.str();
        }

    protected:
        uint16_t _code;
        sirchar_t _message[SIR_MAXERROR];
    };

    inline std::ostream& operator<<(std::ostream& strm, printable const& p) {
        strm << p.to_string();
        return strm;
    }

    ///////////////////////////////////////////////
    class lib_exception : public std::exception, public liberror {
    public:
        lib_exception() : liberror() {
            _what = to_string();
        }

        virtual const char* what() const noexcept {
            static_assert(sizeof(char) == sizeof(sirchar_t), "libsir only supports char");
            return _what.c_str();
        }

    protected:
        string_t _what;
    };

    ///////////////////////////////////////////////
    typedef std::function<void(const string_t&)> strmsync_fn;

    template <typename TChar>
    class bufferT : public std::basic_stringbuf<TChar> {
        using base = std::basic_stringbuf<TChar>;
    public:
        explicit bufferT(const strmsync_fn& fn): _fn(fn) { }
        virtual ~bufferT() = default;

    protected:
        virtual int sync() override {
            if (base::pptr() != base::epptr()) {
                _fn(this->str());
                this->str("");
            }
            return 0;
        }

        strmsync_fn _fn;
    };

    ///////////////////////////////////////////////
    typedef bool (*libsir_fn)(const sirchar_t*, ...);

    template <typename TChar>
    class streamT : public std::basic_ostream<TChar> {
        using base = std::basic_ostream<TChar>;
    public:
        explicit streamT(libsir_fn fn) : base(&_buf),
            _buf(std::bind(&streamT::_on_sync, this, std::placeholders::_1)) {
            _fn = fn;
        }
        virtual ~streamT() = default;

    protected:
        virtual void _on_sync(const string_t& str) {
            if (!_fn(str.c_str()))
                base::setstate(base::failbit);
            else
                base::clear();
        }

       bufferT<TChar> _buf;
       libsir_fn _fn;
    };

    ///////////////////////////////////////////////
    template <typename TChar>
    class loggerT {
    public:
        loggerT() : debug(sir_debug), info(sir_info), notice(sir_notice),
                    warn(sir_warn), error(sir_error), crit(sir_crit),
                    alert(sir_alert), emerg(sir_emerg) { }
        virtual ~loggerT() = default;
        streamT<TChar> debug, info, notice, warn, error, crit, alert, emerg;
    };

    ///////////////////////////////////////////////
    class logger : public loggerT<sirchar_t> {
    public:
        explicit logger(sir_levels l_stdout = SIRL_DEFAULT, sir_options o_stdout = SIRO_DEFAULT,
            sir_levels l_stderr = SIRL_DEFAULT, sir_options o_stderr = SIRO_DEFAULT,
            sir_levels l_syslog = SIRL_DEFAULT, const string_t& procName = "") {
            
            reset();

            _si.d_stdout.levels = l_stdout;
            _si.d_stdout.opts   = o_stdout;
            _si.d_stderr.levels = l_stderr;
            _si.d_stderr.opts   = o_stderr;

    #ifndef SIR_NO_SYSLOG
            _si.d_syslog.levels = l_syslog;
    #endif

            if (!procName.empty())
                strncpy(_si.processName, procName.c_str(), SIR_MAXNAME);

            if (!(_valid = sir_init(&_si))) throw lib_exception();
        }

        virtual ~logger() { reset(); }

        virtual sirfileid_t addfile(const string_t& path, sir_levels levels = SIRL_DEFAULT,
            sir_options options = SIRO_DEFAULT) const {
            sirfileid_t id = sir_addfile(path.c_str(), levels, options);
            if (nullptr == id) throw lib_exception();
            return id;
        }

        virtual void remfile(sirfileid_t id) const {
            if (!sir_remfile(id)) throw lib_exception();
        }

        virtual void settextstyle(sir_level level, sir_textstyle style) {
            if (!sir_settextstyle(level, style)) throw lib_exception();
        }

        virtual void resettextstyles() const {
            if (!sir_resettextstyles()) throw lib_exception();
        }

        virtual liberror geterror() const { return liberror(); }

        virtual bool valid() const { return _valid; }

        virtual void reset() {
            memset(&_si, 0, sizeof(sirinit));
            _valid = !sir_cleanup();
        }

    protected:
        sirinit       _si;
        volatile bool _valid = false;
    };
}  /* !namespace sir */

/** @} */

#endif /* !_SIR_HH_INCLUDED */
