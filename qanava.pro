TEMPLATE    =   subdirs

qanava.file         = src/qan.pro
test-basic.subdir   = tests/basic
test-basic.depends  = qanava

test-container.subdir   = tests/container
test-container.depends  = qanava

SUBDIRS     +=  qanava test-basic test-container



