#!/usr/bin/env zsh

_script_name=$(basename "$0")

# $1 = output directory
compile_report() {
    make clean && bear -- gmake -j 72 && pvs-studio-analyzer analyze --intermodular -j 72 -o log.pvs \
    ; plog-converter -a "GA:1,2,3" -t fullhtml log.pvs -o pvsreport && rm -rf "${1}/pvsreport" || true \
    ; mv pvsreport "${1}"
}

# $1 = output directory
open_report() {
    open "${1}/pvsreport/index.html" || {
        echo "failed to open ${1}/pvsreport/index.html!" &&
        false; return
    }
}

# $1 = output directory
make_dir_if_not_exist() {
    if [[ ! -d "${1}" ]]; then
        echo "${1} is not a directory; atempting to create..."
        if mkdir -p "${1}" 2>&1 >/dev/null; then
            echo "created ${1} successfully."
        else
            echo "failed to create ${1}. check the path and your access rights."
        fi
    fi
}

print_usage() {
    echo "${_script_name} usage:\n\n" \
         "-c, --compile      \x1b[4mdir\x1b[24m Compile a report using PVS Studio and copy it to 'dir'\n" \
         "-o, --open-report  \x1b[4mdir\x1b[24m Open a previously compiled report located in 'dir'\n" \
         "-h, --help         Prints this message\n"
}

main() {
    if [[ -z "${2}" ]]; then
        echo "argument missing: dir"
        print_usage
        exit 1
    fi

    if [[ ${_compile} = true ]]; then
        make_dir_if_not_exist "${2}"
        compile_report "${2}"
    elif [[ ${_open_report} = true ]]; then
        make_dir_if_not_exist "${2}"
        open_report "${2}"
    else
        false; return
    fi

    true; return
}

_args=( "$@" )
_print_usage=false
_compile=false
_open_report=false

if [[ ! ${#_args[@]} -gt 0 ]]; then
    _print_usage=true
else
    for arg in "${_args[@]}"; do
        case $arg in
            -c|--compile)
                _compile=true
                break;
            ;;
            -o|--open-report)
                _open_report=true
                break;
            ;;
            -h|--help)
                _print_usage=true
                break;
            ;;
            *)
                echo "unknown option: '${arg}'"
                _print_usage=true
                break;
            ;;
        esac
    done
fi

if [[ $_print_usage = true ]]; then
    print_usage
    exit 1
fi

main "${_args[@]}"
