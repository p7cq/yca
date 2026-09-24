# Copyright 2026 p7cq
# Distributed under the terms of the Apache License, Version 2.0

EAPI=8

inherit acct-user

DESCRIPTION="User for app-crypt/yca"
KEYWORDS="~amd64 ~arm64"
# Local overlay: dynamic allocation instead of a UID reserved in ::gentoo.
ACCT_USER_ID=-1
ACCT_USER_HOME=/var/lib/yca
ACCT_USER_HOME_PERMS=0700
ACCT_USER_GROUPS=( yca )

acct-user_add_deps
