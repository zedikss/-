#include <GLUT/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// --- Константы ---
#define MAX_BALLS 20           // Максимальное количество основных шаров
#define MAX_PARTICLES 200      // Максимальное количество частиц после взрыва
#define GRAVITY 0.001f         // Сила гравитации
#define PI 3.14159265f

// --- Структуры ---
typedef struct {
    float x, y;           // Позиция
    float vx, vy;         // Скорость
    float r, g, b;        // Цвет
    float size;           // Размер
    float growth_rate;    // Скорость роста
    int is_alive;         // 1 = живой, 0 = мертвый
    int timer;            // Таймер до взрыва
} Ball;

typedef struct {
    float x, y;           // Позиция
    float vx, vy;         // Скорость
    float r, g, b;        // Цвет
    float size;           // Размер
    int is_alive;         // 1 = активна, 0 = неактивна
    float lifetime;       // Текущее время жизни
    float max_lifetime;   // Максимальное время жизни
} Particle;

// --- Глобальные переменные ---
Ball balls[MAX_BALLS];
Particle particles[MAX_PARTICLES];
int window_width = 800;
int window_height = 600;
int frames = 0;
int spawn_counter = 0;
int spawn_delay = 60;      // Задержка между появлением новых шаров

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

// --- Инициализация ---
void init() {
    srand((unsigned int)time(NULL));

    // Инициализация шаров
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].is_alive = 0;
    }

    // Инициализация частиц
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].is_alive = 0;
    }
}

// --- Создание нового шара ---
void create_ball() {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].is_alive) {
            balls[i].x = random_float(-5.0f, 5.0f);
            balls[i].y = -9.0f;  // Появляется внизу экрана
            balls[i].vx = random_float(-0.02f, 0.02f);
            balls[i].vy = random_float(0.15f, 0.25f);  // Вылетает вверх
            balls[i].r = random_float(0.7f, 1.0f);
            balls[i].g = random_float(0.1f, 0.9f);
            balls[i].b = random_float(0.1f, 0.9f);
            balls[i].size = random_float(0.1f, 0.2f);
            balls[i].growth_rate = random_float(0.002f, 0.005f);
            balls[i].is_alive = 1;
            balls[i].timer = (int)random_float(80.0f, 150.0f);  // Время до взрыва
            return;
        }
    }
}

// --- Создание частиц взрыва ---
void create_explosion(float x, float y, float r, float g, float b) {
    int particles_count = (int)random_float(15.0f, 30.0f);

    for (int p = 0; p < particles_count; p++) {
        for (int i = 0; i < MAX_PARTICLES; i++) {
            if (!particles[i].is_alive) {
                float angle = random_float(0.0f, 2.0f * PI);
                float speed = random_float(0.03f, 0.12f);

                particles[i].x = x;
                particles[i].y = y;
                particles[i].vx = cosf(angle) * speed;
                particles[i].vy = sinf(angle) * speed;
                particles[i].r = r * random_float(0.8f, 1.0f);
                particles[i].g = g * random_float(0.8f, 1.0f);
                particles[i].b = b * random_float(0.8f, 1.0f);
                particles[i].size = random_float(0.04f, 0.08f);
                particles[i].is_alive = 1;
                particles[i].lifetime = 0.0f;
                particles[i].max_lifetime = random_float(40.0f, 80.0f);
                break;
            }
        }
    }
}

// --- Обновление состояния ---
void update(int value) {
    frames++;
    spawn_counter++;

    // Создание нового шара с задержкой
    if (spawn_counter >= spawn_delay) {
        create_ball();
        spawn_counter = 0;
        spawn_delay = (int)random_float(40.0f, 100.0f);  // Случайная задержка
    }

    // Обновление шаров
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].is_alive) {
            // Движение
            balls[i].x += balls[i].vx;
            balls[i].y += balls[i].vy;
            balls[i].vy -= GRAVITY;  // Гравитация

            // Увеличение размера
            balls[i].size += balls[i].growth_rate;

            // Таймер до взрыва
            balls[i].timer--;

            // Взрыв по таймеру или при замедлении
            if (balls[i].timer <= 0 || balls[i].vy < -0.05f) {
                create_explosion(balls[i].x, balls[i].y,
                    balls[i].r, balls[i].g, balls[i].b);
                balls[i].is_alive = 0;
            }
        }
    }

    // Обновление частиц
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].is_alive) {
            // Движение
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].vy -= GRAVITY * 0.3f;  // Слабая гравитация

            // Затухание скорости
            particles[i].vx *= 0.97f;
            particles[i].vy *= 0.97f;

            // Увеличение времени жизни
            particles[i].lifetime += 1.0f;

            // Уменьшение размера со временем
            particles[i].size *= 0.98f;

            // Исчезновение по истечении времени жизни
            if (particles[i].lifetime >= particles[i].max_lifetime ||
                particles[i].size < 0.01f) {
                particles[i].is_alive = 0;
            }
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// --- Рисование ---
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Фон (ночное небо)
    glBegin(GL_QUADS);
    glColor3f(0.0f, 0.0f, 0.1f);     // Темно-синий вверху
    glVertex2f(-10.0f, 10.0f);
    glVertex2f(10.0f, 10.0f);
    glColor3f(0.0f, 0.05f, 0.15f);   // Чуть светлее внизу
    glVertex2f(10.0f, -10.0f);
    glVertex2f(-10.0f, -10.0f);
    glEnd();

    // Земля
    glColor3f(0.15f, 0.1f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(-10.0f, -10.0f);
    glVertex2f(10.0f, -10.0f);
    glVertex2f(10.0f, -9.0f);
    glVertex2f(-10.0f, -9.0f);
    glEnd();

    // Рисование частиц (сначала для правильного смешивания)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].is_alive) {
            // Прозрачность в зависимости от времени жизни
            float alpha = 1.0f - (particles[i].lifetime / particles[i].max_lifetime);
            if (alpha < 0.1f) alpha = 0.1f;

            glColor4f(particles[i].r, particles[i].g, particles[i].b, alpha);
            draw_circle(particles[i].x, particles[i].y, particles[i].size, 16);
        }
    }

    // Рисование шаров
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].is_alive) {
            // Свечение вокруг шара
            glColor4f(balls[i].r, balls[i].g, balls[i].b, 0.3f);
            draw_circle(balls[i].x, balls[i].y, balls[i].size * 1.8f, 24);

            // Основной шар
            glColor3f(balls[i].r, balls[i].g, balls[i].b);
            draw_circle(balls[i].x, balls[i].y, balls[i].size, 20);

            // Хвост (траектория) - простой вариант
            glColor4f(balls[i].r, balls[i].g, balls[i].b, 0.3f);
            glBegin(GL_QUADS);
            float trail_length = 0.5f;
            glVertex2f(balls[i].x - balls[i].size * 0.3f, balls[i].y - trail_length);
            glVertex2f(balls[i].x + balls[i].size * 0.3f, balls[i].y - trail_length);
            glVertex2f(balls[i].x + balls[i].size * 0.3f, balls[i].y);
            glVertex2f(balls[i].x - balls[i].size * 0.3f, balls[i].y);
            glEnd();
        }
    }

    glDisable(GL_BLEND);

    // Информация на экране
    glColor3f(1.0f, 1.0f, 1.0f);

    // Подсчет активных объектов
    int active_balls = 0;
    int active_particles = 0;
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].is_alive) active_balls++;
    }
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].is_alive) active_particles++;
    }

    // Отображение информации в заголовке окна
    char title[100];
    sprintf(title, "Fireworks - Balls: %d, Particles: %d", active_balls, active_particles);
    glutSetWindowTitle(title);

    glutSwapBuffers();
}

// --- Обработка клавиатуры ---
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case ' ':  // Пробел - создать взрыв в случайном месте
        create_explosion(random_float(-6.0f, 6.0f),
            random_float(-2.0f, 6.0f),
            random_float(0.5f, 1.0f),
            random_float(0.5f, 1.0f),
            random_float(0.5f, 1.0f));
        break;
    case '+':  // Увеличить частоту появления
    case '=':
        spawn_delay = (spawn_delay > 20) ? spawn_delay - 10 : 20;
        break;
    case '-':  // Уменьшить частоту появления
        spawn_delay += 10;
        break;
    case 'c':  // Очистить экран
    case 'C':
        for (int i = 0; i < MAX_BALLS; i++) balls[i].is_alive = 0;
        for (int i = 0; i < MAX_PARTICLES; i++) particles[i].is_alive = 0;
        break;
    case 'r':  // Сброс
    case 'R':
        init();
        break;
    case 27:   // Esc - выход
        exit(0);
        break;
    }
}

// --- Обработка изменения размера окна ---
void reshape(int w, int h) {
    window_width = w;
    window_height = h;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Поддержание пропорций
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
    glutCreateWindow("Fireworks Show");

    init();

    printf("=== Fireworks Show ===\n");
    printf("Controls:\n");
    printf("  SPACE - Create manual explosion\n");
    printf("  +/=   - Increase firework frequency\n");
    printf("  -     - Decrease firework frequency\n");
    printf("  c     - Clear screen\n");
    printf("  r     - Reset\n");
    printf("  ESC   - Exit\n");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
    return 0;
}