/*=============================================================================
ed_dialog.h
=============================================================================*/

#pragma once

/*=============================================================================
INCLUDES
=============================================================================*/

#include <string>
#include <unordered_map>

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

enum class ed_dialog_state {
	OPENING,
	OPEN,
	CLOSING,
	CLOSED
};

enum class ed_dialog_result {
	NONE,		/* Dialog still open, or never opened */
	OK,
	YES,
	NO,
	CANCEL
};

class ed_dialog {

public:

	virtual ~ed_dialog() = 0;

	/*-----------------------------------------------------
	Public Methods
	-----------------------------------------------------*/

	ed_dialog_state get_state() const;
	ed_dialog_result get_result() const;

	void close();
	void open();

	virtual void think() = 0;

protected:

	ed_dialog();

	ed_dialog_result _result;
	ed_dialog_state	_state;

private:

	/*-----------------------------------------------------
	Private variables
	-----------------------------------------------------*/

	/*-----------------------------------------------------
	Private methods
	-----------------------------------------------------*/

};

}   /* namespace jetz */
