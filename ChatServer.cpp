// ChatServer.cpp : アプリケーションのエントリ ポイントを定義します。
//
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "framework.h"
#include "ChatServer.h"
#include "resource.h"
#include "iostream"
#include <string>
#include <vector>
#include <regex>

#pragma comment( lib, "ws2_32.lib" )


// グローバル変数
HINSTANCE hInst;
HWND hMessageEdit;
HWND hSendMessageEdit;
HWND hIpAddressEdit;
HWND hPortEdit;
HWND hSendNameEdit;

const unsigned short SERVERPORT = 8080;
const UINT_PTR TIMERID = 1000;

//変数
bool join = false;      //入室しているのか

//リスト（要素数＝参加最大人数）
std::vector<std::string> addr(10);
std::vector<std::string> ports(10);

std::string message;   // チャット欄にセットする文字列
SOCKET sock;


// ダイアログプロシージャ
BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);
BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp);

// このコード モジュールに含まれる関数の宣言を転送します:
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int  nCmdShow)
{
    hInst = hInstance;

    //   if (DialogBox(hInstance, MAKEINTRESOURCE(IDD_BINDDIALOG), NULL, (DLGPROC)BindDlgProc) == IDOK)
    {
        // ダイアログ
        DialogBox(hInstance, MAKEINTRESOURCE(IDD_CHATSERVERDIALOG), NULL, (DLGPROC)DlgProc);
    }

    return (int)0;
}

BOOL CALLBACK BindDlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDOK:
            // 送信メッセージを取得

            EndDialog(hDlg, IDOK);
            return TRUE;
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }


        return FALSE;
    }

    return FALSE;
}

BOOL CALLBACK DlgProc(HWND hDlg, UINT msg, WPARAM wp, LPARAM lp)
{
    char buff[1024];            //文字数
    char ipAddr[256];           //アドレス字数
    char sendIpAddr[256];       //返信先アドレス
    char name[16];              //名前の字数

    std::string str;
    std::regex re("N0NZ3ypzgRzm");
    std::smatch m;

    u_short port;
    char portstr[256];
    u_long arg = 0x01;

    int ret;
    int ret2;

    SOCKADDR_IN fromAddr;
    int fromlen = sizeof(fromAddr);

    SOCKADDR_IN toAddr;
    int tolen = sizeof(toAddr);

    switch (msg)
    {
    case WM_INITDIALOG:
        // 各コントロールのハンドル取得
        hMessageEdit = GetDlgItem(hDlg, IDC_MESSAGEEDIT);
        hSendMessageEdit = GetDlgItem(hDlg, IDC_EDIT2);
        hIpAddressEdit = GetDlgItem(hDlg, IDC_IPADDRESS1);
        hPortEdit = GetDlgItem(hDlg, IDC_PORTEDIT);

        // WinSock初期化
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

        // ソケット作成
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock > 0)
        {

        }

        // ノンブロッキングソケットに設定
        ioctlsocket(sock, FIONBIO, &arg);

        // bind
        SOCKADDR_IN bindAddr;
        memset(&bindAddr, 0, sizeof(bindAddr));
        bindAddr.sin_family = AF_INET;
        bindAddr.sin_port = htons(SERVERPORT);
        bindAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// すべての自分のNICが対象
        if (ret = bind(sock, (SOCKADDR*)&bindAddr, sizeof(bindAddr)) == 0)
        {
        }



        // タイマーセット
        SetTimer(hDlg, TIMERID, 100, NULL);

        return TRUE;
    case WM_TIMER:
        // 受信
        ret = recvfrom(sock, (char*)buff, sizeof(buff), 0, (SOCKADDR*)&fromAddr, &fromlen);
        ret2 = recvfrom(sock, (char*)name, sizeof(name), 0, (SOCKADDR*)&fromAddr, &fromlen);

        //参加した人のアドレスとポート番号の変換
        inet_ntop(AF_INET, &fromAddr.sin_addr, ipAddr, sizeof(ipAddr));
        sprintf_s(portstr, "%d", ntohs(fromAddr.sin_port));

        if (ret < 0 || ret2 < 0)
        {
            if (WSAGetLastError() != WSAEWOULDBLOCK)
            {
                // エラー

            }
            return TRUE;
        }
        else
        {
            //配列内の全部を確認する
            for (int i = 0; i < addr.size(); i++)
            {
                //新しい人は最後にいるため
                //addrの途中の人は大丈夫
                if (addr[i] != ipAddr)
                {
                    join = false;
                }
                else
                {
                    join = true;
                    break;
                }
            }

            //もし一回も入ったことのない人だったら
            if (join == false)
            {
                //リストの末尾への追加
                addr.push_back(ipAddr);
                ports.push_back(portstr);

                //初めての参加のため〇〇が入室しましたと表示
                message.append(name);
                message.append("が入室しました");
                message.append("\r\n");

                char buff[1024] = "ADRcsTswm20Okl";
                for (int i = 0; i < addr.size(); i++)
                {
                    if (addr[i] == ipAddr)
                    {
                        // IPアドレスの取得
                        std::string addressStr = addr[i];
                        if (addressStr.size() < 16)
                        {
                            std::char_traits<char>::copy(sendIpAddr, addressStr.c_str(), addressStr.size() + 1);
                        }
                        // 宛先のポート番号の取得
                        std::string pStr = ports[i];        //配列の文字列を変数に入れる
                        port = atoi(pStr.c_str());     //入れた変数を整数に変換してintの変数に入れる

                        memset(&toAddr, 0, sizeof(toAddr));
                        toAddr.sin_family = AF_INET;
                        inet_pton(AF_INET, sendIpAddr, &toAddr.sin_addr.s_addr);
                        toAddr.sin_port = htons(port);
                        ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);
                        ret2 = sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);
                    }
                }

                //文字の表示
                SetWindowTextA(hMessageEdit, message.c_str());

                //入室してきた人のIPアドレスとポート番号を表示
                SetWindowTextA(hIpAddressEdit, ipAddr);
                SetWindowTextA(hPortEdit, portstr);
            }
            else
            {
                str = buff;
                //文字検索
                if (std::regex_search(str, m, re))
                {
                    //〇〇が退出しましたと表示
                    message.append(name);
                    message.append("が退出しました");
                    message.append("\r\n");
                    //文字の表示
                    SetWindowTextA(hMessageEdit, message.c_str());

                    for (int i = 0; i < addr.size(); i++)
                    {
                        if (addr[i] != ipAddr)
                        {
                            // IPアドレスの取得
                            std::string addressStr = addr[i];
                            if (addressStr.size() < 16)
                            {
                                std::char_traits<char>::copy(sendIpAddr, addressStr.c_str(), addressStr.size() + 1);
                            }
                            // 宛先のポート番号の取得
                            std::string pStr = ports[i];        //配列の文字列を変数に入れる
                            port = atoi(pStr.c_str());     //入れた変数を整数に変換してintの変数に入れる

                            memset(&toAddr, 0, sizeof(toAddr));
                            toAddr.sin_family = AF_INET;
                            inet_pton(AF_INET, sendIpAddr, &toAddr.sin_addr.s_addr);
                            toAddr.sin_port = htons(port);
                            ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);
                            ret2 = sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);
                        }
                    }

                    //退出した人のアドレスとポート番号の削除
                    auto Address = std::find(addr.begin(), addr.end(), ipAddr);
                    addr.erase(Address);
                    auto PortNumber = std::find(ports.begin(), ports.end(), portstr);
                    ports.erase(PortNumber);
                }
                else
                {
                    //↓送信されてきたメッセージを表示する
                    message.append(name);
                    message.append(":");
                    message.append(buff);
                    message.append("\r\n");
                    //チャット欄に文字列セット
                    SetWindowTextA(hMessageEdit, message.c_str());

                    for (int i = 0; i < addr.size(); i++)
                    {
                        if (addr[i] != ipAddr)
                        {
                            // IPアドレスの取得
                            std::string addressStr = addr[i];
                            if (addressStr.size() < 16)
                            {
                                std::char_traits<char>::copy(sendIpAddr, addressStr.c_str(), addressStr.size() + 1);
                            }
                            // 宛先のポート番号の取得
                            std::string pStr = ports[i];        //配列の文字列を変数に入れる
                            port = atoi(pStr.c_str());     //入れた変数を整数に変換してintの変数に入れる
                     
                            memset(&toAddr, 0, sizeof(toAddr));
                            toAddr.sin_family = AF_INET;
                            inet_pton(AF_INET, sendIpAddr, &toAddr.sin_addr.s_addr);
                            toAddr.sin_port = htons(port);
                            ret = sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);
                            ret2 = sendto(sock, name, sizeof(name), 0, (SOCKADDR*)&toAddr, tolen);
                        }
                    }
                }
                
            }
            
        }
        //else
        //{
        //     受信データがあれば、チャット欄に追加
        //    message.append((name));
        //    message.append(" : ");
        //    message.append((buff));
        //    message.append("-- from:");
        //    inet_ntop(AF_INET, &fromAddr.sin_addr, ipAddr, sizeof(ipAddr));
        //    message.append(ipAddr);
        //    message.append(":");
        //    sprintf_s(portstr, "%d", ntohs(fromAddr.sin_port));
        //    message.append(portstr);
        //    message.append("\r\n");
        //    SetWindowTextA(hMessageEdit, message.c_str());
        //    SetWindowTextA(hIpAddressEdit, ipAddr);//IPアドレスを送信元から受け取りセットする
        //    SetWindowTextA(hPortEdit, portstr);//	ポート番号	〃
        //}
        return TRUE;
    case WM_COMMAND:
        switch (LOWORD(wp))
        {
            // 送信ボタン押下時
        case IDC_SENDBUTTON:
            // 送信メッセージを取得
            GetWindowTextA(hSendMessageEdit, buff, 1024);

            // 宛先IPアドレスの取得
            GetWindowTextA(hIpAddressEdit, ipAddr, 256);
            // 宛先のポート番号の取得
            port = GetDlgItemInt(hDlg, IDC_PORTEDIT, FALSE, FALSE);

            memset(&toAddr, 0, sizeof(toAddr));
            toAddr.sin_family = AF_INET;
            inet_pton(AF_INET, ipAddr, &toAddr.sin_addr.s_addr);
            toAddr.sin_port = htons(port);
            sendto(sock, buff, sizeof(buff), 0, (SOCKADDR*)&toAddr, tolen);

            // buffをチャット欄に追加
            message.append(buff);
            message.append("\r\n");

            // チャット欄に文字列セット
            SetWindowTextA(hMessageEdit, message.c_str());

            // 送信メッセージ入力欄をクリア
            SetWindowTextA(hSendMessageEdit, "");

            return TRUE;
        case IDCANCEL:
            KillTimer(hDlg, TIMERID);
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        return FALSE;
    }

    return FALSE;
}
