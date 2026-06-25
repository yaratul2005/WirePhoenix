<?php
namespace PhoenixWire;

class Exception extends \Exception {}

class ConnectionException extends Exception {}

class AuthException extends Exception {}

class TimeoutException extends Exception {}

class ProtocolException extends Exception {}
