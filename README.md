# BTMagus

BTMagus is an experimental Bluetooth exploitation and analysis toolkit written in C++.

It is currently in early development but already provides a functional scanning and detection engine.

---

## 🚀 Features

### 🔧 CLI Interface
Built using the lightweight and powerful [daniele77/cli](https://github.com/daniele77/cli) library.

### 🧵 Multithreading
Commands can run in parallel.

### 🔌 Direct DBus Integration
Communicates with the BlueZ Bluetooth daemon over DBus for low-level BLE interaction.

---

## 📡 Current Capabilities

- **Bluetooth Device Scanning**  
  Enumerates nearby BLE devices in real time.

- **Flipper Zero Detection (🐬)**  
  Detects genuine Flipper Zero devices based on their MAC address patterns.

- **Spoofed Flipper Detection (🎭)**  
  Identifies Flipper devices with faked MAC addresses using BLE advertisement UUIDs.

- **Bluetooth Flood Attack Detection**  
  Recognizes BLE spam attacks caused by the Flipper Zero.

- **Bluetooth Spam Attack Module (New)**  
  Performs BLE flood attacks similar to the Flipper Zero for testing and analysis.

---

## 🗺️ Roadmap

### ✔️ `flipper_scan` – Stable
- Improved accuracy of flood attack detection
- Optional logging features implemented

### ➡️ Next: Active Exploitation
- Active Bluetooth exploitation utilities
- Custom attack modules

### 🔮 Future (Planned)
- Extended BlueZ control features
- Plugin system for custom analyzers

---

## 📦 Installation & Usage

### 1️⃣ Download

Go to the Releases page:  
https://github.com/GitClo/BTMagus/releases

Download the latest binary for your architecture.

---

### 2️⃣ Run

BTMagus requires a Linux environment with BlueZ installed.  
You must run the binary with root privileges to access the Bluetooth adapter.

```bash
# Give execution permissions
chmod +x btmagus

# Run as root
sudo ./btmagus
