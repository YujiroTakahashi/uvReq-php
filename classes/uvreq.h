#ifndef PHP_CLASSES_UVREQ_H
#define PHP_CLASSES_UVREQ_H

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus

#include "uvrequest.hpp"

extern "C" {

#include "php.h"
#include "php_ini.h"
#include "main/SAPI.h"

#include "zend_exceptions.h"
#include "zend_interfaces.h"
#include "SAPI.h"
#include "php_uvrequest.h"

#endif /* __cplusplus */

typedef void *uvRequestHandle;

typedef struct _php_uvrequest_object {
    uvRequestHandle handle;
    zend_long number;
    zend_object zo;
} php_uvrequest_object;

static inline php_uvrequest_object *php_uvrequest_from_obj(zend_object *obj) {
    return (php_uvrequest_object*)((char*)(obj) - XtOffsetOf(php_uvrequest_object, zo));
}

#define Z_UVREQUEST_P(zv) php_uvrequest_from_obj(Z_OBJ_P((zv)))

PHP_METHOD(uvrequest, __construct);
PHP_METHOD(uvrequest, __destruct);
PHP_METHOD(uvrequest, execute);
PHP_METHOD(uvrequest, getErrors);

#ifdef __cplusplus
}   // extern "C"
#endif /* __cplusplus */

#endif /* PHP_CLASSES_UVREQ_H */