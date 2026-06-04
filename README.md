# 🔐 Knight

A lightweight Linux-based CLI tool written in C for experimenting with **domain/IP blocking using in-memory rule management and `/etc/hosts` based enforcement**.

---

## 📌 Overview

Knight is a system-level learning project designed to explore how domain/IP blocking works in Linux.

It combines:

* A **rule engine (in-memory hash map using uthash)**
* A **persistent config file (`blocked.txt`)**
* A **system-level enforcement layer using `/etc/hosts`**

This allows both:

* Runtime management of blocked rules
* Actual blocking of domains at the OS resolution level

---

## ⚙️ How It Works

Knight operates in two layers:

### 🧠 1. Rule Engine (Memory Layer)

* Uses a hash map (`uthash`) to store blocked domains/IPs
* Provides fast lookup, add, and remove operations
* Represents the active runtime blocklist

### 💾 2. Persistence Layer

* Stores blocked entries in `blocked.txt`
* Loads rules on startup
* Ensures blocked list persists across sessions

### 🌐 3. Enforcement Layer (`/etc/hosts`)

* Maps blocked domains to `0.0.0.0`
* Forces system-level DNS failure for those domains
* Effectively prevents connection at resolution stage

---

## 🧪 CLI Usage

```bash id="u9k3x1"
./knight block google.com
./knight unblock google.com
./knight block show
```

### Commands

* `block <domain/ip>` → Block a domain/IP (adds rule + updates system)
* `unblock <domain/ip>` → Remove block rule (removes from memory + system)
* `block show` → Show current blocked list

---

## 🧱 Architecture

* Language: C
* Data Structure: hash map (`uthash`)
* Persistence: `blocked.txt`
* System Integration: `/etc/hosts`
* Execution Model:

  * CLI-based (each command executes independently)
  * Rules are loaded and applied per execution

---

## 🧠 Design Notes

* The hash map acts as the **runtime rule engine**
* File storage ensures **persistence across reboots**
* `/etc/hosts` acts as the **actual enforcement layer**
* Current design is CLI-driven, but can evolve into a **long-running daemon** where rules stay active in memory continuously

---

## 🚀 Future Scope (Evolution Path)

This project is the foundation for deeper system-level network control.


- 🌐 Phase 2 — DNS Layer Control (Major Upgrade)
- 🔥 Phase 4 — Kernel-Level Packet Filtering

---

## ⚠️ Limitations (Current Stage)

* Depends on `/etc/hosts` for enforcement
* Requires root privileges for system modification
* Does not yet inspect network packets directly
* CLI-based execution model (not daemon yet)

---

## 🎯 Goal of Project

This project is built for **learning and experimentation**, focusing on:

* Low-level C programming
* Hash maps and rule systems
* Linux DNS resolution behavior
* System-level networking concepts
* Gradual evolution toward firewall/DNS control systems

---

## ⚠️ Disclaimer

This project is strictly for **educational purposes only**.
It is not intended for production security enforcement or malicious use.