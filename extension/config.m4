PHP_ARG_ENABLE(phoenixwire, whether to enable PhoenixWire support,
[  --enable-phoenixwire           Enable PhoenixWire support])

if test "$PHP_PHOENIXWIRE" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_INCLUDE([../native/include])
  PHP_NEW_EXTENSION(phoenixwire, src/phoenixwire.c src/pw_connection.c src/pw_transport.c src/pw_frame.c src/pw_varint.c src/pw_handshake.c src/pw_auth.c src/pw_heartbeat.c src/pw_flow.c, $ext_shared,, -I../native/include)
fi
