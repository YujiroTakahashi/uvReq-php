#ifndef PHP_UVREQUEST_H
#define PHP_UVREQUEST_H

#define PHP_UVREQUEST_VERSION "0.2.0"

extern zend_module_entry uvrequest_module_entry;
#define phpext_uvrequest_ptr &uvrequest_module_entry

ZEND_BEGIN_MODULE_GLOBALS(uvrequest)
    zend_long threadpool_size;
ZEND_END_MODULE_GLOBALS(uvrequest)

#ifdef ZTS
# define UVREQUEST_G(v) TSRMG(uvrequest_globals_id, zend_uvrequest_globals *, v)
# ifdef COMPILE_DL_UVREQUEST
ZEND_TSRMLS_CACHE_EXTERN()
# endif
#else
# define UVREQUEST_G(v) (uvrequest_globals.v)
#endif

#endif  /* PHP_UVREQUEST_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 */
