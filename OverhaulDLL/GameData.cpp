/*
	DARK SOULS OVERHAUL
	
	Contributors to this file:
		Ashley						-	Reverse engineering, Low FPS Disconnect patch
		jellybaby34					-	Game version number patch
		Metal-Crow					-	Reverse engineering, Phantom Limit patch, C++
		Youri "NullBy7e" de Mooij	-	Original Bonfire Input Fix
		Sean Pesce					-	C++
	
*/

#include "DllMain.h"
#include <clocale>


/*
	Initialize constants:
*/
// Default filename prefix for game archive files
const wchar_t *DEFAULT_ARCHIVE_FILE_PREFIX = L"dvdbnd";

// Default file types for game archive files (wide char)
const wchar_t *ARCHIVE_FILE_TYPE_W[2] = { L".bdt", L".bhd5" };

// Default file types for game archive files (char)
const char *ARCHIVE_FILE_TYPE[2] = { ".bdt", ".bhd5" };


/*
	Initialize static variables:
*/

// Base address of Dark Souls game process
void *GameData::ds1_base = NULL;

// Base address for player character data
void *GameData::player_char_base = NULL;

// Player character status (loading, human, co-op, invader, hollow)
SpPointer GameData::player_char_status;

// Game saving on/off
SpPointer GameData::saves_enabled;

// Multiplayer node count
int GameData::node_count = -1;





// Initializes pointers that depend on the game's base address
void GameData::init_pointers()
{
	print_console(std::string("[Overhaul Mod] Initializing pointers...").c_str());

	// Obtain base address for player character data
	GameData::player_char_base = (void*)((unsigned int)GameData::ds1_base + 0xF7E204);

	// Player character status (loading, human, co-op, invader, hollow)
	GameData::player_char_status = SpPointer(GameData::player_char_base, { 0xA28 });

	// Game saving on/off
	GameData::saves_enabled = SpPointer((void*)0x13784A0, { 0xB40 });
}


// Obtains the current game version number
uint8_t GameData::get_game_version()
{
	return *((uint8_t*)GameData::ds1_base + 0x7E719F);
}


// Changes the game version number to avoid compatibility issues with different game builds
void GameData::set_game_version(uint8_t version_number)
{
	std::stringstream hex_stream;
	hex_stream << std::hex << (int)version_number;
	std::string new_version_str = hex_stream.str();
	hex_stream.str(""); // Clear string stream
	hex_stream << std::hex << (int)GameData::get_game_version();

	ModData::startup_messages.push_back(std::string("[Overhaul Mod] Changing game version number from 0x").append(hex_stream.str()).append(" to 0x").append(new_version_str).append("..."));

	uint8_t patch1[5] = { 0xC6, 0x44, 0x24, 0x1C, version_number }; // mov byte ptr [esp+0x1C],0x3C
	uint8_t patch2[3] = { 0x80, 0x38, version_number }; // cmp byte ptr [eax],0x3C

	void *write_address = (uint8_t*)GameData::ds1_base + 0x7E73FA;
	apply_byte_patch(write_address, patch1, 5);

	write_address = (uint8_t*)GameData::ds1_base + 0x7E719D;
	apply_byte_patch(write_address, patch2, 3);

	write_address = (uint8_t*)GameData::ds1_base + 0x7E7229;
	apply_byte_patch(write_address, patch2, 3);
}


// Checks if the player is stuck at the bonfire, and if so, automatically applies the bonfire input fix
void GameData::check_bonfire_input_bug()
{
	// Check that the mod is fully loaded
	if (!ModData::initialized)
		return;

	/*
		If glitch conditions do not exist, this variable is set to 0.
		When bonfire input glitch is detected and first_detected is 0, the time is logged in first_detected.

		If the glitch is detected, and the time in first_detected is more than 1 second ago, the bonfire input fix is applied.
	*/
	static DWORD first_detected; // (Static variable persists between function calls)

	// Get current player status
	int status = DS1_PLAYER_STATUS_LOADING;
	if (player_char_status.read(&status) != ERROR_SUCCESS)
	{
		// Error reading memory location
		first_detected = 0;
		return;
	}


	// Check for bonfire input glitch conditions
	if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Nested if-statements are necessary to avoid crashes related to resolving multi-level pointers

		// Get pointer to current character animation
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

		// Read current character animation
		uint32_t current_anim;
		if (bonfire_anim_fix.read(&current_anim) != ERROR_SUCCESS)
		{
			// Error reading memory location
			first_detected = 0;
			return;
		}


		// Read bonfire menu flag
		SpPointer bonfire_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x40 });
		uint8_t bonfire_menu_is_open;
		if (bonfire_menu_flag.read(&bonfire_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read repair menu flag
		SpPointer repair_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x4C });
		uint8_t repair_menu_is_open;
		if (repair_menu_flag.read(&repair_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read level up menu flag
		SpPointer level_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x78 });
		uint8_t level_menu_is_open;
		if (level_menu_flag.read(&level_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read bottomless box menu flag
		SpPointer bottomless_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x84 });
		uint8_t bottomless_menu_is_open;
		if (bottomless_menu_flag.read(&bottomless_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read attune magic menu flag
		SpPointer attune_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x80 });
		uint8_t attune_menu_is_open;
		if (attune_menu_flag.read(&attune_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read reinforce weapon/armor menu flag
		SpPointer reinforce_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x50 });
		uint8_t reinforce_menu_is_open;
		if (reinforce_menu_flag.read(&reinforce_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read warp menu flag
		SpPointer warp_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0xAC });
		uint8_t warp_menu_is_open;
		if (warp_menu_flag.read(&warp_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}

		// Read "reverse hollowing?"/"can't kindle"/"can't reverse hollowing"/"insufficient attunement slots" dialog flag
		SpPointer dialog_menu_flag = SpPointer((void*)((uint32_t)GetModuleHandle(NULL) + 0xF786D0), { 0x60 });
		uint8_t dialog_menu_is_open;
		if (dialog_menu_flag.read(&dialog_menu_is_open) != ERROR_SUCCESS)
		{
			first_detected = 0;
			return;
		}


		// Check bonfire input fix conditions
		if ( (current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
			&& !bonfire_menu_is_open && !repair_menu_is_open && !bottomless_menu_is_open && !reinforce_menu_is_open
			&& !level_menu_is_open && !attune_menu_is_open && !dialog_menu_is_open && !warp_menu_is_open) // Make sure no bonfire menus are open
		{
			// Bonfire input glitch conditions currently exist

			if (first_detected == 0)
			{
				// First detection; log time
				first_detected = GetTickCount();
				return;
			}
			else if ((GetTickCount() - first_detected) >= 1000)
			{
				// Too much time has elapsed; bonfire input glitch is definitely present
				fix_bonfire_input();
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_AUTOMATIC_);
			}
			else
			{
				// Not enough time has elapsed; return and check again later
				return;
			}
		}
	}

	first_detected = 0;
}


// Fixes input bug that causes players to be stuck at a bonfire (usually after turning human with framerate unlocked)
int GameData::fix_bonfire_input(bool print_to_text_feed, bool print_to_console)
{
	// Get current player status
	int status = DS1_PLAYER_STATUS_LOADING;
	if (player_char_status.read(&status) != ERROR_SUCCESS)
	{
		// Error reading memory location
		if (print_to_text_feed)
			print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);

		if (print_to_console)
			print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_INIT_FAIL_);
		return ERROR_INVALID_ADDRESS;
	}


	if (status == DS1_PLAYER_STATUS_HOLLOW || status == DS1_PLAYER_STATUS_HUMAN) // Check if player is hollow/human
	{
		// Get pointer to current character animation
		SpPointer bonfire_anim_fix = SpPointer((void*)0x12E29E8, { 0x0, 0xFC });

		// Read current character animation
		uint32_t current_anim;
		bonfire_anim_fix.read(&current_anim);

		// Check that player is currently sitting at a bonfire
		if (current_anim == 7701 || current_anim == 7711 || current_anim == 7721) // 3 different bonfire resting animation IDs
		{
			// Write zero to bonfire animation status address
			bonfire_anim_fix.write((uint32_t)0);

			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_SUCCESS_);

			return ERROR_SUCCESS;
		}
		else
		{
			// Player is not at a bonfire
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
		}
	}
	else
	{
		// Player is not hollow/human, so can't be at a bonfire
		if (status == DS1_PLAYER_STATUS_LOADING)
		{
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NO_CHAR_FAIL_);
		}
		else
		{
			if (print_to_text_feed)
				print(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);

			if (print_to_console)
				print_console(_DS1_MOD_MSG_BONFIRE_INPUT_FIX_NOT_AT_BONFIRE_FAIL_);
		}
	}

	return ERROR_BAD_ENVIRONMENT;
}


// Returns multiplayer node count as an int (or -1 if player is not online)
int GameData::get_node_count()
{
	SpPointer node_count_ptr = SpPointer((uint8_t*)ds1_base + 0xF7F77C, { 0x2C, 0x778, 0x80 });

	// Get node count
	int nodes = -1;
	node_count_ptr.read(&nodes);
	return nodes;
}




/////////////////////////////////////////
//////////////// PATCHES ////////////////
/////////////////////////////////////////


// Disables automatic game disconnection when low framerate is detected
void GameData::low_fps_disconnect_enabled(bool enable)
{
	uint8_t *fps_warn = NULL;
	fps_warn = (uint8_t*)aob_scan("74 17 B9 ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"); // Careful, this pattern returns 2 results

	if (fps_warn)
	{
		// AoB Scan was successful
		set_mem_protection(fps_warn, 1, MEM_PROTECT_RWX);
		fps_warn -= 0x1E;
		
		if (enable)
		{
			print_console("[Overhaul Mod] Enabling low FPS disconnect...");
			*fps_warn = 0x51; // Enable low FPS disconnect
		}
		else
		{
			print_console("[Overhaul Mod] Disabling low FPS disconnect...");
			*fps_warn = 0xC3; // Disable low FPS disconnect
		}
			
	}
}

// Increase available pool of memory Dark Souls allocates itself
void GameData::increase_memory_limit()
{
	ModData::startup_messages.push_back("[Overhaul Mod] Increasing available memory...");

	uint8_t patch[5] = { 0x68, 0x00, 0x00, 0xDA, 0x00 }; // push 0x0DA0000. The constant can be increased as desired, and represents dark souls total memory pool

	void *write_address = (uint8_t*)GameData::ds1_base + 0xB8B7E5;
	apply_byte_patch(write_address, patch, 5);

	write_address = (uint8_t*)GameData::ds1_base + 0x9E20;
	apply_byte_patch(write_address, patch, 5);

	write_address = (uint8_t*)GameData::ds1_base + 0x9E41;
	apply_byte_patch(write_address, patch, 5);
}

// Set the .bdt files to be loaded by the game (WARNING: archive_name parameter must be exactly 6 characters)
void GameData::change_loaded_bdt_files(wchar_t *archive_name)
{
	if (archive_name == NULL || (int)std::wstring(archive_name).length() == 0)
	{
		return;
	}

	ModData::startup_messages.push_back(std::string("[Overhaul Mod] Checking if custom game files exist..."));

	// Check that the custom archive name prefix is the correct length
	size_t custom_name_len = 0;
	if ((custom_name_len = (int)std::wstring(archive_name).length()) != ARCHIVE_FILE_PREFIX_LENGTH)
	{
		ModData::startup_messages.push_back(std::string("[Overhaul Mod] ERROR: Custom archive name prefix was invalid length (").append(std::to_string(custom_name_len)).append("). Using default archive files instead."));
		return;
	}

	// Get char* strings for printing console messages
	char archive_name_ch[_MAX_PATH];
	archive_name_ch[0] = '\0';
	std::setlocale(LC_ALL, "en_US.utf8");
	size_t chars_converted;
	errno_t conversion_return;
	if ((conversion_return = wcstombs_s(&chars_converted, archive_name_ch, _MAX_PATH, archive_name, _TRUNCATE)))
	{
		// Error converting from wide char to char
		ModData::startup_messages.push_back(std::string("[Overhaul Mod] ERROR: Unable to parse custom archive file name (Error code ").append(std::to_string(conversion_return)).append("). Using default archive files instead."));
		return;
	}



	// Check that custom game archive files exist
	for (int i = 0; i < ARCHIVE_FILE_PAIR_COUNT; i++)
	{
		std::ifstream check_file(std::wstring(archive_name).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[0]).c_str());
		if (!check_file.good())
		{
			// Custom .bdt file doesn't exist
			ModData::startup_messages.push_back(std::string("[Overhaul Mod] ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]).append("\" could not be found. Using default archive files instead."));
			return;
		}
		else
			ModData::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[0]));
		check_file.close();
		std::ifstream check_file2(std::wstring(archive_name).append(std::to_wstring(i)).append(ARCHIVE_FILE_TYPE_W[1]).c_str());
		if (!check_file2.good())
		{
			// Custom .bhd5 file doesn't exist
			ModData::startup_messages.push_back(std::string("[Overhaul Mod] ERROR: The file \"").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]).append("\" could not be found. Using default archive files."));
			return;
		}
		else
			ModData::startup_messages.push_back(std::string("    Found ").append(archive_name_ch).append(std::to_string(i)).append(ARCHIVE_FILE_TYPE[1]));
		check_file2.close();
	}
	
	
	ModData::startup_messages.push_back(std::string("[Overhaul Mod] SUCCESS: Custom game archive files will be loaded (\"").append(archive_name_ch).append("\")."));
	void *dvd0_bdt = (uint8_t*)GameData::ds1_base + 0xD63AF6;
	apply_byte_patch(dvd0_bdt, archive_name, 12);
	void *dvd0_bhd = (uint8_t*)GameData::ds1_base + 0xD63B22;
	apply_byte_patch(dvd0_bhd, archive_name, 12);
	void *dvd1_bdt = (uint8_t*)GameData::ds1_base + 0xD63B5E;
	apply_byte_patch(dvd1_bdt, archive_name, 12);
	void *dvd1_bhd = (uint8_t*)GameData::ds1_base + 0xD63B8A;
	apply_byte_patch(dvd1_bhd, archive_name, 12);
	void *dvd2_bdt = (uint8_t*)GameData::ds1_base + 0xD63BC6;
	apply_byte_patch(dvd2_bdt, archive_name, 12);
	void *dvd2_bhd = (uint8_t*)GameData::ds1_base + 0xD63BF2;
	apply_byte_patch(dvd2_bhd, archive_name, 12);
	void *dvd3_bdt = (uint8_t*)GameData::ds1_base + 0xD63C2E;
	apply_byte_patch(dvd3_bdt, archive_name, 12);
	void *dvd3_bhd = (uint8_t*)GameData::ds1_base + 0xD63C5A;
	apply_byte_patch(dvd3_bhd, archive_name, 12);
}