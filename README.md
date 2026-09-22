# Mini Brotato+

**Python / Pygame 生存射击游戏原型**

![Python](https://img.shields.io/badge/Python-3-3776AB?logo=python&logoColor=white)
![Pygame](https://img.shields.io/badge/Pygame-2.6.1-2D8C3C)
![Status](https://img.shields.io/badge/Status-Prototype-6F42C1)

一个受 *Brotato* 启发的俯视角生存射击练习项目：玩家通过移动躲避敌人，武器自动锁定目标，在持续战斗中升级武器、击败 Boss 并选择遗物。画面使用几何图形，重点放在玩法循环和交互逻辑的实现。

*A Python / Pygame survival-shooter prototype exploring auto-targeting, weapon progression and time-based encounters.*

[玩法与操作](#玩法与操作) · [本地运行](#本地运行) · [代码导览](#代码导览) · [后续迭代](#后续迭代)

## 玩法与操作

**开始游戏 → 选择初始武器 → 移动与自动射击 → 积分升级 / Boss 遗物 → 尝试生存 10 分钟**

| 系统 | 当前实现 |
| --- | --- |
| 战斗 | 自动寻找最近敌人；子弹与敌人的圆形碰撞检测；生命值与命中反馈 |
| 武器成长 | 散弹、穿透、追踪三种武器；开局选择一次，之后每累计 50 分再选择升级 |
| 敌人与节奏 | 不同速度的追逐敌人；约每 60 秒生成 Boss，后续 Boss 的生命值与速度随难度增加 |
| 遗物选择 | 击败 Boss 后可选择子弹边界反弹、命中回血，或跳过 |
| 界面 | 开始菜单、暂停界面，以及生命值、得分、倒计时、武器等级和遗物状态显示 |

| 操作 | 按键 |
| --- | --- |
| 开始游戏 | `Enter` |
| 上 / 左 / 下 / 右移动 | `W` / `A` / `S` / `D` |
| 射击 | 自动进行，无需鼠标操作 |
| 武器升级 / 遗物选择 | `1` / `2` / `3` |
| 暂停 / 继续 | `Esc` / `R` |

游戏界面当前使用英文。普通敌人击败后获得 1 分，Boss 为 10 分；生命值耗尽则本局结束。

## 本地运行

需要 **Python 3**、可用的桌面图形环境和音频设备。依赖文件固定为 `pygame==2.6.1`。

```bash
git clone https://github.com/mumusama75/mini-brotato9001.git
cd mini-brotato9001
python -m venv .venv
```

激活虚拟环境：

```powershell
# Windows PowerShell
.\.venv\Scripts\Activate.ps1
```

```bash
# macOS / Linux
source .venv/bin/activate
```

随后在项目根目录安装依赖并启动：

```bash
python -m pip install -r requirements.txt
python main.py
```

请保留 `assets/` 目录：程序会读取其中的字体与背景音乐。仓库中的 `setup.py` 目前是空文件，请直接通过 `main.py` 运行。

## 代码导览

当前逻辑集中在 `main.py`，便于沿着单一入口阅读游戏循环。

```text
mini-brotato9001/
├── main.py                 # 游戏入口、对象、战斗、成长与菜单
├── requirements.txt        # Pygame 依赖
├── assets/
│   ├── arial.ttf           # 界面字体
│   ├── background_music.mp3
│   ├── test.py             # 手动字体显示脚本，并非自动化测试
│   └── objects.py          # 预留空文件
├── setup.py                # 预留空文件
└── LICENSE
```

| 阅读入口 | 关注点 |
| --- | --- |
| `Player` / `Enemy` / `Bullet` | 玩家移动、敌人追踪、弹道更新和碰撞 |
| `find_closest_enemy()` / `fire_weapons()` | 最近目标选择与不同武器的发射逻辑 |
| `show_weapon_upgrade()` / `show_relic_selection()` | 升级选择与状态变化 |
| `main()` | 输入、时间推进、生成敌人、战斗结算与绘制 |

## 当前版本说明

这是可继续迭代的游戏原型，以下细节尚待完善：

- 敌人的类型名称包含 `ranged`，但当前各类型均使用追逐移动，尚未实现敌方远程攻击；也尚无商店流程。
- 倒计时使用实际经过时间，暂停和升级选择期间仍会计时；移动、射击与部分反馈以帧为单位更新。
- 仓库没有 `shoot.wav`、`hit.wav`、`upgrade.wav`，对应音效加载失败时会被禁用。音频初始化仍依赖可用设备。
- 开始、暂停及选择菜单尚未统一处理关闭窗口事件。若菜单中无法关闭窗口，可在启动终端按 `Ctrl+C` 结束进程。

以上功能说明依据当前源代码整理；各操作系统的运行兼容性仍需实测。

## 后续迭代

- [ ] 统一菜单与战斗状态，处理暂停计时、窗口关闭和重新开始。
- [ ] 引入基于时间间隔的移动与冷却，让表现减少对帧率的依赖。
- [ ] 将武器、敌人和成长数值抽成配置，便于比较不同参数下的生存节奏。
- [ ] 补充实际游玩录屏、截图和调参记录，展示玩法变化与验证过程。
- [ ] 完善音频回退逻辑，整理资源来源与署名信息。

## 许可

仓库许可见 [LICENSE](LICENSE)。玩法灵感来自 *Brotato*；本项目是学习与原型实践，与原游戏无官方关联。
