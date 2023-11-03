#pragma once
#include "base_service.h"
#include "user.h"

class UserService : public BaseService
{
public:
	UserService() : BaseService() { }
	UserService(Storage* storage) : BaseService(storage) { }

	User* Create(std::string name, std::string password);
	User* Read(int id);
	User* Update(int id, std::string name, std::string password);
	void Delete(int id);
};

