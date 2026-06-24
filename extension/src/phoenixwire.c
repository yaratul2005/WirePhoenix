#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_phoenixwire.h"

#include "phoenixwire.h"

/* PHP functions */
PHP_FUNCTION(phoenixwire_version)
{
    RETURN_STRING(PHP_PHOENIXWIRE_VERSION);
}

/* Argument info */
ZEND_BEGIN_ARG_INFO_EX(arginfo_phoenixwire_version, 0, 0, 0)
ZEND_END_ARG_INFO()

/* Module functions */
static const zend_function_entry phoenixwire_functions[] = {
    PHP_FE(phoenixwire_version, arginfo_phoenixwire_version)
    PHP_FE_END
};

/* Module initialization */
PHP_MINIT_FUNCTION(phoenixwire)
{
    /* Register classes here eventually */
    return SUCCESS;
}

/* Module shutdown */
PHP_MSHUTDOWN_FUNCTION(phoenixwire)
{
    return SUCCESS;
}

/* Request initialization */
PHP_RINIT_FUNCTION(phoenixwire)
{
#if defined(COMPILE_DL_PHOENIXWIRE) && defined(ZTS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif
    return SUCCESS;
}

/* Request shutdown */
PHP_RSHUTDOWN_FUNCTION(phoenixwire)
{
    return SUCCESS;
}

/* Module info */
PHP_MINFO_FUNCTION(phoenixwire)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "PhoenixWire support", "enabled");
    php_info_print_table_row(2, "Version", PHP_PHOENIXWIRE_VERSION);
    php_info_print_table_end();
}

/* Module entry */
zend_module_entry phoenixwire_module_entry = {
    STANDARD_MODULE_HEADER,
    "phoenixwire",
    phoenixwire_functions,
    PHP_MINIT(phoenixwire),
    PHP_MSHUTDOWN(phoenixwire),
    PHP_RINIT(phoenixwire),
    PHP_RSHUTDOWN(phoenixwire),
    PHP_MINFO(phoenixwire),
    PHP_PHOENIXWIRE_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PHOENIXWIRE
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(phoenixwire)
#endif
