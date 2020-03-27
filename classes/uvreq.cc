#include "uvreq.h"

/* {{{ proto void uvrequest::__construct()
 */
PHP_METHOD(uvrequest, __construct)
{
    php_uvrequest_object *uvr_obj;
    zval *object = getThis();
    zend_long number = 3;
    zend_long timeout = 30;
    zend_long sleepTime = 2500;
    char *ua;
    size_t ua_len = 0;

    ZEND_PARSE_PARAMETERS_START(0, 4)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(number)
        Z_PARAM_LONG(timeout)
        Z_PARAM_STRING(ua, ua_len)
        Z_PARAM_LONG(sleepTime)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    uvr_obj = Z_UVREQUEST_P(object);

    croco::uvRequest *uvreq = new croco::uvRequest();

    uvreq->setTimeout(timeout);
    uvreq->setSleepTime(sleepTime);
    if (0 == ua_len) {
        uvreq->setUserAgent("Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:74.0) Gecko/20100101 Firefox/74.0");
    } else {
        std::string sua(ua, ua_len);
        uvreq->setUserAgent(sua);
    }
    uvr_obj->handle = static_cast<uvRequestHandle>(uvreq);
    uvr_obj->number = number;
}
/* }}} */

/* {{{ proto void uvrequest::__destruct()
 */
PHP_METHOD(uvrequest, __destruct)
{
    php_uvrequest_object *uvr_obj;
    zval *object = getThis();

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    uvr_obj = Z_UVREQUEST_P(object);

    delete static_cast<croco::uvRequest*>(uvr_obj->handle);
}
/* }}} */

/* {{{ proto void uvrequest::execute(array urls)
 */
PHP_METHOD(uvrequest, execute)
{
    php_uvrequest_object *uvr_obj;
    zval *object = getThis();
    zval *array;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(array)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    uvr_obj = Z_UVREQUEST_P(object);
    croco::uvRequest *uvreq = static_cast<croco::uvRequest*>(uvr_obj->handle);

    uvreq->setWorker();

    HashTable *ht = Z_ARRVAL_P(array);

    zend_hash_internal_pointer_reset(ht);
    int size = zend_hash_num_elements(ht);

    for (int idx=0; idx<size; idx++) {
        zval *value = zend_hash_get_current_data(ht);
        uvreq->queueWork(std::string(Z_STRVAL_P(value)), uvr_obj->number);
        zend_hash_move_forward(ht);
    }

    uvreq->await();


    std::vector<croco::uvRequest::Node> nodes = uvreq->getReply();

    array_init(return_value);
    zend_ulong idx = 0;
    for (auto & node : nodes) {
        zval rowVal, urlVal, replyVals;

        ZVAL_STRING(&urlVal,       node.url.c_str());
        ZVAL_STRING(&replyVals, node.reply.c_str());

        array_init(&rowVal);
        zend_hash_str_add(Z_ARRVAL_P(&rowVal), "url",   sizeof("url") -1,   &urlVal);
        zend_hash_str_add(Z_ARRVAL_P(&rowVal), "reply", sizeof("reply") -1, &replyVals);

        add_index_zval(return_value, idx, &rowVal);
        idx++;
    }
}
/* }}} */

/* {{{ proto array uvrequest::getErrors()
 */
PHP_METHOD(uvrequest, getErrors)
{
    php_uvrequest_object *uvr_obj;
    zval *object = getThis();

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    uvr_obj = Z_UVREQUEST_P(object);
    croco::uvRequest *uvreq = static_cast<croco::uvRequest*>(uvr_obj->handle);

    std::vector<std::string> errors = uvreq->getErrors();

    array_init(return_value);
    zend_ulong idx = 0;
    for (auto & error : errors) {
        zval errVal;
        ZVAL_STRING(&errVal, error.c_str());
        add_index_zval(return_value, idx, &errVal);
        idx++;
    }
}
/* }}} */

