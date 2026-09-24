# Copyright 2026 p7cq
# Distributed under the terms of the Apache License, Version 2.0

EAPI=8

inherit acct-group

DESCRIPTION="Group for app-crypt/yca"
KEYWORDS="~amd64 ~arm64"
# Local overlay: dynamic allocation instead of a GID reserved in ::gentoo.
ACCT_GROUP_ID=-1
