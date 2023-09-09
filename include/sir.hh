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
     * becomes a public base class of ::log (there can be more than one).
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
        /** Intended to mimic the C interface for maximum performance. */
        virtual bool debug(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool info(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool notice(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool warn(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool error(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool crit(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
        virtual bool alert(const char* format, ...) noexcept = 0;

        /** Intended to mimic the C interface for maximum performance. */
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

        /** Logs a debug message (@see ::sir_debug). */
        bool debug(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_DEBUG, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an info message (@see ::sir_info). */
        bool info(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_INFO, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a notice message (@see ::sir_notice). */
        bool notice(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_NOTICE, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a warning message (@see ::sir_warn). */
        bool warn(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_WARN, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an error message (@see ::sir_error). */
        bool error(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ERROR, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs a critical message (@see ::sir_crit). */
        bool crit(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_CRIT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an alert message (@see ::sir_alert). */
        bool alert(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_ALERT, format, args);
            _SIR_L_END();
            return ret;
        }

        /** Logs an emergency message (@see ::sir_emerg). */
        bool emerg(const char* format, ...) noexcept final {
            _SIR_L_START(format);
            ret = _sir_logv(SIRL_EMERG, format, args);
            _SIR_L_END();
            return ret;
        }
    };

    /**
     * @class policy
     * @brief Base class for policies that control the behavior of the ::log
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
     * change the libsir configuration Applies only when RAII = true for the ::Log
     * template. When RAII = false, you may call ::log::init at any time with a
     * custom configuration..
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

    /**
     * @class log
     * @brief The primary C++ interface to libsir.
     *
     * Instantiate this class in order to access libsir with all the benefits
     * of C++, including RAII initialization/cleanup, custom adapters, and more.
     *
     * @param RAII  bool Set to `true` to enable 'resource acquisition is
     *              initialization' behavior (i.e., libsir is initialized by the
     *              ctor, and cleaned up by the dtor). Set to `false` to manually
     *              manage the initialization/cleanup.
     * @param TInitPolicy init_policy The policy class that determines the libsir
     *                    configuration to use upon initialization.
     *
     * @param TAdapters adapter One or more ::adapter classes whose public
     *                  methods will be exposed by this class.
     */
    template<bool RAII, class TInitPolicy, class... TAdapters>
    class log : public TAdapters... {
        static_assert(std::is_base_of_v<adapter, TAdapters...>,
            "TAdapters must derive from adapter");
        static_assert(std::is_base_of_v<init_policy, TInitPolicy>,
            "TInitPolicy must derive from init_policy");
    public:
        log() : TAdapters()... {
            if (RAII && !init())
                SIR_ASSERT(false);
        }

        virtual ~log() {
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
            TInitPolicy policy {};
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
            return sir_ssysloglevels(levels);
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

    /** The default log: RAII = true, default init policy and adapter. */
    using default_log = log<true, default_init_policy, default_adapter>;
} // ! namespace sir

/**
 * @}
 * @}
 */

#endif // !_SIR_HH_INCLUDED
