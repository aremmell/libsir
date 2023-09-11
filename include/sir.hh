/*
 * @file sir.hh
 * @brief C++ interface to libsir
 *
 * @author    Ryan M. Lederman \<lederman@gmail.com\>
 * @date      2018-2023
 * @version   2.2.4
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
# include <algorithm>
# include <exception>
# include <memory>
# include <string>
# include <array>

# if !defined(SIR_NO_STD_IOSTREAM)
# include <iostream>
# endif

# if defined(__has_include)
#  if !defined(__CPPCHECK__) /* github.com/danmar/simplecpp/issues/229 */
#   if __has_include(<format>) && !defined(SIR_NO_STD_FORMAT) && \
       !defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT)
#    include <format>
#    define __SIR_HAVE_STD_FORMAT__
#   endif
#  else
#   include <format>
#  endif
#  if !defined(__CPPCHECK__)
#   if __has_include(<boost/format.hpp>) && !defined(SIR_NO_BOOST_FORMAT)
#    include <boost/format.hpp>
#    define __SIR_HAVE_BOOST_FORMAT__
#   endif
#  endif
#  if !defined(__CPPCHECK__)
#   if __has_include(<fmt/format.h>) && !defined(SIR_NO_FMT_FORMAT)
#    define FMT_HEADER_ONLY
#    include <fmt/format.h>
#    define __SIR_HAVE_FMT_FORMAT__
#   endif
#  endif
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
     * @brief A libsir error object, containing a numeric error code and associated
     * message.
     */
    struct error {
        uint32_t code = 0U;  /**< The error code associated with the message. */
        std::string message; /**< Description of the error that occurred. */
    };

    /**
     * @class policy
     * @brief Defines the abstract interface for a policy, which controls the
     * behavior of logger at runtime.
     *
     * policy is designed to be practical–sure, you could have 3 or 4 different
     * types of policy interfaces that made decisions for different categories
     * of behavior, but in reality, engineers likely just want to have only one
     * source file to consult when changes are required.
     *
     * For this reason, libsir's policy interface is a one-stop shop. Of course,
     * a derived class can still be as elaborate as the engineer wishes, but the
     * design goal here is a central source of control, and simplicity.
     */
    class policy {
    public:
        policy() = default;
        virtual ~policy() = default;

        /**
         * Called by logger before initializing libsir. Provides the policy with
         * the opportunity to fully customize the initial configuration.
         *
         * @param data libsir initial configuration data structure. Contains
         * options for things such as level registrations and formatting options
         * for stdio and system logger destinations.
         *
         * @see ::sirinit
         * @see ::sir_makeinit
         *
         * @note Most, if not all of the options chosen during initialization
         * can be modified at runtime using functions provided by libsir.
         *
         * @note For the default configuration, pass the address of the ::sirinit
         * structure to ::sir_makeinit.
         *
         * @returns true if data was successfully configured, or false if an
         * error occurred. Returning false will cause logger to abort the
         * initialization process–either by throwing an exception, or by entering
         * an 'invalid' state (determined by policy::throw_on_error).
         */
        virtual bool get_init_data(sirinit& data) const noexcept = 0;

        /**
         * Called by logger immediately after libsir has been initialized. This
         * is the moment in time which should be utilized for further configuration
         * than is possible via ::sirinit alone.
         *
         * Some items include:
         * - Setting the ::sir_colormode for stdio (::sir_setcolormode)
         * - Setting ::sir_textstyle on a per-level basis for stdio (::sir_settextstyle)
         * - Adding log files (::sir_addfile)
         * - Loading plugins (::sir_loadplugin)
         *
         * @returns true if configuration was completed successfully, or false if
         * an error occurred. Returning false will cause logger to abort the
         * initialization process–either by throwing an exception, or by entering
         * an 'invalid' state (determined by policy::throw_on_error).
         */
        virtual bool on_init_complete() const noexcept = 0;

        /**
         * Determines whether or not exceptions are thrown in the event that a
         * libsir or OS-level error is encountered by logger or its associated
         * adapter(s).
         *
         * @note libsir's exception classes derive from std::exception.
         *
         * @returns true if exceptions should be thrown, false otherwise.
         */
        virtual constexpr bool throw_on_error() const noexcept = 0;

        /**
         * Useful when complications arise; the question "which policy made that
         * decision?!" is then easy to answer.
         *
         * @returns A unique name for the policy.
         */
        virtual std::string get_name() const = 0;
    };

    /**
     * @class default_policy
     * @brief Implementation of the default policy, in the event that no
     * custom configuration or behavior is desired.
     */
    class default_policy : policy {
    public:
        default_policy() = default;
        virtual ~default_policy() = default;

        /*
         * policy overrides
         */

        bool get_init_data(sirinit& data) const noexcept final {
            return sir_makeinit(&data);
        }

        bool on_init_complete() const noexcept final {
            return true;
        }

        constexpr bool throw_on_error() const noexcept final {
            return true;
        }

        std::string get_name() const final {
            return "Default";
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
     * For an example of this, see ::std_format_adapter. When std::format is
     * available, and SIR_NO_STD_FORMAT is not defined, std_format_adapter may
     * be used to expose methods that behave exactly like std::format, but the
     * resulting formatted strings are sent directly to libsir.
     *
     * @note One must take care to ensure that the methods implemented by an
     * adapter can coexist with any other adapters that are applied to the logger
     * template.
     *
     * @see ::std_format_adapter
     * @see ::boost_format_adapter
     * @see ::fmt_format_adapter
     * @see ::std_iostream_adapter
     */
    class adapter {
    protected:
        /** Protected default constructor to prevent instantiation outside of
         * a derived class. */
        adapter() = default;
        virtual ~adapter() = default;
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

# if defined(__GNUC__) || defined(__clang__) && \
    !(defined(__OPEN64__) || defined(__OPENCC__))
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wformat-nonliteral"
# endif

        /** Logs a debug message (see ::sir_debug). */
        bool debug(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_DEBUG, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an informational message (see ::sir_info). */
        bool info(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_INFO, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a notice message (see ::sir_notice). */
        bool notice(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_NOTICE, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a warning message (see ::sir_warn). */
        bool warn(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_WARN, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an error message (see ::sir_error). */
        bool error(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ERROR, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a critical message (see ::sir_crit). */
        bool crit(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_CRIT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an alert message (see ::sir_alert). */
        bool alert(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ALERT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an emergency message (see ::sir_emerg). */
        bool emerg(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_EMERG, format, args);
            _SIR_L_END();
            return ret;
        }

# if defined(__GNUC__) || defined(__clang__) && \
    !(defined(__OPEN64__) || defined(__OPENCC__))
#  pragma GCC diagnostic pop
# endif

    };

# if defined(__SIR_HAVE_STD_FORMAT__)
    /**
     * @class std_format_adapter
     * @brief Adapter for std::format (when available).
     *
     * If std::format is available on this platform and SIR_NO_STD_FORMAT is not
     * defined, ::default_logger will export the methods from this adapter in
     * addition to any others.
     */
    class std_format_adapter : public adapter {
    public:
        std_format_adapter() = default;
        virtual ~std_format_adapter() = default;

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool debug_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_debug("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool info_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_info("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool notice_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_notice("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool warn_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_warn("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool error_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_error("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool crit_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_crit("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool alert_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_alert("%s", str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool emerg_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_emerg("%s", str.c_str());
        }
    };
# endif // !__SIR_HAVE_STD_FORMAT__

# if defined(__SIR_HAVE_BOOST_FORMAT__)
    /**
     * @class boost_format_adapter
     * @brief Adapter for boost::format (when available).
     *
     * TODO: update description
     */
    class boost_format_adapter : public adapter {
    public:
        boost_format_adapter() = default;
        virtual ~boost_format_adapter() = default;

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool debug_boost(const boost::format& fmt) const {
            return sir_debug("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool info_boost(const boost::format& fmt) const {
            return sir_info("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool notice_boost(const boost::format& fmt) const {
            return sir_notice("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool warn_boost(const boost::format& fmt) const {
            return sir_warn("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool error_boost(const boost::format& fmt) const {
            return sir_error("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool crit_boost(const boost::format& fmt) const {
            return sir_crit("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool alert_boost(const boost::format& fmt) const {
            return sir_alert("%s", fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool emerg_boost(const boost::format& fmt) const {
            return sir_emerg("%s", fmt.str().c_str());
        }
    };
# endif // !__SIR_HAVE_BOOST_FORMAT__

# if defined(__SIR_HAVE_FMT_FORMAT__)
    /**
     * @class fmt_format_adapter
     * @brief Adapter for fmt (when available).
     *
     * TODO: update description
     */
    class fmt_format_adapter : public adapter {
    public:
        fmt_format_adapter() = default;
        virtual ~fmt_format_adapter() = default;

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool debug_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_debug("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool info_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_info("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool notice_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_notice("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool warn_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_warn("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool error_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_error("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool crit_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_crit("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool alert_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_alert("%s", str.c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool emerg_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            auto str = fmt::vformat(fmt, fmt::make_format_args(args...));
            return sir_emerg("%s", str.c_str());
        }
    };
# endif // !__SIR_HAVE_FMT_FORMAT__

# if !defined(SIR_NO_STD_IOSTREAM)
    /**
     * @class std_iostream_adapter
     * @brief Provides a std::iostream interface to libsir's logging functions.
     *
     * Implements the following public std::ostream members:
     *
     * - debug_stream
     * - info_stream
     * - notice_stream
     * - warn_stream
     * - error_stream
     * - crit_stream
     * - alert_stream
     * - emerg_stream
     */
    class std_iostream_adapter : public adapter {
    public:
        std_iostream_adapter() = default;
        virtual ~std_iostream_adapter() = default;

    private:
        typedef bool(*sir_log_pfn)(const char*, ...);

        class buffer : public std::streambuf {
        public:
            using array_type = std::array<char_type, SIR_MAXMESSAGE>;

            buffer() = delete;
            explicit buffer(sir_log_pfn pfn) : _pfn(pfn) {
                // TODO: if exceptions enabled, throw if pfn or _buf are nullptr.
                SIR_ASSERT(pfn != nullptr && _buf);
                reset_buffer();
            }
            virtual ~buffer() = default;

        protected:
            void reset_buffer() {
                if (_buf) {
                    _buf->fill('\0');
                    setp(_buf->data(), _buf->data() + _buf->size() - 1);
                }
            }

            bool write_out() {
                /* get rid of a trailing newline if present. */
                for (auto it = _buf->rbegin(); it != _buf->rend(); it++) {
                    if (*it != '\0') {
                        if (*it == '\n')
                            *it = '\0';
                        break;
                    }
                }

                bool write = _pfn ? _pfn(_buf->data()) : false;
                SIR_ASSERT(write);
                // TODO: if exceptions enabled, throw if write is false.
                reset_buffer();
                return write;
            }

            int_type overflow(int_type ch) override {
                if (ch != traits_type::eof() && write_out())
                    return ch;
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

                    auto this_write = std::min(static_cast<std::streamsize>(left), count - written);
                    memcpy(pptr(), s + written, this_write);
                    pbump(this_write);
                    written += this_write;
                } while (written < count);

                return written;
            }

            std::unique_ptr<array_type> _buf = std::make_unique<array_type>();
            sir_log_pfn _pfn = nullptr;
        };

        buffer _buf_debug  {&sir_debug};
        buffer _buf_info   {&sir_info};
        buffer _buf_notice {&sir_notice};
        buffer _buf_warn   {&sir_warn};
        buffer _buf_error  {&sir_error};
        buffer _buf_crit   {&sir_crit};
        buffer _buf_alert  {&sir_alert};
        buffer _buf_emerg  {&sir_emerg};

    public:
        std::ostream debug_stream  {&_buf_debug};  /**< ostream for debug level. */
        std::ostream info_stream   {&_buf_info};   /**< ostream for info level. */
        std::ostream notice_stream {&_buf_notice}; /**< ostream for notice level. */
        std::ostream warn_stream   {&_buf_warn};   /**< ostream for warn level. */
        std::ostream error_stream  {&_buf_error};  /**< ostream for error level. */
        std::ostream crit_stream   {&_buf_crit};   /**< ostream for crit level. */
        std::ostream alert_stream  {&_buf_alert};  /**< ostream for alert level. */
        std::ostream emerg_stream  {&_buf_emerg};  /**< ostream for emerg level. */
    };
#endif // SIR_NO_STD_IOSTREAM

    /** Ensures that T derives from adapter. */
    template<typename... T>
    concept DerivedFromAdapter = std::is_base_of_v<adapter, T...>;

    /** Ensures that T derives from policy. */
    template<typename... T>
    concept DerivedFromPolicy = std::is_base_of_v<policy, T...>;

    /**
     * @class logger
     * @brief The primary C++ interface to libsir.
     *
     * Instantiate this class in order to access libsir with all the benefits
     * of C++, including RAII initialization/cleanup, custom adapters, and more.
     *
     * @param RAII      Set to `true` to enable 'resource acquisition is
     *                  initialization' behavior (i.e., libsir is initialized by
     *                  the ctor, and cleaned up by the dtor). Set to `false` for
     *                  manual management of initialization/cleanup.
     * @param TPolicy   A policy class which will be responsible for certain
     *                  aspects of logger's behavior.
     * @param TAdapters One or more adapter classes whose public methods will be
     *                  exposed by this class.
     */
    template<bool RAII, DerivedFromPolicy TPolicy, DerivedFromAdapter... TAdapters>
    class logger : public TAdapters...  {
    public:
        logger() : TAdapters()... {
            if (RAII && !_init()) {
                // TODO: throw exception if policy says
                SIR_ASSERT(false);
            }
        }

        virtual ~logger() {
            if (RAII && !_cleanup()) {
                SIR_ASSERT(false);
            }
        }

        bool init() noexcept {
            return _init();
        }

        bool cleanup() noexcept {
            return _cleanup();
        }

        /** Wraps ::sir_geterror. */
        error get_error() const {
            std::array<char, SIR_MAXERROR> message {};
            uint32_t code = sir_geterror(message.data());
            return { code, message.data() };
        }

        /** Wraps ::sir_addfile. */
        sirfileid add_file(const std::string& path, const sir_levels& levels,
            const sir_options& opts) const {
            return sir_addfile(path.c_str(), levels, opts);
        }

        /** Wraps ::sir_remfile. */
        bool rem_file(const sirfileid& id) const noexcept {
            return sir_remfile(id);
        }

        /** Wraps ::sir_loadplugin. */
        sirpluginid load_plugin(const std::string& path) const {
            return sir_loadplugin(path.c_str());
        }

        /** Wraps ::sir_unloadplugin. */
        bool unload_plugin(const sirpluginid& id) const noexcept {
            return sir_unloadplugin(id);
        }

        /** Wraps ::sir_filelevels. */
        bool set_file_levels(const sirfileid& id, const sir_levels& levels)
            const noexcept {
            return sir_filelevels(id, levels);
        }

        /** Wraps ::sir_fileopts. */
        bool set_file_options(const sirfileid& id, const sir_options& opts)
            const noexcept {
            return sir_fileopts(id, opts);
        }

        /** Wraps ::sir_settextstyle. */
        bool set_text_style(const sir_level& level, const sir_textattr& attr,
            const sir_textcolor& fg, const sir_textcolor& bg) const noexcept {
            return sir_settextstyle(level, attr, fg, bg);
        }

        /** Wraps ::sir_resettextstyles. */
        bool reset_text_styles() const noexcept {
            return sir_resettextstyles();
        }

        /** Wraps ::sir_makergb. */
        sir_textcolor make_rgb(const sir_textcolor& r, const sir_textcolor& g,
            const sir_textcolor& b) const noexcept {
            return sir_makergb(r, g, b);
        }

        /** Wraps ::sir_setcolormode. */
        bool set_color_mode(const sir_colormode& mode) const noexcept {
            return sir_setcolormode(mode);
        }

        /** Wraps ::sir_stdoutlevels. */
        bool set_stdout_levels(const sir_levels& levels) const noexcept {
            return sir_stdoutlevels(levels);
        }

        /** Wraps ::sir_stdoutopts. */
        bool set_stdout_options(const sir_options& opts) const noexcept {
            return sir_stdoutopts(opts);
        }

        /** Wraps ::sir_stderrlevels. */
        bool set_stderr_levels(const sir_levels& levels) const noexcept {
            return sir_stderrlevels(levels);
        }

        /** Wraps ::sir_stderropts. */
        bool set_stderr_options(const sir_options& opts) const noexcept {
            return sir_stderropts(opts);
        }

        /** Wraps ::sir_sysloglevels. */
        bool set_syslog_levels(const sir_levels& levels) const noexcept {
            return sir_sysloglevels(levels);
        }

        /** Wraps ::sir_syslogopts. */
        bool set_syslog_options(const sir_options& opts) const noexcept {
            return sir_syslogopts(opts);
        }

        /** Wraps ::sir_syslogid. */
        bool set_syslog_id(const std::string& id) const {
            return sir_syslogid(id.c_str());
        }

        /** Wraps ::sir_syslogcat. */
        bool set_syslog_category(const std::string& category) const {
            return sir_syslogcat(category.c_str());
        }

        /** Wraps ::sir_getversionstring. */
        std::string get_version_string() const {
            return sir_getversionstring();
        }

        /** Wraps ::sir_getversionhex. */
        uint32_t get_version_hex() const noexcept {
            return sir_getversionhex();
        }

        /** Wraps ::sir_isprerelease. */
        bool is_prerelease() const noexcept {
            return sir_isprerelease();
        }

    protected:
        bool _init() {
            sirinit si {};
            if (bool init = _policy.get_init_data(si) && sir_init(&si) &&
                _policy.on_init_complete(); !init) {
                return false;
            }
            return _initialized = true;
        }

        bool _cleanup() {
            bool cleanup = sir_cleanup();
            _initialized = false;
            return cleanup;
        }

    protected:
        TPolicy _policy;
        bool _initialized = false;
    };

    /**
     * @typedef default_logger
     * @brief A logger that implements the default set of adapters.
     *
     * The default logger has the following template parameters defined:
     * // TODO update description
     */
    using default_logger = logger
    < // TODO: don't include boost and fmt adapters by default
        true,
        default_policy,
        default_adapter
# if defined(__SIR_HAVE_STD_FORMAT__)
        , std_format_adapter
# endif
# if defined(__SIR_HAVE_BOOST_FORMAT__)
        , boost_format_adapter
# endif
# if defined(__SIR_HAVE_FMT_FORMAT__)
        , fmt_format_adapter
# endif
# if !defined(SIR_NO_STD_IOSTREAM)
        , std_iostream_adapter
# endif
    >;
} // ! namespace sir

/**
 * @}
 * @}
 */

#endif // !_SIR_HH_INCLUDED
