<?php
// Simple test runner for PHP API layer

$tests = glob(__DIR__ . '/*Test.php');
$passed = 0;
$failed = 0;

foreach ($tests as $test) {
    echo "Running " . basename($test) . "...\n";
    require_once $test;
    $className = basename($test, '.php');
    if (class_exists($className)) {
        $instance = new $className();
        $methods = get_class_methods($instance);
        foreach ($methods as $method) {
            if (strpos($method, 'test') === 0) {
                try {
                    $instance->$method();
                    echo "  [PASS] $method\n";
                    $passed++;
                } catch (\Throwable $e) {
                    echo "  [FAIL] $method: " . $e->getMessage() . "\n";
                    $failed++;
                }
            }
        }
    }
}

echo "\nTests completed. Passed: $passed, Failed: $failed\n";
exit($failed > 0 ? 1 : 0);
