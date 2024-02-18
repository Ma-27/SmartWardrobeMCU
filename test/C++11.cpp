/**
 * @description:
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/2/12 下午 08:42
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>

int main() {
    // 使用auto关键字进行自动类型推导
    auto message = "Hello, C++11!";
    std::cout << message << std::endl;

    // 使用初始化列表
    std::vector<int> numbers = {1, 2, 3, 4, 5};

    // 使用基于范围的for循环
    for (auto number: numbers) {
        std::cout << number << " ";
    }
    std::cout << std::endl;

    // 使用Lambda表达式
    std::sort(numbers.begin(), numbers.end(), [](int a, int b) {
        return a > b;
    });

    for (auto number: numbers) {
        std::cout << number << " ";
    }
    std::cout << std::endl;

    // 使用智能指针
    auto ptr = std::make_shared<int>(10);
    std::cout << "Value of ptr: " << *ptr << std::endl;

    return 0;
}

