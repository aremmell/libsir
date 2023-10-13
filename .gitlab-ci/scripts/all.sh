#!/usr/bin/env sh

# SPDX-License-Identifier: MIT
# Copyright (c) 2018-current Jeffrey H. Johnson <trnsz@pobox.com>

printf '%s\n' "Sanity check ..."

test -f ./scripts/all.sh 2> /dev/null ||
  {
    printf '%s\n' "ERROR: No ./scripts/all.sh from working directory."
    exit 1
  }

set -eu > /dev/null 2>&1

printf '\n%s\n' "Processing base ..."

(cd base && ../scripts/build.sh && ../scripts/push.sh) ||
  {
    printf '%s\n' "ERROR: base processing failed."
    exit 1
  }

printf '%s\n' "Successfully processed base image."

printf '\n%s\n' "Processing all CI images ..."

for dir in $(printf '%s\n' * | grep -Ev '(^base$|^scripts$)' ); do
    printf '\nProcessing %s ...\n' "${dir:?}"
    (cd "${dir:?}" && ../scripts/build.sh && ../scripts/push.sh) ||
      {
        printf 'ERROR: %s processing failed.\n' "${dir:?}"
      }
    printf 'Successfully processed %s image.\n' "${dir:?}"
done

printf '\n%s\n' "Successfully processed all CI images."
