#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <odbcinst.h>
#include <stdio.h>
#include <iostream>
//#include <stdio.h>
//#include <Windows.h>
#include <Winreg.h>
#include <tchar.h>
#include <string.h>
#include <atlstr.h>
#include <ctime> 
//#define _UNICODE
#define DIV 1048576000
#define WIDTH 7
#define BUFFER 8192
#define SETUP 0

using namespace std;

//Registry 
void GetCPUName(TCHAR* cpu_value);
void GetGraphicName(TCHAR* gpu_value);
void GetMemorySize();
void GetMain_M_Name(TCHAR* mainboard_m_value);
void GetMainName(TCHAR* mainboard_value);
void GetOSName(TCHAR* os_value);
void GetComputerName(TCHAR* comname_value);
void DbCotrol();
void reset_hStmt();

TCHAR os_value[40];
TCHAR comname_value[40];
TCHAR cpu_value[50];
TCHAR gpu_value[40];
TCHAR mainboard_m_value[40];
TCHAR mainboard_value[60];
DWORDLONG memory_value;


SQLHENV hEnv;
SQLHDBC hDbc;
SQLHSTMT hStmt;
SQLRETURN ret = SQL_SUCCESS;


RETCODE retcd;
SQLCHAR ComName[40];
SQLLEN nName = 0;


int main()
{

	GetOSName(os_value);
	GetComputerName(comname_value);
	GetCPUName(cpu_value);
	GetGraphicName(gpu_value);
	GetMemorySize();
	GetMain_M_Name(mainboard_m_value);
	GetMainName(mainboard_value);
	DbCotrol();
	//Direct();
}

void DbCotrol()
{
	SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);
	ret = SQLConnect(hDbc, (SQLCHAR *)"after", SQL_NTS, (SQLCHAR *) "uhs123", SQL_NTS, (SQLCHAR *)"uhs123", SQL_NTS);
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) {
		cout << "Connection:fail\n" << ret;
	}
	else
	{
		cout << "Connection:success\n";
	}
	
		TCHAR select_query[500];

	wsprintf(select_query, _T("SELECT com_name FROM system_default_info WHERE com_name = \'%s\'"), comname_value);

	RETCODE retcd = SQLExecDirect(hStmt, (SQLCHAR*)select_query, SQL_NTS);


	SQLCHAR comname[40];
	SQLLEN nName = 0;

	for (int i = 0; i<sizeof(comname); i++)
	{
		comname[0] = '\0';
	}
	if (retcd == SQL_SUCCESS || retcd == SQL_SUCCESS_WITH_INFO)
	{
		SQLBindCol(hStmt, 1, SQL_C_CHAR, &comname, sizeof(comname), &nName);
		retcd = SQLFetch(hStmt);


		if (comname[0] == '\0')
		{
			reset_hStmt();
			TCHAR insert_query[500];

			wsprintf(insert_query, _T("INSERT INTO system_default_info(os_version,com_name,cpu,gpu,mainboard_m,mainboard,memory,first_date,update_flag,C_cpu,C_gpu,C_mainboard,C_memory) VALUES (\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,getdate(),'N',%d,%d,%d,%d)"), os_value, comname_value, cpu_value, gpu_value, mainboard_m_value, mainboard_value, memory_value, SETUP, SETUP, SETUP, SETUP);
			retcd = SQLExecDirect(hStmt, (SQLCHAR*)insert_query, SQL_NTS);


		}
		else
		{
			reset_hStmt();

			TCHAR log_query[500]; 
				wsprintf(log_query, _T("INSERT INTO system_update_info(os_version,com_name,cpu,gpu,mainboard_m,mainboard,memory,update_date) VALUES (\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',\'%s\',%d,getdate())"), os_value, comname_value, cpu_value, gpu_value, mainboard_m_value, mainboard_value, memory_value);
			retcd = SQLExecDirect(hStmt, (SQLCHAR*)log_query, SQL_NTS);

			for (int i = 0; i < 5; i++)
			{
				switch (i)
				{
					TCHAR Compare_cpu[500];
					TCHAR Compare_gpu[500];
					TCHAR Compare_mainboard[500];
					TCHAR Compare_memory[500];


				case 0:  //cpu
					reset_hStmt();
					wsprintf(Compare_cpu, _T("UPDATE system_default_info SET cpu=\'%s\',C_cpu=C_cpu+1 WHERE (com_name=\'%s\' AND cpu !=(SELECT TOP 1 cpu FROM system_update_info WHERE com_name = \'%s\' ORDER BY update_date DESC))"), cpu_value, comname_value, comname_value);
					retcd = SQLExecDirect(hStmt, (SQLCHAR*)Compare_cpu, SQL_NTS);
					break;

				case 1: //gpu
					reset_hStmt();
					wsprintf(Compare_gpu, _T("UPDATE system_default_info SET gpu=\'%s\',C_gpu=C_gpu+1 WHERE (com_name=\'%s\' AND gpu !=(SELECT TOP 1 gpu FROM system_update_info WHERE com_name = \'%s\' ORDER BY update_date DESC))"), gpu_value, comname_value, comname_value);
					retcd = SQLExecDirect(hStmt, (SQLCHAR*)Compare_gpu, SQL_NTS);
					break;

				case 2: //mainboard
					reset_hStmt();
					//TCHAR Compare_query[500];
					wsprintf(Compare_mainboard, _T("UPDATE system_default_info SET mainboard = \'%s\',C_mainboard=C_mainboard+1 WHERE (com_name=\'%s\' AND mainboard !=(SELECT TOP 1 mainboard FROM system_update_info WHERE com_name = \'%s\' ORDER BY update_date DESC))"), mainboard_value, comname_value, comname_value);
					retcd = SQLExecDirect(hStmt, (SQLCHAR*)Compare_mainboard, SQL_NTS);
					break;

				case 3: //memory
					reset_hStmt();
					wsprintf(Compare_memory, _T("UPDATE system_default_info SET memory='%c' ,C_memory=C_memory+1 WHERE (com_name= \'%s\' AND memory !=(SELECT TOP 1 memory FROM system_update_info WHERE com_name = \'%s\' ORDER BY update_date DESC))"), memory_value, comname_value, comname_value);
					retcd = SQLExecDirect(hStmt, (SQLCHAR*)Compare_memory, SQL_NTS);
					break;
				case 4:
					TCHAR update_query[500];
					wsprintf(update_query, _T("UPDATE system_default_info SET update_date = getdate(), update_flag = 'Y' WHERE com_name= \'%s\'"), comname);
					retcd = SQLExecDirect(hStmt, (SQLCHAR*)update_query, SQL_NTS);
					break;
				default:
					break;
				}
			}
		}
	}

	printf("\n\nEnd\n\n");
}


void GetOSName(TCHAR* os_value)//void GetOSName()
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_OS_Reg, Read_OS_Reg;


	Open_OS_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
		&hKey);
	if (Open_OS_Reg == ERROR_SUCCESS)
	{

		Read_OS_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"),
			_T("ProductName"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)os_value,//(PVOID)&os_value,
			&BufferSize);

		if (Read_OS_Reg == ERROR_SUCCESS)
		{
			
			_tprintf(TEXT("OS version        : %s \n"), os_value);
		}
		else
			_tprintf(TEXT("fail"));
	}


	RegCloseKey(hKey);

}
void GetComputerName(TCHAR* comname_value)
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_ComName_Reg, Read_ComName_Reg;
	Open_ComName_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName"),
		&hKey);
	if (Open_ComName_Reg == ERROR_SUCCESS)
	{
		Read_ComName_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("SYSTEM\\CurrentControlSet\\Control\\ComputerName\\ActiveComputerName"),
			_T("ComputerName"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)comname_value,
			&BufferSize);

		if (Read_ComName_Reg == ERROR_SUCCESS)
		{
			_tprintf(TEXT("computer name     : %s \n"), comname_value);
		}
		else
			_tprintf(TEXT("fail"));


	}
	RegCloseKey(hKey);

}
void GetGraphicName(TCHAR* gpu_value)
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_GPU_Reg, Read_GPU_Reg;
	
	Open_GPU_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000\\Settings"),
		&hKey);
	if (Open_GPU_Reg == ERROR_SUCCESS)
	{
		Read_GPU_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("SYSTEM\\ControlSet001\\Control\\Class\\{4D36E968-E325-11CE-BFC1-08002BE10318}\\0000\\Settings"),
			_T("Device Description"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)gpu_value,
			&BufferSize);


		if (Read_GPU_Reg == ERROR_SUCCESS)
		{
			
			_tprintf(TEXT("GPU card    : %s \n"), gpu_value);
		}
		else
			_tprintf(TEXT("fail"));


	}
	RegCloseKey(hKey);
}
void GetCPUName(TCHAR* cpu_value)
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_CPU_Reg, Read_CPU_Reg;
	Open_CPU_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
		&hKey);
	if (Open_CPU_Reg == ERROR_SUCCESS)
	{
		Read_CPU_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
			_T("ProcessorNameString"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)cpu_value,
			&BufferSize);

		if (Read_CPU_Reg == ERROR_SUCCESS)
		{
			_tprintf(TEXT("     CPU       : %s \n"), cpu_value);
		}
		else
			_tprintf(TEXT("fail"));
	}
	RegCloseKey(hKey);

}
void GetMemorySize()
{
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);

	_tprintf(TEXT("Memory size   : %*I64d  GB \n"), WIDTH, statex.ullTotalPhys / DIV);

	memory_value = statex.ullTotalPhys / DIV;

}
void GetMain_M_Name(TCHAR* mainboard_m_value)
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_MainboardF_Reg, Read_MainboardF_Reg;
	Open_MainboardF_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("HARDWARE\\DESCRIPTION\\System\\BIOS"),
		&hKey);
	if (Open_MainboardF_Reg == ERROR_SUCCESS)
	{
		Read_MainboardF_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("HARDWARE\\DESCRIPTION\\System\\BIOS"),
			_T("BaseBoardManufacturer"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)mainboard_m_value,
			&BufferSize);

		if (Read_MainboardF_Reg == ERROR_SUCCESS)
		{
			_tprintf(TEXT("manufact of mainboard : %s \n"), mainboard_m_value);
		}
		else
			_tprintf(TEXT("fail"));

	}
	RegCloseKey(hKey);
}
void GetMainName(TCHAR* mainboard_value)
{
	HKEY hKey;

	DWORD BufferSize = BUFFER;
	LONG Open_Mainboard_Reg, Read_Mainboard_Reg;
	Open_Mainboard_Reg = RegOpenKey(HKEY_LOCAL_MACHINE,
		TEXT("HARDWARE\\DESCRIPTION\\System\\BIOS"),
		&hKey);
	if (Open_Mainboard_Reg == ERROR_SUCCESS)
	{
		Read_Mainboard_Reg = RegGetValue(HKEY_LOCAL_MACHINE,
			_T("HARDWARE\\DESCRIPTION\\System\\BIOS"),
			_T("BaseBoardProduct"),
			RRF_RT_REG_SZ,
			NULL,
			(PVOID)mainboard_value,
			&BufferSize);

		if (Read_Mainboard_Reg == ERROR_SUCCESS)
		{
			_tprintf(TEXT("mainboard name : %s \n"), mainboard_value);
		}
		else
			_tprintf(TEXT("fail"));

	}
	RegCloseKey(hKey);
}
void reset_hStmt()
{
	SQLCloseCursor(hStmt); 
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	hStmt = NULL;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt); 
}