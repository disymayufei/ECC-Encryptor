#include <iostream>
#include <string>
#include <stdio.h>

#include "AES.h"
#include "Base64.h"
#include "ECC_Calc.h"
#include "RdRand.h"

#include <stdio.h>
#include <bitset>

int runWithoutGuide(int argc, const char* argv[]) {

    ECC_Calc ecc;

    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-?") == 0) {
        std::cout << std::endl;
        std::cout << "[=========================帮助列表=========================]" << std::endl;
        std::cout << "-G：生成一个密钥对" << std::endl;
        std::cout << "-Key 公钥值 -M 明文消息：使用输入的公钥值加密输入的明文消息" << std::endl;
        std::cout << "-Key 私钥值 -EM 密文消息：使用输入的私钥值解密输入的密文消息" << std::endl;
        return 0;
    }
    else {
        std::string Key;
        std::string Mes;
        bool isEncrypt = false;

        for (int i = 1; i < argc; ++i) {
            if (strcmp(argv[i], "-G") == 0) {
                KeyPackage P = ecc.Gen_Keys();

                std::cout << std::endl;
                std::cout << "密钥对生成成功，请妥善保存：" << std::endl;
                std::cout << "私钥：" << P.Private_Key << std::endl;
                std::cout << "公钥：" << P.Public_Key << std::endl;
                return 0;
            }
            if ((std::string)argv[i] == "-Key") {
                if (i + 1 <= argc) {
                    Key = argv[i + 1];
                }
                else {
                    std::cout << "严重错误：不能接受一个空的Key值！" << std::endl;
                    return 1;
                }
            }
            if (strcmp(argv[i], "-M") == 0) {
                isEncrypt = true;
                if (i + 1 < argc) {
                    Mes = argv[i + 1];
                }
                else {
                    std::cout << "严重错误：不能接受一个空的消息值！" << std::endl;
                    return 1;
                }
            }
            if (strcmp(argv[i], "-EM") == 0) {
                if (i + 1 < argc) {
                    Mes = (std::string)argv[i + 1];
                }
                else {
                    std::cout << "严重错误：不能接受一个空的消息值！" << std::endl;
                    return 1;
                }
            }

            if (Mes != "" && Key != "") {
                break;
            }
        }

        if (isEncrypt) {
            if (Key.find("|") == Key.npos) {
                std::cout << "严重错误：输入的公钥格式错误，请重新生成一个密钥对！" << std::endl;
                return 1;
            }
            if (Mes == "") {
                std::cout << "严重错误：加密的信息不能为空！" << std::endl;
                return 1;
            }

            std::vector<G_point> E_Point = ecc.Plain_Encode(Mes);
            std::string E_Mes = ecc.Encrypt(Key, E_Point);
            std::cout << "加密结果是：" << E_Mes << std::endl;
            return 0;
        }
        else {
            if (Key.find("|") != Key.npos) {
                std::cout << "严重错误：输入的私钥格式错误，请重新生成一个密钥对！" << std::endl;
                return 1;
            }
            if (Mes == "") {
                std::cout << "严重错误：解密的信息不能为空！" << std::endl;
                return 1;
            }
            std::cout << "解密结果是：" << ecc.Decrypt(Key, Mes) << std::endl;
            return 0;
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc > 1) {
        return runWithoutGuide(argc, (const char**)argv);
    }
    std::cout << "[===================== ECC Encryptor =====================]" << std::endl;
    std::cout << "请选择你的身份，输入\"C\"代表C端，输入\"S\"代表S端" << std::endl;
    std::cout << "若第一次使用，不知道C端和S端的区别，请输入\"?\"来查看帮助" << std::endl;
    std::cout << std::endl;
    std::cout << "请输入身份：";

    std::string input_1;
    bool isServer = false;

    std::string AES_Password;
    std::string ECC_Public_Key;
    std::string temp; //阻塞用

    while (true) {
        std::cin >> input_1;
        std::cout << std::endl;
        if (input_1 == "C" || input_1 == "c") {
            std::cout << "您选择作为C端！" << std::endl;
            break;
        }
        else if (input_1 == "S" || input_1 == "s") {
            isServer = true;
            std::cout << "您选择作为S端！" << std::endl;
            break;
        }
        else if (input_1 == "?" || input_1 == "？") {
            std::cout << "[================= 帮助 =================]" << std::endl;
            std::cout << "S端相对较为主动，但需要比C端更高的性能" << std::endl;
            std::cout << "相应的，C端在交换密码时需要更多的等待对方" << std::endl;
            std::cout << "但不管如何，两方必须一个为C端，一个为S端！！" << std::endl;
            std::cout << "切记提前商量好，否则将出现不可预料的后果！" << std::endl;
        }
        else {
            std::cout << "您的输入有误，请重新输入！" << std::endl;
        }
        std::cout << "请输入身份：";
    }
    ECC_Calc ecc;
    if (isServer) {
        std::cout << "请等待Key的生成..." << std::endl;
        KeyPackage P = ecc.Gen_Keys();

        std::cout << std::endl;
        std::cout << "您的私钥是：" << P.Private_Key << std::endl;
        std::cout << "私钥是您最关键的数据，请无论如何不要告诉任何人！（包括C端）" << std::endl;
        std::cout << std::endl;
        std::cout << "您的公钥是：" << P.Public_Key << std::endl;
        std::cout << "公钥是对方所需要的，请通过一个相对安全的途径（如短信，QQ等）发给对方C端！" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "发送给C端后，请等待对方发回第一段加密密钥..." << std::endl;
        std::cout << "如果对方发回了，请粘贴在这里（QQ复制后常自带换行，请确认已经去掉了换行）：";
        
        std::string AES_Temp;

        std::cin >> AES_Temp;

        AES_Password = ecc.Decrypt(P.Private_Key, AES_Temp);
        std::cout << std::endl;
        
        std::cout << "握手步骤已顺利完成，可以进行加密通话啦！" << std::endl;

        std::cout << std::endl;
    }
    else {
        std::cout << "作为C端，你需要等待S端给您发送公钥！" << std::endl;
        std::cout << "如果对方发来了，请粘贴在这里（QQ复制后常自带换行，请确认已经去掉了换行）：";
        std::cin >> ECC_Public_Key;
        std::cout << std::endl;
        std::cout << "请等待加密密钥的生成..." << std::endl;
        AES_Password = Gen_AES_Key();
        std::string pwd = ecc.Encrypt(ECC_Public_Key, ecc.Plain_Encode(AES_Password));
        
        std::cout << "生成完毕！加密密钥是：" << pwd << std::endl;
        std::cout << std::endl;
        std::cout << "加密密钥是对方所需要的，请通过一个相对安全的途径（如短信，QQ等）发给对方S端！" << std::endl;
        std::cout << "在对方确认收到并且告知你屏幕显示\"OK\"后，在下方输入\"1\"并按回车继续..." << std::endl;
        std::cout << std::endl;
        std::cout << "输入确认数字：";
        std::cin >> temp;
        std::cout << std::endl;
        std::cout << "握手步骤已顺利完成，可以进行加密通话啦！" << std::endl;
        std::cout << std::endl;
    }

    std::string Mode;
    std::string Plain_Mes;
    std::string Encrypted_Mes;

    std::cout << "请选择消息模式:" << std::endl;
    std::cout << "如果是你要给对方发送消息，要进行加密，请输入\"E\"" << std::endl;
    std::cout << "如果是对方给你发送密文，你要进行解密，请输入\"D\"" << std::endl;
    std::cout << std::endl;

    std::cout << "输入你要选择的消息模式";

    while (true) {
        std::cout << "：";
        std::cin >> Mode;
        std::cin.ignore();
        
        std::cout << std::endl;

        if (Mode == "E" || Mode == "e") {
            std::cout << "输入你要发送的明文(若模式选择错误，请输入\"#Q\"来重新选择！)：";

            std::getline(std::cin, Plain_Mes);

            if (Plain_Mes == "#Q" || Plain_Mes == "#q") {
                continue;
            }
            else {
                std::cout << "密文是：" << AES_Encrypt(Plain_Mes, AES_Password) << std::endl;
                std::cout << "你可以发送给对方！" << std::endl;
                std::cout << std::endl;
            }
        }
        else if (Mode == "D" || Mode == "d") {
            std::cout << "输入你要解密的密文(若模式选择错误，请输入\"#Q\"来重新选择！)：";
            std::cin >> Encrypted_Mes;
            if (Encrypted_Mes == "#Q" || Encrypted_Mes == "#q") {
                continue;
            }
            else {
                std::cout << "对方消息的明文是：" << AES_Decrypt(Encrypted_Mes, AES_Password) << std::endl;
                std::cout << std::endl;
            }
        }
        else {
            std::cout << "输入的模式错误，请重新输入！" << std::endl;
            std::cout << std::endl;
        }
        std::cout << "输入你要选择的消息模式";
    }
}
