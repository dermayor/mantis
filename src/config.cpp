#include "mantis/config.h"

//! initialize externs as defaults
namespace mantis::config {
#define CONFIG_VAR(name, type, default_val) type name{default_val};
    MANTIS_CONFIG_VARS
#undef CONFIG_VAR
}
