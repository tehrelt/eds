#pragma once
class IKernel
{
public:
	virtual void mkdir() = 0;
	virtual void mkfile() = 0;
	virtual void mv() = 0;
	virtual void cp() = 0;
	virtual void rmdir() = 0;
	virtual void rmfile() = 0;
	virtual void su() = 0;
	virtual void cu() = 0;
};

