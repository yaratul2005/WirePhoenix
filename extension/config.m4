PHP_ARG_ENABLE(phoenixwire, whether to enable PhoenixWire support,
[  --enable-phoenixwire           Enable PhoenixWire support])

if test "$PHP_PHOENIXWIRE" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_INCLUDE([../native/include])
  PHP_NEW_EXTENSION(phoenixwire, src/phoenixwire.c, $ext_shared,, -I../native/include)
fi
