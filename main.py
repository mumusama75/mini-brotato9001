import pygame
import random
import math
import time
import os
import sys

def resource_path(filename):
    base_path = getattr(sys, '_MEIPASS', os.path.dirname(os.path.abspath(__file__)))
    return os.path.join(base_path, "assets", filename)


# 初始化
pygame.init()
WIDTH, HEIGHT = 800, 600
WIN = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Mini Brotato")

# 常量
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
GREEN = (0, 255, 0)
PURPLE = (160, 32, 240)
ORANGE = (255, 165, 0)

FONT = pygame.font.Font(resource_path("arial.ttf"), 24)

# 音效
pygame.mixer.init()
try:
    pygame.mixer.music.load(resource_path("background_music.mp3"))
    pygame.mixer.music.play(-1)
    shoot_sound = pygame.mixer.Sound("shoot.wav")
    hit_sound = pygame.mixer.Sound("hit.wav")
    upgrade_sound = pygame.mixer.Sound("upgrade.wav")
except:
    shoot_sound = hit_sound = upgrade_sound = None

# 遗物效果（全局变量控制）
RELICS = {
    "reflect": False,
    "lifesteal": False,
}

# 玩家类
class Player:
    def __init__(self):
        self.x = WIDTH // 2
        self.y = HEIGHT // 2
        self.radius = 20
        self.speed = 5
        self.hp = 100
        self.max_hp = 100
        self.shoot_cooldown = 20
        self.shoot_timer = 0
        self.weapon_levels = {"shotgun": 0, "pierce": 0, "homing": 0}
        self.unlocked_weapons = []

    def draw(self, win):
        pygame.draw.circle(win, BLUE, (self.x, self.y), self.radius)

    def move(self, keys):
        if keys[pygame.K_w] and self.y - self.radius > 0:
            self.y -= self.speed
        if keys[pygame.K_s] and self.y + self.radius < HEIGHT:
            self.y += self.speed
        if keys[pygame.K_a] and self.x - self.radius > 0:
            self.x -= self.speed
        if keys[pygame.K_d] and self.x + self.radius < WIDTH:
            self.x += self.speed

class Bullet:
    def __init__(self, x, y, target_x, target_y, pierce=0, homing_level=0, bounces=0):
        self.x = x
        self.y = y
        self.speed = 10
        angle = math.atan2(target_y - y, target_x - x)
        self.dx = math.cos(angle) * self.speed
        self.dy = math.sin(angle) * self.speed
        self.radius = 5
        self.pierce = pierce
        self.homing = homing_level
        self.bounces = bounces

    def update(self, enemies):
        if self.homing and enemies:
            closest = min(enemies, key=lambda e: math.hypot(self.x - e.x, self.y - e.y))
            angle = math.atan2(closest.y - self.y, closest.x - self.x)
            self.dx += (math.cos(angle) - self.dx) * 0.1 * self.homing
            self.dy += (math.sin(angle) - self.dy) * 0.1 * self.homing
            norm = math.hypot(self.dx, self.dy)
            self.dx = self.dx / norm * self.speed
            self.dy = self.dy / norm * self.speed

        self.x += self.dx
        self.y += self.dy

        # 反弹效果
        if RELICS["reflect"]:
            if self.x <= 0 or self.x >= WIDTH:
                if self.bounces > 0:
                    self.dx *= -1
                    self.bounces -= 1
            if self.y <= 0 or self.y >= HEIGHT:
                if self.bounces > 0:
                    self.dy *= -1
                    self.bounces -= 1

    def draw(self, win):
        pygame.draw.circle(win, YELLOW, (int(self.x), int(self.y)), self.radius)

    def is_off_screen(self):
        return not (0 <= self.x <= WIDTH and 0 <= self.y <= HEIGHT)

class Enemy:
    def __init__(self, type="normal", boss=False, difficulty=1):
        self.boss = boss
        self.type = type
        self.radius = 15 * (2 if boss else 1)
        self.hp = 100 * difficulty if boss else 1
        self.color = ORANGE if boss else (RED if type == "normal" else GREEN if type == "dash" else PURPLE)
        self.speed = (1.5 + 0.1 * difficulty) if boss else (2 if type == "normal" else 4 if type == "dash" else 1)
        self.x = random.randint(0, WIDTH)
        self.y = random.randint(0, HEIGHT)

    def move_towards(self, player):
        angle = math.atan2(player.y - self.y, player.x - self.x)
        self.x += math.cos(angle) * self.speed
        self.y += math.sin(angle) * self.speed

    def draw(self, win):
        pygame.draw.circle(win, self.color, (int(self.x), int(self.y)), self.radius)

    def is_hit(self, bullet):
        dist = math.hypot(self.x - bullet.x, self.y - bullet.y)
        return dist < self.radius + bullet.radius

class HitEffect:
    def __init__(self, x, y):
        self.x = x
        self.y = y
        self.timer = 10

    def update(self):
        self.timer -= 1

    def draw(self, win):
        if self.timer > 0:
            pygame.draw.circle(win, RED, (int(self.x), int(self.y)), 20 - self.timer * 2, 1)

def find_closest_enemy(player, enemies):
    if not enemies:
        return None
    return min(enemies, key=lambda e: math.hypot(player.x - e.x, player.y - e.y))


def fire_weapons(player, bullets, enemies):
    target = find_closest_enemy(player, enemies)
    if not target:
        return
    for weapon, level in player.weapon_levels.items():
        if level <= 0:
            continue
        if weapon == "shotgun":
            spread = 10 + level * 5
            count = 3 + level
            angle_center = math.atan2(target.y - player.y, target.x - player.x)
            for _ in range(count):
                angle = angle_center + math.radians(random.uniform(-spread, spread))
                dx = math.cos(angle)
                dy = math.sin(angle)
                bullets.append(Bullet(player.x, player.y, player.x + dx * 10, player.y + dy * 10, bounces=(1 if RELICS["reflect"] else 0)))
        else:
            pierce = level if weapon == "pierce" else 0
            homing = level if weapon == "homing" else 0
            bullets.append(Bullet(player.x, player.y, target.x, target.y, pierce=pierce, homing_level=homing, bounces=(1 if RELICS["reflect"] else 0)))

def show_weapon_upgrade(player):
    pool = list(player.weapon_levels.keys())
    choices = random.sample(pool, 3)
    WIN.fill(BLACK)
    WIN.blit(FONT.render("Choose a weapon upgrade:", True, WHITE), (WIDTH // 2 - 120, 100))
    for i, w in enumerate(choices):
        lvl = player.weapon_levels[w]
        WIN.blit(FONT.render(f"{i+1}. {w.title()} (Lv.{lvl})", True, WHITE), (WIDTH // 2 - 100, 160 + i * 40))
    pygame.display.update()
    selecting = True
    while selecting:
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN and pygame.K_1 <= event.key <= pygame.K_3:
                selected = choices[event.key - pygame.K_1]
                player.weapon_levels[selected] += 1
                selecting = False

def show_relic_selection():
    choices = ["reflect", "lifesteal", "none"]
    relic_names = {
        "reflect": "Reflect Bullets",
        "lifesteal": "Lifesteal",
        "none": "Skip"
    }
    WIN.fill(BLACK)
    WIN.blit(FONT.render("Boss defeated! Choose a relic:", True, WHITE), (WIDTH // 2 - 150, 100))
    for i, r in enumerate(choices):
        WIN.blit(FONT.render(f"{i+1}. {relic_names[r]}", True, WHITE), (WIDTH // 2 - 100, 160 + i * 40))
    pygame.display.update()
    selecting = True
    while selecting:
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN and pygame.K_1 <= event.key <= pygame.K_3:
                chosen = choices[event.key - pygame.K_1]
                if chosen in RELICS:
                    RELICS[chosen] = True
                selecting = False

def pause_menu():
    overlay = pygame.Surface((WIDTH, HEIGHT), pygame.SRCALPHA)
    overlay.fill((0, 0, 0, 150))
    WIN.blit(overlay, (0, 0))
    WIN.blit(FONT.render("PAUSED - Press R to resume", True, WHITE), (WIDTH // 2 - 150, HEIGHT // 2))
    pygame.display.update()
    while True:
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN and event.key == pygame.K_r:
                return

def start_menu():
    WIN.fill(BLACK)
    title = FONT.render("Mini Brotato+", True, WHITE)
    start = FONT.render("Press ENTER to Start", True, WHITE)
    WIN.blit(title, (WIDTH // 2 - title.get_width() // 2, HEIGHT // 2 - 60))
    WIN.blit(start, (WIDTH // 2 - start.get_width() // 2, HEIGHT // 2 + 10))
    pygame.display.update()
    waiting = True
    while waiting:
        for event in pygame.event.get():
            if event.type == pygame.KEYDOWN and event.key == pygame.K_RETURN:
                waiting = False

def main():
    start_menu()
    clock = pygame.time.Clock()
    run = True
    player = Player()
    bullets = []
    enemies = [Enemy() for _ in range(5)]
    effects = []
    score = 0
    last_weapon_score = 0
    upgrade_timer = 30
    shop_timer = 60
    boss_spawn_timer = 60
    last_boss_time = 0
    difficulty = 1
    start_time = time.time()
    duration = 10 * 60
    show_weapon_upgrade(player)

    while run:
        clock.tick(60)
        WIN.fill(BLACK)
        elapsed = time.time() - start_time
        remaining = max(0, int(duration - elapsed))
        minutes = remaining // 60
        seconds = remaining % 60

        if remaining <= 0:
            WIN.blit(FONT.render("You Survived!", True, WHITE), (WIDTH // 2 - 60, HEIGHT // 2))
            pygame.display.update()
            pygame.time.delay(5000)
            break

        if len(enemies) < 10 + difficulty:
            enemies.append(Enemy(random.choice(["normal", "dash", "ranged"])))

        if int(elapsed) - last_boss_time >= boss_spawn_timer:
            enemies.append(Enemy(type=random.choice(["dash", "ranged", "normal"]), boss=True, difficulty=difficulty))
            last_boss_time = int(elapsed)
            difficulty += 1

        if score // 50 > last_weapon_score // 50:
            show_weapon_upgrade(player)
            last_weapon_score = score

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                pause_menu()

        keys = pygame.key.get_pressed()
        player.move(keys)

        if player.shoot_timer == 0:
            fire_weapons(player, bullets, enemies)
            if shoot_sound:
                shoot_sound.play()
            player.shoot_timer = player.shoot_cooldown
        else:
            player.shoot_timer -= 1

        for bullet in bullets[:]:
            bullet.update(enemies)
            if bullet.is_off_screen() and bullet.bounces <= 0:
                bullets.remove(bullet)

        for bullet in bullets[:]:
            for enemy in enemies[:]:
                if enemy.is_hit(bullet):
                    if bullet.pierce > 0:
                        bullet.pierce -= 1
                    elif bullet.bounces <= 0:
                        if bullet in bullets:
                            bullets.remove(bullet)

                    enemy.hp -= 25
                    effects.append(HitEffect(enemy.x, enemy.y))

                    if enemy.hp <= 0:
                        if enemy in enemies:
                            if enemy.boss:
                                show_relic_selection()
                                score += 10
                            else:
                                score += 1
                            enemies.remove(enemy)

                    if RELICS["lifesteal"]:
                        player.hp = min(player.max_hp, player.hp + 1)

                    if hit_sound:
                        hit_sound.play()
                    break

        for enemy in enemies:
            enemy.move_towards(player)
            if math.hypot(player.x - enemy.x, player.y - enemy.y) < player.radius + enemy.radius:
                player.hp -= 1
                enemies.remove(enemy)
                if player.hp <= 0:
                    WIN.blit(FONT.render("You Died...", True, RED), (WIDTH // 2 - 40, HEIGHT // 2))
                    pygame.display.update()
                    pygame.time.delay(3000)
                    run = False

        for effect in effects[:]:
            effect.update()
            if effect.timer <= 0:
                effects.remove(effect)

        # Draw everything
        player.draw(WIN)
        for bullet in bullets:
            bullet.draw(WIN)
        for enemy in enemies:
            enemy.draw(WIN)
        for effect in effects:
            effect.draw(WIN)

        WIN.blit(FONT.render(f"Score: {score}", True, WHITE), (10, 10))
        WIN.blit(FONT.render(f"HP: {player.hp}/{player.max_hp}", True, WHITE), (10, 40))
        WIN.blit(FONT.render(f"Time: {minutes:02}:{seconds:02}", True, WHITE), (10, 70))

        y_offset = 110
        for w, lv in player.weapon_levels.items():
            if lv > 0:
                WIN.blit(FONT.render(f"{w.title()}: Lv{lv}", True, WHITE), (10, y_offset))
                y_offset += 25

        for r, active in RELICS.items():
            if active:
                WIN.blit(FONT.render(f"RELIC: {r.title()}", True, YELLOW), (10, y_offset))
                y_offset += 25

        pygame.display.update()

    pygame.quit()

if __name__ == "__main__":
    main()
