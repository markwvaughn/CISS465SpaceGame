#include "CNetApp.h"

int CNetMessageApp::NumToLoad()
{
	if (state == EMPTY)
		return 1;
	else
		return 0;
}


int CNetMessageApp::NumToUnLoad()
{
	if (state == FULL)
		return 1;
	else
		return 0;
}


void CNetMessageApp::LoadByte(char ID)
{
	charbuf c;
	c[0] = ID;

	LoadBytes(c, 1);
	finish();
}

char CNetMessageApp::UnLoadByte()
{
	charbuf c;
	UnLoadBytes(c);
	return c[0];
}