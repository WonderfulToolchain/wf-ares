SharpRTC sharprtc;
#include "memory.cpp"
#include "time.cpp"
#include "serialization.cpp"

auto SharpRTC::load(Node::Object parent, Node::Object from) -> void {
  rtc = Node::RealTimeClock::create("RTC");
  Node::load(rtc, from);
  parent->append(rtc);
}

auto SharpRTC::main() -> void {
  tickSecond();

  step(1);
  synchronize(cpu);
}

auto SharpRTC::initialize() -> void {
  second = 0;
  minute = 0;
  hour = 0;
  day = 0;
  month = 0;
  year = 0;
  weekday = 0;
}

auto SharpRTC::unload() -> void {
  cpu.coprocessors.removeValue(this);
  Thread::destroy();
}

auto SharpRTC::power() -> void {
  cpu.coprocessors.removeValue(this);
  create(1, [&] {
    while(true) scheduler.synchronize(), main();
  });
  cpu.coprocessors.append(this);

  state = State::Read;
  index = -1;
}

auto SharpRTC::synchronize(uint64 timestamp) -> void {
  time_t systime = timestamp;
  tm* timeinfo = localtime(&systime);

  second = min(59, timeinfo->tm_sec);
  minute = timeinfo->tm_min;
  hour = timeinfo->tm_hour;
  day = timeinfo->tm_mday;
  month = 1 + timeinfo->tm_mon;
  year = 900 + timeinfo->tm_year;
  weekday = timeinfo->tm_wday;
}

auto SharpRTC::read(uint24 addr, uint8 data) -> uint8 {
  addr &= 1;

  if(addr == 0) {
    if(state != State::Read) return 0;

    if(index < 0) {
      index++;
      return 15;
    } else if(index > 12) {
      index = -1;
      return 15;
    } else {
      return rtcRead(index++);
    }
  }

  return data;
}

auto SharpRTC::write(uint24 addr, uint8 data) -> void {
  addr &= 1, data &= 15;

  if(addr == 1) {
    if(data == 0x0d) {
      state = State::Read;
      index = -1;
      return;
    }

    if(data == 0x0e) {
      state = State::Command;
      return;
    }

    if(data == 0x0f) return;  //unknown behavior

    if(state == State::Command) {
      if(data == 0) {
        state = State::Write;
        index = 0;
      } else if(data == 4) {
        state = State::Ready;
        index = -1;
        //reset time
        second = 0;
        minute = 0;
        hour = 0;
        day = 0;
        month = 0;
        year = 0;
        weekday = 0;
      } else {
        //unknown behavior
        state = State::Ready;
      }
      return;
    }

    if(state == State::Write) {
      if(index >= 0 && index < 12) {
        rtcWrite(index++, data);
        if(index == 12) {
          //day of week is automatically calculated and written
          weekday = calculateWeekday(1000 + year, month, day);
        }
      }
      return;
    }
  }
}
