<?php
/**
 * RandomUserGenerator - Generates random user data for chat simulation
 */
class RandomUserGenerator {
    private static array $firstNames = [
        'Alice', 'Bob', 'Charlie', 'Diana', 'Eve', 'Frank', 'Grace', 'Henry',
        'Ivy', 'Jack', 'Karen', 'Leo', 'Maya', 'Noah', 'Olivia', 'Peter',
        'Quinn', 'Rachel', 'Sam', 'Tina', 'Uma', 'Victor', 'Wendy', 'Xavier',
        'Yara', 'Zoe'
    ];

    private static array $lastNames = [
        'Smith', 'Johnson', 'Williams', 'Brown', 'Jones', 'Garcia', 'Miller',
        'Davis', 'Rodriguez', 'Martinez', 'Anderson', 'Taylor', 'Thomas',
        'Moore', 'Jackson', 'Martin', 'Lee', 'White', 'Harris', 'Clark',
        'Lewis', 'Walker', 'Young', 'King', 'Wright', 'Lopez'
    ];

    private static array $avatarColors = [
        '#FF6B6B', '#4ECDC4', '#45B7D1', '#FFA07A', '#98D8C8',
        '#F7DC6F', '#BB8FCE', '#85C1E2', '#F8B88B', '#52C79A'
    ];

    private static array $statusMessages = [
        'Just joined!',
        'Ready to chat',
        'Saying hello',
        'Online now',
        'Available',
        'In the zone',
        'Coffee break',
        'Late night coding',
        'Morning shift',
        'All ears',
        'Let\'s talk',
        'Good vibes only'
    ];

    /**
     * Generate random user data
     */
    public static function generate(): array {
        $userId = 'user_' . bin2hex(random_bytes(4));
        $firstName = self::$firstNames[array_rand(self::$firstNames)];
        $lastName = self::$lastNames[array_rand(self::$lastNames)];
        $username = strtolower($firstName) . '_' . strtolower(substr($lastName, 0, 3)) . '_' . mt_rand(100, 999);
        
        return [
            'id' => $userId,
            'username' => $username,
            'firstName' => $firstName,
            'lastName' => $lastName,
            'fullName' => "{$firstName} {$lastName}",
            'avatarColor' => self::$avatarColors[array_rand(self::$avatarColors)],
            'status' => self::$statusMessages[array_rand(self::$statusMessages)],
            'joinedAt' => microtime(true),
            'messageCount' => 0,
            'sessionId' => 'sess_' . bin2hex(random_bytes(8))
        ];
    }

    /**
     * Generate random chat messages
     */
    public static function generateMessage(array $user): string {
        $messages = [
            'Hey everyone!',
            'How\'s everyone doing?',
            'That\'s interesting!',
            'I agree with that.',
            'Great point!',
            'Let me think about that...',
            'Has anyone tried this before?',
            'Amazing work on this!',
            'I\'m learning so much!',
            'This is awesome!',
            'Just curious, what do you think?',
            'Thanks for sharing!',
            'That was helpful!',
            'Looking forward to this!',
            'Seems solid to me!',
            'What\'s your take on this?',
            'I love the creativity!',
            'This reminds me of...',
            'Brilliant solution!',
            'Let\'s collaborate on this!'
        ];

        return $messages[array_rand($messages)];
    }

    /**
     * Generate random message frequency (milliseconds)
     */
    public static function getRandomMessageFrequency(): int {
        return mt_rand(2000, 8000); // 2-8 seconds
    }
}
