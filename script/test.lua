-----------------------------------------------------------------------
-- 使用说明：                                                        --
-- V1.3                                                              --
-- 用户需要自行完成以下2个函数的实现                                 --
-- 1、定时下发数据任务初始化函数：device_timer_init(dev)【可选】     --
-- 2、对设备上传数据进行解析（包括心跳等）：device_data_analyze(dev) --
-----------------------------------------------------------------------
-- 在函数外部定义，以保持跟踪数据下发次数
sendCount = 0



------------------------------------
-- begin-添加用户自定义值或函数等 --

-- end-添加用户自定义值或函数等   --
------------------------------------

------------------------------------------------------------------------------------------
-- 设置定时下发设备的数据（可选）                                                       --
-- @param  dev    user_data   设备管理器                                                --
-- @return 无                                                                           --
-- @notice 此函数为回调函数，不可在脚本内调用                                           --
-- @readme dev提供一下几个函数：                                                        --
--         dev:add(interval,name,data)添加定时下发数据                                  --
--           @param   interval   number   数据下发的时间间隔（秒）                      --
--                    name       string   名称（须保证唯一性）                          --
--                    data       string   数据（二进制数据），使用lua转义字符串         --
--           @return  成功返回true，否则返回false                                       --
--           @notice  定时数据下发的平均频率不超过1，及1/interval_1+...+1/interval_n<=1 --
--           @example local ok = dev:add(10,"test","\1\1\0\150\0\37\253\29")            --
--         dev:timeout(sec)设置下发数据的设备响应超时时间（秒）                         --
--           @param   sec        int      响应超时时间（秒）                            --
--                                        如果值为0，表示不检测设备响应超时             --
--           @return  无                                                                --
--           @example dev:timeout(3)                                                    --
--         dev:response()设备响应成功                                                   --
--           @param   无                                                                --
--           @return  无                                                                --
--           @example dev:response()                                                    --
--         dev:send(data)下发数据到设备                                                 --
--           @param   data   string   数据（二进制数据），使用lua转义字符串             --
--           @return  无                                                                --
--           @example dev:send("\2\2\0\150\0\37\206\89")                                --
------------------------------------------------------------------------------------------
function device_timer_init(dev)
    -- 添加用户自定义代码 --
    -- 例如： --
end

-----------------------------------------------------------------------------------------------------------
-- 解析设备上传数据                                                                                      --
-- @param  dev    user_data   设备管理器                                                                 --
-- @return size表示已解析设备上传数据的字节数，json表示解析后的数据点集合，格式如下：                    --
--         [                                                                                             --
--           {                                                                                           --
--             "i" : "dsname1",          // 数据流或数据流模板名称1                                      --
--             "a" : 1234567890,         // 毫秒级时间戳，距离（00:00:00 UTC, January 1, 1970）的毫秒    --
--                                       // 如果值为0，表示使用当前时间                                  --
--             "v" : 123 | "123" | {...} // 布尔值、数值、字符串、json                                   --
--             "b" : "0A0B0C0D..."       // 二进制数据（16进制字符串），与v互斥，不同时存在              --
--             "d" : xxx | "xxx" | {...} // 用于描述b（可选）；布尔值、数值、字符串、json                --
--             "c" : "authcode1"         // 用于标识数据点归属(设备AuthCode,可选)                        --
--                                       // 如果为“”或不存在，表示数据点归属建立TCP连接的设备            --
--           }                                                                                           --
--           ...                                                                                         --
--           {                                                                                           --
--             "i" : "dsnamen",          // 数据流或数据流模板名称1                                      --
--             "a" : 1234567890,         // 毫秒级时间戳，距离（00:00:00 UTC, January 1, 1970）的毫秒    --
--                                       // 如果值为0，表示使用当前时间                                  --
--             "v" : 123 | "123" | {...} // 布尔值、数值、字符串、json                                   --
--             "b" : "0A0B0C0D..."       // 二进制数据（16进制字符串），与v互斥，不同时存在              --
--             "d" : xxx | "xxx" | {...} // 用于描述b（可选）；布尔值、数值、字符串、json                --
--             "c" : "authcoden"         // 用于标识数据点归属(设备AuthCode,可选)                        --
--                                       // 如果为“”或不存在，表示数据点归属建立TCP连接的设备            --
--           }                                                                                           --
--         ]                                                                                             --
-- @notice 此函数为回调函数，不可在脚本内调用                                                            --
-- @readme dev提供一下几个函数：                                                                         --
--         dev:add(interval,name,data)添加定时下发数据                                                   --
--           @param   interval number   数据下发的时间间隔（秒）                                         --
--                    name     string   名称（须保证唯一性）                                             --
--                    data     string   数据（二进制数据），使用lua转义字符串                            --
--           @return  成功返回true，否则返回false                                                        --
--           @notice  定时数据下发的平均频率不超过1，及1/interval_1+...+1/interval_n<=1                  --
--           @example local ok = dev:add(10,"test","\1\1\0\150\0\37\253\29")                             --
--         dev:timeout(sec)设置下发数据的设备响应超时时间（秒）                                          --
--           @param   sec      int      响应超时时间（秒）                                               --
--                                      如果值为0，表示不检测设备响应超时                                --
--           @return  无                                                                                 --
--           @example dev:timeout(3)                                                                     --
--         dev:response()设备响应成功                                                                    --
--           @param   无                                                                                 --
--           @return  无                                                                                 --
--           @example dev:response()                                                                     --
--         dev:send(data)下发数据到设备                                                                  --
--           @param   data   string   数据（二进制数据），使用lua转义字符串                              --
--           @return  无                                                                                 --
--           @example dev:send("\2\2\0\150\0\37\206\89")                                                 --
--         dev:size()获取设备数据大小（字节数）                                                          --
--           @param   无                                                                                 --
--           @return  返回设备数据大小（字节数）                                                         --
--           @example local sz = dev:size()                                                              --
--         dev:byte(pos)获取pos对应位置的设备数据（字节）                                                --
--           @param   pos   number   指定的获取位置，取值范围[1,dev:size()+1)                            --
--           @return  成功返回设备数据（int），否则返回nil                                               --
--           @example local data = dev:byte(1)                                                           --
--         dev:bytes(pos,count)获取从pos开始，count个设备数据                                            --
--           @param   pos   number   指定的获取起始位置，取值范围[1,dev:size()+1)                        --
--                    count number   指定的获取数据总数，取值范围[0,dev:size()+1-pos]                    --
--           @return  成功返回设备数据（string），否则返回nil                                            --
--           @example local datas = dev:bytes(1,dev:size())                                              --
-----------------------------------------------------------------------------------------------------------
function device_data_analyze(dev)
    -- 获取设备发送的所有数据
    local data = dev:bytes(1, dev:size())

    -- 数据下发次数递增
    sendCount = sendCount + 1

    -- 构造新的报文格式
    local json = string.format([[
    {
      "device_id": "1",
      "packet_type": "confirm",
      "data": "server received data",
      "send_count": "%d"
    }
    ]], sendCount, timestamp)

    -- 发送报文
    dev:send(json)

    -- 为了演示，这里假设直接处理并返回全部接收到的数据字节大小
    -- 在实际应用中，可能需要根据数据内容或其他逻辑来决定返回的大小
    return dev:size(), "[]" -- 这里的返回值也根据实际情况进行调整
end


