import os
import subprocess
import sys
import tkinter as tk
from tkinter import messagebox, scrolledtext

class CProgramLauncher:
    def __init__(self, root):
        self.root = root
        self.root.title("Запуск всех программ из FSSD")
        self.root.geometry("700x500")

        # Определяем папку, где лежит этот скрипт
        self.base_dir = os.path.dirname(os.path.abspath(__file__))
        self.build_dir = os.path.join(self.base_dir, "build")

        # Создаем папку для скомпилированных файлов, если её нет
        if not os.path.exists(self.build_dir):
            os.makedirs(self.build_dir)

        # Автоматически находим все .cpp и .c файлы в папке
        self.cpp_files = []
        for file in os.listdir(self.base_dir):
            if file.endswith(('.cpp', '.c')) and os.path.isfile(os.path.join(self.base_dir, file)):
                self.cpp_files.append(file)
        self.cpp_files.sort()
        
        # Удаляем ненужные файлы из списка
        files_to_remove = ['script2.cpp', 'test1.cpp']  # Удаляем script2.cpp и test1.cpp
        self.cpp_files = [f for f in self.cpp_files if f not in files_to_remove]
        
        # Если после удаления файлов не осталось - показываем сообщение
        if not self.cpp_files:
            self.log("⚠️ Не найдено C/C++ файлов для компиляции", "warning")

        # --- Интерфейс ---
        # Заголовок
        tk.Label(root, text="Запуск всех C++ программ", font=("Arial", 14, "bold")).pack(pady=10)
        
        # Информация о файлах
        info_frame = tk.Frame(root)
        info_frame.pack(pady=5)
        
        tk.Label(info_frame, text="Будут запущены:", font=("Arial", 10)).pack(side=tk.LEFT)
        
        files_text = ", ".join(self.cpp_files) if self.cpp_files else "Нет файлов"
        tk.Label(info_frame, text=files_text, font=("Arial", 10, "bold"), fg="blue").pack(side=tk.LEFT, padx=5)
        
        # Кнопки
        button_frame = tk.Frame(root)
        button_frame.pack(pady=10)
        
        self.run_all_button = tk.Button(button_frame, text="▶ Запустить ВСЕ программы", 
                                       command=self.run_all, bg="#4CAF50", fg="white", 
                                       font=("Arial", 11), padx=20, pady=8)
        self.run_all_button.pack(side=tk.LEFT, padx=5)
        
        self.run_selected_button = tk.Button(button_frame, text="🎯 Запустить выбранную", 
                                            command=self.run_selected, bg="#2196F3", fg="white", 
                                            font=("Arial", 10), padx=15, pady=5)
        self.run_selected_button.pack(side=tk.LEFT, padx=5)
        
        self.clear_button = tk.Button(button_frame, text="🗑 Очистить лог", 
                                     command=self.clear_log, bg="#FF9800", fg="white",
                                     padx=15, pady=5)
        self.clear_button.pack(side=tk.LEFT, padx=5)
        
        # Список файлов (для выборочного запуска)
        tk.Label(root, text="Или выберите конкретную программу:", font=("Arial", 9)).pack()
        
        self.listbox = tk.Listbox(root, selectmode=tk.SINGLE, height=4)
        self.listbox.pack(padx=10, fill=tk.BOTH, expand=True)
        
        for file in self.cpp_files:
            self.listbox.insert(tk.END, file)
        
        # Область для вывода логов
        tk.Label(root, text="📋 Лог компиляции и выполнения:", font=("Arial", 9)).pack()
        self.log_area = scrolledtext.ScrolledText(root, height=12, state='disabled', bg="#1e1e1e", fg="#d4d4d4")
        self.log_area.pack(padx=10, pady=5, fill=tk.BOTH, expand=True)
        
        # Статус бар
        self.status_label = tk.Label(root, text="Готов к запуску", bd=1, relief=tk.SUNKEN, anchor=tk.W)
        self.status_label.pack(side=tk.BOTTOM, fill=tk.X)

    def log(self, message, message_type="info"):
        """Добавляет сообщение в лог-область с цветом"""
        self.log_area.config(state='normal')
        
        # Цвета для разных типов сообщений
        colors = {
            "info": "#d4d4d4",      # белый
            "success": "#4EC9B0",   # бирюзовый
            "error": "#F44747",     # красный
            "warning": "#DCDCAA",   # желтый
            "compile": "#CE9178"    # оранжевый
        }
        
        self.log_area.insert(tk.END, message + "\n", message_type)
        self.log_area.tag_config(message_type, foreground=colors.get(message_type, "#d4d4d4"))
        
        self.log_area.see(tk.END)
        self.log_area.config(state='disabled')
        self.root.update()

    def clear_log(self):
        """Очищает лог-область"""
        self.log_area.config(state='normal')
        self.log_area.delete(1.0, tk.END)
        self.log_area.config(state='disabled')
        self.log("Лог очищен", "warning")

    def update_status(self, message):
        """Обновляет статус бар"""
        self.status_label.config(text=message)
        self.root.update()

    def is_graphics_program(self, filepath):
        """Проверяет, использует ли программа OpenGL/GLUT"""
        try:
            with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
                graphics_keywords = ['GL/', 'GLUT', 'OpenGL', 'glut', 'glClear', 'glBegin']
                return any(keyword in content for keyword in graphics_keywords)
        except:
            return False

    def compile_and_run(self, filename, index=None, total=None):
        """Компилирует и запускает один cpp/c файл"""
        
        filepath = os.path.join(self.base_dir, filename)
        
        if not os.path.exists(filepath):
            self.log(f"❌ Файл {filename} не найден!", "error")
            return False
        
        # Имя исполняемого файла
        executable_name = os.path.splitext(filename)[0]
        if sys.platform == "win32":
            executable_name += ".exe"
        executable_path = os.path.join(self.build_dir, executable_name)
        
        progress = f"[{index}/{total}] " if index and total else ""
        self.log(f"\n{'='*60}", "info")
        self.log(f"{progress}Компиляция {filename}...", "compile")
        
        # ФОРМИРУЕМ КОМАНДУ КОМПИЛЯЦИИ
        is_graphics = self.is_graphics_program(filepath)
        
        if sys.platform == "darwin":  # macOS
            if is_graphics:
                # Правильная команда для macOS с GLUT
                compile_command = [
                    "clang++", filepath, "-o", executable_path,
                    "-framework", "OpenGL",
                    "-framework", "GLUT",
                    "-DGL_SILENCE_DEPRECATION"
                ]
                self.log("🔧 macOS: компиляция с OpenGL и GLUT", "compile")
            else:
                compile_command = ["clang++", filepath, "-o", executable_path]
                self.log("🔧 macOS: обычная компиляция", "compile")
        
        elif sys.platform == "win32":  # Windows
            if is_graphics:
                compile_command = ["g++", filepath, "-o", executable_path, "-lopengl32", "-lfreeglut"]
                self.log("🔧 Windows: компиляция с OpenGL", "compile")
            else:
                compile_command = ["g++", filepath, "-o", executable_path]
                self.log("🔧 Windows: обычная компиляция", "compile")
        
        else:  # Linux
            if is_graphics:
                compile_command = ["g++", filepath, "-o", executable_path, "-lGL", "-lglut"]
                self.log("🔧 Linux: компиляция с OpenGL", "compile")
            else:
                compile_command = ["g++", filepath, "-o", executable_path]
                self.log("🔧 Linux: обычная компиляция", "compile")
        
        try:
            self.log(f"🔨 Выполняется: {' '.join(compile_command)}", "compile")
            process = subprocess.run(compile_command, capture_output=True, text=True, timeout=30)
            
            if process.returncode != 0:
                self.log(f"❌ Ошибка компиляции {filename}:", "error")
                self.log(process.stderr, "error")
                return False
            
            self.log(f"✅ Компиляция {filename} успешна!", "success")
            
            # Запуск программы
            self.log(f"▶ Запуск {executable_name}...", "info")
            
            if sys.platform == "win32":
                subprocess.Popen([executable_path], cwd=self.build_dir, shell=True)
            else:
                subprocess.Popen([executable_path], cwd=self.build_dir)
            
            self.log(f"✓ Программа {executable_name} запущена", "success")
            return True
            
        except FileNotFoundError:
            self.log("❌ Ошибка: Компилятор не найден!", "error")
            if sys.platform == "darwin":
                self.log("Установите Xcode Command Line Tools: xcode-select --install", "warning")
            return False
        except subprocess.TimeoutExpired:
            self.log(f"❌ Ошибка: Компиляция {filename} заняла слишком много времени", "error")
            return False
        except Exception as e:
            self.log(f"❌ Непредвиденная ошибка: {e}", "error")
            return False

    def run_all(self):
        """Запускает ВСЕ cpp файлы по порядку"""
        
        if not self.cpp_files:
            self.log("❌ Нет файлов .cpp или .c для запуска!", "error")
            messagebox.showerror("Ошибка", "Нет C++ файлов в папке!")
            return
        
        self.log("\n" + "="*60, "info")
        self.log("🚀 ЗАПУСК ВСЕХ ПРОГРАММ", "success")
        self.log("="*60, "info")
        
        total = len(self.cpp_files)
        success_count = 0
        
        for i, filename in enumerate(self.cpp_files, 1):
            self.update_status(f"Запуск {filename}...")
            if self.compile_and_run(filename, i, total):
                success_count += 1
        
        self.log(f"\n{'='*60}", "info")
        self.log(f"📊 Результат: {success_count} из {total} программ успешно запущено", 
                "success" if success_count == total else "warning")
        self.log("="*60, "info")
        
        self.update_status(f"Готово. Успешно: {success_count}/{total}")
        
        if success_count == total:
            messagebox.showinfo("Готово", f"Все {total} программ успешно запущены!")
        else:
            messagebox.showwarning("Внимание", f"Запущено {success_count} из {total} программ. Проверьте лог.")

    def run_selected(self):
        """Запускает выбранную программу"""
        selection = self.listbox.curselection()
        if not selection:
            messagebox.showwarning("Предупреждение", "Пожалуйста, выберите файл из списка.")
            return
        
        filename = self.listbox.get(selection[0])
        self.update_status(f"Запуск {filename}...")
        
        if self.compile_and_run(filename):
            self.update_status(f"Готово. {filename} запущен")
            messagebox.showinfo("Готово", f"Программа {filename} успешно запущена!")
        else:
            self.update_status(f"Ошибка при запуске {filename}")
            messagebox.showerror("Ошибка", f"Не удалось запустить {filename}.\nПроверьте лог.")

if __name__ == "__main__":
    root = tk.Tk()
    app = CProgramLauncher(root)
    root.mainloop()