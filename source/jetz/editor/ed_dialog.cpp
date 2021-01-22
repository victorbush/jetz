/*=============================================================================
ed_dialog.cpp
=============================================================================*/

/*=============================================================================
INCLUDES
=============================================================================*/

#include "jetz/editor/ed_dialog.h"

/*=============================================================================
NAMESPACE
=============================================================================*/

namespace jetz {

/*=============================================================================
CONSTRUCTORS
=============================================================================*/

ed_dialog::ed_dialog()
	: 
	_result(ed_dialog_result::NONE),
	_state(ed_dialog_state::CLOSED)
{
}

ed_dialog::~ed_dialog()
{
}

/*=============================================================================
PUBLIC METHODS
=============================================================================*/

void ed_dialog::close()
{
	if (_state == ed_dialog_state::CLOSED
		|| _state == ed_dialog_state::CLOSING)
	{
		return;
	}

	_state = ed_dialog_state::CLOSING;
}

ed_dialog_state ed_dialog::get_state() const
{
	return _state;
}

ed_dialog_result ed_dialog::get_result() const
{
	return _result;
}

void ed_dialog::open()
{
	if (_state == ed_dialog_state::OPEN
		|| _state == ed_dialog_state::OPENING)
	{
		return;
	}

	_state = ed_dialog_state::OPENING;
	_result = ed_dialog_result::NONE;
}

/*=============================================================================
PRIVATE METHODS
=============================================================================*/

}   /* namespace jetz */
