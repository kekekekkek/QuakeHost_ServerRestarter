#include "Include.h"

string ReadValue(string strBuffer, string strValue)
{
	string strRead = "";

	int iIndexOf = 0;
	bool bCanRead = false;

	if (strBuffer.empty()
		|| strValue.empty())
		return strRead;

	iIndexOf = strBuffer.find(strValue);

	if (iIndexOf != string::npos)
	{
		for (int i = (iIndexOf + strValue.length()); i < strBuffer.length(); i++)
		{
			if (bCanRead)
			{
				if (strBuffer[i] == ';'
					|| strBuffer[i] == '&'
					|| strBuffer[i] == ' ')
					break;

				strRead += strBuffer[i];
			}

			if (strBuffer[i] == '=')
			{
				bCanRead = true;
				continue;
			}
		}
	}

	return strRead;
}

int IsServerAlive(string strIPv4)
{
	int iStatus = 0;

	HINTERNET hUrl = NULL;
	HINTERNET hOpen = InternetOpenA(NULL, NULL, NULL, NULL, NULL);

	if (hOpen)
	{
		char chResult[3120];
		DWORD dwBytes = NULL;

		hUrl = InternetOpenUrlA(hOpen, ("https://sourcequery.yepoleb.at/query?server=" + strIPv4).c_str(), NULL, NULL, NULL, NULL);

		if (InternetReadFile(hUrl, chResult, sizeof(chResult), &dwBytes))
		{
			//Даже если игрок поставит аналогичное имя, вроде бы эта проверка не срабатывает
			if (string(chResult).find("Server did not respond.") != string::npos)
				iStatus = 0;
			else
				iStatus = 1;
		}
		else
			iStatus = -1;
	}
	else
		iStatus = -1;

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hOpen);

	return iStatus;
}

DWORD QuakeHost_RestartServer(string strLogin, string strPassword, string strIPv4)
{
	DWORD dwResult = NULL;

	HINTERNET hUrl = NULL;
	HINTERNET hData = NULL;
	HINTERNET hRequest = NULL;

	HINTERNET hOpen = InternetOpenA(NULL, NULL, NULL, NULL, INTERNET_OPEN_TYPE_DIRECT);
	HINTERNET hConnect = InternetConnectA(hOpen, "quakehost.ru", INTERNET_DEFAULT_HTTP_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);

	string strData = ("login=" + strLogin + "&password=" + strPassword);
	string strHeader = "Content-Type: application/x-www-form-urlencoded\r\n";

	char chCookie[3120];
	DWORD dwCookie = sizeof(chCookie);

	if (hOpen && hConnect)
	{
		hRequest = HttpOpenRequestA(hConnect, "POST", "/login.php", NULL, NULL, NULL, NULL, NULL);
		HttpAddRequestHeadersA(hRequest, strHeader.c_str(), strHeader.length(), (HTTP_ADDREQ_FLAG_REPLACE | HTTP_ADDREQ_FLAG_ADD));

		if (HttpSendRequestA(hRequest, NULL, NULL, const_cast<char*>(strData.c_str()), strData.length()))
		{
			if (InternetGetCookieA("http://quakehost.ru/login.php", NULL, chCookie, &dwCookie))
			{
				hData = InternetOpenUrlA(hOpen, "http://quakehost.ru/cp", NULL, NULL, NULL, NULL);

				string strSid = "";
				string strMid = ReadValue(chCookie, "mid");
				string strPHash = ReadValue(chCookie, "phash");

				while (InternetReadFile(hData, chCookie, dwCookie, &dwCookie))
				{
					strSid = ReadValue(chCookie, "sid");

					if (!strSid.empty())
						break;
				}

				if (strMid.empty()
					|| strPHash.empty()
					|| strSid.empty())
					dwResult = 2;
				else
				{
					string strRequestUrl = ("http://" + strIPv4 + "/qhcpv3/functions/process.php?mid=" + strMid + "&phash=" + strPHash + "&sid=" + strSid + "&act=restart");
					hUrl = InternetOpenUrlA(hOpen, strRequestUrl.c_str(), NULL, NULL, NULL, NULL);

					if (hUrl)
						dwResult = 1;
				}
			}
			else
				dwResult = GetLastError();
		}
		else
			dwResult = GetLastError();
	}
	else
		dwResult = GetLastError();

	InternetCloseHandle(hUrl);
	InternetCloseHandle(hOpen);
	InternetCloseHandle(hData);
	InternetCloseHandle(hConnect);
	InternetCloseHandle(hRequest);

	return dwResult;
}

string GetFormatedTime()
{
	string strDate = "";

	SYSTEMTIME stSysTime;
	GetLocalTime(&stSysTime);

	if (to_string(stSysTime.wDay).length() < 2)
		strDate += "0";

	strDate += (to_string(stSysTime.wDay) + ".");

	if (to_string(stSysTime.wMonth).length() < 2)
		strDate += "0";

	strDate += (to_string(stSysTime.wMonth) + ".");
	strDate += (to_string(stSysTime.wYear) + " ");

	if (to_string(stSysTime.wHour).length() < 2)
		strDate += "0";

	strDate += (to_string(stSysTime.wHour) + ":");

	if (to_string(stSysTime.wMinute).length() < 2)
		strDate += "0";

	strDate += (to_string(stSysTime.wMinute) + ":");

	if (to_string(stSysTime.wSecond).length() < 2)
		strDate += "0";

	strDate += to_string(stSysTime.wSecond);
	return strDate;
}

string GetProcName(bool bExt)
{
	char chProcName[500];
	string strProcName = "";

	GetModuleFileNameA(GetModuleHandleA(NULL), chProcName, sizeof(chProcName));
	strProcName = chProcName;

	return (bExt ? strProcName.erase(0, (strProcName.find_last_of("\\") + 1)) : strProcName.erase(0, (strProcName.find_last_of("\\") + 1)).erase(strProcName.length() - 4, strProcName.length()));
}

int main(int iArg, char** chArg)
{
	setlocale(LC_ALL, "");
	SetConsoleTitleA("QuakeHost - Server Restarter by kek");

	if (iArg < 5)
	{
		cout << "---------- Ошибка ----------\nОдин или несколько аргументов были пропущены. Требуемые аргументы: \n";
		cout << "- IPv4 сервера (без порта);\n- Логин от профиля \"quakehost\";\n- Пароль от профиля \"quakehost\";\n- Интервал запросов для проверки соединения (в секундах).\n\n";
		cout << "Пример 1: " << GetProcName(false) << " 46.174.55.192 MyLogin MyPassword 15\n";
		cout << "Пример 2: " << GetProcName(false) << " 46.174.55.192 \"My Login\" \"My Password\" 15\n";
		cout << "----------------------------";
	}
	else
	{
		if (iArg > 5)
			cout << "---------- Ошибка ----------\nБыло передано большее количество аргументов, чем ожидалось!\n";
		else
		{
			string* strData = new string[iArg];

			for (int i = 0; i < iArg; i++)
				strData[i] = chArg[i];

			cout << "Нажмите комбинацию клавиш \"Ctrl + C\" чтобы выйти из процесса.\n";
			cout << "---------- Статус ----------\n[" << GetFormatedTime() << "] Проверка работоспособности сервера была запущена...\n";

			while (true)
			{
				if (IsServerAlive(strData[1]) == SERVER_STATUS_DEAD)
				{
					cout << "[" << GetFormatedTime() << "] Сервер \"" << strData[1] << "\" не отвечает. Запущена попытка перезапуска сервера...\n";
					DWORD dwResult = QuakeHost_RestartServer(strData[2], strData[3], strData[1]);

					if (dwResult == 1)
						cout << "[" << GetFormatedTime() << "] Сервер был перезапущен! Следующая проверка произойдёт через \"" << strData[4] << "\" секунд.\n";
					else
					{
						if (dwResult == 2)
						{
							cout << "Не удалось получить один из параметров куки во время парсинга данных! Убедитесь в правильности передачи аргументов во время запуска программы.\n";
							break;
						}

						if (dwResult > 2)
						{
							cout << "Не удалось отправить запрос на перезапуск сервера. Ошибка: " << dwResult << ". Обратитесь к официальной документации Microsoft (https://learn.microsoft.com/en-us/windows/win32/debug/system-error-codes), чтобы определить причину её появления.\n";
							break;
						}
					}
				}
				else
					cout << "[" << GetFormatedTime() << "] Сервер работает стабильно! Следующая проверка произойдёт через \"" << strData[4] << "\" секунд.\n";

				Sleep((atoi(strData[4].c_str()) * 1000));
			}
		}
	}

	cout << "----------------------------\n";

	system("pause");
	return 0;
}