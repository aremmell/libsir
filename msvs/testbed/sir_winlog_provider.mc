; // sir_winlog_provider.mc
; // <insert copyright info>

SeverityNames=(
    Success=0x0:STATUS_SEVERITY_SUCCESS
    Informational=0x1:STATUS_SEVERITY_INFORMATIONAL
    Warning=0x2:STATUS_SEVERITY_WARNING
    Error=0x3:STATUS_SEVERITY_ERROR
)

FacilityNames=(
    System=0x0:FACILITY_SYSTEM
    Runtime=0x2:FACILITY_RUNTIME
)

LanguageNames=(English=0x409:MSG00409)

; // Event categories

MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=GENERAL_CATEGORY
Language=English
General
.

; // Messages

MessageIdTypedef=DWORD

MessageId=0x100
Severity=Critical
Facility=Runtime
SymbolicName=MSG_LIBSIR_CRITICAL
Language=English
%1
.

MessageId=0x101
Severity=Error
Facility=Runtime
SymbolicName=MSG_LIBSIR_ERROR
Language=English
%1
.

MessageId=0x102
Severity=Warning
Facility=Runtime
SymbolicName=MSG_LIBSIR_WARNING
Language=English
%1
.

MessageId=0x103
Severity=Informational
Facility=Runtime
SymbolicName=MSG_LIBSIR_INFO
Language=English
%1
.
