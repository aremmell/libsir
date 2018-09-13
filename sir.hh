/**
 * @file sir.hh
 * @brief libsir C++ wrapper.
 *
 * This file and accompanying source code originated from <https://github.com/ryanlederman/sir>.
 * If you obtained it elsewhere, all bets are off.
 *
 * @author Ryan M. Lederman <lederman@gmail.com>
 * @version 2.1.0
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

#include <cstdio>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <vector>
#include "sir.h"

/**
 * @defgroup cpp C++ wrapper
 *
 * C++ interface to libsir.
 *
 * @addtogroup cpp
 * @{
 */

/** The one and only namespace for libsir. */
namespace sir {

    /**
     * Technically, we could just use std::string, but
     * template over ::sirchar_t in case it may be defined
     * as a type other than char in the future (e.g., wchar_t).
     */
    typedef std::basic_string<sirchar_t>       string_t;
    typedef std::basic_stringstream<sirchar_t> stringstream_t;

    /** @class serializable */
    class serializable {
       public:
        virtual const string_t to_string() const = 0;

        static const string_t format(const string_t& format, ...) {
            if (format.empty()) return string_t();

            va_list args, copy;
            va_start(args, format);
            va_copy(copy, args);

            int needed = std::vsnprintf(NULL, 0, format.c_str(), copy);
            if (needed <= 0) return string_t();

            std::size_t            newsize = static_cast<std::size_t>(needed + 1);
            std::vector<sirchar_t> vec(newsize, '\0');

            int formatted = std::vsnprintf(&vec[0], newsize, format.c_str(), args);
            if (formatted <= 0) return string_t();

            return &vec[0];
        }
    }; /* !class serializable */

    /** For std::ostream serialization. */
    inline std::ostream& operator<<(std::ostream& strm, serializable const& p) {
        strm << p.to_string();
        return strm;
    }

    ///////////////////////////////////////////////
    class error : public serializable {
       public:
        error() { _code = sir_geterror(_message); }

        uint16_t       code() const { return _code; }
        const string_t message() const { return _message; }

        const string_t to_string() const final {
            return format("libsir error, code: %hu %s", _code, _message);
        }

       protected:
        uint16_t  _code;
        sirchar_t _message[SIR_MAXERROR];
    }; /* !class error */

    ///////////////////////////////////////////////
    class lib_exception : public std::exception, public error {
       public:
        lib_exception() : error() { _what = to_string(); }
        lib_exception(const string_t& what) { _what = what; }

        virtual const char* what() const noexcept {
            static_assert(sizeof(char) == sizeof(sirchar_t), "sirchar_t == char");
            return _what.c_str();
        }

       protected:
        string_t _what;
    }; /* !class lib_exception */

    ///////////////////////////////////////////////
    class adapter {
       protected:
        adapter()          = default;
        adapter(adapter&)  = delete;
        adapter(adapter&&) = delete;

       public:
        virtual ~adapter() = default;
    }; /* !class adapter */

    ///////////////////////////////////////////////
    class default_adapter : public adapter {
       public:
        default_adapter()                  = default;
        default_adapter(default_adapter&)  = delete;
        default_adapter(default_adapter&&) = delete;

        bool debug(const stringstream_t& strm) const { return debug(strm.str()); }
        bool debug(const string_t& message) const { return sir_debug(message.c_str()); }

        bool info(const stringstream_t& strm) const { return info(strm.str()); }
        bool info(const string_t& message) const { return sir_info(message.c_str()); }

        bool notice(const stringstream_t& strm) const { return notice(strm.str()); }
        bool notice(const string_t& message) const { return sir_notice(message.c_str()); }

        bool warn(const stringstream_t& strm) const { return warn(strm.str()); }
        bool warn(const string_t& message) const { return sir_warn(message.c_str()); }

        bool error(const stringstream_t& strm) const { return error(strm.str()); }
        bool error(const string_t& message) const { return sir_error(message.c_str()); }

        bool crit(const stringstream_t& strm) const { return crit(strm.str()); }
        bool crit(const string_t& message) const { return sir_crit(message.c_str()); }

        bool alert(const stringstream_t& strm) const { return alert(strm.str()); }
        bool alert(const string_t& message) const { return sir_alert(message.c_str()); }

        bool emerg(const stringstream_t& strm) const { return emerg(strm.str()); }
        bool emerg(const string_t& message) const { return sir_emerg(message.c_str()); }

#ifdef BOOST_FORMAT_HPP
        /** If `BOOST_FORMAT_HPP` is defined, the default adapter is extended with
         * methods that take `const boost::format&`.
         */
        bool debug(const boost::format& message) const { default_adapter::debug(message.str()); }
        bool info(const boost::format& message) const { default_adapter::info(message.str()); }
        bool notice(const boost::format& message)  const { default_adapter::notice(message.str()); }
        bool warn(const boost::format& message) const { default_adapter::warn(message.str()); }
        bool error(const boost::format& message) const { default_adapter::error(message.str()); }
        bool crit(const boost::format& message) const { default_adapter::crit(message.str()); }
        bool alert(const boost::format& message) const { default_adapter::alert(message.str()); }
        bool emerg(const boost::format& message) const { default_adapter::emerg(message.str()); }
#endif /* !BOOST_FORMAT_HPP */
    }; /* !class default_adapter */

    ///////////////////////////////////////////////
    class policy {
       public:
        enum dest { stdout = 1, stderr, syslog, file };

        policy()          = default;
        virtual ~policy() = default;

        virtual sir_levels  levels(dest d) const  = 0;
        virtual sir_options options(dest d) const = 0;
    }; /* !class policy */

    ///////////////////////////////////////////////
    class default_policy : public policy {
       public:
        sir_levels levels(dest d) const final {
            switch (d) {
                case stdout: return SIRL_DEFAULT;
                case stderr: return SIRL_DEFAULT;
                case syslog: return SIRL_DEFAULT;
                case file: return SIRL_DEFAULT;
                default: throw lib_exception("invalid destination");
            }
        }

        sir_options options(dest d) const final {
            switch (d) {
                case stdout: return SIRO_DEFAULT;
                case stderr: return SIRO_DEFAULT;
                case syslog: throw lib_exception("syslog options unavailable");
                case file: return SIRO_DEFAULT;
                default: throw lib_exception("invalid destination");
            }
        }
    }; /* !class default_policy */

    ///////////////////////////////////////////////
    template<class TAdapter = default_adapter, class TPolicy = default_policy>
    class logger_impl : public TAdapter {
       public:
        static_assert(std::is_base_of<adapter, TAdapter>::value, "TAdapter not a sir::adapter");
        static_assert(std::is_base_of<policy, TPolicy>::value, "TPolicy not a sir::policy");

        logger_impl(const string_t& name) : TAdapter() {
            init(name);
        }

        logger_impl() : TAdapter() { }
        virtual ~logger_impl() { sir_cleanup(); }

        virtual void init(const string_t& name) {
            memset(&_si, 0, sizeof(sirinit));
            _si.d_stdout.levels = _policy.levels(policy::stdout);
            _si.d_stdout.opts   = _policy.options(policy::stdout);
            _si.d_stderr.levels = _policy.levels(policy::stderr);
            _si.d_stderr.opts   = _policy.options(policy::stderr);
            _si.d_syslog.levels = _policy.levels(policy::syslog);

            if (!name.empty()) strncpy(_si.processName, name.c_str(), SIR_MAXNAME);

            if (!sir_init(&_si)) throw lib_exception();
        }

        virtual void set_stdout_levels(sir_levels levels) const {
            if (!sir_stdoutlevels(levels)) throw lib_exception();
        }

        virtual void set_stdout_options(sir_options options) const {
            if (!sir_stdoutopts(options)) throw lib_exception();
        }

        virtual void set_stderr_levels(sir_levels levels) const {
            if (!sir_stderrlevels(levels)) throw lib_exception();
        }

        virtual void set_stderr_options(sir_options options) const {
            if (!sir_stderropts(options)) throw lib_exception();
        }

        virtual void set_syslog_levels(sir_levels levels) const {
            if (!sir_sysloglevels(levels)) throw lib_exception();
        }

        virtual void set_file_levels(sirfileid_t id, sir_levels levels) const {
            if (!sir_filelevels(id, levels)) throw lib_exception();
        }

        virtual void set_file_options(sirfileid_t id, sir_options options) const {
            if (!sir_fileopts(id, options)) throw lib_exception();
        }

        virtual sirfileid_t addfile(const string_t& path) const {
            return addfile(path, _policy.levels(policy::file), _policy.options(policy::file));
        }

        virtual sirfileid_t addfile(const string_t& path, sir_levels levels, sir_options options) const {
            sirfileid_t id = sir_addfile(path.c_str(), levels, options);
            if (nullptr == id) throw lib_exception();
            return id;
        }

        virtual void remfile(sirfileid_t id) const {
            if (!sir_remfile(id)) throw lib_exception();
        }

        virtual void settextstyle(sir_level level, sir_textstyle style) const {
            if (!sir_settextstyle(level, style)) throw lib_exception();
        }

        virtual void resettextstyles() const {
            if (!sir_resettextstyles()) throw lib_exception();
        }

        virtual error geterror() const { return error(); }

       protected:
        TPolicy _policy;
        sirinit _si;
    }; /* !class logger_impl */

    typedef logger_impl<default_adapter, default_policy> logger;

}  // namespace sir

/** @} */

#endif /* !_SIR_HH_INCLUDED */
