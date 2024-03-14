/**
 * @file sir.hh
 *
 * @brief C++ interface to libsir
 *
 * @version 2.2.5
 *
 * -----------------------------------------------------------------------------
 *
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2018-2024 Ryan M. Lederman <lederman@gmail.com>
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
 *
 * -----------------------------------------------------------------------------
 */

#ifndef _SIR_HH_INCLUDED
# define _SIR_HH_INCLUDED

# include "sir.h"
# include "sir/helpers.h"
# include "sir/internal.h"
# include <type_traits>
# include <stdexcept>
# include <algorithm>
# include <exception>
# include <iostream>
# include <cstring>
# include <memory>
# include <tuple>
# include <string>
# include <array>

# if HAS_INCLUDE(<format>) && !defined (SIR_NO_STD_FORMAT) && \
     !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)
#  include <format>
#  define __SIR_HAVE_STD_FORMAT__
# endif
# if HAS_INCLUDE(<boost/format.hpp>) && !defined(SIR_NO_BOOST_FORMAT)
#  include <boost/format.hpp>
#  define __SIR_HAVE_BOOST_FORMAT__
# endif
# if HAS_INCLUDE(<fmt/format.h>) && !defined(SIR_NO_FMT_FORMAT)
#  define FMT_HEADER_ONLY
#  include <fmt/format.h>
#  define __SIR_HAVE_FMT_FORMAT__
# endif
# if !defined(SIR_NO_STD_IOSTREAM)
#  include <iostream>
# endif

/**
 * @addtogroup public
 * @{
 *
 * @addtogroup cpp C++ Wrapper
 * @{
 */

/** libsir C++ wrapper implementation. */
namespace sir
{
    /**
     * @struct error
     * @brief Contains basic information about an error.
     *
     * Provides the default error message formatting (SIR_ERRORFORMAT). In
     * the event that the default formatting is undesirable, or to obtain
     * additional information about an error, use the error_info struct.
     *
     * @see SIR_ERRORFORMAT
     * @see error_info
     */
    struct error {
        uint16_t code {};    /**< Numeric error code (see sir_errorcode). */
        std::string message; /**< Formatted error message. */

        constexpr bool is_os_error() const noexcept {
            return code == SIR_E_PLATFORM;
        }

        static error from_last_error() {
            std::string message(SIR_MAXERROR, '\0');
            const auto code = sir_geterror(message.data());
            return { code, std::move(message) };
        }
    };

    /**
     * @struct error_info
     * @brief Contains all available information about an error.
     *
     * Provides extended information about an error that occurred in order to
     * allow for custom error handling. If the numeric error code and default
     * error message formatting are sufficient, use the error struct instead.
     *
     * @see error
     */
    struct error_info : public error {
        std::string func;       /**< Name of the function in which the error occurred. */
        std::string file;       /**< Name of the file in which the error occurred. */
        uint32_t line {};       /**< Line number at which the error occurred. */
        int os_code {};         /**< If an OS/libc error, the associated code. */
        std::string os_message; /**< If an OS/libc error, the associated message. */

        static error_info from_last_error() {
            sir_errorinfo errinfo {};
            sir_geterrorinfo(&errinfo);
            return { { errinfo.code, errinfo.msg }, errinfo.func, errinfo.file,
                errinfo.line, errinfo.os_code, errinfo.os_msg };
        }
    };

    /**
     * @class exception
     * @brief The exception type thrown by libsir.
     *
     * Derives from std::runtime_error in order to be caught easily by standard
     * try/catch blocks, as well as to provide useful error messages by way of
     * the std::exception::what() method.
     *
     * To obtain additional information about the error that caused an exception,
     * use error_info.
     *
     * @see error_info
     */
    class exception : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;

        exception() = delete;
        explicit exception(const error& err) : exception(err.message) { }
        ~exception() override = default;
    };

    /**
     * @class policy
     * @brief Defines the partially abstract interface for a policy which
     * controls the behavior of logger at runtime.
     *
     * @note Only the default constructors may be used. Other constructors will
     * not be called, and if the default constructor is deleted, compilation
     * errors will result.
     */
    class policy {
    protected:
        policy() = default;
        virtual ~policy() = default;

    public:
        /**
         * Called by logger before initializing libsir. Provides the policy with
         * an opportunity to customize the initial configuration.
         *
         * @see sirinit
         * @see sir_makeinit
         *
         * @note For the default configuration, pass the address of data to
         * ::sir_makeinit.
         *
         * @note Most, if not all of the options chosen during initialization
         * can be modified post-initialization.
         *
         * @param data Reference to a ::sirinit struct representing the initial
         * configuration.
         *
         * @returns true if data is ready to pass to ::sir_init, or false if an
         * error occurred. Returning false will cause logger to abort the
         * initialization process, and possibly throw an exception.
         */
        virtual bool get_init_data(sirinit& data) const noexcept = 0;

        /**
         * Called by logger immediately after libsir has been initialized. This
         * is the moment in time which should be utilized for further configuration
         * than is possible via ::sir_init alone.
         *
         * Some potential actions include:
         * - Setting the color mode for stdio (::sir_setcolormode)
         * - Setting text styles on a per-level basis for stdio (::sir_settextstyle)
         * - Adding log files (::sir_addfile)
         * - Loading plugins (::sir_loadplugin)
         *
         * @returns true if configuration was completed successfully, or false if
         * an error occurred. Returning false will cause logger to abort the
         * initialization process, and possibly throw an exception.
         */
        virtual bool on_init_complete() const noexcept = 0;

        /**
         * Determines whether or not exceptions are thrown in the event that an
         * unercoverable error is encountered by logger or its associated
         * adapter(s).
         *
         * @note If exceptions are not enabled, the caller of a method that could
         * potentially fail is responsible for obtaining last error information
         * and reacting accordingly.
         *
         * @returns true if logger should throw exceptions, or false if errors
         * should simply be communicated through a pass/fail Boolean return value.
         */
        static constexpr bool throw_on_error() noexcept {
            return false;
        }
    };

    /** Ensures that T derives from policy. */
    template<typename T>
    concept DerivedFromPolicy = std::is_base_of_v<policy, T>;

    /**
     * @class default_policy
     * @brief In the event that no custom configuration or behavior is desired,
     * provides defaults for everything.
     *
     * - Uses all default values for ::sir_init
     * - Performs no post-initialization configuration
     * - Exceptions are thrown when errors are encountered
     */
    class default_policy : public policy {
    public:
        default_policy() = default;
        ~default_policy() override = default;

        bool get_init_data(sirinit& data) const noexcept final {
            return sir_makeinit(&data);
        }

        constexpr bool on_init_complete() const noexcept final {
            return true;
        }

        static constexpr bool throw_on_error() noexcept {
            return true;
        }
    };

    /**
     * @class adapter
     * @brief Defines the abstract interface for an adapter, which ultimately
     * becomes a public base class of logger (there can be more than one).
     *
     * adapter is designed to provide flexibility and extensibility in relation
     * to the public interface that is implemented by a logger.
     *
     * @note One must take care to ensure that the methods implemented by an
     * adapter can coexist with any other adapters that are applied to the logger
     * template.
     */
    class adapter {
    protected:
        adapter() = default;
        virtual ~adapter() = default;
    };

    /**
     * @brief Handles a potential error; if an error is present and the policy
     * in place requires throwing an exception, retrieves the associated error
     * code and message from libsir, then throws.
     *
     * @tparam TPolicy A derived class of policy which determines whether
     * or not to throw exceptions upon encountering an error.
     *
     * @param expr An expression that is evaluated against false. If false,
     *  an error is determined to have occurred and an exception will be thrown
     * if the policy requires it.
     *
     * @returns bool If no exception was thrown, the return value is expr (true
     * if no error occurred, false otherwise).
     */
    template<DerivedFromPolicy TPolicy>
    static inline bool throw_on_policy(bool expr) noexcept(false) {
        if (!expr) {
            SIR_ASSERT(expr);
            if constexpr(TPolicy::throw_on_error()) {
                throw exception(error::from_last_error());
            }
        }
        return expr;
    }

    /**
     * @class default_adapter
     * @brief The default adapter implementation.
     *
     * Utilizes the same code path that the C interface itself does, in order to
     * achieve maximum performance (i.e., no unnecessary bloat is added).
     *
     * @tparam TPolicy A derived class of policy which controls the behavior
     * of logger and by association, its adapters.
     */
    template<DerivedFromPolicy TPolicy>
    class default_adapter : public adapter {
    public:
        default_adapter() = default;
        ~default_adapter() override = default;

        /** Logs a debug message (see ::sir_debug). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool debug(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_DEBUG, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs an informational message (see ::sir_info). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool info(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_INFO, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs a notice message (see ::sir_notice). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool notice(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_NOTICE, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs a warning message (see ::sir_warn). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool warn(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_WARN, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs an error message (see ::sir_error). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool error(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ERROR, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs a critical message (see ::sir_crit). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool crit(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_CRIT, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs an alert message (see ::sir_alert). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool alert(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ALERT, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }

        /** Logs an emergency message (see ::sir_emerg). */
        PRINTF_FORMAT_ATTR(2, 3)
        bool emerg(PRINTF_FORMAT const char* format, ...) const {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_EMERG, format, args);
            _SIR_L_END();
            return throw_on_policy<TPolicy>(ret);
        }
    };

# if defined(__SIR_HAVE_STD_FORMAT__)
    /**
     * @class std_format_adapter
     * @brief Adapter for std::format (when available).
     *
     * Enabled when std::format is available on this platform and SIR_NO_STD_FORMAT
     * is not defined.
     *
     * Allows for the use of std::format in place of (or in addition to) C-style
     * variadic argument methods.
     *
     * **Example:**
     *
     * ~~~
     * using my_logger = logger<true, default_policy, std_format_adapter>;
     * my_logger log;
     * log.info_std("This is from {}!", "std::format");
     * ~~~
     *
     * @see std_format_logger
     *
     * @tparam TPolicy A derived class of policy which controls the behavior
     * of logger and by association, its adapters.
     */
    template<DerivedFromPolicy TPolicy>
    class std_format_adapter : public adapter {
    public:
        std_format_adapter() = default;
        ~std_format_adapter() override = default;

        template<typename... Args>
        inline bool debug_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_debug("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool info_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_info("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool notice_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_notice("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool warn_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_warn("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool error_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_error("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool crit_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_crit("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool alert_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_alert("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool emerg_std(std::format_string<Args...> fmt, Args&&... args) const {
            const auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            const bool ret = sir_emerg("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }
    };
# endif // !__SIR_HAVE_STD_FORMAT__

# if defined(__SIR_HAVE_BOOST_FORMAT__)
    /**
     * @class boost_format_adapter
     * @brief Adapter for boost::format (when available).
     *
     * Enabled when boost::format is available on this platform and
     * SIR_NO_BOOST_FORMAT is not defined.
     *
     * Allows for the use of boost::format in place of (or in addition to)
     * C-style variadic argument methods.
     *
     * **Example:**
     *
     * ~~~
     * using my_logger = logger<true, default_policy, boost_format_adapter>;
     * my_logger log;
     * log.info_bf(boost::format("This is from %1%!" % "boost::format"));
     * ~~~
     *
     * @see boost_logger
     *
     * @tparam TPolicy A derived class of policy which controls the behavior
     * of logger and by association, its adapters.
     */
    template<DerivedFromPolicy TPolicy>
    class boost_format_adapter : public adapter {
    public:
        boost_format_adapter() = default;
        ~boost_format_adapter() override = default;

        bool debug_bf(const boost::format& fmt) const {
            const bool ret = sir_debug("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool info_bf(const boost::format& fmt) const {
            const bool ret = sir_info("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool notice_bf(const boost::format& fmt) const {
            const bool ret = sir_notice("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool warn_bf(const boost::format& fmt) const {
            const bool ret = sir_warn("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool error_bf(const boost::format& fmt) const {
            const bool ret = sir_error("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool crit_bf(const boost::format& fmt) const {
            const bool ret = sir_crit("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool alert_bf(const boost::format& fmt) const {
            const bool ret = sir_alert("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        bool emerg_bf(const boost::format& fmt) const {
            const bool ret = sir_emerg("%s", fmt.str().c_str());
            return throw_on_policy<TPolicy>(ret);
        }
    };
# endif // !__SIR_HAVE_BOOST_FORMAT__

# if defined(__SIR_HAVE_FMT_FORMAT__)
    /**
     * @class fmt_format_adapter
     * @brief Adapter for {fmt} (when available).
     *
     * Enabled when {fmt} is available on this platform and SIR_NO_FMT_FORMAT is
     * not defined.
     *
     * Allows for the use of fmt::format in place of (or in addition to) C-style
     * variadic argument methods.
     *
     * **Example:**
     *
     * ~~~
     * using my_logger = logger<true, default_policy, fmt_format_adapter>;
     * my_logger log;
     * log.info_fmt("This is from {}}!", "fmt::format"));
     * ~~~
     *
     * @see fmt_logger
     *
     * @tparam TPolicy A derived class of policy which controls the behavior
     * of logger and by association, its adapters.
     */
    template<DerivedFromPolicy TPolicy>
    class fmt_format_adapter : public adapter {
    public:
        fmt_format_adapter() = default;
        ~fmt_format_adapter() override = default;

        template<typename... Args>
        inline bool debug_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_debug("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool info_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_info("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool notice_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_notice("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool warn_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_warn("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool error_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_error("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool crit_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_crit("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool alert_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_alert("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }

        template<typename... Args>
        inline bool emerg_fmt(fmt::format_string<Args...> fmt, Args&&... args) const {
            const auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            const auto ret = sir_emerg("%s", str.c_str());
            return throw_on_policy<TPolicy>(ret);
        }
    };
# endif // !__SIR_HAVE_FMT_FORMAT__

# if !defined(SIR_NO_STD_IOSTREAM)
    /** Ensures that T derives from std::streambuf. */
    template<typename T>
    concept DerivedFromStreamBuf = std::is_base_of_v<std::streambuf, T>;

    /**
     * @class std_iostream_adapter
     * @brief Provides a std::iostream interface to libsir's logging functions.
     *
     * Enabled so long as SIR_NO_STD_IOSTREAM is not defined.
     *
     * Implements a public std::ostream member for each available logging level
     * (e.g. debug_stream, info_stream, ..., emerg_stream).
     *
     * **Example:**
     *
     * ~~~
     * using my_logger = logger<true, default_policy, std_iostream_adapter>;
     * my_logger log;
     * log.info_stream << "This is from std::iostream!" << std::endl;
     * ~~~
     *
     * @see std_iostream_logger
     *
     * @note Use std::endl or std::flush to indicate the end of a log message if
     * using the ostream << operator.
     *
     * @tparam TPolicy A derived class of policy which controls the behavior
     * of logger and by association, its adapters.
     */
    template<DerivedFromPolicy TPolicy>
    class std_iostream_adapter : public adapter {
    public:
        std_iostream_adapter() = default;
        ~std_iostream_adapter() override = default;

        template<typename TFunc>
        class buffer : public std::streambuf {
        public:
            using array_type = std::array<char_type, SIR_MAXMESSAGE>;

            buffer() = delete;

            explicit buffer(TFunc pfn) : _pfn(pfn) {
                [[maybe_unused]]
                bool ok = throw_on_policy<TPolicy>(pfn != nullptr && _arr != nullptr);
                reset();
            }

            ~buffer() override = default;

        protected:
            void reset() {
                if (_arr) {
                    _arr->fill('\0');
                    setp(_arr->data(), _arr->data() + _arr->size() - 1);
                }
            }

            bool write_out() {
                for (auto it = _arr->rbegin(); it != _arr->rend(); it++) {
                    if ((*it != '\0') && (*it == '\n')) {
                        *it = '\0';
                        break;
                    }
                }

                const bool write = _pfn ? _pfn(_arr->data()) : false;
                reset();
                return throw_on_policy<TPolicy>(write);
            }

            int_type overflow(int_type ch) override {
                if (ch != traits_type::eof() && write_out()) {
                    return ch;
                }
                return traits_type::eof();
            }

            int sync() override {
                return write_out() ? 0 : -1;
            }

            std::streamsize xsputn(const char_type* s, std::streamsize count) override {
                std::streamsize written = 0;
                do {
                    ptrdiff_t left = epptr() - pptr();
                    if (!left) {
                        int_type ch = overflow(traits_type::to_int_type(*(s + written)));
                        if (ch != traits_type::eof()) {
                            continue;
                        } else {
                            break;
                        }
                    }

                    auto this_write = std::min(static_cast<std::streamsize>(left),
                        count - written);
                    std::memcpy(pptr(), s + written, static_cast<std::size_t>(this_write));
                    pbump(static_cast<int>(this_write));
                    written += this_write;
                } while (written < count);

                return written;
            }

        private:
            std::shared_ptr<array_type> _arr = std::make_shared<array_type>();
            TFunc _pfn = nullptr;
        };

        template<DerivedFromStreamBuf TBuffer>
        class stream : public std::ostream {
        public:
            explicit stream(const TBuffer& buf) : std::ostream(&_buf), _buf(buf) {}
            ~stream() override = default;

        private:
            TBuffer _buf;
        };

        using log_func = bool(*)(const char*, ...);
        using buffer_type = buffer<log_func>;
        using stream_type = stream<buffer_type>;

        stream_type debug_stream  {buffer_type {&sir_debug}};
        stream_type info_stream   {buffer_type {&sir_info}};
        stream_type notice_stream {buffer_type {&sir_notice}};
        stream_type warn_stream   {buffer_type {&sir_warn}};
        stream_type error_stream  {buffer_type {&sir_error}};
        stream_type crit_stream   {buffer_type {&sir_crit}};
        stream_type alert_stream  {buffer_type {&sir_alert}};
        stream_type emerg_stream  {buffer_type {&sir_emerg}};
    };
# endif // SIR_NO_STD_IOSTREAM

    /** Utility template for obtaining the type of Nth item in a parameter pack. */
    template<size_t N, typename ...Ts>
    using NthTypeOf = typename std::tuple_element<N, std::tuple<Ts...>>::type;

    /** Ensures (at compile time) that all Ts are derived classes of TBase. */
    template<size_t N, typename TBase, typename ...Ts>
    consteval auto all_derived_from_t() {
        if constexpr(!std::is_base_of_v<TBase, NthTypeOf<N, Ts...>>)
            return std::false_type::value;
        if constexpr(N == 0)
            return std::true_type::value;
        else
            return all_derived_from_t<N - 1, TBase, Ts...>();
    }

    /** True if all Ts are derived classes of TBase, otherwise false. */
    template<class TBase, class ...Ts>
    concept DerivedFromT = all_derived_from_t<sizeof...(Ts) - 1, TBase, Ts...>();

    /**
     * @class logger
     * @brief The primary C++ interface to libsir.
     *
     * Instantiate this class in order to access libsir with all the benefits
     * of C++, including RAII initialization/cleanup, custom adapters, and more.
     *
     * @see default_logger
     *
     * @tparam RAII      Set to true to enable 'resource acquisition is
     *                   initialization' behavior (i.e., libsir is initialized by
     *                   the ctor, and cleaned up by the dtor). Set to false for
     *                   manual management of initialization/cleanup.
     * @tparam TPolicy   A policy class which will be responsible for certain
     *                   aspects of logger's behavior.
     * @tparam TAdapters One or more adapter classes whose public methods will be
     *                   exposed by this class.
     */
    template<bool RAII, DerivedFromPolicy TPolicy, template<class> class ...TAdapters>
    requires DerivedFromT<adapter, TAdapters<TPolicy>...>
    class logger : public TAdapters<TPolicy>... {
    public:
        logger() : TAdapters<TPolicy>()... {
            if constexpr(RAII) {
                [[maybe_unused]]
                bool ok = throw_on_policy<TPolicy>(_init());
            }
        }

        logger(const logger&) = delete;
        logger(const logger&&) = delete;

        ~logger() override {
            if constexpr(RAII) {
                if (!cleanup()) {
                    SIR_ASSERT(false);
                }
            }
        }

        logger& operator=(const logger&) = delete;
        logger& operator=(const logger&&) = delete;

        bool init() const noexcept {
            return is_initialized() ? false: _init();
        }

        bool cleanup() const noexcept {
            return is_initialized() ? sir_cleanup() : false;
        }

        bool is_initialized() const noexcept {
            return sir_isinitialized();
        }

        error get_error() const {
            return error::from_last_error();
        }

        error_info get_error_info() const {
            return error_info::from_last_error();
        }

        sirfileid add_file(const std::string& path, const sir_levels& levels,
            const sir_options& opts) const {
            const bool add = sir_addfile(path.c_str(), levels, opts);
            return throw_on_policy<TPolicy>(add);
        }

        bool rem_file(const sirfileid& id) const {
            const bool rem = sir_remfile(id);
            return throw_on_policy<TPolicy>(rem);
        }

        sirpluginid load_plugin(const std::string& path) const {
            const bool load = sir_loadplugin(path.c_str());
            return throw_on_policy<TPolicy>(load);
        }

        bool unload_plugin(const sirpluginid& id) const {
            const bool unload = sir_unloadplugin(id);
            return throw_on_policy<TPolicy>(unload);
        }

        bool set_file_levels(const sirfileid& id, const sir_levels& levels) const {
            const bool set = sir_filelevels(id, levels);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_file_options(const sirfileid& id, const sir_options& opts) const {
            const bool set = sir_fileopts(id, opts);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_text_style(const sir_level& level, const sir_textattr& attr,
            const sir_textcolor& fg, const sir_textcolor& bg) const {
            const bool set = sir_settextstyle(level, attr, fg, bg);
            return throw_on_policy<TPolicy>(set);
        }

        bool reset_text_styles() const {
            const bool reset = sir_resettextstyles();
            return throw_on_policy<TPolicy>(reset);
        }

        sir_textcolor make_rgb(const sir_textcolor& r, const sir_textcolor& g,
            const sir_textcolor& b) const {
            const bool make = sir_makergb(r, g, b);
            return throw_on_policy<TPolicy>(make);
        }

        bool set_color_mode(const sir_colormode& mode) const {
            const bool set = sir_setcolormode(mode);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_stdout_levels(const sir_levels& levels) const {
            const bool set = sir_stdoutlevels(levels);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_stdout_options(const sir_options& opts) const {
            const bool set = sir_stdoutopts(opts);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_stderr_levels(const sir_levels& levels) const {
            const bool set = sir_stderrlevels(levels);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_stderr_options(const sir_options& opts) const {
            const bool set = sir_stderropts(opts);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_syslog_levels(const sir_levels& levels) const {
            const bool set = sir_sysloglevels(levels);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_syslog_options(const sir_options& opts) const {
            const bool set = sir_syslogopts(opts);
            return throw_on_policy<TPolicy>(set);
        }

        bool set_syslog_id(const std::string& id) const {
            const bool set = sir_syslogid(id.c_str());
            return throw_on_policy<TPolicy>(set);
        }

        bool set_syslog_category(const std::string& category) const {
            const bool set = sir_syslogcat(category.c_str());
            return throw_on_policy<TPolicy>(set);
        }

        std::string get_version_string() const {
            return sir_getversionstring();
        }

        uint32_t get_version_hex() const noexcept {
            return sir_getversionhex();
        }

        bool is_prerelease() const noexcept {
            return sir_isprerelease();
        }

    protected:
        bool _init() const noexcept {
            TPolicy policy {};
            sirinit si {};
            if (const bool init = policy.get_init_data(si) && sir_init(&si) &&
                policy.on_init_complete(); !init) {
                return false;
            }
            return true;
        }
    };

    /**
     * @typedef default_logger
     * @brief A logger that implements the default adapter.
     */
    using default_logger = logger<true, default_policy, default_adapter>;

# if defined(__SIR_HAVE_STD_FORMAT__)
    /**
     * @typedef std_format_logger
     * @brief A logger that implements the default adapter and the std::format
     * adapter.
     */
    using std_format_logger = logger
    <
        true,
        default_policy,
        default_adapter,
        std_format_adapter
    >;
# endif

# if defined(__SIR_HAVE_BOOST_FORMAT__)
    /**
     * @typedef boost_logger
     * @brief A logger that implements the default adapter and the boost::format
     * adapter.
     */
    using boost_logger = logger
    <
        true,
        default_policy,
        default_adapter,
        boost_format_adapter
    >;
# endif

# if defined(__SIR_HAVE_FMT_FORMAT__)
    /**
     * @typedef fmt_logger
     * @brief A logger that implements the default adapter and the fmt::format
     * adapter.
     */
    using fmt_logger = logger
    <
        true,
        default_policy,
        default_adapter,
        fmt_format_adapter
    >;
# endif

# if !defined(SIR_NO_STD_IOSTREAM)
    /**
     * @typedef iostream_logger
     * @brief A logger that implements the default adapter and the std::iostream
     * adapter.
     */
    using iostream_logger = logger
    <
        true,
        default_policy,
        default_adapter,
        std_iostream_adapter
    >;
# endif

} // ! namespace sir

/**
 * @}
 * @}
 */

#endif // !_SIR_HH_INCLUDED
