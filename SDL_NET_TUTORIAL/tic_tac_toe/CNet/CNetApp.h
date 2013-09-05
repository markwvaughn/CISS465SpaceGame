#include "CNet.h"

class CNetMessageApp : public CNetMessage 
{
private:
	// Virtual function that indicated how many bytes may have to be loaded onto the object.
	// Overrides the parent class function member to work with only a byte
	virtual int NumToLoad();

	// Virtual function that indicated how many bytes may have to be downloaded from the object.
	// Overrides the parent class function member to work with only a byte
	virtual int NumToUnload();

public:
	//Function simpler than LoadBytes(), inherited from the parent class, as it only works with one byte
	void LoadByte(char);

	//Function simpler than UnLoadBytes(), inherited from the parent class, as it only works with one byte
	char UnloadByte();
};
