// ==== Библиотеки ====

#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <algorithm>
#include <filesystem>
#include <vector>
#include <fstream>
#include <set>
#include <map>
#include <cctype>

// ==== Работа с консолью ====

class consoleHelper {
private:
	HANDLE hConsole;
public:
	consoleHelper() : hConsole(GetStdHandle(STD_OUTPUT_HANDLE)) {
		SetConsoleOutputCP(1251);
		SetConsoleCP(1251);
	}

	void setColor(int color) {
		SetConsoleTextAttribute(hConsole, color);
	}

	void resetColor() {
		SetConsoleTextAttribute(hConsole, 7);
	}

	void printColor(int color, const std::string& text) {
		setColor(color);
		std::cout << text;
		resetColor();
	}

	void clearConsole() {
		system("cls");
	}

	void pause() {
		printColor(12, "Нажмите ENTER чтобы продолжить...");
		std::cin.get();
	}
};



// ==== Функции файлового менеджера ====

class fileManager {
private:
	consoleHelper console;   // Переменная для работы с консолью
	std::string currentPath; // Текущий путь

	// ==== Функция подтверждения ====

	bool isAccept() {
		std::string choice;
		bool isValid = false;

		std::cout << "\nВы уверены, что хотите выполнить данную операцию?\n";
		std::cout << "Введите "; console.printColor(10, "Y (да), "); std::cout << "если хотите продолжить и ";
		console.printColor(4, "N (нет), "); std::cout << "если хотите отменить\n\n";

		console.printColor(2, "Введите действие: ");

		while (!isValid) {
			std::getline(std::cin, choice);

			if (choice == "Y" || choice == "y" || choice == "да" || choice == "lf" ||
				choice == "yes" || choice == "YES" || choice == "Yes" ||
				choice == "N" || choice == "n" || choice == "нет" ||
				choice == "no" || choice == "NO" || choice == "No" || choice == "ytn") {
				isValid = true;
			}
			else {
				console.printColor(4, "\nОшибка: введите либо Y (да), либо N (нет)\n");
				console.pause();
				console.clearConsole();
				printLogo();
				std::cout << "Вы уверены, что хотите выполнить данную операцию?\n";
				std::cout << "Введите "; console.printColor(10, "Y (да), "); std::cout << "если хотите продолжить и ";
				console.printColor(4, "N (нет), "); std::cout << "если хотите отменить\n\n";
				console.printColor(2, "Введите действие: ");
			}
		}


		if (choice == "Y" || choice == "y" || choice == "да" ||
			choice == "lf" || choice == "yes" || choice == "YES" || choice == "Yes") {
			return true;
		}
		else {
			return false;
		}
	}

	// ==== Натуральная сортировка ====

	bool natural_less(const std::string& a, const std::string& b) {
		size_t i = 0, j = 0;
		while (i < a.size() && j < b.size()) {
			if (isdigit(a[i]) && isdigit(b[j])) {
				size_t start1 = i, start2 = j;
				while (i < a.size() && isdigit(a[i])) ++i;
				while (j < b.size() && isdigit(b[j])) ++j;
				std::string n1 = a.substr(start1, i - start1);
				std::string n2 = b.substr(start2, j - start2);
				if (n1.size() != n2.size()) return n1.size() < n2.size();
				if (n1 != n2) return n1 < n2;
			}
			else {
				if (a[i] != b[j]) return a[i] < b[j];
				++i; ++j;
			}
		}

		return i == a.size() && j < b.size() ? false : i < a.size();
	}


	// ==== Возможные ошибки при вводе пути ====

	bool isValidPath(const std::string& currentPath) {
		if (!std::filesystem::exists(currentPath)) {
			console.printColor(4, "\nОшибка: путь указан неверно\n");
			return false;
		}

		if (!std::filesystem::is_directory(currentPath)) {
			console.printColor(4, "\nОшибка: путь ведет не к папке\n");
			return false;
		}

		return true;
	}

	// ==== Название программы ====

	void printLogo() {
		console.printColor(10, "============== COCONUT ==============\n\n");
	}

	// ==== Начало программы ( ввод пути ) ====

	void setupWorkingDirectory() {
		printLogo();
		console.printColor(15, "Добро пожаловать в файловый менеджер - Coconut\n");
		console.printColor(15, "Перед началом работы вам необходимо ввести путь до папки\n");
		console.printColor(15, "Либо напишите 1, чтобы работать в папке, где была запущена программа\n\n");

		console.printColor(2, "Введите путь до папки: ");

		std::getline(std::cin, currentPath);

		// ==== Обработка случая, когда программа запущена из текущей рабочего каталога ====

		if (currentPath == "1") {
			currentPath = std::filesystem::current_path().string();
		}

		// ==== Исправление возможных ошибок ====

		std::replace(currentPath.begin(), currentPath.end(), '/', '\\');
		if (!currentPath.empty() && currentPath.front() == '"' && currentPath.back() == '"') {
			currentPath = currentPath.substr(1, currentPath.length() - 2);
		}

		while (!isValidPath(currentPath)) {
			console.pause();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Введите путь до папки: ");
			std::getline(std::cin, currentPath);
			if (currentPath == "1") {
				currentPath = std::filesystem::current_path().string();
			}
		}
	}

	// ==== Меню ====

	void printMenu() {
		console.printColor(2, "Меню:\n\n");
		console.printColor(10, "1. "); console.printColor(7, "Изменить путь\n");
		console.printColor(10, "2. "); console.printColor(7, "Посмотреть список файлов\n");
		console.printColor(10, "3. "); console.printColor(7, "Переименовать файлы\n");
		console.printColor(10, "4. "); console.printColor(7, "Переименовать папки\n");
		console.printColor(10, "5. "); console.printColor(7, "Создать папки\n");
		console.printColor(10, "6. "); console.printColor(7, "Создать файлы\n");
		console.printColor(10, "7. "); console.printColor(7, "Удалить папки\n");
		console.printColor(10, "8. "); console.printColor(7, "Удалить файлы\n");
		console.printColor(12, "0. Выход\n");
		console.printColor(2, "\nВыберите действие: ");
	}

	// ==== 1. Изменение текущего пути ====
	void editCurrentPath() {
		console.clearConsole();
		printLogo();

		console.printColor(2, "Введите новый путь до папки: ");
		std::getline(std::cin, currentPath);
	}


	// ==== 2. Функция просмотра содержимого папки ====

	void show_folderContent() {
		console.clearConsole();
		printLogo();

		console.printColor(2, "Текущая папка: ");
		console.printColor(15, currentPath);
		console.printColor(14, "\n\n");

		try {
			// Перебираем файлы
			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
				entries.push_back(entry);
			}

			// Используем сортировку
			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			// Показываем содержимое
			if (!entries.empty()) {
				int count = 0;
				for (const auto& entry : entries) {
					if (entry.is_directory()) {
						console.printColor(2, "[ Папка ] ");
					}
					else {
						console.printColor(10, "[ Файл ] ");
					}

					std::cout << entry.path().filename().string() + '\n';
					count++;
				}

				console.printColor(10, "\nВсего элементов: ");
				std::cout << count << std::endl;
			}
			else {
				console.printColor(4, "Папка пуста!\n\n");
			}
		}

		catch (const std::exception& ex) {
			console.printColor(4, "Ошибка доступа: " + std::string(ex.what()) + "\n");
		}

		console.pause();
	}

	// ==== 3 и 4. Функция переименовывания файлов и папок ====
	std::vector<std::pair<std::filesystem::path, std::filesystem::path>> lastRenameHistory;

	void renameFiles(std::string object) {
		std::string fileCount;
		bool isValid = false;

		std::string renameFunction_choice;
		isValid = false;

		console.clearConsole();
		printLogo();

		while (!isValid) {
			console.printColor(10, "1. "); console.printColor(7, "Добавить преффикс\n");
			console.printColor(10, "2. "); console.printColor(7, "Изменить после символа\n");
			console.printColor(10, "3. "); console.printColor(7, "Изменить до символа\n");
			console.printColor(10, "4. "); console.printColor(7, "Полностью изменить название\n");
			console.printColor(10, "5. "); console.printColor(7, "Изменить, взяв названия из файла\n");
			console.printColor(10, "6. "); console.printColor(7, "Исправить раскладку (когда набрано EN вместо RU и наоборот)\n");
			console.printColor(4, "0. Отменить изменения ( последнее )\n\n");
			console.printColor(10, "Выберите действие: ");
			std::getline(std::cin, renameFunction_choice);
			if (renameFunction_choice == "1" || renameFunction_choice == "2" ||
				renameFunction_choice == "2" || renameFunction_choice == "3" ||
				renameFunction_choice == "4" || renameFunction_choice == "5" ||
				renameFunction_choice == "0") isValid = true;
			else {
				console.printColor(4, "\nОшибка: Такого пункта меню нет\n");
				console.pause();
				console.clearConsole();
				printLogo();
			}
		}

		if (renameFunction_choice == "1") {
			lastRenameHistory.clear();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Изменение названия, добавляя префикс\n\n");

			std::string preffix;
			int renamedCount = 0;

			console.printColor(10, "Введите префикс: ");
			std::getline(std::cin, preffix);

			// Перебираем файлы
			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
				entries.push_back(entry);
			}

			// Используем сортировку
			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			if (!isAccept()) {
				console.pause();
				return;
			}

			for (const auto& entry : entries) {
				if (object == "files") {
					if (entry.is_directory()) continue;
				}
				else {
					if (!entry.is_directory()) continue;
				}

				std::string oldName = entry.path().filename().string();
				std::string newName = preffix + oldName;

				std::filesystem::path oldPath = entry.path();
				std::filesystem::path newPath = oldPath.parent_path() / newName;

				try {
					lastRenameHistory.push_back({ oldPath, newPath });
					std::filesystem::rename(oldPath, newPath);
					renamedCount++;
					console.printColor(5, "\nПереименован: "); std::cout << oldName; console.printColor(5, " -> "); std::cout << newName;
				}
				catch (const std::filesystem::filesystem_error& e) {
					console.printColor(12, "Ошибка при переименовании " + oldName + ": " + e.what() + '\n');
				}
			}
			console.printColor(10, "\n\nПереименовано файлов: " + std::to_string(renamedCount) + "\n");
			console.pause();
		}

		else if (renameFunction_choice == "2") {
			lastRenameHistory.clear();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Изменение названия после символа\n\n");

			std::string symbol;
			std::string newEnd;
			int number = 1;
			int renamedCount = 0;
			int count = 0;
			std::string useNumbering;

			std::vector <std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			console.printColor(10, "Введите символ: ");
			std::getline(std::cin, symbol);

			console.printColor(10, "Введите текст после символа ( нажмите ENTER если не хотите ничего добавлять ): ");
			std::getline(std::cin, newEnd);

			console.printColor(10, "Добавить авто нумерацию? (1 - да, 0 - нет): ");
			std::getline(std::cin, useNumbering);

			if (!isAccept()) {
				console.pause();
				return;
			}

			for (const auto& entry : entries) {
				if (object == "files") {
					if (entry.is_directory()) continue;
				}
				else {
					if (!entry.is_directory()) continue;
				}

				std::string oldName = entry.path().filename().string();

				std::filesystem::path oldPath = entry.path();
				std::filesystem::path parentPath = oldPath.parent_path();
				std::string stem = oldPath.stem().string();
				std::string ext = oldPath.extension().string();

				std::string newBase;

				size_t pos = 0;
				int count = 0;
				while ((pos = stem.find(symbol, pos)) != std::string::npos) {
					++count;
					pos += symbol.length();
				}

				if (count > 0) {
					size_t lastPos = stem.rfind(symbol);
					newBase = stem.substr(0, lastPos + symbol.length());
				}
				else
					newBase = stem + symbol;

				std::string newName;
				if (useNumbering == "1") {
					newName = newBase + newEnd + std::to_string(number) + ext;
				}
				else {
					newName = newBase + newEnd + ext;
				}

				std::filesystem::path newPath = parentPath / newName;

				try {
					lastRenameHistory.push_back({ oldPath, newPath });
					std::filesystem::rename(oldPath, newPath);
					renamedCount++;

					console.printColor(5, "\nПереименован: "); std::cout << oldName; console.printColor(5, " -> "); std::cout << newName;
				}
				catch (const std::filesystem::filesystem_error& e) {
					console.printColor(12, "\nОшибка: " + oldName + " -> " + e.what());
				}
				if (useNumbering == "1") {
					number++;
				}
			}

			console.printColor(10, "\n\nГотово! Переименовано файлов: " + std::to_string(renamedCount) + "\n");
			console.pause();
		}

		else if (renameFunction_choice == "3") {
			lastRenameHistory.clear();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Изменение названия до символа\n\n");

			std::string symbol;
			std::string newStart;
			std::string useNumbering;
			int number = 1;
			int renamedCount = 0;
			int count = 0;

			std::vector <std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			console.printColor(10, "Введите символ: ");
			std::getline(std::cin, symbol);

			console.printColor(10, "Введите текст после символа ( нажмите ENTER если не хотите ничего добавлять ): ");
			std::getline(std::cin, newStart);

			console.printColor(10, "Добавить авто нумерацию? (1 - да, 0 - нет): ");
			std::getline(std::cin, useNumbering);

			if (!isAccept()) {
				console.pause();
				return;
			}

			for (const auto& entry : entries) {
				if (object == "files") {
					if (entry.is_directory()) continue;
				}
				else {
					if (!entry.is_directory()) continue;
				}

				std::string oldName = entry.path().filename().string();

				std::filesystem::path oldPath = entry.path();
				std::filesystem::path parentPath = oldPath.parent_path();
				std::string stem = oldPath.stem().string();
				std::string ext = oldPath.extension().string();

				std::string newBase;

				size_t pos = stem.find(symbol);
				if (pos != std::string::npos) {
					newBase = stem.substr(pos + symbol.length());
				}
				else {
					newBase = stem;
				}

				std::string newName;
				if (useNumbering == "1") {
					newName = newStart + newBase + std::to_string(number) + ext;
				}
				else {
					newName = newStart + newBase + ext;
				}

				std::filesystem::path newPath = parentPath / newName;

				try {
					lastRenameHistory.push_back({ oldPath, newPath });
					std::filesystem::rename(oldPath, newPath);
					renamedCount++;

					console.printColor(5, "\nПереименован: "); std::cout << oldName; console.printColor(5, " -> "); std::cout << newName;
				}
				catch (const std::filesystem::filesystem_error& e) {
					console.printColor(12, "\nОшибка: " + oldName + " -> " + e.what());
				}
				if (useNumbering == "1") {
					number++;
				}
			}

			console.printColor(10, "\n\nГотово! Переименовано файлов: " + std::to_string(renamedCount) + "\n");
			console.pause();
		}

		else if (renameFunction_choice == "4") {
			lastRenameHistory.clear();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Изменение названия полностью\n\n");

			std::string newName;
			int renamedCount = 0;
			int number = 1;

			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			console.printColor(10, "Введите новое название: ");
			std::getline(std::cin, newName);

			if (!isAccept()) {
				console.pause();
				return;
			}

			for (const auto& entry : entries) {
				if (object == "files") {
					if (entry.is_directory()) continue;
				}
				else {
					if (!entry.is_directory()) continue;
				}

				std::string oldName = entry.path().filename().string();

				std::filesystem::path oldPath = entry.path();
				std::filesystem::path parentToNumber = oldPath.parent_path();

				std::string ext = oldPath.extension().string();

				std::filesystem::path numbaredName = newName + std::to_string(number) + ext;

				std::filesystem::path newPath = parentToNumber / numbaredName;

				try {
					lastRenameHistory.push_back({ oldPath, newPath });
					std::filesystem::rename(oldPath, newPath);
					renamedCount++;
					console.printColor(5, "\nПереименован: "); std::cout << oldName; console.printColor(5, " -> "); std::cout << numbaredName;
				}
				catch (const std::filesystem::filesystem_error& e) {
					console.printColor(12, "Ошибка при переименовании " + oldName + ": " + e.what() + '\n');
				}

				number++;
			}

			console.printColor(10, "\n\nПереименовано файлов: " + std::to_string(renamedCount) + "\n");
			console.pause();
		}
		else if (renameFunction_choice == "5") {
			lastRenameHistory.clear();

			std::string rename_path;
			isValid = false;

			while (!isValid) {
				console.clearConsole();
				printLogo();
				console.printColor(2, "Взятие названий из файла\n\n");
				console.printColor(10, "1. "); console.printColor(15, "Выберите файл с названиями\n");
				console.printColor(10, "2. "); console.printColor(15, "Перетащите в консоль\n");
				console.printColor(10, "3. "); console.printColor(15, "Нажмите ENTER\n\n");

				console.printColor(9, "Перетащите файл сюда: ");
				std::getline(std::cin, rename_path);

				std::replace(rename_path.begin(), rename_path.end(), '/', '\\');
				if (!rename_path.empty() && rename_path.front() == '"' && rename_path.back() == '"') {
					rename_path = rename_path.substr(1, rename_path.length() - 2);
				}

				if (!std::filesystem::exists(rename_path)) {
					console.printColor(4, "\nОшибка: файл не найден!\n");
					console.pause();
					continue;
				}

				if (std::filesystem::is_directory(rename_path)) {
					console.printColor(4, "\nОшибка: была передана папка, а не файл!\n");
					console.pause();
					continue;
				}

				std::string extension = std::filesystem::path(rename_path).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				if (extension != ".txt") {
					console.printColor(14, "\nПредупреждение: рекомендуется использовать .txt файлы\n");
					if (!isAccept()) {
						console.pause();
						continue;
					}
				}

				isValid = true;
			}

			std::vector<std::string> newNames;
			try {
				// Попробуем несколько подходов к чтению файла

				// Сначала попробуем прочитать как UTF-8
				std::ifstream file(rename_path, std::ios::binary);
				if (!file.is_open()) {
					console.printColor(4, "\nОшибка: не удалось открыть файл!\n");
					console.pause();
					return;
				}

				// Читаем весь файл
				std::string fileContent((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				file.close();

				// Функция для преобразования из UTF-8 в ANSI (Windows-1251)
				auto utf8ToAnsi = [](const std::string& utf8) -> std::string {
					if (utf8.empty()) return "";

					// Попробуем определить, есть ли BOM UTF-8
					std::string content = utf8;
					if (utf8.size() >= 3 &&
						(unsigned char)utf8[0] == 0xEF &&
						(unsigned char)utf8[1] == 0xBB &&
						(unsigned char)utf8[2] == 0xBF) {
						content = utf8.substr(3); // Пропускаем BOM
					}

					// Сначала попробуем преобразовать из UTF-8 в широкую строку
					int wideSize = MultiByteToWideChar(CP_UTF8, 0, content.c_str(),
						(int)content.length(), NULL, 0);
					if (wideSize == 0) {
						// Если не UTF-8, вернем как есть (предполагаем ANSI)
						return content;
					}

					std::wstring wideStr(wideSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.length(),
						&wideStr[0], wideSize);

					// Затем преобразуем из широкой строки в ANSI (CP_1251)
					int ansiSize = WideCharToMultiByte(1251, 0, wideStr.c_str(),
						(int)wideStr.length(), NULL, 0, NULL, NULL);
					if (ansiSize == 0) {
						// Если не удалось, попробуем CP_ACP (системная кодировка)
						ansiSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(),
							(int)wideStr.length(), NULL, 0, NULL, NULL);
						if (ansiSize == 0) return content;
					}

					std::string ansiStr(ansiSize, 0);
					WideCharToMultiByte(1251, 0, wideStr.c_str(), (int)wideStr.length(),
						&ansiStr[0], ansiSize, NULL, NULL);
					return ansiStr;
					};

				// Разделяем содержимое файла на строки
				std::istringstream stream(fileContent);
				std::string line;
				int lineCount = 0;

				while (std::getline(stream, line)) {
					// Убираем символы возврата каретки (для Windows формата \r\n)
					if (!line.empty() && line.back() == '\r') {
						line.pop_back();
					}

					if (line.empty()) continue;

					// Преобразуем кодировку
					std::string convertedLine = utf8ToAnsi(line);

					// Удаляем начальные и конечные пробелы
					convertedLine.erase(0, convertedLine.find_first_not_of(" \t\r\n\v\f"));
					convertedLine.erase(convertedLine.find_last_not_of(" \t\r\n\v\f") + 1);

					if (!convertedLine.empty()) {
						newNames.push_back(convertedLine);
						lineCount++;
					}
				}

				console.printColor(10, "\nПрочитано названий из файла: " + std::to_string(lineCount) + "\n");

				std::vector<std::filesystem::directory_entry> entries;
				for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
					entries.push_back(entry);
				}

				std::sort(entries.begin(), entries.end(),
					[this](const auto& a, const auto& b) {
						if (a.is_directory() && !b.is_directory()) return true;
						if (!a.is_directory() && b.is_directory()) return false;
						return natural_less(a.path().filename().string(),
							b.path().filename().string());
					});

				std::vector<std::filesystem::path> itemsToRename;
				for (const auto& entry : entries) {
					if (object == "files") {
						if (entry.is_directory()) continue;
					}
					else {
						if (!entry.is_directory()) continue;
					}
					itemsToRename.push_back(entry.path());
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				int renamedCount = 0;
				int totalToRename = (newNames.size() < itemsToRename.size() ? newNames.size() : itemsToRename.size());

				for (size_t i = 0; i < totalToRename; ++i) {
					const auto& oldPath = itemsToRename[i];
					const std::string& newName = newNames[i];

					// Сохраняем расширение для файлов
					std::string finalNewName = newName;
					if (object == "files") {
						std::string extension = oldPath.extension().string();
						// Добавляем расширение, если его нет в новом имени
						if (finalNewName.find('.') == std::string::npos && !extension.empty()) {
							finalNewName += extension;
						}
					}

					std::filesystem::path newPath = oldPath.parent_path() / finalNewName;

					try {
						// Проверяем, не существует ли уже файл/папка с таким именем
						if (std::filesystem::exists(newPath)) {
							console.printColor(14, "Пропущено: '" + oldPath.filename().string() +
								"' -> '" + finalNewName + "' (уже существует)\n");
							continue;
						}
						lastRenameHistory.push_back({ oldPath, newPath });
						std::filesystem::rename(oldPath, newPath);
						renamedCount++;

						console.printColor(5, "Переименован: ");
						std::cout << oldPath.filename().string() << " -> " << finalNewName << "\n";
					}
					catch (const std::filesystem::filesystem_error& e) {
						console.printColor(12, "Ошибка: " + oldPath.filename().string() + " -> " + e.what() + "\n");
					}
				}

				console.printColor(10, "\nГотово! Успешно переименовано: " + std::to_string(renamedCount) + " из " +
					std::to_string(itemsToRename.size()) + "\n");

				if (newNames.size() > itemsToRename.size()) {
					console.printColor(14, "Осталось неиспользованных названий: " +
						std::to_string(newNames.size() - itemsToRename.size()) + "\n");
				}
			}
			catch (const std::exception& e) {
				console.printColor(4, "Ошибка при чтении файла: " + std::string(e.what()) + "\n");
			}

			console.pause();
		}

		else if (renameFunction_choice == "6") {
			lastRenameHistory.clear();
			console.clearConsole();
			printLogo();
			console.printColor(2, "Исправление раскладки (EN вместо RU и наоборот)\n\n");

			const std::map<char, char> wrong_to_correct = {
				{'q', 'й'}, {'w', 'ц'}, {'e', 'у'}, {'r', 'к'}, {'t', 'е'},
				{'y', 'н'}, {'u', 'г'}, {'i', 'ш'}, {'o', 'щ'}, {'p', 'з'},
				{'a', 'ф'}, {'s', 'ы'}, {'d', 'в'}, {'f', 'а'}, {'g', 'п'},
				{'h', 'р'}, {'j', 'о'}, {'k', 'л'}, {'l', 'д'}, {'z', 'я'},
				{'x', 'ч'}, {'c', 'с'}, {'v', 'м'}, {'b', 'и'}, {'n', 'т'}, {'m', 'ь'},
				{'Q', 'Й'}, {'W', 'Ц'}, {'E', 'У'}, {'R', 'К'}, {'T', 'Е'},
				{'Y', 'Н'}, {'U', 'Г'}, {'I', 'Ш'}, {'O', 'Щ'}, {'P', 'З'},
				{'A', 'Ф'}, {'S', 'Ы'}, {'D', 'В'}, {'F', 'А'}, {'G', 'П'},
				{'H', 'Р'}, {'J', 'О'}, {'K', 'Л'}, {'L', 'Д'}, {'Z', 'Я'},
				{'X', 'Ч'}, {'C', 'С'}, {'V', 'М'}, {'B', 'И'}, {'N', 'Т'}, {'M', 'Ь'}
			};

			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			if (!isAccept()) {
				console.pause();
				return;
			}

			int renamedCount = 0;

			for (const auto& entry : entries) {
				if (object == "files") {
					if (entry.is_directory()) continue;
				}
				else {
					if (!entry.is_directory()) continue;
				}

				std::filesystem::path oldPath = entry.path();
				std::string oldName = oldPath.filename().string();
				std::string stem = oldPath.stem().string();
				std::string ext = oldPath.extension().string();

				std::string newStem = stem;
				bool changed = false;

				for (char& c : newStem) {
					auto it = wrong_to_correct.find(c);
					if (it != wrong_to_correct.end()) {
						c = it->second;
						changed = true;
					}
				}

				if (changed) {
					std::string newName = newStem + ext;
					std::filesystem::path newPath = oldPath.parent_path() / std::filesystem::path(newName);

					try {
						lastRenameHistory.push_back({ oldPath, newPath });
						std::filesystem::rename(oldPath, newPath);
						renamedCount++;
						console.printColor(5, "\nИсправлено: ");
						std::cout << oldName << " -> " << newName;
					}
					catch (const std::exception& e) {
						console.printColor(12, "\nОшибка: " + oldName + " — " + e.what());
					}
				}
			}

			console.printColor(10, "\n\nГотово! Исправлено файлов: " + std::to_string(renamedCount) + "\n");
			console.pause();
		}

		else if (renameFunction_choice == "0") {
			if (lastRenameHistory.empty()) {
				console.printColor(4, "\nНечего отменять — история пуста.\n");
				console.pause();
				return;
			}

			console.printColor(10, "Отменяем последнее переименование ("
				+ std::to_string(lastRenameHistory.size()) + " файлов)\n\n");

			int undone = 0;
			for (auto it = lastRenameHistory.rbegin(); it != lastRenameHistory.rend(); ++it) {
				const auto& oldPath = it->first;
				const auto& newPath = it->second;

				if (std::filesystem::exists(newPath)) {
					try {
						std::filesystem::rename(newPath, oldPath);
						console.printColor(5, "Отменено: ");
						std::cout << newPath.filename().string() << " -> " << oldPath.filename().string() << "\n";
						undone++;
					}
					catch (const std::exception& e) {
						console.printColor(12, "Не удалось отменить: " + newPath.filename().string() + "\n");
					}
				}
			}

			console.printColor(10, "\nГотово! Отменено изменений: " + std::to_string(undone) + "\n");
			lastRenameHistory.clear();
			console.pause();
		}
	}

	// ==== 5. Функция создания папок ====
	bool createSingleFolder(const std::string& folder_name) {
		std::filesystem::path folder_path = std::filesystem::path(currentPath) / folder_name;

		try {
			if (std::filesystem::exists(folder_path)) {
				console.printColor(4, "Пропущено: '" + folder_name + "'(уже существует)\n");
				return false;
			}

			if (std::filesystem::create_directory(folder_path)) {
				console.printColor(10, "Создано: '" + folder_name + "'\n");
				return true;
			}
			else {
				console.printColor(4, "Ошибка: не удалось создать '" + folder_name + "'\n");
			}
		}
		catch (const std::exception& e) {
			console.printColor(4, "Ошибка создания '" + folder_name + "': " + e.what() + "\n");
			return false;
		}
	}

	void create_folders() {
		bool isValid = false;
		std::string folder_choice;
		int number = 1;

		while (!isValid) {
			console.clearConsole();
			printLogo();

			console.printColor(10, "1. "); console.printColor(7, "Создать файлы с авто нумерацией\n");
			console.printColor(10, "2. "); console.printColor(7, "Создать файлы взяв названия из файла\n");
			console.printColor(10, "3. "); console.printColor(7, "Создать файлы ( 1 или через запятую )\n\n");

			console.printColor(10, "Выберите операцию: ");
			std::getline(std::cin, folder_choice);

			if (folder_choice == "1" || folder_choice == "2" ||
				folder_choice == "3") {
				isValid = true;
			}
			else {
				console.printColor(4, "Ошибка: такого меню пункта нет!\n");
				console.pause();
			}
		}

		if (folder_choice == "1") {
			console.clearConsole();
			printLogo();
			int folder_count;
			std::string count_input;
			std::string base_folderName;
			isValid = false;

			while (!isValid) {
				console.printColor(10, "Введите название: ");
				std::getline(std::cin, base_folderName);

				if (base_folderName.empty()) {
					console.printColor(4, "\nОшибка: название не может быть пустым!\n");
					console.pause();
					console.clearConsole();
					printLogo();
				}
				else {
					isValid = true;
				}
			}

			int file_count;
			isValid = false;

			while (!isValid) {
				console.printColor(10, "Введите количество папок: ");
				std::getline(std::cin, count_input);
				try {
					folder_count = std::stoi(count_input);
					if (folder_count <= 0) {
						console.printColor(4, "\nОшибка: количество должно быть положительным числом!\n");
						console.pause();
						console.clearConsole();
						printLogo();
						console.printColor(10, "Название папки: "); std::cout << base_folderName + "\n";
					}
					else {
						isValid = true;
					}
				}
				catch (...) {
					console.printColor(4, "\nОшибка: введите корректное число!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(10, "Название папки: "); std::cout << base_folderName + "\n";
				}
			}

			console.printColor(7, "Начальный номер (по умолчанию 1): ");
			std::string start_input;
			std::getline(std::cin, start_input);

			int start_number = 1;
			if (!start_input.empty()) {
				try {
					start_number = std::stoi(start_input);
				}
				catch (...) {
					console.printColor(14, "Используется начальный номер по умолчанию: 1\n");
				}
			}

			if (!isAccept()) {
				console.pause();
				return;
			}

			int created_count = 0;
			for (int i = 0; i < folder_count; ++i) {
				std::string folder_name = base_folderName + std::to_string(start_number + i);
				if (createSingleFolder(folder_name)) {
					created_count++;
				}
			}

			console.printColor(10, "\nГотово! Создано папок: " + std::to_string(created_count) + "\n");
			console.pause();
		}

		else if (folder_choice == "2") {
			console.clearConsole();
			printLogo();

			std::string foldersName_path;
			bool isValid = false;

			console.printColor(6, "Для работы вам нужен файл с названиями папок, которые вы хотели бы добавить.\n");
			console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");

			// Аналогичный ввод как в переименовании
			while (!isValid) {
				console.printColor(2, "Введите путь к файлу или перетащите файл сюда: ");
				std::getline(std::cin, foldersName_path);

				// Обработка пути
				std::replace(foldersName_path.begin(), foldersName_path.end(), '/', '\\');
				if (!foldersName_path.empty() && foldersName_path.front() == '"' && foldersName_path.back() == '"') {
					foldersName_path = foldersName_path.substr(1, foldersName_path.length() - 2);
				}

				// Проверка существования файла
				if (!std::filesystem::exists(foldersName_path)) {
					console.printColor(4, "\nОшибка: файл не найден!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями папок, которые вы хотели бы добавить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				// Проверка что это файл, а не папка
				if (std::filesystem::is_directory(foldersName_path)) {
					console.printColor(4, "\nОшибка: была передана папка, а не файл!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями папок, которые вы хотели бы добавить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				// Проверка расширения файла
				std::string extension = std::filesystem::path(foldersName_path).extension().string();
				std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
				if (extension != ".txt") {
					console.printColor(14, "\nПредупреждение: рекомендуется использовать .txt файлы\n");
					console.printColor(14, "Продолжить с этим файлом? (Y/N): ");

					std::string confirm;
					std::getline(std::cin, confirm);
					if (confirm != "Y" && confirm != "y" && confirm != "да" && confirm != "lf") {
						console.pause();
						console.clearConsole();
						printLogo();
						console.printColor(6, "Для работы вам нужен файл с названиями папок, которые вы хотели бы добавить.\n");
						console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
						continue;
					}
				}

				isValid = true;
			}

			// Чтение файла с названиями папок
			std::vector<std::string> folderNames;
			try {
				// Попробуем прочитать как UTF-8
				std::ifstream file(foldersName_path, std::ios::binary);
				if (!file.is_open()) {
					console.printColor(4, "\nОшибка: не удалось открыть файл!\n");
					console.pause();
					return;
				}

				// Читаем весь файл
				std::string fileContent((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				file.close();

				// Функция для преобразования из UTF-8 в ANSI (Windows-1251)
				auto utf8ToAnsi = [](const std::string& utf8) -> std::string {
					if (utf8.empty()) return "";

					// Попробуем определить, есть ли BOM UTF-8
					std::string content = utf8;
					if (utf8.size() >= 3 &&
						(unsigned char)utf8[0] == 0xEF &&
						(unsigned char)utf8[1] == 0xBB &&
						(unsigned char)utf8[2] == 0xBF) {
						content = utf8.substr(3); // Пропускаем BOM
					}

					// Сначала попробуем преобразовать из UTF-8 в широкую строку
					int wideSize = MultiByteToWideChar(CP_UTF8, 0, content.c_str(),
						(int)content.length(), NULL, 0);
					if (wideSize == 0) {
						// Если не UTF-8, вернем как есть (предполагаем ANSI)
						return content;
					}

					std::wstring wideStr(wideSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.length(),
						&wideStr[0], wideSize);

					// Затем преобразуем из широкой строки в ANSI (CP_1251)
					int ansiSize = WideCharToMultiByte(1251, 0, wideStr.c_str(),
						(int)wideStr.length(), NULL, 0, NULL, NULL);
					if (ansiSize == 0) {
						// Если не удалось, попробуем CP_ACP (системная кодировка)
						ansiSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(),
							(int)wideStr.length(), NULL, 0, NULL, NULL);
						if (ansiSize == 0) return content;
					}

					std::string ansiStr(ansiSize, 0);
					WideCharToMultiByte(1251, 0, wideStr.c_str(), (int)wideStr.length(),
						&ansiStr[0], ansiSize, NULL, NULL);
					return ansiStr;
					};

				// Разделяем содержимое файла на строки
				std::istringstream stream(fileContent);
				std::string line;
				int lineCount = 0;

				while (std::getline(stream, line)) {
					// Убираем символы возврата каретки (для Windows формата \r\n)
					if (!line.empty() && line.back() == '\r') {
						line.pop_back();
					}

					if (line.empty()) continue;

					// Преобразуем кодировку
					std::string convertedLine = utf8ToAnsi(line);

					// Удаляем начальные и конечные пробелы
					convertedLine.erase(0, convertedLine.find_first_not_of(" \t\r\n\v\f"));
					convertedLine.erase(convertedLine.find_last_not_of(" \t\r\n\v\f") + 1);

					if (!convertedLine.empty()) {
						folderNames.push_back(convertedLine);
						lineCount++;
					}
				}

				console.printColor(10, "\nПрочитано названий из файла: " + std::to_string(lineCount) + "\n");

				if (folderNames.empty()) {
					console.printColor(4, "Файл пуст или не содержит валидных названий!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Создаем папки
				int createdCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем создание папок...\n\n");

				for (const auto& folderName : folderNames) {
					if (createSingleFolder(folderName)) {
						createdCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно создано папок: " + std::to_string(createdCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось создать папок: " + std::to_string(failedCount) + "\n");
				}

			}
			catch (const std::exception& e) {
				console.printColor(4, "Ошибка при чтении файла: " + std::string(e.what()) + "\n");
			}

			console.pause();
		}

		else if (folder_choice == "3") {
			bool isValid = false;
			std::string input;

			while (!isValid) {
				console.clearConsole();
				printLogo();

				console.printColor(10, "Ввод названий папок\n\n");
				console.printColor(7, "Варианты ввода:\n");
				console.printColor(10, "1. "); console.printColor(7, "Одно название: Работа\n");
				console.printColor(10, "2. "); console.printColor(7, "Несколько через запятую: Работа, Учеба, Проекты\n");
				console.printColor(10, "3. "); console.printColor(7, "С автонумерацией: Папка [1-10]\n\n");

				console.printColor(10, "Введите название или названия через запятую: ");
				std::getline(std::cin, input);

				if (input.empty()) {
					console.printColor(4, "\nОшибка: не введено ни одного названия!\n");
					console.pause();
					continue;
				}

				// Проверяем, не хочет ли пользователь автонумерацию
				if (input.find('[') != std::string::npos && input.find(']') != std::string::npos) {
					// Обработка автонумерации: Папка [1-10]
					try {
						size_t startBracket = input.find('[');
						size_t endBracket = input.find(']');

						if (startBracket != std::string::npos && endBracket != std::string::npos && endBracket > startBracket) {
							std::string baseName = input.substr(0, startBracket);
							std::string rangeStr = input.substr(startBracket + 1, endBracket - startBracket - 1);

							// Удаляем пробелы в базовом имени
							baseName.erase(0, baseName.find_first_not_of(" \t\n\r\f\v"));
							baseName.erase(baseName.find_last_not_of(" \t\n\r\f\v") + 1);

							// Проверяем формат диапазона
							size_t dashPos = rangeStr.find('-');
							if (dashPos != std::string::npos) {
								int startNum = std::stoi(rangeStr.substr(0, dashPos));
								int endNum = std::stoi(rangeStr.substr(dashPos + 1));

								if (startNum <= endNum) {
									console.printColor(10, "\nСоздание папок с именами: " + baseName +
										std::to_string(startNum) + " - " + baseName + std::to_string(endNum) + "\n");

									if (!isAccept()) {
										console.pause();
										return;
									}

									int createdCount = 0;
									int failedCount = 0;

									for (int i = startNum; i <= endNum; i++) {
										std::string folderName = baseName + std::to_string(i);
										if (createSingleFolder(folderName)) {
											createdCount++;
										}
										else {
											failedCount++;
										}
									}

									console.printColor(10, "\nГотово!\n");
									console.printColor(10, "Успешно создано папок: " + std::to_string(createdCount) + "\n");

									if (failedCount > 0) {
										console.printColor(4, "Не удалось создать папок: " + std::to_string(failedCount) + "\n");
									}

									console.pause();
									return;
								}
							}
						}
					}
					catch (...) {
						// Если не удалось разобрать автонумерацию, продолжаем как обычный ввод
					}
				}

				// Обычный ввод через запятую
				std::vector<std::string> folderNames;
				std::stringstream ss(input);
				std::string name;
				int totalNames = 0;

				while (std::getline(ss, name, ',')) {
					name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
					name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);

					if (!name.empty()) {
						folderNames.push_back(name);
						totalNames++;
					}
				}

				if (folderNames.empty()) {
					console.printColor(4, "\nОшибка: не найдено валидных названий!\n");
					console.printColor(7, "Примеры ввода:\n");
					console.printColor(7, "  • Простая папка\n");
					console.printColor(7, "  • Папка1, Папка2, Папка3\n");
					console.printColor(7, "  • Папка [1-5]  (создаст Папка1, Папка2, ..., Папка5)\n");
					console.pause();
					continue;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				int createdCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nСоздаем папки...\n\n");

				for (const auto& folderName : folderNames) {
					if (createSingleFolder(folderName)) {
						createdCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно создано папок: " + std::to_string(createdCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось создать папок: " + std::to_string(failedCount) + "\n");
				}

				console.pause();
				isValid = true;
			}
		}
	}

	// ==== 6. Функция создания файлов ====
	bool createSingleFile(const std::string& file_name) {
		std::filesystem::path file_path = std::filesystem::path(currentPath) / file_name;

		try {
			if (std::filesystem::exists(file_path)) {
				console.printColor(4, "Пропущено: '" + file_name + "' (уже существует)\n");
				return false;
			}

			// Создаем пустой файл
			std::ofstream file(file_path);
			if (file.is_open()) {
				file.close();
				console.printColor(10, "Создано: '" + file_name + "'\n");
				return true;
			}
			else {
				console.printColor(4, "Ошибка: не удалось создать '" + file_name + "'\n");
				return false;
			}
		}
		catch (const std::exception& e) {
			console.printColor(4, "Ошибка создания '" + file_name + "': " + e.what() + "\n");
			return false;
		}
	}

	void create_files() {
		bool isValid = false;
		std::string file_choice;

		while (!isValid) {
			console.clearConsole();
			printLogo();

			console.printColor(10, "1. "); console.printColor(7, "Создать файлы с авто нумерацией\n");
			console.printColor(10, "2. "); console.printColor(7, "Создать файлы взяв названия из файла\n");
			console.printColor(10, "3. "); console.printColor(7, "Создать файлы (1 или через запятую)\n\n");

			console.printColor(10, "Выберите операцию: ");
			std::getline(std::cin, file_choice);

			if (file_choice == "1" || file_choice == "2" || file_choice == "3") {
				isValid = true;
			}
			else {
				console.printColor(4, "Ошибка: такого меню пункта нет!\n");
				console.pause();
			}
		}

		if (file_choice == "1") {
			console.clearConsole();
			printLogo();

			int file_count;
			std::string count_input;
			std::string base_fileName;
			isValid = false;

			// Ввод базового имени
			while (!isValid) {
				console.printColor(10, "Введите название (без расширения): ");
				std::getline(std::cin, base_fileName);

				if (base_fileName.empty()) {
					console.printColor(4, "\nОшибка: название не может быть пустым!\n");
					console.pause();
					console.clearConsole();
					printLogo();
				}
				else {
					isValid = true;
				}
			}

			// Ввод расширения файла (с .txt по умолчанию)
			std::string file_extension;
			console.printColor(10, "Введите расширение файла (по умолчанию .txt): ");
			std::getline(std::cin, file_extension);

			// Если ничего не ввели - используем .txt
			if (file_extension.empty()) {
				file_extension = ".txt";
				console.printColor(14, "Используется расширение по умолчанию: .txt\n");
			}
			// Добавляем точку если ее нет
			else if (file_extension[0] != '.') {
				file_extension = "." + file_extension;
			}

			// Ввод количества файлов
			isValid = false;
			while (!isValid) {
				console.printColor(10, "Введите количество файлов: ");
				std::getline(std::cin, count_input);
				try {
					file_count = std::stoi(count_input);
					if (file_count <= 0) {
						console.printColor(4, "\nОшибка: количество должно быть положительным числом!\n");
						console.pause();
						console.clearConsole();
						printLogo();
						console.printColor(10, "Название файла: "); std::cout << base_fileName + file_extension + "\n";
					}
					else {
						isValid = true;
					}
				}
				catch (...) {
					console.printColor(4, "\nОшибка: введите корректное число!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(10, "Название файла: "); std::cout << base_fileName + file_extension + "\n";
				}
			}

			// Ввод начального номера
			console.printColor(7, "Начальный номер (по умолчанию 1): ");
			std::string start_input;
			std::getline(std::cin, start_input);

			int start_number = 1;
			if (!start_input.empty()) {
				try {
					start_number = std::stoi(start_input);
				}
				catch (...) {
					console.printColor(14, "Используется начальный номер по умолчанию: 1\n");
				}
			}

			if (!isAccept()) {
				console.pause();
				return;
			}

			// Создание файлов
			int created_count = 0;
			console.printColor(10, "\nСоздаю файлы...\n\n");

			for (int i = 0; i < file_count; ++i) {
				std::string file_name = base_fileName + std::to_string(start_number + i) + file_extension;
				if (createSingleFile(file_name)) {
					created_count++;
				}
			}

			console.printColor(10, "\nГотово! Создано файлов: " + std::to_string(created_count) + "\n");
			console.pause();
		}

		else if (file_choice == "2") {
			console.clearConsole();
			printLogo();

			std::string filesName_path;
			isValid = false;

			console.printColor(6, "Для работы вам нужен файл с названиями файлов, которые вы хотели бы добавить.\n");
			console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");

			// Ввод пути к файлу
			while (!isValid) {
				console.printColor(2, "Введите путь к файлу или перетащите файл сюда: ");
				std::getline(std::cin, filesName_path);

				// Обработка пути
				std::replace(filesName_path.begin(), filesName_path.end(), '/', '\\');
				if (!filesName_path.empty() && filesName_path.front() == '"' && filesName_path.back() == '"') {
					filesName_path = filesName_path.substr(1, filesName_path.length() - 2);
				}

				// Проверка существования файла
				if (!std::filesystem::exists(filesName_path)) {
					console.printColor(4, "\nОшибка: файл не найден!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями файлов, которые вы хотели бы добавить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				// Проверка что это файл, а не папка
				if (std::filesystem::is_directory(filesName_path)) {
					console.printColor(4, "\nОшибка: была передана папка, а не файл!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями файлов, которые вы хотели бы добавить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				isValid = true;
			}

			// Автоматически добавляем .txt к именам без расширения
			std::string autoExtension;
			console.printColor(10, "\nАвтоматически добавлять .txt к именам без расширения? (Y/N, по умолчанию Y): ");
			std::getline(std::cin, autoExtension);
			bool addTxtExtension = true;
			if (autoExtension == "N" || autoExtension == "n" || autoExtension == "нет" || autoExtension == "no") {
				addTxtExtension = false;
				console.printColor(14, "Расширения добавляться не будут\n");
			}
			else {
				console.printColor(10, "К именам без расширения будет добавлено .txt\n");
			}

			// Чтение файла с названиями файлов
			std::vector<std::string> fileNames;
			try {
				// Попробуем прочитать как UTF-8
				std::ifstream file(filesName_path, std::ios::binary);
				if (!file.is_open()) {
					console.printColor(4, "\nОшибка: не удалось открыть файл!\n");
					console.pause();
					return;
				}

				// Читаем весь файл
				std::string fileContent((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				file.close();

				// Функция для преобразования из UTF-8 в ANSI
				auto utf8ToAnsi = [](const std::string& utf8) -> std::string {
					if (utf8.empty()) return "";

					// Пропускаем BOM если есть
					std::string content = utf8;
					if (utf8.size() >= 3 &&
						(unsigned char)utf8[0] == 0xEF &&
						(unsigned char)utf8[1] == 0xBB &&
						(unsigned char)utf8[2] == 0xBF) {
						content = utf8.substr(3);
					}

					int wideSize = MultiByteToWideChar(CP_UTF8, 0, content.c_str(),
						(int)content.length(), NULL, 0);
					if (wideSize == 0) {
						return content;
					}

					std::wstring wideStr(wideSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.length(),
						&wideStr[0], wideSize);

					int ansiSize = WideCharToMultiByte(1251, 0, wideStr.c_str(),
						(int)wideStr.length(), NULL, 0, NULL, NULL);
					if (ansiSize == 0) {
						ansiSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(),
							(int)wideStr.length(), NULL, 0, NULL, NULL);
						if (ansiSize == 0) return content;
					}

					std::string ansiStr(ansiSize, 0);
					WideCharToMultiByte(1251, 0, wideStr.c_str(), (int)wideStr.length(),
						&ansiStr[0], ansiSize, NULL, NULL);
					return ansiStr;
					};

				// Разделяем содержимое файла на строки
				std::istringstream stream(fileContent);
				std::string line;
				int lineCount = 0;

				while (std::getline(stream, line)) {
					if (!line.empty() && line.back() == '\r') {
						line.pop_back();
					}

					if (line.empty()) continue;

					// Преобразуем кодировку
					std::string convertedLine = utf8ToAnsi(line);

					// Удаляем начальные и конечные пробелы
					convertedLine.erase(0, convertedLine.find_first_not_of(" \t\r\n\v\f"));
					convertedLine.erase(convertedLine.find_last_not_of(" \t\r\n\v\f") + 1);

					if (!convertedLine.empty()) {
						// Автоматически добавляем .txt если нет расширения и пользователь согласен
						if (addTxtExtension && convertedLine.find('.') == std::string::npos) {
							convertedLine += ".txt";
						}

						fileNames.push_back(convertedLine);
						lineCount++;
					}
				}

				console.printColor(10, "\nПрочитано названий из файла: " + std::to_string(lineCount) + "\n");

				if (fileNames.empty()) {
					console.printColor(4, "Файл пуст или не содержит валидных названий!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Создаем файлы
				int createdCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем создание файлов...\n\n");

				for (const auto& fileName : fileNames) {
					if (createSingleFile(fileName)) {
						createdCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно создано файлов: " + std::to_string(createdCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось создать файлов: " + std::to_string(failedCount) + "\n");
				}

			}
			catch (const std::exception& e) {
				console.printColor(4, "Ошибка при чтении файла: " + std::string(e.what()) + "\n");
			}

			console.pause();
		}

		else if (file_choice == "3") {
			bool isValid = false;
			std::string input;

			while (!isValid) {
				console.clearConsole();
				printLogo();

				console.printColor(10, "Ввод названий файлов\n\n");
				console.printColor(7, "Варианты ввода:\n");
				console.printColor(10, "1. "); console.printColor(7, "Одно название: документ.txt\n");
				console.printColor(10, "2. "); console.printColor(7, "Несколько через запятую: файл1.txt, файл2.doc, файл3.xlsx\n");
				console.printColor(10, "3. "); console.printColor(7, "С автонумерацией: документ[1-5].txt\n\n");

				console.printColor(7, "Если расширение не указано, будет добавлено .txt\n\n");

				console.printColor(10, "Введите название или названия через запятую: ");
				std::getline(std::cin, input);

				if (input.empty()) {
					console.printColor(4, "\nОшибка: не введено ни одного названия!\n");
					console.pause();
					continue;
				}

				// Проверяем, не хочет ли пользователь автонумерацию
				if (input.find('[') != std::string::npos && input.find(']') != std::string::npos) {
					// Обработка автонумерации: документ[1-5].txt
					try {
						size_t startBracket = input.find('[');
						size_t endBracket = input.find(']');

						if (startBracket != std::string::npos && endBracket != std::string::npos && endBracket > startBracket) {
							// Ищем расширение после закрывающей скобки
							size_t extensionPos = input.find('.', endBracket);
							std::string baseName = input.substr(0, startBracket);
							std::string rangeStr = input.substr(startBracket + 1, endBracket - startBracket - 1);
							std::string fileExtension = "";

							if (extensionPos != std::string::npos) {
								fileExtension = input.substr(extensionPos);
							}
							else {
								// Если расширение не указано - используем .txt
								fileExtension = ".txt";
								console.printColor(14, "Расширение не указано, используется .txt\n");
							}

							// Удаляем пробелы в базовом имени
							baseName.erase(0, baseName.find_first_not_of(" \t\n\r\f\v"));
							baseName.erase(baseName.find_last_not_of(" \t\n\r\f\v") + 1);

							// Проверяем формат диапазона
							size_t dashPos = rangeStr.find('-');
							if (dashPos != std::string::npos) {
								int startNum = std::stoi(rangeStr.substr(0, dashPos));
								int endNum = std::stoi(rangeStr.substr(dashPos + 1));

								if (startNum <= endNum) {
									console.printColor(10, "\nСоздание файлов с именами: " + baseName +
										std::to_string(startNum) + fileExtension + " - " + baseName +
										std::to_string(endNum) + fileExtension + "\n");

									if (!isAccept()) {
										console.pause();
										return;
									}

									int createdCount = 0;
									int failedCount = 0;

									for (int i = startNum; i <= endNum; i++) {
										std::string fileName = baseName + std::to_string(i) + fileExtension;
										if (createSingleFile(fileName)) {
											createdCount++;
										}
										else {
											failedCount++;
										}
									}

									console.printColor(10, "\nГотово!\n");
									console.printColor(10, "Успешно создано файлов: " + std::to_string(createdCount) + "\n");

									if (failedCount > 0) {
										console.printColor(4, "Не удалось создать файлов: " + std::to_string(failedCount) + "\n");
									}

									console.pause();
									return;
								}
							}
						}
					}
					catch (...) {
						// Если не удалось разобрать автонумерацию, продолжаем как обычный ввод
					}
				}

				// Обычный ввод через запятую
				std::vector<std::string> fileNames;
				std::stringstream ss(input);
				std::string name;
				int totalNames = 0;

				while (std::getline(ss, name, ',')) {
					name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
					name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);

					if (!name.empty()) {
						// Автоматически добавляем .txt если нет расширения
						if (name.find('.') == std::string::npos) {
							name += ".txt";
							console.printColor(14, "Добавлено .txt к '" + name + "'\n");
						}

						fileNames.push_back(name);
						totalNames++;
					}
				}

				if (fileNames.empty()) {
					console.printColor(4, "\nОшибка: не найдено валидных названий!\n");
					console.printColor(7, "Примеры ввода:\n");
					console.printColor(7, "  • документ.txt\n");
					console.printColor(7, "  • файл1.txt, файл2.doc, файл3.xlsx\n");
					console.printColor(7, "  • документ[1-5].txt  (создаст документ1.txt, ..., документ5.txt)\n");
					console.printColor(7, "  • файл  (будет создан как файл.txt)\n");
					console.pause();
					continue;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				int createdCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nСоздаем файлы...\n\n");

				for (const auto& fileName : fileNames) {
					if (createSingleFile(fileName)) {
						createdCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно создано файлов: " + std::to_string(createdCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось создать файлов: " + std::to_string(failedCount) + "\n");
				}

				console.pause();
				isValid = true;
			}
		}
	}

	// ==== 7. Функция удаления папок ====
	bool deleteSingleFolder(const std::filesystem::path& folder_path) {
		try {
			std::filesystem::remove_all(folder_path);
			console.printColor(10, "Удалено: '" + folder_path.filename().string() + "'\n");
			return true;
		}
		catch (const std::exception& e) {
			console.printColor(4, "Ошибка удаления '" + folder_path.filename().string() + "': " + e.what() + "\n");
			return false;
		}
	}

	bool deleteSingleFile(const std::filesystem::path& file_path) {
		try {
			if (std::filesystem::remove(file_path)) {
				console.printColor(10, "Удалено: '" + file_path.filename().string() + "'\n");
				return true;
			}
			else {
				console.printColor(4, "Ошибка: не удалось удалить '" + file_path.filename().string() + "'\n");
				return false;
			}
		}
		catch (const std::exception& e) {
			console.printColor(4, "Ошибка удаления '" + file_path.filename().string() + "': " + e.what() + "\n");
			return false;
		}
	}

	void deleteFolders() {
		console.clearConsole();
		printLogo();

		std::string delete_choice;
		bool isValid = false;

		while (!isValid) {
			console.clearConsole();
			printLogo();

			console.printColor(10, "1. "); console.printColor(7, "Удалить все папки в текущей директории\n");
			console.printColor(10, "2. "); console.printColor(7, "Удалить папки по названиям из файла\n");
			console.printColor(10, "3. "); console.printColor(7, "Удалить папки по названиям (через запятую)\n");
			console.printColor(4, "0. "); console.printColor(7, "Вернуться в меню\n\n");

			console.printColor(10, "Выберите действие: ");
			std::getline(std::cin, delete_choice);

			if (delete_choice == "1" || delete_choice == "2" || delete_choice == "3" || delete_choice == "0") {
				isValid = true;
			}
			else {
				console.printColor(4, "\nОшибка: Такого пункта меню нет\n");
				console.pause();
			}
		}

		if (delete_choice == "1") {
			// Получаем список папок
			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			std::vector<std::filesystem::path> folders;
			for (const auto& entry : entries) {
				if (entry.is_directory()) {
					folders.push_back(entry.path());
				}
			}

			if (folders.empty()) {
				console.printColor(4, "Ошибка: в текущей директории нет папок\n");
				console.pause();
				return;
			}

			if (!isAccept()) {
				console.pause();
				return;
			}

			int deleted_count = 0;
			int error_count = 0;

			console.printColor(10, "\nНачинаем удаление...\n\n");

			for (const auto& folder_path : folders) {
				if (deleteSingleFolder(folder_path)) {
					deleted_count++;
				}
				else {
					error_count++;
				}
			}

			console.printColor(10, "\nГотово! Удалено папок: " + std::to_string(deleted_count) + "\n");
			if (error_count > 0) {
				console.printColor(4, "Ошибок при удалении: " + std::to_string(error_count) + "\n");
			}
			console.pause();
		}

		else if (delete_choice == "2") {
			console.clearConsole();
			printLogo();

			std::string foldersName_path;
			bool isValid = false;

			console.printColor(6, "Для работы вам нужен файл с названиями папок, которые нужно удалить.\n");
			console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");

			// Ввод пути к файлу
			while (!isValid) {
				console.printColor(2, "Введите путь к файлу или перетащите файл сюда: ");
				std::getline(std::cin, foldersName_path);

				// Обработка пути
				std::replace(foldersName_path.begin(), foldersName_path.end(), '/', '\\');
				if (!foldersName_path.empty() && foldersName_path.front() == '"' && foldersName_path.back() == '"') {
					foldersName_path = foldersName_path.substr(1, foldersName_path.length() - 2);
				}

				// Проверка существования файла
				if (!std::filesystem::exists(foldersName_path)) {
					console.printColor(4, "\nОшибка: файл не найден!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями папок, которые нужно удалить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				// Проверка что это файл, а не папка
				if (std::filesystem::is_directory(foldersName_path)) {
					console.printColor(4, "\nОшибка: была передана папка, а не файл!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями папок, которые нужно удалить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n\n");
					continue;
				}

				isValid = true;
			}

			// Чтение файла с названиями папок
			std::vector<std::string> folderNames;
			try {
				std::ifstream file(foldersName_path, std::ios::binary);
				if (!file.is_open()) {
					console.printColor(4, "\nОшибка: не удалось открыть файл!\n");
					console.pause();
					return;
				}

				// Функция для преобразования из UTF-8 в ANSI
				auto utf8ToAnsi = [](const std::string& utf8) -> std::string {
					if (utf8.empty()) return "";
					std::string content = utf8;
					if (utf8.size() >= 3 &&
						(unsigned char)utf8[0] == 0xEF &&
						(unsigned char)utf8[1] == 0xBB &&
						(unsigned char)utf8[2] == 0xBF) {
						content = utf8.substr(3);
					}

					int wideSize = MultiByteToWideChar(CP_UTF8, 0, content.c_str(),
						(int)content.length(), NULL, 0);
					if (wideSize == 0) return content;

					std::wstring wideStr(wideSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.length(),
						&wideStr[0], wideSize);

					int ansiSize = WideCharToMultiByte(1251, 0, wideStr.c_str(),
						(int)wideStr.length(), NULL, 0, NULL, NULL);
					if (ansiSize == 0) {
						ansiSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(),
							(int)wideStr.length(), NULL, 0, NULL, NULL);
						if (ansiSize == 0) return content;
					}

					std::string ansiStr(ansiSize, 0);
					WideCharToMultiByte(1251, 0, wideStr.c_str(), (int)wideStr.length(),
						&ansiStr[0], ansiSize, NULL, NULL);
					return ansiStr;
					};

				// Читаем файл
				std::string fileContent((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				file.close();

				std::istringstream stream(fileContent);
				std::string line;
				int lineCount = 0;

				while (std::getline(stream, line)) {
					if (!line.empty() && line.back() == '\r') {
						line.pop_back();
					}

					if (line.empty()) continue;

					std::string convertedLine = utf8ToAnsi(line);
					convertedLine.erase(0, convertedLine.find_first_not_of(" \t\r\n\v\f"));
					convertedLine.erase(convertedLine.find_last_not_of(" \t\r\n\v\f") + 1);

					if (!convertedLine.empty()) {
						folderNames.push_back(convertedLine);
						lineCount++;
					}
				}

				console.printColor(10, "\nПрочитано названий из файла: " + std::to_string(lineCount) + "\n");

				if (folderNames.empty()) {
					console.printColor(4, "Файл пуст или не содержит валидных названий!\n");
					console.pause();
					return;
				}

				// Проверяем существование папок
				std::vector<std::filesystem::path> existingFolders;
				std::vector<std::string> missingFolders;

				console.printColor(10, "\nПроверяем существование папок...\n");

				for (const auto& folderName : folderNames) {
					std::filesystem::path folder_path = std::filesystem::path(currentPath) / folderName;
					if (std::filesystem::exists(folder_path) && std::filesystem::is_directory(folder_path)) {
						existingFolders.push_back(folder_path);
					}
					else {
						missingFolders.push_back(folderName);
					}
				}

				if (!missingFolders.empty()) {
					console.printColor(4, "\nНе найдены следующие папки:\n");
					for (const auto& missing : missingFolders) {
						console.printColor(4, "  • " + missing + "\n");
					}
				}

				if (existingFolders.empty()) {
					console.printColor(4, "\nНе найдено ни одной папки для удаления!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Удаляем папки
				int deletedCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем удаление...\n\n");

				for (const auto& folder_path : existingFolders) {
					if (deleteSingleFolder(folder_path)) {
						deletedCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно удалено папок: " + std::to_string(deletedCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось удалить папок: " + std::to_string(failedCount) + "\n");
				}

			}
			catch (const std::exception& e) {
				console.printColor(4, "Ошибка при чтении файла: " + std::string(e.what()) + "\n");
			}

			console.pause();
		}

		else if (delete_choice == "3") {
			bool isValid = false;
			std::string input;

			while (!isValid) {
				console.clearConsole();
				printLogo();

				console.printColor(10, "Ввод названий папок для удаления\n\n");
				console.printColor(7, "Введите названия папок через запятую:\n");
				console.printColor(7, "Пример: Папка1, Папка2, Моя папка\n\n");

				console.printColor(10, "Введите названия: ");
				std::getline(std::cin, input);

				if (input.empty()) {
					console.printColor(4, "\nОшибка: не введено ни одного названия!\n");
					console.pause();
					continue;
				}

				// Разделяем ввод на отдельные названия
				std::vector<std::string> folderNames;
				std::stringstream ss(input);
				std::string name;
				int totalNames = 0;

				while (std::getline(ss, name, ',')) {
					name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
					name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);

					if (!name.empty()) {
						folderNames.push_back(name);
						totalNames++;
					}
				}

				if (folderNames.empty()) {
					console.printColor(4, "\nОшибка: не найдено валидных названий!\n");
					console.printColor(7, "Пример ввода: Папка1, Папка2, Моя папка\n");
					console.pause();
					continue;
				}

				// Проверяем существование папок
				std::vector<std::filesystem::path> existingFolders;
				std::vector<std::string> missingFolders;

				console.printColor(10, "\nПроверяем существование папок...\n");

				for (const auto& folderName : folderNames) {
					std::filesystem::path folder_path = std::filesystem::path(currentPath) / folderName;
					if (std::filesystem::exists(folder_path) && std::filesystem::is_directory(folder_path)) {
						existingFolders.push_back(folder_path);
					}
					else {
						missingFolders.push_back(folderName);
					}
				}

				if (!missingFolders.empty()) {
					console.printColor(4, "\nНе найдены следующие папки:\n");
					for (const auto& missing : missingFolders) {
						console.printColor(4, "  • " + missing + "\n");
					}
				}

				if (existingFolders.empty()) {
					console.printColor(4, "\nНе найдено ни одной папки для удаления!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Удаляем папки
				int deletedCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем удаление...\n\n");

				for (const auto& folder_path : existingFolders) {
					if (deleteSingleFolder(folder_path)) {
						deletedCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно удалено папок: " + std::to_string(deletedCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось удалить папок: " + std::to_string(failedCount) + "\n");
				}

				console.pause();
				isValid = true;
			}
		}

		else if (delete_choice == "0") {
			// Просто возвращаемся в меню
			return;
		}
	}

	// ==== 8. Функция удаления файлов ====
	void deleteFiles() {
		console.clearConsole();
		printLogo();

		std::string delete_choice;
		bool isValid = false;

		while (!isValid) {
			console.clearConsole();
			printLogo();

			console.printColor(10, "1. "); console.printColor(7, "Удалить все файлы в текущей директории\n");
			console.printColor(10, "2. "); console.printColor(7, "Удалить файлы по названиям из файла\n");
			console.printColor(10, "3. "); console.printColor(7, "Удалить файлы по названиям (через запятую)\n");
			console.printColor(4, "0. "); console.printColor(7, "Вернуться в меню\n\n");

			console.printColor(10, "Выберите действие: ");
			std::getline(std::cin, delete_choice);

			if (delete_choice == "1" || delete_choice == "2" || delete_choice == "3" || delete_choice == "0") {
				isValid = true;
			}
			else {
				console.printColor(4, "\nОшибка: Такого пункта меню нет\n");
				console.pause();
			}
		}

		if (delete_choice == "1") {
			// Получаем список файлов (только файлы, не папки)
			std::vector<std::filesystem::directory_entry> entries;
			for (const auto& entry : std::filesystem::directory_iterator(currentPath))
				entries.push_back(entry);

			std::sort(entries.begin(), entries.end(),
				[this](const auto& a, const auto& b) {
					if (a.is_directory() && !b.is_directory()) return true;
					if (!a.is_directory() && b.is_directory()) return false;
					return natural_less(a.path().filename().string(),
						b.path().filename().string());
				});

			std::vector<std::filesystem::path> files;
			for (const auto& entry : entries) {
				if (!entry.is_directory()) {
					files.push_back(entry.path());
				}
			}

			if (files.empty()) {
				console.printColor(4, "Ошибка: в текущей директории нет файлов\n");
				console.pause();
				return;
			}

			if (!isAccept()) {
				console.pause();
				return;
			}

			int deleted_count = 0;
			int error_count = 0;

			console.printColor(10, "\nНачинаем удаление...\n\n");

			for (const auto& file_path : files) {
				if (deleteSingleFile(file_path)) {
					deleted_count++;
				}
				else {
					error_count++;
				}
			}

			console.printColor(10, "\nГотово! Удалено файлов: " + std::to_string(deleted_count) + "\n");
			if (error_count > 0) {
				console.printColor(4, "Ошибок при удалении: " + std::to_string(error_count) + "\n");
			}
			console.pause();
		}

		else if (delete_choice == "2") {
			console.clearConsole();
			printLogo();

			std::string filesName_path;
			bool isValid = false;

			console.printColor(6, "Для работы вам нужен файл с названиями файлов (без расширения), которые нужно удалить.\n");
			console.printColor(6, "Укажите каждое название в файле с новой строки!\n");
			console.printColor(6, "Будут удалены ВСЕ файлы с такими именами, независимо от расширения!\n\n");

			// Ввод пути к файлу
			while (!isValid) {
				console.printColor(2, "Введите путь к файлу или перетащите файл сюда: ");
				std::getline(std::cin, filesName_path);

				// Обработка пути
				std::replace(filesName_path.begin(), filesName_path.end(), '/', '\\');
				if (!filesName_path.empty() && filesName_path.front() == '"' && filesName_path.back() == '"') {
					filesName_path = filesName_path.substr(1, filesName_path.length() - 2);
				}

				if (!std::filesystem::exists(filesName_path)) {
					console.printColor(4, "\nОшибка: файл не найден!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями файлов (без расширения), которые нужно удалить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n");
					console.printColor(6, "Будут удалены ВСЕ файлы с такими именами, независимо от расширения!\n\n");
					continue;
				}

				if (std::filesystem::is_directory(filesName_path)) {
					console.printColor(4, "\nОшибка: была передана папка, а не файл!\n");
					console.pause();
					console.clearConsole();
					printLogo();
					console.printColor(6, "Для работы вам нужен файл с названиями файлов (без расширения), которые нужно удалить.\n");
					console.printColor(6, "Укажите каждое название в файле с новой строки!\n");
					console.printColor(6, "Будут удалены ВСЕ файлы с такими именами, независимо от расширения!\n\n");
					continue;
				}

				isValid = true;
			}

			// Чтение файла с названиями файлов (без расширения)
			std::vector<std::string> fileNamesWithoutExt;
			try {
				std::ifstream file(filesName_path, std::ios::binary);
				if (!file.is_open()) {
					console.printColor(4, "\nОшибка: не удалось открыть файл!\n");
					console.pause();
					return;
				}

				// Функция для преобразования из UTF-8 в ANSI
				auto utf8ToAnsi = [](const std::string& utf8) -> std::string {
					if (utf8.empty()) return "";
					std::string content = utf8;
					if (utf8.size() >= 3 &&
						(unsigned char)utf8[0] == 0xEF &&
						(unsigned char)utf8[1] == 0xBB &&
						(unsigned char)utf8[2] == 0xBF) {
						content = utf8.substr(3);
					}

					int wideSize = MultiByteToWideChar(CP_UTF8, 0, content.c_str(),
						(int)content.length(), NULL, 0);
					if (wideSize == 0) return content;

					std::wstring wideStr(wideSize, 0);
					MultiByteToWideChar(CP_UTF8, 0, content.c_str(), (int)content.length(),
						&wideStr[0], wideSize);

					int ansiSize = WideCharToMultiByte(1251, 0, wideStr.c_str(),
						(int)wideStr.length(), NULL, 0, NULL, NULL);
					if (ansiSize == 0) {
						ansiSize = WideCharToMultiByte(CP_ACP, 0, wideStr.c_str(),
							(int)wideStr.length(), NULL, 0, NULL, NULL);
						if (ansiSize == 0) return content;
					}

					std::string ansiStr(ansiSize, 0);
					WideCharToMultiByte(1251, 0, wideStr.c_str(), (int)wideStr.length(),
						&ansiStr[0], ansiSize, NULL, NULL);
					return ansiStr;
					};

				// Читаем файл
				std::string fileContent((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				file.close();

				std::istringstream stream(fileContent);
				std::string line;
				int lineCount = 0;

				while (std::getline(stream, line)) {
					if (!line.empty() && line.back() == '\r') {
						line.pop_back();
					}

					if (line.empty()) continue;

					std::string convertedLine = utf8ToAnsi(line);
					convertedLine.erase(0, convertedLine.find_first_not_of(" \t\r\n\v\f"));
					convertedLine.erase(convertedLine.find_last_not_of(" \t\r\n\v\f") + 1);

					if (!convertedLine.empty()) {
						fileNamesWithoutExt.push_back(convertedLine);
						lineCount++;
					}
				}

				console.printColor(10, "\nПрочитано названий из файла: " + std::to_string(lineCount) + "\n");

				if (fileNamesWithoutExt.empty()) {
					console.printColor(4, "Файл пуст или не содержит валидных названий!\n");
					console.pause();
					return;
				}

				// Ищем все файлы с такими именами (независимо от расширения)
				std::vector<std::filesystem::path> filesToDelete;

				// Проходим по всем файлам в текущей директории
				for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
					if (!entry.is_directory()) {
						std::string stem = entry.path().stem().string(); // имя без расширения

						// Проверяем, есть ли это имя в нашем списке
						for (const auto& targetName : fileNamesWithoutExt) {
							if (stem == targetName) {
								filesToDelete.push_back(entry.path());
								break; // нашли, переходим к следующему файлу
							}
						}
					}
				}

				if (filesToDelete.empty()) {
					console.printColor(4, "Не найдено ни одного файла для удаления!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Удаляем файлы
				int deletedCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем удаление...\n\n");

				for (const auto& file_path : filesToDelete) {
					if (deleteSingleFile(file_path)) {
						deletedCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно удалено файлов: " + std::to_string(deletedCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось удалить файлов: " + std::to_string(failedCount) + "\n");
				}

			}
			catch (const std::exception& e) {
				console.printColor(4, "Ошибка при чтении файла: " + std::string(e.what()) + "\n");
			}

			console.pause();
		}

		else if (delete_choice == "3") {
			bool isValid = false;
			std::string input;

			while (!isValid) {
				console.clearConsole();
				printLogo();

				console.printColor(10, "Ввод названий файлов для удаления\n\n");
				console.printColor(7, "Введите названия файлов (без расширения) через запятую:\n");
				console.printColor(7, "Пример: файл1, документ, отчет\n");
				console.printColor(7, "Будут удалены ВСЕ файлы с такими именами, независимо от расширения!\n\n");

				console.printColor(10, "Введите названия: ");
				std::getline(std::cin, input);

				if (input.empty()) {
					console.printColor(4, "\nОшибка: не введено ни одного названия!\n");
					console.pause();
					continue;
				}

				// Разделяем ввод на отдельные названия
				std::vector<std::string> fileNamesWithoutExt;
				std::stringstream ss(input);
				std::string name;
				int totalNames = 0;

				while (std::getline(ss, name, ',')) {
					name.erase(0, name.find_first_not_of(" \t\n\r\f\v"));
					name.erase(name.find_last_not_of(" \t\n\r\f\v") + 1);

					if (!name.empty()) {
						fileNamesWithoutExt.push_back(name);
						totalNames++;
					}
				}

				if (fileNamesWithoutExt.empty()) {
					console.printColor(4, "\nОшибка: не найдено валидных названий!\n");
					console.printColor(7, "Пример ввода: файл1, документ, отчет\n");
					console.pause();
					continue;
				}

				// Ищем все файлы с такими именами (независимо от расширения)
				std::vector<std::filesystem::path> filesToDelete;

				// Проходим по всем файлам в текущей директории
				for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
					if (!entry.is_directory()) {
						std::string stem = entry.path().stem().string();

						// Проверяем, есть ли это имя в нашем списке
						for (const auto& targetName : fileNamesWithoutExt) {
							if (stem == targetName) {
								filesToDelete.push_back(entry.path());
								break;
							}
						}
					}
				}

				if (filesToDelete.empty()) {
					console.printColor(4, "Не найдено ни одного файла для удаления!\n");
					console.pause();
					return;
				}

				if (!isAccept()) {
					console.pause();
					return;
				}

				// Удаляем файлы
				int deletedCount = 0;
				int failedCount = 0;

				console.printColor(10, "\nНачинаем удаление...\n\n");

				for (const auto& file_path : filesToDelete) {
					if (deleteSingleFile(file_path)) {
						deletedCount++;
					}
					else {
						failedCount++;
					}
				}

				console.printColor(10, "\nГотово!\n");
				console.printColor(10, "Успешно удалено файлов: " + std::to_string(deletedCount) + "\n");

				if (failedCount > 0) {
					console.printColor(4, "Не удалось удалить файлов: " + std::to_string(failedCount) + "\n");
				}

				console.pause();
				isValid = true;
			}
		}

		else if (delete_choice == "0") {
			return;
		}
	}

public:
	// ==== Запуск файлового менеджера ====

	void run() {
		setupWorkingDirectory();

		while (true) {
			console.clearConsole();
			printLogo();
			console.printColor(2, "Текущая папка: ");
			console.printColor(15, currentPath);
			console.printColor(14, "\n\n");

			printMenu();

			int choice;
			std::string input;

			std::getline(std::cin, input);
			std::stringstream ss(input);
			ss >> choice;

			if (ss.fail()) {
				choice = -1;
			}

			switch (choice) {
			case 1:
				editCurrentPath();
				break;
			case 2:
				show_folderContent();
				break;
			case 3:
				renameFiles("files");
				break;
			case 4:
				renameFiles("directory");
				break;
			case 5:
				create_folders();
				break;
			case 6:
				create_files();
				break;
			case 7:
				deleteFolders();
				break;
			case 8:
				deleteFiles();
				break;
			case 0:
				console.printColor(10, "\nДо свидания!\n");
				return;
			default:
				console.printColor(4, "\nТакого пункта меню пока нет\n");
				console.pause();
				break;
			}
		}
	}
};

int main() {
	fileManager manager;
	manager.run();
	return 0;
}