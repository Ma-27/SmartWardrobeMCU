----------------------------------------------------------------------- --
-- 用户需要自行完成一下2个函数的实现                                 --
-- 1、定时下发数据任务初始化函数：device_timer_init(dev)【可选】     --
-- 2、对设备上传数据进行解析（包括心跳等）：device_data_analyze(dev) --
-----------------------------------------------------------------------
dataPoint = {}
serverReturnDebug = ""
sendCount = 0
info = ""
flag = false
debug = ""
serverReturnDebug = ""
deviceID = ""
from = 0
packetType = ""

-------------------------------------------------------------------------------
-- 注册C函数
-- 参考old文档
-------------------------------------------------------------------------------

--------------------------------------------------------
-- 将bytes string转换hex string                       --
--------------------------------------------------------
function to_hex(s)
    local i
    local t

    t = { s:byte(1, s:len()) }
    for i = 1, #t do
        t[i] = string.format('%02X', t[i])
    end

    return table.concat(t)
end

-----------------------------------------------
-- 将object序列化成字符串                    --
-----------------------------------------------
function to_str(o)
    local i = 1
    local t = {}
    local f

    f = function(x)
        local y = type(x)
        if y == "number" then
            t[i] = x
            i = i + 1
        elseif y == "boolean" then
            t[i] = tostring(x)
            i = i + 1
        elseif y == "string" then
            t[i] = "\""
            t[i + 1] = x
            t[i + 2] = "\""
            i = i + 3
        elseif y == "table" then
            t[i] = "{"
            i = i + 1

            local z = true
            for k, v in pairs(x) do
                if z then
                    z = false
                    t[i] = "\""
                    t[i + 1] = k
                    t[i + 2] = "\""
                    t[i + 3] = ":"
                    i = i + 4
                    f(v)
                else
                    t[i] = ","
                    t[i + 1] = "\""
                    t[i + 2] = k
                    t[i + 3] = "\""
                    t[i + 4] = ":"
                    i = i + 5
                    f(v)
                end
            end

            t[i] = "}"
            i = i + 1
        else
            t[i] = "nil"
            i = i + 1
        end
    end
    f(o)

    return table.concat(t)
end

----------------------------------------------------------------------------------------------------------
-- 添加值数据点到table中                                                                                -- --
----------------------------------------------------------------------------------------------------------
function add_val(t, i, a, v, c)
    if type(t) ~= "table" then
        return false
    elseif type(i) ~= "string" then
        return false
    elseif type(a) ~= "number" then
        return false
    else
        local o = type(v)
        if o ~= "boolean" and o ~= "number" and o ~= "string" and o ~= "table" then
            return false
        end

        local n = { i = i, v = v }
        if a ~= 0 and a ~= nil then
            n["a"] = a
        end
        if c ~= nil then
            n["c"] = c
        end

        -- list push_back --
        if t.h == nil then
            t.h = { nil, n }
            t.t = t.h
        else
            t.t[1] = { nil, n }
            t.t = t.t[1]
        end
    end

    return true
end

----------------------------------------------------------------------------------------------------------
-- 添加二进制数据点到table中                                                                            -- --
----------------------------------------------------------------------------------------------------------
function add_bin(t, i, a, b, d)
    if type(t) ~= "table" then
        return false
    elseif type(i) ~= "string" then
        return false
    elseif type(a) ~= "number" then
        return false
    elseif type(b) ~= "string" then
        return false
    else
        local o = type(d)
        if o ~= "nil" and o ~= "string" and o ~= "table" then
            return false
        end

        local n = { i = i, b = to_hex(b) }
        if a ~= 0 and a ~= nil then
            n["a"] = a
        end
        if d ~= nil then
            n["d"] = d
        end
        if c ~= nil then
            n["c"] = c
        end

        -- list push_back --
        if t.h == nil then
            t.h = { nil, n }
            t.t = t.h
        else
            t.t[1] = { nil, n }
            t.t = t.t[1]
        end
    end

    return true
end

--------------------------------------------------------------
-- 将table序列化成json字符串                                --
-- @param   t   table   通过add_val、add_bin构建起来的table --
-- @return  返回序列化json字符串                            --
-- @example local json = to_json(t)                         --
--------------------------------------------------------------
function to_json(t)
    local i = 1
    local o = {}
    local n

    o[i] = "["
    i = i + 1
    n = t.h
    while n ~= nil do
        if n[2] ~= nil then
            o[i] = to_str(n[2])
            i = i + 1
        end

        n = n[1]
        if n ~= nil then
            o[i] = ","
            i = i + 1
        end
    end
    o[i] = "]"

    return table.concat(o)
end

------------------------------------
-- begin-添加用户自定义值或函数等 --
---------------------------
-- 解析可能嵌套的JSON字符串为Lua表
-- 支持最多三重嵌套，不支持数组
function parseJson(jsonStr)
    -- 去除字符串首尾的空格
    local function strip(str)
        return str:gsub("^%s*(.-)%s*$", "%1")
    end

    -- 解析单个值，可能是数字、布尔值、字符串或嵌套对象
    local function parseValue(val)
        val = strip(val)
        if val:sub(1, 1) == "{" then
            debug = debug .. "Entering nested object. the object have been parsed \n"  -- 进入嵌套对象的调试信息
            -- 注释掉的这行代码是错误的，之前通过括号匹配已经解析过了
            -- return parseObject(val)  -- 递归解析嵌套对象
            return val
        elseif tonumber(val) then
            debug = debug .. "Parsing number: " .. val .. "\n"  -- 解析数字的调试信息
            return tonumber(val)  -- 将字符串转换为数字
        elseif val == "true" then
            debug = debug .. "Parsing boolean true.\n"  -- 解析布尔值true的调试信息
            return true
        elseif val == "false" then
            debug = debug .. "Parsing boolean false.\n"  -- 解析布尔值false的调试信息
            return false
        elseif val:sub(1, 1) == '"' and val:sub(-1) == '"' then
            debug = debug .. "Parsing string: " .. val .. "\n"  -- 解析字符串的调试信息
            return val:sub(2, -2)  -- 去除字符串首尾的引号
        else
            debug = debug .. "Returning raw value: " .. val .. "\n"  -- 返回原始值的调试信息
            return val
        end
    end

    -- 解析对象，可能包含多个键值对
    local function parseObject(objStr)
        debug = debug .. "\nStarting to parse object.\n"  -- 开始解析对象的调试信息
        local obj = {}
        objStr = objStr:gsub("%s*([,:{}%[%]])%s*", "%1")  -- 去除结构性括号前后的空格
        objStr = objStr:gsub("^%s*{", ""):gsub("}%s*$", "")  -- 移除对象字符串首尾的大括号
        local key = nil
        local previousKey = nil
        local notInString = true  --如果 notInString 是 true,表示当前不在字符串内,这是json固有结构
        local start = 1
        local depth = 0

        for i = 1, #objStr do
            local char = objStr:sub(i, i)
            if char == '"' and (i == 1 or objStr:sub(i - 1, i - 1) ~= '\\') then
                --只有当一个双引号既是字符串的第一个字符，或者其前一个字符不是反斜线时，这个双引号才被视为标记字符串值开始或结束的符号。
                notInString = not notInString  -- 切换字符串内外状态
            elseif notInString then
                if char == '{' then
                    depth = depth + 1  -- 增加嵌套深度
                    if depth == 1 then
                        start = i
                    end
                elseif char == '}' then
                    if depth == 1 then
                        local subObjStr = strip(objStr:sub(start, i))  -- 提取嵌套对象的字符串
                        local nestedObj = parseObject(subObjStr)
                        obj[previousKey] = nestedObj  -- 将解析的嵌套对象赋值给对应的键
                        debug = debug .. "Exiting nested object with key: " .. previousKey .. "\n"
                        debug = debug .. "The nested object is: " .. to_str(nestedObj) .. "\n"
                    end
                    key = nil
                    depth = depth - 1  -- 减少嵌套深度
                elseif depth == 0 then
                    if char == ':' then
                        key = strip(objStr:sub(start + 1, i - 2))  -- 提取键名
                        ---检查嵌套的键值对，如果下一个真是嵌套键值对，那将前一个键名保存为前馈键名
                        if objStr:sub(i + 1, i + 1) == '{' then
                            previousKey = key
                        end
                        start = i + 1
                    elseif char == ',' then
                        if key then
                            local value = strip(objStr:sub(start, i - 1))  -- 提取键值
                            obj[key] = parseValue(value)  -- 将解析的值赋值给对应的键
                            key = nil --使用后置为空，避免检查key时错误更新
                        end
                        start = i + 1
                    end
                end
            end
        end

        if key then
            local value = strip(objStr:sub(start))
            obj[key] = parseValue(value)  -- 对最后一个键值对进行处理
        end

        debug = debug .. "Finished parsing object.\n"  -- 完成对象解析的调试信息
        debug = debug .. "The final object is: " .. to_str(obj) .. "\n"  -- 输出解析结果
        return obj
    end

    return parseObject(jsonStr), debug  -- 返回解析结果和调试信息
end

-- 构建全局通用响应报文的函数
function createConfirmResponse(deviceId, remark)
    return to_str({
        device_id = deviceId,
        from = "0", -- 通常这个字段表示消息从哪里发出，这里固定为"0"表示服务器
        packet_type = "Confirm",
        send_count = sendCount,
        data = "server received data",
        remark = remark
    })
end


-- 处理Ping报文
function handlePing(jsonData)
    local response = {
        device_id = jsonData["device_id"],
        from = "0",
        packet_type = "Pong",
        send_count = sendCount,
        data = jsonData["data"]
    }
    debug = debug .. "Pong response, is there an empty? : " .. to_str(response) .. "\n"  -- 输出Pong响应的调试信息
    return to_str(response) -- 转换为JSON字符串
end


-- 处理温湿度数据，包括存储数据等操作
function handleTemperatureHumidity(jsonData)
    local device_id = jsonData["device_id"]
    local temperature = jsonData["data"]["temperature"]
    local humidity = jsonData["data"]["humidity"]
    local measure_time = jsonData["data"]["measure_time"]

    local succeed = true  -- 默认假设所有操作都会成功

    -- 更新数据到云平台，只有当所有调用都返回true时，succeed才保持true
    succeed = succeed and add_val(dataPoint, "Humidity", 0, humidity)
    succeed = succeed and add_val(dataPoint, "Temperature", 0, temperature)
    succeed = succeed and add_val(dataPoint, "MeasureTime", 0, measure_time)

    -- 判断测试结果
    local remark = succeed and "temperature humidity data received" or "failed to parse data"

    -- 转换为JSON字符串并且回复
    return createConfirmResponse(device_id, remark)
end

function handleDeviceRegister(jsonData)
    -- 在这里处理设备注册逻辑，例如验证设备信息，注册设备到数据库等。
    -- 假设注册成功，并构建响应报文。

    local response = {
        device_id = jsonData["device_id"],
        from = "0",
        packet_type = "Register-Ack",
        config = {
            valid_interval = "300000", -- 示例配置信息，实际应根据需求设定
            status = "success"
        }
    }
    return to_str(response) -- 将Lua表转换为JSON字符串
end


-- 如果收到用户 App 等客户端更新数据的请求，则服务器需要向 Arduino 系统请求报文。
function handleRequestData(jsonData, dataType)
    -- 构建请求报文的内容
    local request = {
        device_id = jsonData["device_id"], -- 使用提供的设备ID
        from = 0, -- 报文来源标识（0代表服务器）
        packet_type = "Request-Data", -- 报文类型
        data_type = dataType or "", -- 请求的数据类型
        send_count = sendCount, -- 发送计数
        remark = jsonData["remark"] or ""  -- 备注信息，默认为空
    }

    -- 使用to_str函数将请求报文转换为JSON格式的字符串
    return to_str(request)
end


-- 构建表示错误的响应报文
function handleErrorPacket(deviceID, errorCode, errorMsg)
    local response = {
        device_id = deviceID,
        from = "0",
        packet_type = "Error",
        send_count = sendCount,
        error_code = errorCode,
        error_message = errorMsg
    }
    return to_str(response) -- 将Lua表转换为JSON字符串
end

-- 处理灯光状态数据，包括存储数据等操作
function handleLight(jsonData)
    local device_id = jsonData["device_id"]
    local brightness = jsonData["data"]["brightness"]
    local knob_value = jsonData["data"]["knob_value"]
    local light_openness = jsonData["data"]["light_openness"]
    local measure_time = jsonData["data"]["measure_time"]

    local succeed = true  -- 默认假设所有操作都会成功

    -- 更新数据到云平台，只有当所有调用都返回true时，succeed才保持true
    succeed = succeed and add_val(dataPoint, "Brightness", 0, brightness)
    succeed = succeed and add_val(dataPoint, "KnobValue", 0, knob_value)
    succeed = succeed and add_val(dataPoint, "LightOpenness", 0, light_openness)
    succeed = succeed and add_val(dataPoint, "MeasureTime", 0, measure_time)

    -- 判断测试结果
    local remark = succeed and "light data received" or "failed to parse light data"

    -- 转换为JSON字符串并且回复
    return createConfirmResponse(device_id, remark)
end


-- 筛选和处理报文的方法：这个是逐一匹配报文的，只能通过if-else硬添加
function handleRequest(jsonData)
    -- 获取报文类型
    local packetType = jsonData["packet_type"]
    local deviceID = jsonData["device_id"]

    -- 根据报文类型进行处理
    if packetType == "Ping" then
        -- 处理Ping报文
        return handlePing(jsonData)
    elseif packetType == "Temperature-Humidity" then
        -- 处理温湿度数据报文
        return handleTemperatureHumidity(jsonData)
    elseif packetType == "Log-In" then
        -- 处理设备登录
        return handleDeviceRegister(jsonData)
    elseif packetType == "Light" then
        -- 处理设备发送的灯光数据
        return handleLight(jsonData)
    else
        -- 未找到应当处理的报文类型，返回错误报文
        return handleErrorPacket(deviceID, 1001, "Invalid parameter in parsing packet_type.")
    end
end


-- end-添加用户自定义值或函数等   --
------------------------------------

------------------------------------------------------------------------------------------
-- 设置定时下发设备的数据（可选）                                                       -- --
------------------------------------------------------------------------------------------
function device_timer_init(dev)
    -- 添加用户自定义代码 --
    -- 例如： --
    dev:timeout(1)
end

-----------------------------------------------------------------------------------------------------------
-- 解析设备上传数据                                                                                      -- --
-----------------------------------------------------------------------------------------------------------
function device_data_analyze(dev)
    -- TODO handleRequestData 方法应该由其他设备调用，这里虽然给出了默认的回复实现方法，但是没有真正用到它

    -- 获取设备发送的所有数据
    local data = dev:bytes(1, dev:size())

    -- 数据下发次数递增
    sendCount = sendCount or 0 -- 如果sendCount未初始化，则初始化为0
    sendCount = sendCount + 1

    -- 去掉数据首尾的空格
    data = data:gsub("^%s*(.-)%s*$", "%1")

    -- 检查数据是否看似有效的JSON格式（简单检查）
    if string.sub(data, 1, 1) == "{" and string.sub(data, -1) == "}" then
        -- 如果数据格式看起来像JSON，则尝试解析JSON
        local jsonData = parseJson(data)
        -- 筛选和处理报文
        local response = handleRequest(jsonData)

        -- 将调试信息发送给设备
        --dev:send(debug)

        -- 将正式的响应报文发送给设备
        dev:send(response)
    else
        -- 如果数据不符合基本的JSON格式，发送错误信息报文
        local errorJson = handleErrorPacket(0, 1000, "Invalid JSON format.")
        dev:send(errorJson)
    end

    debug = ""
    dev:response()
    return dev:size(), to_json(dataPoint)
end
