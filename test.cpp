#include "leveldb_client.h"

int main() {
  leveldb_client::LeveldbClient client("0.0.0.0:8000");
  // client.put("yes", "itsme");
  std::cout << client.get("sing").value() << std::endl;
  return 0;
}