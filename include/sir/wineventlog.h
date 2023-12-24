/*
 * wineventlog.h
 *
 * Version: 2.2.5
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

#ifndef _SIR_WINEVENTLOG_H_INCLUDED
# define _SIR_WINEVENTLOG_H_INCLUDED

/* GUID = 7ef5932c-c0f3-4c8b-a0f7-7cfae9c60b0e */
EXTERN_C __declspec(selectany) const GUID SIR_EVENTLOG_GUID = {
    0x7ef5932c, 0xc0f3, 0x4c8b, {0xa0, 0xf7, 0x7c, 0xfa, 0xe9, 0xc6, 0x0b, 0x0e}
};

# ifndef SIR_EVENTLOG_GUID_Traits
#  define SIR_EVENTLOG_GUID_Traits NULL
# endif

# define SIR_EVENTLOG_GUID_CHANNEL_Application         0x9
# define SIR_EVENTLOG_GUID_CHANNEL_Application_KEYWORD 0x8000000000000000

# define SIR_DEBUG_CHANNEL         0x10
# define SIR_DEBUG_CHANNEL_KEYWORD 0x4000000000000000

EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR SIR_EVT_CRITICAL = {
    0x1, 0x1, 0x9, 0x1, 0x0, 0x0, 0x8000000000000000
};
# define SIR_EVT_CRITICAL_value 0x1

EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR SIR_EVT_ERROR = {
    0x2, 0x1, 0x9, 0x2, 0x0, 0x0, 0x8000000000000000
};
# define SIR_EVT_ERROR_value 0x2

EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR SIR_EVT_WARNING = {
    0x3, 0x1, 0x9, 0x3, 0x0, 0x0, 0x8000000000000000
};
# define SIR_EVT_WARNING_value 0x3

EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR SIR_EVT_INFO = {
    0x4, 0x1, 0x9, 0x4, 0x0, 0x0, 0x8000000000000000
};
# define SIR_EVT_INFO_value 0x4

EXTERN_C __declspec(selectany) const EVENT_DESCRIPTOR SIR_EVT_DEBUG = {
    0x5, 0x1, 0x10, 0x5, 0x0, 0x0, 0x4000000000000000
};
# define SIR_EVT_DEBUG_value 0x5

# define MSG_sir_winlog_provider_event_1_message                   0x00000001L
# define MSG_sir_winlog_provider_event_2_message                   0x00000002L
# define MSG_sir_winlog_provider_event_3_message                   0x00000003L
# define MSG_sir_winlog_provider_event_4_message                   0x00000004L
# define MSG_sir_winlog_provider_event_5_message                   0xB0010005L
# define MSG_opcode_Info                                           0x30000000L
# define MSG_level_Critical                                        0x50000001L
# define MSG_level_Error                                           0x50000002L
# define MSG_level_Warning                                         0x50000003L
# define MSG_level_Informational                                   0x50000004L
# define MSG_level_Verbose                                         0x50000005L
# define MSG_task_None                                             0x70000000L
# define MSG_channel_Application                                   0x90000001L
# define MSG_sir_winlog_provider_channel_SIR_DEBUG_CHANNEL_message 0x90000002L

#endif /* !_SIR_WINEVENTLOG_H_INCLUDED */
