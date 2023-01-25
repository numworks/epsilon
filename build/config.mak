# You can override those settings on the command line

PLATFORM ?= device
DEBUG ?= 0
DEVELOPMENT ?= 0

EPSILON_VERSION ?= 23.1.0
EXTERNAL_APPS_API_LEVEL ?= 0
EPSILON_APPS ?= calculation graph code statistics distributions inference solver sequence regression elements finance settings
EPSILON_GETOPT ?= 0
ESCHER_LOG_EVENTS_BINARY ?= 0
ESCHER_LOG_EVENTS_NAME ?= $(DEBUG)
POINCARE_MEMORY_TREE_LOG ?= 0
I18N_COMPRESS ?= 0
ASSERTIONS ?= $(DEBUG)
HWTEST_ALL_KEYS ?= 0
VALGRIND ?= 0

supported_locales := en fr nl pt it de es
EPSILON_I18N ?= $(supported_locales)

supported_countries := WW CA DE ES FR GB IT NL PT US
EPSILON_COUNTRIES ?= $(supported_countries)

