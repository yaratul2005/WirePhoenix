<?php
require_once __DIR__ . '/bootstrap.php';

use PhoenixWire\Server;
use PhoenixWire\ServerOptions;
use PhoenixWire\Session;

class ServerQuotaTest {

    public function testMaxSessionAdmission() {
        $options = new ServerOptions(maxSessions: 2);
        $server = new Server('0.0.0.0', 8080, $options);

        $mockStream1 = fopen('php://memory', 'w+');
        $mockStream2 = fopen('php://memory', 'w+');
        $mockStream3 = fopen('php://memory', 'w+');

        $s1 = new Session($mockStream1);
        $s2 = new Session($mockStream2);
        $s3 = new Session($mockStream3); // This one should be rejected

        if ($server->addSession($s1) !== true) throw new \Exception("s1 should be admitted");
        if ($server->addSession($s2) !== true) throw new \Exception("s2 should be admitted");

        // Quota is 2, third should fail
        if ($server->addSession($s3) !== false) throw new \Exception("s3 should be rejected due to maxSessions");

        // Remove one, then third should succeed
        $server->removeSession($s1->id());
        if ($server->addSession($s3) !== true) throw new \Exception("s3 should be admitted after removal");

        fclose($mockStream1);
        fclose($mockStream2);
        fclose($mockStream3);
    }

    public function testIdleEvictionSweep() {
        $options = new ServerOptions(idleTimeoutMs: 50); // very short idle
        $server = new Server('0.0.0.0', 8080, $options);

        $mockStream = fopen('php://memory', 'w+');
        $s1 = new Session($mockStream);
        $server->addSession($s1);

        $now = (int)(microtime(true) * 1000);
        $server->sweepIdleSessions($now);

        // Hack to inspect private sessions array via Reflection
        $reflection = new \ReflectionClass(Server::class);
        $prop = $reflection->getProperty('sessions');
        $prop->setAccessible(true);

        $sessions = $prop->getValue($server);
        if (!isset($sessions[$s1->id()])) throw new \Exception("s1 should not be evicted yet");

        // Wait for idle timeout
        usleep(60000); // 60ms

        $now = (int)(microtime(true) * 1000);
        $server->sweepIdleSessions($now);

        $sessions = $prop->getValue($server);
        if (isset($sessions[$s1->id()])) throw new \Exception("s1 should have been evicted for being idle");

        fclose($mockStream);
    }
}
