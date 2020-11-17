#ifndef KEYMANAGER_HPP_
#define KEYMANAGER_HPP_

class KeyManager {
   public:
    KeyManager(const KeyManager&) = delete;
    KeyManager(KeyManager&&) noexcept = delete;

    /**
     * Gets the singleton instance
     */
    static KeyManager& get() {
        static KeyManager instance;
        return instance;
    }

    void asciiKeyPressed(unsigned char key) { ascii_keys[key] = true; }
    void asciiKeyReleased(unsigned char key) { ascii_keys[key] = false; }
    void specialKeyPressed(int keycode) { special_keys[keycode] = true; }
    void specialKeyReleased(int keycode) { special_keys[keycode] = false; }
    bool isAsciiKeyPressed(unsigned char key) { return ascii_keys[key]; }
    bool isSpecialKeyPressed(int keycode) { return special_keys[keycode]; }

   private:
    KeyManager() {
        for (int i = 0; i < 256; i++)
            ascii_keys[i] = false;

        for (int i = 0; i < 1024; i++)
            special_keys[i] = false;
    }

    static KeyManager& instance;
    bool ascii_keys[256];
    bool special_keys[1024];
};

#endif // KEYMANAGER_HPP_
