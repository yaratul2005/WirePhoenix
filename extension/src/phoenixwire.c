#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_phoenixwire.h"
#include "zend_exceptions.h"

#include "pw_connection.h"
#include "pw_transport.h"
#include <unistd.h>

zend_class_entry *phoenixwire_client_ce;
zend_class_entry *phoenixwire_exception_ce;

/* Object structure */
typedef struct _phoenixwire_client_obj {
    pw_connection_t conn;
    pw_transport_t transport;
    zend_object std;
} phoenixwire_client_obj;

static inline phoenixwire_client_obj *php_phoenixwire_client_fetch_object(zend_object *obj) {
    return (phoenixwire_client_obj *)((char*)(obj) - XtOffsetOf(phoenixwire_client_obj, std));
}

#define Z_PHOENIXWIRE_CLIENT_P(zv) php_phoenixwire_client_fetch_object(Z_OBJ_P((zv)))

static zend_object_handlers phoenixwire_client_handlers;

/* Free object resources */
static void phoenixwire_client_free_object(zend_object *object) {
    phoenixwire_client_obj *intern = php_phoenixwire_client_fetch_object(object);

    /* Clean up native transport and connection if active */
    if (intern->conn.state != PW_STATE_DISCONNECTED && intern->conn.state != PW_STATE_CLOSED) {
        pw_connection_transition(&intern->conn, PW_STATE_CLOSED);
    }

    if (intern->transport.fd >= 0) {
        close(intern->transport.fd);
        intern->transport.fd = -1;
    }

    zend_object_std_dtor(&intern->std);
}

/* Create object instance */
static zend_object *phoenixwire_client_create_object(zend_class_entry *ce) {
    phoenixwire_client_obj *intern = zend_object_alloc(sizeof(phoenixwire_client_obj), ce);

    zend_object_std_init(&intern->std, ce);
    object_properties_init(&intern->std, ce);

    pw_connection_init(&intern->conn);
    pw_transport_init(&intern->transport, -1);

    intern->std.handlers = &phoenixwire_client_handlers;

    return &intern->std;
}

/* Method: __construct() */
PHP_METHOD(PhoenixWireClient, __construct) {
    ZEND_PARSE_PARAMETERS_NONE();
    /* Initialization already handled in create_object */
}

/* Method: connect(string $host, int $port) */
PHP_METHOD(PhoenixWireClient, connect) {
    char *host;
    size_t host_len;
    zend_long port;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(host, host_len)
        Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END();

    phoenixwire_client_obj *intern = Z_PHOENIXWIRE_CLIENT_P(ZEND_THIS);

    if (intern->conn.state != PW_STATE_DISCONNECTED && intern->conn.state != PW_STATE_CLOSED) {
        zend_throw_exception(phoenixwire_exception_ce, "Client is already connected or connecting", 0);
        return;
    }

    // In a full implementation, we'd do a non-blocking socket connect here.
    // For now, we simulate the state transition.
    if (!pw_connection_transition(&intern->conn, PW_STATE_CONNECTING)) {
        zend_throw_exception(phoenixwire_exception_ce, "Failed to transition to CONNECTING state", 0);
        return;
    }

    RETURN_TRUE;
}

/* Method: disconnect() */
PHP_METHOD(PhoenixWireClient, disconnect) {
    ZEND_PARSE_PARAMETERS_NONE();

    phoenixwire_client_obj *intern = Z_PHOENIXWIRE_CLIENT_P(ZEND_THIS);

    if (intern->conn.state != PW_STATE_CLOSED && intern->conn.state != PW_STATE_DISCONNECTED) {
        if (!pw_connection_transition(&intern->conn, PW_STATE_CLOSING)) {
            // Force close if graceful fails
            pw_connection_transition(&intern->conn, PW_STATE_CLOSED);
        }

        if (intern->transport.fd >= 0) {
            close(intern->transport.fd);
            intern->transport.fd = -1;
        }
    }

    RETURN_TRUE;
}

/* Method: getState() */
PHP_METHOD(PhoenixWireClient, getState) {
    ZEND_PARSE_PARAMETERS_NONE();
    phoenixwire_client_obj *intern = Z_PHOENIXWIRE_CLIENT_P(ZEND_THIS);
    RETURN_LONG(intern->conn.state);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_connect, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, host, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, port, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_disconnect, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_client_getstate, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry phoenixwire_client_methods[] = {
    PHP_ME(PhoenixWireClient, __construct, arginfo_client_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(PhoenixWireClient, connect, arginfo_client_connect, ZEND_ACC_PUBLIC)
    PHP_ME(PhoenixWireClient, disconnect, arginfo_client_disconnect, ZEND_ACC_PUBLIC)
    PHP_ME(PhoenixWireClient, getState, arginfo_client_getstate, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

/* Module functions */
PHP_FUNCTION(phoenixwire_version)
{
    RETURN_STRING(PHP_PHOENIXWIRE_VERSION);
}

ZEND_BEGIN_ARG_INFO_EX(arginfo_phoenixwire_version, 0, 0, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry phoenixwire_functions[] = {
    PHP_FE(phoenixwire_version, arginfo_phoenixwire_version)
    PHP_FE_END
};

/* Module initialization */
PHP_MINIT_FUNCTION(phoenixwire)
{
    zend_class_entry ce_exception;
    INIT_CLASS_ENTRY(ce_exception, "PhoenixWire\\Exception", NULL);
    phoenixwire_exception_ce = zend_register_internal_class_ex(&ce_exception, zend_ce_exception);

    zend_class_entry ce_client;
    INIT_CLASS_ENTRY(ce_client, "PhoenixWire\\Client", phoenixwire_client_methods);
    phoenixwire_client_ce = zend_register_internal_class(&ce_client);
    phoenixwire_client_ce->create_object = phoenixwire_client_create_object;

    memcpy(&phoenixwire_client_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    phoenixwire_client_handlers.offset = XtOffsetOf(phoenixwire_client_obj, std);
    phoenixwire_client_handlers.free_obj = phoenixwire_client_free_object;

    // Register constants for states
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_DISCONNECTED", sizeof("STATE_DISCONNECTED")-1, PW_STATE_DISCONNECTED);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_CONNECTING", sizeof("STATE_CONNECTING")-1, PW_STATE_CONNECTING);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_HANDSHAKING", sizeof("STATE_HANDSHAKING")-1, PW_STATE_HANDSHAKING);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_AUTHENTICATING", sizeof("STATE_AUTHENTICATING")-1, PW_STATE_AUTHENTICATING);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_READY", sizeof("STATE_READY")-1, PW_STATE_READY);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_DRAINING", sizeof("STATE_DRAINING")-1, PW_STATE_DRAINING);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_CLOSING", sizeof("STATE_CLOSING")-1, PW_STATE_CLOSING);
    zend_declare_class_constant_long(phoenixwire_client_ce, "STATE_CLOSED", sizeof("STATE_CLOSED")-1, PW_STATE_CLOSED);

    return SUCCESS;
}

/* Module shutdown */
PHP_MSHUTDOWN_FUNCTION(phoenixwire)
{
    // Any global resources to release upon PHP process shutdown
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
    // PHP request ended. The objects will be destructed by PHP's garbage collector,
    // which calls phoenixwire_client_free_object, so sockets are closed cleanly there.
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
