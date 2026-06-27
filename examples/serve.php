<?php
/**
 * PhoenixWire Chat - Simple PHP Development Server
 * 
 * This is a lightweight development server for the web-based chat interface.
 * Run this to test the chat frontend in your browser.
 * 
 * Usage: php -S localhost:8000 serve.php
 * Then open: http://localhost:8000
 */

// Serve static files
$requested_file = $_SERVER["REQUEST_URI"];

// Remove query string
$requested_file = preg_replace('/\?.*$/', '', $requested_file);

// Remove leading slash
$requested_file = ltrim($requested_file, '/');

// Default to index.html
if (empty($requested_file) || $requested_file === '/') {
    $requested_file = 'index.html';
}

// Build full file path
$file_path = __DIR__ . '/' . $requested_file;

// Security check - prevent directory traversal
$real_path = realpath($file_path);
if ($real_path === false || strpos($real_path, realpath(__DIR__)) !== 0) {
    http_response_code(404);
    echo "404 Not Found";
    exit;
}

// Check if file exists
if (!file_exists($file_path)) {
    http_response_code(404);
    echo "404 Not Found: {$requested_file}";
    exit;
}

// Determine content type
$mime_types = [
    'html' => 'text/html; charset=UTF-8',
    'css'  => 'text/css',
    'js'   => 'application/javascript',
    'json' => 'application/json',
    'png'  => 'image/png',
    'jpg'  => 'image/jpeg',
    'gif'  => 'image/gif',
    'svg'  => 'image/svg+xml',
    'ico'  => 'image/x-icon',
    'woff' => 'font/woff',
    'woff2' => 'font/woff2',
    'ttf'  => 'font/ttf',
];

$ext = strtolower(pathinfo($file_path, PATHINFO_EXTENSION));
$content_type = $mime_types[$ext] ?? 'application/octet-stream';

// Set response headers
header('Content-Type: ' . $content_type);
header('Access-Control-Allow-Origin: *');
header('Cache-Control: public, max-age=3600');

// Serve the file
readfile($file_path);
exit;
