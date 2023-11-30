#pragma once
#include "ikernel.h"
#include "file_system.h"

class Kernel : public IKernel
{
private:
	FileSystem* _fs;

public:
	void mkdir()	override;
	void mkfile()	override;
	void mv()		override;
	void cp()		override;
	void rmdir()	override;
	void rmfile()	override;
	void su()		override;
	void cu()		override;
};

