//
// Created by Mamh on 2024/2/2.
//

#include "SensorManager.h"

SensorManager* SensorManager::instance = nullptr;

SensorManager* SensorManager::getInstance() {
    if (instance == nullptr) {
        instance = new SensorManager(ProjectConfig::DHTPIN, ProjectConfig::DHTTYPE);
    }
    return instance;
}

float SensorManager::readHumidity() {
    return dht.readHumidity();
}

float SensorManager::readTemperature() {
    return dht.readTemperature();
}

