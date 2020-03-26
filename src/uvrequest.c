#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/json/php_json.h"
#include "main/SAPI.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "SAPI.h"
#include "php_uvrequest.h"
#include "classes/uvreq.h"

ZEND_DECLARE_MODULE_GLOBALS(uvrequest)

/* {{{ PHP_INI
*/
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("uvrequest.threadpool_size", "4", PHP_INI_ALL, OnUpdateLong, threadpool_size, zend_uvrequest_globals, uvrequest_globals)
PHP_INI_END()
/* }}} */

/* Handlers */
static zend_object_handlers uvrequest_object_handlers;

/* Class entries */
zend_class_entry *php_uvrequest_sc_entry;



/* {{{ arginfo */
ZEND_BEGIN_ARG_INFO(arginfo_uvrequest_void, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_uvrequest_ctor, 0, 0, 0)
    ZEND_ARG_INFO(0, number)
    ZEND_ARG_INFO(0, timeout)
    ZEND_ARG_INFO(0, ua)
    ZEND_ARG_INFO(0, sleep_time)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_uvrequest_execute, 0, 0, 1)
    ZEND_ARG_INFO(0, urls)
ZEND_END_ARG_INFO()
/* }}} */


/* {{{ php_suvrequest_class_methods */
static zend_function_entry php_uvrequest_class_methods[] = {
    PHP_ME(uvrequest, __construct, arginfo_uvrequest_ctor,    ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
    PHP_ME(uvrequest, __destruct,  arginfo_uvrequest_void,    ZEND_ACC_PUBLIC|ZEND_ACC_DTOR)
    PHP_ME(uvrequest, execute,     arginfo_uvrequest_execute, ZEND_ACC_PUBLIC)
    PHP_ME(uvrequest, getErrors,   arginfo_uvrequest_void,    ZEND_ACC_PUBLIC)

    PHP_FE_END
};
/* }}} */



static void php_uvrequest_object_free_storage(zend_object *object) /* {{{ */
{
    php_uvrequest_object *intern = php_uvrequest_from_obj(object);

    if (!intern) {
        return;
    }

    zend_object_std_dtor(&intern->zo);
}
/* }}} */

static zend_object *php_uvrequest_object_new(zend_class_entry *class_type) /* {{{ */
{
    php_uvrequest_object *intern;

    /* Allocate memory for it */
    intern = ecalloc(1, sizeof(php_uvrequest_object) + zend_object_properties_size(class_type));

    zend_object_std_init(&intern->zo, class_type);
    object_properties_init(&intern->zo, class_type);

    intern->zo.handlers = &uvrequest_object_handlers;

    return &intern->zo;
}
/* }}} */


/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(uvrequest)
{
    zend_class_entry ce;

    char uvenv[24];
    sprintf(uvenv, "UV_THREADPOOL_SIZE=%ld", UVREQUEST_G(threadpool_size));
    putenv(uvenv);

    memcpy(&uvrequest_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    /* Register uvRequest Class */
    INIT_CLASS_ENTRY(ce, "uvRequest", php_uvrequest_class_methods);
    ce.create_object = php_uvrequest_object_new;
    uvrequest_object_handlers.offset = XtOffsetOf(php_uvrequest_object, zo);
    uvrequest_object_handlers.clone_obj = NULL;
    uvrequest_object_handlers.free_obj = php_uvrequest_object_free_storage;
    php_uvrequest_sc_entry = zend_register_internal_class(&ce);

    REGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(uvrequest)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(uvrequest)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "uvRequest support", "enabled");
    php_info_print_table_row(2, "uvRequest module version", PHP_UVREQUEST_VERSION);
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ PHP_GINIT_FUNCTION
*/
static PHP_GINIT_FUNCTION(uvrequest)
{
    memset(uvrequest_globals, 0, sizeof(*uvrequest_globals));
}
/* }}} */

/* {{{ uvrequest_module_entry
*/
zend_module_entry uvrequest_module_entry = {
    STANDARD_MODULE_HEADER,
    "uvrequest",
    NULL,
    PHP_MINIT(uvrequest),
    PHP_MSHUTDOWN(uvrequest),
    NULL,
    NULL,
    PHP_MINFO(uvrequest),
    PHP_UVREQUEST_VERSION,
    PHP_MODULE_GLOBALS(uvrequest),
    PHP_GINIT(uvrequest),
    NULL,
    NULL,
    STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_UVREQUEST
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(uvrequest)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
