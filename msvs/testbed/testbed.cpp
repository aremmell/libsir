#include "testbed.h"
#include <winmeta.h>
#include <evntprov.h>
#include "sir_winlog_provider.h"

#pragma comment(lib, "advapi32.lib")
#define PROVIDER_NAME "libsir"

static const wchar_t* const fake_crit_msg = L"A fake critical from a fake libsir";
static const wchar_t* const fake_err_msg = L"A fake error from a fake libsir";
static const wchar_t* const fake_warn_msg = L"A fake warning from a fake libsir";
static const wchar_t* const fake_info_msg = L"A fake info message from a fake libsir";

using namespace std;

// https://learn.microsoft.com/en-us/windows/win32/etw/writing-manifest-based-events?source=recommendations
// https://learn.microsoft.com/en-us/windows/win32/api/evntprov/nf-evntprov-eventdatadesccreate
// https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-registereventsourcea

int main()
{
	static struct {
		EVENT_DATA_DESCRIPTOR ddesc {};
		const wchar_t* msg {};
		const EVENT_DESCRIPTOR* pdesc {};
	} messages[] = {
		{{}, fake_crit_msg, &SIR_EVT_CRITICAL},
		{{}, fake_err_msg, &SIR_EVT_ERROR},
		{{}, fake_warn_msg, &SIR_EVT_WARNING},
		{{}, fake_info_msg, &SIR_EVT_INFO},
	};

	REGHANDLE evt_log = NULL;
	DWORD status = EventRegister(&PROVIDER_GUID, nullptr, nullptr, &evt_log);
	if (ERROR_SUCCESS != status) {
		printf("failed to register. err: %lu\n", status);
		goto cleanup;
	}

	printf("registered successfully\n");

	for (size_t n = 0; n < _countof(messages); n++) {
		EventDataDescCreate(&messages[n].ddesc, messages[n].msg,
			(ULONG)(wcslen(messages[n].msg) + 1) * sizeof(wchar_t));

		status = EventWrite(
			evt_log,
			messages[n].pdesc,
			1,
			&messages[n].ddesc
		);
		if (ERROR_SUCCESS != status) {
			printf("[%zu] failed to write.err: % ld\n", n, status);
		}

		printf("[%zu] wrote successfully\n", n);
	}

	/*static struct {
		WORD type {};
		DWORD evt_id {};
		const char* msg {};
	} messages[] = {
		{EVENTLOG_ERROR_TYPE, MSG_LIBSIR_CRITICAL, fake_crit_msg},
		{EVENTLOG_ERROR_TYPE, MSG_LIBSIR_ERROR, fake_err_msg},
		{EVENTLOG_WARNING_TYPE, MSG_LIBSIR_WARNING, fake_warn_msg},
		{EVENTLOG_INFORMATION_TYPE, MSG_LIBSIR_INFO, fake_info_msg}
	};

	HANDLE event_log = RegisterEventSource(NULL, PROVIDER_NAME);
	if (!event_log) {
		printf("failed to register. err: %ld\n", GetLastError());
		goto cleanup;
	}

	for (size_t n = 0; n < _countof(messages); n++) {
		if (!ReportEvent(
			event_log,
			messages[n].type, // type
			GENERAL_CATEGORY, // category
			messages[n].evt_id, // event ID
			NULL, // SID
			1, // n strings
			0, // d size
			&messages[n].msg, // strings
			NULL// d
		)) {
			printf("[%zu] failed to report. err: %ld\n", n, GetLastError());
			goto cleanup;
		}

		printf("[%zu] successfully reported\n", n);
	}*/

cleanup:
	//DeregisterEventSource(event_log);
	printf("exiting\n");
	return 0;
}
