/*!
 * \file sir.c
 * \brief Implementation of the Standard Incident Reporter (SIR) library.
 *
 * \author Ryan Matthew Lederman <lederman@gmail.com>
 * \version 1.1.0
 * \date 2003-2018
 * \copyright MIT License
 */
#include "sir.h"

sirinit          sir_s;  /* !< Global data storage (internal) */
sirfiles     sir_fc; /* !< Global data storage (internal) */
sircallbacks sir_cc; /* !< Global data storage (internal) */

int sir_init(const sirinit* si) {

    if (NULL == si || 0 == si->opts)
        return 1;

    sir_cleanup();
    memcpy(&sir_s, si, sizeof(sirinit));
    return 0;
}

int sirdebug(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_DEBUG, format, args);
    _SIR_L_END(args);
    return r;
}

int sirinfo(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_INFO, format, args);
    _SIR_L_END(args);
    return r;
}

int sirnotice(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_NOTICE, format, args);
    _SIR_L_END(args);
    return r;
}
int sirwarn(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_WARNING, format, args);
    _SIR_L_END(args);
    return r;
}
int sirerror(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_ERROR, format, args);
    _SIR_L_END(args);
    return r;
}

int sircrit(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_CRIT, format, args);
    _SIR_L_END(args);
    return r;
}

int siralert(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_ALERT, format, args);
    _SIR_L_END(args);
    return r;
}

int siremerg(const sirchar_t* format, ...) {
    _SIR_L_START();
    r = _sir_lv(SIRL_EMERG, format, args);
    _SIR_L_END(args);
    return r;
}

void sir_cleanup(void) {
    _sir_files_destroy(&sir_fc);
    _sir_callbacks_destroy(&sir_cc);

    memset(&sir_s, 0, sizeof(sirinit));
    memset(&sir_fc, 0, sizeof(sirfiles));
    memset(&sir_cc, 0, sizeof(sircallback));
}

int sir_addfile(const sirchar_t* path, uint32_t mask) {

    sirfile* sf = NULL;
    int      r  = 1;

    if (strptr(path) && 0 != mask) {
        sf = _sirfile_create(path, mask);

        if (NULL != sf)
            r = _sir_files_add(&sir_fc, sf);
    }

    return r;
}

int sir_remfile(const sirchar_t* path) { return _sir_files_rem(&sir_fc, path); }

int sir_addcallback(sircallbackfn cb, uint32_t mask, uint64_t userData) {

    sircallback* scb = NULL;
    int          r   = 1;

    if (NULL != cb && 0 != mask) {
        scb = _sircallback_create(cb, mask, userData);

        if (NULL != scb)
            r = _sir_callbacks_add(&sir_cc, scb);
    }

    return r;
}

int sir_remcallback(sircallbackfn cb) { return _sir_callbacks_rem(&sir_cc, cb); }

/*! \cond PRIVATE */

int _sir_lv(uint32_t type, const sirchar_t* format, va_list args) {

    sirchar_t output[SIR_MAXPRINT] = {0};
    sirchar_t timestr[SIR_TIMELEN] = {0};
    sirbuf*   buf                  = NULL;
    int       r                    = 1;

    buf = _sirbuf_create();

    if (NULL != buf) {
        if (!smaskset(sir_s.opts, SIRO_NOTIMESTAMP)) {

            time_t now = 0;
            time(&now);
            strftime(
                timestr, SIR_TIMELEN, strptr(sir_s.fmtOverride) ? sir_s.fmtOverride : SIR_TIMEFORMAT, localtime(&now));

            timestr[sstrlen(timestr) - 1] = _T('\0');
            _sirbuf_append(buf, timestr);
        }

        if (!smaskset(sir_s.opts, SIRO_NONAME)) {
        }

        if (!smaskset(sir_s.opts, SIRO_NOLEVEL)) {
        }

        svsnprintf(output, SIR_MAXPRINT, format, args);

        if (0 == _sirbuf_append(buf, output)) {
            if (smaskset(sir_s.opts, SIRO_LF)) {
                _sirbuf_append(buf, _T("\n"));
            } else if (smaskset(sir_s.opts, SIRO_CRLF)) {
                _sirbuf_append(buf, _T("\r\n"));
            }

            r = _sir_dispatchall(buf->ptr, type);
        }

        _sirbuf_destroy(buf);
    }

    return r;
}

int _sir_dispatchall(const sirchar_t* output, uint32_t mask) {

    int r = 1;

    if (strptr(output) && 0 != mask) {

        /*
        TODO: this.
        if (mask & SIRT_ERROR)
        _sir_dispatch(output, SIRL_ERROR); }
          if (mask & SIRL_WARNING) { _sir_dispatch(output, SIRT_WARNING); }
          if (mask & SIRL_CRIT)   { _sir_dispatch(output, SIRT_CRIT); }*/

        if (smaskset(sir_s.opts, SIRO_FILES))
            _sir_files_dispatch(&sir_fc, output, mask);

        if (smaskset(sir_s.opts, SIRO_CALLBACKS))
            _sir_callbacks_dispatch(&sir_cc, output, mask);

        r = 0;
    }

    return r;
}

void _sir_dispatch(const sirchar_t* output, uint32_t type) {

    if (smaskset(sir_s.f_stderr, type))
        fputs(output, stderr);

    if (smaskset(sir_s.f_stdout, type))
        fputs(output, stderr);
}

int _sir_dispatchfile(const sirchar_t* path, const sirchar_t* output) {

    int r = 1;

    if (strptr(path) && strptr(output)) {
        FILE* f = sfopen(path, _T("a'"));

        if (NULL != f) {
            if (0 == fwrite(output, sizeof(sirchar_t), sstrlen(output), f)) {
                r = 0 != feof(f) && 0 == ferror(f);
            } else {
                r = 0;
            }

            fclose(f);
        }
    }

    return r;
}

sirbuf* _sirbuf_create(void) {

    sirbuf* buf = (sirbuf*)calloc(1U, sizeof(sirbuf));

    if (NULL != buf) {
        buf->ptr = (sirchar_t*)calloc(SIR_MINPRINT + sizeof(sirchar_t), sizeof(sirchar_t));

        if (NULL != buf->ptr) {
            buf->size = 0U;
            return buf;
        }
    }

    return buf;
}

int _sirbuf_append(sirbuf* buf, const sirchar_t* str) {

    size_t copy = 0U;
    int    r    = 1;

    if (strptr(str) && NULL != buf && NULL != buf->ptr) {
        copy = (sstrlen(str) * sizeof(sirchar_t));

        if (0U < copy) {
            buf->ptr = (sirchar_t*)realloc(buf->ptr, buf->size + copy + sizeof(sirchar_t));

            if (NULL != buf->ptr) {
                memcpy(((char*)buf->ptr + buf->size), str, copy);
                buf->ptr[buf->size + copy - 1U] = _T('\0');
                buf->size += copy;
                r = 0L;
            }
        }
    }

    return r;
}

void _sirbuf_destroy(sirbuf* buf) {
    safefree(buf->ptr);
    safefree(buf);
}

sirfile* _sirfile_create(const sirchar_t* path, uint32_t mask) {

    sirfile* sf = NULL;

    if (strptr(path) && 0 != mask) {
        sf = (sirfile*)calloc(1U, sizeof(sirfile));

        if (NULL != sf) {
            sf->path = (sirchar_t*)calloc(sstrlen(path) + 1U, sizeof(sirchar_t));

            if (NULL != sf->path) {
                memcpy(sf->path, path, sstrlen(path) * sizeof(sirchar_t));
                sf->bitmask = mask;

                return sf;
            }
        }
    }

    return sf;
}

void _sirfile_destroy(sirfile* sf) {
    safefree(sf->path);
    safefree(sf);
}

int _sir_files_add(sirfiles* sfc, sirfile* sf) {

    int r = 1;

    if (NULL != sfc && NULL != sf) {
        if (0U < sfc->count) {
            sfc->files[sfc->count] = sf;
            sfc->count++;
        } else {
            sfc->files[0] = sf;
            sfc->count    = 1U;
        }

        r = 0;
    }

    return r;
}

int _sir_files_rem(sirfiles* sfc, const sirchar_t* path) {

    size_t n = 0U;
    size_t i = 0U;
    int    r = 1;

    if (NULL != sfc && strptr(path) && 0 < sfc->count) {
        for (n = 0; n < sfc->count; n++) {
            if (NULL != sfc->files[n] && 0 != *sfc->files[n]->path) {
                if (0 == sstrcmp(sfc->files[n]->path, path)) {
                    _sirfile_destroy(sfc->files[n]);

                    for (i = n; i < sfc->count - 1; i++) {
                        sfc->files[i] = sfc->files[i + 1];
                    }

                    sfc->count--;
                    r = 0;
                    break;
                }
            }
        }
    }

    return r;
}

int _sir_files_destroy(sirfiles* sfc) {

    int r = 1;

    if (NULL != sfc && 0 < sfc->count) {
        size_t n = 0U;

        for (; n < sfc->count; n++) {
            _sirfile_destroy(sfc->files[n]);
            sfc->files[n] = NULL;
            sfc->count--;
        }

        r = 0;
    }

    return r;
}

int _sir_files_dispatch(sirfiles* sfc, const sirchar_t* output, uint32_t mask) {

    int r = 0;

    if (NULL != sfc && strptr(output) && 0 != mask) {
        size_t n = 0U;

        for (; (n < sfc->count) && (0 == r); n++) {
            if (NULL != sfc->files[n] && strptr(sfc->files[n]->path)) {
                if (sfc->files[n]->bitmask & mask) {
                    r = _sir_dispatchfile(sfc->files[n]->path, output);
                }
            }
        }
    }

    return r;
}

sircallback* _sircallback_create(sircallbackfn cb, uint32_t mask, uint64_t userData) {

    sircallback* scb = NULL;

    if (NULL != cb && 0 != mask) {
        scb = (sircallback*)calloc(1U, sizeof(sircallback));

        if (NULL != scb) {
            scb->cb  = cb;
            scb->mask = mask;
            scb->data = userData;
            return scb;
        }
    }

    return scb;
}

void _sircallback_destroy(sircallback* scb) { safefree(scb); }

int _sir_callbacks_add(sircallbacks* scc, sircallback* scb) {

    int r = 1;

    if (NULL != scc && NULL != scb) {
        if (0 < scc->count) {
            scc->c[scc->count] = scb;
            scc->count++;
        } else {
            scc->c[0]  = scb;
            scc->count = 1U;
        }

        r = 0;
    }

    return r;
}

int _sir_callbacks_rem(sircallbacks* scc, sircallbackfn cb) {

    int r = 1;

    if (NULL != scc && NULL != cb && 0 < scc->count) {
        size_t n = 0U;
        size_t i = 0U;

        for (; n < scc->count; n++) {
            if (NULL != scc->c[n]) {
                if (scc->c[n]->cb == cb) {
                    _sircallback_destroy(scc->c[n]);

                    for (i = n; i < scc->count; i++) {
                        scc->c[i] = scc->c[i + 1];
                    }

                    scc->count--;
                    r = 0;
                    break;
                }
            }
        }
    }

    return r;
}

int _sir_callbacks_destroy(sircallbacks* scc) {

    int r = 1;

    if (NULL != scc && 0 < scc->count) {
        size_t n = 0U;

        for (; n < scc->count; n++) {
            _sircallback_destroy(scc->c[n]);
            scc->c[n] = NULL;
            scc->count--;
        }

        r = 0;
    }

    return r;
}

int _sir_callbacks_dispatch(sircallbacks* scc, const sirchar_t* output, uint32_t mask) {

    int r = 1;

    if (NULL != scc && strptr(output) && 0 != mask) {
        size_t n = 0U;

        for (; n < scc->count; n++) {
            if (scc->c[n]->mask & mask) {
                if ((0L != scc->c[n]->cb)) {
                    scc->c[n]->cb(output, scc->c[n]->data);
                }
            }
        }

        r = 0;
    }

    return r;
}

/*! \endcond PRIVATE */
