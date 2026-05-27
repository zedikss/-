#include <GLut/glut.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- Константы ---
#define MAX_FISH 10           // Максимальное количество рыбок
#define MAX_BUBBLES 100       // Максимальное количество пузырей
#define FISH_SPEED 0.03f      // Скорость рыбок
#define BUBBLE_SPEED 0.05f    // Скорость пузырей
#define PI 3.14159265f

// --- Структуры ---
typedef struct {
    float x, y;           // Позиция
    float vx, vy;         // Скорость
    float r, g, b;        // Цвет
    float size;           // Размер
    float direction;      // Направление (в радианах)
    float wiggle_angle;   // Угол для плавных движений
    float wiggle_speed;   // Скорость колебаний
    int bubble_timer;     // Таймер до создания пузыря
    int is_alive;         // 1 = активна, 0 = неактивна
} Fish;

typedef struct {
    float x, y;           // Позиция
    float vy;             // Скорость (только вверх)
    float size;           // Размер
    float growth_rate;    // Скорость роста
    int is_alive;         // 1 = активен, 0 = неактивен
    float opacity;        // Прозрачность
} Bubble;

// --- Глобальные переменные ---
Fish fishes[MAX_FISH];
Bubble bubbles[MAX_BUBBLES];
int window_width = 800;
int window_height = 600;
int frames = 0;
float aquarium_left = -9.0f;
float aquarium_right = 9.0f;
float aquarium_bottom = -8.0f;
float aquarium_top = 8.0f;

// --- Вспомогательные функции ---
float random_float(float min, float max) {
    return min + (float)rand() / (float)RAND_MAX * (max - min);
}

// --- Рисование круга ---
void draw_circle(float x, float y, float radius, int segments) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);  // Центр круга
    for (int i = 0; i <= segments; i++) {
        float angle = 2.0f * PI * (float)i / (float)segments;
        glVertex2f(x + cosf(angle) * radius, y + sinf(angle) * radius);
    }
    glEnd();
}

// --- Рисование рыбки (треугольник + хвост) ---
void draw_fish(float x, float y, float size, float direction, float wiggle, float r, float g, float b) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f);
    glRotatef(direction * 180.0f / PI, 0.0f, 0.0f, 1.0f);

    // Тело рыбки (эллипс)
    glColor3f(r, g, b);
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * PI * (float)i / 20.0f;
        float rx = cosf(angle) * size;
        float ry = sinf(angle) * size * 0.6f;
        glVertex2f(rx, ry);
    }
    glEnd();

    // Хвост (треугольник с колебаниями)
    glColor3f(r * 0.8f, g * 0.8f, b * 0.8f);
    glBegin(GL_TRIANGLES);
    float tail_wiggle = sinf(wiggle) * 0.3f;
    glVertex2f(-size * 0.8f, 0.0f);
    glVertex2f(-size * 1.5f, size * 0.7f + tail_wiggle);
    glVertex2f(-size * 1.5f, -size * 0.7f - tail_wiggle);
    glEnd();

    // Глаз
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_circle(size * 0.5f, size * 0.2f, size * 0.15f, 10);
    glColor3f(0.0f, 0.0f, 0.0f);
    draw_circle(size * 0.5f, size * 0.2f, size * 0.07f, 10);

    // Плавники
    glColor3f(r * 0.9f, g * 0.9f, b * 0.9f);
    glBegin(GL_TRIANGLES);
    // Верхний плавник
    glVertex2f(-size * 0.3f, size * 0.4f);
    glVertex2f(size * 0.2f, size * 0.6f);
    glVertex2f(size * 0.5f, size * 0.4f);
    // Нижний плавник
    glVertex2f(-size * 0.3f, -size * 0.4f);
    glVertex2f(size * 0.2f, -size * 0.6f);
    glVertex2f(size * 0.5f, -size * 0.4f);
    glEnd();

    glPopMatrix();
}

// --- Инициализация ---
void init() {
    srand((unsigned int)time(NULL));

    // Инициализация рыбок
    for (int i = 0; i < MAX_FISH; i++) {
        fishes[i].x = random_float(aquarium_left, aquarium_right);
        fishes[i].y = random_float(aquarium_bottom, aquarium_top);
        fishes[i].vx = random_float(-FISH_SPEED, FISH_SPEED);
        fishes[i].vy = random_float(-FISH_SPEED * 0.3f, FISH_SPEED * 0.3f);

        // Выбор цвета рыбок
        int color_type = rand() % 3;
        if (color_type == 0) {
            // Золотая рыбка
            fishes[i].r = random_float(0.9f, 1.0f);
            fishes[i].g = random_float(0.6f, 0.7f);
            fishes[i].b = random_float(0.1f, 0.2f);
        }
        else if (color_type == 1) {
            // Серебряная рыбка
            fishes[i].r = random_float(0.7f, 0.8f);
            fishes[i].g = random_float(0.7f, 0.8f);
            fishes[i].b = random_float(0.8f, 0.9f);
        }
        else {
            // Цветная рыбка
            fishes[i].r = random_float(0.5f, 1.0f);
            fishes[i].g = random_float(0.3f, 0.8f);
            fishes[i].b = random_float(0.3f, 0.8f);
        }

        fishes[i].size = random_float(0.3f, 0.6f);
        fishes[i].direction = atan2f(fishes[i].vy, fishes[i].vx);
        fishes[i].wiggle_angle = random_float(0.0f, 2.0f * PI);
        fishes[i].wiggle_speed = random_float(0.1f, 0.3f);
        fishes[i].bubble_timer = (int)random_float(30.0f, 120.0f);
        fishes[i].is_alive = 1;
    }

    // Инициализация пузырей
    for (int i = 0; i < MAX_BUBBLES; i++) {
        bubbles[i].is_alive = 0;
    }
}

// --- Создание пузыря ---
void create_bubble(float x, float y) {
    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (!bubbles[i].is_alive) {
            bubbles[i].x = x + random_float(-0.2f, 0.2f);
            bubbles[i].y = y;
            bubbles[i].vy = random_float(BUBBLE_SPEED * 0.8f, BUBBLE_SPEED * 1.2f);
            bubbles[i].size = random_float(0.05f, 0.12f);
            bubbles[i].growth_rate = random_float(0.0005f, 0.0015f);
            bubbles[i].is_alive = 1;
            bubbles[i].opacity = random_float(0.6f, 0.9f);
            return;
        }
    }
}

// --- Обновление состояния ---
void update(int value) {
    frames++;

    // Обновление рыбок
    for (int i = 0; i < MAX_FISH; i++) {
        if (fishes[i].is_alive) {
            // Движение
            fishes[i].x += fishes[i].vx;
            fishes[i].y += fishes[i].vy;

            // Колебания для плавного движения
            fishes[i].wiggle_angle += fishes[i].wiggle_speed;

            // Обновление направления
            fishes[i].direction = atan2f(fishes[i].vy, fishes[i].vx);

            // Отражение от стенок аквариума
            if (fishes[i].x < aquarium_left || fishes[i].x > aquarium_right) {
                fishes[i].vx = -fishes[i].vx;
                // Небольшое случайное изменение
                fishes[i].vx += random_float(-0.01f, 0.01f);
            }
            if (fishes[i].y < aquarium_bottom || fishes[i].y > aquarium_top) {
                fishes[i].vy = -fishes[i].vy;
                fishes[i].vy += random_float(-0.005f, 0.005f);
            }

            // Ограничение скорости
            float speed = sqrtf(fishes[i].vx * fishes[i].vx + fishes[i].vy * fishes[i].vy);
            if (speed > FISH_SPEED * 1.5f) {
                fishes[i].vx = (fishes[i].vx / speed) * FISH_SPEED;
                fishes[i].vy = (fishes[i].vy / speed) * FISH_SPEED;
            }

            // Таймер для создания пузыря
            fishes[i].bubble_timer--;
            if (fishes[i].bubble_timer <= 0) {
                create_bubble(fishes[i].x, fishes[i].y);
                fishes[i].bubble_timer = (int)random_float(60.0f, 180.0f);
            }

            // Небольшое случайное изменение направления
            if (rand() % 100 < 2) {
                fishes[i].vx += random_float(-0.01f, 0.01f);
                fishes[i].vy += random_float(-0.005f, 0.005f);
            }
        }
    }

    // Обновление пузырей
    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (bubbles[i].is_alive) {
            // Движение вверх
            bubbles[i].y += bubbles[i].vy;

            // Небольшое горизонтальное колебание
            bubbles[i].x += sinf(frames * 0.05f + i) * 0.005f;

            // Увеличение размера
            bubbles[i].size += bubbles[i].growth_rate;

            // Медленное уменьшение прозрачности
            bubbles[i].opacity *= 0.995f;

            // Исчезновение при достижении верха, слишком большого размера или малой прозрачности
            if (bubbles[i].y > aquarium_top + 1.0f ||
                bubbles[i].size > 0.25f ||
                bubbles[i].opacity < 0.1f) {
                bubbles[i].is_alive = 0;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- Рисование ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Фон аквариума (вода)
    glBegin(GL_QUADS);
    glColor3f(0.1f, 0.2f, 0.4f);        // Темно-синий верх
    glVertex2f(-10.0f, 10.0f);
    glVertex2f(10.0f, 10.0f);
    glColor3f(0.2f, 0.4f, 0.6f);        // Светло-синий низ
    glVertex2f(10.0f, -10.0f);
    glVertex2f(-10.0f, -10.0f);
    glEnd();

    // Дно аквариума (песок)
    glColor3f(0.76f, 0.70f, 0.50f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -10.0f);
    glVertex2f(10.0f, -10.0f);
    glVertex2f(10.0f, -8.0f);
    glVertex2f(-10.0f, -8.0f);
    glEnd();

    // Декорации (водоросли)
    glColor3f(0.2f, 0.6f, 0.3f);
    for (int i = 0; i < 5; i++) {
        float x = -8.0f + i * 4.0f;
        glBegin(GL_TRIANGLE_STRIP);
        for (int j = 0; j < 10; j++) {
            float y = -8.0f + j * 0.5f;
            float offset = sinf(frames * 0.02f + i + j) * 0.3f;
            glVertex2f(x + offset, y);
            glVertex2f(x + 0.5f + offset, y);
        }
        glEnd();
    }

    // Камни на дне
    glColor3f(0.4f, 0.4f, 0.4f);
    for (int i = 0; i < 7; i++) {
        float x = -8.5f + i * 2.5f;
        float y = -8.5f;
        float size = random_float(0.3f, 0.6f);
        draw_circle(x, y, size, 12);
    }

    // Пузыри (с прозрачностью)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (bubbles[i].is_alive) {
            // Белый цвет с прозрачностью
            glColor4f(0.9f, 0.95f, 1.0f, bubbles[i].opacity);

            // Основной пузырь
            draw_circle(bubbles[i].x, bubbles[i].y, bubbles[i].size, 16);

            // Блик на пузыре
            glColor4f(1.0f, 1.0f, 1.0f, bubbles[i].opacity * 0.8f);
            draw_circle(bubbles[i].x - bubbles[i].size * 0.3f,
                bubbles[i].y + bubbles[i].size * 0.3f,
                bubbles[i].size * 0.3f, 8);
        }
    }

    // Рыбки
    for (int i = 0; i < MAX_FISH; i++) {
        if (fishes[i].is_alive) {
            draw_fish(fishes[i].x, fishes[i].y,
                fishes[i].size,
                fishes[i].direction,
                fishes[i].wiggle_angle,
                fishes[i].r, fishes[i].g, fishes[i].b);
        }
    }

    // Стекла аквариума (рамка)
    glDisable(GL_BLEND);
    glColor3f(0.3f, 0.3f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-9.0f, -8.0f);
    glVertex2f(9.0f, -8.0f);
    glVertex2f(9.0f, 8.0f);
    glVertex2f(-9.0f, 8.0f);
    glEnd();
    glLineWidth(1.0f);

    // Информация в заголовке окна
    int active_bubbles = 0;
    for (int i = 0; i < MAX_BUBBLES; i++) {
        if (bubbles[i].is_alive) active_bubbles++;
    }

    char title[100];
    sprintf(title, "Aquarium - Fish: %d, Bubbles: %d", MAX_FISH, active_bubbles);
    glutSetWindowTitle(title);

    glutSwapBuffers();
}

// --- Обработка клавиатуры ---
void keyboard(unsigned char key, int x, int y) {
    // Оставляем только выход по клавише ESC
    if (key == 27) { // 27 - это код клавиши ESC
        exit(0);
    }
}

// --- Обработка изменения размера окна ---
void reshape(int w, int h) {
    window_width = w;
    window_height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float aspect = (float)w / (float)h;
    if (w >= h) {
        glOrtho(-10.0 * aspect, 10.0 * aspect, -10.0, 10.0, -1.0, 1.0);
    }
    else {
        glOrtho(-10.0, 10.0, -10.0 / aspect, 10.0 / aspect, -1.0, 1.0);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

// --- Главная функция ---
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Aquarium with Fish and Bubbles");

    init();

    printf("=== Aquarium Simulation ===\n");
    printf("Controls:\n");
    printf("  ESC   - Exit\n");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}
