#include <chrono>

struct time_f {
  /*
   * Timer structure to quickly get
   * the current time an object has been accessed
   * in a data structure
  */

  long timestamp;

  time_f() {
    set_time();
  }

  void set_time() {
    /*
     * SOURCE
     *
     * https://stackoverflow.com/questions/31255486/c-how-do-i-convert-a-stdchronotime-point-to-long-and-back
     *
     * Get time in milliseconds from Jan 1, 1970
     * NOT COMPLIANT WITH WINDOWS, NOT SORRY
    */

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

    auto value = now_ms.time_since_epoch();
    timestamp = value.count();
  }
  long get_time() {
    set_time(); // update the time whenever we access it
    return timestamp;
  }
};

typedef struct time_f time_f;

