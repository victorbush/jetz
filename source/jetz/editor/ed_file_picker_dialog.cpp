/*=============================================================================
ed_file_picker_dialog.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/editor/ed_file_picker_dialog.h"
#include "jetz/main/log.h"
#include "thirdparty/imgui/imgui.h"

#include "thirdparty/dirent/dirent.h"
#undef ERROR // Was causing conflicts with log_level::ERROR for some reason (dirent.h)

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ed_file_picker_dialog::ed_file_picker_dialog()
	: 
	_selected_file_index(-1),
	ed_dialog()
{
}

ed_file_picker_dialog::~ed_file_picker_dialog()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

std::string ed_file_picker_dialog::get_selected_file() const
{
	if (_selected_file_index < 0 || _files.size() == 0)
	{
		return "";
	}

	return fmt::format("{0}/{1}", _directory, _files[_selected_file_index]);
}

void ed_file_picker_dialog::set_directory(const std::string& dir)
{
	_directory = std::string(dir);
}

ed_dialog_result ed_file_picker_dialog::think()
{
	if (_state == ed_dialog_state::CLOSED
		|| _state == ed_dialog_state::CLOSING)
	{
		return ed_dialog_result::NONE;
	}

	if (_state == ed_dialog_state::OPENING)
	{
		handle_opening();
	}

	if (_state == ed_dialog_state::OPEN)
	{
		handle_open();
	}

	return _result;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

void ed_file_picker_dialog::handle_open()
{
	_result = ed_dialog_result::NONE;

	/* Render dialog */
	ImGui::OpenPopup("Open File");
	if (ImGui::BeginPopupModal("Open File", NULL, 0))
	{
		/* List files in directory */
		ImGui::PushItemWidth(-1.0f);
		ImGui::ListBoxHeader("Files", 10, -1);
		for (size_t i = 0; i < _files.size(); ++i)
		{
			ImVec2 size = { 0.0f, 0.0f };
			if (ImGui::Selectable(_files[i].data(), _selected_file_index == i, 0, size))
			{
				_selected_file_index = i;
			}
		}
		ImGui::ListBoxFooter();

		/* Show Open/Cancel buttons */
		ImVec2 buttonSize;
		buttonSize.x = 80;
		buttonSize.y = 0;

		/* Open button - only show if a file is selected */
		if (_files.size() > 0)
		{
			if (ImGui::Button("Open", buttonSize))
			{
				close();
				_result = ed_dialog_result::OK;
			}
		}

		ImGui::SameLine(0, -1);

		/* Close button */
		if (ImGui::Button("Cancel", buttonSize))
		{
			close();
			_result = ed_dialog_result::CANCEL;
			ImGui::CloseCurrentPopup();
		}
	}
	ImGui::EndPopup();
}

void ed_file_picker_dialog::handle_opening()
{
	_files = std::vector<std::string>();
	_selected_file_index = -1;

	/* Get list of files in the worlds directory */
	DIR* dir;
	struct dirent* ent;
	std::string dir_path = _directory + "\\";

	if ((dir = opendir(dir_path.c_str())) != NULL)
	{
		int i = 0;
		while ((ent = readdir(dir)) != NULL)
		{
			/* Skip if not a file */
			if (ent->d_type != DT_REG)
			{
				continue;
			}

			/* Copy filename to buffer */
			std::string filename = std::string(ent->d_name);
			_files.push_back(filename);
		}

		closedir(dir);
	}
	else
	{
		/* Could not open directory */
		LOG_ERROR("Failed to open directory.");
	}

	/* Dialog opened */
	_state = ed_dialog_state::OPEN;
}

}   /* namespace jetz */
