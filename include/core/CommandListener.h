/**
 * @description: 所有处理命令的类都应该实现的接口，用于解析和执行输入的命令
 * @author: Mamh
 * @email: mamhsl@163.com
 * @date: 2024/3/31 上午 09:45
 */

#ifndef COMMANDLISTENER_H
#define COMMANDLISTENER_H

class CommandListener {
public:
    // 析构函数应当是虚函数，特别是当类作为基类时
    virtual ~CommandListener() = default;

    /**
     * 应该先解析命令（parseCommand），然后再执行命令（dispatchCommand）
     * 解析命令的作用是辨认具体哪个部件执行命令，选择匹配具体的执行部件
     * @param command 上级分发来的命令
     * @return
     */
    virtual bool parseCommand(const String &command) = 0;

    /**
     * 对子命令进行处理和派遣，返回命令执行结果
     * @param command 待解析的命令
     * @param listener 监听器, 这个抽象为了接口类便于重用
     * @return
     */
    // 具体解析是哪个负责执行命令，派发给相应的监听器
    virtual bool dispatchCommand(String &command, const String &tag, CommandListener *listener) = 0;
};

#endif

