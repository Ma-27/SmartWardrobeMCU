/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/10 下午 12:09
 */

#include "data/cloth/Cloth.h"


// 构造函数的实现已在头文件中通过初始化列表完成

String Cloth::output() const {
    String message = "Cloth Information:\n";
    message += "ID: " + id + "\n";
    message += "Color: " + color + "\n";
    message += "Style: " + style + "\n";
    message += "Material: " + material + "\n";
    message += "Size: " + size + "\n";
    message += "Is in Closet: " + String(isInCloset) + "\n";
    message += "Hang Position: " + String(hangPosition) + "\n";
    message += "Brand: " + brand + "\n";
    message += "Purchase Date: " + purchaseDate + "\n";
    message += "Is Clean: " + String(isClean) + "\n";
    message += "Last Worn Date: " + lastWornDate + "\n";
    return message;
}


/// -----------------------------------------------------------------------------------------------------------------------------------------
/// 完整的getter和setter方法
String Cloth::getId() const {
    return id;
}

void Cloth::setId(const String &value) {
    id = value;
}

String Cloth::getColor() const {
    return color;
}

void Cloth::setColor(const String &value) {
    color = value;
}

String Cloth::getStyle() const {
    return style;
}

void Cloth::setStyle(const String &value) {
    style = value;
}

String Cloth::getMaterial() const {
    return material;
}

void Cloth::setMaterial(const String &value) {
    material = value;
}

String Cloth::getSize() const {
    return size;
}

void Cloth::setSize(const String &value) {
    size = value;
}

bool Cloth::getIsInCloset() const {
    return isInCloset;
}

void Cloth::setIsInCloset(bool value) {
    isInCloset = value;
}

int Cloth::getHangPosition() const {
    return hangPosition;
}

void Cloth::setHangPosition(int value) {
    hangPosition = value;
}

String Cloth::getBrand() const {
    return brand;
}

void Cloth::setBrand(const String &value) {
    brand = value;
}

String Cloth::getPurchaseDate() const {
    return purchaseDate;
}

void Cloth::setPurchaseDate(const String &value) {
    purchaseDate = value;
}

bool Cloth::getIsClean() const {
    return isClean;
}

void Cloth::setIsClean(bool value) {
    isClean = value;
}

String Cloth::getLastWornDate() const {
    return lastWornDate;
}

void Cloth::setLastWornDate(const String &value) {
    lastWornDate = value;
}



