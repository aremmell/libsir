/*
 * @file sir.hh
 * @brief C++ interface to libsir
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.3
 * @copyright The MIT License (MIT)
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
# define _SIR_HH_INCLUDED

# include "sir.h"
# include "sir/helpers.h"
# include "sir/internal.h"
# include <type_traits>
# include <exception>

/** The one and only namespace for libsir. */
namespace sir
{
    /**
     * @class adapter
     * @brief Defines the abstract interface for an adapter, which ultimately
     * becomes a public base class of `log` (there can be more than one).
     *
     * There is no requirement to implement the default variadic argument
     * interface, other than to declare the methods. `adapter` is designed to
     * provide flexibility with regards to the types taken as input for logging,
     * as well as return values, whether or not to throw exceptions, etc.
     *
     * For example, one could implement a boost adapter that takes boost::format
     * as an argument.
     */
    class adapter {
    protected:
        adapter() = default;
        virtual ~adapter() = default;

    public:
        virtual bool debug(const char* format, ...) noexcept = 0;
        virtual bool info(const char* format, ...) noexcept = 0;
        virtual bool notice(const char* format, ...) noexcept = 0;
        virtual bool warn(const char* format, ...) noexcept = 0;
        virtual bool error(const char* format, ...) noexcept = 0;
        virtual bool crit(const char* format, ...) noexcept = 0;
        virtual bool alert(const char* format, ...) noexcept = 0;
        virtual bool emerg(const char* format, ...) noexcept = 0;
    };

    /**
     * @class default_adapter
     * @brief The default adapter implementation.
     *
     * Utilizes the same code path that the C interface itself does, in order to
     * achieve maximum performance (i.e., no unnecessary bloat is added).
     */
    class default_adapter : public adapter {
    public:
        default_adapter() = default;
        virtual ~default_adapter() = default;

        /** The equivalent of calling ::sir_debug directly. */
        bool debug(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_DEBUG, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_info directly. */
        bool info(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_INFO, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_notice directly. */
        bool notice(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_NOTICE, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_warn directly. */
        bool warn(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_WARN, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_error directly. */
        bool error(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ERROR, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_crit directly. */
        bool crit(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_CRIT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_alert directly. */
        bool alert(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ALERT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** The equivalent of calling ::sir_emerg directly. */
        bool emerg(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_EMERG, format, args);
            _SIR_L_END();
            return ret;
        }
    };

    template<bool RAII = true, class... TAdapters>
    class log : public TAdapters... {
        static_assert(std::is_base_of_v<adapter, TAdapters...>,
            "TAdapters must derive from adapter");
    public:
        log() : TAdapters()... {
            if (RAII && !init())
                throw std::exception(); // TODO
        }

        virtual ~log() {
            if (RAII && !cleanup())
                SIR_ASSERT(false);
        }

        bool init() const noexcept {
            sirinit si;
            return sir_makeinit(&si) && sir_init(&si);
        }

        bool cleanup() const noexcept {
            return sir_cleanup();
        }
    };

    using default_log = log<true, default_adapter>;
} // ! namespace sir

#endif // !_SIR_HH_INCLUDED
