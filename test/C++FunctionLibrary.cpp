/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/3 上午 10:27
 */

#include <functional>
#include <iostream>

int main() {
    std::function<void()> func = []() { std::cout << "Hello, std::function!" << std::endl; };
    func();
    return 0;
}
