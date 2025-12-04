# BTMagus
**BTMagus** is an experimental Bluetooth exploitation and analysis toolkit written in C++.  
It is currently in early development, but already provides a functional scanning and detection engine.

---

## 🚀 Features

### 🔧 CLI Interface
Built using the lightweight and powerful [`daniele77/cli`](https://github.com/daniele77/cli) library.

### 🧵 Multithreading
Commands can run in parallel.

### 🔌 Direct DBus Integration
Communicates with the BlueZ Bluetooth daemon over DBus for low-level BLE interaction.

---

## 📡 Current Capabilities

- **Bluetooth Device Scanning** — Enumerates nearby BLE devices in real time
- **Flipper Zero Detection (🐬)** — Detects genuine Flipper Zero devices based on their MAC patterns
- **Spoofed Flipper Detection (🎭)** — Identifies flippers with faked MAC addresses using BLE advertisement UUIDs
- **Bluetooth Flood Attack Detection** — Recognizes BLE spam attacks caused by the Flipper Zero

---

## 🗺️ Roadmap

### **✔️ flipper_scan – Beta**
- Improve accuracy of flood attack detection
- Add optional logging

### **➡️ Next: Bluetooth Spam Attack Module**
- BLE flood attack similar to the one in flipper zero

### **Future (Planned)**
- Active Bluetooth exploitation utilities
- Custom attack modules
- Extended BlueZ control features
- Plugin system for custom analyzers

---

## 📦 Installation & Usage
(Coming soon — documentation will expand as features stabilize.)

---

## 📄 Release Notes
Latest release: **v0.1.0-alpha**  
Full changelog:  
https://github.com/GitClo/BTMagus/commits/v0.1.0-alpha
