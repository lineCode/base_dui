#include "stdafx.h"
#include "login_form.h"

#include <fstream>
#include "../third_party/libqr/qr.h"

bool LoginForm::GenerateQRCode()
{
	int errcode = QR_ERR_NONE;
	//qrInit��5�������ֱ���version,mode,����ȼ������룬ʹ�ù�����ʹ�ó���QR_EM_8BIT�����mode�����ɲ������Ķ�ά�룬��ʱ��֪��ԭ��
	QRCode* p = qrInit(20, QR_EM_8BIT, QR_ECL_H, -1, &errcode);
	if (p == NULL)
	{
		printf("error\n");
		return -1;
	}

	string strData = "{\"codeType\":1,\"data\":\"PN7JJ/FvtMYeh+Rt9UPDpjEVZtqtbWIRooPJDEaTlpteH2qbtcmawqNbK9vPeTmxri1xxpj7sx8CjomLibHMIpUzb9XBJRJ2mdV/iFzTui76vPbY/Tgdmf6yyrsifSqTVm0lj29ZnFvmOSUfk+jijtO9gSxiNXlr6s8312+pUXyINFhLlR2T7TYQnJ/mL/Aca4wSBu4gccRo42UtTEud1W7U74ZJNfgLdQXLIEXxLLfguhrFEl3vyNztFwXfXm6jjehLihzXF6VaR73nP+yKBG+y1renhqAoXY6rV8gQJIaDjDuZ8+23gXCxg7B9XUFL/InjxZPjnITnOTG0X1uU1Q==\"}";
	qrAddData(p, (const qr_byte_t*)strData.data(), strData.length());
	//ע����Ҫ����qrFinalize����
	if (!qrFinalize(p))
	{
		printf("finalize error\n");
		return -1;
	}
	int size = 0;
	//����5�ֱ��ʾ������֮��ľ���Ͷ�ά��ͼƬ�ķŴ�������Χ����1-16
	int dis = 3, fan = 3;
	qr_byte_t * buffer = qrSymbolToBMP(p, dis, fan, &size);
	if (buffer == NULL)
	{
		printf("error %s", qrGetErrorInfo(p));
		return -1;
	}
	std::ofstream f("c:\\a.bmp");
	f.write((const char *)buffer, size);
	f.close();


	qr_byte_t * buffer2 = qrSymbolToPNG(p, dis, fan, &size);
	if (buffer2 == NULL)
	{
		printf("error %s", qrGetErrorInfo(p));
		return -1;
	}
	size = 0;
	std::ofstream f2("c:\\a.png");
	f2.write((const char *)buffer2, size);
	f.close();
	
}