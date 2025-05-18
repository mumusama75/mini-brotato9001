# Mini Brotato+

A modular Pygame bullet-hell survival game inspired by Brotato. Built with Python.

---

##  Features

- WASD movement + auto-target shooting  
- Weapon upgrade system (every 50 points, 3 random choices)  
- Bosses spawn every 60 seconds, drop relics  
- Relic system (e.g. bullet reflection, lifesteal)  
- Visual effects, start/pause menu, 10-minute survival

---

##  How to Run

### 1. Install dependencies

```bash
pip install pygame
```

Or install all dependencies:

```bash
pip install -r requirements.txt
```

### 2. Run the game

```bash
python main.py
```

### 3. Controls

- `W A S D` – Move  
- Game auto-shoots the nearest enemy  
- `ESC` – Pause  
- `1 2 3` – Make upgrade/relic/shop selections

---

##  File Structure

```
.
├── main.py            # main game logic
├── assets/            # fonts, sound effects, background music
│   ├── arial.ttf
│   └── background_music.mp3
└── README.md
```

---

## 📝 License

MIT License. Free for modification and distribution.
