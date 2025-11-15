#include <iostream>
#include <set>

int main() {
    // Создаём множество из строчных букв латинского алфавита
    std::set<char> latinAlphabet;
    for (char c = 'a'; c <= 'z'; ++c) {
        latinAlphabet.insert(c);
    }

    // Проверяем, содержится ли буква 'c' во множестве
    char target = 'c';
    if (latinAlphabet.find(target) != latinAlphabet.end()) {
        std::cout << "Буква '" << target << "' содержится в множестве." << std::endl;
    } else {
        std::cout << "Буква '" << target << "' не содержится в множестве." << std::endl;
    }

    return 0;
}