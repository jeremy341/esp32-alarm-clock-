#include <Arduino.h>
#include <FS.h>
#include <LittleFS.h>

namespace storage {

bool beginFileSystem() {
  return LittleFS.begin(true);
}

bool readTextFile(const char* path, String& output) {
  File file = LittleFS.open(path, "r");
  if (!file) {
    return false;
  }

  output = file.readString();
  file.close();
  return true;
}

bool writeTextFile(const char* path, const String& content) {
  File file = LittleFS.open(path, "w");
  if (!file) {
    return false;
  }

  const size_t written = file.print(content);
  file.close();
  return written == content.length();
}

}  // namespace storage
