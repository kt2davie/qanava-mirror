TEMPLATE    =   subdirs
CONFIG      += ordered      # Well I know, but no choice...

qanava.file         = src/qan.pro
test-basic.subdir   = tests/basic
test-basic.depends  = qanava

test-container.subdir   = tests/container
test-container.depends  = qanava

test-custom.subdir   = tests/custom
test-custom.depends  = qanava

test-groups.subdir   = tests/groups
test-groups.depends  = qanava

test-styles.subdir   = tests/styles
test-styles.depends  = qanava

SUBDIRS     +=  qanava test-basic test-container test-custom test-groups test-styles



