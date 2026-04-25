
# 🖥️ Windows (easiest method)

### Option 1: Using vcpkg (recommended)

1. Install **vcpkg**

   ```bash
   git clone https://github.com/microsoft/vcpkg
   cd vcpkg
   ./bootstrap-vcpkg.bat
   ```

2. Install libcurl:

   ```bash
   vcpkg install curl
   ```

3. Integrate with Visual Studio:

   ```bash
   vcpkg integrate install
   ```

---

### Option 2: Manual install

1. Download from
   👉 [https://curl.se/windows/](https://curl.se/windows/)
2. Extract files
3. Add:

   * `include` → your compiler include path
   * `lib` → linker path

---

# 🍎 macOS

Use **Homebrew**:

```bash
brew install curl
```

---

# 🐧 Linux (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install libcurl4-openssl-dev
```

---

# 🧪 Simple C++ Example

```cpp
#include <iostream>
#include <curl/curl.h>

int main() {
    CURL* curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    return 0;
}
```

---

# ⚙️ Compile your code

### Linux / macOS:

```bash
g++ main.cpp -lcurl -o app
```

### Windows (MinGW):

```bash
g++ main.cpp -lcurl -o app.exe
```

---

# ⚠️ Common mistakes

* ❌ Installing just `curl` CLI → not enough for C++
* ❌ Missing `-lcurl` while compiling
* ❌ Not setting include/lib paths correctly (Windows)

---

If you want, I can help you set it up specifically for your **Java + MySQL + API project setup** or show how to **fetch JSON (like PokéAPI)** using curl in C++.
