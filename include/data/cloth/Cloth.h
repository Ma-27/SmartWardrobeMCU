/**
 * @description: 
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/5/10 下午 12:09
 */

#ifndef CLOTH_H
#define CLOTH_H

#include <Arduino.h>

class Cloth {
private:
    String id;              // 唯一标识符
    String color;           // 衣物颜色
    String style;           // 衣物款式
    String material;        // 衣物材质
    String size;            // 衣物尺寸
    bool isInCloset;        // 是否在衣柜中
    int hangPosition;       // 悬挂位置
    String brand;           // 品牌
    String purchaseDate;    // 购买日期
    bool isClean;           // 衣物是否干净
    String lastWornDate;    // 上次穿的时间


public:
    // 参数化构造函数
    Cloth(String id = "", String color = "", String style = "", String material = "",
          String size = "", bool isInCloset = false, int hangPosition = -1,
          String brand = "", String purchaseDate = "", bool isClean = true,
          String lastWornDate = "")
            : id(id), color(color), style(style), material(material),
              size(size), isInCloset(isInCloset), hangPosition(hangPosition),
              brand(brand), purchaseDate(purchaseDate), isClean(isClean),
              lastWornDate(lastWornDate) {}

    /// 打印详细信息方法
    String output() const;




    /// -----------------------------------------------------------------------------------------------------------------------------------------
    /// getter 和 setter 方法

    String getId() const;

    void setId(const String &value);

    String getColor() const;

    void setColor(const String &value);

    String getStyle() const;

    void setStyle(const String &value);

    String getMaterial() const;

    void setMaterial(const String &value);

    String getSize() const;

    void setSize(const String &value);

    void setBrand(const String &value);

    String getBrand() const;

    bool getIsInCloset() const;

    void setIsInCloset(bool value);

    int getHangPosition() const;

    void setHangPosition(int value);

    String getPurchaseDate() const;

    void setPurchaseDate(const String &value);

    bool getIsClean() const;

    void setIsClean(bool value);

    String getLastWornDate() const;

    void setLastWornDate(const String &value);
};

#endif // CLOTH_H

