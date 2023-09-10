/*
 * @file sir.hh
 * @brief C++20 interface to libsir
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
# include <exception>
# include <tuple>
# include <array>
# include <string>

# if defined __has_include
#  if __has_include(<format>) && !defined(SIR_NO_STD_FORMAT) && \
      !(defined(__MACOS__) && defined(_LIBCPP_HAS_NO_INCOMPLETE_FORMAT))
#   include <format>
#   define __SIR_HAVE_STD_FORMAT__
#  endif
#  if __has_include(<boost/format.hpp>) && !defined(SIR_NO_BOOST_FORMAT)
#   include <boost/format.hpp>
#   define __SIR_HAVE_BOOST_FORMAT__
#  endif
#  if __has_include(<fmt/format.h>) && !defined(SIR_NO_FMT_FORMAT)
#   define FMT_HEADER_ONLY
#   include <fmt/format.h>
#   define __SIR_HAVE_FMT_FORMAT__
#  endif
# endif

/**
 * @addtogroup public
 * @{
 *
 * @addtogroup cpp C++20 Wrapper
 * @{
 */

/** The one and only namespace for libsir. */
namespace sir
{
    /**
     * @struct error
     * @brief A libsir error object, possibly containing a code and message.
     */
    struct error {
        uint32_t code = 0U;  /**< The error code associated with the message. */
        std::string message; /**< A description of the error that occurred. */
    };

    /**
     * @class adapter
     * @brief Defines the abstract interface for an adapter, which ultimately
     * becomes a public base class of ::logger (there can be more than one).
     *
     * `adapter` is designed to provide flexibility with regards to the types
     * taken as input for logging, as well as return values, whether or not to
     * throw exceptions, etc.
     *
     * For example, one could implement a boost adapter that takes boost::format
     * as an argument.
     */
    class adapter {
    protected:
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

        /** Logs a debug message (see ::sir_debug). */
        bool debug(const char* format, ...) const noexcept {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_DEBUG, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an info message (see ::sir_info). */
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
    };

# if defined(__SIR_HAVE_STD_FORMAT__)
    /**
     * @class std_format_adapter
     * @brief Adapter for std::format (when available)
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
            return sir_debug(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool info_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_info(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool notice_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_notice(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool warn_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_warn(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool error_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_error(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool crit_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_crit(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool alert_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_alert(str.c_str());
        }

        /** Use as if you were calling std::format directly. */
        template<class... Args>
        inline bool emerg_std(std::format_string<Args...> fmt, Args&&... args) const {
            auto str = std::vformat(fmt.get(), std::make_format_args(args...));
            return sir_emerg(str.c_str());
        }
    };
# endif // !__SIR_HAVE_STD_FORMAT__

# if defined(__SIR_HAVE_BOOST_FORMAT__)
    /**
     * @class boost_format_adapter
     * @brief Adapter for boost::format (when available)
     *
     * If boost::format is available on this platform and SIR_NO_BOOST_FORMAT is not
     * defined, ::default_logger will export the methods from this adapter in
     * addition to any others.
     */
    class boost_format_adapter : public adapter {
    public:
        boost_format_adapter() = default;
        virtual ~boost_format_adapter() = default;

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool debug_boost(const boost::format& fmt) const {
            return sir_debug(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool info_boost(const boost::format& fmt) const {
            return sir_info(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool notice_boost(const boost::format& fmt) const {
            return sir_notice(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool warn_boost(const boost::format& fmt) const {
            return sir_warn(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool error_boost(const boost::format& fmt) const {
            return sir_error(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool crit_boost(const boost::format& fmt) const {
            return sir_crit(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool alert_boost(const boost::format& fmt) const {
            return sir_alert(fmt.str().c_str());
        }

        /** Takes `boost::format("...") % args [% ...]` as input. */
        bool emerg_boost(const boost::format& fmt) const {
            return sir_emerg(fmt.str().c_str());
        }
    };
# endif // !__SIR_HAVE_BOOST_FORMAT__

# if defined(__SIR_HAVE_FMT_FORMAT__)
    /**
     * @class fmt_format_adapter
     * @brief Adapter for fmt (when available)
     *
     * If fmt is available on this platform and SIR_NO_FMT_FORMAT is not
     * defined, ::default_logger will export the methods from this adapter in
     * addition to any others.
     */
    class fmt_format_adapter : public adapter {
    public:
        fmt_format_adapter() = default;
        virtual ~fmt_format_adapter() = default;

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool debug_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_debug(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool info_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_info(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool notice_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_notice(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool warn_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_warn(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool error_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_error(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool crit_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_crit(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool alert_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_alert(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }

        /** Use as if you were calling fmt::format directly. */
        template<typename... T>
        inline bool emerg_fmt(fmt::format_string<T...> fmt, T&&... args) const {
            return sir_emerg(fmt::vformat(fmt, fmt::make_format_args(args...)).c_str());
        }
    };
# endif // !__SIR_HAVE_FMT_FORMAT__

    /**
     * @class policy
     * @brief Base class for policies that control the behavior of the ::logger
     * class.
     *
     * Multiple types of policies derive from this class. It serves only to
     * identify derived classes as a policy.
     */
    class policy {
    public:
        /** Returns the name of this policy (e.g., 'My custom policy'). */
        virtual std::string get_name() const = 0;
    };

    /**
     * @class init_policy
     * @brief Controls the configuration of libsir at initialization time.
     */
    class init_policy : public policy {
    public:
        init_policy() = default;
        virtual ~init_policy() = default;

        /** When called, the policy must configure the initialization data `si`
         * as desired. Called directly before ::sir_init. */
        virtual void on_initialization(sirinit* si) const noexcept = 0;

        /** When called, the policy may set a color mode other than the default
         * (16-color mode) if desired. Called directly after ::sir_init. */
        virtual void set_color_mode() const noexcept = 0;

        /** When called, the policy may set per-level text styling if desired.
         * Called directly after ::set_color_mode. */
        virtual void set_text_styles() const noexcept = 0;

        /** From policy. */
        virtual std::string get_name() const override = 0;
    };

    /**
     * @class default_init_policy
     * @brief The default initialization policy.
     *
     * Replace with your own custom class derived from ::init_policy in order to
     * change the libsir configuration Applies only when RAII = true for the
     * ::logger template. When RAII = false, you may call ::logger::init at any
     * time with a custom configuration..
     */
    class default_init_policy : public init_policy {
    public:
        default_init_policy() = default;
        ~default_init_policy() = default;

        /** Uses ::sir_makeinit to set default values for all configuration
         * properties. */
        void on_initialization(sirinit* si) const noexcept final {
            bool init = sir_makeinit(si);
            SIR_ASSERT_UNUSED(init, init);
        }

        /** Uses the default color mode, so this is a NOOP. */
        void set_color_mode() const noexcept final { }

        /** Uses the default text styles, so this is a NOOP. */
        void set_text_styles() const noexcept final { }

        /** From policy. */
        std::string get_name() const final {
            return "Default";
        }
    };

    /** Ensures that the type argument derives from ::init_policy. */
    template<typename T>
    concept DerivedFromInitPolicy = std::is_base_of_v<init_policy, T>;

    /** Ensures that the type argument derives from ::adapter. */
    template<typename... T>
    concept DerivedFromAdapter = std::is_base_of_v<adapter, T...>;

    /**
     * @class logger
     * @brief The primary C++ interface to libsir.
     *
     * Instantiate this class in order to access libsir with all the benefits
     * of C++, including RAII initialization/cleanup, custom adapters, and more.
     *
     * @param RAII bool Set to `true` to enable 'resource acquisition is
     *             initialization' behavior (i.e., libsir is initialized by the
     *             ctor, and cleaned up by the dtor). Set to `false` to manually
     *             manage the initialization/cleanup.
     * @param TIP  init_policy The policy class that determines the libsir
     *             configuration to use upon initialization.
     *
     * @param TA   adapter One or more ::adapter classes whose public
     *             methods will be exposed by this class.
     */
    template<bool RAII, DerivedFromInitPolicy TIP, DerivedFromAdapter... TA>
    class logger : public TA...  {
    public:
        logger() : TA()... {
            if (RAII && !init())
                SIR_ASSERT(false);
        }

        virtual ~logger() {
            if (RAII && !cleanup())
                SIR_ASSERT(false);
        }

        /** When RAII = false, call to manually initialize libsir at your leisure.
         * When RAII = true, always returns false without doing any work. */
        bool init(sirinit& si) const noexcept {
            return RAII ? false : sir_init(&si);
        }

        /** When RAII = true, called by the constructor. The configuration is
         * determined by TInitPolicy. May be called manually when RAII = false
         * if the configuration set by TInitPolicy is desired. */
        bool init() const noexcept {
            sirinit si {};
            TIP policy {};
            _sir_selflog("init policy: '%s'", policy.get_name().c_str());
            policy.on_initialization(&si);

            bool init = sir_init(&si);
            if (init) {
                policy.set_color_mode();
                policy.set_text_styles();
            }

            return init;
        }

        /** May be called manually, or when RAII = true, by the destructor. */
        bool cleanup() const noexcept {
            return sir_cleanup();
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
    };

    /**
     * @class default_logger
     * @brief A ::logger that implements the default set of adapters.
     *
     * The default logger has these properties:
     *   - RAII = true
     *   - TIP  = default_init_policy
     *   - TA   = default_adapter [, std_format_adapter, boost_format_adapter]
     *     (see below)
     *
     * - If std::format is available and SIR_NO_STD_FORMAT is not defined, includes
     * the std::format adapter.
     * - If boost::format is available and SIR_NO_BOOST_FORMAT is not defined,
     * includes the boost::format adapter.
     */
    using default_logger = logger
    <
        true,
        default_init_policy,
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
    >;
} // ! namespace sir

/**
 * @}
 * @}
 */

#endif // !_SIR_HH_INCLUDED
