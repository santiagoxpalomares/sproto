# sproto

A custom binary application protocol over TCP, implemented from scratch in C —
used here to talk between a PC and an ESP32.

## How it's wired up

The ESP32 connects **out** to your PC (your PC runs the server and just
waits). Once connected, sensor data flows ESP32 → PC and commands flow
PC → ESP32, on the same socket.

`protocol.c` is written in plain C using raw BSD-style sockets
(`socket()`/`send()`/`recv()`). This file is used **unmodified on both
sides** — ESP32's networking stack (lwIP) supports the same raw socket
calls even from an Arduino sketch, so there's no duplicate protocol
logic to maintain.

## PC side

```
make          # builds build/server and build/test_client
make test     # runs the protocol unit test
./build/server 9090
```

`test_client` simulates an ESP32 (connects out, sends fake sensor
data, listens briefly for a command) so you can test the whole flow
without hardware:

```
./build/test_client 127.0.0.1 9090
```

## ESP32 side

1. Open `esp32/sproto_esp32/sproto_esp32.ino` in the Arduino IDE.
   `protocol.h` and `protocol.c` are already copied into that folder
   so the IDE picks them up automatically.
2. Edit `WIFI_SSID`, `WIFI_PASSWORD`, and `SERVER_IP` (your PC's LAN
   IP) at the top of the sketch.
3. Start `./build/server` on your PC first.
4. Flash the sketch. Open the Serial Monitor at 115200 baud — you
   should see it connect to WiFi, connect to the server, and start
   sending fake sensor readings every 5 seconds.

Right now the sketch sends a placeholder sensor reading and just acks
any command it receives — see the `TODO`s in `send_fake_sensor_reading()`
and `handle_command()` in the `.ino` file; that's where you'll plug in
a real sensor and real actions.

## Layout

```
sproto/
├── include/protocol.h        # wire format + public API
├── src/protocol.c             # framing (shared by PC and ESP32)
├── src/server.c                # PC server
├── src/test_client.c           # PC-side ESP32 simulator, for testing without hardware
├── tests/test_protocol.c       # unit test for framing
├── esp32/sproto_esp32/         # Arduino sketch (includes copies of protocol.h/.c)
└── docs/ROADMAP.md
```

## Roadmap

See `docs/ROADMAP.md`. Phases 1 and 2 (raw sockets, framing) are done.
Next up: Phase 3 — replace the fake sensor reading and the no-op
command handler with real behavior.
