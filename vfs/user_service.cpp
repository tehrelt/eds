#include "user_service.h"
#include "sha256.h"

User* UserService::Create(std::string name, std::string password)
{
	int id = _storage->GetNextUserId();

	std::string hash_password = sha256(password);

	User* user = new User(id, name, hash_password);

	_storage->SaveUser(user->ToChar());

	return user;
}

User* UserService::Read(int id)
{
	return nullptr;
}

User* UserService::Update(int id, std::string name, std::string password)
{
	return nullptr;
}

void UserService::Delete(int id)
{
}
