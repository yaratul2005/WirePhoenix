#ifndef PHP_PHOENIXWIRE_H
#define PHP_PHOENIXWIRE_H

extern zend_module_entry phoenixwire_module_entry;
#define phpext_phoenixwire_ptr &phoenixwire_module_entry

#define PHP_PHOENIXWIRE_VERSION "0.1.0"

#if defined(ZTS) && defined(COMPILE_DL_PHOENIXWIRE)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif /* PHP_PHOENIXWIRE_H */
