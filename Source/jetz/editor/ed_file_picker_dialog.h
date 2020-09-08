/*=============================================================================
ed_file_picker_dialog.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <vector>

#include "ed_dialog.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

class ed_file_picker_dialog : public ed_dialog {

public:

	ed_file_picker_dialog();
	~ed_file_picker_dialog();

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	std::string					get_selected_file() const;
	void						set_directory(const std::string& dir);
	virtual ed_dialog_result	think() override;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	std::vector<std::string>	_files;
	std::string					_directory;
	int							_selected_file_index;

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

	void handle_open();
	void handle_opening();
};

}   /* namespace jetz */
