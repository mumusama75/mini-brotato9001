import pygame
pygame.init()

screen = pygame.display.set_mode((400, 300))
pygame.display.set_caption("Font Test")
font = pygame.font.Font("arial.ttf", 32)

text = font.render("Test: Hello World!", True, (255, 255, 255))
running = True

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    screen.fill((0, 0, 0))
    screen.blit(text, (50, 120))
    pygame.display.flip()

pygame.quit()
