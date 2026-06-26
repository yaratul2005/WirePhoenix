<?php
namespace PhoenixWire;

/**
 * Factory to determine the best available PhoenixWire client.
 */
class ClientFactory {
    /**
     * Creates a Client instance. Prefers the native extension if available,
     * otherwise falls back to the pure-PHP stream implementation.
     *
     * @param array|ClientOptions $options
     * @return Client|StreamClient
     */
    public static function create($options = []) {
        if (extension_loaded('phoenixwire')) {
            return new Client($options);
        }

        return new StreamClient($options);
    }
}
